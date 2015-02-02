//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
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

#include <set>
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>

#include "ocstack.h"
#include "logger.h"
#include "occlientbasicops.h"

static int IPV4_ADDR_SIZE = 16;
static int UNICAST_DISCOVERY = 0;
static int TEST_CASE = 0;

static const char * UNICAST_DISCOVERY_QUERY = "coap://%s:5298/oc/core";

static std::string putPayload = "{\"state\":\"off\",\"power\":10}";

static std::string coapServerIP = "255.255.255.255";
static std::string coapServerPort = "5683";
static std::string coapServerResource = "/a/led";

//The following variable determines the interface (wifi, ethernet etc.)
//to be used for sending unicast messages. Default set to WIFI.
static OCConnectivityType OC_CONNTYPE = OC_WIFI;
static const char * MULTICAST_RESOURCE_DISCOVERY_QUERY = "/oc/core";

int gQuitFlag = 0;

namespace {
    typedef std::pair<bool, std::string>    extract_result_t;
    typedef std::string                     sid_t;
    typedef std::set<std::string>           SID_set_t;
} // namespace

void collateSIDs(const OCClientResponse * clientResponse, const std::string& server_ip);

/* SIGINT handler: set gQuitFlag to 1 for graceful termination */
void handleSigInt(int signum)
{
    if (signum == SIGINT)
    {
        gQuitFlag = 1;
    }
}

static void PrintUsage()
{
    OC_LOG(INFO, TAG, "Usage : occlient -u <0|1> -t <1|2|3> -c <0|1>");
    OC_LOG(INFO, TAG, "-u <0|1> : Perform multicast/unicast discovery of resources");
    OC_LOG(INFO, TAG, "-c <0|1> : Send unicast messages over Ethernet or WIFI");
    OC_LOG(INFO, TAG, "-t 1 : Discover Resources");
    OC_LOG(INFO, TAG, "-t 2 : Discover Resources and"
            " Initiate Nonconfirmable Get/Put/Post Requests");
    OC_LOG(INFO, TAG, "-t 3 : Discover Resources and Initiate Confirmable Get/Put/Post Requests");
}

OCStackResult InvokeOCDoResource(std::ostringstream &query,
        OCMethod method, OCQualityOfService qos,
        OCClientResponseHandler cb, OCHeaderOption * options, uint8_t numOptions)
{
    OCStackResult ret;
    OCCallbackData cbData;
    OCDoHandle handle;

    cbData.cb = cb;
    cbData.context = (void*)DEFAULT_CONTEXT_VALUE;
    cbData.cd = NULL;

    ret = OCDoResource(&handle, method, query.str().c_str(), 0,
            (method == OC_REST_PUT || method == OC_REST_POST) ? putPayload.c_str() : NULL,
            OC_CONNTYPE, qos, &cbData, options, numOptions);

    if (ret != OC_STACK_OK)
    {
        OC_LOG_V(ERROR, TAG, "OCDoResource returns error %d with method %d", ret, method);
    }

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
        OC_LOG_V(INFO, TAG, "JSON = %s =============> Put Response", clientResponse->resJSONPayload);
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
        OC_LOG_V(INFO, TAG, "JSON = %s =============> Post Response",
                clientResponse->resJSONPayload);
    }
    return OC_STACK_DELETE_TRANSACTION;
}

OCStackApplicationResult getReqCB(void* ctx, OCDoHandle handle, OCClientResponse * clientResponse)
{
    if(ctx == (void*)DEFAULT_CONTEXT_VALUE)
    {
        OC_LOG(INFO, TAG, "Callback Context for GET query recvd successfully");
    }

    if(clientResponse)
    {
        OC_LOG_V(INFO, TAG, "StackResult: %s",  getResult(clientResponse->result));
        OC_LOG_V(INFO, TAG, "SEQUENCE NUMBER: %d", clientResponse->sequenceNumber);
        OC_LOG_V(INFO, TAG, "JSON = %s =============> Get Response",
                clientResponse->resJSONPayload);
    }
    if(clientResponse->rcvdVendorSpecificHeaderOptions &&
            clientResponse->numRcvdVendorSpecificHeaderOptions)
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
                        ((OCHeaderOption)rcvdOptions[i]).optionLength);
            }
        }
    }
    return OC_STACK_DELETE_TRANSACTION;
}

