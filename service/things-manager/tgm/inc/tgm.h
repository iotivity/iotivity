//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

/// @file tgmserver.cpp

/// @brief This sample provides steps to define an interface for a resource (properties and methods) and host this resource on the server.

#include <functional>
#include <pthread.h>
#include <condition_variable> 
#include <algorithm>

#include <actionjson.h>
#include <sqlstring.h>

#include "OCPlatform.h" 
#include "OCApi.h"

#include "GroupManager.h"

using namespace OC;
using namespace std;

const int SUCCESS_RESPONSE = 0;
int g_observation = 0;

OCPlatform *g_platform;

std::map< std::string, std::vector< std::string > > g_mapResourceTypeListOfCallId; 
std::map< std::string, std::vector< Device* > >    *g_mapDeviceListOfResourceType;
std::map< std::string, shared_ptr< OCResource > >   g_mapResouceOfDeviceId;
std::map< std::string, Device* >                   *g_mapDeviceOfDeviceId;
std::map< std::string, std::string >                g_mapReadyDevices;


std::mutex g_executionLock;
std::mutex g_bufferLock;
std::mutex g_deviceDataLock;


void* callFindResource(void* charStr);
void findResourceFromClient(std::string* typeStr);
void removeAllDeviceList(std::string resourceType);



struct OCResourcePutStruct
{
    OCRepresentation rep;
    QueryParamsMap qp;
    Device *device;
    std::shared_ptr< OCResource > resource;
};



class ResourceHandler
{
private:
    std::string m_resourceType;
    std::shared_ptr< OCResource > m_resource;
    OCPlatform *m_ocplatform;
    std::map< std::string, std::vector< Device* > > *m_mapDeviceListOfResourceTypes;
    std::map< std::string, Device* > *m_mapDeviceOfDeviceIds;
    std::vector< std::string > m_mapFoundedDeviceDuid;

public:
    ResourceHandler() :
            m_ocplatform(NULL)
    {
        m_mapDeviceListOfResourceTypes = NULL;
        m_mapDeviceOfDeviceIds = NULL;
    }

    OCPlatform * getOCPlatform()
    {
        return this->m_ocplatform;
    }
    void clearMapFoundedDeviceDuid()
    {
        m_mapFoundedDeviceDuid.clear();
    }
    void setDeviceList(std::map< std::string, std::vector< Device* > > *map)
    {
        this->m_mapDeviceListOfResourceTypes = map;
    }
    void setDeviceListOfId(std::map< std::string, Device* > *map)
    {
        this->m_mapDeviceOfDeviceIds = map;
    }

    void onGetForGroupCreation(const std::shared_ptr< OCResource > resource,
            const OCRepresentation& rep, const int eCode)
    {
        if (eCode == SUCCESS_RESPONSE)
        {
            AttributeMap attributeMap = rep.getAttributeMap();

            
            std::string resourceUri = resource->uri();
            std::vector< std::string > resourceTypes = rep.getResourceTypes();

            //std::string resourceType = resourceTypes.at(0);
            //std::string resourceType = "a.light"; //TODO: hard coding
            std::string duid = resource->host();
            duid.append(resourceUri);

            
            if (std::find(m_mapFoundedDeviceDuid.begin(), m_mapFoundedDeviceDuid.end(), duid)
                    == m_mapFoundedDeviceDuid.end())
            {
                //Create Device from received data
                Device* device = new Device();

                m_mapFoundedDeviceDuid.push_back(duid);

                // TODO : didn't get resourceType now. 
                std::string resourceType = "";
                resourceType.append(split(resource->uri(), '/').at(1)); 
                resourceType.append("."); 
                resourceType.append(split(resource->uri(), '/').at(2)); 

                device->m_name = resourceUri; // not include host
                device->m_type = OC::convertDeviceTypeToEnum(resourceType); //TODO: assign
                device->m_address = resource->host();
                device->m_duid = duid;
                //device->m_resource = resource;

                this->m_resourceType = resourceType;

                std::cout << "GET Succeeded: " << resourceType << " : "
                        << rep.getResourceTypes().size() << std::endl;
                std::cout << "\tURI of the resource: " << resource->uri() << std::endl;
                std::cout << "\tHost address of the resource: " << resource->host() << std::endl;

                for (unsigned int i = 0; i < resourceTypes.size(); i++)
                {
                    std::cout << "Resource Type " << i << " : " << resourceTypes.at(i) << std::endl;
                }

                for (auto it = attributeMap.begin(); it != attributeMap.end(); ++it)
                {
                    //Create Capability from attribute
                    Capability* capa = new Capability();
                    capa->m_name = it->first;
                    capa->m_type = OC::convertCapabilityToEnum(it->first);
                    capa->m_data = "";
                    capa->m_permission = OC::PERMISSION::PERMISSION_RW;

                    std::cout << "\tAttribute name: " << it->first << " value: ";
                    for (auto valueItr = it->second.begin(); valueItr != it->second.end();
                            ++valueItr)
                    {
                        std::cout << "\t" << *valueItr << " ";
                    }
                    std::cout << std::endl;
                    device->m_listCapabilities.push_back(capa);
                }

                if (m_mapDeviceOfDeviceIds != NULL)
                {   
                    g_deviceDataLock.lock();
                    auto findIt = m_mapDeviceOfDeviceIds->find(duid);
                    if (findIt == m_mapDeviceOfDeviceIds->end())
                    {
                        if( m_mapDeviceListOfResourceTypes != NULL )
                        {
                                
                            (*m_mapDeviceListOfResourceTypes)[resourceType].push_back(device);
                        }
                        else 
                        {
                            delete device;
                        }
                        (*m_mapDeviceOfDeviceIds)[duid] = device;
                        g_mapResouceOfDeviceId[duid] = resource;
                    }
                    else
                    {
                        std::cout << "\n\n\n DEVICE EXIST \n\n\n";
                        delete device;                        
                    }
                    g_deviceDataLock.unlock();
                }
                else
                {
                    delete device;
                }
            }
            
        }
        else
        {
            std::cout << "onGet Response error: " << eCode << std::endl;
            std::exit(-1);
        }
    }
    void initPlatform(OCPlatform *platform)
    {
        this->m_ocplatform = platform;
    }

