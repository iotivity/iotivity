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

/// @file    GroupManager.cpp
///  @brief


#include "GroupManager.h"
#include <algorithm>
#include <thread>
#include <unistd.h>

#include <string.h>

#define DESC_DELIMITER "\""
#define ACTION_DELIMITER "*"
#define ATTR_DELIMITER "|"

using namespace OC;

std::map< std::vector< std::string >, CandidateCallback > candidateRequest;
std::map< std::vector< std::string >, CandidateCallback > candidateRequestForTimer;
std::map< std::string, std::map< std::string, std::shared_ptr< OCResource > > > rtForResourceList;
std::vector< std::string > allFoundResourceTypes;

template< typename T >
bool IsSubset(std::vector< T > full, std::vector< T > sub)
{
    std::sort(full.begin(), full.end());
    std::sort(sub.begin(), sub.end());
    return std::includes(full.begin(), full.end(), sub.begin(), sub.end());
}
std::vector< std::string > &str_split(const std::string &s, char delim,
        std::vector< std::string > &elems)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim))
    {
        elems.push_back(item);
    }
    return elems;
}

std::vector< std::string > str_split(const std::string &s, char delim)
{
    std::vector< std::string > elems;
    str_split(s, delim, elems);
    return elems;
}

void GroupManager::onFoundResource(std::shared_ptr< OCResource > resource, int waitsec)
{

    std::string resourceURI;
    std::string hostAddress;
    try
    {
        // Do some operations with resource object.
        if (resource)
        {

            std::cout << "DISCOVERED Resource:" << std::endl;
            // Get the resource URI
            resourceURI = resource->uri();
            std::cout << "\tURI of the resource: " << resourceURI << std::endl;

            // Get the resource host address
            hostAddress = resource->host();
            std::cout << "\tHost address of the resource: " << hostAddress << std::endl;

            // Get the resource types
            std::cout << "\tList of resource types: " << std::endl;

            hostAddress.append(resourceURI);

            for (auto &resourceTypes : resource->getResourceTypes())
            {
                std::cout << "\t\t" << resourceTypes << std::endl;

                if (std::find(allFoundResourceTypes.begin(), allFoundResourceTypes.end(),
                        resourceTypes) == allFoundResourceTypes.end())
                {
                    allFoundResourceTypes.push_back(resourceTypes);
                }

                rtForResourceList[resourceTypes][hostAddress] = resource;
            }

            // Get the resource interfaces
            std::cout << "\tList of resource interfaces: " << std::endl;
            for (auto &resourceInterfaces : resource->getResourceInterfaces())
            {
                std::cout << "\t\t" << resourceInterfaces << std::endl;
            }

            if (waitsec == -1)
            {
                findPreparedRequest(candidateRequest);
            }
        }
        else
        {
            // Resource is invalid
            std::cout << "Resource is invalid" << std::endl;
        }

    }
    catch (std::exception& e)
    {
        //log(e.what());
    }
}

GroupManager::GroupManager(void)
{
    ;
}

/**
 * Virtual destructor
 */
GroupManager::~GroupManager(void)
{
    candidateRequest.clear();
    candidateRequestForTimer.clear();
    rtForResourceList.clear();
    allFoundResourceTypes.clear();
}

void GroupManager::findPreparedRequest(
        std::map< std::vector< std::string >, CandidateCallback > &request)
{
    std::vector < std::shared_ptr < OCResource >> resources;

    for (auto it = request.begin(); it != request.end();)
    {

        if (IsSubset(allFoundResourceTypes, it->first))
        {
            //std::cout << "IS SUBSET !!! \n";

            for (unsigned int i = 0; i < it->first.size(); ++i)
            {

                for (auto secondIt = rtForResourceList[it->first.at(i)].begin();
                        secondIt != rtForResourceList[it->first.at(i)].end(); ++secondIt)
                {
                    resources.push_back(secondIt->second);
                }
            }

            it->second(resources);

            //TODO : decide policy - callback only once
            request.erase(it++);
        }
        else
        {
            ++it;
        }

    }

}

void GroupManager::lazyCallback(int second)
{
    sleep(second);
    findPreparedRequest(candidateRequestForTimer);

}

