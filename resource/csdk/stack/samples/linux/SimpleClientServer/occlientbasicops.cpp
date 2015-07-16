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
#include <stdint.h>
#include <sstream>
#include <iostream>

#include "ocstack.h"
#include "logger.h"
#include "occlientbasicops.h"
#include "ocpayload.h"
#include "oic_malloc.h"
#include "oic_string.h"

#define MAX_IP_ADDR_ST_SZ  16 //string size of "155.255.255.255" (15 + 1)
#define MAX_PORT_ST_SZ  6     //string size of "65535" (5 + 1)

static int IPV4_ADDR_SIZE = 24;
static int UNICAST_DISCOVERY = 0;
static int TEST_CASE = 0;
static int CONNECTIVITY = 0;

static const char UNICAST_DISCOVERY_QUERY[] = "coap://%s/oic/res";

//The following variable determines the interface protocol (IP, etc)
//to be used for sending unicast messages. Default set to IP.
static OCConnectivityType OC_CONNTYPE = CT_ADAPTER_IP;
static const char * MULTICAST_RESOURCE_DISCOVERY_QUERY = "/oic/res";

int gQuitFlag = 0;

struct ResourceNode *resourceList;
/*
 * SIGINT handler: set gQuitFlag to 1 for graceful termination
 * */
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
    OC_LOG(INFO, TAG, "Usage : occlient -u <0|1> -t <1|2|3> -c <0|1>");
    OC_LOG(INFO, TAG, "-u <0|1> : Perform multicast/unicast discovery of resources");
    OC_LOG(INFO, TAG, "-t 1 : Discover Resources");
    OC_LOG(INFO, TAG, "-t 2 : Discover Resources and"
            " Initiate Nonconfirmable Get/Put/Post Requests");
    OC_LOG(INFO, TAG, "-t 3 : Discover Resources and Initiate "
            "Confirmable Get/Put/Post Requests");
    OC_LOG(INFO, TAG, "-c 0 : Default auto-selection");
    OC_LOG(INFO, TAG, "-c 1 : IP Connectivity Type");
}

/*
 * Returns the first resource in the list
 */
const ResourceNode * getResource()
{
    return resourceList;
}

OCStackResult InvokeOCDoResource(std::ostringstream &query, OCMethod method,
                                 OCConnectivityType connType, OCQualityOfService qos,
        OCClientResponseHandler cb, OCHeaderOption * options, uint8_t numOptions)
{
    OCStackResult ret;
    OCCallbackData cbData;

    cbData.cb = cb;
    cbData.context = (void*)DEFAULT_CONTEXT_VALUE;
    cbData.cd = NULL;

    ret = OCDoResource(NULL, method, query.str().c_str(), 0,
        (method == OC_REST_PUT || method == OC_REST_POST) ? putPayload() : NULL,
         connType, qos, &cbData, options, numOptions);

    if (ret != OC_STACK_OK)
    {
        OC_LOG_V(ERROR, TAG, "OCDoResource returns error %d with method %d",
                 ret, method);
    }

    return ret;
}

OCStackApplicationResult putReqCB(void* ctx, OCDoHandle handle,
                                OCClientResponse * clientResponse)
{
    if(ctx == (void*)DEFAULT_CONTEXT_VALUE)
    {
        OC_LOG(INFO, TAG, "<====Callback Context for PUT received successfully====>");
    }
    else
    {
        OC_LOG(ERROR, TAG, "<====Callback Context for PUT fail====>");
    }

    if(clientResponse)
    {
        OC_LOG_PAYLOAD(INFO, TAG, clientResponse->payload);
        OC_LOG(INFO, TAG, PCF("=============> Put Response"));
    }
    else
    {
        OC_LOG(ERROR, TAG, "<====PUT Callback fail to receive clientResponse====>\n");
    }
    return OC_STACK_DELETE_TRANSACTION;
}

OCStackApplicationResult postReqCB(void *ctx, OCDoHandle handle,
                          OCClientResponse *clientResponse)
{
    if(ctx == (void*)DEFAULT_CONTEXT_VALUE)
    {
        OC_LOG(INFO, TAG, "<====Callback Context for POST received successfully====>");
    }
    else
    {
        OC_LOG(ERROR, TAG, "<====Callback Context for POST fail====>");
    }

    if(clientResponse)
    {
        OC_LOG_PAYLOAD(INFO, TAG, clientResponse->payload);
        OC_LOG(INFO, TAG, PCF("=============> Post Response"));
    }
    else
    {
        OC_LOG(ERROR, TAG, "<====POST Callback fail to receive clientResponse====>\n");
    }

    return OC_STACK_DELETE_TRANSACTION;
}