    void foundResourceForGroupCreation(std::shared_ptr< OCResource > resource)
    {
        QueryParamsMap qp;
        resource->get(qp,
                std::function< void(const OCRepresentation, const int) >(
                        std::bind(&ResourceHandler::onGetForGroupCreation, this, resource,
                                std::placeholders::_1, std::placeholders::_2)));
    }
};

std::map< std::string, ResourceHandler* > g_mapResourceTypeHandlerList;
std::map< std::string, std::vector< Device* > > g_mapDeviceListOfResourceTypeBuffer;
std::map< std::string, Device* > g_mapDeviceOfDeviceIdBuffer;

class GroupManagerResource
{
public:
    OCResourceHandle m_resourceHandle;

public:
    /// Constructor
    GroupManagerResource() :
           m_resourceHandle(NULL)
    {
#ifdef TGM_SQLITE
        initSqlite3();
#endif
    }

    /* Note that this does not need to be a member function: for classes you do not have
     access to, you can accomplish this with a free function: */

    /// This function internally calls registerResource API.
    void createResource(OC::OCPlatform& platform,
        std::function<void(const OCResourceRequest::Ptr, const OCResourceResponse::Ptr)> entityHandlerFunction)
    {
        std::string resourceURI = "/a/groupmanager"; // URI of the resource
        std::string resourceTypeName = "core.groupmanager"; // resource type name. In this case, it is light
        std::string resourceInterface = DEFAULT_INTERFACE; // resource interface.

        // OCResourceProperty is defined ocstack.h
        uint8_t resourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE;



        OCStackResult result = platform.registerResource(m_resourceHandle, resourceURI,
                        resourceTypeName, resourceInterface, 
                        entityHandlerFunction, 
                        resourceProperty);

        // This will internally create and register the resource.
        // OCStackResult result = platform.registerResource(m_resourceHandle, resourceURI,
        //           resourceTypeName, resourceInterface, &entityHandler, resourceProperty);

        if (OC_STACK_OK != result)
        {
            cout << "Resource creation was unsuccessful\n";
        }
    }

    OCResourceHandle getHandle()
    {
        return m_resourceHandle;
    }


    void addType(const OC::OCPlatform& platform, const std::string& type) const
    {
        OCStackResult result = platform.bindTypeToResource(m_resourceHandle, type);
        if (OC_STACK_OK != result)
        {
            cout << "Binding TypeName to Resource was unsuccessful\n";
        }
    }

    void addInterface(const OC::OCPlatform& platform, const std::string& interface) const
    {
        OCStackResult result = platform.bindInterfaceToResource(m_resourceHandle, interface);
        if (OC_STACK_OK != result)
        {
            cout << "Binding TypeName to Resource was unsuccessful\n";
        }
    }
};
// Create the instance of the resource class
GroupManagerResource g_groupManagerResource;

class GroupManagerHandler
{
    private:
        OC::GroupManager* g_groupManagerInstance;

    public:
        GroupManagerHandler()
        {
            g_groupManagerInstance = OC::GroupManager::getInstance();
        }
        void groupEntityHandler(std::shared_ptr< OCResourceRequest > request,
        std::shared_ptr< OCResourceResponse > response)
        {
            entityHandler(request,response);
        }



        static void executionPutCallback(const OCRepresentation& rep, const int eCode)
        {
            std::cout << "\n\n\n" << "EXECUTION PUT CALL BACK \n\n\n";
        }

        static void* putResourceFromAction(void* typeStr)
        {
            g_executionLock.lock();

            struct OCResourcePutStruct *putStruct = (static_cast< struct OCResourcePutStruct * >(typeStr));
            Device* target = putStruct->device;

            OCRepresentation rep;
            QueryParamsMap queryParamsMap;
            std::cout << "\n\n\n" << "THREAD For Put Action" << target->m_duid << "\n\n\n";

            //if (target->m_resource->put(putStruct->rep, putStruct->qp,std::bind(&GroupManagerHandler::executionPutCallback, this, std::placeholders::_1,std::placeholders::_2) ) != 0)
            if (putStruct->resource->put(putStruct->rep, putStruct->qp,&executionPutCallback ) != 0)
            {
                std::cout << "PUT ERROR" << std::endl;
            }

            delete putStruct;

            g_executionLock.unlock();
            return NULL;
        }


