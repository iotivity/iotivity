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
#define __STDC_LIMIT_MACROS
#endif

#include <stdlib.h>
#include <string.h>
#include "ocstack.h"
#include "logger.h"
#include "oic_malloc.h"
#include "ocpayload.h"
#include "ocpayloadcbor.h"
#include "payload_logging.h"
#include "cainterface.h"
#include "secureresourcemanager.h"
#include "resourcemanager.h"
#include "srmresourcestrings.h"
#include "srmutility.h"

#define TAG  "SRM-PSI"

//SVR database buffer block size
const size_t DB_FILE_SIZE_BLOCK = 1023;

/**
 * Gets the Secure Virtual Database size.
 *
 * @param ps pointer of OCPersistentStorage for the SVR name ("acl", "cred", "pstat" etc).
 *
 * @return total size of the SVR database.
 */
static size_t GetSVRDatabaseSize(const OCPersistentStorage* ps)
{
    size_t size = 0;
    if (!ps)
    {
        return size;
    }
    size_t bytesRead  = 0;
    char buffer[DB_FILE_SIZE_BLOCK];
    FILE* fp = ps->open(SVR_DB_DAT_FILE_NAME, "r");
    if (fp)
    {
        do
        {
            bytesRead = ps->read(buffer, 1, DB_FILE_SIZE_BLOCK, fp);
            size += bytesRead;
        } while (bytesRead > 0);
        ps->close(fp);
    }
    return size;
}

OCStackResult GetSecureVirtualDatabaseFromPS(const char *rsrcName, uint8_t **data, size_t *size)
{
    if (!data || !size)
    {
        return OC_STACK_INVALID_PARAM;
    }
    OCStackResult ret = OC_STACK_ERROR;
    *data = NULL;

    FILE *fp = NULL;
    size_t fileSize = 0;
    uint8_t *fsData = NULL;

    OCPersistentStorage *ps = SRMGetPersistentStorageHandler();
    VERIFY_NON_NULL(TAG, ps, ERROR);

    fileSize = GetSVRDatabaseSize(ps);
    if (fileSize != 0)
    {
        OIC_LOG_V(DEBUG, TAG, "File Read Size: %zu", fileSize);
        fsData = (uint8_t *)OICCalloc(1, fileSize);
        VERIFY_NON_NULL(TAG, fsData, ERROR);

        FILE *fp = ps->open(SVR_DB_DAT_FILE_NAME, "rb");
        VERIFY_NON_NULL(TAG, fp, ERROR);
        size_t itemsRead = ps->read(fsData, 1, fileSize, fp);
        if (itemsRead == fileSize)
        {
            VERIFY_NON_NULL(TAG, fsData, ERROR);
            if (rsrcName != NULL)
            {
                CborParser parser = { .end = NULL };
                CborValue cbor =  { .parser = NULL };
                cbor_parser_init(fsData, fileSize, 0, &parser, &cbor);
                CborValue cborValue =  { .parser = NULL };
                // CborError cborFindResult = cbor_value_enter_container(&cbor, &cborValue);
                CborError cborFindResult = cbor_value_map_find_value(&cbor, rsrcName, &cborValue);
                if (CborNoError == cborFindResult && cbor_value_is_byte_string(&cborValue))
                {
                    cborFindResult = cbor_value_dup_byte_string(&cborValue, data, size, NULL);
                    VERIFY_SUCCESS(TAG, cborFindResult == CborNoError, ERROR);
                    ret = OC_STACK_OK;
                    goto exit;
                }
            }
            // return everything in case rsrcName is NULL
            else
            {
                *size = fileSize;
                *data = (uint8_t *)OICCalloc(1, fileSize);
                VERIFY_NON_NULL(TAG, *data, ERROR);
                memcpy(*data, fsData, fileSize);
            }
        }
    }
    else
    {
        ret = OC_STACK_OK;
    }

exit:
    OICFree(fsData);
    return ret;
}

