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

#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdbool.h>
#include <string.h>

#include "logger.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "cacommon.h"
#include "cainterface.h"
#include "base64.h"
#include "cJSON.h"
#include "global.h"

#include "srmresourcestrings.h"
#include "doxmresource.h"
#include "pstatresource.h"
#include "credresource.h"
#include "aclresource.h"
#include "ownershiptransfermanager.h"
#include "securevirtualresourcetypes.h"
#include "oxmjustworks.h"
#include "pmtypes.h"
#include "pmutility.h"
#include "srmutility.h"
#include "provisioningdatabasemanager.h"
#include "oxmrandompin.h"
#include "ocpayload.h"
#include "payload_logging.h"

#define TAG "OTM"

/**
 * Array to store the callbacks for each owner transfer method.
 */
static OTMCallbackData_t g_OTMDatas[OIC_OXM_COUNT];

/**
 * Variables for pointing the OTMContext to be used in the DTLS handshake result callback.
 */
static OTMContext_t* g_otmCtx = NULL;

/**
 * Function to select appropriate  provisioning method.
 *
 * @param[in] supportedMethods   Array of supported methods
 * @param[in] numberOfMethods   number of supported methods
 * @param[out]  selectedMethod         Selected methods
 * @return  OC_STACK_OK on success
 */
static OCStackResult SelectProvisioningMethod(const OicSecOxm_t *supportedMethods,
        size_t numberOfMethods, OicSecOxm_t *selectedMethod)
{
    OIC_LOG(DEBUG, TAG, "IN SelectProvisioningMethod");

    if(numberOfMethods == 0 || !supportedMethods)
    {
        OIC_LOG(WARNING, TAG, "Could not find a supported OxM.");
        return OC_STACK_ERROR;
    }

    *selectedMethod  = supportedMethods[0];
    for(size_t i = 0; i < numberOfMethods; i++)
    {
        if(*selectedMethod < supportedMethods[i])
        {
            *selectedMethod =  supportedMethods[i];
        }
    }

    return OC_STACK_OK;
}

/**
 * Function to select operation mode.This function will return most secure common operation mode.
 *
 * @param[in] selectedDeviceInfo   selected device information to performing provisioning.
 * @param[out]   selectedMode   selected operation mode
 * @return  OC_STACK_OK on success
 */
static void SelectOperationMode(const OCProvisionDev_t *selectedDeviceInfo,
                                OicSecDpom_t *selectedMode)
{
    OIC_LOG(DEBUG, TAG, "IN SelectOperationMode");
    *selectedMode = selectedDeviceInfo->pstat->sm[0];
    OIC_LOG_V(DEBUG, TAG, "Selected Operation Mode = %d", *selectedMode);
}

/**
 * Function to start ownership transfer.
 * This function will send the first request for provisioning,
 * The next request message is sent from the response handler for this request.
 *
 * @param[in] ctx   context value passed to callback from calling function.
 * @param[in] selectedDevice   selected device information to performing provisioning.
 * @return  OC_STACK_OK on success
 */
static OCStackResult StartOwnershipTransfer(void* ctx, OCProvisionDev_t* selectedDevice);

/**
 * Function to update owner transfer mode
 *
 * @param[in]  otmCtx  Context value of ownership transfer.
 * @return  OC_STACK_OK on success
 */
static OCStackResult PutOwnerTransferModeToResource(OTMContext_t* otmCtx);

/**
 * Function to send request to resource to get its pstat resource information.
 *
 * @param[in]  otmCtx  Context value of ownership transfer.
 * @return  OC_STACK_OK on success
 */
static OCStackResult GetProvisioningStatusResource(OTMContext_t* otmCtx);


/**
 * Function to send  uuid of owner device to new device.
 * This function would update 'owner of doxm' as UUID for provisioning tool.
 *
 * @param[in]  otmCtx  Context value of ownership transfer.
 * @return  OC_STACK_OK on success
 */
static OCStackResult PutOwnerUuid(OTMContext_t* otmCtx);

/**
 * Function to update the operation mode. As per the spec. Operation mode in client driven
 * single service provisioning it will be updated to 0x3
 *
 * @param[in]  otmCtx  Context value of ownership transfer.
 * @return  OC_STACK_OK on success
 */
static OCStackResult PutUpdateOperationMode(OTMContext_t* otmCtx);

/**
 * Function to update the owner credential to new device
 *
 * @param[in]  otmCtx  Context value of ownership transfer.
 * @param[in] selectedOperationMode selected operation mode
 * @return  OC_STACK_OK on success
 */
static OCStackResult PutOwnerCredential(OTMContext_t* otmCtx);

/**
 * Function to send ownerShip info.
 * This function would update 'owned of doxm' as true.
 *
 * @param[in]  otmCtx  Context value of ownership transfer.
 * @return  OC_STACK_OK on success
 */
static OCStackResult PutOwnershipInformation(OTMContext_t* otmCtx);

/**
 * Function to update pstat as Ready for provisioning.
 * This function would update 'cm' from bx0000,0010 to bx0000,0000.
 *
 * @param[in] ctx   context value passed to callback from calling function.
 * @param[in] selectedDevice   selected device information to performing provisioning.
 * @return  OC_STACK_OK on success
 */
static OCStackResult PutProvisioningStatus(OTMContext_t* otmCtx);

/**
 * Function to update pstat as Ready for Normal Operation.
 * This function would update 'isop' from false to true.
 *
 * @param[in] ctx   context value passed to callback from calling function.
 * @param[in] selectedDevice   selected device information to performing provisioning.
 * @return  OC_STACK_OK on success
 */
static OCStackResult PutNormalOperationStatus(OTMContext_t* otmCtx);

static bool IsComplete(OTMContext_t* otmCtx)
{
    for(size_t i = 0; i < otmCtx->ctxResultArraySize; i++)
    {
        if(OC_STACK_CONTINUE == otmCtx->ctxResultArray[i].res)
        {
            return false;
        }
    }

    return true;
}

/**
 * Function to save the result of provisioning.
 *
 * @param[in,out] otmCtx   Context value of ownership transfer.
 * @param[in] res   result of provisioning
 */
static void SetResult(OTMContext_t* otmCtx, const OCStackResult res)
{
    OIC_LOG_V(DEBUG, TAG, "IN SetResult : %d ", res);

    if(!otmCtx)
    {
        OIC_LOG(WARNING, TAG, "OTMContext is NULL");
        return;
    }

    if(otmCtx->selectedDeviceInfo)
    {
        //Revert psk_info callback and new deivce uuid in case of random PIN OxM
        if(OIC_RANDOM_DEVICE_PIN == otmCtx->selectedDeviceInfo->doxm->oxmSel)
        {
            if(CA_STATUS_OK != CARegisterDTLSCredentialsHandler(GetDtlsPskCredentials))
            {
                OIC_LOG(WARNING, TAG, "Failed to revert  is DTLS credential handler.");
            }
            OicUuid_t emptyUuid = { .id={0}};
            SetUuidForRandomPinOxm(&emptyUuid);
        }

        for(size_t i = 0; i < otmCtx->ctxResultArraySize; i++)
        {
            if(memcmp(otmCtx->selectedDeviceInfo->doxm->deviceID.id,
                      otmCtx->ctxResultArray[i].deviceId.id, UUID_LENGTH) == 0)
            {
                otmCtx->ctxResultArray[i].res = res;
                if(OC_STACK_OK != res)
                {
                    otmCtx->ctxHasError = true;
                }
            }
        }

        g_otmCtx = NULL;

        //If all request is completed, invoke the user callback.
        if(IsComplete(otmCtx))
        {
            otmCtx->ctxResultCallback(otmCtx->userCtx, otmCtx->ctxResultArraySize,
                                       otmCtx->ctxResultArray, otmCtx->ctxHasError);
            OICFree(otmCtx->ctxResultArray);
            OICFree(otmCtx);
        }
        else
        {
            if(OC_STACK_OK != StartOwnershipTransfer(otmCtx,
                                                     otmCtx->selectedDeviceInfo->next))
            {
                OIC_LOG(ERROR, TAG, "Failed to StartOwnershipTransfer");
            }
        }
    }

    OIC_LOG(DEBUG, TAG, "OUT SetResult");
}

