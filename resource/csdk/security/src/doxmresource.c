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

#if HAVE_STRINGS_H
#include <strings.h>
#endif

#include <stdlib.h>
#include <string.h>
#include "ocstack.h"
#include "logger.h"
#include "oic_malloc.h"
#include "cJSON.h"
#include "resourcemanager.h"
#include "doxmresource.h"
#include "psinterface.h"
#include "utlist.h"
#include "srmresourcestrings.h"
#include "securevirtualresourcetypes.h"
#include "ocrandom.h"
#include "cainterface.h"
#include "credresource.h"
#include "ocserverrequest.h"
#include "srmutility.h"
#include "pinoxmcommon.h"
#include "resourcemanager.h"
#include "payload_logging.h"

#ifdef __WITH_DTLS__
#include "global.h"
#endif

#include "ocpayload.h"
#include "ocpayloadcbor.h"

#define TAG  PCF("SRM-DOXM")

OicSecDoxm_t *gDoxm = NULL;
static OCResourceHandle gDoxmHandle = NULL;

static OicSecOxm_t gOicSecDoxmJustWorks = OIC_JUST_WORKS;
static OicSecDoxm_t gDefaultDoxm =
{
    NULL,                   /* OicUrn_t *oxmType */
    0,                      /* size_t oxmTypeLen */
    &gOicSecDoxmJustWorks,  /* uint16_t *oxm */
    1,                      /* size_t oxmLen */
    OIC_JUST_WORKS,         /* uint16_t oxmSel */
    false,                  /* bool owned */
    {.id = {0}},            /* OicUuid_t deviceID */
    {.id = {0}},            /* OicUuid_t owner */
};

void DeleteDoxmBinData(OicSecDoxm_t* doxm)
{
    if (doxm)
    {
        //Clean oxmType
        for (size_t i = 0; i < doxm->oxmTypeLen; i++)
        {
            OICFree(doxm->oxmType[i]);
        }
        OICFree(doxm->oxmType);

        //clean oxm
        OICFree(doxm->oxm);

        //Clean doxm itself
        OICFree(doxm);
    }
}

/**
 * This method converts DOXM data into OCRepPayload.
 * Caller needs to invoke 'free' when finished done using
 * return string
 */
OCRepPayload* DoxmToPayload(const OicSecDoxm_t * doxm)
{
    size_t dimensions[MAX_REP_ARRAY_DEPTH] = {};

    if(NULL == doxm)
    {
        return NULL;
    }
    OCRepPayload* payload = OCRepPayloadCreate();
    if(!payload)
    {
        OC_LOG(ERROR, TAG, PCF("Failed to allocate Payload"));
        return NULL;
    }
    bool result = OCRepPayloadSetUri(payload, OIC_RSRC_DOXM_URI);
    VERIFY_SUCCESS(TAG, result, ERROR);
    //OxmType -- Not Mandatory
    dimensions[0] = doxm->oxmTypeLen;
    if (doxm->oxmTypeLen)
    {
        result = OCRepPayloadSetStringArray(payload,
                OIC_JSON_OXM_TYPE_NAME, (const char**)doxm->oxmType, dimensions);
        VERIFY_SUCCESS(TAG, result, ERROR);
    }
    //Oxm -- Not Mandatory
    dimensions[0] = doxm->oxmLen;
    result = OCRepPayloadSetIntArray(payload, OIC_JSON_OXM_NAME,
            (int64_t*)doxm->oxm, dimensions);
    VERIFY_SUCCESS(TAG, result, ERROR);
    //OxmSel -- Mandatory
    result = OCRepPayloadSetPropInt(payload, OIC_JSON_OXM_SEL_NAME, doxm->oxmSel);
    VERIFY_SUCCESS(TAG, result, ERROR);
    //Owned -- Mandatory
    result = OCRepPayloadSetPropBool(payload, OIC_JSON_OWNED_NAME, doxm->owned);
    VERIFY_SUCCESS(TAG, result, ERROR);
    //DeviceId -- Mandatory
    dimensions[0] = SVR_UUID_LENGTH;
    result = OCRepPayloadSetIntArray(payload, OIC_JSON_DEVICE_ID_NAME,
            (int64_t*)doxm->deviceID.id, dimensions);
    VERIFY_SUCCESS(TAG, result, ERROR);
    //Owner -- Mandatory
    dimensions[0] = SVR_UUID_LENGTH;
    result = OCRepPayloadSetIntArray(payload, OIC_JSON_OWNER_NAME,
            (int64_t*)doxm->owner.id, dimensions);
    VERIFY_SUCCESS(TAG, result, ERROR);
    OC_LOG_PAYLOAD(INFO, TAG, (OCPayload*)payload);

exit:
    if(!result)
    {
        OCRepPayloadDestroy(payload);
        payload = NULL;
    }
    return payload;
}

