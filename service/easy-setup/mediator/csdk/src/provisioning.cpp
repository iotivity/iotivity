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
#include "oic_malloc.h"
#include "oic_string.h"

#define ES_PROV_TAG "EASY_SETUP_PROVISIONING"

bool gProvisioningCondFlag = false;

static ProvConfig *gProvConfig;
static WiFiOnboadingConnection *gOnboardConn;
static char gSzFindResourceQueryUri[64] =
{ 0 };

/**
 * @var cbData
 * @brief Callback for providing provisioning status callback to application
 */
static OCProvisioningStatusCB cbData = NULL;

/**
 * Utility function for error callback.
 */
void ErrorCallback(ProvStatus status)
{
    ProvisioningInfo *provInfo = GetCallbackObjectOnError(status);
    if(cbData != NULL)
    {
        cbData(provInfo);
    }
}

/**
 * Functions implementing the exposed APIs.
 */
OCStackResult InitProvisioningHandler()
{
    OCStackResult ret = OC_STACK_ERROR;
    /* Initialize OCStack*/
    if (OCInit(NULL, 0, OC_CLIENT) != OC_STACK_OK)
    {
        OIC_LOG(ERROR, ES_PROV_TAG, "OCStack init error");
        return ret;
    }

    return OC_STACK_OK;
}

OCStackResult StartProvisioningProcess(const ProvConfig *netInfo,
        WiFiOnboadingConnection *onboardConn, OCProvisioningStatusCB provisioningStatusCallback,
        char *findResQuery)
{

    if (findResQuery != NULL)
    {
        OICStrcpy(gSzFindResourceQueryUri, sizeof(gSzFindResourceQueryUri) - 1, findResQuery);
    }
    else
    {
        OIC_LOG(ERROR, ES_PROV_TAG, PCF("Find resource query is NULL"));
        goto Error;
    }

    pthread_t thread_handle;

    if (!ValidateEasySetupParams(netInfo, onboardConn, provisioningStatusCallback))
    {
        goto Error;
    }

    if (!SetProgress(provisioningStatusCallback))
    {
        // Device provisioning session is running already.
        OIC_LOG(INFO, ES_PROV_TAG, PCF("Device provisioning session is running already"));
        goto Error;
    }

    if (!ConfigEnrolleeObject(netInfo, onboardConn))
    {
        goto Error;
    }

    if (pthread_create(&thread_handle, NULL, FindProvisioningResource, NULL))
    {
        goto Error;

    }

    pthread_join(thread_handle, NULL);

    return OC_STACK_OK;

    Error:
    {
        ErrorCallback(DEVICE_NOT_PROVISIONED);
        ClearMemory();
        return OC_STACK_ERROR;
    }

}

void StopProvisioningProcess()
{
    ResetProgress();
}

OCStackResult TerminateProvisioningHandler()
{
    OCStackResult ret = OC_STACK_ERROR;
    if (OCStop() != OC_STACK_OK)
    {
        OIC_LOG(ERROR, ES_PROV_TAG, "OCStack stop error");
    }

    gProvisioningCondFlag = true;
    ResetProgress();

    ret = OC_STACK_OK;
    return ret;
}

OCStackApplicationResult TriggerNetworkConnectionResponse(void* /*ctx*/, OCDoHandle /*handle*/,
        OCClientResponse *clientResponse)
{
    OIC_LOG_V(DEBUG, ES_PROV_TAG, "INSIDE TriggerNetworkConnectionResponse");

    // If user stopped the process then return from this function;
    if (IsSetupStopped())
    {
        ErrorCallback(DEVICE_NOT_PROVISIONED);
        ClearMemory();
        return OC_STACK_DELETE_TRANSACTION;
    }

    if (!ValidateEnrolleeBasicResponse(clientResponse))
    {
        ErrorCallback(DEVICE_NOT_PROVISIONED);
        return OC_STACK_DELETE_TRANSACTION;
    }

    SuccessCallback(clientResponse);
    return OC_STACK_KEEP_TRANSACTION;
}

OCStackResult TriggerNetworkConnection(OCQualityOfService qos, const char *query,
        const char *resUri, OCDevAddr *destination, int /*pauseBeforeStart*/)
{
    OIC_LOG_V(INFO, ES_PROV_TAG, "Inside TriggerNetworkConnection");

    OCRepPayload *payload = OCRepPayloadCreate();

    OCRepPayloadSetUri(payload, resUri);
    OCRepPayloadSetPropInt(payload, OC_RSRVD_ES_TR, ES_PS_TRIGGER_CONNECTION);

    OIC_LOG_V(DEBUG, ES_PROV_TAG, "Triggering network connection from Mediator");

    OCStackResult ret = InvokeOCDoResource(query, OC_REST_POST, destination, qos,
            TriggerNetworkConnectionResponse, payload, NULL, 0);

    return ret;
}



