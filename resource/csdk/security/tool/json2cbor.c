//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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
#include "cJSON.h"
#include "base64.h"
#include "cainterface.h"
#include "ocstack.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "ocpayload.h"
#include "ocpayloadcbor.h"
#include "payload_logging.h"
#include "secureresourcemanager.h"
#include "srmresourcestrings.h"
#include "srmutility.h"
#include "aclresource.h"
#include "pstatresource.h"
#include "doxmresource.h"
#include "amaclresource.h"
#include "credresource.h"
#include "svcresource.h"
#include "security_internals.h"

#define TAG  "JSON2CBOR"
//SVR database buffer block size
static const size_t DB_FILE_SIZE_BLOCK = 1023;

static OicSecPstat_t* JSONToPstatBin(const char * jsonStr);
static OicSecDoxm_t* JSONToDoxmBin(const char * jsonStr);
static OicSecAcl_t *JSONToAclBin(const char * jsonStr);
static OicSecSvc_t* JSONToSvcBin(const char * jsonStr);
static OicSecAmacl_t* JSONToAmaclBin(const char * jsonStr);
static OicSecCred_t* JSONToCredBin(const char * jsonStr);

static size_t GetJSONFileSize(const char *jsonFileName)
{
    size_t size = 0;
    size_t bytesRead  = 0;
    char buffer[DB_FILE_SIZE_BLOCK];
    FILE* fp = fopen(jsonFileName, "r");
    if (fp)
    {
        do
        {
            bytesRead = fread(buffer, 1, DB_FILE_SIZE_BLOCK, fp);
            size += bytesRead;
        } while (bytesRead > 0);
        fclose(fp);
    }
    return size;
}

