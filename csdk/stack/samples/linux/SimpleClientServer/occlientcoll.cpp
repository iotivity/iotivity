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
#include <ocstack.h>
#include <iostream>
#include <sstream>
#include "logger.h"
const char *getResult(OCStackResult result);
std::string getIPAddrTBServer(OCClientResponse * clientResponse);
std::string getPortTBServer(OCClientResponse * clientResponse);
std::string getQueryStrForGetPut(unsigned  const char * responsePayload);

#define TAG PCF("occlient")
#define CTX_VAL 0x99
#ifndef MAX_LENGTH_IPv4_ADDR
#define MAX_LENGTH_IPv4_ADDR 16
#endif

#define MAX_TEST_CASES 5

static int UNICAST_DISCOVERY = 0;
static int TEST_CASE = 0;
static std::string putPayload = "{\"state\":\"off\",\"power\":\"0\"}";

// The handle for the observe registration
OCDoHandle gObserveDoHandle;
// After this crosses a threshold client deregisters for further observations
int gNumObserveNotifies = 1;

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

void PrintUsage()
{
    OC_LOG(INFO, TAG, "Usage : occlient <Unicast Discovery> <Test Case>");
    OC_LOG(INFO, TAG, "Test Case 1 : Discover Resources");
    OC_LOG(INFO, TAG, "Test Case 2 : Discover Resources and Initiate Get Request");
    OC_LOG(INFO, TAG, "Test Case 3 : Discover Resources and Initiate Get/Put Requests");
    OC_LOG(INFO, TAG, "Test Case 4 : Discover Resources and Initiate Observe Requests");
    OC_LOG(INFO, TAG, "Test Case 5 : Discover Resources and Initiate Get Request for a resource which is unavailable");
}

OCStackApplicationResult putReqCB(void* ctx, OCDoHandle handle, OCClientResponse * clientResponse) {
    if(clientResponse) {}
    if(ctx == (void*)CTX_VAL) {
        OC_LOG_V(INFO, TAG, "Callback Context for PUT query recvd successfully");
        OC_LOG_V(INFO, TAG, "JSON = %s =============> Discovered", clientResponse->resJSONPayload);
    }

    return OC_STACK_KEEP_TRANSACTION;
}

OCStackApplicationResult getReqCB(void* ctx, OCDoHandle handle, OCClientResponse * clientResponse) {
    OC_LOG_V(INFO, TAG, "StackResult: %s",
            getResult(clientResponse->result));
    if(ctx == (void*)CTX_VAL) {
        OC_LOG_V(INFO, TAG, "SEQUENCE NUMBER: %d", clientResponse->sequenceNumber);
        if(clientResponse->sequenceNumber == 0) {
            OC_LOG_V(INFO, TAG, "Callback Context for GET query recvd successfully");
            OC_LOG_V(INFO, TAG, "Fnd' Rsrc': %s", clientResponse->resJSONPayload);
        }
        else {
            OC_LOG_V(INFO, TAG, "Callback Context for OBSERVE notification recvd successfully %d", gNumObserveNotifies);
            OC_LOG_V(INFO, TAG, "Fnd' Rsrc': %s", clientResponse->resJSONPayload);
            gNumObserveNotifies++;
            if (gNumObserveNotifies == 3)
            {
                if (OCCancel (gObserveDoHandle, OC_NON_CONFIRMABLE) != OC_STACK_OK){
                    OC_LOG(ERROR, TAG, "Observe cancel error");
                }
            }
        }
    }
    InitPutRequest(clientResponse);
    return OC_STACK_KEEP_TRANSACTION;
}


// This is a function called back when a device is discovered
OCStackApplicationResult discoveryReqCB(void* ctx, OCDoHandle handle,
        OCClientResponse * clientResponse) {
    uint8_t remoteIpAddr[4];
    uint16_t remotePortNu;

    OC_LOG(INFO, TAG,
            "Entering discoveryReqCB (Application Layer CB)");
    OC_LOG_V(INFO, TAG, "StackResult: %s",
            getResult(clientResponse->result));

    if (ctx == (void*) CTX_VAL) {
        OC_LOG_V(INFO, TAG, "Callback Context recvd successfully");
    }

    OCDevAddrToIPv4Addr((OCDevAddr *) clientResponse->addr, remoteIpAddr,
            remoteIpAddr + 1, remoteIpAddr + 2, remoteIpAddr + 3);
    OCDevAddrToPort((OCDevAddr *) clientResponse->addr, &remotePortNu);
#if 0
    OC_LOG_V(INFO, TAG,
            "Device =============> Discovered %s @ %d.%d.%d.%d:%d",
            clientResponse->resJSONPayload, remoteIpAddr[0], remoteIpAddr[1],
            remoteIpAddr[2], remoteIpAddr[3], remotePortNu);
#endif

    InitGetRequest(clientResponse);

    return OC_STACK_KEEP_TRANSACTION;
}


