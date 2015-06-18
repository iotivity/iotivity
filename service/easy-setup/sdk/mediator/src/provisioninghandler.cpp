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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include "provisioninghandler.h"

// External includes
#include "cJSON.h"
#include "camutex.h"
#include "cathreadpool.h"
#include "logger.h"
#include "oic_malloc.h"


/**
 * @var g_provisioningMutex
 * @brief Mutex to synchronize access to g_caDtlsContext.
 */
static ca_mutex g_provisioningMutex = NULL;
static ca_cond g_provisioningCond = NULL;
bool g_provisioningCondFlag = false;

static EnrolleeNWProvInfo_t* netProvInfo;

/**
 * @var cbData
 * @brief Callback for providing provisioning status callback to application
 */
static OCProvisioningStatusCB cbData = NULL;
static ca_thread_pool_t g_threadPoolHandle = NULL;

OCStackResult InitProvisioningHandler() {
    OCStackResult ret = OC_STACK_ERROR;
    /* Initialize OCStack*/
    if (OCInit(NULL, 0, OC_CLIENT) != OC_STACK_OK) {
        OIC_LOG(ERROR, TAG, "OCStack init error");
        return ret;
    }

    g_provisioningMutex = ca_mutex_new();

    OIC_LOG(DEBUG, TAG, "ca_thread_pool_init initializing");

    if (CA_STATUS_OK != ca_thread_pool_init(2, &g_threadPoolHandle)) {
        OIC_LOG(DEBUG, TAG, "thread_pool_init failed");
        return OC_STACK_ERROR;
    }

    g_provisioningCond = ca_cond_new();
    if (NULL == g_provisioningCond) {
        OIC_LOG(DEBUG, TAG, "Failed to create condition");
        ca_mutex_free(g_provisioningMutex);
        ca_thread_pool_free(g_threadPoolHandle);
        return OC_STACK_ERROR;
    }

    char *string = "listeningFunc invoked in a thread";
    if (CA_STATUS_OK
            != ca_thread_pool_add_task(g_threadPoolHandle, listeningFunc,
                    (void *) string)) {
        OIC_LOG(DEBUG, TAG, "thread_pool_add_task failed");
        ca_thread_pool_free(g_threadPoolHandle);
        ca_mutex_unlock(g_provisioningMutex);
        ca_mutex_free(g_provisioningMutex);
        ca_cond_free(g_provisioningCond);
        return OC_STACK_ERROR;
    }
    return OC_STACK_OK;
}

OCStackResult TerminateProvisioningHandler() {
    OCStackResult ret = OC_STACK_ERROR;
    if (OCStop() != OC_STACK_OK) {
        OIC_LOG(ERROR, TAG, "OCStack stop error");
    }

    ca_mutex_lock(g_provisioningMutex);
    g_provisioningCondFlag = true;
    //ca_cond_signal(g_provisioningCond);
    ca_mutex_unlock(g_provisioningMutex);

    ca_mutex_free(g_provisioningMutex);
    g_provisioningMutex = NULL;

    ca_thread_pool_free(g_threadPoolHandle);
    g_threadPoolHandle = NULL;

    ret = OC_STACK_OK;
    return ret;
}

void listeningFunc(void *data) {
    while (!g_provisioningCondFlag) {
        OCStackResult result;

        ca_mutex_lock(g_provisioningMutex);
        result = OCProcess();
        ca_mutex_unlock(g_provisioningMutex);

        if (result != OC_STACK_OK) {
            OIC_LOG(ERROR, TAG, "OCStack stop error");
        }

        // To minimize CPU utilization we may wish to do this with sleep
        sleep(1);
    }
}

