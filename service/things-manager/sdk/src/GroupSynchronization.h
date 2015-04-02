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

public:

    static GroupSynchronization* getInstance();
    void deleteInstance();

    OCStackResult findGroup(std::vector< std::string > collectionResourceTypes,
            FindCallback callback);
    OCStackResult createGroup(std::string collectionResourceType);
    OCStackResult joinGroup(std::string collectionResourceTyps,
            OCResourceHandle resourceHandle);
    OCStackResult joinGroup(const std::shared_ptr< OCResource > resource,
            OCResourceHandle resourceHandle);
    OCStackResult leaveGroup(std::string collectionResourceType,
            OCResourceHandle resourceHandle);
    OCStackResult leaveGroup(const std::shared_ptr< OCResource > resource,
                std::string collectionResourceType,
                OCResourceHandle resourceHandle);
    void deleteGroup(std::string collectionResourceType);

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
//    void onSubscribePresence (OCStackResult result,
//        const unsigned int nonce/*, std::string resourceType, std::string host*/);

    void checkFindGroup(void);
    bool IsSameGroup(std::shared_ptr< OCResource > resource);
    void saveGroup(std::shared_ptr< OCResource > resource);

    void debugGroupSync(void);

};
}
#endif    // __OC_GROUPSYNCHRONIZATION__
