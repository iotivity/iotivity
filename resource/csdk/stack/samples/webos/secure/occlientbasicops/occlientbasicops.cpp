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

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <regex>
#include <iomanip>

using namespace std;

#define TAG "occlientbasicops"
#define MAX_RESOURCE_TYPE_SIZE      (32)
#define MAX_RESOURCES_REMEMBERED    (100)
#define MAX_USER_INPUT              (100)

pthread_t threadId_client;
PmLogContext gLogContext;

//Secure Virtual Resource database for Iotivity Client application
//It contains Client's Identity and the PSK credentials
//of other devices which the client trusts
static char CRED_FILE_CLIENT[] = "oic_svr_db_client.dat";
static const char INTROSPECTION_SWAGGER_FILE[] = "introspection_swagger.dat";

int gQuitFlag = 0;

OCDoHandle gDiscoverHandle;
static guint discoveryPollingSource = 0;

static LSHandle *pLsHandle = NULL;
static GMainLoop *mainloop = NULL;
static const char *gResourceUri = "/a/led";

typedef struct
{
    char uri[MAX_URI_LENGTH];
    char resourceType[MAX_RESOURCE_TYPE_SIZE];
    OCDevAddr address;

} DiscoveredResourceInfo;

static uint32_t DISC_RES_COUNTER = 0;
static DiscoveredResourceInfo DISC_RES[MAX_RESOURCES_REMEMBERED];
static bool PROMPT_USER = false;
static bool DISCOVERING = false;

static const char *gPlatformId = "0A3E0D6F-DBF5-404E-8719-D6880042463A";
static const char *gManufacturerName = "myName";
static const char *gManufacturerLink = "https://www.iotivity.org";
static const char *gModelNumber = "myModelNumber";
static const char *gDateOfManufacture = "2016-01-15";
static const char *gPlatformVersion = "myPlatformVersion";
static const char *gOperatingSystemVersion = "myOS";
static const char *gHardwareVersion = "myHardwareVersion";
static const char *gFirmwareVersion = "myFirmwareVersion";
static const char *gSupportLink = "https://www.iotivity.org";
static const char *gSystemTime = "2017-12-01T12:00:00.52Z";
static const char *gDeviceName = "Smart Home Client Device";
static const char *gDeviceType = "oic.wk.d";
static const char *gSpecVersion = "ocf.2.0.0";
static const char *gDataModelVersions = "ocf.res.1.3.0";
static const char *gProtocolIndependentID = "fa008167-3bbf-4c9d-8604-c9bcb96cb712";

OCPlatformInfo platformInfo;

void DeletePlatformInfo();
void initializePlatform();

OCStackResult SetDeviceInfo();

