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
#include "ocpayload.h"


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
 * @brief Structure for holding the Provisioning status and target information required to connect to the target network
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
OCEntityHandlerResult OCEntityHandlerCb(OCEntityHandlerFlag flag,
                                                OCEntityHandlerRequest *ehRequest, void *callback);
const char *getResult(OCStackResult result);
OCEntityHandlerResult ProcessGetRequest(OCEntityHandlerRequest *ehRequest,
                                               OCRepPayload** payload);
OCEntityHandlerResult ProcessPutRequest(OCEntityHandlerRequest *ehRequest,
                                               OCRepPayload** payload);
OCEntityHandlerResult ProcessPostRequest(OCEntityHandlerRequest *ehRequest,
                                                OCRepPayload** payload);
OCRepPayload* constructResponse(OCEntityHandlerRequest *ehRequest);

static int g_flag = 0;

ResourceEventCallback gNetworkInfoProvEventCb = NULL;

void RegisterResourceEventCallBack(ResourceEventCallback cb)
{
    gNetworkInfoProvEventCb = cb;
}

void GetTargetNetworkInfoFromProvResource(char *name, char *pass)
{
    if (name != NULL && pass != NULL)
    {
        sprintf(name, "%s", gProvResource.tnn);
        sprintf(pass, "%s", gProvResource.cd);
    }
}

OCStackResult CreateProvisioningResource()
{
    gProvResource.ps = 1; // need to do provisioning
    gProvResource.tnt = CT_ADAPTER_IP;
    sprintf(gProvResource.tnn, "Unknown");
    sprintf(gProvResource.cd, "Unknown");

    OCStackResult res = OCCreateResource(&gProvResource.handle, "oic.r.prov", OC_RSRVD_INTERFACE_DEFAULT,
                                                OC_RSRVD_ES_URI_PROV, OCEntityHandlerCb, NULL,
                                                OC_DISCOVERABLE | OC_OBSERVABLE);
    OC_LOG_V(INFO, ES_RH_TAG, "Created Prov resource with result: %s", getResult(res));
    return res;
}

#ifdef ESWIFI
OCStackResult CreateNetworkResource()
{
    NetworkInfo netInfo;

    if (getCurrentNetworkInfo(CT_ADAPTER_IP, &netInfo) != ES_OK)
    {
        return OC_STACK_ERROR;
    }

    if (netInfo.type != CT_ADAPTER_IP)
    {
        return OC_STACK_ERROR;
    }

    gNetResource.cnt = (int) netInfo.type;
    gNetResource.ant[0] = (int) CT_ADAPTER_IP;

    if(netInfo.ipaddr != NULL)
    {
        sprintf(gNetResource.ipaddr, "%d.%d.%d.%d", netInfo.ipaddr[0], netInfo.ipaddr[1],
                                            netInfo.ipaddr[2], netInfo.ipaddr[3]);
    }
    sprintf(gNetResource.cnn, "%s", netInfo.ssid);

    OC_LOG_V(INFO, ES_RH_TAG, "SSID: %s", gNetResource.cnn);
    OC_LOG_V(INFO, ES_RH_TAG, "IP Address: %s", gNetResource.ipaddr);

    OCStackResult res = OCCreateResource(&gNetResource.handle, "oic.r.net", OC_RSRVD_INTERFACE_DEFAULT,
            OC_RSRVD_ES_URI_NET, OCEntityHandlerCb,NULL, OC_DISCOVERABLE | OC_OBSERVABLE);

    OC_LOG_V(INFO, ES_RH_TAG, "Created Net resource with result: %s", getResult(res));

    return res;
}
#endif
OCEntityHandlerResult ProcessGetRequest(OCEntityHandlerRequest *ehRequest,
                                                OCRepPayload **payload)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    if (!ehRequest)
    {
        OC_LOG(ERROR, ES_RH_TAG, "Request is Null");
        return ehResult;
    }
    if (ehRequest->payload && ehRequest->payload->type != PAYLOAD_TYPE_REPRESENTATION)
    {
        OC_LOG(ERROR, ES_RH_TAG, "Incoming payload not a representation");
        return ehResult;
    }

    OCRepPayload *getResp = constructResponse(ehRequest);
    if (!getResp)
    {
        OC_LOG(ERROR, ES_RH_TAG, "constructResponse failed");
        return OC_EH_ERROR;
    }

    *payload = getResp;
    ehResult = OC_EH_OK;

    return ehResult;
}