        void createGroup(AttributeMap attributeMap, AttributeMap &responseAtbMap)
        {
            std::cout << "\t\t\tCreateGroup" << std::endl;

            std::string sessionID;
            std::string groupName;
            std::string groupuuid;

            vector< Device* > mandatory_devices;
            vector< Device* > optional_devices;
            AttributeValues::iterator it;

            sessionID = attributeMap["sessionid"][0];
            groupName = attributeMap["groupname"][0];
            groupuuid = attributeMap["guid"][0];

            std::cout << "\t\t\t:D sessionid : " << sessionID << std::endl; // check
            std::cout << "\t\t\t:D groupuuid : " << groupuuid << std::endl; // check
            std::cout << "\t\t\t:D groupName : " << groupName << std::endl; //check

            // std::cout << endl << endl << "dump :: " << attributeMap["mandatory"][0]
            //         << std::endl <<std::endl;

            //TODO: vector is not comming.
            /*
            for (it = attributeMap["mandatory"].begin();
                    it != attributeMap["mandatory"].end(); ++it)
            {
                cout << (*it) << endl;
                std::string temp = (*it);
                Device* dev = JsonUnpackDevice(temp.c_str());
                mandatory_devices.push_back(dev);
            }

            for (it = attributeMap["optional"].begin();
                    it != attributeMap["optional"].end(); ++it)
            {
                Device* dev = JsonUnpackDevice((*it).c_str());
                optional_devices.push_back(dev);
            }
            */
            // if(attributeMap.find("mandatory") != attributeMap.end())
            // {
            //     mandatory_devices = JsonUnpackDeviceListReturn(attributeMap["mandatory"][0].c_str());
            // }
            
            // if(attributeMap.find("optional") != attributeMap.end())
            // {
            //     optional_devices = JsonUnpackDeviceListReturn(attributeMap["optional"][0].c_str());
            // }

            g_groupManagerInstance->createGroupResource(groupuuid, groupName);
            // g_groupManagerInstance->addGroupMember(groupuuid, mandatory_devices,
            //         optional_devices);
            // g_groupManagerInstance->printGroup(groupuuid);

#ifdef TGM_SQLITE            
            OC::Group *createdGroup = g_groupManagerInstance->getGroup(groupuuid);
            insertGroupToGroupTable(createdGroup);
#endif
            /// Build up response;
            AttributeValues attr_sessionID;
            attr_sessionID.push_back(sessionID);
            responseAtbMap["sessionid"] = attr_sessionID;

            Group* group = g_groupManagerInstance->getGroup(groupuuid);
            AttributeValues attr_groupid;
            attr_groupid.push_back(group->m_guid);
            responseAtbMap["guid"] = attr_groupid;

            AttributeValues attr_Group;
            std::string temp = escapeJsonString(JsonPackGroup(group));
            // cout << temp << endl;
            // cout << (temp = escapeJsonString(temp)) << endl;
            attr_Group.push_back(temp);
            responseAtbMap["groupinfo"] = attr_Group;
        }

        void addGroupMember(AttributeMap attributeMap, AttributeMap &responseAtbMap)
        {
            std::cout << "\t\t\tAddGroupMember" << std::endl;

            std::string sessionID;
            std::string groupuuid;

            vector< Device* > mandatory_devices;
            vector< Device* > optional_devices;
            AttributeValues::iterator it;

            sessionID = attributeMap["sessionid"][0];
            groupuuid = attributeMap["guid"][0];

            std::cout << "\t\t\t:D sessionid : " << sessionID << std::endl; // check
            std::cout << "\t\t\t:D groupuuid : " << groupuuid << std::endl; // check
/* //TODO : need update base for list
            for (it = attributeMap["mandatory"].begin();
                    it != attributeMap["mandatory"].end(); ++it)
            {
                Device* dev = JsonUnpackDevice((*it).c_str());
                mandatory_devices.push_back(dev);
            }

            for (it = attributeMap["optional"].begin();
                    it != attributeMap["optional"].end(); ++it)
            {
                Device* dev = JsonUnpackDevice((*it).c_str());
                optional_devices.push_back(dev);
            }
*/
            if(attributeMap.find("mandatory") != attributeMap.end())
            {
                mandatory_devices = JsonUnpackDeviceListReturn(attributeMap["mandatory"][0].c_str());
            }
            
            if(attributeMap.find("optional") != attributeMap.end())
            {
                optional_devices = JsonUnpackDeviceListReturn(attributeMap["optional"][0].c_str());
            }

            g_groupManagerInstance->addGroupMember(groupuuid, mandatory_devices,
                    optional_devices);

            g_groupManagerInstance->printGroup(groupuuid);
#ifdef TGM_SQLITE
            OC::Group* changedGroup = g_groupManagerInstance->getGroup(groupuuid);
            deleteGroupToGroupTable(groupuuid.c_str());
            insertGroupToGroupTable(changedGroup);
#endif
            /// Build up response;
            AttributeValues attr_sessionID;
            attr_sessionID.push_back(sessionID);
            responseAtbMap["sessionid"] = attr_sessionID;

            // TBI
            // Need to serialization;
            AttributeValues attr_GroupID;
            attr_GroupID.push_back(groupuuid);
            responseAtbMap["guid"] = attr_GroupID;

            AttributeValues attr_Result;
            attr_Result.push_back("DEVICE_ADDED");
            responseAtbMap["result"] = attr_Result;

        }

        void deleteGroupMember(AttributeMap attributeMap, AttributeMap &responseAtbMap)
        {
            std::cout << "\t\t\tDeleteGroupMember" << std::endl;

            GID guid;
            std::string duid;
            std::string sessionID;

            sessionID = attributeMap["sessionid"][0];
            guid = attributeMap["guid"][0];
            duid = attributeMap["duid"][0];

            std::cout << "\t\t\t:D group id : " << guid << std::endl; // check
            std::cout << "\t\t\t:D device id : " << duid << std::endl; // check

            g_groupManagerInstance->removeGroupMember(guid, duid);
            g_groupManagerInstance->printGroup(guid);
#ifdef TGM_SQLITE
            OC::Group* changedGroup = g_groupManagerInstance->getGroup(guid);
            deleteGroupToGroupTable(guid.c_str());
            insertGroupToGroupTable(changedGroup);
#endif
            /// Build up response;
            AttributeValues attr_sessionID;
            attr_sessionID.push_back(sessionID);
            responseAtbMap["sessionid"] = attr_sessionID;

            // TBI
            // Need to serialization;
            AttributeValues attr_GroupID;
            attr_GroupID.push_back(guid);
            responseAtbMap["guid"] = attr_GroupID;

            AttributeValues attr_Result;
            attr_Result.push_back("DEVICE_REMOVED");
            responseAtbMap["result"] = attr_Result;

        }