const char* decode_oc_stack_result (OCStackResult result)
{
    switch (result)
    {
        case OC_STACK_OK:
            return "OC_STACK_OK";
        case OC_STACK_RESOURCE_DELETED:
            return "OC_STACK_RESOURCE_DELETED";
        case OC_STACK_CONTINUE:
            return "OC_STACK_CONTINUE";
        case OC_STACK_RESOURCE_CREATED:
            return "OC_STACK_RESOURCE_CREATED";
        case OC_STACK_RESOURCE_CHANGED:
            return "OC_STACK_RESOURCE_CHANGED";
        case OC_STACK_INVALID_URI:
            return "OC_STACK_INVALID_URI";
        case OC_STACK_INVALID_QUERY:
            return "OC_STACK_INVALID_QUERY";
        case OC_STACK_INVALID_IP:
            return "OC_STACK_INVALID_IP";
        case OC_STACK_INVALID_PORT:
            return "OC_STACK_INVALID_PORT";
        case OC_STACK_INVALID_CALLBACK:
            return "OC_STACK_INVALID_CALLBACK";
        case OC_STACK_INVALID_METHOD:
            return "OC_STACK_INVALID_METHOD";
        case OC_STACK_INVALID_PARAM:
            return "OC_STACK_INVALID_PARAM";
        case OC_STACK_INVALID_OBSERVE_PARAM:
            return "OC_STACK_INVALID_OBSERVE_PARAM";
        case OC_STACK_NO_MEMORY:
            return "OC_STACK_NO_MEMORY";
        case OC_STACK_COMM_ERROR:
            return "OC_STACK_COMM_ERROR";
        case OC_STACK_TIMEOUT:
            return "OC_STACK_TIMEOUT";
        case OC_STACK_ADAPTER_NOT_ENABLED:
            return "OC_STACK_ADAPTER_NOT_ENABLED";
        case OC_STACK_NOTIMPL:
            return "OC_STACK_NOTIMPL";
        case OC_STACK_NO_RESOURCE:
            return "OC_STACK_NO_RESOURCE";
        case OC_STACK_RESOURCE_ERROR:
            return "OC_STACK_RESOURCE_ERROR";
        case OC_STACK_SLOW_RESOURCE:
            return "OC_STACK_SLOW_RESOURCE";
        case OC_STACK_DUPLICATE_REQUEST:
            return "OC_STACK_DUPLICATE_REQUEST";
        case OC_STACK_NO_OBSERVERS:
            return "OC_STACK_NO_OBSERVERS";
        case OC_STACK_OBSERVER_NOT_FOUND:
            return "OC_STACK_OBSERVER_NOT_FOUND";
        case OC_STACK_VIRTUAL_DO_NOT_HANDLE:
            return "OC_STACK_VIRTUAL_DO_NOT_HANDLE";
        case OC_STACK_INVALID_OPTION:
            return "OC_STACK_INVALID_OPTION";
        case OC_STACK_MALFORMED_RESPONSE:
            return "OC_STACK_MALFORMED_RESPONSE";
        case OC_STACK_PERSISTENT_BUFFER_REQUIRED:
            return "OC_STACK_PERSISTENT_BUFFER_REQUIRED";
        case OC_STACK_INVALID_REQUEST_HANDLE:
            return "OC_STACK_INVALID_REQUEST_HANDLE";
        case OC_STACK_INVALID_DEVICE_INFO:
            return "OC_STACK_INVALID_DEVICE_INFO";
        case OC_STACK_INVALID_JSON:
            return "OC_STACK_INVALID_JSON";
        case OC_STACK_UNAUTHORIZED_REQ:
            return "OC_STACK_UNAUTHORIZED_REQ";
        case OC_STACK_PDM_IS_NOT_INITIALIZED:
            return "OC_STACK_PDM_IS_NOT_INITIALIZED";
        case OC_STACK_DUPLICATE_UUID:
            return "OC_STACK_DUPLICATE_UUID";
        case OC_STACK_INCONSISTENT_DB:
            return "OC_STACK_INCONSISTENT_DB";
        case OC_STACK_AUTHENTICATION_FAILURE:
            return "OC_STACK_AUTHENTICATION_FAILURE";
        case OC_STACK_ERROR:
            return "OC_STACK_ERROR";
        case OC_STACK_TOO_LARGE_REQ:
            return "OC_STACK_TOO_LARGE_REQ";
        case OC_STACK_NOT_ALLOWED_OXM:
            return "OC_STACK_NOT_ALLOWED_OXM";
        case OC_STACK_BAD_ENDPOINT:
            return "OC_STACK_BAD_ENDPOINT";
        case OC_STACK_USER_DENIED_REQ:
            return "OC_STACK_USER_DENIED_REQ";
        case OC_STACK_NOT_ACCEPTABLE:
            return "OC_STACK_NOT_ACCEPTABLE";
        case OC_STACK_FORBIDDEN_REQ:
            return "OC_STACK_FORBIDDEN_REQ";
        case OC_STACK_INTERNAL_SERVER_ERROR:
            return "OC_STACK_INTERNAL_SERVER_ERROR";
        case OC_STACK_GATEWAY_TIMEOUT:
            return "OC_STACK_GATEWAY_TIMEOUT";
        case OC_STACK_SERVICE_UNAVAILABLE:
            return "OC_STACK_SERVICE_UNAVAILABLE";
    #ifdef WITH_PRESENCE
        case OC_STACK_PRESENCE_STOPPED:
            return "OC_STACK_PRESENCE_STOPPED";
        case OC_STACK_PRESENCE_TIMEOUT:
            return "OC_STACK_PRESENCE_TIMEOUT";
        case OC_STACK_PRESENCE_DO_NOT_HANDLE:
            return "OC_STACK_PRESENCE_DO_NOT_HANDLE";
    #endif
        default:
            return "Unknown result!";
    }
}