bool ClearMemory()
{

    OIC_LOG(DEBUG, ES_PROV_TAG, "thread_pool_add_task of FindProvisioningResource failed");
    OICFree(gProvConfig);
    OICFree(gOnboardConn);

    return true;

}

bool ConfigEnrolleeObject(const ProvConfig *netInfo, WiFiOnboadingConnection *connection)
{

    //Copy Network Provisioning  Information
    gProvConfig = (ProvConfig *) OICCalloc(1, sizeof(ProvConfig));
    gOnboardConn = (WiFiOnboadingConnection *) OICCalloc(1, sizeof(WiFiOnboadingConnection));

    if (gProvConfig == NULL)
    {
        OIC_LOG(ERROR, ES_PROV_TAG, "Invalid input..");
        return false;
    }

    if (gOnboardConn == NULL)
    {
        OIC_LOG(ERROR, ES_PROV_TAG, "Invalid input..");
        return false;
    }

    memcpy(gProvConfig, netInfo, sizeof(ProvConfig));
    memcpy(gOnboardConn, connection, sizeof(WiFiOnboadingConnection));

    OIC_LOG_V(DEBUG, ES_PROV_TAG, "Network Provisioning Info. SSID = %s",
            gProvConfig->provData.WIFI.ssid);

    OIC_LOG_V(DEBUG, ES_PROV_TAG, "Network Provisioning Info. PWD = %s",
            gProvConfig->provData.WIFI.pwd);

    return true;

}

