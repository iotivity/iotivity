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

///
/// This sample provides the way to create cloud sample
///
#include <memory>
#include <iostream>
#include <stdexcept>
#include <condition_variable>
#include <map>
#include <vector>
#include <string>
#include <pthread.h>
#include <unistd.h>

#include "ocstack.h"
#include "ocpayload.h"
#include "oicresourcedirectory.h"

#define DEFAULT_CONTEXT_VALUE 0x99
#define DEFAULT_AUTH_SIGNUP "/oic/account"
#define DEFAULT_AUTH_SESSION "/oic/account/session"
#define DEFAULT_AUTH_REFRESH "/oic/account/tokenrefresh"


OCStackResult OCCloudSignup(const char *host, const char *deviceId,
                            const char *authprovider,
                            const char *authcode, OCClientResponseHandler response)
{
    char    targetUri[MAX_URI_LENGTH * 2] = { 0, };
    snprintf(targetUri, MAX_URI_LENGTH * 2, "%s%s", host, DEFAULT_AUTH_SIGNUP);

    OCCallbackData cbData;
    memset(&cbData, 0, sizeof(OCCallbackData));
    cbData.cb = response;
    cbData.cd = NULL;
    cbData.context = (void *)DEFAULT_CONTEXT_VALUE;

    OCRepPayload *registerPayload = OCRepPayloadCreate();
    if (!registerPayload)
    {
        goto no_memory;
    }

    OCRepPayloadSetPropString(registerPayload, "di", deviceId);
    OCRepPayloadSetPropString(registerPayload, "authprovider", authprovider);
    OCRepPayloadSetPropString(registerPayload, "authcode", authcode);

    return OCDoResource(NULL, OC_REST_POST, targetUri, NULL, (OCPayload *)registerPayload,
                        CT_ADAPTER_TCP, OC_LOW_QOS, &cbData, NULL, 0);

no_memory:
    OCRepPayloadDestroy(registerPayload);
    return OC_STACK_NO_MEMORY;
}

OCStackResult OCCloudSession(const char *host, const char *query, const char *uId,
                             const char *deviceId,
                             const char *accesstoken,
                             bool isLogin, OCClientResponseHandler response)
{
    char    targetUri[MAX_URI_LENGTH * 2] = { 0, };
    snprintf(targetUri, MAX_URI_LENGTH * 2, "%s%s", host, query);

    OCCallbackData cbData;
    memset(&cbData, 0, sizeof(OCCallbackData));
    cbData.cb = response;
    cbData.cd = NULL;
    cbData.context = (void *)DEFAULT_CONTEXT_VALUE;

    OCRepPayload *loginoutPayload = OCRepPayloadCreate();
    if (!loginoutPayload)
    {
        goto no_memory;
    }

    if (uId != NULL)
    {
        OCRepPayloadSetPropString(loginoutPayload, "uid", uId);
    }

    if (deviceId != NULL)
    {
        OCRepPayloadSetPropString(loginoutPayload, "di", deviceId);
    }

    if (accesstoken != NULL)
    {
        OCRepPayloadSetPropString(loginoutPayload, "accesstoken", accesstoken);
    }
    OCRepPayloadSetPropBool(loginoutPayload, "login", isLogin);

    return OCDoResource(NULL, OC_REST_POST, targetUri, NULL, (OCPayload *)loginoutPayload,
                        CT_ADAPTER_TCP, OC_LOW_QOS, &cbData, NULL, 0);

no_memory:
    OCRepPayloadDestroy(loginoutPayload);
    return OC_STACK_NO_MEMORY;
}

//Client should call refresh before expiresin or when receive 4.01 during sign-in
OCStackResult OCCloudRefresh(const char *host, const char *query, const char *uId,
                             const char *deviceId, const char *refreshtoken, OCClientResponseHandler response)
{
    char    targetUri[MAX_URI_LENGTH * 2] = { 0, };
    snprintf(targetUri, MAX_URI_LENGTH * 2, "%s%s", host, query);

    OCCallbackData cbData;
    memset(&cbData, 0, sizeof(OCCallbackData));
    cbData.cb = response;
    cbData.cd = NULL;
    cbData.context = (void *)DEFAULT_CONTEXT_VALUE;

    OCRepPayload *refreshPayload = OCRepPayloadCreate();
    if (!refreshPayload)
    {
        goto no_memory;
    }

    OCRepPayloadSetPropString(refreshPayload, "uid", uId);
    OCRepPayloadSetPropString(refreshPayload, "di", deviceId);
    OCRepPayloadSetPropString(refreshPayload, "granttype", "refresh_token");
    OCRepPayloadSetPropString(refreshPayload, "refreshtoken", refreshtoken);

    return OCDoResource(NULL, OC_REST_POST, targetUri, NULL, (OCPayload *)refreshPayload,
                        CT_ADAPTER_TCP, OC_LOW_QOS, &cbData, NULL, 0);

no_memory:
    OCRepPayloadDestroy(refreshPayload);
    return OC_STACK_NO_MEMORY;
}