const char* decode_oc_eh_result (OCEntityHandlerResult result)
{
    switch(result)
    {
        case OC_EH_OK:
            return "OC_EH_OK";
        case OC_EH_ERROR:
            return "OC_EH_ERROR";
        case OC_EH_SLOW:
            return "OC_EH_SLOW";
        case OC_EH_RESOURCE_CREATED:
            return "OC_EH_RESOURCE_CREATED";
        case OC_EH_RESOURCE_DELETED:
            return "OC_EH_RESOURCE_DELETED";
        case OC_EH_VALID:
            return "OC_EH_VALID";
        case OC_EH_CHANGED:
            return "OC_EH_CHANGED";
        case OC_EH_CONTENT:
            return "OC_EH_CONTENT";
        case OC_EH_BAD_REQ:
            return "OC_EH_BAD_REQ";
        case OC_EH_UNAUTHORIZED_REQ:
            return "OC_EH_UNAUTHORIZED_REQ";
        case OC_EH_BAD_OPT:
            return "OC_EH_BAD_OPT";
        case OC_EH_FORBIDDEN:
            return "OC_EH_FORBIDDEN";
        case OC_EH_RESOURCE_NOT_FOUND:
            return "OC_EH_RESOURCE_NOT_FOUND";
        case OC_EH_METHOD_NOT_ALLOWED:
            return "OC_EH_METHOD_NOT_ALLOWED";
        case OC_EH_NOT_ACCEPTABLE:
            return "OC_EH_NOT_ACCEPTABLE";
        case OC_EH_TOO_LARGE:
            return "OC_EH_TOO_LARGE";
        case OC_EH_UNSUPPORTED_MEDIA_TYPE:
            return "OC_EH_UNSUPPORTED_MEDIA_TYPE";
        case OC_EH_INTERNAL_SERVER_ERROR:
            return "OC_EH_INTERNAL_SERVER_ERROR";
        case OC_EH_BAD_GATEWAY:
            return "OC_EH_BAD_GATEWAY";
        case OC_EH_SERVICE_UNAVAILABLE:
            return "OC_EH_SERVICE_UNAVAILABLE";
        case OC_EH_RETRANSMIT_TIMEOUT:
            return "OC_EH_RETRANSMIT_TIMEOUT";
        default:
            return "Unknown result!";
    }
}

const char* decode_oc_method (OCMethod method)
{
    switch(method)
    {
        case OC_REST_NOMETHOD:
            return "OC_REST_NOMETHOD";
        case OC_REST_GET:
            return "OC_REST_GET";
        case OC_REST_PUT:
            return "OC_REST_PUT";
        case OC_REST_POST:
            return "OC_REST_POST";
        case OC_REST_DELETE:
            return "OC_REST_DELETE";
        case OC_REST_OBSERVE:
            return "OC_REST_OBSERVE";
        case OC_REST_OBSERVE_ALL:
            return "OC_REST_OBSERVE_ALL";
        case OC_REST_DISCOVER:
            return "OC_REST_DISCOVER";
    #ifdef WITH_PRESENCE
        case OC_REST_PRESENCE:
            return "OC_REST_PRESENCE";
    #endif
        default:
            return "Unknown method";
    }
}

const char* decode_oc_eh_flag (OCEntityHandlerFlag flag)
{
    switch(flag)
    {
        case OC_REQUEST_FLAG:
            return "OC_REQUEST_FLAG";
        case OC_OBSERVE_FLAG:
            return "OC_OBSERVE_FLAG";
        default:
            return "Unknown oc eh flag";
    }
}

void DeletePlatformInfo()
{
    free(platformInfo.platformID);
    free(platformInfo.manufacturerName);
    free(platformInfo.manufacturerUrl);
    free(platformInfo.modelNumber);
    free(platformInfo.dateOfManufacture);
    free(platformInfo.platformVersion);
    free(platformInfo.operatingSystemVersion);
    free(platformInfo.hardwareVersion);
    free(platformInfo.firmwareVersion);
    free(platformInfo.supportUrl);
    free(platformInfo.systemTime);
}

