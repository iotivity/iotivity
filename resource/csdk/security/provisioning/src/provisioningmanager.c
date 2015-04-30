/* *****************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * *****************************************************************/

// Defining _POSIX_C_SOURCE macro with 199309L (or greater) as value
// causes header files to expose definitions
// corresponding to the POSIX.1b, Real-time extensions
// (IEEE Std 1003.1b-1993) specification
//
// For this specific file, see use of clock_gettime,
// Refer to http://pubs.opengroup.org/stage7tc1/functions/clock_gettime.html
// and to http://man7.org/linux/man-pages/man2/clock_gettime.2.html

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#include "cJSON.h"
#include "ocmalloc.h"
#include "logger.h"
#include "cacommon.h"
#include "cainterface.h"
#include "provisioningmanager.h"
#include "credentialgenerator.h"
#include "global.h"
#include "base64.h"
#include "aclresource.h"

const char SP_OWNER_TRANSFER_METHOD_ATTR[] = "oxm";
const char SP_DEVICE_ID_ATTR[]             = "deviceID";
const char SP_SUPP_METHOD_ATTR[]           = "sm";
const char SP_OC[]                         = "oc";
typedef enum
{
    SP_NO_MASK                = (0       ),
    SP_DISCOVERY_STARTED      = (0x1 << 1),
    SP_DISCOVERY_ERROR        = (0x1 << 2),
    SP_DISCOVERY_DONE         = (0x1 << 3),
    SP_UP_OWN_TR_METH_STARTED = (0x1 << 4),
    SP_UP_OWN_TR_METH_ERROR   = (0x1 << 5),
    SP_UP_OWN_TR_METH_DONE    = (0x1 << 6),
    SP_LIST_METHODS_STARTED   = (0x1 << 7),
    SP_LIST_METHODS_ERROR     = (0x1 << 8),
    SP_LIST_METHODS_DONE      = (0x1 << 9),
    SP_UPDATE_OP_MODE_STARTED = (0x1 << 10),
    SP_UPDATE_OP_MODE_ERROR   = (0x1 << 11),
    SP_UPDATE_OP_MODE_DONE    = (0x1 << 12),
    SP_UPDATE_OWNER_STARTED   = (0x1 << 13),
    SP_UPDATE_OWNER_ERROR     = (0x1 << 14),
    SP_UPDATE_OWNER_DONE      = (0x1 << 15),
    SP_PROV_ACL_STARTED       = (0x1 << 16),
    SP_PROV_ACL_ERROR         = (0x1 << 17),
    SP_PROV_ACL_DONE          = (0x1 << 18),
    SP_UP_HASH_STARTED        = (0x1 << 19),
    SP_UP_HASH_ERROR          = (0x1 << 20),
    SP_UP_HASH_DONE           = (0x1 << 21),
    SP_PROV_CRED_STARTED      = (0x1 << 22),
    SP_PROV_CRED_ERROR        = (0x1 << 23),
    SP_PROV_CRED_DONE         = (0x1 << 24)

} SPProvisioningStates;

#define SP_MAX_BUF_LEN 1024
#define TAG "SPProvisionAPI"
#define ENABLE_TLS_ECDH_anon_WITH_AES_128_CBC_SHA 1
#define COAP_QUERY "coap://%s:%d%s"
#define AUTO_BASE 0
#define CA_SECURE_PORT   5684

void (*handler)(const CARemoteEndpoint_t *, const CAResponseInfo_t *);

/**
 * Device UUID. The value will be set by the provisioning tool.
 */
static OicUuid_t gDeviceUUID;

/**
 * CA token to keep track of response.
 */
static CAToken_t gToken = NULL;

/**
 * start pointer for discovered device linked list.
 */
static SPTargetDeviceInfo_t *gStartOfDiscoveredDevices = NULL;

/**
 * current pointer of device linked list.
 */
static SPTargetDeviceInfo_t *gCurrent = NULL;

/**
 * Variable to keep track of various request.
 */
static uint32_t gStateManager = 0;

/**
 * Variable to keep track of supported methods of target device.
 */
static OicSecDpom_t *gDeviceSupportedMethods = NULL;

/**
 * Variable to track number of supported methods.
 */
static int gNumberOfSupportedOM = 0;

/**
 * Variable for storing provisioning tool's provisioning capabilities
 * Must be in decreasing order of preference. More prefered method should
 * have lower array index.
 */
static OicSecDpom_t gProvisioningToolCapability[] = { SINGLE_SERVICE_CLIENT_DRIVEN };

/**
 * Number of supported provisioning methods
 * current version supports only one.
 */
static int gNumOfProvisioningMethodsPT = 1;

/**
 * Secure String copy function
 * @param[in] destination  Pointer to destination string.
 * @param[in] source       Pointer to source string.
 * @return pointer to destination string, NULL in case of error.
 */
static inline char *SPStringCopy(char *destination, const char *source, size_t num)
{
    if (strncpy(destination, source, num))
    {
        destination[num - 1] = '\0';
        return destination;
    }
    return NULL;
}

/**
 * Function to convert CA result code to SP result code.
 *
 * @return result code of SP corresponding to that of CA.
 */
static SPResult convertCAResultToSPResult(CAResult_t caResult)
{
    switch (caResult)
    {
        case CA_STATUS_OK:
            {
                return SP_RESULT_SUCCESS;
            }
        case CA_STATUS_INVALID_PARAM:
            {
                return SP_RESULT_CONN_INVALID_PARAM;
            }
        case CA_ADAPTER_NOT_ENABLED:
            {
                return SP_RESULT_CONN_SERVER_STARTED_ALREADY;
            }
        case CA_SERVER_STARTED_ALREADY:
            {
                return SP_RESULT_CONN_SERVER_STARTED_ALREADY;
            }
        case CA_SERVER_NOT_STARTED:
            {
                return SP_RESULT_CONN_SERVER_NOT_STARTED;
            }
        case CA_DESTINATION_NOT_REACHABLE:
            {
                return SP_RESULT_CONN_DESTINATION_NOT_REACHABLE;
            }
        case CA_SOCKET_OPERATION_FAILED:
            {
                return SP_RESULT_CONN_SOCKET_OPERATION_FAILED;
            }
        case CA_SEND_FAILED:
            {
                return SP_RESULT_CONN_SEND_FAILED;
            }
        case CA_RECEVIE_FAILED:
            {
                return SP_RESULT_CONN_RECEVIE_FAILED;
            }
        case CA_MEMORY_ALLOC_FAILED:
            {
                return SP_RESULT_CONN_MEMORY_ALLOC_FAILED;
            }
        case CA_REQUEST_TIMEOUT:
            {
                return SP_RESULT_CONN_REQUEST_TIMEOUT;
            }
        case CA_DESTINATION_DISCONNECTED:
            {
                return SP_RESULT_CONN_DESTINATION_DISCONNECTED;
            }
        case CA_STATUS_FAILED:
            {
                return SP_RESULT_CONN_STATUS_FAILED;
            }
        case CA_NOT_SUPPORTED:
            {
                return SP_RESULT_CONN_NOT_SUPPORTED;
            }
        default:
            {
                return SP_RESULT_INTERNAL_ERROR;
            }
    }
}

