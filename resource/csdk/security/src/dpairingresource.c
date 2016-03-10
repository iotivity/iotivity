/* *****************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
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

#include <stdlib.h>
#include <string.h>
#include "ocstack.h"
#include "logger.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "cJSON.h"
#include "base64.h"
#include "resourcemanager.h"
#include "dpairingresource.h"
#include "psinterface.h"
#include "utlist.h"
#include "srmresourcestrings.h"
#include "cainterface.h"
#include "doxmresource.h"
#include "pconfresource.h"
#include "credresource.h"
#include "aclresource.h"
#include "srmutility.h"
#include "ocserverrequest.h"
#include <stdlib.h>
#ifdef WITH_ARDUINO
#include <string.h>
#else
#include <strings.h>
#endif

#ifdef __WITH_DTLS__
#include "global.h"
#endif

#define TAG  "SRM-DPAIRING"


static OicSecDpairing_t     *gDpair = NULL;
static OCResourceHandle   gDpairHandle = NULL;
static OicSecDpairing_t      gDefaultDpair =
{
    PRM_NOT_ALLOWED,       /* OicSecPrm_t spm */
    {.id = {0}},                   /* OicUuid_t pdeviceID */
    {.id = {0}},                   /* OicUuid_t rowner */
};

void DeleteDpairingBinData(OicSecDpairing_t* dpair)
{
    if (dpair)
    {
        //Clean dpairing itself
        OICFree(dpair);
    }
}

/**
 * Get the default value.
 * @retval  the gDefaultDpair pointer;
 */
static OicSecDpairing_t* GetDpairingDefault()
{
    OIC_LOG (DEBUG, TAG, "GetDpairingDefault");

    return &gDefaultDpair;
}

/**
 * This method is used by SRM to retrieve Dpairing resource data..
 */
void SetDpairingResourceOwner(OicUuid_t *rowner)
{
    OIC_LOG (DEBUG, TAG, "SetDpairingResourceOwner");
    if (gDpair)
    {
        memcpy(&gDpair->rowner, rowner, sizeof(OicUuid_t));
    }
}

#ifdef __WITH_DTLS__
/**
 * Function to save PairingPSK.
 *
 * @param[in] endpoint   current endpoint.
 * @param[in] peerDevID   peer device indentitiy.
 * @param[in] isPairingServer   indicate if it generates PairingPSK for server or client.
 *
 * @return  OC_STACK_OK on success
 */