bool DuplicateString(char** targetString, const char* sourceString)
{
    if(!sourceString)
    {
        return false;
    }
    else
    {
        *targetString = (char *) malloc(strlen(sourceString) + 1);

        if(*targetString)
        {
            strncpy(*targetString, sourceString, (strlen(sourceString) + 1));
            return true;
        }
    }
    return false;
}

static gboolean discoveryPollingCb(gpointer user_data)
{
    if (OCCancel(gDiscoverHandle, OC_HIGH_QOS, NULL, 0) != OC_STACK_OK)
    {
        OCSAMPLE_LOG_ERROR(TAG, 0, "Discovery cancel error");
    }
    else
    {
        OCSAMPLE_LOG_INFO(TAG, 0, "Discovery cancel successfully.");
    }

    DISCOVERING = false;
    g_source_remove(discoveryPollingSource);
    discoveryPollingSource = 0;

    return FALSE;
}

void StartDiscoveryPolling()
{
    if (discoveryPollingSource == 0)
    {
        discoveryPollingSource = g_timeout_add_seconds(5, discoveryPollingCb , NULL);
    }
}

void StopDiscoveryPolling()
{
    if (discoveryPollingSource != 0)
    {
        if (OCCancel(gDiscoverHandle, OC_HIGH_QOS, NULL, 0) != OC_STACK_OK)
        {
            OCSAMPLE_LOG_ERROR(TAG, 0, "Discovery cancel error");
        }
        else
        {
            OCSAMPLE_LOG_INFO(TAG, 0, "Discovery cancel successfully.");
        }

        g_source_remove(discoveryPollingSource);
        discoveryPollingSource = 0;
    }

}

OCStackResult SetDeviceInfo()
{
    OCStackResult result = OC_STACK_ERROR;

    // OC_RSRVD_DEVICE_URI = "/oic/d"
    OCResourceHandle resourceHandle = OCGetResourceHandleAtUri(OC_RSRVD_DEVICE_URI);
    if (resourceHandle == NULL)
    {
        OCSAMPLE_LOG_ERROR(TAG, 0, "Device Resource does not exist.");
        return result;
    }

    result = OCBindResourceTypeToResource(resourceHandle, gDeviceType);
    if (result != OC_STACK_OK)
    {
        OCSAMPLE_LOG_ERROR(TAG, 0, "Failed to add device type");
        return result;
    }
    // OC_RSRVD_DEVICE_NAME = "n"
    result = OCSetPropertyValue(PAYLOAD_TYPE_DEVICE, OC_RSRVD_DEVICE_NAME, gDeviceName);
    if (result != OC_STACK_OK)
    {
        OCSAMPLE_LOG_ERROR(TAG, 0, "Failed to set device name");
        return result;
    }
    // OC_RSRVD_DATA_MODEL_VERSION = "dmv"
    result = OCSetPropertyValue(PAYLOAD_TYPE_DEVICE, OC_RSRVD_DATA_MODEL_VERSION,
                                      gDataModelVersions);
    if (result != OC_STACK_OK)
    {
        OCSAMPLE_LOG_ERROR(TAG, 0, "Failed to set data model versions");
        return result;
    }
    // OC_RSRVD_SPEC_VERSION = "icv"
    result = OCSetPropertyValue(PAYLOAD_TYPE_DEVICE, OC_RSRVD_SPEC_VERSION, gSpecVersion);
    if (result != OC_STACK_OK)
    {
        OCSAMPLE_LOG_ERROR(TAG, 0, "Failed to set spec version");
        return result;
    }
    // OC_RSRVD_PROTOCOL_INDEPENDENT_ID = "piid"
    result = OCSetPropertyValue(PAYLOAD_TYPE_DEVICE, OC_RSRVD_PROTOCOL_INDEPENDENT_ID,
                                      gProtocolIndependentID);
    if (result != OC_STACK_OK)
    {
        OCSAMPLE_LOG_ERROR(TAG, 0, "Failed to set piid");
        return result;
    }

    OCSAMPLE_LOG_INFO(TAG, 0, "Device information initialized successfully.");
    return OC_STACK_OK;
}

