/******************************************************************
*
* Copyright 2018 Vprime All Rights Reserved.
*
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*****************************************************************
\author    Rami Alshafi <ralshafi@vprime.com>
*/
///////////////////////////////////////////////////////////////////////
//NOTE :  This sample server is generated based on IotivityandZigbeeClient.c
///////////////////////////////////////////////////////////////////////


#include <signal.h>
#include <unistd.h>
#include "payload_logging.h"
#include "utilities.c"
#include "ocstackconfig.h"

#define DEFAULT_CONTEXT_VALUE       (0x99)
#define MAX_RESOURCE_TYPE_SIZE      (32)
#define MAX_RESOURCES_REMEMBERED    (100)
#define MAX_USER_INPUT              (100)
#define TAG "CLIENT_APP"

static uint32_t DISC_RES_COUNTER = 0;
static bool PROMPT_USER = false;
static char SVR_DB[] = "./ocf_svr_db_client.dat";
static char DEV_PROP[] = "./device_properties.dat";

static bool STOP = false;
typedef struct
{
    char uri[MAX_URI_LENGTH];
    char resourceType[MAX_RESOURCE_TYPE_SIZE];
    OCDevAddr address;

} DiscoveredResourceInfo;

static DiscoveredResourceInfo DISC_RES[MAX_RESOURCES_REMEMBERED];


/**
* Utility method to print the list of discovered resources to the stdout
*/
static void
PrintDiscoveredResources(void)
{
    printf("=============================================================\n");
    printf("Discovered Resources:\n");
    for(uint32_t i = 0; i < DISC_RES_COUNTER; ++i)
    {
        DiscoveredResourceInfo currentResource = DISC_RES[i];

        printf("\t#%u: uri:%s\taddr:%s\tport: %u\tresource type: %s\n",
               i,
               currentResource.uri,
               currentResource.address.addr,
               currentResource.address.port,
               currentResource.resourceType);
    }
}


/**
* Utility method to format and print the response result to the stdout.
*
* @param[in] clientResponse contains the content to be printed.
*/
void
PrintFormattedResult(OCClientResponse *clientResponse)
{
    OCStackResult clientResult = clientResponse->result;
    printf("==========================================================\n");
    printf("Result: (%d) - %s\n", clientResult,
            decode_oc_stack_result(clientResult));
    OIC_LOG_PAYLOAD(INFO, clientResponse->payload);
    printf("==========================================================\n");
}


/**
* Method to register the discovered resources.
*
* @param[in] clientResponse containing info about discovered resources.
*/
void
RegisterDiscoveredResources(OCClientResponse *clientResponse)
{
    if (!(OCDiscoveryPayload*)clientResponse->payload)
    {
        OIC_LOG_V(INFO, TAG,
            "[%s] Could not discover any resources.", __func__);
        return;
    }
    OCResourcePayload* discoveredResource =
    ((OCDiscoveryPayload*)(clientResponse->payload))->resources;
    while (discoveredResource && discoveredResource != discoveredResource->next)
    {
        if (DISC_RES_COUNTER == MAX_RESOURCES_REMEMBERED)
        {
            OIC_LOG_V(WARNING, TAG,
                "[%s] Max of %u resources reached. Ignoring the rest.",
                __func__, MAX_RESOURCES_REMEMBERED);
            break;
        }
        // 'oic/res' must be 1st resource to test out queries and its responses.
        if (DISC_RES_COUNTER == 0)
        {
            strncpy(DISC_RES[DISC_RES_COUNTER].uri,
                    clientResponse->resourceUri, MAX_URI_LENGTH - 1);
            strncpy(DISC_RES[DISC_RES_COUNTER].resourceType,
                    OC_RSRVD_RESOURCE_TYPE_RES, MAX_RESOURCE_TYPE_SIZE - 1);
            DISC_RES[DISC_RES_COUNTER].address = clientResponse->devAddr;
            ++DISC_RES_COUNTER;
        }

        strncpy(DISC_RES[DISC_RES_COUNTER].uri,
            discoveredResource->uri, MAX_URI_LENGTH - 1);

        strncpy(DISC_RES[DISC_RES_COUNTER].resourceType,
            discoveredResource->types->value, MAX_RESOURCE_TYPE_SIZE - 1);
        DISC_RES[DISC_RES_COUNTER].address = clientResponse->devAddr;
        // secure the endpoint of the discovered secure resource.
        if (discoveredResource->eps->family & OC_FLAG_SECURE)
        {
            if (0 == strcmp(discoveredResource->eps->tps, "coaps"))
            {
                strncpy(clientResponse->devAddr.addr,
                        discoveredResource->eps->addr,
                        sizeof(clientResponse->devAddr.addr));
                clientResponse->devAddr.port = discoveredResource->eps->port;
                clientResponse->devAddr.flags =
                (OCTransportFlags)(discoveredResource->eps->family | OC_SECURE);
                OIC_LOG_V(INFO, TAG, "[%s] DTLS port: %d",
                          __func__,
                          clientResponse->devAddr.port);
            }
        }
        ++DISC_RES_COUNTER;
        discoveredResource = discoveredResource->next;
    }

}


