//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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

// NOTE : Keeping Wifi provisioning in this file to have adaptability while doing OOPs refactoring

#include "provisioning.h"

//Standard includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>

//EasySetup include files
#include "ocpayload.h"
#include "escommon.h"

// External includes
#include "logger.h"
#include "oic_string.h"

#define ES_WIFI_PROV_TAG "ES_WIFI_PROVISIONING"

static const char * UNICAST_PROV_STATUS_QUERY = "coap://%s:%d%s";

OCStackApplicationResult ProvisionEnrolleeResponse(void* /*ctx*/, OCDoHandle /*handle*/,
        OCClientResponse *clientResponse)
{
    OIC_LOG_V(DEBUG, ES_WIFI_PROV_TAG, "Inside ProvisionEnrolleeResponse");

    // If user stopped the process then return from this function;
    if (IsSetupStopped())
    {
        ErrorCallback(DEVICE_NOT_PROVISIONED);
        ClearMemory();
        return OC_STACK_DELETE_TRANSACTION;
    }

    if (!ValidateEnrolleeResponse(clientResponse))
    {
        ErrorCallback(DEVICE_NOT_PROVISIONED);
        return OC_STACK_DELETE_TRANSACTION;
    }

    char query[OIC_STRING_MAX_VALUE] =
    { '\0' };
    char resUri[MAX_URI_LENGTH] =
    { '\0' };

    OIC_LOG_V(DEBUG, ES_WIFI_PROV_TAG, "Resource URI = %s", clientResponse->resourceUri);

    OICStrcpy(resUri, sizeof(resUri), clientResponse->resourceUri);

#ifdef REMOTE_ARDUINO_ENROLEE
    //Arduino Enrollee needs mediator application provide IP and port55555 which is specific
    // to Arduino WiFi enrollee
    // REMOTE_ARDUINO_ENROLEE has to be defined if Mediator is being tested with Arduino
    snprintf(query, sizeof(query), UNICAST_PROV_STATUS_QUERY, clientResponse->addr->addr, IP_PORT,
            resUri);
#else
    snprintf(query, sizeof(query), UNICAST_PROV_STATUS_QUERY, clientResponse->addr->addr,
            clientResponse->addr->port, resUri);
#endif

    if (TriggerNetworkConnection(OC_HIGH_QOS, query, OC_RSRVD_ES_URI_PROV, clientResponse->addr, 0)
            != OC_STACK_OK)
    {
        OIC_LOG(INFO, ES_WIFI_PROV_TAG, "GetProvisioningStatusResponse received NULL clientResponse");

        ErrorCallback(DEVICE_NOT_PROVISIONED);
        ClearMemory();
    }

    return OC_STACK_DELETE_TRANSACTION;
}

OCStackApplicationResult GetProvisioningStatusResponse(void* /*ctx*/, OCDoHandle /*handle*/,
        OCClientResponse *clientResponse)
{
    // If user stopped the process then return from this function;
    if (IsSetupStopped())
    {
        ErrorCallback(DEVICE_NOT_PROVISIONED);
        ClearMemory();
        return OC_STACK_DELETE_TRANSACTION;
    }

    if (!ValidateEnrolleeResponse(clientResponse))
    {
        ErrorCallback(DEVICE_NOT_PROVISIONED);
        ClearMemory();
        return OC_STACK_DELETE_TRANSACTION;
    }

    OCRepPayload *input = (OCRepPayload *) (clientResponse->payload);

    char resUri[MAX_URI_LENGTH] = { '\0' };

    OIC_LOG_V(DEBUG, ES_WIFI_PROV_TAG, "resUri = %s", clientResponse->resourceUri);

    OICStrcpy(resUri, sizeof(resUri), clientResponse->resourceUri);

    while (input)
    {
        int64_t ps;
        if (OCRepPayloadGetPropInt(input, OC_RSRVD_ES_PS, &ps))
        {

            if (ps == ES_PS_NEED_PROVISIONING)
            {
                input = input->next;
                continue;
            }
            else
            {
                ErrorCallback(DEVICE_NOT_PROVISIONED);
                ClearMemory();
                return OC_STACK_DELETE_TRANSACTION;
            }
        }

        LogProvisioningResponse(input->values);
        input = input->next;
    }

    char query[OIC_STRING_MAX_VALUE] =
    { '\0' };

#ifdef REMOTE_ARDUINO_ENROLEE
    //Arduino Enrollee needs mediator application provide IP and port55555 which is specific
    // to Arduino WiFi enrollee
    // REMOTE_ARDUINO_ENROLEE has to be defined if Mediator is being tested with Arduino
    snprintf(query, sizeof(query), UNICAST_PROV_STATUS_QUERY, clientResponse->addr->addr, IP_PORT,
            resUri);
#else
    snprintf(query, sizeof(query), UNICAST_PROV_STATUS_QUERY, clientResponse->addr->addr,
            clientResponse->addr->port, resUri);
#endif

    if (ProvisionEnrollee(OC_HIGH_QOS, query, OC_RSRVD_ES_URI_PROV, clientResponse->addr, 0)
            != OC_STACK_OK)
    {
        OIC_LOG(INFO, ES_WIFI_PROV_TAG, "GetProvisioningStatusResponse received NULL clientResponse");

        ErrorCallback(DEVICE_NOT_PROVISIONED);
        ClearMemory();
        return OC_STACK_DELETE_TRANSACTION;
    }

    return OC_STACK_KEEP_TRANSACTION;

}

