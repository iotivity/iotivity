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
#include "ocserverrequest.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "ocrandom.h"
#include "utlist.h"
#include "payload_logging.h"
#include "srmresourcestrings.h"
#include "aclresource.h"
#include "doxmresource.h"
#include "resourcemanager.h"
#include "srmutility.h"
#include "psinterface.h"

#include "security_internals.h"

#define TAG  "SRM-ACL"

// CborSize is the default cbor payload size being used.
static uint64_t CborSize = 255;

static OicSecAcl_t *gAcl = NULL;
static OCResourceHandle gAclHandle = NULL;

/**
 * This function frees OicSecAcl_t object's fields and object itself.
 */
static void FreeACE(OicSecAcl_t *ace)
{
    size_t i;
    if (NULL == ace)
    {
        OC_LOG(ERROR, TAG, "Invalid Parameter");
        return;
    }

    // Clean Resources
    for (i = 0; i < ace->resourcesLen; i++)
    {
        OICFree(ace->resources[i]);
    }
    OICFree(ace->resources);

    //Clean Period
    if (ace->periods)
    {
        for (i = 0; i < ace->prdRecrLen; i++)
        {
            OICFree(ace->periods[i]);
        }
        OICFree(ace->periods);
    }

    //Clean Recurrence
    if (ace->recurrences)
    {
        for (i = 0; i < ace->prdRecrLen; i++)
        {
            OICFree(ace->recurrences[i]);
        }
        OICFree(ace->recurrences);
    }

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

static size_t OicSecAclSize(const OicSecAcl_t *secAcl)
{
    if (!secAcl)
    {
        return 0;
    }
    OicSecAcl_t *acl = (OicSecAcl_t *)secAcl;
    size_t size = 0;
    while (acl)
    {
       size++;
       acl = acl->next;
    }
    return size;
}

uint8_t* AclToCBORPayload(const OicSecAcl_t *secAcl, size_t *size)
{
    if (NULL == secAcl)
    {
        return NULL;
    }

    CborEncoder encoder = { {.ptr = NULL }, .end = 0, .added = 0, .flags = 0 };
    CborEncoder oicSecAclArray = { {.ptr = NULL }, .end = 0, .added = 0, .flags = 0 };
    *size = CborSize;

    // Please note: This has been initialized prior to use because of VERIFY macro
    CborError cborEncoderResult = CborNoError;
    OicSecAcl_t *acl = (OicSecAcl_t *)secAcl;

    uint8_t *outPayload = (uint8_t *)OICCalloc(1, *size);
    VERIFY_NON_NULL(TAG, outPayload, ERROR);
    cbor_encoder_init(&encoder, outPayload, *size, 0);

    // Create ACL Array
    cborEncoderResult = cbor_encoder_create_array(&encoder, &oicSecAclArray, OicSecAclSize(secAcl));
    VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);

    while (acl)
    {
        CborEncoder oicSecAclMap = { {.ptr = NULL }, .end = 0, .added = 0, .flags = 0 };
        // ACL Map size - Number of mandatory items
        uint8_t aclMapSize = 4;
        // Create ACL Map
        if (acl->periods)
        {
            ++aclMapSize;
        }
        if (acl->recurrences)
        {
            ++aclMapSize;
        }
        cborEncoderResult = cbor_encoder_create_map(&oicSecAclArray, &oicSecAclMap, aclMapSize);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);

        // Subject -- Mandatory
        cborEncoderResult = cbor_encode_text_string(&oicSecAclMap, OIC_JSON_SUBJECT_NAME,
            sizeof(OIC_JSON_SUBJECT_NAME) - 1);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);
        size_t inLen = 0;
        if (memcmp(&(acl->subject), &WILDCARD_SUBJECT_ID, sizeof(OicUuid_t)) == 0)
        {
            inLen = WILDCARD_SUBJECT_ID_LEN;
        }
        else
        {
            inLen = sizeof(OicUuid_t);
        }
        cborEncoderResult = cbor_encode_byte_string(&oicSecAclMap, (uint8_t *)acl->subject.id, inLen);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);

        // Resources
        CborEncoder resources = { {.ptr = NULL }, .end = 0, .added = 0, .flags = 0 };
        cborEncoderResult = cbor_encode_text_string(&oicSecAclMap, OIC_JSON_RESOURCES_NAME,
            sizeof(OIC_JSON_RESOURCES_NAME) -1);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);
        cborEncoderResult = cbor_encoder_create_array(&oicSecAclMap, &resources, acl->resourcesLen);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);
        for (size_t i = 0; i < acl->resourcesLen; i++)
        {
            cborEncoderResult = cbor_encode_text_string(&resources, acl->resources[i],
                strlen(acl->resources[i]));
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);

        }
        cborEncoderResult = cbor_encoder_close_container(&oicSecAclMap, &resources);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);

        // Permissions -- Mandatory
        cborEncoderResult = cbor_encode_text_string(&oicSecAclMap, OIC_JSON_PERMISSION_NAME,
            sizeof(OIC_JSON_PERMISSION_NAME) -1);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);
        cborEncoderResult = cbor_encode_int(&oicSecAclMap, acl->permission);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);

        // Period -- Not Mandatory
        if (acl->periods)
        {
            CborEncoder period = { {.ptr = NULL }, .end = 0, .added = 0, .flags = 0 };
            cborEncoderResult = cbor_encode_text_string(&oicSecAclMap, OIC_JSON_PERIODS_NAME,
                sizeof(OIC_JSON_PERIODS_NAME) -1);
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);
            cborEncoderResult = cbor_encoder_create_array(&oicSecAclMap, &period, acl->prdRecrLen);
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);
            for (size_t i = 0; i < acl->prdRecrLen; i++)
            {
                cborEncoderResult = cbor_encode_text_string(&period, acl->periods[i],
                    strlen(acl->periods[i]));
                VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);

            }
            cborEncoderResult = cbor_encoder_close_container(&oicSecAclMap, &period);
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);
        }

        // Recurrence -- Not Mandatory
        if (acl->recurrences)
        {
            CborEncoder recurrences = { {.ptr = NULL }, .end = 0, .added = 0, .flags = 0 };
            cborEncoderResult = cbor_encode_text_string(&oicSecAclMap, OIC_JSON_RECURRENCES_NAME,
                sizeof(OIC_JSON_RECURRENCES_NAME) -1);
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);
            cborEncoderResult = cbor_encoder_create_array(&oicSecAclMap, &recurrences, acl->prdRecrLen);
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);
            for (size_t i = 0; i < acl->prdRecrLen; i++)
            {
                cborEncoderResult = cbor_encode_text_string(&recurrences, acl->recurrences[i],
                    strlen(acl->recurrences[i]));
                VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);

            }
            cborEncoderResult = cbor_encoder_close_container(&oicSecAclMap, &recurrences);
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);
        }

        cborEncoderResult = cbor_encode_text_string(&oicSecAclMap, OIC_JSON_OWNERS_NAME,
            sizeof(OIC_JSON_OWNERS_NAME) - 1);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);
        CborEncoder owners = { {.ptr = NULL }, .end = 0, .added = 0, .flags = 0 };
        cborEncoderResult = cbor_encoder_create_array(&oicSecAclMap, &owners, acl->ownersLen);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);
        for (size_t i = 0; i < acl->ownersLen; i++)
        {
            cborEncoderResult = cbor_encode_byte_string(&owners, (uint8_t *)acl->owners[i].id,
                sizeof(acl->owners[i].id));
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);

        }
        cborEncoderResult = cbor_encoder_close_container(&oicSecAclMap, &owners);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);

        cborEncoderResult = cbor_encoder_close_container(&oicSecAclArray, &oicSecAclMap);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);

        acl = acl->next;
    }
    cborEncoderResult = cbor_encoder_close_container(&encoder, &oicSecAclArray);
    VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);

    *size = encoder.ptr - outPayload;