/**
 * This method converts OCRepPayload into binary doxm.
 * The OCRepPayload can be from persistent database or
 * or received as PUT/POST request.
 */
OicSecDoxm_t* PayloadToDoxm(const OCRepPayload* payload)
{
    if(NULL == payload)
    {
        return NULL;
    }
    OC_LOG_PAYLOAD(INFO, TAG, (OCPayload*)payload);
    OicSecDoxm_t *doxm = NULL;
    size_t dimensions[MAX_REP_ARRAY_DEPTH]={};
    bool result = false;
    doxm = (OicSecDoxm_t*) OICCalloc(1, sizeof(OicSecDoxm_t));
    VERIFY_NON_NULL(TAG, doxm, INFO);
    //OxmType
    result = OCRepPayloadGetStringArray(payload, OIC_JSON_OXM_TYPE_NAME, &doxm->oxmType,
            dimensions);
    doxm->oxmTypeLen = result ? dimensions[0] : 0;
    //Oxm -- Not Mandatory
    result = OCRepPayloadGetIntArray(payload, OIC_JSON_OXM_NAME, (int64_t**) &doxm->oxm,
            dimensions);
    doxm->oxmLen = result ? dimensions[0] : 0;
     //OxmSel -- Mandatory
    result = OCRepPayloadGetPropInt(payload, OIC_JSON_OXM_SEL_NAME, (int64_t*) &doxm->oxmSel);
    VERIFY_SUCCESS(TAG, result, ERROR);
    //Owned -- Mandatory
    result = OCRepPayloadGetPropBool(payload, OIC_JSON_OWNED_NAME, &doxm->owned);
    VERIFY_SUCCESS(TAG, result, ERROR);
    //DeviceId -- Mandatory
    int64_t *id;
    result = OCRepPayloadGetIntArray(payload, OIC_JSON_DEVICE_ID_NAME, (int64_t**) &id, dimensions);
    VERIFY_SUCCESS(TAG, result, ERROR);
    memcpy(&doxm->deviceID.id, id, dimensions[0]);
    OC_LOG(ERROR, TAG, PCF("Device ID:"));
    OICFree(id);
    //Owner -- Mandatory
    result = OCRepPayloadGetIntArray(payload, OIC_JSON_OWNER_NAME, (int64_t**) &id, dimensions);
    VERIFY_SUCCESS(TAG, result, ERROR);
    memcpy(&doxm->owner.id, id, dimensions[0]);
    OC_LOG(ERROR, TAG, PCF("owner ID:"));
    OICFree(id);

exit:
    if(!result)
    {
        DeleteDoxmBinData(doxm);
        doxm = NULL;
    }
    return doxm;
}

/**
 * @todo document this function including why code might need to call this.
 * The current suspicion is that it's not being called as much as it should.
 */
static bool UpdatePersistentStorage(OicSecDoxm_t * doxm)
{
    bool bRet = false;

    if (NULL != doxm)
    {
        // TODO:Convert Doxm data into JSON for update to persistent storage
     }

    return bRet;
}

