//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include "ocstack.h"
#include "logger.h"
#include "occlient.h"
#include "ocpayload.h"

// Tracking user input
static int UNICAST_DISCOVERY = 0;
static int TEST_CASE = 0;
static int CONNECTIVITY = 0;

static const char * UNICAST_DEVICE_DISCOVERY_QUERY = "coap://%s/oic/d";
static const char * MULTICAST_DEVICE_DISCOVERY_QUERY = "/oic/d";
static const char * UNICAST_PLATFORM_DISCOVERY_QUERY = "coap://%s/oic/p";
static const char * MULTICAST_PLATFORM_DISCOVERY_QUERY = "/oic/p";

static const char * UNICAST_RESOURCE_DISCOVERY_QUERY = "coap://%s/oic/res";
static const char * MULTICAST_RESOURCE_DISCOVERY_QUERY = "/oic/res";
//The following variable determines the interface protocol (IPv4, IPv6, etc)
//to be used for sending unicast messages. Default set to IPv4.
static OCConnectivityType OC_CONNTYPE = CT_ADAPTER_IP;
static std::string coapServerIP = "255.255.255.255";
static std::string coapServerPort = "5683";
static std::string coapServerResource = "/a/light";
// Size to hold IPV4_ADDRESS:PORT
static const int IPV4_ADDR_SIZE = 24;
//Use ipv4addr for both InitDiscovery and InitPlatformOrDeviceDiscovery
char ipv4addr[IPV4_ADDR_SIZE];
void StripNewLineChar(char* str);

// The handle for the observe registration
OCDoHandle gObserveDoHandle;
#ifdef WITH_PRESENCE
// The handle for observe registration
OCDoHandle gPresenceHandle;
#endif
// After this crosses a threshold client deregisters for further notifications
int gNumObserveNotifies = 0;

#ifdef WITH_PRESENCE
int gNumPresenceNotifies = 0;
#endif

int gQuitFlag = 0;
/* SIGINT handler: set gQuitFlag to 1 for graceful termination */
void handleSigInt(int signum)
{
    if (signum == SIGINT)
    {
        gQuitFlag = 1;
    }
}

OCPayload* putPayload()
{
    OCRepPayload* payload = OCRepPayloadCreate();

    if(!payload)
    {
        std::cout << "Failed to create put payload object"<<std::endl;
        std::exit(1);
    }

    OCRepPayloadSetPropInt(payload, "power", 15);
    OCRepPayloadSetPropBool(payload, "state", true);

    return (OCPayload*) payload;
}

static void PrintUsage()
{
    OC_LOG(INFO, TAG, "Usage : occlient -u <0|1> -t <1..17> -c <0|1>");
    OC_LOG(INFO, TAG, "-u <0|1> : Perform multicast/unicast discovery of resources");
    OC_LOG(INFO, TAG, "-c 0 : Use Default connectivity(IP)");
    OC_LOG(INFO, TAG, "-c 1 : IP Connectivity Type");
    OC_LOG(INFO, TAG, "-t 1  :  Discover Resources");
    OC_LOG(INFO, TAG, "-t 2  :  Discover Resources and Initiate Nonconfirmable Get Request");
    OC_LOG(INFO, TAG, "-t 3  :  Discover Resources and Initiate Nonconfirmable Get Request"
            " with query filter.");
    OC_LOG(INFO, TAG, "-t 4  :  Discover Resources and Initiate Nonconfirmable Put Requests");
    OC_LOG(INFO, TAG, "-t 5  :  Discover Resources and Initiate Nonconfirmable Post Requests");
    OC_LOG(INFO, TAG, "-t 6  :  Discover Resources and Initiate Nonconfirmable Delete Requests");
    OC_LOG(INFO, TAG, "-t 7  :  Discover Resources and Initiate Nonconfirmable Observe Requests");
    OC_LOG(INFO, TAG, "-t 8  :  Discover Resources and Initiate Nonconfirmable Get Request "\
            "for a resource which is unavailable");
    OC_LOG(INFO, TAG, "-t 9  :  Discover Resources and Initiate Confirmable Get Request");
    OC_LOG(INFO, TAG, "-t 10 :  Discover Resources and Initiate Confirmable Post Request");
    OC_LOG(INFO, TAG, "-t 11 :  Discover Resources and Initiate Confirmable Delete Requests");
    OC_LOG(INFO, TAG, "-t 12 :  Discover Resources and Initiate Confirmable Observe Requests"\
            " and cancel with Low QoS");

#ifdef WITH_PRESENCE
    OC_LOG(INFO, TAG, "-t 13 :  Discover Resources and Initiate Nonconfirmable presence");
    OC_LOG(INFO, TAG, "-t 14 :  Discover Resources and Initiate Nonconfirmable presence with "\
            "filter");
    OC_LOG(INFO, TAG, "-t 15 :  Discover Resources and Initiate Nonconfirmable presence with "\
            "2 filters");
    OC_LOG(INFO, TAG, "-t 16 :  Discover Resources and Initiate Nonconfirmable multicast presence.");
#endif

    OC_LOG(INFO, TAG, "-t 17 :  Discover Resources and Initiate Nonconfirmable Observe Requests "\
            "then cancel immediately with High QOS");
    OC_LOG(INFO, TAG, "-t 18 :  Discover Resources and Initiate Nonconfirmable Get Request and "\
            "add  vendor specific header options");
    OC_LOG(INFO, TAG, "-t 19 :  Discover Platform");
    OC_LOG(INFO, TAG, "-t 20 :  Discover Devices");
}

