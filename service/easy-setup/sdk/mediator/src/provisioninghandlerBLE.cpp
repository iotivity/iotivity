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
#include "ocpayload.h"
#include "provisioninghandler.h"
#include "common.h"
// External includes

#include "camutex.h"
#include "cathreadpool.h"
#include "logger.h"
#include "oic_malloc.h"
static bool sent_provision_request = false;
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

void ErrorCallback(ProvStatus status)
{
    ProvisioningInfo *provInfo = GetCallbackObjectOnError(status);
    cbData(provInfo);
}

OCStackResult InitProvisioningHandler()
{
    OCStackResult ret = OC_STACK_ERROR;
    /* Initialize OCStack*/
    if (OCInit(NULL, 0, OC_CLIENT) != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "OCStack init error");
        return ret;
    }

    g_provisioningMutex = ca_mutex_new();

    OIC_LOG(DEBUG, TAG, "ca_thread_pool_init initializing");

    if (CA_STATUS_OK != ca_thread_pool_init(2, &g_threadPoolHandle))
    {
        OIC_LOG(DEBUG, TAG, "thread_pool_init failed");
        return OC_STACK_ERROR;
    }

    g_provisioningCond = ca_cond_new();
    if (NULL == g_provisioningCond)
    {
        OIC_LOG(DEBUG, TAG, "Failed to create condition");
        ca_mutex_free(g_provisioningMutex);
        ca_thread_pool_free(g_threadPoolHandle);
        return OC_STACK_ERROR;
    }

    char *string = "listeningFunc invoked in a thread";
    if (CA_STATUS_OK != ca_thread_pool_add_task(g_threadPoolHandle, listeningFunc, (void *) string))
    {
        OIC_LOG(DEBUG, TAG, "thread_pool_add_task failed");
        ca_thread_pool_free(g_threadPoolHandle);
        ca_mutex_unlock(g_provisioningMutex);
        ca_mutex_free(g_provisioningMutex);
        ca_cond_free(g_provisioningCond);
        return OC_STACK_ERROR;
    }
    return OC_STACK_OK;
}

