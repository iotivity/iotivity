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

#include "resourcehandler.h"

#include <stdio.h>

#include "ocpayload.h"
#include "oic_string.h"

/**
 * @var ES_RH_TAG
 * @brief Logging tag for module name.
 */
#define ES_RH_TAG "ES_RH"
//-----------------------------------------------------------------------------
// Private variables
//-----------------------------------------------------------------------------

/**
 * @var gProvResource
 * @brief Structure for holding the Provisioning status and target information required to
 * connect to the target network
 */
static ProvResource gProvResource;

/**
 * @var gNetResource
 * @brief Structure forr holding the Provisioning status of network information
 */
static NetResource gNetResource;

//-----------------------------------------------------------------------------
// Private internal function prototypes
//-----------------------------------------------------------------------------
OCEntityHandlerResult OCEntityHandlerCb(OCEntityHandlerFlag flag, OCEntityHandlerRequest *ehRequest,
        void *callback);
const char *getResult(OCStackResult result);
OCEntityHandlerResult ProcessGetRequest(OCEntityHandlerRequest *ehRequest, OCRepPayload** payload);
OCEntityHandlerResult ProcessPutRequest(OCEntityHandlerRequest *ehRequest, OCRepPayload** payload);
OCEntityHandlerResult ProcessPostRequest(OCEntityHandlerRequest *ehRequest, OCRepPayload** payload);
OCRepPayload* constructResponse(OCEntityHandlerRequest *ehRequest);

ESEnrolleeResourceEventCallback gNetworkInfoProvEventCb = NULL;

void RegisterResourceEventCallBack(ESEnrolleeResourceEventCallback cb)
{
    gNetworkInfoProvEventCb = cb;
}

void UnRegisterResourceEventCallBack()
{
    if (gNetworkInfoProvEventCb)
    {
        gNetworkInfoProvEventCb = NULL;
    }
}

void GetTargetNetworkInfoFromProvResource(char *name, char *pass)
{
    if (name != NULL && pass != NULL)
    {
        OICStrcpy(name, MAXSSIDLEN, gProvResource.tnn);
        OICStrcpy(pass, MAXNETCREDLEN, gProvResource.cd);
    }
}

OCStackResult CreateProvisioningResource(bool isSecured)
{
    gProvResource.ps = ES_PS_NEED_PROVISIONING;
    gProvResource.tr = ES_PS_TRIGGER_INIT_VALUE;

    gProvResource.tnt = CT_ADAPTER_IP;
    OICStrcpy(gProvResource.tnn, sizeof(gProvResource.tnn), "Unknown");
    OICStrcpy(gProvResource.cd, sizeof(gProvResource.cd), "Unknown");

    OCStackResult res = OC_STACK_ERROR;
    if (isSecured)
    {
        res = OCCreateResource(&gProvResource.handle, OC_RSRVD_ES_PROV_RES_TYPE,
        OC_RSRVD_INTERFACE_DEFAULT,
        OC_RSRVD_ES_URI_PROV, OCEntityHandlerCb,
        NULL, OC_DISCOVERABLE | OC_OBSERVABLE | OC_SECURE);
    }
    else
    {
        res = OCCreateResource(&gProvResource.handle, OC_RSRVD_ES_PROV_RES_TYPE,
        OC_RSRVD_INTERFACE_DEFAULT,
        OC_RSRVD_ES_URI_PROV, OCEntityHandlerCb,
        NULL, OC_DISCOVERABLE | OC_OBSERVABLE);
    }

    OIC_LOG_V(INFO, ES_RH_TAG, "Created Prov resource with result: %s", getResult(res));
    return res;
}

OCStackResult DeleteProvisioningResource()
{
    OCStackResult res = OCDeleteResource(gProvResource.handle);
    if (res != OC_STACK_OK)
    {
        OIC_LOG_V(INFO, ES_RH_TAG, "Deleting Prov resource error with result: %s", getResult(res));
    }

    return res;
}

OCEntityHandlerResult ProcessGetRequest(OCEntityHandlerRequest *ehRequest, OCRepPayload **payload)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    if (!ehRequest)
    {
        OIC_LOG(ERROR, ES_RH_TAG, "Request is Null");
        return ehResult;
    }
    if (ehRequest->payload && ehRequest->payload->type != PAYLOAD_TYPE_REPRESENTATION)
    {
        OIC_LOG(ERROR, ES_RH_TAG, "Incoming payload not a representation");
        return ehResult;
    }

    OCRepPayload *getResp = constructResponse(ehRequest);
    if (!getResp)
    {
        OIC_LOG(ERROR, ES_RH_TAG, "constructResponse failed");
        return OC_EH_ERROR;
    }

    *payload = getResp;
    ehResult = OC_EH_OK;

    return ehResult;
}

