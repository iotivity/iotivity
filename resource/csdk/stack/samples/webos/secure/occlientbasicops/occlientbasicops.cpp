/*******************************************************************
 *
 * Copyright (c) 2018 LG Electronics, Inc.
 * Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

#include "iotivity_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <iostream>
#include <sstream>
#include <getopt.h>
#include "ocstack.h"
#include "occlientbasicops.h"
#include "ocpayload.h"
#include "sample_payload_logging.h"
#include "oic_string.h"
#include "common.h"


/// This example is using experimental API, so there is no guarantee of support for future release,
/// nor any there any guarantee that breaking changes will not occur across releases.
#include "experimental/logger.h"

#include <luna-service2/lunaservice.h>
#include <pbnjson.h>
#include <pthread.h>
#include "logging.h"

pthread_t threadId_client;
PmLogContext gLogContext;

#define TAG "occlientbasicops"
static int UnicastDiscovery = 0;
static int TestCase = 0;
static int ConnType = 0;
static int DevOwner = 0;
static int WithTcp = 0;

static char DISCOVERY_QUERY[] = "%s/oic/res";
OCConnectivityType discoveryReqConnType = CT_ADAPTER_IP;

static std::string coapServerResource;
static int coapSecureResource;
static OCConnectivityType ocConnType;

//Secure Virtual Resource database for Iotivity Client application
//It contains Client's Identity and the PSK credentials
//of other devices which the client trusts
static char CRED_FILE_DEVOWNER[] = "oic_svr_db_client_devowner.dat";
static char CRED_FILE_NONDEVOWNER[] = "oic_svr_db_client_nondevowner.dat";

//Standard uri prefix for secure virtual resources
const char * OIC_STD_URI_PREFIX = "/oic/";

const char * COAPS_STR = "coaps";
#ifdef __WITH_TLS__
const char * COAPS_TCP_STR = "coaps+tcp";
#endif

int gQuitFlag = 0;
const char * COAPS_TCP_STR = "coaps+tcp";

static LSHandle *pLsHandle = NULL;
static GMainLoop *mainloop = NULL;
static const char *gResourceUri = "/a/led";

/* SIGINT handler: set gQuitFlag to 1 for graceful termination */
void handleSigInt(int signum)
{
    if (signum == SIGINT)
    {
        gQuitFlag = 1;
        g_main_loop_quit(mainloop);
    }
}

OCPayload *putPayload()
{
    OCRepPayload *payload = OCRepPayloadCreate();

    if (!payload)
    {
        std::cout << "Failed to create put payload object" << std::endl;
        std::exit(1);
    }

    OCRepPayloadSetPropInt(payload, "power", 15);
    OCRepPayloadSetPropBool(payload, "state", true);

    return (OCPayload *) payload;
}

static void PrintUsage()
{
    printf("Usage : occlient -u <0|1> -t <1|2|3> -c <0|1>\n");
    printf("-u <0|1> : Perform multicast/unicast discovery of resources\n");
    printf("-t 1 : Discover Resources\n");
    printf("-t 2 : Discover Resources and\n"
            " Initiate Nonconfirmable Get/Put/Post Requests\n");
    printf("-t 3 : Discover Resources and Initiate Confirmable Get/Put/Post Requests\n");
    printf("-c 0 : Default auto-selection\n");
    printf("-c 1 : IP Connectivity Type\n");
    printf("-d 0 : Client as Non Device Owner\n");
    printf("-d 1 : Client as Device Owner\n");
    printf("-p 0 : Use UDP protocol\n");
    printf("-p 1 : Use TCP protocol\n");
}

OCStackResult InvokeOCDoResource(std::ostringstream &query,
                                 OCMethod method,
                                 const OCDevAddr *dest,
                                 OCQualityOfService qos,
                                 OCClientResponseHandler cb,
                                 OCHeaderOption *options, uint8_t numOptions)
{
    OCStackResult ret;
    OCCallbackData cbData;

    cbData.cb = cb;
    cbData.context = NULL;
    cbData.cd = NULL;

    OCPayload *payload = (method == OC_REST_PUT || method == OC_REST_POST) ? putPayload() : NULL;

    ret = OCDoRequest(NULL, method, query.str().c_str(), dest,
                      payload, ocConnType, qos, &cbData, options, numOptions);

    OCPayloadDestroy(payload);

    if (ret != OC_STACK_OK)
    {
        OCSAMPLE_LOG_ERROR(TAG, 0, "OCDoResource returns error %d with method %d", ret, method);
    }

    return ret;
}

