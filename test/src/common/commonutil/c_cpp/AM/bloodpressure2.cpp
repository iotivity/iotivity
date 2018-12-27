/******************************************************************
*
* Copyright 2018 Open Connectivity Foundation All Rights Reserved.
*
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/

#include "iotivity_config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif
#include <signal.h>
#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif
#include "ocstack.h"
#include "experimental/logger.h"
#include "ocpayload.h"
#include "bloodpressure2.h"
#include "common.h"
#include "MySignals/bloodpressure.h"

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------

#define TAG "SERVER-BLOODPRESSURE-2"

//-----------------------------------------------------------------------------
// Typedefs
//-----------------------------------------------------------------------------

/* Structure to represent a resource */
typedef struct BLOODPRESSURE2RESOURCE{
    OCResourceHandle handle;
} BloodPressure2Resource;

//-----------------------------------------------------------------------------
// Variables
//-----------------------------------------------------------------------------

static BloodPressure2Resource BP2;

char *gBP2ResourceType= (char *)"oic.r.pulserate";
char *gBP2ResourceUri= (char *)"/myPulseRateResURI";

//-----------------------------------------------------------------------------
// Function prototype
//-----------------------------------------------------------------------------

OCRepPayload* getBP2Payload(const char* uri);

/* This method converts the payload to JSON format */
OCRepPayload* constructBP2Response (OCEntityHandlerRequest *ehRequest);

/* Following methods process the GET
 * requests
 */
OCEntityHandlerResult ProcessBP2GetRequest (OCEntityHandlerRequest *ehRequest,
                                         OCRepPayload **payload);

int createBP2ResourceEx (char *uri, BloodPressure2Resource *BP2Resource);

//-----------------------------------------------------------------------------
// Callback functions
//-----------------------------------------------------------------------------

/* Entity Handler callback functions */
OCEntityHandlerResult
BP2OCEntityHandlerCb (OCEntityHandlerFlag flag,
        OCEntityHandlerRequest *entityHandlerRequest);

//-----------------------------------------------------------------------------
// Function Implementations
//-----------------------------------------------------------------------------
OCRepPayload* getBP2Payload(const char* uri)
{
    OCRepPayload* payload = OCRepPayloadCreate();
    if(!payload)
    {
        OIC_LOG(ERROR, TAG, PCF("Failed to allocate Payload"));
        return nullptr;
    }
    size_t dimensions[MAX_REP_ARRAY_DEPTH] = { 0 };

    dimensions[0] = 1;
    char * rtStr[] = {"oic.r.pulserate"};
    OCRepPayloadSetStringArray(payload, "rt", (const char **)rtStr, dimensions);
    OCRepPayloadSetPropString(payload, "id", "user_example_id");
    OCRepPayloadSetPropInt(payload, "pulserate", getPulseRate());

    return payload;
}

OCRepPayload* constructBP2Response (OCEntityHandlerRequest *ehRequest)
{
    if(ehRequest->payload && ehRequest->payload->type != PAYLOAD_TYPE_REPRESENTATION)
    {
        OIC_LOG(ERROR, TAG, PCF("Incoming payload not a representation"));
        return nullptr;
    }

    return getBP2Payload(gBP2ResourceUri);
}

OCEntityHandlerResult ProcessBP2GetRequest (OCEntityHandlerRequest *ehRequest,
    OCRepPayload **payload)
{
    OCEntityHandlerResult ehResult;

    OCRepPayload *getResp = constructBP2Response(ehRequest);

    if(getResp)
    {
        *payload = getResp;
        ehResult = OC_EH_OK;
    }
    else
    {
        ehResult = OC_EH_ERROR;
    }

    return ehResult;
}


OCEntityHandlerResult
BP2OCEntityHandlerCb (OCEntityHandlerFlag flag,
        OCEntityHandlerRequest *entityHandlerRequest,
        void* /*callbackParam*/)
{
    OIC_LOG_V (INFO, TAG, "Inside entity handler - flags: 0x%x", flag);

    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    OCEntityHandlerResponse response = { 0, 0, OC_EH_ERROR, 0, 0, { },{ 0 }, false };
    // Validate pointer
    if (!entityHandlerRequest)
    {
        OIC_LOG (ERROR, TAG, "Invalid request pointer");
        return OC_EH_ERROR;
    }

    OCRepPayload* payload = nullptr;

    if (flag & OC_REQUEST_FLAG)
    {
        OIC_LOG (INFO, TAG, "Flag includes OC_REQUEST_FLAG");
        if (entityHandlerRequest)
        {
            ehResult = OC_EH_FORBIDDEN;
        }
    }

    OCRepPayloadDestroy(payload);
    return ehResult;
}

int createBP2Resource () {
    createBP2ResourceEx(gBP2ResourceUri, &BP2);
    return 0;
}

int createBP2ResourceEx (char *uri, BloodPressure2Resource *BP2Resource)
{
    if (!uri)
    {
        OIC_LOG(ERROR, TAG, "Resource URI cannot be NULL");
        return -1;
    }

    OCStackResult res = OCCreateResource(&(BP2Resource->handle),
            gBP2ResourceType,
            OC_RSRVD_INTERFACE_DEFAULT,
            gBP2ResourceUri,
            BP2OCEntityHandlerCb,
            NULL,
            OC_OBSERVABLE
            // OC_DISCOVERABLE | OC_OBSERVABLE
#if IS_SECURE_MODE
            | OC_SECURE
#endif
        );
    OCBindResourceInterfaceToResource(&(BP2Resource->handle), OC_RSRVD_INTERFACE_SENSOR);
    OIC_LOG_V(INFO, TAG, "Created BP2 resource with result: %s", getResult(res));

    return 0;
}