/**
 * Function to handle the handshake result in OTM.
 * This function will be invoked after DTLS handshake
 * @param   endPoint  [IN] The remote endpoint.
 * @param   errorInfo [IN] Error information from the endpoint.
 * @return  NONE
 */
void DTLSHandshakeCB(const CAEndpoint_t *endpoint, const CAErrorInfo_t *info)
{
    if(NULL != g_otmCtx && NULL != g_otmCtx->selectedDeviceInfo &&
       NULL != endpoint && NULL != info)
    {
        OIC_LOG_V(INFO, TAG, "Received status from remote device(%s:%d) : %d",
                 endpoint->addr, endpoint->port, info->result);

        OicSecDoxm_t* newDevDoxm = g_otmCtx->selectedDeviceInfo->doxm;

        if(NULL != newDevDoxm)
        {
            OicUuid_t emptyUuid = {.id={0}};

            //Make sure the address matches.
            if(strncmp(g_otmCtx->selectedDeviceInfo->endpoint.addr,
               endpoint->addr,
               sizeof(endpoint->addr)) == 0 &&
               g_otmCtx->selectedDeviceInfo->securePort == endpoint->port)
            {
                OCStackResult res = OC_STACK_ERROR;

                //If temporal secure sesstion established successfully
                if(CA_STATUS_OK == info->result &&
                   false == newDevDoxm->owned &&
                   memcmp(&(newDevDoxm->owner), &emptyUuid, sizeof(OicUuid_t)) == 0)
                {
                    //Send request : PUT /oic/sec/doxm [{... , "devowner":"PT's UUID"}]
                    res = PutOwnerUuid(g_otmCtx);
                    if(OC_STACK_OK != res)
                    {
                        OIC_LOG(ERROR, TAG, "OperationModeUpdate : Failed to send owner information");
                        SetResult(g_otmCtx, res);
                    }
                }
                //In case of authentication failure
                else if(CA_DTLS_AUTHENTICATION_FAILURE == info->result)
                {
                    //in case of error from owner credential
                    if(memcmp(&(newDevDoxm->owner), &emptyUuid, sizeof(OicUuid_t)) != 0 &&
                        true == newDevDoxm->owned)
                    {
                        OIC_LOG(ERROR, TAG, "The owner credential may incorrect.");

                        if(OC_STACK_OK != RemoveCredential(&(newDevDoxm->deviceID)))
                        {
                            OIC_LOG(WARNING, TAG, "Failed to remove the invaild owner credential");
                        }
                        SetResult(g_otmCtx, OC_STACK_AUTHENTICATION_FAILURE);
                    }
                    //in case of error from wrong PIN, re-start the ownership transfer
                    else if(OIC_RANDOM_DEVICE_PIN == newDevDoxm->oxmSel)
                    {
                        OIC_LOG(ERROR, TAG, "The PIN number may incorrect.");

                        memcpy(&(newDevDoxm->owner), &emptyUuid, sizeof(OicUuid_t));
                        newDevDoxm->owned = false;
                        g_otmCtx->attemptCnt++;

                        if(WRONG_PIN_MAX_ATTEMP > g_otmCtx->attemptCnt)
                        {
                            res = StartOwnershipTransfer(g_otmCtx, g_otmCtx->selectedDeviceInfo);
                            if(OC_STACK_OK != res)
                            {
                                SetResult(g_otmCtx, res);
                                OIC_LOG(ERROR, TAG, "Failed to Re-StartOwnershipTransfer");
                            }
                        }
                        else
                        {
                            OIC_LOG(ERROR, TAG, "User has exceeded the number of authentication attempts.");
                            SetResult(g_otmCtx, OC_STACK_AUTHENTICATION_FAILURE);
                        }
                    }
                    else
                    {
                        OIC_LOG(ERROR, TAG, "Failed to establish secure session.");
                        SetResult(g_otmCtx, OC_STACK_AUTHENTICATION_FAILURE);
                    }
                }
            }
        }
    }
}

/**
 * Function to save ownerPSK at provisioning tool end.
 *
 * @param[in] selectedDeviceInfo   selected device information to performing provisioning.
 * @return  OC_STACK_OK on success
 */
static OCStackResult SaveOwnerPSK(OCProvisionDev_t *selectedDeviceInfo)
{
    OIC_LOG(DEBUG, TAG, "IN SaveOwnerPSK");

    OCStackResult res = OC_STACK_ERROR;

    CAEndpoint_t endpoint;
    memset(&endpoint, 0x00, sizeof(CAEndpoint_t));
    OICStrcpy(endpoint.addr, MAX_ADDR_STR_SIZE_CA, selectedDeviceInfo->endpoint.addr);
    endpoint.addr[MAX_ADDR_STR_SIZE_CA - 1] = '\0';
    endpoint.port = selectedDeviceInfo->securePort;

    OicUuid_t ptDeviceID = {.id={0}};
    if (OC_STACK_OK != GetDoxmDeviceID(&ptDeviceID))
    {
        OIC_LOG(ERROR, TAG, "Error while retrieving provisioning tool's device ID");
        return res;
    }

    uint8_t ownerPSK[OWNER_PSK_LENGTH_128] = {0};
    OicSecKey_t ownerKey = {ownerPSK, OWNER_PSK_LENGTH_128};

    //Generating OwnerPSK
    CAResult_t pskRet = CAGenerateOwnerPSK(&endpoint,
            (uint8_t *)GetOxmString(selectedDeviceInfo->doxm->oxmSel),
            strlen(GetOxmString(selectedDeviceInfo->doxm->oxmSel)),
            ptDeviceID.id, sizeof(ptDeviceID.id),
            selectedDeviceInfo->doxm->deviceID.id, sizeof(selectedDeviceInfo->doxm->deviceID.id),
            ownerPSK, OWNER_PSK_LENGTH_128);

    if (CA_STATUS_OK == pskRet)
    {
        OIC_LOG(INFO, TAG,"ownerPSK dump:\n");
        OIC_LOG_BUFFER(INFO, TAG,ownerPSK, OWNER_PSK_LENGTH_128);
        //Generating new credential for provisioning tool
        OicSecCred_t *cred = GenerateCredential(&selectedDeviceInfo->doxm->deviceID,
                SYMMETRIC_PAIR_WISE_KEY, NULL,
                &ownerKey, &ptDeviceID);
        VERIFY_NON_NULL(TAG, cred, ERROR);

        res = AddCredential(cred);
        if(res != OC_STACK_OK)
        {
            DeleteCredList(cred);
            return res;
        }
    }
    else
    {
        OIC_LOG(ERROR, TAG, "CAGenerateOwnerPSK failed");
    }

    OIC_LOG(DEBUG, TAG, "OUT SaveOwnerPSK");
exit:
    return res;
}

