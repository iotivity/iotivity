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

#include <stdlib.h>
#include <string.h>
#include "ocstack.h"
#include "logger.h"
#include "oic_malloc.h"
#include "cJSON.h"
#include "base64.h"
#include "resourcemanager.h"
#include "aclresource.h"
#include "psinterface.h"
#include "utlist.h"
#include "srmresourcestrings.h"
#include "doxmresource.h"
#include "srmutility.h"
#include <stdlib.h>
#include <string.h>

#define TAG  PCF("SRM-ACL")

OicSecAcl_t        *gAcl = NULL;
static OCResourceHandle    gAclHandle = NULL;

void DeleteACLList(OicSecAcl_t* acl)
{
    if (acl)
    {
        OicSecAcl_t *aclTmp1 = NULL, *aclTmp2 = NULL;
        LL_FOREACH_SAFE(acl, aclTmp1, aclTmp2)
        {
            int i = 0;

            LL_DELETE(acl, aclTmp1);

            // Clean Resources
            for (i = 0; i < aclTmp1->resourcesLen; i++)
            {
                OICFree(aclTmp1->resources[i]);
            }
            OICFree(aclTmp1->resources);

            // Clean Owners
            OICFree(aclTmp1->owners);

            // Clean ACL node itself
            OICFree(aclTmp1);
        }
    }
}

/*
 * This internal method converts ACL data into JSON format.
 *
 * Note: Caller needs to invoke 'free' when finished done using
 * return string.
 */