OCStackResult InvokeOCDoResource(std::ostringstream &query,
                                 OCMethod method,
                                 OCQualityOfService qos,
                                 OCClientResponseHandler cb,
                                 OCHeaderOption * options,
                                 uint8_t numOptions)
{
    OCStackResult ret;
    OCCallbackData cbData;
    OCDoHandle handle;

    cbData.cb = cb;
    cbData.context = (void*)DEFAULT_CONTEXT_VALUE;
    cbData.cd = NULL;

    ret = OCDoResource(&handle, method, query.str().c_str(), 0,
                       (method == OC_REST_PUT) ? putPayload() : NULL,
                       (OC_CONNTYPE), qos, &cbData, options, numOptions);

    if (ret != OC_STACK_OK)
    {
        OC_LOG_V(ERROR, TAG, "OCDoResource returns error %d with method %d", ret, method);
    }
    else if (method == OC_REST_OBSERVE || method == OC_REST_OBSERVE_ALL)
    {
        gObserveDoHandle = handle;
    }
#ifdef WITH_PRESENCE
    else if (method == OC_REST_PRESENCE)
    {
        gPresenceHandle = handle;
    }
#endif

    return ret;
}

OCStackApplicationResult putReqCB(void* ctx, OCDoHandle handle, OCClientResponse * clientResponse)
{
    if(ctx == (void*)DEFAULT_CONTEXT_VALUE)
    {
        OC_LOG(INFO, TAG, "Callback Context for PUT recvd successfully");
    }

    if(clientResponse)
    {
        OC_LOG_V(INFO, TAG, "StackResult: %s",  getResult(clientResponse->result));
        OC_LOG_PAYLOAD(INFO, TAG, clientResponse->payload);
        OC_LOG(INFO, TAG, PCF("=============> Put Response"));
    }
    else
    {
        OC_LOG_V(INFO, TAG, "putReqCB received Null clientResponse");
    }
    return OC_STACK_DELETE_TRANSACTION;
}

OCStackApplicationResult postReqCB(void *ctx, OCDoHandle handle, OCClientResponse *clientResponse)
{
    if(ctx == (void*)DEFAULT_CONTEXT_VALUE)
    {
        OC_LOG(INFO, TAG, "Callback Context for POST recvd successfully");
    }

    if(clientResponse)
    {
        OC_LOG_V(INFO, TAG, "StackResult: %s",  getResult(clientResponse->result));
        OC_LOG_PAYLOAD(INFO, TAG, clientResponse->payload);
        OC_LOG(INFO, TAG, PCF("=============> Post Response"));
    }
    else
    {
        OC_LOG_V(INFO, TAG, "postReqCB received Null clientResponse");
    }
    return OC_STACK_DELETE_TRANSACTION;
}

OCStackApplicationResult deleteReqCB(void *ctx,
        OCDoHandle handle, OCClientResponse *clientResponse)
{
    if(ctx == (void*)DEFAULT_CONTEXT_VALUE)
    {
        OC_LOG(INFO, TAG, "Callback Context for DELETE recvd successfully");
    }

    if(clientResponse)
    {
        OC_LOG_V(INFO, TAG, "StackResult: %s",  getResult(clientResponse->result));
        OC_LOG_PAYLOAD(INFO, TAG, clientResponse->payload);
        OC_LOG(INFO, TAG, PCF("=============> Delete Response"));
    }
    else
    {
        OC_LOG_V(INFO, TAG, "deleteReqCB received Null clientResponse");
    }
    return OC_STACK_DELETE_TRANSACTION;
}

