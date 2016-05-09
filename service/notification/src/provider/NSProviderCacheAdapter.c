//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
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
#include "NSProviderCacheAdapter.h"


NSWhiteList whiteMessageList;
NSBlackList blackMessageList;

NSWhiteList whiteSyncList;
NSBlackList blackSyncList;

NSResult NSProviderInitCache()
{
    // create cache

    return NS_OK;
}

NSResult NSProviderWriteToCache(NSCache target, OCObservationId observeId, int state)
{
    printf("NSProviderWriteToCache - 0\n");
    if(state == 0) // message
    {
        printf("NSProviderWriteToCache - 0 - 1\n");
        if (NS_CONSUMER_BLACKLIST == target)
        {
            printf("NSProviderWriteToCache - 0 - 1 - 1\n");
            blackMessageList.idList[blackMessageList.size] = observeId;
            blackMessageList.size++;
        }
        else if (NS_CONSUMER_WHITELIST == target)
        {
            printf("NSProviderWriteToCache - 0 - 1 - 2\n");
            whiteMessageList.idList[whiteMessageList.size] = observeId;
            whiteMessageList.size++;
        }
    }
    else if(state == 1)
    {
        printf("NSProviderWriteToCache - 1 - 1\n");
        if (NS_CONSUMER_BLACKLIST == target)
        {
            printf("NSProviderWriteToCache - 1 - 1 - 1\n");
            blackSyncList.idList[blackSyncList.size] = observeId;
            blackSyncList.size++;
        }
        else if (NS_CONSUMER_WHITELIST == target)
        {
            printf("NSProviderWriteToCache - 1 - 1 - 2\n");
            whiteSyncList.idList[whiteSyncList.size] = observeId;
            whiteSyncList.size++;
        }
    }

    return NS_OK;
}

NSCommonList * NSProviderReadCache(NSCache target, int state)
{
    if(state == 0) // message
    {
        if(NS_CONSUMER_BLACKLIST == target)
        {
            return (NSCommonList *)&blackMessageList;
        }
        else if(NS_CONSUMER_WHITELIST == target)
        {
            return (NSCommonList *)&whiteMessageList;
        }
    }
    else if(state == 1)
    {
        if(NS_CONSUMER_BLACKLIST == target)
        {
            return (NSCommonList *)&blackSyncList;
        }
        else if(NS_CONSUMER_WHITELIST == target)
        {
            return (NSCommonList *)&whiteSyncList;
        }
    }
    return NULL;
}

NSResult NSProviderDeleteCache()
{
    return NS_OK;
}
