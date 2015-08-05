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

#include "GroupSynchronization.h"
#include "logger.h"

using namespace OC;
using namespace std;

#define TAG "GROUP_SYNC"

namespace OIC
{
    GroupSynchronization* GroupSynchronization::groupSyncnstance = NULL;
    bool GroupSynchronization::bIsFinding = false;

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
        OC_LOG(DEBUG, TAG, "GroupSynchronization::findGroup");

        if(bIsFinding)
        {
            OC_LOG(DEBUG, TAG, "It was searching already.");
            return OC_STACK_ERROR;
        }

        std::lock_guard < std::mutex > guard(foundGroupMutex);
        foundGroupResourceList.clear();
        findCallback = callback;

        if (findCallback == NULL)
        {
            OC_LOG(DEBUG, TAG, "Find callback is NULL.");
            return OC_STACK_ERROR;
        }

        for (unsigned int i = 0; i < collectionResourceTypes.size(); ++i)
        {

            std::string query = OC_RSRVD_WELL_KNOWN_URI;
            query.append("?rt=");
            query.append(collectionResourceTypes.at(i));

            OCPlatform::findResource("", query,
                    CT_DEFAULT,
                    std::bind(&GroupSynchronization::onFindGroup, this,
                        std::placeholders::_1));
        }

        bIsFinding = true;

        // thread to check if GroupSynchronization::onFoundGroup is called or not.
        std::thread t(std::bind(&GroupSynchronization::checkFindGroup, this));
        t.detach();