OCStackApplicationResult getReqCB(void* ctx, OCDoHandle handle, OCClientResponse * clientResponse)
{
    if(clientResponse == NULL)
    {
        OC_LOG(INFO, TAG, "getReqCB received NULL clientResponse");
        return   OC_STACK_DELETE_TRANSACTION;
    }

    if(ctx == (void*)DEFAULT_CONTEXT_VALUE)
    {
        OC_LOG(INFO, TAG, "Callback Context for GET query recvd successfully");
    }

    OC_LOG_V(INFO, TAG, "StackResult: %s",  getResult(clientResponse->result));
    OC_LOG_V(INFO, TAG, "SEQUENCE NUMBER: %d", clientResponse->sequenceNumber);
    OC_LOG_PAYLOAD(INFO, TAG, clientResponse->payload);
    OC_LOG(INFO, TAG, PCF("=============> Get Response"));

    if(clientResponse->numRcvdVendorSpecificHeaderOptions > 0)
    {
        OC_LOG (INFO, TAG, "Received vendor specific options");
        uint8_t i = 0;
        OCHeaderOption * rcvdOptions = clientResponse->rcvdVendorSpecificHeaderOptions;
        for( i = 0; i < clientResponse->numRcvdVendorSpecificHeaderOptions; i++)
        {
            if(((OCHeaderOption)rcvdOptions[i]).protocolID == OC_COAP_ID)
            {
                OC_LOG_V(INFO, TAG, "Received option with OC_COAP_ID and ID %u with",
                        ((OCHeaderOption)rcvdOptions[i]).optionID );

                OC_LOG_BUFFER(INFO, TAG, ((OCHeaderOption)rcvdOptions[i]).optionData,
                    MAX_HEADER_OPTION_DATA_LENGTH);
            }
        }
    }
    return OC_STACK_DELETE_TRANSACTION;
}

OCStackApplicationResult obsReqCB(void* ctx, OCDoHandle handle, OCClientResponse * clientResponse)
{
    if(ctx == (void*)DEFAULT_CONTEXT_VALUE)
    {
        OC_LOG(INFO, TAG, "Callback Context for OBS query recvd successfully");
    }

    if(clientResponse)
    {
        OC_LOG_V(INFO, TAG, "StackResult: %s",  getResult(clientResponse->result));
        OC_LOG_V(INFO, TAG, "SEQUENCE NUMBER: %d", clientResponse->sequenceNumber);
        OC_LOG_V(INFO, TAG, "Callback Context for OBSERVE notification recvd successfully %d",
                gNumObserveNotifies);
        OC_LOG_PAYLOAD(INFO, TAG, clientResponse->payload);
        OC_LOG(INFO, TAG, PCF("=============> Obs Response"));
        gNumObserveNotifies++;
        if (gNumObserveNotifies == 15) //large number to test observing in DELETE case.
        {
            if(TEST_CASE == TEST_OBS_REQ_NON || TEST_CASE == TEST_OBS_REQ_CON)
            {
                if (OCCancel (gObserveDoHandle, OC_LOW_QOS, NULL, 0) != OC_STACK_OK)
                {
                    OC_LOG(ERROR, TAG, "Observe cancel error");
                }
                return OC_STACK_DELETE_TRANSACTION;
            }
            else if(TEST_CASE == TEST_OBS_REQ_NON_CANCEL_IMM)
            {
                if (OCCancel (gObserveDoHandle, OC_HIGH_QOS, NULL, 0) != OC_STACK_OK)
                {
                    OC_LOG(ERROR, TAG, "Observe cancel error");
                }
            }
        }
        if(clientResponse->sequenceNumber == OC_OBSERVE_REGISTER)
        {
            OC_LOG(INFO, TAG, "This also serves as a registration confirmation");
        }
        else if(clientResponse->sequenceNumber == OC_OBSERVE_DEREGISTER)
        {
            OC_LOG(INFO, TAG, "This also serves as a deregistration confirmation");
            return OC_STACK_DELETE_TRANSACTION;
        }
        else if(clientResponse->sequenceNumber == OC_OBSERVE_NO_OPTION)
        {
            OC_LOG(INFO, TAG, "This also tells you that registration/deregistration failed");
            return OC_STACK_DELETE_TRANSACTION;
        }
    }
    else
    {
        OC_LOG_V(INFO, TAG, "obsReqCB received Null clientResponse");
    }
    return OC_STACK_KEEP_TRANSACTION;
}
#ifdef WITH_PRESENCE
OCStackApplicationResult presenceCB(void* ctx, OCDoHandle handle, OCClientResponse * clientResponse)
{
    if (ctx == (void*) DEFAULT_CONTEXT_VALUE)
    {
        OC_LOG(INFO, TAG, "Callback Context for Presence recvd successfully");
    }

    if (clientResponse)
    {
        OC_LOG_V(INFO, TAG, "StackResult: %s", getResult(clientResponse->result));
        OC_LOG_V(INFO, TAG, "NONCE NUMBER: %u", clientResponse->sequenceNumber);
        OC_LOG_V(INFO, TAG, "Callback Context for Presence notification recvd successfully %d",
                gNumPresenceNotifies);
        OC_LOG_PAYLOAD(INFO, TAG, clientResponse->payload);
        OC_LOG(INFO, TAG, PCF("=============> Presence Response"));
        gNumPresenceNotifies++;
        if (gNumPresenceNotifies == 20)
        {
            if (OCCancel(gPresenceHandle, OC_LOW_QOS, NULL, 0) != OC_STACK_OK)
            {
                OC_LOG(ERROR, TAG, "Presence cancel error");
            }
            return OC_STACK_DELETE_TRANSACTION;
        }
    }
    else
    {
        OC_LOG_V(INFO, TAG, "presenceCB received Null clientResponse");
    }
    return OC_STACK_KEEP_TRANSACTION;
}
#endif