static bool ValidateQuery(unsigned char * query)
{
    // Send doxm resource data if the state of doxm resource
    // matches with the query parameters.
    // else send doxm resource data as NULL
    // TODO Remove this check and rely on Policy Engine
    // and Provisioning Mode to enforce provisioning-state
    // access rules. Eventually, the PE and PM code will
    // not send a request to the /doxm Entity Handler at all
    // if it should not respond.
    OC_LOG (INFO, TAG, PCF("In ValidateQuery"));
    if(NULL == gDoxm)
    {
        return false;
    }

    OicParseQueryIter_t parseIter = {.attrPos = NULL};

    ParseQueryIterInit(query, &parseIter);

    while(GetNextQuery(&parseIter))
    {
        if(strncasecmp((char *)parseIter.attrPos, OIC_JSON_OWNED_NAME, parseIter.attrLen) == 0)
        {
            if((strncasecmp((char *)parseIter.valPos, OIC_SEC_TRUE, parseIter.valLen) == 0) &&
                    (gDoxm->owned))
            {
                return true;
            }
            else if((strncasecmp((char *)parseIter.valPos, OIC_SEC_FALSE, parseIter.valLen) == 0)
                    && (!gDoxm->owned))
            {
                return true;
            }
        }
    }
    return false;
}

static OCEntityHandlerResult HandleDoxmGetRequest (const OCEntityHandlerRequest * ehRequest)
{
    if(!ehRequest)
    {
        return OC_EH_ERROR;
    }
    char* jsonStr = NULL;
    OCEntityHandlerResult ehRet = OC_EH_OK;

    OC_LOG (INFO, TAG, PCF("Doxm EntityHandle processing GET request"));

    //Checking if Get request is a query.
    if(ehRequest->query)
    {
        OC_LOG (INFO, TAG, PCF("HandleDoxmGetRequest processing query"));
        if(!ValidateQuery((unsigned char *)ehRequest->query))
        {
            ehRet = OC_EH_ERROR;
        }
    }

    /*
     * For GET or Valid Query request return doxm resource json payload.
     * For non-valid query return NULL json payload.
     * A device will 'always' have a default Doxm, so BinToDoxmJSON will
     * return valid doxm resource json.
     */
    OCRepPayload* payload = NULL;
    if (OC_EH_OK == ehRet)
    {
        // Convert doxm data into OCRepPayload.
        payload = DoxmToPayload(gDoxm);
        // A device should always have a default doxm. Therefore, payload should never be NULL.
        ehRet = (payload ? OC_EH_OK : OC_EH_ERROR);
    }
    // Send response payload to request originator
    SendSRMResponse(ehRequest, ehRet, payload);

    OICFree(jsonStr);

    return ehRet;
}

#ifdef __WITH_DTLS__
/*
 * Generating new credential for provisioning tool
 *
 * PSK generated by
 */
static OCEntityHandlerResult AddOwnerPSK(const CAEndpoint_t* endpoint,
                    OicSecDoxm_t* ptDoxm,
                    const uint8_t* label, const size_t labelLen)
{
    size_t ownLen = 1;
    uint32_t outLen = 0;
    OicSecCred_t *cred = NULL;
    uint8_t ownerPSK[OWNER_PSK_LENGTH_128] = {};

    CAResult_t pskRet = CAGenerateOwnerPSK(endpoint,
        label, labelLen,
        ptDoxm->owner.id, sizeof(ptDoxm->owner.id),
        gDoxm->deviceID.id, sizeof(gDoxm->deviceID.id),
        ownerPSK, OWNER_PSK_LENGTH_128);

    VERIFY_SUCCESS(TAG, pskRet == CA_STATUS_OK, ERROR);

/*
 * TODO: CBOR
    char base64Buff[B64ENCODE_OUT_SAFESIZE(OWNER_PSK_LENGTH_128) + 1] = {};
    B64Result b64Ret = b64Encode(ownerPSK, OWNER_PSK_LENGTH_128, base64Buff,
                    sizeof(base64Buff), &outLen);
    VERIFY_SUCCESS(TAG, b64Ret == B64_OK, ERROR);

    OC_LOG (INFO, TAG, PCF("Doxm EntityHandle  generating Credential"));
    cred = GenerateCredential(&ptDoxm->owner, SYMMETRIC_PAIR_WISE_KEY,
                              NULL, base64Buff, ownLen, &ptDoxm->owner);
    VERIFY_NON_NULL(TAG, cred, ERROR);
*/

    //Adding provisioning tool credential to cred Resource.
    VERIFY_SUCCESS(TAG, OC_STACK_OK == AddCredential(cred), ERROR);

    gDoxm->owned = true;
    gDoxm->oxmSel = ptDoxm->oxmSel;
    memcpy(&(gDoxm->owner), &(ptDoxm->owner), sizeof(OicUuid_t));

    return OC_EH_OK;

exit:
    return OC_EH_ERROR;
}
#endif //__WITH_DTLS__