OCStackApplicationResult putReqCB(void *, OCDoHandle, OCClientResponse *clientResponse)
{
    OCSAMPLE_LOG_INFO(TAG, 0, "Callback Context for PUT recvd successfully");

    if (clientResponse)
    {
        OCSAMPLE_LOG_INFO(TAG, 0, "StackResult: %s",  getResult(clientResponse->result));
        OCSAMPLE_LOG_PAYLOAD(INFO, clientResponse->payload);
        OCSAMPLE_LOG_INFO(TAG, 0, "=============> Put Response");
    }
    return OC_STACK_DELETE_TRANSACTION;
}

OCStackApplicationResult postReqCB(void *, OCDoHandle, OCClientResponse *clientResponse)
{
    OCSAMPLE_LOG_INFO(TAG, 0, "Callback Context for POST recvd successfully");

    if (clientResponse)
    {
        OCSAMPLE_LOG_INFO(TAG, 0, "StackResult: %s",  getResult(clientResponse->result));
        OCSAMPLE_LOG_PAYLOAD(INFO, clientResponse->payload);
        OCSAMPLE_LOG_INFO(TAG, 0, "=============> Post Response");
    }
    return OC_STACK_DELETE_TRANSACTION;
}

OCStackApplicationResult getReqCB(void *, OCDoHandle, OCClientResponse *clientResponse)
{
    OCSAMPLE_LOG_INFO(TAG, 0, "Callback Context for GET query recvd successfully");

    if (clientResponse)
    {
        OCSAMPLE_LOG_INFO(TAG, 0, "StackResult: %s",  getResult(clientResponse->result));
        OCSAMPLE_LOG_INFO(TAG, 0, "SEQUENCE NUMBER: %d", clientResponse->sequenceNumber);
        OCSAMPLE_LOG_PAYLOAD(INFO, clientResponse->payload);
        OCSAMPLE_LOG_INFO(TAG, 0, "=============> Get Response");
    }
    return OC_STACK_DELETE_TRANSACTION;
}

// This is a function called back when a device is discovered
OCStackApplicationResult discoveryReqCB(void *, OCDoHandle,
                                        OCClientResponse *clientResponse)
{
    OCSAMPLE_LOG_INFO(TAG, 0, "Callback Context for DISCOVER query recvd successfully");

    if (clientResponse)
    {
        OCSAMPLE_LOG_INFO(TAG, 0, "StackResult: %s", getResult(clientResponse->result));
        OCSAMPLE_LOG_INFO(TAG, 0,
                  "Device =============> Discovered @ %s:%d",
                  clientResponse->devAddr.addr,
                  clientResponse->devAddr.port);

        if (clientResponse->result == OC_STACK_OK)
        {
            OCSAMPLE_LOG_PAYLOAD(INFO, clientResponse->payload);

            ocConnType = clientResponse->connType;

            if (parseClientResponse(clientResponse) != -1)
            {
                OCDiscoveryPayload *payload = (OCDiscoveryPayload *) clientResponse->payload;
                OCResourcePayload *resource = (OCResourcePayload *) payload->resources;
                for (;resource; resource = resource->next)
                {
                    if ((0 == strcmp(gResourceUri, resource->uri))
                         && (0 == strcmp(COAPS_STR, resource->eps->tps)))
                    {
                        OCDevAddr* endpoint = &clientResponse->devAddr;
                        strcpy(endpoint->addr, resource->eps->addr);
                        endpoint->port = resource->eps->port;
                        endpoint->flags = resource->eps->family;

                        switch (TestCase)
                        {
                        case TEST_NON_CON_OP:
                            InitGetRequest(endpoint, OC_LOW_QOS);
                            InitPutRequest(endpoint, OC_LOW_QOS);
                            InitPostRequest(endpoint, OC_LOW_QOS);
                            break;
                        case TEST_CON_OP:
                            InitGetRequest(endpoint, OC_HIGH_QOS);
                            InitPutRequest(endpoint, OC_HIGH_QOS);
                            InitPostRequest(endpoint, OC_HIGH_QOS);

                        break;
                        }
                    }
                }
            }
        }
    }

    return (UnicastDiscovery) ? OC_STACK_DELETE_TRANSACTION : OC_STACK_KEEP_TRANSACTION ;

}
int InitPutRequest(OCDevAddr *endpoint, OCQualityOfService qos)
{
    OCSAMPLE_LOG_INFO(TAG, 0, "Executing %s", __func__);
    std::ostringstream query;
    query << coapServerResource;

    return (InvokeOCDoResource(query, OC_REST_PUT, endpoint,
                               ((qos == OC_HIGH_QOS) ? OC_HIGH_QOS : OC_LOW_QOS), putReqCB, NULL, 0));
}