OCStackResult GroupManager::findCandidateResources(std::vector< std::string > resourceTypes,
        CandidateCallback callback, int waitsec)
{
    if (resourceTypes.size() < 1)
    {
        return OC_STACK_ERROR;
    }

    std::sort(resourceTypes.begin(), resourceTypes.end());
    resourceTypes.erase(std::unique(resourceTypes.begin(), resourceTypes.end()),
            resourceTypes.end());

    if (waitsec != -1)
    {
        candidateRequestForTimer.insert(std::make_pair(resourceTypes, callback));
    }
    else
    {
        candidateRequest.insert(std::make_pair(resourceTypes, callback));
    }

    for (unsigned int i = 0; i < resourceTypes.size(); ++i)
    {
        std::cout << "resourceTypes : " << resourceTypes.at(i) << std::endl;
        std::string query = "coap://224.0.1.187/oc/core?rt=";
        query.append(resourceTypes.at(i));
        OCPlatform::findResource("", query.c_str(),
                std::function < void(std::shared_ptr < OCResource > resource)
                        > (std::bind(&GroupManager::onFoundResource, this, std::placeholders::_1,
                                waitsec)));
    }

    if (waitsec != -1)
    {
        std::thread exec(
                std::function< void(int second) >(
                        std::bind(&GroupManager::lazyCallback, this, std::placeholders::_1)), waitsec);
        exec.detach();
    }

    return OC_STACK_OK;
}




















/*
    Presence Check
*/



std::map<std::string, CollectionPresenceCallback> presenceCallbacks;

// Callback to presence
void GroupManager::collectionPresenceHandler(OCStackResult result, const unsigned int nonce, const std::string& hostAddress, std::string host, std::string uri)
{
    std::cout << "uri : " << uri << std::endl;
    std::cout << "host : " << host << std::endl;
    std::cout << "result : " << result << std::endl;
    switch(result)
    {
        case OC_STACK_OK:
            std::cout << "Nonce# " << nonce << std::endl;
            break;
        case OC_STACK_PRESENCE_STOPPED:
            std::cout << "Presence Stopped\n";
            break;
        case OC_STACK_PRESENCE_DO_NOT_HANDLE:
            std::cout << "Presence do not handle\n";
            break;
        case OC_STACK_PRESENCE_TIMEOUT:
            std::cout << "Presence TIMEOUT\n";
            break;
        default:
            std::cout << "Error\n";
            break;
    }

    if(presenceCallbacks.find(uri) != presenceCallbacks.end())
    {
        (presenceCallbacks.find(uri)->second)(uri,result);
    }
}

void GroupManager::checkCollectionRepresentation(const OCRepresentation& rep, CollectionPresenceCallback callback)
{
    std::cout << "\tResource URI: " << rep.getUri() << std::endl;

/* //bug not found
    if(rep.hasAttribute("name"))
    {
        std::cout << "\tRoom name: " << rep.getValue<std::string>("name") << std::endl;
    }
*/
    std::vector<OCRepresentation> children = rep.getChildren();

    for(auto oit = children.begin(); oit != children.end(); ++oit)
    {
        std::cout << "\t\tChild Resource URI: " << oit->getUri() << std::endl;
        std::vector<std::string> hostAddressVector = str_split(oit->getUri(), '/');
        std::string hostAddress = "";
        for(unsigned int i = 0 ; i  < hostAddressVector.size() ; ++i)
        {
            if(i < 3)
            {
                hostAddress.append(hostAddressVector.at(i));
                if(i!=2)
                {
                    hostAddress.append("/");
                }
            }
        }


        std::vector<std::string> resourceTypes = oit->getResourceTypes();
        for(unsigned int i = 0 ; i < resourceTypes.size() ; ++i)
        {
             std::cout << "\t\t\tresourcetype :" <<  resourceTypes.at(i) << std::endl;
        }

        std::string resourceType = "core.";
        resourceType.append(str_split(oit->getUri(), '/').at(4));
        std::cout << "\t\tconvertRT : " << resourceType << std::endl;
        std::cout << "\t\thost : " << hostAddress << std::endl;
        OCPlatform::OCPresenceHandle presenceHandle;
        OCStackResult result = OCPlatform::subscribePresence(presenceHandle, hostAddress, resourceType ,
            std::function< void(OCStackResult result, const unsigned int nonce, const std::string& hostAddress)>
                        (std::bind(&GroupManager::collectionPresenceHandler, this, std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,hostAddress,oit->getUri())));

        if(result == OC_STACK_OK)
        {
            std::cout << "\t\tOK!" << std::endl;
            presenceCallbacks.insert(std::make_pair(oit->getUri(),callback));
        }
        else
        {
            callback("",OC_STACK_ERROR);
        }

    }
}

