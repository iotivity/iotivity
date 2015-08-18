//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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
#include "rd_storage.h"

#include <pthread.h>

#include "oic_malloc.h"
#include "logger.h"

#include "rd_payload.h"

#define TAG  PCF("RDStorage")

pthread_mutex_t storageMutex = PTHREAD_MUTEX_INITIALIZER;
// This variable holds the published resources on the RD.
static OCRDStorePublishResources *g_rdStorage = NULL;

static void printStoragedResources(OCRDStorePublishResources *payload)
{
    OC_LOG_V(DEBUG, TAG, "Print Storage Resources ... ");
    for (OCRDStorePublishResources *temp = payload; temp; temp = temp->next)
    {
        if (temp->publishResource)
        {
            OCRDPublishPayloadLog(DEBUG, TAG, temp->publishResource);
        }
    }
}

OCStackResult OCRDStorePublishedResources(OCRDPublishPayload *payload)
{
    OCRDStorePublishResources *storeResource = OICCalloc(1, sizeof(OCRDStorePublishResources));
    if (!storeResource)
    {
        OC_LOG_V(ERROR, TAG, "Failed allocating memory for OCRDStorePublishResources.");
        return OC_STACK_NO_MEMORY;
    }

    OC_LOG_V(DEBUG, TAG, "Storing Resources ... ");

    storeResource->publishResource = OCRDPublishPayloadCreate(payload->ttl, payload->links);
    if (!storeResource->publishResource)
    {
        OC_LOG_V(ERROR, TAG, "Failed allocating memory for OCRDPublishResources.");
        OICFree(storeResource);
        return OC_STACK_NO_MEMORY;
    }
    storeResource->next = NULL;

    pthread_mutex_lock(&storageMutex);
    if (g_rdStorage)
    {
        OCRDStorePublishResources *temp = g_rdStorage;
        while(temp->next)
        {
            temp = temp->next;
        }
        temp->next = storeResource;
    }
    else
    {
        g_rdStorage = storeResource;
    }
    pthread_mutex_unlock(&storageMutex);

    printStoragedResources(g_rdStorage);
    return OC_STACK_OK;
}