int InitPostRequest(OCDevAddr *endpoint, OCQualityOfService qos)
{
    OCStackResult result;

    OCSAMPLE_LOG_INFO(TAG, 0, "Executing %s", __func__);
    std::ostringstream query;
    query << coapServerResource;

    // First PUT operation (to create an LED instance)
    result = InvokeOCDoResource(query, OC_REST_PUT, endpoint,
                                ((qos == OC_HIGH_QOS) ? OC_HIGH_QOS : OC_LOW_QOS),
                                putReqCB, NULL, 0);
    if (OC_STACK_OK != result)
    {
        // Error can happen if for example, network connectivity is down
        OCSAMPLE_LOG_INFO(TAG, 0, "First POST call did not succeed");
    }

    // Second PUT operation (to create an LED instance)
    result = InvokeOCDoResource(query, OC_REST_PUT, endpoint,
                                ((qos == OC_HIGH_QOS) ? OC_HIGH_QOS : OC_LOW_QOS),
                                putReqCB, NULL, 0);
    if (OC_STACK_OK != result)
    {
        OCSAMPLE_LOG_INFO(TAG, 0, "Second POST call did not succeed");
    }

    // This POST operation will update the original resourced /a/led (as long as
    // the server is set to max 2 /lcd resources)
    result = InvokeOCDoResource(query, OC_REST_POST, endpoint,
                                ((qos == OC_HIGH_QOS) ? OC_HIGH_QOS : OC_LOW_QOS),
                                postReqCB, NULL, 0);
    if (OC_STACK_OK != result)
    {
        OCSAMPLE_LOG_INFO(TAG, 0, "Third POST call did not succeed");
    }
    return result;
}

int InitGetRequest(OCDevAddr *endpoint, OCQualityOfService qos)
{
    OCSAMPLE_LOG_INFO(TAG, 0, "Executing %s", __func__);
    std::ostringstream query;
    query << coapServerResource;

    return (InvokeOCDoResource(query, OC_REST_GET, endpoint,
                               ((qos == OC_HIGH_QOS) ?  OC_HIGH_QOS : OC_LOW_QOS),
                               getReqCB, NULL, 0));
}

int InitDiscovery()
{
    OCStackResult ret;
    OCCallbackData cbData;
    char queryUri[200];
    char ipaddr[100] = { '\0' };

    if (UnicastDiscovery)
    {
        OCSAMPLE_LOG_INFO(TAG, 0, "Enter IP address (with optional port) of the Server hosting resource\n");
        OCSAMPLE_LOG_INFO(TAG, 0, "IPv4: 192.168.0.15:45454\n");
        OCSAMPLE_LOG_INFO(TAG, 0, "IPv6: [fe80::20c:29ff:fe1b:9c5]:45454\n");

        if (fgets(ipaddr, sizeof (ipaddr), stdin))
        {
            StripNewLineChar(ipaddr); //Strip newline char from ipaddr
        }
        else
        {
            OCSAMPLE_LOG_ERROR(TAG, 0, "!! Bad input for IP address. !!");
            return OC_STACK_INVALID_PARAM;
        }
    }
    snprintf(queryUri, sizeof (queryUri), DISCOVERY_QUERY, ipaddr);

    cbData.cb = discoveryReqCB;
    cbData.context = NULL;
    cbData.cd = NULL;

    /* Start a discovery query*/
    OCSAMPLE_LOG_INFO(TAG, 0, "Initiating %s Resource Discovery : %s\n",
              (UnicastDiscovery) ? "Unicast" : "Multicast",
              queryUri);

    ret = OCDoRequest(NULL, OC_REST_DISCOVER, queryUri, 0, 0, CT_DEFAULT,
                      OC_LOW_QOS, &cbData, NULL, 0);
    if (ret != OC_STACK_OK)
    {
        OCSAMPLE_LOG_ERROR(TAG, 0, "OCStack resource error");
    }
    return ret;
}

