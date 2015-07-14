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

#ifdef WITH_ARDUINO
#include <string.h>
#else
#include <strings.h>
#endif

#include <stdlib.h>
#include "ocstack.h"
#include "logger.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "cJSON.h"
#include "base64.h"
#include "resourcemanager.h"
#include "aclresource.h"
#include "psinterface.h"
#include "utlist.h"
#include "srmresourcestrings.h"
#include "doxmresource.h"
#include "srmutility.h"
#include "ocserverrequest.h"
#include "ocpayload.h"
#include "ocpayloadcbor.h"
#include "payload_logging.h"

#define TAG  PCF("SRM-ACL")

OicSecAcl_t               *gAcl = NULL;
static OCResourceHandle    gAclHandle = NULL;

/**
 * This function frees OicSecAcl_t object's fields and object itself.
 */
static void FreeACE(OicSecAcl_t *ace)
{
    size_t i;
    if(NULL == ace)
    {
        OC_LOG (INFO, TAG, PCF("Invalid Parameter"));
        return;
    }

    // Clean Resources
    for (i = 0; i < ace->resourcesLen; i++)
    {
        OICFree(ace->resources[i]);
    }
    OICFree(ace->resources);

    //Clean Period & Recurrence
    for(i = 0; i < ace->prdRecrLen; i++)
    {
        OICFree(ace->periods[i]);
        OICFree(ace->recurrences[i]);
    }
    OICFree(ace->periods);
    OICFree(ace->recurrences);

    // Clean Owners
    OICFree(ace->owners);

    // Clean ACL node itself
    OICFree(ace);
}

void DeleteACLList(OicSecAcl_t* acl)
{
    if (acl)
    {
        OicSecAcl_t *aclTmp1 = NULL;
        OicSecAcl_t *aclTmp2 = NULL;
        LL_FOREACH_SAFE(acl, aclTmp1, aclTmp2)
        {
            LL_DELETE(acl, aclTmp1);
            FreeACE(aclTmp1);
        }
    }
}

/**
 * This function converts ACL data into OCRepPayload.
 * Caller needs to invoke 'free' when done using
 */
OCRepPayload* AclToPayload(const OicSecAcl_t * acl)
{
    if(NULL == acl)
    {
        return NULL;
    }
    size_t dimensions[MAX_REP_ARRAY_DEPTH] = {0};
    OCRepPayload* headPayload = NULL;
    OCRepPayload* prevPayload = NULL;
    bool result = false;
    while (acl)
    {
        OCRepPayload* payload = OCRepPayloadCreate();
        if (!payload)
        {
            OC_LOG(ERROR, TAG, PCF("Failed to allocate Payload"));
            return NULL;
        }
        payload->next = NULL;
        headPayload = (headPayload) ? headPayload : payload;
        OCRepPayloadAppend(prevPayload, payload);
        size_t inLen = 0;

        // Subject -- Mandatory
        if (memcmp(&(acl->subject), &WILDCARD_SUBJECT_ID, sizeof(OicUuid_t)) == 0)
        {
            inLen = WILDCARD_SUBJECT_ID_LEN;
        }
        else
        {
            inLen = sizeof(OicUuid_t);
        }
        dimensions[0] = inLen;
        dimensions[1] = 0;
        dimensions[2] = 0;
        result = OCRepPayloadSetIntArray(payload, OIC_JSON_SUBJECT_NAME,
                (int64_t*) acl->subject.id, dimensions);
        VERIFY_SUCCESS(TAG, result, ERROR);
        //Resources
        dimensions[0] = acl->resourcesLen;
        dimensions[1] = 0;
        dimensions[2] = 0;
        OCRepPayloadSetStringArray(payload, OIC_JSON_RESOURCES_NAME, (const char**) acl->resources,
                dimensions);
        // Permissions -- Mandatory
        result = OCRepPayloadSetPropInt(payload, OIC_JSON_PERMISSION_NAME, acl->permission);
        VERIFY_SUCCESS(TAG, result, ERROR);
        //Owners
        dimensions[0] = acl->ownersLen;
        dimensions[1] = SVR_UUID_LENGTH;
        dimensions[2] = 0;
        result = OCRepPayloadSetIntArray(payload, OIC_JSON_OWNERS_NAME, (int64_t*) &acl->owners,
                dimensions);
        VERIFY_SUCCESS(TAG, result, ERROR);
        acl = acl->next;
        prevPayload = payload;

    }
    result = OCRepPayloadSetUri(headPayload, OIC_RSRC_ACL_URI);
    OC_LOG_PAYLOAD(INFO, TAG, (OCPayload*)headPayload);
exit:
    if(!result)
    {
        OCRepPayloadDestroy(headPayload);
        headPayload = NULL;
    }
    return headPayload;
}

