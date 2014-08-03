//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
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

#include <string.h>
#include "ocstack.h"
#include "ocstackinternal.h"
#include "ocresource.h"
#include "logger.h"
#include "debug.h"

/// Module Name
#define TAG PCF("ocresource")

extern OCResource *headResource;

static const char * VIRTUAL_RSRCS[] = {
       "/oc/core",
       "/oc/core/d",
       "/oc/core/types/d",
};

TODO ("Does it make sense to make this method as inline")
const char * GetVirtualResourceUri( OCVirtualResources resource)
{
    if (resource < OC_MAX_RESOURCES)
    {
        return VIRTUAL_RSRCS[resource];
    }

    return NULL;
}

uint8_t IsVirtualResource(const char* resourceUri)
{
    for (int i = 0; i < OC_MAX_RESOURCES; i++)
    {
        if (strcmp(resourceUri, GetVirtualResourceUri(i)) == 0)
        {
            return 1;
        }
    }
    return 0;
}


OCResource *FindResourceByUri(const char* resourceUri)
{
    OCResource * pointer = headResource;
    while (pointer) {
        if (strcmp(resourceUri, pointer->uri) == 0) {
            return pointer;
        }
        pointer = pointer->next;
    }
    OC_LOG(INFO, TAG, PCF("Resource not found"));
    return NULL;
}

