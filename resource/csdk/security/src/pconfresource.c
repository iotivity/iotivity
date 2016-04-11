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
#include "pconfresource.h"
#include "psinterface.h"
#include "utlist.h"
#include "srmresourcestrings.h"
#include "doxmresource.h"
#include "srmutility.h"
#include "ocserverrequest.h"
#include <stdlib.h>
#ifdef WITH_ARDUINO
#include <string.h>
#else
#include <strings.h>
#endif

#define TAG  "SRM-PCONF"


static OicSecPconf_t          *gPconf = NULL;
static OCResourceHandle   gPconfHandle = NULL;
static OicSecPconf_t         gDefaultPconf =
{
    false,                  /* bool edp */
    NULL,                  /* OicSecPrm *prm */
    0,                       /* size_t prmLen */
    {.val = {0}},       /* OicDpPin_t pin */
    NULL,                  /* OicSecPdAcl_t *pdacls */
    NULL,                  /* OicUuid_t *pddevs */
    0,                       /* size_t pddevLen */
    {.id = {0}},        /* OicUuid_t deviceID */
    {.id = {0}},        /* OicUuid_t rowner */
};

/**
 * This function frees OicSecPdAcl_t object's fields and object itself.
 */
void FreePdAclList(OicSecPdAcl_t* pdacls)
{
    if (pdacls)
    {
        size_t i = 0;

        //Clean pdacl objecs
        OicSecPdAcl_t *aclTmp1 = NULL;
        OicSecPdAcl_t *aclTmp2 = NULL;
        LL_FOREACH_SAFE(pdacls, aclTmp1, aclTmp2)
        {
            LL_DELETE(pdacls, aclTmp1);

            // Clean Resources
            for (i = 0; i < aclTmp1->resourcesLen; i++)
            {
                OICFree(aclTmp1->resources[i]);
            }
            OICFree(aclTmp1->resources);

            //Clean Period
            if(aclTmp1->periods)
            {
                for(i = 0; i < aclTmp1->prdRecrLen; i++)
                {
                    OICFree(aclTmp1->periods[i]);
                }
                OICFree(aclTmp1->periods);
            }

            //Clean Recurrence
            if(aclTmp1->recurrences)
            {
                for(i = 0; i < aclTmp1->prdRecrLen; i++)
                {
                    OICFree(aclTmp1->recurrences[i]);
                }
                OICFree(aclTmp1->recurrences);
            }
        }

        //Clean pconf itself
        OICFree(pdacls);
    }
}

void DeletePconfBinData(OicSecPconf_t* pconf)
{
    if (pconf)
    {
        //Clean prm
        OICFree(pconf->prm);

        //Clean pdacl
        if (pconf->pdacls)
        {
            FreePdAclList(pconf->pdacls);
        }

        //Clean pddev
        OICFree(pconf->pddevs);

        //Clean pconf itself
        OICFree(pconf);
    }
}

/*
 * This internal method converts PCONF data into JSON format.
 *
 * Note: Caller needs to invoke 'free' when finished done using
 * return string.
 */