OCStackResult TerminateProvisioningHandler()
{
    OCStackResult ret = OC_STACK_ERROR;
    if (OCStop() != OC_STACK_OK)
    {
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

void listeningFunc(void *data)
{
    while (!g_provisioningCondFlag)
    {
        OCStackResult result;

        ca_mutex_lock(g_provisioningMutex);
        result = OCProcess();
        ca_mutex_unlock(g_provisioningMutex);

        if (result != OC_STACK_OK)
        {
            OIC_LOG(ERROR, TAG, "OCStack stop error");
        }

        // To minimize CPU utilization we may wish to do this with sleep
        sleep(1);
    }
}

OCStackApplicationResult ProvisionEnrolleeResponse(void* ctx, OCDoHandle handle,
        OCClientResponse * clientResponse)
{
    OIC_LOG_V(DEBUG, TAG, "INSIDE ProvisionEnrolleeResponse");
    ProvisioningInfo *provInfo;

    if (!ValidateEnrolleResponse(clientResponse))
    {
        ErrorCallback( DEVICE_NOT_PROVISIONED);
        return OC_STACK_DELETE_TRANSACTION;
    }

    char* tnn;
    char* cd;

    OCRepPayload* input = (OCRepPayload*) (clientResponse->payload);

    while (input)
    {

        int64_t ps;
        if (OCRepPayloadGetPropInt(input, OC_RSRVD_ES_PS, &ps))
        {

            if (ps == 1)
            {
                input = input->next;
                continue;
            }
            else
            {
                OIC_LOG_V(DEBUG, TAG, "PS is NOT proper");
                goto Error;

            }
        }

        if (OCRepPayloadGetPropString(input, OC_RSRVD_ES_TNN, &tnn))
        {
            if (!strcmp(tnn, netProvInfo->netAddressInfo.WIFI.ssid))
            {
                OIC_LOG_V(DEBUG, TAG, "SSID is proper");
                input = input->next;
                continue;
            }
            else
            {
                OIC_LOG_V(DEBUG, TAG, "SSID is NOT proper");
                goto Error;
            }
        }

        if (OCRepPayloadGetPropString(input, OC_RSRVD_ES_CD, &cd))
        {
            if (!strcmp(cd, netProvInfo->netAddressInfo.WIFI.pwd))
            {
                OIC_LOG_V(DEBUG, TAG, "Password is proper");
                input = input->next;
                continue;
            }
            else
            {
                OIC_LOG_V(DEBUG, TAG, "Password is NOT proper");
                goto Error;
            }
        }

        LogProvisioningResponse(input->values);

        input = input->next;

    }

    SuccessCallback(clientResponse);

    return OC_STACK_KEEP_TRANSACTION;

    Error:
    {

        ErrorCallback( DEVICE_NOT_PROVISIONED);

        return OC_STACK_DELETE_TRANSACTION;
    }

}

OCStackResult ProvisionEnrollee(OCQualityOfService qos, const char* query, const char* resUri,
        OCDevAddr *destination)
{
    OIC_LOG_V(INFO, TAG, "Sleeping for 2 seconds");
    sleep(2);
    OIC_LOG_V(INFO, TAG, "\n\nExecuting ProvisionEnrollee%s", __func__);

    OCRepPayload* payload = OCRepPayloadCreate();

    OCRepPayloadSetUri(payload, resUri);
    OCRepPayloadSetPropString(payload, OC_RSRVD_ES_TNN, netProvInfo->netAddressInfo.WIFI.ssid);
    OCRepPayloadSetPropString(payload, OC_RSRVD_ES_CD, netProvInfo->netAddressInfo.WIFI.pwd);

    OIC_LOG_V(DEBUG, TAG, "OCPayload ready for ProvisionEnrollee");

    OCStackResult ret = InvokeOCDoResource(query, OC_REST_PUT, destination, OC_HIGH_QOS,
            ProvisionEnrolleeResponse, payload, NULL, 0);

    return ret;
}
OCStackApplicationResult GetProvisioningStatusResponse(void* ctx, OCDoHandle handle,
        OCClientResponse * clientResponse)
{
    if (sent_provision_request == true)
        return OC_STACK_DELETE_TRANSACTION;
    sent_provision_request = true;
    OIC_LOG_V(DEBUG, TAG, "INside  GetProvisioningStatusResponse");

    ProvisioningInfo *provInfo;

    if (!ValidateEnrolleResponse(clientResponse))
    {
        ErrorCallback( DEVICE_NOT_PROVISIONED);
        ClearMemory();
        return OC_STACK_DELETE_TRANSACTION;
    }

    OCRepPayload* input = (OCRepPayload*) (clientResponse->payload);

    char query[OIC_STRING_MAX_VALUE] =
    { '\0' };
    char resURI[MAX_URI_LENGTH] =
    { '\0' };

    OIC_LOG_V(DEBUG, TAG, "resUri = %s", input->uri);

    strncpy(resURI, input->uri, sizeof(resURI));

    snprintf(query, sizeof(query), UNICAST_PROVISIONING_QUERY_BLE, clientResponse->addr->addr);

    OIC_LOG_V(DEBUG, TAG, "query = %s", query);

    OCDevAddr *devaddress = &clientResponse->devAddr;
    devaddress->adapter = OC_ADAPTER_GATT_BTLE;

    //OCPayloadLogRep(DEBUG,TAG,input);

    if (ProvisionEnrollee(OC_HIGH_QOS, query, OC_RSRVD_ES_URI_PROV, devaddress) != OC_STACK_OK)
    {
        OIC_LOG(INFO, TAG,
                "GetProvisioningStatusResponse received NULL clientResponse.Invoking Provisioing Status Callback");

        ErrorCallback( DEVICE_NOT_PROVISIONED);
        ClearMemory();
        return OC_STACK_DELETE_TRANSACTION;
    }

    return OC_STACK_KEEP_TRANSACTION;

}

OCStackResult InvokeOCDoResource(const char* query, OCMethod method, const OCDevAddr *dest,
        OCQualityOfService qos, OCClientResponseHandler cb, OCRepPayload* payload,
        OCHeaderOption * options, uint8_t numOptions)
{
    OCStackResult ret;
    OCCallbackData cbData;

    cbData.cb = cb;
    cbData.context = (void*) DEFAULT_CONTEXT_VALUE;
    cbData.cd = NULL;

    ret = OCDoResource(NULL, method, query, dest, (OCPayload*) payload, OC_CONNTYPE_BLE, qos,
            &cbData, options, numOptions);

    if (ret != OC_STACK_OK)
    {
        OIC_LOG_V(ERROR, TAG, "OCDoResource returns error %d with method %d", ret, method);
    }

    return ret;
}

OCStackResult GetProvisioningStatus(OCQualityOfService qos, const char* query,
        const OCDevAddr *destination)
{
    OCStackResult ret = OC_STACK_ERROR;
    OIC_LOG_V(INFO, TAG, "\n\nExecuting %s %d", __func__, destination->adapter);
    ret = InvokeOCDoResource(query, OC_REST_GET, destination, OC_HIGH_QOS,
            GetProvisioningStatusResponse, NULL, NULL, 0);
    return ret;
}

OCStackResult StartProvisioningProcess(const EnrolleeNWProvInfo_t *netInfo,
        OCProvisioningStatusCB provisioningStatusCallback)
{

    OCStackResult result = OC_STACK_ERROR;

    if (!ValidateEasySetupParams(netInfo, provisioningStatusCallback))
    {
        goto Error;
    }

    //Only basis test is done for below API
    if (!SetProgress(provisioningStatusCallback))
    {
        // Device provisioning session is running already.
        OIC_LOG(INFO, TAG, PCF("Device provisioning session is running already"));
        goto Error;
    }

    if (!ConfigEnrolleeObject(netInfo))
    {
        goto Error;
    }

    if (CA_STATUS_OK != ca_thread_pool_add_task(g_threadPoolHandle, FindProvisioningResource,
            (void *) ""))
    {
        goto Error;
    }

    return OC_STACK_OK;

    Error:
    {
        ErrorCallback( DEVICE_NOT_PROVISIONED);
        ClearMemory();
        return OC_STACK_ERROR;
    }

}

void StopProvisioningProcess()
{
    //Only basis test is done for below API
    ResetProgress();
}

// This is a function called back when a device is discovered
OCStackApplicationResult FindProvisioningResourceResponse(void* ctx, OCDoHandle handle,
        OCClientResponse * clientResponse)
{

    OIC_LOG(INFO, TAG, PCF("Entering FindProvisioningResourceResponse"));

    if (!ValidateFinddResourceResponse(clientResponse))
    {
        ErrorCallback( DEVICE_NOT_PROVISIONED);
        return OC_STACK_DELETE_TRANSACTION;
    }

    OCStackApplicationResult response = OC_STACK_DELETE_TRANSACTION;

    ProvisioningInfo *provInfo;
    char szQueryUri[64] =
    { 0 };

    OCDiscoveryPayload* discoveryPayload = (OCDiscoveryPayload*) (clientResponse->payload);

    // Need to conform if below check is required or not. As Null check of clientResponse->payload is already performed above
    if (!discoveryPayload)
    {
        OIC_LOG_V(DEBUG, TAG, "Failed To parse");
        ErrorCallback( DEVICE_NOT_PROVISIONED);
        return OC_STACK_DELETE_TRANSACTION;
    }

    OIC_LOG_V(DEBUG, TAG, "resUri = %s", discoveryPayload->resources->uri);

    snprintf(szQueryUri, sizeof(szQueryUri), UNICAST_PROVISIONING_QUERY_BLE,
            clientResponse->addr->addr);

    OCDevAddr *devaddress = &clientResponse->devAddr;
    devaddress->adapter = OC_ADAPTER_GATT_BTLE;

    if (strcmp(netProvInfo->netAddressInfo.WIFI.ipAddress, clientResponse->devAddr.addr))
        OIC_LOG(INFO, TAG, "equal");
    else
        OIC_LOG_V(INFO, TAG, "unequal %s %s", netProvInfo->netAddressInfo.WIFI.ipAddress,
                clientResponse->devAddr.addr);

    OIC_LOG_V(DEBUG, TAG, "query before GetProvisioningStatus call = %s %d", szQueryUri,
            devaddress->adapter);

    if (GetProvisioningStatus(OC_HIGH_QOS, szQueryUri, devaddress) != OC_STACK_OK)
    {
        ErrorCallback( DEVICE_NOT_PROVISIONED);
        return OC_STACK_DELETE_TRANSACTION;
    }

    return OC_STACK_KEEP_TRANSACTION;

}

void FindProvisioningResource(void *data)
{
    OCStackResult ret = OC_STACK_ERROR;

    /* Start a discovery query*/
    char szQueryUri[64] =
    { 0 };

    snprintf(szQueryUri, sizeof(szQueryUri), MULTICAST_PROVISIONING_QUERY_BLE);

    OIC_LOG_V(DEBUG, TAG, "szQueryUri = %s", szQueryUri);

    OCCallbackData ocCBData;

    ocCBData.cb = FindProvisioningResourceResponse;
    ocCBData.context = (void*) DEFAULT_CONTEXT_VALUE;
    ocCBData.cd = NULL;

    ret = OCDoResource(NULL, OC_REST_DISCOVER, szQueryUri, NULL, NULL, OC_CONNTYPE_BLE, OC_LOW_QOS,
            &ocCBData, NULL, 0);

    if (ret != OC_STACK_OK)
    {
        ErrorCallback( DEVICE_NOT_PROVISIONED);
        ClearMemory();
    }
}

OCStackApplicationResult SubscribeProvPresenceCallback(void* ctx, OCDoHandle handle,
        OCClientResponse* clientResponse)
{
    OIC_LOG(INFO, TAG, PCF("Entering SubscribeProvPresenceCallback"));

    OCStackApplicationResult response = OC_STACK_DELETE_TRANSACTION;

    if (clientResponse->result != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "OCStack stop error");
        return response;
    }

    if (clientResponse)
    {
        OIC_LOG(INFO, TAG, PCF("Client Response exists"));

        if (clientResponse->payload && clientResponse->payload->type != PAYLOAD_TYPE_REPRESENTATION)
        {
            OIC_LOG_V(DEBUG, TAG, "Incoming payload not a representation");
            return response;
        }

        OCRepPayload* discoveryPayload = (OCRepPayload*) (clientResponse->payload);
        if (!discoveryPayload)
        {
            OIC_LOG_V(DEBUG, TAG, "invalid payload");
            return response;
        }

        char sourceIPAddr[OIC_STRING_MAX_VALUE] =
        { '\0' };
        snprintf(sourceIPAddr, sizeof(sourceIPAddr), "%s", clientResponse->addr->addr);

        OIC_LOG_V(DEBUG, TAG, "Discovered %s @ %s", discoveryPayload->uri, sourceIPAddr);

        /* Start a discovery query*/
        char szQueryUri[64] =
        { 0 };

        snprintf(szQueryUri, sizeof(szQueryUri), UNICAST_PROVISIONING_QUERY, sourceIPAddr, IP_PORT);

        /*if (FindProvisioningResource(qos, szQueryUri) != OC_STACK_OK) {
         OIC_LOG(ERROR, TAG, "FindProvisioningResource failed");
         return OC_STACK_KEEP_TRANSACTION;
         }*/
    }
    else
    {
        // clientResponse is invalid
        OIC_LOG(ERROR, TAG, PCF("Client Response is NULL!"));
    }
    return OC_STACK_KEEP_TRANSACTION;
}

OCStackResult SubscribeProvPresence(OCQualityOfService qos, const char* requestURI)
{
    OCStackResult ret = OC_STACK_ERROR;

    OCCallbackData cbData;

    cbData.cb = &SubscribeProvPresenceCallback;
    cbData.context = (void*) DEFAULT_CONTEXT_VALUE;
    cbData.cd = NULL;

    ret = OCDoResource(NULL, OC_REST_PRESENCE, requestURI, 0, 0, OC_CONNTYPE_BLE, OC_LOW_QOS,
            &cbData, NULL, 0);

    if (ret != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "OCStack resource error");
    }

    return ret;
}