/**
 * Callback handler for OwnerShipTransferModeHandler API.
 *
 * @param[in] ctx             ctx value passed to callback from calling function.
 * @param[in] UNUSED          handle to an invocation
 * @param[in] clientResponse  Response from queries to remote servers.
 * @return  OC_STACK_DELETE_TRANSACTION to delete the transaction
 *          and  OC_STACK_KEEP_TRANSACTION to keep it.
 */
static OCStackApplicationResult OwnerTransferModeHandler(void *ctx, OCDoHandle UNUSED,
                                                         OCClientResponse *clientResponse)
{
    OIC_LOG(DEBUG, TAG, "IN OwnerTransferModeHandler");

    VERIFY_NON_NULL(TAG, clientResponse, WARNING);
    VERIFY_NON_NULL(TAG, ctx, WARNING);

    OTMContext_t* otmCtx = (OTMContext_t*)ctx;
    (void)UNUSED;
    if(clientResponse->result == OC_STACK_OK)
    {
        OIC_LOG(INFO, TAG, "OwnerTransferModeHandler : response result = OC_STACK_OK");
        //Send request : GET /oic/sec/pstat
        OCStackResult res = GetProvisioningStatusResource(otmCtx);
        if(OC_STACK_OK != res)
        {
            OIC_LOG(WARNING, TAG, "Failed to get pstat information");
            SetResult(otmCtx, res);
        }
    }
    else
    {
        OIC_LOG_V(WARNING, TAG, "OwnerTransferModeHandler : Client response is incorrect : %d",
        clientResponse->result);
        SetResult(otmCtx, clientResponse->result);
    }

    OIC_LOG(DEBUG, TAG, "OUT OwnerTransferModeHandler");

exit:
    return  OC_STACK_DELETE_TRANSACTION;
}

/**
 * Callback handler for ProvisioningStatusResouceHandler API.
 *
 * @param[in] ctx             ctx value passed to callback from calling function.
 * @param[in] UNUSED          handle to an invocation
 * @param[in] clientResponse  Response from queries to remote servers.
 * @return  OC_STACK_DELETE_TRANSACTION to delete the transaction
 *          and  OC_STACK_KEEP_TRANSACTION to keep it.
 */
static OCStackApplicationResult ListMethodsHandler(void *ctx, OCDoHandle UNUSED,
                                                    OCClientResponse *clientResponse)
{
    OIC_LOG(DEBUG, TAG, "IN ListMethodsHandler");

    VERIFY_NON_NULL(TAG, clientResponse, WARNING);
    VERIFY_NON_NULL(TAG, ctx, WARNING);

    OTMContext_t* otmCtx = (OTMContext_t*)ctx;
    (void)UNUSED;
    if  (OC_STACK_OK == clientResponse->result)
    {
        if  (NULL == clientResponse->payload)
        {
            OIC_LOG(INFO, TAG, "Skiping Null payload");
            SetResult(otmCtx, OC_STACK_ERROR);
            return OC_STACK_DELETE_TRANSACTION;
        }

        if (PAYLOAD_TYPE_SECURITY != clientResponse->payload->type)
        {
            OIC_LOG(INFO, TAG, "Unknown payload type");
            SetResult(otmCtx, OC_STACK_ERROR);
            return OC_STACK_DELETE_TRANSACTION;
        }
        OicSecPstat_t* pstat = NULL;
        OCStackResult result = CBORPayloadToPstat(
                ((OCSecurityPayload*)clientResponse->payload)->securityData,
                ((OCSecurityPayload*)clientResponse->payload)->payloadSize,
                &pstat);
        if(NULL == pstat || result != OC_STACK_OK)
        {
            OIC_LOG(ERROR, TAG, "Error while converting cbor to pstat.");
            SetResult(otmCtx, OC_STACK_ERROR);
            return OC_STACK_DELETE_TRANSACTION;
        }
        if(false == (TAKE_OWNER & pstat->cm))
        {
            OIC_LOG(ERROR, TAG, "Device pairing mode enabling owner transfer operations is disabled");
            SetResult(otmCtx, OC_STACK_ERROR);
            return OC_STACK_DELETE_TRANSACTION;
        }
        otmCtx->selectedDeviceInfo->pstat = pstat;

        //Select operation mode (Currently supported SINGLE_SERVICE_CLIENT_DRIVEN only)
        SelectOperationMode(otmCtx->selectedDeviceInfo, &(otmCtx->selectedDeviceInfo->pstat->om));

        //Send request : PUT /oic/sec/pstat [{"om":"bx11", .. }]
        OCStackResult res = PutUpdateOperationMode(otmCtx);
        if (OC_STACK_OK != res)
        {
            OIC_LOG(ERROR, TAG, "Error while updating operation mode.");
            SetResult(otmCtx, res);
        }
    }
    else
    {
        OIC_LOG_V(WARNING, TAG, "ListMethodsHandler : Client response is incorrect : %d",
            clientResponse->result);
        SetResult(otmCtx, clientResponse->result);
    }

    OIC_LOG(DEBUG, TAG, "OUT ListMethodsHandler");
exit:
    return  OC_STACK_DELETE_TRANSACTION;
}

/**
 * Response handler for update owner uuid request.
 *
 * @param[in] ctx             ctx value passed to callback from calling function.
 * @param[in] UNUSED          handle to an invocation
 * @param[in] clientResponse  Response from queries to remote servers.
 * @return  OC_STACK_DELETE_TRANSACTION to delete the transaction
 *          and  OC_STACK_KEEP_TRANSACTION to keep it.
 */
static OCStackApplicationResult OwnerUuidUpdateHandler(void *ctx, OCDoHandle UNUSED,
                                OCClientResponse *clientResponse)
{
    VERIFY_NON_NULL(TAG, clientResponse, WARNING);
    VERIFY_NON_NULL(TAG, ctx, WARNING);

    OIC_LOG(DEBUG, TAG, "IN OwnerUuidUpdateHandler");
    (void)UNUSED;
    OCStackResult res = OC_STACK_OK;
    OTMContext_t* otmCtx = (OTMContext_t*)ctx;

    if(OC_STACK_OK == clientResponse->result)
    {
        if(otmCtx && otmCtx->selectedDeviceInfo)
        {
            res = SaveOwnerPSK(otmCtx->selectedDeviceInfo);
            if(OC_STACK_OK != res)
            {
                OIC_LOG(ERROR, TAG, "OwnerUuidUpdateHandler:Failed to owner PSK generation");
                SetResult(otmCtx, res);
                return OC_STACK_DELETE_TRANSACTION;
            }

            //PUT owner credential to new device according to security spec B.
            res = PutOwnerCredential(otmCtx);
            if(OC_STACK_OK != res)
            {
                OIC_LOG(ERROR, TAG,
                        "OwnerUuidUpdateHandler:Failed to send PUT request for onwer credential");
                SetResult(otmCtx, res);
                return OC_STACK_DELETE_TRANSACTION;
            }
        }
    }
    else
    {
        res = clientResponse->result;
        OIC_LOG_V(ERROR, TAG, "OwnerUuidHandler : Unexpected result %d", res);
        SetResult(otmCtx, res);
    }

    OIC_LOG(DEBUG, TAG, "OUT OwnerUuidUpdateHandler");

exit:
    return  OC_STACK_DELETE_TRANSACTION;
}

/**
 * Response handler for update operation mode.
 *
 * @param[in] ctx             ctx value passed to callback from calling function.
 * @param[in] UNUSED          handle to an invocation
 * @param[in] clientResponse  Response from queries to remote servers.
 * @return  OC_STACK_DELETE_TRANSACTION to delete the transaction
 *          and  OC_STACK_KEEP_TRANSACTION to keep it.
 */