static void ConvertJsonToCBOR(const char *jsonFileName, const char *cborFileName)
{
    char *jsonStr = NULL;
    FILE *fp = NULL;
    FILE *fp1 = NULL;
    uint8_t *aclCbor = NULL;
    uint8_t *pstatCbor = NULL;
    uint8_t *doxmCbor = NULL;
    uint8_t *amaclCbor = NULL;
    uint8_t *svcCbor = NULL;
    uint8_t *credCbor = NULL;
    cJSON *jsonRoot = NULL;
    size_t size = GetJSONFileSize(jsonFileName);
    if (0 == size)
    {
        OIC_LOG (ERROR, TAG, "Failed converting to JSON");
        return;
    }

    fp = fopen(jsonFileName, "r");
    if (fp)
    {
        jsonStr = (char *)OICMalloc(size + 1);
        VERIFY_NON_NULL(TAG, jsonStr, FATAL);
        size_t bytesRead = fread(jsonStr, 1, size, fp);
        jsonStr[bytesRead] = '\0';

        OIC_LOG_V(DEBUG, TAG, "Read %zu bytes", bytesRead);
        fclose(fp);
        fp = NULL;
    }
    else
    {
        OIC_LOG (ERROR, TAG, "Unable to open JSON file!!");
        goto exit;
    }

    jsonRoot = cJSON_Parse(jsonStr);
    cJSON *value = cJSON_GetObjectItem(jsonRoot, OIC_JSON_ACL_NAME);
    size_t aclCborSize = 0;
    if (NULL != value)
    {
        OicSecAcl_t *acl = JSONToAclBin(jsonStr);
        AclToCBORPayload(acl, &aclCbor, &aclCborSize);
        printf("ACL Cbor Size: %zd\n", aclCborSize);
        DeleteACLList(acl);
    }
    value = cJSON_GetObjectItem(jsonRoot, OIC_JSON_PSTAT_NAME);
    size_t pstatCborSize = 0;
    if (NULL != value)
    {
        OicSecPstat_t *pstat = JSONToPstatBin(jsonStr);
        PstatToCBORPayload(pstat, &pstatCbor, &pstatCborSize);
        printf("PSTAT Cbor Size: %zd\n", pstatCborSize);
        DeletePstatBinData(pstat);
    }
    value = cJSON_GetObjectItem(jsonRoot, OIC_JSON_DOXM_NAME);
    size_t doxmCborSize = 0;
    if (NULL != value)
    {
        OicSecDoxm_t *doxm = JSONToDoxmBin(jsonStr);
        DoxmToCBORPayload(doxm, &doxmCbor, &doxmCborSize);
        printf("DOXM Cbor Size: %zd\n", doxmCborSize);
        DeleteDoxmBinData(doxm);
    }
    value = cJSON_GetObjectItem(jsonRoot, OIC_JSON_AMACL_NAME);
    size_t amaclCborSize = 0;
    if (NULL != value)
    {
        OicSecAmacl_t *amacl = JSONToAmaclBin(jsonStr);
        AmaclToCBORPayload(amacl, &amaclCbor, &amaclCborSize);
        printf("AMACL Cbor Size: %zd\n", amaclCborSize);
        DeleteAmaclList(amacl);
    }
    value = cJSON_GetObjectItem(jsonRoot, OIC_JSON_SVC_NAME);
    size_t svcCborSize = 0;
    if (NULL != value)
    {
        OicSecSvc_t *svc = JSONToSvcBin(jsonStr);
        SVCToCBORPayload(svc, &svcCbor, &svcCborSize);
        printf("SVC Cbor Size: %zd\n", svcCborSize);
        DeleteSVCList(svc);
    }
    value = cJSON_GetObjectItem(jsonRoot, OIC_JSON_CRED_NAME);
    size_t credCborSize = 0;
    if (NULL != value)
    {
        OicSecCred_t *cred = JSONToCredBin(jsonStr);
        CredToCBORPayload(cred, &credCbor, &credCborSize);
        printf("CRED Cbor Size: %zd\n", credCborSize);
        DeleteCredList(cred);
    }

    cJSON_Delete(value);
    CborEncoder encoder;
    size_t cborSize = aclCborSize + pstatCborSize + doxmCborSize + svcCborSize + credCborSize + amaclCborSize;
    printf("Total Cbor Size : %zd\n", cborSize);
    cborSize += 255; // buffer margin for adding map and byte string
    uint8_t *outPayload = (uint8_t *)OICCalloc(1, cborSize);
    VERIFY_NON_NULL(TAG, outPayload, ERROR);
    cbor_encoder_init(&encoder, outPayload, cborSize, 0);
    CborEncoder map;
    CborError cborEncoderResult = cbor_encoder_create_map(&encoder, &map, CborIndefiniteLength);
    VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Creating Main Map.");
    if (aclCborSize > 0)
    {
        cborEncoderResult = cbor_encode_text_string(&map, OIC_JSON_ACL_NAME, strlen(OIC_JSON_ACL_NAME));
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding ACL Name.");
        cborEncoderResult = cbor_encode_byte_string(&map, aclCbor, aclCborSize);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding ACL Value.");
    }
    if (pstatCborSize > 0)
    {
        cborEncoderResult = cbor_encode_text_string(&map, OIC_JSON_PSTAT_NAME, strlen(OIC_JSON_PSTAT_NAME));
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding PSTAT Name.");
        cborEncoderResult = cbor_encode_byte_string(&map, pstatCbor, pstatCborSize);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding PSTAT Value.");
    }
    if (doxmCborSize > 0)
    {
        cborEncoderResult = cbor_encode_text_string(&map, OIC_JSON_DOXM_NAME, strlen(OIC_JSON_DOXM_NAME));
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding DOXM Name.");
        cborEncoderResult = cbor_encode_byte_string(&map, doxmCbor, doxmCborSize);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding DOXM Value.");
    }
    if (amaclCborSize > 0)
    {
        cborEncoderResult = cbor_encode_text_string(&map, OIC_JSON_AMACL_NAME, strlen(OIC_JSON_AMACL_NAME));
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding AMACL Name.");
        cborEncoderResult = cbor_encode_byte_string(&map, amaclCbor, amaclCborSize);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding AMACL Value.");
    }
    if (svcCborSize > 0)
    {
        cborEncoderResult = cbor_encode_text_string(&map, OIC_JSON_SVC_NAME, strlen(OIC_JSON_SVC_NAME));
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding SVC Name.");
        cborEncoderResult = cbor_encode_byte_string(&map, svcCbor, svcCborSize);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding SVC Value.");
    }
    if (credCborSize > 0)
    {
        cborEncoderResult = cbor_encode_text_string(&map, OIC_JSON_CRED_NAME, strlen(OIC_JSON_CRED_NAME));
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding CRED Name.");
        cborEncoderResult = cbor_encode_byte_string(&map, credCbor, credCborSize);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding CRED Value.");
    }
    cborEncoderResult = cbor_encoder_close_container(&encoder, &map);
    VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Closing Container.");

    size_t s = encoder.ptr - outPayload;
    OIC_LOG_V(DEBUG, TAG, "Payload size %zu", s);

    fp1 = fopen(cborFileName, "w");
    if (fp1)
    {
        size_t bytesWritten = fwrite(outPayload, 1, s, fp1);
        OIC_LOG_V(DEBUG, TAG, "Written %zu bytes", bytesWritten);
        fclose(fp1);
        fp1 = NULL;
    }
exit:
    if (fp)
    {
        fclose(fp);
    }
    if (fp1)
    {
        fclose(fp1);
    }
    // cJSON_Delete(jsonRoot);
    OICFree(aclCbor);
    OICFree(doxmCbor);
    OICFree(pstatCbor);
    OICFree(amaclCbor);
    OICFree(svcCbor);
    OICFree(credCbor);
    OICFree(jsonStr);
    return ;
}

