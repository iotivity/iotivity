//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
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
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <ocstack.h>

const char *getResult(OCStackResult result);
std::string getIPAddrTBServer(OCClientResponse * clientResponse);
std::string getPortTBServer(OCClientResponse * clientResponse);
std::string getQueryStrForGetPut(unsigned  const char * responsePayload);

#define TAG PCF("occlient")
#define CTX_VAL 0x99
#ifndef MAX_LENGTH_IPv4_ADDR
#define MAX_LENGTH_IPv4_ADDR 16
#endif


typedef enum {
    TEST_DISCOVER_REQ = 1,
    TEST_GET_REQ,
    TEST_PUT_REQ,
    TEST_OBS_REQ,
    TEST_GET_UNAVAILABLE_RES_REQ,
    MAX_TESTS
} CLIENT_TEST;

static int UNICAST_DISCOVERY = 0;
static int TEST_CASE = 0;
static const char * TEST_APP_UNICAST_DISCOVERY_QUERY = "coap://0.0.0.0:5683/oc/core";
static std::string putPayload = "{\"state\":\"off\",\"power\":\"0\"}";

// The handle for the observe registration
OCDoHandle gObserveDoHandle;
// After this crosses a threshold client deregisters for further observations
int gNumNotifies = 1;

int gQuitFlag = 0;
/* SIGINT handler: set gQuitFlag to 1 for graceful termination */
void handleSigInt(int signum) {
    if (signum == SIGINT) {
        gQuitFlag = 1;
    }
}

// Forward Declaration
OCStackApplicationResult getReqCB(void* ctx, OCDoHandle handle, OCClientResponse * clientResponse);
int InitGetRequestToUnavailableResource(OCClientResponse * clientResponse);
int InitObserveRequest(OCClientResponse * clientResponse);
int InitPutRequest(OCClientResponse * clientResponse);
int InitGetRequest(OCClientResponse * clientResponse);
int InitDiscovery();

static void PrintUsage()
{
    OC_LOG(INFO, TAG, "Usage : occlient -u <0|1> -t <1|2|3|4|5>");
    OC_LOG(INFO, TAG, "-u <0|1> : Perform multicast/unicast discovery of resources");
    OC_LOG(INFO, TAG, "-t 1 : Discover Resources");
    OC_LOG(INFO, TAG, "-t 2 : Discover Resources and Initiate Get Request");
    OC_LOG(INFO, TAG, "-t 3 : Discover Resources and Initiate Put Requests");
    OC_LOG(INFO, TAG, "-t 4 : Discover Resources and Initiate Observe Requests");
    OC_LOG(INFO, TAG, "-t 5 : Discover Resources and Initiate Get Request for a resource which is unavailable");
}


OCStackResult InvokeOCDoResource(std::ostringstream &query,
                                 OCMethod method, OCQualityOfService qos,
                                 OCClientResponseHandler cb )
{
    OCStackResult ret;
    OCCallbackData cbData;
    OCDoHandle handle;

    cbData.cb = cb;
    cbData.context = (void*)CTX_VAL;

    ret = OCDoResource(&handle, method, query.str().c_str(), 0,
                       (method == OC_REST_PUT) ? putPayload.c_str() : NULL,
                       qos, &cbData);

    if (ret != OC_STACK_OK)
    {
        OC_LOG_V(ERROR, TAG, "OCDoResource returns error %d with method %d", ret, method);
    }
    else if (method == OC_REST_OBSERVE)
    {
        gObserveDoHandle = handle;
        printf ("Obs handle %016" PRIxPTR "\n", (uintptr_t)gObserveDoHandle);
    }

    return ret;
}

OCStackApplicationResult putReqCB(void* ctx, OCDoHandle handle, OCClientResponse * clientResponse) {
    if(ctx == (void*)CTX_VAL)
    {
        OC_LOG_V(INFO, TAG, "Callback Context for PUT query recvd successfully");
    }

    if(clientResponse)
    {
        OC_LOG_V(INFO, TAG, "StackResult: %s",  getResult(clientResponse->result));
        OC_LOG_V(INFO, TAG, "JSON = %s =============> Put Response", clientResponse->resJSONPayload);
    }
    return OC_STACK_DELETE_TRANSACTION;
}

OCStackApplicationResult getReqCB(void* ctx, OCDoHandle handle, OCClientResponse * clientResponse) {
    if(ctx == (void*)CTX_VAL)
    {
        OC_LOG_V(INFO, TAG, "Callback Context for GET query recvd successfully");
    }

    if(clientResponse)
    {
        OC_LOG_V(INFO, TAG, "StackResult: %s",  getResult(clientResponse->result));
        OC_LOG_V(INFO, TAG, "SEQUENCE NUMBER: %d", clientResponse->sequenceNumber);
        OC_LOG_V(INFO, TAG, "JSON = %s =============> Get Response", clientResponse->resJSONPayload);
    }
    return OC_STACK_DELETE_TRANSACTION;
}

