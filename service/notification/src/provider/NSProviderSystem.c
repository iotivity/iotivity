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
#include "NSProviderSystem.h"

static NSConnectionState NSProviderConnectionState;
NSProviderInfo * providerInfo;

void NSSetProviderConnectionState(NSConnectionState state)
{
    NS_LOG(DEBUG, "NSSetProviderConnectionState");

    NSProviderConnectionState = state;
}

NSConnectionState NSGetProviderConnectionState()
{
    NS_LOG(DEBUG, "Change Connection State");

    return NSProviderConnectionState;
}

void NSInitProviderInfo()
{
    NS_LOG(DEBUG, "NSInitProviderInfo");

    providerInfo = (NSProviderInfo *) OICMalloc(sizeof(NSProviderInfo));
    NSGenerateUUIDStr(providerInfo->providerId);
    providerInfo->providerName = NULL;
}

void NSDeinitProviderInfo()
{
    NS_LOG(DEBUG, "NSDeinitProviderInfo");

    if(providerInfo == NULL)
    {
        NS_LOG(DEBUG, "providerInfo is NULL");
        return;
    }

    if(providerInfo->providerName != NULL)
    {
        OICFree(providerInfo->providerName);
        providerInfo->providerName = NULL;
    }

    OICFree(providerInfo);
    providerInfo = NULL;
}

NSProviderInfo * NSGetProviderInfo()
{
    NS_LOG(DEBUG, "Change Connection State");

    return providerInfo;
}