/**
 * This function converts OCRepPayload to ACL data.
 * Caller needs to invoke 'free' when done using
 *
 */
OicSecAcl_t* PayloadToAcl(const OCRepPayload* payload)
{
    if (NULL == payload)
    {
        return NULL;
    }
    OC_LOG_PAYLOAD(INFO, TAG, (OCPayload*)payload);

    OicSecAcl_t *acl = NULL;
    OicSecAcl_t * headAcl = NULL;
    OicSecAcl_t * prevAcl = NULL;
    size_t dimensions[MAX_REP_ARRAY_DEPTH] = {0};
    bool result = false;
    do
    {
        acl = (OicSecAcl_t*) OICCalloc(1, sizeof(OicSecAcl_t));
        VERIFY_NON_NULL(TAG, acl, ERROR);

        headAcl = (headAcl) ? headAcl : acl;
        if (prevAcl)
        {
            prevAcl->next = acl;
        }
        int64_t *id;
        // Subject -- Mandatory
        result = OCRepPayloadGetIntArray(payload, OIC_JSON_SUBJECT_NAME, (int64_t**) &id,
                dimensions);
        VERIFY_SUCCESS(TAG, result, ERROR);
        memcpy(&acl->subject.id, id, dimensions[0]);
        OICFree(id);
        // Resources -- Mandatory
        OCRepPayloadGetStringArray(payload, OIC_JSON_RESOURCES_NAME, &acl->resources, dimensions);
        acl->resourcesLen = dimensions[0];
        // Permissions -- Mandatory
        result = OCRepPayloadGetPropInt(payload,
                OIC_JSON_PERMISSION_NAME, (int64_t*)&acl->permission);
        VERIFY_SUCCESS(TAG, result, ERROR);
        // Owners -- Mandatory
        result = OCRepPayloadGetIntArray(payload,
                OIC_JSON_OWNERS_NAME, (int64_t**) &acl->owners, dimensions);
        VERIFY_SUCCESS(TAG, result, ERROR);
        acl->ownersLen = dimensions[0];
        OC_LOG_V(INFO, TAG, "Owners [%d]", acl->ownersLen);
        for (size_t i = 0; i < acl->ownersLen; i++)
        {
            OC_LOG_BUFFER(INFO, TAG, (const uint8_t *)&acl->owners[i], SVR_UUID_LENGTH);
        }
        payload = payload->next;
        prevAcl = acl;
    } while (payload);

exit:
    if (!result)
    {
        DeleteACLList(headAcl);
        headAcl = NULL;
    }
    return headAcl;
}

static bool UpdatePersistentStorage(const OicSecAcl_t *acl)
{
    // Convert ACL data into JSON for update to persistent storage
/*
 * TODO: CBOR
    char *jsonStr = BinToAclJSON(acl);
    if (jsonStr)
    {
        cJSON *jsonAcl = cJSON_Parse(jsonStr);
        OICFree(jsonStr);

        if ((jsonAcl) && (OC_STACK_OK == UpdateSVRDatabase(OIC_JSON_ACL_NAME, jsonAcl)))
        {
            return true;
        }
        cJSON_Delete(jsonAcl);
    }
*/
    return false;
}
/*
 * This method removes ACE for the subject and resource from the ACL
 *
 * @param subject  - subject of the ACE
 * @param resource - resource of the ACE
 *
 * @return
 *     OC_STACK_RESOURCE_DELETED on success
 *     OC_STACK_NO_RESOURC on failure to find the appropriate ACE
 *     OC_STACK_INVALID_PARAM on invalid parameter
 */