// This is a function called back when a device is discovered
OCStackApplicationResult discoveryReqCB(void* ctx, OCDoHandle handle,
        OCClientResponse * clientResponse)
{
    uint8_t remoteIpAddr[4];
    uint16_t remotePortNu;

    if (ctx == (void*)DEFAULT_CONTEXT_VALUE)
    {
        OC_LOG(INFO, TAG, "Callback Context for DISCOVER query recvd successfully");
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

        parseClientResponse(clientResponse);

        collateSIDs(clientResponse, getIPAddrTBServer(clientResponse));

        switch(TEST_CASE)
        {
            case TEST_NON_CON_OP:
                InitGetRequest(OC_LOW_QOS);
                InitPutRequest();
                InitPostRequest(OC_LOW_QOS);
                break;
            case TEST_CON_OP:
                InitGetRequest(OC_HIGH_QOS);
                InitPutRequest();
                InitPostRequest(OC_HIGH_QOS);
                break;
            default:
                PrintUsage();
                break;
        }
    }

    return (UNICAST_DISCOVERY) ? OC_STACK_DELETE_TRANSACTION : OC_STACK_KEEP_TRANSACTION ;
}

int InitPutRequest()
{
    OC_LOG_V(INFO, TAG, "\n\nExecuting %s", __func__);
    std::ostringstream query;
    query << "coap://" << coapServerIP << ":" << coapServerPort << coapServerResource;
    return (InvokeOCDoResource(query, OC_REST_PUT, OC_LOW_QOS, putReqCB, NULL, 0));
}

int InitPostRequest(OCQualityOfService qos)
{
    OCStackResult result;
    OC_LOG_V(INFO, TAG, "\n\nExecuting %s", __func__);
    std::ostringstream query;
    query << "coap://" << coapServerIP << ":" << coapServerPort << coapServerResource;

    // First POST operation (to create an LED instance)
    result = InvokeOCDoResource(query, OC_REST_POST,
            ((qos == OC_HIGH_QOS) ? OC_HIGH_QOS: OC_LOW_QOS),
            postReqCB, NULL, 0);
    if (OC_STACK_OK != result)
    {
        // Error can happen if for example, network connectivity is down
        OC_LOG(INFO, TAG, "First POST call did not succeed");
    }

    // Second POST operation (to create an LED instance)
    result = InvokeOCDoResource(query, OC_REST_POST,
            ((qos == OC_HIGH_QOS) ? OC_HIGH_QOS: OC_LOW_QOS),
            postReqCB, NULL, 0);
    if (OC_STACK_OK != result)
    {
        OC_LOG(INFO, TAG, "Second POST call did not succeed");
    }

    // This POST operation will update the original resourced /a/led
    return (InvokeOCDoResource(query, OC_REST_POST,
                ((qos == OC_HIGH_QOS) ? OC_HIGH_QOS: OC_LOW_QOS),
                postReqCB, NULL, 0));
}

