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

/**
 * @file
 *
 * This file contains the declaration of classes and its members related to
 * GroupSynchronization.
 */

#ifndef __OC_GROUPSYNCHRONIZATION__
#define __OC_GROUPSYNCHRONIZATION__

#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include "OCPlatform.h"
#include "OCApi.h"

using namespace OC;

namespace OIC
{
    class GroupSynchronization
    {
    private:

        std::map< std::string, OCResourceHandle > collectionResourceHandleList;
        // collection resource handle list
        std::map< OCResourceHandle, std::vector< OCResourceHandle > > childResourceHandleList;

        std::map< std::string, OCResourceHandle > groupSyncResourceHandleList;
        // group sync resource handle list
        std::map< std::string, std::shared_ptr< OCResource > > groupSyncResourceList;
        // remote group sync resource list

        std::vector< OCResourceHandle > deviceResourceHandleList; // these cannot be removed.
        OCResourceHandle deviceResourceHandle;

        OCResourceHandle collectionResourceHandle; // collection handle
        std::shared_ptr< OCResource > remoteCollectionResource;

        FindCallback findCallback;

        std::vector< std::shared_ptr< OCResource > > foundGroupResourceList;

        std::mutex foundGroupMutex;
    //    std::mutex groupSyncMutex;

        std::shared_ptr< OCResourceRequest > resourceRequest; // this is used for slow response

        static GroupSynchronization* groupSyncnstance;
        static bool bIsFinding;

    public:

        GroupSynchronization()
        {
            collectionResourceHandleList.clear();
            childResourceHandleList.clear();
            groupSyncResourceHandleList.clear();
            groupSyncResourceList.clear();
            deviceResourceHandleList.clear();

            deviceResourceHandle = NULL;
            collectionResourceHandle = NULL;
            remoteCollectionResource = NULL;
            findCallback = NULL;
        }
        ;

        ~GroupSynchronization()
        {
            std::map< std::string, OCResourceHandle >::iterator handleIt;
            for (handleIt = collectionResourceHandleList.begin();
                    handleIt != collectionResourceHandleList.end(); ++handleIt)
            {
                deleteGroup(handleIt->first);
            }
        }
        ;

        // TODO: deprecated
        static GroupSynchronization* getInstance();
        void deleteInstance();

        /**
         * API for finding a specific remote group when a resource tries to join a group.
         * Callback is called when a group is found or not.
         *
         * @param collectionResourceTypes resource types of a group to find and join
         * @param callback callback. It has OCResource param.
         *                    If a group is found, OCResource has the group resource.
         *                    Otherwise, OCResource is NULL.
         *
         * @return Returns ::OC_STACK_OK if success.
         * @note It returns OC_STACK ERROR when it is already finding a group.
         *       You should call this api after the group finding process has stopped.
         *       OCStackResult is defined in ocstack.h.
         */
        OCStackResult findGroup(std::vector< std::string > collectionResourceTypes,
                FindCallback callback);

        /**
         * API for creating a new group.
         *
         * @param collectionResourceType resource type of a group to create
         *
         * @return Returns ::OC_STACK_OK if success.
         * @note OCStackResult is defined in ocstack.h.
         */
        OCStackResult createGroup(std::string collectionResourceType);

        /**
         * API for joining a group. This API is used when a resource that has a group tries
         * to find a specific remote resource and makes it join a group
         *
         * @param collectionResourceType resource type of a group to join.
         * @param resourceHandle resource handle to join a group.
         *
         * @return Returns ::OC_STACK_OK if success.
         *
         * @note If you want to join the resource in the remote(other) process,
         *       use joinGroup(const std::shared_ptr< OCResource >, OCResourceHandle)
         *       instead of this.
         *       OCStackResult is defined in ocstack.h.
         */
        OCStackResult joinGroup(std::string collectionResourceTyps,
                OCResourceHandle resourceHandle);

        /**
         * API for joining a group. This API is used when a resource that
         * doesn't have a group tries to find and join a specific remote group.
         *
         * @param resource group resource pointer to join.
         *                   It can be the callback result of findGroup().
         * @param resourceHandle resource handle to join a group.
         *
         * @return Returns ::OC_STACK_OK if success.
         * @note If you want to join the resource in the same process,
         *       use joinGroup(std::string, OCResourceHandle)
         *       instead of this.
         *       OCStackResult is defined in ocstack.h.
         */
        OCStackResult joinGroup(const std::shared_ptr< OCResource > resource,
                OCResourceHandle resourceHandle);

        /**
         * API for leaving a joined group.
         *
         * @param collectionResourceType resource type of a group to leave.
         * @param resourceHandle resource handle to leave a group.
         *
         * @return Returns ::OC_STACK_OK if success.
         * @note OCStackResult is defined in ocstack.h.
         */
        OCStackResult leaveGroup(std::string collectionResourceType,
                OCResourceHandle resourceHandle);

        /**
         * API for leaving a joined group.
         *
         * @param resource group resource pointer to join.
         *                   It can be the callback result of findGroup().
         *
         * @param collectionResourceType resource type of a group to leave.
         * @param resourceHandle resource handle to leave a group.
         *
         * @return Returns ::OC_STACK_OK if success.
         *
         * @note OCStackResult is defined in ocstack.h.
         */
        OCStackResult leaveGroup(const std::shared_ptr< OCResource > resource,
                    std::string collectionResourceType,
                    OCResourceHandle resourceHandle);

        /**
         * API for deleting a group.
         *
         * @param collectionResourceType resource type of a group to delete.
         *
         * @return void
         */
        void deleteGroup(std::string collectionResourceType);

        /**
         * API for getting a list of joined groups.
         *
         * @return std::map return value of this API.
         *                  It returns group resource type and group resource handle as a map type.
         */
        std::map< std::string, OCResourceHandle > getGroupList();

    private:

        OCEntityHandlerResult groupEntityHandler(
                const std::shared_ptr< OCResourceRequest > request);

        void onFindGroup(std::shared_ptr< OCResource > resource);
        void onJoinGroup(const HeaderOptions& headerOptions, const OCRepresentation& rep,
                const int eCode);
        void onFindResource(std::shared_ptr< OCResource > resource);
        void onGetJoinedRemoteChild(const HeaderOptions& headerOptions, const OCRepresentation& rep,
                const int eCode);
        void onLeaveGroup(const HeaderOptions& headerOptions, const OCRepresentation& rep,
                const int eCode);
        void checkFindGroup(void);
        bool IsSameGroup(std::shared_ptr< OCResource > resource);
        void saveGroup(std::shared_ptr< OCResource > resource);

        void debugGroupSync(void);

    };
}
#endif    // __OC_GROUPSYNCHRONIZATION__