OCStackApplicationResult ProvisionEnrolleeResponse(void* ctx, OCDoHandle handle,
        OCClientResponse * clientResponse) {
    ProvisioningInfo provInfo;

    if (clientResponse) {
        OIC_LOG_V(INFO, TAG, "Put Response JSON = %s",
                clientResponse->resJSONPayload);
    } else {
        OIC_LOG_V(INFO, TAG,
                "ProvisionEnrolleeResponse received Null clientResponse");
        PrepareProvisioingStatusCB(&provInfo, clientResponse,
                DEVICE_NOT_PROVISIONED);
        cbData(provInfo);
        return OC_STACK_DELETE_TRANSACTION;
    }

    uint16_t remotePortNum;

    OCDevAddrToPort((OCDevAddr *) clientResponse->addr, &remotePortNum);

    char sourceaddr[OIC_STRING_MAX_VALUE] = { '\0' };
    snprintf(sourceaddr, sizeof(sourceaddr), "%d.%d.%d.%d:%d%s",
            clientResponse->addr->addr[0], clientResponse->addr->addr[1],
            clientResponse->addr->addr[2], clientResponse->addr->addr[3],
            remotePortNum, OIC_PROVISIONING_URI);

    OIC_LOG_V(DEBUG, TAG, "ProvisionEnrolleeResponse %s @ %s",
            clientResponse->resJSONPayload, sourceaddr);

    if (clientResponse->resJSONPayload) {
        cJSON *observeJson = cJSON_CreateObject();
        observeJson = cJSON_Parse(clientResponse->resJSONPayload);

        cJSON *ocArray = cJSON_GetObjectItem(observeJson, "oic");
        cJSON *ocArray_sub = cJSON_GetArrayItem(ocArray, 0);

        cJSON *representationArray = cJSON_GetObjectItem(ocArray_sub, "rep");
        char *ocArray_str = cJSON_PrintUnformatted(representationArray);

        if (strstr(ocArray_str, "[{}") == ocArray_str) {
            OIC_LOG_V(DEBUG, TAG, "invalid payload : %s", ocArray_str);
            cJSON_Delete(observeJson);
            return OC_STACK_DELETE_TRANSACTION;
        }

        int countofrep = cJSON_GetArraySize(representationArray);

        for (int i = 0; i < countofrep; ++i) {
            cJSON *arrayJSON = cJSON_GetArrayItem(representationArray, i);
            OIC_LOG_V(DEBUG, TAG, "rep#%d's name : %s", i, arrayJSON->string);

            switch (arrayJSON->type) {
            case cJSON_False:
            case cJSON_True:
                OIC_LOG_V(DEBUG, TAG, "rep#%d's value : %d", i,
                        arrayJSON->valueint);
                break;
            case cJSON_Number:
                OIC_LOG_V(DEBUG, TAG, "rep#%d's value : %f", i,
                        arrayJSON->valuedouble);
                break;
            case cJSON_String:
                OIC_LOG_V(DEBUG, TAG, "rep#%d's value : %s", i,
                        arrayJSON->valuestring);
                break;
            case cJSON_NULL:
            default:
                OIC_LOG_V(DEBUG, TAG, "rep#%d's value : NULL", i);
                break;
            }
        }

        cJSON_Delete(observeJson);

        PrepareProvisioingStatusCB(&provInfo, clientResponse,
                DEVICE_PROVISIONED);
        cbData(provInfo);

        return OC_STACK_KEEP_TRANSACTION;
    } else {
        OIC_LOG(INFO, TAG,
                "ProvisionEnrolleeResponse received NULL clientResponse. \
                                Invoking Provisioing Status Callback");
        PrepareProvisioingStatusCB(&provInfo, clientResponse,
                DEVICE_NOT_PROVISIONED);
        cbData(provInfo);
        return OC_STACK_DELETE_TRANSACTION;
    }
}