static OCStackApplicationResult OperationModeUpdateHandler(void *ctx, OCDoHandle UNUSED,
                                OCClientResponse *clientResponse)
{
    OIC_LOG(DEBUG, TAG, "IN OperationModeUpdateHandler");

    VERIFY_NON_NULL(TAG, clientResponse, WARNING);
    VERIFY_NON_NULL(TAG, ctx, WARNING);

    OTMContext_t* otmCtx = (OTMContext_t*)ctx;
    (void) UNUSED;
    if  (OC_STACK_OK == clientResponse->result)
    {
        OCStackResult res = OC_STACK_ERROR;
        OicSecOxm_t selOxm = otmCtx->selectedDeviceInfo->doxm->oxmSel;
        //DTLS Handshake
        //Load secret for temporal secure session.
        if(g_OTMDatas[selOxm].loadSecretCB)
        {
            res = g_OTMDatas[selOxm].loadSecretCB(otmCtx);
            if(OC_STACK_OK != res)
            {
                OIC_LOG(ERROR, TAG, "OperationModeUpdate : Failed to load secret");
                SetResult(otmCtx, res);
                return  OC_STACK_DELETE_TRANSACTION;
            }
        }

        //It will be used in handshake event handler
        g_otmCtx = otmCtx;

        //Try DTLS handshake to generate secure session
        if(g_OTMDatas[selOxm].createSecureSessionCB)
        {
            res = g_OTMDatas[selOxm].createSecureSessionCB(otmCtx);
            if(OC_STACK_OK != res)
            {
                OIC_LOG(ERROR, TAG, "OperationModeUpdate : Failed to create DTLS session");
                SetResult(otmCtx, res);
                return OC_STACK_DELETE_TRANSACTION;
            }
        }
    }
    else
    {
        OIC_LOG(ERROR, TAG, "Error while update operation mode");
        SetResult(otmCtx, clientResponse->result);
    }

    OIC_LOG(DEBUG, TAG, "OUT OperationModeUpdateHandler");

exit:
    return  OC_STACK_DELETE_TRANSACTION;
}

/**
 * Response handler for update owner crendetial request.
 *
 * @param[in] ctx             ctx value passed to callback from calling function.
 * @param[in] UNUSED          handle to an invocation
 * @param[in] clientResponse  Response from queries to remote servers.
 * @return  OC_STACK_DELETE_TRANSACTION to delete the transaction
 *          and  OC_STACK_KEEP_TRANSACTION to keep it.
 */
static OCStackApplicationResult OwnerCredentialHandler(void *ctx, OCDoHandle UNUSED,
                                OCClientResponse *clientResponse)
{
    VERIFY_NON_NULL(TAG, clientResponse, WARNING);
    VERIFY_NON_NULL(TAG, ctx, WARNING);

    OIC_LOG(DEBUG, TAG, "IN OwnerCredentialHandler");
    (void)UNUSED;
    OCStackResult res = OC_STACK_OK;
    OTMContext_t* otmCtx = (OTMContext_t*)ctx;

    if(OC_STACK_RESOURCE_CREATED == clientResponse->result)
    {
        if(otmCtx && otmCtx->selectedDeviceInfo)
        {
            //Close the temporal secure session to verify the owner credential
            CAEndpoint_t* endpoint = (CAEndpoint_t *)&otmCtx->selectedDeviceInfo->endpoint;
            endpoint->port = otmCtx->selectedDeviceInfo->securePort;
            CAResult_t caResult = CACloseDtlsSession(endpoint);
            if(CA_STATUS_OK != caResult)
            {
                OIC_LOG(ERROR, TAG, "Failed to close DTLS session");
                SetResult(otmCtx, caResult);
                return OC_STACK_DELETE_TRANSACTION;
            }

            /**
             * If we select NULL cipher,
             * client will select appropriate cipher suite according to server's cipher-suite list.
             */
            caResult = CASelectCipherSuite(TLS_NULL_WITH_NULL_NULL);
            if(CA_STATUS_OK != caResult)
            {
                OIC_LOG(ERROR, TAG, "Failed to select TLS_NULL_WITH_NULL_NULL");
                SetResult(otmCtx, caResult);
                return OC_STACK_DELETE_TRANSACTION;
            }

            /**
             * in case of random PIN based OxM,
             * revert get_psk_info callback of tinyDTLS to use owner credential.
             */
            if(OIC_RANDOM_DEVICE_PIN == otmCtx->selectedDeviceInfo->doxm->oxmSel)
            {
                OicUuid_t emptyUuid = { .id={0}};
                SetUuidForRandomPinOxm(&emptyUuid);

                if(CA_STATUS_OK != CARegisterDTLSCredentialsHandler(GetDtlsPskCredentials))
                {
                    OIC_LOG(ERROR, TAG, "Failed to revert DTLS credential handler.");
                    SetResult(otmCtx, OC_STACK_INVALID_CALLBACK);
                    return OC_STACK_DELETE_TRANSACTION;
                }
            }

            //PUT /oic/sec/doxm [{ ..., "owned":"TRUE" }]
            res = PutOwnershipInformation(otmCtx);
            if(OC_STACK_OK != res)
            {
                OIC_LOG(ERROR, TAG, "Failed to put ownership information to new device");
                SetResult(otmCtx, res);
                return OC_STACK_DELETE_TRANSACTION;
            }
        }
    }
    else
    {
        res = clientResponse->result;
        OIC_LOG_V(ERROR, TAG, "OwnerCredentialHandler : Unexpected result %d", res);
        SetResult(otmCtx, res);
    }

    OIC_LOG(DEBUG, TAG, "OUT OwnerCredentialHandler");

exit:
    return  OC_STACK_DELETE_TRANSACTION;
}


/**
 * Response handler for update owner information request.
 *
 * @param[in] ctx             ctx value passed to callback from calling function.
 * @param[in] UNUSED          handle to an invocation
 * @param[in] clientResponse  Response from queries to remote servers.
 * @return  OC_STACK_DELETE_TRANSACTION to delete the transaction
 *          and  OC_STACK_KEEP_TRANSACTION to keep it.
 */
static OCStackApplicationResult OwnershipInformationHandler(void *ctx, OCDoHandle UNUSED,
                                OCClientResponse *clientResponse)
{
    VERIFY_NON_NULL(TAG, clientResponse, WARNING);
    VERIFY_NON_NULL(TAG, ctx, WARNING);

    OIC_LOG(DEBUG, TAG, "IN OwnershipInformationHandler");
    (void)UNUSED;
    OCStackResult res = OC_STACK_OK;
    OTMContext_t* otmCtx = (OTMContext_t*)ctx;

    if(OC_STACK_OK == clientResponse->result)
    {
        if(otmCtx && otmCtx->selectedDeviceInfo)
        {
            OIC_LOG(INFO, TAG, "Ownership transfer was successfully completed.");
            OIC_LOG(INFO, TAG, "Set Ready for provisioning state .");

            res = PutProvisioningStatus(otmCtx);
            if(OC_STACK_OK != res)
            {
                OIC_LOG(ERROR, TAG, "Failed to update pstat");
                SetResult(otmCtx, res);
            }
        }
    }
    else
    {
        res = clientResponse->result;
        OIC_LOG_V(ERROR, TAG, "OwnershipInformationHandler : Unexpected result %d", res);
        SetResult(otmCtx, res);
    }

    OIC_LOG(DEBUG, TAG, "OUT OwnershipInformationHandler");

exit:
    return  OC_STACK_DELETE_TRANSACTION;
}