OCStackResult OCCloudLogin(const char *host, const char *uId, const char *deviceId,
                           const char *accesstoken, OCClientResponseHandler response)
{
    return OCCloudSession(host, DEFAULT_AUTH_SESSION, uId, deviceId, accesstoken, true, response);
}

OCStackResult OCCloudLogout(const char *host, OCClientResponseHandler response)
{
    return OCCloudSession(host, DEFAULT_AUTH_SESSION, NULL, NULL, NULL, false, response);
}

////////////////////////////////////////Device Sample

#define SAMPLE_MAX_NUM_POST_INSTANCE  1
typedef struct LIGHTRESOURCE
{
    OCResourceHandle handle;
    bool state;
    int power;
} LightResource;
static LightResource gLightInstance[SAMPLE_MAX_NUM_POST_INSTANCE];


OCRepPayload *responsePayload(int64_t power, bool state)
{
    OCRepPayload *payload = OCRepPayloadCreate();
    if (!payload)
    {
        std::cout << "Failed to allocate Payload" << std::endl;
        return nullptr;
    }

    OCRepPayloadSetPropBool(payload, "state", state);
    OCRepPayloadSetPropInt(payload, "power", power);

    return payload;
}

OCRepPayload *constructResponse(OCEntityHandlerRequest *ehRequest)
{
    if (ehRequest->payload && ehRequest->payload->type != PAYLOAD_TYPE_REPRESENTATION)
    {
        std::cout << "Incoming payload not a representation" << std::endl;
        return nullptr;
    }

    LightResource *currLightResource = NULL;

    if (ehRequest->resource == gLightInstance[0].handle)
    {
        currLightResource = &gLightInstance[0];
    }
    else if (ehRequest->resource == gLightInstance[1].handle)
    {
        currLightResource = &gLightInstance[1];
    }

    if (OC_REST_PUT == ehRequest->method)
    {
        // Get pointer to query
        int64_t pow;
        OCRepPayload *input = reinterpret_cast<OCRepPayload *>(ehRequest->payload);

        if (OCRepPayloadGetPropInt(input, "power", &pow))
        {
            currLightResource->power = pow;
        }

        bool state;
        if (OCRepPayloadGetPropBool(input, "state", &state))
        {
            currLightResource->state = state;
        }
    }

    return responsePayload(currLightResource->power, currLightResource->state);
}

OCEntityHandlerResult ProcessGetRequest(OCEntityHandlerRequest *ehRequest,
                                        OCRepPayload **payload)
{
    OCRepPayload *getResp = constructResponse(ehRequest);
    if (!getResp)
    {
        std::cout << "constructResponse failed" << std::endl;
        return OC_EH_ERROR;
    }

    *payload = getResp;

    return OC_EH_OK;
}

OCEntityHandlerResult ProcessPutRequest(OCEntityHandlerRequest *ehRequest,
                                        OCRepPayload **payload)
{
    OCEntityHandlerResult ehResult;
    OCRepPayload *putResp = constructResponse(ehRequest);

    if (!putResp)
    {
        std::cout << "Failed to construct Json response" << std::endl;
        return OC_EH_ERROR;
    }

    *payload = putResp;
    ehResult = OC_EH_OK;

    return ehResult;
}

#define SAMPLE_MAX_NUM_OBSERVATIONS  2
static bool observeThreadStarted = false;
int gLightUnderObservation = 0;
pthread_t threadId_observe;
typedef struct
{
    OCObservationId observationId;
    bool valid;
    OCResourceHandle resourceHandle;
} Observers;
Observers interestedObservers[SAMPLE_MAX_NUM_OBSERVATIONS];

void *ChangeLightRepresentation(void *param)
{
    (void)param;
    OCStackResult result = OC_STACK_ERROR;

    while (true)
    {
        sleep(3);
        gLightInstance[0].power += 1;
        gLightInstance[1].power += 3;

        if (gLightUnderObservation)
        {
            std::cout << " =====> Notifying stack of new power level " << gLightInstance[0].power << std::endl;
            std::cout << " =====> Notifying stack of new power level " << gLightInstance[1].power << std::endl;
            // Notifying all observers
            result = OCNotifyAllObservers(gLightInstance[0].handle, OC_NA_QOS);
            result = OCNotifyAllObservers(gLightInstance[1].handle, OC_NA_QOS);

            std::cout << " =====> Notifying result " << result << std::endl;
        }
    }
    return NULL;
}

