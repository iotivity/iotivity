//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
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

#include "resourceHandler.h"
#include "cJSON.h"

PROGMEM const char TAG[] = "resourceHandler";

ProvResource g_prov;
NetResource g_net;

OCEntityHandlerResult OCEntityHandlerCb(OCEntityHandlerFlag, OCEntityHandlerRequest *);
const char *getResult(OCStackResult result);

OCEntityHandlerResult ProcessGetRequest(OCEntityHandlerRequest *ehRequest, char *payload,
        uint16_t maxPayloadSize);
OCEntityHandlerResult ProcessPutRequest(OCEntityHandlerRequest *ehRequest, char *payload,
        uint16_t maxPayloadSize);
OCEntityHandlerResult ProcessPostRequest(OCEntityHandlerRequest *ehRequest, char *payload,
        uint16_t maxPayloadSize);
char* constructJsonResponse(OCEntityHandlerRequest *ehRequest);

int g_flag = 0;

ResourceEventCallback g_cbForResEvent = NULL;

void RegisterResourceEventCallBack(ResourceEventCallback cb)
{
    g_cbForResEvent = cb;
}

void GetTargetNetworkInfoFromProvResource(char *name, char *pass)
{
    if (name != NULL && pass != NULL)
    {
        sprintf(name, "%s", g_prov.tnn);
        sprintf(pass, "%s", g_prov.cd);
    }
}

OCStackResult CreateProvisioningResource()
{
    g_prov.ps = 1; // need to provisioning
    g_prov.tnt = ES_WIFI;
    sprintf(g_prov.tnn, "Unknown");
    sprintf(g_prov.cd, "Unknown");

    OCStackResult res = OCCreateResource(&g_prov.handle, "oic.prov", OC_RSRVD_INTERFACE_DEFAULT,
            "/oic/prov", OCEntityHandlerCb, OC_DISCOVERABLE | OC_OBSERVABLE);

    OC_LOG_V(INFO, TAG, "Created Prov resource with result: %s", getResult(res));

    return res;
}

OCStackResult CreateNetworkResource()
{
    NetworkInfo netInfo;

    if (getCurrentNetworkInfo(ES_WIFI, &netInfo) != 0)
    {
        return OC_STACK_ERROR;
    }

    if (netInfo.type != ES_WIFI)
    {
        return OC_STACK_ERROR;
    }

    g_net.cnt = (int) netInfo.type;
    g_net.ant[0] = (int) ES_WIFI;
    sprintf(g_net.ipaddr, "%d.%d.%d.%d", netInfo.ipaddr[0], netInfo.ipaddr[1], netInfo.ipaddr[2],
            netInfo.ipaddr[3]);
    sprintf(g_net.cnn, "%s", netInfo.ssid);

    OC_LOG_V(INFO, TAG, "SSID: %s", g_net.cnn);
    OC_LOG_V(INFO, TAG, "IP Address: %s", g_net.ipaddr);

    OCStackResult res = OCCreateResource(&g_net.handle, "oic.net", OC_RSRVD_INTERFACE_DEFAULT,
            "/oic/net", OCEntityHandlerCb, OC_DISCOVERABLE | OC_OBSERVABLE);
    OC_LOG_V(INFO, TAG, "Created Net resource with result: %s", getResult(res));

    return res;
}

OCEntityHandlerResult ProcessGetRequest(OCEntityHandlerRequest *ehRequest, char *payload,
        uint16_t maxPayloadSize)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;

    char *getResp = constructJsonResponse(ehRequest);

    if (!getResp)
    {
        OC_LOG(ERROR, TAG, "constructJsonResponse failed");
        return OC_EH_ERROR;
    }

    if (MAX_RESPONSE_LENGTH > strlen(getResp))
    {
        strncpy(payload, getResp, strlen(getResp));
        ehResult = OC_EH_OK;
    }
    else
    {
        OC_LOG_V(INFO, TAG, "Response buffer: %d bytes is too small", maxPayloadSize);
        ehResult = OC_EH_ERROR;
    }

    free(getResp);

    return ehResult;
}