OicSecAcl_t* JSONToAclBin(const char * jsonStr)
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

            size_t idxx = 0;
            do
            {
                cJSON *jsonRsrc = cJSON_GetArrayItem(jsonObj, idxx);
                VERIFY_NON_NULL(TAG, jsonRsrc, ERROR);

                jsonObjLen = strlen(jsonRsrc->valuestring) + 1;
                acl->resources[idxx] = (char*)OICMalloc(jsonObjLen);
                VERIFY_NON_NULL(TAG, (acl->resources[idxx]), ERROR);
                OICStrcpy(acl->resources[idxx], jsonObjLen, jsonRsrc->valuestring);
            } while ( ++idxx < acl->resourcesLen);

            // Permissions -- Mandatory
            jsonObj = cJSON_GetObjectItem(jsonAcl, OIC_JSON_PERMISSION_NAME);
            VERIFY_NON_NULL(TAG, jsonObj, ERROR);
            VERIFY_SUCCESS(TAG, cJSON_Number == jsonObj->type, ERROR);
            acl->permission = jsonObj->valueint;

            //Period -- Not Mandatory
            cJSON *jsonPeriodObj = cJSON_GetObjectItem(jsonAcl, OIC_JSON_PERIODS_NAME);
            if(jsonPeriodObj)
            {
                VERIFY_SUCCESS(TAG, cJSON_Array == jsonPeriodObj->type, ERROR);
                acl->prdRecrLen = cJSON_GetArraySize(jsonPeriodObj);
                if(acl->prdRecrLen > 0)
                {
                    acl->periods = (char**)OICCalloc(acl->prdRecrLen, sizeof(char*));
                    VERIFY_NON_NULL(TAG, acl->periods, ERROR);

                    cJSON *jsonPeriod = NULL;
                    for(size_t i = 0; i < acl->prdRecrLen; i++)
                    {
                        jsonPeriod = cJSON_GetArrayItem(jsonPeriodObj, i);
                        VERIFY_NON_NULL(TAG, jsonPeriod, ERROR);

                        jsonObjLen = strlen(jsonPeriod->valuestring) + 1;
                        acl->periods[i] = (char*)OICMalloc(jsonObjLen);
                        VERIFY_NON_NULL(TAG, acl->periods[i], ERROR);
                        OICStrcpy(acl->periods[i], jsonObjLen, jsonPeriod->valuestring);
                    }
                }
            }

            //Recurrence -- Not mandatory
            cJSON *jsonRecurObj = cJSON_GetObjectItem(jsonAcl, OIC_JSON_RECURRENCES_NAME);
            if(jsonRecurObj)
            {
                VERIFY_SUCCESS(TAG, cJSON_Array == jsonRecurObj->type, ERROR);

                if(acl->prdRecrLen > 0)
                {
                    acl->recurrences = (char**)OICCalloc(acl->prdRecrLen, sizeof(char*));
                    VERIFY_NON_NULL(TAG, acl->recurrences, ERROR);

                    cJSON *jsonRecur = NULL;
                    for(size_t i = 0; i < acl->prdRecrLen; i++)
                    {
                        jsonRecur = cJSON_GetArrayItem(jsonRecurObj, i);
                        VERIFY_NON_NULL(TAG, jsonRecur, ERROR);
                        jsonObjLen = strlen(jsonRecur->valuestring) + 1;
                        acl->recurrences[i] = (char*)OICMalloc(jsonObjLen);
                        VERIFY_NON_NULL(TAG, acl->recurrences[i], ERROR);
                        OICStrcpy(acl->recurrences[i], jsonObjLen, jsonRecur->valuestring);
                    }
                }
            }

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