        return OC_STACK_OK;
    }

    OCStackResult GroupSynchronization::createGroup(std::string collectionResourceType)
    {
        std::lock_guard < std::mutex > guard(foundGroupMutex);
        foundGroupResourceList.clear();

        OCResourceHandle collectionResHandle = NULL;
        OCResourceHandle groupSyncResHandle = NULL;

        if (0 == collectionResourceType.length())
        {
            OC_LOG(DEBUG, TAG,
                "GroupSynchronization::createGroup : Error! Input params are wrong.");
            return OC_STACK_INVALID_PARAM;
        }

        OC_LOG(DEBUG, TAG, "GroupSynchronization::createGroup - The created group is added.");

        OCStackResult result;

        // creating master collection resource
        std::string collectionUri = "/" + collectionResourceType;
        std::size_t i;
        while ((i = collectionUri.find(".")) != std::string::npos)
        {
            collectionUri.replace(i, 1, "/");
        }

        OC_LOG_V(DEBUG, TAG, "GroupSynchronization::createGroup : collection uri - %s, type - %s",
            collectionUri.c_str(), collectionResourceType.c_str());

        std::string resourceInterface = DEFAULT_INTERFACE;

        result = OCPlatform::registerResource(collectionResHandle, collectionUri,
                collectionResourceType, resourceInterface, NULL, OC_DISCOVERABLE | OC_OBSERVABLE);
        if (result != OC_STACK_OK)
        {
            OC_LOG_V(DEBUG, TAG, "To register resource (%s) was unsuccessful. result - %d",
                collectionUri.c_str(), result);
            goto Error;
        }

        {
            OCPlatform::bindInterfaceToResource(collectionResHandle, GROUP_INTERFACE);
            if (result != OC_STACK_OK)
            {
                OC_LOG_V(DEBUG, TAG,
                    "To bind Interface (collection) was unsuccessful. result - %d",
                    result);
            }

            collectionResourceHandleList[collectionResourceType] = collectionResHandle;

            // creating master group sync resource
            std::string groupSyncUri = collectionUri + "/groupsync";
            std::string groupSyncResType = collectionResourceType + ".groupsync";

            result = OCPlatform::registerResource(groupSyncResHandle, groupSyncUri,
                    groupSyncResType, resourceInterface,
                    std::bind(&GroupSynchronization::groupEntityHandler, this,
                            std::placeholders::_1), OC_DISCOVERABLE | OC_OBSERVABLE);
            if (result != OC_STACK_OK)
            {
                OC_LOG_V(DEBUG, TAG,
                    "To register resource (groupsync) was unsuccessful. result - %d", result);
                goto Error;
            }

            groupSyncResourceHandleList[collectionResourceType] = groupSyncResHandle;

            return OC_STACK_OK;
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
        std::lock_guard < std::mutex > guard(foundGroupMutex);
        if ((0 == collectionResourceType.length()) || (!resourceHandle))
        {
            OC_LOG(DEBUG, TAG,
                "GroupSynchronization::joinGroup : Error! input params are wrong.");
            return OC_STACK_INVALID_PARAM;
        }

        auto resIt = collectionResourceHandleList.find(collectionResourceType);
        if (resIt == collectionResourceHandleList.end())
        {
            OC_LOG(DEBUG, TAG,
                "GroupSynchronization::joinGroup : error! There is no collection to join");
            return OC_STACK_INVALID_PARAM;
        }

        OCResourceHandle collectionResHandle = resIt->second;
        try{
            OCStackResult result = OCPlatform::bindResource(collectionResHandle, resourceHandle);
            if (result != OC_STACK_OK)
            {
                OC_LOG_V(DEBUG, TAG,
                    "GroupSynchronization::joinGroup : To bind resource was unsuccessful." \
                    " result - %d", result);
                return OC_STACK_ERROR;
            }
        } catch(OCException &e) {
            return OC_STACK_INVALID_PARAM;
        }

        OC_LOG(DEBUG, TAG,
            "GroupSynchronization::joinGroup : To bind collectionResHandle and resourceHandle");

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

        return OC_STACK_OK;
    }

    OCStackResult GroupSynchronization::joinGroup(const std::shared_ptr< OCResource > resource,
            OCResourceHandle resourceHandle)
    {
        std::lock_guard < std::mutex > guard(foundGroupMutex);
        if ((!resource) || (!resourceHandle))
        {
            OC_LOG(DEBUG, TAG, "GroupSynchronization::joinGroup : Error! Input params are wrong.");
            return OC_STACK_INVALID_PARAM;
        }

        OC_LOG(DEBUG, TAG, "GroupSynchronization::joinGroup");

        // making representation to join group
        std::string method = "joinGroup";
        std::vector< std::string > type = resource->getResourceTypes();
        std::string resourceType;
        resourceType.append(OCGetResourceTypeName(resourceHandle, 0));

        OCRepresentation rep;
        rep.setValue("method", method);
        rep.setValue("collectionResourceType", type[0]);
        rep.setValue("resourceType", resourceType);

        OC_LOG_V(DEBUG, TAG, "\tmethod - %s", method.c_str());
        OC_LOG_V(DEBUG, TAG, "\tcollectionResourceType - %s", type[0].c_str());
        OC_LOG_V(DEBUG, TAG, "\tresourceType - %s", resourceType.c_str());

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

        OCResource::Ptr groupSyncResource =
                        OCPlatform::constructResourceObject(host, uri,

                            CT_DEFAULT, false,
                            resourceTypes, resourceInterface);

        // OCResource::Ptr groupSyncResource = OCPlatform::constructResourceObject(host, uri,
        //         OC_WIFI, false, resourceTypes, resourceInterface);

        groupSyncResourceList[type[0]] = groupSyncResource;

        OC_LOG(DEBUG, TAG, "GroupSynchronization::joinGroup : creating groupSyncResource.");

        // Create QueryParameters Map and add query params (if any)
        QueryParamsMap queryParamsMap;

        // request to join group to the remote group sync resource
        OCStackResult result = groupSyncResource->put(rep, queryParamsMap,
                std::bind(&GroupSynchronization::onJoinGroup, this, std::placeholders::_1,
                        std::placeholders::_2, std::placeholders::_3));
        if (OC_STACK_OK == result)
        {
            OC_LOG(DEBUG, TAG,
                "GroupSynchronization::joinGroup : groupSyncResource->put was successful.");
        }
        else
        {
            OC_LOG_V(DEBUG, TAG,
                "GroupSynchronization::joinGroup : groupSyncResource->put was unsuccessful." \
                "result - %d", result);
        }

        // saving the remote collection resource.
        // It is used in onJoinGroup() and onGetJoinedRemoteChild().
        remoteCollectionResource = resource;

        // saving the resource handle to join. It is used in onGetJoinedRemoteChild()
        deviceResourceHandle = resourceHandle;

        return OC_STACK_OK;
    }

    OCStackResult GroupSynchronization::leaveGroup(std::string collectionResourceType,
            OCResourceHandle resourceHandle)
    {
        if ((0 == collectionResourceType.length()) || (!resourceHandle))
        {
            OC_LOG(DEBUG, TAG,
                "GroupSynchronization::leaveGroup : Error! Input params are wrong.");
            return OC_STACK_INVALID_PARAM;
        }

        OC_LOG_V(DEBUG, TAG,
            "GroupSynchronization::leaveGroup : collectionResourceType - %s",
            collectionResourceType.c_str());

        OCResourceHandle collectionResHandle;
        auto handleIt = groupSyncResourceHandleList.find(collectionResourceType);

        // if groupSyncResourceHandleList has resourceType,
        // this app created collection resource handle.
        if (handleIt != groupSyncResourceHandleList.end())
        {
            handleIt = collectionResourceHandleList.find(collectionResourceType);
            if (handleIt == collectionResourceHandleList.end())
            {
                OC_LOG(DEBUG, TAG, "GroupSynchronization::leaveGroup : Error!" \
                    "There is no collection resource handle to leave.");
                return OC_STACK_INVALID_PARAM;
            }

            collectionResHandle = handleIt->second;
            if(collectionResHandle == NULL)
                return OC_STACK_INVALID_PARAM;

            OCStackResult result;
            try
            {
                result = OCPlatform::unbindResource(collectionResHandle, resourceHandle);
                if (OC_STACK_OK == result)
                {
                    OC_LOG(DEBUG, TAG,
                        "GroupSynchronization::leaveGroup : To unbind resource was successful.");
                }
                else
                {
                    OC_LOG_V(DEBUG, TAG, "GroupSynchronization::leaveGroup : " \
                            "To unbind resource was unsuccessful. result - %d", result);
                    return result;
                }
            } catch(OCException &e) {
                OC_LOG_V(DEBUG, TAG, "ERROR : %s", e.reason().c_str());
                return OC_STACK_NO_RESOURCE;
            }

            auto It = std::find(deviceResourceHandleList.begin(), deviceResourceHandleList.end(),
                    resourceHandle);
            if (It == deviceResourceHandleList.end()) // there is no resource handle to find
            {
                try
                {
                    result = OCPlatform::unregisterResource(resourceHandle);
                    if (OC_STACK_OK == result)
                    {
                        OC_LOG(DEBUG, TAG, "GroupSynchronization::leaveGroup : " \
                                "To unregister resource was successful.");
                    }
                    else
                    {
                        OC_LOG_V(DEBUG, TAG, "GroupSynchronization::leaveGroup : " \
                            "To unregister resource was unsuccessful. result - %d", result);
                        return result;
                    }
                } catch(OCException &e)
                {
                    OC_LOG_V(DEBUG, TAG, "ERROR : %s", e.reason().c_str());
                    return OC_STACK_NO_RESOURCE;
                }
            }
            else
            {
                OC_LOG(DEBUG, TAG, "GroupSynchronization::leaveGroup : " \
                    "This resource cannot be unregistered.");
                deviceResourceHandleList.erase(It);
            }

            auto handleListIt = childResourceHandleList.find(collectionResHandle);
            if (handleListIt == childResourceHandleList.end())
            {
                OC_LOG(DEBUG, TAG, "GroupSynchronization::leaveGroup : " \
                    "Error! There is no child resource list to delete.");
                return OC_STACK_INVALID_PARAM;
            }

            std::vector< OCResourceHandle > childList = handleListIt->second;
            auto childIt = std::find(childList.begin(), childList.end(), resourceHandle);
            if (childIt != childList.end())
            {
                OC_LOG(DEBUG, TAG, "GroupSynchronization::groupEntityHandler : " \
                    "Found! The resource to leave is found.");
                childList.erase(childIt);
            }

            childResourceHandleList[collectionResHandle] = childList;

            debugGroupSync();
        }

        return OC_STACK_OK;
    }

OCStackResult GroupSynchronization::leaveGroup(
        const std::shared_ptr<OCResource> resource,
        std::string collectionResourceType, OCResourceHandle resourceHandle)
    {
        if ((!resource) || (!resourceHandle))
        {
            OC_LOG(DEBUG, TAG, "GroupSynchronization::joinGroup : Error! Input params are wrong.");
            return OC_STACK_INVALID_PARAM;
        }

        OC_LOG(DEBUG, TAG, "GroupSynchronization::joinGroup");

        // making representation to join group
        std::vector< std::string > type = resource->getResourceTypes();
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

        OCResource::Ptr groupSyncResource;
        groupSyncResource = OCPlatform::constructResourceObject(host, uri,
                CT_DEFAULT, false,
                resourceTypes, resourceInterface);
        // groupSyncResource = OCPlatform::constructResourceObject(host, uri,
        //         OC_WIFI, false, resourceTypes, resourceInterface);

        // making representation to leave group
        std::string method = "leaveGroup";
//        std::string type = OCGetResourceTypeName(collectionResourceType, 0);
        std::string resourceType;
        resourceType.append(OCGetResourceTypeName(resourceHandle, 0));

        OCRepresentation rep;
        rep.setValue("method", method);
        rep.setValue("collectionResourceType", collectionResourceType);
        rep.setValue("resourceType", resourceType);

        OC_LOG_V(DEBUG, TAG, "\tmethod - %s", method.c_str());
        OC_LOG_V(DEBUG, TAG, "\tcollectionResourceType - %s", collectionResourceType.c_str());
        OC_LOG_V(DEBUG, TAG, "\tresourceType - %s", resourceType.c_str());

        QueryParamsMap queryParamsMap;

        // request to leave group to the remote group sync resource
        OCStackResult result = groupSyncResource->put(rep, queryParamsMap,
                std::bind(&GroupSynchronization::onLeaveGroup, this, std::placeholders::_1,
                        std::placeholders::_2, std::placeholders::_3));
        if (OC_STACK_OK == result)
        {
            OC_LOG(DEBUG, TAG, "GroupSynchronization::leaveGroup : " \
                "groupSyncResource->put was successful.");
        }
        else
        {
            OC_LOG_V(DEBUG, TAG, "GroupSynchronization::leaveGroup : " \
                "groupSyncResource->put was unsuccessful. result - %d", result);
        }
        return result;
    }

    void GroupSynchronization::deleteGroup(std::string collectionResourceType)
    {
        if (0 == collectionResourceType.length())
        {
            OC_LOG(DEBUG, TAG,
                "GroupSynchronization::deleteGroup : Error! Input params are wrong.");
            return;
        }

        OC_LOG(DEBUG, TAG, "GroupSynchronization::deleteGroup");

        OCStackResult result;
        OCResourceHandle resourceHandle;

        auto handleIt = groupSyncResourceHandleList.find(collectionResourceType);

        // if groupSyncResourceHandleList has resourceType,
        // group sync of this app created collection resource.
        if (handleIt != groupSyncResourceHandleList.end())
        {
            resourceHandle = handleIt->second; // group sync resource handle
            result = OCPlatform::unregisterResource(resourceHandle);
            if (OC_STACK_OK == result)
            {
                OC_LOG(DEBUG, TAG, "GroupSynchronization::deleteGroup : " \
                    "To unregister group sync resource handle was successful.");
            }
            else
            {
                OC_LOG_V(DEBUG, TAG, "GroupSynchronization::deleteGroup : " \
                        "To unregister group sync resource handle was unsuccessful. " \
                        "result - %d", result);
            }

            groupSyncResourceHandleList.erase(handleIt);
        }

        auto resourceIt = groupSyncResourceList.find(collectionResourceType);
        if (resourceIt != groupSyncResourceList.end())
        {
            groupSyncResourceList.erase(resourceIt);
        }

        handleIt = collectionResourceHandleList.find(collectionResourceType);
        if (handleIt == collectionResourceHandleList.end())
        {
            OC_LOG(DEBUG, TAG, "GroupSynchronization::deleteGroup : " \
                "Error! There is no collection resource handle to delete.");
            return;
        }
        OCResourceHandle collectionResHandle = handleIt->second;

        collectionResourceHandleList.erase(handleIt);

        auto handleListIt = childResourceHandleList.find(collectionResHandle);
        if (handleListIt == childResourceHandleList.end())
        {
            OC_LOG(DEBUG, TAG, "GroupSynchronization::deleteGroup : " \
                "There is no child resource list to delete.");

            result = OCPlatform::unregisterResource(collectionResHandle);
            if (result == OC_STACK_OK)
            {
                OC_LOG(DEBUG, TAG, "GroupSynchronization::deleteGroup : " \
                     "To unregister collection resource handle was successful.");
            }
            else
            {
                OC_LOG_V(DEBUG, TAG, "GroupSynchronization::deleteGroup : " \
                    " To unregister collection resource handle was unsuccessful. result - %d",
                    result);
            }

            debugGroupSync();
            return;
        }

        std::vector< OCResourceHandle > childList = handleListIt->second;

        childResourceHandleList.erase(handleListIt);

        result = OCPlatform::unbindResources(collectionResHandle, childList);
        if (OC_STACK_OK == result)
        {
            OC_LOG(DEBUG, TAG, "GroupSynchronization::deleteGroup : " \
                "To unbind resources was successful.");
        }
        else
        {
            OC_LOG_V(DEBUG, TAG, "GroupSynchronization::deleteGroup : " \
                "To unbind resources was unsuccessful. result - %d", result);
        }

        result = OCPlatform::unregisterResource(collectionResHandle);
        if (result == OC_STACK_OK)
        {
            OC_LOG(DEBUG, TAG, "GroupSynchronization::deleteGroup : " \
                "To unregister collection resource handle was successful.");
        }
        else
        {
            OC_LOG_V(DEBUG, TAG, "GroupSynchronization::deleteGroup : " \
                " To unregister collection resource handle was unsuccessful. result - %d",
                result);
        }

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
                    OC_LOG_V(DEBUG, TAG,
                        "GroupSynchronization::deleteGroup : UnregisterResource(%d)" \
                        " was successful.", i + 1);
                }
                else
                {
                    OC_LOG_V(DEBUG, TAG,
                        "GroupSynchronization::deleteGroup : UnregisterResource(%d)" \
                        " was unsuccessful. result - ", i + 1, result);
                }
            }
        }

        debugGroupSync();
    }

    std::map< std::string, OCResourceHandle > GroupSynchronization::getGroupList()
    {
        return collectionResourceHandleList;
    }

    OCEntityHandlerResult GroupSynchronization::groupEntityHandler(
            const std::shared_ptr< OCResourceRequest > request)
    {
        OC_LOG(DEBUG, TAG, "GroupSynchronization::groupEntityHandler");

        if (request)
        {
            // Get the request type and request flag
            std::string requestType = request->getRequestType();
            int requestFlag = request->getRequestHandlerFlag();

            if (requestFlag == RequestHandlerFlag::RequestFlag)
            {
                OC_LOG(DEBUG, TAG, "\trequestFlag : Request");

                // If the request type is GET
                if (requestType == "GET")
                {
                    OC_LOG(DEBUG, TAG, "\t\trequestType : GET");
                }
                else if (requestType == "PUT")
                {
                    OC_LOG(DEBUG, TAG, "\t\trequestType : PUT");

                    //get method name, group resource type and resource type to join group
                    OCRepresentation rp = request->getResourceRepresentation();
                    std::string methodType = rp.getValue< std::string >("method");
                    std::string collectionResourceType = rp.getValue< std::string >(
                            "collectionResourceType");
                    std::string resourceType = rp.getValue< std::string >("resourceType");

                    OC_LOG_V(DEBUG, TAG, "\t\t\tmethod : %s", methodType.c_str());
                    OC_LOG_V(DEBUG, TAG, "\t\t\tcollection resourceType : %s",
                        collectionResourceType.c_str());
                    OC_LOG_V(DEBUG, TAG, "\t\t\tresourceType : %s", resourceType.c_str());

                    auto handleIt = collectionResourceHandleList.find(collectionResourceType);
                    if (handleIt == collectionResourceHandleList.end())
                    {
                        OC_LOG(DEBUG, TAG, "GroupSynchronization::groupEntityHandler : " \
                            "Error! There is no collection resource handle to delete.");
                        return OC_EH_ERROR;
                    }
                    collectionResourceHandle = handleIt->second;
                    // in case of join group it is used in onFindResource()

                    if (methodType == "joinGroup")
                    {
                        std::string resourceName = OC_RSRVD_WELL_KNOWN_URI;
                        resourceName.append("?rt=");
                        resourceName.append(resourceType);
                        OC_LOG_V(DEBUG, TAG, "\t\t\tresourceName : %s", resourceName.c_str());

                        resourceRequest = request;

                        OCPlatform::findResource("", resourceName,
                            CT_DEFAULT,
                            std::bind(&GroupSynchronization::onFindResource, this,
                                std::placeholders::_1));

                        return OC_EH_SLOW;
                    }
                    else if (methodType == "leaveGroup")
                    {
                        auto it = childResourceHandleList.find(collectionResourceHandle);
                        if (it == childResourceHandleList.end())
                        {
                            OC_LOG(DEBUG, TAG, "GroupSynchronization::groupEntityHandler : " \
                                "Error! There is no child resource list.");
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
                                OC_LOG_V(DEBUG, TAG,
                                    "GroupSynchronization::groupEntityHandler : " \
                                    "Found! The resource to leave is found. - %s", type);

                                childIt = childList.erase(childIt++);

                                OCStackResult result = OCPlatform::unbindResource(
                                        collectionResourceHandle, resourceHandle);
                                if (OC_STACK_OK == result)
                                {
                                    OC_LOG(DEBUG, TAG,
                                        "GroupSynchronization::groupEntityHandler : " \
                                        "To unbind resource was successful.");
                                }
                                else
                                {
                                    OC_LOG_V(DEBUG, TAG,
                                        "GroupSynchronization::groupEntityHandler : " \
                                        "To unbind resource was unsuccessful. result - %d",
                                        result);
                                }

                                result = OCPlatform::unregisterResource(resourceHandle);
                                if (OC_STACK_OK == result)
                                {
                                    OC_LOG(DEBUG, TAG,
                                        "GroupSynchronization::groupEntityHandler : " \
                                        "To unregister resource was successful.");
                                }
                                else
                                {
                                    OC_LOG_V(DEBUG, TAG,
                                        "GroupSynchronization::groupEntityHandler : "
                                        "To unregister resource was unsuccessful. result - %d",
                                        result);
                                }
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
                            OC_LOG(DEBUG, TAG, "GroupSynchronization::groupEntityHandler : " \
                                "sendResponse is successful.");
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
                OC_LOG(DEBUG, TAG, "\trequestFlag : Observer");
            }
        }
        else
        {
            OC_LOG(DEBUG, TAG, "Request invalid");
        }

        return OC_EH_OK;
    }

    void GroupSynchronization::onFindGroup(std::shared_ptr< OCResource > resource)
    {
        OC_LOG(DEBUG, TAG, "GroupSynchronization::onFindGroup");

        try
        {
            if (resource)
            {
                // Debugging
                std::string resourceURI;
                std::string hostAddress;

                // Get the resource URI
                resourceURI = resource->uri();
                OC_LOG_V(DEBUG, TAG, "\tURI of the resource: %s", resourceURI.c_str());

                // Get the resource host address
                hostAddress = resource->host();
                OC_LOG_V(DEBUG, TAG, "\tHost address of the resource: %s", hostAddress.c_str());

                hostAddress.append(resourceURI);

#ifndef NDEBUG
                // Get the resource types
                OC_LOG(DEBUG, TAG, "\tList of resource types: ");
                for (auto &resourceTypes : resource->getResourceTypes())
                {
                    OC_LOG_V(DEBUG, TAG, "\t\t", resourceTypes.c_str());
                }

                // Get the resource interfaces
                OC_LOG(DEBUG, TAG, "\tList of resource interfaces: ");
                for (auto &resourceInterfaces : resource->getResourceInterfaces())
                {
                    OC_LOG_V(DEBUG, TAG, "\t\t", resourceInterfaces.c_str());
                }
#endif

                if (false == IsSameGroup(resource))
                {
                    saveGroup(resource);
                    findCallback(resource);
                }
            }
            else
            {
                // Resource is invalid
                OC_LOG(DEBUG, TAG, "Resource is invalid");
                findCallback(NULL);
            }

            bIsFinding = false;
        }
        catch (std::exception& e)
        {
            //log(e.what());
        }
    }

    void GroupSynchronization::checkFindGroup(void)
    {
        OC_LOG(DEBUG, TAG, "GroupSynchronization::checkFindGroup");

        for (int i = 0; i < 15; i++)
        {
            std::chrono::milliseconds workTime(300);
            std::this_thread::sleep_for(workTime);

            std::lock_guard < std::mutex > guard(foundGroupMutex);

            if (false == foundGroupResourceList.empty())
            {
                OC_LOG(DEBUG, TAG, "GroupSynchronization::checkFoundGroup : " \
                    "Some group is received.");
                return;
            }
        }

        OC_LOG(DEBUG, TAG, "GroupSynchronization::checkFoundGroup : " \
            "It is failed to find resource within 3s.");

        onFindGroup(NULL);
        return;
    }

    bool GroupSynchronization::IsSameGroup(std::shared_ptr< OCResource > resource)
    {
        std::lock_guard < std::mutex > guard(foundGroupMutex);

        if (true == foundGroupResourceList.empty())
        {
            OC_LOG(DEBUG, TAG, "GroupSynchronization::IsSameGroup : There is no found group.");
            return false;
        }

        std::string foundHostAddress, savedHostAddress;
        std::string foundHostUri, savedHostUri;
        foundHostAddress = resource->host();
        foundHostUri = resource->uri();

        for (unsigned int i = 0; i < foundGroupResourceList.size(); ++i)
        {
            savedHostAddress = (foundGroupResourceList.at(i))->host();
            savedHostUri = (foundGroupResourceList.at(i))->uri();

            if (0 == foundHostAddress.compare(savedHostAddress.c_str()))
            {
                if( 0 == foundHostUri.compare(savedHostAddress) )
                {
                    OC_LOG(DEBUG, TAG,
                        "GroupSynchronization::IsSameGroup : Found! The same group is found.");
                    return true;
                }
            }
        }

        OC_LOG(DEBUG, TAG, "GroupSynchronization::IsSameGroup :  There is no same group.");
        return false;
    }

    void GroupSynchronization::saveGroup(std::shared_ptr< OCResource > resource)
    {
        OC_LOG(DEBUG, TAG, "GroupSynchronization::saveGroup");

        std::lock_guard < std::mutex > guard(foundGroupMutex);

        foundGroupResourceList.push_back(resource);
    }

    void GroupSynchronization::onJoinGroup(const HeaderOptions& headerOptions,
            const OCRepresentation& rep, const int eCode)
    {
        if (eCode != OC_STACK_OK)
        {
            OC_LOG_V(DEBUG, TAG, "GroupSynchronization::onJoinGroup : error - %d", eCode);
            return;
        }
    }

    void GroupSynchronization::onFindResource(std::shared_ptr< OCResource > resource)
    {
        OC_LOG(DEBUG, TAG, "GroupSynchronization::onFindResource");

        if (resource)
        {
            if(NULL == collectionResourceHandle)
                return;

            // start of debugging
            std::string resourceURI;
            std::string hostAddress;

            // Get the resource URI
            resourceURI = resource->uri();
            OC_LOG_V(DEBUG, TAG, "\tURI of the resource: %s", resourceURI.c_str());

            // Get the resource host address
            hostAddress = resource->host();
            OC_LOG_V(DEBUG, TAG, "\tHost address of the resource: %s", hostAddress.c_str());

            hostAddress.append(resourceURI);

#ifndef NDEBUG
            // Get the resource types
            OC_LOG(DEBUG, TAG, "\tList of resource types: ");
            for (auto &resourceTypes : resource->getResourceTypes())
            {
                OC_LOG_V(DEBUG, TAG, "\t\t%s", resourceTypes.c_str());
            }

            // Get the resource interfaces
            OC_LOG(DEBUG, TAG, "\tList of resource interfaces: ");
            for (auto &resourceInterfaces : resource->getResourceInterfaces())
            {
                OC_LOG_V(DEBUG, TAG, "\t\t%s", resourceInterfaces.c_str());
            }
            // end of debugging
#endif

            OCResourceHandle resourceHandle;
            OCStackResult result = OCPlatform::registerResource(resourceHandle, resource);
            if (result != OC_STACK_OK)
            {
                OC_LOG_V(DEBUG, TAG, "GroupSynchronization::" \
                    "onFindResource - Resource to join creation was unsuccessful. result - %d",
                    result);
                return;
            }

            result = OCPlatform::bindResource(collectionResourceHandle, resourceHandle);
            if (result != OC_STACK_OK)
            {
                OC_LOG_V(DEBUG, TAG, "GroupSynchronization::onFindResource : " \
                    "To bind resource was unsuccessful. result - %d", result);
                return;
            }

            OC_LOG(DEBUG, TAG, "GroupSynchronization::onFindResource : " \
                    "To bind joinGroupHandle and resourceHandle was successful.");

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
            pResponse->setResourceRepresentation(rep);
            try{
                if (OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    OC_LOG(DEBUG, TAG,
                        "GroupSynchronization::onFindResource : sendResponse is successful.");
                }
            }
            catch( OCException &e )
            {
                // OC_LOG(DEBUG, TAG, e.what);
                return;
            }
        }
        else
        {
            OC_LOG(DEBUG, TAG, "GroupSynchronization::onFindResource : " \
                "Resource is invalid. So a new Group Resource has to be created.");
        }

        debugGroupSync();
    }

    void GroupSynchronization::onGetJoinedRemoteChild(const HeaderOptions& headerOptions,
            const OCRepresentation& rep, const int eCode)
    {
        if (eCode != OC_STACK_OK)
        {
            OC_LOG_V(DEBUG, TAG,
                "GroupSynchronization::onGetJoinedRemoteChild : error - %d", eCode);
            return;
        }

        std::string resourceURI;
        std::vector< OCRepresentation > childList;
        OCRepresentation child;

        OC_LOG(DEBUG, TAG, "GroupSynchronization::onGetJoinedRemoteChild");
#ifndef NDEBUG
        // debugging
        // Get the resource URI
        resourceURI = rep.getUri();
        OC_LOG_V(DEBUG, TAG, "\tURI of the resource: %s", resourceURI.c_str());

        // Get the resource types
        OC_LOG(DEBUG, TAG, "\tList of resource types: ");

        for (auto &resourceTypes : rep.getResourceTypes())
        {
            OC_LOG_V(DEBUG, TAG, "\t\t%s", resourceTypes.c_str());
        }

        // Get the resource interfaces
        OC_LOG(DEBUG, TAG, "\tList of resource interfaces: ");
        for (auto &resourceInterfaces : rep.getResourceInterfaces())
        {
            OC_LOG_V(DEBUG, TAG, "\t\t%s", resourceInterfaces.c_str());
        }

        childList = rep.getChildren();
        for (unsigned int i = 0; i < childList.size(); ++i)
        {
            OC_LOG_V(DEBUG, TAG, "\tchild resource - %d", i + 1);

            child = childList.at(i);
            resourceURI = child.getUri();
            OC_LOG_V(DEBUG, TAG, "\t\tURI of the resource: %s", resourceURI.c_str());

            OC_LOG_V(DEBUG, TAG, "\t\tList of resource types: ");
            for (auto &types : child.getResourceTypes())
            {
                OC_LOG_V(DEBUG, TAG, "\t\t\t%s", types.c_str());
            }

            OC_LOG(DEBUG, TAG, "\tList of resource interfaces: ");
            for (auto &interfaces : child.getResourceInterfaces())
            {
                OC_LOG_V(DEBUG, TAG, "\t\t\t%s", interfaces.c_str());
            }
        }
#endif

        // creating remote collection resource handle
        OCResourceHandle remoteCollectionResourceHandle;
        resourceURI = remoteCollectionResource->uri();
        std::vector< std::string > types = remoteCollectionResource->getResourceTypes();
        std::vector< std::string > interfaces = remoteCollectionResource->getResourceInterfaces();

        OCStackResult result = OCPlatform::registerResource(remoteCollectionResourceHandle,
                resourceURI, types[0], interfaces[0], NULL, OC_OBSERVABLE);
        if (result != OC_STACK_OK)
        {
            OC_LOG_V(DEBUG, TAG, "GroupSynchronization::onGetJoinedRemoteChild - " \
                "To register remoteCollectionResourceHandle" \
                " was unsuccessful. result - %d", result);
            return;
        }
        OC_LOG(DEBUG, TAG, "GroupSynchronization::onGetJoinedRemoteChild : " \
                "To register remoteCollectionResourceHandle was successful.");

        // binding remote collection resource handle and resource handle to join
        collectionResourceHandleList[types[0]] = remoteCollectionResourceHandle;

        result = OCPlatform::bindResource(remoteCollectionResourceHandle, deviceResourceHandle);
        if (OC_STACK_OK == result)
        {
            OC_LOG(DEBUG, TAG, "GroupSynchronization::onGetJoinedRemoteChild : " \
                "binding remoteCollectionResourceHandle and deviceResourceHandle");
        }
        else
        {
            OC_LOG_V(DEBUG, TAG, "GroupSynchronization::onGetJoinedRemoteChild - " \
                "To bind remoteCollectionResourceHandle and deviceResourceHandle " \
                "was unsuccessful. result - %d", result);
        }

        std::vector< OCResourceHandle > childHandleList;
        childHandleList.push_back(deviceResourceHandle);
        deviceResourceHandleList.push_back(deviceResourceHandle);

        // binding copied remote collection resource handle and copied remote resource
        OCResourceHandle resourceHandle;
        for (unsigned int i = 0; i < childList.size(); ++i)
        {
            OC_LOG_V(DEBUG, TAG, "\tremote resource - %d", i + 1);

            child = childList.at(i);
            resourceURI = child.getUri();
            types = child.getResourceTypes();
            interfaces = child.getResourceInterfaces();

            if (0 == types[0].compare(OCGetResourceTypeName(deviceResourceHandle, 0)))
            {
                OC_LOG_V(DEBUG, TAG, "GroupSynchronization::onGetJoinedRemoteChild : %s" \
                    " is bind already.", types[0].c_str());
                continue;
            }

            result = OCPlatform::registerResource(resourceHandle, resourceURI, types[0],
                    interfaces[0], NULL, OC_OBSERVABLE);
            if (OC_STACK_OK == result)
            {
                result = OCPlatform::bindResource(remoteCollectionResourceHandle, resourceHandle);
                if (result != OC_STACK_OK)
                {
                    OC_LOG_V(DEBUG, TAG, "GroupSynchronization::onGetJoinedRemoteChild - " \
                        "binding remoteCollectionResourceHandle and resourceHandle " \
                        "was unsuccessful. result - %d", result);
                    OCPlatform::unregisterResource(resourceHandle);
                }

                childHandleList.push_back(resourceHandle);
                OC_LOG(DEBUG, TAG, "GroupSynchronization::onGetJoinedRemoteChild : " \
                    "binding remoteCollectionResourceHandle and resourceHandle");
            }
            else
            {
                OC_LOG_V(DEBUG, TAG, "GroupSynchronization::onGetJoinedRemoteChild - " \
                    "To register remoteCollectionResourceHandle was unsuccessful." \
                    " result - %d", result);
            }
        }

        childResourceHandleList[remoteCollectionResourceHandle] = childHandleList;
        // this handle list is used to leave group

        debugGroupSync();
    }

    void GroupSynchronization::onLeaveGroup(const HeaderOptions& headerOptions,
            const OCRepresentation& rep, const int eCode)
    {
        if (eCode != OC_STACK_OK)
        {
            OC_LOG_V(DEBUG, TAG, "GroupSynchronization::onLeaveGroup : error - %d", eCode);
            return;
        }

        OC_LOG(DEBUG, TAG, "GroupSynchronization::onLeaveGroup");
        debugGroupSync();
    }

    void GroupSynchronization::debugGroupSync(void)
    {
#ifndef NDEBUG
        OC_LOG(DEBUG, TAG, "GroupSynchronization::debugGroupSync");

        unsigned int i;
        std::map< std::string, OCResourceHandle >::iterator handleIt;
        std::map< OCResourceHandle, std::vector< OCResourceHandle > >::iterator childIt;
        std::string type;
        OCResourceHandle resourceHandle;
        std::vector< OCResourceHandle > handleList;
        std::shared_ptr< OCResource > resource;

        OC_LOG(DEBUG, TAG, "Resource Handle Created by App");
        for (i = 0; i < deviceResourceHandleList.size(); i++)
        {
            resourceHandle = deviceResourceHandleList.at(i);

            OC_LOG_V(DEBUG, TAG, "%d. details", i + 1);
            OC_LOG_V(DEBUG, TAG, "\turi - %s", OCGetResourceUri(resourceHandle));
            OC_LOG_V(DEBUG, TAG, "\tresource type - %s",
                OCGetResourceTypeName(resourceHandle, 0));
            OC_LOG_V(DEBUG, TAG, "\tresource interface - %s",
                OCGetResourceInterfaceName(resourceHandle, 0));
        }

        OC_LOG_V(DEBUG, TAG, "Group Sync Resource Handle List. The number is %d",
            groupSyncResourceHandleList.size());

        i = 1;
        for (handleIt = groupSyncResourceHandleList.begin();
                handleIt != groupSyncResourceHandleList.end(); ++handleIt)
        {
            type = handleIt->first;
            OC_LOG_V(DEBUG, TAG, "\t%d. group sync resource type - %s", i, type.c_str());
            OC_LOG(DEBUG, TAG, "\tdetails");

            resourceHandle = handleIt->second;
            OC_LOG_V(DEBUG, TAG, "\turi - %s",
                OCGetResourceUri(resourceHandle));
            OC_LOG_V(DEBUG, TAG, "\tresource type - %s",
                OCGetResourceTypeName(resourceHandle, 0));
            OC_LOG_V(DEBUG, TAG, "\tresource interface - %s",
                OCGetResourceInterfaceName(resourceHandle, 0));

            i++;
        }

        OC_LOG_V(DEBUG, TAG, "Copied Remote Group Sync Resource List. The number is %d",
            groupSyncResourceList.size());
        std::vector< std::string > list;
        i = 1;
        for (auto resourceIt = groupSyncResourceList.begin();
                resourceIt != groupSyncResourceList.end(); ++resourceIt)
        {
            type = resourceIt->first;
            OC_LOG_V(DEBUG, TAG, "\t%d. group sync resource type - %s", i, type.c_str());
            OC_LOG(DEBUG, TAG, "\tdetails");

            resource = resourceIt->second;
            OC_LOG_V(DEBUG, TAG, "\thost - %s", resource->host().c_str());
            OC_LOG_V(DEBUG, TAG, "\turi - %s", resource->uri().c_str());
            list = resource->getResourceTypes();
            OC_LOG_V(DEBUG, TAG, "\tresource type - %s", list[0].c_str());
            list = resource->getResourceInterfaces();
            OC_LOG_V(DEBUG, TAG, "\tresource interface - %s", list[0].c_str());
            i++;
        }

        OC_LOG(DEBUG, TAG, "Collection Resource Handle List");
        i = 1;
        for (handleIt = collectionResourceHandleList.begin();
                handleIt != collectionResourceHandleList.end(); ++handleIt)
        {
            type = handleIt->first;
            OC_LOG_V(DEBUG, TAG, "\t%d. collection resource type - %s", i, type.c_str());
            OC_LOG(DEBUG, TAG, "\tdetails");

            resourceHandle = handleIt->second;
            OC_LOG_V(DEBUG, TAG, "\turi - %s", OCGetResourceUri(resourceHandle));
            OC_LOG_V(DEBUG, TAG, "\tresource type - %s",
                OCGetResourceTypeName(resourceHandle, 0));
            OC_LOG_V(DEBUG, TAG, "\tresource interface - %s",
                OCGetResourceInterfaceName(resourceHandle, 0));

            childIt = childResourceHandleList.find(resourceHandle);
            if (childIt != childResourceHandleList.end())
            {
                handleList = childIt->second;
                for (unsigned int j = 0; j < handleList.size(); j++)
                {

                    OC_LOG_V(DEBUG, TAG, "\t\t%d. child resource details", j + 1);

                    resourceHandle = handleList.at(j);
                    OC_LOG_V(DEBUG, TAG, "\t\turi - %s",
                        OCGetResourceUri(resourceHandle));
                    OC_LOG_V(DEBUG, TAG, "\t\tresource type - %s",
                        OCGetResourceTypeName(resourceHandle, 0));
                    OC_LOG_V(DEBUG, TAG, "\t\tresource interface - %s",
                        OCGetResourceInterfaceName(resourceHandle, 0));
                }
            }
            i++;
        }
#endif
    }
}