        void findGroupMemberCandidatesByResourceType(AttributeMap &attributeMap, AttributeMap &responseAtbMap)
        {
            std::cout << "\t\t\tFindGroupMemberCandidatesByResourceType" << std::endl;

            std::string sessionID = attributeMap["sessionid"][0];
            std::vector< std::string > mandatory;
            std::vector< std::string > optional;

            /** TODO: temp **/
            std::vector< std::string > manTemp;
            std::vector< std::string > optTemp;
            manTemp = split(attributeMap["mandatory"][0], '|');
            if (attributeMap.find("optional") != attributeMap.end())
            {
                optTemp = split(attributeMap["optional"][0], '|');
            }
            attributeMap["mandatory"] = manTemp;
            attributeMap["optional"] = optTemp;
            /** temp end **/

            std::vector< std::string >::iterator it;
            for (it = attributeMap["mandatory"].begin();
                    it != attributeMap["mandatory"].end(); ++it)
            {                
                if (std::find(mandatory.begin(), mandatory.end(), (*it)) == mandatory.end())
                {
                  std::cout << "\t\t\tMandatory " << ":  " << (*it) << std::endl; // check
                  mandatory.push_back((*it));
                }
                
            }

            g_mapResourceTypeListOfCallId[sessionID] = mandatory;

            for (it = attributeMap["optional"].begin();
                    it != attributeMap["optional"].end(); ++it)
            {
                if (std::find(optional.begin(), optional.end(), (*it)) == optional.end())
                {
                  std::cout << "\t\t\tOptional : " << (*it) << std::endl; // check
                  optional.push_back((*it));
                }                
            }

            // Check start time and make thread.
            // and wait about 2s then call callback.
            for (it = mandatory.begin(); it != mandatory.end(); ++it)
            {
                //platform->findResource("", (*it), &foundResource);

                std::string *queryStr = new std::string((*it));

                //TODO: only one thread for find device
                auto findIt = g_mapResourceTypeHandlerList.find(queryStr->c_str());

                if (findIt == g_mapResourceTypeHandlerList.end())
                {
                    std::cout << "\n\n\n\n NEW THREAD :" << *queryStr << "\n\n\n\n";
                    g_mapResourceTypeHandlerList[*queryStr] = NULL;
                    findResourceFromClient(queryStr);
                }
                else 
                {
                    delete queryStr;
                }

                /*
                 {
                 std::unique_lock<std::mutex> ulEntity(m_mutexEntity);
                 m_cvEntity.wait(ulEntity);
                 }
                 */
            }

            for (it = optional.begin(); it != optional.end(); ++it)
            {
                //platform->findResource("", (*it), &foundResource);
            }

            /// Build up response;
            AttributeValues attr_sessionID;
            attr_sessionID.push_back(sessionID);
            responseAtbMap["sessionid"] = attr_sessionID;

        }

        void removeGroup(AttributeMap attributeMap, AttributeMap &responseAtbMap)
        {
            std::string guid;
            std::string sessionID;

            sessionID = attributeMap["sessionID"][0];
            guid = attributeMap["guid"][0];

            g_groupManagerInstance->removeGroupResource(guid);

            AttributeValues attr_sessionID;
            attr_sessionID.push_back(sessionID);
            responseAtbMap["sessionid"] = attr_sessionID;

        }

        void getGroup(AttributeMap attributeMap, AttributeMap &responseAtbMap)
        {
            Group* group;
            std::string sessionID;
            std::string guid;

            sessionID = attributeMap["sessionid"][0];
            guid = attributeMap["groupid"][0];

            group = g_groupManagerInstance->getGroup(guid);

            /// Build up response;
            AttributeValues attr_sessionID;
            responseAtbMap["sessionid"] = attr_sessionID;

            AttributeValues attr_Group;
            std::string groupStr = JsonPackGroup(group);
            std::string escape = escapeJsonString(groupStr);
            attr_Group.push_back(escape);
            responseAtbMap["groupinfo"] = attr_Group;

        }

        void getDevice(AttributeMap attributeMap, AttributeMap &responseAtbMap)
        {
            Device* device;
            std::string sessionID;
            std::string duid;

            sessionID = attributeMap["sessionid"][0];
            duid = attributeMap["duid"][0];

            device = g_groupManagerInstance->findDeviceInGroups(duid);

            /// Build up response;
            AttributeValues attr_sessionID;
            attr_sessionID.push_back(sessionID);
            responseAtbMap["sessionid"] = attr_sessionID;

            AttributeValues attr_device;
            attr_device.push_back(JsonPackDevice(device));
            responseAtbMap["device"] = attr_device;
        }

        void createActionSet(AttributeMap attributeMap, AttributeMap &responseAtbMap)
        {
            ///////////////////////////////////
            //TODO: remove callback dependency
            ////////////////////////////////////
            std::cout << "\t\t\tCreateActionSet" << std::endl;

            //groupManagerInstance->CreateGroupResource("0", "0");

            std::string sessionID;
            OC::GID guid;
            std::string actionset;

            sessionID = attributeMap["sessionid"][0];
            guid = attributeMap["guid"][0];
            actionset = attributeMap["actionset"][0];
          

            std::cout << "\t\t\t:D group id : " << guid << std::endl; // check
            std::cout << "\t\t\t:D actuation_set : " << actionset << std::endl; // check

            OC::ActionSet* as = JsonUnpackActionSet(actionset.c_str());


            //TODO: Serialize
            unsigned int retAsid = g_groupManagerInstance->createActionSet(guid, as);

            /// Build up response;
            AttributeValues attr_sessionID;
            attr_sessionID.push_back(sessionID);
            responseAtbMap["sessionid"] = attr_sessionID;

            AttributeValues attr_groupID;
            attr_groupID.push_back(guid);
            responseAtbMap["guid"] = attr_groupID;

            AttributeValues asidAttr;
            asidAttr.push_back(to_string(retAsid));
            responseAtbMap["asid"] = asidAttr;

            AttributeValues resultAttr;
            resultAttr.push_back("true");
            responseAtbMap["result"] = resultAttr;
        }

