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
#include "aclresource.h"
#include "psinterface.h"
#include "utlist.h"
#include "srmresourcestrings.h"
#include <stdlib.h>
#include <string.h>

#define TAG  PCF("SRM-ACL")

#define VERIFY_SUCCESS(op, logLevel) { if ((op)) \
            {OC_LOG((logLevel), TAG, PCF(#op " failed!!")); goto exit;} }

#define VERIFY_NON_NULL(arg, logLevel) { if (!(arg)) { OC_LOG((logLevel), \
             TAG, PCF(#arg " is NULL")); goto exit; } }

OicSecAcl_t        *gAcl = NULL;
OCResourceHandle    gAclHandle = NULL;

static void DeleteACLList(OicSecAcl_t* acl)
{
    if (acl)
    {
        OicSecAcl_t *aclTmp1 = NULL, *aclTmp2 = NULL;
        LL_FOREACH_SAFE(acl, aclTmp1, aclTmp2)
        {
            int i = 0;

            LL_DELETE(acl, aclTmp1);

            /* Clean Subject */
            OCFree(aclTmp1->Subject);

            /* Clean Resources */
            for (i = 0; i < aclTmp1->ResourcesLen; i++)
            {
                OCFree(aclTmp1->Resources[i]);
            }
            OCFree(aclTmp1->Resources);

            /* Clean Owners */
            OCFree(aclTmp1->Owners);

            /* Clean ACL node itself */
            OCFree(aclTmp1);
        }
    }
}


/*
 * This internal method converts ACL data into JSON format.
 *
 * Note: Caller needs to invoke 'free' when finished done using
 * return string.
 */
static char * BinToJSON(OicSecAcl_t * acl)
{
    cJSON *jsonRoot = NULL;
    char *jsonStr = NULL;

    if (acl)
    {
        jsonRoot = cJSON_CreateObject();
        VERIFY_NON_NULL(jsonRoot, INFO);

        cJSON *jsonAclArray = NULL;
        cJSON_AddItemToObject (jsonRoot, OIC_JSON_ACL_NAME, jsonAclArray = cJSON_CreateArray());
        VERIFY_NON_NULL(jsonAclArray, INFO);

        while(acl)
        {
            cJSON *jsonAcl = cJSON_CreateObject();

            /* Subject -- Mandatory */
            cJSON_AddStringToObject(jsonAcl, OIC_JSON_SUBJECT_NAME,  acl->Subject);

            /* Resources -- Mandatory */
            cJSON *jsonRsrcArray = NULL;
            cJSON_AddItemToObject (jsonAcl, OIC_JSON_RESOURCES_NAME, jsonRsrcArray = cJSON_CreateArray());
            VERIFY_NON_NULL(jsonRsrcArray, INFO);
            for (int i = 0; i < acl->ResourcesLen; i++)
            {
                cJSON_AddItemToArray (jsonRsrcArray, cJSON_CreateString(acl->Resources[i]));
            }

            /* Permissions -- Mandatory */
            cJSON_AddNumberToObject (jsonAcl, OIC_JSON_PERMISSION_NAME, acl->Permission);

            /* Owners -- Mandatory */
            cJSON *jsonOwnrArray = NULL;
            cJSON_AddItemToObject (jsonAcl, OIC_JSON_OWNERS_NAME, jsonOwnrArray = cJSON_CreateArray());
            VERIFY_NON_NULL(jsonOwnrArray, INFO);
            for (int i = 0; i < acl->OwnersLen; i++)
            {
#if 0
                /* TODO Update once the format of OicSecSvc_t is finalized */
                cJSON_AddItemToArray (jsonOwnrArray, cJSON_CreateString(acl->Owners[i]));
#endif
                cJSON_AddItemToArray (jsonOwnrArray, cJSON_CreateString("Unknown"));
            }

            /* Attach current acl node to Acl Array */
            cJSON_AddItemToArray(jsonAclArray, jsonAcl);
            acl = acl->next;
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
 * This internal method converts JSON ACL into binary ACL.
 */
static OicSecAcl_t * JSONToBin(const char * jsonStr)
{
    OCStackResult ret = OC_STACK_ERROR;
    OicSecAcl_t * headAcl = NULL;
    OicSecAcl_t * prevAcl = NULL;

    cJSON *jsonRoot = cJSON_Parse(jsonStr);
    VERIFY_NON_NULL(jsonRoot, INFO);

    cJSON *jsonAclArray = cJSON_GetObjectItem(jsonRoot, OIC_JSON_ACL_NAME);
    VERIFY_NON_NULL(jsonAclArray, INFO);

    if (jsonAclArray->type == cJSON_Array)
    {
        int numAcl = cJSON_GetArraySize(jsonAclArray);
        int idx = 0;

        VERIFY_SUCCESS(numAcl > 0, INFO);
        do
        {
            cJSON *jsonAcl = cJSON_GetArrayItem(jsonAclArray, idx);
            VERIFY_NON_NULL(jsonAcl, INFO);

            OicSecAcl_t *acl = (OicSecAcl_t*)OCCalloc(1, sizeof(OicSecAcl_t));
            VERIFY_NON_NULL(acl, INFO);

            headAcl = (headAcl) ? headAcl : acl;
            if (prevAcl)
            {
                prevAcl->next = acl;
            }

            size_t jsonObjLen = 0;
            cJSON *jsonObj = NULL;
            /* Subject -- Mandatory */
            // TODO -- Confirmt he data type for Subject field
            jsonObj = cJSON_GetObjectItem(jsonAcl, OIC_JSON_SUBJECT_NAME);
            VERIFY_NON_NULL(jsonObj, ERROR);
            if (jsonObj->type == cJSON_String)
            {
                jsonObjLen = strlen(jsonObj->valuestring) + 1;
                acl->Subject = (char*)OCMalloc(jsonObjLen);
                VERIFY_NON_NULL(acl->Subject, INFO);
                strncpy(acl->Subject, jsonObj->valuestring, jsonObjLen);
            }

            /* Resources -- Mandatory */
            jsonObj = cJSON_GetObjectItem(jsonAcl, OIC_JSON_RESOURCES_NAME);
            VERIFY_NON_NULL(jsonObj, ERROR);
            if (jsonObj->type == cJSON_Array)
            {
                acl->ResourcesLen = cJSON_GetArraySize(jsonObj);
                int idxx = 0;

                VERIFY_SUCCESS(acl->ResourcesLen > 0, INFO);
                acl->Resources = (char**)OCCalloc(acl->ResourcesLen, sizeof(char*));
                VERIFY_NON_NULL(acl->Resources, FATAL);
                do
                {
                    cJSON *jsonRsrc = cJSON_GetArrayItem(jsonObj, idxx);
                    VERIFY_NON_NULL(jsonRsrc, ERROR);

                    jsonObjLen = strlen(jsonRsrc->valuestring) + 1;
                    acl->Resources[idxx] = (char*)OCMalloc(jsonObjLen);
                    VERIFY_NON_NULL(acl->Resources[idxx], FATAL);
                    strncpy(acl->Resources[idxx], jsonObj->valuestring, jsonObjLen);
                } while ( ++idxx < acl->ResourcesLen);
            }

            /* Permissions -- Mandatory */
            jsonObj = cJSON_GetObjectItem(jsonAcl, OIC_JSON_PERMISSION_NAME);
            VERIFY_NON_NULL(jsonObj, ERROR);
            if (jsonObj && jsonObj->type == cJSON_Number)
            {
                acl->Permission = jsonObj->valueint;
            }

            /* Owners -- Mandatory */
            jsonObj = cJSON_GetObjectItem(jsonAcl, OIC_JSON_OWNERS_NAME);
            VERIFY_NON_NULL(jsonObj, ERROR);
            if (jsonObj->type == cJSON_Array)
            {
                acl->OwnersLen = cJSON_GetArraySize(jsonObj);
                int idxx = 0;

                VERIFY_SUCCESS(acl->OwnersLen > 0, INFO);
                acl->Owners = (OicSecSvc_t*)OCCalloc(acl->OwnersLen, sizeof(OicSecSvc_t));
                VERIFY_NON_NULL(acl->Owners, FATAL);
                do
                {
                    cJSON *jsonOwnr = cJSON_GetArrayItem(jsonObj, idxx);
                    VERIFY_NON_NULL(jsonOwnr, ERROR);
#if 0
                    /* TODO Update once the format of OicSecSvc_t is finalized */
                    jsonObjLen = strlen(jsonOwnr->valuestring) + 1;
                    acl->Owners[idxx] = (char*)OCMalloc(jsonObjLen);
                    VERIFY_NON_NULL(acl->Owners[idxx], FATAL);
                    strncpy(acl->Owners[idxx], jsonObj->valuestring, jsonObjLen);
#endif
                } while ( ++idxx < acl->OwnersLen);
            }

            prevAcl = acl;
        } while( ++idx < numAcl);
    }

    ret = OC_STACK_OK;

exit:
    cJSON_Delete(jsonRoot);
    if (OC_STACK_OK != ret)
    {
        DeleteACLList(headAcl);
        headAcl = NULL;
    }
    return headAcl;
}


/*
 * This internal method is the entity handler for ACL resources.
 */
static OCEntityHandlerResult ACLEntityHandler (OCEntityHandlerFlag flag,
                                        OCEntityHandlerRequest * ehRequest)
{
    OCEntityHandlerResult ret = OC_EH_ERROR;
    char *jsonRsp = NULL;

    /*
     * This method will handle REST request (GET/PUT/POST/DEL) for
     * virtual resources such as: /oic/sec/cred, /oic/sec/acl etc
     */

    if (flag & OC_REQUEST_FLAG)
    {
        /* TODO :  Handle PUT/POST/DEL methods */
        OC_LOG (INFO, TAG, PCF("Flag includes OC_REQUEST_FLAG"));
        if (OC_REST_GET == ehRequest->method)
        {
            /* Convert ACL data into JSON for transmission */
            jsonRsp = BinToJSON(gAcl);

            /* Send payload to request originator */
            ret = (SendSRMResponse(ehRequest, jsonRsp) == OC_STACK_OK ?
                   OC_EH_OK : OC_EH_ERROR);
        }
    }
    OCFree(jsonRsp);
    return ret;
}


/*
 * This internal method is used to create '/oic/sec/acl' resource.
 */
static OCStackResult CreateACLResource()
{
    OCStackResult ret;

    /* TODO : Does these resources needs to be OC_DISCOVERABLE */
    ret = OCCreateResource(&gAclHandle,
                           OIC_RSRC_TYPE_SEC_ACL,
                           "oic.mi.def",
                           OIC_RSRC_ACL_URI,
                           ACLEntityHandler,
                           OC_DISCOVERABLE | OC_OBSERVABLE);

    if (ret != OC_STACK_OK)
    {
        OC_LOG (FATAL, TAG, PCF("Unable to instantiate ACL resource"));
        DeInitACLResource();
    }
    return ret;
}

/**
 * Initialize ACL resource by loading data from persistent storage.
 *
 * @retval  OC_STACK_OK for Success, otherwise some error value
 */
OCStackResult InitACLResource()
{
    OCStackResult ret;

    /* Read ACL resource from PS */
    char* jsonSVRDatabase = GetSVRDatabase();
    VERIFY_NON_NULL(jsonSVRDatabase, FATAL);

    /* Convert JSON ACL into binary format */
    gAcl = JSONToBin(jsonSVRDatabase);
    VERIFY_NON_NULL(gAcl, FATAL);

    /* Instantiate 'oic.sec.acl' */
    ret = CreateACLResource();

exit:
    OCFree(jsonSVRDatabase);
    return ret;
}

/**
 * Perform cleanup for ACL resources.
 *
 * @retval  none
 */
void DeInitACLResource()
{
    OCDeleteResource(gAclHandle);
    DeleteACLList(gAcl);
    gAcl = NULL;
}

/**
 * This method is used by PolicyEngine to retrieve ACL for a Subject.
 *
 * @param subjectId - ID of the subject for which ACL is required.
 * @param subjectIdLen - length of subjectId field.
 *
 * @retval  reference to @ref OicSecAcl_t if ACL is found, else NULL
 */
const OicSecAcl_t* GetACLResourceData(const unsigned char* subjectId, size_t subjectIdLen)
{
    OicSecAcl_t *acl = NULL;

    if ( (NULL == subjectId) || (0 == subjectIdLen))
    {
        return NULL;
    }

    LL_FOREACH(gAcl, acl)
    {
        /* TODO : Need to synch on 'Subject' data type */
        // if (memcmp(acl->subject, subjectId, subjectIdLen) == 0)
        //     return acl;
    }
    return NULL;
}