OCStackApplicationResult obsReqCB(void* ctx, OCDoHandle handle, OCClientResponse * clientResponse) {
    if(ctx == (void*)CTX_VAL)
    {
        OC_LOG_V(INFO, TAG, "Callback Context for OBS query recvd successfully");
    }

    if(clientResponse)
    {
        OC_LOG_V(INFO, TAG, "StackResult: %s",  getResult(clientResponse->result));
        OC_LOG_V(INFO, TAG, "SEQUENCE NUMBER: %d", clientResponse->sequenceNumber);
        OC_LOG_V(INFO, TAG, "Callback Context for OBSERVE notification recvd successfully %d", gNumNotifies);
        OC_LOG_V(INFO, TAG, "JSON = %s =============> Obs Response", clientResponse->resJSONPayload);
        gNumNotifies++;
        if (gNumNotifies == 3)
        {
            printf ("************************** CANCEL OBSERVE\n");
            if (OCCancel (gObserveDoHandle) != OC_STACK_OK){
                OC_LOG(ERROR, TAG, "Observe cancel error");
            }
            return OC_STACK_DELETE_TRANSACTION;
        }
    }
    return OC_STACK_KEEP_TRANSACTION;
}


// This is a function called back when a device is discovered
OCStackApplicationResult discoveryReqCB(void* ctx, OCDoHandle handle,
        OCClientResponse * clientResponse) {
    uint8_t remoteIpAddr[4];
    uint16_t remotePortNu;

    if (ctx == (void*) CTX_VAL)
    {
        OC_LOG_V(INFO, TAG, "Callback Context for DISCOVER query recvd successfully");
    }

    if (clientResponse)
    {
        OC_LOG_V(INFO, TAG, "StackResult: %s", getResult(clientResponse->result));

        OCDevAddrToIPv4Addr((OCDevAddr *) clientResponse->addr, remoteIpAddr,
                remoteIpAddr + 1, remoteIpAddr + 2, remoteIpAddr + 3);
        OCDevAddrToPort((OCDevAddr *) clientResponse->addr, &remotePortNu);

        OC_LOG_V(INFO, TAG,
                "Device =============> Discovered %s @ %d.%d.%d.%d:%d",
                clientResponse->resJSONPayload, remoteIpAddr[0], remoteIpAddr[1],
                remoteIpAddr[2], remoteIpAddr[3], remotePortNu);


        if (TEST_CASE == TEST_GET_REQ)
        {
            InitGetRequest(clientResponse);
        }
        else if (TEST_CASE == TEST_PUT_REQ)
        {
            InitPutRequest(clientResponse);
        }
        else if (TEST_CASE == TEST_OBS_REQ)
        {
            InitObserveRequest(clientResponse);
        }
        else if (TEST_CASE == TEST_GET_UNAVAILABLE_RES_REQ)
        {
            InitGetRequestToUnavailableResource(clientResponse);
        }
    }

    return (UNICAST_DISCOVERY) ? OC_STACK_DELETE_TRANSACTION : OC_STACK_KEEP_TRANSACTION ;

}


int InitGetRequestToUnavailableResource(OCClientResponse * clientResponse)
{
    OC_LOG_V(INFO, TAG, "\n\nExecuting %s", __func__);
    std::ostringstream query;
    query << "coap://" << getIPAddrTBServer(clientResponse) << ":" << getPortTBServer(clientResponse) << "/SomeUnknownResource";
    return (InvokeOCDoResource(query, OC_REST_GET, OC_NON_CONFIRMABLE, getReqCB));
}


int InitObserveRequest(OCClientResponse * clientResponse)
{
    OC_LOG_V(INFO, TAG, "\n\nExecuting %s", __func__);
    std::ostringstream query;
    query << "coap://" << getIPAddrTBServer(clientResponse) << ":" << getPortTBServer(clientResponse) << getQueryStrForGetPut(clientResponse->resJSONPayload);
    return (InvokeOCDoResource(query, OC_REST_OBSERVE, OC_NON_CONFIRMABLE, obsReqCB));
}


int InitPutRequest(OCClientResponse * clientResponse)
{
    OC_LOG_V(INFO, TAG, "\n\nExecuting %s", __func__);
    std::ostringstream query;
    query << "coap://" << getIPAddrTBServer(clientResponse) << ":" << getPortTBServer(clientResponse) << getQueryStrForGetPut(clientResponse->resJSONPayload);
    return (InvokeOCDoResource(query, OC_REST_PUT, OC_NON_CONFIRMABLE, putReqCB));
}