OCEntityHandlerResult ProcessPostRequest(OCEntityHandlerRequest *ehRequest, OCRepPayload** payload)
{
    OIC_LOG(INFO, ES_RH_TAG, "ProcessPostRequest enter");
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    if (ehRequest->payload && ehRequest->payload->type != PAYLOAD_TYPE_REPRESENTATION)
    {
        OIC_LOG(ERROR, ES_RH_TAG, "Incoming payload not a representation");
        return ehResult;
    }

    OCRepPayload* input = (OCRepPayload*) (ehRequest->payload);
    if (!input)
    {
        OIC_LOG(ERROR, ES_RH_TAG, "Failed to parse");
        return ehResult;
    }

    int64_t tr;
    if (OCRepPayloadGetPropInt(input, OC_RSRVD_ES_TR, &tr))
    {
        // Triggering
        gProvResource.tr = tr;
    }

    //ES_PS_PROVISIONING_COMPLETED state indicates that already provisioning is completed.
    // A new request for provisioning means overriding existing network provisioning information.
    if (gProvResource.ps == ES_PS_PROVISIONING_COMPLETED && tr == ES_PS_TRIGGER_CONNECTION)
    {
        OIC_LOG(DEBUG, ES_RH_TAG, "Provisioning already completed."
                "Tiggering the network connection");

        if (gNetworkInfoProvEventCb)
        {
            gNetworkInfoProvEventCb(ES_RECVTRIGGEROFPROVRES);
            ehResult = OC_EH_OK;
        }
        else
        {
            OIC_LOG(ERROR, ES_RH_TAG, "gNetworkInfoProvEventCb is NULL."
                    "Network handler not registered. Failed to connect to the network");
            ehResult = OC_EH_ERROR;
        }

        return ehResult;
    }
    else if (gProvResource.ps == ES_PS_PROVISIONING_COMPLETED)
    {
        OIC_LOG(DEBUG, ES_RH_TAG, "Provisioning already completed. "
                "This a request to override the existing the network provisioning information");
    }
    else
    {
        OIC_LOG(DEBUG, ES_RH_TAG, "Provisioning the network information to the Enrollee.");
    }

    OICStrcpy(gProvResource.tnn, sizeof(gProvResource.tnn), "");
    OICStrcpy(gProvResource.cd, sizeof(gProvResource.cd), "");

    char* tnn;
    if (OCRepPayloadGetPropString(input, OC_RSRVD_ES_TNN, &tnn))
    {
        OICStrcpy(gProvResource.tnn, sizeof(gProvResource.tnn), tnn);
        OIC_LOG(INFO, ES_RH_TAG, "got ssid");
    }

    OIC_LOG_V(INFO, ES_RH_TAG, "gProvResource.tnn %s", gProvResource.tnn);

    char* cd;
    if (OCRepPayloadGetPropString(input, OC_RSRVD_ES_CD, &cd))
    {
        OICStrcpy(gProvResource.cd, sizeof(gProvResource.cd), cd);
        OIC_LOG(INFO, ES_RH_TAG, "got password");
    }OIC_LOG_V(INFO, ES_RH_TAG, "gProvResource.cd %s", gProvResource.cd);

    gProvResource.ps = ES_PS_PROVISIONING_COMPLETED;

    OIC_LOG_V(INFO, ES_RH_TAG, "gProvResource.ps %lld", gProvResource.ps);

    OCRepPayload *getResp = constructResponse(ehRequest);
    if (!getResp)
    {
        OIC_LOG(ERROR, ES_RH_TAG, "constructResponse failed");
        return OC_EH_ERROR;
    }

    *payload = getResp;
    ehResult = OC_EH_OK;

    return ehResult;
}

OCEntityHandlerResult ProcessPutRequest(OCEntityHandlerRequest * ehRequest,
        OCRepPayload** payload)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;

    return ehResult;
}

OCRepPayload* constructResponse(OCEntityHandlerRequest *ehRequest)
{
    OCRepPayload* payload = OCRepPayloadCreate();
    if (!payload)
    {
        OIC_LOG(ERROR, ES_RH_TAG, "Failed to allocate Payload");
        return NULL;
    }

    if (ehRequest->resource == gProvResource.handle)
    {
        OIC_LOG(INFO, ES_RH_TAG, "constructResponse prov res");
        OCRepPayloadSetUri(payload, OC_RSRVD_ES_URI_PROV);
        OCRepPayloadSetPropInt(payload, OC_RSRVD_ES_PS, gProvResource.ps);
        OCRepPayloadSetPropInt(payload, OC_RSRVD_ES_TNT, gProvResource.tnt);
    }
    else if (ehRequest->requestHandle == gNetResource.handle)
    {

        OCRepPayloadSetUri(payload, OC_RSRVD_ES_URI_NET);
        OCRepPayloadSetPropInt(payload, "ant", gNetResource.ant[0]);
    }
    return payload;
}

