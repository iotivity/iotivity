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
#include <stdarg.h>

#include "rd_client.h"

#include "logger.h"
#include "oic_malloc.h"
#include "oic_string.h"

#include "rd_payload.h"

#define DEFAULT_CONTEXT_VALUE 0x99
#define OC_RD_PUBLISH_TTL 86400

#define TAG  PCF("RDClient")

static OCStackResult sendRequest(OCMethod method, char *uri, OCDevAddr *addr,
        OCPayload *payload, OCCallbackData cbData)
{
    OCDoHandle handle;
    OCStackResult result;

    result = OCDoResource(handle,
        method,
        uri,
        addr,
        payload,
        CT_ADAPTER_IP,
        OC_LOW_QOS,
        &cbData,
        NULL,
        0);

    if (result == OC_STACK_OK)
    {
        OC_LOG_V(DEBUG, TAG, "Resource Directory send successful...");
    }
    else
    {
        OC_LOG_V(ERROR, TAG, "Resource Directory send failed...");
    }

    return result;
}

static OCStackApplicationResult handlePublishCB(void *ctx,
        OCDoHandle handle, OCClientResponse *clientResponse)
{
    OCStackApplicationResult ret = OC_STACK_DELETE_TRANSACTION;
    OC_LOG_V(DEBUG, TAG, "Successfully published resources.");

    // TOOO: Stop multicast traffic on the client.

    return ret;
}

static void retreiveRDDetails(OCClientResponse *clientResponse, OCRDBiasFactorCB clientCB)
{
    OC_LOG_V(DEBUG, TAG, "\tAddress of the RD: %s : %d",  clientResponse->devAddr.addr,
            clientResponse->devAddr.port);

    OCRDPayload *payload = (OCRDPayload *) clientResponse->payload;
    OCRDPayloadLog(DEBUG, TAG, payload);

    // TODO: Multiple Resource Directory will have different biasFactor,
    // needs to cache here detail
    // and after certain timeout then decide based on the biasFactor.
    //if (biasFactor > 75)
    clientCB(clientResponse->devAddr.addr, clientResponse->devAddr.port);

}

static OCStackApplicationResult handleDiscoverCB(void *ctx,
        OCDoHandle handle, OCClientResponse *clientResponse)
{
    OC_LOG(DEBUG, TAG, "Found Resource Directory");
    OCStackApplicationResult ret = OC_STACK_DELETE_TRANSACTION;

    OCRDClientContextCB *cb = (OCRDClientContextCB *) ctx;
    if (!cb)
    {
        OC_LOG(ERROR, TAG, "RD Context Invalid Parameters.");
        return OC_STACK_INVALID_PARAM;
    }

    if (cb->context != (void *) DEFAULT_CONTEXT_VALUE)
    {
        OC_LOG(ERROR, TAG, "RD Context Invalid Context Value Parameters.");
        return OC_STACK_INVALID_PARAM;
    }

    OC_LOG(DEBUG, TAG, "Callback Context for DISCOVER query received successfully.");

    if (clientResponse && clientResponse->result == OC_STACK_OK)
    {
        retreiveRDDetails(clientResponse, cb->cbFunc);
    }
    else
    {
        OC_LOG(ERROR, TAG, "Discovery of RD Failed");
    }

    return ret;
}

OCStackResult OCRDDiscover(OCRDBiasFactorCB cbBiasFactor)
{
    if (!cbBiasFactor)
    {
        OC_LOG(DEBUG, TAG, "No callback function specified.");
        return OC_STACK_INVALID_CALLBACK;
    }

    /* Start a discovery query*/
    char queryUri[MAX_URI_LENGTH] = { '\0' };
    snprintf(queryUri, MAX_URI_LENGTH, "coap://%s%s", OC_MULTICAST_PREFIX, OC_RSRVD_RD_URI);

    OC_LOG_V(DEBUG, TAG, "Querying RD: %s\n", queryUri);

    OCRDClientContextCB cbContext;
    cbContext.context = (void *)DEFAULT_CONTEXT_VALUE;
    cbContext.cbFunc = cbBiasFactor;

    OCCallbackData cbData;
    cbData.cb = handleDiscoverCB;
    cbData.context = (void *)(&cbContext);
    cbData.cd = NULL;

    return sendRequest(OC_REST_DISCOVER, queryUri, NULL, NULL, cbData);
}

OCStackResult OCRDPublish(char *addr, uint16_t port, int numArg, ... )
{
    if (!addr)
    {
        OC_LOG_V(ERROR, TAG, "RD address not specified.");
        return OC_STACK_INVALID_PARAM;
    }

    char targetUri[MAX_URI_LENGTH];
    snprintf(targetUri, MAX_URI_LENGTH, "coap://%s:%d%s?rt=%s", addr, port,
            OC_RSRVD_RD_URI, OC_RSRVD_RESOURCE_TYPE_RDPUBLISH);
    OC_LOG_V(DEBUG, TAG, "Target URI : %s", targetUri);

    // Gather all resources locally and do publish
    OCCallbackData cbData = { 0 };
    cbData.cb = &handlePublishCB;
    cbData.cd = NULL;
    cbData.context = (void*)DEFAULT_CONTEXT_VALUE;

    OCRDLinksPayload* linksPayload = NULL;

    va_list arguments;
    va_start (arguments, numArg);

    for (int j = 0 ; j < numArg; j++)
    {
        OCResourceHandle handle = va_arg(arguments, OCResourceHandle);
        if (handle)
        {
            const char* uri = OCGetResourceUri(handle);
            const char* rt  = OCGetResourceTypeName(handle, 0);
            const char* itf = OCGetResourceInterfaceName(handle, 0);
            if (uri && rt && itf)
            {
                OCRDLinksPayloadCreate(uri, rt, itf, &linksPayload);
            }
        }
    }
    va_end(arguments);

    OCRDPayload *rdPayload = OCRDPayloadCreate(RD_PAYLOAD_TYPE_PUBLISH);
    if (!rdPayload)
    {
        OC_LOG_V(ERROR, TAG, "Failed allocating memory.");
        return OC_STACK_NO_MEMORY;
    }
    rdPayload->rdPublish = OCRDPublishPayloadCreate(OC_RD_PUBLISH_TTL, linksPayload);
    if (!rdPayload->rdPublish)
    {
        OC_LOG_V(ERROR, TAG, "Failed allocating memory.");
        return OC_STACK_NO_MEMORY;
    }

    OCDevAddr rdAddr = { 0 };
    OICStrcpy(rdAddr.addr, MAX_ADDR_STR_SIZE, addr);
    rdAddr.port = port;

    OCRDPayloadLog(DEBUG, TAG, rdPayload);

    return sendRequest(OC_REST_POST, targetUri, &rdAddr, (OCPayload *)rdPayload, cbData);
}