static OCEntityHandlerResult HandleDoxmPutRequest (const OCEntityHandlerRequest * ehRequest)
{
    OCEntityHandlerResult ehRet = OC_EH_ERROR;
    OC_LOG (INFO, TAG, PCF("Doxm EntityHandle processing PUT request"));
    if(!ehRequest)
    {
        return OC_EH_ERROR;
    }
    OicUuid_t emptyOwner = {};
    OicSecDoxm_t* newDoxm = NULL;
    if (ehRequest->resource)
    {
        OCRepPayload* payload = ( OCRepPayload*)ehRequest->payload;
        if (payload)
        {
            newDoxm = PayloadToDoxm(payload);
            VERIFY_NON_NULL(TAG, newDoxm, ERROR);
            // Iotivity SRM ONLY supports OIC_JUST_WORKS now
            if (OIC_JUST_WORKS == newDoxm->oxmSel)
            {
                /*
                 * If current state of the device is un-owned, enable
                 * anonymous ECDH cipher in tinyDTLS so that Provisioning
                 * tool can initiate JUST_WORKS ownership transfer process.
                 */
                if ((false == gDoxm->owned) && (false == newDoxm->owned))
                {
                    OC_LOG (INFO, TAG, PCF("Doxm EntityHandle  enabling AnonECDHCipherSuite"));
    #ifdef __WITH_DTLS__
                    ehRet = (CAEnableAnonECDHCipherSuite(true) == CA_STATUS_OK) ? OC_EH_OK : OC_EH_ERROR;
    #endif //__WITH_DTLS__
                    goto exit;
                }
            }

            /*
             * When current state of the device is un-owned and Provisioning
             * Tool is attempting to change the state to 'Owned' with a
             * qualified value for the field 'Owner'
             */
            if ((false == gDoxm->owned) && (true == newDoxm->owned) &&
                (memcmp(&(newDoxm->owner), &emptyOwner, sizeof(OicUuid_t)) != 0))
            {
                /*
                 * Generate OwnerPSK and create credential for Provisioning
                 * tool with the generated OwnerPSK.
                 * Update persistent storage and disable anonymous ECDH cipher
                 *
                 */
#ifdef __WITH_DTLS__
                OCServerRequest *request = (OCServerRequest *)ehRequest->requestHandle;

                //Generating OwnerPSK
                OC_LOG (INFO, TAG, PCF("Doxm EntityHandle  generating OwnerPSK"));
                int len = strlen(OXM_JUST_WORKS);
 /*     TODO: CBOR
  *     CAResult_t pskRet = CAGenerateOwnerPSK((CAEndpoint_t *)&request->devAddr,
                        (uint8_t*) OXM_JUST_WORKS, len,
                        newDoxm->owner.id, sizeof(newDoxm->owner.id),
                        gDoxm->deviceID.id, sizeof(gDoxm->deviceID.id),
                        ownerPSK, OWNER_PSK_LENGTH_128);

                VERIFY_SUCCESS(TAG, pskRet == CA_STATUS_OK, ERROR);
*/
                //Generate new credential for provisioning tool
                ehRet = AddOwnerPSK((CAEndpoint_t *)&request->devAddr, newDoxm,
                        (uint8_t*) OXM_JUST_WORKS, strlen(OXM_JUST_WORKS));

                VERIFY_SUCCESS(TAG, OC_EH_OK == ehRet, ERROR);

                // convert data and update it into persistent storage
                if (OC_STACK_OK == UpdateSVRData())
                {
                    ehRet = OC_EH_OK;
                }
                else
                {
                    ehRet = OC_EH_ERROR;

                    /*
                     * If persistent storage update failed, revert back the state
                     * for global variable.
                     */
                    gDoxm->owned = false;
                    gDoxm->oxmSel = 0;
                    memset(&(gDoxm->owner), 0, sizeof(OicUuid_t));
                }

                /*
                 * Disable anonymous ECDH cipher in tinyDTLS since device is now
                 * in owned state.
                 */
                CAEnableAnonECDHCipherSuite(false);
#endif //__WITH_DTLS__
            }

        }
        else if(OIC_RANDOM_DEVICE_PIN == newDoxm->oxmSel)
        {
            //this temp Credential ID is used to track temporal Cred Id
            static OicUuid_t tmpCredId = {.id={0}};
            static bool tmpCredGenFlag = false;

            if ((false == gDoxm->owned) && (false == newDoxm->owned))
            {
#ifdef __WITH_DTLS__
                CAEnableAnonECDHCipherSuite(false);
                OC_LOG(DEBUG, TAG, "ECDH_ANON CipherSuite is DISABLED");
                CASelectCipherSuite(TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA_256);

                char ranPin[OXM_RANDOM_PIN_SIZE + 1] = {0,};
                if(OC_STACK_OK == GeneratePin(ranPin, OXM_RANDOM_PIN_SIZE + 1))
                {
                    if(tmpCredGenFlag)
                    {
                       OC_LOG(DEBUG, TAG, "Corrupted PSK is detected!!!");
                       VERIFY_SUCCESS(TAG,
                                      OC_STACK_RESOURCE_DELETED == RemoveCredential(&tmpCredId),
                                      ERROR);
                    }

                    OCStackResult res = AddTmpPskWithPIN( &(newDoxm->owner), SYMMETRIC_PAIR_WISE_KEY,
                                     ranPin, OXM_RANDOM_PIN_SIZE, 1, &(newDoxm->owner), &tmpCredId);
                    VERIFY_SUCCESS(TAG, res == OC_STACK_OK, ERROR);
                    tmpCredGenFlag = true;
                    ehRet = OC_EH_OK;
                }
                else
                {
                    OC_LOG(ERROR, TAG, "Failed to generate random PIN");
                    ehRet = OC_EH_ERROR;
                }

#endif //__WITH_DTLS__
            }

            /*
             * When current state of the device is un-owned and Provisioning
             * Tool is attempting to change the state to 'Owned' with a
             * qualified value for the field 'Owner'
             */
            if ((false == gDoxm->owned) && (true == newDoxm->owned) &&
                (memcmp(&(newDoxm->owner), &emptyOwner, sizeof(OicUuid_t)) != 0))
            {
#ifdef __WITH_DTLS__
                OCServerRequest * request = (OCServerRequest *)ehRequest->requestHandle;

                //Remove Temporal Credential resource
                if(tmpCredGenFlag)
                {
                    VERIFY_SUCCESS(TAG,
                                   OC_STACK_RESOURCE_DELETED == RemoveCredential(&tmpCredId),
                                   ERROR);
                    tmpCredGenFlag = false;
                }

                //Generate new credential for provisioning tool
                ehRet = AddOwnerPSK((CAEndpoint_t*)(&request->devAddr), newDoxm,
                                    (uint8_t*)OXM_RANDOM_DEVICE_PIN, strlen(OXM_RANDOM_DEVICE_PIN));
                VERIFY_SUCCESS(TAG, OC_EH_OK == ehRet, ERROR);

                //Update new state in persistent storage
                if((UpdatePersistentStorage(gDoxm) == true))
                {
                    ehRet = OC_EH_OK;
                }
                else
                {
                    /*
                     * If persistent storage update failed, revert back the state
                     * for global variable.
                     */
                    gDoxm->owned = false;
                    gDoxm->oxmSel = 0;
                    memset(&(gDoxm->owner), 0, sizeof(OicUuid_t));
                    ehRet = OC_EH_ERROR;

                }
#endif
             }
        }
    }
exit:
    //Send payload to request originator
    if(OC_STACK_OK != SendSRMResponse(ehRequest, ehRet, NULL))
    {
        OC_LOG (ERROR, TAG, PCF("SendSRMResponse failed in HandleDoxmPutRequest"));
    }
    DeleteDoxmBinData(newDoxm);
    return ehRet;
}