exit:
    if (CborErrorOutOfMemory == cborEncoderResult)
    {
        // reallocate and try again!
        OICFree(outPayload);
        // Since the allocated initial memory failed, double the memory.
        CborSize += CborSize;
        cborEncoderResult = CborNoError;
        outPayload = AclToCBORPayload(secAcl, size);
    }

    if (cborEncoderResult != CborNoError)
    {
        OICFree(outPayload);
        outPayload = NULL;
        *size = 0;
    }

    return outPayload;
}

// This function converts CBOR format to ACL data.
// Caller needs to invoke 'free' when done using
// note: This function is used in unit test hence not declared static,
OicSecAcl_t* CBORPayloadToAcl(const uint8_t *cborPayload, const size_t size)
{
    if (NULL == cborPayload)
    {
        return NULL;
    }

    CborValue aclCbor = { .parser = NULL, .ptr = NULL, .remaining = 0, .extra = 0, .type = 0, .flags = 0 };
    CborParser parser = { .end = NULL, .flags = 0 };
    CborError cborFindResult = CborNoError;
    cbor_parser_init(cborPayload, size, 0, &parser, &aclCbor);

    OicSecAcl_t *headAcl = NULL;

    CborValue aclArray = { .parser = NULL, .ptr = NULL, .remaining = 0, .extra = 0, .type = 0, .flags = 0 };
    cborFindResult = cbor_value_enter_container(&aclCbor, &aclArray);
    VERIFY_CBOR_SUCCESS(TAG, cborFindResult, ERROR);

    while (cbor_value_is_valid(&aclArray))
    {
        CborValue aclMap = { .parser = NULL, .ptr = NULL, .remaining = 0, .extra = 0, .type = 0, .flags = 0 };
        cborFindResult = cbor_value_enter_container(&aclArray, &aclMap);
        VERIFY_CBOR_SUCCESS(TAG, cborFindResult, ERROR);

        OicSecAcl_t *acl = (OicSecAcl_t *) OICCalloc(1, sizeof(OicSecAcl_t));
        VERIFY_NON_NULL(TAG, acl, ERROR);

        while (cbor_value_is_valid(&aclMap))
        {
            char* name = NULL;
            size_t len = 0;
            cborFindResult = cbor_value_dup_text_string(&aclMap, &name, &len, NULL);
            VERIFY_CBOR_SUCCESS(TAG, cborFindResult, ERROR);
            cborFindResult = cbor_value_advance(&aclMap);
            VERIFY_CBOR_SUCCESS(TAG, cborFindResult, ERROR);

            CborType type = cbor_value_get_type(&aclMap);

            // Subject -- Mandatory
            if (strcmp(name, OIC_JSON_SUBJECT_NAME)  == 0)
            {
                uint8_t *subjectId = NULL;
                cborFindResult = cbor_value_dup_byte_string(&aclMap, &subjectId, &len, NULL);
                VERIFY_CBOR_SUCCESS(TAG, cborFindResult, ERROR);
                memcpy(acl->subject.id, subjectId, len);
                OICFree(subjectId);
            }

            // Resources -- Mandatory
            if (strcmp(name, OIC_JSON_RESOURCES_NAME) == 0)
            {
                CborValue resources = { .parser = NULL, .ptr = NULL, .remaining = 0, .extra = 0, .type = 0, .flags = 0 };
                cborFindResult = cbor_value_get_array_length(&aclMap, &acl->resourcesLen);
                VERIFY_CBOR_SUCCESS(TAG, cborFindResult, ERROR);

                cborFindResult = cbor_value_enter_container(&aclMap, &resources);
                VERIFY_CBOR_SUCCESS(TAG, cborFindResult, ERROR);

                acl->resources = (char **) OICMalloc(acl->resourcesLen * sizeof(char*));
                VERIFY_NON_NULL(TAG, acl->resources, ERROR);
                int i = 0;
                while (cbor_value_is_text_string(&resources))
                {
                    cborFindResult = cbor_value_dup_text_string(&resources, &acl->resources[i++],
                        &len, NULL);
                    VERIFY_CBOR_SUCCESS(TAG, cborFindResult, ERROR);
                    cborFindResult = cbor_value_advance(&resources);
                    VERIFY_CBOR_SUCCESS(TAG, cborFindResult, ERROR);
                }
            }

            // Permissions -- Mandatory
            if (strcmp(name, OIC_JSON_PERMISSION_NAME) == 0)
            {
                cborFindResult = cbor_value_get_uint64(&aclMap, (uint64_t *) &acl->permission);
                VERIFY_CBOR_SUCCESS(TAG, cborFindResult, ERROR);
            }

            // Period -- Not mandatory
            if (strcmp(name, OIC_JSON_PERIODS_NAME) == 0)
            {
                CborValue period = { .parser = NULL, .ptr = NULL, .remaining = 0, .extra = 0, .type = 0, .flags = 0 };
                cborFindResult = cbor_value_get_array_length(&aclMap, &acl->prdRecrLen);
                VERIFY_CBOR_SUCCESS(TAG, cborFindResult, ERROR);
                cborFindResult = cbor_value_enter_container(&aclMap, &period);
                VERIFY_CBOR_SUCCESS(TAG, cborFindResult, ERROR);
                int i = 0;
                while (cbor_value_is_text_string(&period))
                {
                    cborFindResult = cbor_value_dup_text_string(&period, &acl->periods[i++],
                        &len, NULL);
                    VERIFY_CBOR_SUCCESS(TAG, cborFindResult, ERROR);
                    cborFindResult = cbor_value_advance(&period);
                    VERIFY_CBOR_SUCCESS(TAG, cborFindResult, ERROR);

                    acl->prdRecrLen++;
                }
            }

            // Recurrence -- Not mandatory
            if (strcmp(name, OIC_JSON_RECURRENCES_NAME) == 0)
            {
                CborValue recurrences = { .parser = NULL, .ptr = NULL, .remaining = 0, .extra = 0, .type = 0, .flags = 0 };
                cborFindResult = cbor_value_enter_container(&aclMap, &recurrences);
                VERIFY_CBOR_SUCCESS(TAG, cborFindResult, ERROR);
                int i = 0;
                while (cbor_value_is_text_string(&recurrences))
                {
                    cborFindResult = cbor_value_dup_text_string(&recurrences,
                        &acl->recurrences[i++], &len, NULL);
                    VERIFY_CBOR_SUCCESS(TAG, cborFindResult, ERROR);
                    cborFindResult = cbor_value_advance(&recurrences);
                    VERIFY_CBOR_SUCCESS(TAG, cborFindResult, ERROR);
                }
            }

            // Owners -- Mandatory
            if (strcmp(name, OIC_JSON_OWNERS_NAME) == 0)
            {
                CborValue owners = { .parser = NULL, .ptr = NULL, .remaining = 0, .extra = 0, .type = 0, .flags = 0 };
                cborFindResult = cbor_value_get_array_length(&aclMap, &acl->ownersLen);
                VERIFY_CBOR_SUCCESS(TAG, cborFindResult, ERROR);
                cborFindResult = cbor_value_enter_container(&aclMap, &owners);
                VERIFY_CBOR_SUCCESS(TAG, cborFindResult, ERROR);
                int i = 0;
                acl->owners = (OicUuid_t *)OICMalloc(acl->ownersLen * sizeof(OicUuid_t));
                VERIFY_NON_NULL(TAG, acl->owners, ERROR);
                while (cbor_value_is_valid(&owners))
                {
                    uint8_t *owner = NULL;
                    cborFindResult = cbor_value_dup_byte_string(&owners, &owner, &len, NULL);
                    VERIFY_CBOR_SUCCESS(TAG, cborFindResult, ERROR);
                    cborFindResult = cbor_value_advance(&owners);
                    VERIFY_CBOR_SUCCESS(TAG, cborFindResult, ERROR);
                    memcpy(acl->owners[i].id, owner, len);
                    OICFree(owner);
                }
            }
            if (type != CborMapType && cbor_value_is_valid(&aclMap))
            {
                cborFindResult = cbor_value_advance(&aclMap);
                VERIFY_CBOR_SUCCESS(TAG, cborFindResult, ERROR);
            }
        }

        acl->next = NULL;
        if (headAcl == NULL)
        {
            headAcl = acl;
        }
        else
        {
            OicSecAcl_t *temp = headAcl;
            while (temp->next)
            {
                temp = temp->next;
            }
            temp->next = acl;
        }
        if (cbor_value_is_valid(&aclArray))
        {
            cborFindResult = cbor_value_advance(&aclArray);
            VERIFY_CBOR_SUCCESS(TAG, cborFindResult, ERROR);
        }
    }

exit:
    if (cborFindResult != CborNoError)
    {
        DeleteACLList(headAcl);
        headAcl = NULL;
    }
    return headAcl;
}

