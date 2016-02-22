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

#define __STDC_LIMIT_MACROS
#include <stdlib.h>
#ifdef WITH_ARDUINO
#include <string.h>
#else
#include <strings.h>
#endif
#include <stdint.h>

#include "cainterface.h"
#include "payload_logging.h"
#include "ocstack.h"
#include "ocrandom.h"
#include "oic_malloc.h"
#include "utlist.h"
#include "credresource.h"
#include "doxmresource.h"
#include "iotvticalendar.h"
#include "pbkdf2.h"
#include "resourcemanager.h"
#include "srmresourcestrings.h"
#include "srmutility.h"
#include "psinterface.h"

#define TAG  "SRM-CREDL"

/** Default cbor payload size. This value is increased in case of CborErrorOutOfMemory.
 * The value of payload size is increased until reaching belox max cbor size. */
static const uint8_t CBOR_SIZE = 255;

/** Max cbor size payload. */
static const uint16_t CBOR_MAX_SIZE = 4400;

/** CRED Map size - Number of mandatory items. */
static const uint8_t CRED_MAP_SIZE = 4;

static OicSecCred_t        *gCred = NULL;
static OCResourceHandle    gCredHandle = NULL;

/**
 * This function frees OicSecCred_t object's fields and object itself.
 */
static void FreeCred(OicSecCred_t *cred)
{
    if(NULL == cred)
    {
        OC_LOG(ERROR, TAG, "Invalid Parameter");
        return;
    }
    //Note: Need further clarification on roleID data type
#if 0
    //Clean roleIds
    OICFree(cred->roleIds);
#endif

    //Clean PublicData
    OICFree(cred->publicData.data);

    //Clean PrivateData
    OICFree(cred->privateData.data);

    //Clean Period
    OICFree(cred->period);

    //Clean Owners
    OICFree(cred->owners);

    //Clean Cred node itself
    OICFree(cred);
}

void DeleteCredList(OicSecCred_t* cred)
{
    if (cred)
    {
        OicSecCred_t *credTmp1 = NULL, *credTmp2 = NULL;
        LL_FOREACH_SAFE(cred, credTmp1, credTmp2)
        {
            LL_DELETE(cred, credTmp1);
            FreeCred(credTmp1);
        }
    }
}


static size_t OicSecCredCount(const OicSecCred_t *secCred)
{
    size_t size = 0;
    for (const OicSecCred_t *cred = secCred; cred; cred = cred->next)
    {
        size++;
    }
    return size;
}

OCStackResult CredToCBORPayload(const OicSecCred_t *credS, uint8_t **cborPayload,
                                size_t *cborSize)
{
    if (NULL == credS || NULL == cborPayload || NULL != *cborPayload || NULL == cborSize)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult ret = OC_STACK_ERROR;
    size_t cborLen = *cborSize;
    if (0 == cborLen)
    {
        cborLen = CBOR_SIZE;
    }

    *cborSize = 0;
    *cborPayload = NULL;

    CborEncoder encoder = { {.ptr = NULL }, .end = 0 };
    CborEncoder credArray = { {.ptr = NULL }, .end = 0 };
    CborError cborEncoderResult = CborNoError;

    const OicSecCred_t *cred = credS;
    size_t mapSize = CRED_MAP_SIZE;
    if (cred->period)
    {
        mapSize++;
    }
    if (cred->publicData.data)
    {
        mapSize++;
    }
    if (cred->privateData.data)
    {
        mapSize++;
    }
    if (cred->period)
    {
        mapSize++;
    }
    uint8_t *outPayload = (uint8_t *)OICCalloc(1, cborLen);
    VERIFY_NON_NULL(TAG, outPayload, ERROR);
    cbor_encoder_init(&encoder, outPayload, cborLen, 0);

    // Create CRED Array
    cborEncoderResult = cbor_encoder_create_array(&encoder, &credArray, OicSecCredCount(cred));
    VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);

    while (cred)
    {
        CborEncoder credMap = { {.ptr = NULL }, .end = 0 };
        cborEncoderResult = cbor_encoder_create_map(&credArray, &credMap, mapSize);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);

        //CredID -- Mandatory
        cborEncoderResult = cbor_encode_text_string(&credMap, OIC_JSON_CREDID_NAME,
            strlen(OIC_JSON_CREDID_NAME));
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);
        cborEncoderResult = cbor_encode_int(&credMap, cred->credId);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);

        //Subject -- Mandatory
        cborEncoderResult = cbor_encode_text_string(&credMap, OIC_JSON_SUBJECT_NAME,
            strlen(OIC_JSON_SUBJECT_NAME));
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);
        cborEncoderResult = cbor_encode_byte_string(&credMap, cred->subject.id,
            sizeof(cred->subject.id));
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);

        //CredType -- Mandatory
        cborEncoderResult = cbor_encode_text_string(&credMap, OIC_JSON_CREDTYPE_NAME,
            strlen(OIC_JSON_CREDTYPE_NAME));
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);
        cborEncoderResult = cbor_encode_int(&credMap, cred->credType);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);

