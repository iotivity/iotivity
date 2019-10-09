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
#include <string.h>
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <signal.h>
#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif
#include "ocstack.h"
#include "ocpayload.h"
#include "ocserverbasicops.h"
#include "common.h"


/// This example is using experimental API, so there is no guarantee of support for future release,
/// nor any there any guarantee that breaking changes will not occur across releases.
#include "experimental/logger.h"

#include <luna-service2/lunaservice.h>
#include <glib.h>
#include <pbnjson.h>
#include <pthread.h>
#include "logging.h"

#define SAMPLE_MAX_NUM_POST_INSTANCE  2
#define SAMPLE_MAX_NUM_OBSERVATIONS     8

/* Structure to represent the observers */
typedef struct
{
    OCObservationId observationId;
    bool valid;
    OCResourceHandle resourceHandle;
} Observers;

pthread_t threadId_server;
PmLogContext gLogContext;
PmLogContext gLogLibContext;

int gQuitFlag = 0;

static LEDResource LED;
// This variable determines instance number of the LED resource.
// Used by POST method to create a new instance of LED resource.
static int gCurrLedInstance = 0;
static LEDResource gLedInstance[SAMPLE_MAX_NUM_POST_INSTANCE];

char *gResourceUri= (char *)"/binaryswitch";

//Secure Virtual Resource database for Iotivity Server
//It contains Server's Identity and the PSK credentials
//of other devices which the server trusts
static char CRED_FILE[] = "oic_svr_db_server.dat";
static char INTROSPECTION_FILE[] = "server_introspection.dat";

static LSHandle *pLsHandle = NULL;
static GMainLoop *mainloop = NULL;

OCObservationId obsNotify[(SAMPLE_MAX_NUM_OBSERVATIONS)/2];
Observers interestedObservers[SAMPLE_MAX_NUM_OBSERVATIONS];

// Set of strings for each of platform Info fields
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

static const char *gDeviceName = "webOS Light";
static const char *gDeviceType = "oic.d.light";
static const char *gSpecVersion = "ocf.2.0.0";
static const char *gDataModelVersions = "ocf.res.1.3.0,ocf.sh.1.3.0";
static const char *gProtocolIndependentID = "fa008167-3bbf-4c9d-8604-c9bcb96cb712";

OCPlatformInfo platformInfo;

// forward declarations
void DeletePlatformInfo();
void initializePlatform();
OCStackResult SetDeviceInfo();
OCStackResult SetPlatformInfo(const char *platformID, const char *manufacturerName,
        const char *manufacturerUrl, const char *modelNumber, const char *dateOfManufacture,
        const char *platformVersion, const char *operatingSystemVersion,
        const char *hardwareVersion, const char *firmwareVersion, const char *supportUrl,
        const char *systemTime);

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

OCStackResult SetPlatformInfo(const char* platformID, const char *manufacturerName,
    const char *manufacturerUrl, const char *modelNumber, const char *dateOfManufacture,
    const char *platformVersion, const char* operatingSystemVersion, const char* hardwareVersion,
    const char *firmwareVersion, const char* supportUrl, const char* systemTime)
{

    bool success = true;

    if(manufacturerName != NULL && (strlen(manufacturerName) > MAX_PLATFORM_NAME_LENGTH))
    {
        return OC_STACK_INVALID_PARAM;
    }

    if(manufacturerUrl != NULL && (strlen(manufacturerUrl) > MAX_PLATFORM_URL_LENGTH))
    {
        return OC_STACK_INVALID_PARAM;
    }

    if(!DuplicateString(&platformInfo.platformID, platformID))
    {
        success = false;
    }

    if(!DuplicateString(&platformInfo.manufacturerName, manufacturerName))
    {
        success = false;
    }

    if(!DuplicateString(&platformInfo.manufacturerUrl, manufacturerUrl))
    {
        success = false;
    }

    if(!DuplicateString(&platformInfo.modelNumber, modelNumber))
    {
        success = false;
    }

    if(!DuplicateString(&platformInfo.dateOfManufacture, dateOfManufacture))
    {
        success = false;
    }

    if(!DuplicateString(&platformInfo.platformVersion, platformVersion))
    {
        success = false;
    }

    if(!DuplicateString(&platformInfo.operatingSystemVersion, operatingSystemVersion))
    {
        success = false;
    }
    if(!DuplicateString(&platformInfo.hardwareVersion, hardwareVersion))
    {
        success = false;
    }

    if(!DuplicateString(&platformInfo.firmwareVersion, firmwareVersion))
    {
        success = false;
    }

    if(!DuplicateString(&platformInfo.supportUrl, supportUrl))
    {
        success = false;
    }

    if(!DuplicateString(&platformInfo.systemTime, systemTime))
    {
        success = false;
    }

    if(success)
    {
        return OC_STACK_OK;
    }

    DeletePlatformInfo();
    return OC_STACK_ERROR;
}