/**
 * Response handler of update provisioning status.
 *
 * @param[in] ctx             ctx value passed to callback from calling function.
 * @param[in] UNUSED          handle to an invocation
 * @param[in] clientResponse  Response from queries to remote servers.
 * @return  OC_STACK_DELETE_TRANSACTION to delete the transaction
 *          and OC_STACK_KEEP_TRANSACTION to keep it.
 */
static OCStackApplicationResult ProvisioningStatusHandler(void *ctx, OCDoHandle UNUSED,
                                                       OCClientResponse *clientResponse)
{
    OIC_LOG_V(INFO, TAG, "IN ProvisioningStatusHandler.");

    VERIFY_NON_NULL(TAG, clientResponse, ERROR);
    VERIFY_NON_NULL(TAG, ctx, ERROR);

    OTMContext_t* otmCtx = (OTMContext_t*) ctx;
    (void)UNUSED;
    OCStackResult res = OC_STACK_OK;

    if(OC_STACK_OK == clientResponse->result)
    {
        if(otmCtx && otmCtx->selectedDeviceInfo)
        {
            OIC_LOG(INFO, TAG, "Device state is in Ready for Provisionig.");

            res = PutNormalOperationStatus(otmCtx);
            if(OC_STACK_OK != res)
            {
                OIC_LOG(ERROR, TAG, "Failed to update pstat");
                SetResult(otmCtx, res);
            }
        }
    }
    else
    {
        OIC_LOG_V(INFO, TAG, "Error occured in provisionDefaultACLCB :: %d\n",
                            clientResponse->result);
        SetResult(otmCtx, clientResponse->result);
    }

exit:
    OIC_LOG_V(INFO, TAG, "OUT ProvisioningStatusHandler.");
    return OC_STACK_DELETE_TRANSACTION;
}

/**
 * Response handler of update provisioning status to Ready for Normal..
 *
 * @param[in] ctx             ctx value passed to callback from calling function.
 * @param[in] UNUSED          handle to an invocation
 * @param[in] clientResponse  Response from queries to remote servers.
 * @return  OC_STACK_DELETE_TRANSACTION to delete the transaction
 *          and OC_STACK_KEEP_TRANSACTION to keep it.
 */
static OCStackApplicationResult ReadyForNomalStatusHandler(void *ctx, OCDoHandle UNUSED,
                                                       OCClientResponse *clientResponse)
{
    OIC_LOG_V(INFO, TAG, "IN ReadyForNomalStatusHandler.");

    VERIFY_NON_NULL(TAG, clientResponse, ERROR);
    VERIFY_NON_NULL(TAG, ctx, ERROR);

    OTMContext_t* otmCtx = (OTMContext_t*) ctx;
    (void)UNUSED;

    if (OC_STACK_OK == clientResponse->result)
    {
        OIC_LOG(INFO, TAG, "Device state is in Ready for Normal Operation.");
        OCStackResult res = PDMAddDevice(&otmCtx->selectedDeviceInfo->doxm->deviceID);
         if (OC_STACK_OK == res)
         {
                OIC_LOG_V(INFO, TAG, "Add device's UUID in PDM_DB");
                SetResult(otmCtx, OC_STACK_OK);
                return OC_STACK_DELETE_TRANSACTION;
         }
          else
         {
              OIC_LOG(ERROR, TAG, "Ownership transfer is complete but adding information to DB is failed.");
         }
    }
    else
    {
        OIC_LOG_V(INFO, TAG, "Error occured in provisionDefaultACLCB :: %d\n",
                            clientResponse->result);
        SetResult(otmCtx, clientResponse->result);
    }

exit:
    OIC_LOG_V(INFO, TAG, "OUT ReadyForNomalStatusHandler.");
    return OC_STACK_DELETE_TRANSACTION;
}

static OCStackResult PutOwnerCredential(OTMContext_t* otmCtx)
{
    OIC_LOG(DEBUG, TAG, "IN PutOwnerCredential");

    if(!otmCtx || !otmCtx->selectedDeviceInfo)
    {
        OIC_LOG(ERROR, TAG, "Invalid parameters");
        return OC_STACK_INVALID_PARAM;
    }

    OCProvisionDev_t* deviceInfo = otmCtx->selectedDeviceInfo;
    char query[MAX_URI_LENGTH + MAX_QUERY_LENGTH] = {0};

    if(!PMGenerateQuery(true,
                        deviceInfo->endpoint.addr, deviceInfo->securePort,
                        deviceInfo->connType,
                        query, sizeof(query), OIC_RSRC_CRED_URI))
    {
        OIC_LOG(ERROR, TAG, "PutOwnerCredential : Failed to generate query");
        return OC_STACK_ERROR;
    }
    OIC_LOG_V(DEBUG, TAG, "Query=%s", query);
    OCSecurityPayload* secPayload = (OCSecurityPayload*)OICCalloc(1, sizeof(OCSecurityPayload));
    if(!secPayload)
    {
        OIC_LOG(ERROR, TAG, "Failed to memory allocation");
        return OC_STACK_NO_MEMORY;
    }

    //Generate owner credential for new device
    secPayload->base.type = PAYLOAD_TYPE_SECURITY;
    const OicSecCred_t* ownerCredential = GetCredResourceData(&(deviceInfo->doxm->deviceID));
    if(!ownerCredential)
    {
        OIC_LOG(ERROR, TAG, "Can not find OwnerPSK.");
        return OC_STACK_NO_RESOURCE;
    }

    OicUuid_t credSubjectId = {.id={0}};
    if(OC_STACK_OK == GetDoxmDeviceID(&credSubjectId))
    {
        OicSecCred_t newCredential;
        memcpy(&newCredential, ownerCredential, sizeof(OicSecCred_t));
        newCredential.next = NULL;

        //Set subject ID as PT's ID
        memcpy(&(newCredential.subject), &credSubjectId, sizeof(OicUuid_t));

        //Fill private data as empty string
        newCredential.privateData.data = NULL;
        newCredential.privateData.len = 0;
#ifdef __WITH_X509__
        newCredential.publicData.data = NULL;
        newCredential.publicData.len = 0;
#endif

        //Send owner credential to new device : PUT /oic/sec/cred [ owner credential ]
        if (OC_STACK_OK != CredToCBORPayload(&newCredential, &secPayload->securityData, &secPayload->payloadSize))
        {
            OICFree(secPayload);
            OIC_LOG(ERROR, TAG, "Error while converting bin to cbor.");
            return OC_STACK_ERROR;
        }
        OIC_LOG(DEBUG, TAG, "Cred Payload:");
        OIC_LOG_BUFFER(DEBUG, TAG, secPayload->securityData, secPayload->payloadSize);

        OCCallbackData cbData;
        cbData.cb = &OwnerCredentialHandler;
        cbData.context = (void *)otmCtx;
        cbData.cd = NULL;
        OCStackResult res = OCDoResource(NULL, OC_REST_PUT, query,
                                         &deviceInfo->endpoint, (OCPayload*)secPayload,
                                         deviceInfo->connType, OC_LOW_QOS, &cbData, NULL, 0);
        if (res != OC_STACK_OK)
        {
            OIC_LOG(ERROR, TAG, "OCStack resource error");
        }
    }
    else
    {
        OIC_LOG(ERROR, TAG, "Failed to read DOXM device ID.");
        return OC_STACK_NO_RESOURCE;
    }

    OIC_LOG(DEBUG, TAG, "OUT PutOwnerCredential");

    return OC_STACK_OK;
}