void ProcessObserveRegister(OCEntityHandlerRequest *ehRequest)
{
    std::cout << "Received observation registration request with observation Id " <<
              ehRequest->obsInfo.obsId << std::endl;

    if (!observeThreadStarted)
    {
        pthread_create(&threadId_observe, NULL, ChangeLightRepresentation, (void *)NULL);
        observeThreadStarted = 1;
    }
    for (uint8_t i = 0; i < SAMPLE_MAX_NUM_OBSERVATIONS; i++)
    {
        if (interestedObservers[i].valid == false)
        {
            interestedObservers[i].observationId = ehRequest->obsInfo.obsId;
            interestedObservers[i].valid = true;
            gLightUnderObservation = 1;
            break;
        }
    }
}

void ProcessObserveDeregister(OCEntityHandlerRequest *ehRequest)
{
    bool clientStillObserving = false;

    std::cout << "Received observation deregistration request for observation Id " <<
              ehRequest->obsInfo.obsId << std::endl;
    for (uint8_t i = 0; i < SAMPLE_MAX_NUM_OBSERVATIONS; i++)
    {
        if (interestedObservers[i].observationId == ehRequest->obsInfo.obsId)
        {
            interestedObservers[i].valid = false;
        }
        if (interestedObservers[i].valid == true)
        {
            // Even if there is one single client observing we continue notifying entity handler
            clientStillObserving = true;
        }
    }
    if (clientStillObserving == false)
        gLightUnderObservation = 0;
}

OCEntityHandlerResult
OCEntityHandlerCb(OCEntityHandlerFlag flag,
                  OCEntityHandlerRequest *entityHandlerRequest, void * /*callback*/)
{
    OCEntityHandlerResult ehResult = OC_EH_OK;
    OCEntityHandlerResponse response = { 0, 0, OC_EH_ERROR, 0, 0, {}, { 0 }, false };

    // Validate pointer
    if (!entityHandlerRequest)
    {
        std::cout << "Invalid request pointer" << std::endl;
        return OC_EH_ERROR;
    }

    // Initialize certain response fields
    response.numSendVendorSpecificHeaderOptions = 0;
    memset(response.sendVendorSpecificHeaderOptions,
           0, sizeof response.sendVendorSpecificHeaderOptions);
    memset(response.resourceUri, 0, sizeof response.resourceUri);
    OCRepPayload *payload = nullptr;

    if (flag & OC_REQUEST_FLAG)
    {
        std::cout << "Flag includes OC_REQUEST_FLAG" << std::endl;

        if (OC_REST_GET == entityHandlerRequest->method)
        {
            std::cout << "Received OC_REST_GET from client" << std::endl;
            ehResult = ProcessGetRequest(entityHandlerRequest, &payload);
        }
        else if (OC_REST_PUT == entityHandlerRequest->method)
        {
            std::cout << "Received OC_REST_PUT from client" << std::endl;
            ehResult = ProcessPutRequest(entityHandlerRequest, &payload);
        }
        else
        {
            std::cout << "Received unsupported method %d from client " << entityHandlerRequest->method <<
                      std::endl;
            ehResult = OC_EH_ERROR;
        }
        // If the result isn't an error or forbidden, send response
        if (!((ehResult == OC_EH_ERROR) || (ehResult == OC_EH_FORBIDDEN)))
        {
            // Format the response.  Note this requires some info about the request
            response.requestHandle = entityHandlerRequest->requestHandle;
            response.resourceHandle = entityHandlerRequest->resource;
            response.ehResult = ehResult;
            response.payload = reinterpret_cast<OCPayload *>(payload);
            // Indicate that response is NOT in a persistent buffer
            response.persistentBufferFlag = 0;

            // Send the response
            if (OCDoResponse(&response) != OC_STACK_OK)
            {
                std::cout << "Error sending response" << std::endl;
                ehResult = OC_EH_ERROR;
            }
        }
    }

    if (flag & OC_OBSERVE_FLAG)
    {
        std::cout << "Flag includes OC_OBSERVE_FLAG" << std::endl;
        if (OC_OBSERVE_REGISTER == entityHandlerRequest->obsInfo.action)
        {
            std::cout << "Received OC_OBSERVE_REGISTER from client" << std::endl;
            ProcessObserveRegister(entityHandlerRequest);
        }
        else if (OC_OBSERVE_DEREGISTER == entityHandlerRequest->obsInfo.action)
        {
            std::cout << "Received OC_OBSERVE_DEREGISTER from client" << std::endl;
            ProcessObserveDeregister(entityHandlerRequest);
        }
    }

    OCPayloadDestroy(response.payload);
    return ehResult;
}