char * BinToAclJSON(const OicSecAcl_t * acl)
{
    cJSON *jsonRoot = NULL;
    char *jsonStr = NULL;

    if (acl)
    {
        jsonRoot = cJSON_CreateObject();
        VERIFY_NON_NULL(TAG, jsonRoot, ERROR);

        cJSON *jsonAclArray = NULL;
        cJSON_AddItemToObject (jsonRoot, OIC_JSON_ACL_NAME, jsonAclArray = cJSON_CreateArray());
        VERIFY_NON_NULL(TAG, jsonAclArray, ERROR);

        while(acl)
        {
            char base64Buff[B64ENCODE_OUT_SAFESIZE(sizeof(((OicUuid_t*)0)->id)) + 1] = {};
            uint32_t outLen = 0;
            size_t inLen = 0;
            B64Result b64Ret = B64_OK;

            cJSON *jsonAcl = cJSON_CreateObject();

            // Subject -- Mandatory
            outLen = 0;
            if (memcmp(&(acl->subject), &WILDCARD_SUBJECT_ID, sizeof(OicUuid_t)) == 0)
            {
                inLen = WILDCARD_SUBJECT_ID_LEN;
            }
            else
            {
                inLen =  sizeof(OicUuid_t);
            }
            b64Ret = b64Encode(acl->subject.id, inLen, base64Buff,
                sizeof(base64Buff), &outLen);
            VERIFY_SUCCESS(TAG, b64Ret == B64_OK, ERROR);
            cJSON_AddStringToObject(jsonAcl, OIC_JSON_SUBJECT_NAME, base64Buff );

            // Resources -- Mandatory
            cJSON *jsonRsrcArray = NULL;
            cJSON_AddItemToObject (jsonAcl, OIC_JSON_RESOURCES_NAME, jsonRsrcArray = cJSON_CreateArray());
            VERIFY_NON_NULL(TAG, jsonRsrcArray, ERROR);
            for (int i = 0; i < acl->resourcesLen; i++)
            {
                cJSON_AddItemToArray (jsonRsrcArray, cJSON_CreateString(acl->resources[i]));
            }

            // Permissions -- Mandatory
            cJSON_AddNumberToObject (jsonAcl, OIC_JSON_PERMISSION_NAME, acl->permission);

            // Owners -- Mandatory
            cJSON *jsonOwnrArray = NULL;
            cJSON_AddItemToObject (jsonAcl, OIC_JSON_OWNERS_NAME, jsonOwnrArray = cJSON_CreateArray());
            VERIFY_NON_NULL(TAG, jsonOwnrArray, ERROR);
            for (int i = 0; i < acl->ownersLen; i++)
            {
                outLen = 0;

                b64Ret = b64Encode(acl->owners[i].id, sizeof(((OicUuid_t*)0)->id), base64Buff,
                    sizeof(base64Buff), &outLen);
                VERIFY_SUCCESS(TAG, b64Ret == B64_OK, ERROR);

                cJSON_AddItemToArray (jsonOwnrArray, cJSON_CreateString(base64Buff));
            }

            // Attach current acl node to Acl Array
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
OicSecAcl_t * JSONToAclBin(const char * jsonStr)
{
    OCStackResult ret = OC_STACK_ERROR;
    OicSecAcl_t * headAcl = NULL;
    OicSecAcl_t * prevAcl = NULL;
    cJSON *jsonRoot = NULL;
    cJSON *jsonAclArray = NULL;

    VERIFY_NON_NULL(TAG, jsonStr, ERROR);

    jsonRoot = cJSON_Parse(jsonStr);
    VERIFY_NON_NULL(TAG, jsonRoot, ERROR);

    jsonAclArray = cJSON_GetObjectItem(jsonRoot, OIC_JSON_ACL_NAME);
    VERIFY_NON_NULL(TAG, jsonAclArray, ERROR);

    if (cJSON_Array == jsonAclArray->type)
    {
        int numAcl = cJSON_GetArraySize(jsonAclArray);
        int idx = 0;

        VERIFY_SUCCESS(TAG, numAcl > 0, INFO);
        do
        {
            cJSON *jsonAcl = cJSON_GetArrayItem(jsonAclArray, idx);
            VERIFY_NON_NULL(TAG, jsonAcl, ERROR);

            OicSecAcl_t *acl = (OicSecAcl_t*)OICCalloc(1, sizeof(OicSecAcl_t));
            VERIFY_NON_NULL(TAG, acl, ERROR);

            headAcl = (headAcl) ? headAcl : acl;
            if (prevAcl)
            {
                prevAcl->next = acl;
            }

            size_t jsonObjLen = 0;
            cJSON *jsonObj = NULL;

            unsigned char base64Buff[sizeof(((OicUuid_t*)0)->id)] = {};
            uint32_t outLen = 0;
            B64Result b64Ret = B64_OK;

            // Subject -- Mandatory
            jsonObj = cJSON_GetObjectItem(jsonAcl, OIC_JSON_SUBJECT_NAME);
            VERIFY_NON_NULL(TAG, jsonObj, ERROR);
            VERIFY_SUCCESS(TAG, cJSON_String == jsonObj->type, ERROR);
            outLen = 0;
            b64Ret = b64Decode(jsonObj->valuestring, strlen(jsonObj->valuestring), base64Buff,
                        sizeof(base64Buff), &outLen);
            VERIFY_SUCCESS(TAG, (b64Ret == B64_OK && outLen <= sizeof(acl->subject.id)), ERROR);
            memcpy(acl->subject.id, base64Buff, outLen);

            // Resources -- Mandatory
            jsonObj = cJSON_GetObjectItem(jsonAcl, OIC_JSON_RESOURCES_NAME);
            VERIFY_NON_NULL(TAG, jsonObj, ERROR);
            VERIFY_SUCCESS(TAG, cJSON_Array == jsonObj->type, ERROR);

            acl->resourcesLen = cJSON_GetArraySize(jsonObj);
            VERIFY_SUCCESS(TAG, acl->resourcesLen > 0, ERROR);
            acl->resources = (char**)OICCalloc(acl->resourcesLen, sizeof(char*));
            VERIFY_NON_NULL(TAG, (acl->resources), ERROR);

            int idxx = 0;
            do
            {
                cJSON *jsonRsrc = cJSON_GetArrayItem(jsonObj, idxx);
                VERIFY_NON_NULL(TAG, jsonRsrc, ERROR);

                jsonObjLen = strlen(jsonRsrc->valuestring) + 1;
                acl->resources[idxx] = (char*)OICMalloc(jsonObjLen);
                VERIFY_NON_NULL(TAG, (acl->resources[idxx]), ERROR);
                strncpy(acl->resources[idxx], jsonRsrc->valuestring, jsonObjLen);
            } while ( ++idxx < acl->resourcesLen);

            // Permissions -- Mandatory
            jsonObj = cJSON_GetObjectItem(jsonAcl, OIC_JSON_PERMISSION_NAME);
            VERIFY_NON_NULL(TAG, jsonObj, ERROR);
            VERIFY_SUCCESS(TAG, cJSON_Number == jsonObj->type, ERROR);
            acl->permission = jsonObj->valueint;

            // Owners -- Mandatory
            jsonObj = cJSON_GetObjectItem(jsonAcl, OIC_JSON_OWNERS_NAME);
            VERIFY_NON_NULL(TAG, jsonObj, ERROR);
            VERIFY_SUCCESS(TAG, cJSON_Array == jsonObj->type, ERROR);

            acl->ownersLen = cJSON_GetArraySize(jsonObj);
            VERIFY_SUCCESS(TAG, acl->ownersLen > 0, ERROR);
            acl->owners = (OicUuid_t*)OICCalloc(acl->ownersLen, sizeof(OicUuid_t));
            VERIFY_NON_NULL(TAG, (acl->owners), ERROR);

            idxx = 0;
            do
            {
                cJSON *jsonOwnr = cJSON_GetArrayItem(jsonObj, idxx);
                VERIFY_NON_NULL(TAG, jsonOwnr, ERROR);
                VERIFY_SUCCESS(TAG, cJSON_String == jsonOwnr->type, ERROR);

                outLen = 0;
                b64Ret = b64Decode(jsonOwnr->valuestring, strlen(jsonOwnr->valuestring), base64Buff,
                            sizeof(base64Buff), &outLen);

                VERIFY_SUCCESS(TAG, (b64Ret == B64_OK && outLen <= sizeof(acl->owners[idxx].id)),
                                    ERROR);
                memcpy(acl->owners[idxx].id, base64Buff, outLen);
            } while ( ++idxx < acl->ownersLen);

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

static OCEntityHandlerResult HandleACLGetRequest (const OCEntityHandlerRequest * ehRequest)
{
    // Convert ACL data into JSON for transmission
    char* jsonStr = BinToAclJSON(gAcl);

    /*
     * A device should 'always' have a default ACL. Therefore,
     * jsonStr should never be NULL.
     */
    OCEntityHandlerResult ehRet = (jsonStr ? OC_EH_OK : OC_EH_ERROR);

    // Send response payload to request originator
    SendSRMResponse(ehRequest, ehRet, jsonStr);

    OICFree(jsonStr);

    OC_LOG_V (INFO, TAG, PCF("%s RetVal %d"), __func__ , ehRet);
    return ehRet;
}

static OCEntityHandlerResult HandleACLPostRequest (const OCEntityHandlerRequest * ehRequest)
{
    OCEntityHandlerResult ehRet = OC_EH_ERROR;

    // Convert JSON ACL data into binary. This will also validate the ACL data received.
    OicSecAcl_t* newAcl = JSONToAclBin(((OCSecurityPayload*)ehRequest->payload)->securityData);

    if (newAcl)
    {
        // Append the new ACL to existing ACL
        LL_APPEND(gAcl, newAcl);

        // Convert ACL data into JSON for update to persistent storage
        char *jsonStr = BinToAclJSON(gAcl);
        if (jsonStr)
        {
            cJSON *jsonAcl = cJSON_Parse(jsonStr);
            OICFree(jsonStr);

            if ((jsonAcl) &&
                (OC_STACK_OK == UpdateSVRDatabase(OIC_JSON_ACL_NAME, jsonAcl)))
            {
                ehRet = OC_EH_RESOURCE_CREATED;
            }
            cJSON_Delete(jsonAcl);
        }
    }

    // Send payload to request originator
    SendSRMResponse(ehRequest, ehRet, NULL);

    OC_LOG_V (INFO, TAG, PCF("%s RetVal %d"), __func__ , ehRet);
    return ehRet;
}

/*
 * This internal method is the entity handler for ACL resources and
 * will handle REST request (GET/PUT/POST/DEL) for them.
 */
OCEntityHandlerResult ACLEntityHandler (OCEntityHandlerFlag flag,
                                        OCEntityHandlerRequest * ehRequest,
                                        void* callbackParameter)
{
    OCEntityHandlerResult ehRet = OC_EH_ERROR;

    if (!ehRequest)
    {
        return ehRet;
    }

    if (flag & OC_REQUEST_FLAG)
    {
        // TODO :  Handle PUT and DEL methods
        OC_LOG (INFO, TAG, PCF("Flag includes OC_REQUEST_FLAG"));
        switch (ehRequest->method)
        {
            case OC_REST_GET:
                ehRet = HandleACLGetRequest(ehRequest);
                break;

            case OC_REST_POST:
                ehRet = HandleACLPostRequest(ehRequest);
                break;

            default:
                ehRet = OC_EH_ERROR;
                SendSRMResponse(ehRequest, ehRet, NULL);
        }
    }

    return ehRet;
}

/*
 * This internal method is used to create '/oic/sec/acl' resource.
 */
OCStackResult CreateACLResource()
{
    OCStackResult ret;

    ret = OCCreateResource(&gAclHandle,
                           OIC_RSRC_TYPE_SEC_ACL,
                           OIC_MI_DEF,
                           OIC_RSRC_ACL_URI,
                           ACLEntityHandler,
                           NULL,
                           OC_OBSERVABLE | OC_SECURE | OC_EXPLICIT_DISCOVERABLE);

    if (OC_STACK_OK != ret)
    {
        OC_LOG (FATAL, TAG, PCF("Unable to instantiate ACL resource"));
        DeInitACLResource();
    }
    return ret;
}

/*
 * This internal method is to retrieve the default ACL.
 * If SVR database in persistent storage got corrupted or
 * is not available for some reason, a default ACL is created
 * which allows user to initiate ACL provisioning again.
 */
OCStackResult  GetDefaultACL(OicSecAcl_t** defaultAcl)
{
    OCStackResult ret = OC_STACK_ERROR;

    OicUuid_t ownerId = {};

    /*
     * TODO In future, when new virtual resources will be added in OIC
     * specification, Iotivity stack should be able to add them in
     * existing SVR database. To support this, we need to add 'versioning'
     * mechanism in SVR database.
     */

    const char *rsrcs[] = {
        OC_RSRVD_WELL_KNOWN_URI,
        OC_RSRVD_DEVICE_URI,
        OC_RSRVD_PLATFORM_URI,
        OC_RSRVD_RESOURCE_TYPES_URI,
#ifdef WITH_PRESENCE
        OC_RSRVD_PRESENCE_URI,
#endif //WITH_PRESENCE
        OIC_RSRC_ACL_URI,
        OIC_RSRC_DOXM_URI,
        OIC_RSRC_PSTAT_URI,
    };

    if (!defaultAcl)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OicSecAcl_t *acl = (OicSecAcl_t *)OICCalloc(1, sizeof(OicSecAcl_t));
    VERIFY_NON_NULL(TAG, acl, ERROR);

    // Subject -- Mandatory
    memcpy(&(acl->subject), &WILDCARD_SUBJECT_ID, sizeof(acl->subject));

    // Resources -- Mandatory
    acl->resourcesLen = sizeof(rsrcs)/sizeof(rsrcs[0]);

    acl->resources = (char**)OICCalloc(acl->resourcesLen, sizeof(char*));
    VERIFY_NON_NULL(TAG, (acl->resources), ERROR);

    for (int i = 0; i <  acl->resourcesLen; i++)
    {
        size_t len = strlen(rsrcs[i]) + 1;
        acl->resources[i] = (char*)OICMalloc(len * sizeof(char));
        VERIFY_NON_NULL(TAG, (acl->resources[i]), ERROR);
        strncpy(acl->resources[i], rsrcs[i], len);
    }

    acl->permission = PERMISSION_READ;
    acl->periodsLen = 0;
    acl->periods = NULL;
    acl->recurrences = NULL;

    // Device ID is the owner of this default ACL
    ret = GetDoxmDeviceID( &ownerId);
    VERIFY_SUCCESS(TAG, OC_STACK_OK == ret, FATAL);

    acl->ownersLen = 1;
    acl->owners = (OicUuid_t*)OICMalloc(sizeof(OicUuid_t));
    VERIFY_NON_NULL(TAG, (acl->owners), ERROR);
    memcpy(acl->owners, &ownerId, sizeof(OicUuid_t));

    acl->next = NULL;

    *defaultAcl = acl;
    ret = OC_STACK_OK;

exit:

    if (ret != OC_STACK_OK)
    {
        DeleteACLList(acl);
        acl = NULL;
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
    OCStackResult ret = OC_STACK_ERROR;

    // Read ACL resource from PS
    char* jsonSVRDatabase = GetSVRDatabase();

    if (jsonSVRDatabase)
    {
        // Convert JSON ACL into binary format
        gAcl = JSONToAclBin(jsonSVRDatabase);
        OICFree(jsonSVRDatabase);
    }
    /*
     * If SVR database in persistent storage got corrupted or
     * is not available for some reason, a default ACL is created
     * which allows user to initiate ACL provisioning again.
     */
    if (!jsonSVRDatabase || !gAcl)
    {
        GetDefaultACL(&gAcl);
        // TODO Needs to update persistent storage
    }
    VERIFY_NON_NULL(TAG, gAcl, FATAL);

    // Instantiate 'oic.sec.acl'
    ret = CreateACLResource();

exit:
    if (OC_STACK_OK != ret)
    {
        DeInitACLResource();
    }
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
    gAclHandle = NULL;

    DeleteACLList(gAcl);
    gAcl = NULL;
}

/**
 * This method is used by PolicyEngine to retrieve ACL for a Subject.
 *
 * @param subjectId ID of the subject for which ACL is required.
 * @param savePtr is used internally by @ref GetACLResourceData to maintain index between
 *                successive calls for same subjectId.
 *
 * @retval  reference to @ref OicSecAcl_t if ACL is found, else NULL
 *
 * @note On the first call to @ref GetACLResourceData, savePtr should point to NULL
 */
const OicSecAcl_t* GetACLResourceData(const OicUuid_t* subjectId, OicSecAcl_t **savePtr)
{
    OicSecAcl_t *acl = NULL;
    OicSecAcl_t *begin = NULL;

    if ( NULL == subjectId)
    {
        return NULL;
    }

    /*
     * savePtr MUST point to NULL if this is the 'first' call to retrieve ACL for
     * subjectID.
     */
    if (NULL == *savePtr)
    {
        begin = gAcl;
    }
    else
    {
        /*
         * If this is a 'successive' call, search for location pointed by
         * savePtr and assign 'begin' to the next ACL after it in the linked
         * list and start searching from there.
         */
        LL_FOREACH(gAcl, acl)
        {
            if (acl == *savePtr)
            {
                begin = acl->next;
            }
        }
    }

    // Find the next ACL corresponding to the 'subjectID' and return it.
    LL_FOREACH(begin, acl)
    {
        if (memcmp(&(acl->subject), subjectId, sizeof(OicUuid_t)) == 0)
        {
            *savePtr = acl;
            return acl;
        }
    }

    // Cleanup in case no ACL is found
    *savePtr = NULL;
    return NULL;
}
