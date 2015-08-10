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
#include "logger.h"
#include "oic_malloc.h"

#include "rd_client.h"
#include "rd_payload.h"

#define DEFAULT_CONTEXT_VALUE 0x99

#define TAG  PCF("RDClient")

static void retreiveRDDetails(OCClientResponse *clientResponse, OCRDBiasFactorCB clientCB)
{
    OC_LOG_V(DEBUG, TAG, "\tAddress: %s : %d",  clientResponse->devAddr.addr,
             clientResponse->devAddr.port);

    OCRDPayload *payload = (OCRDPayload *) clientResponse->payload;
    if (payload)
    {
        OC_LOG_V(DEBUG, TAG, "\tType: %d",  payload->base.type);
        OC_LOG_V(DEBUG, TAG, "\tPayloadType: %d",  payload->payloadType);
        OC_LOG_V(DEBUG, TAG, "\tBiasFactor: %d",  payload->rdDiscovery->sel);
    }
    // TODO: Multiple Resource Directory will have different biasFactor,
    // needs to cache here detail
    // and after certain timeout then decide based on the biasFactor.
    //if (biasFactor > 75)
    clientCB(clientResponse->devAddr.addr, clientResponse->devAddr.port);

}

static OCStackApplicationResult discoverRDCB(void *ctx,
        OCDoHandle handle, OCClientResponse *clientResponse)
{
    OC_LOG(DEBUG, TAG, "Found Resource Directory");
    OCStackApplicationResult ret = OC_STACK_DELETE_TRANSACTION;

    OCRDClientContextCB *cb = (OCRDClientContextCB*) ctx;
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
    OCStackResult result = OC_STACK_ERROR;
    OCCallbackData cbData;
    OCDoHandle handle;

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

    cbData.cb = discoverRDCB;
    cbData.context = (void *)(&cbContext);
    cbData.cd = NULL;

    result = OCDoResource(&handle,
                          OC_REST_DISCOVER,
                          queryUri,
                          0,
                          0,
                          CT_ADAPTER_IP,
                          OC_LOW_QOS,
                          &cbData,
                          NULL,
                          0);

    if (result == OC_STACK_OK)
    {
        OC_LOG_V(DEBUG, TAG, "Resource Directory finding query send successful...");
    }
    else
    {
        OC_LOG_V(ERROR, TAG, "Resource Directory finding query send failed...");
    }

    return result;
}