static OCStackResult RemoveACE(const OicUuid_t * subject,
                               const char * resource)
{
    OC_LOG(INFO, TAG, PCF("IN RemoveACE"));

    OicSecAcl_t *acl = NULL;
    OicSecAcl_t *tempAcl = NULL;
    bool deleteFlag = false;
    OCStackResult ret = OC_STACK_NO_RESOURCE;

    if(memcmp(subject->id, &WILDCARD_SUBJECT_ID, sizeof(subject->id)) == 0)
    {
        OC_LOG_V (INFO, TAG, PCF("%s received invalid parameter"), __func__ );
        return  OC_STACK_INVALID_PARAM;
    }

    //If resource is NULL then delete all the ACE for the subject.
    if(NULL == resource)
    {
        LL_FOREACH_SAFE(gAcl, acl, tempAcl)
        {
            if(memcmp(acl->subject.id, subject->id, sizeof(subject->id)) == 0)
            {
                LL_DELETE(gAcl, acl);
                FreeACE(acl);
                deleteFlag = true;
            }
        }
    }
    else
    {
        //Looping through ACL to find the right ACE to delete. If the required resource is the only
        //resource in the ACE for the subject then delete the whole ACE. If there are more resources
        //than the required resource in the ACE, for the subject then just delete the resource from
        //the resource array
        LL_FOREACH_SAFE(gAcl, acl, tempAcl)
        {
            if(memcmp(acl->subject.id, subject->id, sizeof(subject->id)) == 0)
            {
                if(1 == acl->resourcesLen && strcmp(acl->resources[0],  resource) == 0)
                {
                    LL_DELETE(gAcl, acl);
                    FreeACE(acl);
                    deleteFlag = true;
                    break;
                }
                else
                {
                    int resPos = -1;
                    size_t i;
                    for(i = 0; i < acl->resourcesLen; i++)
                    {
                        if(strcmp(acl->resources[i],  resource) == 0)
                        {
                            resPos = i;
                            break;
                        }
                    }
                    if((0 <= resPos))
                    {
                        OICFree(acl->resources[resPos]);
                        acl->resources[resPos] = NULL;
                        acl->resourcesLen -= 1;
                        for(i = resPos; i < acl->resourcesLen; i++)
                        {
                            acl->resources[i] = acl->resources[i+1];
                        }
                        deleteFlag = true;
                        break;
                    }
                }
            }
        }
    }

    if(deleteFlag)
    {
        if(UpdatePersistentStorage(gAcl))
        {
            ret = OC_STACK_RESOURCE_DELETED;
        }
    }
    return ret;
}

static OCEntityHandlerResult HandleACLGetRequest (const OCEntityHandlerRequest * ehRequest)
{
    OC_LOG (INFO, TAG, PCF("HandleACLGetRequest processing the request"));
    // Convert acl data into OCRepPayload.
    OCRepPayload* payload = AclToPayload(gAcl);
    // A device should always have a default acl. Therefore, payload should never be NULL.
    OCEntityHandlerResult ehRet = (payload ? OC_EH_OK : OC_EH_ERROR);

    SendSRMResponse(ehRequest, ehRet, payload);

    OC_LOG_V (INFO, TAG, PCF("%s RetVal %d"), __func__ , ehRet);
    return ehRet;
}

static OCEntityHandlerResult HandleACLPostRequest (const OCEntityHandlerRequest * ehRequest)
{
    OC_LOG (INFO, TAG, PCF("HandleACLPostRequest processing the request"));
    if(!ehRequest)
    {
        return OC_EH_ERROR;
    }
    OCEntityHandlerResult ehRet = OC_EH_ERROR;

    // Convert JSON ACL data into binary. This will also validate the ACL data received.
    OCRepPayload* payload = (OCRepPayload*)ehRequest->payload;
    OicSecAcl_t* newAcl = PayloadToAcl(payload);

    if (newAcl)
    {
        // Append the new ACL to existing ACL
        LL_APPEND(gAcl, newAcl);
        // convert data and update it into persistent storage
        if (OC_STACK_OK == UpdateSVRData())
        {
            ehRet =  OC_EH_OK;
        }
        else
        {
            OC_LOG (ERROR, TAG, PCF("UpdateSVRData Failed"));
        }
    }

    // Send payload to request originator
    SendSRMResponse(ehRequest, ehRet, NULL);

    OC_LOG_V (INFO, TAG, PCF("%s RetVal %d"), __func__ , ehRet);
    return ehRet;
}