/**
 * This is the entity handler for the registered resource.
 * This is invoked by OCStack whenever it recevies a request for this resource.
 */
OCEntityHandlerResult OCEntityHandlerCb(OCEntityHandlerFlag flag,
        OCEntityHandlerRequest* entityHandlerRequest, void *callback)
{
    (void) callback;
    OCEntityHandlerResult ehRet = OC_EH_OK;
    OCEntityHandlerResponse response =
    { 0, 0, OC_EH_ERROR, 0, 0,
    { },
    { 0 }, false };
    OCRepPayload* payload = NULL;

    if (entityHandlerRequest && (flag & OC_REQUEST_FLAG))
    {
        if (OC_REST_GET == entityHandlerRequest->method)
        {
            OIC_LOG(INFO, ES_RH_TAG, "Received GET request");
            ehRet = ProcessGetRequest(entityHandlerRequest, &payload);
        }
        else if (OC_REST_PUT == entityHandlerRequest->method)
        {
            OIC_LOG(INFO, ES_RH_TAG, "Received PUT request");

            //PUT request will be handled in the internal implementation
            if (gProvResource.handle != NULL
                    && entityHandlerRequest->resource == gProvResource.handle)
            {
                ehRet = ProcessPutRequest(entityHandlerRequest, &payload);
            }
            else
            {
                OIC_LOG(ERROR, ES_RH_TAG, "Cannot process put");
                ehRet = OC_EH_ERROR;
            }
        }
        else if (OC_REST_POST == entityHandlerRequest->method)
        {
            OIC_LOG(INFO, ES_RH_TAG, "Received OC_REST_POST from client");
            if (gProvResource.handle != NULL
                    && entityHandlerRequest->resource == gProvResource.handle)
            {
                ehRet = ProcessPostRequest(entityHandlerRequest, &payload);
            }
            else
            {
                OIC_LOG(ERROR, ES_RH_TAG, "Cannot process put");
                ehRet = OC_EH_ERROR;
            }
        }

        if (ehRet == OC_EH_OK)
        {
            // Format the response.  Note this requires some info about the request
            response.requestHandle = entityHandlerRequest->requestHandle;
            response.resourceHandle = entityHandlerRequest->resource;
            response.ehResult = ehRet;
            //response uses OCPaylod while all get,put methodes use OCRepPayload
            response.payload = (OCPayload*) (payload);
            response.numSendVendorSpecificHeaderOptions = 0;
            memset(response.sendVendorSpecificHeaderOptions, 0,
                    sizeof(response.sendVendorSpecificHeaderOptions));
            memset(response.resourceUri, 0, sizeof(response.resourceUri));
            // Indicate that response is NOT in a persistent buffer
            response.persistentBufferFlag = 0;

            // Send the response
            if (OCDoResponse(&response) != OC_STACK_OK)
            {
                OIC_LOG(ERROR, ES_RH_TAG, "Error sending response");
                ehRet = OC_EH_ERROR;
            }
        }
    }

    return ehRet;
}

const char *getResult(OCStackResult result)
{
    switch (result)
    {
        case OC_STACK_OK:
            return "OC_STACK_OK";
        case OC_STACK_INVALID_URI:
            return "OC_STACK_INVALID_URI";
        case OC_STACK_INVALID_QUERY:
            return "OC_STACK_INVALID_QUERY";
        case OC_STACK_INVALID_IP:
            return "OC_STACK_INVALID_IP";
        case OC_STACK_INVALID_PORT:
            return "OC_STACK_INVALID_PORT";
        case OC_STACK_INVALID_CALLBACK:
            return "OC_STACK_INVALID_CALLBACK";
        case OC_STACK_INVALID_METHOD:
            return "OC_STACK_INVALID_METHOD";
        case OC_STACK_NO_MEMORY:
            return "OC_STACK_NO_MEMORY";
        case OC_STACK_COMM_ERROR:
            return "OC_STACK_COMM_ERROR";
        case OC_STACK_INVALID_PARAM:
            return "OC_STACK_INVALID_PARAM";
        case OC_STACK_NOTIMPL:
            return "OC_STACK_NOTIMPL";
        case OC_STACK_NO_RESOURCE:
            return "OC_STACK_NO_RESOURCE";
        case OC_STACK_RESOURCE_ERROR:
            return "OC_STACK_RESOURCE_ERROR";
        case OC_STACK_SLOW_RESOURCE:
            return "OC_STACK_SLOW_RESOURCE";
        case OC_STACK_NO_OBSERVERS:
            return "OC_STACK_NO_OBSERVERS";
        case OC_STACK_ERROR:
            return "OC_STACK_ERROR";
        default:
            return "UNKNOWN";
    }
}