void GroupManager::onGetForPresence(const HeaderOptions& headerOptions, const OCRepresentation& rep, const int eCode, CollectionPresenceCallback callback)
{
    if (eCode == OC_STACK_OK)
    {
        std::cout << "GET request was successful" << std::endl;
        std::cout << "Resource URI: " << rep.getUri() << std::endl;

        checkCollectionRepresentation(rep,callback);

    }
    else
    {
        std::cout << "onGET Response error: " << eCode << std::endl;
        callback("",OC_STACK_ERROR);
        std::exit(-1);
    }
}

OCStackResult GroupManager::subscribeCollectionPresence(std::shared_ptr< OCResource > collectionResource, CollectionPresenceCallback callback)
{
    OCStackResult result = OC_STACK_OK;
    //callback("core.room",OC_STACK_OK);

    QueryParamsMap queryParam;

    //parameter 1 = resourceType
    collectionResource->get("",DEFAULT_INTERFACE,queryParam,
        std::function< void(const HeaderOptions& headerOptions,const OCRepresentation& rep,const int eCode)>
                        (std::bind(&GroupManager::onGetForPresence, this, std::placeholders::_1,std::placeholders::_2,std::placeholders::_3, callback)));

    return result;
}






















/*
    Group Action
*/

std::string GroupManager::getStringFromActionSet(const ActionSet *newActionSet)
{
    std::string message = "";

    message = newActionSet->actionsetName;
    message.append("*");
    for(auto iterAction= newActionSet->listOfAction.begin(); iterAction != newActionSet->listOfAction.end();
        iterAction++)
    {
        message.append("uri=");
        message.append((*iterAction)->target);
        message.append("|");

        for( auto iterCapa = (*iterAction)->listOfCapability.begin(); iterCapa != (*iterAction)->listOfCapability.end();
            iterCapa++)
        {
            message.append((*iterCapa)->capability);
            message.append("=");
            message.append((*iterCapa)->status);

            if( iterCapa + 1 != (*iterAction)->listOfCapability.end() )
                message.append( "|");
        }

        if( iterAction + 1 !=  newActionSet->listOfAction.end())
        {
             message.append("*");
        }
    }

    return message;
}