        void readActionSet(AttributeMap attributeMap, AttributeMap &responseAtbMap)
        {
            std::cout << "\t\t\tReadActionSet" << std::endl;

            std::string sessionID;
            GID guid;
            int asid;

            sessionID = attributeMap["sessionid"][0];
            guid = attributeMap["guid"][0];
            asid = atoi(attributeMap["asid"][0].c_str());

            std::cout << "\t\t\t:D group id : " << guid << std::endl; // check
            std::cout << "\t\t\t:D actionset id : " << asid << std::endl; // check

            ActionSet *actionSet = NULL;
            std::string encodeStr;
            try
            {
                actionSet = g_groupManagerInstance->readActionSet(guid, asid);
                encodeStr = JsonPackActionSet(actionSet);
            }
            catch (GraphInternalServiceException &e)
            {
                std::cerr << e.what() << std::endl;
            }

            /// Build up response;
            AttributeValues attr_sessionID;
            attr_sessionID.push_back(sessionID);
            responseAtbMap["sessionid"] = attr_sessionID;

            AttributeValues guidAttr;
            guidAttr.push_back(guid);
            responseAtbMap["guid"] = guidAttr;

            AttributeValues asidAttr;
            asidAttr.push_back(to_string(asid));
            responseAtbMap["asid"] = asidAttr;

            AttributeValues resultAttr;
            resultAttr.push_back("true");
            responseAtbMap["result"] = resultAttr;

            AttributeValues asAttr;
            asAttr.push_back(escapeJsonString(encodeStr));
            responseAtbMap["actionset"] = asAttr;

            
        }

        void updateActionSet(AttributeMap attributeMap, AttributeMap &responseAtbMap)
        {
            std::cout << "\t\t\tUpdateActionSet" << std::endl;

            std::string sessionID;
            GID guid;
            std::string actuation_set;

            sessionID = attributeMap["sessionid"][0];
            actuation_set = attributeMap["actionset"][0];
            guid = attributeMap["guid"][0];

            std::cout << "\t\t\t:D group id : " << guid << std::endl; // check
            std::cout << "\t\t\t:D actuation_set : " << actuation_set << std::endl; // check

            OC::ActionSet* as = JsonUnpackActionSet(actuation_set.c_str());
            bool ret = g_groupManagerInstance->updateActionSet(guid, as);

            /// Build up response;
            AttributeValues attr_sessionID;
            attr_sessionID.push_back(sessionID);
            responseAtbMap["sessionid"] = attr_sessionID;

            AttributeValues guidAttr;
            guidAttr.push_back(guid);
            responseAtbMap["guid"] = guidAttr;

            AttributeValues resultAttr;
            if (ret == true)
            {
                resultAttr.push_back("true");
            }
            else
            {
                resultAttr.push_back("false");
            }
            responseAtbMap["result"] = resultAttr;

            AttributeValues asidAttr;
            asidAttr.push_back("0"); //TODO: find ASID : aSet->actuation_seid
            responseAtbMap["asid"] = asidAttr;
        }