/**
* Internal method to invoke the OCDoRequest API
*
* @param[in] requestUri resource URI used to invoke the request.
* @param[in] payload payload used to invoke the request
* @param[in] method the desired method to be used to invoke the request
* @param[in] cb the call back handler to be used to invoke the request
* @param[in] address the address to be used to invoke the request
*
* @return stack result
*/
OCStackResult
InvokeOCDoRequest(const char *requestUri,
                  OCPayload *payload,
                  OCMethod method,
                  OCClientResponseHandler cb,
                  OCDevAddr *address)
{
    OCCallbackData cbData = {
                                .context = (void*)DEFAULT_CONTEXT_VALUE,
                                .cb = cb
                            };

    OCConnectivityType connType = CT_ADAPTER_IP;
    OCQualityOfService qos = OC_LOW_QOS;
    OCDoHandle handle = NULL;
    uint8_t numOptions = 0;

    OCStackResult ret = OCDoRequest(&handle,
                                    method,
                                    requestUri,
                                    address,
                                    payload,
                                    connType,
                                    qos,
                                    &cbData,
                                    NULL,
                                    numOptions);
    if (ret != OC_STACK_OK)
    {
        PROMPT_USER = true;
        OIC_LOG_V(ERROR, TAG,
                  "[%s] OCDoRequest with method %s returns error (%d): %s",
                  __func__,
                  decode_oc_method(method),
                  ret,
                  decode_oc_stack_result(ret));
    }
    return ret;
}


/**
* The response call back method which prints the response to the stdout
* @param[in] ctx unused
* @param[in] handle unused
* @param[in] clientResponse the response that need to be printed
*
* @return stack application result
*/
OCStackApplicationResult
ResponseCallbacks(void* ctx,
                  OCDoHandle handle,
                  OCClientResponse * clientResponse)
{
    OC_UNUSED(handle);
    OC_UNUSED(ctx);
    if (clientResponse == NULL)
    {
        OIC_LOG_V(INFO, TAG, "[%s] Client request response is NULL", __func__);
        return   OC_STACK_DELETE_TRANSACTION;
    }
    PrintFormattedResult(clientResponse);
    PROMPT_USER = true;
    return OC_STACK_KEEP_TRANSACTION;
}


/**
* Call back method for discovery request that logs the response payload
* and registers the discovered resources.
*
* @param[in] ctx unused.
* @param[in] handle unused.
* @param[in] clientResponse discovery response with discovered resources info.
*
* @return stack application result
*/
OCStackApplicationResult
DiscoveryRequestCallBack(void* ctx,
                         OCDoHandle handle,
                         OCClientResponse * clientResponse)
{
    OC_UNUSED(handle);
    OC_UNUSED(ctx);
    if (!clientResponse)
    {
        OIC_LOG_V(INFO, TAG, "[%s] Client discovery response is NULL",
                  __func__);
        return OC_STACK_KEEP_TRANSACTION;
    }
    OIC_LOG_V(INFO, TAG, "[%s] Discovered addr:%s\tport:%d",
          __func__,
          clientResponse->devAddr.addr,
          clientResponse->devAddr.port);
    PrintFormattedResult(clientResponse);
    RegisterDiscoveredResources(clientResponse);

    PROMPT_USER = true;

    return OC_STACK_KEEP_TRANSACTION;
}


