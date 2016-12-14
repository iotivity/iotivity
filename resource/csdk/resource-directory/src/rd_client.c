//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License a
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
#include "rd_client.h"

#include <stdlib.h>
#include <string.h>

#include "oic_malloc.h"
#include "oic_string.h"
#include "octypes.h"
#include "ocstack.h"
#include "ocpayload.h"
#include "payload_logging.h"

#define TAG "OIC_RD_CLIENT"

#ifdef RD_CLIENT

OCStackResult OCRDDiscover(OCConnectivityType connectivityType, OCCallbackData *cbBiasFactor,
                           OCQualityOfService qos)
{
    if (!cbBiasFactor || !cbBiasFactor->cb)
    {
        OIC_LOG(DEBUG, TAG, "No callback function specified.");
        return OC_STACK_INVALID_CALLBACK;
    }

    /* Start a discovery query*/
    char queryUri[MAX_URI_LENGTH] = { '\0' };
    snprintf(queryUri, MAX_URI_LENGTH, "coap://%s%s", OC_MULTICAST_PREFIX, OC_RSRVD_RD_URI);
    OIC_LOG_V(DEBUG, TAG, "Querying RD: %s\n", queryUri);

    return OCDoResource(NULL, OC_REST_DISCOVER, queryUri, NULL, NULL, connectivityType, qos,
                        cbBiasFactor, NULL, 0);
}

static void RDPublishContextDeleter(void *ctx)
{
    OCCallbackData *cbData = (OCCallbackData*)ctx;
    if (cbData->cd)
    {
        cbData->cd(cbData->context);
    }
    OICFree(cbData);
}

OCStackApplicationResult RDPublishCallback(void *ctx,
                                           OCDoHandle handle,
                                           OCClientResponse *clientResponse)
{
    OCCallbackData *cbData = (OCCallbackData*)ctx;

    // Update resource unique id in stack.
    OCRepPayload **links = NULL;
    size_t dimensions[MAX_REP_ARRAY_DEPTH] = {0};
    if (clientResponse && clientResponse->payload)
    {
        OCRepPayload *rdPayload = (OCRepPayload *) clientResponse->payload;
        if (!OCRepPayloadGetPropObjectArray(rdPayload, OC_RSRVD_LINKS, &links, dimensions))
        {
            OIC_LOG_V(DEBUG, TAG, "No links in publish response");
            goto exit;
        }
        for(size_t i = 0; i < dimensions[0]; i++)
        {
            char *uri = NULL;
            if (!OCRepPayloadGetPropString(links[i], OC_RSRVD_HREF, &uri))
            {
                OIC_LOG_V(ERROR, TAG, "Missing 'href' in publish response");
                goto next;
            }
            OCResourceHandle handle = OCGetResourceHandleAtUri(uri);
            if (handle == NULL)
            {
                OIC_LOG_V(ERROR, TAG, "No resource exists with uri: %s", uri);
                goto next;
            }
            int64_t ins = 0;
            if (!OCRepPayloadGetPropInt(links[i], OC_RSRVD_INS, &ins))
            {
                OIC_LOG_V(ERROR, TAG, "Missing 'ins' in publish response");
                goto next;
            }
            OCBindResourceInsToResource(handle, ins);
        next:
            OICFree(uri);
        }
    }

exit:
    for (size_t i = 0; i < dimensions[0]; i++)
    {
        OCRepPayloadDestroy(links[i]);
    }
    OICFree(links);
    return cbData->cb(cbData->context, handle, clientResponse);
}

OCStackResult OCRDPublish(const char *host, OCConnectivityType connectivityType,
                          OCResourceHandle *resourceHandles, uint8_t nHandles,
                          OCCallbackData *cbData, OCQualityOfService qos)
{
    // Validate input parameters.
    if (!host)
    {
        return OC_STACK_INVALID_IP;
    }

    if (!cbData || !cbData->cb)
    {
        return OC_STACK_INVALID_CALLBACK;
    }

    // Get Device ID from stack.
    const unsigned char *id = (const unsigned char *) OCGetServerInstanceIDString();

    return OCRDPublishWithDeviceId(host, id, connectivityType, resourceHandles, nHandles,
                                   cbData, qos);
}