// This is a function called back when a device is discovered
OCStackApplicationResult discoveryReqCB(void* ctx, OCDoHandle handle,
        OCClientResponse * clientResponse)
{
    if (ctx == (void*) DEFAULT_CONTEXT_VALUE)
    {
        OC_LOG(INFO, TAG, "Callback Context for DISCOVER query recvd successfully");
    }

    if (clientResponse)
    {
        OC_LOG_V(INFO, TAG, "StackResult: %s", getResult(clientResponse->result));

        std::string connectionType = getConnectivityType (clientResponse->connType);
        OC_LOG_V(INFO, TAG, "Discovered on %s", connectionType.c_str());
        OC_LOG_V(INFO, TAG,
                "Device =============> Discovered @ %s:%d",
                clientResponse->devAddr.addr,
                clientResponse->devAddr.port);
        OC_LOG_PAYLOAD(INFO, TAG, clientResponse->payload);

        OC_CONNTYPE = clientResponse->connType;
        parseClientResponse(clientResponse);

        switch(TEST_CASE)
        {
            case TEST_GET_REQ_NON:
                InitGetRequest(OC_LOW_QOS, 0, 0);
                break;
            case TEST_GET_REQ_NON_WITH_FILTERS:
                InitGetRequest(OC_LOW_QOS, 0, 1);
                break;
            case TEST_PUT_REQ_NON:
                InitPutRequest(OC_LOW_QOS);
                break;
            case TEST_POST_REQ_NON:
                InitPostRequest(OC_LOW_QOS);
                break;
            case TEST_DELETE_REQ_NON:
                InitDeleteRequest(OC_LOW_QOS);
                break;
            case TEST_OBS_REQ_NON:
            case TEST_OBS_REQ_NON_CANCEL_IMM:
                InitObserveRequest(OC_LOW_QOS);
                break;
            case TEST_GET_UNAVAILABLE_RES_REQ_NON:
                InitGetRequestToUnavailableResource(OC_LOW_QOS);
                break;
            case TEST_GET_REQ_CON:
                InitGetRequest(OC_HIGH_QOS, 0, 0);
                break;
            case TEST_POST_REQ_CON:
                InitPostRequest(OC_HIGH_QOS);
                break;
            case TEST_DELETE_REQ_CON:
                InitDeleteRequest(OC_HIGH_QOS);
                break;
            case TEST_OBS_REQ_CON:
                InitObserveRequest(OC_HIGH_QOS);
                break;
#ifdef WITH_PRESENCE
            case TEST_OBS_PRESENCE:
            case TEST_OBS_PRESENCE_WITH_FILTER:
            case TEST_OBS_PRESENCE_WITH_FILTERS:
            case TEST_OBS_MULTICAST_PRESENCE:
                InitPresence();
                break;
#endif
            case TEST_GET_REQ_NON_WITH_VENDOR_HEADER_OPTIONS:
                InitGetRequest(OC_LOW_QOS, 1, 0);
                break;
            case TEST_DISCOVER_PLATFORM_REQ:
                InitPlatformDiscovery(OC_LOW_QOS);
                break;
            case TEST_DISCOVER_DEV_REQ:
                InitDeviceDiscovery(OC_LOW_QOS);
                break;
            default:
                PrintUsage();
                break;
        }
    }
    else
    {
        OC_LOG_V(INFO, TAG, "discoveryReqCB received Null clientResponse");
    }
    return OC_STACK_KEEP_TRANSACTION;
}

