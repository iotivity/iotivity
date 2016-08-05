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
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#include <stdint.h>

#include "cainterface.h"
#include "payload_logging.h"
#include "ocstack.h"
#include "ocrandom.h"
#include "base64.h"
#include "ocserverrequest.h"
#include "oic_malloc.h"
#include "ocpayload.h"
#include "utlist.h"
#include "credresource.h"
#include "doxmresource.h"
#include "pstatresource.h"
#include "iotvticalendar.h"
#include "pbkdf2.h"
#include "resourcemanager.h"
#include "srmresourcestrings.h"
#include "srmutility.h"
#include "psinterface.h"
#include "pinoxmcommon.h"

#ifdef __WITH_DTLS__
#include "global.h"
#endif

#define TAG  "SRM-CREDL"

/** Default cbor payload size. This value is increased in case of CborErrorOutOfMemory.
 * The value of payload size is increased until reaching belox max cbor size. */
static const uint16_t CBOR_SIZE = 2048;

/** Max cbor size payload. */
static const uint16_t CBOR_MAX_SIZE = 4400;

/** CRED size - Number of mandatory items. */
static const uint8_t CRED_ROOT_MAP_SIZE = 4;
static const uint8_t CRED_MAP_SIZE = 3;


static OicSecCred_t        *gCred = NULL;
static OCResourceHandle    gCredHandle = NULL;

/**
 * This function frees OicSecCred_t object's fields and object itself.
 */
