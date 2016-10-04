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
#include "iotivity_config.h"
#include "oicresourcedirectory.h"

#include "rdpayload.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "octypes.h"
#include "ocstack.h"
#include "ocpayload.h"
#include "rdpayload.h"
#include "ocresource.h"
#include "payload_logging.h"

#define TAG "OIC_RI_RESOURCE_DIRECTORY"

#ifdef RD_CLIENT
OCStackResult OCRDPublish(const char *host, OCConnectivityType connectivityType,
                          OCResourceHandle *resourceHandles, uint8_t nHandles,
                          OCCallbackData *cbData, OCQualityOfService qos)
{
    // Validate input parameters.
    if (!host || !cbData || !cbData->cb)
    {
        return OC_STACK_INVALID_CALLBACK;
    }

    // Get Device ID from stack.
    const unsigned char *id = (const unsigned char *) OCGetServerInstanceIDString();

    return OCRDPublishWithDeviceId(host, id, connectivityType, resourceHandles, nHandles,
                                   cbData, qos);
}

OCStackResult OCRDPublishWithDeviceId(const char *host, const unsigned char *id,
                                      OCConnectivityType connectivityType,
                                      OCResourceHandle *resourceHandles, uint8_t nHandles,
                                      OCCallbackData *cbData, OCQualityOfService qos)
{
    // Validate input parameters.
    if (!host || !cbData || !cbData->cb || !id)
    {
        return OC_STACK_INVALID_CALLBACK;
    }

    OIC_LOG_V(DEBUG, TAG, "Publish Resource to RD with device id [%s]", id);

    OCResourceHandle *pubResHandle = resourceHandles;
    uint8_t nPubResHandles = nHandles;

    // if resource handles is null, "/oic/p" and "/oic/d" resource will be published to RD.
    if (!pubResHandle && !nPubResHandles)
    {
        OCResourceHandle defaultResHandles[OIC_RD_DEFAULT_RESOURCE] = { 0 };

        // get "/oic/d" and "/oic/p" resource handle from stack.
        defaultResHandles[0] = OCGetResourceHandleAtUri(OC_RSRVD_DEVICE_URI);
        defaultResHandles[1] = OCGetResourceHandleAtUri(OC_RSRVD_PLATFORM_URI);

        for (uint8_t j = 0; j < OIC_RD_DEFAULT_RESOURCE; j++)
        {
            if (defaultResHandles[j])
            {
                OIC_LOG_V(DEBUG, TAG, "Add virtual resource(%s) to resource handle list",
                          OCGetResourceUri(defaultResHandles[j]));
            }
        }

        pubResHandle = defaultResHandles;
        nPubResHandles = OIC_RD_DEFAULT_RESOURCE;
    }

    char targetUri[MAX_URI_LENGTH] = { 0 };
    snprintf(targetUri, MAX_URI_LENGTH, "%s%s?rt=%s", host,
             OC_RSRVD_RD_URI, OC_RSRVD_RESOURCE_TYPE_RDPUBLISH);
    OIC_LOG_V(DEBUG, TAG, "Target URI: %s", targetUri);

    OCPayload *rdPayload = (OCPayload *) OCRDPublishPayloadCreate(id, pubResHandle, nPubResHandles,
                                                                  OIC_RD_PUBLISH_TTL);
    if (!rdPayload)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to create RD Payload");
        return OC_STACK_NO_MEMORY;
    }

    OIC_LOG(DEBUG, TAG, "Create RD payload successfully");

    return OCDoResource(NULL, OC_REST_POST, targetUri, NULL, (OCPayload *)rdPayload,
                        connectivityType, qos, cbData, NULL, 0);
}

OCStackResult OCRDDelete(const char *host, OCConnectivityType connectivityType,
                         OCResourceHandle *resourceHandles, uint8_t nHandles,
                         OCCallbackData *cbData, OCQualityOfService qos)
{
    // Validate input parameters
    if (!host || !cbData || !cbData->cb)
    {
        return OC_STACK_INVALID_CALLBACK;
    }

    const unsigned char *id = (const unsigned char *) OCGetServerInstanceIDString();

    return OCRDDeleteWithDeviceId(host, id, connectivityType, resourceHandles, nHandles,
                                  cbData, qos);
}

OCStackResult OCRDDeleteWithDeviceId(const char *host, const unsigned char *id,
                                     OCConnectivityType connectivityType,
                                     OCResourceHandle *resourceHandles, uint8_t nHandles,
                                     OCCallbackData *cbData, OCQualityOfService qos)
{
    // Validate input parameters
    if (!host || !cbData || !cbData->cb || !id)
    {
        return OC_STACK_INVALID_CALLBACK;
    }

    OIC_LOG_V(DEBUG, TAG, "Delete Resource to RD with device id [%s]", id);

    char targetUri[MAX_URI_LENGTH] = { 0 };
    snprintf(targetUri, MAX_URI_LENGTH, "%s%s?di=%s", host, OC_RSRVD_RD_URI, id);

    char queryParam[MAX_URI_LENGTH] = { 0 };
    for (uint8_t j = 0; j < nHandles; j++)
    {
        OCResource *handle = (OCResource *) resourceHandles[j];
        snprintf(queryParam, MAX_URI_LENGTH, "&ins=%d", handle->ins);
    }

    OICStrcatPartial(targetUri, sizeof(targetUri), queryParam, strlen(queryParam));
    OIC_LOG_V(DEBUG, TAG, "Target URI: %s", targetUri);

    return OCDoResource(NULL, OC_REST_DELETE, targetUri, NULL, NULL, connectivityType,
                        qos, cbData, NULL, 0);
}
#endif