FILE *client_fopen_devowner(const char *path, const char *mode)
{
    if (0 == strcmp(path, OC_SECURITY_DB_DAT_FILE_NAME))
    {
        return fopen(CRED_FILE_DEVOWNER, mode);
    }
    else
    {
        return fopen(path, mode);
    }
}

FILE *client_fopen_nondevowner(const char *path, const char *mode)
{
    if (0 == strcmp(path, OC_SECURITY_DB_DAT_FILE_NAME))
    {
        return fopen(CRED_FILE_NONDEVOWNER, mode);
    }
    else
    {
        return fopen(path, mode);
    }
}

static gboolean clientStarter(gpointer user_data)
{
    if (!gQuitFlag)
    {   
        if (OCProcess() != OC_STACK_OK)
        {
            OCSAMPLE_LOG_ERROR(TAG, 0, "OCStack process error");
        }
        return TRUE;
    }   
    else
    {   
        OCSAMPLE_LOG_INFO(TAG, 0, "Stopping clientStarter loop...");
        return FALSE;
    }   
}

int main(int argc, char *argv[])
{
    int opt;
    struct timespec timeout;
    OCPersistentStorage ps;

    LSError lserror;
    LSErrorInit(&lserror);

    (void) PmLogGetContext("OCCLIENTBASICOPS", &gLogContext);

    // Initialize g_main_loop
    mainloop = g_main_loop_new(NULL, FALSE);
    if (!mainloop) {
        OCSAMPLE_LOG_ERROR(TAG, 0, "Failed to create main loop");
        return 0;
    }

    if (!LSRegister("org.ocf.webossample.occlientbasicops", &pLsHandle, &lserror)) {
        OCSAMPLE_LOG_ERROR(TAG, 0, "Failed to register LS Handle");
        LSErrorLog(gLogContext, "LS_SRVC_ERROR", &lserror);
        return 0;
    }

    if (!LSGmainAttach(pLsHandle, mainloop, &lserror)) {
        OCSAMPLE_LOG_ERROR(TAG, 0, "Failed to attach main loop");
        LSErrorLog(gLogContext, "LS_SRVC_ATTACH_ERROR", &lserror);
        return 0;
    }

    while ((opt = getopt(argc, argv, "u:t:c:d:p:")) != -1)
    {
        switch (opt)
        {
            case 'u':
                UnicastDiscovery = atoi(optarg);
                break;
            case 't':
                TestCase = atoi(optarg);
                break;
            case 'c':
                ConnType = atoi(optarg);
                break;
            case 'd':
                DevOwner = atoi(optarg);
                break;
            case 'p':
                {
                    WithTcp = atoi(optarg);
                    if (WithTcp > 1)
                    {
                        PrintUsage();
                        return -1;
                    }
                }
                break;
            default:
                PrintUsage();
                return -1;
        }
    }

    if ((UnicastDiscovery != 0 && UnicastDiscovery != 1) ||
        (TestCase < TEST_DISCOVER_REQ || TestCase >= MAX_TESTS) ||
        (ConnType < CT_ADAPTER_DEFAULT || ConnType >= MAX_CT))
    {
        PrintUsage();
        return -1;
    }


    if (ConnType == CT_ADAPTER_DEFAULT || ConnType ==  CT_IP)
    {
        discoveryReqConnType = CT_DEFAULT;
    }
    else
    {
        OCSAMPLE_LOG_INFO(TAG, 0, "Using Default Connectivity type");
        PrintUsage();
    }


    // Initialize Persistent Storage for SVR database
    if (DevOwner)
        ps = { client_fopen_devowner, fread, fwrite, fclose, unlink };
    else
        ps = { client_fopen_nondevowner, fread, fwrite, fclose, unlink };
    OCRegisterPersistentStorageHandler(&ps);

    /* Initialize OCStack*/
    if (OCInit(NULL, 0, OC_CLIENT_SERVER) != OC_STACK_OK)
    {
        OCSAMPLE_LOG_ERROR(TAG, 0, "OCStack init error");
        return 0;
    }

    InitDiscovery();

    timeout.tv_sec  = 0;
    timeout.tv_nsec = 100000000L;

    // Break from loop with Ctrl+C
    OCSAMPLE_LOG_INFO(TAG, 0, "Entering clientStarter main loop...");
    signal(SIGINT, handleSigInt);
    g_timeout_add_seconds(1, clientStarter, NULL);
    g_main_loop_run(mainloop);

    OCSAMPLE_LOG_INFO(TAG, 0, "Exiting occlient main loop...");

    if (OCStop() != OC_STACK_OK)
    {
        OCSAMPLE_LOG_ERROR(TAG, 0, "OCStack stop error");
    }

    return 0;
}