#ifdef __WITH_X509__
        //PublicData -- Not Mandatory
        if (cred->publicData.data)
        {
            cborEncoderResult = cbor_encode_text_string(&credMap,
                OIC_JSON_PUBLICDATA_NAME, strlen(OIC_JSON_PUBLICDATA_NAME));
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);
            cborEncoderResult = cbor_encode_byte_string(&credMap,
                (uint8_t *) cred->publicData.data,
                sizeof((uint8_t *) cred->publicData.data));
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);
        }
#endif /*__WITH_X509__*/
        //PrivateData -- Not Mandatory
        if(cred->privateData.data)
        {
            cborEncoderResult = cbor_encode_text_string(&credMap,
                OIC_JSON_PRIVATEDATA_NAME, strlen(OIC_JSON_PRIVATEDATA_NAME));
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);
            cborEncoderResult = cbor_encode_byte_string(&credMap,
                (uint8_t *) cred->privateData.data,
                sizeof((uint8_t *) cred->privateData.data));
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);
        }

        //Period -- Not Mandatory
        if(cred->period)
        {
            cborEncoderResult = cbor_encode_text_string(&credMap, OIC_JSON_PERIOD_NAME,
                strlen(OIC_JSON_PERIOD_NAME));
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);
            cborEncoderResult = cbor_encode_text_string(&credMap, cred->period,
                strlen(cred->period));
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);
        }

        //Owners -- Mandatory
        {
            cborEncoderResult = cbor_encode_text_string(&credMap, OIC_JSON_OWNERS_NAME,
                strlen(OIC_JSON_OWNERS_NAME));
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);
            CborEncoder owners = { {.ptr = NULL }, .end = 0 };
            cborEncoderResult = cbor_encoder_create_array(&credMap, &owners, cred->ownersLen);
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);
            for (size_t i = 0; i < cred->ownersLen; i++)
            {
                cborEncoderResult = cbor_encode_byte_string(&owners,
                    (uint8_t *)cred->owners[i].id,
                    sizeof(cred->owners[i].id));
                VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);
            }
            cborEncoderResult = cbor_encoder_close_container(&credMap, &owners);
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);
        }
        cborEncoderResult = cbor_encoder_close_container(&credArray, &credMap);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);

        cred = cred->next;
   }
   cborEncoderResult = cbor_encoder_close_container(&encoder, &credArray);
   VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);

   *cborPayload = outPayload;
   *cborSize = encoder.ptr - outPayload;
    ret = OC_STACK_OK;

exit:
if ((CborErrorOutOfMemory == cborEncoderResult) && (cborLen < CBOR_MAX_SIZE))
    {
       // reallocate and try again!
       OICFree(outPayload);
       outPayload = NULL;
       // Since the allocated initial memory failed, double the memory.
       cborLen += encoder.ptr - encoder.end;
       cborEncoderResult = CborNoError;
       ret = CredToCBORPayload(credS, cborPayload, &cborLen);
    }

    if (CborNoError != cborEncoderResult)
    {
       OICFree(outPayload);
       outPayload = NULL;
       *cborSize = 0;
       *cborPayload = NULL;
       ret = OC_STACK_ERROR;
    }

    return ret;
}