int InitGetRequest(OCClientResponse * clientResponse)
{
    OC_LOG_V(INFO, TAG, "\n\nExecuting %s", __func__);
    std::ostringstream query;
    query << "coap://" << getIPAddrTBServer(clientResponse) << ":" << getPortTBServer(clientResponse) << getQueryStrForGetPut(clientResponse->resJSONPayload);
    return (InvokeOCDoResource(query, OC_REST_GET, OC_NON_CONFIRMABLE, getReqCB));
}

int InitDiscovery()
{
    OCStackResult ret;
    OCCallbackData cbData;
    OCDoHandle handle;
    /* Start a discovery query*/
    char szQueryUri[64] = { 0 };
    if (UNICAST_DISCOVERY)
    {
        strcpy(szQueryUri, TEST_APP_UNICAST_DISCOVERY_QUERY);
    }
    else
    {
        strcpy(szQueryUri, OC_WELL_KNOWN_QUERY);
    }
    cbData.cb = discoveryReqCB;
    cbData.context = (void*)CTX_VAL;
    ret = OCDoResource(&handle, OC_REST_GET, szQueryUri, 0, 0, OC_NON_CONFIRMABLE, &cbData);
    if (ret != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, "OCStack resource error");
    }
    return ret;
}

int main(int argc, char* argv[]) {
    uint8_t addr[20] = {0};
    uint8_t* paddr = NULL;
    uint16_t port = USE_RANDOM_PORT;
    uint8_t ifname[] = "eth0";
    int opt;

    while ((opt = getopt(argc, argv, "u:t:")) != -1)
    {
        switch(opt)
        {
            case 'u':
                UNICAST_DISCOVERY = atoi(optarg);
                break;
            case 't':
                TEST_CASE = atoi(optarg);
                break;
            default:
                PrintUsage();
                return -1;
        }
    }

    if ((UNICAST_DISCOVERY != 0 && UNICAST_DISCOVERY != 1) ||
            (TEST_CASE < TEST_DISCOVER_REQ || TEST_CASE >= MAX_TESTS) )
    {
        PrintUsage();
        return -1;
    }


    /*Get Ip address on defined interface and initialize coap on it with random port number
     * this port number will be used as a source port in all coap communications*/
    if ( OCGetInterfaceAddress(ifname, sizeof(ifname), AF_INET, addr,
                sizeof(addr)) == ERR_SUCCESS)
    {
        OC_LOG_V(INFO, TAG, "Starting occlient on address %s",addr);
        paddr = addr;
    }

    /* Initialize OCStack*/
    if (OCInit((char *) paddr, port, OC_CLIENT) != OC_STACK_OK) {
        OC_LOG(ERROR, TAG, "OCStack init error");
        return 0;
    }

    InitDiscovery();

    // Break from loop with Ctrl+C
    OC_LOG(INFO, TAG, "Entering occlient main loop...");
    signal(SIGINT, handleSigInt);
    while (!gQuitFlag) {

        if (OCProcess() != OC_STACK_OK) {
            OC_LOG(ERROR, TAG, "OCStack process error");
            return 0;
        }

        sleep(3);
    }
    OC_LOG(INFO, TAG, "Exiting occlient main loop...");

    if (OCStop() != OC_STACK_OK) {
        OC_LOG(ERROR, TAG, "OCStack stop error");
    }

    return 0;
}

std::string getIPAddrTBServer(OCClientResponse * clientResponse) {
    if(!clientResponse) return "";
    if(!clientResponse->addr) return "";
    uint8_t a, b, c, d = 0;
    if(0 != OCDevAddrToIPv4Addr(clientResponse->addr, &a, &b, &c, &d) ) return "";

    char ipaddr[16] = {'\0'};
    snprintf(ipaddr,  sizeof(ipaddr), "%d.%d.%d.%d", a,b,c,d); // ostringstream not working correctly here, hence snprintf
    //printf("IP address string of the TB server = %s\n", *out_ipaddr);
    return std::string (ipaddr);
}


std::string getPortTBServer(OCClientResponse * clientResponse){
    if(!clientResponse) return "";
    if(!clientResponse->addr) return "";
    uint16_t p = 0;
    if(0 != OCDevAddrToPort(clientResponse->addr, &p) ) return "";
    std::ostringstream ss;
    ss << p;
    return ss.str();
}

std::string getQueryStrForGetPut(unsigned  const char * responsePayload){
    //  JSON = {"oc":{"payload":[{"href":"/a/led","rt":["core.led"],"if":["core.rw"],"obs":1}]}}

    //std::string jsonPayload(responsePayload, responsePayload + sizeof responsePayload / sizeof responsePayload[0]);
    std::string jsonPayload(reinterpret_cast<char*>(const_cast<unsigned char*>(responsePayload)));
    //std::cout << jsonPayload << std::endl;

    return "/a/led";
}