OCStackResult SetPlatformInfo(const char* platformID, const char *manufacturerName,
    const char *manufacturerUrl, const char *modelNumber, const char *dateOfManufacture,
    const char *platformVersion, const char* operatingSystemVersion, const char* hardwareVersion,
    const char *firmwareVersion, const char* supportUrl, const char* systemTime)
{
    if((manufacturerName != NULL && (strlen(manufacturerName) > MAX_PLATFORM_NAME_LENGTH))
        || (manufacturerUrl != NULL && (strlen(manufacturerUrl) > MAX_PLATFORM_URL_LENGTH)))
    {
        return OC_STACK_INVALID_PARAM;
    }

    if(!DuplicateString(&platformInfo.platformID, platformID)
        || !DuplicateString(&platformInfo.manufacturerName, manufacturerName)
        || !DuplicateString(&platformInfo.manufacturerUrl, manufacturerUrl)
        || !DuplicateString(&platformInfo.modelNumber, modelNumber)
        || !DuplicateString(&platformInfo.dateOfManufacture, dateOfManufacture)
        || !DuplicateString(&platformInfo.platformVersion, platformVersion)
        || !DuplicateString(&platformInfo.operatingSystemVersion, operatingSystemVersion)
        || !DuplicateString(&platformInfo.hardwareVersion, hardwareVersion)
        || !DuplicateString(&platformInfo.firmwareVersion, firmwareVersion)
        || !DuplicateString(&platformInfo.supportUrl, supportUrl)
        || !DuplicateString(&platformInfo.systemTime, systemTime))
    {
        DeletePlatformInfo();
        return OC_STACK_ERROR;
    }

    return OC_STACK_OK;
}


void initializePlatform()
{
    OCSAMPLE_LOG_INFO(TAG, 0, "Running initializePlatform...");

    // initialize "oic/p"
    OCSAMPLE_LOG_INFO(TAG, 0, "oic/p");
    OCStackResult result = SetPlatformInfo(gPlatformId, gManufacturerName, gManufacturerLink,
            gModelNumber, gDateOfManufacture, gPlatformVersion, gOperatingSystemVersion,
            gHardwareVersion, gFirmwareVersion, gSupportLink, gSystemTime);
    if (result != OC_STACK_OK)
    {
        OCSAMPLE_LOG_ERROR(TAG, 0, "Platform info setting failed locally!");
        return;
    }

    result = OCSetPlatformInfo(platformInfo);
    if (result != OC_STACK_OK)
    {
        OCSAMPLE_LOG_ERROR(TAG, 0, "Platform Registration failed!");
        return;
    }

    // initialize "oic/d"
    OCSAMPLE_LOG_INFO(TAG, 0, "oic/d");
    result = SetDeviceInfo();
    if (result != OC_STACK_OK)
    {
        OCSAMPLE_LOG_ERROR(TAG, 0, "Device Registration failed!");
        return;
    }
}

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

FILE *client_fopen(const char *path, const char *mode)
{
    if (0 == strcmp(path, OC_SECURITY_DB_DAT_FILE_NAME))
    {
        return fopen(CRED_FILE_CLIENT, mode);
    }
    else if (0 == strcmp(path, OC_INTROSPECTION_FILE_NAME))
    {
        return fopen(INTROSPECTION_SWAGGER_FILE, mode);
    }

    else
    {
        return fopen(path, mode);
    }
}