/**
 * Convert SP network types to CA network types,
 *
 * @param[in]  connType   connection type.
 * @return  CA connectivity type corresponding to SP connectivity type.
 */
static CAConnectivityType_t getConnectivity(SPConnectivityType connType)
{
    switch (connType)
    {
        case SP_CONN_ETHERNET:
            {
                return CA_ETHERNET;
            }
        case SP_CONN_WIFI:
            {
                return CA_WIFI;
            }
        case SP_CONN_EDR:
            {
                return CA_EDR;
            }
        case SP_CONN_LE:
            {
                return CA_LE;
            }
        default:
            {
                return CA_ETHERNET;
            }
    }
    return CA_ETHERNET;
}

/**
 * Convert CA network types to SP network types,
 *
 * @param[in]  connType   connection type.
 * @return  SPConnectitivty type corresponding to CAConnectivityType_t.
 */
static SPConnectivityType getConnectivitySP(CAConnectivityType_t connType)
{
    switch (connType)
    {
        case CA_ETHERNET:
            {
                return SP_CONN_ETHERNET;
            }
        case CA_WIFI:
            {
                return SP_CONN_WIFI;
            }
        case CA_EDR:
            {
                return SP_CONN_EDR;
            }
        case CA_LE:
            {
                return SP_CONN_LE;
            }
        default:
            {
                return SP_CONN_ETHERNET;
            }
    }
    return SP_CONN_ETHERNET;
}

/**
 * Function to delete memory allocated to linked list.
 *
 */
static void deleteList()
{
    SPTargetDeviceInfo_t *current = gStartOfDiscoveredDevices;

    while (current)
    {
        SPTargetDeviceInfo_t *next = current->next;
        OCFree(current->oxm);
        OCFree(current);
        current = next;
    }
    gStartOfDiscoveredDevices = NULL;
}

/**
 * Timeout implementation.
 * @param[in]  timeout  Timeout in seconds. with 0 it will wait forever for success.
 * @param[in]  mask     Mask of operation and 0 for no mask.
 * @return SP_RESULT_SUCCESS on success otherwise error.
 */
static SPResult SPTimeout(unsigned short timeout, uint32_t mask)
{
    struct timespec startTime = {};
    struct timespec currTime  = {};

    CAResult_t res = SP_RESULT_SUCCESS;
#ifdef _POSIX_MONOTONIC_CLOCK
    int clock_res = clock_gettime(CLOCK_MONOTONIC, &startTime);
#else
    int clock_res = clock_gettime(CLOCK_REALTIME, &startTime);
#endif
    if (0 != clock_res)
    {
        return SP_RESULT_INTERNAL_ERROR;
    }
    while (CA_STATUS_OK == res)
    {
        res = CAHandleRequestResponse();
#ifdef _POSIX_MONOTONIC_CLOCK
        clock_res = clock_gettime(CLOCK_MONOTONIC, &currTime);
#else
        clock_res = clock_gettime(CLOCK_REALTIME, &currTime);
#endif
        if (0 != clock_res)
        {
            return SP_RESULT_INTERNAL_ERROR;
        }
        long elapsed = (currTime.tv_sec - startTime.tv_sec);
        if (SP_NO_MASK == mask)
        {
            if (elapsed > timeout)
            {
                return SP_RESULT_SUCCESS;
            }
        }
        else
        {
            if (gStateManager & mask)
            {
                return SP_RESULT_SUCCESS;
            }
            if ((elapsed > timeout) && timeout)
            {
                return SP_RESULT_INTERNAL_ERROR;
            }
        }
    }
    return convertCAResultToSPResult(res);
}

/**
 * Function to send request to resource server.
 * @param[in] uri             Request URI.
 * @param[in] payload         Payload to be sent with data. NULL is case message
 *                            doesn't have payload.
 * @param[in] payloadLen      Size of data to be sent.
 * @param[in] token           CA token.
 * @param[in] method          method to be used for sending rquest.
 * @param[in] conntype        Connectivity type.
 * @return  CA_STATUS_OK on success, otherwise error code.
 */
static CAResult_t sendCARequest(CAURI_t uri, char *payload, int payloadLen,
                                CAToken_t token, CAMethod_t method, SPConnectivityType conntype)
{
    CARemoteEndpoint_t *endpoint = NULL;
    CAConnectivityType_t caConnType = getConnectivity(conntype);
    if (CA_STATUS_OK != CACreateRemoteEndpoint(uri, caConnType, &endpoint) || !endpoint)
    {
        OC_LOG(ERROR, TAG, "Failed to create remote endpoint");
        CADestroyRemoteEndpoint(endpoint);
        return CA_STATUS_FAILED;
    }
    CAMessageType_t msgType = CA_MSG_CONFIRM; // set confirmable message type
    CAInfo_t requestData = { 0 };
    requestData.token = token;
    if (payload && '\0' != (*(payload + payloadLen)))
    {
        OC_LOG(ERROR, TAG, "Payload not properly terminated.");
        return SP_RESULT_INTERNAL_ERROR;
    }
    requestData.payload = payload;
    requestData.type = msgType;
    CARequestInfo_t requestInfo = { 0 };
    requestInfo.method = method;
    requestInfo.info = requestData;
    CAResult_t caResult = CA_STATUS_OK;
    caResult = CASendRequest(endpoint, &requestInfo);
    if (CA_STATUS_OK != caResult)
    {
        OC_LOG(ERROR, TAG, "Send Request Error !!");
    }
    CADestroyRemoteEndpoint(endpoint);
    return caResult;
}

/**
 * addDevice to list.
 *
 * @param[in] deviceId              Device id of the target device.
 * @param[in] ip                    IP of target device.
 * @param[in] port                  port of remote server.
 * @param[in] ownerTransferMethods  array of ownership transfer methods
 * @param[in] numberOfMethods       number of ownership transfer methods.
 * @param[in] connType              connectivity type of endpoint.
 * @return SP_RESULT_SUCCESS for success and errorcode otherwise.
 */