OCEntityHandlerResult ProcessPutRequest(OCEntityHandlerRequest *ehRequest,
                                               OCRepPayload** payload)
{
    OC_LOG(INFO, ES_RH_TAG, "ProcessPutRequest enter");
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    if (ehRequest->payload && ehRequest->payload->type != PAYLOAD_TYPE_REPRESENTATION)
    {
        OC_LOG(ERROR, ES_RH_TAG, "Incoming payload not a representation");
        return ehResult;
    }

    OCRepPayload* input = (OCRepPayload*) (ehRequest->payload);
    if (!input)
    {
        OC_LOG(ERROR, ES_RH_TAG, "Failed to parse");
        return ehResult;
    }

    char* tnn;
    if (OCRepPayloadGetPropString(input, OC_RSRVD_ES_TNN, &tnn))
    {
        sprintf(gProvResource.tnn, "%s", tnn);
        OC_LOG(INFO, ES_RH_TAG, "got ssid");
    }

    char* cd;
    if (OCRepPayloadGetPropString(input, OC_RSRVD_ES_CD, &cd))
    {
        sprintf(gProvResource.cd, "%s", cd);
        OC_LOG(INFO, ES_RH_TAG, "got password");
    }
    gProvResource.ps = 2;
    OC_LOG_V(INFO, ES_RH_TAG, "gProvResource.ps %d", gProvResource.ps);
    g_flag = 1;

    OCRepPayload *getResp = constructResponse(ehRequest);
    if (!getResp)
    {
        OC_LOG(ERROR, ES_RH_TAG, "constructResponse failed");
        return OC_EH_ERROR;
    }

    *payload = getResp;
    ehResult = OC_EH_OK;

    return ehResult;
}

OCEntityHandlerResult ProcessPostRequest(OCEntityHandlerRequest *ehRequest,
                                                OCRepPayload** payload)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    if (!ehRequest)
    {
        OC_LOG(ERROR, ES_RH_TAG, "Request is Null");
        return ehResult;
    }
    if (ehRequest->payload && ehRequest->payload->type != PAYLOAD_TYPE_REPRESENTATION)
    {
        OC_LOG(ERROR, ES_RH_TAG, "Incoming payload not a representation");
        return ehResult;
    }

    OCRepPayload* input = (OCRepPayload*) (ehRequest->payload);
    if (!input)
    {
        OC_LOG(ERROR, ES_RH_TAG, "Failed to parse");
        return ehResult;
    }
    char* tr;
    if (OCRepPayloadGetPropString(input, OC_RSRVD_ES_TR, &tr))
    {
        // Triggering
        ehResult = OC_EH_OK;
    }
    g_flag = 1;

    return ehResult;
}

OCRepPayload* constructResponse(OCEntityHandlerRequest *ehRequest)
{
    OCRepPayload* payload = OCRepPayloadCreate();
    if (!payload)
    {
        OC_LOG(ERROR, ES_RH_TAG, "Failed to allocate Payload");
        return NULL;
    }

    if (ehRequest->resource == gProvResource.handle)
    {
        OC_LOG(INFO, ES_RH_TAG, "constructResponse prov res");
        OCRepPayloadSetUri(payload, OC_RSRVD_ES_URI_PROV);
        OCRepPayloadSetPropInt(payload, OC_RSRVD_ES_PS, gProvResource.ps);
        OCRepPayloadSetPropInt(payload, OC_RSRVD_ES_TNT, gProvResource.tnt);
        OCRepPayloadSetPropString(payload, OC_RSRVD_ES_TNN, gProvResource.tnn);
        OCRepPayloadSetPropString(payload, OC_RSRVD_ES_CD, gProvResource.cd);
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
    OCEntityHandlerResponse response = { 0 };
    OCRepPayload* payload = NULL;
    if (entityHandlerRequest && (flag & OC_REQUEST_FLAG))
    {
        if (OC_REST_GET == entityHandlerRequest->method)
        {
            OC_LOG(INFO, ES_RH_TAG, "Received GET request");
            ehRet = ProcessGetRequest(entityHandlerRequest, &payload);
        }
        else if (OC_REST_PUT == entityHandlerRequest->method)
        {
            OC_LOG(INFO, ES_RH_TAG, "Received PUT request");

            if (gProvResource.handle != NULL && entityHandlerRequest->resource == gProvResource.handle)
            {
                ehRet = ProcessPutRequest(entityHandlerRequest, &payload);
            }
            else
            {
                OC_LOG(ERROR, ES_RH_TAG, "Cannot process put");
                ehRet = OC_EH_ERROR;
            }
        }
        else if (OC_REST_POST == entityHandlerRequest->method)
        {
            // TODO: As of now, POST request will be not received.
            OC_LOG(INFO, ES_RH_TAG, "Received OC_REST_POST from client");
            //ehRet = ProcessPostRequest (entityHandlerRequest, payload, sizeof(payload) - 1);
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
                    sizeof response.sendVendorSpecificHeaderOptions);
            memset(response.resourceUri, 0, sizeof response.resourceUri);
            // Indicate that response is NOT in a persistent buffer
            response.persistentBufferFlag = 0;

            // Send the response
            if (OCDoResponse(&response) != OC_STACK_OK)
            {
                OC_LOG(ERROR, ES_RH_TAG, "Error sending response");
                ehRet = OC_EH_ERROR;
            }
        }
    }

    if (g_flag == 1)
    {
        gNetworkInfoProvEventCb(ES_RECVTRIGGEROFPROVRES);
        g_flag = 0;
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