static OCEntityHandlerResult HandleACLDeleteRequest(const OCEntityHandlerRequest *ehRequest)
{
    OC_LOG (INFO, TAG, PCF("Processing ACLDeleteRequest"));
    OCEntityHandlerResult ehRet = OC_EH_ERROR;

    if(!ehRequest || ehRequest->query)
    {
        return ehRet;
    }
    OicParseQueryIter_t parseIter = {.attrPos=NULL};
    OicUuid_t subject = {.id={0}};
    char * resource = NULL;

    //Parsing REST query to get subject & resource
    ParseQueryIterInit((unsigned char *)ehRequest->query, &parseIter);

    while(GetNextQuery(&parseIter))
    {
        if(strncasecmp((char *)parseIter.attrPos, OIC_JSON_SUBJECT_NAME, parseIter.attrLen) == 0)
        {
            unsigned char base64Buff[sizeof(((OicUuid_t*)0)->id)] = {};
            uint32_t outLen = 0;
            B64Result b64Ret = B64_OK;

           b64Ret = b64Decode((char *)parseIter.valPos, parseIter.valLen, base64Buff,
                               sizeof(base64Buff), &outLen);

           VERIFY_SUCCESS(TAG, (b64Ret == B64_OK && outLen <= sizeof(subject.id)), ERROR);
           memcpy(subject.id, base64Buff, outLen);
        }
        if(strncasecmp((char *)parseIter.attrPos, OIC_JSON_RESOURCES_NAME, parseIter.attrLen) == 0)
        {
            resource = (char *)OICMalloc(parseIter.valLen);
            VERIFY_NON_NULL(TAG, resource, ERROR);
            OICStrcpy(resource, sizeof(resource), (char *)parseIter.valPos);
        }
    }

    if(OC_STACK_RESOURCE_DELETED == RemoveACE(&subject, resource))
    {
        ehRet = OC_EH_RESOURCE_DELETED;
    }
    OICFree(resource);

    // Send payload to request originator
    SendSRMResponse(ehRequest, ehRet, NULL);

exit:
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
    OC_LOG(INFO, TAG, PCF("Received request ACLEntityHandler"));
    (void)callbackParameter;
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

            case OC_REST_DELETE:
                ehRet = HandleACLDeleteRequest(ehRequest);
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

    OicUuid_t ownerId = {.id = {0}};

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

    for (size_t i = 0; i <  acl->resourcesLen; i++)
    {
        size_t len = strlen(rsrcs[i]) + 1;
        acl->resources[i] = (char*)OICMalloc(len * sizeof(char));
        VERIFY_NON_NULL(TAG, (acl->resources[i]), ERROR);
        OICStrcpy(acl->resources[i], len, rsrcs[i]);
    }

    acl->permission = PERMISSION_READ;
    acl->prdRecrLen = 0;
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
 * This method converts SVR buffers into OCRepPayload and updates the persistent storage.
 *
 * @param[out] payload is a pointer of CBOR acl payload.
 * @param[out] size is CBOR acl payload size.
 * @retval  OC_STACK_OK for Success, otherwise some error value
 */
OCStackResult ConvertAclData(uint8_t **payload,  size_t *size)
{
    OCStackResult result = OC_STACK_ERROR;
    if (gAcl)
    {
        OCRepPayload* rePayload = AclToPayload(gAcl);
        result = OCConvertPayload((OCPayload*) rePayload, payload,
                size);
        OCPayloadDestroy((OCPayload*)rePayload);
        VERIFY_SUCCESS(TAG, result == OC_STACK_OK, ERROR);
    }

exit:
    return result;
}
/**
 * This method parses OCRepPayload into SVR buffers.
 *
 * @param[in] payload is a pointer of CBOR acl payload.
 * @param[in] size is CBOR acl payload size.
 * @retval  OC_STACK_OK for Success, otherwise some error value
 */
OCStackResult ParseAclPayload(uint8_t *payload,  size_t size)
{
    OCPayload* outPayload = NULL;
    OCStackResult result = OC_STACK_ERROR;
    if (size)
    {
        result = OCParsePayload(&outPayload, payload, size);
        if (result == OC_STACK_OK)
        {
            gAcl = PayloadToAcl((const OCRepPayload*) outPayload);
            OCPayloadDestroy(outPayload);
        }
    }
    return result;
}

/**
 * Initialize ACL resource by loading data from persistent storage.
 *
 * @param[in] payload CBOR acl payload pointers.
 * @param[in] size size of CBOR acl payload.
 * @retval  OC_STACK_OK for Success, otherwise some error value
 */
OCStackResult InitACLResource(uint8_t *payload,  size_t size)
{
    OCStackResult ret = OC_STACK_ERROR;
    ret = ParseAclPayload(payload, size);
    if(ret != OC_STACK_OK)
    {
        OC_LOG (ERROR, TAG, PCF("ParseAclPayload failed"));

    }
    // Read ACL resource from PS
    /*
     * If SVR database in persistent storage got corrupted or
     * is not available for some reason, a default ACL is created
     * which allows user to initiate ACL provisioning again.
     */
    if (!gAcl)
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
 * @retval  OC_STACK_OK for Success, otherwise some error value
 */
OCStackResult DeInitACLResource()
{
    OCStackResult ret = OCDeleteResource(gAclHandle);
    gAclHandle = NULL;

    DeleteACLList(gAcl);
    gAcl = NULL;
    return ret;
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
