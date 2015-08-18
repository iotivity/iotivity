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
#include <stdbool.h>

#include "cJSON.h"
#include "ocpayload.h"
#include "ocpayloadcbor.h"
#include "oic_malloc.h"
#include "logger.h"
#include "cacommon.h"
#include "cainterface.h"
#include "provisioningmanager.h"
#include "credentialgenerator.h"
#include "global.h"
#include "base64.h"
#include "aclresource.h"
#include "doxmresource.h"
#include "pstatresource.h"
#include "srmresourcestrings.h"
#include "credresource.h"
#include "oic_string.h"
#include "secureresourcemanager.h"

typedef enum
{
    SP_NO_MASK                = (0       ),
    SP_DISCOVERY_STARTED      = (0x1 << 1),
    SP_DISCOVERY_ERROR        = (0x1 << 2),
    SP_DISCOVERY_DONE         = (0x1 << 3),
    SP_SEC_RES_INFO_STARTED   = (0x1 << 4),
    SP_SEC_RES_INFO_ERROR     = (0x1 << 5),
    SP_SEC_RES_INFO_DONE      = (0x1 << 6),
    SP_UP_OWN_TR_METH_STARTED = (0x1 << 7),
    SP_UP_OWN_TR_METH_ERROR   = (0x1 << 8),
    SP_UP_OWN_TR_METH_DONE    = (0x1 << 9),
    SP_LIST_METHODS_STARTED   = (0x1 << 10),
    SP_LIST_METHODS_ERROR     = (0x1 << 11),
    SP_LIST_METHODS_DONE      = (0x1 << 12),
    SP_UPDATE_OP_MODE_STARTED = (0x1 << 13),
    SP_UPDATE_OP_MODE_ERROR   = (0x1 << 14),
    SP_UPDATE_OP_MODE_DONE    = (0x1 << 15),
    SP_UPDATE_OWNER_STARTED   = (0x1 << 16),
    SP_UPDATE_OWNER_ERROR     = (0x1 << 17),
    SP_UPDATE_OWNER_DONE      = (0x1 << 18),
    SP_PROV_ACL_STARTED       = (0x1 << 19),
    SP_PROV_ACL_ERROR         = (0x1 << 20),
    SP_PROV_ACL_DONE          = (0x1 << 21),
    SP_UP_HASH_STARTED        = (0x1 << 22),
    SP_UP_HASH_ERROR          = (0x1 << 23),
    SP_UP_HASH_DONE           = (0x1 << 24),
    SP_PROV_CRED_STARTED      = (0x1 << 25),
    SP_PROV_CRED_ERROR        = (0x1 << 26),
    SP_PROV_CRED_DONE         = (0x1 << 27)
} SPProvisioningStates;

#define SP_MAX_BUF_LEN 1024
#define TAG "SPProvisionAPI"
#define COAP_QUERY "coap://%s:%d%s"
#define COAPS_QUERY "coaps://%s:%d%s"

bool (*handler)(const CAEndpoint_t *, const CAResponseInfo_t *);

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
 * Global variable to save pstat.
 */