OCEntityHandlerResult ProcessPutRequest(OCEntityHandlerRequest *ehRequest, char *payload,
        uint16_t maxPayloadSize)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;

    OC_LOG_V(INFO, TAG, "PUT Request Payload: %s", ehRequest->reqJSONPayload);

    // Get cJSON pointer to query
    cJSON *putJson = cJSON_Parse(ehRequest->reqJSONPayload);
    if (!putJson)
    {
        OC_LOG_V(ERROR, TAG, "Failed to parse JSON: %s", ehRequest->reqJSONPayload);
        return ehResult;
    }

    // Get root of JSON payload, then the 1st resource.
    cJSON* carrier = cJSON_GetObjectItem(putJson, "oic");
    if (!carrier)
    {
        OC_LOG_V(ERROR, TAG, "Failed to parse JSON: %s", ehRequest->reqJSONPayload);
        return ehResult;
    }
    carrier = cJSON_GetArrayItem(carrier, 0);
    carrier = cJSON_GetObjectItem(carrier, "rep");
    if (!carrier)
    {
        OC_LOG_V(ERROR, TAG, "Failed to parse JSON: %s", ehRequest->reqJSONPayload);
        return ehResult;
    }

    cJSON* prop = cJSON_GetObjectItem(carrier, "tnn");
    if (prop)
    {
        sprintf(g_prov.tnn, "%s", prop->valuestring);
    }

    prop = cJSON_GetObjectItem(carrier, "cd");
    if (prop)
    {
        sprintf(g_prov.cd, "%s", prop->valuestring);
    }

    cJSON_Delete(putJson);

    // TODO: Now once receiving PUT request, the enrollee start to connect to enroller.
    g_flag = 1;

    char *getResp = constructJsonResponse(ehRequest);

    if (!getResp)
    {
        OC_LOG(ERROR, TAG, "constructJsonResponse failed");
        return OC_EH_ERROR;
    }

    if (MAX_RESPONSE_LENGTH > strlen(getResp))
    {
        strncpy(payload, getResp, strlen(getResp));
        ehResult = OC_EH_OK;
    }
    else
    {
        OC_LOG_V(INFO, TAG, "Response buffer: %d bytes is too small", maxPayloadSize);
        ehResult = OC_EH_ERROR;
    }

    free(getResp);
    return ehResult;
}

OCEntityHandlerResult ProcessPostRequest(OCEntityHandlerRequest *ehRequest, char *payload,
        uint16_t maxPayloadSize)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    OC_LOG_V(INFO, TAG, "PUT Request Payload: %s", ehRequest->reqJSONPayload);

    // Get cJSON pointer to query
    cJSON *putJson = cJSON_Parse(ehRequest->reqJSONPayload);

    if (!putJson)
    {
        OC_LOG_V(ERROR, TAG, "Failed to parse JSON: %s", ehRequest->reqJSONPayload);
        return ehResult;
    }

    // Get root of JSON payload, then the 1st resource.
    cJSON* carrier = cJSON_GetObjectItem(putJson, "oic");
    if (!carrier)
    {
        OC_LOG_V(ERROR, TAG, "Failed to parse JSON: %s", ehRequest->reqJSONPayload);
        return ehResult;
    }
    carrier = cJSON_GetArrayItem(carrier, 0);
    carrier = cJSON_GetObjectItem(carrier, "rep");
    if (!carrier)
    {
        OC_LOG_V(ERROR, TAG, "Failed to parse JSON: %s", ehRequest->reqJSONPayload);
        return ehResult;
    }

    cJSON* prop = cJSON_GetObjectItem(carrier, "tr");
    if (prop)
    {
        // Triggering
        ehResult = OC_EH_OK;
    }

    cJSON_Delete(putJson);

    g_flag = 1;

    return ehResult;
}