OicSecDoxm_t* JSONToDoxmBin(const char * jsonStr)
{
    if (NULL == jsonStr)
    {
        return NULL;
    }

    OCStackResult ret = OC_STACK_ERROR;
    OicSecDoxm_t *doxm =  NULL;
    cJSON *jsonDoxm = NULL;
    cJSON *jsonObj = NULL;

    size_t jsonObjLen = 0;
    unsigned char base64Buff[sizeof(((OicUuid_t*)0)->id)] = {};
    uint32_t outLen = 0;
    B64Result b64Ret = B64_OK;

    cJSON *jsonRoot = cJSON_Parse(jsonStr);
    VERIFY_NON_NULL(TAG, jsonRoot, ERROR);

    jsonDoxm = cJSON_GetObjectItem(jsonRoot, OIC_JSON_DOXM_NAME);
    VERIFY_NON_NULL(TAG, jsonDoxm, ERROR);

    doxm = (OicSecDoxm_t *)OICCalloc(1, sizeof(OicSecDoxm_t));
    VERIFY_NON_NULL(TAG, doxm, ERROR);

    //OxmType -- not Mandatory
    jsonObj = cJSON_GetObjectItem(jsonDoxm, OIC_JSON_OXM_TYPE_NAME);
    if ((jsonObj) && (cJSON_Array == jsonObj->type))
    {
        doxm->oxmTypeLen = cJSON_GetArraySize(jsonObj);
        VERIFY_SUCCESS(TAG, doxm->oxmTypeLen > 0, ERROR);

        doxm->oxmType = (OicUrn_t *)OICCalloc(doxm->oxmTypeLen, sizeof(char *));
        VERIFY_NON_NULL(TAG, (doxm->oxmType), ERROR);

        for (size_t i  = 0; i < doxm->oxmTypeLen ; i++)
        {
            cJSON *jsonOxmTy = cJSON_GetArrayItem(jsonObj, i);
            VERIFY_NON_NULL(TAG, jsonOxmTy, ERROR);

            jsonObjLen = strlen(jsonOxmTy->valuestring) + 1;
            doxm->oxmType[i] = (char*)OICMalloc(jsonObjLen);
            VERIFY_NON_NULL(TAG, doxm->oxmType[i], ERROR);
            strncpy((char *)doxm->oxmType[i], (char *)jsonOxmTy->valuestring, jsonObjLen);
        }
    }

    //Oxm -- not Mandatory
    jsonObj = cJSON_GetObjectItem(jsonDoxm, OIC_JSON_OXM_NAME);
    if (jsonObj && cJSON_Array == jsonObj->type)
    {
        doxm->oxmLen = cJSON_GetArraySize(jsonObj);
        VERIFY_SUCCESS(TAG, doxm->oxmLen > 0, ERROR);

        doxm->oxm = (OicSecOxm_t*)OICCalloc(doxm->oxmLen, sizeof(OicSecOxm_t));
        VERIFY_NON_NULL(TAG, doxm->oxm, ERROR);

        for (size_t i  = 0; i < doxm->oxmLen ; i++)
        {
            cJSON *jsonOxm = cJSON_GetArrayItem(jsonObj, i);
            VERIFY_NON_NULL(TAG, jsonOxm, ERROR);
            doxm->oxm[i] = (OicSecOxm_t)jsonOxm->valueint;
        }
    }

    //OxmSel -- Mandatory
    jsonObj = cJSON_GetObjectItem(jsonDoxm, OIC_JSON_OXM_SEL_NAME);
    if (jsonObj)
    {
        VERIFY_SUCCESS(TAG, cJSON_Number == jsonObj->type, ERROR);
        doxm->oxmSel = (OicSecOxm_t)jsonObj->valueint;
    }

    //sct -- Mandatory
    jsonObj = cJSON_GetObjectItem(jsonDoxm, OIC_JSON_SUPPORTED_CRED_TYPE_NAME);
    if (jsonObj)
    {
        VERIFY_SUCCESS(TAG, cJSON_Number == jsonObj->type, ERROR);
        doxm->sct = (OicSecCredType_t)jsonObj->valueint;
    }

    //Owned -- Mandatory
    jsonObj = cJSON_GetObjectItem(jsonDoxm, OIC_JSON_OWNED_NAME);
    if (jsonObj)
    {
        VERIFY_SUCCESS(TAG, (cJSON_True == jsonObj->type || cJSON_False == jsonObj->type), ERROR);
        doxm->owned = jsonObj->valueint;
    }

    //DeviceId -- Mandatory
    jsonObj = cJSON_GetObjectItem(jsonDoxm, OIC_JSON_DEVICE_ID_NAME);
    if (jsonObj)
    {
        VERIFY_SUCCESS(TAG, cJSON_String == jsonObj->type, ERROR);
        if (cJSON_String == jsonObj->type)
        {
            //Check for empty string, in case DeviceId field has not been set yet
            if (jsonObj->valuestring[0])
            {
                outLen = 0;
                b64Ret = b64Decode(jsonObj->valuestring, strlen(jsonObj->valuestring), base64Buff,
                       sizeof(base64Buff), &outLen);
                VERIFY_SUCCESS(TAG, (b64Ret == B64_OK && outLen <= sizeof(doxm->deviceID.id)),
                               ERROR);
                memcpy(doxm->deviceID.id, base64Buff, outLen);
            }
        }
    }

    //Owner -- will be empty when device status is unowned.
    jsonObj = cJSON_GetObjectItem(jsonDoxm, OIC_JSON_OWNER_NAME);
    if (true == doxm->owned)
    {
        VERIFY_NON_NULL(TAG, jsonObj, ERROR);
    }
    if (jsonObj)
    {
        outLen = 0;
        b64Ret = b64Decode(jsonObj->valuestring, strlen(jsonObj->valuestring), base64Buff,
              sizeof(base64Buff), &outLen);
        VERIFY_SUCCESS(TAG, ((b64Ret == B64_OK) && (outLen <= sizeof(doxm->owner.id))), ERROR);
        memcpy(doxm->owner.id, base64Buff, outLen);
    }

    ret = OC_STACK_OK;

exit:
    cJSON_Delete(jsonRoot);
    if (OC_STACK_OK != ret)
    {
        DeleteDoxmBinData(doxm);
        doxm = NULL;
    }

    return doxm;
}