OCStackApplicationResult getReqCB(void* ctx, OCDoHandle handle,
                           OCClientResponse * clientResponse)
{
    if (ctx == (void*) DEFAULT_CONTEXT_VALUE)
    {
        OC_LOG(INFO, TAG, "<====Callback Context for GET received successfully====>");
    }
    else
    {
        OC_LOG(ERROR, TAG, "<====Callback Context for GET fail====>");
    }

    if (clientResponse)
    {
        OC_LOG_V(INFO, TAG, "StackResult: %s",  getResult(clientResponse->result));
        OC_LOG_V(INFO, TAG, "SEQUENCE NUMBER: %d", clientResponse->sequenceNumber);
        OC_LOG_PAYLOAD(INFO, TAG, clientResponse->payload);
        OC_LOG(INFO, TAG, PCF("=============> Get Response"));

        if (clientResponse->numRcvdVendorSpecificHeaderOptions > 0 )
        {
            OC_LOG (INFO, TAG, "Received vendor specific options");
            uint8_t i = 0;
            OCHeaderOption * rcvdOptions = clientResponse->rcvdVendorSpecificHeaderOptions;
            for (i = 0; i < clientResponse->numRcvdVendorSpecificHeaderOptions; i++)
            {
                if (((OCHeaderOption) rcvdOptions[i]).protocolID == OC_COAP_ID)
                {
                    OC_LOG_V(INFO, TAG, "Received option with OC_COAP_ID and ID %u with",
                            ((OCHeaderOption)rcvdOptions[i]).optionID );

                    OC_LOG_BUFFER(INFO, TAG, ((OCHeaderOption)rcvdOptions[i]).optionData,
                        MAX_HEADER_OPTION_DATA_LENGTH);
                }
            }
        }
    }
    else
    {
        OC_LOG(ERROR, TAG, "<====GET Callback fail to receive clientResponse====>\n");
    }
    return OC_STACK_DELETE_TRANSACTION;
}

/*
 * This is a function called back when a device is discovered
 */
OCStackApplicationResult discoveryReqCB(void* ctx, OCDoHandle handle,
        OCClientResponse * clientResponse)
{
    if (ctx == (void*)DEFAULT_CONTEXT_VALUE)
    {
        OC_LOG(INFO, TAG, "\n<====Callback Context for DISCOVERY query "
               "received successfully====>");
    }
    else
    {
        OC_LOG(ERROR, TAG, "\n<====Callback Context for DISCOVERY fail====>");
    }

    if (clientResponse)
    {
        OC_LOG_V(INFO, TAG,
                "Device =============> Discovered @ %s:%d",
                clientResponse->devAddr.addr,
                clientResponse->devAddr.port);
        OC_LOG_PAYLOAD(INFO, TAG, clientResponse->payload);

        collectUniqueResource(clientResponse);
    }
    else
    {
        OC_LOG(ERROR, TAG, "<====DISCOVERY Callback fail to receive clientResponse====>\n");
    }
    return (UNICAST_DISCOVERY) ?
           OC_STACK_DELETE_TRANSACTION : OC_STACK_KEEP_TRANSACTION ;
}

int InitPutRequest(OCQualityOfService qos)
{
    std::ostringstream query;
    //Get most recently inserted resource
    const ResourceNode * resource  = getResource();

    if(!resource)
    {
        OC_LOG_V(ERROR, TAG, "Resource null, can't do PUT request\n");
        return -1;
    }
    query << "coap://" << resource->ip << ":" << resource->port << resource->uri ;
    OC_LOG_V(INFO, TAG,"Executing InitPutRequest, Query: %s", query.str().c_str());

    return (InvokeOCDoResource(query, OC_REST_PUT, resource->connType,
           ((qos == OC_HIGH_QOS) ? OC_HIGH_QOS: OC_LOW_QOS),
            putReqCB, NULL, 0));
}