char * BinToPconfJSON(const OicSecPconf_t * pconf)
{
    OIC_LOG(DEBUG, TAG, "BinToPconfJSON() IN");

    if (NULL == pconf)
    {
        return NULL;
    }

    char *jsonStr = NULL;
    cJSON *jsonPconf = NULL;
    char base64Buff[B64ENCODE_OUT_SAFESIZE(sizeof(((OicUuid_t*)0)->id)) + 1] = {};
    uint32_t outLen = 0;
    B64Result b64Ret = B64_OK;

    cJSON *jsonRoot = cJSON_CreateObject();
    VERIFY_NON_NULL(TAG, jsonRoot, ERROR);

    jsonPconf = cJSON_CreateObject();
    VERIFY_NON_NULL(TAG, jsonPconf, ERROR);
    cJSON_AddItemToObject(jsonRoot, OIC_JSON_PCONF_NAME, jsonPconf );


    //EDP -- Mandatory
    cJSON_AddBoolToObject(jsonPconf, OIC_JSON_EDP_NAME, pconf->edp);

    //PRM type -- Mandatory
    if(0< pconf->prmLen)
    {
        cJSON *jsonPrmArray = cJSON_CreateArray();
        VERIFY_NON_NULL(TAG, jsonPrmArray, ERROR);
        cJSON_AddItemToObject (jsonPconf, OIC_JSON_PRM_NAME, jsonPrmArray);
        OIC_LOG_V (DEBUG, TAG, "pconf->prmLen = %d", (int)pconf->prmLen);
        for (size_t i = 0; i < pconf->prmLen; i++)
        {
            OIC_LOG_V (DEBUG, TAG, "pconf->prm[%d] = %d", (int)i, pconf->prm[i]);
            cJSON_AddItemToArray (jsonPrmArray, cJSON_CreateNumber(pconf->prm[i]));
        }
    }

    //PIN -- Mandatory
    if(DP_PIN_LENGTH == strlen((const char*)pconf->pin.val))
    {
        cJSON_AddStringToObject(jsonPconf, OIC_JSON_PIN_NAME, (char*)pconf->pin.val);
    }

    //PDACL -- Mandatory
    if(pconf->pdacls)
    {
        cJSON *jsonAclArray = NULL;
        OicSecPdAcl_t *pdacl = NULL;
        cJSON_AddItemToObject (jsonPconf, OIC_JSON_PDACL_NAME,
                jsonAclArray = cJSON_CreateArray());
        VERIFY_NON_NULL(TAG, jsonAclArray, ERROR);

        pdacl = pconf->pdacls;
        while(pdacl)
        {
            cJSON *jsonAcl = cJSON_CreateObject();

            // Resources -- Mandatory
            cJSON *jsonRsrcArray = NULL;
            cJSON_AddItemToObject (jsonAcl, OIC_JSON_RESOURCES_NAME,
                    jsonRsrcArray = cJSON_CreateArray());
            VERIFY_NON_NULL(TAG, jsonRsrcArray, ERROR);
            for (size_t i = 0; i < pdacl->resourcesLen; i++)
            {
                cJSON_AddItemToArray (jsonRsrcArray,
                        cJSON_CreateString(pdacl->resources[i]));
            }

            // Permissions -- Mandatory
            cJSON_AddNumberToObject (jsonAcl, OIC_JSON_PERMISSION_NAME, pdacl->permission);

            //Period & Recurrence -- Not Mandatory
            if(0 != pdacl->prdRecrLen && pdacl->periods)
            {
                cJSON *jsonPeriodArray = NULL;
                cJSON_AddItemToObject (jsonAcl, OIC_JSON_PERIODS_NAME,
                        jsonPeriodArray = cJSON_CreateArray());
                VERIFY_NON_NULL(TAG, jsonPeriodArray, ERROR);
                for (size_t i = 0; i < pdacl->prdRecrLen; i++)
                {
                    cJSON_AddItemToArray (jsonPeriodArray,
                            cJSON_CreateString(pdacl->periods[i]));
                }
            }

            //Recurrence -- Not Mandatory
            if(0 != pdacl->prdRecrLen && pdacl->recurrences)
            {
                cJSON *jsonRecurArray  = NULL;
                cJSON_AddItemToObject (jsonAcl, OIC_JSON_RECURRENCES_NAME,
                        jsonRecurArray = cJSON_CreateArray());
                VERIFY_NON_NULL(TAG, jsonRecurArray, ERROR);
                for (size_t i = 0; i < pdacl->prdRecrLen; i++)
                {
                    cJSON_AddItemToArray (jsonRecurArray,
                            cJSON_CreateString(pdacl->recurrences[i]));
                }
            }

            // Attach current acl node to Acl Array
            cJSON_AddItemToArray(jsonAclArray, jsonAcl);
            pdacl = pdacl->next;
        }
    }

    //PDDev -- Mandatory
    //There may not be paired devices if it did not pairing before
    if (pconf->pddevs && 0 < pconf->pddevLen)
    {
        cJSON *jsonPdDevArray = cJSON_CreateArray();
        VERIFY_NON_NULL(TAG, jsonPdDevArray, ERROR);
        cJSON_AddItemToObject (jsonPconf, OIC_JSON_PDDEV_LIST_NAME, jsonPdDevArray );
        for (size_t i = 0; i < pconf->pddevLen; i++)
        {
            outLen = 0;
            b64Ret = b64Encode(pconf->pddevs[i].id, sizeof(pconf->pddevs[i].id), base64Buff,
                        sizeof(base64Buff), &outLen);
            VERIFY_SUCCESS(TAG, b64Ret == B64_OK, ERROR);
            cJSON_AddItemToArray (jsonPdDevArray, cJSON_CreateString(base64Buff));
        }
    }

    //DeviceId -- Mandatory
    //There may not be devicd id if caller is provisoning tool
    if ('\0' != (char)pconf->deviceID.id[0])
    {
        outLen = 0;
        b64Ret = b64Encode(pconf->deviceID.id, sizeof(pconf->deviceID.id), base64Buff,
                    sizeof(base64Buff), &outLen);
        VERIFY_SUCCESS(TAG, b64Ret == B64_OK, ERROR);
        cJSON_AddStringToObject(jsonPconf, OIC_JSON_DEVICE_ID_NAME, base64Buff);
    }

    //ROwner -- Mandatory
    VERIFY_SUCCESS(TAG, '\0' != (char)pconf->rowner.id[0], ERROR);
    outLen = 0;
    b64Ret = b64Encode(pconf->rowner.id, sizeof(pconf->rowner.id), base64Buff,
                    sizeof(base64Buff), &outLen);
    VERIFY_SUCCESS(TAG, b64Ret == B64_OK, ERROR);
    cJSON_AddStringToObject(jsonPconf, OIC_JSON_ROWNER_NAME, base64Buff);


    jsonStr = cJSON_PrintUnformatted(jsonRoot);

exit:
    if (jsonRoot)
    {
        cJSON_Delete(jsonRoot);
    }
    return jsonStr;
}