static SPResult addDevice(const char *deviceId, const char *ip, int port,
                          uint16_t *ownerTransferMethods,
                          int numberOfMethods, SPConnectivityType connType)
{
    if (NULL == ip || NULL == deviceId || 0 >= port)
    {
        return SP_RESULT_INVALID_PARAM;
    }
    SPTargetDeviceInfo_t *ptr = (SPTargetDeviceInfo_t *) OCMalloc(sizeof(SPTargetDeviceInfo_t));
    if (NULL == ptr)
    {
        OC_LOG(ERROR, TAG, "Error while allocating memory for linkedlist node !!");
        return SP_RESULT_MEM_ALLOCATION_FAIL;
    }
    memcpy(ptr->deviceId.id, deviceId, sizeof(ptr->deviceId.id));

    SPStringCopy(ptr->ip, ip, sizeof(ptr->ip));
    ptr->port = port;
    ptr->oxm = ownerTransferMethods;
    ptr->numberofOxmMethods = numberOfMethods;
    ptr->connType = connType;
    ptr->next = NULL;
    if (NULL == gStartOfDiscoveredDevices)
    {
        gStartOfDiscoveredDevices = ptr;
        gCurrent = ptr;
    }
    else
    {
        gCurrent->next = ptr;
        gCurrent = ptr;
    }
    return SP_RESULT_SUCCESS;
}

/**
 * Function to provide timeframe in which response can be received.
 *
 * @param[in]  timeout   Timeout in seconds.
 * @return  SP_RESULT_SUCCESS on success , otherwise error code.
 */
static SPResult SPWaitForResponse(unsigned short timeout)
{
    return SPTimeout(timeout, SP_NO_MASK);
}

/**
 * Function to select appropriate  provisioning method.
 *
 * @param[in]   supportedMethodsList   List of supported methods
 * @param[out]  selectedMethod         Selected methods
 * @return  SP_SUCCESS on success
 */
static SPResult selectProvisioningMethod(uint16_t *supportedMethods, int numberOfMethods,
        uint16_t *selectedMethod)
{
    /*
     TODO Logic to find appropiate method and assign it to out param
     for mid april release method at index 0 will be returned.
     */
    *selectedMethod = supportedMethods[0];
    return SP_RESULT_SUCCESS;
}

/**
 * Response handler for discovery.
 *
 * @param[in] object       Remote endpoint object
 * @param[in] requestInfo  Datastructure containing request information.
 */
static void ProvisionDiscoveryHandler(const CARemoteEndpoint_t *object,
                                      const CAResponseInfo_t *responseInfo)
{
    if ((gStateManager & SP_DISCOVERY_STARTED) && gToken)
    {
        // Response handler for discovery.
        // Expected response: {"deviceID":"xxx","oxm":["0","1"]}
        if (0 == strncmp(gToken, responseInfo->info.token, CA_MAX_TOKEN_LEN))
        {
            OC_LOG(INFO, TAG, "Inside ProvisionDiscoveryHandler.");
            cJSON *resJson = cJSON_Parse(responseInfo->info.payload);
            if (NULL == resJson)
            {
                OC_LOG(ERROR, TAG, "Invalid Json");
                gStateManager |= SP_DISCOVERY_ERROR;
                return;
            }
            cJSON *objItem = cJSON_GetObjectItem(resJson, SP_OC);
            cJSON *obj = cJSON_GetArrayItem(objItem, 0);
            cJSON *cJsonDevId = cJSON_GetObjectItem(obj, SP_DEVICE_ID_ATTR);
            if ( NULL == cJsonDevId)
            {
                OC_LOG(ERROR, TAG, "Invalid JSON data");
                gStateManager |= SP_DISCOVERY_ERROR;
                return;
            }
            char *stDeviceId = cJsonDevId->valuestring;

            cJSON *oxmObjectItem = cJSON_GetObjectItem(resJson, SP_OWNER_TRANSFER_METHOD_ATTR);
            int numSupportingMethods = cJSON_GetArraySize(oxmObjectItem);

            uint16_t *supportedMethods = (uint16_t *) OCMalloc(numSupportingMethods * sizeof(uint16_t));
            if (NULL == supportedMethods)
            {
                OC_LOG(ERROR, TAG, "Error while allocating memory.");
                gStateManager |= SP_DISCOVERY_ERROR;
                if (resJson)
                {
                    cJSON_Delete(resJson);
                }
                return;
            }

            for (int i = 0; i < numSupportingMethods; i++)
            {
                cJSON *cJsonSuppMethods = cJSON_GetArrayItem(oxmObjectItem, i);
                if (NULL == cJsonSuppMethods)
                {
                    OC_LOG(ERROR, TAG, "Invalid JSON data");
                    gStateManager |= SP_DISCOVERY_ERROR;
                    return;
                }
                const char *stSupportedMethod = cJsonSuppMethods->valuestring;
                if (stSupportedMethod)
                {
                    *((supportedMethods) + i) = atoi(stSupportedMethod);
                }
            }

            SPConnectivityType connType = getConnectivitySP(object->connectivityType);
            SPResult res = addDevice(stDeviceId, object->addressInfo.IP.ipAddress, object->addressInfo.IP.port,
                                     supportedMethods,
                                     numSupportingMethods, connType);
            if (SP_RESULT_SUCCESS != res)
            {
                OC_LOG(ERROR, TAG, "Error while adding data to linkedlist.");
                gStateManager = gStateManager | SP_DISCOVERY_ERROR;
                if (resJson)
                {
                    cJSON_Delete(resJson);
                }
                if (supportedMethods)
                {
                    OCFree(supportedMethods);
                }
                return;
            }
            if (resJson)
            {
                cJSON_Delete(resJson);
            }
            OC_LOG(INFO, TAG, "Exiting ProvisionDiscoveryHandler.");
            gStateManager |= SP_DISCOVERY_DONE;
        }
    }
}

/**
 * Response handler ownership transfer.
 *
 * @param[in] object       Remote endpoint object
 * @param[in] requestInfo  Datastructure containing request information.
 */
static void OwnerShipTransferModeHandler(const CARemoteEndpoint_t *object,
        const CAResponseInfo_t *responseInfo)
{
    if ((gStateManager & SP_UP_OWN_TR_METH_STARTED) && gToken)
    {
        // response handler for ownership tranfer
        OC_LOG(INFO, TAG, "Inside OwnerShipTransferModeHandler.");
        if (strncmp(gToken, responseInfo->info.token, CA_MAX_TOKEN_LEN) == 0)
        {
            if (CA_SUCCESS == responseInfo->result)
            {
                gStateManager |= SP_UP_OWN_TR_METH_DONE;
                OC_LOG(INFO, TAG, "Exiting OwnerShipTransferModeHandler.");
            }
            else
            {
                gStateManager |= SP_UP_OWN_TR_METH_ERROR;
                OC_LOG(ERROR, TAG, "Error in OwnerShipTransferModeHandler.");
            }
        }
    }
}

/**
 * Response handler list methods.
 *
 * @param[in] object       Remote endpoint object
 * @param[in] requestInfo  Datastructure containing request information.
 */