OCStackResult SavePairingPSK(OCDevAddr *endpoint,
            OicUuid_t *peerDevID, OicUuid_t *owner, bool isPairingServer)
{
    OIC_LOG(DEBUG, TAG, "IN SavePairingPSK");

    if(NULL == endpoint || NULL == peerDevID || NULL == owner)
    {
        OIC_LOG_V(ERROR, TAG, "Invalid Input parameters in [%s]\n", __FUNCTION__);
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult res = OC_STACK_ERROR;

    OicUuid_t ptDeviceID = {.id={0}};
    if (OC_STACK_OK != GetDoxmDeviceID(&ptDeviceID))
    {
        OIC_LOG(ERROR, TAG, "Error while retrieving provisioning tool's device ID");
        return res;
    }

    uint8_t pairingPSK[OWNER_PSK_LENGTH_128] = {0};

    //Generating PairingPSK using OwnerPSK scheme
    CAResult_t pskRet = CAGenerateOwnerPSK((const CAEndpoint_t *)endpoint,
            (uint8_t *)OIC_RSRC_TYPE_SEC_DPAIRING,
            strlen(OIC_RSRC_TYPE_SEC_DPAIRING),
            (isPairingServer ? ptDeviceID.id : peerDevID->id), sizeof(OicUuid_t), // server
            (isPairingServer ? peerDevID->id : ptDeviceID.id), sizeof(OicUuid_t), // client
            pairingPSK, OWNER_PSK_LENGTH_128);

    if (CA_STATUS_OK == pskRet)
    {
        OIC_LOG(INFO, TAG, "pairingPSK dump:\n");
        OIC_LOG_BUFFER(INFO, TAG, pairingPSK, OWNER_PSK_LENGTH_128);
        //Generating new credential for direct-pairing client
        size_t ownLen = 1;
        uint32_t outLen = 0;

        char base64Buff[B64ENCODE_OUT_SAFESIZE(sizeof(pairingPSK)) + 1] = {};
        B64Result b64Ret = b64Encode(pairingPSK, sizeof(pairingPSK), base64Buff, sizeof(base64Buff),
                &outLen);
        VERIFY_SUCCESS(TAG, B64_OK == b64Ret, ERROR);

        OicSecCred_t *cred = GenerateCredential(peerDevID,
                SYMMETRIC_PAIR_WISE_KEY, NULL,
                base64Buff, ownLen, owner);
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

    OIC_LOG(DEBUG, TAG, "OUT SavePairingPSK");
exit:
    return res;
}
#endif // __WITH_DTLS__

/*
 * This internal method converts DPairing data into JSON format.
 * Does not error-check here, but check it in caller
 *
 * Note: Caller needs to invoke 'free' when finished done using
 * return string.
 */
char * BinToDpairingJSON(const OicSecDpairing_t * dpair)
{
    OIC_LOG(DEBUG, TAG, "BinToDpairingJSON() IN");

    if (NULL == dpair)
    {
        return NULL;
    }

    char *jsonStr = NULL;
    cJSON *jsonDpair = NULL;
    char base64Buff[B64ENCODE_OUT_SAFESIZE(sizeof(((OicUuid_t*)0)->id)) + 1] = {};
    uint32_t outLen = 0;
    B64Result b64Ret = B64_OK;

    cJSON *jsonRoot = cJSON_CreateObject();
    VERIFY_NON_NULL(TAG, jsonRoot, ERROR);

    jsonDpair = cJSON_CreateObject();
    VERIFY_NON_NULL(TAG, jsonDpair, ERROR);
    cJSON_AddItemToObject(jsonRoot, OIC_JSON_DPAIRING_NAME, jsonDpair );

    //SPM -- Mandatory
    if(PRM_RANDOM_PIN >= dpair->spm) // don't need to check "PRM_NOT_ALLOWED <= dpair->spm" because of always true
    {
        cJSON_AddNumberToObject(jsonDpair, OIC_JSON_SPM_NAME, (int)dpair->spm);
    }

    //PDeviceID -- Mandatory
    //There may not be paired devices if it did not be received pairing request
    if ('\0' != (char)dpair->pdeviceID.id[0])
    {
        outLen = 0;
        b64Ret = b64Encode(dpair->pdeviceID.id, sizeof(dpair->pdeviceID.id), base64Buff,
                    sizeof(base64Buff), &outLen);
        VERIFY_SUCCESS(TAG, b64Ret == B64_OK, ERROR);
        cJSON_AddStringToObject(jsonDpair, OIC_JSON_PDEVICE_ID_NAME, base64Buff);
    }

    //ROwner -- Mandatory
    if ('\0' != (char)dpair->rowner.id[0])
    {
        outLen = 0;
        b64Ret = b64Encode(dpair->rowner.id, sizeof(dpair->rowner.id), base64Buff,
                    sizeof(base64Buff), &outLen);
        VERIFY_SUCCESS(TAG, b64Ret == B64_OK, ERROR);
        cJSON_AddStringToObject(jsonDpair, OIC_JSON_ROWNER_NAME, base64Buff);
    }


    jsonStr = cJSON_PrintUnformatted(jsonRoot);

exit:
    if (jsonRoot)
    {
        cJSON_Delete(jsonRoot);
    }
    return jsonStr;
}

/*
 * This internal method converts JSON Dpairing into binary Dpairing.
 * Does not error-check here, but check it in caller
 */
OicSecDpairing_t* JSONToDpairingBin(const char * jsonStr)
{
    OIC_LOG(DEBUG, TAG, "JSONToDpairingBin() IN");

    OCStackResult ret = OC_STACK_ERROR;
    OicSecDpairing_t *dpair =  NULL;
    cJSON *jsonRoot = NULL;
    cJSON *jsonDpair = NULL;
    cJSON *jsonObj = NULL;

    unsigned char base64Buff[sizeof(((OicUuid_t*)0)->id)] = {};
    uint32_t outLen = 0;
    B64Result b64Ret = B64_OK;


    VERIFY_NON_NULL(TAG, jsonStr, ERROR);

    jsonRoot = cJSON_Parse(jsonStr);
    VERIFY_NON_NULL(TAG, jsonRoot, ERROR);

    jsonDpair = cJSON_GetObjectItem(jsonRoot, OIC_JSON_DPAIRING_NAME);
    VERIFY_NON_NULL(TAG, jsonDpair, ERROR);

    dpair = (OicSecDpairing_t*)OICCalloc(1, sizeof(OicSecDpairing_t));
    VERIFY_NON_NULL(TAG, dpair, ERROR);

    //SPM -- Mandatory
    jsonObj = cJSON_GetObjectItem(jsonDpair, OIC_JSON_SPM_NAME);
    if (jsonObj && cJSON_Number == jsonObj->type)
    {
        dpair->spm = (OicSecPrm_t)jsonObj->valueint;
        OIC_LOG_V (DEBUG, TAG, "jsonObj->valueint = %d", jsonObj->valueint);
        OIC_LOG_V (DEBUG, TAG, "dpair->spm = %d", dpair->spm);

        // don't need to check "PRM_NOT_ALLOWED <= dpair->spm" because of always true
        VERIFY_SUCCESS(TAG, (PRM_RANDOM_PIN >= dpair->spm), ERROR);
    }
    else
    {
        dpair->spm = PRM_NOT_ALLOWED;
    }

    //PDeviceId -- Mandatory
    outLen = 0;
    jsonObj = cJSON_GetObjectItem(jsonDpair, OIC_JSON_PDEVICE_ID_NAME);
    if (jsonObj && cJSON_String == jsonObj->type)
    {
        b64Ret = b64Decode(jsonObj->valuestring, strlen(jsonObj->valuestring), base64Buff,
                sizeof(base64Buff), &outLen);
        VERIFY_SUCCESS(TAG, (b64Ret == B64_OK && outLen <= sizeof(dpair->pdeviceID.id)), ERROR);
        memcpy(dpair->pdeviceID.id, base64Buff, outLen);
    }
    else
    {
        memset(&dpair->pdeviceID, 0, sizeof(OicUuid_t));
    }

    // ROwner -- Mandatory
    outLen = 0;
    jsonObj = cJSON_GetObjectItem(jsonDpair, OIC_JSON_ROWNER_NAME);
    if (jsonObj && cJSON_String == jsonObj->type)
    {
        b64Ret = b64Decode(jsonObj->valuestring, strlen(jsonObj->valuestring), base64Buff,
                sizeof(base64Buff), &outLen);
        VERIFY_SUCCESS(TAG, (b64Ret == B64_OK && outLen <= sizeof(dpair->rowner.id)), ERROR);
        memcpy(dpair->rowner.id, base64Buff, outLen);
    }
    else
    {
        memset(&dpair->rowner, 0, sizeof(OicUuid_t));
    }

    ret = OC_STACK_OK;

exit:
    cJSON_Delete(jsonRoot);
    if (OC_STACK_OK != ret)
    {
        DeleteDpairingBinData(dpair);
        dpair = NULL;
    }

    OIC_LOG(DEBUG, TAG, "JSONToDpairingBin() OUT");
    return dpair;
}

/**
 * Function to handle the handshake result in Direct-Pairing.
 * This function will be invoked after DTLS handshake
 * @param   endPoint  [IN] The remote endpoint.
 * @param   errorInfo [IN] Error information from the endpoint.
 * @return  NONE
 */
void DPairingDTLSHandshakeCB(const CAEndpoint_t *endpoint, const CAErrorInfo_t *info)
{
    OIC_LOG_V(INFO, TAG, "IN DPairingDTLSHandshakeCB");

    if(gDpair && endpoint && info)
    {
        OIC_LOG_V(INFO, TAG, "Received status from remote device(%s:%d) : %d",
                 endpoint->addr, endpoint->port, info->result);

        if(CA_STATUS_OK == info->result)
        {
            OIC_LOG(INFO, TAG, "DPairingDTLSHandshakeCB - Connection success.");
        }
        else if(CA_DTLS_AUTHENTICATION_FAILURE == info->result)
        {
            OIC_LOG(INFO, TAG, "DPairingDTLSHandshakeCB - Authentication failed");

        }

#ifdef __WITH_DTLS__
        CARegisterDTLSHandshakeCallback(NULL);
#endif // __WITH_DTLS__

        // delete temporary key
        RemoveCredential(&gDpair->pdeviceID);
    }

    OIC_LOG_V(INFO, TAG, "OUT DPairingDTLSHandshakeCB");
}

static OCEntityHandlerResult HandleDpairingPostRequest (const OCEntityHandlerRequest * ehRequest)
{
    OIC_LOG (DEBUG, TAG, "Dpairing EntityHandle  processing POST request");
    OCEntityHandlerResult ehRet = OC_EH_ERROR;
    OicSecDpairing_t* newDpair = NULL;

    const OicSecPconf_t *pconf = GetPconfResourceData();
    if (true == pconf->edp)
    {
        // Convert JSON DPAIRING data into binary. This will also validate the DPAIRING data received.
        newDpair = JSONToDpairingBin(((OCSecurityPayload*)ehRequest->payload)->securityData);
    }
    else
    {
        OIC_LOG (DEBUG, TAG, "EDP == false : Direct-Pairing Disabled");
        ehRet = OC_EH_ERROR;
    }

    if (newDpair && false == IsPairedDevice(&newDpair->pdeviceID))
    {
        // Check if valid Post request
        bool prmMached = false;
        for (size_t i=0; i<pconf->prmLen; i++)
        {
            if (newDpair->spm == pconf->prm[i])
            {
                prmMached = true;
                break;
            }
        }
        OIC_LOG_V(DEBUG, TAG, "Parsed spm is %s", prmMached ? "valid" : "invalid, send error response");

        // Update local Dpairing with new Dpairing & prepare dtls session
        if (prmMached && '\0' != (char)newDpair->pdeviceID.id[0])
        {
            if(!gDpair)
            {
                gDpair = GetDpairingDefault();
            }
            gDpair->spm = newDpair->spm;
            memcpy(&gDpair->pdeviceID, &newDpair->pdeviceID, sizeof(OicUuid_t));
            memcpy(&gDpair->rowner, &pconf->rowner, sizeof(OicUuid_t));

#ifdef __WITH_DTLS__
            // Add temporary psk
            OCStackResult res;
            OicUuid_t subjectId = {.id={0}};
            res = AddTmpPskWithPIN(&gDpair->pdeviceID,
                           SYMMETRIC_PAIR_WISE_KEY,
                           (char*)pconf->pin.val, DP_PIN_LENGTH,
                           1, &gDpair->rowner, &subjectId);
            if(res != OC_STACK_OK ||
                    memcmp(&gDpair->pdeviceID, &subjectId, sizeof(OicUuid_t)))
            {
                OIC_LOG_V(ERROR, TAG, "Failed to save the temporal PSK : %d", res);
                goto exit;
            }

            // Prepare to establish a secure channel with Pin-based PSK cipher suite
            if (CA_STATUS_OK != CAEnableAnonECDHCipherSuite(false) ||
                CA_STATUS_OK != CASelectCipherSuite(TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA_256))
            {
                OIC_LOG_V(ERROR, TAG, "Failed to select TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA_256");
                goto exit;
            }

            if(CA_STATUS_OK != CARegisterDTLSHandshakeCallback(DPairingDTLSHandshakeCB))
            {
                OIC_LOG(WARNING, TAG, "DirectPairingHandler : Failed to register DTLS handshake callback.");
                goto exit;
            }
#endif // __WITH_DTLS__

            // should be lock /oic/sec/dpairing resource if Direct-Pairing starts normally ?
            OIC_LOG (DEBUG, TAG, "/oic/sec/dpairing resource created");

            ehRet = OC_EH_RESOURCE_CREATED;
        }
        else
        {
            OIC_LOG(ERROR, TAG, "Error in request check");
        }
    }


#ifdef __WITH_DTLS__
exit:
#endif // __WITH_DTLS__

    if (OC_EH_ERROR == ehRet && gDpair)
    {
        RemoveCredential(&gDpair->pdeviceID);
        gDpair = NULL;
    }

    // Send payload to request originator
    if(OC_STACK_OK != SendSRMResponse(ehRequest, ehRet, NULL))
    {
        OIC_LOG (ERROR, TAG, "SendSRMResponse failed in HandleDpairingPostRequest");
    }

    DeleteDpairingBinData(newDpair);
    OIC_LOG_V (DEBUG, TAG, "%s RetVal %d", __func__ , ehRet);
    return ehRet;
}

static OCEntityHandlerResult HandleDpairingPutRequest (const OCEntityHandlerRequest * ehRequest)
{
    OIC_LOG (DEBUG, TAG, "Dpairing EntityHandle  processing PUT request (Comfirmation)");

    OCEntityHandlerResult ehRet = OC_EH_ERROR;
    OicSecDpairing_t* newDpair = NULL;

    const OicSecPconf_t *pconf = GetPconfResourceData();
    if (true == pconf->edp)
    {
        // Convert JSON DPAIRING data into binary. This will also validate the DPAIRING data received.
        newDpair = JSONToDpairingBin(((OCSecurityPayload*)ehRequest->payload)->securityData);
    }
    else
    {
        OIC_LOG (DEBUG, TAG, "EDP == false : Direct-Pairing Disabled");
        ehRet = OC_EH_ERROR;
    }

    if (gDpair && newDpair)
    {
        OIC_LOG(DEBUG, TAG, "Received direct-pairing finalization request");

        // Check if valid Put request
        VERIFY_SUCCESS(TAG, PRM_NOT_ALLOWED == newDpair->spm, ERROR);

        const OicSecPconf_t *pconf = GetPconfResourceData();
        VERIFY_NON_NULL(TAG, pconf, ERROR);

#ifdef __WITH_DTLS__
        OCServerRequest * request = (OCServerRequest *)ehRequest->requestHandle;
        VERIFY_SUCCESS(TAG, (request->devAddr.flags | OC_FLAG_SECURE), ERROR);

        //Generate new credential
        OIC_LOG_V(INFO, TAG, "SavePairingPSK for %s(%d)", request->devAddr.addr, request->devAddr.port);
        OCStackResult res = SavePairingPSK(&request->devAddr, &newDpair->pdeviceID,
                                                                  (OicUuid_t *)&pconf->rowner, true);
        VERIFY_SUCCESS(TAG, OC_STACK_OK == res, ERROR);
#endif //__WITH_DTLS__

        //Generate new acl
        OicSecPdAcl_t *pdAcl;
        LL_FOREACH(pconf->pdacls, pdAcl)
        {
            OicSecAcl_t acl;
            memset(&acl, 0, sizeof(OicSecAcl_t));
            memcpy(&acl.subject, &gDpair->pdeviceID, sizeof(OicUuid_t));
            acl.resources = pdAcl->resources;
            acl.resourcesLen = pdAcl->resourcesLen;
            acl.owners = (OicUuid_t*)&pconf->rowner;
            acl.ownersLen = 1;
            acl.permission = pdAcl->permission;
            acl.periods = pdAcl->periods;
            acl.recurrences = pdAcl->recurrences;
            acl.prdRecrLen = pdAcl->prdRecrLen;

            char* aclJson = BinToAclJSON(&acl);
            if (aclJson)
            {
                InstallNewACL(aclJson);
                OICFree(aclJson);
            }
        }

        //update pconf device list
        AddPairedDevice(&newDpair->pdeviceID);

        //Initialize dpairing resource
        gDpair = NULL;

        OIC_LOG (DEBUG, TAG, "/oic/sec/dpairing resource updated, direct-pairing finalization success");
        ehRet = OC_EH_OK;
    }

exit:

    //Send payload to request originator
    if(OC_STACK_OK != SendSRMResponse(ehRequest, ehRet, NULL))
    {
        OIC_LOG (ERROR, TAG, "SendSRMResponse failed in HandleDpairingPutRequest");
    }

    DeleteDpairingBinData(newDpair);
    OIC_LOG_V (DEBUG, TAG, "%s RetVal %d", __func__ , ehRet);
    return ehRet;
}
/*
 * This internal method is the entity handler for Dpairing resources and
 * will handle REST request (GET/POST) for them.
 */
OCEntityHandlerResult DpairingEntityHandler (OCEntityHandlerFlag flag,
                                        OCEntityHandlerRequest * ehRequest,
                                        void* callbackParameter)
{
    OIC_LOG(DEBUG, TAG, "Received request DpairingEntityHandler");
    (void)callbackParameter;
    OCEntityHandlerResult ehRet = OC_EH_ERROR;

    if (!ehRequest)
    {
        return ehRet;
    }

    if (flag & OC_REQUEST_FLAG)
    {
        OIC_LOG (DEBUG, TAG, "Flag includes OC_REQUEST_FLAG");
        switch (ehRequest->method)
        {
            case OC_REST_GET:
                break;

            case OC_REST_POST:
                ehRet = HandleDpairingPostRequest(ehRequest);
                break;

            case OC_REST_PUT:
                ehRet = HandleDpairingPutRequest(ehRequest);
                break;

            case OC_REST_DELETE:
                break;

            default:
                ehRet = OC_EH_ERROR;
                SendSRMResponse(ehRequest, ehRet, NULL);
        }
    }

    return ehRet;
}

/*
 * This internal method is used to create '/oic/sec/dpairing' resource.
 */
OCStackResult CreateDpairingResource()
{
    OCStackResult ret;

    ret = OCCreateResource(&gDpairHandle,
                           OIC_RSRC_TYPE_SEC_DPAIRING,
                           OIC_MI_DEF,
                           OIC_RSRC_DPAIRING_URI,
                           DpairingEntityHandler,
                           NULL,
                           OC_SECURE | OC_EXPLICIT_DISCOVERABLE);

    if (OC_STACK_OK != ret)
    {
        OIC_LOG (ERROR, TAG, "Unable to instantiate Dpairing resource");
        DeInitDpairingResource();
    }
    return ret;
}

/**
 * Initialize Dpairing resource by loading data from persistent storage.
 *
 * @retval  OC_STACK_OK for Success, otherwise some error value
 */
OCStackResult InitDpairingResource()
{
    OCStackResult ret = OC_STACK_ERROR;

    // Instantiate 'oic.sec.dpairing'
    ret = CreateDpairingResource();
    if (OC_STACK_OK != ret)
    {
        DeInitDpairingResource();
    }
    return ret;
}

/**
 * Perform cleanup for Dpairing resources.
 *
 * @return
 * OC_STACK_OK    - no error
 * OC_STACK_ERROR - stack process error
 *
 */
OCStackResult DeInitDpairingResource()
{
    OCStackResult ret = OCDeleteResource(gDpairHandle);
    gDpair = NULL;

    if(OC_STACK_OK == ret)
    {
        return OC_STACK_OK;
    }
    else
    {
        return OC_STACK_ERROR;
    }
}