OCStackResult CBORPayloadToCred(const uint8_t *cborPayload, size_t size,
                                OicSecCred_t **secCred)
{
    if (NULL == cborPayload || NULL == secCred || NULL != *secCred)
    {
        return OC_STACK_INVALID_PARAM;
    }

    *secCred = NULL;

    OCStackResult ret = OC_STACK_ERROR;

    CborValue credCbor = { .parser = NULL };
    CborParser parser = { .end = NULL };
    CborError cborFindResult = CborNoError;
    OicSecCred_t *cred = NULL;
    char *name = NULL;

    int cborLen = size;
    if (0 == size)
    {
        cborLen = CBOR_SIZE;
    }
    cbor_parser_init(cborPayload, cborLen, 0, &parser, &credCbor);

    OicSecCred_t *headCred = NULL;

    CborValue credArray = { .parser = NULL };
    cborFindResult = cbor_value_enter_container(&credCbor, &credArray);
    VERIFY_CBOR_SUCCESS(TAG, cborFindResult, ERROR);

    while (cbor_value_is_valid(&credArray))
    {
        CborValue credMap = { .parser = NULL };
        cborFindResult = cbor_value_enter_container(&credArray, &credMap);
        VERIFY_CBOR_SUCCESS(TAG, cborFindResult, ERROR);

        cred = (OicSecCred_t *) OICCalloc(1, sizeof(*cred));
        VERIFY_NON_NULL(TAG, cred, ERROR);

        while (cbor_value_is_valid(&credMap))
        {
            size_t len = 0;
            cborFindResult = cbor_value_dup_text_string(&credMap, &name, &len, NULL);
            VERIFY_CBOR_SUCCESS(TAG, cborFindResult, ERROR);
            cborFindResult = cbor_value_advance(&credMap);
            VERIFY_CBOR_SUCCESS(TAG, cborFindResult, ERROR);

            CborType type = cbor_value_get_type(&credMap);

            //CredId -- Mandatory
            if (0 == strcmp(OIC_JSON_CREDID_NAME, name))
            {
                cborFindResult = cbor_value_get_int(&credMap, (int *) &cred->credId);
                VERIFY_CBOR_SUCCESS(TAG, cborFindResult, ERROR);
            }
            //subject -- Mandatory
            if (0 == strcmp(OIC_JSON_SUBJECT_NAME, name))
            {
                uint8_t *id = NULL;
                cborFindResult = cbor_value_dup_byte_string(&credMap, &id, &len, NULL);
                VERIFY_CBOR_SUCCESS(TAG, cborFindResult, ERROR);
                memcpy(cred->subject.id, id, len);
                OICFree(id);
            }
            //CredType -- Mandatory
            if (0 == strcmp(OIC_JSON_CREDTYPE_NAME, name))
            {
                cborFindResult = cbor_value_get_int(&credMap, (int *) &cred->credType);
                VERIFY_CBOR_SUCCESS(TAG, cborFindResult, ERROR);
            }
            //Owners -- Mandatory
            if (0 == strcmp(OIC_JSON_OWNERS_NAME, name))
            {
                CborValue owners = { .parser = NULL };
                cborFindResult = cbor_value_get_array_length(&credMap, &cred->ownersLen);
                VERIFY_CBOR_SUCCESS(TAG, cborFindResult, ERROR);
                cborFindResult = cbor_value_enter_container(&credMap, &owners);
                VERIFY_CBOR_SUCCESS(TAG, cborFindResult, ERROR);
                int i = 0;
                cred->owners = (OicUuid_t *)OICCalloc(cred->ownersLen, sizeof(*cred->owners));
                VERIFY_NON_NULL(TAG, cred->owners, ERROR);
                while (cbor_value_is_valid(&owners))
                {
                    uint8_t *owner = NULL;
                    cborFindResult = cbor_value_dup_byte_string(&owners, &owner, &len, NULL);
                    VERIFY_CBOR_SUCCESS(TAG, cborFindResult, ERROR);
                    memcpy(cred->owners[i].id, owner, len);
                    OICFree(owner);
                    cborFindResult = cbor_value_advance(&owners);
                    VERIFY_CBOR_SUCCESS(TAG, cborFindResult, ERROR);
                }
            }
            //PrivateData is mandatory for some of the credential types listed below.
            if (0 == strcmp(OIC_JSON_PRIVATEDATA_NAME, name))
            {
                if ((cred->credType & SYMMETRIC_PAIR_WISE_KEY) ||
                    (cred->credType & SYMMETRIC_GROUP_KEY) ||
                    (cred->credType & PIN_PASSWORD))
                {
                    cborFindResult = cbor_value_dup_byte_string(&credMap,
                        &cred->privateData.data, &len, NULL);
                    VERIFY_CBOR_SUCCESS(TAG, cborFindResult, ERROR);
                }
            }

            //PublicData is mandatory only for SIGNED_ASYMMETRIC_KEY credentials type.
            if (0 == strcmp(OIC_JSON_PUBLICDATA_NAME, name))
            {
                if (cred->credType & SIGNED_ASYMMETRIC_KEY)
                {
                    cborFindResult = cbor_value_dup_byte_string(&credMap,
                        &cred->publicData.data, &len, NULL);
                    VERIFY_CBOR_SUCCESS(TAG, cborFindResult, ERROR);
                }
            }
            //Period -- Not Mandatory
            if (0 == strcmp(OIC_JSON_PERIOD_NAME, name))
            {
                cborFindResult = cbor_value_dup_text_string(&credMap,
                    &cred->period, &len, NULL);
                VERIFY_CBOR_SUCCESS(TAG, cborFindResult, ERROR);
            }
            if (CborMapType != type && cbor_value_is_valid(&credMap))
            {
                cborFindResult = cbor_value_advance(&credMap);
                VERIFY_CBOR_SUCCESS(TAG, cborFindResult, ERROR);
            }
            OICFree(name);
            name = NULL;
        }

        cred->next = NULL;
        if (NULL == headCred)
        {
            headCred = cred;
        }
        else
        {
            OicSecCred_t *temp = headCred;
            while (temp->next)
            {
                temp = temp->next;
            }
            temp->next = cred;
        }
        if (cbor_value_is_valid(&credArray))
        {
            cborFindResult = cbor_value_advance(&credArray);
            VERIFY_CBOR_SUCCESS(TAG, cborFindResult, ERROR);
        }
    }
    *secCred = headCred;
    ret = OC_STACK_OK;
exit:
    if (CborNoError != cborFindResult)
    {
        DeleteCredList(headCred);
        headCred = NULL;
        ret = OC_STACK_ERROR;
    }
    if (cred)
    {
        DeleteCredList(cred);
    }
    if (name)
    {
        OICFree(name);
    }
    return ret;
}