/*
 * This internal method converts JSON PCONF into binary PCONF.
 */
OicSecPconf_t * JSONToPconfBin(const char * jsonStr)
{
    OIC_LOG(DEBUG, TAG, "JSONToPconfBin() IN");

    OCStackResult ret = OC_STACK_ERROR;
    OicSecPconf_t * pconf =  NULL;
    cJSON *jsonRoot = NULL;
    cJSON *jsonPconf = NULL;
    cJSON *jsonObj = NULL;
    size_t jsonObjLen = 0;

    unsigned char base64Buff[sizeof(((OicUuid_t*)0)->id)] = {};
    uint32_t outLen = 0;
    B64Result b64Ret = B64_OK;


    VERIFY_NON_NULL(TAG, jsonStr, ERROR);

    jsonRoot = cJSON_Parse(jsonStr);
    VERIFY_NON_NULL(TAG, jsonRoot, ERROR);

    jsonPconf = cJSON_GetObjectItem(jsonRoot, OIC_JSON_PCONF_NAME);
    VERIFY_NON_NULL(TAG, jsonPconf, ERROR);

    pconf = (OicSecPconf_t*)OICCalloc(1, sizeof(OicSecPconf_t));
    VERIFY_NON_NULL(TAG, pconf, ERROR);

    //EDP -- Mandatory
    jsonObj = cJSON_GetObjectItem(jsonPconf, OIC_JSON_EDP_NAME);
    VERIFY_NON_NULL(TAG, jsonObj, ERROR);
    VERIFY_SUCCESS(TAG, (cJSON_True == jsonObj->type || cJSON_False == jsonObj->type) , ERROR);
    pconf->edp = jsonObj->valueint;

    //PRM type -- Mandatory
    jsonObj = cJSON_GetObjectItem(jsonPconf, OIC_JSON_PRM_NAME);
    if (jsonObj && cJSON_Array == jsonObj->type)
    {
        int arrLen = cJSON_GetArraySize(jsonObj);
        if(0 < arrLen)
        {
            pconf->prmLen = (size_t)arrLen;
            pconf->prm = (OicSecPrm_t *)OICCalloc(pconf->prmLen, sizeof(OicSecPrm_t));
            VERIFY_NON_NULL(TAG, pconf->prm, ERROR);

            for (size_t i  = 0; i < pconf->prmLen ; i++)
            {
                cJSON *jsonPrm = cJSON_GetArrayItem(jsonObj, i);
                VERIFY_NON_NULL(TAG, jsonPrm, ERROR);
                pconf->prm[i] = (OicSecPrm_t)jsonPrm->valueint;
                OIC_LOG_V (DEBUG, TAG, "jsonPrm->valueint = %d", jsonPrm->valueint);
                OIC_LOG_V (DEBUG, TAG, "pconf->prm[%d] = %d", (int)i, pconf->prm[i]);
            }
        }
    }

    //PIN -- Mandatory
    jsonObj = cJSON_GetObjectItem(jsonPconf, OIC_JSON_PIN_NAME);
    if (jsonObj)
    {
        VERIFY_SUCCESS(TAG, cJSON_String == jsonObj->type, ERROR);
        VERIFY_SUCCESS(TAG, DP_PIN_LENGTH == strlen(jsonObj->valuestring), ERROR);
        OICStrcpy((char*)pconf->pin.val, DP_PIN_LENGTH + 1, (char*)jsonObj->valuestring);
    }
    else
    {
        memset(pconf->pin.val, 0, DP_PIN_LENGTH+1);
    }

    //PDACL -- Mandatory
    jsonObj = cJSON_GetObjectItem(jsonPconf, OIC_JSON_PDACL_NAME);
    if (jsonObj)
    {
        VERIFY_SUCCESS(TAG, cJSON_Array == jsonObj->type, ERROR);

        OicSecPdAcl_t * headAcl = NULL;
        OicSecPdAcl_t * prevAcl = NULL;
        int numPdAcl = cJSON_GetArraySize(jsonObj);
        int idx = 0;

        while(idx < numPdAcl)
        {
            cJSON *jsonPdAcl = cJSON_GetArrayItem(jsonObj, idx);
            VERIFY_NON_NULL(TAG, jsonPdAcl, ERROR);

            OicSecPdAcl_t *pdacl = (OicSecPdAcl_t*)OICCalloc(1, sizeof(OicSecPdAcl_t));
            VERIFY_NON_NULL(TAG, pdacl, ERROR);

            headAcl = (headAcl) ? headAcl : pdacl;
            if (prevAcl)
            {
                prevAcl->next = pdacl;
            }

            cJSON *jsonAclObj = NULL;

            // Resources -- Mandatory
            jsonAclObj = cJSON_GetObjectItem(jsonPdAcl, OIC_JSON_RESOURCES_NAME);
            VERIFY_NON_NULL(TAG, jsonAclObj, ERROR);
            VERIFY_SUCCESS(TAG, cJSON_Array == jsonAclObj->type, ERROR);

            pdacl->resourcesLen = (size_t)cJSON_GetArraySize(jsonAclObj);
            VERIFY_SUCCESS(TAG, pdacl->resourcesLen > 0, ERROR);
            pdacl->resources = (char**)OICCalloc(pdacl->resourcesLen, sizeof(char*));
            VERIFY_NON_NULL(TAG, (pdacl->resources), ERROR);

            size_t idxx = 0;
            do
            {
                cJSON *jsonRsrc = cJSON_GetArrayItem(jsonAclObj, idxx);
                VERIFY_NON_NULL(TAG, jsonRsrc, ERROR);

                jsonObjLen = strlen(jsonRsrc->valuestring) + 1;
                pdacl->resources[idxx] = (char*)OICMalloc(jsonObjLen);
                VERIFY_NON_NULL(TAG, (pdacl->resources[idxx]), ERROR);
                OICStrcpy(pdacl->resources[idxx], jsonObjLen, jsonRsrc->valuestring);
            } while ( ++idxx < pdacl->resourcesLen);

            // Permissions -- Mandatory
            jsonAclObj = cJSON_GetObjectItem(jsonPdAcl,
                                OIC_JSON_PERMISSION_NAME);
            VERIFY_NON_NULL(TAG, jsonAclObj, ERROR);
            VERIFY_SUCCESS(TAG, cJSON_Number == jsonAclObj->type, ERROR);
            pdacl->permission = jsonAclObj->valueint;

            //Period -- Not Mandatory
            cJSON *jsonPeriodObj = cJSON_GetObjectItem(jsonPdAcl,
                    OIC_JSON_PERIODS_NAME);
            if(jsonPeriodObj)
            {
                VERIFY_SUCCESS(TAG, cJSON_Array == jsonPeriodObj->type,
                               ERROR);
                pdacl->prdRecrLen = (size_t)cJSON_GetArraySize(jsonPeriodObj);
                if(pdacl->prdRecrLen > 0)
                {
                    pdacl->periods = (char**)OICCalloc(pdacl->prdRecrLen,
                                    sizeof(char*));
                    VERIFY_NON_NULL(TAG, pdacl->periods, ERROR);

                    cJSON *jsonPeriod = NULL;
                    for(size_t i = 0; i < pdacl->prdRecrLen; i++)
                    {
                        jsonPeriod = cJSON_GetArrayItem(jsonPeriodObj, i);
                        VERIFY_NON_NULL(TAG, jsonPeriod, ERROR);

                        jsonObjLen = strlen(jsonPeriod->valuestring) + 1;
                        pdacl->periods[i] = (char*)OICMalloc(jsonObjLen);
                        VERIFY_NON_NULL(TAG, pdacl->periods[i], ERROR);
                        OICStrcpy(pdacl->periods[i], jsonObjLen,
                                  jsonPeriod->valuestring);
                    }
                }
            }

            //Recurrence -- Not mandatory
            cJSON *jsonRecurObj = cJSON_GetObjectItem(jsonPdAcl,
                                        OIC_JSON_RECURRENCES_NAME);
            if(jsonRecurObj)
            {
                VERIFY_SUCCESS(TAG, cJSON_Array == jsonRecurObj->type,
                               ERROR);

                if(pdacl->prdRecrLen > 0)
                {
                    pdacl->recurrences = (char**)OICCalloc(pdacl->prdRecrLen,
                                             sizeof(char*));
                    VERIFY_NON_NULL(TAG, pdacl->recurrences, ERROR);

                    cJSON *jsonRecur = NULL;
                    for(size_t i = 0; i < pdacl->prdRecrLen; i++)
                    {
                        jsonRecur = cJSON_GetArrayItem(jsonRecurObj, i);
                        VERIFY_NON_NULL(TAG, jsonRecur, ERROR);
                        jsonObjLen = strlen(jsonRecur->valuestring) + 1;
                        pdacl->recurrences[i] = (char*)OICMalloc(jsonObjLen);
                        VERIFY_NON_NULL(TAG, pdacl->recurrences[i], ERROR);
                        OICStrcpy(pdacl->recurrences[i], jsonObjLen,
                              jsonRecur->valuestring);
                    }
                }
            }

            prevAcl = pdacl;
            idx++;
        }

        pconf->pdacls = headAcl;
    }
    else
    {
        pconf->pdacls = NULL;
    }

    //PDDev -- Mandatory
    jsonObj = cJSON_GetObjectItem(jsonPconf, OIC_JSON_PDDEV_LIST_NAME);
    if (jsonObj && cJSON_Array == jsonObj->type)
    {
        pconf->pddevLen = (size_t)cJSON_GetArraySize(jsonObj);
        if(0 < pconf->pddevLen)
        {
            pconf->pddevs = (OicUuid_t *)OICCalloc(pconf->pddevLen, sizeof(OicUuid_t));
            VERIFY_NON_NULL(TAG, pconf->pddevs, ERROR);

            for (size_t i  = 0; i < pconf->pddevLen ; i++)
            {
                cJSON *jsonPdDev = cJSON_GetArrayItem(jsonObj, i);
                VERIFY_NON_NULL(TAG, jsonPdDev, ERROR);

                outLen = 0;
                b64Ret = b64Decode(jsonPdDev->valuestring, strlen(jsonPdDev->valuestring), base64Buff,
                        sizeof(base64Buff), &outLen);
                VERIFY_SUCCESS(TAG, (b64Ret == B64_OK && outLen <= sizeof(pconf->pddevs[i].id)), ERROR);
                memcpy(pconf->pddevs[i].id, base64Buff, outLen);
            }
        }
    }
    else
    {
        pconf->pddevs = NULL;
        pconf->pddevLen = 0;
    }

    //DeviceId -- Mandatory
    outLen = 0;
    jsonObj = cJSON_GetObjectItem(jsonPconf, OIC_JSON_DEVICE_ID_NAME);
    if (jsonObj && cJSON_String == jsonObj->type)
    {
        b64Ret = b64Decode(jsonObj->valuestring, strlen(jsonObj->valuestring), base64Buff,
                sizeof(base64Buff), &outLen);
        VERIFY_SUCCESS(TAG, (b64Ret == B64_OK && outLen <= sizeof(pconf->deviceID.id)), ERROR);
        memcpy(pconf->deviceID.id, base64Buff, outLen);
    }
    else
    {
        OicUuid_t deviceId = {.id = {0}};
        OCStackResult ret = GetDoxmDeviceID( &deviceId);
        VERIFY_SUCCESS(TAG, OC_STACK_OK == ret, ERROR);
        memcpy(&pconf->deviceID, &deviceId, sizeof(OicUuid_t));
    }

    // ROwner -- Mandatory
    outLen = 0;
    jsonObj = cJSON_GetObjectItem(jsonPconf, OIC_JSON_ROWNER_NAME);
    if (jsonObj && cJSON_String == jsonObj->type)
    {
        b64Ret = b64Decode(jsonObj->valuestring, strlen(jsonObj->valuestring), base64Buff,
                sizeof(base64Buff), &outLen);
        VERIFY_SUCCESS(TAG, (b64Ret == B64_OK && outLen <= sizeof(pconf->rowner.id)), ERROR);
        memcpy(pconf->rowner.id, base64Buff, outLen);
    }
    else
    {
        memset(&pconf->rowner, 0, sizeof(OicUuid_t));
    }

    ret = OC_STACK_OK;

exit:
    cJSON_Delete(jsonRoot);
    if (OC_STACK_OK != ret)
    {
        DeletePconfBinData(pconf);
        pconf = NULL;
    }

    OIC_LOG(DEBUG, TAG, "JSONToPconfBin() OUT");
    return pconf;
}