static OCRepPayload *RDPublishPayloadCreate(const unsigned char *id,
                                            const OCResourceHandle *resourceHandles, uint8_t nHandles)
{
    OCRepPayload *rdPayload =  (OCRepPayload *)OCRepPayloadCreate();
    if (!rdPayload)
    {
        return NULL;
    }

    if (id)
    {
        OCRepPayloadSetPropString(rdPayload, OC_RSRVD_DEVICE_ID, (const char*) id);
    }

    char *deviceName = NULL;
    OCGetPropertyValue(PAYLOAD_TYPE_DEVICE, OC_RSRVD_DEVICE_NAME, (void **) &deviceName);
    if (deviceName)
    {
        OCRepPayloadSetPropString(rdPayload, OC_RSRVD_DEVICE_NAME, deviceName);
        OICFree(deviceName);
    }

    char *platformModelName = NULL;
    OCGetPropertyValue(PAYLOAD_TYPE_PLATFORM, OC_RSRVD_MODEL_NUM, (void **) &platformModelName);
    if (platformModelName)
    {
        OCRepPayloadSetPropString(rdPayload, OC_DATA_MODEL_NUMBER, platformModelName);
        OICFree(platformModelName);
    }

    OCRepPayloadSetPropInt(rdPayload, OC_RSRVD_DEVICE_TTL, OIC_RD_PUBLISH_TTL);

    OCRepPayload **linkArr = (OCRepPayload**)OICCalloc(nHandles, sizeof(OCRepPayload *));
    if (!linkArr)
    {
        OCRepPayloadDestroy(rdPayload);
        return NULL;
    }

    size_t dimensions[MAX_REP_ARRAY_DEPTH] = {nHandles, 0, 0};
    for (uint8_t j = 0; j < nHandles; j++)
    {
        OCResourceHandle handle = resourceHandles[j];
        if (handle)
        {
            OCRepPayload *link = OCRepPayloadCreate();

            const char *uri = OCGetResourceUri(handle);
            if (uri)
            {
                OCRepPayloadSetPropString(link, OC_RSRVD_HREF, uri);
            }

            uint8_t numElement = 0;
            if (OC_STACK_OK == OCGetNumberOfResourceTypes(handle, &numElement))
            {
                size_t rtDim[MAX_REP_ARRAY_DEPTH] = {numElement, 0, 0};
                char **rt = (char **)OICMalloc(sizeof(char *) * numElement);
                for (uint8_t i = 0; i < numElement; ++i)
                {
                    const char *value = OCGetResourceTypeName(handle, i);
                    OIC_LOG_V(DEBUG, TAG, "value: %s", value);
                    rt[i] = OICStrdup(value);
                }
                OCRepPayloadSetStringArrayAsOwner(link, OC_RSRVD_RESOURCE_TYPE, rt, rtDim);
            }

            numElement = 0;
            if (OC_STACK_OK == OCGetNumberOfResourceInterfaces(handle, &numElement))
            {
                size_t ifDim[MAX_REP_ARRAY_DEPTH] = {numElement, 0, 0};
                char **itf = (char **)OICMalloc(sizeof(char *) * numElement);
                for (uint8_t i = 0; i < numElement; ++i)
                {
                    const char *value = OCGetResourceInterfaceName(handle, i);
                    OIC_LOG_V(DEBUG, TAG, "value: %s", value);
                    itf[i] = OICStrdup(value);
                }
                OCRepPayloadSetStringArrayAsOwner(link, OC_RSRVD_INTERFACE, itf, ifDim);
            }

            uint8_t ins = 0;
            if (OC_STACK_OK == OCGetResourceIns(handle, &ins))
            {
                OCRepPayloadSetPropInt(link, OC_RSRVD_INS, ins);
            }

            size_t mtDim[MAX_REP_ARRAY_DEPTH] = {1, 0, 0};
            char **mediaType = (char **)OICMalloc(sizeof(char *) * 1);
            if (!mediaType)
            {
                OIC_LOG(ERROR, TAG, "Memory allocation failed!");

                for(uint8_t i = 0; i < nHandles; i++)
                {
                    OCRepPayloadDestroy(linkArr[i]);
                }
                OICFree(linkArr);
                OCRepPayloadDestroy(rdPayload);
                return NULL;
            }

            mediaType[0] = OICStrdup(OC_MEDIA_TYPE_APPLICATION_JSON);
            OCRepPayloadSetStringArrayAsOwner(link, OC_RSRVD_MEDIA_TYPE, mediaType,
            mtDim);

            OCResourceProperty p = OCGetResourceProperties(handle);
            p = (OCResourceProperty) ((p & OC_DISCOVERABLE) | (p & OC_OBSERVABLE));
            OCRepPayload *policy = OCRepPayloadCreate();
            OCRepPayloadSetPropInt(policy, OC_RSRVD_BITMAP, p);
            OCRepPayloadSetPropObjectAsOwner(link, OC_RSRVD_POLICY, policy);

            linkArr[j] = link;
        }
    }

    OCRepPayloadSetPropObjectArray(rdPayload, OC_RSRVD_LINKS, (const OCRepPayload **)linkArr, dimensions);
    OIC_LOG_PAYLOAD(DEBUG, (OCPayload *) rdPayload);

    for (uint8_t i = 0; i < nHandles; i++)
    {
        OCRepPayloadDestroy(linkArr[i]);
    }
    OICFree(linkArr);

    return rdPayload;
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
    OCResourceHandle defaultResHandles[OIC_RD_DEFAULT_RESOURCE] = { 0 };
    uint8_t nPubResHandles = nHandles;

    // if resource handles is null, "/oic/p" and "/oic/d" resource will be published to RD.
    if (!pubResHandle)
    {
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

    OCRepPayload *rdPayload = RDPublishPayloadCreate(id, pubResHandle, nPubResHandles);
    if (!rdPayload)
    {
        return OC_STACK_ERROR;
    }

    OCCallbackData *rdPublishContext = (OCCallbackData*)OICMalloc(sizeof(OCCallbackData));
    if (!rdPublishContext)
    {
        return OC_STACK_NO_MEMORY;
    }
    memcpy(rdPublishContext, cbData, sizeof(OCCallbackData));
    OCCallbackData rdPublishCbData;
    rdPublishCbData.context = rdPublishContext;
    rdPublishCbData.cb = RDPublishCallback;
    rdPublishCbData.cd = RDPublishContextDeleter;
    return OCDoResource(NULL, OC_REST_POST, targetUri, NULL, (OCPayload *)rdPayload,
                        connectivityType, qos, &rdPublishCbData, NULL, 0);
}

OCStackResult OCRDDelete(const char *host, OCConnectivityType connectivityType,
                         OCResourceHandle *resourceHandles, uint8_t nHandles,
                         OCCallbackData *cbData, OCQualityOfService qos)
{
    // Validate input parameters
    if (!host)
    {
        return OC_STACK_INVALID_IP;
    }

    if (!cbData || !cbData->cb)
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
    int targetUriBufferRequired = snprintf(targetUri, MAX_URI_LENGTH, "%s%s?di=%s", host, OC_RSRVD_RD_URI, id);
    if (targetUriBufferRequired >= MAX_URI_LENGTH || targetUriBufferRequired < 0)
    {
        return OC_STACK_INVALID_URI;
    }


    int queryLength = 0;
    char queryParam[MAX_URI_LENGTH] = { 0 };
    for (uint8_t j = 0; j < nHandles; j++)
    {
        OCResource *handle = (OCResource *) resourceHandles[j];
        uint8_t ins = 0;
        OCGetResourceIns(handle, &ins);
        int lenBufferRequired = snprintf(queryParam + queryLength, MAX_URI_LENGTH - queryLength, "&ins=%d", ins);
        if (lenBufferRequired >= (MAX_URI_LENGTH - queryLength) || lenBufferRequired < 0)
        {
            return OC_STACK_INVALID_URI;
        }
        queryLength += lenBufferRequired;
        OIC_LOG_V(DEBUG, TAG, "queryParam [%s]", queryParam);
    }

    if (targetUriBufferRequired + queryLength + 1 > MAX_URI_LENGTH)
    {
        return OC_STACK_INVALID_URI;
    }

    OICStrcatPartial(targetUri, sizeof(targetUri), queryParam, strlen(queryParam));
    OIC_LOG_V(DEBUG, TAG, "Target URI: %s", targetUri);

    return OCDoResource(NULL, OC_REST_DELETE, targetUri, NULL, NULL, connectivityType,
                        qos, cbData, NULL, 0);
}
#endif