static void
PrintDiscoveredResources(void)
{
    printf("\n");
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

void
RegisterDiscoveredResources(OCClientResponse *clientResponse)
{
    if (!(OCDiscoveryPayload*)clientResponse->payload)
    {
        OCSAMPLE_LOG_INFO(TAG, 0,
            "[%s] Could not discover any resources.", __func__);
        return;
    }
    OCResourcePayload* discoveredResource =
    ((OCDiscoveryPayload*)(clientResponse->payload))->resources;
    while (discoveredResource && discoveredResource != discoveredResource->next)
    {
        if (DISC_RES_COUNTER == MAX_RESOURCES_REMEMBERED)
        {
            OCSAMPLE_LOG_INFO(TAG, 0,
                "[%s] Max of %u resources reached. Ignoring the rest.",
                __func__, MAX_RESOURCES_REMEMBERED);
            break;
        }

        // 'oic/res' must be 1st resource to test out queries and its responses.
        if (DISC_RES_COUNTER == 0 && clientResponse->resourceUri)
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
        if (discoveredResource->eps && (discoveredResource->eps->family & OC_FLAG_SECURE))
        {
            if (0 == strcmp(discoveredResource->eps->tps, "coaps"))
            {
                strncpy(clientResponse->devAddr.addr,
                        discoveredResource->eps->addr,
                        sizeof(clientResponse->devAddr.addr));
                clientResponse->devAddr.port = discoveredResource->eps->port;
                clientResponse->devAddr.flags =
                (OCTransportFlags)(discoveredResource->eps->family | OC_SECURE);
                OCSAMPLE_LOG_INFO(TAG, 0, "[%s] DTLS port: %d",
                          __func__,
                          clientResponse->devAddr.port);
            }
        }
        ++DISC_RES_COUNTER;
        discoveredResource = discoveredResource->next;
    }

}

void
PrintFormattedResult(OCClientResponse *clientResponse)
{
    OCStackResult clientResult = clientResponse->result;
    printf("\n==========================================================\n");
    printf("Result: (%d) - %s\n", clientResult,
            decode_oc_stack_result(clientResult));
    OCSAMPLE_LOG_PAYLOAD(INFO, clientResponse->payload);
    printf("==========================================================\n");
}

OCStackApplicationResult
DiscoveryRequestCallBack(void* ctx,
                         OCDoHandle handle,
                         OCClientResponse * clientResponse)
{
    OC_UNUSED(handle);
    OC_UNUSED(ctx);

    if (!clientResponse)
    {
        OCSAMPLE_LOG_INFO(TAG, 0, "[%s] Client discovery response is NULL", __func__);
        printf("Discovery result is null.......\n");
        DISCOVERING = false;
        return OC_STACK_KEEP_TRANSACTION;
    }
    OCSAMPLE_LOG_INFO(TAG, 0, "[%s] Discovered addr:%s\tport:%d",
          __func__,
          clientResponse->devAddr.addr,
          clientResponse->devAddr.port);
    PrintFormattedResult(clientResponse);
    RegisterDiscoveredResources(clientResponse);

    PROMPT_USER = true;
    DISCOVERING = false;

    return OC_STACK_KEEP_TRANSACTION;
}

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

OCStackResult
InvokeOCDoRequest(const char *requestUri,
                  OCPayload *payload,
                  OCMethod method,
                  OCClientResponseHandler cb,
                  OCDevAddr *address)
{
    OCCallbackData cbData;

    cbData.cb = cb;
    cbData.context = (void*)DEFAULT_CONTEXT_VALUE;
    cbData.cd = NULL;

    OCConnectivityType connType = CT_ADAPTER_IP;
    OCQualityOfService qos = OC_HIGH_QOS;
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
        OCSAMPLE_LOG_ERROR(TAG, 0,
                  "[%s] OCDoRequest with method %s returns error (%d): %s",
                  __func__,
                  decode_oc_method(method),
                  ret,
                  decode_oc_stack_result(ret));
    }
    else if (method == OC_REST_DISCOVER)
    {
        gDiscoverHandle = handle;
        StartDiscoveryPolling();
    }

    return ret;
}

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
        case OC_REST_OBSERVE:
        {
            OIC_LOG_V(INFO, TAG,
                      "[%s] Initializing OBSERVE request for resource: %s",
                      __func__,
                      resourceUri);
            InvokeOCDoRequest(resourceUri,
                              NULL,
                              OC_REST_OBSERVE,
                              ResponseCallbacks,
                              &resourceAddr);
            break;
        }

        default:
            PROMPT_USER = true;
            OIC_LOG_V(INFO, TAG, "[%s] Invalid client request method (%d) %s",
                      __func__,
                      clientMethod,
                      decode_oc_method((OCMethod)clientMethod));
    }
}

static gboolean startClientCb(gpointer user_data)
{
    if (gQuitFlag)
        return FALSE;

    if (OCProcess() != OC_STACK_OK)
    {
        OCSAMPLE_LOG_ERROR(TAG, 0, "OCStack process error");
        return FALSE;
    }

    return TRUE;
}