int InitPostRequest(OCQualityOfService qos)
{
    OCStackResult result;
    std::ostringstream query;
    //Get most recently inserted resource
    const ResourceNode * resource  = getResource();

    if(!resource)
    {
        OC_LOG_V(ERROR, TAG, "Resource null, can't do POST request\n");
        return -1;
    }

    query << "coap://" << resource->ip << ":" << resource->port << resource->uri ;
    OC_LOG_V(INFO, TAG,"Executing InitPostRequest, Query: %s", query.str().c_str());

    // First POST operation (to create an LED instance)
    result = InvokeOCDoResource(query, OC_REST_POST, resource->connType,
            ((qos == OC_HIGH_QOS) ? OC_HIGH_QOS: OC_LOW_QOS),
            postReqCB, NULL, 0);
    if (OC_STACK_OK != result)
    {
        // Error can happen if for example, network connectivity is down
        OC_LOG(ERROR, TAG, "First POST call did not succeed");
    }

    // Second POST operation (to create an LED instance)
    result = InvokeOCDoResource(query, OC_REST_POST, resource->connType,
            ((qos == OC_HIGH_QOS) ? OC_HIGH_QOS: OC_LOW_QOS),
            postReqCB, NULL, 0);
    if (OC_STACK_OK != result)
    {
        OC_LOG(ERROR, TAG, "Second POST call did not succeed");
    }

    // This POST operation will update the original resourced /a/led
    return (InvokeOCDoResource(query, OC_REST_POST,resource->connType,
                ((qos == OC_HIGH_QOS) ? OC_HIGH_QOS: OC_LOW_QOS),
                postReqCB, NULL, 0));
}

int InitGetRequest(OCQualityOfService qos)
{
    std::ostringstream query;
    //Get most recently inserted resource
    const ResourceNode * resource  = getResource();

    if(!resource)
    {
        OC_LOG_V(ERROR, TAG, "Resource null, can't do GET request\n");
        return -1;
    }
    query << "coap://" << resource->ip << ":" << resource->port << resource->uri ;
    OC_LOG_V(INFO, TAG,"Executing InitGetRequest, Query: %s", query.str().c_str());

    return (InvokeOCDoResource(query, OC_REST_GET, resource->connType,
            (qos == OC_HIGH_QOS)?OC_HIGH_QOS:OC_LOW_QOS, getReqCB, NULL, 0));
}

int InitDiscovery()
{
    OCStackResult ret;
    OCCallbackData cbData;
    /* Start a discovery query*/
    char szQueryUri[64] = {};
    if (UNICAST_DISCOVERY)
    {
        char ipv4addr[IPV4_ADDR_SIZE];
        OC_LOG(INFO, TAG, "Enter IP address with port of the Server hosting resource"\
                    "(Ex: 192.168.0.15:1234) ");

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
        ret = OCDoResource(NULL, OC_REST_GET, szQueryUri, 0, 0, OC_CONNTYPE,
                OC_LOW_QOS, &cbData, NULL, 0);
    }
    else
    {
        ret = OCDoResource(NULL, OC_REST_DISCOVER, szQueryUri, 0, 0, CT_DEFAULT,
                OC_LOW_QOS, &cbData, NULL, 0);
    }

    if (ret != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, "OCStack resource error");
    }
    return ret;
}

const char *getIPAddr(const OCClientResponse *clientResponse)
{
    if (!clientResponse)
    {
        return "";
    }

    const OCDevAddr *devAddr = &clientResponse->devAddr;
    char *ipaddr = (char *) OICCalloc(1, strlen(devAddr->addr) +1);
    if (ipaddr)
    {
        snprintf(ipaddr, MAX_IP_ADDR_ST_SZ, "%s", devAddr->addr);
    }
    else
    {
        OC_LOG(ERROR, TAG, "Memory not allocated to ipaddr");
    }
    return ipaddr;
}

const char *getPort(const OCClientResponse *clientResponse)
{
    if(!clientResponse)
    {
        return "";
    }

    char *port = NULL;
    if((port = (char *)OICCalloc(1, MAX_PORT_ST_SZ)))
    {
        snprintf(port, MAX_PORT_ST_SZ, "%d", clientResponse->devAddr.port);
    }
    else
    {
        OC_LOG(ERROR, TAG, "Memory not allocated to port");
    }
    return port;
}

void queryResource()
{
    switch(TEST_CASE)
    {
        case TEST_DISCOVER_REQ:
            break;
        case TEST_NON_CON_OP:
            InitGetRequest(OC_LOW_QOS);
            InitPutRequest(OC_LOW_QOS);
            InitPostRequest(OC_LOW_QOS);
            break;
        case TEST_CON_OP:
            InitGetRequest(OC_HIGH_QOS);
            InitPutRequest(OC_HIGH_QOS);
            InitPostRequest(OC_HIGH_QOS);
            break;
        default:
            PrintUsage();
            break;
    }
}


