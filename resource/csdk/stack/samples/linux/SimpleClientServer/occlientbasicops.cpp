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

#include "ocstack.h"
#include "logger.h"
#include "occlientbasicops.h"
#include "cJSON.h"
#include "ocmalloc.h"

#define MAX_IP_ADDR_ST_SZ  16 //string size of "155.255.255.255" (15 + 1)
#define MAX_PORT_ST_SZ  6     //string size of "65535" (5 + 1)

static int IPV4_ADDR_SIZE = 16;
static int UNICAST_DISCOVERY = 0;
static int TEST_CASE = 0;

static const char UNICAST_DISCOVERY_QUERY[] = "coap://%s:6298/oc/core";
static std::string putPayload = "{\"state\":\"off\",\"power\":10}";

//The following variable determines the interface protocol (IPv4, IPv6, etc)
//to be used for sending unicast messages. Default set to IPv4.
static OCConnectivityType OC_CONNTYPE = OC_IPV4;
static const char * MULTICAST_RESOURCE_DISCOVERY_QUERY = "/oc/core";

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

static void PrintUsage()
{
    OC_LOG(INFO, TAG, "Usage : occlient -u <0|1> -t <1|2|3> -c <0|1>");
    OC_LOG(INFO, TAG, "-u <0|1> : Perform multicast/unicast discovery of resources");
    OC_LOG(INFO, TAG, "-t 1 : Discover Resources");
    OC_LOG(INFO, TAG, "-t 2 : Discover Resources and"
            " Initiate Nonconfirmable Get/Put/Post Requests");
    OC_LOG(INFO, TAG, "-t 3 : Discover Resources and Initiate "
            "Confirmable Get/Put/Post Requests");
    OC_LOG(INFO, TAG, "-c <0|1> : IPv4/IPv6 (IPv6 not currently supported)");
    OC_LOG(INFO, TAG, "Default connectivityType IPv4");
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
        (method == OC_REST_PUT || method == OC_REST_POST) ? putPayload.c_str() : NULL,
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
    uint8_t remoteIpAddr[4];
    uint16_t remotePortNu;

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
        OCDevAddrToIPv4Addr((OCDevAddr *) clientResponse->addr, remoteIpAddr,
                            remoteIpAddr + 1, remoteIpAddr + 2, remoteIpAddr + 3);
        OCDevAddrToPort((OCDevAddr *) clientResponse->addr, &remotePortNu);

        OC_LOG_V(INFO, TAG,"PUT Response: %s \nFrom %d.%d.%d.%d:%d\n",
                 clientResponse->resJSONPayload, remoteIpAddr[0], remoteIpAddr[1],
                remoteIpAddr[2], remoteIpAddr[3], remotePortNu);
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
    uint8_t remoteIpAddr[4];
    uint16_t remotePortNu;

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
        OCDevAddrToIPv4Addr((OCDevAddr *) clientResponse->addr, remoteIpAddr,
                            remoteIpAddr + 1, remoteIpAddr + 2, remoteIpAddr + 3);
        OCDevAddrToPort((OCDevAddr *) clientResponse->addr, &remotePortNu);

        OC_LOG_V(INFO, TAG,"POST Response: %s \nFrom %d.%d.%d.%d:%d\n",
                    clientResponse->resJSONPayload, remoteIpAddr[0], remoteIpAddr[1],
                    remoteIpAddr[2], remoteIpAddr[3], remotePortNu);
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
    uint8_t remoteIpAddr[4];
    uint16_t remotePortNu;

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
        OCDevAddrToIPv4Addr((OCDevAddr *) clientResponse->addr, remoteIpAddr, remoteIpAddr + 1,
                remoteIpAddr + 2, remoteIpAddr + 3);
        OCDevAddrToPort((OCDevAddr *) clientResponse->addr, &remotePortNu);

        OC_LOG_V(INFO, TAG,"Get Response: %s \nFrom %d.%d.%d.%d:%d\n",
                clientResponse->resJSONPayload, remoteIpAddr[0], remoteIpAddr[1],
                remoteIpAddr[2], remoteIpAddr[3], remotePortNu);

        if (clientResponse->rcvdVendorSpecificHeaderOptions
                && clientResponse->numRcvdVendorSpecificHeaderOptions)
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
    uint8_t remoteIpAddr[4];
    uint16_t remotePortNu;
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
        OCDevAddrToIPv4Addr((OCDevAddr *) clientResponse->addr, remoteIpAddr,
                remoteIpAddr + 1, remoteIpAddr + 2, remoteIpAddr + 3);
        OCDevAddrToPort((OCDevAddr *) clientResponse->addr, &remotePortNu);

        OC_LOG_V(INFO, TAG,
                "Device Discovered %s \n @ %d.%d.%d.%d:%d\n",
                clientResponse->resJSONPayload, remoteIpAddr[0], remoteIpAddr[1],
                remoteIpAddr[2], remoteIpAddr[3], remotePortNu);

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
        printf("Enter IPv4 address of the Server hosting "
               "resource (Ex: 192.168.0.15)\n");
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
        ret = OCDoResource(NULL, OC_REST_GET, szQueryUri, 0, 0, (OC_ALL),
                OC_LOW_QOS, &cbData, NULL, 0);
    }

    if (ret != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, "OCStack resource error");
    }
    return ret;
}