/**
 * This method removes ACE for the subject and resource from the ACL
 *
 * @param subject of the ACE
 * @param resource of the ACE
 *
 * @return
 *     ::OC_STACK_RESOURCE_DELETED on success
 *     ::OC_STACK_NO_RESOURCE on failure to find the appropriate ACE
 *     ::OC_STACK_INVALID_PARAM on invalid parameter
 */
static OCStackResult RemoveACE(const OicUuid_t * subject, const char * resource)
{
    OC_LOG(DEBUG, TAG, "IN RemoveACE");

    OicSecAcl_t *acl = NULL;
    OicSecAcl_t *tempAcl = NULL;
    bool deleteFlag = false;
    OCStackResult ret = OC_STACK_NO_RESOURCE;

    //If resource is NULL then delete all the ACE for the subject.
    if (NULL == resource || resource[0] == '\0')
    {
        LL_FOREACH_SAFE(gAcl, acl, tempAcl)
        {
            if (memcmp(acl->subject.id, subject->id, sizeof(subject->id)) == 0)
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
            if (memcmp(acl->subject.id, subject->id, sizeof(subject->id)) == 0)
            {
                if (1 == acl->resourcesLen && strcmp(acl->resources[0], resource) == 0)
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
                    for (i = 0; i < acl->resourcesLen; i++)
                    {
                        if (strcmp(acl->resources[i], resource) == 0)
                        {
                            resPos = i;
                            break;
                        }
                    }
                    if ((0 <= resPos))
                    {
                        OICFree(acl->resources[resPos]);
                        acl->resources[resPos] = NULL;
                        acl->resourcesLen -= 1;
                        for (i = resPos; i < acl->resourcesLen; i++)
                        {
                            acl->resources[i] = acl->resources[i + 1];
                        }
                        deleteFlag = true;
                        break;
                    }
                }
            }
        }
    }

    if (deleteFlag)
    {
        size_t size = 0;
        // In case of unit test do not update persistant storage.
        if (memcmp(subject->id, &WILDCARD_SUBJECT_B64_ID, sizeof(subject->id)) == 0)
        {
            ret = OC_STACK_RESOURCE_DELETED;
        }
        else
        {
            uint8_t *payload = AclToCBORPayload(gAcl, &size);
            if (payload &&
                (UpdateSecureResourceInPS(OIC_JSON_ACL_NAME, payload, size) == OC_STACK_OK))
            {
                ret = OC_STACK_RESOURCE_DELETED;
            }
            OICFree(payload);
        }
    }
    return ret;
}

