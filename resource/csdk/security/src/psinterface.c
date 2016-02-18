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
#include "cJSON.h"
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

char* GetSVRDatabase()
{
    char * jsonStr = NULL;
    FILE * fp = NULL;
    OCPersistentStorage* ps = SRMGetPersistentStorageHandler();
    int size = GetSVRDatabaseSize(ps);
    if (0 == size)
    {
        OC_LOG (ERROR, TAG, "FindSVRDatabaseSize failed");
        return NULL;
    }

    if (ps && ps->open)
    {
        // Open default SRM database file. An app could change the path for its server.
        fp = ps->open(SVR_DB_FILE_NAME, "r");
        if (fp)
        {
            jsonStr = (char*)OICMalloc(size + 1);
            VERIFY_NON_NULL(TAG, jsonStr, FATAL);
            size_t bytesRead = ps->read(jsonStr, 1, size, fp);
            jsonStr[bytesRead] = '\0';

            OC_LOG_V(DEBUG, TAG, "Read %zu bytes from SVR database file", bytesRead);
            ps->close(fp);
            fp = NULL;
        }
        else
        {
            OC_LOG (ERROR, TAG, "Unable to open SVR database file!!");
        }
    }

exit:
    if (ps && fp)
    {
        ps->close(fp);
    }
    return jsonStr;
}