static OCStackResult PutOwnerTransferModeToResource(OTMContext_t* otmCtx)
{
    OIC_LOG(DEBUG, TAG, "IN PutOwnerTransferModeToResource");

    if(!otmCtx || !otmCtx->selectedDeviceInfo)
    {
        OIC_LOG(ERROR, TAG, "Invalid parameters");
        return OC_STACK_INVALID_PARAM;
    }

    OCProvisionDev_t* deviceInfo = otmCtx->selectedDeviceInfo;
    OicSecOxm_t selectedOxm = deviceInfo->doxm->oxmSel;
    char query[MAX_URI_LENGTH + MAX_QUERY_LENGTH] = {0};

    if(!PMGenerateQuery(false,
                        deviceInfo->endpoint.addr, deviceInfo->endpoint.port,
                        deviceInfo->connType,
                        query, sizeof(query), OIC_RSRC_DOXM_URI))
    {
        OIC_LOG(ERROR, TAG, "PutOwnerTransferModeToResource : Failed to generate query");
        return OC_STACK_ERROR;
    }
    OIC_LOG_V(DEBUG, TAG, "Query=%s", query);
    OCSecurityPayload* secPayload = (OCSecurityPayload*)OICCalloc(1, sizeof(OCSecurityPayload));
    if(!secPayload)
    {
        OIC_LOG(ERROR, TAG, "Failed to memory allocation");
        return OC_STACK_NO_MEMORY;
    }
    secPayload->base.type = PAYLOAD_TYPE_SECURITY;
    OCStackResult res = g_OTMDatas[selectedOxm].createSelectOxmPayloadCB(otmCtx,
            &secPayload->securityData, &secPayload->payloadSize);
    if (OC_STACK_OK != res && NULL == secPayload->securityData)
    {
        OCPayloadDestroy((OCPayload *)secPayload);
        OIC_LOG(ERROR, TAG, "Error while converting bin to cbor");
        return OC_STACK_ERROR;
    }

    OCCallbackData cbData;
    cbData.cb = &OwnerTransferModeHandler;
    cbData.context = (void *)otmCtx;
    cbData.cd = NULL;
    res = OCDoResource(NULL, OC_REST_PUT, query,
                       &deviceInfo->endpoint, (OCPayload *)secPayload,
                       deviceInfo->connType, OC_LOW_QOS, &cbData, NULL, 0);
    if (res != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "OCStack resource error");
    }

    OIC_LOG(DEBUG, TAG, "OUT PutOwnerTransferModeToResource");

    return res;
}

static OCStackResult GetProvisioningStatusResource(OTMContext_t* otmCtx)
{
    OIC_LOG(DEBUG, TAG, "IN GetProvisioningStatusResource");

    if(!otmCtx || !otmCtx->selectedDeviceInfo)
    {
        OIC_LOG(ERROR, TAG, "Invailed parameters");
        return OC_STACK_INVALID_PARAM;
    }

    OCProvisionDev_t* deviceInfo = otmCtx->selectedDeviceInfo;
    char query[MAX_URI_LENGTH + MAX_QUERY_LENGTH] = {0};
    if(!PMGenerateQuery(false,
                        deviceInfo->endpoint.addr, deviceInfo->endpoint.port,
                        deviceInfo->connType,
                        query, sizeof(query), OIC_RSRC_PSTAT_URI))
    {
        OIC_LOG(ERROR, TAG, "GetProvisioningStatusResource : Failed to generate query");
        return OC_STACK_ERROR;
    }
    OIC_LOG_V(DEBUG, TAG, "Query=%s", query);

    OCCallbackData cbData;
    cbData.cb = &ListMethodsHandler;
    cbData.context = (void *)otmCtx;
    cbData.cd = NULL;
    OCStackResult res = OCDoResource(NULL, OC_REST_GET, query, NULL, NULL,
                                     deviceInfo->connType, OC_LOW_QOS, &cbData, NULL, 0);
    if (res != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "OCStack resource error");
    }

    OIC_LOG(DEBUG, TAG, "OUT GetProvisioningStatusResource");

    return res;
}

static OCStackResult PutOwnerUuid(OTMContext_t* otmCtx)
{
    OIC_LOG(DEBUG, TAG, "IN PutOwnerUuid");

    if(!otmCtx || !otmCtx->selectedDeviceInfo)
    {
        OIC_LOG(ERROR, TAG, "Invailed parameters");
        return OC_STACK_INVALID_PARAM;
    }

    OCProvisionDev_t* deviceInfo = otmCtx->selectedDeviceInfo;
    char query[MAX_URI_LENGTH + MAX_QUERY_LENGTH] = {0};
    if(!PMGenerateQuery(true,
                        deviceInfo->endpoint.addr, deviceInfo->securePort,
                        deviceInfo->connType,
                        query, sizeof(query), OIC_RSRC_DOXM_URI))
    {
        OIC_LOG(ERROR, TAG, "PutOwnershipInformation : Failed to generate query");
        return OC_STACK_ERROR;
    }
    OIC_LOG_V(DEBUG, TAG, "Query=%s", query);

    //PUT PT's uuid to new device
    OCSecurityPayload* secPayload = (OCSecurityPayload*)OICCalloc(1, sizeof(OCSecurityPayload));
    if(!secPayload)
    {
        OIC_LOG(ERROR, TAG, "Failed to memory allocation");
        return OC_STACK_NO_MEMORY;
    }
    secPayload->base.type = PAYLOAD_TYPE_SECURITY;
    OCStackResult res =  g_OTMDatas[deviceInfo->doxm->oxmSel].createOwnerTransferPayloadCB(
            otmCtx, &secPayload->securityData, &secPayload->payloadSize);
    if (OC_STACK_OK != res && NULL == secPayload->securityData)
    {
        OCPayloadDestroy((OCPayload *)secPayload);
        OIC_LOG(ERROR, TAG, "Error while converting doxm bin to cbor.");
        return OC_STACK_INVALID_PARAM;
    }
    OIC_LOG_BUFFER(DEBUG, TAG, secPayload->securityData, secPayload->payloadSize);

    OCCallbackData cbData;
    cbData.cb = &OwnerUuidUpdateHandler;
    cbData.context = (void *)otmCtx;
    cbData.cd = NULL;

    res = OCDoResource(NULL, OC_REST_PUT, query, 0, (OCPayload *)secPayload,
            deviceInfo->connType, OC_LOW_QOS, &cbData, NULL, 0);
    if (res != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "OCStack resource error");
    }

    OIC_LOG(DEBUG, TAG, "OUT PutOwnerUuid");

    return res;
}