void LogProvisioningResponse(OCRepPayloadValue * val)
{

    switch (val->type)
    {
        case OCREP_PROP_NULL:
            OIC_LOG_V(DEBUG, ES_PROV_TAG, "\t\t%s: NULL", val->name);
            break;
        case OCREP_PROP_INT:
            OIC_LOG_V(DEBUG, ES_PROV_TAG, "\t\t%s(int):%lld", val->name, val->i);
            break;
        case OCREP_PROP_DOUBLE:
            OIC_LOG_V(DEBUG, ES_PROV_TAG, "\t\t%s(double):%f", val->name, val->d);
            break;
        case OCREP_PROP_BOOL:
            OIC_LOG_V(DEBUG, ES_PROV_TAG, "\t\t%s(bool):%s", val->name, val->b ? "true" : "false");
            break;
        case OCREP_PROP_STRING:
            OIC_LOG_V(DEBUG, ES_PROV_TAG, "\t\t%s(string):%s", val->name, val->str);
            break;
        case OCREP_PROP_OBJECT:
            // Note: Only prints the URI (if available), to print further, you'll
            // need to dig into the object better!
            OIC_LOG_V(DEBUG, ES_PROV_TAG, "\t\t%s(OCRep):%s", val->name, val->obj->uri);
            break;
        case OCREP_PROP_ARRAY:
            switch (val->arr.type)
            {
                case OCREP_PROP_INT:
                    OIC_LOG_V(DEBUG, ES_PROV_TAG, "\t\t%s(int array):%zu x %zu x %zu",
                            val->name,
                            val->arr.dimensions[0], val->arr.dimensions[1],
                            val->arr.dimensions[2]);
                    break;
                case OCREP_PROP_DOUBLE:
                    OIC_LOG_V(DEBUG, ES_PROV_TAG, "\t\t%s(double array):%zu x %zu x %zu",
                            val->name,
                            val->arr.dimensions[0], val->arr.dimensions[1],
                            val->arr.dimensions[2]);
                    break;
                case OCREP_PROP_BOOL:
                    OIC_LOG_V(DEBUG, ES_PROV_TAG, "\t\t%s(bool array):%zu x %zu x %zu",
                            val->name,
                            val->arr.dimensions[0], val->arr.dimensions[1],
                            val->arr.dimensions[2]);
                    break;
                case OCREP_PROP_STRING:
                    OIC_LOG_V(DEBUG, ES_PROV_TAG, "\t\t%s(string array):%zu x %zu x %zu",
                            val->name,
                            val->arr.dimensions[0], val->arr.dimensions[1],
                            val->arr.dimensions[2]);
                    break;
                case OCREP_PROP_OBJECT:
                    OIC_LOG_V(DEBUG, ES_PROV_TAG, "\t\t%s(OCRep array):%zu x %zu x %zu",
                            val->name,
                            val->arr.dimensions[0], val->arr.dimensions[1],
                            val->arr.dimensions[2]);
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}

OCStackResult FindNetworkResource()
{
    OCStackResult ret = OC_STACK_ERROR;
    if (OCStop() != OC_STACK_OK)
    {
        OIC_LOG(ERROR, ES_PROV_TAG, "OCStack stop error");
    }

    return ret;
}

ProvisioningInfo *PrepareProvisioingStatusCB(OCClientResponse *clientResponse,
        ProvStatus provStatus)
{

    ProvisioningInfo *provInfo = (ProvisioningInfo *) OICCalloc(1, sizeof(ProvisioningInfo));

    if (provInfo == NULL)
    {
        OIC_LOG_V(ERROR, ES_PROV_TAG, "Failed to allocate memory");
        return NULL;
    }

    OCDevAddr *devAddr = (OCDevAddr *) OICCalloc(1, sizeof(OCDevAddr));

    if (devAddr == NULL)
    {
        OIC_LOG_V(ERROR, ES_PROV_TAG, "Failed to allocate memory");
        OICFree(provInfo);
        return NULL;
    }

    OICStrcpy(devAddr->addr, sizeof(devAddr->addr), clientResponse->addr->addr);

    devAddr->port = clientResponse->addr->port;

    provInfo->provDeviceInfo.addr = devAddr;

    provInfo->provStatus = provStatus;

    return provInfo;
}

bool InProgress()
{

    // It means already Easy Setup provisioning session is going on.
    if (NULL != cbData)
    {
        OIC_LOG(ERROR, ES_PROV_TAG, "Easy setup session is already in progress");
        return true;
    }

    return false;
}

bool SetProgress(OCProvisioningStatusCB provisioningStatusCallback)
{

    if (InProgress())
        return false;

    cbData = provisioningStatusCallback;

    return true;
}

bool ResetProgress()
{

    cbData = NULL;
    return true;
}

ProvisioningInfo *CreateCallBackObject()
{

    ProvisioningInfo *provInfo = (ProvisioningInfo *) OICCalloc(1, sizeof(ProvisioningInfo));

    if (provInfo == NULL)
    {
        OIC_LOG_V(ERROR, ES_PROV_TAG, "Failed to allocate memory");
        return NULL;
    }

    OCDevAddr *devAddr = (OCDevAddr *) OICCalloc(1, sizeof(OCDevAddr));

    if (devAddr == NULL)
    {
        OIC_LOG_V(ERROR, ES_PROV_TAG, "Failed to allocate memory");
        OICFree(provInfo);
        return NULL;
    }

    provInfo->provDeviceInfo.addr = devAddr;

    return provInfo;

}

ProvisioningInfo *GetCallbackObjectOnError(ProvStatus status)
{

    ProvisioningInfo *provInfo = CreateCallBackObject();
    OICStrcpy(provInfo->provDeviceInfo.addr->addr, sizeof(provInfo->provDeviceInfo.addr->addr),
            gOnboardConn->ipAddress);

    provInfo->provDeviceInfo.addr->port = IP_PORT;
    provInfo->provStatus = status;
    return provInfo;
}

ProvisioningInfo *GetCallbackObjectOnSuccess(OCClientResponse *clientResponse,
        ProvStatus provStatus)
{
    ProvisioningInfo *provInfo = CreateCallBackObject();
    OICStrcpy(provInfo->provDeviceInfo.addr->addr, sizeof(provInfo->provDeviceInfo.addr->addr),
            clientResponse->addr->addr);

    provInfo->provDeviceInfo.addr->port = clientResponse->addr->port;
    provInfo->provStatus = provStatus;
    return provInfo;
}

bool ValidateFindResourceResponse(OCClientResponse * clientResponse)
{
    if (!(clientResponse) || !(clientResponse->payload))
    {
        OIC_LOG_V(INFO, ES_PROV_TAG, "Received Null clientResponse");
        return false;
    }

    if (clientResponse->payload->type != PAYLOAD_TYPE_DISCOVERY)
    {
        OIC_LOG_V(DEBUG, ES_PROV_TAG, "Payload is not discovery type");
        return false;
    }

    return true;
}

bool ValidateEnrolleeResponse(OCClientResponse * clientResponse)
{
    if (!(clientResponse) || !(clientResponse->payload))
    {
        OIC_LOG_V(INFO, ES_PROV_TAG, "Received Null clientResponse");
        return false;
    }

    if (clientResponse->payload->type != PAYLOAD_TYPE_REPRESENTATION)
    {
        OIC_LOG_V(DEBUG, ES_PROV_TAG, "Incoming payload is not a representation");
        return false;
    }

    // If flow reachese here means no error condition hit.
    return true;
}

bool ValidateEnrolleeBasicResponse(OCClientResponse * clientResponse)
{
    if (!clientResponse)
    {
        OIC_LOG_V(INFO, ES_PROV_TAG, "Received Null clientResponse");
        return false;
    }

    if(clientResponse->result != OC_STACK_OK)
    {
        OIC_LOG_V(INFO, ES_PROV_TAG, "Received error response");
        return false;
    }

    // If flow reaches, then there no error condition hit.
    return true;
}


void SuccessCallback(OCClientResponse * clientResponse)
{
    ProvisioningInfo *provInfo = GetCallbackObjectOnSuccess(clientResponse, DEVICE_PROVISIONED);
    if(cbData != NULL)
    {
        cbData(provInfo);
    }
}

void* FindProvisioningResource(void* /*data*/)
{

    // If user stopped the process before thread get scheduled then check and return from this function;
    if (IsSetupStopped())
    {
        ErrorCallback(DEVICE_NOT_PROVISIONED);
        ClearMemory();
        return NULL;
    }

    OCStackResult ret = OC_STACK_ERROR;

    OIC_LOG_V(DEBUG, ES_PROV_TAG, "szFindResourceQueryUri = %s", gSzFindResourceQueryUri);

    OCCallbackData ocCBData;

    ocCBData.cb = FindProvisioningResourceResponse;
    ocCBData.context = (void *) EASY_SETUP_DEFAULT_CONTEXT_VALUE;
    ocCBData.cd = NULL;

    ret = OCDoResource(NULL, OC_REST_DISCOVER, gSzFindResourceQueryUri, NULL, NULL,
            gProvConfig->connType, OC_LOW_QOS, &ocCBData, NULL, 0);

    if (ret != OC_STACK_OK)
    {
        ErrorCallback(DEVICE_NOT_PROVISIONED);
        ClearMemory();
    }

    return NULL;
}

OCStackResult InvokeOCDoResource(const char *query, OCMethod method, const OCDevAddr *dest,
        OCQualityOfService qos, OCClientResponseHandler cb, OCRepPayload *payload,
        OCHeaderOption *options, uint8_t numOptions)
{
    OCStackResult ret;
    OCCallbackData cbData;

    cbData.cb = cb;
    cbData.context = (void *) EASY_SETUP_DEFAULT_CONTEXT_VALUE;
    cbData.cd = NULL;

    ret = OCDoResource(NULL, method, query, dest, (OCPayload *) payload, gProvConfig->connType, qos,
            &cbData, options, numOptions);

    if (ret != OC_STACK_OK)
    {
        OIC_LOG_V(ERROR, ES_PROV_TAG, "OCDoResource returns error %d with method %d", ret, method);
    }

    return ret;
}

OCStackResult ProvisionEnrollee(OCQualityOfService qos, const char *query, const char *resUri,
        OCDevAddr *destination, int pauseBeforeStart)
{

    // This sleep is required in case of BLE provisioning due to packet drop issue.
    OIC_LOG_V(INFO, ES_PROV_TAG, "Sleeping for %d seconds", pauseBeforeStart);
    sleep(pauseBeforeStart);
    OIC_LOG_V(INFO, ES_PROV_TAG, "\n\nExecuting ProvisionEnrollee%s", __func__);

    OCRepPayload *payload = OCRepPayloadCreate();

    OCRepPayloadSetUri(payload, resUri);
    OCRepPayloadSetPropString(payload, OC_RSRVD_ES_TNN, gProvConfig->provData.WIFI.ssid);
    OCRepPayloadSetPropString(payload, OC_RSRVD_ES_CD, gProvConfig->provData.WIFI.pwd);

    OIC_LOG_V(DEBUG, ES_PROV_TAG, "OCPayload ready for ProvisionEnrollee");

    OCStackResult ret = InvokeOCDoResource(query, OC_REST_POST, destination, qos,
            ProvisionEnrolleeResponse, payload, NULL, 0);

    return ret;
}

bool IsSetupStopped()
{
    return (cbData == NULL) ? true : false;
}