OCStackResult UpdateSecureResourceInPS(const char* rsrcName, const uint8_t* psPayload, size_t psSize)
{
    OIC_LOG(DEBUG,  TAG, "Entering UpdateSecureResourceInPS IN");
    /*
     * This function stores cbor payload of each resource by appending resource name.
     */
     // Empty payload implies deleting the value
    if (!rsrcName)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult ret = OC_STACK_ERROR;

    uint8_t *dbData = NULL;
    size_t dbSize = 0;
    size_t cborSize = 0;
    uint8_t *outPayload = NULL;
    uint8_t *aclCbor = NULL;
    uint8_t *pstatCbor = NULL;
    uint8_t *amaclCbor = NULL;
    uint8_t *doxmCbor = NULL;
    uint8_t *svcCbor = NULL;
    uint8_t *credCbor = NULL;
    uint8_t *pconfCbor = NULL;

    int64_t cborEncoderResult = CborNoError;
    CborEncoder encoder;

    ret = GetSecureVirtualDatabaseFromPS(NULL, &dbData, &dbSize);
    if (dbData && dbSize != 0)
    {
        size_t aclCborLen = 0;
        size_t pstatCborLen = 0;
        size_t doxmCborLen = 0;
        size_t amaclCborLen = 0;
        size_t svcCborLen = 0;
        size_t credCborLen = 0;
        size_t pconfCborLen = 0;

        {
            CborValue cbor;
            CborParser parser;
            cbor_parser_init(dbData, dbSize, 0, &parser, &cbor);
            CborError cborFindResult = CborNoError;

            CborValue curVal;
            cborFindResult = cbor_value_map_find_value(&cbor, OIC_JSON_ACL_NAME, &curVal);
            if (CborNoError == cborFindResult && cbor_value_is_byte_string(&curVal))
            {
                cborFindResult = cbor_value_dup_byte_string(&curVal, &aclCbor, &aclCborLen, NULL);
                VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Finding ACL Name Value.");
            }

            cborFindResult = cbor_value_map_find_value(&cbor, OIC_JSON_DOXM_NAME, &curVal);
            if (CborNoError == cborFindResult && cbor_value_is_byte_string(&curVal))
            {
                cborFindResult = cbor_value_dup_byte_string(&curVal, &doxmCbor, &doxmCborLen, NULL);
                VERIFY_CBOR_SUCCESS(TAG, cborFindResult,  "Failed Finding DOXM Name Value.");
            }

            cborFindResult = cbor_value_map_find_value(&cbor, OIC_JSON_PSTAT_NAME, &curVal);
            if (CborNoError == cborFindResult && cbor_value_is_byte_string(&curVal))
            {
                cborFindResult = cbor_value_dup_byte_string(&curVal, &pstatCbor, &pstatCborLen, NULL);
                VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Finding PSTAT Name Value.");
            }

            cborFindResult = cbor_value_map_find_value(&cbor, OIC_JSON_AMACL_NAME, &curVal);
            if (CborNoError == cborFindResult && cbor_value_is_byte_string(&curVal))
            {
                cborFindResult = cbor_value_dup_byte_string(&curVal, &amaclCbor, &amaclCborLen, NULL);
                VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Finding AMACL Name Value.");
            }

            cborFindResult = cbor_value_map_find_value(&cbor, OIC_JSON_SVC_NAME, &curVal);
            if (CborNoError == cborFindResult && cbor_value_is_byte_string(&curVal))
            {
                cborFindResult = cbor_value_dup_byte_string(&curVal, &svcCbor, &svcCborLen, NULL);
                VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Finding SVC Name Value.");
            }

            cborFindResult = cbor_value_map_find_value(&cbor, OIC_JSON_CRED_NAME, &curVal);
            if (CborNoError == cborFindResult && cbor_value_is_byte_string(&curVal))
            {
                cborFindResult = cbor_value_dup_byte_string(&curVal, &credCbor, &credCborLen, NULL);
                VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Finding CRED Name Value.");
            }
            cborFindResult = cbor_value_map_find_value(&cbor, OIC_JSON_PCONF_NAME, &curVal);
            if (CborNoError == cborFindResult && cbor_value_is_byte_string(&curVal))
            {
                cborFindResult = cbor_value_dup_byte_string(&curVal, &pconfCbor, &pconfCborLen, NULL);
                VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Finding PCONF Name Value.");
            }

        }

        {
            size_t size = aclCborLen + pstatCborLen + doxmCborLen + amaclCborLen + svcCborLen
                + credCborLen + pconfCborLen +psSize;
            // This is arbitrary value that is added to cover the name of the resource, map addition and ending.
            size += 255;

            outPayload = (uint8_t *)OICCalloc(1, size);
            VERIFY_NON_NULL(TAG, outPayload, ERROR);
            cbor_encoder_init(&encoder, outPayload, size, 0);

            CborEncoder secRsrc;
            cborEncoderResult |= cbor_encoder_create_map(&encoder, &secRsrc, CborIndefiniteLength);
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding PS Map.");

            if (psPayload)
            {
                cborEncoderResult |= cbor_encode_text_string(&secRsrc, rsrcName, strlen(rsrcName));
                VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding Value Tag");
                cborEncoderResult |= cbor_encode_byte_string(&secRsrc, psPayload, psSize);
                VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding Value.");
            }

            if (0 != strcmp(OIC_JSON_ACL_NAME, rsrcName) && aclCborLen > 0)
            {
                cborEncoderResult |= cbor_encode_text_string(&secRsrc, OIC_JSON_ACL_NAME, strlen(OIC_JSON_ACL_NAME));
                VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding ACL Name.");
                cborEncoderResult |= cbor_encode_byte_string(&secRsrc, aclCbor, aclCborLen);
                VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding ACL Value.");
            }
            if (0 != strcmp(OIC_JSON_PSTAT_NAME, rsrcName) && pstatCborLen > 0)
            {
                cborEncoderResult |= cbor_encode_text_string(&secRsrc, OIC_JSON_PSTAT_NAME, strlen(OIC_JSON_PSTAT_NAME));
                VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding PSTAT Name.");
                cborEncoderResult |= cbor_encode_byte_string(&secRsrc, pstatCbor, pstatCborLen);
                VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding PSTAT Value.");
            }
            if (0 != strcmp(OIC_JSON_DOXM_NAME, rsrcName) && doxmCborLen > 0)
            {
                cborEncoderResult |= cbor_encode_text_string(&secRsrc, OIC_JSON_DOXM_NAME, strlen(OIC_JSON_DOXM_NAME));
                VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding Doxm Name.");
                cborEncoderResult |= cbor_encode_byte_string(&secRsrc, doxmCbor, doxmCborLen);
                VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding Doxm Value.");
            }
            if (0 != strcmp(OIC_JSON_AMACL_NAME, rsrcName) && amaclCborLen > 0)
            {
                cborEncoderResult |= cbor_encode_text_string(&secRsrc, OIC_JSON_AMACL_NAME, strlen(OIC_JSON_AMACL_NAME));
                VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding Amacl Name.");
                cborEncoderResult |= cbor_encode_byte_string(&secRsrc, amaclCbor, amaclCborLen);
                VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding Amacl Value.");
            }
            if (0 != strcmp(OIC_JSON_SVC_NAME, rsrcName) && svcCborLen > 0)
            {
                cborEncoderResult |= cbor_encode_text_string(&secRsrc, OIC_JSON_SVC_NAME, strlen(OIC_JSON_SVC_NAME));
                VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding SVC Name.");
                cborEncoderResult |= cbor_encode_byte_string(&secRsrc, svcCbor, svcCborLen);
                VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding SVC Value.");
            }
            if (0 != strcmp(OIC_JSON_CRED_NAME, rsrcName) && credCborLen > 0)
            {
                cborEncoderResult |= cbor_encode_text_string(&secRsrc, OIC_JSON_CRED_NAME, strlen(OIC_JSON_CRED_NAME));
                VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding Cred Name.");
                cborEncoderResult |= cbor_encode_byte_string(&secRsrc, credCbor, credCborLen);
                VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding Cred Value.");
            }
            if (0 != strcmp(OIC_JSON_PCONF_NAME, rsrcName) && pconfCborLen > 0)
            {
                cborEncoderResult |= cbor_encode_text_string(&secRsrc, OIC_JSON_PCONF_NAME, strlen(OIC_JSON_PCONF_NAME));
                VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding Pconf Name.");
                cborEncoderResult |= cbor_encode_byte_string(&secRsrc, pconfCbor, pconfCborLen);
                VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding Pconf Value.");
            }

            cborEncoderResult |= cbor_encoder_close_container(&encoder, &secRsrc);
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Closing Array.");

            cborSize = encoder.ptr - outPayload;
        }
    }
    else if (psPayload)
    {
        size_t size = psSize + 255;
        outPayload = (uint8_t *)OICCalloc(1, size);
        VERIFY_NON_NULL(TAG, outPayload, ERROR);
        CborEncoder encoder;
        cbor_encoder_init(&encoder, outPayload, size, 0);

        CborEncoder secRsrc ;
        cborEncoderResult |= cbor_encoder_create_map(&encoder, &secRsrc, CborIndefiniteLength);
        cborEncoderResult |= cbor_encode_text_string(&secRsrc, rsrcName, strlen(rsrcName));
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding Value Tag");
        cborEncoderResult |= cbor_encode_byte_string(&secRsrc, psPayload, psSize);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding Value.");
        cborEncoderResult |= cbor_encoder_close_container(&encoder, &secRsrc);
        cborSize = encoder.ptr - outPayload;
    }

    if (outPayload && cborSize > 0)
    {
        OIC_LOG_V(DEBUG, TAG, "Writting in the file. %zd", cborSize);
        OCPersistentStorage* ps = SRMGetPersistentStorageHandler();
        if (ps)
        {
            FILE *fp = ps->open(SVR_DB_DAT_FILE_NAME, "w+");
            if (fp)
            {
                size_t numberItems = ps->write(outPayload, 1, cborSize, fp);
                if (cborSize == numberItems)
                {
                    OIC_LOG_V(DEBUG, TAG, "Written %zu bytes into SVR database file", cborSize);
                    ret = OC_STACK_OK;
                }
                else
                {
                    OIC_LOG_V(ERROR, TAG, "Failed writing %zu in the database", numberItems);
                }
                ps->close(fp);
            }
            else
            {
                OIC_LOG(ERROR, TAG, "File open failed.");
            }

        }
        OIC_LOG_V(DEBUG, TAG, "Writing in the file . %zd", cborSize);
    }

    OIC_LOG(DEBUG,  TAG, "Exiting UpdateSecureResourceInPS OUT");

exit:
    OICFree(dbData);
    OICFree(outPayload);

    OICFree(aclCbor);
    OICFree(pstatCbor);
    OICFree(amaclCbor);
    OICFree(doxmCbor);
    OICFree(svcCbor);
    OICFree(credCbor);

    return ret;
}
