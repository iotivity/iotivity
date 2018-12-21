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
// NOTE :  This sample server is generated based on ocserverbasicops.cpp
///////////////////////////////////////////////////////////////////////


#include "iotivity_config.h"
#ifdef WITH_MRAA
#include "mraa.hpp"
#endif
#include <signal.h>
#include "ocstack.h"
#include "ocpayload.h"
#include "logger.h"
#include "utilities.c"
#include <unistd.h>
#include <time.h>
#define TAG "SERVER_APP"

static bool STOP = false;
static char SVR_DB[] = "ocf_svr_db_server.dat";
static char INTROSPECTION_FILE[] = "switch_introspection.dat";
#ifdef WITH_MRAA
mraa::Gpio *GPIO;
#endif

typedef struct
{
    OCResourceHandle handle;
    const char* type = "oic.r.switch.binary";
    const char * interface = OC_RSRVD_INTERFACE_ACTUATOR;
    const char* uri = "/switch";
    uint8_t properties = OC_DISCOVERABLE | OC_SECURE;
    bool value = false;

} BinarySwitch;

static BinarySwitch SWITCH;

static const char* PLATFORM_ID = "12345678-1234-1234-1234-123456789012";
static const char* MANUFACTURER_NAME = "Vprime";
// Set of strings for each of device info fields
static const char*  DEVICE_NAME = "OCF Developer kit";
static const char*  SPEC_VERSION = "ocf.1.0.0";
static const char*  DATA_MODEL_VERSION = "ocf.res.1.0.0,ocf.sh.1.0.0";
static const char*  PROTO_INDEP_ID = "b0ed9259-ec95-4ac6-8f62-241d0da02683";
static const char*  DEVICE_TYPE = "oic.d.switch";


/**
* utility function to set the platform information
*
* @return stack result
*/
OCStackResult SetPlatformInfo()
{
    OCStackResult result = OC_STACK_ERROR;
    OCResourceHandle rHandle = OCGetResourceHandleAtUri(OC_RSRVD_PLATFORM_URI);
    if (rHandle == NULL)
    {
        OIC_LOG_V(ERROR, TAG,
                  "[%s] Platform Resource does not exist", __func__);
        return result;
    }
    result = OCSetPropertyValue(PAYLOAD_TYPE_PLATFORM,
                                OC_RSRVD_PLATFORM_ID,
                                PLATFORM_ID);
    if (result != OC_STACK_OK)
    {
        OIC_LOG_V(ERROR, TAG, "[%s] Failed to set platform ID", __func__);
        return result;
    }
    else
    {
        char * pin = NULL;
        OCGetPropertyValue(PAYLOAD_TYPE_PLATFORM,
                           OC_RSRVD_PLATFORM_ID,
                           (void **) &pin);
        OIC_LOG_V(INFO, TAG, "[%s] Set platform ID successfully to %s",
            __func__,
            pin);
    }
    result = OCSetPropertyValue(PAYLOAD_TYPE_PLATFORM,
                                OC_RSRVD_MFG_NAME,
                                MANUFACTURER_NAME);
    if (result != OC_STACK_OK)
    {
        OIC_LOG_V(ERROR, TAG, "[%s] Failed to set platform name",
                  __func__);
        return result;
    }
    else
    {
        char * mn = NULL;
        OCGetPropertyValue(PAYLOAD_TYPE_PLATFORM,
                           OC_RSRVD_MFG_NAME,
                           (void **) &mn);
        OIC_LOG_V(INFO, TAG, "[%s] Set manufacturer name successfully to %s",
                  __func__,
                  mn);
    }

    return OC_STACK_OK;
}