void showMainMenu()
{
        printf("*********************************\n");
        printf("******** IoTivity Client ********\n");
        printf("*********************************\n");
        printf("Control the server remotely:\n");
        printf("1: Discover Resource\n");
        printf("99: quit the menu\n");
        printf("*********************************\n");
        printf("Select your choice:\n");
}

void showControlMenu()
{
    PrintDiscoveredResources();
    printf("#####################################################\n");
    printf("%d: GET\n", OC_REST_GET);
    printf("%d: POST\n", OC_REST_POST);
    printf("%d: OBSERVE\n", OC_REST_OBSERVE);
    printf("9999: Go back to main menu\n");
    printf("Usage:<resource number> <request method>\n");
    printf("#####################################################\n");
    printf("Select your choice:\n");
}

static gboolean userInputCb(GIOChannel *channel, GIOCondition cond, gpointer data)
{
    gchar *str_return;
    gsize length;
    gsize terminator_pos;
    GError *error = NULL;

    if (g_io_channel_read_line (channel, &str_return, &length, &terminator_pos, &error) == G_IO_STATUS_ERROR)
    {
        printf("Error occurred while reading input from command line\n");
    }
    if (error != NULL)
    {
        printf ("Error code: %s\n", error->message);
        exit(1);
    }

    if (PROMPT_USER && !DISCOVERING)
    {
        showControlMenu();

        if (!g_strcmp0(str_return, "9999\n"))
        {
            PROMPT_USER = false;
            DISC_RES_COUNTER = 0;
            showMainMenu();
            return TRUE;
        }
        else if (!g_strcmp0(str_return, "\n"))
        {
            return TRUE;
        }

        uint32_t resourceNo = 0;
        int requestMethod = 0;
        int inCount = sscanf(str_return, "%d %d", &resourceNo, &requestMethod);

        if (inCount != 2)
        {
            printf("Invalid input\n");
            PROMPT_USER = true;
            return TRUE;
        }
        if (resourceNo >= DISC_RES_COUNTER)
        {
            printf("Invalid resource\n");
            PROMPT_USER = true;
            return TRUE;
        }
        ProcessUserInput(resourceNo, requestMethod);
    }
    else if(!PROMPT_USER && !DISCOVERING)
    {
        if (!g_strcmp0(str_return, "1\n"))
        {    
            StopDiscoveryPolling();

            OCStackResult res = OC_STACK_OK;
            res = InvokeOCDoRequest(OC_RSRVD_WELL_KNOWN_URI, 0,
                              OC_REST_DISCOVER,
                              DiscoveryRequestCallBack,
                              0);
            DISCOVERING = true;
        }
        else if (!g_strcmp0(str_return, "99\n"))
        {
            gQuitFlag = true;
            g_main_loop_quit(mainloop);
        }
        else if (!g_strcmp0(str_return, "\n"))
        {
            showMainMenu();
        }
        else
        {
            printf("Invalid selection...\n");
            showMainMenu();
        }
    }
    g_free(str_return);
    return TRUE;
}

int main(int argc, char *argv[])
{
    int opt;
    OCPersistentStorage ps;

    OCStackResult res = OC_STACK_OK;

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

    // Initialize Persistent Storage for SVR database
    ps = { client_fopen, fread, fwrite, fclose, unlink };
    OCRegisterPersistentStorageHandler(&ps);

    /* Initialize OCStack*/
    if (OCInit(NULL, 0, OC_CLIENT_SERVER) != OC_STACK_OK)
    {
        OCSAMPLE_LOG_ERROR(TAG, 0, "OCStack init error");
        return 0;
    }

    initializePlatform();
    g_timeout_add_full(G_PRIORITY_HIGH, 100, startClientCb, NULL, NULL);

    GIOChannel *channel = g_io_channel_unix_new (STDIN_FILENO);
    g_io_add_watch (channel, G_IO_IN, userInputCb, NULL);
    showMainMenu();

    g_main_loop_run(mainloop);

    g_main_loop_unref(mainloop);

    OCSAMPLE_LOG_INFO(TAG, 0, "Exiting occlient main loop...");

    if (OCStop() != OC_STACK_OK)
    {
        OCSAMPLE_LOG_ERROR(TAG, 0, "OCStack stop error");
    }

    return 0;
}