/**
 * This method parses the query string received for REST requests and
 * retrieves the 'subject' field.
 *
 * @param query querystring passed in REST request
 * @param subject subject UUID parsed from query string
 *
 * @return true if query parsed successfully and found 'subject', else false.
 */
static bool GetSubjectFromQueryString(const char *query, OicUuid_t *subject)
{
    OicParseQueryIter_t parseIter = { .attrPos = NULL };

    ParseQueryIterInit((unsigned char *) query, &parseIter);

    while (GetNextQuery (&parseIter))
    {
        if (strncasecmp((char *) parseIter.attrPos, OIC_JSON_SUBJECT_NAME, parseIter.attrLen) == 0)
        {
            VERIFY_SUCCESS(TAG, 0 != parseIter.valLen, ERROR);
            memcpy(subject->id, parseIter.valPos, parseIter.valLen);
            return true;
        }
    }

exit:
    return false;
}

/**
 * This method parses the query string received for REST requests and
 * retrieves the 'resource' field.
 *
 * @param query querystring passed in REST request
 * @param resource resource parsed from query string
 * @param resourceSize size of the memory pointed to resource
 *
 * @return true if query parsed successfully and found 'resource', else false.
 */
static bool GetResourceFromQueryString(const char *query, char *resource, size_t resourceSize)
{
    OicParseQueryIter_t parseIter = { .attrPos = NULL };

    ParseQueryIterInit((unsigned char *) query, &parseIter);

    while (GetNextQuery (&parseIter))
    {
        if (strncasecmp((char *) parseIter.attrPos, OIC_JSON_RESOURCES_NAME, parseIter.attrLen)
                == 0)
        {
            VERIFY_SUCCESS(TAG, 0 != parseIter.valLen, ERROR);
            OICStrcpy(resource, resourceSize, (char *) parseIter.valPos);

            return true;
        }
    }

exit:
   return false;
}