/**
* utility function to set the device information
*
* @return stack result
*/
OCStackResult SetDeviceInfo(void)
{
    OCStackResult result = OC_STACK_ERROR;

    OCResourceHandle handle = OCGetResourceHandleAtUri(OC_RSRVD_DEVICE_URI);
    if (handle == NULL)
    {
        OIC_LOG_V(ERROR, TAG, "[%s] Failed to find device resource %s",
                  __func__,
                  OC_RSRVD_DEVICE_URI);
        return result;
    }

    result = OCBindResourceTypeToResource(handle, DEVICE_TYPE);
    if (result != OC_STACK_OK)
    {
        OIC_LOG_V(ERROR, TAG, "[%s] Failed to add device type", __func__);
        return result;
    }

    result = OCSetPropertyValue(PAYLOAD_TYPE_DEVICE,
                                OC_RSRVD_DEVICE_NAME,
                                DEVICE_NAME);
    if (result != OC_STACK_OK)
    {
        OIC_LOG_V(ERROR, TAG, "[%s] Failed to set device name", __func__);
        return result;
    }

    result = OCSetPropertyValue(PAYLOAD_TYPE_DEVICE,
                                OC_RSRVD_DATA_MODEL_VERSION,
                                DATA_MODEL_VERSION);
    if (result != OC_STACK_OK)
    {
        OIC_LOG_V(ERROR, TAG, "[%s] Failed to set data model versions",
                  __func__);
        return result;
    }

    result = OCSetPropertyValue(PAYLOAD_TYPE_DEVICE,
                                OC_RSRVD_SPEC_VERSION,
                                SPEC_VERSION);
    if (result != OC_STACK_OK)
    {
        OIC_LOG_V(ERROR, TAG, "[%s] Failed to set spec version", __func__);
        return result;
    }

    result = OCSetPropertyValue(PAYLOAD_TYPE_DEVICE,
                                OC_RSRVD_PROTOCOL_INDEPENDENT_ID,
                                PROTO_INDEP_ID);
    if (result != OC_STACK_OK)
    {
        OIC_LOG_V(ERROR, TAG, "[%s] Failed to set proptocol independent id",
                  __func__);
        return result;
    }

    return OC_STACK_OK;
}


/**
* Internal method to create the response payload and updates it with the
* resource properties.
*
* @param[in] resource the resource used to created the response payload
*
* @return response payload
*/
OCRepPayload*
CreateResponsePayload(BinarySwitch resource)
{
    OCRepPayload* payload = OCRepPayloadCreate();
    if (!payload)
    {
        OIC_LOG_V(ERROR, TAG, "[%s] Failed to create response payload",
                  __func__);
        return NULL;
    }
    OIC_LOG_V(DEBUG, TAG, "[%s] Created response payload successfully."
              "Setting up properties...",
              __func__);
    OCRepPayloadAddInterface(payload, OC_RSRVD_INTERFACE_DEFAULT);
    OCRepPayloadAddInterface(payload, resource.interface);
    OCRepPayloadAddResourceType(payload, resource.type);
    OCRepPayloadSetPropBool(payload, "value", resource.value);

    return payload;
}


/**
* Internal method to process the GET request.
*
* @param[out] payload pointer to the payload that needs to be updated
*
* @return Entity handler result
*/
OCEntityHandlerResult
ProcessGetRequest(OCRepPayload **payload)
{
    OCEntityHandlerResult eh_res = OC_EH_ERROR;
    OIC_LOG_V(DEBUG, TAG, "[%s] Processing GET request", __func__);
    OCRepPayload *ResponsePayload = CreateResponsePayload(SWITCH);

    if (ResponsePayload)
    {
        *payload = ResponsePayload;
        eh_res = OC_EH_OK;
    }

    return eh_res;
}


/**
* Internal method to process the POST request.
*
* @param[in] ehRequest a pointer to the entity handler request
* @param[out] payload a pointer to the pointer to the payload that needs to be
* updated
*
* @return Entity handler result
*/
OCEntityHandlerResult
ProcessPostRequest(OCEntityHandlerRequest *ehRequest,
                   OCRepPayload **payload)
{
    OCEntityHandlerResult eh_res = OC_EH_ERROR;
    OIC_LOG_V(DEBUG, TAG, "[%s] Processing POST request", __func__);
    // casting the request payload into a OCRepPayload data type to
    // read the value property
    const OCRepPayload* requestPayload = (OCRepPayload*)(ehRequest->payload);
    bool value;
    if (OCRepPayloadGetPropBool(requestPayload, "value", &value))
    {
        SWITCH.value = value;
#ifdef WITH_MRAA
        GPIO->write(value);
#endif
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "[%s] Failed updating the value property",
                  __func__);
        return eh_res;
    }

    OCRepPayload *ResponsePayload = CreateResponsePayload(SWITCH);

    if (ResponsePayload)
    {
        *payload = ResponsePayload;
        eh_res = OC_EH_OK;
    }

    return eh_res;
}