        void deleteActionSet(AttributeMap attributeMap, AttributeMap &responseAtbMap)
        {
            std::cout << "\t\t\tDeleteActionSet" << std::endl;

            std::string sessionID;
            GID guid;
            int asid;

            sessionID = attributeMap["sessionid"][0];
            guid = attributeMap["guid"][0];
            asid = atoi(attributeMap["asid"][0].c_str());

            std::cout << "\t\t\t:D group id : " << guid << std::endl; // check
            std::cout << "\t\t\t:D actionset id : " << asid << std::endl; // check

            bool ret = g_groupManagerInstance->deleteActionSet(guid, asid);

            AttributeValues guidAttr;
            guidAttr.push_back(guid);
            responseAtbMap["guid"] = guidAttr;

            AttributeValues resultAttr;
            if (ret == true)
            {
                resultAttr.push_back("true");
            }
            else
            {
                resultAttr.push_back("false");
            }
            responseAtbMap["result"] = resultAttr;

            AttributeValues attr_sessionID;
            attr_sessionID.push_back(sessionID);
            responseAtbMap["sessionid"] = attr_sessionID;

            AttributeValues asidAttr;
            asidAttr.push_back(to_string(asid));
            responseAtbMap["asid"] = asidAttr;


        }
        void executeActionSetByDesc(string groupMethodType, AttributeMap &attributeMap, AttributeMap &responseAtbMap)
        {
            std::cout << "\t\t\tExecuteActionSetByDescription" << std::endl;

            std::shared_ptr< OCResource > targetResource;
            std::string sessionID;
            std::string desc;

            sessionID = attributeMap["sessionid"][0];

            desc = attributeMap["description"][0];

            AttributeValues typeAttr;
            typeAttr.push_back(groupMethodType);
            responseAtbMap["type"] = typeAttr;

            responseAtbMap["sessionid"] = attributeMap["sessionid"];

            QueryParamsMap queryParamsMap;

            ActionSet *actionSet = g_groupManagerInstance->readActionSetFromDesc(desc);

            for (unsigned i = 0; i < actionSet->m_listActions.size(); ++i)
            {
                Action* action = actionSet->m_listActions.at(i);
                std::string targetId = action->m_duid;

                auto targetIt = g_mapDeviceOfDeviceId->find(targetId);
                Device* targetDevice = NULL;
                ;
                if (targetIt != g_mapDeviceOfDeviceId->end())
                {
                    targetDevice = targetIt->second;
                }
                //std::cout << "TARGET : " << targetId << std::endl;

                if (targetDevice != NULL)
                {
                    //targetDevice->printDevice();

                    AttributeMap actionAttributeMap;
                    AttributeValues attrSessionID;
                    attrSessionID.push_back(sessionID);
                    actionAttributeMap["sessionid"] = attrSessionID;

                    AttributeValues attrMethod;
                    attrMethod.push_back(groupMethodType);
                    actionAttributeMap["type"] = attrMethod;

                    AttributeValues attrAction;
                    attrAction.push_back(action->m_capability->m_data);
                    actionAttributeMap[s_capabilityTypes[action->m_capability->m_type]] =
                            attrAction;

                    struct OCResourcePutStruct* putStruct =
                            new struct OCResourcePutStruct();

                    OCRepresentation rep;
                    rep.setAttributeMap(actionAttributeMap);

                    putStruct->rep = rep;
                    putStruct->qp = queryParamsMap;
                    putStruct->device = targetDevice;
                    //putStruct->resource = targetDevice->m_resource;
                    putStruct->resource = g_mapResouceOfDeviceId[targetDevice->m_duid];

                    pthread_t threadId;
                    pthread_create(&threadId, NULL,
                        putResourceFromAction,
                         //std::function< void*(void*) >(std::bind(&GroupManagerHandler::putResourceFromAction, this, std::placeholders::_1)),
                            (void*) putStruct);
                    //pthread_join(threadId, NULL);
                }
            }
            /// Build up response;
            AttributeValues attr_sessionID;
            attr_sessionID.push_back(sessionID);
            responseAtbMap["sessionid"] = attr_sessionID;

            AttributeValues descAttr;
            descAttr.push_back(desc);
            responseAtbMap["description"] = descAttr;

            AttributeValues resultAttr;
            if(actionSet != NULL)
            {
                resultAttr.push_back("true");
            }
            else
            {
                resultAttr.push_back("false");
            }
            responseAtbMap["result"] = resultAttr;
        }
        void executeActionSet(string groupMethodType, AttributeMap &attributeMap, AttributeMap &responseAtbMap)
        {
            std::cout << "\t\t\tExecuteActionSet" << std::endl;

            std::shared_ptr< OCResource > targetResource;
            std::string sessionID;
            GID guid;
            int asid;

            sessionID = attributeMap["sessionid"][0];

            guid = attributeMap["guid"][0];
            asid = atoi(attributeMap["asid"][0].c_str());


            responseAtbMap["sessionid"] = attributeMap["sessionid"];

            QueryParamsMap queryParamsMap;

            ActionSet *actionSet = g_groupManagerInstance->readActionSet(guid, asid);

            for (unsigned i = 0; i < actionSet->m_listActions.size(); ++i)
            {
                Action* action = actionSet->m_listActions.at(i);
                std::string targetId = action->m_duid;
         
                auto targetIt = g_mapDeviceOfDeviceId->find(targetId);
                Device* targetDevice = NULL;
                ;
                if (targetIt != g_mapDeviceOfDeviceId->end())
                {
                    targetDevice = targetIt->second;
                }
                std::cout << "TARGET : " << targetId << std::endl;

                if (targetDevice != NULL)
                {
                    targetDevice->printDevice();

                    AttributeMap actionAttributeMap;
                    AttributeValues attrSessionID;
                    attrSessionID.push_back(sessionID);
                    actionAttributeMap["sessionid"] = attrSessionID;

                    AttributeValues attrMethod;
                    attrMethod.push_back(groupMethodType);
                    actionAttributeMap["type"] = attrMethod;

                    AttributeValues attrAction;
                    attrAction.push_back(action->m_capability->m_data);
                    actionAttributeMap[s_capabilityTypes[action->m_capability->m_type]] =
                            attrAction;

                    struct OCResourcePutStruct* putStruct =
                            new struct OCResourcePutStruct();

                    OCRepresentation rep;
                    rep.setAttributeMap(actionAttributeMap);

                    putStruct->rep = rep;
                    putStruct->qp = queryParamsMap;
                    putStruct->device = targetDevice;
                    //putStruct->resource = targetDevice->m_resource;
                    putStruct->resource = g_mapResouceOfDeviceId[targetDevice->m_duid];

                    pthread_t threadId;
                    pthread_create(&threadId, NULL, putResourceFromAction,
                            (void*) putStruct);
                    //pthread_join(threadId, NULL);
                }
            }
            /// Build up response;
            AttributeValues attr_sessionID;
            attr_sessionID.push_back(sessionID);
            responseAtbMap["sessionid"] = attr_sessionID;

            AttributeValues guidAttr;
            guidAttr.push_back(guid);
            responseAtbMap["guid"] = guidAttr;

            AttributeValues asidAttr;
            asidAttr.push_back(to_string(asid));
            responseAtbMap["asid"] = asidAttr;

            AttributeValues resultAttr;
            if(actionSet != NULL)
            {
                resultAttr.push_back("true");
            }
            else
            {
                resultAttr.push_back("false");
            }
            responseAtbMap["result"] = resultAttr;

        }
        void entityHandler(std::shared_ptr< OCResourceRequest > request,
        std::shared_ptr< OCResourceResponse > response)
        {
            cout << "\tIn Server CPP entity handler:\n";
            
            if (request)
            {
                // Get the request type and request flag
                std::string requestType = request->getRequestType();
                RequestHandlerFlag requestFlag = request->getRequestHandlerFlag();

                if (requestFlag == RequestHandlerFlag::InitFlag)
                {
                    cout << "\t\trequestFlag : Init\n";

                    // entity handler to perform resource initialization operations
                }
                else if (requestFlag == RequestHandlerFlag::RequestFlag)
                {
                    cout << "\t\trequestFlag : Request\n";

                    // If the request type is GET
                    if (requestType == "GET")
                    {
                        cout << "\t\t\trequestType : GET\n";

                        // Check for query params (if any)
                        QueryParamsMap queryParamsMap = request->getQueryParameters();

                        for (auto it = queryParamsMap.begin(); it != queryParamsMap.end(); ++it)
                        {
                            std::string callId = it->first;
                            std::string deviceList = it->second;
                            std::cout << "GET QUERY : " << callId << " : " << deviceList << std::endl;
                        }
                        // Process query params and do required operations ..

                        // Get the representation of this resource at this point and send it as response
                        AttributeMap responseAttributeMap;
                        AttributeValues typeAttr;
                        typeAttr.push_back("GET");
                        responseAttributeMap["type"] = typeAttr;

                        std::string encodeStr;
                        std::vector< std::string > callIdForRemove;

                        for (auto it = g_mapReadyDevices.begin(); it != g_mapReadyDevices.end(); ++it)
                        {
                            std::string callId = it->first;
                            std::string deviceList = it->second;
            
                            encodeStr = escapeJsonString(deviceList);

                            AttributeValues deviceAttr;
                            deviceAttr.push_back(encodeStr);
                            responseAttributeMap[callId] = deviceAttr;

                            callIdForRemove.push_back(it->first);
                        }


                        if (response)
                        {
                            // TODO Error Code
                            response->setErrorCode(200);
                            response->setResourceRepresentation(responseAttributeMap);
                        }
                    }
                    else if (requestType == "PUT")
                    {
                        cout << "\t\t\trequestType : PUT\n";

                        // Check for query params (if any)
                        QueryParamsMap queryParamsMap = request->getQueryParameters();
                        AttributeMap responseAttributeMap;

                        std::string groupMethodType;

                        std::map< std::string, std::string >::iterator it = queryParamsMap.begin();
                        for (; it != queryParamsMap.end(); ++it)
                        {
                            std::cout << it->first << " / " << it->second << std::endl;
                        }

                        //get method name
                        OCRepresentation rp = request->getResourceRepresentation();
                        AttributeMap attributeMap = request->getAttributeRepresentation();
                        
                        groupMethodType = attributeMap["method"][0];

                        if (groupMethodType == "CreateGroup")
                        {
                            createGroup(attributeMap, responseAttributeMap);

                        }
                        else if (groupMethodType == "AddGroupMember")
                        {
                            addGroupMember(attributeMap, responseAttributeMap);
                        }

                        else if (groupMethodType == "DeleteGroupMember")
                        {
                            deleteGroupMember(attributeMap, responseAttributeMap);                    
                        }
                        else if (groupMethodType == "FindGroupMemeberCandidatesByCapabilities")
                        {
                            std::cout << "\t\t\tFindGroupMemeberCandidatesByCapabilities" << std::endl;
                            // TODO : after work when we get attribute in foundResource callback
                        }
                        else if (groupMethodType == "FindGroupMemberCandidatesByResourceType")
                        {
                            findGroupMemberCandidatesByResourceType(attributeMap, responseAttributeMap);
                        }
                        else if (groupMethodType == "RemoveGroup")
                        {
                            removeGroup(attributeMap, responseAttributeMap);
                        }
                        else if (groupMethodType == "GetGroup")
                        {
                            getGroup(attributeMap, responseAttributeMap);

                        }
                        else if (groupMethodType == "GetDevice")
                        {
                            getDevice(attributeMap, responseAttributeMap);
                        }
                        else if (groupMethodType == "CreateActionSet")
                        {
                            createActionSet(attributeMap, responseAttributeMap);
                        }

                        else if (groupMethodType == "ReadActionSet")
                        {
                            readActionSet(attributeMap, responseAttributeMap);
                        }

                        else if (groupMethodType == "UpdateActionSet")
                        {
                            updateActionSet(attributeMap, responseAttributeMap);
                        }

                        else if (groupMethodType == "DeleteActionSet")
                        {
                            deleteActionSet(attributeMap, responseAttributeMap);
                        }

                        else if (groupMethodType == "ExecuteActionSetByDescription")
                        {
                            
                            executeActionSetByDesc(groupMethodType, attributeMap, responseAttributeMap);
                        }
                        else if (groupMethodType == "ExecuteActionSet")
                        {
                            executeActionSet(groupMethodType, attributeMap, responseAttributeMap);
                        }


                        if (groupMethodType != "") //TODO: Check groupmethodtype NULL
                        {
                            AttributeValues typeAttr;
                            typeAttr.push_back(groupMethodType);
                            responseAttributeMap["type"] = typeAttr;
                        }

                        if (response)
                        {
                            response->setErrorCode(200);
                            response->setResourceRepresentation(responseAttributeMap);
                        }
                    }
                    else if (requestType == "POST")
                    {
                        // POST request operations
                    }
                    else if (requestType == "DELETE")
                    {
                        // DELETE request operations
                    }
                }
                else if (requestFlag == RequestHandlerFlag::ObserverFlag)
                {
                    pthread_t threadId;

                    cout << "\t\trequestFlag : Observer\n";
                    g_observation = 1;

                    static int startedThread = 0;

                    // Observation happens on a different thread in ChangeLightRepresentation function.
                    // If we have not created the thread already, we will create one here.
                    if (!startedThread)
                    {
                        pthread_create(&threadId, NULL, observeHandler, (void *) NULL);
                        startedThread = 1;
                    }

                }
            }
            else
            {
                std::cout << "Request invalid" << std::endl;
            }
        }