static void FreeCred(OicSecCred_t *cred)
{
    if(NULL == cred)
    {
        OIC_LOG(ERROR, TAG, "Invalid Parameter");
        return;
    }
    //Note: Need further clarification on roleID data type
#if 0
    //Clean roleIds
    OICFree(cred->roleIds);
#endif

    //Clean PublicData/OptionalData/Credusage
#if defined(__WITH_X509__) || defined(__WITH_TLS__)
     // TODO: Need to check credUsage.
    OICFree(cred->publicData.data);
    OICFree(cred->optionalData.data);
    OICFree(cred->credUsage);

#endif /* __WITH_X509__ ||  __WITH_TLS__*/

    //Clean PrivateData
    OICFree(cred->privateData.data);

    //Clean Period
    OICFree(cred->period);

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
                                size_t *cborSize, int secureFlag)
{
    if (NULL == credS || NULL == cborPayload || NULL != *cborPayload || NULL == cborSize)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult ret = OC_STACK_ERROR;

    CborError cborEncoderResult = CborNoError;
    uint8_t *outPayload = NULL;
    size_t cborLen = *cborSize;
    *cborSize = 0;
    *cborPayload = NULL;
    const OicSecCred_t *cred = credS;
    CborEncoder encoder;
    CborEncoder credArray;
    CborEncoder credRootMap;

    if (0 == cborLen)
    {
        cborLen = CBOR_SIZE;
    }

    outPayload = (uint8_t *)OICCalloc(1, cborLen);
    VERIFY_NON_NULL(TAG, outPayload, ERROR);
    cbor_encoder_init(&encoder, outPayload, cborLen, 0);

    // Create CRED Root Map (creds, rownerid)
    cborEncoderResult = cbor_encoder_create_map(&encoder, &credRootMap, CRED_ROOT_MAP_SIZE);
    VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding Cred Root Map");

    // creds
    cborEncoderResult = cbor_encode_text_string(&credRootMap, OIC_JSON_CREDS_NAME,
        strlen(OIC_JSON_CREDS_NAME));
    VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding creds Name Tag.");

    // creds array
    cborEncoderResult = cbor_encoder_create_array(&credRootMap, &credArray, OicSecCredCount(cred));
    VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding Cred Array.");

    while (cred)
    {
        CborEncoder credMap;
        size_t mapSize = CRED_MAP_SIZE;
        size_t inLen = 0;
        if (cred->period)
        {
            mapSize++;
        }
#if defined(__WITH_X509__) || defined(__WITH_TLS__)
        if (SIGNED_ASYMMETRIC_KEY == cred->credType && cred->publicData.data)
        {
            mapSize++;
        }
        if (SIGNED_ASYMMETRIC_KEY == cred->credType && cred->optionalData.data)
        {
            mapSize++;
        }
        if (cred->credUsage)
        {
            mapSize++;
        }
#endif /* __WITH_X509__ ||  __WITH_TLS__*/
        if (!secureFlag && cred->privateData.data)
        {
            mapSize++;
        }
        cborEncoderResult = cbor_encoder_create_map(&credArray, &credMap, mapSize);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding Cred Map");

        //CredID -- Mandatory
        cborEncoderResult = cbor_encode_text_string(&credMap, OIC_JSON_CREDID_NAME,
            strlen(OIC_JSON_CREDID_NAME));
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding Cred Id Tag. ");
        cborEncoderResult = cbor_encode_int(&credMap, cred->credId);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding Cred Id Value.");

        //Subject -- Mandatory
        cborEncoderResult = cbor_encode_text_string(&credMap, OIC_JSON_SUBJECTID_NAME,
            strlen(OIC_JSON_SUBJECTID_NAME));
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding Subject Tag.");
        inLen = (memcmp(&(cred->subject), &WILDCARD_SUBJECT_ID, WILDCARD_SUBJECT_ID_LEN) == 0) ?
            WILDCARD_SUBJECT_ID_LEN : sizeof(OicUuid_t);
        if(inLen == WILDCARD_SUBJECT_ID_LEN)
        {
            cborEncoderResult = cbor_encode_text_string(&credMap, WILDCARD_RESOURCE_URI,
                strlen(WILDCARD_RESOURCE_URI));
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Addding Subject Id wildcard Value.");
        }
        else
        {
            char *subject = NULL;
            ret = ConvertUuidToStr(&cred->subject, &subject);
            VERIFY_SUCCESS(TAG, ret == OC_STACK_OK, ERROR);
            cborEncoderResult = cbor_encode_text_string(&credMap, subject, strlen(subject));
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Addding Subject Id Value.");
            OICFree(subject);
        }

        //CredType -- Mandatory
        cborEncoderResult = cbor_encode_text_string(&credMap, OIC_JSON_CREDTYPE_NAME,
            strlen(OIC_JSON_CREDTYPE_NAME));
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding Cred Type Tag.");
        cborEncoderResult = cbor_encode_int(&credMap, cred->credType);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding Cred Type Value.");

#if defined(__WITH_X509__) || defined(__WITH_TLS__)
        //PublicData -- Not Mandatory
        if (SIGNED_ASYMMETRIC_KEY == cred->credType && cred->publicData.data)
        {
            CborEncoder publicMap;
            const size_t publicMapSize = 2;

            cborEncoderResult = cbor_encode_text_string(&credMap, OIC_JSON_PUBLICDATA_NAME,
                strlen(OIC_JSON_PUBLICDATA_NAME));
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding PublicData Tag.");

            cborEncoderResult = cbor_encoder_create_map(&credMap, &publicMap, publicMapSize);
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding PublicData Map");

            cborEncoderResult = cbor_encode_text_string(&publicMap, OIC_JSON_DATA_NAME,
                strlen(OIC_JSON_DATA_NAME));
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding Pub Data Tag.");
            cborEncoderResult = cbor_encode_byte_string(&publicMap, cred->publicData.data,
                cred->publicData.len);
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding Pub Value.");

            // TODO: Need to data strucure modification for OicSecCert_t.
            cborEncoderResult = cbor_encode_text_string(&publicMap, OIC_JSON_ENCODING_NAME,
                strlen(OIC_JSON_ENCODING_NAME));
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding Public Encoding Tag.");
            cborEncoderResult = cbor_encode_text_string(&publicMap, OIC_SEC_ENCODING_RAW,
                strlen(OIC_SEC_ENCODING_RAW));
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding Public Encoding Value.");

            cborEncoderResult = cbor_encoder_close_container(&credMap, &publicMap);
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Closing PublicData Map.");
        }
        //OptionalData -- Not Mandatory
        if (SIGNED_ASYMMETRIC_KEY == cred->credType && cred->optionalData.data)
        {
            CborEncoder optionalMap;
            const size_t optionalMapSize = 2;

            cborEncoderResult = cbor_encode_text_string(&credMap, OIC_JSON_OPTDATA_NAME,
                strlen(OIC_JSON_OPTDATA_NAME));
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding OptionalData Tag.");

            cborEncoderResult = cbor_encoder_create_map(&credMap, &optionalMap, optionalMapSize);
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding OptionalData Map");

            // TODO: Need to data strucure modification for OicSecCert_t.
            if(OIC_ENCODING_RAW == cred->optionalData.encoding)
            {
                cborEncoderResult = cbor_encode_text_string(&optionalMap, OIC_JSON_ENCODING_NAME,
                    strlen(OIC_JSON_ENCODING_NAME));
                VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding optional Encoding Tag.");
                cborEncoderResult = cbor_encode_text_string(&optionalMap, OIC_SEC_ENCODING_RAW,
                    strlen(OIC_SEC_ENCODING_RAW));
                VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding optional Encoding Value.");

                cborEncoderResult = cbor_encode_text_string(&optionalMap, OIC_JSON_DATA_NAME,
                    strlen(OIC_JSON_DATA_NAME));
                VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding optional Tag.");
                cborEncoderResult = cbor_encode_byte_string(&optionalMap, cred->optionalData.data,
                    cred->optionalData.len);
                VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding optional Value.");
            }
            else if(OIC_ENCODING_BASE64 == cred->optionalData.encoding)
            {
                cborEncoderResult = cbor_encode_text_string(&optionalMap, OIC_JSON_ENCODING_NAME,
                    strlen(OIC_JSON_ENCODING_NAME));
                VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding optional Encoding Tag.");
                cborEncoderResult = cbor_encode_text_string(&optionalMap, OIC_SEC_ENCODING_BASE64,
                    strlen(OIC_SEC_ENCODING_BASE64));
                VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding optional Encoding Value.");

                cborEncoderResult = cbor_encode_text_string(&optionalMap, OIC_JSON_DATA_NAME,
                    strlen(OIC_JSON_DATA_NAME));
                VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding optional Tag.");
                cborEncoderResult = cbor_encode_text_string(&optionalMap, (char*)(cred->optionalData.data),
                    cred->optionalData.len);
                VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding optional Value.");
            }
            else if(OIC_ENCODING_PEM == cred->optionalData.encoding)
            {
                cborEncoderResult = cbor_encode_text_string(&optionalMap, OIC_JSON_ENCODING_NAME,
                    strlen(OIC_JSON_ENCODING_NAME));
                VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding optional Encoding Tag.");
                cborEncoderResult = cbor_encode_text_string(&optionalMap, OIC_SEC_ENCODING_PEM,
                    strlen(OIC_SEC_ENCODING_PEM));
                VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding optional Encoding Value.");

                cborEncoderResult = cbor_encode_text_string(&optionalMap, OIC_JSON_DATA_NAME,
                    strlen(OIC_JSON_DATA_NAME));
                VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding optional Tag.");
                cborEncoderResult = cbor_encode_text_string(&optionalMap, (char*)(cred->optionalData.data),
                    cred->optionalData.len);
                VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding optional Value.");
            }
            else if(OIC_ENCODING_DER == cred->optionalData.encoding)
            {
                cborEncoderResult = cbor_encode_text_string(&optionalMap, OIC_JSON_ENCODING_NAME,
                    strlen(OIC_JSON_ENCODING_NAME));
                VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding optional Encoding Tag.");
                cborEncoderResult = cbor_encode_text_string(&optionalMap, OIC_SEC_ENCODING_DER,
                    strlen(OIC_SEC_ENCODING_DER));
                VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding optional Encoding Value.");

                cborEncoderResult = cbor_encode_text_string(&optionalMap, OIC_JSON_DATA_NAME,
                    strlen(OIC_JSON_DATA_NAME));
                VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding optional Tag.");
                cborEncoderResult = cbor_encode_byte_string(&optionalMap, cred->optionalData.data,
                    cred->optionalData.len);
                VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding optional Value.");
            }
            else
            {
                OIC_LOG(ERROR, TAG, "Unknow encoding type for optional data.");
                VERIFY_CBOR_SUCCESS(TAG, CborErrorUnknownType, "Failed Adding optional Encoding Value.");
            }

            cborEncoderResult = cbor_encoder_close_container(&credMap, &optionalMap);
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Closing OptionalData Map.");
        }
        //CredUsage -- Not Mandatory
        if(cred->credUsage)
        {
            cborEncoderResult = cbor_encode_text_string(&credMap, OIC_JSON_CREDUSAGE_NAME,
                strlen(OIC_JSON_CREDUSAGE_NAME));
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding Credusage Name Tag.");
            cborEncoderResult = cbor_encode_text_string(&credMap, cred->credUsage,
                strlen(cred->credUsage));
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding Credusage Name Value.");
        }
#endif /* __WITH_X509__ ||  __WITH_TLS__*/
        //PrivateData -- Not Mandatory
        if(!secureFlag && cred->privateData.data)
        {
            CborEncoder privateMap;
            const size_t privateMapSize = 2;

            cborEncoderResult = cbor_encode_text_string(&credMap, OIC_JSON_PRIVATEDATA_NAME,
                strlen(OIC_JSON_PRIVATEDATA_NAME));
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding PrivateData Tag.");

            cborEncoderResult = cbor_encoder_create_map(&credMap, &privateMap, privateMapSize);
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding PrivateData Map");

            // TODO: Need to data strucure modification for OicSecKey_t.
            // TODO: Added as workaround, will be replaced soon.
            if(OIC_ENCODING_RAW == cred->privateData.encoding)
            {
                cborEncoderResult = cbor_encode_text_string(&privateMap, OIC_JSON_ENCODING_NAME,
                    strlen(OIC_JSON_ENCODING_NAME));
                VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding Private Encoding Tag.");
                cborEncoderResult = cbor_encode_text_string(&privateMap, OIC_SEC_ENCODING_RAW,
                    strlen(OIC_SEC_ENCODING_RAW));
                VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding Private Encoding Value.");

                cborEncoderResult = cbor_encode_text_string(&privateMap, OIC_JSON_DATA_NAME,
                    strlen(OIC_JSON_DATA_NAME));
                VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding Priv Tag.");
                cborEncoderResult = cbor_encode_byte_string(&privateMap, cred->privateData.data,
                    cred->privateData.len);
                VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding Priv Value.");
            }
            else if(OIC_ENCODING_BASE64 == cred->privateData.encoding)
            {
                cborEncoderResult = cbor_encode_text_string(&privateMap, OIC_JSON_ENCODING_NAME,
                    strlen(OIC_JSON_ENCODING_NAME));
                VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding Private Encoding Tag.");
                cborEncoderResult = cbor_encode_text_string(&privateMap, OIC_SEC_ENCODING_BASE64,
                    strlen(OIC_SEC_ENCODING_BASE64));
                VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding Private Encoding Value.");

                cborEncoderResult = cbor_encode_text_string(&privateMap, OIC_JSON_DATA_NAME,
                    strlen(OIC_JSON_DATA_NAME));
                VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding Priv Tag.");
                cborEncoderResult = cbor_encode_text_string(&privateMap, (char*)(cred->privateData.data),
                    cred->privateData.len);
                VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding Priv Value.");
            }
            else
            {
                OIC_LOG(ERROR, TAG, "Unknow encoding type for private data.");
                VERIFY_CBOR_SUCCESS(TAG, CborErrorUnknownType, "Failed Adding Private Encoding Value.");
            }

            cborEncoderResult = cbor_encoder_close_container(&credMap, &privateMap);
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Closing PrivateData Map.");
        }

        //Period -- Not Mandatory
        if(cred->period)
        {
            cborEncoderResult = cbor_encode_text_string(&credMap, OIC_JSON_PERIOD_NAME,
                strlen(OIC_JSON_PERIOD_NAME));
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding Period Name Tag.");
            cborEncoderResult = cbor_encode_text_string(&credMap, cred->period,
                strlen(cred->period));
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding Period Name Value.");
        }


        cborEncoderResult = cbor_encoder_close_container(&credArray, &credMap);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Closing Cred Map.");

        cred = cred->next;
    }
    cborEncoderResult = cbor_encoder_close_container(&credRootMap, &credArray);
    VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Closing Cred Array.");

    cred = credS;

    // Rownerid
    {
        char *rowner = NULL;
        cborEncoderResult = cbor_encode_text_string(&credRootMap, OIC_JSON_ROWNERID_NAME,
            strlen(OIC_JSON_ROWNERID_NAME));
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding rownerid Name.");
        ret = ConvertUuidToStr(&cred->rownerID, &rowner);
        VERIFY_SUCCESS(TAG, ret == OC_STACK_OK, ERROR);
        cborEncoderResult = cbor_encode_text_string(&credRootMap, rowner, strlen(rowner));
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Addding rownerid Value.");
        OICFree(rowner);
    }

    //RT -- Mandatory
    CborEncoder rtArray;
    cborEncoderResult = cbor_encode_text_string(&credRootMap, OIC_JSON_RT_NAME,
            strlen(OIC_JSON_RT_NAME));
    VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Addding RT Name Tag.");
    cborEncoderResult = cbor_encoder_create_array(&credRootMap, &rtArray, 1);
    VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Addding RT Value.");
    for (size_t i = 0; i < 1; i++)
    {
        cborEncoderResult = cbor_encode_text_string(&rtArray, OIC_RSRC_TYPE_SEC_CRED,
                strlen(OIC_RSRC_TYPE_SEC_CRED));
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding RT Value.");
    }
    cborEncoderResult = cbor_encoder_close_container(&credRootMap, &rtArray);
    VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Closing RT.");

    //IF-- Mandatory
    CborEncoder ifArray;
    cborEncoderResult = cbor_encode_text_string(&credRootMap, OIC_JSON_IF_NAME,
             strlen(OIC_JSON_IF_NAME));
    VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Addding IF Name Tag.");
    cborEncoderResult = cbor_encoder_create_array(&credRootMap, &ifArray, 1);
    VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Addding IF Value.");
    for (size_t i = 0; i < 1; i++)
    {
        cborEncoderResult = cbor_encode_text_string(&ifArray, OC_RSRVD_INTERFACE_DEFAULT,
                strlen(OC_RSRVD_INTERFACE_DEFAULT));
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding IF Value.");
    }
    cborEncoderResult = cbor_encoder_close_container(&credRootMap, &ifArray);
    VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Closing IF.");


    // Close CRED Root Map
    cborEncoderResult = cbor_encoder_close_container(&encoder, &credRootMap);
    VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Closing CRED Root Map.");

    if (CborNoError == cborEncoderResult)
    {
        OIC_LOG(DEBUG, TAG, "CredToCBORPayload Successed");
        *cborPayload = outPayload;
        *cborSize = encoder.ptr - outPayload;
        ret = OC_STACK_OK;
    }
    OIC_LOG(DEBUG, TAG, "CredToCBORPayload OUT");
exit:
    if (CborErrorOutOfMemory == cborEncoderResult)
    {
        OIC_LOG(DEBUG, TAG, "CredToCBORPayload:CborErrorOutOfMemory : retry with more memory");
        // reallocate and try again!
        OICFree(outPayload);
        // Since the allocated initial memory failed, double the memory.
        cborLen += encoder.ptr - encoder.end;
        cborEncoderResult = CborNoError;
        ret = CredToCBORPayload(credS, cborPayload, &cborLen, secureFlag);
        *cborSize = cborLen;
    }

    if (CborNoError != cborEncoderResult)
    {
        OIC_LOG(ERROR, TAG, "Failed to CredToCBORPayload");
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
    if (NULL == cborPayload || NULL == secCred || NULL != *secCred || 0 == size)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult ret = OC_STACK_ERROR;
    CborValue credCbor = { .parser = NULL };
    CborParser parser = { .end = NULL };
    CborError cborFindResult = CborNoError;
    cbor_parser_init(cborPayload, size, 0, &parser, &credCbor);

    OicSecCred_t *headCred = (OicSecCred_t *) OICCalloc(1, sizeof(OicSecCred_t));

    // Enter CRED Root Map
    CborValue CredRootMap = { .parser = NULL, .ptr = NULL, .remaining = 0, .extra = 0, .type = 0, .flags = 0 };
    cborFindResult = cbor_value_enter_container(&credCbor, &CredRootMap);
    VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Entering CRED Root Map.");

    while (cbor_value_is_valid(&CredRootMap))
    {
        char* tagName = NULL;
        size_t len = 0;
        CborType type = cbor_value_get_type(&CredRootMap);
        if (type == CborTextStringType && cbor_value_is_text_string(&CredRootMap))
        {
            cborFindResult = cbor_value_dup_text_string(&CredRootMap, &tagName, &len, NULL);
            VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Finding Name in CRED Root Map.");
            cborFindResult = cbor_value_advance(&CredRootMap);
            VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Advancing Value in CRED Root Map.");
        }
        if(tagName)
        {
            if (strcmp(tagName, OIC_JSON_CREDS_NAME)  == 0)
            {
                // Enter CREDS Array
                size_t len = 0;
                int credCount = 0;
                CborValue credArray = { .parser = NULL, .ptr = NULL, .remaining = 0, .extra = 0, .type = 0, .flags = 0 };
                cborFindResult = cbor_value_enter_container(&CredRootMap, &credArray);
                VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Finding Cred Array.");

                while (cbor_value_is_valid(&credArray))
                {
                    credCount++;
                    //CredId -- Mandatory
                    CborValue credMap = { .parser = NULL, .ptr = NULL, .remaining = 0, .extra = 0, .type = 0, .flags = 0 };
                    cborFindResult = cbor_value_enter_container(&credArray, &credMap);
                    VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Finding Cred Map.");
                    OicSecCred_t *cred = NULL;

                    if(1 == credCount)
                    {
                        cred = headCred;
                    }
                    else
                    {
                        cred = (OicSecCred_t *) OICCalloc(1, sizeof(OicSecCred_t));
                        OicSecCred_t *temp = headCred;
                        while (temp->next)
                        {
                            temp = temp->next;
                        }
                        temp->next = cred;
                    }

                    VERIFY_NON_NULL(TAG, cred, ERROR);

                    while(cbor_value_is_valid(&credMap) && cbor_value_is_text_string(&credMap))
                    {
                        char* name = NULL;
                        CborType type = cbor_value_get_type(&credMap);
                        if (type == CborTextStringType)
                        {
                            cborFindResult = cbor_value_dup_text_string(&credMap, &name, &len, NULL);
                            VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Finding Name in CRED Map.");
                            cborFindResult = cbor_value_advance(&credMap);
                            VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Advancing Value in CRED Map.");
                        }
                        if(name)
                        {
                            //credid
                            if (strcmp(name, OIC_JSON_CREDID_NAME)  == 0)
                            {
                                uint64_t credId = 0;
                                cborFindResult = cbor_value_get_uint64(&credMap, &credId);
                                VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Finding CredId.");
                                cred->credId = (uint16_t)credId;
                            }
                            // subjectid
                            if (strcmp(name, OIC_JSON_SUBJECTID_NAME)  == 0)
                            {
                                char *subjectid = NULL;
                                cborFindResult = cbor_value_dup_text_string(&credMap, &subjectid, &len, NULL);
                                VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Finding subjectid Value.");
                                if(strcmp(subjectid, WILDCARD_RESOURCE_URI) == 0)
                                {
                                    cred->subject.id[0] = '*';
                                }
                                else
                                {
                                    ret = ConvertStrToUuid(subjectid, &cred->subject);
                                    VERIFY_SUCCESS(TAG, ret == OC_STACK_OK, ERROR);
                                }
                                OICFree(subjectid);
                            }
                            // credtype
                            if (strcmp(name, OIC_JSON_CREDTYPE_NAME)  == 0)
                            {
                                uint64_t credType = 0;
                                cborFindResult = cbor_value_get_uint64(&credMap, &credType);
                                VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Finding CredType.");
                                cred->credType = (OicSecCredType_t)credType;
                            }
                            // privatedata
                            if (strcmp(name, OIC_JSON_PRIVATEDATA_NAME)  == 0)
                            {
                                CborValue privateMap = { .parser = NULL };
                                cborFindResult = cbor_value_enter_container(&credMap, &privateMap);

                                while (cbor_value_is_valid(&privateMap))
                                {
                                    char* privname = NULL;
                                    CborType type = cbor_value_get_type(&privateMap);
                                    if (type == CborTextStringType && cbor_value_is_text_string(&privateMap))
                                    {
                                        cborFindResult = cbor_value_dup_text_string(&privateMap, &privname,
                                                &len, NULL);
                                        VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed to get text");
                                        cborFindResult = cbor_value_advance(&privateMap);
                                        VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed to advance value");
                                    }
                                    if (privname)
                                    {
                                        // PrivateData::privdata -- Mandatory
                                        if (strcmp(privname, OIC_JSON_DATA_NAME) == 0)
                                        {
                                            if(cbor_value_is_byte_string(&privateMap))
                                            {
                                                cborFindResult = cbor_value_dup_byte_string(&privateMap, &cred->privateData.data,
                                                    &cred->privateData.len, NULL);
                                            }
                                            else if(cbor_value_is_text_string(&privateMap))
                                            {
                                                cborFindResult = cbor_value_dup_text_string(&privateMap, (char**)(&cred->privateData.data),
                                                    &cred->privateData.len, NULL);
                                            }
                                            else
                                            {
                                                cborFindResult = CborErrorUnknownType;
                                                OIC_LOG(ERROR, TAG, "Unknow type for private data.");
                                            }
                                            VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Finding PrivateData.");
                                        }

                                        // PrivateData::encoding -- Mandatory
                                        if (strcmp(privname, OIC_JSON_ENCODING_NAME) == 0)
                                        {
                                            // TODO: Added as workaround. Will be replaced soon.
                                            char* strEncoding = NULL;
                                            cborFindResult = cbor_value_dup_text_string(&privateMap, &strEncoding, &len, NULL);
                                            VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Finding EncodingType");

                                            if(strcmp(strEncoding, OIC_SEC_ENCODING_RAW) == 0)
                                            {
                                                cred->privateData.encoding = OIC_ENCODING_RAW;
                                            }
                                            else if(strcmp(strEncoding, OIC_SEC_ENCODING_BASE64) == 0)
                                            {
                                                cred->privateData.encoding = OIC_ENCODING_BASE64;
                                            }
                                            else
                                            {
                                                //For unit test
                                                cred->privateData.encoding = OIC_ENCODING_RAW;
                                                OIC_LOG(WARNING, TAG, "Unknow encoding type dectected for private data.");
                                            }

                                            OICFree(strEncoding);
                                        }
                                    }
                                    if (cbor_value_is_valid(&privateMap))
                                    {
                                        cborFindResult = cbor_value_advance(&privateMap);
                                        VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Advancing privatedata Map.");
                                    }
                                    OICFree(privname);
                                }

                            }
#if defined(__WITH_X509__) || defined(__WITH_TLS__)
                            //PublicData -- Not Mandatory
                            if (strcmp(name, OIC_JSON_PUBLICDATA_NAME)  == 0)
                            {
                                CborValue pubMap = { .parser = NULL };
                                cborFindResult = cbor_value_enter_container(&credMap, &pubMap);

                                while (cbor_value_is_valid(&pubMap))
                                {
                                    char* pubname = NULL;
                                    CborType type = cbor_value_get_type(&pubMap);
                                    if (type == CborTextStringType && cbor_value_is_text_string(&pubMap))
                                    {
                                        cborFindResult = cbor_value_dup_text_string(&pubMap, &pubname,
                                                &len, NULL);
                                        VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed to get text");
                                        cborFindResult = cbor_value_advance(&pubMap);
                                        VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed to advance value");
                                    }
                                    if (pubname)
                                    {
                                        // PrivateData::privdata -- Mandatory
                                        if (strcmp(pubname, OIC_JSON_DATA_NAME) == 0 && cbor_value_is_byte_string(&pubMap))
                                        {
                                            cborFindResult = cbor_value_dup_byte_string(&pubMap, &cred->publicData.data,
                                                &cred->publicData.len, NULL);
                                            VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Finding PubData.");
                                        }
                                        // PublicData::encoding -- Mandatory
                                        if (strcmp(pubname, OIC_JSON_ENCODING_NAME) == 0)
                                        {
                                            // TODO: Need to update data structure, just ignore encoding value now.
                                        }
                                    }
                                    if (cbor_value_is_valid(&pubMap))
                                    {
                                        cborFindResult = cbor_value_advance(&pubMap);
                                        VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Advancing publicdata Map.");
                                    }
                                    OICFree(pubname);
                                }
                            }
                            //OptionalData -- Not Mandatory
                            if (strcmp(name, OIC_JSON_OPTDATA_NAME)  == 0)
                            {
                                CborValue optMap = { .parser = NULL };
                                cborFindResult = cbor_value_enter_container(&credMap, &optMap);

                                while (cbor_value_is_valid(&optMap))
                                {
                                    char* optname = NULL;
                                    CborType type = cbor_value_get_type(&optMap);
                                    if (type == CborTextStringType && cbor_value_is_text_string(&optMap))
                                    {
                                        cborFindResult = cbor_value_dup_text_string(&optMap, &optname,
                                                &len, NULL);
                                        VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed to get text");
                                        cborFindResult = cbor_value_advance(&optMap);
                                        VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed to advance value");
                                    }
                                    if (optname)
                                    {
                                        // OptionalData::optdata -- Mandatory
                                        if (strcmp(optname, OIC_JSON_DATA_NAME) == 0)
                                        {
                                            if(cbor_value_is_byte_string(&optMap))
                                            {
                                                cborFindResult = cbor_value_dup_byte_string(&optMap, &cred->optionalData.data,
                                                    &cred->optionalData.len, NULL);
                                            }
                                            else if(cbor_value_is_text_string(&optMap))
                                            {
                                                cborFindResult = cbor_value_dup_text_string(&optMap, (char**)(&cred->optionalData.data),
                                                    &cred->optionalData.len, NULL);
                                            }
                                            else
                                            {
                                                cborFindResult = CborErrorUnknownType;
                                                OIC_LOG(ERROR, TAG, "Unknow type for optional data.");
                                            }
                                            VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Finding OptionalData.");
                                        }
                                        // OptionalData::encoding -- Mandatory
                                        if (strcmp(optname, OIC_JSON_ENCODING_NAME) == 0)
                                        {
                                            // TODO: Added as workaround. Will be replaced soon.
                                            char* strEncoding = NULL;
                                            cborFindResult = cbor_value_dup_text_string(&optMap, &strEncoding, &len, NULL);
                                            VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Finding EncodingType");

                                            if(strcmp(strEncoding, OIC_SEC_ENCODING_RAW) == 0)
                                            {
                                                OIC_LOG(INFO,TAG,"cbor_value_is_byte_string");
                                                cred->optionalData.encoding = OIC_ENCODING_RAW;
                                            }
                                            else if(strcmp(strEncoding, OIC_SEC_ENCODING_BASE64) == 0)
                                            {
                                                cred->optionalData.encoding = OIC_ENCODING_BASE64;
                                            }
                                            else if(strcmp(strEncoding, OIC_SEC_ENCODING_PEM) == 0)
                                            {
                                                cred->optionalData.encoding = OIC_ENCODING_PEM;
                                            }
                                            else if(strcmp(strEncoding, OIC_SEC_ENCODING_DER) == 0)
                                            {
                                                cred->optionalData.encoding = OIC_ENCODING_DER;
                                            }
                                            else
                                            {
                                                //For unit test
                                                cred->optionalData.encoding = OIC_ENCODING_RAW;
                                                OIC_LOG(WARNING, TAG, "Unknow encoding type dectected for optional data.");
                                            }
                                            OICFree(strEncoding);
                                        }
                                    }
                                    if (cbor_value_is_valid(&optMap))
                                    {
                                        cborFindResult = cbor_value_advance(&optMap);
                                        VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Advancing optdata Map.");
                                    }
                                    OICFree(optname);
                                }
                            }
                            //Credusage -- Not Mandatory
                            if (0 == strcmp(OIC_JSON_CREDUSAGE_NAME, name))
                            {
                                cborFindResult = cbor_value_dup_text_string(&credMap, &cred->credUsage, &len, NULL);
                                VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Finding Period.");
                            }
#endif  //__WITH_X509__ ||  __WITH_TLS__

                            if (0 == strcmp(OIC_JSON_PERIOD_NAME, name))
                            {
                                cborFindResult = cbor_value_dup_text_string(&credMap, &cred->period, &len, NULL);
                                VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Finding Period.");
                            }

                            if (cbor_value_is_valid(&credMap))
                            {
                                cborFindResult = cbor_value_advance(&credMap);
                                VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Advancing CRED Map.");
                            }
                            OICFree(name);
                        }
                    }
                    cred->next = NULL;
                    if (cbor_value_is_valid(&credArray))
                    {
                        cborFindResult = cbor_value_advance(&credArray);
                        VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Advancing CRED Array.");
                    }
                }
            }

            //ROwner -- Mandatory
            if (strcmp(tagName, OIC_JSON_ROWNERID_NAME)  == 0 && cbor_value_is_text_string(&CredRootMap))
            {
                char *stRowner = NULL;
                cborFindResult = cbor_value_dup_text_string(&CredRootMap, &stRowner, &len, NULL);
                VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Finding Rownerid Value.");

                ret = ConvertStrToUuid(stRowner, &headCred->rownerID);
                VERIFY_SUCCESS(TAG, ret == OC_STACK_OK, ERROR);
                OICFree(stRowner);
            }
            OICFree(tagName);
        }
        if (cbor_value_is_valid(&CredRootMap))
        {
            cborFindResult = cbor_value_advance(&CredRootMap);
            VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Advancing CRED Root Map.");
        }
    }

    *secCred = headCred;
    ret = OC_STACK_OK;

exit:
    if (CborNoError != cborFindResult)
    {
        DeleteCredList(headCred);
        headCred = NULL;
        *secCred = NULL;
        ret = OC_STACK_ERROR;
    }

    return ret;
}

OicSecCred_t * GenerateCredential(const OicUuid_t * subject, OicSecCredType_t credType,
                                  const OicSecCert_t * publicData, const OicSecKey_t* privateData,
                                  const OicUuid_t * rownerID)
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
    if (publicData && publicData->data)
    {
        cred->publicData.data = (uint8_t *)OICCalloc(1, publicData->len);
        VERIFY_NON_NULL(TAG, cred->publicData.data, ERROR);
        memcpy(cred->publicData.data, publicData->data, publicData->len);
        cred->publicData.len = publicData->len;
    }
#endif // __WITH_X509__

    if (privateData && privateData->data)
    {
        cred->privateData.data = (uint8_t *)OICCalloc(1, privateData->len);
        VERIFY_NON_NULL(TAG, cred->privateData.data, ERROR);
        memcpy(cred->privateData.data, privateData->data, privateData->len);
        cred->privateData.len = privateData->len;

        // TODO: Added as workaround. Will be replaced soon.
        cred->privateData.encoding = OIC_ENCODING_RAW;

#if 0
        // NOTE: Test codes to use base64 for credential.
        uint32_t outSize = 0;
        size_t b64BufSize = B64ENCODE_OUT_SAFESIZE((privateData->len + 1));
        char* b64Buf = (uint8_t *)OICCalloc(1, b64BufSize);
        VERIFY_NON_NULL(TAG, b64Buf, ERROR);
        b64Encode(privateData->data, privateData->len, b64Buf, b64BufSize, &outSize);

        OICFree( cred->privateData.data );
        cred->privateData.data = (uint8_t *)OICCalloc(1, outSize + 1);
        VERIFY_NON_NULL(TAG, cred->privateData.data, ERROR);

        strcpy(cred->privateData.data, b64Buf);
        cred->privateData.encoding = OIC_ENCODING_BASE64;
        cred->privateData.len = outSize;
        OICFree(b64Buf);
#endif //End of Test codes

    }

    VERIFY_NON_NULL(TAG, rownerID, ERROR);
    memcpy(&cred->rownerID, rownerID, sizeof(OicUuid_t));

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
    if (cred)
    {
        uint8_t *payload = NULL;
        size_t size = 0;
        int secureFlag = 0;
        OCStackResult res = CredToCBORPayload(cred, &payload, &size, secureFlag);
        if ((OC_STACK_OK == res) && payload)
        {
            if (OC_STACK_OK == UpdateSecureResourceInPS(OIC_JSON_CRED_NAME, payload, size))
            {
                ret = true;
            }
            OICFree(payload);
        }
    }
    else //Empty cred list
    {
        if (OC_STACK_OK == UpdateSecureResourceInPS(OIC_JSON_CRED_NAME, NULL, 0))
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

/**
 * Remove all credential data on credential resource and persistent storage
 *
 * @retval
 *     OC_STACK_OK              - no errors
 *     OC_STACK_ERROR           - stack process error
 */
OCStackResult RemoveAllCredentials(void)
{
    DeleteCredList(gCred);
    gCred = GetCredDefault();

    if (!UpdatePersistentStorage(gCred))
    {
        return OC_STACK_ERROR;
    }
    return OC_STACK_OK;
}

#if defined(__WITH_DTLS__) || defined(__WITH_TLS__)
/**
 * Internal function to fill private data of owner PSK.
 *
 * @param receviedCred recevied owner credential from OBT(PT)
 * @param ownerAdd address of OBT(PT)
 * @param doxm current device's doxm resource
 *
 * @return
 *     true successfully done and valid ower psk information
 *     false Invalid owner psk information or failed to owner psk generation
 */
static bool FillPrivateDataOfOwnerPSK(OicSecCred_t* receviedCred, const CAEndpoint_t* ownerAddr,
                           const OicSecDoxm_t* doxm)
{
    //Derive OwnerPSK locally
    const char* oxmLabel = GetOxmString(doxm->oxmSel);
    VERIFY_NON_NULL(TAG, oxmLabel, ERROR);

    uint8_t ownerPSK[OWNER_PSK_LENGTH_128] = {0};
    CAResult_t pskRet = CAGenerateOwnerPSK(ownerAddr,
        (uint8_t*)oxmLabel, strlen(oxmLabel),
        doxm->owner.id, sizeof(doxm->owner.id),
        doxm->deviceID.id, sizeof(doxm->deviceID.id),
        ownerPSK, OWNER_PSK_LENGTH_128);
    VERIFY_SUCCESS(TAG, pskRet == CA_STATUS_OK, ERROR);

    OIC_LOG(DEBUG, TAG, "OwnerPSK dump :");
    OIC_LOG_BUFFER(DEBUG, TAG, ownerPSK, OWNER_PSK_LENGTH_128);

    //Generate owner credential based on recevied credential information

    // TODO: Added as workaround, will be replaced soon.
    if(OIC_ENCODING_RAW == receviedCred->privateData.encoding)
    {
        receviedCred->privateData.data = (uint8_t *)OICCalloc(1, OWNER_PSK_LENGTH_128);
        VERIFY_NON_NULL(TAG, receviedCred->privateData.data, ERROR);
        receviedCred->privateData.len = OWNER_PSK_LENGTH_128;
        memcpy(receviedCred->privateData.data, ownerPSK, OWNER_PSK_LENGTH_128);
    }
    else if(OIC_ENCODING_BASE64 == receviedCred->privateData.encoding)
    {
        uint32_t b64OutSize = 0;
        size_t b64BufSize = B64ENCODE_OUT_SAFESIZE((OWNER_PSK_LENGTH_128 + 1));
        char* b64Buf = OICCalloc(1, b64BufSize);
        VERIFY_NON_NULL(TAG, b64Buf, ERROR);

        b64Encode(ownerPSK, OWNER_PSK_LENGTH_128, b64Buf, b64BufSize, &b64OutSize);

        receviedCred->privateData.data = (uint8_t *)OICCalloc(1, b64OutSize + 1);
        VERIFY_NON_NULL(TAG, receviedCred->privateData.data, ERROR);
        receviedCred->privateData.len = b64OutSize;
        strncpy((char*)receviedCred->privateData.data, b64Buf, b64OutSize);
        receviedCred->privateData.data[b64OutSize] = '\0';
    }
    else
    {
        // TODO: error
        VERIFY_SUCCESS(TAG, OIC_ENCODING_UNKNOW, ERROR);
    }

    OIC_LOG(INFO, TAG, "PrivateData of OwnerPSK was calculated successfully");

    //Verify OwnerPSK information
    return (memcmp(&(receviedCred->subject), &(doxm->owner), sizeof(OicUuid_t)) == 0 &&
            receviedCred->credType == SYMMETRIC_PAIR_WISE_KEY);
exit:
    //receviedCred->privateData.data will be deallocated when deleting credential.
    return false;
}

#endif //__WITH_DTLS__

static OCEntityHandlerResult HandlePostRequest(const OCEntityHandlerRequest * ehRequest)
{
    OCEntityHandlerResult ret = OC_EH_ERROR;
    OIC_LOG(DEBUG, TAG, "HandleCREDPostRequest IN");

    static uint16_t previousMsgId = 0;
    //Get binary representation of cbor
    OicSecCred_t *cred  = NULL;
    uint8_t *payload = (((OCSecurityPayload*)ehRequest->payload)->securityData);
    size_t size = (((OCSecurityPayload*)ehRequest->payload)->payloadSize);

    OCStackResult res = CBORPayloadToCred(payload, size, &cred);
    if (res == OC_STACK_OK)
    {
#if defined(__WITH_DTLS__) || defined(__WITH_TLS__)
        OicUuid_t emptyUuid = {.id={0}};
        const OicSecDoxm_t* doxm = GetDoxmResourceData();
        if(doxm && false == doxm->owned && memcmp(&(doxm->owner), &emptyUuid, sizeof(OicUuid_t)) != 0)
        {
            //in case of owner PSK
            switch(cred->credType)
            {
                case SYMMETRIC_PAIR_WISE_KEY:
                {
                    OCServerRequest *request = (OCServerRequest *)ehRequest->requestHandle;
                    if(FillPrivateDataOfOwnerPSK(cred, (CAEndpoint_t *)&request->devAddr, doxm))
                    {
                        if(OC_STACK_RESOURCE_DELETED == RemoveCredential(&cred->subject))
                        {
                            OIC_LOG(WARNING, TAG, "The credential with the same subject ID was detected!");
                        }

                        OIC_LOG(ERROR, TAG, "OwnerPSK was generated successfully.");
                        if(OC_STACK_OK == AddCredential(cred))
                        {
                            ret = OC_EH_CHANGED;
                        }
                        else
                        {
                            OIC_LOG(ERROR, TAG, "Failed to save the OwnerPSK as cred resource");
                            ret = OC_EH_ERROR;
                        }
                    }
                    else
                    {
                        OIC_LOG(ERROR, TAG, "Failed to verify receviced OwnerPKS.");
                        ret = OC_EH_ERROR;
                    }

                    if(OC_EH_CHANGED == ret)
                    {
                        /**
                         * in case of random PIN based OxM,
                         * revert get_psk_info callback of tinyDTLS to use owner credential.
                         */
                        if(OIC_RANDOM_DEVICE_PIN == doxm->oxmSel)
                        {
                            OicUuid_t emptyUuid = { .id={0}};
                            SetUuidForRandomPinOxm(&emptyUuid);

#ifdef __WITH_TLS__
                            if(CA_STATUS_OK != CAregisterTlsCredentialsHandler(GetDtlsPskCredentials))
                            {
                                OIC_LOG(ERROR, TAG, "Failed to revert TLS credential handler.");
                                ret = OC_EH_ERROR;
                                break;
                            }
#endif
                            if(CA_STATUS_OK != CARegisterDTLSCredentialsHandler(GetDtlsPskCredentials))
                            {
                                OIC_LOG(ERROR, TAG, "Failed to revert DTLS credential handler.");
                                ret = OC_EH_ERROR;
                                break;
                            }
                        }

                        //Select cipher suite to use owner PSK
                        if(CA_STATUS_OK != CAEnableAnonECDHCipherSuite(false))
                        {
                            OIC_LOG(ERROR, TAG, "Failed to disable anonymous cipher suite");
                            ret = OC_EH_ERROR;
                        }
                        else
                        {
                            OIC_LOG(INFO, TAG, "Anonymous cipher suite is DISABLED");
                        }

                        if(CA_STATUS_OK !=
                           CASelectCipherSuite(TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA_256, ehRequest->devAddr.adapter))
                        {
                            OIC_LOG(ERROR, TAG, "Failed to select cipher suite");
                            ret = OC_EH_ERROR;
                        }
                    }

                    break;
                }
                case SYMMETRIC_GROUP_KEY:
                case ASYMMETRIC_KEY:
                case SIGNED_ASYMMETRIC_KEY:
                case PIN_PASSWORD:
                case ASYMMETRIC_ENCRYPTION_KEY:
                {
                    OIC_LOG(WARNING, TAG, "Unsupported credential type for owner credential.");
                    ret = OC_EH_ERROR;
                    break;
                }
                default:
                {
                    OIC_LOG(WARNING, TAG, "Unknow credential type for owner credential.");
                    ret = OC_EH_ERROR;
                    break;
                }
            }

            if(OC_EH_CHANGED != ret)
            {
                /*
                  * If some error is occured while ownership transfer,
                  * ownership transfer related resource should be revert back to initial status.
                  */
                const OicSecDoxm_t* doxm =  GetDoxmResourceData();
                if(doxm)
                {
                    if(!doxm->owned && previousMsgId != ehRequest->messageID)
                    {
                        OIC_LOG(WARNING, TAG, "The operation failed during handle DOXM request,"\
                                            "DOXM will be reverted.");
                        RestoreDoxmToInitState();
                        RestorePstatToInitState();
                    }
                }
                else
                {
                    OIC_LOG(ERROR, TAG, "Invalid DOXM resource");
                }
            }
        }
        else
        {
            /*
             * If the post request credential has credId, it will be
             * discarded and the next available credId will be assigned
             * to it before getting appended to the existing credential
             * list and updating svr database.
             */
            ret = (OC_STACK_OK == AddCredential(cred))? OC_EH_CHANGED : OC_EH_ERROR;
        }
#else //not __WITH_DTLS__
        /*
         * If the post request credential has credId, it will be
         * discarded and the next available credId will be assigned
         * to it before getting appended to the existing credential
         * list and updating svr database.
         */
        ret = (OC_STACK_OK == AddCredential(cred))? OC_EH_CHANGED : OC_EH_ERROR;
        OC_UNUSED(previousMsgId);
#endif//__WITH_DTLS__
    }

    if (OC_EH_CHANGED != ret)
    {
        if(OC_STACK_OK != RemoveCredential(&cred->subject))
        {
            OIC_LOG(WARNING, TAG, "Failed to remove the invalid credential");
        }
        FreeCred(cred);
    }
    else
    {
        previousMsgId = ehRequest->messageID;
    }
    //Send response to request originator
    ret = ((SendSRMResponse(ehRequest, ret, NULL, 0)) == OC_STACK_OK) ?
                   OC_EH_OK : OC_EH_ERROR;

    OIC_LOG(DEBUG, TAG, "HandleCREDPostRequest OUT");
    return ret;
}

/**
 * The entity handler determines how to process a GET request.
 */
static OCEntityHandlerResult HandleGetRequest (const OCEntityHandlerRequest * ehRequest)
{
    OIC_LOG(INFO, TAG, "HandleGetRequest  processing GET request");

    // Convert Cred data into CBOR for transmission
    size_t size = 0;
    uint8_t *payload = NULL;
    int secureFlag = 1;

    const OicSecCred_t *cred = gCred;
    OCStackResult res = CredToCBORPayload(cred, &payload, &size, secureFlag);

    // A device should always have a default cred. Therefore, payload should never be NULL.
    OCEntityHandlerResult ehRet = (res == OC_STACK_OK) ? OC_EH_OK : OC_EH_ERROR;


    //Send payload to request originator
    ehRet = ((SendSRMResponse(ehRequest, ehRet, payload, size)) == OC_STACK_OK) ?
                       OC_EH_OK : OC_EH_ERROR;
    OICFree(payload);
    return ehRet;
}

static OCEntityHandlerResult HandleDeleteRequest(const OCEntityHandlerRequest *ehRequest)
{
    OIC_LOG(DEBUG, TAG, "Processing CredDeleteRequest");

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
        if (strncasecmp((char *)parseIter.attrPos, OIC_JSON_SUBJECTID_NAME,
                parseIter.attrLen) == 0)
        {
            OCStackResult ret = ConvertStrToUuid((const char*)parseIter.valPos, &subject);
            VERIFY_SUCCESS(TAG, OC_STACK_OK == ret, ERROR);
        }
    }

    if (OC_STACK_RESOURCE_DELETED == RemoveCredential(&subject))
    {
        ehRet = OC_EH_RESOURCE_DELETED;
    }
    //Send response to request originator
    ehRet = ((SendSRMResponse(ehRequest, ehRet, NULL, 0)) == OC_STACK_OK) ?
                   OC_EH_OK : OC_EH_ERROR;
exit:
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
        OIC_LOG (DEBUG, TAG, "Flag includes OC_REQUEST_FLAG");
        //TODO :  Remove Handle PUT methods once CTT have changed to POST on OTM
        switch (ehRequest->method)
        {
            case OC_REST_GET:
                ret = HandleGetRequest(ehRequest);;
                break;
            case OC_REST_PUT:
            case OC_REST_POST:
                ret = HandlePostRequest(ehRequest);
                break;
            case OC_REST_DELETE:
                ret = HandleDeleteRequest(ehRequest);
                break;
            default:
                ret = ((SendSRMResponse(ehRequest, ret, NULL, 0)) == OC_STACK_OK) ?
                               OC_EH_OK : OC_EH_ERROR;
                break;
        }
    }
    return ret;
}