int InitGetRequest(OCQualityOfService qos)
{
    OC_LOG_V(INFO, TAG, "\n\nExecuting %s", __func__);
    std::ostringstream query;
    query << "coap://" << coapServerIP << ":" << coapServerPort << coapServerResource;

    return (InvokeOCDoResource(query, OC_REST_GET, (qos == OC_HIGH_QOS)?
            OC_HIGH_QOS:OC_LOW_QOS, getReqCB, NULL, 0));
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
        char ipv4addr[IPV4_ADDR_SIZE];
        printf("Enter IPv4 address of the Server hosting resource (Ex: 192.168.0.15)\n");
        if (fgets(ipv4addr, IPV4_ADDR_SIZE, stdin))
        {
            //Strip newline char from ipv4addr
            StripNewLineChar(ipv4addr);
            snprintf(szQueryUri, sizeof(szQueryUri), UNICAST_DISCOVERY_QUERY, ipv4addr);
        }
        else
        {
            OC_LOG(ERROR, TAG, "!! Bad input for IPV4 address. !!");
            return OC_STACK_INVALID_PARAM;
        }
    }
    else
    {
        strcpy(szQueryUri, MULTICAST_RESOURCE_DISCOVERY_QUERY);
    }
    cbData.cb = discoveryReqCB;
    cbData.context = (void*)DEFAULT_CONTEXT_VALUE;
    cbData.cd = NULL;
    if (UNICAST_DISCOVERY)
    {
        ret = OCDoResource(&handle, OC_REST_GET, szQueryUri, 0, 0, OC_CONNTYPE,
                OC_LOW_QOS, &cbData, NULL, 0);
    }
    else
    {
        ret = OCDoResource(&handle, OC_REST_GET, szQueryUri, 0, 0, (OC_ALL),
                OC_LOW_QOS, &cbData, NULL, 0);
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
                OC_CONNTYPE = OCConnectivityType(atoi(optarg));
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

    /* Initialize OCStack*/
    if (OCInit(NULL, 0, OC_CLIENT) != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, "OCStack init error");
        return 0;
    }

    InitDiscovery();

    // Break from loop with Ctrl+C
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
    if(!clientResponse) return "";
    if(!clientResponse->addr) return "";
    uint8_t a, b, c, d = 0;
    if(0 != OCDevAddrToIPv4Addr(clientResponse->addr, &a, &b, &c, &d) ) return "";

    char ipaddr[16] = {'\0'};
    // ostringstream not working correctly here, hence snprintf
    snprintf(ipaddr,  sizeof(ipaddr), "%d.%d.%d.%d", a,b,c,d);
    return std::string (ipaddr);
}


std::string getPortTBServer(OCClientResponse * clientResponse)
{
    if(!clientResponse) return "";
    if(!clientResponse->addr) return "";
    uint16_t p = 0;
    if(0 != OCDevAddrToPort(clientResponse->addr, &p) ) return "";
    std::ostringstream ss;
    ss << p;
    return ss.str();
}

std::string getQueryStrForGetPut(OCClientResponse * clientResponse)
{
    return "/a/led";
}


/* You could do this with the JSON parser of your choice, a regular expression, PEG
grammar, etc.. This "sample" version does not handle cases like escaping strings,
but shows a simple way you might approach the task with just the C++98 library: */
extract_result_t extract_value(const std::string& search_key, const std::string& input)
{
    const std::string key('\"' + search_key + "\":\"");

    const size_t key_mark = input.find(key);
    const size_t key_edge = key_mark + key.length();
    const size_t val_mark = input.find_first_of("\"", key_edge);

    if(std::string::npos == key_mark || std::string::npos == val_mark) {
       std::ostringstream os;

       os << "extract_value(): \"" << search_key << "\" not found in input";
       OC_LOG(ERROR, TAG, os.str().c_str());

       return std::make_pair(false, "");
     }

    return std::make_pair(true, input.substr(key_edge, val_mark - key_edge));
}

extract_result_t parseSID(const OCClientResponse * const clientResponse)
{
    const char* const& pl_in = reinterpret_cast<const char *>(clientResponse->resJSONPayload);

    const std::string pl(pl_in, strlen(pl_in));

    const extract_result_t sid = extract_value("sid", pl);
    const extract_result_t uri = extract_value("href", pl);
    //TODO-CA: It should be just enough to send the SID alone and not the combination
    // of it.
    return std::make_pair(sid.first and uri.first, sid.second + ':' + uri.second);
}

void parseClientResponse(OCClientResponse * clientResponse)
{
    coapServerIP = getIPAddrTBServer(clientResponse);
    coapServerPort = getPortTBServer(clientResponse);
    coapServerResource = getQueryStrForGetPut(clientResponse);
}

void collateSIDs(const OCClientResponse * clientResponse, const std::string& server_ip)
{
    static SID_set_t sids;

    const extract_result_t sid_result = parseSID(clientResponse);

    if(false == sid_result.first)
     return;

    const sid_t& sid = sid_result.second;

    /* ...there's no need for an application to take any special action, but we can tell
    if we've seen a resource before, regardless of the transport it arrive on: */
    std::ostringstream msg;

    if(not sids.insert(sid).second)
     msg << "SID " << sid << " has been seen before.\n";
    else
     msg << "SID " << sid << " is new.\n";

    OC_LOG(INFO, TAG, msg.str().c_str());
}