static void ListMethodsHandler(const CARemoteEndpoint_t *object,
                               const CAResponseInfo_t *responseInfo)
{
    // expected response : {"IsOp":"FALSE","sm":["0x3"]}
    if ((gStateManager & SP_LIST_METHODS_STARTED) && gToken)
    {
        OC_LOG(INFO, TAG, "Inside ListMethodsHandler.");
        if (strncmp(gToken, responseInfo->info.token, CA_MAX_TOKEN_LEN) == 0)
        {
            if (CA_SUCCESS == responseInfo->result)
            {
                cJSON *resJson = cJSON_Parse(responseInfo->info.payload);
                if (NULL == resJson)
                {
                    OC_LOG(ERROR, TAG, "Invalid Json");
                    gStateManager |= SP_LIST_METHODS_ERROR;
                    return;
                }
                cJSON *resSM = cJSON_GetObjectItem(resJson, SP_SUPP_METHOD_ATTR);
                int numOfSupportedMethods = cJSON_GetArraySize(resSM);
                if (0 == numOfSupportedMethods)
                {
                    OC_LOG(ERROR, TAG, "Invalid Json data.");
                    gStateManager |= SP_LIST_METHODS_ERROR;
                    if (resJson)
                    {
                        cJSON_Delete(resJson);
                    }
                    return;
                }
                gNumberOfSupportedOM = numOfSupportedMethods;
                gDeviceSupportedMethods = (OicSecDpom_t *) OCMalloc(
                                              numOfSupportedMethods * sizeof(OicSecDpom_t));
                if (NULL == gDeviceSupportedMethods)
                {
                    OC_LOG(ERROR, TAG, "Error while memory allocation.");
                    gStateManager |= SP_LIST_METHODS_ERROR;
                    if (resJson)
                    {
                        cJSON_Delete(resJson);
                    }
                    return;
                }

                for (int i = 0; i < numOfSupportedMethods; i++)
                {
                    const char *stSuppMethods = cJSON_GetArrayItem(resSM, i)->valuestring;
                    gDeviceSupportedMethods[i] = strtol(stSuppMethods, NULL, AUTO_BASE);
                }
                if (resJson)
                {
                    cJSON_Delete(resJson);
                }
                gStateManager |= SP_LIST_METHODS_DONE;
                OC_LOG(INFO, TAG, "Exiting ListMethodsHandler.");
            }
        }
    }
}

/**
 * Response handler for update operation mode.
 *
 * @param[in] object       Remote endpoint object
 * @param[in] requestInfo  Datastructure containing request information.
 */
static void OperationModeUpdateHandler(const CARemoteEndpoint_t *object,
                                       const CAResponseInfo_t *responseInfo)
{
    if ((gStateManager & SP_UPDATE_OP_MODE_STARTED) && gToken)
    {
        if (0 == strncmp(gToken, responseInfo->info.token, CA_MAX_TOKEN_LEN))
        {
            OC_LOG(INFO, TAG, "Inside OperationModeUpdateHandler.");
            if (CA_SUCCESS == responseInfo->result)
            {
                gStateManager |= SP_UPDATE_OP_MODE_DONE;
                OC_LOG(INFO, TAG, "Exiting OperationModeUpdateHandler.");
            }
            else
            {
                gStateManager |= SP_UPDATE_OP_MODE_ERROR;
                OC_LOG(ERROR, TAG, "Error in OperationModeUpdateHandler.");
            }
        }
    }
}

/**
 * Response handler for ownership transfer.
 *
 * @param[in] object       Remote endpoint object
 * @param[in] requestInfo  Datastructure containing request information.
 */
static void OwnerShipUpdateHandler(const CARemoteEndpoint_t *object,
                                   const CAResponseInfo_t *responseInfo)
{
    if ((gStateManager & SP_UPDATE_OWNER_STARTED) && gToken)
    {
        // response handler for ownership tranfer
        if (0 == strncmp(gToken, responseInfo->info.token, CA_MAX_TOKEN_LEN))
        {
            OC_LOG(INFO, TAG, "Inside OwnerShipUpdateHandler.");
            if (CA_SUCCESS == responseInfo->result)
            {
                gStateManager |= SP_UPDATE_OWNER_DONE;
                OC_LOG(INFO, TAG, "Exiting OwnerShipUpdateHandler.");
            }
            else
            {
                gStateManager |= SP_UPDATE_OWNER_ERROR;
                OC_LOG(ERROR, TAG, "Error in OwnerShipUpdateHandler.");
            }
        }
    }
}

/**
 * Response handler for ACL provisioning.
 *
 * @param[in] object       Remote endpoint object
 * @param[in] requestInfo  Datastructure containing request information.
 */
static void ACLProvisioningHandler(const CARemoteEndpoint_t *object,
                                   const CAResponseInfo_t *responseInfo)
{
    if ((gStateManager & SP_PROV_ACL_STARTED) && gToken)
    {
        OC_LOG(INFO, TAG, "Inside ACLProvisioningHandler.");
        // response handler for ACL provisioning.
        if (0 == strncmp(gToken, responseInfo->info.token, CA_MAX_TOKEN_LEN))
        {
            if (CA_CREATED == responseInfo->result)
            {
                OC_LOG(INFO, TAG, "Exiting ACLProvisioningHandler.");
                gStateManager |= SP_PROV_ACL_DONE;
            }
            else
            {
                OC_LOG(ERROR, TAG, "Error in ACLProvisioningHandler.");
                gStateManager |= SP_PROV_ACL_ERROR;
            }
        }
    }
}

/**
 * Response handler for provisioning finalization.
 *
 * @param[in] object       Remote endpoint object
 * @param[in] requestInfo  Datastructure containing request information.
 */
static void FinalizeProvisioningHandler(const CARemoteEndpoint_t *object,
                                        const CAResponseInfo_t *responseInfo)
{
    if ((gStateManager & SP_UP_HASH_STARTED) && gToken)
    {
        // response handler for finalize provisioning.
        if (0 == strncmp(gToken, responseInfo->info.token, CA_MAX_TOKEN_LEN))
        {
            OC_LOG(INFO, TAG, "Inside FinalizeProvisioningHandler.");
            if (CA_CREATED == responseInfo->result)
            {
                gStateManager |= SP_UP_HASH_DONE;
                OC_LOG(INFO, TAG, "Exiting FinalizeProvisioningHandler.");
            }
            else
            {
                gStateManager |= SP_UP_HASH_ERROR;
                OC_LOG(ERROR, TAG, "Error in FinalizeProvisioningHandler.");
            }
        }
    }
}

/**
 * Response handler for Credential provisioning.
 *
 * @param[in] object        Remote endpoint object
 * @param[in] requestInfo   Datastructure containing request information.
 */
static void CredProvisioningHandler(const CARemoteEndpoint_t *object,
                                    const CAResponseInfo_t *responseInfo)
{
    if ((gStateManager & SP_PROV_CRED_STARTED) && gToken)
    {
        // response handler for CRED provisioning.
        OC_LOG(INFO, TAG, "Inside CredProvisioningHandler.");
        if (0 == strncmp(gToken, responseInfo->info.token, CA_MAX_TOKEN_LEN))
        {
            if (CA_CREATED == responseInfo->result)
            {
                gStateManager |= SP_PROV_CRED_DONE;
                OC_LOG(INFO, TAG, "Exiting CredProvisioningHandler.");
            }
            else
            {
                gStateManager |= SP_PROV_CRED_ERROR;
                OC_LOG(ERROR, TAG, "Error in CredProvisioningHandler.");
            }
        }
    }
}