static OCEntityHandlerResult HandleACLGetRequest(const OCEntityHandlerRequest *ehRequest)
{
    OC_LOG(INFO, TAG, "HandleACLGetRequest processing the request");
    uint8_t* payload = NULL;
    size_t size = 0;
    OCEntityHandlerResult ehRet;

    // Process the REST querystring parameters
    if (ehRequest->query)
    {
        OC_LOG(DEBUG, TAG, "HandleACLGetRequest processing query");

        OicUuid_t subject = {.id= { 0 } };
        char resource[MAX_URI_LENGTH] = { 0 };

        OicSecAcl_t *savePtr = NULL;
        const OicSecAcl_t *currentAce = NULL;

        // 'Subject' field is MUST for processing a querystring in REST request.
        VERIFY_SUCCESS(TAG, true == GetSubjectFromQueryString(ehRequest->query, &subject), ERROR);

        GetResourceFromQueryString(ehRequest->query, resource, sizeof(resource));

        /*
         * TODO : Currently, this code only provides one ACE for a Subject.
         * Below code needs to be updated for scenarios when Subject have
         * multiple ACE's in ACL resource.
         */
        while ((currentAce = GetACLResourceData(&subject, &savePtr)))
        {
            /*
             * If REST querystring contains a specific resource, we need
             * to search for that resource in ACE.
             */
            if (resource[0] != '\0')
            {
                for (size_t n = 0; n < currentAce->resourcesLen; n++)
                {
                    if ((currentAce->resources[n])
                            && (0 == strcmp(resource, currentAce->resources[n])
                                    || 0 == strcmp(WILDCARD_RESOURCE_URI, currentAce->resources[n])))
                    {
                        // Convert ACL data into CBOR format for transmission
                        payload = AclToCBORPayload(currentAce, &size);
                        if (payload == NULL)
                        {
                            ehRet = OC_EH_ERROR;
                        }
                        goto exit;
                    }
                }
            }
            else
            {
                // Convert ACL data into CBOR format for transmission
                payload = AclToCBORPayload(currentAce, &size);
                if (payload == NULL)
                {
                    ehRet = OC_EH_ERROR;
                }
                goto exit;
            }
        }
    }
    else
    {
        // Convert ACL data into CBOR format for transmission
        payload = AclToCBORPayload(gAcl, &size);
        if (payload == NULL)
        {
            ehRet = OC_EH_ERROR;
        }
    }
exit:
    // A device should always have a default acl. Therefore, payload should never be NULL.
    ehRet = (payload ? OC_EH_OK : OC_EH_ERROR);

    // Send response payload to request originator
    SendSRMCBORResponse(ehRequest, ehRet, payload);

    OC_LOG_V(DEBUG, TAG, "%s RetVal %d", __func__, ehRet);
    return ehRet;
}