int InitGetRequestToUnavailableResource(OCClientResponse * clientResponse)
{
    OCStackResult ret;
    OCCallbackData cbData;
    OCDoHandle handle;
    std::ostringstream getQuery;
    getQuery << "coap://" << getIPAddrTBServer(clientResponse) << ":" << getPortTBServer(clientResponse) << "/SomeUnknownResource";
    cbData.cb = getReqCB;
    cbData.context = (void*)CTX_VAL;
    cbData.cd = NULL;
    ret = OCDoResource(&handle, OC_REST_GET, getQuery.str().c_str(), 0, 0, OC_NON_CONFIRMABLE, &cbData);
    if (ret != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, "OCStack resource error");
    }
    return ret;
}


int InitObserveRequest(OCClientResponse * clientResponse)
{
    OCStackResult ret;
    OCCallbackData cbData;
    OCDoHandle handle;
    std::ostringstream obsReg;
    obsReg << "coap://" << getIPAddrTBServer(clientResponse) << ":" << getPortTBServer(clientResponse) << getQueryStrForGetPut(clientResponse->resJSONPayload);
    cbData.cb = getReqCB;
    cbData.context = (void*)CTX_VAL;
    cbData.cd = NULL;
    OC_LOG_V(INFO, TAG, "PUT payload from client = %s ", putPayload.c_str());
    ret = OCDoResource(&handle, OC_REST_OBSERVE, obsReg.str().c_str(), 0, 0, OC_NON_CONFIRMABLE, &cbData);
    if (ret != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, "OCStack resource error");
    }
    else
    {
        gObserveDoHandle = handle;
    }
    return ret;
}


int InitPutRequest(OCClientResponse * clientResponse)
{
    OCStackResult ret;
    OCCallbackData cbData;
    OCDoHandle handle;
    //* Make a PUT query*/
    std::ostringstream getQuery;
    getQuery << "coap://" << getIPAddrTBServer(clientResponse) << ":" << getPortTBServer(clientResponse) <<
    "/a/sroom?if=oc.mi.b";
    cbData.cb = putReqCB;
    cbData.context = (void*)CTX_VAL;
    cbData.cd = NULL;
    OC_LOG_V(INFO, TAG, "PUT payload from client = %s ", putPayload.c_str());
    ret = OCDoResource(&handle, OC_REST_PUT, getQuery.str().c_str(), 0, putPayload.c_str(), OC_NON_CONFIRMABLE, &cbData);
    if (ret != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, "OCStack resource error");
    }
    return ret;
}


int InitGetRequest(OCClientResponse * clientResponse)
{
    OCStackResult ret;
    OCCallbackData cbData;
    OCDoHandle handle;

    uint8_t remoteIpAddr[4];
    uint16_t remotePortNu;

    OCDevAddrToIPv4Addr((OCDevAddr *) clientResponse->addr, remoteIpAddr,
            remoteIpAddr + 1, remoteIpAddr + 2, remoteIpAddr + 3);
    OCDevAddrToPort((OCDevAddr *) clientResponse->addr, &remotePortNu);

    //* Make a GET query*/
    std::ostringstream getQuery;
    getQuery << "coap://" << getIPAddrTBServer(clientResponse) << ":" << getPortTBServer(clientResponse) <<
    //"/a/sroom?if=oc.mi.def";
    //"/a/sroom?if=oc.mi.ll";
    "/a/sroom?if=oc.mi.b";

    std::cout << "Get Query: " << getQuery.str() << std::endl;

    cbData.cb = getReqCB;
    cbData.context = (void*)CTX_VAL;
    cbData.cd = NULL;
    ret = OCDoResource(&handle, OC_REST_GET, getQuery.str().c_str(), 0, 0, OC_NON_CONFIRMABLE, &cbData);
    if (ret != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, "OCStack resource error");
    }
    return ret;
}

#define TEST_APP_UNICAST_DISCOVERY_QUERY                  PCF("coap://0.0.0.0:5683/oc/core")
int InitDiscovery()
{
    OCStackResult ret;
    OCCallbackData cbData;
    OCDoHandle handle;
    /* Start a discovery query*/
    char szQueryUri[64] = { 0 };

    //strcpy(szQueryUri, "coap://224.0.1.187:5683/oc/core");//?rt=core.sroom");
    strcpy(szQueryUri, "coap://0.0.0.0:5683/oc/core");

    cbData.cb = discoveryReqCB;
    cbData.context = (void*)CTX_VAL;
    cbData.cd = NULL;
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

        sleep(2);
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

    std::string jsonPayload(reinterpret_cast<char*>(const_cast<unsigned char*>(responsePayload)));

    return "/a/sroom";
}