        static void createMeetMandatoryList(std::map< std::string, std::vector< std::string > > typeListOfCall,
                std::map< std::string, std::vector< Device* > > deviceListOfType)
        {
            g_mapReadyDevices.clear();

            for (auto it = typeListOfCall.begin(); it != typeListOfCall.end(); ++it)
            {
                std::string callId = it->first;
                bool isMeet = true;
                std::string jsonPackDeviceStr = "{\"devices\":[";
                for (auto valueItr = it->second.begin(); valueItr != it->second.end(); ++valueItr)
                {
                    std::string resourceType = *valueItr;
                    //unsigned int deviceNum = (*deviceListOfResourceType)[resourceType].size();

                    auto findIt = deviceListOfType.find(resourceType);

                    if (findIt == deviceListOfType.end())
                    {
                        isMeet = false;
                        break;
                    }
                    else //if device exist then create json data
                    {
                        for (unsigned int i = 0; i < deviceListOfType[resourceType].size(); ++i)
                        {
                            if (i != 0)
                            {
                                jsonPackDeviceStr.append(",");
                            }
                            jsonPackDeviceStr.append(JsonPackDevice(deviceListOfType[resourceType].at(i)));
                            //std::cout << "JSON :::: " << jsonPackDeviceStr << std::endl;
                        }

                    }
                    if (valueItr + 1 != it->second.end())
                    {
                        jsonPackDeviceStr.append(",");
                    }
                }
                jsonPackDeviceStr.append("]}");
                if (isMeet == true)
                {
                    callId.append("|YES");
                    g_mapReadyDevices[callId] = jsonPackDeviceStr;
                    std::cout << " Call ID " << callId << " is OK" << std::endl;
                }
                else
                {
                    callId.append("|NO");
                    g_mapReadyDevices[callId] = jsonPackDeviceStr;
                    std::cout << " Call ID " << callId << " is not ready" << std::endl;
                }
            }
        }
        static void * observeHandler(void *param)
        {
            while (1)
            {
                sleep(4);

                if (g_mapDeviceListOfResourceType->size() < g_mapDeviceListOfResourceTypeBuffer.size())
                {
                    createMeetMandatoryList(g_mapResourceTypeListOfCallId, g_mapDeviceListOfResourceTypeBuffer);
                }
                else
                {
                    createMeetMandatoryList(g_mapResourceTypeListOfCallId, *g_mapDeviceListOfResourceType);
                }
                
                g_bufferLock.lock();
                g_mapDeviceListOfResourceTypeBuffer = (*g_mapDeviceListOfResourceType);
                g_mapDeviceOfDeviceIdBuffer = (*g_mapDeviceOfDeviceId);
                g_bufferLock.unlock();

                cout << "\t\t\tObserve Handler from client\n";
                OCStackResult result = OCPlatform::notifyObservers(g_groupManagerResource.getHandle());

                if (OC_STACK_NO_OBSERVERS == result)
                {
                    cout << "No More observers, stopping notifications" << endl;
                    g_observation = 0;
                }
            }

            return NULL;
        }
};