OCStackResult UpdateSVRDatabase(const char* rsrcName, cJSON* jsonObj)
{
    OCStackResult ret = OC_STACK_ERROR;
    cJSON *jsonSVRDb = NULL;
    OCPersistentStorage* ps = NULL;

    // Read SVR database from PS
    char* jsonSVRDbStr = GetSVRDatabase();
    VERIFY_NON_NULL(TAG,jsonSVRDbStr, ERROR);

    // Use cJSON_Parse to parse the existing SVR database
    jsonSVRDb = cJSON_Parse(jsonSVRDbStr);
    VERIFY_NON_NULL(TAG,jsonSVRDb, ERROR);

    OICFree(jsonSVRDbStr);
    jsonSVRDbStr = NULL;

    //If Cred resource gets updated with empty list then delete the Cred
    //object from database.
    if(NULL == jsonObj && (0 == strcmp(rsrcName, OIC_JSON_CRED_NAME)))
    {
        cJSON_DeleteItemFromObject(jsonSVRDb, rsrcName);
    }
    else if (jsonObj->child )
    {
        // Create a duplicate of the JSON object which was passed.
        cJSON* jsonDuplicateObj = cJSON_Duplicate(jsonObj, 1);
        VERIFY_NON_NULL(TAG,jsonDuplicateObj, ERROR);

        cJSON* jsonObj = cJSON_GetObjectItem(jsonSVRDb, rsrcName);

        /*
         ACL, PStat & Doxm resources at least have default entries in the database but
         Cred resource may have no entries. The first cred resource entry (for provisioning tool)
         is created when the device is owned by provisioning tool and it's ownerpsk is generated.*/
        if((strcmp(rsrcName, OIC_JSON_CRED_NAME) == 0 || strcmp(rsrcName, OIC_JSON_CRL_NAME) == 0)
                                                                                    && (!jsonObj))
        {
            // Add the fist cred object in existing SVR database json
            cJSON_AddItemToObject(jsonSVRDb, rsrcName, jsonDuplicateObj->child);
        }
        else
        {
            VERIFY_NON_NULL(TAG,jsonObj, ERROR);

            // Replace the modified json object in existing SVR database json
            cJSON_ReplaceItemInObject(jsonSVRDb, rsrcName, jsonDuplicateObj->child);
        }
    }

    // Generate string representation of updated SVR database json object
    jsonSVRDbStr = cJSON_PrintUnformatted(jsonSVRDb);
    VERIFY_NON_NULL(TAG,jsonSVRDbStr, ERROR);

    // Update the persistent storage with new SVR database
    ps = SRMGetPersistentStorageHandler();
    if (ps && ps->open)
    {
        FILE* fp = ps->open(SVR_DB_FILE_NAME, "w");
        if (fp)
        {
            size_t bytesWritten = ps->write(jsonSVRDbStr, 1, strlen(jsonSVRDbStr), fp);
            if (bytesWritten == strlen(jsonSVRDbStr))
            {
                ret = OC_STACK_OK;
            }
            OC_LOG_V(DEBUG, TAG, "Written %zu bytes into SVR database file", bytesWritten);
            ps->close(fp);
            fp = NULL;
        }
        else
        {
            OC_LOG (ERROR, TAG, "Unable to open SVR database file!! ");
        }
    }

exit:
    OICFree(jsonSVRDbStr);
    cJSON_Delete(jsonSVRDb);

    return ret;
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

    OCPersistentStorage *ps = SRMGetPersistentStorageHandler();
    VERIFY_NON_NULL(TAG, ps, ERROR);

    fileSize = GetSVRDatabaseSize(ps);
    if (fileSize != 0)
    {
        OC_LOG_V(DEBUG, TAG, "File Read Size: %zu", fileSize);
        uint8_t *fsData = (uint8_t *)OICCalloc(1, fileSize);
        VERIFY_NON_NULL(TAG, fsData, ERROR);

        FILE *fp = ps->open(SVR_DB_DAT_FILE_NAME, "r");
        VERIFY_NON_NULL(TAG, fp, ERROR);
        size_t itemsRead = ps->read(fsData, 1, fileSize, fp);
        if (itemsRead == fileSize)
        {
            VERIFY_NON_NULL(TAG, fsData, ERROR);
            if (rsrcName != NULL)
            {
                CborParser parser = { .end = NULL, .flags = 0 };
                CborValue cbor =  { .parser = NULL, .ptr = NULL, .remaining = 0, .extra = 0, .type = 0, .flags = 0 };
                cbor_parser_init(fsData, fileSize, 0, &parser, &cbor);
                CborValue cborValue =  { .parser = NULL, .ptr = NULL, .remaining = 0, .extra = 0, .type = 0, .flags = 0 };
                CborError cborFindResult = cbor_value_enter_container(&cbor, &cborValue);

                while (cbor_value_is_valid(&cborValue))
                {
                    char *name = NULL;
                    size_t len = 0;
                    cborFindResult = cbor_value_dup_text_string(&cborValue, &name, &len, NULL);
                    VERIFY_SUCCESS(TAG, cborFindResult == CborNoError, ERROR);
                    cborFindResult = cbor_value_advance(&cborValue);
                    VERIFY_SUCCESS(TAG, cborFindResult == CborNoError, ERROR);
                    if (strcmp(name, rsrcName) == 0)
                    {
                        cborFindResult = cbor_value_dup_byte_string(&cborValue, data, size, NULL);
                        VERIFY_SUCCESS(TAG, cborFindResult == CborNoError, ERROR);
                        ret = OC_STACK_OK;
                        OICFree(fsData);
                        OICFree(name);
                        goto exit;
                    }
                    OICFree(name);
                    cborFindResult = cbor_value_advance(&cborValue);
                    VERIFY_SUCCESS(TAG, cborFindResult == CborNoError, ERROR);
                }
            }
            // return everything in case rsrcName is NULL
            else
            {
                *data = fsData;
                *size = fileSize;
            }
        }
    }
    else
    {
        OC_LOG (ERROR, TAG, "Unable to open SVR database to read!! ");
    }

exit:
    if (ps && fp)
    {
        ps->close(fp);
    }
    return ret;
}

