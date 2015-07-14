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

#ifdef WITH_ARDUINO
#include <string.h>
#else
#include <strings.h>
#endif
#include <stdint.h>
#include <stdlib.h>
#include "ocstack.h"
#include "logger.h"
#include "oic_malloc.h"
#include "resourcemanager.h"
#include "psinterface.h"
#include "utlist.h"
#include "srmresourcestrings.h"
#include "credresource.h"
#include "ocrandom.h"
#include "doxmresource.h"
#include "srmutility.h"
#include "cainterface.h"
#include "pbkdf2.h"
#include "ocpayload.h"
#include "ocpayloadcbor.h"
#include "resourcemanager.h"
#include "payload_logging.h"

#define TAG  PCF("SRM-CREDL")

OicSecCred_t        *gCred = NULL;
static OCResourceHandle    gCredHandle = NULL;

/**
 * This function frees OicSecCred_t object's fields and object itself.
 */
static void FreeCred(OicSecCred_t *cred)
{
    if(NULL == cred)
    {
        OC_LOG (INFO, TAG, PCF("Invalid Parameter"));
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

/**
 * This function converts credential data into OCRepPayload.
 * Caller needs to invoke 'free' when done using
 * returned string.
 *
 * @param cred  pointer to instance of OicSecCred_t structure.
 *
 * @retval
 *      pointer to OCRepPayload credential representation - if credential for subjectId found
 *      NULL                                              - if credential for subjectId not found
 */
OCRepPayload* CredToPayload(const OicSecCred_t * cred)
{
    if(NULL == cred)
    {
        return NULL;
    }

    OCRepPayload* headPayload = NULL;
    OCRepPayload* prevPayload = NULL;
    size_t dimensions[MAX_REP_ARRAY_DEPTH] = {0};
    bool result;
    while (cred)
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
        if (memcmp(&(cred->subject), &WILDCARD_SUBJECT_ID, sizeof(OicUuid_t)) == 0)
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
                (int64_t*) cred->subject.id, dimensions);

        VERIFY_SUCCESS(TAG, result, ERROR);
        //CredID -- Mandatory
        result = OCRepPayloadSetPropInt(payload, OIC_JSON_CREDID_NAME, (int64_t) cred->credId);
        VERIFY_SUCCESS(TAG, result, ERROR);
        //CredType -- Mandatory
        result = OCRepPayloadSetPropInt(payload, OIC_JSON_CREDTYPE_NAME, (int64_t) cred->credType);
        VERIFY_SUCCESS(TAG, result, ERROR);
        //PrivateData -- Not Mandatory
        if (cred->privateData.data)
        {
            dimensions[0] = SVR_UUID_LENGTH;
            dimensions[1] = 0;
            dimensions[2] = 0;
            result = OCRepPayloadSetIntArray(payload, OIC_JSON_PRIVATEDATA_NAME,
                    (int64_t*) cred->privateData.data, dimensions);
            VERIFY_SUCCESS(TAG, result, ERROR);
        }
        //Period -- Not Mandatory
        if (cred->period)
        {
            result = OCRepPayloadSetPropString(payload, OIC_JSON_PERIOD_NAME, cred->period);
            VERIFY_SUCCESS(TAG, result, ERROR);
        }
        //Owners
        dimensions[0] = cred->ownersLen;
        dimensions[1] = SVR_UUID_LENGTH;
        dimensions[2] = 0;
        result = OCRepPayloadSetIntArray(payload, OIC_JSON_OWNERS_NAME, (int64_t*) &cred->owners,
                dimensions);
        VERIFY_SUCCESS(TAG, result, ERROR);
        cred = cred->next;
        prevPayload = payload;

    }
    result = OCRepPayloadSetUri(headPayload, OIC_RSRC_CRED_URI);
    VERIFY_SUCCESS(TAG, result, ERROR);
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
 * This function converts credential data from OCRepPayload.
 *
 * @param cred  payload to instance of OCRepPayload.
 *
 * @retval
 *      pointer to instance of OicSecCred_t structure - success
 *      NULL                                          - error
 */
OicSecCred_t* PayloadToCred(const OCRepPayload* payload)
{
    if(NULL == payload)
    {
        return NULL;
    }
    OC_LOG_PAYLOAD(INFO, TAG, (OCPayload*)payload);

    OicSecCred_t *cred = NULL;
    OicSecCred_t * headCred = NULL;
    OicSecCred_t * prevCred = NULL;
    size_t dimensions[MAX_REP_ARRAY_DEPTH] = {0};
    bool result = false;
    do
    {
        cred = (OicSecCred_t*) OICCalloc(1, sizeof(OicSecCred_t));
        VERIFY_NON_NULL(TAG, cred, ERROR);

        headCred = (headCred) ? headCred : cred;
        if (prevCred)
        {
            prevCred->next = cred;
        }
        int64_t *id;
        //CredId -- Mandatory
        result = OCRepPayloadGetPropInt(payload,
                OIC_JSON_CREDID_NAME,(int64_t*) &cred->credId);
        // Subject -- Mandatory
        result = OCRepPayloadGetIntArray(payload, OIC_JSON_SUBJECT_NAME, (int64_t**) &id,
                dimensions);
        if (result)
        {
            memcpy(&cred->subject.id, id, dimensions[0]);
        }
        OICFree(id);
        //CredType -- Mandatory
        result = OCRepPayloadGetPropInt(payload,
                OIC_JSON_CREDTYPE_NAME, (int64_t* )&cred->credType);
        //PrivateData is mandatory for some of the credential types listed below.
        result = OCRepPayloadGetIntArray(payload,
                OIC_JSON_PRIVATEDATA_NAME, (int64_t**) &cred->privateData.data,
                dimensions);

        // Owners -- Mandatory
        result = OCRepPayloadGetIntArray(payload,
                OIC_JSON_OWNERS_NAME, (int64_t**) &cred->owners, dimensions);
        cred->ownersLen = dimensions[0];
        payload = payload->next;
        prevCred = cred;
    }while(payload);

exit:
    if (!result)
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
 *        The privateData is expected in base64 encoded format.
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
    (void)publicData;
    OCStackResult ret = OC_STACK_ERROR;

    OicSecCred_t *cred = (OicSecCred_t*)OICCalloc(1, sizeof(OicSecCred_t));
    VERIFY_NON_NULL(TAG, cred, ERROR);

    //CredId is assigned before appending new cred to the existing
    //credential list and updating svr database in AddCredential().
    cred->credId = 0;

    VERIFY_NON_NULL(TAG, subject, ERROR);
    memcpy(cred->subject.id, subject->id , sizeof(cred->subject.id));

    VERIFY_SUCCESS(TAG, credType < (NO_SECURITY_MODE | SYMMETRIC_PAIR_WISE_KEY |
            SYMMETRIC_GROUP_KEY | ASYMMETRIC_KEY | SIGNED_ASYMMETRIC_KEY | PIN_PASSWORD), ERROR);
    cred->credType = credType;

    if(privateData)
    {
        cred->privateData.data = (char *)OICMalloc(OWNER_PSK_LENGTH_128);
        VERIFY_NON_NULL(TAG, cred->privateData.data, ERROR);
        memcpy((char *)cred->privateData.data, privateData, OWNER_PSK_LENGTH_128);
    }

    VERIFY_SUCCESS(TAG, ownersLen > 0, ERROR);
    cred->ownersLen = ownersLen;

    cred->owners = (OicUuid_t*)OICCalloc(cred->ownersLen, sizeof(OicUuid_t));
    VERIFY_NON_NULL(TAG, cred->owners, ERROR);
    for(size_t i = 0; i < cred->ownersLen; i++)
    {
        memcpy(cred->owners[i].id, owners[i].id, sizeof(cred->owners[i].id));
    }
    //All is good
    ret = OC_STACK_OK;

exit:
    if (OC_STACK_OK != ret)
    {
        DeleteCredList(cred);
        cred = NULL;
    }
    return cred;
}

static bool UpdatePersistentStorage(const OicSecCred_t * cred)
{
    bool ret = false;
    (void) cred;
    // Convert Cred data into JSON for update to persistent storage
/*
 * TODO: CBOR
    char *jsonStr = BinToCredJSON(cred);
    if (jsonStr)
    {
        cJSON *jsonCred = cJSON_Parse(jsonStr);
        OICFree(jsonStr);

        if ((jsonCred) &&
          (OC_STACK_OK == UpdateSVRDatabase(OIC_JSON_CRED_NAME, jsonCred)))
        {
            ret = true;
        }
        cJSON_Delete(jsonCred );
    }
    else //Empty cred list
    {
        if (OC_STACK_OK == UpdateSVRDatabase(OIC_JSON_CRED_NAME, NULL))
        {
            ret = true;
        }
    }
*/
    return ret;
}

/**
 * Compare function used LL_SORT for sorting credentials
 *
 * @param first   pointer to OicSecCred_t struct
 * @param second  pointer to OicSecCred_t struct
 *
 *@retval
 *  -1    if credId of first is less than credId of second
 *   0    if credId of first is equal to credId of second
 *   1    if credId of first is greater than credId of second
 */
static int CmpCredId(const OicSecCred_t * first, const OicSecCred_t *second)
{
    if(first->credId < second->credId)
    {
        return -1;
    }
    else if(first->credId > second->credId)
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
 * @retval
 *      next available credId  - success
 *      0                      - error
 */

static uint16_t GetCredId()
{
    //Sorts credential list in incremental order of credId
    LL_SORT(gCred, CmpCredId);


    OicSecCred_t *currentCred = NULL, *credTmp = NULL;
    uint16_t nextCredId = 1;

    LL_FOREACH_SAFE(gCred, currentCred, credTmp)
    {
        if(currentCred->credId == nextCredId)
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

    VERIFY_SUCCESS(TAG, NULL != newCred, ERROR);

    //Assigning credId to the newCred
    newCred->credId = GetCredId();

    VERIFY_SUCCESS(TAG, newCred->credId != 0, ERROR);

    //Append the new Cred to existing list
    LL_APPEND(gCred, newCred);
    // convert data and update it into persistent storage
    ret = UpdateSVRData();
    if (OC_STACK_OK != ret)
    {
        OC_LOG (ERROR, TAG, PCF("ConvertSVRData Failed"));
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
        // convert data and update it into persistent storage
        ret = UpdateSVRData();
        if (OC_STACK_OK != ret)
        {
            OC_LOG (ERROR, TAG, PCF("ConvertSVRData Failed"));
        }
    }
    if(deleteFlag)
    {
        if(UpdatePersistentStorage(gCred))
        {
            ret = OC_STACK_RESOURCE_DELETED;
        }
    }
    return ret;

}

OCEntityHandlerResult HandlePostRequest(const OCEntityHandlerRequest * ehRequest)
{
    OC_LOG (INFO, TAG, PCF("HandlePostRequest processing the request"));
    OCEntityHandlerResult ret = OC_EH_ERROR;
    if(!ehRequest)
    {
        return ret;
    }
    //Get binary representation of json
    OCRepPayload* payload = (OCRepPayload*)ehRequest->payload;
    OicSecCred_t * cred = PayloadToCred(payload);

    if(cred)
    {
        //If the Post request credential has credId, it will be
        //discarded and the next available credId will be assigned
        //to it before getting appended to the existing credential
        //list and updating svr database.
        ret = (OC_STACK_OK == AddCredential(cred))? OC_EH_RESOURCE_CREATED : OC_EH_ERROR;
    }

    return ret;
}

OCEntityHandlerResult HandleDeleteRequest(const OCEntityHandlerRequest *ehRequest)
{
    OC_LOG_V (INFO, TAG, PCF("Processing CredDeleteRequest"));

    OCEntityHandlerResult ehRet = OC_EH_ERROR;

   if(!ehRequest && NULL == ehRequest->query)
   {
       return ehRet;
   }

   OicParseQueryIter_t parseIter = {.attrPos=NULL};
   OicUuid_t subject = {.id={0}};

   //Parsing REST query to get the subject
   ParseQueryIterInit((unsigned char *)ehRequest->query, &parseIter);
   while(GetNextQuery(&parseIter))
   {
       if(strncasecmp((char *)parseIter.attrPos, OIC_JSON_SUBJECT_NAME,
               parseIter.attrLen) == 0)
       {
/*
 * TODO: CBOR
           unsigned char base64Buff[sizeof(((OicUuid_t*)0)->id)] = {};
           uint32_t outLen = 0;
           B64Result b64Ret = B64_OK;

           b64Ret = b64Decode((char *)parseIter.valPos, parseIter.valLen,
                   base64Buff, sizeof(base64Buff), &outLen);

           VERIFY_SUCCESS(TAG, (b64Ret == B64_OK && outLen <= sizeof(subject.id)), ERROR);
           memcpy(subject.id, base64Buff, outLen);
*/
       }
   }

   if(OC_STACK_RESOURCE_DELETED == RemoveCredential(&subject))
   {
       ehRet = OC_EH_RESOURCE_DELETED;
   }

exit:
    return ehRet;
}


/*
 * This internal method is the entity handler for Cred resources
 * to handle REST request (PUT/POST/DEL)
 */
OCEntityHandlerResult CredEntityHandler (OCEntityHandlerFlag flag,
                                        OCEntityHandlerRequest * ehRequest,
                                        void* callbackParameter)
{
    (void)callbackParameter;
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
    ret = (SendSRMResponse(ehRequest, ret, NULL) == OC_STACK_OK ?
                       ret : OC_EH_ERROR);

    return ret;
}

/*
 * This internal method is used to create '/oic/sec/Cred' resource.
 */
OCStackResult CreateCredResource()
{
    OCStackResult ret;

    ret = OCCreateResource(&gCredHandle,
                           OIC_RSRC_TYPE_SEC_CRED,
                           OIC_MI_DEF,
                           OIC_RSRC_CRED_URI,
                           CredEntityHandler,
                           NULL,
                           OC_RES_PROP_NONE);

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
OicSecCred_t* GetCredDefault()
{
    return NULL;
}

/**
 * This method converts SVR buffers into OCRepPayload and updates the persistent storage.
 *
 * @param[out] payload is a pointer of CBOR cred payload.
 * @param[out] size is CBOR cred payload size.
 * @retval  OC_STACK_OK for Success, otherwise some error value
 */
OCStackResult ConvertCredData(uint8_t **payload,  size_t *size)
{
    OCStackResult result = OC_STACK_ERROR;
    VERIFY_NON_NULL(TAG, payload, ERROR);
    VERIFY_NON_NULL(TAG, size, ERROR);

    if (gCred)
    {
        OCRepPayload* rePayload = CredToPayload(gCred);
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
 * @param[in] payload is a pointer of CBOR cred payload.
 * @param[in] size is CBOR cred payload size.
 * @retval  OC_STACK_OK for Success, otherwise some error value
 */
OCStackResult ParseCredPayload(uint8_t *payload,  size_t size)
{
    OCStackResult result = OC_STACK_ERROR;
    VERIFY_NON_NULL(TAG, payload, ERROR);
    if (size)
    {
        OCPayload* outPayload = NULL;
        result = OCParsePayload(&outPayload, payload, size);
        if (result == OC_STACK_OK)
        {
            gCred = PayloadToCred((const OCRepPayload*) outPayload);
            OCPayloadDestroy(outPayload);
        }
    }
exit:
    return result;
}

/**
 * Initialize Cred resource by loading data from persistent storage.
 *
 * @param[in] payload CBOR cred payload pointers.
 * @param[in] size size of CBOR cred payload.
 * @retval
 *     OC_STACK_OK    - no errors
 *     OC_STACK_ERROR - stack process error
 */
OCStackResult InitCredResource(uint8_t *payload, size_t size)
{
    OCStackResult ret = OC_STACK_ERROR;
    VERIFY_NON_NULL(TAG, payload, ERROR);

    ret = ParseCredPayload(payload, size);
    if (ret != OC_STACK_OK)
    {
        OC_LOG (ERROR, TAG, PCF("ParseCredPayload failed"));
    }

    /*
     * If SVR database in persistent storage got corrupted or
     * is not available for some reason, a default Cred is created
     * which allows user to initiate Cred provisioning again.
     */
    if (!gCred)
    {
        gCred = GetCredDefault();
    }
    //Instantiate 'oic.sec.cred'
    ret = CreateCredResource();
exit:
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
        if(memcmp(cred->subject.id, subject->id, sizeof(subject->id)) == 0)
        {
            return cred;
        }
    }
    return NULL;
}


#if defined(__WITH_DTLS__)
/**
 * This internal callback is used by lower stack (i.e. CA layer) to
 * retrieve PSK credentials from RI security layer.
 *
 * Note: When finished, caller should initialize memory to zeros and
 * invoke OICFree to delete @p credInfo.
 *
 * @param credInfo
 *     binary blob containing PSK credentials
 *
 * @retval none
 */
void GetDtlsPskCredentials(CADtlsPskCredsBlob_t **credInfo)
{
    CADtlsPskCredsBlob_t * caBlob = NULL;
    if(credInfo)
    {
        caBlob = (CADtlsPskCredsBlob_t *)OICCalloc(sizeof(CADtlsPskCredsBlob_t), 1);
        if (caBlob)
        {
            OicUuid_t deviceID = {.id={}};

            // Retrieve Device ID from doxm resource and copy in PSK creds blob
            VERIFY_SUCCESS(TAG, GetDoxmDeviceID(&deviceID) == OC_STACK_OK, ERROR);
            memcpy(caBlob->identity, deviceID.id, sizeof(caBlob->identity));

            OicSecCred_t *cred = NULL;
            size_t count = 0;
            LL_FOREACH(gCred, cred)
            {
                // Currently, Iotivity supports only symmetric pair wise key credentials
                if (cred->credType == SYMMETRIC_PAIR_WISE_KEY)
                {
                    ++count;
                }
            }
            caBlob->num = count;
            if (caBlob->num)
            {
                caBlob->creds =
                    (OCDtlsPskCreds*) OICMalloc(caBlob->num * sizeof(OCDtlsPskCreds));
                VERIFY_NON_NULL(TAG, caBlob->creds, ERROR);

                unsigned int i = 0;
                LL_FOREACH(gCred, cred)
                {
                    if ((cred->credType == SYMMETRIC_PAIR_WISE_KEY) &&
                            (i < count))

                    {
                        // Copy subject ID
                        memcpy(caBlob->creds[i].id, cred->subject.id,
                                sizeof(caBlob->creds[i].id));

                        // Copy PSK
                        if(cred->privateData.data)
                        {
                            memcpy(caBlob->creds[i].psk, cred->privateData.data,
                                    sizeof(caBlob->creds[i].psk));
                        }
                        i++;
                    }
                }
            }
        }
        *credInfo = caBlob;
        // Return from here after making the credential list
        return;
    }

exit:
    if (caBlob && caBlob->creds)
    {
        memset(caBlob->creds, 0, caBlob->num * sizeof(OCDtlsPskCreds));
        OICFree(caBlob->creds);
    }
    OICFree(caBlob);
}

/**
 * Add temporal PSK to PIN based OxM
 *
 * @param[in] tmpSubject UUID of target device
 * @param[in] credType Type of credential to be added
 * @param[in] pin numeric characters
 * @param[in] pinSize length of 'pin'
 * @param[in] ownersLen Number of owners
 * @param[in] owners Array of owners
 * @param[out] tmpCredSubject Generated credential's subject.
 *
 * @return OC_STACK_OK for success and errorcode otherwise.
 */
OCStackResult AddTmpPskWithPIN(const OicUuid_t* tmpSubject, OicSecCredType_t credType,
                            const char * pin, size_t pinSize,
                            size_t ownersLen, const OicUuid_t * owners, OicUuid_t* tmpCredSubject)
{
    OCStackResult ret = OC_STACK_ERROR;

    VERIFY_NON_NULL(TAG, tmpSubject, ERROR);
    VERIFY_NON_NULL(TAG, pin, ERROR);
    VERIFY_NON_NULL(TAG, tmpCredSubject, ERROR);

    if(pinSize == 0)
    {
        return OC_STACK_INVALID_PARAM;
    }

/*
 * TODO: CBOR
    uint8_t privData[OWNER_PSK_LENGTH_128] = {0,};
    int dtlsRes = DeriveCryptoKeyFromPassword((const unsigned char *)pin, pinSize, owners->id,
                                              UUID_LENGTH, PBKDF_ITERATIONS,
                                              OWNER_PSK_LENGTH_128, privData);
    VERIFY_SUCCESS(TAG, (dtlsRes == 0) , ERROR);

    uint32_t outLen = 0;
    char base64Buff[B64ENCODE_OUT_SAFESIZE(OWNER_PSK_LENGTH_128) + 1] = {};
    B64Result b64Ret = b64Encode(privData, OWNER_PSK_LENGTH_128, base64Buff,
                                sizeof(base64Buff), &outLen);
    VERIFY_SUCCESS(TAG, (B64_OK == b64Ret), ERROR);

    OicSecCred_t* cred = GenerateCredential(tmpSubject, credType, NULL,
                                            base64Buff, ownersLen, owners);
    if(NULL == cred)
    {
        OC_LOG(ERROR, TAG, "GeneratePskWithPIN() : Failed to generate credential");
        return OC_STACK_ERROR;
    }

    memcpy(tmpCredSubject->id, cred->subject.id, UUID_LENGTH);

    ret = AddCredential(cred);
*/
    if( OC_STACK_OK != ret)
    {
        OC_LOG(ERROR, TAG, "GeneratePskWithPIN() : Failed to add credential");
    }

exit:
    return ret;
}

#endif /* __WITH_DTLS__ */