/*
 * This internal method is the entity handler for DOXM resources.
 */
OCEntityHandlerResult DoxmEntityHandler (OCEntityHandlerFlag flag,
                                        OCEntityHandlerRequest * ehRequest,
                                        void* callbackParam)
{
    (void)callbackParam;
    OCEntityHandlerResult ehRet = OC_EH_ERROR;

    if(NULL == ehRequest)
    {
        return ehRet;
    }

    if (flag & OC_REQUEST_FLAG)
    {
        OC_LOG (INFO, TAG, PCF("Flag includes OC_REQUEST_FLAG"));
        switch (ehRequest->method)
        {
            case OC_REST_GET:
                ehRet = HandleDoxmGetRequest(ehRequest);
                break;

            case OC_REST_PUT:
                ehRet = HandleDoxmPutRequest(ehRequest);
                break;

            default:
                ehRet = OC_EH_ERROR;
                SendSRMResponse(ehRequest, ehRet, NULL);
                break;
        }
    }
    return ehRet;
}

/*
 * This internal method is used to create '/oic/sec/doxm' resource.
 */
OCStackResult CreateDoxmResource()
{
    OCStackResult ret;

    ret = OCCreateResource(&gDoxmHandle,
                           OIC_RSRC_TYPE_SEC_DOXM,
                           OIC_MI_DEF,
                           OIC_RSRC_DOXM_URI,
                           DoxmEntityHandler,
                           NULL,
                           OC_OBSERVABLE | OC_SECURE | OC_EXPLICIT_DISCOVERABLE);

    if (OC_STACK_OK != ret)
    {
        OC_LOG (FATAL, TAG, PCF("Unable to instantiate Doxm resource"));
        DeInitDoxmResource();
    }
    return ret;
}