/**
 * Response Handler
 *
 * @param[in] object        Remote endpoint object
 * @param[in] responseInfo  Datastructure containing response information.
 */
static void SPResponseHandler(const CARemoteEndpoint_t *object,
                              const CAResponseInfo_t *responseInfo)
{
    if ((NULL != responseInfo) && (NULL != responseInfo->info.token))
    {
        handler(object, responseInfo);
    }
}

/**
 * Request Handler
 *
 * @param[in] object       Remote endpoint object
 * @param[in] requestInfo  Datastructure containing request information.
 */
static void SPRequestHandler(const CARemoteEndpoint_t *object, const CARequestInfo_t *requestInfo)
{
    OC_LOG(INFO, TAG, "Request Handler.");
}

/**
 * Function to find the resources using multicast discovery.
 *
 * @param[in]   timeout     timeout in secs
 * @return  SP_RESULT_SUCCESS normally otherwise error code.
 */
static SPResult findResource(unsigned short timeout)
{
    static char DOXM_OWNED_FALSE_MULTICAST_QUERY[] = "/oic/sec/doxm?Owned=\"FALSE\"";
    CAResult_t res = CAGenerateToken(&gToken);
    if (CA_STATUS_OK != res)
    {
        OC_LOG(ERROR, TAG, "Error while generating token.");
        return SP_RESULT_INTERNAL_ERROR;
    }
    res = CAFindResource(DOXM_OWNED_FALSE_MULTICAST_QUERY, gToken);
    handler = &ProvisionDiscoveryHandler;
    gStateManager |= SP_DISCOVERY_STARTED;
    if (CA_STATUS_OK != res)
    {
        OC_LOG(ERROR, TAG, "Error while finding resource.");
        return convertCAResultToSPResult(res);
    }
    else
    {
        OC_LOG(INFO, TAG, "Resource found successfully.");
    }
    return SPWaitForResponse(timeout);
}

/**
 * Function to update the operation mode. As per the spec. Operation mode in client driven
 * single service provisioning it will be updated to 0x3
 *
 * @param[in]  timeout     timeout for operation.
 * @param[in]  deviceInfo  Device Info.
 * @return  SP_SUCCESS on success
 */
static SPResult updateOwnerTransferModeToResource(unsigned short timeout,
        const SPTargetDeviceInfo_t *deviceInfo)
{
    static const char DOXM_UPDATE_OWNER_TRANSFER_MODE_QUERY[] = "/oic/sec/doxm";
    SPResult res = SP_RESULT_INTERNAL_ERROR;
    char uri[CA_MAX_URI_LENGTH] = {0};
    size_t uriLen = sizeof(uri);
    snprintf(uri, uriLen - 1, COAP_QUERY, deviceInfo->ip,
             deviceInfo->port, DOXM_UPDATE_OWNER_TRANSFER_MODE_QUERY);
    uri[uriLen - 1] = '\0';
    // TODO payload will be created at runtime in future
    char *payload = "{\"OxmSel\":\"oic.sec.doxm.jw\"}";
    int payloadLen = strlen(payload);

    CAMethod_t method = CA_POST;
    if (CA_STATUS_OK != CAGenerateToken(&gToken))
    {
        OC_LOG(ERROR, TAG, "Error while generating token");
        return SP_RESULT_INTERNAL_ERROR;
    }
    handler = &OwnerShipTransferModeHandler;
    gStateManager |= SP_UP_OWN_TR_METH_STARTED;

    CAResult_t result = sendCARequest(uri, payload, payloadLen, gToken, method,
                                      deviceInfo->connType);
    if (CA_STATUS_OK != result)
    {
        OC_LOG(ERROR, TAG, "Error while sending request.");
        CADestroyToken(gToken);
        return convertCAResultToSPResult(result);
    }
    res = SPTimeout(timeout, SP_UP_OWN_TR_METH_DONE);
    if (SP_RESULT_SUCCESS != res)
    {
        OC_LOG(ERROR, TAG, "Internal Error occured");
        CADestroyToken(gToken);
        return SP_RESULT_TIMEOUT;
    }
    CADestroyToken(gToken);
    return SP_RESULT_SUCCESS;
}

/**
 * Function to send request to resource to get its pstat resource information.
 *
 * @param[in]  timeout     timeout for operation.
 * @param[in]  deviceInfo  Device Info.
 * @return  SP_SUCCESS on success
 */
static SPResult getProvisioningStatusResource(unsigned short timeout,
        const SPTargetDeviceInfo_t *deviceInfo)
{

    const char PSTAT_STATUS_QUERY[] = "/oic/sec/pstat";
    char uri[CA_MAX_URI_LENGTH] = {0};
    size_t uriLen = sizeof(uri);
    snprintf(uri, uriLen - 1, COAP_QUERY, deviceInfo->ip,
             deviceInfo->port, PSTAT_STATUS_QUERY);
    uri[uriLen - 1] = '\0';
    CAMethod_t method = CA_GET;
    if (CA_STATUS_OK != CAGenerateToken(&gToken))
    {
        OC_LOG(ERROR, TAG, "Error while generating token");
        return SP_RESULT_INTERNAL_ERROR;
    }
    handler = &ListMethodsHandler;
    gStateManager |= SP_LIST_METHODS_STARTED;
    CAResult_t result = sendCARequest(uri, NULL, 0, gToken, method, deviceInfo->connType);
    if (CA_STATUS_OK != result)
    {
        OC_LOG(ERROR, TAG, "Failure while sending request.");
        CADestroyToken(gToken);
        return convertCAResultToSPResult(result);
    }
    SPResult res = SPTimeout(timeout, SP_LIST_METHODS_DONE);
    if (SP_RESULT_SUCCESS != res)
    {
        OC_LOG(ERROR, TAG, "Timeout while getting method list.");
        CADestroyToken(gToken);
        return SP_RESULT_TIMEOUT;
    }
    if (gStateManager && SP_LIST_METHODS_DONE)
    {
        CADestroyToken(gToken);
        return SP_RESULT_SUCCESS;
    }
    CADestroyToken(gToken);
    return SP_RESULT_INTERNAL_ERROR;
}

/**
 * Function to update the operation mode. As per the spec. Operation mode in client driven
 * single service provisioning it will be updated to 0x3
 *
 * @param[in]  timeout     timeout for operation.
 * @param[in]  deviceInfo  Device Info.
 * @return  SP_SUCCESS on success
 */
