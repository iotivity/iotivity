//******************************************************************
//
// Copyright 2015 Intel Mobile Communications GmbH All Rights Reserved.
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

#include "ocstack.h"
#include "logger.h"
#include "ocmalloc.h"
#include "cJSON.h"
#include "resourcemanager.h"
#include "pstatresource.h"
#include "psinterface.h"
#include "utlist.h"
#include "base64.h"
#include "srmresourcestrings.h"
#include <stdlib.h>
#include <string.h>

#define TAG  PCF("SRM-PSTAT")

// TODO : Add info about macro
#define VERIFY_SUCCESS(op, logLevel) { if (!(op)) \
            {OC_LOG((logLevel), TAG, PCF(#op " failed!!")); goto exit;} }

// TODO : Add info about macro
#define VERIFY_NON_NULL(arg, logLevel) { if (!(arg)) { OC_LOG((logLevel), \
             TAG, PCF(#arg " is NULL")); goto exit; } }

static OicSecDpom_t gSm = SINGLE_SERVICE_CLIENT_DRIVEN;
static OicSecPstat_t gDefaultPstat =
{
    false,                                    // bool isOwned
    TAKE_OWNER | BOOTSTRAP_SERVICE | SECURITY_MANAGEMENT_SERVICES |
    PROVISION_CREDENTIALS | PROVISION_ACLS,   // OicSecDpm_t cm
    TAKE_OWNER | BOOTSTRAP_SERVICE | SECURITY_MANAGEMENT_SERVICES |
    PROVISION_CREDENTIALS | PROVISION_ACLS,   // OicSecDpm_t tm
    {},                                       // OicUuid_t deviceID
    SINGLE_SERVICE_CLIENT_DRIVEN,             // OicSecDpom_t om */
    1,                                        // the number of elts in Sms
    &gSm,                                     // OicSecDpom_t *sm
    0,                                        // uint16_t commitHash
};
static OicSecPstat_t    *gPstat = NULL;
static OCResourceHandle gPstatHandle = NULL;

void DeletePstatBinData(OicSecPstat_t* pstat)
{
    if (pstat)
    {
        //Clean 'supported modes' field
        OCFree(pstat->sm);

        //Clean pstat itself
        OCFree(pstat);
    }
}

char * BinToPstatJSON(const OicSecPstat_t * pstat)
{
    cJSON *jsonRoot = cJSON_CreateObject();
    cJSON *jsonPstat = NULL;
    char *jsonStr = NULL;
    if(NULL == pstat)
    {
        return NULL;
    }
    char base64Buff[B64ENCODE_OUT_SAFESIZE(sizeof(((OicUuid_t*) 0)->id)) + 1] = {};
    uint32_t outLen = 0;
    B64Result b64Ret = B64_OK;

    cJSON_AddItemToObject(jsonRoot, OIC_JSON_PSTAT_NAME, jsonPstat=cJSON_CreateObject());
    cJSON_AddBoolToObject(jsonPstat, OIC_JSON_ISOP_NAME, pstat->isOp);
    outLen = 0;
    b64Ret = b64Encode(pstat->deviceID.id,
            sizeof(pstat->deviceID.id), base64Buff, sizeof(base64Buff), &outLen);
    VERIFY_SUCCESS(b64Ret == B64_OK, ERROR);

    cJSON_AddStringToObject(jsonPstat, OIC_JSON_DEVICE_ID_NAME, base64Buff);
    cJSON_AddNumberToObject(jsonPstat, OIC_JSON_COMMIT_HASH_NAME, pstat->commitHash);
    cJSON_AddNumberToObject(jsonPstat, OIC_JSON_CM_NAME, (int)pstat->cm);
    cJSON_AddNumberToObject(jsonPstat, OIC_JSON_TM_NAME, (int)pstat->tm);
    cJSON_AddNumberToObject(jsonPstat, OIC_JSON_OM_NAME, (int)pstat->om);
    cJSON *jsonSmArray = NULL;
    cJSON_AddItemToObject(jsonPstat, OIC_JSON_SM_NAME, jsonSmArray = cJSON_CreateArray());
    VERIFY_NON_NULL(jsonSmArray, INFO);
    for (int i = 0; i < pstat->smLen; i++)
    {
        cJSON_AddItemToArray(jsonSmArray, cJSON_CreateNumber((int )pstat->sm[i]));
    }
    jsonStr = cJSON_Print(jsonRoot);
exit:
    if (jsonRoot)
    {
        cJSON_Delete(jsonRoot);
    }
    return jsonStr;
}

OicSecPstat_t * JSONToPstatBin(const char * jsonStr)
{
    OCStackResult ret = OC_STACK_ERROR;
    OicSecPstat_t *pstat = NULL;
    cJSON *jsonRoot = cJSON_Parse(jsonStr);
    VERIFY_NON_NULL(jsonRoot, INFO);

    cJSON *jsonPstat = cJSON_GetObjectItem(jsonRoot, OIC_JSON_PSTAT_NAME);
    VERIFY_NON_NULL(jsonPstat, INFO);

    pstat = (OicSecPstat_t*)OCCalloc(1, sizeof(OicSecPstat_t));
    VERIFY_NON_NULL(pstat, INFO);
    cJSON *jsonObj = NULL;
    jsonObj = cJSON_GetObjectItem(jsonPstat, OIC_JSON_ISOP_NAME);
    VERIFY_NON_NULL(jsonObj, ERROR);
    VERIFY_SUCCESS((cJSON_True == jsonObj->type || cJSON_False == jsonObj->type) , ERROR);
    pstat->isOp = jsonObj->valueint;

    unsigned char base64Buff[sizeof(((OicUuid_t*) 0)->id)] = {};
    uint32_t outLen = 0;
    B64Result b64Ret = B64_OK;
    jsonObj = cJSON_GetObjectItem(jsonPstat, OIC_JSON_DEVICE_ID_NAME);
    VERIFY_NON_NULL(jsonObj, ERROR);
    VERIFY_SUCCESS(cJSON_String == jsonObj->type, ERROR);
    b64Ret = b64Decode(jsonObj->valuestring, strlen(jsonObj->valuestring), base64Buff,
                sizeof(base64Buff), &outLen);
    VERIFY_SUCCESS((b64Ret == B64_OK) && (outLen <= sizeof(pstat->deviceID.id)), ERROR);
    memcpy(pstat->deviceID.id, base64Buff, outLen);

    jsonObj = cJSON_GetObjectItem(jsonPstat, OIC_JSON_COMMIT_HASH_NAME);
    VERIFY_NON_NULL(jsonObj, ERROR);
    VERIFY_SUCCESS(cJSON_Number == jsonObj->type, ERROR);
    pstat->commitHash  = jsonObj->valueint;

    jsonObj = cJSON_GetObjectItem(jsonPstat, OIC_JSON_CM_NAME);
    VERIFY_NON_NULL(jsonObj, ERROR);
    VERIFY_SUCCESS(cJSON_Number == jsonObj->type, ERROR);
    pstat->cm  = jsonObj->valueint;

    jsonObj = cJSON_GetObjectItem(jsonPstat, OIC_JSON_OM_NAME);
    VERIFY_NON_NULL(jsonObj, ERROR);
    VERIFY_SUCCESS(cJSON_Number == jsonObj->type, ERROR);
    pstat->om  = jsonObj->valueint;

    jsonObj = cJSON_GetObjectItem(jsonPstat, OIC_JSON_SM_NAME);
    VERIFY_NON_NULL(jsonObj, ERROR);
    if (cJSON_Array == jsonObj->type)
    {
        pstat->smLen = cJSON_GetArraySize(jsonObj);
        int idxx = 0;
        VERIFY_SUCCESS(pstat->smLen != 0, ERROR);
        pstat->sm = (OicSecDpom_t*)OCCalloc(pstat->smLen, sizeof(OicSecDpom_t));
        VERIFY_NON_NULL(pstat->sm, FATAL);
        do
        {
            cJSON *jsonSm = cJSON_GetArrayItem(jsonObj, idxx);
            VERIFY_NON_NULL(jsonSm, ERROR);
            pstat->sm[idxx] = jsonSm->valueint;
        }while ( ++idxx < pstat->smLen);
    }
    ret = OC_STACK_OK;

exit:
    cJSON_Delete(jsonRoot);
    if (OC_STACK_OK != ret)
    {
        OC_LOG (ERROR, TAG, PCF("JSONToPstatBin failed"));
        DeletePstatBinData(pstat);
        pstat = NULL;
    }
    return pstat;
}

/**
 * The entity handler determines how to process a GET request.
 */
static OCEntityHandlerResult HandlePstatGetRequest (const OCEntityHandlerRequest * ehRequest)
{
    // Convert ACL data into JSON for transmission
    char* jsonStr = BinToPstatJSON(gPstat);

    // A device should always have a default pstat. Therefore, jsonStr should never be NULL.
    OCEntityHandlerResult ehRet = (jsonStr ? OC_EH_OK : OC_EH_ERROR);

    // Send response payload to request originator
    SendSRMResponse(ehRequest, ehRet, jsonStr);
    OCFree(jsonStr);
    return ehRet;
}

/**
 * The entity handler determines how to process a POST request.
 * Per the REST paradigm, POST can also be used to update representation of existing
 * resource or create a new resource.
 * For pstat, it updates only tm and om.
 */
static OCEntityHandlerResult HandlePstatPutRequest(const OCEntityHandlerRequest *ehRequest)
{
    OCEntityHandlerResult ehRet = OC_EH_ERROR;
    cJSON *postJson = NULL;

    if (ehRequest->resource)
    {
        postJson = cJSON_Parse((char *) ehRequest->reqJSONPayload);
        VERIFY_NON_NULL(postJson, INFO);
        cJSON *jsonPstat = cJSON_GetObjectItem(postJson, OIC_JSON_PSTAT_NAME);
        VERIFY_NON_NULL(jsonPstat, INFO);
        cJSON *commitHashJson = cJSON_GetObjectItem(jsonPstat, OIC_JSON_COMMIT_HASH_NAME);
        uint16_t commitHash = 0;
        if (commitHashJson)
        {
            commitHash = commitHashJson->valueint;
        }
        cJSON *tmJson = cJSON_GetObjectItem(jsonPstat, OIC_JSON_TM_NAME);
        if (tmJson && gPstat)
        {
            gPstat->tm = tmJson->valueint;
            if(0 == tmJson->valueint && gPstat->commitHash == commitHash)
            {
                gPstat->isOp = true;
                gPstat->cm = 0;
                OC_LOG (INFO, TAG, PCF("CommitHash is valid and isOp is TRUE"));
            }
            else
            {
                OC_LOG (INFO, TAG, PCF("CommitHash is not valid"));
            }
        }
        cJSON *omJson = cJSON_GetObjectItem(jsonPstat, OIC_JSON_OM_NAME);
        if (omJson && gPstat)
        {
            /*
             * Check if the operation mode is in the supported provisioning services
             * operation mode list.
             */
            for(size_t i=0; i< gPstat->smLen; i++)
            {
                if(gPstat->sm[i] == omJson->valueint)
                {
                    gPstat->om = omJson->valueint;
                    break;
                }
            }
        }
        // Convert pstat data into JSON for update to persistent storage
        char *jsonStr = BinToPstatJSON(gPstat);
        if (jsonStr)
        {
            cJSON *jsonPstat = cJSON_Parse(jsonStr);
            OCFree(jsonStr);
            if (OC_STACK_OK == UpdateSVRDatabase(OIC_JSON_PSTAT_NAME, jsonPstat))
            {
                ehRet = OC_EH_OK;
            }
        }
    }
 exit:
    //Send payload to request originator
    if(OC_STACK_OK != SendSRMResponse(ehRequest, ehRet, NULL))
    {
        OC_LOG (ERROR, TAG, PCF("SendSRMResponse failed in HandlePstatPostRequest"));
    }
    cJSON_Delete(postJson);
    return ehRet;
}

/**
 * This internal method is the entity handler for pstat resources.
 */
OCEntityHandlerResult PstatEntityHandler(OCEntityHandlerFlag flag,
        OCEntityHandlerRequest * ehRequest)
{
    OCEntityHandlerResult ehRet = OC_EH_ERROR;
    // This method will handle REST request (GET/POST) for /oic/sec/pstat
    if (flag & OC_REQUEST_FLAG)
    {
        OC_LOG (INFO, TAG, PCF("Flag includes OC_REQUEST_FLAG"));
        switch (ehRequest->method)
        {
            case OC_REST_GET:
                ehRet = HandlePstatGetRequest(ehRequest);
                break;
            case OC_REST_PUT:
                ehRet = HandlePstatPutRequest(ehRequest);
                break;
            default:
                ehRet = OC_EH_ERROR;
                SendSRMResponse(ehRequest, ehRet, NULL);
                break;
        }
    }
    return ehRet;
}

/**
 * This internal method is used to create '/oic/sec/pstat' resource.
 */
OCStackResult CreatePstatResource()
{
    OCStackResult ret;

    ret = OCCreateResource(&gPstatHandle,
                           OIC_RSRC_TYPE_SEC_PSTAT,
                           OIC_MI_DEF,
                           OIC_RSRC_PSTAT_URI,
                           PstatEntityHandler,
                           OC_RES_PROP_NONE);

    if (ret != OC_STACK_OK)
    {
        OC_LOG (FATAL, TAG, PCF("Unable to instantiate pstat resource"));
        DeInitPstatResource();
    }
    return ret;
}

/**
 * Post ACL hander update the commitHash during ACL provisioning.
 */
void SetCommitHash(uint16_t commitHash)
{
    gPstat->commitHash = commitHash;
}

/**
 * Get the default value
 * @retval  the gDefaultPstat pointer
 */
static OicSecPstat_t* GetPstatDefault()
{
    return &gDefaultPstat;
}

/**
 * Initialize pstat resource by loading data from persistent storage.
 *
 * @retval  OC_STACK_OK for Success, otherwise some error value
 */
OCStackResult InitPstatResource()
{
    OCStackResult ret = OC_STACK_ERROR;

    // Read Pstat resource from PS
    char* jsonSVRDatabase = GetSVRDatabase();
    if (jsonSVRDatabase)
    {
        // Convert JSON Pstat into binary format
        gPstat = JSONToPstatBin(jsonSVRDatabase);
    }
    /*
     * If SVR database in persistent storage got corrupted or
     * is not available for some reason, a default pstat is created
     * which allows user to initiate pstat provisioning again.
     */
    if(!jsonSVRDatabase || !gPstat)
    {
        gPstat = GetPstatDefault();
    }
    // Instantiate 'oic.sec.pstat'
    ret = CreatePstatResource();

    OCFree(jsonSVRDatabase);
    return ret;
}

/**
 * Perform cleanup for pstat resources.
 *
 * @retval  OC_STACK_OK for Success, otherwise some error value
 */
OCStackResult DeInitPstatResource()
{
    if(gPstat != &gDefaultPstat)
    {
        DeletePstatBinData(gPstat);
        gPstat = NULL;
    }
    return OCDeleteResource(gPstatHandle);
}