OCStackResult SetDeviceInfo()
{
    OCStackResult result = OC_STACK_ERROR;

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
    result = OCSetPropertyValue(PAYLOAD_TYPE_DEVICE, OC_RSRVD_DEVICE_NAME, gDeviceName);
    if (result != OC_STACK_OK)
    {
        OCSAMPLE_LOG_ERROR(TAG, 0, "Failed to set device name");
        return result;
    }
    result = OCSetPropertyValue(PAYLOAD_TYPE_DEVICE, OC_RSRVD_DATA_MODEL_VERSION,
                                      gDataModelVersions);
    if (result != OC_STACK_OK)
    {
        OCSAMPLE_LOG_ERROR(TAG, 0, "Failed to set data model versions");
        return result;
    }
    result = OCSetPropertyValue(PAYLOAD_TYPE_DEVICE, OC_RSRVD_SPEC_VERSION, gSpecVersion);
    if (result != OC_STACK_OK)
    {
        OCSAMPLE_LOG_ERROR(TAG, 0, "Failed to set spec version");
        return result;
    }
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

OCRepPayload* getPayload(const char* uri, int64_t power, bool state)
{
    OCRepPayload* payload = OCRepPayloadCreate();
    if(!payload)
    {
        OCSAMPLE_LOG_ERROR(TAG, 0, "Failed to allocate Payload");
        return nullptr;
    }

    OCRepPayloadSetPropBool(payload, "value", state);
    OCRepPayloadSetPropString(payload, "n", "");
    OCRepPayloadAddResourceType(payload, "oic.r.switch.binary");
    OCRepPayloadAddInterface(payload, "oic.if.baseline");
    OCRepPayloadAddInterface(payload, "oic.if.a");

    return payload;
}

//This function takes the request as an input and returns the response
OCRepPayload* constructResponse (OCEntityHandlerRequest *ehRequest)
{
    if(ehRequest->payload && ehRequest->payload->type != PAYLOAD_TYPE_REPRESENTATION)
    {
        OCSAMPLE_LOG_ERROR(TAG, 0, "Incoming payload not a representation");
        return nullptr;
    }

    OCRepPayload* input = reinterpret_cast<OCRepPayload*>(ehRequest->payload);

    LEDResource *currLEDResource = &LED;

    if(OC_REST_PUT == ehRequest->method
        || OC_REST_POST == ehRequest->method)
    {
        bool state;
        if(OCRepPayloadGetPropBool(input, "value", &state))
        {
            currLEDResource->state = state;
        }
    }

    return getPayload(gResourceUri, currLEDResource->power, currLEDResource->state);
}

OCEntityHandlerResult ProcessGetRequest (OCEntityHandlerRequest *ehRequest,
        OCRepPayload **payload)
{
    OCEntityHandlerResult ehResult;

    OCRepPayload *getResp = constructResponse(ehRequest);

    if(getResp)
    {
        *payload = getResp;
        ehResult = OC_EH_OK;
    }
    else
    {
        ehResult = OC_EH_ERROR;
    }

    return ehResult;
}

OCEntityHandlerResult ProcessPutRequest (OCEntityHandlerRequest *ehRequest,
        OCRepPayload **payload)
{
    OCEntityHandlerResult ehResult;

    OCRepPayload *putResp = constructResponse(ehRequest);

    if(putResp)
    {
        *payload = putResp;
        ehResult = OC_EH_OK;
    }
    else
    {
        ehResult = OC_EH_ERROR;
    }

    return ehResult;
}

OCEntityHandlerResult ProcessPostRequest (OCEntityHandlerRequest *ehRequest,
        OCEntityHandlerResponse *response, OCRepPayload **payload)
{
    OCRepPayload *respPLPost_led = nullptr;
    OCEntityHandlerResult ehResult = OC_EH_OK;

    OCRepPayload *postResp = constructResponse(ehRequest);

    // Notify the list of observers
    int j = 0;
    for (uint8_t i = 0; i < SAMPLE_MAX_NUM_OBSERVATIONS; (i=i+2))
    {
        printf("interestedObservers[i].valid = %d\n", interestedObservers[i].valid);
        printf("interestedObservers[i].observationId = %d\n", interestedObservers[i].observationId);
        if (interestedObservers[i].valid == true)
        {
            obsNotify[j] = interestedObservers[i].observationId;
            j++;
            printf("obsNotify[j]=%d\n", j);
        }
    }

    printf("POST_LED.state=%d\n", LED.state);
    OCStackResult res = OCNotifyListOfObservers (LED.handle, obsNotify, j, postResp, OC_NA_QOS);

    if(postResp)
    {
        *payload = postResp;
        ehResult = OC_EH_OK;
    }
    else
    {
        ehResult = OC_EH_ERROR;
    }

    return ehResult;
}

void ProcessObserveRegister (OCEntityHandlerRequest *ehRequest)
{
    OIC_LOG_V (INFO, TAG, "Received observation registration request with observation Id %d",
            ehRequest->obsInfo.obsId);

    for (uint8_t i = 0; i < SAMPLE_MAX_NUM_OBSERVATIONS; i++)
    {
        if (interestedObservers[i].valid == false)
        {
            interestedObservers[i].observationId = ehRequest->obsInfo.obsId;
            interestedObservers[i].valid = true;
            break;
        }
    }
}

void ProcessObserveDeregister (OCEntityHandlerRequest *ehRequest)
{
    OIC_LOG_V (INFO, TAG, "Received observation deregistration request for observation Id %d",
            ehRequest->obsInfo.obsId);
    for (uint8_t i = 0; i < SAMPLE_MAX_NUM_OBSERVATIONS; i++)
    {
        if (interestedObservers[i].observationId == ehRequest->obsInfo.obsId)
        {
            interestedObservers[i].valid = false;
        }
    }
}

OCEntityHandlerResult
OCEntityHandlerCb (OCEntityHandlerFlag flag,
        OCEntityHandlerRequest *entityHandlerRequest,
        void* /*callbackParam*/)
{
    OCSAMPLE_LOG_INFO(TAG, 0, "Inside entity handler - flags: 0x%x", flag);

    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    OCEntityHandlerResponse response = { 0, 0, OC_EH_ERROR, 0, 0, { },{ 0 }, false };
    // Validate pointer
    if (!entityHandlerRequest)
    {
        OCSAMPLE_LOG_ERROR(TAG, 0, "Invalid request pointer");
        return OC_EH_ERROR;
    }

    OCRepPayload* payload = nullptr;

    if (flag & OC_REQUEST_FLAG)
    {
        OCSAMPLE_LOG_INFO(TAG, 0, "Flag includes OC_REQUEST_FLAG");
        if (entityHandlerRequest)
        {
            if (OC_REST_GET == entityHandlerRequest->method)
            {
                OCSAMPLE_LOG_INFO(TAG, 0, "Received OC_REST_GET from client");
                ehResult = ProcessGetRequest (entityHandlerRequest, &payload);
            }
            else if (OC_REST_PUT == entityHandlerRequest->method)
            {
                OCSAMPLE_LOG_INFO(TAG, 0, "Received OC_REST_PUT from client");
                ehResult = ProcessPutRequest (entityHandlerRequest, &payload);
            }
            else if (OC_REST_POST == entityHandlerRequest->method)
            {
                OCSAMPLE_LOG_INFO(TAG, 0, "Received OC_REST_POST from client");
                ehResult = ProcessPostRequest (entityHandlerRequest, &response, &payload);
            }
            else
            {
                OCSAMPLE_LOG_INFO(TAG, 0, "Received unsupported method %d from client",
                        entityHandlerRequest->method);
                ehResult = OC_EH_ERROR;
            }

            if (ehResult == OC_EH_OK && ehResult != OC_EH_FORBIDDEN)
            {
                // Format the response.  Note this requires some info about the request
                response.requestHandle = entityHandlerRequest->requestHandle;
                response.resourceHandle = entityHandlerRequest->resource;
                response.ehResult = ehResult;
                response.payload = reinterpret_cast<OCPayload*>(payload);
                response.numSendVendorSpecificHeaderOptions = 0;
                memset(response.sendVendorSpecificHeaderOptions, 0, sizeof response.sendVendorSpecificHeaderOptions);
                memset(response.resourceUri, 0, sizeof(response.resourceUri));
                // Indicate that response is NOT in a persistent buffer
                response.persistentBufferFlag = 0;

                // Send the response
                if (OCDoResponse(&response) != OC_STACK_OK)
                {
                    OCSAMPLE_LOG_ERROR(TAG, 0, "Error sending response");
                    ehResult = OC_EH_ERROR;
                }
            }
        }
    }

    // Handle observe request
    if (flag & OC_OBSERVE_FLAG)
    {
        OIC_LOG(INFO, TAG, "Flag includes OC_OBSERVE_FLAG");

        if (OC_OBSERVE_REGISTER == entityHandlerRequest->obsInfo.action)
        {
            OIC_LOG (INFO, TAG, "Received OC_OBSERVE_REGISTER from client");
            ProcessObserveRegister (entityHandlerRequest);
        }
        else if (OC_OBSERVE_DEREGISTER == entityHandlerRequest->obsInfo.action)
        {
            OIC_LOG (INFO, TAG, "Received OC_OBSERVE_DEREGISTER from client");
            ProcessObserveDeregister (entityHandlerRequest);
        }
    }

    OCRepPayloadDestroy(payload);
    return ehResult;
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

FILE* server_fopen(const char *path, const char *mode)
{
    if (0 == strcmp(path, OC_SECURITY_DB_DAT_FILE_NAME))
    {
        OCSAMPLE_LOG_INFO(TAG, 0, "reading security file 'oic_svr_db_server.dat' ptr...");
        return fopen(CRED_FILE, mode);
    }
    else if (0 == strcmp(path, OC_INTROSPECTION_FILE_NAME))
    {
        OCSAMPLE_LOG_INFO(TAG, 0, "reading introspection file  'server_introspection.dat' ptr...");
        return fopen(INTROSPECTION_FILE, mode);
    }
    else
    {
        return fopen(path, mode);
    }
}

void *serverStarter(gpointer user_data)
{
    while (!gQuitFlag)
    {
        if (OCProcess() != OC_STACK_OK)
        {
            OCSAMPLE_LOG_ERROR(TAG, 0, "OCStack process error");
        }
    }
    OCSAMPLE_LOG_INFO(TAG, 0, "Stopping serverStarter loop...");

    return NULL;
}

int main(int /*argc*/, char* /*argv*/[])
{
    struct timespec timeout;
    LSError lserror;
    LSErrorInit(&lserror);
    (void) PmLogGetContext("OCSERVERBASICOPS", &gLogContext);
    (void) PmLogGetContext("OCSERVERBASICOPS-LIB", &gLogLibContext);
    PmLogSetLibContext(gLogLibContext);

    mainloop = g_main_loop_new(NULL, FALSE);

    // Initialize g_main_loop
    if (!mainloop) {
        OCSAMPLE_LOG_ERROR(TAG, 0, "Failed to create main loop");
        return 0;
    }

    OCSAMPLE_LOG_INFO(TAG, 0, "OCServer is starting...");

    if (!LSRegister("org.ocf.webossample.ocserverbasicops", &pLsHandle, &lserror)) {
        OCSAMPLE_LOG_ERROR(TAG, 0, "Failed to register LS Handle");
        LSErrorLog(gLogContext, "LS_SRVC_ERROR", &lserror);
        return 0;
    }

    if (!LSGmainAttach(pLsHandle, mainloop, &lserror)) {
        OCSAMPLE_LOG_ERROR(TAG, 0, "Failed to attach main loop: %s", &lserror);
        LSErrorLog(gLogContext, "LS_SRVC_ATTACH_ERROR", &lserror);
        return 0;
    }

    // Initialize Persistent Storage for SVR database
    OCPersistentStorage ps = { server_fopen, fread, fwrite, fclose, unlink };
    OCRegisterPersistentStorageHandler(&ps);

    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    {
        OCSAMPLE_LOG_ERROR(TAG, 0, "OCStack init error");
        return 0;
    }

    initializePlatform();

    /*
     * Declare and create the example resource: LED
     */
    createLEDResource(gResourceUri, &LED, false, 0);

    timeout.tv_sec  = 0;
    timeout.tv_nsec = 100000000L;

    // Break from loop with Ctrl-C
    OCSAMPLE_LOG_INFO(TAG, 0, "Entering ocserver main loop...");
    signal(SIGINT, handleSigInt);

    pthread_create(&threadId_server, NULL, serverStarter, (void *)NULL);
    OCSAMPLE_LOG_INFO(TAG, 0, "Entering serverStarter main loop...");

    char input[10] = {0};
    uint8_t j = 0;

    // Initialize observations data structure for the resource
    for (uint8_t p = 0; p < SAMPLE_MAX_NUM_OBSERVATIONS; p++)
    {
        interestedObservers[p].valid = false;
    }

    OCRepPayload* payload = nullptr;

    OCStackResult res = OC_STACK_OK;
    while (!gQuitFlag)
    {
		printf("\n");
		printf("*********************************\n");
		printf("********** webOS Light **********\n");
		printf("*********************************\n");
		printf("Control the light locally:\n");
        printf("1: turn on the switch\n");
        printf("2: turn off the switch\n");
        printf("3: show current status\n");
		printf("99: quit the menu\n");
		printf("*********************************\n");
		printf("Select your choice: ");
        char * ret = fgets(input, sizeof(input), stdin);
        switch (atoi(input))
        {
            case 1:
                printf("Power up...\n");
                LED.state = true;
                j = 0;
                for (uint8_t i = 0; i < SAMPLE_MAX_NUM_OBSERVATIONS; (i=i+2))
                {
                    if (interestedObservers[i].valid == true)
                    {
                        obsNotify[j] = interestedObservers[i].observationId;
                        j++;
                    }
                }

                payload = getPayload(gResourceUri, 0, LED.state);
                res = OCNotifyListOfObservers (LED.handle, obsNotify, j, payload, OC_NA_QOS);
                if (res != OC_STACK_OK)
                    printf("Notify failed\n");
                else
                    printf("Notify succeeded\n");

                OCRepPayloadDestroy(payload);

                break;
            case 2:
                printf("Power down...\n");
                LED.state = false;
                j = 0;
                for (uint8_t i = 0; i < SAMPLE_MAX_NUM_OBSERVATIONS; (i=i+2))
                {
                    if (interestedObservers[i].valid == true)
                    {
                        obsNotify[j] = interestedObservers[i].observationId;
                        j++;
                    }
                }

                payload = getPayload(gResourceUri, 0, LED.state);
                OCNotifyListOfObservers (LED.handle, obsNotify, j,
                        payload, OC_HIGH_QOS);
                if (res != OC_STACK_OK)
                    printf("Notify failed\n");
                else
                    printf("Notify succeeded\n");
                OCRepPayloadDestroy(payload);
                break;

            case 3:
                printf("\n*****************************************************\n");
				printf("Status of the binary switch: %d\n", LED.state);
                printf("*****************************************************\n");
                break;
            case 99:
                gQuitFlag = true;
                break;
            default:
                printf("Invalid selection...\n");
                break;
        }
    }

    OCSAMPLE_LOG_INFO(TAG, 0, "Exiting ocserver main loop...");

    if (OCStop() != OC_STACK_OK)
    {
        OCSAMPLE_LOG_ERROR(TAG, 0, "OCStack process error");
        return 0;
    }

    return 0;
}

int createLEDResource (char *uri, LEDResource *ledResource, bool resourceState, int64_t resourcePower)
{
    if (!uri)
    {
        OCSAMPLE_LOG_ERROR(TAG, 0, "Resource URI cannot be NULL");
        return -1;
    }

    ledResource->state = resourceState;
    ledResource->power= resourcePower;
    OCStackResult res = OCCreateResource(&(ledResource->handle),
            "oic.r.switch.binary",
            OC_RSRVD_INTERFACE_DEFAULT,
            uri,
            OCEntityHandlerCb,
            NULL,
            OC_DISCOVERABLE|OC_OBSERVABLE | OC_SECURE);

    OCSAMPLE_LOG_INFO(TAG, 0, "Created LED resource with result: %s", getResult(res));

    res = OCBindResourceInterfaceToResource(ledResource->handle, "oic.if.a");
    if (res != OC_STACK_OK)
    {
        OCSAMPLE_LOG_ERROR(TAG, 0, "Failed to Bind resource interface...");
    }

    OCSAMPLE_LOG_INFO(TAG, 0, "Bind interface with result: %s", getResult(res));

    return 0;
}