OCStackResult FindNetworkResource()
{
    OCStackResult ret = OC_STACK_ERROR;
    if (OCStop() != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "OCStack stop error");
    }

    return ret;
}

ProvisioningInfo* PrepareProvisioingStatusCB(OCClientResponse * clientResponse,
        ProvStatus provStatus)
{

    ProvisioningInfo *provInfo = (ProvisioningInfo *) OICCalloc(1, sizeof(ProvisioningInfo));

    if (provInfo == NULL)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to allocate memory");
        return NULL;
    }

    OCDevAddr *devAddr = (OCDevAddr *) OICCalloc(1, sizeof(OCDevAddr));

    if (devAddr == NULL)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to allocate memory");
        return NULL;
    }

    strncpy(devAddr->addr, clientResponse->addr->addr, sizeof(devAddr->addr));
    devAddr->port = clientResponse->addr->port;

    provInfo->provDeviceInfo.addr = devAddr;

    provInfo->provStatus = provStatus;

    return provInfo;
}

bool ValidateEasySetupParams(const EnrolleeNWProvInfo_t *netInfo,
        OCProvisioningStatusCB provisioningStatusCallback)
{

    if (netInfo == NULL || netInfo->netAddressInfo.WIFI.ipAddress == NULL)
    {
        OIC_LOG(ERROR, TAG, "Request URI is NULL");
        return false;
    }

    if (provisioningStatusCallback == NULL)
    {
        OIC_LOG(ERROR, TAG, "ProvisioningStatusCallback is NULL");
        return false;
    }

    return true;

}