ActionSet* GroupManager::getActionSetfromString(std::string desc)
{
    char *acitonRequest;
    char *iterTokenPtr = NULL;
    char *iterToken = NULL;
    char *description = NULL;
    char *iterDescPtr = NULL;

    char *attributes = NULL;
    char *iterAttrbutesPtr = NULL;

    char *attr = NULL;
    char *iterAttrPtr = NULL;

    std::string actionsetName;

    ActionSet* actionset = NULL;
    Action* action = NULL;

    actionset = new ActionSet();

    acitonRequest = new char[strlen((char *)desc.c_str() + 1)];
    strncpy(acitonRequest, (char *)desc.c_str(), strlen((char *)desc.c_str()) + 1);

    //printf("\t%s\n", acitonRequest);
    if(acitonRequest != NULL)
    {
        iterToken = (char *)strtok_r(acitonRequest, DESC_DELIMITER, &iterTokenPtr);

        while(iterToken != NULL)
        {
            if( strcmp(iterToken, "ActionSet") == 0)
            {// if iterToken is ActionSet, will be created and added a new action set.
                iterToken = (char *)strtok_r(NULL, DESC_DELIMITER, &iterTokenPtr);           // it is mean ':'.
                iterToken = (char *)strtok_r(NULL, DESC_DELIMITER, &iterTokenPtr);           // it is body of action description.

                description = new char[(strlen(iterToken) + 1)];
                strncpy(description, iterToken, strlen(iterToken) + 1);

                // Find the action name from description.
                iterDescPtr = NULL;
                iterToken = (char *)strtok_r(description, ACTION_DELIMITER, &iterDescPtr);
                //while(iterToken != NULL)
                if(iterToken != NULL)
                {
                    //  Actionset name.
                    actionsetName = std::string(iterToken);
                    // printf("ACTION SET NAME :: %s\n", *actionsetName);
                    iterToken = (char *)strtok_r(NULL, ACTION_DELIMITER, &iterDescPtr);
                } else {
                    return NULL;

                }// end Action Set Name.

                // New ActionSet Add to OCResource's ActionSet list.
                // 1. Allocate a new pointer for actionset.
                actionset  = new ActionSet;
                // 2. Initiate actionset.
                actionset->actionsetName = std::string(actionsetName);
                // printf("ACTION SET NAME :: %s\n", actionset->actionsetName);

                while(iterToken != NULL)
                {
                    action = new Action;

                    // printf("ATTR Copied :: %s\n", iterToken);
                    attributes = new char[strlen(iterToken) + 1];
                    strncpy(attributes, iterToken, strlen(iterToken) + 1);
                    // printf("ATTR Copied :: %s\n", attributes);

                    iterToken = (char *)strtok_r(attributes, ATTR_DELIMITER, &iterAttrbutesPtr);
                    while(iterToken != NULL)
                    {
                        attr = new char[(strlen(iterToken) + 1)];
                        strncpy(attr, iterToken, strlen(iterToken) + 1);

                        iterToken = (char *)strtok_r(attr, "=",&iterAttrPtr);
                        while(iterToken != NULL)
                        {
                            // Find the URI from description.
                            if(strcmp(iterToken, "uri") == 0)
                            {
                                iterToken = (char *)strtok_r(NULL, "=", &iterAttrPtr);
                                // printf("uri :: %s\n", iterToken);

                                action->target =std::string(iterToken);
                            } else {
                                Capability* capa = new Capability();
                                // printf("%s :: ", iterToken);
                                capa->capability = std::string(iterToken);
                                iterToken = (char *)strtok_r(NULL, "=", &iterAttrPtr);
                                // printf("%s\n", iterToken);
                                capa->status = std::string(iterToken);

                                action->listOfCapability.push_back(capa);
                            }

                            iterToken = (char *)strtok_r(NULL, "=", &iterAttrPtr);
                        }

                        iterToken = (char *)strtok_r(NULL, ATTR_DELIMITER, &iterAttrbutesPtr);
                    }// End of Action

                    actionset->listOfAction.push_back(action);

                    iterToken = (char *)strtok_r(NULL, ACTION_DELIMITER, &iterDescPtr);
                }

                return actionset;
            }
            iterToken = (char *)strtok_r(NULL, DESC_DELIMITER, &iterTokenPtr);
        }
    }

    return NULL;
}

OCStackResult GroupManager::addActionSet(std::shared_ptr< OCResource > resource, const ActionSet* newActionSet, PutCallback cb)
{
    // BUILD message of ActionSet which it is included delimiter.
    std::string message = getStringFromActionSet(newActionSet);
    OCRepresentation rep;

    rep.setValue("ActionSet", message);

    return resource->put(resource->getResourceTypes().front(),
                        GROUP_INTERFACE, rep, QueryParamsMap(),
                        cb );
}


OCStackResult GroupManager::executeActionSet(std::shared_ptr< OCResource > resource, std::string actionsetName, PostCallback cb)
{
    OCRepresentation rep;

    rep.setValue("DoAction", actionsetName);
    return resource->post(resource->getResourceTypes().front(),
                        GROUP_INTERFACE, rep, QueryParamsMap(),
                        cb );
}

OCStackResult GroupManager::getActionSet(std::shared_ptr< OCResource > resource, std::string actionsetName, GetCallback cb)
{
    OCRepresentation rep;

    rep.setValue("GetActionSet", actionsetName);

    return resource->put(resource->getResourceTypes().front(),
                        GROUP_INTERFACE, rep, QueryParamsMap(),
                        cb );
}


OCStackResult GroupManager::deleteActionSet(std::shared_ptr< OCResource > resource, std::string actionsetName, PostCallback cb)
{
    OCRepresentation rep;

    rep.setValue("DelActionSet", actionsetName);

    return resource->put(resource->getResourceTypes().front(),
                        GROUP_INTERFACE, rep, QueryParamsMap(),
                        cb );
}