/**
* allows the user to create their own custom payload with however many
* key value pairs.
*
* @return payload
*/
OCPayload*
GetCustomPostPayload(void)
{
    OCRepPayload* payload = OCRepPayloadCreate();
    if (!payload)
    {
        OIC_LOG_V(ERROR, TAG, "[%s] Failed to create payload object", __func__);
        exit(1);
    }

    char key[MAX_USER_INPUT] = {0};
    char input[MAX_USER_INPUT] = {0};
    char valueString[MAX_USER_INPUT] = {0};
    int value = 0;
    double valueDouble = 0.0;
    int type = -1;
    printf("\nNeed to create a custom POST payload");
    printf("\nEnter key value pairs as:\t<type(int)> <key> <value>");
    printf("\nType: 0:bool \t 1:int \t 2:double \t 3:string\n");
    while (true)
    {
        printf("press ENTER to finish :");
        char *ret = fgets(input, sizeof(input), stdin);
        (void) ret;
        int inCount = sscanf(input, "%d %s %s", &type, key, valueString);

        if (inCount <= 0)
        {
            break;
        }
        if (inCount != 3)
        {
            printf("Invalid input\n");
            OCRepPayloadDestroy(payload);
            PROMPT_USER = true;
            return NULL;
        }

        if (type == 0)  //bool
        {
            if (sscanf(valueString, "%d", &value) == 1)
            {
                OCRepPayloadSetPropBool(payload, key, value);
            }
        }
        else if (type == 1)  //int
        {
            if (sscanf(valueString, "%d", &value) == 1)
            {
                OCRepPayloadSetPropInt(payload, key, value);
            }
        }
        else if (type == 2)  //double
        {
            if (sscanf(valueString, "%lf", &valueDouble) == 1)
            {
                OCRepPayloadSetPropDouble(payload, key, valueDouble);
            }
        }
        else if (type == 3)  //string
        {
            OCRepPayloadSetPropString(payload, key, valueString);
        }
        else
        {
            OIC_LOG_V(ERROR, TAG,
                      "[%s] Invalid entry. stopping accepting key-values",
                      __func__);
            OCRepPayloadDestroy(payload);
            PROMPT_USER = true;
            return NULL;
        }
        memset(input, 0, sizeof (input));
        memset(key, 0, sizeof (key));
        memset(valueString, 0, sizeof (valueString));
    }

    if (payload->values)
    {
        return (OCPayload *) payload;
    }
    else
    {
        OCRepPayloadDestroy(payload);
        return NULL;
    }
}


/**
* Processes the user input to figure out which request to initialize.
*
* @param[in] resourceNo the number of discovered resource.
* @param[in] clientMethod the method the user wants to initiate.
*/
void
ProcessUserInput(int resourceNo, int clientMethod)
{
    const char *resourceUri = DISC_RES[resourceNo].uri;
    OCDevAddr resourceAddr = DISC_RES[resourceNo].address;
    switch (clientMethod)
    {
        case OC_REST_GET:
            OIC_LOG_V(INFO, TAG,
                      "[%s] Initializing GET request for resource: %s",
                      __func__,
                      resourceUri);
            InvokeOCDoRequest(resourceUri,
                              NULL,
                              OC_REST_GET,
                              ResponseCallbacks,
                              &resourceAddr);
            break;

        case OC_REST_POST:
        {
            OIC_LOG_V(INFO, TAG,
                      "[%s] Initializing POST request for resource: %s",
                      __func__,
                      resourceUri);
            OCPayload *payload = GetCustomPostPayload();
            if (payload)
            {
                InvokeOCDoRequest(resourceUri,
                                  payload,
                                  OC_REST_POST,
                                  ResponseCallbacks,
                                  &resourceAddr);
            }
            else
            {
                OIC_LOG_V(ERROR, TAG,
                          "[%s] Error creating POST payload. Aborting",
                          __func__);
                PROMPT_USER = true;
            }
            break;
        }

        default:
            PROMPT_USER = true;
            OIC_LOG_V(INFO, TAG, "[%s] Invalid client request method (%d) %s",
                      __func__,
                      clientMethod,
                      decode_oc_method(clientMethod));
    }
}