// For device discovery
void findResourceFromClient(std::string* typeStr)
{
    pthread_t threadId;
    pthread_create(&threadId, NULL, callFindResource, (void *) typeStr);
}

void* callFindResource(void* charStr)
{
    std::string* resourceType = (static_cast< std::string* >(charStr));
    std::string *uriStr = new std::string("coap://224.0.1.187/oc/core?rt=");
    uriStr->append(resourceType->c_str());
    std::cout << "\t\t\t FindResource Thread : " << uriStr->c_str() << std::endl;

    ResourceHandler *handler = new ResourceHandler();
    handler->initPlatform(g_platform);
    handler->setDeviceList(g_mapDeviceListOfResourceType);
    handler->setDeviceListOfId(g_mapDeviceOfDeviceId);

    g_mapResourceTypeHandlerList[*resourceType] = handler;

    //while per resource type
    while (true)
    {
        if (g_mapDeviceOfDeviceIdBuffer.size() <= g_mapDeviceOfDeviceId->size())
        {
            g_bufferLock.lock();
            g_mapDeviceListOfResourceTypeBuffer = (*g_mapDeviceListOfResourceType);
            g_mapDeviceOfDeviceIdBuffer = (*g_mapDeviceOfDeviceId);
            g_bufferLock.unlock();
        }

        removeAllDeviceList(*resourceType);
        handler->clearMapFoundedDeviceDuid();

        //std::cout << deviceOfDeviceId->size() << " : " << deviceOfDeviceIdBuffer.size() <<std::endl;

        //TODO: remove device list
        handler->getOCPlatform()->findResource("", uriStr->c_str(),
                std::bind(&ResourceHandler::foundResourceForGroupCreation, handler,
                        std::placeholders::_1));
        sleep(17);
    }
 
    delete static_cast< std::string* >(charStr);
    delete resourceType;
    delete uriStr;
    return NULL; 
}

void removeAllDeviceList(std::string resourceType)
{
    g_deviceDataLock.lock();

    auto listIt = g_mapDeviceListOfResourceType->find(resourceType);
    if (listIt != g_mapDeviceListOfResourceType->end())
    {
        for (unsigned int i = 0; i < listIt->second.size(); ++i)
        {
            OC::Device* device = listIt->second.at(i);

            auto deviceIt = g_mapDeviceOfDeviceId->find(device->m_duid);
            if (deviceIt != g_mapDeviceOfDeviceId->end())
            {
                g_mapDeviceOfDeviceId->erase(deviceIt);
            }
            //delete device;
        }
        listIt->second.clear();
        g_mapDeviceListOfResourceType->erase(listIt);
    }

   g_deviceDataLock.unlock();
}

void createGroupManagerResource(OC::OCPlatform* platform, GroupManagerHandler *gmHandler)
{
    g_groupManagerResource.createResource(*platform,std::bind(&GroupManagerHandler::groupEntityHandler, gmHandler, std::placeholders::_1,std::placeholders::_2));

}