OCStackResult ProvisionEnrollee(OCQualityOfService qos, const char* query) {
    OIC_LOG_V(INFO, TAG, "\n\nExecuting ProvisionEnrollee%s", __func__);

    cJSON *jsonFinal = cJSON_CreateObject();
    cJSON *json = cJSON_CreateObject();
    cJSON *jsonArray;
    cJSON *format;
    char* payload = NULL;

    char nodeData[OIC_STRING_MAX_VALUE] = { '\0' };
    snprintf(nodeData, sizeof(nodeData), "%s", OIC_PROVISIONING_URI);

    cJSON_AddStringToObject(json, "href", nodeData);
    cJSON_AddItemToObject(json, "rep", format = cJSON_CreateObject());
    cJSON_AddStringToObject(format, "tnn", netProvInfo->netAddressInfo.WIFI.ssid);
    cJSON_AddStringToObject(format, "cd", netProvInfo->netAddressInfo.WIFI.pwd);
    cJSON_AddItemToObject(jsonFinal, "oic", jsonArray = cJSON_CreateArray());
    cJSON_AddItemToArray(jsonArray, json);

    OIC_LOG_V(DEBUG, TAG, "ProvisionEnrollee : %s",
            cJSON_PrintUnformatted(jsonFinal));
    payload = cJSON_Print(jsonFinal);
    OIC_LOG_V(DEBUG, TAG, "Payload : %s", payload);

    OCStackResult ret = InvokeOCDoResource(query, OC_REST_PUT, OC_HIGH_QOS,
            ProvisionEnrolleeResponse, payload, NULL, 0);

    cJSON_Delete(json);
    return ret;
}

OCStackApplicationResult GetProvisioningStatusResponse(void* ctx,
        OCDoHandle handle, OCClientResponse * clientResponse) {
    ProvisioningInfo provInfo;

    if (clientResponse == NULL) {
        OIC_LOG(INFO, TAG,
                "getReqCB received NULL clientResponse. \
            Invoking Provisioing Status Callback");
        PrepareProvisioingStatusCB(&provInfo, clientResponse,
                DEVICE_NOT_PROVISIONED);
        cbData(provInfo);
        return OC_STACK_DELETE_TRANSACTION;
    }

    if (clientResponse->rcvdVendorSpecificHeaderOptions
            && clientResponse->numRcvdVendorSpecificHeaderOptions) {
        OIC_LOG(INFO, TAG, "Received vendor specific options");
        uint8_t i = 0;
        OCHeaderOption * rcvdOptions =
                clientResponse->rcvdVendorSpecificHeaderOptions;
        for (i = 0; i < clientResponse->numRcvdVendorSpecificHeaderOptions;
                i++) {
            if (((OCHeaderOption) rcvdOptions[i]).protocolID == OC_COAP_ID) {
                OIC_LOG_V(INFO, TAG,
                        "Received option with OC_COAP_ID and ID %u with",
                        ((OCHeaderOption) rcvdOptions[i]).optionID);

                OIC_LOG_BUFFER(INFO, TAG,
                        ((OCHeaderOption) rcvdOptions[i]).optionData,
                        MAX_HEADER_OPTION_DATA_LENGTH);
            }
        }
    }

    char sourceaddr[OIC_STRING_MAX_VALUE] = { '\0' };
    snprintf(sourceaddr, sizeof(sourceaddr), "%d.%d.%d.%d:%d%s",
            clientResponse->addr->addr[0], clientResponse->addr->addr[1],
            clientResponse->addr->addr[2], clientResponse->addr->addr[3],
            6298, OIC_PROVISIONING_URI);

    if (clientResponse->resJSONPayload) {
        cJSON *observeJson = cJSON_CreateObject();
        observeJson = cJSON_Parse(clientResponse->resJSONPayload);

        cJSON *ocArray = cJSON_GetObjectItem(observeJson, "oic");
        cJSON *ocArray_sub = cJSON_GetArrayItem(ocArray, 0);

        cJSON *representationArray = cJSON_GetObjectItem(ocArray_sub, "rep");
        char *ocArray_str = cJSON_PrintUnformatted(representationArray);

        if (strstr(ocArray_str, "[{}") == ocArray_str) {
            OIC_LOG_V(DEBUG, TAG, "invalid payload : %s", ocArray_str);
            cJSON_Delete(observeJson);
            return OC_STACK_DELETE_TRANSACTION;
        }

        int countofrep = cJSON_GetArraySize(representationArray);

        for (int i = 0; i < countofrep; ++i) {
            cJSON *arrayJSON = cJSON_GetArrayItem(representationArray, i);
            OIC_LOG_V(DEBUG, TAG, "rep#%d's name : %s", i, arrayJSON->string);

            switch (arrayJSON->type) {
            case cJSON_False:
            case cJSON_True:
                OIC_LOG_V(DEBUG, TAG, "rep#%d's value : %d", i,
                        arrayJSON->valueint);
                break;
            case cJSON_Number:
                OIC_LOG_V(DEBUG, TAG, "rep#%d's value : %f", i,
                        arrayJSON->valuedouble);
                break;
            case cJSON_String:
                OIC_LOG_V(DEBUG, TAG, "rep#%d's value : %s", i,
                        arrayJSON->valuestring);
                break;
            case cJSON_NULL:
            default:
                OIC_LOG_V(DEBUG, TAG, "rep#%d's value : NULL", i);
                break;
            }
        }
        cJSON_Delete(observeJson);

        if (ProvisionEnrollee(OC_HIGH_QOS, sourceaddr) != OC_STACK_OK) {
            OIC_LOG(INFO, TAG,
                    "GetProvisioningStatusResponse received NULL clientResponse. \
                Invoking Provisioing Status Callback");
            PrepareProvisioingStatusCB(&provInfo, clientResponse,
                    DEVICE_NOT_PROVISIONED);
            cbData(provInfo);

            return OC_STACK_DELETE_TRANSACTION;
        }
    } else {
        OIC_LOG(INFO, TAG,
                "GetProvisioningStatusResponse received NULL clientResponse. \
            Invoking Provisioing Status Callback");
        PrepareProvisioingStatusCB(&provInfo, clientResponse,
                DEVICE_NOT_PROVISIONED);
        cbData(provInfo);
        return OC_STACK_DELETE_TRANSACTION;
    }
    return OC_STACK_DELETE_TRANSACTION;
}

