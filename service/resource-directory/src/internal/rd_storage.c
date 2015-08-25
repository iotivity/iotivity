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
#include "oic_string.h"
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

    OCRDLinksPayload* linksPayload = NULL;
    for ( OCRDLinksPayload* links = payload->links ; links; links = links->next)
    {
        OCRDLinksPayloadCreate(links->href, links->rt, links->itf, &linksPayload);
    }

    storeResource->publishResource = OCRDPublishPayloadCreate(payload->ttl, linksPayload);
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
        while (temp->next)
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

OCStackResult OCRDCheckPublishedResource(const char *interfaceType, const char *resourceType,
        char **uri, char **rt, char **itf)
{
    // ResourceType and InterfaceType if both are NULL it will return. If either is
    // not null it will continue execution.
    if (!resourceType && !interfaceType)
    {
        OC_LOG_V(DEBUG, TAG, "Missing resource type and interace type.");
        return OC_STACK_INVALID_PARAM;
    }

    OC_LOG_V(DEBUG, TAG, "Check Resource in RD");

    if (g_rdStorage && g_rdStorage->publishResource && g_rdStorage->publishResource->links)
    {
        for (OCRDLinksPayload *tLinks = g_rdStorage->publishResource->links; tLinks; tLinks = tLinks->next)
        {
            bool found = false;
            // If either rt or itf are NULL, it should skip remaining code execution.
            if (!tLinks->rt || !tLinks->itf)
            {
                OC_LOG_V(DEBUG, TAG, "Either resource type and interface type are missing.");
                continue;
            }
            OC_LOG_V(DEBUG, TAG, "Resource Type: %s %s", resourceType, tLinks->rt);
            OC_LOG_V(DEBUG, TAG, "Resource Type: %s %s", interfaceType, tLinks->itf);
            if (resourceType && strcmp(resourceType, tLinks->rt) == 0)
            {
                found = true;
            }

            if (interfaceType && strcmp(interfaceType, tLinks->itf) == 0)
            {
                found = true;
            }

            if (found)
            {
                *uri = OICStrdup(tLinks->href);
                if (!*uri)
                {
                    OC_LOG_V(ERROR, TAG, "Copy failed..");
                    return false;
                }
                *rt = OICStrdup(tLinks->rt);
                if (!*rt)
                {
                    OC_LOG_V(ERROR, TAG, "Copy failed..");
                    OICFree(*uri);
                    return false;
                }
                *itf = OICStrdup(tLinks->itf);
                if (!*itf)
                {
                    OC_LOG_V(ERROR, TAG, "Copy failed..");
                    OICFree(*uri);
                    OICFree(*rt);
                    return false;
                }
                return true;
            }
        }
    }

    return false;
}