OCStackResult CreateCredResource()
{
    OCStackResult ret = OCCreateResource(&gCredHandle,
                                         OIC_RSRC_TYPE_SEC_CRED,
                                         OC_RSRVD_INTERFACE_DEFAULT,
                                         OIC_RSRC_CRED_URI,
                                         CredEntityHandler,
                                         NULL,
                                         OC_SECURE);

    if (OC_STACK_OK != ret)
    {
        OIC_LOG (FATAL, TAG, "Unable to instantiate Cred resource");
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
        OIC_LOG (DEBUG, TAG, "ReadSVDataFromPS failed");
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

OicSecCred_t* GetCredResourceData(const OicUuid_t* subject)
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

OicSecCred_t* GetCredResourceDataByCredId(const uint16_t credId)
{
    OicSecCred_t *cred = NULL;

   if ( 1 > credId)
    {
       return NULL;
    }

    LL_FOREACH(gCred, cred)
    {
        if(cred->credId == credId)
        {
            return cred;
        }
    }
    return NULL;
}

#if defined(__WITH_DTLS__) || defined(__WITH_TLS__)
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
                OicUuid_t deviceID = {.id={0}};
                // Retrieve Device ID from doxm resource
                if ( OC_STACK_OK != GetDoxmDeviceID(&deviceID) )
                {
                    OIC_LOG (ERROR, TAG, "Unable to retrieve doxm Device ID");
                    return ret;
                }

                if (result_length < sizeof(deviceID.id))
                {
                    OIC_LOG (ERROR, TAG, "Wrong value for result_length");
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
                                OIC_LOG (INFO, TAG, "Credentials are expired.");
                                return ret;
                            }
                        }

                        // Copy PSK.
                        // TODO: Added as workaround. Will be replaced soon.
                        if(OIC_ENCODING_RAW == cred->privateData.encoding)
                        {
                            ret = cred->privateData.len;
                            memcpy(result, cred->privateData.data, ret);
                        }
                        else if(OIC_ENCODING_BASE64 == cred->privateData.encoding)
                        {
                            size_t outBufSize = B64DECODE_OUT_SAFESIZE((cred->privateData.len + 1));
                            uint8_t* outKey = OICCalloc(1, outBufSize);
                            uint32_t outKeySize;
                            if(NULL == outKey)
                            {
                                OIC_LOG (ERROR, TAG, "Failed to memoray allocation.");
                                return ret;
                            }

                            if(B64_OK == b64Decode((char*)cred->privateData.data, cred->privateData.len, outKey, outBufSize, &outKeySize))
                            {
                                memcpy(result, outKey, outKeySize);
                                ret = outKeySize;
                            }
                            else
                            {
                                OIC_LOG (ERROR, TAG, "Failed to base64 decoding.");
                            }

                            OICFree(outKey);
                        }

                        return ret;
                    }
                }
            }
            break;

        default:
            {
                OIC_LOG (ERROR, TAG, "Wrong value passed for CADtlsPskCredType_t.");
            }
            break;
    }

    return ret;
}

