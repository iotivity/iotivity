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
/// This sample provides steps to define an interface for a resource
/// (properties and methods) and host this resource on the server.
/// Additionally, it'll have a client example to discover it as well.
///
#include <memory>
#include <iostream>
#include <stdexcept>
#include <condition_variable>
#include <map>
#include <vector>
#include <string>

#include "ocstack.h"
#include "ocpayload.h"

#include "cloud_connector.h"

#define DEFAULT_CONTEXT_VALUE 0x99
#define DEFAULT_PUBLISH_QUERY "/oic/rd?rt=oic.wk.rdpub"
#define DEFAULT_DISCOVER_QUERY "/oic/res?rt=core.light"

////////////////////////////////////////Device Sample
#define SAMPLE_MAX_NUM_POST_INSTANCE  2
typedef struct LIGHTRESOURCE
{
    OCResourceHandle handle;
    bool state;
    int power;
} LightResource;
static LightResource Light;
static LightResource gLightInstance[SAMPLE_MAX_NUM_POST_INSTANCE];


OCRepPayload *getPayload(const char *uri, int64_t power, bool state)
{
    OCRepPayload *payload = OCRepPayloadCreate();
    if (!payload)
    {
        std::cout << "Failed to allocate Payload" << std::endl;
        return nullptr;
    }

    OCRepPayloadSetUri(payload, uri);
    OCRepPayloadSetPropBool(payload, "state", state);
    OCRepPayloadSetPropInt(payload, "power", power);

    return payload;
}