static SPResult updateOperationMode(unsigned short timeout, const SPTargetDeviceInfo_t *deviceInfo,
                                    OicSecDpom_t selectedOperationMode)
{
    const char PSTAT_UPDATE_OPERATION_QUERY[] = "/oic/sec/pstat";
    SPResult res = SP_RESULT_INTERNAL_ERROR;
    char uri[CA_MAX_URI_LENGTH] = {0};
    size_t uriLen = sizeof(uri);
    snprintf(uri, uriLen - 1, COAP_QUERY, deviceInfo->ip,
             deviceInfo->port , PSTAT_UPDATE_OPERATION_QUERY);
    uri[uriLen - 1] = '\0';

    char payloadBuffer[SP_MAX_BUF_LEN] = {0};
    size_t payLoadSize = sizeof(payloadBuffer);
    snprintf(payloadBuffer, payLoadSize - 1, "{\"om\":\"%d\"}", selectedOperationMode);

    payloadBuffer[payLoadSize - 1] = '\0';
    int payloadLen = strlen(payloadBuffer);

    CAMethod_t method = CA_PUT;
    if (CA_STATUS_OK != CAGenerateToken(&gToken))
    {
        OC_LOG(ERROR, TAG, "Error while generating token");
        return SP_RESULT_INTERNAL_ERROR;
    }
    handler = &OperationModeUpdateHandler;
    gStateManager |= SP_UPDATE_OP_MODE_STARTED;
    CAResult_t result = sendCARequest(uri, payloadBuffer, payloadLen, gToken, method,
                                      deviceInfo->connType);
    if (CA_STATUS_OK != result)
    {
        OC_LOG(ERROR, TAG, "Error while sending request.");
        CADestroyToken(gToken);
        return convertCAResultToSPResult(result);
    }
    res = SPTimeout(timeout, SP_UPDATE_OP_MODE_DONE);
    if (SP_RESULT_SUCCESS != res)
    {
        OC_LOG(ERROR, TAG, "Internal Error occured");
        CADestroyToken(gToken);
        return SP_RESULT_TIMEOUT;
    }
    CADestroyToken(gToken);

    if (gStateManager & SP_UPDATE_OP_MODE_DONE)
    {
        return SP_RESULT_SUCCESS;
    }
    return SP_RESULT_INTERNAL_ERROR;
}

/**
 * Function to initiate DTLS handshake.
 *
 * @param[in]  deviceInfo  Provisioning context
 * @return SP_SUCCESS on success
 */
static SPResult initiateDtlsHandshake(const SPTargetDeviceInfo_t *deviceInfo)
{
    CAResult_t caresult = CASelectCipherSuite(TLS_ECDH_anon_WITH_AES_128_CBC_SHA);

    if (CA_STATUS_OK != caresult)
    {
        OC_LOG(ERROR, TAG, "Unable to select cipher suite");
        return SP_RESULT_INTERNAL_ERROR;
    }
    OC_LOG(INFO, TAG, "Anonymous cipher suite selected. ");
    caresult = CAEnablesAnonEcdh(ENABLE_TLS_ECDH_anon_WITH_AES_128_CBC_SHA);
    if (CA_STATUS_OK != caresult)
    {
        OC_LOG_V(ERROR, TAG, "Unable to enable anon cipher suite");
        return SP_RESULT_INTERNAL_ERROR;
    }
    OC_LOG(INFO, TAG, "Anonymous cipher suite Enabled.");
    CAAddress_t *address = (CAAddress_t *) OCMalloc(sizeof(CAAddress_t));
    if (NULL == address)
    {
        OC_LOG_V(ERROR, TAG, "Error while memory allocation");
        return SP_RESULT_MEM_ALLOCATION_FAIL;
    }
    strncpy(address->IP.ipAddress, deviceInfo->ip, DEV_ADDR_SIZE_MAX);
    address->IP.ipAddress[DEV_ADDR_SIZE_MAX - 1] = '\0';
    address->IP.port = CA_SECURE_PORT;
    caresult = CAInitiateHandshake(address, deviceInfo->connType);
    if (CA_STATUS_OK != caresult)
    {
        OC_LOG_V(ERROR, TAG, "DTLS handshake failure.");
    }

    return SP_RESULT_SUCCESS;
}

/**
 * Function to send ownerShip info. This function would update Owned as true and
 * owner as UUID for provisioning tool
 *
 * @param[in]  timeout     timeout value for the operation.
 * @param[in]  deviceInfo  provisioning context.
 * @return  SP_SUCCESS on success
 */
static SPResult sendOwnershipInfo(unsigned short timeout,
                                  const SPTargetDeviceInfo_t *selectedDeviceInfo)
{
    const char DOXM_SEND_OWNERSHIP_QUERY[] = "/oic/sec/doxm";
    char uri[CA_MAX_URI_LENGTH] = {0};
    size_t uriLen = sizeof(uri);
    snprintf(uri, uriLen - 1, COAP_QUERY, selectedDeviceInfo->ip,
             selectedDeviceInfo->port, DOXM_SEND_OWNERSHIP_QUERY);
    uri[uriLen - 1] = '\0';


    size_t encodedUUIDSize = B64ENCODE_OUT_SAFESIZE(UUID_LENGTH);
    char encodedUUID[encodedUUIDSize];
    uint32_t outLen = 0;
    if ( B64_OK != b64Encode (gDeviceUUID.id, UUID_LENGTH, encodedUUID, encodedUUIDSize, &outLen))
    {
        OC_LOG(ERROR, TAG, "Memory while encoding");
        return SP_RESULT_INTERNAL_ERROR;
    }
    char payloadBuffer[SP_MAX_BUF_LEN] = {0};
    size_t payloadSize = sizeof(payloadBuffer);
    snprintf(payloadBuffer, payloadSize - 1, "{\"owned\":\"T\",\"owner\":\"%s\"}", encodedUUID);

    payloadBuffer[payloadSize - 1] = '\0';

    int payloadLen = strlen(payloadBuffer);

    CAMethod_t method = CA_PUT;
    if (CA_STATUS_OK != CAGenerateToken(&gToken))
    {
        OC_LOG(ERROR, TAG, "Error while generating token");
        return SP_RESULT_INTERNAL_ERROR;

    }
    handler = &OwnerShipUpdateHandler;
    gStateManager |= SP_UPDATE_OWNER_STARTED;

    CAResult_t result = sendCARequest(uri, payloadBuffer, payloadLen, gToken, method,
                                      selectedDeviceInfo->connType);
    if (CA_STATUS_OK != result)
    {
        OC_LOG(ERROR, TAG, "Error while sending request.");
        CADestroyToken(gToken);
        return convertCAResultToSPResult(result);
    }
    SPResult res = SPTimeout(timeout, SP_UPDATE_OWNER_DONE);
    if (SP_RESULT_SUCCESS != res)
    {
        OC_LOG(ERROR, TAG, "Internal Error occured");
        CADestroyToken(gToken);
        return SP_RESULT_TIMEOUT;
    }
    CADestroyToken(gToken);
    return SP_RESULT_SUCCESS;
}

/**
 * Function to save ownerPSK at provisioning tool end.
 *
 * @return  SP_SUCCESS on success
 */