bool InProgress()
{

    // It means already Easy Setup provisioning session is going on.
    if (NULL != cbData)
    {
        OIC_LOG(ERROR, TAG, "Easy setup session is already in progress");
        return true;
    }

    return false;
}

bool SetProgress(OCProvisioningStatusCB provisioningStatusCallback)
{
    ca_mutex_lock(g_provisioningMutex);

    if (InProgress())
        return false;

    cbData = provisioningStatusCallback;

    ca_mutex_unlock(g_provisioningMutex);

    return true;
}

bool ResetProgress()
{
    ca_mutex_lock(g_provisioningMutex);

    cbData = NULL;

    ca_mutex_unlock(g_provisioningMutex);
}

ProvisioningInfo* CreateCallBackObject()
{

    ProvisioningInfo *provInfo = (ProvisioningInfo *) OICCalloc(1, sizeof(ProvisioningInfo));

    if (provInfo == NULL)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to allocate memory");
        return NULL;
    }

    OCDevAddr *devAddr = (OCDevAddr *) OICCalloc(1, sizeof(OCDevAddr));

    if (devAddr == NULL)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to allocate memory");
        return NULL;
    }

    provInfo->provDeviceInfo.addr = devAddr;

    return provInfo;

}

ProvisioningInfo* GetCallbackObjectOnError(ProvStatus status)
{

    ProvisioningInfo *provInfo = CreateCallBackObject();
    strncpy(provInfo->provDeviceInfo.addr->addr, netProvInfo->netAddressInfo.WIFI.ipAddress,
            sizeof(provInfo->provDeviceInfo.addr->addr));
    provInfo->provDeviceInfo.addr->port = IP_PORT;
    provInfo->provStatus = status;
    return provInfo;
}

