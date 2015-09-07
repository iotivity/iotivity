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
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#include "ocprovisioningmanager.h"
#include "secureresourceprovider.h"
#include "logger.h"
#include "oic_malloc.h"
#include "aclresource.h"
#include "pstatresource.h"
#include "srmresourcestrings.h"
#include "credresource.h"
#include "doxmresource.h"
#include "credentialgenerator.h"
#include "cainterface.h"
#include "cJSON.h"
#include "pmtypes.h"
#include "pmutility.h"
#include "provisioningdatabasemanager.h"

#define SRP_MAX_URI_LENGTH 512
#define TAG "SRPAPI"

/**
 * Macro to verify argument is not equal to NULL.
 * eg: VERIFY_NON_NULL(TAG, ptrData, ERROR,OC_STACK_ERROR);
 */
#define VERIFY_NON_NULL(tag, arg, logLevel, retValue) { if (NULL == (arg)) \
            { OC_LOG((logLevel), tag, (#arg " is NULL")); return retValue; } }

/**
 * Macro to verify success of operation.
 * eg: VERIFY_SUCCESS(TAG, OC_STACK_OK == foo(), ERROR, OC_STACK_ERROR);
 */
#define VERIFY_SUCCESS(tag, op, logLevel, retValue) { if (!(op)) \
            {OC_LOG((logLevel), tag, (#op " failed!!")); return retValue;} }

/**
 * Structure to carry credential data to callback.
 */
typedef struct CredentialData CredentialData_t;
struct CredentialData
{
    void *ctx;                                  /**< Pointer to user context.**/
    const OCProvisionDev_t *deviceInfo1;        /**< Pointer to OCProvisionDev_t.**/
    const OCProvisionDev_t *deviceInfo2;        /**< Pointer to OCProvisionDev_t.**/
    OicSecCred_t *credInfo;                     /**< Pointer to OicSecCred_t.**/
    OicSecCred_t *credInfoFirst;                /**< Pointer to OicSecCred_t.**/
    OCProvisionResultCB resultCallback;         /**< Pointer to result callback.**/
    OCProvisionResult_t *resArr;                /**< Result array.**/
    int numOfResults;                           /**< Number of results in result array.**/
};

/**
 * Structure to carry ACL provision API data to callback.
 */
typedef struct ACLData ACLData_t;
struct ACLData
{
    void *ctx;                                  /**< Pointer to user context.**/
    const OCProvisionDev_t *deviceInfo;         /**< Pointer to PMDevInfo_t.**/
    OCProvisionResultCB resultCallback;         /**< Pointer to result callback.**/
    OCProvisionResult_t *resArr;                /**< Result array.**/
    int numOfResults;                           /**< Number of results in result array.**/
};

/**
 * Function prototype
 */
static OCStackResult provisionCredentials(const OicSecCred_t *cred,
        const OCProvisionDev_t *deviceInfo, CredentialData_t *credData,
        OCClientResponseHandler responseHandler);


/**
 * Internal function to update result in result array.
 */
static void registerResultForCredProvisioning(CredentialData_t *credData,
                                              OCStackResult stackresult, int cause)
{

   OC_LOG_V(INFO,TAG,"value of credData->numOfResults is %d",credData->numOfResults);
   if(1 == cause)
   {
       memcpy(credData->resArr[(credData->numOfResults)].deviceId.id,
              credData->deviceInfo1->doxm->deviceID.id,UUID_LENGTH);
   }
   else
   {
       memcpy(credData->resArr[(credData->numOfResults)].deviceId.id,
              credData->deviceInfo2->doxm->deviceID.id,UUID_LENGTH);
   }
   credData->resArr[(credData->numOfResults)].res = stackresult;
   ++(credData->numOfResults);
}

/**
 * Callback handler for handling callback of provisioning device 2.
 *
 * @param[in] ctx             ctx value passed to callback from calling function.
 * @param[in] UNUSED          handle to an invocation
 * @param[in] clientResponse  Response from queries to remote servers.
 * @return  OC_STACK_DELETE_TRANSACTION to delete the transaction
 *          and  OC_STACK_KEEP_TRANSACTION to keep it.
 */
static OCStackApplicationResult provisionCredentialCB2(void *ctx, OCDoHandle UNUSED,
                                                       OCClientResponse *clientResponse)
{
    VERIFY_NON_NULL(TAG, ctx, ERROR, OC_STACK_DELETE_TRANSACTION);
    CredentialData_t *credData = (CredentialData_t *) ctx;
    (void)UNUSED;

    OCProvisionResultCB resultCallback = credData->resultCallback;
    OC_LOG(INFO, TAG, "provisionCredentialCB2 called");
    if (clientResponse)
    {
        if(OC_STACK_RESOURCE_CREATED == clientResponse->result)
        {
            registerResultForCredProvisioning(credData, OC_STACK_RESOURCE_CREATED, 2);
            OCStackResult res =  PDMLinkDevices(&credData->deviceInfo1->doxm->deviceID,
                    &credData->deviceInfo2->doxm->deviceID);
            if (OC_STACK_OK != res)
            {
                OC_LOG(ERROR, TAG, "Error occured on PDMlinkDevices");
                return OC_STACK_DELETE_TRANSACTION;
            }
            OC_LOG(INFO, TAG, "Link created successfully");

            ((OCProvisionResultCB)(resultCallback))(credData->ctx, credData->numOfResults,
                                                    credData->resArr,
                                                    false);
             OICFree(credData->resArr);
             OICFree(credData);
             return OC_STACK_DELETE_TRANSACTION;
        }

    }
    OC_LOG(INFO, TAG, "provisionCredentialCB2 received Null clientResponse");
    registerResultForCredProvisioning(credData, OC_STACK_ERROR, 2);
    ((OCProvisionResultCB)(resultCallback))(credData->ctx, credData->numOfResults,
                                            credData->resArr,
                                            true);
    OICFree(credData->resArr);
    OICFree(credData);
    return OC_STACK_DELETE_TRANSACTION;
}

/**
 * Callback handler for handling callback of provisioning device 1.
 *
 * @param[in] ctx             ctx value passed to callback from calling function.
 * @param[in] UNUSED          handle to an invocation
 * @param[in] clientResponse  Response from queries to remote servers.
 * @return  OC_STACK_DELETE_TRANSACTION to delete the transaction
 *          and  OC_STACK_KEEP_TRANSACTION to keep it.
 */
static OCStackApplicationResult provisionCredentialCB1(void *ctx, OCDoHandle UNUSED,
                                                       OCClientResponse *clientResponse)
{
    VERIFY_NON_NULL(TAG, ctx, ERROR, OC_STACK_DELETE_TRANSACTION);
    (void)UNUSED;
    CredentialData_t* credData = (CredentialData_t*) ctx;
    OICFree(credData->credInfoFirst);
    const OCProvisionDev_t *deviceInfo = credData->deviceInfo2;
    OicSecCred_t *credInfo = credData->credInfo;
    const OCProvisionResultCB resultCallback = credData->resultCallback;
    if (clientResponse)
    {
        if (OC_STACK_RESOURCE_CREATED == clientResponse->result)
        {
            // send credentials to second device
            registerResultForCredProvisioning(credData, OC_STACK_RESOURCE_CREATED,1);
            OCStackResult res = provisionCredentials(credInfo, deviceInfo, credData,
                    provisionCredentialCB2);
            DeleteCredList(credInfo);
            if (OC_STACK_OK != res)
            {
                registerResultForCredProvisioning(credData, res,2);
                ((OCProvisionResultCB)(resultCallback))(credData->ctx, credData->numOfResults,
                                                        credData->resArr,
                                                        true);
                OICFree(credData->resArr);
                OICFree(credData);
                credData = NULL;
            }
        }
        else
        {
            registerResultForCredProvisioning(credData, OC_STACK_ERROR,1);
            ((OCProvisionResultCB)(resultCallback))(credData->ctx, credData->numOfResults,
                                                    credData->resArr,
                                                    true);
            OICFree(credData->resArr);
            OICFree(credData);
            credData = NULL;
        }
    }
    else
    {
        OC_LOG(INFO, TAG, "provisionCredentialCB received Null clientResponse for first device");
        registerResultForCredProvisioning(credData, OC_STACK_ERROR,1);
       ((OCProvisionResultCB)(resultCallback))(credData->ctx, credData->numOfResults,
                                                     credData->resArr,
                                                     true);
        DeleteCredList(credInfo);
        OICFree(credData->resArr);
        OICFree(credData);
        credData = NULL;
    }
    return OC_STACK_DELETE_TRANSACTION;
}



/**
 * Internal function for handling credential generation and sending credential to resource server.
 *
 * @param[in] cred Instance of cred resource.
 * @param[in] deviceInfo information about device to which credential is to be provisioned.
 * @param[in] responseHandler callbak called by OC stack when request API receives response.
 * @return  OC_STACK_OK in case of success and other value otherwise.
 */
static OCStackResult provisionCredentials(const OicSecCred_t *cred,
        const OCProvisionDev_t *deviceInfo, CredentialData_t *credData,
        OCClientResponseHandler responseHandler)
{
    OCSecurityPayload* secPayload = (OCSecurityPayload*)OICCalloc(1, sizeof(OCSecurityPayload));
    if(!secPayload)
    {
        OC_LOG(ERROR, TAG, "Failed to memory allocation");
        return OC_STACK_NO_MEMORY;
    }
    secPayload->base.type = PAYLOAD_TYPE_SECURITY;
    secPayload->securityData = BinToCredJSON(cred);
    if(NULL == secPayload->securityData)
    {
        OICFree(secPayload);
        OC_LOG(ERROR, TAG, "Failed to BinToCredJSON");
        return OC_STACK_NO_MEMORY;
    }

    OC_LOG_V(INFO, TAG, "Credential for provisioning : %s",secPayload->securityData);
    char query[MAX_URI_LENGTH + MAX_QUERY_LENGTH] = {0};
    if(!PMGenerateQuery(true,
                        deviceInfo->endpoint.addr,
                        deviceInfo->securePort,
                        deviceInfo->connType,
                        query, sizeof(query), OIC_RSRC_CRED_URI))
    {
        OC_LOG(ERROR, TAG, "DeviceDiscoveryHandler : Failed to generate query");
        return OC_STACK_ERROR;
    }
    OC_LOG_V(DEBUG, TAG, "Query=%s", query);

    OCCallbackData cbData = {.context=NULL, .cb=NULL, .cd=NULL};
    cbData.cb = responseHandler;
    cbData.context = (void *) credData;
    cbData.cd = NULL;

    OCDoHandle handle = NULL;
    OCMethod method = OC_REST_POST;
    OCStackResult ret = OCDoResource(&handle, method, query, 0, (OCPayload*)secPayload,
            deviceInfo->connType, OC_HIGH_QOS, &cbData, NULL, 0);
    OC_LOG_V(INFO, TAG, "OCDoResource::Credential provisioning returned : %d",ret);
    if (ret != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, "OCStack resource error");
        return ret;
    }
    return OC_STACK_OK;
}

OCStackResult SRPProvisionCredentials(void *ctx, OicSecCredType_t type, size_t keySize,
                                      const OCProvisionDev_t *pDev1,
                                      const OCProvisionDev_t *pDev2,
                                      OCProvisionResultCB resultCallback)
{
    VERIFY_NON_NULL(TAG, pDev1, ERROR,  OC_STACK_INVALID_PARAM);
    VERIFY_NON_NULL(TAG, pDev2, ERROR,  OC_STACK_INVALID_PARAM);
    VERIFY_NON_NULL(TAG, resultCallback, ERROR,  OC_STACK_INVALID_CALLBACK);

    if (!(keySize == OWNER_PSK_LENGTH_128 || keySize == OWNER_PSK_LENGTH_256))
    {
        OC_LOG(INFO, TAG, "Invalid key size");
        return OC_STACK_INVALID_PARAM;
    }

    OC_LOG(INFO, TAG, "In SRPProvisionCredentials");

    OicUuid_t provTooldeviceID =   {{0,}};
    if (OC_STACK_OK != GetDoxmDeviceID(&provTooldeviceID))
    {
        OC_LOG(ERROR, TAG, "Error while retrieving provisioning tool's device ID");
        return OC_STACK_ERROR;
    }
    OC_LOG(INFO, TAG, "retrieved deviceid");
    switch (type)
    {
        case SYMMETRIC_PAIR_WISE_KEY:
        {
            const OCProvisionDev_t *firstDevice = pDev1;
            const OCProvisionDev_t *secondDevice = pDev2;

            OicSecCred_t *firstCred = NULL;
            OicSecCred_t *secondCred = NULL;
            OCStackResult res = PMGeneratePairWiseCredentials(type, keySize, &provTooldeviceID,
                    &firstDevice->doxm->deviceID, &secondDevice->doxm->deviceID,
                    &firstCred, &secondCred);
            VERIFY_SUCCESS(TAG, (res==OC_STACK_OK), ERROR, OC_STACK_ERROR);
            OC_LOG(INFO, TAG, "Credentials generated successfully");
            CredentialData_t *credData = (CredentialData_t *) OICMalloc(sizeof(CredentialData_t));
            if (NULL == credData)
            {
                OC_LOG(ERROR, TAG, "Memory allocation problem");
                return OC_STACK_NO_MEMORY;
            }
            memset(credData, 0x00, sizeof(CredentialData_t));
            credData->deviceInfo1 = firstDevice;
            credData->deviceInfo2 = secondDevice;
            credData->credInfo = secondCred;
            credData->ctx = ctx;
            credData->credInfoFirst = firstCred;
            credData->numOfResults = 0;
            credData->resultCallback = resultCallback;
            // first call to provision creds to device1.
            // second call to provision creds to device2.
            int noOfRiCalls = 2;
            credData->resArr =
                (OCProvisionResult_t*)OICMalloc(sizeof(OCProvisionResult_t) * noOfRiCalls);
            if (NULL == credData->resArr)
            {
                OC_LOG(ERROR, TAG, "Memory allocation problem");
                return OC_STACK_NO_MEMORY;
            }
            memset(credData->resArr, 0x00, sizeof(sizeof(OCProvisionResult_t)*noOfRiCalls));
            res = provisionCredentials(firstCred, firstDevice, credData, &provisionCredentialCB1);
            if (OC_STACK_OK != res)
            {
                DeleteCredList(firstCred);
                DeleteCredList(secondCred);
                OICFree(credData->resArr);
                OICFree(credData);
            }
            OC_LOG_V(INFO, TAG, "provisionCredentials returned: %d",res);
            VERIFY_SUCCESS(TAG, (res==OC_STACK_OK), ERROR, OC_STACK_ERROR);
            return res;
        }
        default:
        {
            OC_LOG(ERROR, TAG, "Invalid option.");
            return OC_STACK_INVALID_PARAM;
        }
    }
    return OC_STACK_ERROR;
}

/**
 * Internal Function to store results in result array during ACL provisioning.
 */
static void registerResultForACLProvisioning(ACLData_t *aclData,
                                             OCStackResult stackresult)
{
   OC_LOG_V(INFO, TAG, "Inside registerResultForACLProvisioning aclData->numOfResults is %d\n",
                       aclData->numOfResults);
   memcpy(aclData->resArr[(aclData->numOfResults)].deviceId.id,
          aclData->deviceInfo->doxm->deviceID.id, UUID_LENGTH);
   aclData->resArr[(aclData->numOfResults)].res = stackresult;
   ++(aclData->numOfResults);
}

/**
 * Callback handler of SRPProvisionACL.
 *
 * @param[in] ctx             ctx value passed to callback from calling function.
 * @param[in] UNUSED          handle to an invocation
 * @param[in] clientResponse  Response from queries to remote servers.
 * @return  OC_STACK_DELETE_TRANSACTION to delete the transaction
 *          and  OC_STACK_KEEP_TRANSACTION to keep it.
 */
static OCStackApplicationResult SRPProvisionACLCB(void *ctx, OCDoHandle UNUSED,
                                                  OCClientResponse *clientResponse)
{
    OC_LOG_V(INFO, TAG, "Inside SRPProvisionACLCB.");
    (void)UNUSED;
    VERIFY_NON_NULL(TAG, ctx, ERROR, OC_STACK_DELETE_TRANSACTION);
    ACLData_t *aclData = (ACLData_t*)ctx;
    OCProvisionResultCB resultCallback = aclData->resultCallback;

    if (clientResponse)
    {
        if(OC_STACK_RESOURCE_CREATED == clientResponse->result)
        {
            registerResultForACLProvisioning(aclData, OC_STACK_RESOURCE_CREATED);
            ((OCProvisionResultCB)(resultCallback))(aclData->ctx, aclData->numOfResults,
                                                    aclData->resArr,
                                                    false);
             OICFree(aclData->resArr);
             OICFree(aclData);
             return OC_STACK_DELETE_TRANSACTION;
        }
    }
    registerResultForACLProvisioning(aclData, OC_STACK_ERROR);
    ((OCProvisionResultCB)(resultCallback))(aclData->ctx, aclData->numOfResults,
                                            aclData->resArr,
                                            true);
    OC_LOG_V(ERROR, TAG, "SRPProvisionACLCB received Null clientResponse");
    OICFree(aclData->resArr);
    OICFree(aclData);
    return OC_STACK_DELETE_TRANSACTION;
}

OCStackResult SRPProvisionACL(void *ctx, const OCProvisionDev_t *selectedDeviceInfo,
        OicSecAcl_t *acl, OCProvisionResultCB resultCallback)
{
    VERIFY_NON_NULL(TAG, selectedDeviceInfo, ERROR,  OC_STACK_INVALID_PARAM);
    VERIFY_NON_NULL(TAG, acl, ERROR,  OC_STACK_INVALID_PARAM);
    VERIFY_NON_NULL(TAG, resultCallback, ERROR,  OC_STACK_INVALID_CALLBACK);

    OCSecurityPayload* secPayload = (OCSecurityPayload*)OICCalloc(1, sizeof(OCSecurityPayload));
    if(!secPayload)
    {
        OC_LOG(ERROR, TAG, "Failed to memory allocation");
        return OC_STACK_NO_MEMORY;
    }
    secPayload->base.type = PAYLOAD_TYPE_SECURITY;
    secPayload->securityData = BinToAclJSON(acl);
    if(NULL == secPayload->securityData)
    {
        OICFree(secPayload);
        OC_LOG(ERROR, TAG, "Failed to BinToAclJSON");
        return OC_STACK_NO_MEMORY;
    }
    OC_LOG_V(INFO, TAG, "ACL : %s", secPayload->securityData);

    char query[MAX_URI_LENGTH + MAX_QUERY_LENGTH] = {0};
    if(!PMGenerateQuery(true,
                        selectedDeviceInfo->endpoint.addr,
                        selectedDeviceInfo->securePort,
                        selectedDeviceInfo->connType,
                        query, sizeof(query), OIC_RSRC_ACL_URI))
    {
        OC_LOG(ERROR, TAG, "DeviceDiscoveryHandler : Failed to generate query");
        return OC_STACK_ERROR;
    }
    OC_LOG_V(DEBUG, TAG, "Query=%s", query);

    OCCallbackData cbData =  {.context=NULL, .cb=NULL, .cd=NULL};
    cbData.cb = &SRPProvisionACLCB;
    ACLData_t *aclData = (ACLData_t *) OICMalloc(sizeof(ACLData_t));
    if (aclData == NULL)
    {
        OICFree(secPayload);
        OC_LOG(ERROR, TAG, "Unable to allocate memory");
        return OC_STACK_NO_MEMORY;
    }
    memset(aclData, 0x00, sizeof(ACLData_t));
    aclData->deviceInfo = selectedDeviceInfo;
    aclData->resultCallback = resultCallback;
    aclData->numOfResults=0;
    aclData->ctx = ctx;
    // call to provision ACL to device1.
    int noOfRiCalls = 1;
    aclData->resArr = (OCProvisionResult_t*)OICMalloc(sizeof(OCProvisionResult_t)*noOfRiCalls);
    if (aclData->resArr == NULL)
    {
        OICFree(secPayload);
        OC_LOG(ERROR, TAG, "Unable to allocate memory");
        return OC_STACK_NO_MEMORY;
    }
    memset(aclData->resArr, 0x00, sizeof(sizeof(OCProvisionResult_t)*noOfRiCalls));
    cbData.context = (void *)aclData;
    cbData.cd = NULL;
    OCMethod method = OC_REST_POST;
    OCDoHandle handle = NULL;
    OC_LOG(DEBUG, TAG, "Sending ACL info to resource server");
    OCStackResult ret = OCDoResource(&handle, method, query,
            &selectedDeviceInfo->endpoint, (OCPayload*)secPayload,
            selectedDeviceInfo->connType, OC_HIGH_QOS, &cbData, NULL, 0);
    if (ret != OC_STACK_OK)
    {
        OICFree(aclData->resArr);
        OICFree(aclData);
    }
    VERIFY_SUCCESS(TAG, (OC_STACK_OK == ret), ERROR, OC_STACK_ERROR);
    return OC_STACK_OK;
}