static OCStackResult PutOwnershipInformation(OTMContext_t* otmCtx)
{
    OIC_LOG(DEBUG, TAG, "IN PutOwnershipInformation");

    if(!otmCtx || !otmCtx->selectedDeviceInfo)
    {
        OIC_LOG(ERROR, TAG, "Invailed parameters");
        return OC_STACK_INVALID_PARAM;
    }

    OCProvisionDev_t* deviceInfo = otmCtx->selectedDeviceInfo;
    char query[MAX_URI_LENGTH + MAX_QUERY_LENGTH] = {0};
    if(!PMGenerateQuery(true,
                        deviceInfo->endpoint.addr, deviceInfo->securePort,
                        deviceInfo->connType,
                        query, sizeof(query), OIC_RSRC_DOXM_URI))
    {
        OIC_LOG(ERROR, TAG, "PutOwnershipInformation : Failed to generate query");
        return OC_STACK_ERROR;
    }
    OIC_LOG_V(DEBUG, TAG, "Query=%s", query);

    //OwnershipInformationHandler
    OCSecurityPayload *secPayload = (OCSecurityPayload*)OICCalloc(1, sizeof(OCSecurityPayload));
    if (!secPayload)
    {
        OIC_LOG(ERROR, TAG, "Failed to memory allocation");
        return OC_STACK_NO_MEMORY;
    }

    otmCtx->selectedDeviceInfo->doxm->owned = true;

    secPayload->base.type = PAYLOAD_TYPE_SECURITY;
    OCStackResult res = DoxmToCBORPayload(otmCtx->selectedDeviceInfo->doxm,
            &secPayload->securityData, &secPayload->payloadSize);
    if (OC_STACK_OK != res && NULL == secPayload->securityData)
    {
        OCPayloadDestroy((OCPayload *)secPayload);
        OIC_LOG(ERROR, TAG, "Error while converting doxm bin to json");
        return OC_STACK_INVALID_PARAM;
    }

    OCCallbackData cbData;
    cbData.cb = &OwnershipInformationHandler;
    cbData.context = (void *)otmCtx;
    cbData.cd = NULL;

    res = OCDoResource(NULL, OC_REST_PUT, query, 0, (OCPayload*)secPayload,
                       deviceInfo->connType, OC_LOW_QOS, &cbData, NULL, 0);
    if (res != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "OCStack resource error");
    }

    OIC_LOG(DEBUG, TAG, "OUT PutOwnershipInformation");

    return res;
}

static OCStackResult PutUpdateOperationMode(OTMContext_t* otmCtx)
{
    OIC_LOG(DEBUG, TAG, "IN PutUpdateOperationMode");

    if(!otmCtx || !otmCtx->selectedDeviceInfo)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OCProvisionDev_t* deviceInfo = otmCtx->selectedDeviceInfo;
    char query[MAX_URI_LENGTH + MAX_QUERY_LENGTH] = {0};
    if(!PMGenerateQuery(false,
                        deviceInfo->endpoint.addr, deviceInfo->endpoint.port,
                        deviceInfo->connType,
                        query, sizeof(query), OIC_RSRC_PSTAT_URI))
    {
        OIC_LOG(ERROR, TAG, "PutUpdateOperationMode : Failed to generate query");
        return OC_STACK_ERROR;
    }
    OIC_LOG_V(DEBUG, TAG, "Query=%s", query);

    OCSecurityPayload* secPayload = (OCSecurityPayload*)OICCalloc(1, sizeof(OCSecurityPayload));
    if(!secPayload)
    {
        OIC_LOG(ERROR, TAG, "Failed to memory allocation");
        return OC_STACK_NO_MEMORY;
    }
    secPayload->base.type = PAYLOAD_TYPE_SECURITY;
    OCStackResult res = PstatToCBORPayload(deviceInfo->pstat, &secPayload->securityData,
                                           &secPayload->payloadSize);
   if (OC_STACK_OK != res)
    {
        OCPayloadDestroy((OCPayload *)secPayload);
        OIC_LOG(ERROR, TAG, "Error while converting pstat to cbor.");
        return OC_STACK_INVALID_PARAM;
    }

    OCCallbackData cbData;
    cbData.cb = &OperationModeUpdateHandler;
    cbData.context = (void *)otmCtx;
    cbData.cd = NULL;
    res = OCDoResource(NULL, OC_REST_PUT, query, 0, (OCPayload *)secPayload,
                       deviceInfo->connType, OC_LOW_QOS, &cbData, NULL, 0);
    if (res != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "OCStack resource error");
    }

    OIC_LOG(DEBUG, TAG, "OUT PutUpdateOperationMode");

    return res;
}

static OCStackResult StartOwnershipTransfer(void* ctx, OCProvisionDev_t* selectedDevice)
{
    OIC_LOG(INFO, TAG, "IN StartOwnershipTransfer");
    OTMContext_t* otmCtx = (OTMContext_t*)ctx;
    otmCtx->selectedDeviceInfo = selectedDevice;

    //Set to the lowest level OxM, and then find more higher level OxM.
    OCStackResult res = SelectProvisioningMethod(selectedDevice->doxm->oxm,
                                                 selectedDevice->doxm->oxmLen,
                                                 &selectedDevice->doxm->oxmSel);
    if(OC_STACK_OK != res)
    {
        OIC_LOG(ERROR, TAG, "Failed to select the provisioning method");
        SetResult(otmCtx, res);
        return res;
    }
    OIC_LOG_V(DEBUG, TAG, "Selected provisoning method = %d", selectedDevice->doxm->oxmSel);

    //Send Req: PUT /oic/sec/doxm [{..."OxmSel" :g_OTMDatas[Index of Selected OxM].OXMString,...}]
    res = PutOwnerTransferModeToResource(otmCtx);
    if(OC_STACK_OK != res)
    {
        OIC_LOG(WARNING, TAG, "Failed to select the provisioning method");
        SetResult(otmCtx, res);
        return res;
    }

    //Register DTLS event handler to catch the dtls event while handshake
    if(CA_STATUS_OK != CARegisterDTLSHandshakeCallback(DTLSHandshakeCB))
    {
        OIC_LOG(WARNING, TAG, "StartOwnershipTransfer : Failed to register DTLS handshake callback.");
    }

    OIC_LOG(INFO, TAG, "OUT StartOwnershipTransfer");

    return res;

}

OCStackResult OTMSetOwnershipTransferCallbackData(OicSecOxm_t oxmType, OTMCallbackData_t* data)
{
    OIC_LOG(DEBUG, TAG, "IN OTMSetOwnerTransferCallbackData");

    if(!data)
    {
        OIC_LOG(ERROR, TAG, "OTMSetOwnershipTransferCallbackData : Invalid parameters");
        return OC_STACK_INVALID_PARAM;
    }
    if(oxmType >= OIC_OXM_COUNT)
    {
        OIC_LOG(INFO, TAG, "Unknow ownership transfer method");
        return OC_STACK_INVALID_PARAM;
    }

    g_OTMDatas[oxmType].loadSecretCB= data->loadSecretCB;
    g_OTMDatas[oxmType].createSecureSessionCB = data->createSecureSessionCB;
    g_OTMDatas[oxmType].createSelectOxmPayloadCB = data->createSelectOxmPayloadCB;
    g_OTMDatas[oxmType].createOwnerTransferPayloadCB = data->createOwnerTransferPayloadCB;

    OIC_LOG(DEBUG, TAG, "OUT OTMSetOwnerTransferCallbackData");

    return OC_STACK_OK;
}

/**
 * NOTE : Unowned discovery should be done before performing OTMDoOwnershipTransfer
 */