/**
 * Add temporal PSK to PIN based OxM
 *
 * @param[in] tmpSubject UUID of target device
 * @param[in] credType Type of credential to be added
 * @param[in] pin numeric characters
 * @param[in] pinSize length of 'pin'
 * @param[in] rownerID Resource owner's UUID
 * @param[out] tmpCredSubject Generated credential's subject.
 *
 * @return OC_STACK_OK for success and errorcode otherwise.
 */
OCStackResult AddTmpPskWithPIN(const OicUuid_t* tmpSubject, OicSecCredType_t credType,
                            const char * pin, size_t pinSize,
                            const OicUuid_t * rownerID, OicUuid_t* tmpCredSubject)
{
    OCStackResult ret = OC_STACK_ERROR;
    OIC_LOG(DEBUG, TAG, "AddTmpPskWithPIN IN");

    if(NULL == tmpSubject || NULL == pin || 0 == pinSize || NULL == tmpCredSubject)
    {
        return OC_STACK_INVALID_PARAM;
    }

    uint8_t privData[OWNER_PSK_LENGTH_128] = {0,};
    OicSecKey_t privKey = {privData, OWNER_PSK_LENGTH_128, OIC_ENCODING_RAW};
    OicSecCred_t* cred = NULL;
    int dtlsRes = DeriveCryptoKeyFromPassword((const unsigned char *)pin, pinSize, rownerID->id,
                                              UUID_LENGTH, PBKDF_ITERATIONS,
                                              OWNER_PSK_LENGTH_128, privData);
    VERIFY_SUCCESS(TAG, (0 == dtlsRes) , ERROR);

    cred = GenerateCredential(tmpSubject, credType, NULL,
                              &privKey, rownerID);
    if(NULL == cred)
    {
        OIC_LOG(ERROR, TAG, "GeneratePskWithPIN() : Failed to generate credential");
        return OC_STACK_ERROR;
    }

    memcpy(tmpCredSubject->id, cred->subject.id, UUID_LENGTH);

    ret = AddCredential(cred);
    if( OC_STACK_OK != ret)
    {
        RemoveCredential(tmpSubject);
        OIC_LOG(ERROR, TAG, "GeneratePskWithPIN() : Failed to add credential");
    }
    OIC_LOG(DEBUG, TAG, "AddTmpPskWithPIN OUT");

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

static uint32_t parseCertPrefix(uint8_t *prefix)
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

static OCStackResult GetCAPublicKeyData(CADtlsX509Creds_t *credInfo)
{
    OCStackResult ret = OC_STACK_ERROR;
    uint8_t *ccPtr = credInfo->certificateChain;
    for (uint8_t i = 0; i < credInfo->chainLen - 1; ++i)
    {
        ccPtr += CERT_LEN_PREFIX + parseCertPrefix(ccPtr);
    }

    ByteArray cert = { .data = ccPtr + CERT_LEN_PREFIX, .len = parseCertPrefix(ccPtr) };
    CertificateX509 certStruct;

    VERIFY_SUCCESS(TAG, PKI_SUCCESS == DecodeCertificate(cert, &certStruct), ERROR);

    INC_BYTE_ARRAY(certStruct.pubKey, 2);

    memcpy(credInfo->rootPublicKeyX, certStruct.pubKey.data, PUBLIC_KEY_SIZE / 2);
    memcpy(credInfo->rootPublicKeyY, certStruct.pubKey.data + PUBLIC_KEY_SIZE / 2, PUBLIC_KEY_SIZE / 2);

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

    if (cred->publicData.len > MAX_CERT_MESSAGE_LEN || cred->privateData.len > PRIVATE_KEY_SIZE)
    {
        goto exit;
    }
    credInfo->chainLen = 2;
    memcpy(credInfo->certificateChain, cred->publicData.data, cred->publicData.len);
    memcpy(credInfo->devicePrivateKey, cred->privateData.data, cred->privateData.len);
    credInfo->certificateChainLen = cred->publicData.len;
    GetCAPublicKeyData(credInfo);
    ret = 0;

exit:

    return ret;
}
#undef CERT_LEN_PREFIX
#endif /* __WITH_X509__ */

OCStackResult SetCredRownerId(const OicUuid_t* newROwner)
{
    OCStackResult ret = OC_STACK_ERROR;
    uint8_t *cborPayload = NULL;
    size_t size = 0;
    int secureFlag = 0;
    OicUuid_t prevId = {.id={0}};

    if(NULL == newROwner)
    {
        ret = OC_STACK_INVALID_PARAM;
    }
    if(NULL == gCred)
    {
        ret = OC_STACK_NO_RESOURCE;
    }

    if(newROwner && gCred)
    {
        memcpy(prevId.id, gCred->rownerID.id, sizeof(prevId.id));
        memcpy(gCred->rownerID.id, newROwner->id, sizeof(newROwner->id));

        ret = CredToCBORPayload(gCred, &cborPayload, &size, secureFlag);
        VERIFY_SUCCESS(TAG, OC_STACK_OK == ret, ERROR);

        ret = UpdateSecureResourceInPS(OIC_JSON_CRED_NAME, cborPayload, size);
        VERIFY_SUCCESS(TAG, OC_STACK_OK == ret, ERROR);

        OICFree(cborPayload);
    }

    return ret;

exit:
    OICFree(cborPayload);
    memcpy(gCred->rownerID.id, prevId.id, sizeof(prevId.id));
    return ret;
}

OCStackResult GetCredRownerId(OicUuid_t *rowneruuid)
{
    OCStackResult retVal = OC_STACK_ERROR;
    if (gCred)
    {
        *rowneruuid = gCred->rownerID;
        retVal = OC_STACK_OK;
    }
    return retVal;
}

#ifdef __WITH_TLS__
OCStackResult AddCA(OicSecCert_t * cert)
{
    OIC_LOG_V(DEBUG, TAG, "In %s", __func__);
    OCStackResult ret = OC_STACK_ERROR;
    if (NULL == cert || NULL == cert->data)
    {
        return OC_STACK_INVALID_PARAM;
    }
    OicUuid_t  subject;
    OicUuid_t rowner;
    ret = GetCredRownerId(&rowner);
    VERIFY_SUCCESS(TAG, OC_STACK_OK == ret, ERROR);
    ret = ConvertStrToUuid(CA_SUBJECT_ID, &subject);
    VERIFY_SUCCESS(TAG, OC_STACK_OK == ret, ERROR);

    OicSecCred_t * cred  = GetCredResourceData(&subject);
    if (NULL == cred)
    {
        //Generating new credential for CA
        cred = GenerateCredential(&subject, SIGNED_ASYMMETRIC_KEY, cert, NULL, &rowner);
        VERIFY_NON_NULL(TAG, cred, ERROR);
        return AddCredential(cred);
    }

    uint8_t * tempData = OICRealloc(cred->publicData.data, cred->publicData.len + cert->len);
    if (NULL == tempData)
    {
        return OC_STACK_NO_MEMORY;
    }

    memcpy(tempData + cred->publicData.len, cert->data, cert->len);
    cred->publicData.data = tempData;
    cred->publicData.len += cert->len;

    if (UpdatePersistentStorage(gCred))
    {
        ret = OC_STACK_OK;
    }

exit:
    OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
    return ret;
}
// TODO for testing only
// will be removed
const unsigned char ca_cert[] = {
// AMAZON
0x30, 0x82, 0x02, 0xFE, 0x30, 0x82, 0x01, 0xE6, 0x02, 0x01, 0x01, 0x30, 0x0D, 0x06, 0x09, 0x2A,
0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x0B, 0x05, 0x00, 0x30, 0x45, 0x31, 0x0B, 0x30, 0x09,
0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x41, 0x55, 0x31, 0x13, 0x30, 0x11, 0x06, 0x03, 0x55,
0x04, 0x08, 0x0C, 0x0A, 0x53, 0x6F, 0x6D, 0x65, 0x2D, 0x53, 0x74, 0x61, 0x74, 0x65, 0x31, 0x21,
0x30, 0x1F, 0x06, 0x03, 0x55, 0x04, 0x0A, 0x0C, 0x18, 0x49, 0x6E, 0x74, 0x65, 0x72, 0x6E, 0x65,
0x74, 0x20, 0x57, 0x69, 0x64, 0x67, 0x69, 0x74, 0x73, 0x20, 0x50, 0x74, 0x79, 0x20, 0x4C, 0x74,
0x64, 0x30, 0x1E, 0x17, 0x0D, 0x31, 0x36, 0x30, 0x36, 0x32, 0x38, 0x30, 0x35, 0x33, 0x33, 0x35,
0x38, 0x5A, 0x17, 0x0D, 0x32, 0x36, 0x30, 0x36, 0x32, 0x36, 0x30, 0x35, 0x33, 0x33, 0x35, 0x38,
0x5A, 0x30, 0x45, 0x31, 0x0B, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x41, 0x55,
0x31, 0x13, 0x30, 0x11, 0x06, 0x03, 0x55, 0x04, 0x08, 0x0C, 0x0A, 0x53, 0x6F, 0x6D, 0x65, 0x2D,
0x53, 0x74, 0x61, 0x74, 0x65, 0x31, 0x21, 0x30, 0x1F, 0x06, 0x03, 0x55, 0x04, 0x0A, 0x0C, 0x18,
0x49, 0x6E, 0x74, 0x65, 0x72, 0x6E, 0x65, 0x74, 0x20, 0x57, 0x69, 0x64, 0x67, 0x69, 0x74, 0x73,
0x20, 0x50, 0x74, 0x79, 0x20, 0x4C, 0x74, 0x64, 0x30, 0x82, 0x01, 0x22, 0x30, 0x0D, 0x06, 0x09,
0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x82, 0x01, 0x0F, 0x00,
0x30, 0x82, 0x01, 0x0A, 0x02, 0x82, 0x01, 0x01, 0x00, 0xB5, 0x1C, 0x79, 0x1F, 0x30, 0x6F, 0x5C,
0x04, 0x9E, 0xD9, 0x78, 0x22, 0x01, 0x29, 0xEE, 0xE9, 0x4A, 0x3A, 0x69, 0xB6, 0xF8, 0xBE, 0x87,
0x3A, 0x5D, 0x57, 0xBD, 0x96, 0xEE, 0xCC, 0xE9, 0x58, 0x39, 0x53, 0x41, 0xD5, 0x1A, 0x47, 0x12,
0x8B, 0xF5, 0xAA, 0x0D, 0xC5, 0xBC, 0xAE, 0x17, 0x93, 0x8B, 0xF3, 0x10, 0x10, 0xF6, 0xCD, 0xBC,
0x22, 0xA2, 0x31, 0x74, 0xF5, 0xBB, 0x5F, 0x72, 0x40, 0x9A, 0x42, 0xE2, 0x83, 0x33, 0x02, 0xA3,
0x6C, 0x6D, 0x5C, 0x7C, 0xFF, 0x3B, 0x2B, 0xE8, 0x99, 0x4E, 0x9F, 0x86, 0x26, 0xE7, 0x79, 0x0C,
0x39, 0x0D, 0xEF, 0xA2, 0x52, 0x80, 0xFA, 0xF3, 0x37, 0x46, 0xFB, 0xF9, 0x35, 0x35, 0xC0, 0x16,
0xC7, 0x1F, 0x95, 0x10, 0x03, 0x30, 0xE1, 0xFB, 0xD9, 0xBD, 0x84, 0x95, 0x11, 0x0D, 0x5A, 0x28,
0x59, 0xE3, 0xB6, 0xB4, 0x3C, 0xA3, 0xA6, 0x5F, 0x7B, 0x9F, 0x8C, 0x45, 0x3B, 0xC1, 0xAB, 0xE1,
0xB4, 0xFA, 0x3C, 0x19, 0x58, 0x91, 0x28, 0x29, 0xBA, 0x71, 0x20, 0xFA, 0x4D, 0x58, 0xF1, 0xB1,
0x01, 0x5D, 0x7B, 0xB4, 0x4A, 0x5E, 0xFA, 0x58, 0x36, 0xEE, 0x31, 0x18, 0x0D, 0x81, 0xB5, 0x41,
0x48, 0x8B, 0xD9, 0x31, 0xBC, 0xD1, 0x98, 0xD0, 0x40, 0xBF, 0x79, 0x3A, 0x31, 0x13, 0x61, 0xFF,
0x04, 0x23, 0x1A, 0x2A, 0xAC, 0xA1, 0xEF, 0x1C, 0x2B, 0xC3, 0x8A, 0x7D, 0x33, 0x75, 0xDF, 0x84,
0xA8, 0xF3, 0x74, 0x63, 0xE4, 0x61, 0x92, 0x5D, 0xCF, 0x62, 0x8C, 0x56, 0x9B, 0xB7, 0x7C, 0xCF,
0x4A, 0x75, 0x98, 0x3E, 0xE5, 0x73, 0x03, 0x8C, 0xBF, 0x2D, 0x4A, 0x80, 0x48, 0x2D, 0x27, 0xB4,
0x87, 0xCB, 0x40, 0xF6, 0x48, 0x85, 0xBF, 0x29, 0xA0, 0xE8, 0xC7, 0xF9, 0x11, 0x3C, 0x2D, 0x4F,
0x92, 0xC1, 0x5E, 0x01, 0x2D, 0x81, 0xD1, 0x9F, 0x73, 0x02, 0x03, 0x01, 0x00, 0x01, 0x30, 0x0D,
0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x0B, 0x05, 0x00, 0x03, 0x82, 0x01,
0x01, 0x00, 0x0B, 0x47, 0x8E, 0x29, 0x8C, 0xCB, 0x05, 0xE7, 0xF3, 0xDD, 0xC8, 0x7E, 0xED, 0x6F,
0xB4, 0xD9, 0xCB, 0xF4, 0x84, 0xCC, 0xA5, 0x08, 0x6C, 0x1C, 0x09, 0x60, 0xD5, 0x00, 0x55, 0x13,
0x28, 0xC6, 0x64, 0xB2, 0x23, 0x52, 0x03, 0x21, 0xB1, 0x69, 0x63, 0x57, 0x04, 0xC2, 0xD4, 0xF7,
0x41, 0xAC, 0xEE, 0xD5, 0xD0, 0x49, 0x58, 0x6D, 0xE5, 0x7E, 0x2C, 0xA6, 0x06, 0xC0, 0x39, 0x3A,
0x7E, 0x30, 0x49, 0xA2, 0x00, 0x8B, 0x81, 0x98, 0x94, 0xC3, 0x5F, 0x05, 0xF1, 0x38, 0xC7, 0x0B,
0xFA, 0x83, 0xB2, 0x85, 0x84, 0xB2, 0x6D, 0x62, 0x07, 0x82, 0x9C, 0x4D, 0x99, 0x24, 0xD2, 0x79,
0xBF, 0xDA, 0xF8, 0x9D, 0x1C, 0xAD, 0x13, 0x30, 0xBC, 0xC2, 0xA2, 0x14, 0x7B, 0xD7, 0xFB, 0xCD,
0x29, 0x2D, 0xAB, 0xB6, 0x24, 0x03, 0x60, 0x62, 0x9E, 0xF4, 0x4C, 0xE3, 0x35, 0x23, 0xB7, 0x1A,
0x50, 0x96, 0x91, 0x54, 0xD8, 0xB4, 0x93, 0x61, 0x00, 0xB6, 0xBF, 0x05, 0xF0, 0xF3, 0x6B, 0x99,
0x1E, 0x46, 0x4C, 0x26, 0x95, 0xD2, 0x58, 0x86, 0x5C, 0x78, 0xAD, 0x01, 0xF9, 0xC9, 0x54, 0x67,
0xB7, 0x99, 0x3C, 0xEE, 0xF8, 0xD7, 0xD2, 0x1E, 0xE6, 0xF0, 0xCC, 0xC8, 0xC2, 0x20, 0x1B, 0xDA,
0xCF, 0xDB, 0xF5, 0x70, 0x65, 0x33, 0x51, 0x6E, 0x17, 0x1D, 0xC5, 0xC5, 0xC5, 0x63, 0x06, 0x5E,
0xCA, 0xB5, 0x40, 0x14, 0xEE, 0xDC, 0x14, 0xF2, 0xFE, 0x4B, 0x7A, 0x78, 0xD6, 0x0D, 0x21, 0xF5,
0x0F, 0x58, 0xE7, 0x2A, 0x26, 0x54, 0x52, 0xCA, 0x60, 0xA2, 0xF8, 0x68, 0xB0, 0xF4, 0x6A, 0x9B,
0x11, 0x8D, 0x5E, 0x57, 0x5A, 0x5F, 0x51, 0x3E, 0x44, 0x99, 0xB1, 0x76, 0xDA, 0x94, 0x56, 0x0E,
0x1C, 0xA3, 0xFC, 0xE3, 0x01, 0xB4, 0xD8, 0xEC, 0x2F, 0xBB, 0x65, 0x82, 0x19, 0x06, 0x8E, 0x3A,
0x2E, 0x5D,
//Google

0x30, 0x82, 0x03, 0x54, 0x30, 0x82, 0x02, 0x3c, 0xa0, 0x03, 0x02, 0x01, 0x02, 0x02, 0x03, 0x02,
0x34, 0x56, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x05, 0x05,
0x00, 0x30, 0x42, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x55, 0x53,
0x31, 0x16, 0x30, 0x14, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x13, 0x0d, 0x47, 0x65, 0x6f, 0x54, 0x72,
0x75, 0x73, 0x74, 0x20, 0x49, 0x6e, 0x63, 0x2e, 0x31, 0x1b, 0x30, 0x19, 0x06, 0x03, 0x55, 0x04,
0x03, 0x13, 0x12, 0x47, 0x65, 0x6f, 0x54, 0x72, 0x75, 0x73, 0x74, 0x20, 0x47, 0x6c, 0x6f, 0x62,
0x61, 0x6c, 0x20, 0x43, 0x41, 0x30, 0x1e, 0x17, 0x0d, 0x30, 0x32, 0x30, 0x35, 0x32, 0x31, 0x30,
0x34, 0x30, 0x30, 0x30, 0x30, 0x5a, 0x17, 0x0d, 0x32, 0x32, 0x30, 0x35, 0x32, 0x31, 0x30, 0x34,
0x30, 0x30, 0x30, 0x30, 0x5a, 0x30, 0x42, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06,
0x13, 0x02, 0x55, 0x53, 0x31, 0x16, 0x30, 0x14, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x13, 0x0d, 0x47,
0x65, 0x6f, 0x54, 0x72, 0x75, 0x73, 0x74, 0x20, 0x49, 0x6e, 0x63, 0x2e, 0x31, 0x1b, 0x30, 0x19,
0x06, 0x03, 0x55, 0x04, 0x03, 0x13, 0x12, 0x47, 0x65, 0x6f, 0x54, 0x72, 0x75, 0x73, 0x74, 0x20,
0x47, 0x6c, 0x6f, 0x62, 0x61, 0x6c, 0x20, 0x43, 0x41, 0x30, 0x82, 0x01, 0x22, 0x30, 0x0d, 0x06,
0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x82, 0x01, 0x0f,
0x00, 0x30, 0x82, 0x01, 0x0a, 0x02, 0x82, 0x01, 0x01, 0x00, 0xda, 0xcc, 0x18, 0x63, 0x30, 0xfd,
0xf4, 0x17, 0x23, 0x1a, 0x56, 0x7e, 0x5b, 0xdf, 0x3c, 0x6c, 0x38, 0xe4, 0x71, 0xb7, 0x78, 0x91,
0xd4, 0xbc, 0xa1, 0xd8, 0x4c, 0xf8, 0xa8, 0x43, 0xb6, 0x03, 0xe9, 0x4d, 0x21, 0x07, 0x08, 0x88,
0xda, 0x58, 0x2f, 0x66, 0x39, 0x29, 0xbd, 0x05, 0x78, 0x8b, 0x9d, 0x38, 0xe8, 0x05, 0xb7, 0x6a,
0x7e, 0x71, 0xa4, 0xe6, 0xc4, 0x60, 0xa6, 0xb0, 0xef, 0x80, 0xe4, 0x89, 0x28, 0x0f, 0x9e, 0x25,
0xd6, 0xed, 0x83, 0xf3, 0xad, 0xa6, 0x91, 0xc7, 0x98, 0xc9, 0x42, 0x18, 0x35, 0x14, 0x9d, 0xad,
0x98, 0x46, 0x92, 0x2e, 0x4f, 0xca, 0xf1, 0x87, 0x43, 0xc1, 0x16, 0x95, 0x57, 0x2d, 0x50, 0xef,
0x89, 0x2d, 0x80, 0x7a, 0x57, 0xad, 0xf2, 0xee, 0x5f, 0x6b, 0xd2, 0x00, 0x8d, 0xb9, 0x14, 0xf8,
0x14, 0x15, 0x35, 0xd9, 0xc0, 0x46, 0xa3, 0x7b, 0x72, 0xc8, 0x91, 0xbf, 0xc9, 0x55, 0x2b, 0xcd,
0xd0, 0x97, 0x3e, 0x9c, 0x26, 0x64, 0xcc, 0xdf, 0xce, 0x83, 0x19, 0x71, 0xca, 0x4e, 0xe6, 0xd4,
0xd5, 0x7b, 0xa9, 0x19, 0xcd, 0x55, 0xde, 0xc8, 0xec, 0xd2, 0x5e, 0x38, 0x53, 0xe5, 0x5c, 0x4f,
0x8c, 0x2d, 0xfe, 0x50, 0x23, 0x36, 0xfc, 0x66, 0xe6, 0xcb, 0x8e, 0xa4, 0x39, 0x19, 0x00, 0xb7,
0x95, 0x02, 0x39, 0x91, 0x0b, 0x0e, 0xfe, 0x38, 0x2e, 0xd1, 0x1d, 0x05, 0x9a, 0xf6, 0x4d, 0x3e,
0x6f, 0x0f, 0x07, 0x1d, 0xaf, 0x2c, 0x1e, 0x8f, 0x60, 0x39, 0xe2, 0xfa, 0x36, 0x53, 0x13, 0x39,
0xd4, 0x5e, 0x26, 0x2b, 0xdb, 0x3d, 0xa8, 0x14, 0xbd, 0x32, 0xeb, 0x18, 0x03, 0x28, 0x52, 0x04,
0x71, 0xe5, 0xab, 0x33, 0x3d, 0xe1, 0x38, 0xbb, 0x07, 0x36, 0x84, 0x62, 0x9c, 0x79, 0xea, 0x16,
0x30, 0xf4, 0x5f, 0xc0, 0x2b, 0xe8, 0x71, 0x6b, 0xe4, 0xf9, 0x02, 0x03, 0x01, 0x00, 0x01, 0xa3,
0x53, 0x30, 0x51, 0x30, 0x0f, 0x06, 0x03, 0x55, 0x1d, 0x13, 0x01, 0x01, 0xff, 0x04, 0x05, 0x30,
0x03, 0x01, 0x01, 0xff, 0x30, 0x1d, 0x06, 0x03, 0x55, 0x1d, 0x0e, 0x04, 0x16, 0x04, 0x14, 0xc0,
0x7a, 0x98, 0x68, 0x8d, 0x89, 0xfb, 0xab, 0x05, 0x64, 0x0c, 0x11, 0x7d, 0xaa, 0x7d, 0x65, 0xb8,
0xca, 0xcc, 0x4e, 0x30, 0x1f, 0x06, 0x03, 0x55, 0x1d, 0x23, 0x04, 0x18, 0x30, 0x16, 0x80, 0x14,
0xc0, 0x7a, 0x98, 0x68, 0x8d, 0x89, 0xfb, 0xab, 0x05, 0x64, 0x0c, 0x11, 0x7d, 0xaa, 0x7d, 0x65,
0xb8, 0xca, 0xcc, 0x4e, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01,
0x05, 0x05, 0x00, 0x03, 0x82, 0x01, 0x01, 0x00, 0x35, 0xe3, 0x29, 0x6a, 0xe5, 0x2f, 0x5d, 0x54,
0x8e, 0x29, 0x50, 0x94, 0x9f, 0x99, 0x1a, 0x14, 0xe4, 0x8f, 0x78, 0x2a, 0x62, 0x94, 0xa2, 0x27,
0x67, 0x9e, 0xd0, 0xcf, 0x1a, 0x5e, 0x47, 0xe9, 0xc1, 0xb2, 0xa4, 0xcf, 0xdd, 0x41, 0x1a, 0x05,
0x4e, 0x9b, 0x4b, 0xee, 0x4a, 0x6f, 0x55, 0x52, 0xb3, 0x24, 0xa1, 0x37, 0x0a, 0xeb, 0x64, 0x76,
0x2a, 0x2e, 0x2c, 0xf3, 0xfd, 0x3b, 0x75, 0x90, 0xbf, 0xfa, 0x71, 0xd8, 0xc7, 0x3d, 0x37, 0xd2,
0xb5, 0x05, 0x95, 0x62, 0xb9, 0xa6, 0xde, 0x89, 0x3d, 0x36, 0x7b, 0x38, 0x77, 0x48, 0x97, 0xac,
0xa6, 0x20, 0x8f, 0x2e, 0xa6, 0xc9, 0x0c, 0xc2, 0xb2, 0x99, 0x45, 0x00, 0xc7, 0xce, 0x11, 0x51,
0x22, 0x22, 0xe0, 0xa5, 0xea, 0xb6, 0x15, 0x48, 0x09, 0x64, 0xea, 0x5e, 0x4f, 0x74, 0xf7, 0x05,
0x3e, 0xc7, 0x8a, 0x52, 0x0c, 0xdb, 0x15, 0xb4, 0xbd, 0x6d, 0x9b, 0xe5, 0xc6, 0xb1, 0x54, 0x68,
0xa9, 0xe3, 0x69, 0x90, 0xb6, 0x9a, 0xa5, 0x0f, 0xb8, 0xb9, 0x3f, 0x20, 0x7d, 0xae, 0x4a, 0xb5,
0xb8, 0x9c, 0xe4, 0x1d, 0xb6, 0xab, 0xe6, 0x94, 0xa5, 0xc1, 0xc7, 0x83, 0xad, 0xdb, 0xf5, 0x27,
0x87, 0x0e, 0x04, 0x6c, 0xd5, 0xff, 0xdd, 0xa0, 0x5d, 0xed, 0x87, 0x52, 0xb7, 0x2b, 0x15, 0x02,
0xae, 0x39, 0xa6, 0x6a, 0x74, 0xe9, 0xda, 0xc4, 0xe7, 0xbc, 0x4d, 0x34, 0x1e, 0xa9, 0x5c, 0x4d,
0x33, 0x5f, 0x92, 0x09, 0x2f, 0x88, 0x66, 0x5d, 0x77, 0x97, 0xc7, 0x1d, 0x76, 0x13, 0xa9, 0xd5,
0xe5, 0xf1, 0x16, 0x09, 0x11, 0x35, 0xd5, 0xac, 0xdb, 0x24, 0x71, 0x70, 0x2c, 0x98, 0x56, 0x0b,
0xd9, 0x17, 0xb4, 0xd1, 0xe3, 0x51, 0x2b, 0x5e, 0x75, 0xe8, 0xd5, 0xd0, 0xdc, 0x4f, 0x34, 0xed,
0xc2, 0x05, 0x66, 0x80, 0xa1, 0xcb, 0xe6, 0x33,
// Device
0x30, 0x82, 0x01, 0x55, 0x30, 0x81, 0xFB, 0xA0, 0x03, 0x02, 0x01, 0x02, 0x02, 0x01, 0x01, 0x30,
0x0A, 0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x04, 0x03, 0x02, 0x30, 0x34, 0x31, 0x32, 0x30,
0x30, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0C, 0x29, 0x75, 0x75, 0x69, 0x64, 0x3A, 0x33, 0x32, 0x33,
0x32, 0x33, 0x32, 0x33, 0x32, 0x2D, 0x33, 0x32, 0x33, 0x32, 0x2D, 0x33, 0x32, 0x33, 0x32, 0x2D,
0x33, 0x32, 0x33, 0x32, 0x2D, 0x33, 0x32, 0x33, 0x32, 0x33, 0x32, 0x33, 0x32, 0x33, 0x32, 0x33,
0x32, 0x30, 0x1E, 0x17, 0x0D, 0x31, 0x36, 0x30, 0x37, 0x32, 0x36, 0x31, 0x34, 0x31, 0x32, 0x32,
0x31, 0x5A, 0x17, 0x0D, 0x31, 0x37, 0x30, 0x31, 0x30, 0x31, 0x32, 0x32, 0x30, 0x30, 0x30, 0x30,
0x5A, 0x30, 0x34, 0x31, 0x32, 0x30, 0x30, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0C, 0x29, 0x75, 0x75,
0x69, 0x64, 0x3A, 0x33, 0x32, 0x33, 0x32, 0x33, 0x32, 0x33, 0x32, 0x2D, 0x33, 0x32, 0x33, 0x32,
0x2D, 0x33, 0x32, 0x33, 0x32, 0x2D, 0x33, 0x32, 0x33, 0x32, 0x2D, 0x33, 0x32, 0x33, 0x32, 0x33,
0x32, 0x33, 0x32, 0x33, 0x32, 0x33, 0x32, 0x30, 0x59, 0x30, 0x13, 0x06, 0x07, 0x2A, 0x86, 0x48,
0xCE, 0x3D, 0x02, 0x01, 0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x01, 0x07, 0x03, 0x42,
0x00, 0x04, 0x19, 0x1D, 0x6B, 0x4A, 0x12, 0xA7, 0x20, 0xF1, 0x95, 0xC6, 0x6D, 0x2A, 0xD7, 0x3B,
0xFA, 0x90, 0x8C, 0x52, 0xEB, 0x75, 0x67, 0xFF, 0x0A, 0x3F, 0xF2, 0xDF, 0x8D, 0x81, 0x44, 0xC7,
0xC8, 0x84, 0x60, 0xD4, 0x07, 0x57, 0xB1, 0x96, 0xAF, 0x5E, 0x00, 0xA5, 0xED, 0xA1, 0x48, 0x3F,
0x88, 0x43, 0x8D, 0x15, 0x81, 0x0A, 0x21, 0x9B, 0x6C, 0xD3, 0xBD, 0x85, 0x86, 0xE1, 0xA6, 0xDA,
0xC5, 0xCE, 0x30, 0x0A, 0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x04, 0x03, 0x02, 0x03, 0x49,
0x00, 0x30, 0x46, 0x02, 0x21, 0x00, 0xF7, 0x8E, 0x43, 0x19, 0xD2, 0x86, 0xF9, 0x21, 0x84, 0x66,
0x23, 0x4B, 0x18, 0x7C, 0x56, 0x18, 0x37, 0x48, 0xDF, 0x16, 0x3B, 0x70, 0x52, 0x26, 0x62, 0xB3,
0xAA, 0xD8, 0x3D, 0xE9, 0x43, 0xC3, 0x02, 0x21, 0x00, 0xD0, 0x16, 0xA4, 0x33, 0x7A, 0xE3, 0x7C,
0x62, 0x88, 0x88, 0x7B, 0x76, 0x99, 0xBF, 0x2D, 0xDF, 0x6C, 0xF5, 0xD0, 0x5F, 0xBE, 0x4B, 0xAE,
0xBA, 0xE5, 0xC0, 0x05, 0x26, 0xBC, 0x8B, 0x20, 0x84
};
const unsigned char own_cert[] = {
0x30, 0x82, 0x01, 0x55, 0x30, 0x81, 0xFB, 0xA0, 0x03, 0x02, 0x01, 0x02, 0x02, 0x01, 0x01, 0x30,
0x0A, 0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x04, 0x03, 0x02, 0x30, 0x34, 0x31, 0x32, 0x30,
0x30, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0C, 0x29, 0x75, 0x75, 0x69, 0x64, 0x3A, 0x33, 0x32, 0x33,
0x32, 0x33, 0x32, 0x33, 0x32, 0x2D, 0x33, 0x32, 0x33, 0x32, 0x2D, 0x33, 0x32, 0x33, 0x32, 0x2D,
0x33, 0x32, 0x33, 0x32, 0x2D, 0x33, 0x32, 0x33, 0x32, 0x33, 0x32, 0x33, 0x32, 0x33, 0x32, 0x33,
0x32, 0x30, 0x1E, 0x17, 0x0D, 0x31, 0x36, 0x30, 0x37, 0x32, 0x36, 0x31, 0x34, 0x31, 0x32, 0x32,
0x31, 0x5A, 0x17, 0x0D, 0x31, 0x37, 0x30, 0x31, 0x30, 0x31, 0x32, 0x32, 0x30, 0x30, 0x30, 0x30,
0x5A, 0x30, 0x34, 0x31, 0x32, 0x30, 0x30, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0C, 0x29, 0x75, 0x75,
0x69, 0x64, 0x3A, 0x33, 0x32, 0x33, 0x32, 0x33, 0x32, 0x33, 0x32, 0x2D, 0x33, 0x32, 0x33, 0x32,
0x2D, 0x33, 0x32, 0x33, 0x32, 0x2D, 0x33, 0x32, 0x33, 0x32, 0x2D, 0x33, 0x32, 0x33, 0x32, 0x33,
0x32, 0x33, 0x32, 0x33, 0x32, 0x33, 0x32, 0x30, 0x59, 0x30, 0x13, 0x06, 0x07, 0x2A, 0x86, 0x48,
0xCE, 0x3D, 0x02, 0x01, 0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x01, 0x07, 0x03, 0x42,
0x00, 0x04, 0x19, 0x1D, 0x6B, 0x4A, 0x12, 0xA7, 0x20, 0xF1, 0x95, 0xC6, 0x6D, 0x2A, 0xD7, 0x3B,
0xFA, 0x90, 0x8C, 0x52, 0xEB, 0x75, 0x67, 0xFF, 0x0A, 0x3F, 0xF2, 0xDF, 0x8D, 0x81, 0x44, 0xC7,
0xC8, 0x84, 0x60, 0xD4, 0x07, 0x57, 0xB1, 0x96, 0xAF, 0x5E, 0x00, 0xA5, 0xED, 0xA1, 0x48, 0x3F,
0x88, 0x43, 0x8D, 0x15, 0x81, 0x0A, 0x21, 0x9B, 0x6C, 0xD3, 0xBD, 0x85, 0x86, 0xE1, 0xA6, 0xDA,
0xC5, 0xCE, 0x30, 0x0A, 0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x04, 0x03, 0x02, 0x03, 0x49,
0x00, 0x30, 0x46, 0x02, 0x21, 0x00, 0xF7, 0x8E, 0x43, 0x19, 0xD2, 0x86, 0xF9, 0x21, 0x84, 0x66,
0x23, 0x4B, 0x18, 0x7C, 0x56, 0x18, 0x37, 0x48, 0xDF, 0x16, 0x3B, 0x70, 0x52, 0x26, 0x62, 0xB3,
0xAA, 0xD8, 0x3D, 0xE9, 0x43, 0xC3, 0x02, 0x21, 0x00, 0xD0, 0x16, 0xA4, 0x33, 0x7A, 0xE3, 0x7C,
0x62, 0x88, 0x88, 0x7B, 0x76, 0x99, 0xBF, 0x2D, 0xDF, 0x6C, 0xF5, 0xD0, 0x5F, 0xBE, 0x4B, 0xAE,
0xBA, 0xE5, 0xC0, 0x05, 0x26, 0xBC, 0x8B, 0x20, 0x84
};
const unsigned char ca_key[] = {
0x30, 0x81, 0x93, 0x02, 0x01, 0x00, 0x30, 0x13, 0x06, 0x07, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x02,
0x01, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x03, 0x01, 0x07, 0x04, 0x79, 0x30, 0x77, 0x02,
0x01, 0x01, 0x04, 0x20, 0xe9, 0xe4, 0x27, 0xb2, 0x5c, 0xe1, 0xe9, 0xc4, 0x9d, 0x23, 0x55, 0x67,
0x08, 0x66, 0x0c, 0xe5, 0x83, 0xa9, 0xf1, 0xe2, 0x09, 0xfb, 0x89, 0xea, 0xa8, 0xe4, 0x46, 0x6e,
0x76, 0xff, 0x75, 0x02, 0xa0, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x03, 0x01, 0x07,
0xa1, 0x44, 0x03, 0x42, 0x00, 0x04, 0x19, 0x1d, 0x6b, 0x4a, 0x12, 0xa7, 0x20, 0xf1, 0x95, 0xc6,
0x6d, 0x2a, 0xd7, 0x3b, 0xfa, 0x90, 0x8c, 0x52, 0xeb, 0x75, 0x67, 0xff, 0x0a, 0x3f, 0xf2, 0xdf,
0x8d, 0x81, 0x44, 0xc7, 0xc8, 0x84, 0x60, 0xd4, 0x07, 0x57, 0xb1, 0x96, 0xaf, 0x5e, 0x00, 0xa5,
0xed, 0xa1, 0x48, 0x3f, 0x88, 0x43, 0x8d, 0x15, 0x81, 0x0a, 0x21, 0x9b, 0x6c, 0xd3, 0xbd, 0x85,
0x86, 0xe1, 0xa6, 0xda, 0xc5, 0xce
};

void GetDerCaCert(ByteArray * crt)
{
    uint8_t *data = NULL;
    OCStackResult ret = OC_STACK_ERROR;
    OicSecCred_t * cred;
    OicSecCred_t * temp = NULL;
    OIC_LOG(DEBUG, TAG, "IN GetDerCaCert");
    LL_FOREACH(gCred, temp)
    {
        if (0==memcmp((temp->credUsage), TRUST_CA, sizeof(TRUST_CA)))
        {
            OIC_LOG_V(DEBUG, TAG, "len: %d, crt len: %d", temp->optionalData.len, crt->len);
            if(OIC_ENCODING_BASE64 == temp->optionalData.encoding)
            {
                size_t bufSize = B64DECODE_OUT_SAFESIZE((temp->optionalData.len + 1));
                uint8 * buf = OICCalloc(1, bufSize);
                if(NULL == buf)
                {
                    OIC_LOG(ERROR, TAG, "Failed to allocate memory");
                    return;
                }
                uint32_t outSize;
                b64Decode(temp->optionalData.data, temp->optionalData.len, buf, bufSize, &outSize);
                crt->data = OICRealloc(crt->data, crt->len + outSize);
                memcpy(crt->data + crt->len, buf, outSize);
                crt->len += outSize;
                OICFree(buf);
            }
            else
            {
                crt->data = OICRealloc(crt->data, crt->len + temp->optionalData.len);
                memcpy(crt->data + crt->len, temp->optionalData.data, temp->optionalData.len);
                crt->len += temp->optionalData.len;
            }
            OIC_LOG_V(DEBUG, TAG, "Trust CA Found!! %d", crt->len);
        }
    }
    if(!crt->len) OIC_LOG(DEBUG, TAG, "Trust CA Not Found!!");
    return;
}


void GetDerKey(ByteArray * key)
{
    key->data = OICMalloc(sizeof(ca_key));
    memcpy(key->data, ca_key, sizeof(ca_key));
    key->len = sizeof(ca_key);
    return;
}
#endif