/**
* Utility method to capture the user input.
*/
void
GetUserInput(void)
{
    PrintDiscoveredResources();
    printf("\nRequest Methods:\t%d: GET\t%d: POST", OC_REST_GET, OC_REST_POST);
    printf("\nUsage:<resource number> <request method> :");

    char input[10] = {0};
    uint32_t resourceNo = 0;
    int requestMethod = 0;

    char * ret = fgets(input, sizeof(input), stdin);
    (void) ret;
    int inCount = sscanf(input, "%d %d", &resourceNo, &requestMethod);

    if (inCount != 2)
    {
        printf("Invalid input\n");
        PROMPT_USER = true;
        return;
    }
    if (resourceNo >= DISC_RES_COUNTER)
    {
        printf("Invalid resource\n");
        PROMPT_USER = true;
        return;
    }
    ProcessUserInput(resourceNo, requestMethod);
}


/**
* SIGINT handler call back method to update the STOP flag to exit the main loop.
*
* @param[in] signalNumber
*/
void
SIGINTHandlerCallBack(int signalNumber)
{
    OIC_LOG_V(INFO, TAG, "[%s] Received SIGINT", __func__);
    if (signalNumber == SIGINT)
    {
        STOP = true;
    }
}


/**
* Custom fopen method to open the secure virtual resource database and
* the device properties.
*
* @param[in] file file name to be opened
* @param[in] mode the mode in which the file will be opened
*
* @return opened file
*/
FILE*
ClientFOpen(const char *file, const char *mode)
{
    if (0 == strcmp(file, DEV_PROP))
    {
        return fopen(DEV_PROP, mode);
    }
    else if (0 == strcmp(file, OC_SECURITY_DB_DAT_FILE_NAME))
    {
        return fopen(SVR_DB, mode);
    }
    else
    {
        return fopen(file, mode);
    }
}


/**
* main function
*/
int
main(void)
{

    OCStackResult stack_res;
    OIC_LOG_V(DEBUG, TAG,
              "[%s] Initializing and registering persistent storage",
              __func__);
    OCPersistentStorage ps = {ClientFOpen, fread, fwrite, fclose, unlink};
    OCRegisterPersistentStorageHandler(&ps);
    OIC_LOG_V(DEBUG, TAG,
              "[%s] Initializing IoTivity stack for client_server",
              __func__);
    stack_res = OCInit(NULL, 0, OC_CLIENT_SERVER);
    // notice you need client_server, not client in order to enable security.
    if (stack_res != OC_STACK_OK)
    {
        OIC_LOG_V(ERROR, TAG,
                  "[%s] Failed to initialize IoTivity stack (%d): %s",
                  __func__,
                  stack_res,
                  decode_oc_stack_result(stack_res));
        return -1;
    }
    // initiate discovery request
    stack_res = InvokeOCDoRequest(OC_RSRVD_WELL_KNOWN_URI,
                                  0,
                                  OC_REST_DISCOVER,
                                  DiscoveryRequestCallBack,
                                  0);
    if (stack_res != OC_STACK_OK)
    {
        OIC_LOG_V(ERROR, TAG,
                  "[%s] Failed to initiate discovery request (%d): %s",
                  __func__,
                  stack_res,
                  decode_oc_stack_result(stack_res));
        return -1;
    }
    OIC_LOG_V(INFO, TAG,
              "[%s] Client is running, press ctrl+c to STOP...",
              __func__);
    signal(SIGINT, SIGINTHandlerCallBack);
    while (!STOP)
    {
        stack_res = OCProcess();
        if (stack_res != OC_STACK_OK)
        {
            OIC_LOG_V(ERROR, TAG,
                      "[%s] IoTivity stack process failure (%d): %s",
                      __func__,
                      stack_res,
                      decode_oc_stack_result(stack_res));
            return -1;
        }

        if (PROMPT_USER)
        {
            PROMPT_USER = false;
            GetUserInput();
        }
    }

    OIC_LOG_V(INFO, TAG, "[%s] stopping IoTivity client...", __func__);
    stack_res = OCStop();
    if (stack_res != OC_STACK_OK)
    {
        OIC_LOG_V(ERROR, TAG,
                  "[%s] Failed to STOP IoTivity client (%d): %s",
                  __func__,
                  stack_res,
                  decode_oc_stack_result(stack_res));
        return -1;
    }

    return 0;
}