static bool UpdatePersistentStorage(const OicSecPconf_t * pconf)
{
    bool ret = false;

    // Convert PCONF data into JSON for update to persistent storage
    char *jsonStr = BinToPconfJSON(pconf);
    if (jsonStr)
    {
        cJSON *jsonPconf = cJSON_Parse(jsonStr);
        OICFree(jsonStr);

        if ((jsonPconf) &&
                (OC_STACK_OK == UpdateSVRDatabase(OIC_JSON_PCONF_NAME, jsonPconf)))
        {
            ret = true;
        }
        cJSON_Delete(jsonPconf);
    }
    return ret;
}

static OCEntityHandlerResult HandlePconfGetRequest (const OCEntityHandlerRequest * ehRequest)
{
    char* jsonStr = NULL;
    OCEntityHandlerResult ehRet = OC_EH_OK;

    OicSecPconf_t pconf;
    memset(&pconf, 0, sizeof(OicSecPconf_t));

    OIC_LOG (DEBUG, TAG, "Pconf EntityHandle processing GET request");

    if (true == GetDoxmResourceData()->dpc)
    {
        //Making response elements for Get request
        if( (true == gPconf->edp) && (gPconf->prm && 0 < gPconf->prmLen) &&
            (0 < strlen((const char*)gPconf->deviceID.id)) && (0 < strlen((const char*)gPconf->rowner.id)))
        {
            pconf.edp = true;
            pconf.prm = gPconf->prm;
            pconf.prmLen = gPconf->prmLen;
            memcpy(&pconf.deviceID, &gPconf->deviceID, sizeof(OicUuid_t));
            memcpy(&pconf.rowner, &gPconf->rowner, sizeof(OicUuid_t));
            OIC_LOG (DEBUG, TAG, "PCONF - direct pairing enabled");
        }
        else if (false == gPconf->edp)
        {
            pconf.edp = false;
            memcpy(&pconf.rowner, &gPconf->rowner, sizeof(OicUuid_t));
            OIC_LOG (DEBUG, TAG, "PCONF - direct pairing disable");
        }
        else
        {
            ehRet= OC_EH_ERROR;
            OIC_LOG (DEBUG, TAG, "PCONF - error");
        }
    }
    else
    {
        OIC_LOG (DEBUG, TAG, "DPC == false : Direct-Pairing Disabled");
        ehRet = OC_EH_ERROR;
    }

    jsonStr = (ehRet == OC_EH_OK) ? BinToPconfJSON(&pconf) : NULL;

    // Send response payload to request originator
    if(OC_STACK_OK != SendSRMResponse(ehRequest, ehRet, jsonStr))
    {
        OIC_LOG (ERROR, TAG, "SendSRMResponse failed in HandleDpairingGetRequest");
    }

    OICFree(jsonStr);

    return ehRet;
}