/**
 * Checks if DeviceID is generated during provisioning for the new device.
 * If DeviceID is NULL then generates the new DeviceID.
 * Once DeviceID is assigned to the device it does not change for the lifetime of the device.
 *
 */
static OCStackResult CheckDeviceID()
{
    OCStackResult ret = OC_STACK_ERROR;
    bool validId = false;
    for (uint8_t i = 0; i < sizeof(gDoxm->deviceID.id); i++)
    {
        if (gDoxm->deviceID.id[i] != 0)
        {
            validId = true;
            break;
        }
    }

    if (!validId)
    {
        if (OCGenerateUuid(gDoxm->deviceID.id) != RAND_UUID_OK)
        {
            OC_LOG(FATAL, TAG, PCF("Generate UUID for Server Instance failed!"));
            return ret;
        }
        ret = OC_STACK_OK;

        if (UpdatePersistentStorage(gDoxm))
        {
            //TODO: After registering PSI handler in all samples, do ret = OC_STACK_OK here.
            OC_LOG(FATAL, TAG, PCF("UpdatePersistentStorage failed!"));
        }
    }
    else
    {
        ret = OC_STACK_OK;
    }
    return ret;
}

/**
 * Get the default value.
 * @retval  the gDefaultDoxm pointer;
 */
OicSecDoxm_t* GetDoxmDefault()
{
    OC_LOG (INFO, TAG, PCF("GetDoxmToDefault"));
    return &gDefaultDoxm;
}

/**
 * This method is used by SRM to retrieve DOXM resource data.
 *
 * @retval  reference to @ref OicSecDoxm_t, binary format of Doxm resource data
 */
const OicSecDoxm_t* GetDoxmResourceData()
{
    return gDoxm;
}

/**
 * This method converts SVR buffers into OCRepPayload and updates the persistent storage.
 *
 * @param[out] payload is a pointer of CBOR doxm payload.
 * @param[out] size is CBOR doxm payload size.
 * @retval  OC_STACK_OK for Success, otherwise some error value
 */