OCStackApplicationResult PlatformDiscoveryReqCB (void* ctx, OCDoHandle handle,
        OCClientResponse * clientResponse)
{
    if (ctx == (void*) DEFAULT_CONTEXT_VALUE)
    {
        OC_LOG(INFO, TAG, "Callback Context for Platform DISCOVER query recvd successfully");
    }

    if(clientResponse)
    {
        OC_LOG(INFO, TAG, PCF("Discovery Response:"));
        OC_LOG_PAYLOAD(INFO, TAG, clientResponse->payload);
    }
    else
    {
        OC_LOG_V(INFO, TAG, "PlatformDiscoveryReqCB received Null clientResponse");
    }

    return (UNICAST_DISCOVERY) ? OC_STACK_DELETE_TRANSACTION : OC_STACK_KEEP_TRANSACTION;
}

OCStackApplicationResult DeviceDiscoveryReqCB (void* ctx, OCDoHandle handle,
        OCClientResponse * clientResponse)
{
    if (ctx == (void*) DEFAULT_CONTEXT_VALUE)
    {
        OC_LOG(INFO, TAG, "Callback Context for Device DISCOVER query recvd successfully");
    }

    if(clientResponse)
    {
        OC_LOG(INFO, TAG, PCF("Discovery Response:"));
        OC_LOG_PAYLOAD(INFO, TAG, clientResponse->payload);
    }
    else
    {
        OC_LOG_V(INFO, TAG, "PlatformDiscoveryReqCB received Null clientResponse");
    }

    return (UNICAST_DISCOVERY) ? OC_STACK_DELETE_TRANSACTION : OC_STACK_KEEP_TRANSACTION;
}

#ifdef WITH_PRESENCE
int InitPresence()
{
    OCStackResult result = OC_STACK_OK;
    OC_LOG_V(INFO, TAG, "\n\nExecuting %s", __func__);
    std::ostringstream query;
    std::ostringstream querySuffix;
    query << "coap://" << coapServerIP << ":" << coapServerPort << OC_RSRVD_PRESENCE_URI;
    if(TEST_CASE == TEST_OBS_PRESENCE)
    {
        result = InvokeOCDoResource(query, OC_REST_PRESENCE, OC_LOW_QOS,
                presenceCB, NULL, 0);
    }
    if(TEST_CASE == TEST_OBS_PRESENCE_WITH_FILTER || TEST_CASE == TEST_OBS_PRESENCE_WITH_FILTERS)
    {
        querySuffix.str("");
        querySuffix << query.str() << "?rt=core.led";
        result = InvokeOCDoResource(querySuffix, OC_REST_PRESENCE, OC_LOW_QOS,
                presenceCB, NULL, 0);
    }
    if(TEST_CASE == TEST_OBS_PRESENCE_WITH_FILTERS)
    {
        if(result == OC_STACK_OK)
        {
            querySuffix.str("");
            querySuffix << query.str() << "?rt=core.fan";
            result = InvokeOCDoResource(querySuffix, OC_REST_PRESENCE, OC_LOW_QOS,
                    presenceCB, NULL, 0);
        }
    }
    if(TEST_CASE == TEST_OBS_MULTICAST_PRESENCE)
    {
        if(result == OC_STACK_OK)
        {
            std::ostringstream multicastPresenceQuery;
            multicastPresenceQuery.str("");
            multicastPresenceQuery << "coap://" << OC_MULTICAST_PREFIX << OC_RSRVD_PRESENCE_URI;
            result = InvokeOCDoResource(multicastPresenceQuery, OC_REST_PRESENCE, OC_LOW_QOS,
                    presenceCB, NULL, 0);
        }
    }
    return result;
}
#endif

int InitGetRequestToUnavailableResource(OCQualityOfService qos)
{
    OC_LOG_V(INFO, TAG, "\n\nExecuting %s", __func__);
    std::ostringstream query;
    query << "coap://" << coapServerIP << ":" << coapServerPort << "/SomeUnknownResource";
    return (InvokeOCDoResource(query, OC_REST_GET, (qos == OC_HIGH_QOS)? OC_HIGH_QOS:OC_LOW_QOS,
            getReqCB, NULL, 0));
}

int InitObserveRequest(OCQualityOfService qos)
{
    OC_LOG_V(INFO, TAG, "\n\nExecuting %s", __func__);
    std::ostringstream query;
    query << "coap://" << coapServerIP << ":" << coapServerPort << coapServerResource;
    return (InvokeOCDoResource(query,
            OC_REST_OBSERVE, (qos == OC_HIGH_QOS)? OC_HIGH_QOS:OC_LOW_QOS, obsReqCB, NULL, 0));
}

int InitPutRequest(OCQualityOfService qos)
{
    OC_LOG_V(INFO, TAG, "\n\nExecuting %s", __func__);
    std::ostringstream query;
    query << "coap://" << coapServerIP << ":" << coapServerPort << coapServerResource;
    return (InvokeOCDoResource(query, OC_REST_PUT, (qos == OC_HIGH_QOS)? OC_HIGH_QOS:OC_LOW_QOS,
            putReqCB, NULL, 0));
}

