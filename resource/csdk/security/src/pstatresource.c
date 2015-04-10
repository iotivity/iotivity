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

/* TODO : Add info about macro */
#define VERIFY_SUCCESS(op, logLevel) { if (!(op)) \
            {OC_LOG((logLevel), TAG, PCF(#op " failed!!")); goto exit;} }

/* TODO : Add info about macro */
#define VERIFY_NON_NULL(arg, logLevel) { if (!(arg)) { OC_LOG((logLevel), \
             TAG, PCF(#arg " is NULL")); goto exit; } }

OicSecPstat_t    *gPstat = NULL;
OCResourceHandle gPstatHandle = NULL;

/*
 * This internal method converts pstat data into JSON format.
 *
 * Note: Caller needs to invoke 'free' when finished done using
 * return string.
 */
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
    cJSON *jsonSmsArray = NULL;
    cJSON_AddItemToObject(jsonPstat, OIC_JSON_SMS_NAME, jsonSmsArray = cJSON_CreateArray());
    VERIFY_NON_NULL(jsonSmsArray, INFO);
    for (int i = 0; i < pstat->smsLen; i++)
    {
        cJSON_AddItemToArray(jsonSmsArray, cJSON_CreateNumber((int )pstat->sms[i]));
    }
    jsonStr = cJSON_Print(jsonRoot);
exit:
    if (jsonRoot)
    {
        cJSON_Delete(jsonRoot);
    }
    return jsonStr;
}

/*
 * This internal method converts JSON pstat into binary pstat.
 */
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

    pstat->isOp = cJSON_GetObjectItem(jsonPstat, OIC_JSON_ISOP_NAME)->valueint;
    cJSON *jsonObj = NULL;
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
    pstat->commitHash  = cJSON_GetObjectItem(jsonPstat, OIC_JSON_COMMIT_HASH_NAME)->valueint;
    pstat->cm  = cJSON_GetObjectItem(jsonPstat, OIC_JSON_CM_NAME)->valueint;
    pstat->om  = cJSON_GetObjectItem(jsonPstat, OIC_JSON_OM_NAME)->valueint;
    jsonObj = cJSON_GetObjectItem(jsonPstat, OIC_JSON_SMS_NAME);
    VERIFY_NON_NULL(jsonObj, ERROR);
    if (cJSON_Array == jsonObj->type)
    {
        pstat->smsLen = cJSON_GetArraySize(jsonObj);
        int idxx = 0;
        VERIFY_SUCCESS(pstat->smsLen != 0, ERROR);
        pstat->sms = (OicSecDpom_t*)OCCalloc(pstat->smsLen, sizeof(OicSecDpom_t));
        VERIFY_NON_NULL(pstat->sms, FATAL);
        do
        {
            cJSON *jsonSms = cJSON_GetArrayItem(jsonObj, idxx);
            VERIFY_NON_NULL(jsonSms, ERROR);
            pstat->sms[idxx] = jsonSms->valueint;
        }while ( ++idxx < pstat->smsLen);
    }
    ret = OC_STACK_OK;

exit:
    cJSON_Delete(jsonRoot);
    if (OC_STACK_OK != ret)
    {
        OCFree(pstat);
        pstat = NULL;
    }
    return pstat;
}

/*
 * This internal method is the entity handler for pstat resources.
 */
OCEntityHandlerResult PstatEntityHandler (OCEntityHandlerFlag flag,
                                        OCEntityHandlerRequest * ehRequest)
{
    OCEntityHandlerResult ret = OC_EH_ERROR;
    char *jsonRsp = NULL;

    /*
     * This method will handle REST request (GET/PUT/POST/DEL) for
     * virtual resources such as: /oic/sec/pstat, /oic/sec/acl etc
     */

    if (flag & OC_REQUEST_FLAG)
    {
        /* TODO :  Handle PUT/POST/DEL methods */
        OC_LOG (INFO, TAG, PCF("Flag includes OC_REQUEST_FLAG"));
        if (OC_REST_GET == ehRequest->method)
        {
            /* Convert pstat data into JSON for transmission */
            jsonRsp = BinToPstatJSON(gPstat);

            /* Send payload to request originator */
            ret = (SendSRMResponse(ehRequest, jsonRsp) == OC_STACK_OK ?
                   OC_EH_OK : OC_EH_ERROR);
        }
    }
    OCFree(jsonRsp);
    return ret;
}

/*
 * This internal method is used to create '/oic/sec/pstat' resource.
 */
OCStackResult CreatePstatResource()
{
    OCStackResult ret;

    /* TODO : Does these resources needs to be OC_DISCOVERABLE */
    ret = OCCreateResource(&gPstatHandle,
                           OIC_RSRC_TYPE_SEC_PSTAT,
                           OIC_MI_DEF,
                           OIC_RSRC_PSTAT_URI,
                           PstatEntityHandler,
                           OC_DISCOVERABLE | OC_OBSERVABLE);

    if (ret != OC_STACK_OK)
    {
        OC_LOG (FATAL, TAG, PCF("Unable to instantiate pstat resource"));
        DeInitPstatResource();
    }
    return ret;
}

/**
 * Initialize pstat resource by loading data from persistent storage.
 *
 * @retval  OC_STACK_OK for Success, otherwise some error value
 */
OCStackResult InitPstatResource()
{
    OCStackResult ret = OC_STACK_ERROR;

    /* Read Pstat resource from PS */
    char* jsonSVRDatabase = GetSVRDatabase();
    VERIFY_NON_NULL(jsonSVRDatabase, FATAL);

    /* Convert JSON Pstat into binary format */
    gPstat = JSONToPstatBin(jsonSVRDatabase);
    VERIFY_NON_NULL(gPstat, FATAL);

    /* Instantiate 'oic.sec.pstat' */
    ret = CreatePstatResource();

exit:
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
    OCFree(gPstat);
    gPstat = NULL;
    return OCDeleteResource(gPstatHandle);
}