static OCEntityHandlerResult HandlePconfPostRequest (const OCEntityHandlerRequest * ehRequest)
{
    OCEntityHandlerResult ehRet = OC_EH_OK;
    OicSecPconf_t* newPconf = NULL;

    if (true == GetDoxmResourceData()->dpc)
    {
        // Convert JSON PCONF data into binary. This will also validate the PCONF data received.
        newPconf = JSONToPconfBin(((OCSecurityPayload*)ehRequest->payload)->securityData);
    }
    else
    {
        OIC_LOG (DEBUG, TAG, "DPC == false : Direct-Pairing Disabled");
        ehRet = OC_EH_ERROR;
    }

    if (newPconf)
    {
        // Check if valid Post request
        if ((true == newPconf->edp) && (0 < newPconf->prmLen) &&
                DP_PIN_LENGTH == strlen((const char*)newPconf->pin.val))
        {
            OicSecPrm_t *oldPrm = gPconf->prm;
            OicSecPdAcl_t *oldPdacl = gPconf->pdacls;

            // Update local PCONF with new PCONF
            gPconf->edp = true;
            memcpy(&gPconf->pin, &newPconf->pin, sizeof(OicDpPin_t));
            gPconf->prm = newPconf->prm;
            gPconf->prmLen = newPconf->prmLen;
            gPconf->pdacls = newPconf->pdacls;
            memcpy(&gPconf->rowner, &newPconf->rowner, sizeof(OicUuid_t));

            // to delete old value(prm, pdacl)
            newPconf->prm = oldPrm;
            newPconf->pdacls = oldPdacl;
        }
        else if (false == newPconf->edp)
        {
            gPconf->edp = false;
        }
        else
        {
            ehRet = OC_EH_ERROR;
        }

        // Update storage
        if(OC_EH_ERROR != ehRet && true == UpdatePersistentStorage(gPconf))
        {
            ehRet = OC_EH_RESOURCE_CREATED;
        }

        DeletePconfBinData(newPconf);
    }
    else
    {
        ehRet = OC_EH_ERROR;
    }

    // Send payload to request originator
    SendSRMResponse(ehRequest, ehRet, NULL);

    OIC_LOG_V (DEBUG, TAG, "%s RetVal %d", __func__ , ehRet);
    return ehRet;
}