ProvisioningInfo* GetCallbackObjectOnSuccess(OCClientResponse * clientResponse,
        ProvStatus provStatus)
{
    ProvisioningInfo *provInfo = CreateCallBackObject();
    strncpy(provInfo->provDeviceInfo.addr->addr, clientResponse->addr->addr,
            sizeof(provInfo->provDeviceInfo.addr->addr));
    provInfo->provDeviceInfo.addr->port = clientResponse->addr->port;
    provInfo->provStatus = provStatus;
    return provInfo;
}

bool ValidateFinddResourceResponse(OCClientResponse * clientResponse)
{

    if (!(clientResponse) || !(clientResponse->payload))
    {

        OIC_LOG_V(INFO, TAG, "ProvisionEnrolleeResponse received Null clientResponse");

        return false;

    }
    return true;
}

bool ValidateEnrolleResponse(OCClientResponse * clientResponse)
{

    if (!(clientResponse) || !(clientResponse->payload))
    {

        OIC_LOG_V(INFO, TAG, "ProvisionEnrolleeResponse received Null clientResponse");

        return false;

    }

    if (clientResponse->payload->type != PAYLOAD_TYPE_REPRESENTATION)
    {

        OIC_LOG_V(DEBUG, TAG, "Incoming payload not a representation");
        return false;

    }

    // If flow reachese here means no error condition hit.
    return true;

}

void SuccessCallback(OCClientResponse * clientResponse)
{
    ProvisioningInfo *provInfo = GetCallbackObjectOnSuccess(clientResponse, DEVICE_PROVISIONED);
    cbData(provInfo);
}