OCStackResult OTMDoOwnershipTransfer(void* ctx,
                                     OCProvisionDev_t *selectedDevicelist,
                                     OCProvisionResultCB resultCallback)
{
    OIC_LOG(DEBUG, TAG, "IN OTMDoOwnershipTransfer");

    if (NULL == selectedDevicelist)
    {
        return OC_STACK_INVALID_PARAM;
    }
    if (NULL == resultCallback)
    {
        return OC_STACK_INVALID_CALLBACK;
    }

    OTMContext_t* otmCtx = (OTMContext_t*)OICCalloc(1,sizeof(OTMContext_t));
    if(!otmCtx)
    {
        OIC_LOG(ERROR, TAG, "Failed to create OTM Context");
        return OC_STACK_NO_MEMORY;
    }
    otmCtx->ctxResultCallback = resultCallback;
    otmCtx->ctxHasError = false;
    otmCtx->userCtx = ctx;
    OCProvisionDev_t* pCurDev = selectedDevicelist;

    //Counting number of selected devices.
    otmCtx->ctxResultArraySize = 0;
    while(NULL != pCurDev)
    {
        otmCtx->ctxResultArraySize++;
        pCurDev = pCurDev->next;
    }

    otmCtx->ctxResultArray =
        (OCProvisionResult_t*)OICCalloc(otmCtx->ctxResultArraySize, sizeof(OCProvisionResult_t));
    if(NULL == otmCtx->ctxResultArray)
    {
        OIC_LOG(ERROR, TAG, "OTMDoOwnershipTransfer : Failed to memory allocation");
        OICFree(otmCtx);
        return OC_STACK_NO_MEMORY;
    }
    pCurDev = selectedDevicelist;

    OCStackResult res = OC_STACK_OK;
    //Fill the device UUID for result array.
    for(size_t devIdx = 0; devIdx < otmCtx->ctxResultArraySize; devIdx++)
    {
        //Checking duplication of Device ID.
        bool isDuplicate = true;
        res = PDMIsDuplicateDevice(&pCurDev->doxm->deviceID, &isDuplicate);
        if (OC_STACK_OK != res)
        {
            goto error;
        }
        if (isDuplicate)
        {
            OIC_LOG(ERROR, TAG, "OTMDoOwnershipTransfer : Device ID is duplicated");
            res = OC_STACK_INVALID_PARAM;
            goto error;
        }
        memcpy(otmCtx->ctxResultArray[devIdx].deviceId.id,
               pCurDev->doxm->deviceID.id,
               UUID_LENGTH);
        otmCtx->ctxResultArray[devIdx].res = OC_STACK_CONTINUE;
        pCurDev = pCurDev->next;
    }

    StartOwnershipTransfer(otmCtx, selectedDevicelist);

    OIC_LOG(DEBUG, TAG, "OUT OTMDoOwnershipTransfer");
    return OC_STACK_OK;

error:
    OICFree(otmCtx->ctxResultArray);
    OICFree(otmCtx);
    return res;
}

OCStackResult PutProvisioningStatus(OTMContext_t* otmCtx)
{
    OIC_LOG(INFO, TAG, "IN PutProvisioningStatus");

    if(!otmCtx || !otmCtx->selectedDeviceInfo)
    {
        OIC_LOG(ERROR, TAG, "OTMContext is NULL");
        return OC_STACK_INVALID_PARAM;
    }

    //Change the TAKE_OWNER bit of CM to 0.
    otmCtx->selectedDeviceInfo->pstat->cm &= (~TAKE_OWNER);

    OCSecurityPayload *secPayload = (OCSecurityPayload *)OICCalloc(1, sizeof(OCSecurityPayload));
    if (!secPayload)
    {
        OIC_LOG(ERROR, TAG, "Failed to memory allocation");
        return OC_STACK_NO_MEMORY;
    }
    secPayload->base.type = PAYLOAD_TYPE_SECURITY;
    if (OC_STACK_OK != PstatToCBORPayload(otmCtx->selectedDeviceInfo->pstat,
            &secPayload->securityData, &secPayload->payloadSize))
    {
        OCPayloadDestroy((OCPayload *)secPayload);
        return OC_STACK_INVALID_JSON;
    }
    OIC_LOG(DEBUG, TAG, "Created payload for chage to Provisiong state");
    OIC_LOG_BUFFER(DEBUG, TAG, secPayload->securityData, secPayload->payloadSize);

    char query[MAX_URI_LENGTH + MAX_QUERY_LENGTH] = {0};
    if(!PMGenerateQuery(true,
                        otmCtx->selectedDeviceInfo->endpoint.addr,
                        otmCtx->selectedDeviceInfo->securePort,
                        otmCtx->selectedDeviceInfo->connType,
                        query, sizeof(query), OIC_RSRC_PSTAT_URI))
    {
        OIC_LOG(ERROR, TAG, "PutProvisioningStatus : Failed to generate query");
        return OC_STACK_ERROR;
    }
    OIC_LOG_V(DEBUG, TAG, "Query=%s", query);

    OCCallbackData cbData = {.context=NULL, .cb=NULL, .cd=NULL};
    cbData.cb = &ProvisioningStatusHandler;
    cbData.context = (void*)otmCtx;
    cbData.cd = NULL;
    OCStackResult ret = OCDoResource(NULL, OC_REST_PUT, query, 0, (OCPayload*)secPayload,
            otmCtx->selectedDeviceInfo->connType, OC_HIGH_QOS, &cbData, NULL, 0);
    OIC_LOG_V(INFO, TAG, "OCDoResource returned: %d",ret);
    if (ret != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "OCStack resource error");
    }

    OIC_LOG(INFO, TAG, "OUT PutProvisioningStatus");

    return ret;
}

OCStackResult PutNormalOperationStatus(OTMContext_t* otmCtx)
{
    OIC_LOG(INFO, TAG, "IN PutNormalOperationStatus");

    if(!otmCtx || !otmCtx->selectedDeviceInfo)
    {
        OIC_LOG(ERROR, TAG, "OTMContext is NULL");
        return OC_STACK_INVALID_PARAM;
    }

    //Set isop to true.
    otmCtx->selectedDeviceInfo->pstat->isOp = true;

    OCSecurityPayload *secPayload = (OCSecurityPayload *)OICCalloc(1, sizeof(OCSecurityPayload));
    if (!secPayload)
    {
        OIC_LOG(ERROR, TAG, "Failed to memory allocation");
        return OC_STACK_NO_MEMORY;
    }
    secPayload->base.type = PAYLOAD_TYPE_SECURITY;
    if (OC_STACK_OK != PstatToCBORPayload(otmCtx->selectedDeviceInfo->pstat,
            &secPayload->securityData, &secPayload->payloadSize))
    {
        OCPayloadDestroy((OCPayload *)secPayload);
        return OC_STACK_INVALID_JSON;
    }
    OIC_LOG(DEBUG, TAG, "Created payload for chage to Provisiong state");
    OIC_LOG_BUFFER(DEBUG, TAG, secPayload->securityData, secPayload->payloadSize);

    char query[MAX_URI_LENGTH + MAX_QUERY_LENGTH] = {0};
    if(!PMGenerateQuery(true,
                        otmCtx->selectedDeviceInfo->endpoint.addr,
                        otmCtx->selectedDeviceInfo->securePort,
                        otmCtx->selectedDeviceInfo->connType,
                        query, sizeof(query), OIC_RSRC_PSTAT_URI))
    {
        OIC_LOG(ERROR, TAG, "PutNormalOperationStatus : Failed to generate query");
        return OC_STACK_ERROR;
    }
    OIC_LOG_V(DEBUG, TAG, "Query=%s", query);

    OCCallbackData cbData = {.context=NULL, .cb=NULL, .cd=NULL};
    cbData.cb = &ReadyForNomalStatusHandler;
    cbData.context = (void*)otmCtx;
    cbData.cd = NULL;
    OCStackResult ret = OCDoResource(NULL, OC_REST_PUT, query, 0, (OCPayload*)secPayload,
            otmCtx->selectedDeviceInfo->connType, OC_HIGH_QOS, &cbData, NULL, 0);
    OIC_LOG_V(INFO, TAG, "OCDoResource returned: %d",ret);
    if (ret != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "OCStack resource error");
    }

    OIC_LOG(INFO, TAG, "OUT PutNormalOperationStatus");

    return ret;
}