OicSecPstat_t* JSONToPstatBin(const char * jsonStr)
{
    if(NULL == jsonStr)
    {
        return NULL;
    }

    OCStackResult ret = OC_STACK_ERROR;
    OicSecPstat_t *pstat = NULL;
    cJSON *jsonPstat = NULL;
    cJSON *jsonObj = NULL;

    unsigned char base64Buff[sizeof(((OicUuid_t*) 0)->id)] = {};
    uint32_t outLen = 0;
    B64Result b64Ret = B64_OK;

    cJSON *jsonRoot = cJSON_Parse(jsonStr);
    VERIFY_NON_NULL(TAG, jsonRoot, INFO);

    jsonPstat = cJSON_GetObjectItem(jsonRoot, OIC_JSON_PSTAT_NAME);
    VERIFY_NON_NULL(TAG, jsonPstat, INFO);

    pstat = (OicSecPstat_t*)OICCalloc(1, sizeof(OicSecPstat_t));
    VERIFY_NON_NULL(TAG, pstat, INFO);
    jsonObj = cJSON_GetObjectItem(jsonPstat, OIC_JSON_ISOP_NAME);
    VERIFY_NON_NULL(TAG, jsonObj, ERROR);
    VERIFY_SUCCESS(TAG, (cJSON_True == jsonObj->type || cJSON_False == jsonObj->type) , ERROR);
    pstat->isOp = jsonObj->valueint;

    jsonObj = cJSON_GetObjectItem(jsonPstat, OIC_JSON_DEVICE_ID_NAME);
    VERIFY_NON_NULL(TAG, jsonObj, ERROR);
    VERIFY_SUCCESS(TAG, cJSON_String == jsonObj->type, ERROR);
    b64Ret = b64Decode(jsonObj->valuestring, strlen(jsonObj->valuestring), base64Buff,
                  sizeof(base64Buff), &outLen);
    VERIFY_SUCCESS(TAG, (b64Ret == B64_OK && outLen <= sizeof(pstat->deviceID.id)), ERROR);
    memcpy(pstat->deviceID.id, base64Buff, outLen);

    jsonObj = cJSON_GetObjectItem(jsonPstat, OIC_JSON_COMMIT_HASH_NAME);
    VERIFY_NON_NULL(TAG, jsonObj, ERROR);
    VERIFY_SUCCESS(TAG, cJSON_Number == jsonObj->type, ERROR);
    pstat->commitHash  = jsonObj->valueint;

    jsonObj = cJSON_GetObjectItem(jsonPstat, OIC_JSON_CM_NAME);
    VERIFY_NON_NULL(TAG, jsonObj, ERROR);
    VERIFY_SUCCESS(TAG, cJSON_Number == jsonObj->type, ERROR);
    pstat->cm  = (OicSecDpm_t)jsonObj->valueint;

    jsonObj = cJSON_GetObjectItem(jsonPstat, OIC_JSON_OM_NAME);
    VERIFY_NON_NULL(TAG, jsonObj, ERROR);
    VERIFY_SUCCESS(TAG, cJSON_Number == jsonObj->type, ERROR);
    pstat->om  = (OicSecDpom_t)jsonObj->valueint;

    jsonObj = cJSON_GetObjectItem(jsonPstat, OIC_JSON_SM_NAME);
    VERIFY_NON_NULL(TAG, jsonObj, ERROR);
    if (cJSON_Array == jsonObj->type)
    {
        pstat->smLen = cJSON_GetArraySize(jsonObj);
        size_t idxx = 0;
        VERIFY_SUCCESS(TAG, pstat->smLen != 0, ERROR);
        pstat->sm = (OicSecDpom_t*)OICCalloc(pstat->smLen, sizeof(OicSecDpom_t));
        VERIFY_NON_NULL(TAG, pstat->sm, ERROR);
        do
        {
            cJSON *jsonSm = cJSON_GetArrayItem(jsonObj, idxx);
            VERIFY_NON_NULL(TAG, jsonSm, ERROR);
            pstat->sm[idxx] = (OicSecDpom_t)jsonSm->valueint;
        } while ( ++idxx < pstat->smLen);
    }
    ret = OC_STACK_OK;

exit:
    cJSON_Delete(jsonRoot);
    if (OC_STACK_OK != ret)
    {
        OIC_LOG(ERROR, TAG, "JSONToPstatBin failed");
    }
    return pstat;
}