void collectUniqueResource(const OCClientResponse * clientResponse)
{
    OCResourcePayload* res = ((OCDiscoveryPayload*)clientResponse->payload)->resources;
    char sidStr[UUID_LENGTH];

    while(res) {

        int ret = snprintf(sidStr, UUID_LENGTH,
                "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                res->sid[0], res->sid[1], res->sid[2], res->sid[3],
                res->sid[4], res->sid[5], res->sid[6], res->sid[7],
                res->sid[8], res->sid[9], res->sid[10], res->sid[11],
                res->sid[12], res->sid[13], res->sid[14], res->sid[15]
                );

        if (ret == UUID_LENGTH - 1)
        {
            if(insertResource(sidStr, res->uri, clientResponse) == 1)
            {
                OC_LOG_V(INFO,TAG,"%s%s%s%s\n",sidStr, ":", res->uri, " is new");
                printResourceList();
                queryResource();
            }
            else {
                OC_LOG_V(INFO,TAG,"%s%s%s%s\n",sidStr, ":", res->uri, " is old");
            }
        }
        else
        {
            OC_LOG(ERROR, TAG, "Could Not Retrieve the Server ID");
        }

        res = res->next;
    }
}

/* This function searches for the resource(sid:uri) in the ResourceList.
 * If the Resource is found in the list then it returns 0 else insert
 * the resource to front of the list and returns 1.
 */
int insertResource(const char * sid, char const * uri,
            const OCClientResponse * clientResponse)
{
    ResourceNode * iter = resourceList;
    char * sid_cpy =  OICStrdup(sid);
    char * uri_cpy = OICStrdup(uri);

    //Checking if the resource(sid:uri) is new
    while(iter)
    {
        if((strcmp(iter->sid, sid) == 0) && (strcmp(iter->uri, uri) == 0))
        {
            OICFree(sid_cpy);
            OICFree(uri_cpy);
            return 0;
        }
        else
        {
            iter = iter->next;
        }
    }

    //Creating new ResourceNode
    if((iter = (ResourceNode *) OICMalloc(sizeof(ResourceNode))))
    {
        iter->sid = sid_cpy;
        iter->uri = uri_cpy;
        iter->ip = getIPAddr(clientResponse);
        iter->port = getPort(clientResponse);
        iter->connType = clientResponse->connType;
        iter->next = NULL;
    }
    else
    {
        OC_LOG(ERROR, TAG, "Memory not allocated to ResourceNode");
        OICFree(sid_cpy);
        OICFree(uri_cpy);
        return -1;
    }

    //Adding new ResourceNode to front of the ResourceList
    if(!resourceList)
    {
        resourceList = iter;
    }
    else
    {
        iter->next = resourceList;
        resourceList = iter;
    }
    return 1;
}

void printResourceList()
{
    ResourceNode * iter;
    iter = resourceList;
    OC_LOG(INFO, TAG, "Resource List: ");
    while(iter)
    {
        OC_LOG(INFO, TAG, "*****************************************************");
        OC_LOG_V(INFO, TAG, "sid = %s",iter->sid);
        OC_LOG_V(INFO, TAG, "uri = %s", iter->uri);
        OC_LOG_V(INFO, TAG, "ip = %s", iter->ip);
        OC_LOG_V(INFO, TAG, "port = %s", iter->port);
        switch (iter->connType & CT_MASK_ADAPTER)
        {
            case CT_ADAPTER_IP:
                OC_LOG(INFO, TAG, "connType = Default (IPv4)");
                break;
            case OC_ADAPTER_GATT_BTLE:
                OC_LOG(INFO, TAG, "connType = BLE");
                break;
            case OC_ADAPTER_RFCOMM_BTEDR:
                OC_LOG(INFO, TAG, "connType = BT");
                break;
            default:
                OC_LOG(INFO, TAG, "connType = Invalid connType");
                break;
        }
        OC_LOG(INFO, TAG, "*****************************************************");
        iter = iter->next;
    }
}

void freeResourceList()
{
    OC_LOG(INFO, TAG, "Freeing ResourceNode List");
    ResourceNode * temp;
    while(resourceList)
    {

        temp = resourceList;
        resourceList = resourceList->next;
        OICFree((void *)temp->sid);
        OICFree((void *)temp->uri);
        OICFree((void *)temp->ip);
        OICFree((void *)temp->port);
        OICFree(temp);
    }
    resourceList = NULL;
}

int main(int argc, char* argv[])
{
    int opt;
    resourceList = NULL;
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

    if(CONNECTIVITY == CT_ADAPTER_DEFAULT || CONNECTIVITY ==  CT_IP)
    {
        OC_CONNTYPE =  CT_ADAPTER_IP;//CT_DEFAULT;
    }
    else
    {
        OC_LOG(INFO, TAG, "Default Connectivity type selected");
        PrintUsage();
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

    freeResourceList();
    OC_LOG(INFO, TAG, "Exiting occlient main loop...");
    if (OCStop() != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, "OCStack stop error");
    }
    return 0;
}


