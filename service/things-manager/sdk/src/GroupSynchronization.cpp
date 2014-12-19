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

/// @file    GroupSynchronization.cpp
/// @brief

#include "GroupSynchronization.h"

using namespace OC;

namespace OIC
{

GroupSynchronization* GroupSynchronization::groupSyncnstance = NULL;

GroupSynchronization* GroupSynchronization::getInstance()
{
    if (groupSyncnstance == NULL)
    {
        groupSyncnstance = new GroupSynchronization();
    }
    return groupSyncnstance;
}

void GroupSynchronization::deleteInstance()
{
    if (groupSyncnstance)
    {
        delete groupSyncnstance;
        groupSyncnstance = NULL;
    }
}

OCStackResult GroupSynchronization::findGroup(
        std::vector< std::string > collectionResourceTypes, FindCallback callback)
{
    cout << "GroupSynchronization::findGroup" << endl;

    foundGroupResourceList.clear();

    findCallback = callback;

    for (unsigned int i = 0; i < collectionResourceTypes.size(); ++i)
    {
        std::string query = "coap://224.0.1.187/oc/core?rt=";
        query.append(collectionResourceTypes.at(i));
        cout << "GroupSynchronization::findGroup - " << query << endl;

        OCPlatform::findResource("", query,
                std::bind(&GroupSynchronization::onFindGroup, this, std::placeholders::_1));
    }

    // thread to check if GroupSynchronization::onFoundGroup is called or not.
    std::thread t(std::bind(&GroupSynchronization::checkFindGroup, this));
    t.detach();

    return OC_STACK_OK;
}

OCStackResult GroupSynchronization::createGroup(std::string collectionResourceType)
{
    foundGroupResourceList.clear();

    OCResourceHandle collectionResHandle = NULL;
    OCResourceHandle groupSyncResHandle = NULL;

    if (0 != collectionResourceType.length())
    {
        cout << "GroupSynchronization::createGroup - The created group is added." << endl;

        OCStackResult result;

        // creating master collection resource
        std::string collectionUri = "/" + collectionResourceType;
        int i;
        while ((i = collectionUri.find(".")) != std::string::npos)
        {
            collectionUri.replace(i, 1, "/");
        }
        cout << "GroupSynchronization::createGroup : collection uri - " << collectionUri
                << ", type - " << collectionResourceType << endl;

        std::string resourceInterface = DEFAULT_INTERFACE;

        result = OCPlatform::registerResource(collectionResHandle, collectionUri,
                collectionResourceType, resourceInterface, NULL,
                OC_DISCOVERABLE | OC_OBSERVABLE);
        if (result != OC_STACK_OK)
        {
            cout << "To register resource (" << collectionUri << ") was unsuccessful. result - "
                    << result << endl;
            goto Error;
        }

        OCPlatform::bindInterfaceToResource(collectionResHandle, GROUP_INTERFACE);
        if (result != OC_STACK_OK)
        {
            cout << "To bind Interface (collection) was unsuccessful. result - " << result
                    << endl;
        }

        collectionResourceHandleList[collectionResourceType] = collectionResHandle;

        // creating master group sync resource
        std::string groupSyncUri = collectionUri + "/groupsync";
        std::string groupSyncResType = collectionResourceType + ".groupsync";

//        cout << "GroupSynchronization::createGroup : groupSync uri - " << groupSyncUri
//                << ", type - " << collectionResourceType << endl;

        result = OCPlatform::registerResource(groupSyncResHandle, groupSyncUri,
                groupSyncResType, resourceInterface,
                std::bind(&GroupSynchronization::groupEntityHandler, this,
                        std::placeholders::_1), OC_DISCOVERABLE | OC_OBSERVABLE);
        if (result != OC_STACK_OK)
        {
            cout << "To register resource (groupsync) was unsuccessful. result - " << result
                    << endl;
            goto Error;
        }

        groupSyncResourceHandleList[collectionResourceType] = groupSyncResHandle;

        return OC_STACK_OK;
    }
    else
    {
        cout << "GroupSynchronization::createGroup : Error! Input params are wrong." << endl;
        return OC_STACK_INVALID_PARAM;
    }

    Error:

    if (collectionResHandle)
    {
        OCPlatform::unregisterResource(collectionResHandle);
        auto iterator = collectionResourceHandleList.find(collectionResourceType);
        if (iterator != collectionResourceHandleList.end())
        {
            collectionResourceHandleList.erase(iterator);
        }
    }

    if (groupSyncResHandle)
    {
        OCPlatform::unregisterResource(groupSyncResHandle);
        auto iterator = groupSyncResourceHandleList.find(collectionResourceType);
        if (iterator != groupSyncResourceHandleList.end())
        {
            groupSyncResourceHandleList.erase(iterator);
        }
    }

    return OC_STACK_NO_RESOURCE;
}

OCStackResult GroupSynchronization::joinGroup(std::string collectionResourceType,
        OCResourceHandle resourceHandle)
{
    if ((0 != collectionResourceType.length()) && (resourceHandle))
    {
        auto resIt = collectionResourceHandleList.find(collectionResourceType);
        if (resIt == groupSyncResourceHandleList.end())
        {
            cout << "GroupSynchronization::joinGroup : error! There is no collection to join"
                    << endl;
            return OC_STACK_INVALID_PARAM;
        }

        OCResourceHandle collectionResHandle = resIt->second;

        OCStackResult result = OCPlatform::bindResource(collectionResHandle, resourceHandle);
        if (result != OC_STACK_OK)
        {
            cout << "GroupSynchronization::joinGroup : To bind resource was unsuccessful."
                    << "result - " << result << endl;
            return OC_STACK_ERROR;
        }
        cout << "GroupSynchronization::joinGroup : "
                << "To bind collectionResHandle and resourceHandle" << endl;

        std::vector< OCResourceHandle > childHandleList;

        auto childIt = childResourceHandleList.find(collectionResHandle);
        if (childIt != childResourceHandleList.end())
        {
            childHandleList = childIt->second;
        }

        childHandleList.push_back(resourceHandle);
        childResourceHandleList[collectionResHandle] = childHandleList;

        deviceResourceHandleList.push_back(resourceHandle);

        debugGroupSync();
    }
    else
    {
        cout << "GroupSynchronization::joinGroup : Error! input params are wrong." << endl;
        return OC_STACK_INVALID_PARAM;
    }

    return OC_STACK_OK;
}

OCStackResult GroupSynchronization::joinGroup(const std::shared_ptr< OCResource > resource,
        OCResourceHandle resourceHandle)
{
    if ((resource) && (resourceHandle))
    {
        cout << "GroupSynchronization::joinGroup" << endl;

        // making representation to join group
        std::string method = "joinGroup";
        std::vector< std::string > type = resource->getResourceTypes();
        std::string resourceType;
        resourceType.append(OCGetResourceTypeName(resourceHandle, 0));

        OCRepresentation rep;
        rep.setValue("method", method);
        rep.setValue("collectionResourceType", type[0]);
        rep.setValue("resourceType", resourceType);

        cout << "\tmethod - " << method << endl;
        cout << "\tcollectionResourceType - " << type[0] << endl;
        cout << "\tresourceType - " << resourceType << endl;

        // creating group sync resource with the received collection resource.
        // entity handler of group sync is used to join group.
        std::string host = resource->host();
        std::string uri = resource->uri() + "/groupsync";

        std::vector< std::string > resourceTypes;
        std::string temp;
        for (unsigned int i = 0; i < type.size(); ++i)
        {
            temp = type[0] + ".groupsync";
            resourceTypes.push_back(temp);
        }

        std::vector< std::string > resourceInterface;
        resourceInterface.push_back(DEFAULT_INTERFACE);

        OCResource::Ptr groupSyncResource = OCPlatform::constructResourceObject(host, uri, 1,
                resourceTypes, resourceInterface);
        groupSyncResourceList[type[0]] = groupSyncResource;

        cout << "GroupSynchronization::joinGroup : creating groupSyncResource." << endl;

        // Create QueryParameters Map and add query params (if any)
        QueryParamsMap queryParamsMap;

        // request to join group to the remote group sync resource
        OCStackResult result = groupSyncResource->put(rep, queryParamsMap,
                std::bind(&GroupSynchronization::onJoinGroup, this, std::placeholders::_1,
                        std::placeholders::_2, std::placeholders::_3));
        if (OC_STACK_OK == result)
        {
            cout << "GroupSynchronization::joinGroup : groupSyncResource->put was successful."
                    << endl;
        }
        else
        {
            cout << "GroupSynchronization::joinGroup : "
                    << "groupSyncResource->put was unsuccessful. result - " << result << endl;
        }

        // saving the remote collection resource.
        // It is used in onJoinGroup() and onGetJoinedRemoteChild().
        remoteCollectionResource = resource;

        // saving the resource handle to join. It is used in onGetJoinedRemoteChild()
        deviceResourceHandle = resourceHandle;

        return OC_STACK_OK;
    }
    else
    {
        cout << "GroupSynchronization::joinGroup : Error! Input params are wrong." << endl;
        return OC_STACK_INVALID_PARAM;
    }
}

OCStackResult GroupSynchronization::leaveGroup(std::string collectionResourceType,
        OCResourceHandle resourceHandle)
{
    if ((0 != collectionResourceType.length()) && (resourceHandle))
    {
        cout << "GroupSynchronization::leaveGroup : collectionResourceType - "
                << collectionResourceType << endl;

        OCResourceHandle collectionResHandle;

        auto handleIt = groupSyncResourceHandleList.find(collectionResourceType);

        // if groupSyncResourceHandleList has resourceType,
        // this app created collection resource handle.
        if (handleIt != groupSyncResourceHandleList.end())
        {
            handleIt = collectionResourceHandleList.find(collectionResourceType);
            if (handleIt == collectionResourceHandleList.end())
            {
                cout << "GroupSynchronization::leaveGroup : "
                        << "Error! There is no collection resource handle to leave." << endl;
                return OC_STACK_INVALID_PARAM;
            }

            collectionResHandle = handleIt->second;
//            cout << "GroupSynchronization::leaveGroup : collection handle uri - "
//                    << OCGetResourceUri(collectionResHandle) << endl;

            OCStackResult result = OCPlatform::unbindResource(collectionResHandle,
                    resourceHandle);
            if (OC_STACK_OK == result)
            {
                cout << "GroupSynchronization::leaveGroup : "
                        << "To unbind resource was successful." << endl;
            }
            else
            {
                cout << "GroupSynchronization::leaveGroup : "
                        << "To unbind resource was unsuccessful. result - " << result << endl;
            }

            auto It = std::find(deviceResourceHandleList.begin(),
                    deviceResourceHandleList.end(), resourceHandle);
            if (It == deviceResourceHandleList.end()) // there is no resource handle to find
            {
                result = OCPlatform::unregisterResource(resourceHandle);
                if (OC_STACK_OK == result)
                {
                    cout << "GroupSynchronization::leaveGroup : "
                            << "To unregister resource was successful." << endl;
                }
                else
                {
                    cout << "GroupSynchronization::leaveGroup : "
                            << "To unregister resource was unsuccessful. result - " << result
                            << endl;
                }
            }
            else
            {
                cout << "GroupSynchronization::leaveGroup : "
                        << "This resource cannot be unregistered." << endl;
                deviceResourceHandleList.erase(It);
            }

            auto handleListIt = childResourceHandleList.find(collectionResHandle);
            if (handleListIt == childResourceHandleList.end())
            {
                cout << "GroupSynchronization::leaveGroup : "
                        << "Error! There is no child resource list to delete." << endl;
                return OC_STACK_INVALID_PARAM;
            }

            std::vector< OCResourceHandle > childList = handleListIt->second;
            auto childIt = std::find(childList.begin(), childList.end(), resourceHandle);
            if (childIt != childList.end())
            {
                cout << "GroupSynchronization::groupEntityHandler : "
                        << "Found! The resource to leave is found." << endl;
                childList.erase(childIt);
            }

            childResourceHandleList[collectionResHandle] = childList;

            debugGroupSync();
        }
        else // requesting to unbind this resourceHandle to the remote collection resource
        {
            auto resourceIt = groupSyncResourceList.find(collectionResourceType);

            if (resourceIt == groupSyncResourceList.end())
            {
                cout << "GroupSynchronization::leaveGroup : "
                        << "Error! There is no collectin resource type to leave." << endl;
                return OC_STACK_INVALID_PARAM;
            }

            std::shared_ptr< OCResource > resource = resourceIt->second;
//            cout << "GroupSynchronization::leaveGroup : group sync resource uri - "
//                    << resource->uri() << endl;

            handleIt = collectionResourceHandleList.find(collectionResourceType);
            if (handleIt == collectionResourceHandleList.end())
            {
                cout << "GroupSynchronization::leaveGroup : "
                        << "Error! There is no collection resource handle to leave." << endl;
                return OC_STACK_INVALID_PARAM;
            }

            collectionResHandle = handleIt->second;

            // making representation to leave group
            std::string method = "leaveGroup";
            std::string type = OCGetResourceTypeName(collectionResHandle, 0);
            std::string resourceType;
            resourceType.append(OCGetResourceTypeName(resourceHandle, 0));

            OCRepresentation rep;
            rep.setValue("method", method);
            rep.setValue("collectionResourceType", type);
            rep.setValue("resourceType", resourceType);

            cout << "\tmethod - " << method << endl;
            cout << "\tcollectionResourceType - " << type << endl;
            cout << "\tresourceType - " << resourceType << endl;

            QueryParamsMap queryParamsMap;

            // request to leave group to the remote group sync resource
            OCStackResult result = resource->put(rep, queryParamsMap,
                    std::bind(&GroupSynchronization::onLeaveGroup, this, std::placeholders::_1,
                            std::placeholders::_2, std::placeholders::_3));
            if (OC_STACK_OK == result)
            {
                cout << "GroupSynchronization::leaveGroup : "
                        << "groupSyncResource->put was successful." << endl;
            }
            else
            {
                cout << "GroupSynchronization::leaveGroup : "
                        << "groupSyncResource->put was unsuccessful. result - " << result
                        << endl;
            }

            // deleting all remote resources. These are copied in onGetJoinedRemoteChild()
            deleteGroup(collectionResourceType);
        }
    }
    else
    {
        cout << "GroupSynchronization::leaveGroup : Error! Input params are wrong." << endl;
        return OC_STACK_INVALID_PARAM;
    }

    return OC_STACK_OK;
}

void GroupSynchronization::deleteGroup(std::string collectionResourceType)
{
    if (0 != collectionResourceType.length())
    {
        cout << "GroupSynchronization::deleteGroup" << endl;

        OCStackResult result;

        auto handleIt = collectionResourceHandleList.find(collectionResourceType);
        if (handleIt == collectionResourceHandleList.end())
        {
            cout << "GroupSynchronization::deleteGroup : "
                    << "Error! There is no collection resource handle to delete." << endl;
            return;
        }
        OCResourceHandle collectionResHandle = handleIt->second;

        collectionResourceHandleList.erase(handleIt);

        auto handleListIt = childResourceHandleList.find(collectionResHandle);
        if (handleListIt == childResourceHandleList.end())
        {
            cout << "GroupSynchronization::deleteGroup : "
                    << "Error! There is no child resource list to delete." << endl;
            return;
        }
        std::vector< OCResourceHandle > childList = handleListIt->second;

        childResourceHandleList.erase(handleListIt);

        result = OCPlatform::unbindResources(collectionResHandle, childList);
        if (OC_STACK_OK == result)
        {
            cout << "GroupSynchronization::deleteGroup : "
                    << "To unbind resources was successful." << endl;
        }
        else
        {
            cout << "GroupSynchronization::deleteGroup : "
                    << "To unbind resources was unsuccessful. result - " << result << endl;
        }

        result = OCPlatform::unregisterResource(collectionResHandle);
        if (result != OC_STACK_OK)
        {
            cout << "GroupSynchronization::deleteGroup : "
                    << "To unregister collection resource handle was successful." << endl;
        }
        else
        {
            cout << "GroupSynchronization::deleteGroup : "
                    << " To unregister collection resource handle was unsuccessful. result - "
                    << result << endl;
        }

        OCResourceHandle resourceHandle;
        std::vector< OCResourceHandle >::iterator It;

        for (unsigned int i = 0; i < childList.size(); i++)
        {
            resourceHandle = childList.at(i);

            It = std::find(deviceResourceHandleList.begin(), deviceResourceHandleList.end(),
                    resourceHandle);
            if (It != deviceResourceHandleList.end()) // find !!
            {
                deviceResourceHandleList.erase(It);
            }
            else
            {
                result = OCPlatform::unregisterResource(resourceHandle);
                if (OC_STACK_OK == result)
                {
                    cout << "GroupSynchronization::deleteGroup : UnregisterResource(" << i + 1
                            << ") was successful." << endl;
                }
                else
                {
                    cout << "GroupSynchronization::deleteGroup : UnregisterResource(" << i + 1
                            << ") was unsuccessful. result - " << result << endl;
                }
            }
        }

        handleIt = groupSyncResourceHandleList.find(collectionResourceType);

        // if groupSyncResourceHandleList has resourceType,
        // group sync of this app created collection resource.
        if (handleIt != groupSyncResourceHandleList.end())
        {
            resourceHandle = handleIt->second; // group sync resource handle
            result = OCPlatform::unregisterResource(resourceHandle);
            if (OC_STACK_OK == result)
            {
                cout << "GroupSynchronization::deleteGroup : "
                        << "To unregister group sync resource handle was successful." << endl;
            }
            else
            {
                cout << "GroupSynchronization::deleteGroup : "
                        << "To unregister group sync resource handle was unsuccessful. "
                        << "result - " << result << endl;
            }

            groupSyncResourceHandleList.erase(handleIt);
        }

        auto resourceIt = groupSyncResourceList.find(collectionResourceType);
        if (resourceIt != groupSyncResourceList.end())
        {
            groupSyncResourceList.erase(resourceIt);
        }

        debugGroupSync();
    }
    else
    {
        cout << "GroupSynchronization::deleteGroup : Error! Input params are wrong." << endl;
    }
}

std::map< std::string, OCResourceHandle > GroupSynchronization::getGroupList()
{
    return collectionResourceHandleList;
}

OCEntityHandlerResult GroupSynchronization::groupEntityHandler(
        const std::shared_ptr< OCResourceRequest > request)
{
    cout << "GroupSynchronization::groupEntityHandler\n";

    if (request)
    {
        // Get the request type and request flag
        std::string requestType = request->getRequestType();
        int requestFlag = request->getRequestHandlerFlag();

        if (requestFlag == RequestHandlerFlag::InitFlag)
        {
            cout << "\trequestFlag : Init\n";

            // entity handler to perform resource initialization operations
        }
        else if (requestFlag == RequestHandlerFlag::RequestFlag)
        {
            cout << "\trequestFlag : Request\n";

            // If the request type is GET
            if (requestType == "GET")
            {
                cout << "\t\trequestType : GET\n";
            }
            else if (requestType == "PUT")
            {
                cout << "\t\trequestType : PUT\n";

                //get method name, group resource type and resource type to join group
                OCRepresentation rp = request->getResourceRepresentation();
                std::string methodType = rp.getValue< std::string >("method");
                std::string collectionResourceType = rp.getValue< std::string >(
                        "collectionResourceType");
                std::string resourceType = rp.getValue< std::string >("resourceType");

                cout << "\t\t\tmethod : " << methodType << endl;
                cout << "\t\t\tcollection resourceType : " << collectionResourceType << endl;
                cout << "\t\t\tresourceType : " << resourceType << endl;

                auto handleIt = collectionResourceHandleList.find(collectionResourceType);
                if (handleIt == collectionResourceHandleList.end())
                {
                    cout << "GroupSynchronization::groupEntityHandler : "
                            << "Error! There is no collection resource handle to delete."
                            << endl;
                    return OC_EH_ERROR;
                }
                collectionResourceHandle = handleIt->second;
                // in case of join group it is used in onFindResource()

                if (methodType == "joinGroup")
                {
                    std::string resourceName = "coap://224.0.1.187/oc/core?rt=";
                    resourceName += resourceType;
                    cout << "\t\t\tresourceName : " << resourceName << endl;

                    resourceRequest = request;

                    OCPlatform::findResource("", resourceName,
                            std::bind(&GroupSynchronization::onFindResource, this,
                                    std::placeholders::_1));
                }
                else if (methodType == "leaveGroup")
                {
                    auto it = childResourceHandleList.find(collectionResourceHandle);
                    if (it == childResourceHandleList.end())
                    {
                        cout << "GroupSynchronization::groupEntityHandler : "
                                << "Error! There is no child resource list." << endl;
                        return OC_EH_ERROR;
                    }

                    std::vector< OCResourceHandle > childList = it->second;
                    OCResourceHandle resourceHandle;
                    for (auto childIt = childList.begin(); childIt != childList.end();)
                    {
                        resourceHandle = (*childIt);
                        char* type = (char*) OCGetResourceTypeName(resourceHandle, 0);

                        if (0 == resourceType.compare(type))
                        {
                            cout << "GroupSynchronization::groupEntityHandler : "
                                    << "Found! The resource to leave is found. - " << type
                                    << endl;

                            childIt = childList.erase(childIt++);

                            OCStackResult result = OCPlatform::unbindResource(
                                    collectionResourceHandle, resourceHandle);
                            if (OC_STACK_OK == result)
                            {
                                cout << "GroupSynchronization::groupEntityHandler : "
                                        << "To unbind resource was successful." << endl;
                            }
                            else
                            {
                                cout << "GroupSynchronization::groupEntityHandler : "
                                        << "To unbind resource was unsuccessful. result - "
                                        << result << endl;
                            }

                            result = OCPlatform::unregisterResource(resourceHandle);
                            if (OC_STACK_OK == result)
                            {
                                cout << "GroupSynchronization::groupEntityHandler : "
                                        << "To unregister resource was successful." << endl;
                            }
                            else
                            {
                                cout << "GroupSynchronization::groupEntityHandler : "
                                        << "To unregister resource was unsuccessful. result - "
                                        << result << endl;
                            }

//                            break;
                        }
                        else
                        {
                            ++childIt;
                        }

                    }

                    childResourceHandleList[collectionResourceHandle] = childList;

                    debugGroupSync();

                    auto pResponse = std::make_shared< OC::OCResourceResponse >();
                    pResponse->setRequestHandle(request->getRequestHandle());
                    pResponse->setResourceHandle(request->getResourceHandle());
                    pResponse->setErrorCode(200);
                    pResponse->setResponseResult(OC_EH_OK);

                    OCRepresentation rep = request->getResourceRepresentation();
                    pResponse->setResourceRepresentation(rep, DEFAULT_INTERFACE);
                    if (OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                    {
                        cout << "GroupSynchronization::groupEntityHandler : "
                                << "sendResponse is successful." << endl;
                    }
                }

                if (methodType != "") //TODO: Check groupmethodtype NULL
                {
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
            cout << "\trequestFlag : Observer\n";
        }
    }
    else
    {
        std::cout << "Request invalid" << std::endl;
    }

    return OC_EH_OK;
}

void GroupSynchronization::onFindGroup(std::shared_ptr< OCResource > resource)
{
    cout << "GroupSynchronization::onFindGroup" << endl;

    try
    {
        if (resource)
        {
//////////////////////////////////////////////////////////////////////////////////////////////////
////////////debugging
            std::string resourceURI;
            std::string hostAddress;

            // Get the resource URI
            resourceURI = resource->uri();
            cout << "\tURI of the resource: " << resourceURI << endl;

            // Get the resource host address
            hostAddress = resource->host();
            cout << "\tHost address of the resource: " << hostAddress << endl;

            hostAddress.append(resourceURI);

            // Get the resource types
            cout << "\tList of resource types: " << endl;

            for (auto &resourceTypes : resource->getResourceTypes())
            {
                cout << "\t\t" << resourceTypes << endl;
            }

            // Get the resource interfaces
            cout << "\tList of resource interfaces: " << endl;
            for (auto &resourceInterfaces : resource->getResourceInterfaces())
            {
                cout << "\t\t" << resourceInterfaces << endl;
            }
//////////////////////////////////////////////////////////////////////////////////////////////////

            if (false == IsSameGroup(resource))
            {
                saveGroup(resource);
                findCallback(resource);
            }
        }
        else
        {
            // Resource is invalid
            cout << "Resource is invalid" << endl;
            findCallback(NULL);
        }

    }
    catch (std::exception& e)
    {
        //log(e.what());
    }
}

void GroupSynchronization::checkFindGroup(void)
{
    cout << "GroupSynchronization::checkFindGroup" << endl;

    for (int i = 0; i < 15; i++)
    {
        std::chrono::milliseconds workTime(300);
        std::this_thread::sleep_for(workTime);

        std::lock_guard < std::mutex > guard(foundGroupMutex);

        if (false == foundGroupResourceList.empty())
        {
            cout << "GroupSynchronization::checkFoundGroup : " << "Some group is received."
                    << endl;
            return;
        }
    }

    cout << "GroupSynchronization::checkFoundGroup : "
            << "It is failed to find resource within 3s." << endl;

    onFindGroup(NULL);
    return;
}

bool GroupSynchronization::IsSameGroup(std::shared_ptr< OCResource > resource)
{
    std::lock_guard < std::mutex > guard(foundGroupMutex);

    if (true == foundGroupResourceList.empty())
    {
        cout << "GroupSynchronization::IsSameGroup : There is no found group." << endl;
        return false;
    }

    std::string foundHostAddress, savedHostAddress;
    foundHostAddress = resource->host();
//    foundHostAddress.append (resource->uri());

    for (unsigned int i = 0; i < foundGroupResourceList.size(); ++i)
    {
        savedHostAddress = (foundGroupResourceList.at(i))->host();
//        savedHostAddress.append ((foundGroupResourceList.at(i))->uri());
//        cout << "GroupSynchronization::IsSameGroup : foundHostAddress - " << foundHostAddress
//                << ", savedHostAddress - " << savedHostAddress << endl;

        if (0 == foundHostAddress.compare(savedHostAddress.c_str()))
        {
            cout << "GroupSynchronization::IsSameGroup : Found! The same group is found."
                    << endl;
            return true;
        }
    }

    cout << "GroupSynchronization::IsSameGroup :  There is no same group." << endl;
    return false;
}

void GroupSynchronization::saveGroup(std::shared_ptr< OCResource > resource)
{
    cout << "GroupSynchronization::saveGroup" << endl;

    std::lock_guard < std::mutex > guard(foundGroupMutex);

    foundGroupResourceList.push_back(resource);
}

void GroupSynchronization::onJoinGroup(const HeaderOptions& headerOptions,
        const OCRepresentation& rep, const int eCode)
{
    if (eCode == OC_STACK_OK)
    {
        cout << "GroupSynchronization::onJoinGroup : " << endl;

        if (remoteCollectionResource)
        {
            std::string resourceInterface = DEFAULT_INTERFACE;
            QueryParamsMap queryParamsMap;

            OCStackResult result = remoteCollectionResource->get("", resourceInterface,
                    queryParamsMap,
                    std::bind(&GroupSynchronization::onGetJoinedRemoteChild, this,
                            std::placeholders::_1, std::placeholders::_2,
                            std::placeholders::_3));
            if (OC_STACK_OK == result)
            {
                cout << "GroupSynchronization::onJoinGroup : "
                        << "remoteCollectionResource->get was successful." << endl;
            }
            else
            {
                cout << "GroupSynchronization::onJoinGroup : "
                        << "remoteCollectionResource->get was unsuccessful. result - " << result
                        << endl;
            }
        }
    }
    else
    {
        cout << "GroupSynchronization::onJoinGroup : error - " << eCode << endl;
    }
}

void GroupSynchronization::onFindResource(std::shared_ptr< OCResource > resource)
{
    cout << "GroupSynchronization::onFindResource" << endl;

    if (resource)
    {
//////////////////////////////////////////////////////////////////////////////////////////////////
////////// debugging
        std::string resourceURI;
        std::string hostAddress;

        // Get the resource URI
        resourceURI = resource->uri();
        cout << "\tURI of the resource: " << resourceURI << endl;

        // Get the resource host address
        hostAddress = resource->host();
        cout << "\tHost address of the resource: " << hostAddress << endl;

        hostAddress.append(resourceURI);

        // Get the resource types
        cout << "\tList of resource types: " << endl;

        for (auto &resourceTypes : resource->getResourceTypes())
        {
            cout << "\t\t" << resourceTypes << endl;
        }

        // Get the resource interfaces
        cout << "\tList of resource interfaces: " << endl;
        for (auto &resourceInterfaces : resource->getResourceInterfaces())
        {
            cout << "\t\t" << resourceInterfaces << endl;
        }
//////////////////////////////////////////////////////////////////////////////////////////////////

        OCResourceHandle resourceHandle;
        OCStackResult result = OCPlatform::registerResource(resourceHandle, resource);
        if (result != OC_STACK_OK)
        {
            cout << "GroupSynchronization::"
                    << "onFindResource - Resource to join creation was unsuccessful. result - "
                    << result << endl;
            return;
        }
//        cout << "GroupSynchronization::onFindResource : creating resourceHandle. resource type - "
//                << OCGetResourceTypeName(resourceHandle, 0) << endl;

        result = OCPlatform::bindResource(collectionResourceHandle, resourceHandle);
        if (result != OC_STACK_OK)
        {
            cout << "GroupSynchronization::onFindResource : "
                    << "To bind resource was unsuccessful. result - " << result << endl;
            return;
        }
        cout << "GroupSynchronization::onFindResource : "
                << "To bind joinGroupHandle and resourceHandle was successful." << endl;

        auto it = childResourceHandleList.find(collectionResourceHandle);
        std::vector< OCResourceHandle > childHandleList;
        if (it != childResourceHandleList.end())
        {
            childHandleList = it->second;
        }

        childHandleList.push_back(resourceHandle);
        childResourceHandleList[collectionResourceHandle] = childHandleList;

        auto pResponse = std::make_shared< OC::OCResourceResponse >();
        pResponse->setRequestHandle(resourceRequest->getRequestHandle());
        pResponse->setResourceHandle(resourceRequest->getResourceHandle());
        pResponse->setErrorCode(200);
        pResponse->setResponseResult(OC_EH_OK);

        OCRepresentation rep = resourceRequest->getResourceRepresentation();
        pResponse->setResourceRepresentation(rep, DEFAULT_INTERFACE);
        if (OC_STACK_OK == OCPlatform::sendResponse(pResponse))
        {
            cout << "GroupSynchronization::onFindResource : sendResponse is successful."
                    << endl;
        }
    }
    else
    {
        cout << "GroupSynchronization::onFindResource : "
                << "Resource is invalid. So a new Group Resource has to be created." << endl;
    }

    debugGroupSync();
}

void GroupSynchronization::onGetJoinedRemoteChild(const HeaderOptions& headerOptions,
        const OCRepresentation& rep, const int eCode)
{
    if (eCode == OC_STACK_OK)
    {
        cout << "GroupSynchronization::onGetJoinedRemoteChild" << endl;

//////////////////////////////////////////////////////////////////////////////////////////////////
////////// debugging
        std::string resourceURI;

        // Get the resource URI
        resourceURI = rep.getUri();
        cout << "\tURI of the resource: " << resourceURI << endl;

        // Get the resource types
        cout << "\tList of resource types: " << endl;

        for (auto &resourceTypes : rep.getResourceTypes())
        {
            cout << "\t\t" << resourceTypes << endl;
        }

        // Get the resource interfaces
        cout << "\tList of resource interfaces: " << endl;
        for (auto &resourceInterfaces : rep.getResourceInterfaces())
        {
            cout << "\t\t" << resourceInterfaces << endl;
        }

        std::vector< OCRepresentation > childList = rep.getChildren();
        OCRepresentation child;
        for (unsigned int i = 0; i < childList.size(); ++i)
        {
            cout << "\n\tchild resource - " << i + 1 << endl;

            child = childList.at(i);
            resourceURI = child.getUri();
            cout << "\t\tURI of the resource: " << resourceURI << endl;

            cout << "\t\tList of resource types: " << endl;
            for (auto &types : child.getResourceTypes())
            {
                cout << "\t\t\t" << types << endl;
            }

            cout << "\tList of resource interfaces: " << endl;
            for (auto &interfaces : child.getResourceInterfaces())
            {
                cout << "\t\t\t" << interfaces << endl;
            }
        }
//////////////////////////////////////////////////////////////////////////////////////////////////

        // creating remote collection resource handle
        OCResourceHandle remoteCollectionResourceHandle;
        resourceURI = remoteCollectionResource->uri();
        std::vector< std::string > types = remoteCollectionResource->getResourceTypes();
        std::vector< std::string > interfaces =
                remoteCollectionResource->getResourceInterfaces();

        OCStackResult result = OCPlatform::registerResource(remoteCollectionResourceHandle,
                resourceURI, types[0], interfaces[0], NULL, OC_OBSERVABLE);
        if (result != OC_STACK_OK)
        {
            cout << "GroupSynchronization::onGetJoinedRemoteChild - "
                    << "To register remoteCollectionResourceHandle"
                    << " was unsuccessful. result - " << result << endl;
            return;
        }
        cout << "GroupSynchronization::onGetJoinedRemoteChild : "
                "To register remoteCollectionResourceHandle was successful." << endl;

        // binding remote collection resource handle and resource handle to join
        collectionResourceHandleList[types[0]] = remoteCollectionResourceHandle;

        result = OCPlatform::bindResource(remoteCollectionResourceHandle, deviceResourceHandle);
        if (OC_STACK_OK == result)
        {
            cout << "GroupSynchronization::onGetJoinedRemoteChild : "
                    << "binding remoteCollectionResourceHandle and deviceResourceHandle"
                    << endl;
        }
        else
        {
            cout << "GroupSynchronization::onGetJoinedRemoteChild - "
                    << "To bind remoteCollectionResourceHandle and deviceResourceHandle "
                    << "was unsuccessful. result - " << result << endl;
        }

        std::vector< OCResourceHandle > childHandleList;
        childHandleList.push_back(deviceResourceHandle);
        deviceResourceHandleList.push_back(deviceResourceHandle);

        // binding copied remote collection resource handle and copied remote resource
        OCResourceHandle resourceHandle;
        for (unsigned int i = 0; i < childList.size(); ++i)
        {
            cout << "\tremote resource - " << i + 1 << endl;

            child = childList.at(i);
            resourceURI = child.getUri();
            types = child.getResourceTypes();
            interfaces = child.getResourceInterfaces();

            if (0 == types[0].compare(OCGetResourceTypeName(deviceResourceHandle, 0)))
            {
                cout << "GroupSynchronization::onGetJoinedRemoteChild : " << types[0]
                        << " is bind already." << endl;
                continue;
            }

            result = OCPlatform::registerResource(resourceHandle, resourceURI, types[0],
                    interfaces[0], NULL, OC_OBSERVABLE);
            if (OC_STACK_OK == result)
            {
                result = OCPlatform::bindResource(remoteCollectionResourceHandle,
                        resourceHandle);
                if (result != OC_STACK_OK)
                {
                    cout << "GroupSynchronization::onGetJoinedRemoteChild - "
                            << "binding remoteCollectionResourceHandle and resourceHandle "
                            << "was unsuccessful. result - " << result << endl;
                    OCPlatform::unregisterResource(resourceHandle);
                }

                childHandleList.push_back(resourceHandle);
                cout << "GroupSynchronization::onGetJoinedRemoteChild : "
                        << "binding remoteCollectionResourceHandle and resourceHandle" << endl;
            }
            else
            {
                cout << "GroupSynchronization::onGetJoinedRemoteChild - "
                        << "To register remoteCollectionResourceHandle was unsuccessful."
                        << " result - " << result << endl;
            }
        }

        childResourceHandleList[remoteCollectionResourceHandle] = childHandleList;
        // this handle list is used to leave group
    }
    else
    {
        cout << "GroupSynchronization::onGetJoinedRemoteChild : error - " << eCode << endl;
    }

    debugGroupSync();
}

void GroupSynchronization::onLeaveGroup(const HeaderOptions& headerOptions,
        const OCRepresentation& rep, const int eCode)
{
    if (eCode == OC_STACK_OK)
    {
        cout << "GroupSynchronization::onLeaveGroup" << endl;
    }
    else
    {
        cout << "GroupSynchronization::onLeaveGroup : error - " << eCode << endl;
    }
    debugGroupSync();
}

void GroupSynchronization::debugGroupSync(void)
{
    cout << "GroupSynchronization::debugGroupSync" << endl;

    unsigned int i;
    std::map< std::string, OCResourceHandle >::iterator handleIt;
    std::map< OCResourceHandle, std::vector< OCResourceHandle > >::iterator childIt;
    std::string type;
    OCResourceHandle resourceHandle;
    std::vector< OCResourceHandle > handleList;
    std::shared_ptr< OCResource > resource;

    cout << "Resource Handle Created by App" << endl;
    for (i = 0; i < deviceResourceHandleList.size(); i++)
    {
        resourceHandle = deviceResourceHandleList.at(i);

        cout << i + 1 << ". details" << endl;
        cout << "  uri - " << OCGetResourceUri(resourceHandle) << endl;
        cout << "  resource type - " << OCGetResourceTypeName(resourceHandle, 0) << endl;
        cout << "  resource interface - " << OCGetResourceInterfaceName(resourceHandle, 0)
                << endl << endl;
    }

    cout << "\nGroup Sync Resource Handle List. The number is "
            << groupSyncResourceHandleList.size() << endl;
    i = 1;
    for (handleIt = groupSyncResourceHandleList.begin();
            handleIt != groupSyncResourceHandleList.end(); ++handleIt)
    {
        type = handleIt->first;
        cout << "\t" << i << ". group sync resource type - " << type << endl;
        cout << "\t  details" << endl;

        resourceHandle = handleIt->second;
        cout << "\t  uri - " << OCGetResourceUri(resourceHandle) << endl;
        cout << "\t  resource type - " << OCGetResourceTypeName(resourceHandle, 0) << endl;
        cout << "\t  resource interface - " << OCGetResourceInterfaceName(resourceHandle, 0)
                << endl << endl;
        ;
        i++;
    }

    cout << "Copied Remote Group Sync Resource List. The number is "
            << groupSyncResourceList.size() << endl;
    std::vector< std::string > list;
    i = 1;
    for (auto resourceIt = groupSyncResourceList.begin();
            resourceIt != groupSyncResourceList.end(); ++resourceIt)
    {
        type = resourceIt->first;
        cout << "\t" << i << ". group sync resource type - " << type << endl;
        cout << "\t details" << endl;

        resource = resourceIt->second;
        cout << "\t  host - " << resource->host() << endl;
        cout << "\t  uri - " << resource->uri() << endl;
        list = resource->getResourceTypes();
        cout << "\t  resource type - " << list[0] << endl;
        list = resource->getResourceInterfaces();
        cout << "\t  resource interface - " << list[0] << endl << endl;
        i++;
    }

//    cout << "The number of collection Resource Handle is " << collectionResourceHandleList.size()
//            << endl;
//    cout << "The number of child resource handle list is " << childResourceHandleList.size()
//            << endl;

    cout << "Collection Resource Handle List" << endl;
    i = 1;
    for (handleIt = collectionResourceHandleList.begin();
            handleIt != collectionResourceHandleList.end(); ++handleIt)
    {
        type = handleIt->first;
        cout << "\t" << i << ". collection resource type - " << type << endl;
        cout << "\t  details" << endl;

        resourceHandle = handleIt->second;
        cout << "\t  uri - " << OCGetResourceUri(resourceHandle) << endl;
        cout << "\t  resource type - " << OCGetResourceTypeName(resourceHandle, 0) << endl;
        cout << "\t  resource interface - " << OCGetResourceInterfaceName(resourceHandle, 0)
                << endl << endl;

        childIt = childResourceHandleList.find(resourceHandle);
        if (childIt != childResourceHandleList.end())
        {
            handleList = childIt->second;
            for (unsigned int j = 0; j < handleList.size(); j++)
            {

                cout << "\t\t" << j + 1 << ". child resource details" << endl;

                resourceHandle = handleList.at(j);
                cout << "\t\t  uri - " << OCGetResourceUri(resourceHandle) << endl;
                cout << "\t\t  resource type - " << OCGetResourceTypeName(resourceHandle, 0)
                        << endl;
                cout << "\t\t  resource interface - "
                        << OCGetResourceInterfaceName(resourceHandle, 0) << endl << endl;
            }
        }

        i++;
    }
}
}