OCStackResult GetProvisioningStatus(OCQualityOfService qos, const char *query,
        const OCDevAddr *destination)
{
    OCStackResult ret = OC_STACK_ERROR;
    OCHeaderOption options[MAX_HEADER_OPTIONS];

    OIC_LOG(DEBUG, ES_WIFI_PROV_TAG, "Inside GetProvisioningStatus");

    uint8_t option0[] =
    { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    uint8_t option1[] =
    { 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 };
    memset(options, 0, sizeof(OCHeaderOption) * MAX_HEADER_OPTIONS);
    options[0].protocolID = OC_COAP_ID;
    options[0].optionID = 2048;
    memcpy(options[0].optionData, option0, sizeof(option0));
    options[0].optionLength = 10;
    options[1].protocolID = OC_COAP_ID;
    options[1].optionID = 3000;
    memcpy(options[1].optionData, option1, sizeof(option1));
    options[1].optionLength = 10;

    ret = InvokeOCDoResource(query, OC_REST_GET, destination, qos, GetProvisioningStatusResponse,
    NULL, options, 2);
    return ret;
}

// This is a function called back when a device is discovered
OCStackApplicationResult FindProvisioningResourceResponse(void* /*ctx*/, OCDoHandle /*handle*/,
        OCClientResponse *clientResponse)
{

    OIC_LOG_V(INFO, ES_WIFI_PROV_TAG, "Entering FindProvisioningResourceResponse %s",
            clientResponse->devAddr.addr);

    // If user stopped the process then return from this function;
    if (IsSetupStopped())
    {
        ErrorCallback(DEVICE_NOT_PROVISIONED);
        ClearMemory();
        return OC_STACK_DELETE_TRANSACTION;
    }

    if (!ValidateFindResourceResponse(clientResponse))
    {
        ErrorCallback(DEVICE_NOT_PROVISIONED);
        return OC_STACK_DELETE_TRANSACTION;
    }

    char szQueryUri[64] =
    { 0 };

    OCDiscoveryPayload *discoveryPayload = (OCDiscoveryPayload *) (clientResponse->payload);

    OIC_LOG_V(DEBUG, ES_WIFI_PROV_TAG, "resUri = %s", discoveryPayload->resources->uri);

#ifdef REMOTE_ARDUINO_ENROLEE
    //Arduino Enrollee needs mediator application provide IP and port55555 which is specific
    // to Arduino WiFi enrollee
    // REMOTE_ARDUINO_ENROLEE has to be defined if Mediator is being tested with Arduino
    snprintf(szQueryUri, sizeof(szQueryUri), UNICAST_PROV_STATUS_QUERY,
            clientResponse->addr->addr,
            IP_PORT,
            discoveryPayload->resources->uri);
#else
    snprintf(szQueryUri, sizeof(szQueryUri), UNICAST_PROV_STATUS_QUERY,
            clientResponse->devAddr.addr, clientResponse->devAddr.port,
            discoveryPayload->resources->uri);
#endif

    OIC_LOG_V(DEBUG, ES_WIFI_PROV_TAG, "query before GetProvisioningStatus call = %s", szQueryUri);

    if (GetProvisioningStatus(OC_HIGH_QOS, szQueryUri, &clientResponse->devAddr) != OC_STACK_OK)
    {
        ErrorCallback(DEVICE_NOT_PROVISIONED);
        return OC_STACK_DELETE_TRANSACTION;
    }

    return OC_STACK_KEEP_TRANSACTION;

}

bool ValidateEasySetupParams(const ProvConfig */*netInfo*/, WiFiOnboadingConnection *onboardConn,
        OCProvisioningStatusCB provisioningStatusCallback)
{

    if (onboardConn == NULL || strlen(onboardConn->ipAddress) == 0)
    {
        OIC_LOG(ERROR, ES_WIFI_PROV_TAG, "Request URI is NULL");
        return false;
    }

    if (provisioningStatusCallback == NULL)
    {
        OIC_LOG(ERROR, ES_WIFI_PROV_TAG, "ProvisioningStatusCallback is NULL");
        return false;
    }

    return true;

}