static SPResult saveOwnerPSK()
{
    //SRMGenerateOwnerPSK();
    return SP_RESULT_SUCCESS;

}

/**
 * Function to select operation mode.This function will return most secure common operation mode.
 *
 * @param[out]   selectedMode   selected operation mode
 * @return  SP_SUCCESS on success
 */
static void selectOperationMode(OicSecDpom_t **selectedMode)
{
    int i = 0;
    int j = 0;
    while (i < gNumOfProvisioningMethodsPT && j < gNumberOfSupportedOM)
    {
        if (gProvisioningToolCapability[i] < gDeviceSupportedMethods[j])
        {
            i++;
        }
        else if (gDeviceSupportedMethods[j] < gProvisioningToolCapability[i])
        {
            j++;
        }
        else /* if gProvisioningToolCapability[i] == deviceSupportedMethods[j] */
        {
            *selectedMode = &(gProvisioningToolCapability[j]);
            break;
        }
    }
}

/**
 * Function to perform onwership tranfer based to ownership transfer mode.
 *
 * @param[in]  timeout            timeout in secs to perform operation. 0 timeout means
                                  function will wait forever.
 * @param[in]  selectedDeviceInfo instance of SPTargetDeviceInfo_t structure.
 * @return  SP_SUCCESS on success
 */
static SPResult doOwnerShipTransfer(unsigned short timeout,
                                    const SPTargetDeviceInfo_t *selectedDeviceInfo)
{
    OicSecDpom_t *selectedOperationMode = NULL;
    selectOperationMode(&selectedOperationMode);

    SPResult res = updateOperationMode(timeout, selectedDeviceInfo, *selectedOperationMode);
    if (SP_RESULT_SUCCESS != res)
    {
        OC_LOG(ERROR, TAG, "Error while updating operation mode.");
        return SP_RESULT_INTERNAL_ERROR;
    }
    if (*selectedOperationMode == SINGLE_SERVICE_CLIENT_DRIVEN)
    {
        res = initiateDtlsHandshake(selectedDeviceInfo);
        if (SP_RESULT_SUCCESS != res)
        {
            OC_LOG(ERROR, TAG, "Error while DTLS handshake.");
            return SP_RESULT_INTERNAL_ERROR;
        }

        res = sendOwnershipInfo(timeout, selectedDeviceInfo);
        if (SP_RESULT_SUCCESS != res)
        {
            OC_LOG(ERROR, TAG, "Error while updating ownership information.");
            return SP_RESULT_INTERNAL_ERROR;
        }

        saveOwnerPSK();
    }
    return SP_RESULT_SUCCESS;

}

/**
 * Function to provision credentials to specific device.
 *
 * @param[in] timeout     timeout in secs to perform operation. 0 timeout means function will
                          wait till success.
 * @param[in] cred        credential to be provisioned.
 * @param[in] deviceInfo  Instance of SPDevInfo_t structure. Representing a selected device for
                          provisioning.
 * @return  SP_SUCCESS on success
 */
SPResult provisionCredentials(unsigned short timeout, const OicSecCred_t *cred,
                              const SPDevInfo_t *deviceInfo)
{
    const char CRED_RES[] = "/oic/sec/cred";
    char *credJson = NULL;
    //credJson = BinToJson(cred); // TODO from SRM.

    char uri[CA_MAX_URI_LENGTH] = {0};
    size_t uriLen = sizeof(uri);
    snprintf(uri, uriLen - 1, COAP_QUERY, deviceInfo->ip,
             deviceInfo->port, CRED_RES);
    uri[uriLen - 1] = '\0';

    int payloadLen = strlen(credJson);
    CAMethod_t method = CA_POST;
    if (CA_STATUS_OK != CAGenerateToken(&gToken))
    {
        OC_LOG(ERROR, TAG, "Error while generating token");
        return SP_RESULT_INTERNAL_ERROR;
    }
    handler = &CredProvisioningHandler;
    // TODO need to change the uri to secure URI.
    CAResult_t result = sendCARequest(uri, credJson, payloadLen, gToken, method,
                                      deviceInfo->connType);
    if (CA_STATUS_OK != result)
    {
        OC_LOG(ERROR, TAG, "Internal Error while sending Credentials.");
        if (credJson)
        {
            OCFree(credJson);
        }
        CADestroyToken(gToken);
        return convertCAResultToSPResult(result);
    }

    SPResult res = SPTimeout(timeout, SP_PROV_CRED_DONE);
    if (SP_RESULT_SUCCESS != res)
    {
        OC_LOG(ERROR, TAG, "Internal Error occured");
        if (credJson)
        {
            OCFree(credJson);
        }
        CADestroyToken(gToken);
        return SP_RESULT_TIMEOUT;
    }
    if (credJson)
    {
        OCFree(credJson);
    }
    CADestroyToken(gToken);
    return res;
}

SPResult SPProvisioningDiscovery(unsigned short timeout, SPConnectivityType connType,
                                 SPTargetDeviceInfo_t **list)
{
    if ((SP_CONN_ETHERNET != connType) && (SP_CONN_WIFI != connType) && (SP_CONN_EDR != connType)
        && (SP_CONN_LE != connType))
    {
        return SP_RESULT_INVALID_PARAM;
    }
    if (NULL != *list)
    {
        OC_LOG(ERROR, TAG, "List is not null can cause memory leak");
    }

    CAConnectivityType_t networkType = getConnectivity(connType);
    CAResult_t res = SP_RESULT_SUCCESS;
    res = CAInitialize();
    if (CA_STATUS_OK != res)
    {
        OC_LOG(ERROR, TAG, "CA Initialization Failed.");
        return convertCAResultToSPResult(res);
    }
    res = CASelectNetwork(networkType);
    if (CA_STATUS_OK != res)
    {
        OC_LOG(ERROR, TAG, "CA Network Error.");
        return convertCAResultToSPResult(res);
    }
    res = CAStartDiscoveryServer();
    if (CA_STATUS_OK != res)
    {
        OC_LOG(ERROR, TAG, "Start Discovery Failed.");
        return convertCAResultToSPResult(res);
    }
    CARegisterHandler(SPRequestHandler, SPResponseHandler);
    SPResult smResponse = SP_RESULT_SUCCESS;
    smResponse = findResource(timeout);
    if (SP_RESULT_SUCCESS != smResponse)
    {
        return SP_RESULT_INTERNAL_ERROR;
    }
    if (gStateManager & SP_DISCOVERY_DONE)
    {
        if (gStateManager & SP_DISCOVERY_ERROR)
        {
            return SP_RESULT_INTERNAL_ERROR;
        }
        *list = gStartOfDiscoveredDevices;
        return SP_RESULT_SUCCESS;
    }
    return SP_RESULT_INTERNAL_ERROR;
}