int createLightResource(char *uri, LightResource *lightResource)
{
    if (!uri)
    {
        std::cout << "Resource URI cannot be NULL" << std::endl;
        return -1;
    }

    lightResource->state = false;
    lightResource->power = 0;
    OCStackResult res = OCCreateResource(&(lightResource->handle),
                                         "core.light",
                                         "oc.mi.def",
                                         uri,
                                         OCEntityHandlerCb,
                                         NULL,
                                         OC_DISCOVERABLE | OC_OBSERVABLE);
    std::cout << "Created Light resource with result:" << res << std::endl;

    return res;
}

OCStackApplicationResult handlePublishCB(void *ctx,
        OCDoHandle /*handle*/,
        OCClientResponse *clientResponse)
{
    if (ctx != (void *)DEFAULT_CONTEXT_VALUE)
    {
        std::cout << "Invalid Publish callback received" << std::endl;
    }

    std::cout << "Publish resource response received, code: " << clientResponse->result << std::endl;

    return OC_STACK_KEEP_TRANSACTION;
}

void PublishResources(std::string host)
{
    std::cout << "Publishing resources..." << std::endl;

    if (createLightResource((char *)"/a/light/0", &gLightInstance[0]) != 0)
    {
        std::cout << "Unable to create sample resource" << std::endl;
    }

    OCResourceHandle    resourceHandles[1] = { gLightInstance[0].handle,
                                             };
    OCCallbackData cbData;
    cbData.cb = handlePublishCB;
    cbData.context = (void *)DEFAULT_CONTEXT_VALUE;
    cbData.cd = NULL;

    std::cout << "Publish default resources" << std::endl;

    OCDeviceInfo        devInfoRoomLight;
    OCStringLL          deviceType;

    deviceType.value = "oic.d.light";
    deviceType.next = NULL;
    devInfoRoomLight.deviceName = "Living Room Light";
    devInfoRoomLight.types = &deviceType;
    devInfoRoomLight.specVersion = NULL;
    devInfoRoomLight.dataModelVersions = NULL;

    OCStackResult res = OCSetDeviceInfo(devInfoRoomLight);

    if (res != OC_STACK_OK)
    {
        std::cout << "Setting device info failed" << std::endl;
    }

    res = OCRDPublish(host.c_str(), CT_ADAPTER_TCP, NULL, 0, &cbData,
                      OC_LOW_QOS);
    if (res != OC_STACK_OK)
    {
        std::cout << "Unable to publish default resources to cloud" << std::endl;
    }

    std::cout << "Publish user resources" << std::endl;

    res = OCRDPublish(host.c_str(), CT_ADAPTER_TCP, resourceHandles, 1, &cbData,
                      OC_LOW_QOS);
    if (res != OC_STACK_OK)
    {
        std::cout << "Unable to publish user resources to cloud" << std::endl;
    }
}

/////////////////////////////////////////////Common sample
void printRepresentation(OCRepPayloadValue *value)
{
    while (value)
    {
        std::cout << "Key: " << value->name;
        switch (value->type)
        {
            case OCREP_PROP_NULL:
                std::cout << " Value: None" << std::endl;
                break;
            case OCREP_PROP_INT:
                std::cout << " Value: " << value->i << std::endl;
                break;
            case OCREP_PROP_DOUBLE:
                std::cout << " Value: " << value->d << std::endl;
                break;
            case OCREP_PROP_BOOL:
                std::cout << " Value: " << value->b << std::endl;
                break;
            case OCREP_PROP_STRING:
                std::cout << " Value: " << value->str << std::endl;
                break;
            case OCREP_PROP_BYTE_STRING:
                std::cout << " Value: Byte String" << std::endl;
                break;
            case OCREP_PROP_OBJECT:
                std::cout << " Value: Object" << std::endl;
                break;
            case OCREP_PROP_ARRAY:
                std::cout << " Value: Array" << std::endl;
                break;
        }
        value = value->next;
    }
}

std::string g_host = "coap+tcp://";