OCStackResult InvokeOCDoResource(const char* query, OCMethod method,
        OCQualityOfService qos, OCClientResponseHandler cb, const char* request,
        OCHeaderOption * options, uint8_t numOptions) {
    OCStackResult ret;
    OCCallbackData cbData;

    cbData.cb = cb;
    cbData.context = (void*) DEFAULT_CONTEXT_VALUE;
    cbData.cd = NULL;

    ret = OCDoResource(NULL, method, query, 0, request, OC_CONNTYPE, qos,
            &cbData, options, numOptions);

    if (ret != OC_STACK_OK) {
        OIC_LOG_V(ERROR, TAG, "OCDoResource returns error %d with method %d",
                ret, method);
    }

    return ret;
}

OCStackResult GetProvisioningStatus(OCQualityOfService qos, const char* query) {
    OCStackResult ret = OC_STACK_ERROR;
    OCHeaderOption options[MAX_HEADER_OPTIONS];

    OIC_LOG_V(INFO, TAG, "\n\nExecuting %s", __func__);

    uint8_t option0[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    uint8_t option1[] = { 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 };
    memset(options, 0, sizeof(OCHeaderOption) * MAX_HEADER_OPTIONS);
    options[0].protocolID = OC_COAP_ID;
    options[0].optionID = 2048;
    memcpy(options[0].optionData, option0, sizeof(option0));
    options[0].optionLength = 10;
    options[1].protocolID = OC_COAP_ID;
    options[1].optionID = 3000;
    memcpy(options[1].optionData, option1, sizeof(option1));
    options[1].optionLength = 10;

    ret = InvokeOCDoResource(query, OC_REST_GET, OC_HIGH_QOS,
            GetProvisioningStatusResponse, NULL, options, 2);
    return ret;
}

OCStackResult StartProvisioningProcess(const EnrolleeNWProvInfo_t *netInfo,
        OCProvisioningStatusCB provisioningStatusCallback) {
    OCStackResult result = OC_STACK_ERROR;
    ProvisioningInfo provInfo;

    if (netInfo->netAddressInfo.WIFI.ipAddress == NULL) {
        OIC_LOG(ERROR, TAG, "Request URI is NULL");
        return result;
    }

    if (provisioningStatusCallback == NULL) {
        OIC_LOG(ERROR, TAG, "ProvisioningStatusCallback is NULL");
        return result;
    }

    cbData = provisioningStatusCallback;

    //Copy Network Provisioning  Information
    netProvInfo = (EnrolleeNWProvInfo_t *)OICCalloc(1, sizeof(EnrolleeNWProvInfo_t));

    if (NULL == netProvInfo)
    {
        OIC_LOG(ERROR, TAG, "Invalid input..");
        return OC_STACK_ERROR;
    }
    memcpy(netProvInfo, netInfo, sizeof(EnrolleeNWProvInfo_t));

    OIC_LOG_V(DEBUG, TAG, "Network Provisioning Info. SSID = %s",
        netProvInfo->netAddressInfo.WIFI.ssid);

    OIC_LOG_V(DEBUG, TAG, "Network Provisioning Info. PWD = %s",
        netProvInfo->netAddressInfo.WIFI.pwd);

    /* Start a discovery query*/
    char szQueryUri[64] = { 0 };

    snprintf(szQueryUri, sizeof(szQueryUri), UNICAST_PROVISIONING_QUERY,
            netProvInfo->netAddressInfo.WIFI.ipAddress, IP_PORT);

    OIC_LOG_V(DEBUG, TAG, "szQueryUri = %s", szQueryUri);

    if (FindProvisioningResource(OC_HIGH_QOS, szQueryUri) != OC_STACK_OK) {
        OIC_LOG(ERROR, TAG,
                "FindProvisioningResource failed. \
            Invoking Provisioing Status Callback");

        ProvDeviceInfo provDeviceIndo;
        OCDevAddr dst = { };

        dst.addr[0] = netProvInfo->netAddressInfo.WIFI.ipAddress[0];
        dst.addr[1] = netProvInfo->netAddressInfo.WIFI.ipAddress[1];
        dst.addr[2] = netProvInfo->netAddressInfo.WIFI.ipAddress[2];
        dst.addr[3] = netProvInfo->netAddressInfo.WIFI.ipAddress[3];
        dst.addr[4] = (uint8_t) IP_PORT;
        dst.addr[5] = (uint8_t)(IP_PORT >> 8);

        provInfo.provStatus = DEVICE_NOT_PROVISIONED;
        provDeviceIndo.addr = &dst;
        provDeviceIndo.connType = OC_IPV4;
        provInfo.provDeviceInfo = provDeviceIndo;

        cbData(provInfo);
        return result;
    }
    result = OC_STACK_OK;

    return result;
}

void StopProvisioningProcess() {
    cbData = NULL;
}

// This is a function called back when a device is discovered
OCStackApplicationResult FindProvisioningResourceResponse(void* ctx,
        OCDoHandle handle, OCClientResponse * clientResponse) {
    OIC_LOG(INFO, TAG, PCF("Entering FindProvisioningResourceResponse"));

    OCStackApplicationResult response = OC_STACK_DELETE_TRANSACTION;

    ProvisioningInfo provInfo;

    if (clientResponse->result != OC_STACK_OK) {
        OIC_LOG(ERROR, TAG,
                "OCStack stop error. Calling Provisioing Status Callback");

        PrepareProvisioingStatusCB(&provInfo, clientResponse,
                DEVICE_NOT_PROVISIONED);

        cbData(provInfo);
        return response;
    }

    if (clientResponse) {
        // Parse header options from server
        uint16_t optionID;
        uint8_t* optionData;
        const char* payload;

        for (int i = 0; i < clientResponse->numRcvdVendorSpecificHeaderOptions;
                i++) {
            optionID =
                    clientResponse->rcvdVendorSpecificHeaderOptions[i].optionID;
            optionData =
                    clientResponse->rcvdVendorSpecificHeaderOptions[i].optionData;
            payload = clientResponse->resJSONPayload;
        }

        cJSON *discoveryJson = cJSON_CreateObject();
        discoveryJson = cJSON_Parse((char *) clientResponse->resJSONPayload);

        cJSON *ocArray = cJSON_GetObjectItem(discoveryJson, "oic");
        char *ocArray_str = cJSON_PrintUnformatted(ocArray);

        if (strstr(ocArray_str, "[{}") == ocArray_str) {
            OIC_LOG_V(DEBUG, TAG, "invalid payload : %s", ocArray_str);
            cJSON_Delete(discoveryJson);

            PrepareProvisioingStatusCB(&provInfo, clientResponse,
                    DEVICE_NOT_PROVISIONED);
            cbData(provInfo);
            return response;
        }

        char sourceaddr[OIC_STRING_MAX_VALUE] = { '\0' };
        snprintf(sourceaddr, sizeof(sourceaddr), "%d.%d.%d.%d:%d%s",
                clientResponse->addr->addr[0], clientResponse->addr->addr[1],
                clientResponse->addr->addr[2], clientResponse->addr->addr[3],
                IP_PORT, OIC_PROVISIONING_URI);

        OIC_LOG_V(DEBUG, TAG, "Discovered %s @ %s",
                clientResponse->resJSONPayload, sourceaddr);

        if (GetProvisioningStatus(OC_HIGH_QOS, sourceaddr) != OC_STACK_OK) {
            OIC_LOG(INFO, TAG,
                    "GetProvisioningStatus returned error. \
                                Invoking Provisioing Status Callback");
            PrepareProvisioingStatusCB(&provInfo, clientResponse,
                    DEVICE_NOT_PROVISIONED);
            cbData(provInfo);

            return OC_STACK_DELETE_TRANSACTION;
        }
    } else {
        // clientResponse is invalid
        OIC_LOG(ERROR, TAG,
                "Invalid response for Provisioning Discovery request. \
        Invoking Provisioing Status Callback");
        PrepareProvisioingStatusCB(&provInfo, clientResponse,
                DEVICE_NOT_PROVISIONED);
        cbData(provInfo);
        return response;
    }
    return OC_STACK_KEEP_TRANSACTION;
}

OCStackResult FindProvisioningResource(OCQualityOfService qos,
        const char* requestURI) {
    OCStackResult ret = OC_STACK_ERROR;

    OCCallbackData cbData;

    cbData.cb = FindProvisioningResourceResponse;
    cbData.context = (void*) DEFAULT_CONTEXT_VALUE;
    cbData.cd = NULL;

    ret = OCDoResource(NULL, OC_REST_GET, requestURI, 0, 0, OC_CONNTYPE,
            OC_LOW_QOS, &cbData, NULL, 0);

    if (ret != OC_STACK_OK) {
        OIC_LOG(ERROR, TAG, "OCStack resource error");
    }

    return ret;
}

OCStackApplicationResult SubscribeProvPresenceCallback(void* ctx, OCDoHandle handle,
        OCClientResponse* clientResponse) {
    OIC_LOG(INFO, TAG, PCF("Entering SubscribeProvPresenceCallback"));

    OCStackApplicationResult response = OC_STACK_DELETE_TRANSACTION;

    if (clientResponse->result != OC_STACK_OK) {
        OIC_LOG(ERROR, TAG, "OCStack stop error");
        return response;
    }

    if (clientResponse) {
        OIC_LOG(INFO, TAG, PCF("Client Response exists"));
        // Parse header options from server
        uint16_t optionID;
        uint8_t* optionData;
        const char* payload;

        for (int i = 0; i < clientResponse->numRcvdVendorSpecificHeaderOptions;
                i++) {
            optionID =
                    clientResponse->rcvdVendorSpecificHeaderOptions[i].optionID;
            optionData =
                    clientResponse->rcvdVendorSpecificHeaderOptions[i].optionData;
            payload = clientResponse->resJSONPayload;
        }

        cJSON *discoveryJson = cJSON_CreateObject();
        discoveryJson = cJSON_Parse((char *) clientResponse->resJSONPayload);

        cJSON *ocArray = cJSON_GetObjectItem(discoveryJson, "oic");
        char *ocArray_str = cJSON_PrintUnformatted(ocArray);

        if (strstr(ocArray_str, "[{}") == ocArray_str) {
            OIC_LOG_V(DEBUG, TAG, "invalid payload : %s", ocArray_str);
            cJSON_Delete(discoveryJson);
            return response;
        }

        char sourceIPAddr[OIC_STRING_MAX_VALUE] = { '\0' };
        snprintf(sourceIPAddr, sizeof(sourceIPAddr), "%d.%d.%d.%d",
                clientResponse->addr->addr[0], clientResponse->addr->addr[1],
                clientResponse->addr->addr[2], clientResponse->addr->addr[3]);

        OIC_LOG_V(DEBUG, TAG, "Discovered %s @ %s",
                clientResponse->resJSONPayload, sourceIPAddr);

        /* Start a discovery query*/
        char szQueryUri[64] = { 0 };
        OCQualityOfService qos = OC_NA_QOS;

        snprintf(szQueryUri, sizeof(szQueryUri), UNICAST_PROVISIONING_QUERY,
                sourceIPAddr, IP_PORT);

        if (FindProvisioningResource(qos, szQueryUri) != OC_STACK_OK) {
            OIC_LOG(ERROR, TAG, "FindProvisioningResource failed");
            return OC_STACK_KEEP_TRANSACTION;
        }
    } else {
        // clientResponse is invalid
        OIC_LOG(ERROR, TAG, PCF("Client Response is NULL!"));
    }
    return OC_STACK_KEEP_TRANSACTION;
}

OCStackResult SubscribeProvPresence(OCQualityOfService qos,
        const char* requestURI) {
    OCStackResult ret = OC_STACK_ERROR;

    OCCallbackData cbData;

    cbData.cb = &SubscribeProvPresenceCallback;
    cbData.context = (void*) DEFAULT_CONTEXT_VALUE;
    cbData.cd = NULL;

    ret = OCDoResource(NULL, OC_REST_PRESENCE, requestURI, 0, 0, OC_CONNTYPE,
            OC_LOW_QOS, &cbData, NULL, 0);

    if (ret != OC_STACK_OK) {
        OIC_LOG(ERROR, TAG, "OCStack resource error");
    }

    return ret;
}

OCStackResult FindNetworkResource() {
    OCStackResult ret = OC_STACK_ERROR;
    if (OCStop() != OC_STACK_OK) {
        OIC_LOG(ERROR, TAG, "OCStack stop error");
    }

    return ret;
}

void PrepareProvisioingStatusCB(ProvisioningInfo *provInfo,
        OCClientResponse * clientResponse, ProvStatus provStatus) {
    ProvDeviceInfo provDeviceIndo;
    OCDevAddr dst = { };

    uint16_t remotePortNum;
    OCDevAddrToPort((OCDevAddr *) clientResponse->addr, &remotePortNum);

    dst.addr[0] = clientResponse->addr->addr[0];
    dst.addr[1] = clientResponse->addr->addr[1];
    dst.addr[2] = clientResponse->addr->addr[2];
    dst.addr[3] = clientResponse->addr->addr[3];
    dst.addr[4] = (uint8_t) remotePortNum;
    dst.addr[5] = (uint8_t)(remotePortNum >> 8);

    provInfo->provStatus = provStatus;

    provDeviceIndo.addr = &dst;
    provDeviceIndo.connType = OC_IPV4;

    provInfo->provDeviceInfo = provDeviceIndo;
}