int InitPostRequest(OCQualityOfService qos)
{
    OCStackResult result;
    OC_LOG_V(INFO, TAG, "\n\nExecuting %s", __func__);
    std::ostringstream query;
    query << "coap://" << coapServerIP << ":" << coapServerPort << coapServerResource;

    // First POST operation (to create an Light instance)
    result = InvokeOCDoResource(query, OC_REST_POST,
                               ((qos == OC_HIGH_QOS) ? OC_HIGH_QOS: OC_LOW_QOS),
                               postReqCB, NULL, 0);
    if (OC_STACK_OK != result)
    {
        // Error can happen if for example, network connectivity is down
        OC_LOG(INFO, TAG, "First POST call did not succeed");
    }

    // Second POST operation (to create an Light instance)
    result = InvokeOCDoResource(query, OC_REST_POST,
                               ((qos == OC_HIGH_QOS) ? OC_HIGH_QOS: OC_LOW_QOS),
                               postReqCB, NULL, 0);
    if (OC_STACK_OK != result)
    {
        OC_LOG(INFO, TAG, "Second POST call did not succeed");
    }

    // This POST operation will update the original resourced /a/light
    return (InvokeOCDoResource(query, OC_REST_POST,
                               ((qos == OC_HIGH_QOS) ? OC_HIGH_QOS: OC_LOW_QOS),
                               postReqCB, NULL, 0));
}

void* RequestDeleteDeathResourceTask(void* myqos)
{
    sleep (30);//long enough to give the server time to finish deleting the resource.
    std::ostringstream query;
    query << "coap://" << coapServerIP << ":" << coapServerPort << coapServerResource;

    OC_LOG_V(INFO, TAG, "\n\nExecuting %s", __func__);

    // Second DELETE operation to delete the resource that might have been removed already.
    OCQualityOfService qos;
    if (myqos == NULL)
    {
        qos = OC_LOW_QOS;
    }
    else
    {
        qos = OC_HIGH_QOS;
    }

    OCStackResult result = InvokeOCDoResource(query, OC_REST_DELETE,
                               qos,
                               deleteReqCB, NULL, 0);

    if (OC_STACK_OK != result)
    {
        OC_LOG(INFO, TAG, "Second DELETE call did not succeed");
    }

    return NULL;
}

int InitDeleteRequest(OCQualityOfService qos)
{
    OCStackResult result;
    std::ostringstream query;
    query << "coap://" << coapServerIP << ":" << coapServerPort << coapServerResource;

    OC_LOG_V(INFO, TAG, "\n\nExecuting %s", __func__);

    // First DELETE operation
    result = InvokeOCDoResource(query, OC_REST_DELETE,
                               qos,
                               deleteReqCB, NULL, 0);
    if (OC_STACK_OK != result)
    {
        // Error can happen if for example, network connectivity is down
        OC_LOG(INFO, TAG, "First DELETE call did not succeed");
    }
    else
    {
        //Create a thread to delete this resource again
        pthread_t threadId;
        pthread_create (&threadId, NULL, RequestDeleteDeathResourceTask, (void*)qos);
    }

    OC_LOG_V(INFO, TAG, "\n\nExit  %s", __func__);
    return result;
}