OCRepPayload *constructResponse(OCEntityHandlerRequest *ehRequest)
{
    char *resourceUri = NULL;

    if (ehRequest->payload && ehRequest->payload->type != PAYLOAD_TYPE_REPRESENTATION)
    {
        std::cout << "Incoming payload not a representation" << std::endl;
        return nullptr;
    }

    LightResource *currLightResource = &Light;

    if (ehRequest->resource == gLightInstance[0].handle)
    {
        currLightResource = &gLightInstance[0];
        resourceUri = (char *) "/a/light/0";
    }
    else if (ehRequest->resource == gLightInstance[1].handle)
    {
        currLightResource = &gLightInstance[1];
        resourceUri = (char *) "/a/light/1";
    }

    std::cout << "Resource URI " << resourceUri << std::endl;

    return getPayload(resourceUri, currLightResource->power, currLightResource->state);
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

void PublishResources(std::string host, std::string additionalQuery)
{
    std::cout << "Running as Server mode" << std::endl;

    std::string requestQuery = DEFAULT_PUBLISH_QUERY;
    requestQuery += additionalQuery;

    std::cout << "Publishing resources..." << std::endl;
    std::cout << host.c_str() << requestQuery.c_str() << std::endl;

    if (createLightResource((char *)"/a/light/0", &gLightInstance[0]) != 0)
    {
        std::cout << "Unable to create sample resource" << std::endl;
    }

    if (createLightResource((char *)"/a/light/1", &gLightInstance[1]) != 0)
    {
        std::cout << "Unable to create sample resource" << std::endl;
    }

    if (OCCloudPublish(host.c_str(), requestQuery.c_str(), &handlePublishCB, 2,
                       gLightInstance[0].handle, gLightInstance[1].handle) != OC_STACK_OK)
    {
        std::cout << "Unable to publish resources to cloud" << std::endl;
    }
}

////////////////////////////////////////Client Sample
std::string g_host = "coap+tcp://";

OCStackApplicationResult handleGetCB(void *ctx,
                                     OCDoHandle /*handle*/,
                                     OCClientResponse *clientResponse)
{
    std::cout << "Get response received from " << clientResponse->resourceUri << std::endl;

    if (ctx != (void *)DEFAULT_CONTEXT_VALUE)
    {
        std::cout << "Invalid Publish callback received" << std::endl;
    }

    if (clientResponse->payload == NULL)
        std::cout << "No payload received" << std::endl;

    if (clientResponse->payload != NULL &&
        clientResponse->payload->type == PAYLOAD_TYPE_REPRESENTATION)
    {
        std::cout << "PAYLOAD_TYPE_REPRESENTATION received" << std::endl;

        OCRepPayloadValue *val = ((OCRepPayload *)clientResponse->payload)->values;

        while (val)
        {
            std::cout << "Key: " << val->name << " Value: ";
            switch (val->type)
            {
                case OCREP_PROP_NULL:
                    std::cout << "NULL" << std::endl;
                    break;

                case OCREP_PROP_INT:
                    std::cout << val->i << std::endl;
                    break;

                case OCREP_PROP_DOUBLE:
                    std::cout << val->d << std::endl;
                    break;

                case OCREP_PROP_BOOL:
                    std::cout << val->b << std::endl;
                    break;

                case OCREP_PROP_STRING:
                    std::cout << val->str << std::endl;
                    break;

                case OCREP_PROP_BYTE_STRING:
                    std::cout << "[ByteString]" << std::endl;
                    break;

                case OCREP_PROP_OBJECT:
                    std::cout << "[Object]" << std::endl;
                    break;

                case OCREP_PROP_ARRAY:
                    std::cout << "[Array]" << std::endl;
                    break;
            }

            val = val->next;
        }
    }

    return OC_STACK_KEEP_TRANSACTION;
}

// This is a function called back when a device is discovered
OCStackApplicationResult discoveryReqCB(void *ctx, OCDoHandle /*handle*/,
                                        OCClientResponse *clientResponse)
{
    if (ctx == (void *)DEFAULT_CONTEXT_VALUE)
    {
        std::cout << "Callback Context for DISCOVER query recvd successfully" << std::endl;
    }

    if (clientResponse)
    {
        std::cout << "StackResult: " << clientResponse->result << std::endl;

        OCDiscoveryPayload *payload = (OCDiscoveryPayload *)clientResponse->payload;
        if (!payload)
        {
            std::cout << "Empty payload" << std::endl;
            return OC_STACK_DELETE_TRANSACTION;
        }

        OCResourcePayload *resource = (OCResourcePayload *)payload->resources;
        if (!resource)
        {
            std::cout << "No resources in payload" << std::endl;
            return OC_STACK_DELETE_TRANSACTION;
        }

        OCCallbackData cbData;
        cbData.cb = handleGetCB;
        cbData.context = (void *)DEFAULT_CONTEXT_VALUE;
        cbData.cd = NULL;

        while (resource)
        {
            std::cout << "Found Resource " << resource->uri << std::endl;
            std::string requestUri = g_host;
            requestUri += resource->uri;

            std::cout << "Request GET to resource " << requestUri.c_str() << std::endl;

            OCStackResult res = OCDoResource(NULL, OC_REST_GET, requestUri.c_str(), NULL, NULL,
                                             CT_ADAPTER_TCP, OC_LOW_QOS, &cbData, NULL, 0);

            std::cout << "Requesting GET res=" << res << std::endl;

            resource = resource->next;
        }
    }
    else
    {
        std::cout << "discoveryReqCB received Null clientResponse" << std::endl;
    }
    return OC_STACK_KEEP_TRANSACTION;
}

void DiscoverResources(std::string host, std::string additionalQuery)
{
    std::cout << "Running as Client mode" << std::endl;

    std::string requestQuery = host;
    requestQuery += DEFAULT_DISCOVER_QUERY;
    requestQuery += additionalQuery;

    std::cout << "Finding resources..." << std::endl;
    std::cout << requestQuery.c_str() << std::endl;

    OCCallbackData cbData;

    cbData.cb = discoveryReqCB;
    cbData.context = (void *)DEFAULT_CONTEXT_VALUE;
    cbData.cd = NULL;

    if (OCDoResource(NULL, OC_REST_DISCOVER, requestQuery.c_str(), NULL, 0, CT_ADAPTER_TCP,
                     OC_LOW_QOS, &cbData, NULL, 0) != OC_STACK_OK)
    {
        std::cout << "Unable to find resources from cloud" << std::endl;
    }
}



/////////////////////////////////////////////Common sample

int g_runningMode = 0;

OCStackApplicationResult handleLoginoutCB(void *ctx,
        OCDoHandle /*handle*/,
        OCClientResponse *clientResponse)
{
    if (ctx != (void *)DEFAULT_CONTEXT_VALUE)
    {
        std::cout << "Invalid Login callback received" << std::endl;
    }

    std::cout << "Login/out response received code: " << clientResponse->result << std::endl;

    if (clientResponse->payload != NULL &&
        clientResponse->payload->type == PAYLOAD_TYPE_REPRESENTATION)
    {
        std::cout << "PAYLOAD_TYPE_REPRESENTATION received" << std::endl;

        OCRepPayloadValue *val = ((OCRepPayload *)clientResponse->payload)->values;

        while (val)
        {
            std::cout << "Key: " << val->name << " Value: " << val->str << std::endl;
            val = val->next;
        }

        if (g_runningMode == 1)
        {
            PublishResources(g_host, "");
        }
        else if (g_runningMode == 2)
        {
            DiscoverResources(g_host, "");
        }

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
        std::cout << "You can login using received session variable after disconnected or reboot" <<
                  std::endl;

        OCRepPayloadValue *val = ((OCRepPayload *)clientResponse->payload)->values;

        while (val)
        {
            std::cout << "Key: " << val->name << " Value: " << val->str << std::endl;
            val = val->next;
        }
    }

    return OC_STACK_KEEP_TRANSACTION;
}

void PrintUsage()
{
    std::cout << std::endl;
    std::cout << "Usage : cloud_device <addr:port> <session> <mode>\n";
    std::cout << "<addr:port>: Cloud Address, \"127.0.0.1:5683\"\n";
    std::cout <<
              "<session>: String value, Provided by response of onboarding scenario\n\tor kind of registration portal\n\n";
    std::cout <<
              "<mode>: String value, 's' for publish resource, 'c' for start discovery\n\n";
    std::cout <<
              "If you want to get session key using OAuth 2 auth code,\n\tleave blank to <session>, <mode> fields\n";
    std::cout <<
              "sample: \"cloud_device 127.0.0.1:5683\"\n\t-OAuth 2 registration mode\n\n";
    std::cout <<
              "sample: \"cloud_device 127.0.0.1:5683 1234567890123456 s\"\n\t-Publish resource under registered session\n\n";
    std::cout <<
              "sample: \"cloud_device 127.0.0.1:5683 1234567890123456 c\"\n\t-Discover resource under registered session\n\n";
}

int main(int argc, char *argv[])
{
    std::string session;

    std::string authProvider;
    std::string authCode;

    switch (argc)
    {
        case 2:
            std::cout << "Put auth provider name(ex: github)" << std::endl;
            std::cin >> authProvider;
            std::cout << "Put auth code(provided by auth provider)" << std::endl;
            std::cin >> authCode;
            break;

        case 4:
            session = argv[2];
            if (argv[3][0] == 's')
                g_runningMode = 1;
            else if (argv[3][0] == 'c')
                g_runningMode = 2;
            break;

        default:
            PrintUsage();
            return 0;
    }

    g_host += argv[1];

    std::cout << "Host " << g_host.c_str() << std::endl;

    OCStackResult   res = OC_STACK_ERROR;

    if (OCInit(NULL, 0, OC_CLIENT_SERVER) != OC_STACK_OK)
    {
        std::cout << "OCStack init error" << std::endl;
        return 0;
    }

    switch (argc)
    {
        case 2:
            std::cout << "Register account to cloud using " << authProvider << " " << authCode << std::endl;
            res = OCCloudRegisterLogin(g_host.c_str(), authProvider.c_str(), authCode.c_str(),
                                       handleRegisterCB);
            std::cout << "OCCloudRegisterLogin return " << res << std::endl;
            break;

        case 4:
            res = OCCloudLogin(g_host.c_str(), session.c_str(), handleLoginoutCB);
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