static OCEntityHandlerResult HandleACLPostRequest(const OCEntityHandlerRequest *ehRequest)
{
    OC_LOG(INFO, TAG, "HandleACLPostRequest processing the request");
    OCEntityHandlerResult ehRet = OC_EH_ERROR;

    // Convert CBOR into ACL data and update to SVR buffers. This will also validate the ACL data received.
    uint8_t *payload = ((OCSecurityPayload *) ehRequest->payload)->securityData1;;
    if (payload)
    {
        OicSecAcl_t *newAcl = CBORPayloadToAcl(payload, CborSize);
        if (newAcl)
        {
            // Append the new ACL to existing ACL
            LL_APPEND(gAcl, newAcl);
            size_t size = 0;
            // In case of unit test do not update persistant storage.
            if (memcmp(newAcl->subject.id, &WILDCARD_SUBJECT_ID, sizeof(newAcl->subject.id)) == 0
                || memcmp(newAcl->subject.id, &WILDCARD_SUBJECT_B64_ID, sizeof(newAcl->subject.id)) == 0)
            {
                ehRet = OC_EH_RESOURCE_CREATED;
            }
            else
            {
                uint8_t *cborPayload = AclToCBORPayload(gAcl, &size);
                if (cborPayload &&
                    UpdateSecureResourceInPS(OIC_JSON_ACL_NAME, cborPayload, size) == OC_STACK_OK)
                {
                    ehRet = OC_EH_RESOURCE_CREATED;
                }
            }
        }
    }

    // Send payload to request originator
    SendSRMCBORResponse(ehRequest, ehRet, NULL);

    OC_LOG_V(DEBUG, TAG, "%s RetVal %d", __func__, ehRet);
    return ehRet;
}