OCStackResult ConvertDoxmData(uint8_t **payload,  size_t *size)
{
    OCStackResult result = OC_STACK_ERROR;
    VERIFY_NON_NULL(TAG, payload, ERROR);
    VERIFY_NON_NULL(TAG, size, ERROR);
    if (gDoxm)
    {
        OCRepPayload* rePayload = DoxmToPayload(gDoxm);
        result = OCConvertPayload((OCPayload*) rePayload, payload, size);
        OCPayloadDestroy((OCPayload*) rePayload);
        VERIFY_SUCCESS(TAG, result == OC_STACK_OK, ERROR);
    }

exit:
    return result;
}

/**
 * This method parses OCRepPayload into SVR buffers.
 *
 * @param[in] payload is a pointer of CBOR doxm payload.
 * @param[in] size is CBOR doxm payload size.
 * @retval  OC_STACK_OK for Success, otherwise some error value
 */
OCStackResult ParseDoxmPayload(uint8_t *payload,  size_t size)
{
    OCStackResult result = OC_STACK_ERROR;
    VERIFY_NON_NULL(TAG, payload, ERROR);
    if (size)
    {
        OCPayload* outPayload = NULL;
        result = OCParsePayload(&outPayload, payload, size);
        if (result == OC_STACK_OK)
        {
            gDoxm = PayloadToDoxm((const OCRepPayload*) outPayload);
            OCPayloadDestroy(outPayload);
        }
    }
exit:
    return result;
}

/**
 * Initialize DOXM resource by loading data from persistent storage.
 *
 * @param[in] payload CBOR doxm payload pointers.
 * @param[in] size size of CBOR doxm payload.
 * @retval  OC_STACK_OK for Success, otherwise some error value
 */
OCStackResult InitDoxmResource(uint8_t *payload,  size_t size)
{
    OCStackResult ret = OC_STACK_ERROR;
    VERIFY_NON_NULL(TAG, payload, ERROR);
    gDoxm = NULL;
    ret = ParseDoxmPayload(payload, size);
    if (ret != OC_STACK_OK)
    {
        OC_LOG (ERROR, TAG, PCF("ParseDoxmPayload failed"));
    }
   /*
     * If SVR database in persistent storage got corrupted or
     * is not available for some reason, a default doxm is created
     * which allows user to initiate doxm provisioning again.
     */
    if(!gDoxm)
    {
        gDoxm = GetDoxmDefault();
    }
    ret = CheckDeviceID();
    if (ret == OC_STACK_OK)
    {
        //Instantiate 'oic.sec.doxm'
        ret = CreateDoxmResource();
    }
    else
    {
        OC_LOG (ERROR, TAG, PCF("CheckDeviceID failed"));
    }

exit:
    return ret;
}

/**
 * Perform cleanup for DOXM resources.
 *
 * @return
 * OC_STACK_OK    - no error
 * OC_STACK_ERROR - stack process error
 *
 */
OCStackResult DeInitDoxmResource()
{
    OCStackResult ret = OCDeleteResource(gDoxmHandle);
    if(gDoxm != &gDefaultDoxm)
    {
        DeleteDoxmBinData(gDoxm);
    }
    gDoxm = NULL;

    if(OC_STACK_OK == ret)
    {
        return OC_STACK_OK;
    }
    else
    {
        return OC_STACK_ERROR;
    }
}

/**
 * This method returns the SRM device ID for this device.
 *
 * @retval  OC_STACK_OK for Success, otherwise some error value
 */
OCStackResult GetDoxmDeviceID(OicUuid_t *deviceID)
{
    if(deviceID && gDoxm)
    {
       *deviceID = gDoxm->deviceID;
        return OC_STACK_OK;
    }
    return OC_STACK_ERROR;
}

/**
 * @brief Gets the OicUuid_t value for the owner of this device.
 *
 * @return OC_STACK_OK if devOwner is a valid UUID, otherwise OC_STACK_ERROR.
 */
OCStackResult GetDoxmDevOwnerId(OicUuid_t *devOwner)
{
    OCStackResult retVal = OC_STACK_ERROR;
    if(gDoxm)
    {
        if(gDoxm->owned) {
            *devOwner = gDoxm->owner; // TODO change to devOwner when available
            retVal = OC_STACK_OK;
        }
    }
    return retVal;
}