OicSecCred_t * JSONToCredBin(const char * jsonStr)
{
    OCStackResult ret = OC_STACK_ERROR;
    OicSecCred_t * headCred = NULL;
    OicSecCred_t * prevCred = NULL;
    cJSON *jsonCredArray = NULL;

    cJSON *jsonRoot = cJSON_Parse(jsonStr);
    VERIFY_NON_NULL(TAG, jsonRoot, ERROR);

    jsonCredArray = cJSON_GetObjectItem(jsonRoot, OIC_JSON_CRED_NAME);
    VERIFY_NON_NULL(TAG, jsonCredArray, ERROR);
    if (cJSON_Array == jsonCredArray->type)
    {
        int numCred = cJSON_GetArraySize(jsonCredArray);
        VERIFY_SUCCESS(TAG, numCred > 0, ERROR);
        unsigned char base64Buff[sizeof(((OicUuid_t*)0)->id)] = {};
        uint32_t outLen = 0;
        B64Result b64Ret = B64_OK;
        int idx = 0;
        do
        {
            cJSON *jsonCred = cJSON_GetArrayItem(jsonCredArray, idx);
            VERIFY_NON_NULL(TAG, jsonCred, ERROR);

            OicSecCred_t *cred = (OicSecCred_t*)OICCalloc(1, sizeof(OicSecCred_t));
            VERIFY_NON_NULL(TAG, cred, ERROR);

            headCred = (headCred) ? headCred : cred;
            if (prevCred)
            {
                prevCred->next = cred;
            }
            size_t jsonObjLen = 0;
            cJSON *jsonObj = NULL;

            //CredId -- Mandatory
            jsonObj = cJSON_GetObjectItem(jsonCred, OIC_JSON_CREDID_NAME);
            if(jsonObj)
            {
                VERIFY_SUCCESS(TAG, cJSON_Number == jsonObj->type, ERROR);
                cred->credId = jsonObj->valueint;
            }

            //subject -- Mandatory
            jsonObj = cJSON_GetObjectItem(jsonCred, OIC_JSON_SUBJECT_NAME);
            VERIFY_NON_NULL(TAG, jsonObj, ERROR);
            VERIFY_SUCCESS(TAG, cJSON_String == jsonObj->type, ERROR);
            outLen = 0;
            memset(base64Buff, 0, sizeof(base64Buff));
            b64Ret = b64Decode(jsonObj->valuestring, strlen(jsonObj->valuestring),
                   base64Buff, sizeof(base64Buff), &outLen);
            VERIFY_SUCCESS(TAG, (b64Ret == B64_OK && outLen <= sizeof(cred->subject.id)),
                          ERROR);
            memcpy(cred->subject.id, base64Buff, outLen);

            //CredType -- Mandatory
            jsonObj = cJSON_GetObjectItem(jsonCred, OIC_JSON_CREDTYPE_NAME);
            VERIFY_NON_NULL(TAG, jsonObj, ERROR);
            VERIFY_SUCCESS(TAG, cJSON_Number == jsonObj->type, ERROR);
            cred->credType = (OicSecCredType_t)jsonObj->valueint;

            //PrivateData is mandatory for some of the credential types listed below.
            jsonObj = cJSON_GetObjectItem(jsonCred, OIC_JSON_PRIVATEDATA_NAME);
            if ((cred->credType & SYMMETRIC_PAIR_WISE_KEY) ||
                (cred->credType & SYMMETRIC_GROUP_KEY) ||
                (cred->credType & PIN_PASSWORD))
            {
                VERIFY_NON_NULL(TAG, jsonObj, ERROR);
                VERIFY_SUCCESS(TAG, cJSON_String == jsonObj->type, ERROR);
            }
#ifdef __WITH_X509__
            else if (cred->credType & SIGNED_ASYMMETRIC_KEY)
            {
                VERIFY_NON_NULL(TAG, jsonObj, ERROR);
                VERIFY_SUCCESS(TAG, cJSON_Object == jsonObj->type, ERROR);
            }
#endif //  __WITH_X509__
            if (NULL != jsonObj)
            {
                if (cJSON_String == jsonObj->type)
                {
                    jsonObjLen = strlen(jsonObj->valuestring) + 1;
                    cred->privateData.data = (uint8_t *)OICCalloc(1, jsonObjLen);
                    VERIFY_NON_NULL(TAG, (cred->privateData.data), ERROR);
                    memcpy(cred->privateData.data, jsonObj->valuestring, jsonObjLen);
                }
#ifdef __WITH_X509__
                else if (SIGNED_ASYMMETRIC_KEY == cred->credType && cJSON_Object == jsonObj->type)
                {
                    cred->privateData.data = cJSON_PrintUnformatted(jsonObj);
                    VERIFY_NON_NULL(TAG, (cred->privateData.data), ERROR);
                }
#endif // __WITH_X509__
            }

            //PublicData is mandatory only for SIGNED_ASYMMETRIC_KEY credentials type.
            jsonObj = cJSON_GetObjectItem(jsonCred, OIC_JSON_PUBLICDATA_NAME);
#ifdef __WITH_X509__
            if (cred->credType & SIGNED_ASYMMETRIC_KEY)
            {
                VERIFY_NON_NULL(TAG, jsonObj, ERROR);
                VERIFY_SUCCESS(TAG, cJSON_Object == jsonObj->type, ERROR);
            }
#endif //  __WITH_X509__
            if (NULL != jsonObj)
            {
                if (cJSON_String == jsonObj->type)
                {
                    jsonObjLen = strlen(jsonObj->valuestring) + 1;
                    cred->publicData.data = (uint8_t *)OICCalloc(1, jsonObjLen);
                    VERIFY_NON_NULL(TAG, (cred->publicData.data), ERROR);
                    memcpy(cred->publicData.data, jsonObj->valuestring, jsonObjLen);
                }
#ifdef __WITH_X509__
                else if (SIGNED_ASYMMETRIC_KEY == cred->credType && cJSON_Object == jsonObj->type)
                {
                    cred->publicData.data = cJSON_PrintUnformatted(jsonObj);
                    VERIFY_NON_NULL(TAG, (cred->publicData.data), ERROR);
                }
#endif //  __WITH_X509__
            }

            //Period -- Not Mandatory
            jsonObj = cJSON_GetObjectItem(jsonCred, OIC_JSON_PERIOD_NAME);
            if(jsonObj && cJSON_String == jsonObj->type)
            {
                jsonObjLen = strlen(jsonObj->valuestring) + 1;
                cred->period = (char *)OICMalloc(jsonObjLen);
                VERIFY_NON_NULL(TAG, cred->period, ERROR);
                strncpy(cred->period, jsonObj->valuestring, jsonObjLen);
            }

            //Owners -- Mandatory
            jsonObj = cJSON_GetObjectItem(jsonCred, OIC_JSON_OWNERS_NAME);
            VERIFY_NON_NULL(TAG, jsonObj, ERROR);
            VERIFY_SUCCESS(TAG, cJSON_Array == jsonObj->type, ERROR);
            cred->ownersLen = cJSON_GetArraySize(jsonObj);
            VERIFY_SUCCESS(TAG, cred->ownersLen > 0, ERROR);
            cred->owners = (OicUuid_t*)OICCalloc(cred->ownersLen, sizeof(OicUuid_t));
            VERIFY_NON_NULL(TAG, (cred->owners), ERROR);
            for(size_t i = 0; i < cred->ownersLen; i++)
            {
                cJSON *jsonOwnr = cJSON_GetArrayItem(jsonObj, i);
                VERIFY_NON_NULL(TAG, jsonOwnr, ERROR);
                VERIFY_SUCCESS(TAG, cJSON_String == jsonOwnr->type, ERROR);
                outLen = 0;
                memset(base64Buff, 0, sizeof(base64Buff));
                b64Ret = b64Decode(jsonOwnr->valuestring, strlen(jsonOwnr->valuestring),
                         base64Buff, sizeof(base64Buff), &outLen);
                VERIFY_SUCCESS(TAG, (b64Ret == B64_OK &&
                               outLen <= sizeof(cred->owners[i].id)), ERROR);
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

static OicSecSvc_t* JSONToSvcBin(const char * jsonStr)
{
    OCStackResult ret = OC_STACK_ERROR;
    OicSecSvc_t * headSvc = NULL;
    OicSecSvc_t * prevSvc = NULL;
    cJSON *jsonRoot = NULL;
    cJSON *jsonSvcArray = NULL;

    VERIFY_NON_NULL(TAG, jsonStr, ERROR);

    jsonRoot = cJSON_Parse(jsonStr);
    VERIFY_NON_NULL(TAG, jsonRoot, ERROR);

    jsonSvcArray = cJSON_GetObjectItem(jsonRoot, OIC_JSON_SVC_NAME);
    VERIFY_NON_NULL(TAG, jsonSvcArray, INFO);

    if (cJSON_Array == jsonSvcArray->type)
    {
        int numSvc = cJSON_GetArraySize(jsonSvcArray);
        int idx = 0;

        VERIFY_SUCCESS(TAG, numSvc > 0, INFO);
        do
        {
            cJSON *jsonSvc = cJSON_GetArrayItem(jsonSvcArray, idx);
            VERIFY_NON_NULL(TAG, jsonSvc, ERROR);

            OicSecSvc_t *svc = (OicSecSvc_t*)OICCalloc(1, sizeof(OicSecSvc_t));
            VERIFY_NON_NULL(TAG, svc, ERROR);

            headSvc = (headSvc) ? headSvc : svc;
            if (prevSvc)
            {
                prevSvc->next = svc;
            }

            cJSON *jsonObj = NULL;
            unsigned char base64Buff[sizeof(((OicUuid_t*)0)->id)] = {};
            uint32_t outLen = 0;
            B64Result b64Ret = B64_OK;

            // Service Device Identity
            jsonObj = cJSON_GetObjectItem(jsonSvc, OIC_JSON_SERVICE_DEVICE_ID);
            VERIFY_NON_NULL(TAG, jsonObj, ERROR);
            VERIFY_SUCCESS(TAG, cJSON_String == jsonObj->type, ERROR);
            outLen = 0;
            b64Ret = b64Decode(jsonObj->valuestring, strlen(jsonObj->valuestring), base64Buff,
                       sizeof(base64Buff), &outLen);
            VERIFY_SUCCESS(TAG, (b64Ret == B64_OK && outLen <= sizeof(svc->svcdid.id)), ERROR);
            memcpy(svc->svcdid.id, base64Buff, outLen);

            // Service Type
            jsonObj = cJSON_GetObjectItem(jsonSvc, OIC_JSON_SERVICE_TYPE);
            VERIFY_NON_NULL(TAG, jsonObj, ERROR);
            VERIFY_SUCCESS(TAG, cJSON_Number == jsonObj->type, ERROR);
            svc->svct = (OicSecSvcType_t)jsonObj->valueint;

            // Resource Owners
            jsonObj = cJSON_GetObjectItem(jsonSvc, OIC_JSON_OWNERS_NAME);
            VERIFY_NON_NULL(TAG, jsonObj, ERROR);
            VERIFY_SUCCESS(TAG, cJSON_Array == jsonObj->type, ERROR);

            svc->ownersLen = cJSON_GetArraySize(jsonObj);
            VERIFY_SUCCESS(TAG, svc->ownersLen > 0, ERROR);
            svc->owners = (OicUuid_t*)OICCalloc(svc->ownersLen, sizeof(OicUuid_t));
            VERIFY_NON_NULL(TAG, (svc->owners), ERROR);

            size_t idxx = 0;
            do
            {
                cJSON *jsonOwnr = cJSON_GetArrayItem(jsonObj, idxx);
                VERIFY_NON_NULL(TAG, jsonOwnr, ERROR);
                VERIFY_SUCCESS(TAG, cJSON_String == jsonOwnr->type, ERROR);
                outLen = 0;
                b64Ret = b64Decode(jsonOwnr->valuestring, strlen(jsonOwnr->valuestring), base64Buff,
                           sizeof(base64Buff), &outLen);

                VERIFY_SUCCESS(TAG, (b64Ret == B64_OK && outLen <= sizeof(svc->owners[idxx].id)),
                                   ERROR);
                memcpy(svc->owners[idxx].id, base64Buff, outLen);
            } while ( ++idxx < svc->ownersLen);

            prevSvc = svc;
        } while( ++idx < numSvc);
    }

    ret = OC_STACK_OK;

exit:
    cJSON_Delete(jsonRoot);
    if (OC_STACK_OK != ret)
    {
        DeleteSVCList(headSvc);
        headSvc = NULL;
    }
    return headSvc;
}

static OicSecAmacl_t* JSONToAmaclBin(const char * jsonStr)
{
    OCStackResult ret = OC_STACK_ERROR;
    OicSecAmacl_t * headAmacl = NULL;
    OicSecAmacl_t * prevAmacl = NULL;
    cJSON *jsonRoot = NULL;
    cJSON *jsonAmaclArray = NULL;

    VERIFY_NON_NULL(TAG, jsonStr, ERROR);

    jsonRoot = cJSON_Parse(jsonStr);
    VERIFY_NON_NULL(TAG, jsonRoot, ERROR);

    jsonAmaclArray = cJSON_GetObjectItem(jsonRoot, OIC_JSON_AMACL_NAME);
    VERIFY_NON_NULL(TAG, jsonAmaclArray, INFO);

    if (cJSON_Array == jsonAmaclArray->type)
    {
        int numAmacl = cJSON_GetArraySize(jsonAmaclArray);
        int idx = 0;

        VERIFY_SUCCESS(TAG, numAmacl > 0, INFO);
        do
        {
            cJSON *jsonAmacl = cJSON_GetArrayItem(jsonAmaclArray, idx);
            VERIFY_NON_NULL(TAG, jsonAmacl, ERROR);

            OicSecAmacl_t *amacl = (OicSecAmacl_t*)OICCalloc(1, sizeof(OicSecAmacl_t));
            VERIFY_NON_NULL(TAG, amacl, ERROR);

            headAmacl = (headAmacl) ? headAmacl : amacl;
            if (prevAmacl)
            {
                prevAmacl->next = amacl;
            }

            size_t jsonObjLen = 0;
            cJSON *jsonObj = NULL;

            // Resources -- Mandatory
            jsonObj = cJSON_GetObjectItem(jsonAmacl, OIC_JSON_RESOURCES_NAME);
            VERIFY_NON_NULL(TAG, jsonObj, ERROR);
            VERIFY_SUCCESS(TAG, cJSON_Array == jsonObj->type, ERROR);

            amacl->resourcesLen = cJSON_GetArraySize(jsonObj);
            VERIFY_SUCCESS(TAG, amacl->resourcesLen > 0, ERROR);
            amacl->resources = (char**)OICCalloc(amacl->resourcesLen, sizeof(char*));
            VERIFY_NON_NULL(TAG, (amacl->resources), ERROR);

            size_t idxx = 0;
            do
            {
                cJSON *jsonRsrc = cJSON_GetArrayItem(jsonObj, idxx);
                VERIFY_NON_NULL(TAG, jsonRsrc, ERROR);

                jsonObjLen = strlen(jsonRsrc->valuestring) + 1;
                amacl->resources[idxx] = (char*)OICMalloc(jsonObjLen);
                VERIFY_NON_NULL(TAG, (amacl->resources[idxx]), ERROR);
                OICStrcpy(amacl->resources[idxx], jsonObjLen, jsonRsrc->valuestring);
            } while ( ++idxx < amacl->resourcesLen);

            // Amss -- Mandatory
            VERIFY_SUCCESS( TAG, OC_STACK_OK == AddUuidArray(jsonAmacl, OIC_JSON_AMSS_NAME,
                               &(amacl->amssLen), &(amacl->amss)), ERROR);

            // Owners -- Mandatory
            VERIFY_SUCCESS( TAG, OC_STACK_OK == AddUuidArray(jsonAmacl, OIC_JSON_OWNERS_NAME,
                               &(amacl->ownersLen), &(amacl->owners)), ERROR);

            prevAmacl = amacl;
        } while( ++idx < numAmacl);
    }

    ret = OC_STACK_OK;

exit:
    cJSON_Delete(jsonRoot);
    if (OC_STACK_OK != ret)
    {
        DeleteAmaclList(headAmacl);
        headAmacl = NULL;
    }
    return headAmacl;
}

int main(int argc, char* argv[])
{
    if (argc == 3)
    {
        printf("JSON File Name: %s\n CBOR File Name: %s \n", argv[1], argv[2]);
        ConvertJsonToCBOR(argv[1], argv[2]);
    }
    else
    {
        printf("This program requires two inputs:\n");
        printf("1. First input is a json file tha will be converted to cbor. \n");
        printf("2. Second input is a resulting cbor file that will store converted cbor. \n");
        printf("\t json2cbor <json_file_name> <cbor_file_name>. \n");
    }
}