static OCEntityHandlerResult HandleACLDeleteRequest(const OCEntityHandlerRequest *ehRequest)
{
    OC_LOG(DEBUG, TAG, "Processing ACLDeleteRequest");
    OCEntityHandlerResult ehRet = OC_EH_ERROR;
    OicUuid_t subject = { .id= { 0 } };
    char resource[MAX_URI_LENGTH] = { 0 };

    VERIFY_NON_NULL(TAG, ehRequest->query, ERROR);

    // 'Subject' field is MUST for processing a querystring in REST request.
    VERIFY_SUCCESS(TAG, true == GetSubjectFromQueryString(ehRequest->query, &subject), ERROR);

    GetResourceFromQueryString(ehRequest->query, resource, sizeof(resource));

    if (OC_STACK_RESOURCE_DELETED == RemoveACE(&subject, resource))
    {
        ehRet = OC_EH_RESOURCE_DELETED;
    }

exit:
    // Send payload to request originator
    SendSRMCBORResponse(ehRequest, ehRet, NULL);

    return ehRet;
}

OCEntityHandlerResult ACLEntityHandler(OCEntityHandlerFlag flag, OCEntityHandlerRequest * ehRequest,
        void* callbackParameter)
{
    OC_LOG(DEBUG, TAG, "Received request ACLEntityHandler");
    (void)callbackParameter;
    OCEntityHandlerResult ehRet = OC_EH_ERROR;

    if (!ehRequest)
    {
        return ehRet;
    }

    if (flag & OC_REQUEST_FLAG)
    {
        // TODO :  Handle PUT method
        OC_LOG(DEBUG, TAG, "Flag includes OC_REQUEST_FLAG");
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
                SendSRMCBORResponse(ehRequest, ehRet, NULL);
        }
    }

    return ehRet;
}

/**
 * This internal method is used to create '/oic/sec/acl' resource.
 */
static OCStackResult CreateACLResource()
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
        OC_LOG(FATAL, TAG, "Unable to instantiate ACL resource");
        DeInitACLResource();
    }
    return ret;
}

// This function sets the default ACL and is defined for the unit test only.
OCStackResult SetDefaultACL(OicSecAcl_t *acl)
{
    gAcl = acl;
    return OC_STACK_OK;
}