/**
* The entity handler call back method for the SWITCH resource to handle the GET
* and POST requests.
*
* @param[in] flag entity handler flag which could be request or observer
* @param[in] ehRequest a pointer to the entity handler request
* @param[in] callbackParam unused
*
* @return Entity handler result
*/
OCEntityHandlerResult
OCEntityHandlerCallBack(OCEntityHandlerFlag flag,
                        OCEntityHandlerRequest *ehRequest,
                        void* callbackParam)
{
    OC_UNUSED(callbackParam);
    if (!ehRequest)
    {
        OIC_LOG_V(ERROR, TAG, "[%s] Invalid request pointer", __func__);
        return OC_EH_ERROR;
    }
    if (ehRequest->payload &&
        ehRequest->payload->type != PAYLOAD_TYPE_REPRESENTATION)
    {
        OIC_LOG_V(ERROR, TAG, "[%s] payload type must be representation",
                  __func__);
        return OC_EH_ERROR;
    }
    OIC_LOG_V(INFO, TAG, "[%s] Flags: 0x%x: %s", __func__, flag,
              decode_oc_eh_flag(flag));


    OCEntityHandlerResult eh_res = OC_EH_ERROR;
    OCStackResult stackResult = OC_STACK_ERROR;

    OCEntityHandlerResponse ehResponse;
    memset(&ehResponse, 0, sizeof(ehResponse));
    OCMethod requestMethod = ehRequest->method;


    OCRepPayload* payload = NULL;

    if (flag & OC_REQUEST_FLAG)
    {
        OIC_LOG_V(INFO, TAG, "[%s] OC_REQUEST_FLAG is detected", __func__);
        if (ehRequest)
        {
            if (OC_REST_GET == requestMethod)
            {
                OIC_LOG_V(INFO, TAG, "[%s] Processing GET request", __func__);
                eh_res = ProcessGetRequest(&payload);
            }
            else if (OC_REST_POST == requestMethod)
            {
                OIC_LOG_V(INFO, TAG, "[%s] Processing POST request", __func__);
                eh_res = ProcessPostRequest(ehRequest, &payload);
            }
            else
            {
                OIC_LOG_V(INFO, TAG, "[%s] Received unsupported method (%d):"
                          " %s",
                          __func__,
                          ehRequest->method,
                          decode_oc_method(ehRequest->method));
                eh_res = OC_EH_ERROR;
            }

            if (eh_res == OC_EH_OK)
            {
                ehResponse.requestHandle = ehRequest->requestHandle;
                ehResponse.resourceHandle = ehRequest->resource;
                ehResponse.ehResult = eh_res;
                ehResponse.payload = (OCPayload*)(payload);
                ehResponse.numSendVendorSpecificHeaderOptions = 0;
                memset(ehResponse.sendVendorSpecificHeaderOptions, 0,
                       sizeof(ehResponse.sendVendorSpecificHeaderOptions));
                memset(ehResponse.resourceUri,
                       0,
                       sizeof(ehResponse.resourceUri));
                ehResponse.persistentBufferFlag = 0;

                // Send the ehResponse
                stackResult = OCDoResponse(&ehResponse);
                if (stackResult != OC_STACK_OK)
                {
                    OIC_LOG_V(ERROR, TAG, "[%s] Failed to send a ehResponse "
                              "(%d): %s",
                              __func__,
                              stackResult,
                              decode_oc_stack_result(stackResult));
                    eh_res = OC_EH_ERROR;
                }
            }
            else
            {
                OIC_LOG_V(ERROR, TAG, "[%s] Entity handler failure (%d): %s",
                          __func__,
                          eh_res,
                          decode_oc_eh_result(eh_res));
            }
        }
    }

    OCPayloadDestroy(ehResponse.payload);
    return eh_res;
}