SPResult SPInitProvisionContext(unsigned short timeout, OicUuid_t *deviceId,
                                const SPTargetDeviceInfo_t *selectedDeviceInfo)
{
    if (NULL == selectedDeviceInfo || NULL == deviceId)
    {
        return SP_RESULT_INVALID_PARAM;
    }
    memcpy(gDeviceUUID.id, deviceId->id, UUID_LENGTH);

    SPResult res = SP_RESULT_SUCCESS;
    uint16_t selectedMethod = 0;

    selectProvisioningMethod(selectedDeviceInfo->oxm, selectedDeviceInfo->numberofOxmMethods,
                             &selectedMethod);

    res = updateOwnerTransferModeToResource(timeout, selectedDeviceInfo);
    if (SP_RESULT_SUCCESS != res)
    {
        OC_LOG(ERROR, TAG, "Error while updating owner transfer mode.");
        return SP_RESULT_INTERNAL_ERROR;
    }

    res = getProvisioningStatusResource(timeout, selectedDeviceInfo);
    if (SP_RESULT_SUCCESS != res)
    {
        OC_LOG(ERROR, TAG, "Error while getting provisioning status.");
        return SP_RESULT_INTERNAL_ERROR;
    }
    return doOwnerShipTransfer(timeout, selectedDeviceInfo);

}

SPResult SPProvisionACL(unsigned short timeout, const SPTargetDeviceInfo_t *selectedDeviceInfo,
                        OicSecAcl_t *acl)
{
    if (NULL == selectedDeviceInfo || NULL == acl)
    {
        return SP_RESULT_INVALID_PARAM;
    }
    char *aclString = NULL;
    aclString = BinToAclJSON(acl);

    if (NULL == aclString)
    {
        OC_LOG(ERROR, TAG, "Memory allocation problem");
        return SP_RESULT_MEM_ALLOCATION_FAIL;
    }

    const char ACL_RESOURCE_PROVISIONING_QUERY[] = "/oic/sec/acl";
    char uri[CA_MAX_URI_LENGTH] = {0};
    size_t uriLen = sizeof(uri);
    snprintf(uri, uriLen - 1, COAP_QUERY, selectedDeviceInfo->ip,
             selectedDeviceInfo->port, ACL_RESOURCE_PROVISIONING_QUERY);
    uri[uriLen - 1] = '\0';

    int payloadLen = strlen(aclString);
    CAMethod_t method = CA_POST;
    if (CA_STATUS_OK != CAGenerateToken(&gToken))
    {
        OC_LOG(ERROR, TAG, "Error while generating token");
        return SP_RESULT_INTERNAL_ERROR;

    }
    handler = &ACLProvisioningHandler;
    gStateManager |= SP_PROV_ACL_STARTED;
    // TODO need to change the uri to secure URI.
    CAResult_t result = sendCARequest(uri, aclString, payloadLen, gToken, method,
                                      selectedDeviceInfo->connType);
    if (CA_STATUS_OK != result)
    {
        OC_LOG(ERROR, TAG, "Internal Error while sending ACL.");
        if (aclString)
        {
            OCFree(aclString);
        }
        CADestroyToken(gToken);
        return convertCAResultToSPResult(result);
    }

    SPResult res = SPTimeout(timeout, SP_PROV_ACL_DONE);
    if (SP_RESULT_SUCCESS != res)
    {
        OC_LOG(ERROR, TAG, "Internal Error occured");
        if (aclString)
        {
            OCFree(aclString);
        }
        CADestroyToken(gToken);
        return SP_RESULT_TIMEOUT;
    }
    if (aclString)
    {
        OCFree(aclString);
    }
    CADestroyToken(gToken);
    return res;
}

SPResult SPProvisionCredentials(unsigned short timeout, OicSecCredType_t type,
                                const SPDevInfo_t *pDevList)
{
    if (NULL == pDevList)
    {
        return SP_RESULT_INVALID_PARAM;
    }
    const SPDevInfo_t *curr = pDevList;
    while (curr)
    {
        SPDevInfo_t *next = curr->next;
        OicSecCred_t *cred = NULL;
        SPResult res = SPGenerateCredentials(type, curr, &gDeviceUUID, &cred);
        if (res != SP_RESULT_SUCCESS)
        {
            OC_LOG(ERROR, TAG, "error while generating credentials");
            return SP_RESULT_INTERNAL_ERROR;
        }
        provisionCredentials(timeout, cred, curr);
        if (SP_RESULT_SUCCESS != res)
        {
            OC_LOG_V(ERROR, TAG, "Credentials provisioning Error");
            return SP_RESULT_INTERNAL_ERROR;
        }
        gStateManager = 0;
        curr = next;
    }
    return SP_RESULT_SUCCESS;
}

SPResult SPFinalizeProvisioning(unsigned short timeout,
                                const SPTargetDeviceInfo_t *selectedDeviceInfo)
{
    // TODO
    if (NULL == selectedDeviceInfo)
    {
        OC_LOG(ERROR, TAG, "Target device Info is NULL.");
        return SP_RESULT_INVALID_PARAM;
    }
    const char PSTAT_COMMIT_HASH_QUERY[] = "/oic/sec/pstat";
    char uri[CA_MAX_URI_LENGTH] = {0};
    size_t uriLen = sizeof(uri);
    snprintf(uri, uriLen - 1, COAP_QUERY, selectedDeviceInfo->ip,
             selectedDeviceInfo->port, PSTAT_COMMIT_HASH_QUERY);
    uri[uriLen - 1] = '\0';


    char payloadBuffer[SP_MAX_BUF_LEN] = {0};
    size_t payloadSize = sizeof(payloadBuffer);

    char aclHash[] = "0"; // value for beachhead version.
    snprintf(payloadBuffer, payloadSize - 1, "{\"tm\":\"0\",\"commitHash\":\"%s\"}", aclHash);


    payloadBuffer[payloadSize - 1] = '\0';

    int payloadLen = strlen(payloadBuffer);

    CAMethod_t method = CA_PUT;
    if (CA_STATUS_OK != CAGenerateToken(&gToken))
    {
        OC_LOG(ERROR, TAG, "Error while generating token");
        return SP_RESULT_INTERNAL_ERROR;
    }
    handler = &FinalizeProvisioningHandler;
    gStateManager |= SP_UP_HASH_STARTED;
    CAResult_t result = sendCARequest(uri, payloadBuffer, payloadLen, gToken, method,
                                      selectedDeviceInfo->connType);
    if (CA_STATUS_OK != result)
    {
        OC_LOG(ERROR, TAG, "Internal Error occured");
        CADestroyToken(gToken);
        return convertCAResultToSPResult(result);
    }

    SPResult res = SPTimeout(timeout, SP_UP_HASH_DONE);
    if (SP_RESULT_SUCCESS != res)
    {
        OC_LOG(ERROR, TAG, "Internal Error occured");
        CADestroyToken(gToken);
        return SP_RESULT_TIMEOUT;
    }
    CADestroyToken(gToken);
    return res;
}

SPResult SPTerminateProvisioning()
{
    deleteList();
    return SP_RESULT_SUCCESS;;
}