const char * getIPAddr(const OCClientResponse * clientResponse)
{
    uint8_t a, b, c, d;
   if(!clientResponse || 0 != OCDevAddrToIPv4Addr(clientResponse->addr, &a, &b, &c, &d))
    {
        return "";
    }

    char * ipaddr = NULL;
    if((ipaddr = (char *) OCCalloc(1, MAX_IP_ADDR_ST_SZ)))
    {
        snprintf(ipaddr, MAX_IP_ADDR_ST_SZ, "%d.%d.%d.%d", a,b,c,d);
    }
    else
    {
        OC_LOG(ERROR, TAG, "Memory not allocated to ipaddr");
    }
    return ipaddr;
}

const char * getPort(const OCClientResponse * clientResponse)
{
    uint16_t p = 0;
    if(!clientResponse || 0 != OCDevAddrToPort(clientResponse->addr, &p) )
    {
        return "";
    }

    char * port = NULL;
    if((port = (char *) OCCalloc(1, MAX_PORT_ST_SZ)))
    {
        snprintf(port, MAX_PORT_ST_SZ, "%d", p);
    }
    else
    {
        OC_LOG(ERROR, TAG, "Memory not allocated to port");
    }
    return port;
}

int parseJSON(const char * resJSONPayload, char ** sid_c,
              char *** uri_c, int * totalRes)
{
    cJSON * root = NULL;
    cJSON * oc = NULL;

    root = cJSON_Parse((char *)(resJSONPayload));

    if (!root)
    {
        OC_LOG(ERROR, TAG, "JSON Parsing Error");
        return OC_STACK_INVALID_JSON;
    }

    oc = cJSON_GetObjectItem(root,"oc");
    if (!oc)
    {
        OC_LOG(ERROR, TAG, "Invalid JSON : Missing oc object");
        return OC_STACK_INVALID_JSON;
    }

    * totalRes = cJSON_GetArraySize(oc);

    if(oc->type == cJSON_Array)
    {
        cJSON * resource = cJSON_GetArrayItem(oc, 0);

        if(!resource)
        {
            return OC_STACK_INVALID_JSON;
        }

        if (cJSON_GetObjectItem(resource, "sid"))
        {
            char * sid = cJSON_GetObjectItem(resource, "sid")->valuestring;
            if((* sid_c = (char *)OCCalloc(1, strlen (sid) + 1)))
            {
                memcpy(* sid_c, sid, strlen(sid) + 1);
            }
            else
            {
                OC_LOG(ERROR, TAG, "Memory not allocated to sid");
                return OC_STACK_NO_MEMORY;
            }
        }
        else
        {
            OC_LOG(ERROR, TAG, "Invalid JSON : Missing sid object");
            return OC_STACK_INVALID_JSON;
        }

        if(!(* uri_c =  (char ** )OCMalloc ((* totalRes) * sizeof(char **))))
        {
            OC_LOG(ERROR, TAG, "Memory not allocated to sid_c array");
            return OC_STACK_NO_MEMORY;
        }

        int i = 0;

        while(true)
        {
            if (cJSON_GetObjectItem(resource, "href"))
            {
                char *uri= cJSON_GetObjectItem(resource, "href")->valuestring;
                if(((*uri_c)[i] = (char *)OCCalloc(1, strlen (uri) + 1)))
                {
                    memcpy((*uri_c)[i], uri, strlen(uri) + 1);
                }
                else
                {
                    OC_LOG(ERROR, TAG, "Memory not allocated to uri");
                    return OC_STACK_NO_MEMORY;
                }
                i++;
                if(i >= (* totalRes))
                    break;
                resource = cJSON_GetArrayItem(oc, i);
            }
            else
            {
               OC_LOG(ERROR, TAG, "Invalid JSON : Missing uri object");
               return OC_STACK_INVALID_JSON;
           }
        }
    }
    else
    {
        return OC_STACK_INVALID_JSON;
        OC_LOG(ERROR, TAG, "Invalid JSON : oc object type is not an array");
    }
    return OC_STACK_OK;
}

