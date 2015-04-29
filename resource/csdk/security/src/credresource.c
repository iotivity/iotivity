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
#include "psinterface.h"
#include "utlist.h"
#include "srmresourcestrings.h"
#include "credresource.h"
#include "ocrandom.h"
#include "base64.h"
#include <stdlib.h>
#include <string.h>

#define TAG  PCF("SRM-CREDL")

#define VERIFY_SUCCESS(op, logLevel) { if (!(op)) \
            {OC_LOG((logLevel), TAG, PCF(#op " failed!!")); goto exit;} }

#define VERIFY_NON_NULL(arg, logLevel) { if (!(arg)) { OC_LOG((logLevel), \
             TAG, PCF(#arg " is NULL")); goto exit; } }

static OicSecCred_t        *gCred = NULL;
static OCResourceHandle    gCredHandle = NULL;

void DeleteCredList(OicSecCred_t* cred)
{
    if (cred)
    {
        OicSecCred_t *credTmp1 = NULL, *credTmp2 = NULL;
        LL_FOREACH_SAFE(cred, credTmp1, credTmp2)
        {
            LL_DELETE(cred, credTmp1);

            //Note: Need further clarification on roleID data type
#if 0
            //Clean roleIds
            OCFree(credTmp1->roleIds);
#endif

            //Clean PublicData
            OCFree(credTmp1->publicData.data);

            //Clean PrivateData
            OCFree(credTmp1->privateData.data);

            //Clean Period
            OCFree(credTmp1->period);

            //Clean Owners
            OCFree(credTmp1->owners);

            //Clean Cred node itself
            OCFree(credTmp1);
        }
    }
}

/**
 * This function converts credential data into JSON format.
 * Caller needs to invoke 'free' when done using
 * returned string.
 * @param cred  pointer to instance of OicSecCred_t structure.
 *
 * @retval
 *      pointer to JSON credential representation - if credential for subjectId found
 *      NULL                                      - if credential for subjectId not found
 */
char * BinToCredJSON(const OicSecCred_t * cred)
{
    cJSON *jsonRoot = NULL;
    char *jsonStr = NULL;

    if (cred)
    {
        char base64Buff[B64ENCODE_OUT_SAFESIZE(sizeof(((OicUuid_t*)0)->id)) + 1] = {};
        uint32_t outLen = 0;
        B64Result b64Ret = B64_OK;

        jsonRoot = cJSON_CreateObject();
        VERIFY_NON_NULL(jsonRoot, ERROR);

        cJSON *jsonCredArray = NULL;
        cJSON_AddItemToObject(jsonRoot, OIC_JSON_CRED_NAME,
                jsonCredArray = cJSON_CreateArray());
        VERIFY_NON_NULL(jsonCredArray, ERROR);

        while(cred)
        {
            cJSON *jsonCred = cJSON_CreateObject();
            VERIFY_NON_NULL(jsonCred, ERROR);

            //CredID -- Mandatory
            cJSON_AddNumberToObject(jsonCred, OIC_JSON_CREDID_NAME, (int)cred->credId);

            //Subject -- Mandatory
            outLen = 0;
            memset(base64Buff, 0, sizeof(base64Buff));
            b64Ret = b64Encode(cred->subject.id, sizeof(cred->subject.id), base64Buff,
                   sizeof(base64Buff), &outLen);
            VERIFY_SUCCESS(b64Ret == B64_OK, ERROR);
            cJSON_AddStringToObject(jsonCred, OIC_JSON_SUBJECT_NAME, base64Buff);

            //Note: Need further clarification on roleID data type
#if 0
            //RoleId -- Not Mandatory
            if(cred->roleIdsLen > 0)
            {
                cJSON *jsonRoleIdsArray = NULL;
                cJSON_AddItemToObject (jsonCred, OIC_JSON_ROLEIDS_NAME,
                                         jsonRoleIdsArray = cJSON_CreateArray());
                VERIFY_NON_NULL(jsonRoleIdsArray, ERROR);
                for (size_t i = 0; i < cred->roleIdsLen; i++)
                {
                    cJSON_AddItemToArray (jsonRoleIdsArray,
                            cJSON_CreateString((char *)cred->roleIds[i].id));
                }
            }
#endif

            //CredType -- Mandatory
            cJSON_AddNumberToObject(jsonCred, OIC_JSON_CREDTYPE_NAME,(int)cred->credType);

#if 0
            //PublicData -- Not Mandatory
            if(cred->publicData.data)
            {
                cJSON_AddStringToObject(jsonCred, OIC_JSON_PUBLICDATA_NAME, cred->publicData.data);
            }
#endif
            //PrivateData -- Not Mandatory
            if(cred->privateData.data)
            {
                cJSON_AddStringToObject(jsonCred, OIC_JSON_PRIVATEDATA_NAME, cred->privateData.data);
            }

            //Period -- Not Mandatory
            if(cred->period)
            {
                cJSON_AddStringToObject(jsonCred, OIC_JSON_PERIOD_NAME,
                                        cred->period);
            }

            //Owners -- Mandatory
            cJSON *jsonOwnrArray = NULL;
            cJSON_AddItemToObject (jsonCred, OIC_JSON_OWNERS_NAME,
                                             jsonOwnrArray = cJSON_CreateArray());
            VERIFY_NON_NULL(jsonOwnrArray, ERROR);
            for (size_t i = 0; i < cred->ownersLen; i++)
            {
                outLen = 0;
                memset(base64Buff, 0, sizeof(base64Buff));
                b64Ret = b64Encode(cred->owners[i].id, sizeof(cred->owners[i].id),
                        base64Buff, sizeof(base64Buff), &outLen);
                VERIFY_SUCCESS(b64Ret == B64_OK, ERROR);
                cJSON_AddItemToArray (jsonOwnrArray,
                                       cJSON_CreateString((char *)(base64Buff)));
            }

            /* Attach current cred node to cred Array */
            cJSON_AddItemToArray(jsonCredArray, jsonCred);
            cred = cred->next;
        }

        jsonStr = cJSON_PrintUnformatted(jsonRoot);
    }

exit:
    if (jsonRoot)
    {
        cJSON_Delete(jsonRoot);
    }
    return jsonStr;
}

/*
 * This internal method converts JSON cred into binary cred.
 */
OicSecCred_t * JSONToCredBin(const char * jsonStr)
{
    OCStackResult ret = OC_STACK_ERROR;
    OicSecCred_t * headCred = NULL;
    OicSecCred_t * prevCred = NULL;

    cJSON *jsonRoot = cJSON_Parse(jsonStr);
    VERIFY_NON_NULL(jsonRoot, ERROR);

    cJSON *jsonCredArray = cJSON_GetObjectItem(jsonRoot, OIC_JSON_CRED_NAME);
    VERIFY_NON_NULL(jsonCredArray, ERROR);
    if (cJSON_Array == jsonCredArray->type)
    {
        int numCred = cJSON_GetArraySize(jsonCredArray);
        int idx = 0;

        unsigned char base64Buff[sizeof(((OicUuid_t*)0)->id)] = {};
        uint32_t outLen = 0;
        B64Result b64Ret = B64_OK;

        VERIFY_SUCCESS(numCred > 0, ERROR);
        do
        {
            cJSON *jsonCred = cJSON_GetArrayItem(jsonCredArray, idx);
            VERIFY_NON_NULL(jsonCred, ERROR);

            OicSecCred_t *cred = (OicSecCred_t*)OCCalloc(1, sizeof(OicSecCred_t));
            VERIFY_NON_NULL(cred, ERROR);

            headCred = (headCred) ? headCred : cred;
            if (prevCred)
            {
                prevCred->next = cred;
            }
            size_t jsonObjLen = 0;
            cJSON *jsonObj = NULL;

            //CredId -- Mandatory
            jsonObj = cJSON_GetObjectItem(jsonCred, OIC_JSON_CREDID_NAME);
            VERIFY_NON_NULL(jsonObj, ERROR);
            VERIFY_SUCCESS(cJSON_Number == jsonObj->type, ERROR)
            cred->credId = jsonObj->valueint;

            //subject -- Mandatory
            jsonObj = cJSON_GetObjectItem(jsonCred, OIC_JSON_SUBJECT_NAME);
            VERIFY_NON_NULL(jsonObj, ERROR);
            VERIFY_SUCCESS(cJSON_String == jsonObj->type, ERROR)
            outLen = 0;
            memset(base64Buff, 0, sizeof(base64Buff));
            b64Ret = b64Decode(jsonObj->valuestring, strlen(jsonObj->valuestring),
                    base64Buff, sizeof(base64Buff), &outLen);
            VERIFY_SUCCESS((b64Ret == B64_OK) && (outLen <= sizeof(cred->subject.id)),
                           ERROR);
            memcpy(cred->subject.id, base64Buff, outLen);

            //Note: Need further clarification on roleID data type
#if 0
            //RoleIds -- Not Mandatory
            jsonObj = cJSON_GetObjectItem(jsonCred, OIC_JSON_ROLEIDS_NAME);
            if (jsonObj && cJSON_Array == jsonObj->type)
            {
                cred->roleIdsLen = cJSON_GetArraySize(jsonObj);
                VERIFY_SUCCESS(cred->roleIdsLen > 0, ERROR);
                cred->roleIds = (OicSecRole_t *)OCCalloc(cred->roleIdsLen, sizeof(OicSecRole_t));
                VERIFY_NON_NULL(cred->roleIds, ERROR);
                for(size_t i = 0; i < cred->roleIdsLen; i++)
                {
                    cJSON *jsonRole = cJSON_GetArrayItem(jsonObj, i);
                    VERIFY_NON_NULL(jsonRole, ERROR);
                    jsonObjLen = strlen(jsonRole->valuestring) + 1;
                    strncpy((char *)cred->roleIds[i].id, (char *)jsonRole->valuestring, jsonObjLen);
                }
            }
#endif

            //CredType -- Mandatory
            jsonObj = cJSON_GetObjectItem(jsonCred, OIC_JSON_CREDTYPE_NAME);
            VERIFY_NON_NULL(jsonObj, ERROR);
            VERIFY_SUCCESS(cJSON_Number == jsonObj->type, ERROR)
            cred->credType = jsonObj->valueint;

#if 0
            //PublicData -- Not Mandatory
            jsonObj = cJSON_GetObjectItem(jsonCred, OIC_JSON_PUBLICDATA_NAME);
            if(jsonObj && cJSON_String == jsonObj->type)
            {
                jsonObjLen = strlen(jsonObj->valuestring) + 1;
                cred->publicData.data = (char *)OCMalloc(jsonObjLen);
                VERIFY_NON_NULL(cred->publicData.data, ERROR);
                strncpy((char *)cred->publicData.data, (char *)jsonObj->valuestring, jsonObjLen);
            }
#endif

            //PrivateData -- Not Mandatory
            jsonObj = cJSON_GetObjectItem(jsonCred, OIC_JSON_PRIVATEDATA_NAME);
            if(jsonObj && cJSON_String == jsonObj->type)
            {
                jsonObjLen = strlen(jsonObj->valuestring) + 1;
                cred->privateData.data = (char *)OCMalloc(jsonObjLen);
                VERIFY_NON_NULL(cred->privateData.data, ERROR);
                strncpy((char *)cred->privateData.data, (char *)jsonObj->valuestring, jsonObjLen);
            }

            //Period -- Not Mandatory
            jsonObj = cJSON_GetObjectItem(jsonCred, OIC_JSON_PERIOD_NAME);
            if(jsonObj && cJSON_String == jsonObj->type)
            {
                jsonObjLen = strlen(jsonObj->valuestring) + 1;
                cred->period = (char *)OCMalloc(jsonObjLen);
                VERIFY_NON_NULL(cred->period, ERROR);
                strncpy(cred->period, jsonObj->valuestring, jsonObjLen);
            }

            //Owners -- Mandatory
            jsonObj = cJSON_GetObjectItem(jsonCred, OIC_JSON_OWNERS_NAME);
            VERIFY_NON_NULL(jsonObj, ERROR);
            VERIFY_SUCCESS(cJSON_Array == jsonObj->type, ERROR)
            cred->ownersLen = cJSON_GetArraySize(jsonObj);
            VERIFY_SUCCESS(cred->ownersLen > 0, ERROR);
            cred->owners = (OicUuid_t*)OCCalloc(cred->ownersLen, sizeof(OicUuid_t));
            VERIFY_NON_NULL(cred->owners, ERROR);
            for(size_t i = 0; i < cred->ownersLen; i++)
            {
                cJSON *jsonOwnr = cJSON_GetArrayItem(jsonObj, i);
                VERIFY_NON_NULL(jsonOwnr, ERROR);
                VERIFY_SUCCESS(cJSON_String == jsonOwnr->type, ERROR);
                /* TODO Update once the format of OicSecSvc_t is finalized */
                outLen = 0;
                memset(base64Buff, 0, sizeof(base64Buff));
                b64Ret = b64Decode(jsonOwnr->valuestring, strlen(jsonOwnr->valuestring),
                         base64Buff, sizeof(base64Buff), &outLen);
                VERIFY_SUCCESS((b64Ret == B64_OK) &&
                        (outLen <= sizeof(cred->owners[i].id)), ERROR);
                memcpy(cred->owners[i].id, base64Buff, outLen);
            }
            prevCred = cred;
        } while( ++idx < numCred);
    }

    ret = OC_STACK_OK;

exit:
    cJSON_Delete(jsonRoot);
    if (OC_STACK_OK != ret)
    {
        DeleteCredList(headCred);
        headCred = NULL;
    }
    return headCred;
}

/**
 * This function generates the bin credential data.
 *
 * @param subject pointer to subject of this credential.
 * @param credType credential type.
 * @param publicData public data such as public key.
 * @param privateData private data such as private key.
 * @param ownersLen length of owners array
 * @param owners array of owners.
 *
 * @retval
 *      pointer to instance of OicSecCred_t  - success
 *      NULL                                 - error
 */
OicSecCred_t * GenerateCredential(const OicUuid_t * subject, OicSecCredType_t credType,
                                 const char * publicData, const char * privateData,
                                 size_t ownersLen, const OicUuid_t * owners)
{
    OCStackResult ret = OC_STACK_ERROR;

    OicSecCred_t *cred = (OicSecCred_t*)OCCalloc(1, sizeof(OicSecCred_t));
    VERIFY_NON_NULL(cred, ERROR);

    //TODO:Need more clarification on credId
    OCFillRandomMem((uint8_t*)&cred->credId, sizeof(cred->credId));

    VERIFY_NON_NULL(subject, ERROR);
    memcpy(cred->subject.id, subject->id , sizeof(cred->subject.id));

    //TODO: check credType has one of the values {0, 1, 2, 4, 6, 8, 16}
    cred->credType = credType;

#if 0
    if(publicData)
    {
        cred->publicData.data = (char *)OCMalloc(strlen(publicData)+1);
        VERIFY_NON_NULL(cred->publicData.data, ERROR);
        strncpy((char *)cred->publicData.data, publicData, strlen(publicData)+1);
    }
#endif

    if(privateData)
    {
        cred->privateData.data = (char *)OCMalloc(strlen(privateData)+1);
        VERIFY_NON_NULL(cred->privateData.data, ERROR);
        strncpy((char *)cred->privateData.data, privateData, strlen(privateData)+1);
    }

    VERIFY_SUCCESS(ownersLen > 0, ERROR);
    cred->ownersLen = ownersLen;

    cred->owners = (OicUuid_t*)OCCalloc(cred->ownersLen, sizeof(OicUuid_t));
    VERIFY_NON_NULL(cred->owners, ERROR);
    for(size_t i = 0; i < cred->ownersLen; i++)
    {
        memcpy(cred->owners[i].id, owners[i].id, sizeof(cred->owners[i].id));
    }

    ret = OC_STACK_OK;
exit:
    if (OC_STACK_OK != ret)
    {
        DeleteCredList(cred);
        cred = NULL;
    }
    return cred;
}

/**
 * This function adds the new cred to the credential list.
 *
 * @param cred pointer to new credential.
 *
 * @retval
 *      OC_STACK_OK     - cred not NULL and persistent storage gets updated
 *      OC_STACK_ERROR  - cred is NULL or fails to update persistent storage
 */
OCStackResult AddCredential(OicSecCred_t * newCred)
{
    OCStackResult ret = OC_STACK_ERROR;

    if(NULL == newCred)
    {
        return OC_STACK_ERROR;
    }

    //Append the new Cred to existing list
    LL_APPEND(gCred, newCred);

    //Convert CredList to JSON and update the persistent Storage
    char * jsonStr = BinToCredJSON(gCred);

    if(jsonStr)
    {
        cJSON *jsonCred = cJSON_Parse(jsonStr);
        OCFree(jsonStr);

        if((jsonCred) && (OC_STACK_OK == UpdateSVRDatabase(OIC_JSON_CRED_NAME, jsonCred)))
        {
            ret = OC_STACK_OK;
        }
        cJSON_Delete(jsonCred);
    }

    return ret;
}

static OCEntityHandlerResult HandlePostRequest(const OCEntityHandlerRequest * ehRequest)
{
    OCEntityHandlerResult ret = OC_EH_ERROR;

    //Get binary representation of json
    OicSecCred_t * cred  = JSONToCredBin((char *)ehRequest->reqJSONPayload);

    if(cred)
    {
        //Append the new Cred to existing list
        ret = (OC_STACK_OK == AddCredential(cred))? OC_EH_OK : OC_EH_ERROR;
    }

    return ret;
}

/*
 * This internal method is the entity handler for Cred resources
 * to handle REST request (PUT/POST/DEL)
 */
OCEntityHandlerResult CredEntityHandler (OCEntityHandlerFlag flag,
                                        OCEntityHandlerRequest * ehRequest)
{
    OCEntityHandlerResult ret = OC_EH_ERROR;

    if(!ehRequest)
    {
        return OC_EH_ERROR;
    }
    if (flag & OC_REQUEST_FLAG)
    {
        OC_LOG (INFO, TAG, PCF("Flag includes OC_REQUEST_FLAG"));
        //TODO :  Handle PUT/DEL methods
        switch(ehRequest->method)
        {
            case OC_REST_GET:
                break;
            case OC_REST_POST:
                ret = HandlePostRequest(ehRequest);
                break;
            default:
                break;
        }
    }

    //Send payload to request originator
    ret = (SendSRMResponse(ehRequest, ret, NULL) == OC_STACK_OK ?
                       OC_EH_OK : OC_EH_ERROR);

    return ret;
}

/*
 * This internal method is used to create '/oic/sec/Cred' resource.
 */
OCStackResult CreateCredResource()
{
    OCStackResult ret;

    //TODO : Does these resources needs to be OC_DISCOVERABLE
    ret = OCCreateResource(&gCredHandle,
                           OIC_RSRC_TYPE_SEC_CRED,
                           OIC_MI_DEF,
                           OIC_RSRC_CRED_URI,
                           CredEntityHandler,
                           OC_DISCOVERABLE);

    if (OC_STACK_OK != ret)
    {
        OC_LOG (FATAL, TAG, PCF("Unable to instantiate Cred resource"));
        DeInitCredResource();
    }
    return ret;
}

/**
 * Get the default value
 * @retval  NULL for now. Update it when we finalize the default info.
 */
static OicSecCred_t* GetCredDefault()
{
    //TODO: return NULL for the default cred for now. Update it when we finalize the default info.
    return NULL;
}

/**
 * Initialize Cred resource by loading data from persistent storage.
 *
 * @retval
 *     OC_STACK_OK    - no errors
 *     OC_STACK_ERROR - stack process error
 */
OCStackResult InitCredResource()
{
    OCStackResult ret = OC_STACK_ERROR;

    //Read Cred resource from PS
    char* jsonSVRDatabase = GetSVRDatabase();

    if (jsonSVRDatabase)
    {
        //Convert JSON Cred into binary format
        gCred = JSONToCredBin(jsonSVRDatabase);
    }
    /*
     * If SVR database in persistent storage got corrupted or
     * is not available for some reason, a default Cred is created
     * which allows user to initiate Cred provisioning again.
     */
    if (!jsonSVRDatabase || !gCred)
    {
        gCred = GetCredDefault();
    }
    //Instantiate 'oic.sec.cred'
    ret = CreateCredResource();
    OCFree(jsonSVRDatabase);
    return ret;
}

/**
 * Perform cleanup for Cred resources.
 *
 * @return
 *     OC_STACK_OK              - no errors
 *     OC_STACK_ERROR           - stack process error
 *     OC_STACK_NO_RESOURCE     - resource not found
 *     OC_STACK_INVALID_PARAM   - invalid param
 */
OCStackResult DeInitCredResource()
{
    OCStackResult result = OCDeleteResource(gCredHandle);
    DeleteCredList(gCred);
    gCred = NULL;
    return result;
}

/**
 * This method is used by tinydtls/SRM to retrieve credential for given Subject.
 *
 * @param subject - subject for which credential is required.
 *
 * @retval
 *     reference to OicSecCred_t - if credential is found
 *     NULL                      - if credential not found
 */
const OicSecCred_t* GetCredResourceData(const OicUuid_t* subject)
{
    OicSecCred_t *cred = NULL;

    if ( NULL == subject)
    {
        return NULL;
    }

    LL_FOREACH(gCred, cred)
    {
        /* TODO : Need to synch on 'Subject' data type */
        if(memcmp(cred->subject.id, subject->id, sizeof(subject->id)) == 0)
        {
             return cred;
        }
    }
    return NULL;
}