bool ClearMemory()
{

    OIC_LOG(DEBUG, TAG, "thread_pool_add_task of FindProvisioningResource failed");
    ca_thread_pool_free(g_threadPoolHandle);
    ca_mutex_unlock(g_provisioningMutex);
    ca_mutex_free(g_provisioningMutex);
    ca_cond_free(g_provisioningCond);

    return true;

}

bool ConfigEnrolleeObject(const EnrolleeNWProvInfo_t *netInfo)
{

    //Copy Network Provisioning  Information
    netProvInfo = (EnrolleeNWProvInfo_t *) OICCalloc(1, sizeof(EnrolleeNWProvInfo_t));

    if (netProvInfo == NULL)
    {
        OIC_LOG(ERROR, TAG, "Invalid input..");
        return false;
    }

    memcpy(netProvInfo, netInfo, sizeof(EnrolleeNWProvInfo_t));

    OIC_LOG_V(DEBUG, TAG, "Network Provisioning Info. SSID = %s",
            netProvInfo->netAddressInfo.WIFI.ssid);

    OIC_LOG_V(DEBUG, TAG, "Network Provisioning Info. PWD = %s",
            netProvInfo->netAddressInfo.WIFI.pwd);

    OIC_LOG_V(DEBUG, TAG, "Network Provisioning Info. MAC ADDRESS = %s",
            netProvInfo->netAddressInfo.WIFI.ipAddress);

    return true;

}

void LogProvisioningResponse(OCRepPayloadValue* val)
{

    switch (val->type)
    {
        case OCREP_PROP_NULL:
            OIC_LOG_V(DEBUG, TAG, "\t\t%s: NULL", val->name);
            break;
        case OCREP_PROP_INT:
            OIC_LOG_V(DEBUG, TAG, "\t\t%s(int):%lld", val->name, val->i);
            break;
        case OCREP_PROP_DOUBLE:
            OIC_LOG_V(DEBUG, TAG, "\t\t%s(double):%f", val->name, val->d);
            break;
        case OCREP_PROP_BOOL:
            OIC_LOG_V(DEBUG, TAG, "\t\t%s(bool):%s", val->name, val->b ? "true" : "false");
            break;
        case OCREP_PROP_STRING:
            OIC_LOG_V(DEBUG, TAG, "\t\t%s(string):%s", val->name, val->str);
            break;
        case OCREP_PROP_OBJECT:
            // Note: Only prints the URI (if available), to print further, you'll
            // need to dig into the object better!
            OIC_LOG_V(DEBUG, TAG, "\t\t%s(OCRep):%s", val->name, val->obj->uri);
            break;
        case OCREP_PROP_ARRAY:
            switch (val->arr.type)
            {
                case OCREP_PROP_INT:
                    OIC_LOG_V(DEBUG, TAG, "\t\t%s(int array):%lld x %lld x %lld", val->name,
                            val->arr.dimensions[0], val->arr.dimensions[1], val->arr.dimensions[2]);
                    break;
                case OCREP_PROP_DOUBLE:
                    OIC_LOG_V(DEBUG, TAG, "\t\t%s(double array):%lld x %lld x %lld", val->name,
                            val->arr.dimensions[0], val->arr.dimensions[1], val->arr.dimensions[2]);
                    break;
                case OCREP_PROP_BOOL:
                    OIC_LOG_V(DEBUG, TAG, "\t\t%s(bool array):%lld x %lld x %lld", val->name,
                            val->arr.dimensions[0], val->arr.dimensions[1], val->arr.dimensions[2]);
                    break;
                case OCREP_PROP_STRING:
                    OIC_LOG_V(DEBUG, TAG, "\t\t%s(string array):%lld x %lld x %lld", val->name,
                            val->arr.dimensions[0], val->arr.dimensions[1], val->arr.dimensions[2]);
                    break;
                case OCREP_PROP_OBJECT:
                    OIC_LOG_V(DEBUG, TAG, "\t\t%s(OCRep array):%lld x %lld x %lld", val->name,
                            val->arr.dimensions[0], val->arr.dimensions[1], val->arr.dimensions[2]);
                    break;
                default:
                    //OIC_LOG_V(ERROR, TAG, "\t\t%s <-- Unknown/unsupported array type!",
                    //  val->name);
                    break;
            }
            break;
        default:
            /*OC_LOG_V(ERROR, TAG
             , "\t\t%s <-- Unknown type!", val->name);*/
            break;
    }
}