OCStackApplicationResult handleLoginoutCB(void *ctx,
        OCDoHandle /*handle*/,
        OCClientResponse *clientResponse)
{
    if (ctx != (void *)DEFAULT_CONTEXT_VALUE)
    {
        std::cout << "Invalid Login/out callback received" << std::endl;
    }

    std::cout << "Login/out response received code: " << clientResponse->result << std::endl;

    if (clientResponse->payload != NULL &&
        clientResponse->payload->type == PAYLOAD_TYPE_REPRESENTATION)
    {
        std::cout << "PAYLOAD_TYPE_REPRESENTATION received" << std::endl;

        OCRepPayloadValue *val = ((OCRepPayload *)clientResponse->payload)->values;

        printRepresentation(val);
    }

    if (clientResponse->result < 5)
    {
        PublishResources(g_host);
    }

    return OC_STACK_KEEP_TRANSACTION;
}

OCStackApplicationResult handleRegisterCB(void *ctx,
        OCDoHandle /*handle*/,
        OCClientResponse *clientResponse)
{
    if (ctx != (void *)DEFAULT_CONTEXT_VALUE)
    {
        std::cout << "Invalid Register callback received" << std::endl;
    }

    std::cout << "Register response received code: " << clientResponse->result << std::endl;

    if (clientResponse->payload != NULL &&
        clientResponse->payload->type == PAYLOAD_TYPE_REPRESENTATION)
    {
        std::cout << "PAYLOAD_TYPE_REPRESENTATION received" << std::endl;
        std::cout << "You can Sign-In using retrieved accesstoken when disconnected or reboot" <<
                  std::endl;

        OCRepPayloadValue *val = ((OCRepPayload *)clientResponse->payload)->values;

        printRepresentation(val);
    }

    return OC_STACK_KEEP_TRANSACTION;
}

void PrintUsage()
{
    std::cout << std::endl;
    std::cout << "Usage : thin_cloud_device <addr:port> <uid> <accesstoken>\n";
    std::cout << "<addr:port>: Cloud Address, \"127.0.0.1:5683\"\n";
    std::cout <<
              "<accesstoken>: String value, Provided by response of onboarding scenario\n\tor kind of registration portal\n\n";
    std::cout <<
              "sample: \"cloud_device 127.0.0.1:5683\"\n\t-Sign-Up mode\n\n";
    std::cout <<
              "sample: \"cloud_device 127.0.0.1:5683 abcdefg 1234567890123456\"\n\t-Sign-in and Publish resource to registered account\n\n";
}

static FILE *client_open(const char * /*path*/, const char *mode)
{
    return fopen("./thin_resource_server.dat", mode);
}

int main(int argc, char *argv[])
{
    std::string uId;
    std::string accessToken;

    std::string authProvider;
    std::string authCode;

    OCMode      stackMode = OC_CLIENT_SERVER;

    switch (argc)
    {
        case 2:
            std::cout << "Put auth provider name(ex: github)" << std::endl;
            std::cin >> authProvider;
            std::cout << "Put auth code(provided by auth provider)" << std::endl;
            std::cin >> authCode;
            break;

        case 4:
            uId = argv[2];
            accessToken = argv[3];
            break;

        default:
            PrintUsage();
            return 0;
    }

    g_host += argv[1];

    std::cout << "Host " << g_host.c_str() << std::endl;

    OCPersistentStorage ps{ client_open, fread, fwrite, fclose, unlink };
    if (OCRegisterPersistentStorageHandler(&ps) != OC_STACK_OK)
    {
        std::cout << "OCStack init persistent storage error" << std::endl;
        return 0;
    }

    if (OCInit(NULL, 0, stackMode) != OC_STACK_OK)
    {
        std::cout << "OCStack init error" << std::endl;
        return 0;
    }

    OCStackResult   res = OC_STACK_ERROR;

    switch (argc)
    {
        case 2:
            std::cout << "Sign-Up to cloud using " << authProvider << " " << authCode << std::endl;
            res = OCCloudSignup(g_host.c_str(), OCGetServerInstanceIDString(), authProvider.c_str(),
                                authCode.c_str(), handleRegisterCB);
            std::cout << "OCCloudSignup return " << res << std::endl;
            break;

        case 4:
            std::cout << "Sign-In to cloud using " << accessToken << std::endl;
            res = OCCloudLogin(g_host.c_str(), uId.c_str(), OCGetServerInstanceIDString(), accessToken.c_str(),
                               handleLoginoutCB);
            std::cout << "OCCloudLogin return " << res << std::endl;
            break;

        default:
            PrintUsage();
            return 0;
    }



    std::cout << "Waiting response.." << std::endl;

    while (true)
    {
        if (OCProcess() != OC_STACK_OK)
        {
            std::cout << "OCProcess process error" << std::endl;
        }

        sleep(1);
    }

    if (OCStop() != OC_STACK_OK)
    {
        std::cout << "OCStop process error" << std::endl;
    }

    return 0;
}