/*
 * This internal method is the entity handler for PCONF resources and
 * will handle REST request (POST) for them.
 */
OCEntityHandlerResult PconfEntityHandler (OCEntityHandlerFlag flag,
                                        OCEntityHandlerRequest * ehRequest,
                                        void* callbackParameter)
{
    OIC_LOG(DEBUG, TAG, "Received request PconfEntityHandler");
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
                ehRet = HandlePconfGetRequest(ehRequest);
                break;

            case OC_REST_POST:
                ehRet = HandlePconfPostRequest(ehRequest);
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
 * This internal method is used to create '/oic/sec/pconf' resource.
 */
OCStackResult CreatePconfResource()
{
    OCStackResult ret;

    ret = OCCreateResource(&gPconfHandle,
                           OIC_RSRC_TYPE_SEC_PCONF,
                           OIC_MI_DEF,
                           OIC_RSRC_PCONF_URI,
                           PconfEntityHandler,
                           NULL,
                           OC_SECURE | OC_EXPLICIT_DISCOVERABLE);

    if (OC_STACK_OK != ret)
    {
        OIC_LOG (ERROR, TAG, "Unable to instantiate PCONF resource");
        DeInitPconfResource();
    }
    return ret;
}

/**
 * Get the default value.
 * @retval  the gDefaultPconf pointer;
 */
static OicSecPconf_t* GetPconfDefault()
{
    OIC_LOG (DEBUG, TAG, "GetPconfDefault");

    return &gDefaultPconf;
}

/**
 * This method is used by SRM to retrieve PCONF resource data..
 *
 * @retval  reference to @ref OicSecPconf_t, binary format of Pconf resource data
 */
const OicSecPconf_t* GetPconfResourceData()
{
    return gPconf;
}

/**
 * Initialize PCONF resource by loading data from persistent storage.
 *
 * @retval  OC_STACK_OK for Success, otherwise some error value
 */
OCStackResult InitPconfResource()
{
    OCStackResult ret = OC_STACK_ERROR;

    // Read PCONF resource from PS
    char* jsonSVRDatabase = GetSVRDatabase();

    if (jsonSVRDatabase)
    {
        // Convert JSON PCONF into binary format
        gPconf = JSONToPconfBin(jsonSVRDatabase);
    }

    if(!jsonSVRDatabase || !gPconf)
    {
        gPconf = GetPconfDefault();

        // device id from doxm
        OicUuid_t deviceId = {.id = {0}};
        OCStackResult ret = GetDoxmDeviceID( &deviceId);
        VERIFY_SUCCESS(TAG, OC_STACK_OK == ret, ERROR);
        memcpy(&gPconf->deviceID, &deviceId, sizeof(OicUuid_t));
    }
    VERIFY_NON_NULL(TAG, gPconf, ERROR);

    // Instantiate 'oic.sec.pconf'
    ret = CreatePconfResource();

exit:
    if (OC_STACK_OK != ret)
    {
        DeInitPconfResource();
    }
    OICFree(jsonSVRDatabase);
    return ret;
}

/**
 * Perform cleanup for PCONF resources.
 *
 * @return
 * OC_STACK_OK    - no error
 * OC_STACK_ERROR - stack process error
 *
 */
OCStackResult DeInitPconfResource()
{
    OCStackResult ret = OCDeleteResource(gPconfHandle);
    if(gPconf!= &gDefaultPconf)
    {
        DeletePconfBinData(gPconf);
    }
    gPconf = NULL;

    if(OC_STACK_OK == ret)
    {
        return OC_STACK_OK;
    }
    else
    {
        return OC_STACK_ERROR;
    }
}

/**
 * This method might be used to add a paired device id after direct-pairing process complete.
 *
 * @param pdeviceId ID of the paired device.
 *
 * @retval  OC_STACK_OK for Success, otherwise some error value
 */
OCStackResult AddPairedDevice(OicUuid_t *pdeviceId)
{
    if (!gPconf || !pdeviceId)
    {
        return OC_STACK_INVALID_PARAM;
    }


    OicUuid_t *prevList = gPconf->pddevs;
    gPconf->pddevs = (OicUuid_t *)OICCalloc(gPconf->pddevLen+1, sizeof(OicUuid_t));
    if(!gPconf->pddevs)
    {
        return OC_STACK_NO_MEMORY;
    }
    for (size_t i=0; i<gPconf->pddevLen; i++)
    {
        memcpy(&gPconf->pddevs[i], &prevList[i], sizeof(OicUuid_t));
    }

    // add new paired device id
    memcpy(&gPconf->pddevs[gPconf->pddevLen], pdeviceId, sizeof(OicUuid_t));
    gPconf->pddevLen++;

    // Update storage
    if(true != UpdatePersistentStorage(gPconf))
    {
        OIC_LOG (ERROR, TAG, "Fail to update pconf resource");
        return OC_STACK_ERROR;
    }

    OIC_LOG (ERROR, TAG, "Add paired device success");
    return OC_STACK_OK;
}

/**
 * This method might be used by PolicyEngine to retrieve PDACL for a Subject.
 *
 * @param subjectId ID of the subject for which PDACL is required.
 * @param savePtr is used internally by @ref GetACLResourceData to maintain index between
 *                successive calls for same subjectId.
 *
 * @retval  reference to @ref OicSecPdAcl_t if PDACL is found, else NULL
 */
const OicSecPdAcl_t* GetPdAclData(const OicUuid_t* subjectId, OicSecPdAcl_t **savePtr)
{
    OicSecPdAcl_t *pdacl = NULL;

    if ( NULL == subjectId)
    {
        return NULL;
    }

    /*
     * savePtr MUST point to NULL if this is the 'first' call to retrieve PDACL for
     * subjectID.
     */
    if (NULL == *savePtr)
    {
        pdacl = gPconf->pdacls;

        // Find if 'subjectID' is in paired device list.
        for(size_t i=0; i<gPconf->pddevLen; i++)
        {
            if (memcmp(&(gPconf->pddevs[i]), subjectId, sizeof(OicUuid_t)) == 0)
            {
                *savePtr = pdacl;
                return pdacl;
            }
        }
    }
    else
    {
        OicSecPdAcl_t *temp = NULL;

        /*
         * If this is a 'successive' call, search for location pointed by
         * savePtr and assign 'begin' to the next PDACL after it in the linked
         * list and start searching from there.
         */
        LL_FOREACH(gPconf->pdacls, temp)
        {
            if (temp == *savePtr)
            {
                pdacl = temp->next;
                *savePtr = pdacl;
                return pdacl;
            }
        }
    }

    // Cleanup in case no PDACL is found
    *savePtr = NULL;
    return NULL;
}

/**
 * This method return whether device is paired or not.
 *
 * @param pdeviceId Target device ID to find in paired list.
 * @retval  ture if device is already paired, else false
 */
bool IsPairedDevice(const OicUuid_t* pdeviceId)
{
    // Find if 'pdeviceId' is in paired device list.
    for(size_t i=0; i<gPconf->pddevLen; i++)
    {
        if (memcmp(&(gPconf->pddevs[i]), pdeviceId, sizeof(OicUuid_t)) == 0)
        {
            return true;
        }
    }
    return false;
}


