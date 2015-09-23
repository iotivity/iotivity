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


// NOTE : Keeping Ble provisioning in this file to have adaptability while doing OOPs refactoring 


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>

#include "ocpayload.h"
#include "provisioning.h"
#include "common.h"
// External includes

#include "logger.h"
#include "oic_malloc.h"

static bool sent_provision_request = false;

static EnrolleeNWProvInfo_t *netProvInfo;

/**
 * @var cbData
 * @brief Callback for providing provisioning status callback to application
 */
static OCProvisioningStatusCB cbData = NULL;


OCStackApplicationResult GetProvisioningStatusResponse(void *ctx, OCDoHandle handle,
                                                       OCClientResponse *clientResponse) {

    OIC_LOG_V(DEBUG, TAG, "INside  GetProvisioningStatusResponse");

    // If user stopped the process then return from this function;
    if (IsSetupStopped()) {
        ErrorCallback(DEVICE_NOT_PROVISIONED);
        ClearMemory();
        return OC_STACK_DELETE_TRANSACTION;
    }

    if (sent_provision_request == true)
        return OC_STACK_DELETE_TRANSACTION;
    sent_provision_request = true;

    ProvisioningInfo *provInfo;

    if (!ValidateEnrolleResponse(clientResponse)) {
        ErrorCallback(DEVICE_NOT_PROVISIONED);
        ClearMemory();
        return OC_STACK_DELETE_TRANSACTION;
    }

    OCRepPayload *input = (OCRepPayload * )(clientResponse->payload);

    char query[OIC_STRING_MAX_VALUE] =
            {'\0'};
    char resURI[MAX_URI_LENGTH] =
            {'\0'};

    OIC_LOG_V(DEBUG, TAG, "resUri = %s", input->uri);

    strncpy(resURI, input->uri, sizeof(resURI) - 1);

    snprintf(query, sizeof(query), UNICAST_PROVISIONING_QUERY_BLE, clientResponse->addr->addr);

    OIC_LOG_V(DEBUG, TAG, "query = %s", query);

    OCDevAddr *devaddress = &clientResponse->devAddr;
    devaddress->adapter = OC_ADAPTER_GATT_BTLE;

    //OCPayloadLogRep(DEBUG,TAG,input);

    if (ProvisionEnrollee(OC_HIGH_QOS, query, OC_RSRVD_ES_URI_PROV, devaddress,
                          5  /* 5s delay to avoid packet drop*/) != OC_STACK_OK) {
        OIC_LOG(INFO, TAG,
                "GetProvisioningStatusResponse received NULL clientResponse.Invoking Provisioing Status Callback");

        ErrorCallback(DEVICE_NOT_PROVISIONED);
        ClearMemory();
        return OC_STACK_DELETE_TRANSACTION;
    }

    return OC_STACK_KEEP_TRANSACTION;

}

OCStackResult GetProvisioningStatus(OCQualityOfService qos, const char *query,
                                    const OCDevAddr *destination) {
    OCStackResult ret = OC_STACK_ERROR;
    OIC_LOG_V(INFO, TAG, "\n\nExecuting %s %d", __func__, destination->adapter);
    ret = InvokeOCDoResource(query, OC_REST_GET, destination, OC_HIGH_QOS,
                             GetProvisioningStatusResponse, NULL, NULL, 0);
    return ret;
}


// This is a function called back when a device is discovered
OCStackApplicationResult FindProvisioningResourceResponse(void *ctx, OCDoHandle handle,
                                                          OCClientResponse *clientResponse) {

    OIC_LOG(INFO, TAG, PCF("Entering FindProvisioningResourceResponse"));

    // If user stopped the process then return from this function;
    if (IsSetupStopped()) {
        ErrorCallback(DEVICE_NOT_PROVISIONED);
        ClearMemory();
        return OC_STACK_DELETE_TRANSACTION;
    }

    if (!ValidateFinddResourceResponse(clientResponse)) {
        ErrorCallback(DEVICE_NOT_PROVISIONED);
        return OC_STACK_DELETE_TRANSACTION;
    }

    OCStackApplicationResult response = OC_STACK_DELETE_TRANSACTION;

    ProvisioningInfo *provInfo;
    char szQueryUri[64] =
            {0};

    OCDiscoveryPayload *discoveryPayload = (OCDiscoveryPayload * )(clientResponse->payload);

    // Need to conform if below check is required or not. As Null check of clientResponse->payload is already performed above
    if (!discoveryPayload) {
        OIC_LOG_V(DEBUG, TAG, "Failed To parse");
        ErrorCallback(DEVICE_NOT_PROVISIONED);
        return OC_STACK_DELETE_TRANSACTION;
    }

    OIC_LOG_V(DEBUG, TAG, "resUri = %s", discoveryPayload->resources->uri);

    snprintf(szQueryUri, sizeof(szQueryUri), UNICAST_PROVISIONING_QUERY_BLE,
             clientResponse->addr->addr);

    OCDevAddr *devaddress = &clientResponse->devAddr;
    devaddress->adapter = OC_ADAPTER_GATT_BTLE;

    if (strcmp(netProvInfo->netAddressInfo.WIFI.ipAddress, clientResponse->devAddr.addr))
        OIC_LOG_V(INFO, TAG, "unequal %s %s", netProvInfo->netAddressInfo.WIFI.ipAddress,
                  clientResponse->devAddr.addr);
    else
        OIC_LOG_V(INFO, TAG, "unequal %s %s", netProvInfo->netAddressInfo.WIFI.ipAddress,
                  clientResponse->devAddr.addr);

    OIC_LOG_V(DEBUG, TAG, "query before GetProvisioningStatus call = %s %d", szQueryUri,
              devaddress->adapter);

    if (GetProvisioningStatus(OC_HIGH_QOS, szQueryUri, devaddress) != OC_STACK_OK) {
        ErrorCallback(DEVICE_NOT_PROVISIONED);
        return OC_STACK_DELETE_TRANSACTION;
    }

    return OC_STACK_KEEP_TRANSACTION;

}

bool ValidateEasySetupParams(const EnrolleeNWProvInfo_t *netInfo,
                             OCProvisioningStatusCB provisioningStatusCallback) {

    if (netInfo == NULL || strlen(netInfo->netAddressInfo.LE.leMacAddress) == 0) {
        OIC_LOG(ERROR, TAG, "Request URI is NULL");
        return false;
    }

    if (provisioningStatusCallback == NULL) {
        OIC_LOG(ERROR, TAG, "ProvisioningStatusCallback is NULL");
        return false;
    }

    return true;

}