OicSecCred_t * GenerateCredential(const OicUuid_t * subject, OicSecCredType_t credType,
                                  const uint8_t * publicData, const uint8_t* privateData,
                                  size_t ownersLen, const OicUuid_t * owners)
{
    (void)publicData;
    OCStackResult ret = OC_STACK_ERROR;

    OicSecCred_t *cred = (OicSecCred_t *)OICCalloc(1, sizeof(*cred));
    VERIFY_NON_NULL(TAG, cred, ERROR);

    //CredId is assigned before appending new cred to the existing
    //credential list and updating svr database in AddCredential().
    cred->credId = 0;

    VERIFY_NON_NULL(TAG, subject, ERROR);
    memcpy(cred->subject.id, subject->id , sizeof(cred->subject.id));

    VERIFY_SUCCESS(TAG, credType < (NO_SECURITY_MODE | SYMMETRIC_PAIR_WISE_KEY |
            SYMMETRIC_GROUP_KEY | ASYMMETRIC_KEY | SIGNED_ASYMMETRIC_KEY | PIN_PASSWORD), ERROR);
    cred->credType = credType;

#ifdef __WITH_X509__
    if (publicData)
    {
        cred->publicData.data = (uint8_t *)OICCalloc(1, sizeof(publicData));
        VERIFY_NON_NULL(TAG, cred->publicData.data, ERROR);
        memcpy(cred->publicData.data, publicData, sizeof(publicData));
    }
#endif // __WITH_X509__

    if (privateData)
    {
#ifdef __WITH_X509__
        cred->privateData.data = (uint8_t *)OICCalloc(1, sizeof(privateData));
        VERIFY_NON_NULL(TAG, cred->privateData.data, ERROR);
        memcpy(cred->privateData.data, privateData, PRIVATE_KEY_SIZE);
#endif // __WITH_X509__
    }

    VERIFY_SUCCESS(TAG, ownersLen > 0, ERROR);
    cred->ownersLen = ownersLen;

    cred->owners = (OicUuid_t *)OICCalloc(cred->ownersLen, sizeof(*cred->owners));
    VERIFY_NON_NULL(TAG, cred->owners, ERROR);
    for (size_t i = 0; i < cred->ownersLen; i++)
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

static bool UpdatePersistentStorage(const OicSecCred_t *cred)
{
    bool ret = false;

    // Convert Cred data into JSON for update to persistent storage
    uint8_t *payload = NULL;
    size_t size = 0;
    OCStackResult res = CredToCBORPayload(cred, &payload, &size);
    if ((OC_STACK_OK == res) && payload)
    {
        if (OC_STACK_OK == UpdateSecureResourceInPS(OIC_JSON_CRED_NAME, payload, size))
        {
            ret = true;
        }
        OICFree(payload);
    }
    else //Empty cred list
    {
        if (OC_STACK_OK == UpdateSecureResourceInPS(OIC_JSON_CRED_NAME, NULL, size))
        {
            ret = true;
        }
    }
    return ret;
}

/**
 * Compare function used LL_SORT for sorting credentials.
 *
 * @param first pointer to OicSecCred_t struct.
 * @param second  pointer to OicSecCred_t struct.
 *
 *@return -1, if credId of first is less than credId of second.
 * 0, if credId of first is equal to credId of second.
 * 1, if credId of first is greater than credId of second.
 */
static int CmpCredId(const OicSecCred_t * first, const OicSecCred_t *second)
{
    if (first->credId < second->credId)
    {
        return -1;
    }
    else if (first->credId > second->credId)
    {
        return 1;
    }
    else
        return 0;
}

/**
 * GetCredId goes through the cred list and returns the next
 * available credId. The next credId could be the credId that is
 * available due deletion of OicSecCred_t object or one more than
 * credId of last credential in the list.
 *
 * @return next available credId if successful, else 0 for error.
 */
static uint16_t GetCredId()
{
    //Sorts credential list in incremental order of credId
    LL_SORT(gCred, CmpCredId);

    OicSecCred_t *currentCred = NULL, *credTmp = NULL;
    uint16_t nextCredId = 1;

    LL_FOREACH_SAFE(gCred, currentCred, credTmp)
    {
        if (currentCred->credId == nextCredId)
        {
            nextCredId += 1;
        }
        else
        {
            break;
        }
    }

    VERIFY_SUCCESS(TAG, nextCredId < UINT16_MAX, ERROR);
    return nextCredId;

exit:
    return 0;
}

/**
 * Get the default value.
 *
 * @return  NULL for now.
 */
static OicSecCred_t* GetCredDefault()
{
    // TODO:Update it when we finalize the default info.
    return NULL;
}

OCStackResult AddCredential(OicSecCred_t * newCred)
{
    OCStackResult ret = OC_STACK_ERROR;

    VERIFY_SUCCESS(TAG, NULL != newCred, ERROR);

    //Assigning credId to the newCred
    newCred->credId = GetCredId();

    VERIFY_SUCCESS(TAG, newCred->credId != 0, ERROR);

    //Append the new Cred to existing list
    LL_APPEND(gCred, newCred);

    if (UpdatePersistentStorage(gCred))
    {
        ret = OC_STACK_OK;
    }

exit:
    return ret;
}

OCStackResult RemoveCredential(const OicUuid_t *subject)
{
    OCStackResult ret = OC_STACK_ERROR;
    OicSecCred_t *cred = NULL;
    OicSecCred_t *tempCred = NULL;
    bool deleteFlag = false;

    LL_FOREACH_SAFE(gCred, cred, tempCred)
    {
        if (memcmp(cred->subject.id, subject->id, sizeof(subject->id)) == 0)
        {
            LL_DELETE(gCred, cred);
            FreeCred(cred);
            deleteFlag = 1;
        }
    }

    if (deleteFlag)
    {
        if (UpdatePersistentStorage(gCred))
        {
            ret = OC_STACK_RESOURCE_DELETED;
        }
    }
    return ret;

}

static OCEntityHandlerResult HandlePostRequest(const OCEntityHandlerRequest * ehRequest)
{
    OCEntityHandlerResult ret = OC_EH_ERROR;

    //Get binary representation of CBOR
    OicSecCred_t *cred  = NULL;
    uint8_t *payload = ((OCSecurityPayload*)ehRequest->payload)->securityData1;
    OCStackResult res = CBORPayloadToCred(payload, 0, &cred);
    if ((OC_STACK_OK == res) && cred)
    {
        //If the Post request credential has credId, it will be
        //discarded and the next available credId will be assigned
        //to it before getting appended to the existing credential
        //list and updating svr database.
        ret = (OC_STACK_OK == AddCredential(cred))? OC_EH_RESOURCE_CREATED : OC_EH_ERROR;
    }
    return ret;
}

static OCEntityHandlerResult HandleDeleteRequest(const OCEntityHandlerRequest *ehRequest)
{
    OC_LOG(DEBUG, TAG, "Processing CredDeleteRequest");

    OCEntityHandlerResult ehRet = OC_EH_ERROR;

    if (NULL == ehRequest->query)
   {
       return ehRet;
   }

   OicParseQueryIter_t parseIter = { .attrPos=NULL };
   OicUuid_t subject = {.id={0}};

   //Parsing REST query to get the subject
   ParseQueryIterInit((unsigned char *)ehRequest->query, &parseIter);
   while (GetNextQuery(&parseIter))
   {
       if (strncasecmp((char *)parseIter.attrPos, OIC_JSON_SUBJECT_NAME,
               parseIter.attrLen) == 0)
       {
           memcpy(subject.id, parseIter.valPos, parseIter.valLen);
       }
   }

   if (OC_STACK_RESOURCE_DELETED == RemoveCredential(&subject))
   {
       ehRet = OC_EH_RESOURCE_DELETED;
   }

    return ehRet;
}

OCEntityHandlerResult CredEntityHandler(OCEntityHandlerFlag flag,
                                        OCEntityHandlerRequest * ehRequest,
                                        void* callbackParameter)
{
    (void)callbackParameter;
    OCEntityHandlerResult ret = OC_EH_ERROR;

    if (!ehRequest)
    {
        return OC_EH_ERROR;
    }
    if (flag & OC_REQUEST_FLAG)
    {
        OC_LOG (DEBUG, TAG, "Flag includes OC_REQUEST_FLAG");
        //TODO :  Handle PUT/DEL methods
        switch (ehRequest->method)
        {
            case OC_REST_GET:
                ret = OC_EH_FORBIDDEN;
                break;
            case OC_REST_POST:
                ret = HandlePostRequest(ehRequest);
                break;
            case OC_REST_DELETE:
                ret = HandleDeleteRequest(ehRequest);
                break;
            default:
                ret = OC_EH_ERROR;
                break;
        }
    }

    //Send payload to request originator
    ret = (SendSRMCBORResponse(ehRequest, ret, NULL) == OC_STACK_OK) ?
                       ret : OC_EH_ERROR;

    return ret;
}

OCStackResult CreateCredResource()
{
    OCStackResult ret = OCCreateResource(&gCredHandle,
                                         OIC_RSRC_TYPE_SEC_CRED,
                                         OIC_MI_DEF,
                                         OIC_RSRC_CRED_URI,
                                         CredEntityHandler,
                                         NULL,
                                         OC_RES_PROP_NONE);

    if (OC_STACK_OK != ret)
    {
        OC_LOG (FATAL, TAG, "Unable to instantiate Cred resource");
        DeInitCredResource();
    }
    return ret;
}

OCStackResult InitCredResource()
{
    OCStackResult ret = OC_STACK_ERROR;

    //Read Cred resource from PS
    uint8_t *data = NULL;
    size_t size = 0;
    ret = GetSecureVirtualDatabaseFromPS(OIC_JSON_CRED_NAME, &data, &size);
    // If database read failed
    if (ret != OC_STACK_OK)
    {
        OC_LOG (DEBUG, TAG, "ReadSVDataFromPS failed");
    }
    if (data)
    {
        // Read ACL resource from PS
        ret = CBORPayloadToCred(data, size, &gCred);
    }

    /*
     * If SVR database in persistent storage got corrupted or
     * is not available for some reason, a default Cred is created
     * which allows user to initiate Cred provisioning again.
     */
    if (ret != OC_STACK_OK || !data || !gCred)
    {
        gCred = GetCredDefault();
    }
    //Instantiate 'oic.sec.cred'
    ret = CreateCredResource();
    OICFree(data);
    return ret;
}

OCStackResult DeInitCredResource()
{
    OCStackResult result = OCDeleteResource(gCredHandle);
    DeleteCredList(gCred);
    gCred = NULL;
    return result;
}

const OicSecCred_t* GetCredResourceData(const OicUuid_t* subject)
{
    OicSecCred_t *cred = NULL;

   if ( NULL == subject)
    {
       return NULL;
    }

    LL_FOREACH(gCred, cred)
    {
        if(memcmp(cred->subject.id, subject->id, sizeof(subject->id)) == 0)
        {
            return cred;
        }
    }
    return NULL;
}


#if defined(__WITH_DTLS__)
int32_t GetDtlsPskCredentials(CADtlsPskCredType_t type,
              const uint8_t *desc, size_t desc_len,
              uint8_t *result, size_t result_length)
{
    int32_t ret = -1;

    if (NULL == result)
    {
        return ret;
    }

    switch (type)
    {
        case CA_DTLS_PSK_HINT:
        case CA_DTLS_PSK_IDENTITY:
            {
                OicUuid_t deviceID = {.id={}};
                // Retrieve Device ID from doxm resource
                if ( OC_STACK_OK != GetDoxmDeviceID(&deviceID) )
                {
                    OC_LOG (ERROR, TAG, "Unable to retrieve doxm Device ID");
                    return ret;
                }

                if (result_length < sizeof(deviceID.id))
                {
                    OC_LOG (ERROR, TAG, "Wrong value for result_length");
                    return ret;
                }
                memcpy(result, deviceID.id, sizeof(deviceID.id));
                return (sizeof(deviceID.id));
            }
            break;

        case CA_DTLS_PSK_KEY:
            {
                OicSecCred_t *cred = NULL;
                LL_FOREACH(gCred, cred)
                {
                    if (cred->credType != SYMMETRIC_PAIR_WISE_KEY)
                    {
                        continue;
                    }

                    if ((desc_len == sizeof(cred->subject.id)) &&
                        (memcmp(desc, cred->subject.id, sizeof(cred->subject.id)) == 0))
                    {
                        /*
                         * If the credentials are valid for limited time,
                         * check their expiry.
                         */
                        if (cred->period)
                        {
                            if(IOTVTICAL_VALID_ACCESS != IsRequestWithinValidTime(cred->period, NULL))
                            {
                                OC_LOG (INFO, TAG, "Credentials are expired.");
                                ret = -1;
                                return ret;
                            }
                        }

                        // Copy PSK.
                        result_length = sizeof(cred->privateData.data);
                        memcpy(result, cred->privateData.data, result_length);
                        return result_length;
                    }
                }
            }
            break;

        default:
            {
                OC_LOG (ERROR, TAG, "Wrong value passed for CADtlsPskCredType_t.");
                ret = -1;
            }
            break;
    }

    return ret;
}

OCStackResult AddTmpPskWithPIN(const OicUuid_t* tmpSubject, OicSecCredType_t credType,
                            const char * pin, size_t pinSize,
                            size_t ownersLen, const OicUuid_t * owners, OicUuid_t* tmpCredSubject)
{
    OCStackResult ret = OC_STACK_ERROR;

    if (tmpSubject == NULL || pin == NULL || pinSize == 0 || tmpCredSubject == NULL)
    {
        return OC_STACK_INVALID_PARAM;
    }

    uint8_t privData[OWNER_PSK_LENGTH_128] = {0,};
    OicSecCred_t* cred = NULL;
    int dtlsRes = DeriveCryptoKeyFromPassword((const unsigned char *)pin, pinSize, owners->id,
                                              UUID_LENGTH, PBKDF_ITERATIONS,
                                              OWNER_PSK_LENGTH_128, privData);
    VERIFY_SUCCESS(TAG, (dtlsRes == 0) , ERROR);

    cred = GenerateCredential(tmpSubject, credType, NULL,
                              privData, ownersLen, owners);
    if(NULL == cred)
    {
        OC_LOG(ERROR, TAG, "GeneratePskWithPIN() : Failed to generate credential");
        return OC_STACK_ERROR;
    }

    memcpy(tmpCredSubject->id, cred->subject.id, UUID_LENGTH);

    ret = AddCredential(cred);
    if( OC_STACK_OK != ret)
    {
        OC_LOG(ERROR, TAG, "GeneratePskWithPIN() : Failed to add credential");
    }

exit:
    return ret;
}

#endif /* __WITH_DTLS__ */
#ifdef __WITH_X509__
#define CERT_LEN_PREFIX (3)
#define BYTE_SIZE (8) //bits
#define PUB_KEY_X_COORD ("x")
#define PUB_KEY_Y_COORD ("y")
#define CERTIFICATE ("x5c")
#define PRIVATE_KEY ("d")

static void WriteCertPrefix(uint8_t *prefix, uint32_t certLen)
{
    for (size_t i = 0; i < CERT_LEN_PREFIX; ++i)
    {
        prefix[i] = (certLen >> (BYTE_SIZE * (CERT_LEN_PREFIX - 1 - i))) & 0xFF;
    }
}

static uint32_t ParseCertPrefix(uint8_t *prefix)
{
    uint32_t res = 0;
    if (NULL != prefix)
    {
        for (int i = 0; i < CERT_LEN_PREFIX; ++i)
        {
            res |= (((uint32_t) prefix[i]) << ((CERT_LEN_PREFIX - 1 -i) * BYTE_SIZE));
        }
    }
    return res;
}

static uint32_t appendCert2Chain(uint8_t *appendPoint, uint8_t *cert, uint32_t max_len)
{
    uint32_t ret = 0;
    VERIFY_NON_NULL(TAG, appendPoint, ERROR);
    VERIFY_NON_NULL(TAG, cert, ERROR);

    memcpy(appendPoint + CERT_LEN_PREFIX, cert, max_len - CERT_LEN_PREFIX);
    certLen = strlen(cert);
    WriteCertPrefix(appendPoint, certLen);

    ret = certLen + CERT_LEN_PREFIX;
exit:
    return ret;
}

static OCStackResult GetCAPublicKeyData(CADtlsX509Creds_t *credInfo)
{
    OCStackResult ret = OC_STACK_ERROR;
    uint8_t *ccPtr = credInfo->certificateChain;
    for (uint32_t i =0; i < credInfo->chainLen - 1; ++i)
    {
        ccPtr += CERT_LEN_PREFIX + ParseCertPrefix(ccPtr);
    }

    ByteArray cert = { .data = ccPtr + CERT_LEN_PREFIX, .len = ParseCertPrefix(ccPtr) };
    CertificateX509 certStruct;

    VERIFY_SUCCESS(TAG, PKI_SUCCESS == DecodeCertificate(cert, &certStruct), ERROR);

    INC_BYTE_ARRAY(certStruct.pubKey, 2);

    memcpy(credInfo->rootPublicKeyX, certStruct.pubKey.data, PUBLIC_KEY_SIZE / 2);
    memcpy(credInfo->rootPublicKeyY, certStruct.pubKey.data + PUBLIC_KEY_SIZE / 2, PUBLIC_KEY_SIZE / 2);

    ret = OC_STACK_OK;
    exit:
    return ret;
}

static OCStackResult GetCertCredPublicData(CADtlsX509Creds_t *credInfo, OicSecCred_t *cred)
{
    OCStackResult ret = OC_STACK_ERROR;
    VERIFY_NON_NULL(TAG, credInfo, ERROR);
    VERIFY_NON_NULL(TAG, cred, ERROR);
    VERIFY_NON_NULL(TAG, cred->publicData.data, ERROR);
    //VERIFY_SUCCESS(TAG, NULL == credInfo->certificateChain.data, ERROR);
    (cred->publicData.data);


    //Get certificate chain
    cJSON *jsonObj = cJSON_GetObjectItem(jsonRoot, CERTIFICATE);//TODO define field names constants
    VERIFY_SUCCESS(TAG, NULL != jsonObj && cJSON_Array == jsonObj->type, ERROR);

    size_t certChainLen = cJSON_GetArraySize(jsonObj);
    credInfo->chainLen = certChainLen;
    VERIFY_SUCCESS(TAG, MAX_CHAIN_LEN >= certChainLen, ERROR);

    uint32_t len = 0;
    for (size_t i = 0; i < certChainLen; ++i)
    {
        cJSON *item = cJSON_GetArrayItem(jsonObj, i);
        VERIFY_SUCCESS(TAG, cJSON_String == item->type, ERROR);
        uint32_t appendedLen = appendCert2Chain(credInfo->certificateChain + len, item->valuestring,
                                              MAX_CERT_MESSAGE_LEN - len);
        VERIFY_SUCCESS(TAG, 0 != appendedLen, ERROR);
        len += appendedLen;
    }
    credInfo->certificateChainLen = len;
    VERIFY_SUCCESS(TAG, OC_STACK_OK == GetCAPublicKeyData(credInfo), ERROR);
    ret = OC_STACK_OK;
exit:
    cJSON_Delete(jsonRoot);
    return ret;
}

static OCStackResult GetCertCredPrivateData(CADtlsX509Creds_t *credInfo, OicSecCred_t *cred)
{
    OCStackResult ret = OC_STACK_ERROR;
    VERIFY_NON_NULL(TAG, credInfo, ERROR);
    VERIFY_NON_NULL(TAG, cred, ERROR);
    VERIFY_NON_NULL(TAG, cred->privateData.data, ERROR);
    memcpy(credInfo->devicePrivateKey, cred->privateData.data, PRIVATE_KEY_SIZE);
    ret = OC_STACK_OK;

exit:
    return ret;
}

int GetDtlsX509Credentials(CADtlsX509Creds_t *credInfo)
{
    int ret = 1;
    VERIFY_NON_NULL(TAG, credInfo, ERROR);
    if (NULL == gCred)
    {
        VERIFY_SUCCESS(TAG, OC_STACK_OK == InitCredResource(), ERROR);
    }

    OicSecCred_t *cred = NULL;
    LL_SEARCH_SCALAR(gCred, cred, credType, SIGNED_ASYMMETRIC_KEY);
    VERIFY_NON_NULL(TAG, cred, ERROR);

    VERIFY_SUCCESS(TAG, OC_STACK_OK == GetCertCredPrivateData(credInfo, cred), ERROR);
    VERIFY_SUCCESS(TAG, OC_STACK_OK == GetCertCredPublicData(credInfo, cred), ERROR);

    ret = 0;
exit:

    return ret;
}
#undef CERT_LEN_PREFIX
#endif /* __WITH_X509__ */