int InitGetRequest(OCQualityOfService qos, uint8_t withVendorSpecificHeaderOptions, bool getWithQuery)
{

    OCHeaderOption options[MAX_HEADER_OPTIONS];

    OC_LOG_V(INFO, TAG, "\n\nExecuting %s", __func__);
    std::ostringstream query;
    query << "coap://" << coapServerIP << ":" << coapServerPort << coapServerResource;

    // ocserver is written to only process "power<X" query.
    if (getWithQuery)
    {
        OC_LOG(INFO, TAG, "Using query power<50");
        query << "?power<50";
    }

    if (withVendorSpecificHeaderOptions)
    {
        uint8_t option0[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
        uint8_t option1[] = { 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 };
        memset(options, 0, sizeof(OCHeaderOption) * MAX_HEADER_OPTIONS);
        options[0].protocolID = OC_COAP_ID;
        options[0].optionID = 2048;
        memcpy(options[0].optionData, option0, sizeof(option0));
        options[0].optionLength = 10;
        options[1].protocolID = OC_COAP_ID;
        options[1].optionID = 3000;
        memcpy(options[1].optionData, option1, sizeof(option1));
        options[1].optionLength = 10;
    }
    if (withVendorSpecificHeaderOptions)
    {
        return (InvokeOCDoResource(query, OC_REST_GET,
                (qos == OC_HIGH_QOS) ? OC_HIGH_QOS : OC_LOW_QOS, getReqCB, options, 2));
    }
    else
    {
        return (InvokeOCDoResource(query, OC_REST_GET,
                (qos == OC_HIGH_QOS) ? OC_HIGH_QOS : OC_LOW_QOS, getReqCB, NULL, 0));
    }
}

int InitPlatformDiscovery(OCQualityOfService qos)
{
    OC_LOG_V(INFO, TAG, "\n\nExecuting %s", __func__);

    OCStackResult ret;
    OCCallbackData cbData;
    char szQueryUri[64] = { 0 };

    cbData.cb = PlatformDiscoveryReqCB;
    cbData.context = (void*)DEFAULT_CONTEXT_VALUE;
    cbData.cd = NULL;

    if(UNICAST_DISCOVERY)
    {
        snprintf(szQueryUri, sizeof(szQueryUri), UNICAST_PLATFORM_DISCOVERY_QUERY, ipv4addr);
    }
    else
    {
        strncpy(szQueryUri, MULTICAST_PLATFORM_DISCOVERY_QUERY, sizeof(szQueryUri) -1 );
    }
    szQueryUri[sizeof(szQueryUri) -1] = '\0';

    if(UNICAST_DISCOVERY)
    {
        ret = OCDoResource(NULL, OC_REST_GET, szQueryUri, 0, 0, OC_CONNTYPE,
                (qos == OC_HIGH_QOS) ? OC_HIGH_QOS : OC_LOW_QOS, &cbData, NULL, 0);
    }
    else
    {

        ret = OCDoResource(NULL, OC_REST_DISCOVER, szQueryUri, 0, 0, OC_CONNTYPE,
                        (qos == OC_HIGH_QOS) ? OC_HIGH_QOS : OC_LOW_QOS, &cbData, NULL, 0);
    }

    if (ret != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, "OCStack device error");
    }

    return ret;
}

int InitDeviceDiscovery(OCQualityOfService qos)
{
    OC_LOG_V(INFO, TAG, "\n\nExecuting %s", __func__);

    OCStackResult ret;
    OCCallbackData cbData;
    char szQueryUri[64] = { 0 };

    cbData.cb = DeviceDiscoveryReqCB;
    cbData.context = (void*)DEFAULT_CONTEXT_VALUE;
    cbData.cd = NULL;

    if(UNICAST_DISCOVERY)
    {
        snprintf(szQueryUri, sizeof(szQueryUri), UNICAST_DEVICE_DISCOVERY_QUERY, ipv4addr);
    }
    else
    {
        strncpy(szQueryUri, MULTICAST_DEVICE_DISCOVERY_QUERY, sizeof(szQueryUri) -1 );
    }
    szQueryUri[sizeof(szQueryUri) -1] = '\0';

    if(UNICAST_DISCOVERY)
    {
        ret = OCDoResource(NULL, OC_REST_GET, szQueryUri, 0, 0, OC_CONNTYPE,
                (qos == OC_HIGH_QOS) ? OC_HIGH_QOS : OC_LOW_QOS, &cbData, NULL, 0);
    }
    else
    {
        ret = OCDoResource(NULL, OC_REST_DISCOVER, szQueryUri, 0, 0, OC_CONNTYPE,
                        (qos == OC_HIGH_QOS) ? OC_HIGH_QOS : OC_LOW_QOS, &cbData, NULL, 0);
    }

    if (ret != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, "OCStack device error");
    }

    return ret;
}

int InitDiscovery(OCQualityOfService qos)
{
    OCStackResult ret;
    OCCallbackData cbData;
    /* Start a discovery query*/
    char szQueryUri[64] = { 0 };

    if (UNICAST_DISCOVERY)
    {
        snprintf(szQueryUri, sizeof(szQueryUri), UNICAST_RESOURCE_DISCOVERY_QUERY, ipv4addr);
    }
    else
    {
        strcpy(szQueryUri, MULTICAST_RESOURCE_DISCOVERY_QUERY);
    }

    cbData.cb = discoveryReqCB;
    cbData.context = (void*)DEFAULT_CONTEXT_VALUE;
    cbData.cd = NULL;
    if(UNICAST_DISCOVERY)
    {
        ret = OCDoResource(NULL, OC_REST_GET, szQueryUri, 0, 0, OC_CONNTYPE,
                (qos == OC_HIGH_QOS) ? OC_HIGH_QOS : OC_LOW_QOS, &cbData, NULL, 0);
    }
    else
    {
        ret = OCDoResource(NULL, OC_REST_DISCOVER, szQueryUri, 0, 0, OC_CONNTYPE,
                        (qos == OC_HIGH_QOS) ? OC_HIGH_QOS : OC_LOW_QOS, &cbData, NULL, 0);
    }
    if (ret != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, "OCStack resource error");
    }
    return ret;
}