/**
* SIGINT call back handler to update the STOP flag to exit the main loop.
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
* Custom fopen method to open the secure virtual resource database correctly.
*
* @param[in] path file name to be opened
* @param[in] mode the mode in which the file will be opened in
*
* @return opened file
*/
FILE*
ServerFOpen(const char *path,
            const char *mode)
{
    if (0 == strcmp(path, OC_SECURITY_DB_DAT_FILE_NAME))
    {
        OIC_LOG_V(DEBUG, TAG, "[%s] reading file: %s with mode: %s",
                  __func__,
                  SVR_DB,
                  mode);
        return fopen(SVR_DB, mode);
    }
    else if(0 == strcmp(path, OC_INTROSPECTION_FILE_NAME))
    {
        OIC_LOG_V(DEBUG, TAG, "[%s] reading file: %s with mode: %s",
                  __func__,
                  INTROSPECTION_FILE,
                  mode);
        return fopen(INTROSPECTION_FILE, mode);
    }
    else
    {
        OIC_LOG_V(DEBUG, TAG, "[%s] reading file: %s with mode: %s",
                  __func__,
                  path,
                  mode);
        return fopen(path, mode);
    }
}


/**
* main function
*/
int
main(void)
{
    OCStackResult stack_res = OC_STACK_ERROR;
    #if defined(WITH_MRAA)
    #if defined(RAW_GPIO)
    GPIO = new mraa::Gpio(LED_PIN, true, true);
    #else
    GPIO = new mraa::Gpio(LED_PIN);
    #endif
    if (!GPIO)
    {
        OIC_LOG_V(ERROR, TAG, "Error instantiating gpio %d", LED_PIN);
    }
    GPIO->dir(mraa::DIR_OUT);
    #endif

    OIC_LOG_V(DEBUG, TAG, "[%s] Initializing and registering persistent"
              "storage",
              __func__);
    OCPersistentStorage ps = {ServerFOpen, fread, fwrite, fclose, unlink};
    OCRegisterPersistentStorageHandler(&ps);

    OIC_LOG_V(DEBUG, TAG, "[%s] Initializing IoTivity stack for server",
              __func__);
    stack_res = OCInit(NULL, 0, OC_SERVER);
    if (stack_res != OC_STACK_OK)
    {
        OIC_LOG_V(ERROR, TAG, "[%s] Failed to initialize IoTivity stack (%d): "
                  "%s",
                  __func__, stack_res, decode_oc_stack_result(stack_res));
        return stack_res;
    }

    stack_res = SetPlatformInfo();

    if (stack_res != OC_STACK_OK)
    {
        OIC_LOG_V(ERROR, TAG, "[%s] Platform Registration failed\n", __func__);
        return stack_res;
    }
    else
    {
        OIC_LOG_V(INFO, TAG, "[%s] Platform set successfully", __func__);
    }

    stack_res = SetDeviceInfo();
    if (stack_res != OC_STACK_OK)
    {
        OIC_LOG_V(ERROR, TAG, "[%s] Device Registration failed\n", __func__);
        return stack_res;
    }
    else
    {
        OIC_LOG_V(INFO, TAG, "[%s] Device set successfully", __func__);
    }

    stack_res = OCCreateResource(&(SWITCH.handle),
                                 SWITCH.type,
                                 SWITCH.interface,
                                 SWITCH.uri,
                                 OCEntityHandlerCallBack,
                                 NULL,
                                 SWITCH.properties);
    if (stack_res != OC_STACK_OK)
    {
        OIC_LOG_V(ERROR, TAG, "[%s] Failed to create resource\n", __func__);
        return stack_res;
    }
    else
    {
        OIC_LOG_V(INFO, TAG, "[%s] Created resource successfully", __func__);
    }

    struct timespec timeout;
    timeout.tv_sec  = 0;
    timeout.tv_nsec = 100000000L;

    OIC_LOG_V(INFO, TAG, "[%s] Server is running, press ctrl+c to stop...",
              __func__);

    signal(SIGINT, SIGINTHandlerCallBack);

    while (!STOP)
    {
        stack_res = OCProcess();
        if (stack_res != OC_STACK_OK)
        {
            OIC_LOG_V(ERROR, TAG, "[%s] IoTivity stack process failure (%d): "
                      "%s",
                      __func__, stack_res, decode_oc_stack_result(stack_res));
            return stack_res;
        }
        nanosleep(&timeout, NULL);
    }

    OIC_LOG_V(INFO, TAG, "[%s] Stopping IoTivity server...", __func__);

    stack_res = OCStop();
    if (stack_res != OC_STACK_OK)
    {
        OIC_LOG_V(ERROR, TAG, "[%s] Failed to stop IoTivity server (%d): %s",
                  __func__,
                  stack_res,
                  decode_oc_stack_result(stack_res));
        return stack_res;
    }

    return 0;
}
