// Copyright 2015 Samsung Electronics All Rights Reserved.
//******************************************************************
//
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
#include "rd_server.h"
#include "rd_types.h"
#include "rd_payload.h"

#include "logger.h"

#define TAG  PCF("RDServer")

// This is temporary hardcoded value for bias factor.
#define OC_RD_DISC_SEL 100

static OCResourceHandle gRDHandle = NULL;

/**
 * This internal method handles RD discovery request.
 * Responds with the RD discovery payload message.
 */
static OCEntityHandlerResult HandleRDGetRequest(const OCEntityHandlerRequest *ehRequest)
{
    if (!ehRequest)
    {
        OC_LOG_V(DEBUG, TAG, "Invalid request pointer.");
        return OC_EH_ERROR;
    }

    OCEntityHandlerResult ehResult = OC_EH_OK;
    OC_LOG_V(DEBUG, TAG, "Received OC_REST_GET from client with query: %s.", ehRequest->query);

    OCRDPayload *rdPayload = OCRDPayloadCreate(RD_PAYLOAD_TYPE_DISCOVERY);
    if (!rdPayload)
    {
        return OC_STACK_NO_MEMORY;
    }

    rdPayload->rdDiscovery = OCRDDiscoveryPayloadCreate(OC_RD_DISC_SEL);
    if (!rdPayload->rdDiscovery)
    {
        OCRDPayloadDestroy(rdPayload);
        return OC_STACK_NO_MEMORY;
    }

    OCRDPayloadLog(DEBUG, TAG, rdPayload);

    OCEntityHandlerResponse ehResponse = {};
    ehResponse.requestHandle = ehRequest->requestHandle;
    ehResponse.resourceHandle = ehRequest->resource;
    ehResponse.payload = (OCPayload *)rdPayload;
    ehResponse.ehResult = ehResult;

    if (OCDoResponse(&ehResponse) != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, "Sending response failed.");
        ehResult = OC_EH_ERROR;
    }

    OCRDPayloadDestroy(rdPayload);

    return ehResult;
}

/*
 * This internal method is the entity handler for RD resources and
 * will handle REST request (GET/PUT/POST/DEL) for them.
 */
static OCEntityHandlerResult RDEntityHandler(OCEntityHandlerFlag flag,
        OCEntityHandlerRequest *ehRequest, void *callbackParameter)
{
    OCEntityHandlerResult ehRet = OC_EH_ERROR;

    if (!ehRequest)
    {
        return ehRet;
    }

    if (flag & OC_REQUEST_FLAG)
    {
        OC_LOG_V(DEBUG, TAG, "Flag includes OC_REQUEST_FLAG.");
        switch (ehRequest->method)
        {
            case OC_REST_GET:
            case OC_REST_DISCOVER:
                HandleRDGetRequest(ehRequest);
                break;
            case OC_REST_POST:
            case OC_REST_PUT:
            case OC_REST_DELETE:
            case OC_REST_OBSERVE:
            case OC_REST_OBSERVE_ALL:
            case OC_REST_CANCEL_OBSERVE:
            case OC_REST_PRESENCE:
            case OC_REST_NOMETHOD:
                break;
        }
    }

    return ehRet;
}

/**
 * Starts resource directory server and registers RD resource
 */
OCStackResult OCRDStart()
{
    OCStackResult result = OCInit(NULL, 0, OC_CLIENT_SERVER);

    if (result == OC_STACK_OK)
    {
        result = OCCreateResource(&gRDHandle,
                                  OC_RSRVD_RESOURCE_TYPE_RD,
                                  OC_RSRVD_INTERFACE_DEFAULT,
                                  OC_RSRVD_RD_URI,
                                  RDEntityHandler,
                                  NULL,
                                  (OC_ACTIVE | OC_DISCOVERABLE | OC_OBSERVABLE));

        if (result == OC_STACK_OK)
        {
            OC_LOG_V(DEBUG, TAG, "Resource Directory Started.");
        }
        else
        {
            OC_LOG(ERROR, TAG, "Failed starting Resource Directory.");
        }
    }

    return result;
}

/**
 * Stops resource directory server
 */
OCStackResult OCRDStop()
{
    OCStackResult result = OCStop();

    if (result == OC_STACK_OK)
    {
        OC_LOG_V(DEBUG, TAG, "Resource Directory Stopped.");
    }
    else
    {
        OC_LOG(ERROR, TAG, "Failed stopping Resource Directory.");
    }
    return result;
}