char* constructJsonResponse(OCEntityHandlerRequest *ehRequest)
{
    cJSON *json = cJSON_CreateObject();
    cJSON *format;
    char *jsonResponse;
    char *JsonResp;
    char temp_resp[256];

    if (g_prov.handle != NULL && ehRequest->resource == g_prov.handle)
    {
        char *uri = (char *) "/oic/prov";

        cJSON_AddStringToObject(json, "href", uri);
        cJSON_AddItemToObject(json, "rep", format = cJSON_CreateObject());
        cJSON_AddNumberToObject(format, "ps", g_prov.ps);
        cJSON_AddNumberToObject(format, "tnt", g_prov.tnt);
        cJSON_AddStringToObject(format, "tnn", g_prov.tnn);
        cJSON_AddStringToObject(format, "cd", g_prov.cd);

        jsonResponse = cJSON_Print(json);
        cJSON_Delete(json);
    }
    else if (g_net.handle != NULL && ehRequest->requestHandle == g_net.handle)
    {
        char *uri = (char *) "/oic/net";

        cJSON_AddStringToObject(json, "href", uri);
        cJSON_AddItemToObject(json, "rep", format = cJSON_CreateObject());
        cJSON_AddNumberToObject(format, "ant", g_net.ant[0]);

        jsonResponse = cJSON_Print(json);
        cJSON_Delete(json);
    }
    else
    {
        return jsonResponse;
    }

    OC_LOG_V(INFO, TAG, "Constructed Response: %s", jsonResponse);

    return jsonResponse;
}

// This is the entity handler for the registered resource.
// This is invoked by OCStack whenever it recevies a request for this resource.

OCEntityHandlerResult OCEntityHandlerCb(OCEntityHandlerFlag flag,
        OCEntityHandlerRequest* entityHandlerRequest)
{
    OCEntityHandlerResult ehRet = OC_EH_OK;
    OCEntityHandlerResponse response =
    { 0 };
    char payload[MAX_RESPONSE_LENGTH] =
    { 0 };

    if (entityHandlerRequest && (flag & OC_REQUEST_FLAG))
    {
        if (OC_REST_GET == entityHandlerRequest->method)
        {
            OC_LOG_V(INFO, TAG, "Received GET request");
            ehRet = ProcessGetRequest(entityHandlerRequest, payload, sizeof(payload) - 1);
        }
        else if (OC_REST_PUT == entityHandlerRequest->method)
        {
            OC_LOG_V(INFO, TAG, "Received PUT request");

            if (g_prov.handle != NULL && entityHandlerRequest->resource == g_prov.handle)
            {
                ehRet = ProcessPutRequest(entityHandlerRequest, payload, sizeof(payload) - 1);
            }
            else
            {
                ehRet = OC_EH_ERROR;
            }
        }
        else if (OC_REST_POST == entityHandlerRequest->method)
        {
            // TODO: As of now, POST request will be not received.
            OC_LOG(INFO, TAG, "Received OC_REST_POST from client");
            //ehRet = ProcessPostRequest (entityHandlerRequest, payload, sizeof(payload) - 1);
        }

        if (ehRet == OC_EH_OK)
        {
            // Format the response.  Note this requires some info about the request
            response.requestHandle = entityHandlerRequest->requestHandle;
            response.resourceHandle = entityHandlerRequest->resource;
            response.ehResult = ehRet;
            response.payload = payload;
            response.payloadSize = strlen(payload);
            response.numSendVendorSpecificHeaderOptions = 0;
            memset(response.sendVendorSpecificHeaderOptions, 0,
                    sizeof response.sendVendorSpecificHeaderOptions);
            memset(response.resourceUri, 0, sizeof response.resourceUri);
            // Indicate that response is NOT in a persistent buffer
            response.persistentBufferFlag = 0;

            // Send the response
            if (OCDoResponse(&response) != OC_STACK_OK)
            {
                OC_LOG(ERROR, TAG, "Error sending response");
                ehRet = OC_EH_ERROR;
            }
        }
    }

    if (g_flag == 1)
    {
        g_cbForResEvent(ES_RECVTRIGGEROFPROVRES);
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