int main(int argc, char* argv[])
{
    int opt;

    while ((opt = getopt(argc, argv, "u:t:c:")) != -1)
    {
        switch(opt)
        {
            case 'u':
                UNICAST_DISCOVERY = atoi(optarg);
                break;
            case 't':
                TEST_CASE = atoi(optarg);
                break;
            case 'c':
                CONNECTIVITY = atoi(optarg);
                break;
            default:
                PrintUsage();
                return -1;
        }
    }

    if ((UNICAST_DISCOVERY != 0 && UNICAST_DISCOVERY != 1) ||
            (TEST_CASE < TEST_DISCOVER_REQ || TEST_CASE >= MAX_TESTS) ||
            (CONNECTIVITY < CT_ADAPTER_DEFAULT || CONNECTIVITY >= MAX_CT))
    {
        PrintUsage();
        return -1;
    }

    /* Initialize OCStack*/
    if (OCInit(NULL, 0, OC_CLIENT) != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, "OCStack init error");
        return 0;
    }

    if(CONNECTIVITY == CT_ADAPTER_DEFAULT || CONNECTIVITY == CT_IP)
    {
        OC_CONNTYPE = CT_ADAPTER_IP;
    }
    else
    {
        OC_LOG(INFO, TAG, "Default Connectivity type selected...");
        PrintUsage();
    }

    if (UNICAST_DISCOVERY)
    {
        OC_LOG(INFO, TAG, "Enter IP address with port number of the Server hosting resource");
        OC_LOG(INFO, TAG, "as follows - eg: 192.168.0.15:45454 (IP:Port) \n");

        if (fgets(ipv4addr, IPV4_ADDR_SIZE, stdin))
        {
            //Strip newline char from ipv4addr
            StripNewLineChar(ipv4addr);
        }
        else
        {
            OC_LOG(ERROR, TAG, "!! Bad input for IPV4 address. !!");
            return OC_STACK_INVALID_PARAM;
        }
    }

    if(UNICAST_DISCOVERY  == 0  && TEST_CASE == TEST_DISCOVER_DEV_REQ)
    {
        InitDeviceDiscovery(OC_LOW_QOS);
    }
    else if(UNICAST_DISCOVERY  == 0  && TEST_CASE == TEST_DISCOVER_PLATFORM_REQ)
    {
        InitPlatformDiscovery(OC_LOW_QOS);
    }
    else
    {
        InitDiscovery(OC_LOW_QOS);
    }

    // Break from loop with Ctrl+C
    OC_LOG(INFO, TAG, "Entering occlient main loop...");
    signal(SIGINT, handleSigInt);
    while (!gQuitFlag)
    {

        if (OCProcess() != OC_STACK_OK)
        {
            OC_LOG(ERROR, TAG, "OCStack process error");
            return 0;
        }

        sleep(2);
    }
    OC_LOG(INFO, TAG, "Exiting occlient main loop...");

    if (OCStop() != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, "OCStack stop error");
    }

    return 0;
}

std::string getIPAddrTBServer(OCClientResponse * clientResponse)
{
    if (!clientResponse)
    {
        return "";
    }
    if (!clientResponse->addr)
    {
        return "";
    }

    return std::string(clientResponse->devAddr.addr);
}

std::string getPortTBServer(OCClientResponse * clientResponse)
{
    if (!clientResponse)
    {
        return "";
    }
    if (!clientResponse->addr)
    {
        return "";
    }
    std::ostringstream ss;
    ss << clientResponse->devAddr.port;
    return ss.str();
}

std::string getConnectivityType (OCConnectivityType connType)
{
    switch (connType & CT_MASK_ADAPTER)
    {
        case CT_ADAPTER_IP:
            return "IP";

        case CT_IP_USE_V4:
            return "IPv4";

        case CT_IP_USE_V6:
            return "IPv6";

        case CT_ADAPTER_GATT_BTLE:
            return "GATT";

        case CT_ADAPTER_RFCOMM_BTEDR:
            return "RFCOMM";

        default:
            return "Incorrect connectivity";
    }
}

std::string getQueryStrForGetPut(OCClientResponse * clientResponse)
{

    return "/a/light";
}

void parseClientResponse(OCClientResponse * clientResponse)
{
    coapServerIP = getIPAddrTBServer(clientResponse);
    coapServerPort = getPortTBServer(clientResponse);
    coapServerResource = getQueryStrForGetPut(clientResponse);
}