int parseClientResponse(OCClientResponse *clientResponse)
{
    OCResourcePayload *res = ((OCDiscoveryPayload *)clientResponse->payload)->resources;

    // Initialize all global variables
    coapServerResource.clear();
    coapSecureResource = 0;

    while (res)
    {
        coapServerResource.assign(res->uri);
        OCSAMPLE_LOG_INFO(TAG, 0, "Uri -- %s", coapServerResource.c_str());

        if (0 == strncmp(coapServerResource.c_str(), OIC_STD_URI_PREFIX, strlen(OIC_STD_URI_PREFIX)) ||
            0 == strncmp(coapServerResource.c_str(), "/introspection", strlen("/introspection")))
        {
            OCSAMPLE_LOG_INFO(TAG, 0, "Skip resource");
            res = res->next;
            continue;
        }

        OCDevAddr *endpoint = &clientResponse->devAddr;
        if (res && res->eps)
        {
            endpoint->port = 0;
            OCEndpointPayload* eps = res->eps;
            while (NULL != eps)
            {
                if (eps->family & OC_FLAG_SECURE)
                {
#ifdef __WITH_TLS__
                    if (WithTcp && 0 == strcmp(eps->tps, COAPS_TCP_STR))
                    {
                        strncpy(endpoint->addr, eps->addr, sizeof(endpoint->addr));
                        endpoint->port = eps->port;
                        endpoint->flags = (OCTransportFlags)(eps->family | OC_SECURE);
                        endpoint->adapter = OC_ADAPTER_TCP;
                        coapSecureResource = 1;
                        OCSAMPLE_LOG_INFO(TAG, 0, "TLS port: %d", endpoint->port);
                        break;
                    }
#endif
                    if (!WithTcp && 0 == strcmp(eps->tps, COAPS_STR))
                    {
                        strncpy(endpoint->addr, eps->addr, sizeof(endpoint->addr));
                        endpoint->port = eps->port;
                        endpoint->flags = (OCTransportFlags)(eps->family | OC_SECURE);
                        endpoint->adapter = OC_ADAPTER_IP;
                        coapSecureResource = 1;
                        OCSAMPLE_LOG_INFO(TAG, 0, "DTLS port: %d", endpoint->port);
                    }
                }
                eps = eps->next;
            }
            if (!endpoint->port)
            {
                OCSAMPLE_LOG_INFO(TAG, 0, "Can not find secure port information.");
            }
        }

        //old servers support
        if (0 == coapSecureResource && res->secure)
        {
#ifdef __WITH_TLS__
            if (WithTcp)
            {
                endpoint->flags = (OCTransportFlags)(endpoint->flags | OC_SECURE);
                endpoint->adapter = OC_ADAPTER_TCP;
                endpoint->port = res->tcpPort;
                OCSAMPLE_LOG_INFO(TAG, 0, "TLS port: %d", endpoint->port);
            }
            else
#endif
            {
                endpoint->port = res->port;
                endpoint->flags = (OCTransportFlags)(endpoint->flags | OC_SECURE);
                endpoint->adapter = OC_ADAPTER_IP;
                OCSAMPLE_LOG_INFO(TAG, 0, "DTLS port: %d", endpoint->port);
            }
            coapSecureResource = 1;
        }

        OCSAMPLE_LOG_INFO(TAG, 0, "Secure -- %s", coapSecureResource == 1 ? "YES" : "NO");

        // If we discovered a secure resource, exit from here
        if (coapSecureResource)
        {
            break;
        }

        res = res->next;
    }

    return 0;
}