OCStackResult UpdateSecureResourceInPS(const char* rsrcName, const uint8_t* psPayload, size_t psSize)
{
    /*
     * This function stores cbor payload of each resource by appending resource name.
     */
    if (!rsrcName || !*psPayload)
    {
        return OC_STACK_INVALID_PARAM;
    }
    OCStackResult ret = OC_STACK_ERROR;

    size_t cborSize = 0;
    uint8_t *dbData = NULL;
    uint8_t *outPayload = NULL;
    size_t dbSize = 0;

    ret = GetSecureVirtualDatabaseFromPS(NULL, &dbData, &dbSize);
    if (dbData && dbSize != 0)
    {
        uint8_t size = dbSize + psSize;

        outPayload = (uint8_t *)OICCalloc(1, size);
        VERIFY_NON_NULL(TAG, outPayload, ERROR);

        CborEncoder encoder = { { .ptr = NULL }, .end = NULL, .added = 0, .flags = 0};
        cbor_encoder_init(&encoder, outPayload, size, 0);
        {
            CborEncoder map = { {.ptr = NULL }, .end = 0, .added = 0, .flags = 0};
            CborError cborEncoderResult = cbor_encoder_create_map(&encoder, &map, CborIndefiniteLength);
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);
            {
                bool found = false;
                CborValue cbor = { .parser = NULL, .ptr = NULL, .remaining = 0, .extra = 0, .type = 0, .flags = 0 };
                CborParser parser = { .end = NULL, .flags = 0 };
                cbor_parser_init(dbData, size, 0, &parser, &cbor);

                CborValue cborValue = { .parser = NULL, .ptr = NULL, .remaining = 0, .extra = 0, .type = 0, .flags = 0 };
                CborError cborFindResult = CborNoError;

                if (cbor_value_is_container(&cbor))
                {
                    cborFindResult = cbor_value_enter_container(&cbor, &cborValue);
                    VERIFY_CBOR_SUCCESS(TAG, cborFindResult, ERROR);
                }

                while (cbor_value_is_valid(&cborValue))
                {
                    char *name = NULL;
                    size_t len = 0;
                    cborFindResult = cbor_value_dup_text_string(&cborValue, &name, &len, NULL);
                    VERIFY_CBOR_SUCCESS(TAG, cborFindResult, ERROR);
                    cborFindResult = cbor_value_advance(&cborValue);
                    VERIFY_CBOR_SUCCESS(TAG, cborFindResult, ERROR);

                    cborEncoderResult = cbor_encode_text_string(&map, name, strlen(name));
                    VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);

                    if (strcmp(name, rsrcName) == 0)
                    {
                        cborEncoderResult = cbor_encode_byte_string(&map, psPayload, psSize);
                        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);
                        found = true;
                    }
                    else
                    {
                        uint8_t *byteString = NULL;
                        size_t byteLen = 0;
                        cborFindResult = cbor_value_dup_byte_string(&cborValue, &byteString, &byteLen, NULL);
                        VERIFY_SUCCESS(TAG, cborFindResult == CborNoError, ERROR);
                        if (byteString)
                        {
                            cborEncoderResult = cbor_encode_byte_string(&map, byteString, byteLen);
                            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);
                        }
                        OICFree(byteString);
                    }
                    OICFree(name);
                    cbor_value_advance(&cborValue);
                }

                // This is an exception when the value is not stored in the database.
                if (!found)
                {
                    cborEncoderResult = cbor_encode_text_string(&map, rsrcName, strlen(rsrcName));
                    VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);
                    cborEncoderResult = cbor_encode_byte_string(&map, psPayload, psSize);
                    VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);
                }
            }
            cborEncoderResult = cbor_encoder_close_container(&encoder, &map);
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, ERROR);
        }
        cborSize = encoder.ptr - outPayload;
    }

    {
        OCPersistentStorage* ps = SRMGetPersistentStorageHandler();
        if (ps)
        {
            FILE *fp = ps->open(SVR_DB_DAT_FILE_NAME, "w+");
            if (fp)
            {
                size_t numberItems = ps->write(outPayload, 1, cborSize, fp);
                if (cborSize == numberItems)
                {
                    OC_LOG_V(DEBUG, TAG, "Written %zu bytes into SVR database file", cborSize);
                    ret = OC_STACK_OK;
                }
                else
                {
                    OC_LOG_V(ERROR, TAG, "Failed writing %zu in the database", numberItems);
                }
                ps->close(fp);
            }
            else
            {
                OC_LOG(ERROR, TAG, "File open failed.");
            }

        }
    }


exit:
    if (dbData)
    {
        OICFree(dbData);
    }
    if (outPayload)
    {
        OICFree(outPayload);
    }

    return ret;
}