void queryResource()
{
    printf("\n");
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
    printf("\n");
}


void collectUniqueResource(const OCClientResponse * clientResponse)
{
    char * sid = NULL;
    char ** uri = NULL;
    int totalRes = 0;

    if(parseJSON(clientResponse->resJSONPayload, & sid, & uri, &totalRes)
            != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, "Error while parsing JSON payload in OCClientResponse");
        OCFree(sid);
        OCFree(uri);
        return;
    }

    int i;
    for(i = 0; i < totalRes; i++)
    {
        if(insertResource(sid, uri[i], clientResponse) == 1)
        {
            printf("%s%s%s%s\n",sid, ":", uri[i], " is new");
            printResourceList();
            queryResource();
        }
        else
        {
            printf("%s%s%s%s\n\n",sid, ":", uri[i], " has been seen before");
        }
    }

    OCFree(sid);
    OCFree(uri);
 }

/* This function searches for the resource(sid:uri) in the ResourceList.
 * If the Resource is found in the list then it returns 0 else insert
 * the resource to front of the list and returns 1.
 */
int insertResource(const char * sid, char const * uri,
            const OCClientResponse * clientResponse)
{
    ResourceNode * iter = resourceList;

    //Checking if the resource(sid:uri) is new
    while(iter)
    {
        if((strcmp(iter->sid, sid) == 0) && (strcmp(iter->uri, uri) == 0))
        {
            return 0;
        }
        else
        {
            iter = iter->next;
        }
    }

    //Creating new ResourceNode
    if((iter = (ResourceNode *) OCMalloc(sizeof(ResourceNode))))
    {
        iter->sid = sid;
        iter->uri = uri;
        iter->ip = getIPAddr(clientResponse);
        iter->port = getPort(clientResponse);
        iter->connType = clientResponse->connType;
        iter->next = NULL;
    }
    else
    {
        OC_LOG(ERROR, TAG, "Memory not allocated to ResourceNode");
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
    printf("\nResource List\n");
    while(iter)
    {
        printf("*****************************************************\n");
        printf("sid = %s \n",iter->sid);
        printf("uri = %s\n", iter->uri);
        printf("ip = %s\n", iter->ip);
        printf("port = %s\n", iter->port);
        switch (iter->connType)
        {
            case OC_IPV4:
                printf("connType = %s\n","IPv4");
                break;
            case OC_IPV6:
                // TODO: Allow IPv6 when support is added
                printf("IPv6 not currently supported, default to IPv4\n");
                //printf("connType = %s\n","IPv6");
                printf("connType = %s\n","IPv4");
                break;
            case OC_LE:
                printf("connType = %s\n","BLE");
                break;
            case OC_EDR:
                printf("connType = %s\n","BT");
                break;
            case OC_ALL:
            default:
                printf("connType = %s\n","Invalid connType");
                break;
        }
        printf("*****************************************************\n");
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
        OCFree((void *)temp->sid);
        OCFree((void *)temp->uri);
        OCFree((void *)temp->ip);
        OCFree((void *)temp->port);
        OCFree(temp);
    }
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
                // TODO: re-enable IPv4/IPv6 command line selection when IPv6 is supported
                // OC_CONNTYPE = OCConnectivityType(atoi(optarg));
                OC_CONNTYPE = OC_IPV4;
                OC_LOG(INFO, TAG, "Using default IPv4, IPv6 not currently supported.");
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

    freeResourceList();
    OC_LOG(INFO, TAG, "Exiting occlient main loop...");
    if (OCStop() != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, "OCStack stop error");
    }
    return 0;
}