OCStackResult GetDefaultACL(OicSecAcl_t** defaultAcl)
{
    OCStackResult ret = OC_STACK_ERROR;

    OicUuid_t ownerId = { .id = { 0 } };

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

    OicSecAcl_t *acl = (OicSecAcl_t *) OICCalloc(1, sizeof(OicSecAcl_t));
    VERIFY_NON_NULL(TAG, acl, ERROR);

    // Subject -- Mandatory
    memcpy(&(acl->subject), &WILDCARD_SUBJECT_ID, sizeof(acl->subject));

    // Resources -- Mandatory
    acl->resourcesLen = sizeof(rsrcs) / sizeof(rsrcs[0]);

    acl->resources = (char**) OICCalloc(acl->resourcesLen, sizeof(char*));
    VERIFY_NON_NULL(TAG, (acl->resources), ERROR);

    for (size_t i = 0; i < acl->resourcesLen; i++)
    {
        size_t len = strlen(rsrcs[i]) + 1;
        acl->resources[i] = (char*) OICMalloc(len * sizeof(char));
        VERIFY_NON_NULL(TAG, (acl->resources[i]), ERROR);
        OICStrcpy(acl->resources[i], len, rsrcs[i]);
    }

    acl->permission = PERMISSION_READ;
    acl->prdRecrLen = 0;
    acl->periods = NULL;
    acl->recurrences = NULL;

    // Device ID is the owner of this default ACL
    if (GetDoxmResourceData() != NULL)
    {
        ret = GetDoxmDeviceID(&ownerId);
        VERIFY_SUCCESS(TAG, OC_STACK_OK == ret, FATAL);
    }
    else
    {
        OCRandomUuidResult rdm = OCGenerateUuid(ownerId.id);
        VERIFY_SUCCESS(TAG, RAND_UUID_OK == rdm, FATAL);
    }
    acl->ownersLen = 1;
    acl->owners = (OicUuid_t*) OICMalloc(sizeof(OicUuid_t));
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

OCStackResult InitACLResource()
{
    OCStackResult ret = OC_STACK_ERROR;

    uint8_t *data = NULL;
    size_t size = 0;
    ret = GetSecureVirtualDatabaseFromPS(OIC_JSON_ACL_NAME, &data, &size);
    // If database read failed
    if (ret != OC_STACK_OK)
    {
        OC_LOG (DEBUG, TAG, "ReadSVDataFromPS failed");
    }
    if (data)
    {
        // Read ACL resource from PS
        gAcl = CBORPayloadToAcl(data, size);
    }
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

OCStackResult DeInitACLResource()
{
    OCStackResult ret =  OCDeleteResource(gAclHandle);
    gAclHandle = NULL;

    if (gAcl)
    {
        DeleteACLList(gAcl);
        gAcl = NULL;
    }
    return ret;
}

const OicSecAcl_t* GetACLResourceData(const OicUuid_t* subjectId, OicSecAcl_t **savePtr)
{
    OicSecAcl_t *acl = NULL;
    OicSecAcl_t *begin = NULL;

    if (NULL == subjectId)
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

OCStackResult InstallNewACL(const uint8_t *cborPayload, const size_t size)
{
    OCStackResult ret = OC_STACK_ERROR;

    // Convert CBOR format to ACL data. This will also validate the ACL data received.
    OicSecAcl_t* newAcl = CBORPayloadToAcl(cborPayload, size);

    if (newAcl)
    {
        // Append the new ACL to existing ACL
        LL_APPEND(gAcl, newAcl);

        // Update persistent storage only if it is not WILDCARD_SUBJECT_ID
        if (memcmp(newAcl->subject.id, &WILDCARD_SUBJECT_ID, sizeof(newAcl->subject.id)) == 0
            || memcmp(newAcl->subject.id, &WILDCARD_SUBJECT_B64_ID, sizeof(newAcl->subject.id)) == 0)
        {
            ret = OC_STACK_OK;
        }
        else
        {
            size_t size = 0;
            uint8_t *payload = AclToCBORPayload(gAcl, &size);
            if (payload && UpdateSecureResourceInPS(OIC_JSON_ACL_NAME, payload, size) == OC_STACK_OK)
            {
                ret = OC_STACK_OK;
            }
            OICFree(payload);
        }
    }

    return ret;
}