static OicSecPstat_t *gPstat = NULL;

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
        case CA_RECEIVE_FAILED:
            {
                return SP_RESULT_CONN_RECEIVE_FAILED;
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
 * Function to delete memory allocated to linked list.
 *
 */
static void deleteList()
{
    SPTargetDeviceInfo_t *current = gStartOfDiscoveredDevices;

    while (current)
    {
        SPTargetDeviceInfo_t *next = current->next;
        DeleteDoxmBinData(current->doxm);
        DeletePstatBinData(current->pstat);
        OICFree(current);
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
 * @param[in] method          method to be used for sending rquest.
 * @param[in] endpoint        endpoint address
 * @param[in] secure          use secure connection
 * @param[in] resourceUri     resourceUri token.
 * @param[in] payload         Payload to be sent with data. NULL is case message
 *                            doesn't have payload.
 * @param[in] payloadLen      Size of data to be sent.
 * @return  CA_STATUS_OK on success, otherwise error code.
 */
static CAResult_t sendCARequest(CAMethod_t method,
                                const OCDevAddr *devAddr,
                                OCTransportFlags secure,
                                const char *resourceUri,
                                char *payload, int payloadLen)
{
    if (payload && '\0' != (*(payload + payloadLen)))
    {
        OC_LOG(ERROR, TAG, "Payload not properly terminated.");
        return CA_STATUS_INVALID_PARAM;
    }

    if (CA_STATUS_OK != CAGenerateToken(&gToken, CA_MAX_TOKEN_LEN))
    {
        OC_LOG(ERROR, TAG, "Error while generating token");
        return CA_MEMORY_ALLOC_FAILED;
    }

    CAEndpoint_t *endpoint = NULL;
    if (CA_STATUS_OK != CACreateEndpoint(devAddr->flags | (CATransportFlags_t)secure,
                                         devAddr->adapter, devAddr->addr,
                                         devAddr->port, &endpoint))
    {
        OC_LOG(ERROR, TAG, "Failed to create remote endpoint");
        CADestroyEndpoint(endpoint);
        return CA_STATUS_FAILED;
    }

    OCSecurityPayload secPayload = {};
    secPayload.securityData = payload;
    secPayload.base.type = PAYLOAD_TYPE_SECURITY;

    CARequestInfo_t requestInfo = {};
    requestInfo.method = method;
    requestInfo.isMulticast = false;
    OCConvertPayload((OCPayload*)(&secPayload), &requestInfo.info.payload,
            &requestInfo.info.payloadSize);

    requestInfo.info.type = CA_MSG_CONFIRM;
    requestInfo.info.token = gToken;
    requestInfo.info.tokenLength  = CA_MAX_TOKEN_LEN;
    requestInfo.info.resourceUri  = (CAURI_t)resourceUri;

    CAResult_t caResult = CA_STATUS_OK;
    caResult = CASendRequest(endpoint, &requestInfo);
    if (CA_STATUS_OK != caResult)
    {
        OC_LOG(ERROR, TAG, "Send Request Error !!");
    }
    CADestroyEndpoint(endpoint);
    return caResult;
}

/**
 * addDevice to list.
 *
 * @param[in] endpoint   Endpoint information
 * @param[in] doxm   pointer to doxm instance.
 * @return SP_RESULT_SUCCESS for success and errorcode otherwise.
 */
static SPResult addDevice(const CAEndpoint_t *endpoint, OicSecDoxm_t* doxm)
{
    if (NULL == endpoint)
    {
        return SP_RESULT_INVALID_PARAM;
    }
    SPTargetDeviceInfo_t *ptr = (SPTargetDeviceInfo_t *)OICCalloc(1, sizeof (SPTargetDeviceInfo_t));
    if (NULL == ptr)
    {
        OC_LOG(ERROR, TAG, "Error while allocating memory for linkedlist node !!");
        return SP_RESULT_MEM_ALLOCATION_FAIL;
    }

    memcpy(&(ptr->endpoint), endpoint, sizeof(CAEndpoint_t));
    ptr->doxm = doxm;

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
 * updateDevice to update resource info for the endpoint.
 *
 * @param[in] endpoint   Endpoint information
 * @param[in] port   secure port.
 * @return SP_RESULT_SUCCESS for success and errorcode otherwise.
 */

static SPResult updateDevice(const CAEndpoint_t *endpoint, uint16_t port)
{
    if (NULL == endpoint)
    {
        return SP_RESULT_INVALID_PARAM;
    }
    SPTargetDeviceInfo_t *ptr = gStartOfDiscoveredDevices;
    while(ptr)
    {
        if(0 == strcmp(ptr->endpoint.addr, endpoint->addr) &&
                ptr->endpoint.port == endpoint->port)
        {
            ptr->securePort = port;
            return SP_RESULT_SUCCESS;
        }
        ptr = ptr->next;
    }
    return SP_RESULT_INTERNAL_ERROR;
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
static SPResult selectProvisioningMethod(OicSecOxm_t *supportedMethods, size_t numberOfMethods,
        OicSecOxm_t *selectedMethod)
{
    /*
     TODO Logic to find appropiate method and assign it to out param
     for beachhead release method at index 0 will be returned.
     */
    *selectedMethod = supportedMethods[0];
    return SP_RESULT_SUCCESS;
}

/**
 * Response handler for discovery.
 *
 * @param[in] object       Remote endpoint object
 * @param[in] requestInfo  Datastructure containing request information.
 * @return true is CA token matches request token, false otherwise.
 */
static bool ProvisionDiscoveryHandler(const CAEndpoint_t *object,
                                      const CAResponseInfo_t *responseInfo)
{
    if ((gStateManager & SP_DISCOVERY_STARTED) && gToken)
    {
        // Response handler for discovery.
        if (0 == memcmp(gToken, responseInfo->info.token, responseInfo->info.tokenLength))
        {
            OC_LOG(INFO, TAG, "Inside ProvisionDiscoveryHandler.");
            if (NULL == responseInfo->info.payload)
            {
                OC_LOG(INFO, TAG, "Skiping Null payload");
            }
            else
            {
                OCPayload* payload = NULL;
                OCStackResult result = OCParsePayload(&payload, responseInfo->info.payload,
                        responseInfo->info.payloadSize);

                OicSecDoxm_t *ptrDoxm = NULL;

                if(result == OC_STACK_OK && payload->type == PAYLOAD_TYPE_SECURITY)
                {
                    ptrDoxm = JSONToDoxmBin(((OCSecurityPayload*)payload)->securityData);
                }

                OCPayloadDestroy(payload);

                if (NULL == ptrDoxm)
                {
                    OC_LOG(INFO, TAG, "Ignoring malformed JSON");
                }
                else
                {
                    OC_LOG(DEBUG, TAG, "Successfully converted doxm json to bin.");

                    SPResult res = addDevice(object, ptrDoxm);
                    if (SP_RESULT_SUCCESS != res)
                    {
                        OC_LOG(ERROR, TAG, "Error while adding data to linkedlist.");
                        gStateManager = gStateManager | SP_DISCOVERY_ERROR;
                        DeleteDoxmBinData(ptrDoxm);
                        return true;
                    }
                    OC_LOG(INFO, TAG, "Exiting ProvisionDiscoveryHandler.");
                    gStateManager |= SP_DISCOVERY_DONE;
                }
            }
            return true;
        }
    }
    return false;
}

/**
 * Response handler for discovery.
 *
 * @param[in] object       Remote endpoint object
 * @param[in] requestInfo  Datastructure containing request information.
 * @return true is CA token matches request token, false otherwise.
 */

static bool ProvisionSecureResourceInfoHandler(const CAEndpoint_t *object,
                                      const CAResponseInfo_t *responseInfo)
{
    if (!object || !responseInfo)
    {
        return false;
    }

    if ((gStateManager & SP_SEC_RES_INFO_STARTED) && gToken)
    {
        // Response handler for discovery.
        if (0 == memcmp(gToken, responseInfo->info.token, CA_MAX_TOKEN_LEN))
        {
            OC_LOG(INFO, TAG, "Inside ProvisionSecureResourceInfoHandler.");
            if (NULL == responseInfo->info.payload)
            {
                OC_LOG(ERROR, TAG, "Exiting ProvisionSecureResourceInfoHandler.");
                gStateManager |= SP_SEC_RES_INFO_ERROR;
            }
            else
            {
                OCPayload* payload = NULL;
                OCStackResult result = OCParsePayload(&payload, responseInfo->info.payload,
                        responseInfo->info.payloadSize);

                OCDiscoveryPayload* discover = (OCDiscoveryPayload*) payload;
                // Discovered secure resource payload contains secure port; update the device
                // with the secure port using endpoint.
                if (result == OC_STACK_OK && discover)
                {
                    if (updateDevice(object, discover->resources->port) == SP_RESULT_SUCCESS)
                    {
                        gStateManager |= SP_SEC_RES_INFO_DONE;
                    }
                    else
                    {
                        gStateManager |= SP_SEC_RES_INFO_ERROR;
                    }
                    OC_LOG(INFO, TAG, "Exiting ProvisionSecureResourceInfoHandler.");
                }

                OCPayloadDestroy(payload);
            }
            return true;
        }
        else
        {
            OC_LOG(ERROR, TAG, "Error in ProvisionSecureResourceInfoHandler.");
            gStateManager |= SP_SEC_RES_INFO_ERROR;
            return false;
        }
    }
    return false;
}

/**
 * Response handler ownership transfer.
 *
 * @param[in] object       Remote endpoint object
 * @param[in] requestInfo  Datastructure containing request information.
 * @return true is CA token matches request token, false otherwise.
 */
static bool OwnerShipTransferModeHandler(const CAEndpoint_t *object,
        const CAResponseInfo_t *responseInfo)
{
    if ((gStateManager & SP_UP_OWN_TR_METH_STARTED) && gToken)
    {
        // response handler for ownership tranfer
        OC_LOG(INFO, TAG, "Inside OwnerShipTransferModeHandler.");
        if (memcmp(gToken, responseInfo->info.token, responseInfo->info.tokenLength) == 0)
        {
            OC_LOG_V(DEBUG, TAG, "Response result for OwnerShipTransferMode: %d", responseInfo->result);
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
            return true;
        }
    }
    return false;
}

/**
 * Response handler list methods.
 *
 * @param[in] object       Remote endpoint object
 * @param[in] requestInfo  Datastructure containing request information.
 * @return true is CA token matches request token, false otherwise.
 */
static bool ListMethodsHandler(const CAEndpoint_t *object,
                               const CAResponseInfo_t *responseInfo)
{
    if ((gStateManager & SP_LIST_METHODS_STARTED) && gToken)
    {
        OC_LOG(INFO, TAG, "Inside ListMethodsHandler.");
        if (memcmp(gToken, responseInfo->info.token, responseInfo->info.tokenLength) == 0)
        {
            OC_LOG_V(DEBUG, TAG, "Response result for ListMethodsHandler: %d", responseInfo->result);
            if (CA_SUCCESS == responseInfo->result)
            {
                OC_LOG_V (DEBUG, TAG, "Response Payload: %s", responseInfo->info.payload);
                // Temp logic to trim oc attribute from json
                // JSONToPstatBin should handle OC in JSON.
                if (NULL == responseInfo->info.payload)
                {
                    OC_LOG(ERROR, TAG, "response payload is null.");
                    gStateManager |= SP_LIST_METHODS_ERROR;
                    return true;
                }

                OCPayload* payload = NULL;
                OCStackResult result = OCParsePayload(&payload, responseInfo->info.payload,
                        responseInfo->info.payloadSize);

                OicSecPstat_t *pstat = NULL;

                if(result == OC_STACK_OK && payload->type == PAYLOAD_TYPE_SECURITY)
                {
                    pstat =  JSONToPstatBin(((OCSecurityPayload*)payload)->securityData);
                }

                OCPayloadDestroy(payload);

                if (NULL == pstat)
                {
                    OC_LOG(ERROR, TAG, "Error while converting json to pstat bin");
                    gStateManager |= SP_LIST_METHODS_ERROR;
                    return true;
                }
                DeletePstatBinData(gPstat);

                gPstat = pstat;
                gStateManager |= SP_LIST_METHODS_DONE;

                OC_LOG(INFO, TAG, "Exiting ListMethodsHandler.");
            }
            return true;
        }
    }
    return false;
}

/**
 * Response handler for update operation mode.
 *
 * @param[in] object       Remote endpoint object
 * @param[in] requestInfo  Datastructure containing request information.
 * @return true is CA token matches request token, false otherwise.
 */
static bool OperationModeUpdateHandler(const CAEndpoint_t *object,
                                       const CAResponseInfo_t *responseInfo)
{
    if ((gStateManager & SP_UPDATE_OP_MODE_STARTED) && gToken)
    {
        if (0 == memcmp(gToken, responseInfo->info.token, responseInfo->info.tokenLength))
        {
            OC_LOG(INFO, TAG, "Inside OperationModeUpdateHandler.");
            OC_LOG_V(DEBUG, TAG, "Response result for OperationModeUpdateHandler: %d", responseInfo->result);
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
            return true;
        }
    }
    return false;
}

/**
 * Response handler for ownership transfer.
 *
 * @param[in] object       Remote endpoint object
 * @param[in] requestInfo  Datastructure containing request information.
 * @return true is CA token matches request token, false otherwise.
 */
static bool OwnerShipUpdateHandler(const CAEndpoint_t *object,
                                   const CAResponseInfo_t *responseInfo)
{
    if ((gStateManager & SP_UPDATE_OWNER_STARTED) && gToken)
    {
        // response handler for ownership tranfer
        if (0 == memcmp(gToken, responseInfo->info.token, responseInfo->info.tokenLength))
        {
            OC_LOG(INFO, TAG, "Inside OwnerShipUpdateHandler.");
            OC_LOG_V(DEBUG, TAG, "Response result for OwnerShipUpdateHandler: %d", responseInfo->result);
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
            return true;
        }
    }
    return false;
}

/**
 * Response handler for ACL provisioning.
 *
 * @param[in] object       Remote endpoint object
 * @param[in] requestInfo  Datastructure containing request information.
 * @return true is CA token matches request token, false otherwise.
 */
static bool ACLProvisioningHandler(const CAEndpoint_t *object,
                                   const CAResponseInfo_t *responseInfo)
{
    if ((gStateManager & SP_PROV_ACL_STARTED) && gToken)
    {

        // response handler for ACL provisioning.
        if (0 == memcmp(gToken, responseInfo->info.token, responseInfo->info.tokenLength))
        {
            OC_LOG(INFO, TAG, "Inside ACLProvisioningHandler.");
            OC_LOG_V(DEBUG, TAG, "Response result for ACLProvisioningHandler: %d", responseInfo->result);
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
            return true;
        }
    }
    return false;
}

/**
 * Response handler for provisioning finalization.
 *
 * @param[in] object       Remote endpoint object
 * @param[in] requestInfo  Datastructure containing request information.
 * @return true is CA token matches request token, false otherwise.
 */
static bool FinalizeProvisioningHandler(const CAEndpoint_t *object,
                                        const CAResponseInfo_t *responseInfo)
{
    if ((gStateManager & SP_UP_HASH_STARTED) && gToken)
    {
        // response handler for finalize provisioning.
        if (0 == memcmp(gToken, responseInfo->info.token, responseInfo->info.tokenLength))
        {
            OC_LOG(INFO, TAG, "Inside FinalizeProvisioningHandler.");
            OC_LOG_V(DEBUG, TAG, "Response result for FinalizeProvisioningHandler: %d", responseInfo->result);
            if (CA_SUCCESS == responseInfo->result)
            {
                gStateManager |= SP_UP_HASH_DONE;
                OC_LOG(INFO, TAG, "Exiting FinalizeProvisioningHandler.");
            }
            else
            {
                gStateManager |= SP_UP_HASH_ERROR;
                OC_LOG(ERROR, TAG, "Error in FinalizeProvisioningHandler.");
            }
            return true;
        }
    }
    return false;
}

/**
 * Response handler for Credential provisioning.
 *
 * @param[in] object        Remote endpoint object
 * @param[in] requestInfo   Datastructure containing request information.
 * @return true is CA token matches request token, false otherwise.
 */
static bool CredProvisioningHandler(const CAEndpoint_t *object,
                                    const CAResponseInfo_t *responseInfo)
{
    if ((gStateManager & SP_PROV_CRED_STARTED) && gToken)
    {
        // response handler for CRED provisioning.
        OC_LOG(INFO, TAG, "Inside CredProvisioningHandler.");
        OC_LOG_V(DEBUG, TAG, "Response result for CredProvisioningHandler: %d", responseInfo->result);
        if (0 == memcmp(gToken, responseInfo->info.token, responseInfo->info.tokenLength))
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
            return true;
        }
    }
    return false;
}

/**
 * Response Handler
 *
 * @param[in] object        Remote endpoint object
 * @param[in] responseInfo  Datastructure containing response information.
 * @return true if received response is for provisioning API false otherwise.
 */
static bool SPResponseHandler(const CAEndpoint_t *object,
                              const CAResponseInfo_t *responseInfo)
{
    bool isProvResponse = false;
    if ((NULL != responseInfo) && (NULL != responseInfo->info.token))
    {
        isProvResponse = handler(object, responseInfo);
    }
    return isProvResponse;
}

/**
 * Function to find the resources using multicast discovery.
 *
 * @param[in]   timeout     timeout in secs
 * @return  SP_RESULT_SUCCESS normally otherwise error code.
 */
static SPResult findResource(unsigned short timeout)
{
    static char DOXM_OWNED_FALSE_MULTICAST_QUERY[] = "/oic/sec/doxm?Owned=FALSE";
    CAResult_t res = CAGenerateToken(&gToken, CA_MAX_TOKEN_LEN);
    if (CA_STATUS_OK != res)
    {
        OC_LOG(ERROR, TAG, "Error while generating token.");
        return SP_RESULT_INTERNAL_ERROR;
    }

    CAEndpoint_t endpoint = {};

    // Only IP is supported currently for provisioning and ownership transfer
    endpoint.adapter = CA_ADAPTER_IP;
    endpoint.flags   = CA_IPV4 | CA_IPV6 | CA_SCOPE_LINK;

    CAMessageType_t msgType = CA_MSG_NONCONFIRM;
    CAInfo_t requestData = { 0 };
    requestData.token = gToken;
    requestData.tokenLength  = CA_MAX_TOKEN_LEN;
    requestData.payload = NULL;
    requestData.payloadSize = 0;
    requestData.type = msgType;
    requestData.resourceUri = DOXM_OWNED_FALSE_MULTICAST_QUERY;
    CARequestInfo_t requestInfo = { 0 };
    requestInfo.method = CA_GET;
    requestInfo.info = requestData;
    requestInfo.isMulticast = true;
    res = CASendRequest(&endpoint, &requestInfo);

    handler = &ProvisionDiscoveryHandler;
    gStateManager |= SP_DISCOVERY_STARTED;
    if (CA_STATUS_OK != res)
    {
        OC_LOG(ERROR, TAG, "Error while finding resource.");
        return convertCAResultToSPResult(res);
    }
    else
    {
        OC_LOG(INFO, TAG, "Discovery Request sent successfully");
    }
    return SPWaitForResponse(timeout);
}

/**
 * Function to get the secure resource info.
 *
 * @param[in]   devAddr     Device address for the destination
 * @param[in]   timeout     timeout in secs
 * @return  SP_RESULT_SUCCESS normally otherwise error code.
 */
static SPResult getSecureResourceInfo(OCDevAddr *devAddr, unsigned short timeout)
{
    char OIC_UNICAST_SEC_QUERY[] = "/oic/res?rt=oic.sec.doxm";
    CAResult_t res = CAGenerateToken(&gToken, CA_MAX_TOKEN_LEN);
    if (CA_STATUS_OK != res)
    {
        OC_LOG(ERROR, TAG, "Error while generating token.");
        return SP_RESULT_INTERNAL_ERROR;
    }

    CAInfo_t requestData = {};
    requestData.token = gToken;
    requestData.tokenLength  = CA_MAX_TOKEN_LEN;
    requestData.payload = NULL;
    requestData.payloadSize = 0;
    requestData.type = CA_MSG_NONCONFIRM;
    requestData.resourceUri = OIC_UNICAST_SEC_QUERY;
    CARequestInfo_t requestInfo = { 0 };
    requestInfo.method = CA_GET;
    requestInfo.info = requestData;
    requestInfo.isMulticast = false;
    handler = &ProvisionSecureResourceInfoHandler;
    res = CASendRequest((CAEndpoint_t*)devAddr, &requestInfo);

    gStateManager |= SP_SEC_RES_INFO_STARTED;
    if (CA_STATUS_OK != res)
    {
        OC_LOG(ERROR, TAG, "Error while finding secure resource.");
        return convertCAResultToSPResult(res);
    }
    else
    {
        OC_LOG(INFO, TAG, "Secure resource info request sent successfully");
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
        SPTargetDeviceInfo_t *deviceInfo, OicSecOxm_t selectedMethod)
{
    SPResult res = SP_RESULT_INTERNAL_ERROR;

    deviceInfo->doxm->oxmSel = selectedMethod;
    char *payload = BinToDoxmJSON(deviceInfo->doxm);
    if (NULL == payload)
    {
        OC_LOG(ERROR, TAG, "Error while converting bin to json");
        return SP_RESULT_INTERNAL_ERROR;
    }
    OC_LOG_V(DEBUG, TAG, "Payload: %s", payload);
    int payloadLen = strlen(payload);

    handler = &OwnerShipTransferModeHandler;
    gStateManager |= SP_UP_OWN_TR_METH_STARTED;

    CAResult_t result = sendCARequest(CA_PUT,
                                      &deviceInfo->endpoint,
                                      OC_DEFAULT_FLAGS,
                                      OIC_RSRC_DOXM_URI,
                                      payload, payloadLen);
    OICFree(payload);
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
                                              SPTargetDeviceInfo_t *deviceInfo)
{
    handler = &ListMethodsHandler;
    gStateManager |= SP_LIST_METHODS_STARTED;

    CAResult_t result = sendCARequest(CA_GET,
                                      &deviceInfo->endpoint,
                                      OC_DEFAULT_FLAGS,
                                      OIC_RSRC_PSTAT_URI,
                                      NULL, 0);
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
        deviceInfo->pstat = gPstat;
        CADestroyToken(gToken);
        OC_LOG(DEBUG, TAG, "getProvisioningStatusResource completed.");
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
static SPResult updateOperationMode(unsigned short timeout,
                                    SPTargetDeviceInfo_t *deviceInfo,
                                    OicSecDpom_t selectedOperationMode)
{

    SPResult res = SP_RESULT_INTERNAL_ERROR;

    deviceInfo->pstat->om = selectedOperationMode;

    char *payloadBuffer = BinToPstatJSON(deviceInfo->pstat);
    if (NULL == payloadBuffer)
    {
        OC_LOG(ERROR, TAG, "Error while converting pstat bin to json");
        return SP_RESULT_INTERNAL_ERROR;
    }

    size_t payloadLen = strlen(payloadBuffer);
    handler = &OperationModeUpdateHandler;
    gStateManager |= SP_UPDATE_OP_MODE_STARTED;

    CAResult_t result = sendCARequest(CA_PUT,
                                      &deviceInfo->endpoint,
                                      OC_DEFAULT_FLAGS,
                                      OIC_RSRC_PSTAT_URI,
                                      payloadBuffer, payloadLen);
    if (CA_STATUS_OK != result)
    {
        OC_LOG(ERROR, TAG, "Error while sending request.");
        CADestroyToken(gToken);
        OICFree(payloadBuffer);
        return convertCAResultToSPResult(result);
    }
    res = SPTimeout(timeout, SP_UPDATE_OP_MODE_DONE);
    if (SP_RESULT_SUCCESS != res)
    {
        OC_LOG(ERROR, TAG, "Internal Error occured");
        CADestroyToken(gToken);
        OICFree(payloadBuffer);
        return SP_RESULT_TIMEOUT;
    }
    CADestroyToken(gToken);
    OICFree(payloadBuffer);

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
static SPResult initiateDtlsHandshake(const CAEndpoint_t *endpoint)
{
    CAResult_t caresult = CAEnableAnonECDHCipherSuite(true);
    if (CA_STATUS_OK != caresult)
    {
        OC_LOG_V(ERROR, TAG, "Unable to enable anon cipher suite");
        return SP_RESULT_INTERNAL_ERROR;
    }
    OC_LOG(INFO, TAG, "Anonymous cipher suite Enabled.");

    caresult = CAInitiateHandshake(endpoint);
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
                                  SPTargetDeviceInfo_t *selectedDeviceInfo)
{
    OicUuid_t provTooldeviceID = {};

    if (OC_STACK_OK != GetDoxmDeviceID(&provTooldeviceID))
    {
        OC_LOG(ERROR, TAG, "Error while retrieving provisioning tool's device ID");
        return SP_RESULT_INTERNAL_ERROR;
    }
    memcpy(selectedDeviceInfo->doxm->owner.id, provTooldeviceID.id , UUID_LENGTH);

    selectedDeviceInfo->doxm->owned = true;

    char *payloadBuffer =  BinToDoxmJSON(selectedDeviceInfo->doxm);
    if (NULL == payloadBuffer)
    {
        OC_LOG(ERROR, TAG, "Error while converting doxm bin to json");
        return SP_RESULT_INTERNAL_ERROR;
    }
    int payloadLen = strlen(payloadBuffer);

    handler = &OwnerShipUpdateHandler;
    gStateManager |= SP_UPDATE_OWNER_STARTED;

    CAResult_t result = sendCARequest(CA_PUT,
                                      &selectedDeviceInfo->endpoint,
                                      OC_FLAG_SECURE,
                                      OIC_RSRC_DOXM_URI,
                                      payloadBuffer, payloadLen);
    if (CA_STATUS_OK != result)
    {
        OC_LOG(ERROR, TAG, "Error while sending request.");
        CADestroyToken(gToken);
        OICFree(payloadBuffer);
        return convertCAResultToSPResult(result);
    }
    SPResult res = SPTimeout(timeout, SP_UPDATE_OWNER_DONE);
    if (SP_RESULT_SUCCESS != res)
    {
        OC_LOG(ERROR, TAG, "Internal Error occured");
        CADestroyToken(gToken);
        OICFree(payloadBuffer);
        return SP_RESULT_TIMEOUT;
    }
    CADestroyToken(gToken);
    OICFree(payloadBuffer);
    return SP_RESULT_SUCCESS;
}

/**
 * Function to save ownerPSK at provisioning tool end.
 *
 * @return  SP_SUCCESS on success
 */
static SPResult saveOwnerPSK(SPTargetDeviceInfo_t *selectedDeviceInfo)
{
    SPResult result = SP_RESULT_INTERNAL_ERROR;

    CAEndpoint_t endpoint = {};
    OICStrcpy(endpoint.addr, MAX_ADDR_STR_SIZE_CA, selectedDeviceInfo->endpoint.addr);
    endpoint.port = selectedDeviceInfo->securePort;

    OicUuid_t provTooldeviceID = {};
    if (OC_STACK_OK != GetDoxmDeviceID(&provTooldeviceID))
    {
        OC_LOG(ERROR, TAG, "Error while retrieving provisioning tool's device ID");
        return result;
    }

    uint8_t ownerPSK[OWNER_PSK_LENGTH_128] = {};

    //Generating OwnerPSK
    CAResult_t pskRet = CAGenerateOwnerPSK(&endpoint,
            (uint8_t *)OXM_JUST_WORKS, strlen(OXM_JUST_WORKS), provTooldeviceID.id,
            sizeof(provTooldeviceID.id), selectedDeviceInfo->doxm->deviceID.id,
            sizeof(selectedDeviceInfo->doxm->deviceID.id), ownerPSK,
            OWNER_PSK_LENGTH_128);

    if (CA_STATUS_OK == pskRet)
    {
        OC_LOG(INFO, TAG,"ownerPSK dump:\n");
        OC_LOG_BUFFER(INFO, TAG,ownerPSK, OWNER_PSK_LENGTH_128);
        //Generating new credential for provisioning tool
        size_t ownLen = 1;
        uint32_t outLen = 0;

        char base64Buff[B64ENCODE_OUT_SAFESIZE(sizeof(ownerPSK)) + 1] = {};
        B64Result b64Ret = b64Encode(ownerPSK, sizeof(ownerPSK), base64Buff, sizeof(base64Buff),
                &outLen);
        if (B64_OK == b64Ret)
        {
            OicSecCred_t *cred = GenerateCredential(&selectedDeviceInfo->doxm->deviceID,
                    SYMMETRIC_PAIR_WISE_KEY, NULL,
                    base64Buff, ownLen, &provTooldeviceID);
            if (cred)
            {
                //Update the SVR database.
                if (OC_STACK_OK == AddCredential(cred))
                {
                    result = SP_RESULT_SUCCESS;
                }
                else
                {
                    OC_LOG(ERROR, TAG, "AddCredential failed");
                }
            }
            else
            {
                OC_LOG(ERROR, TAG, "GenerateCredential failed");
            }
        }
        else
        {
            OC_LOG(ERROR, TAG, "b64Encode failed");
        }
    }
    else
    {
        OC_LOG(ERROR, TAG, "CAGenerateOwnerPSK failed");
    }
    return result;
}

/**
 * Function to select operation mode.This function will return most secure common operation mode.
 *
 * @param[out]   selectedMode   selected operation mode
 * @return  SP_SUCCESS on success
 */
static void selectOperationMode(const SPTargetDeviceInfo_t *selectedDeviceInfo,
                                OicSecDpom_t **selectedMode)
{
    int i = 0;
    int j = 0;
    while (i < gNumOfProvisioningMethodsPT && j < selectedDeviceInfo->pstat->smLen)
    {
        if (gProvisioningToolCapability[i] < selectedDeviceInfo->pstat->sm[j])
        {
            i++;
        }
        else if (selectedDeviceInfo->pstat->sm[j] < gProvisioningToolCapability[i])
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
                                    SPTargetDeviceInfo_t *selectedDeviceInfo)
{
    OicSecDpom_t *selectedOperationMode = NULL;
    selectOperationMode(selectedDeviceInfo, &selectedOperationMode);

    SPResult res = updateOperationMode(timeout, selectedDeviceInfo, *selectedOperationMode);
    if (SP_RESULT_SUCCESS != res)
    {
        OC_LOG(ERROR, TAG, "Error while updating operation mode.");
        return SP_RESULT_INTERNAL_ERROR;
    }
    if (*selectedOperationMode == SINGLE_SERVICE_CLIENT_DRIVEN)
    {
        CAEndpoint_t endpoint = {0};
        OICStrcpy(endpoint.addr, MAX_ADDR_STR_SIZE_CA, selectedDeviceInfo->endpoint.addr);
        endpoint.port = selectedDeviceInfo->securePort;

        res = initiateDtlsHandshake(&endpoint);
        if (SP_RESULT_SUCCESS == res)
        {
            selectedDeviceInfo->endpoint.port = selectedDeviceInfo->securePort;
            res = sendOwnershipInfo(timeout, selectedDeviceInfo);
            if (SP_RESULT_SUCCESS != res)
            {
                OC_LOG(ERROR, TAG, "Error while updating ownership information.");
            }
            res = saveOwnerPSK(selectedDeviceInfo);

            //Close temporal DTLS session
            if(CA_STATUS_OK != CACloseDtlsSession(&endpoint))
            {
                OC_LOG(WARNING, TAG, "doOwnerShipTransfer() : failed to close the dtls session");
            }
        }
        else
        {
            OC_LOG(ERROR, TAG, "Error during initiating DTLS handshake.");
        }

        //Disable Anonymous ECDH cipher suite before leaving this method
        if(CA_STATUS_OK != CAEnableAnonECDHCipherSuite(false))
        {
            OC_LOG(WARNING, TAG, "doOwnerShipTransfer() : failed to disable Anon ECDH cipher suite");
        }
    }
    return (res != SP_RESULT_SUCCESS) ? SP_RESULT_INTERNAL_ERROR : SP_RESULT_SUCCESS;

}
/**
 * The function is responsible for discovering secure resources(such as, /oic/sec/doxm etc) with
 * OC_EXPLICIT_DISCOVERABLE on a OIC device which needs to be provisioned.
 *
 * @param[in] timeout Timeout in seconds, value till which function will listen to responses from
 *                    client before returning the list of devices.
 * @param[in] selectedDeviceInfo Device information.
 * @return SP_SUCCESS in case of success and other value otherwise.
 */
static SPResult discoverSecureResource(unsigned short timeout,
        SPTargetDeviceInfo_t *selectedDeviceInfo)
{
    if (NULL == selectedDeviceInfo)
    {
        OC_LOG(ERROR, TAG, "List is not null can cause memory leak");
        return SP_RESULT_INVALID_PARAM;
    }
    SPResult smResponse = SP_RESULT_SUCCESS;
    smResponse = getSecureResourceInfo(&selectedDeviceInfo->endpoint, timeout);
    if (SP_RESULT_SUCCESS != smResponse)
    {
        return SP_RESULT_INTERNAL_ERROR;
    }
    if (gStateManager & SP_SEC_RES_INFO_DONE)
    {
        if (gStateManager & SP_SEC_RES_INFO_ERROR)
        {
            return SP_RESULT_INTERNAL_ERROR;
        }
        return SP_RESULT_SUCCESS;
    }
    return SP_RESULT_INTERNAL_ERROR;
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
    char *credJson = NULL;
    credJson = BinToCredJSON(cred);
    if (NULL == credJson)
    {
        OC_LOG(ERROR, TAG, "Memory allocation problem");
        return SP_RESULT_MEM_ALLOCATION_FAIL;
    }

    int payloadLen = strlen(credJson);
    handler = &CredProvisioningHandler;
    gStateManager |= SP_PROV_CRED_STARTED;

    CAResult_t result = sendCARequest(CA_POST,
                                      &deviceInfo->endpoint,
                                      OC_FLAG_SECURE,
                                      OIC_RSRC_CRED_URI,
                                      credJson, payloadLen);
    OICFree(credJson);
    if (CA_STATUS_OK != result)
    {
        OC_LOG(ERROR, TAG, "Internal Error while sending Credentials.");
        CADestroyToken(gToken);
        return convertCAResultToSPResult(result);
    }

    SPResult res = SPTimeout(timeout, SP_PROV_CRED_DONE);
    if (SP_RESULT_SUCCESS != res)
    {
        OC_LOG(ERROR, TAG, "Internal Error occured");
        CADestroyToken(gToken);
        return SP_RESULT_TIMEOUT;
    }
    CADestroyToken(gToken);
    gStateManager = 0;
    return res;
}

SPResult SPProvisioningDiscovery(unsigned short timeout,
                                 SPTargetDeviceInfo_t **list)
{
    if (NULL != *list)
    {
        OC_LOG(ERROR, TAG, "List is not null can cause memory leak");
        return SP_RESULT_INVALID_PARAM;
    }
    SRMRegisterProvisioningResponseHandler(SPResponseHandler);
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

SPResult SPInitProvisionContext(unsigned short timeout,
                                SPTargetDeviceInfo_t *selectedDeviceInfo)
{
    if (NULL == selectedDeviceInfo )
    {
        return SP_RESULT_INVALID_PARAM;
    }
    SPResult res = SP_RESULT_SUCCESS;

    //Discover secure resource and update the device info.
    res = discoverSecureResource(timeout, selectedDeviceInfo);
    if (SP_RESULT_SUCCESS != res)
    {
        OC_LOG(ERROR, TAG, "Error in discoverSecureResource");
        return SP_RESULT_INTERNAL_ERROR;
    }

    OicSecOxm_t selectedMethod = OIC_JUST_WORKS;

    selectProvisioningMethod(selectedDeviceInfo->doxm->oxm, selectedDeviceInfo->doxm->oxmLen,
                             &selectedMethod);
    OC_LOG_V(DEBUG, TAG, "Selected method %d:", selectedMethod);
    res = updateOwnerTransferModeToResource(timeout, selectedDeviceInfo, selectedMethod);

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
    OC_LOG(INFO, TAG, "Starting ownership transfer");
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

    int payloadLen = strlen(aclString);
    handler = &ACLProvisioningHandler;
    gStateManager |= SP_PROV_ACL_STARTED;

    CAResult_t result = sendCARequest(CA_POST,
                                      &selectedDeviceInfo->endpoint,
                                      OC_FLAG_SECURE,
                                      OIC_RSRC_ACL_URI,
                                      aclString, payloadLen);
    OICFree(aclString);
    if (CA_STATUS_OK != result)
    {
        OC_LOG(ERROR, TAG, "Internal Error while sending ACL.");
        CADestroyToken(gToken);
        return convertCAResultToSPResult(result);
    }

    SPResult res = SPTimeout(timeout, SP_PROV_ACL_DONE);
    if (SP_RESULT_SUCCESS != res)
    {
        OC_LOG(ERROR, TAG, "Internal Error occured");
        CADestroyToken(gToken);
        return SP_RESULT_TIMEOUT;
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
    OicUuid_t provTooldeviceID = {};
    if (OC_STACK_OK != GetDoxmDeviceID(&provTooldeviceID))
    {
        OC_LOG(ERROR, TAG, "Error while retrieving provisioning tool's device ID");
        return SP_RESULT_INTERNAL_ERROR;
    }
    //TODO Need to support other key types in future.
    switch (type)
    {
        case SYMMETRIC_PAIR_WISE_KEY:
            {
                if (NULL == curr->next)
                {
                    return SP_RESULT_INVALID_PARAM;
                }
                // Devices if present after second node will not be considered.
                // in scenario-2. 2 devices are provisioned with credentials.
                const SPDevInfo_t *firstDevice = curr;
                const SPDevInfo_t *secondDevice = curr->next;

                OicSecCred_t *firstCred = NULL;
                OicSecCred_t *secondCred = NULL;

                SPResult res = SPGeneratePairWiseCredentials(type, &provTooldeviceID,
                               &firstDevice->deviceId, &secondDevice->deviceId,
                               &firstCred, &secondCred);
                if (res != SP_RESULT_SUCCESS)
                {
                    OC_LOG(ERROR, TAG, "error while generating credentials");
                    return SP_RESULT_INTERNAL_ERROR;
                }
                res = provisionCredentials(timeout, firstCred, firstDevice);
                if (SP_RESULT_SUCCESS != res)
                {
                    OC_LOG_V(ERROR, TAG, "Credentials provisioning Error");
                    DeleteCredList(firstCred);
                    DeleteCredList(secondCred);
                    return SP_RESULT_INTERNAL_ERROR;
                }
                res = provisionCredentials(timeout, secondCred, secondDevice);
                if (SP_RESULT_SUCCESS != res)
                {
                    OC_LOG_V(ERROR, TAG, "Credentials provisioning Error");
                    DeleteCredList(firstCred);
                    DeleteCredList(secondCred);
                    return SP_RESULT_INTERNAL_ERROR;
                }
                DeleteCredList(firstCred);
                DeleteCredList(secondCred);
                return SP_RESULT_SUCCESS;
            }
        default:
            {
                OC_LOG(ERROR, TAG, "Invalid option.");
                return SP_RESULT_INVALID_PARAM;
            }
            return SP_RESULT_INTERNAL_ERROR;
    }
}

SPResult SPFinalizeProvisioning(unsigned short timeout,
                                SPTargetDeviceInfo_t *selectedDeviceInfo)
{
    // TODO
    if (NULL == selectedDeviceInfo)
    {
        OC_LOG(ERROR, TAG, "Target device Info is NULL.");
        return SP_RESULT_INVALID_PARAM;
    }

    uint16_t aclHash = 0; // value for beachhead version.
    selectedDeviceInfo->pstat->commitHash = aclHash;
    selectedDeviceInfo->pstat->tm = NORMAL;
    char *payloadBuffer = BinToPstatJSON(selectedDeviceInfo->pstat);
    if (NULL == payloadBuffer)
    {
        OC_LOG(ERROR, TAG, "Error while converting pstat bin to json");
        return SP_RESULT_INTERNAL_ERROR;
    }
    int payloadLen = strlen(payloadBuffer);

    handler = &FinalizeProvisioningHandler;
    gStateManager |= SP_UP_HASH_STARTED;

    CAResult_t result = sendCARequest(CA_PUT,
                                      &selectedDeviceInfo->endpoint,
                                      OC_FLAG_SECURE,
                                      OIC_RSRC_PSTAT_URI,
                                      payloadBuffer, payloadLen);
    OICFree(payloadBuffer);
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

    result = CACloseDtlsSession((CAEndpoint_t*)&selectedDeviceInfo->endpoint);
    if (CA_STATUS_OK != result)
    {
        OC_LOG(WARNING, TAG, "Failed to close the DTLS session.");
    }

    CADestroyToken(gToken);
    gStateManager = 0;
    gPstat = NULL;
    return res;
}

SPResult SPTerminateProvisioning()
{
    deleteList();
    return SP_RESULT_SUCCESS;;
}
