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
#include "base64.h"
#include "ocrandom.h"
#include "cainterface.h"
#include "credresource.h"
#include "ocserverrequest.h"
#include "srmutility.h"
#include <stdlib.h>
#include <string.h>

#if HAVE_STRINGS_H
#include <strings.h>
#endif

#define TAG  PCF("SRM-DOXM")

static OicSecDoxm_t        *gDoxm = NULL;
static OCResourceHandle    gDoxmHandle = NULL;

static OicSecOxm_t gOicSecDoxmJustWorks = OIC_JUST_WORKS;
static OicSecDoxm_t gDefaultDoxm =
{
    NULL,                   /* OicUrn_t *oxmType */
    0,                      /* size_t oxmTypeLen */
    &gOicSecDoxmJustWorks,  /* uint16_t *oxm */
    1,                      /* size_t oxmLen */
    OIC_JUST_WORKS,         /* uint16_t oxmSel */
    false,                  /* bool owned */
    {},                     /* OicUuid_t deviceID */
    {},                     /* OicUuid_t owner */
};

void DeleteDoxmBinData(OicSecDoxm_t* doxm)
{
    if (doxm)
    {
        //Clean oxmType
        for(int i = 0; i < doxm->oxmTypeLen; i++)
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

char * BinToDoxmJSON(const OicSecDoxm_t * doxm)
{
    if (NULL == doxm)
    {
        return NULL;
    }

    char *jsonStr = NULL;
    cJSON *jsonDoxm = NULL;
    char base64Buff[B64ENCODE_OUT_SAFESIZE(sizeof(((OicUuid_t*)0)->id)) + 1] = {};
    uint32_t outLen = 0;
    B64Result b64Ret = B64_OK;

    cJSON *jsonRoot = cJSON_CreateObject();
    VERIFY_NON_NULL(TAG, jsonRoot, ERROR);

    jsonDoxm = cJSON_CreateObject();
    VERIFY_NON_NULL(TAG, jsonDoxm, ERROR);
    cJSON_AddItemToObject(jsonRoot, OIC_JSON_DOXM_NAME, jsonDoxm );

    //OxmType -- Not Mandatory
    if(doxm->oxmTypeLen > 0)
    {
        cJSON *jsonOxmTyArray = cJSON_CreateArray();
        VERIFY_NON_NULL(TAG, jsonOxmTyArray, ERROR);
        cJSON_AddItemToObject (jsonDoxm, OIC_JSON_OXM_TYPE_NAME, jsonOxmTyArray );
        for (size_t i = 0; i < doxm->oxmTypeLen; i++)
        {
            cJSON_AddItemToArray (jsonOxmTyArray, cJSON_CreateString(doxm->oxmType[i]));
        }
    }

    //Oxm -- Not Mandatory
    if(doxm->oxmLen > 0)
    {
        cJSON *jsonOxmArray = cJSON_CreateArray();
        VERIFY_NON_NULL(TAG, jsonOxmArray, ERROR);
        cJSON_AddItemToObject (jsonDoxm, OIC_JSON_OXM_NAME,jsonOxmArray );
        for (size_t i = 0; i < doxm->oxmLen; i++)
        {
            cJSON_AddItemToArray (jsonOxmArray, cJSON_CreateNumber(doxm->oxm[i]));
        }
    }

    //OxmSel -- Mandatory
    cJSON_AddNumberToObject(jsonDoxm, OIC_JSON_OXM_SEL_NAME, (int)doxm->oxmSel);

    //Owned -- Mandatory
    cJSON_AddBoolToObject(jsonDoxm, OIC_JSON_OWNED_NAME, doxm->owned);

    //TODO: Need more clarification on deviceIDFormat field type.
#if 0
    //DeviceIdFormat -- Mandatory
    cJSON_AddNumberToObject(jsonDoxm, OIC_JSON_DEVICE_ID_FORMAT_NAME, doxm->deviceIDFormat);
#endif

    //DeviceId -- Mandatory
    outLen = 0;
    b64Ret = b64Encode(doxm->deviceID.id, sizeof(doxm->deviceID.id), base64Buff,
                    sizeof(base64Buff), &outLen);
    VERIFY_SUCCESS(TAG, b64Ret == B64_OK, ERROR);
    cJSON_AddStringToObject(jsonDoxm, OIC_JSON_DEVICE_ID_NAME, base64Buff);

    //Owner -- Mandatory
    outLen = 0;
    b64Ret = b64Encode(doxm->owner.id, sizeof(doxm->owner.id), base64Buff,
                    sizeof(base64Buff), &outLen);
    VERIFY_SUCCESS(TAG, b64Ret == B64_OK, ERROR);
    cJSON_AddStringToObject(jsonDoxm, OIC_JSON_OWNER_NAME, base64Buff);

    jsonStr = cJSON_PrintUnformatted(jsonRoot);

exit:
    if (jsonRoot)
    {
        cJSON_Delete(jsonRoot);
    }
    return jsonStr;
}

OicSecDoxm_t * JSONToDoxmBin(const char * jsonStr)
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

    doxm = (OicSecDoxm_t*)OICCalloc(1, sizeof(OicSecDoxm_t));
    VERIFY_NON_NULL(TAG, doxm, ERROR);

    //OxmType -- not Mandatory
    jsonObj = cJSON_GetObjectItem(jsonDoxm, OIC_JSON_OXM_TYPE_NAME);
    if ((jsonObj) && (cJSON_Array == jsonObj->type))
    {
        doxm->oxmTypeLen = cJSON_GetArraySize(jsonObj);
        VERIFY_SUCCESS(TAG, doxm->oxmTypeLen > 0, ERROR);

        doxm->oxmType = (OicUrn_t *)OICCalloc(doxm->oxmTypeLen, sizeof(char *));
        VERIFY_NON_NULL(TAG, (doxm->oxmType), ERROR);

        for(int i  = 0; i < doxm->oxmTypeLen ; i++)
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

        for(int i  = 0; i < doxm->oxmLen ; i++)
        {
            cJSON *jsonOxm = cJSON_GetArrayItem(jsonObj, i);
            VERIFY_NON_NULL(TAG, jsonOxm, ERROR);
            doxm->oxm[i] = (OicSecOxm_t)jsonOxm->valueint;
        }
    }

    //OxmSel -- Mandatory
    jsonObj = cJSON_GetObjectItem(jsonDoxm, OIC_JSON_OXM_SEL_NAME);
    if(jsonObj)
    {
        VERIFY_SUCCESS(TAG, cJSON_Number == jsonObj->type, ERROR)
        doxm->oxmSel = (OicSecOxm_t)jsonObj->valueint;
    }
    else // PUT/POST JSON may not have oxmsel so set it to the gDoxm->oxmSel
    {
        VERIFY_NON_NULL(TAG, gDoxm, ERROR);
        doxm->oxmSel = gDoxm->oxmSel;
    }

    //Owned -- Mandatory
    jsonObj = cJSON_GetObjectItem(jsonDoxm, OIC_JSON_OWNED_NAME);
    if(jsonObj)
    {
        VERIFY_SUCCESS(TAG, (cJSON_True == jsonObj->type || cJSON_False == jsonObj->type), ERROR)
        doxm->owned = jsonObj->valueint;
    }
    else // PUT/POST JSON may not have owned so set it to the gDomx->owned
    {
        VERIFY_NON_NULL(TAG, gDoxm, ERROR);
        doxm->owned = gDoxm->owned;
    }

    //DeviceId -- Mandatory
    jsonObj = cJSON_GetObjectItem(jsonDoxm, OIC_JSON_DEVICE_ID_NAME);
    if(jsonObj)
    {
        VERIFY_SUCCESS(TAG, cJSON_String == jsonObj->type, ERROR);
        if(cJSON_String == jsonObj->type)
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
    else // PUT/POST JSON will not have deviceID so set it to the gDoxm->deviceID.id
    {
        VERIFY_NON_NULL(TAG, gDoxm, ERROR);
        VERIFY_SUCCESS(TAG, strcmp((char *)gDoxm->deviceID.id, "") != 0, ERROR);
        strncpy((char *)doxm->deviceID.id, (char *)gDoxm->deviceID.id, sizeof(doxm->deviceID.id));
    }

    // Owner -- will be empty when device state is unowned.
    if (true == doxm->owned)
    {
        jsonObj = cJSON_GetObjectItem(jsonDoxm, OIC_JSON_OWNER_NAME);
        VERIFY_NON_NULL(TAG, jsonObj, ERROR);
        VERIFY_SUCCESS(TAG, cJSON_String == jsonObj->type, ERROR)
            outLen = 0;
        b64Ret = b64Decode(jsonObj->valuestring, strlen(jsonObj->valuestring), base64Buff,
                sizeof(base64Buff), &outLen);
        VERIFY_SUCCESS(TAG, (b64Ret == B64_OK && outLen <= sizeof(doxm->owner.id)), ERROR);
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

/**
 * @todo document this function including why code might need to call this.
 * The current suspicion is that it's not being called as much as it should.
 */
static bool UpdatePersistentStorage(OicSecDoxm_t * doxm)
{
    bool bRet = false;

    if (NULL != doxm)
    {
        // Convert Doxm data into JSON for update to persistent storage
        char *jsonStr = BinToDoxmJSON(doxm);
        if (jsonStr)
        {
            cJSON *jsonDoxm = cJSON_Parse(jsonStr);
            OICFree(jsonStr);

            if (jsonDoxm &&
                    (OC_STACK_OK == UpdateSVRDatabase(OIC_JSON_DOXM_NAME, jsonDoxm)))
            {
                bRet = true;
            }
            cJSON_Delete(jsonDoxm);
        }
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

    OicParseQueryIter_t parseIter = {};

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

    jsonStr = (ehRet == OC_EH_OK) ? BinToDoxmJSON(gDoxm) : NULL;

    // Send response payload to request originator
    if(OC_STACK_OK != SendSRMResponse(ehRequest, ehRet, jsonStr))
    {
        OC_LOG (ERROR, TAG, PCF("SendSRMResponse failed in HandleDoxmGetRequest"));
    }

    OICFree(jsonStr);

    return ehRet;
}


static OCEntityHandlerResult HandleDoxmPutRequest (const OCEntityHandlerRequest * ehRequest)
{
    OC_LOG (INFO, TAG, PCF("Doxm EntityHandle  processing PUT request"));
    OCEntityHandlerResult ehRet = OC_EH_ERROR;
    OicUuid_t emptyOwner = {};

    /*
     * Convert JSON Doxm data into binary. This will also validate
     * the Doxm data received.
     */
    OicSecDoxm_t* newDoxm = JSONToDoxmBin(((OCSecurityPayload*)ehRequest->payload)->securityData);

    if (newDoxm)
    {
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
                CAResult_t pskRet;

                OCServerRequest *request = (OCServerRequest *)ehRequest->requestHandle;
                uint8_t ownerPSK[OWNER_PSK_LENGTH_128] = {};

                //Generating OwnerPSK
                OC_LOG (INFO, TAG, PCF("Doxm EntityHandle  generating OwnerPSK"));
                pskRet = CAGenerateOwnerPSK((CAEndpoint_t *)&request->devAddr,
                        (uint8_t*) OXM_JUST_WORKS, strlen(OXM_JUST_WORKS),
                        newDoxm->owner.id, sizeof(newDoxm->owner.id),
                        gDoxm->deviceID.id, sizeof(gDoxm->deviceID.id),
                        ownerPSK, OWNER_PSK_LENGTH_128);

                VERIFY_SUCCESS(TAG, pskRet == CA_STATUS_OK, ERROR);

                //Generating new credential for provisioning tool
                size_t ownLen = 1;
                uint32_t outLen = 0;

                char base64Buff[B64ENCODE_OUT_SAFESIZE(sizeof(ownerPSK)) + 1] = {};
                B64Result b64Ret = b64Encode(ownerPSK, sizeof(ownerPSK), base64Buff,
                                sizeof(base64Buff), &outLen);
                VERIFY_SUCCESS(TAG, b64Ret == B64_OK, ERROR);

                OC_LOG (INFO, TAG, PCF("Doxm EntityHandle  generating Credential"));
                OicSecCred_t *cred = GenerateCredential(&newDoxm->owner, SYMMETRIC_PAIR_WISE_KEY,
                                        NULL, base64Buff, ownLen, &newDoxm->owner);
                VERIFY_NON_NULL(TAG, cred, ERROR);

                //Adding provisioning tool credential to cred Resource.
                VERIFY_SUCCESS(TAG, OC_STACK_OK == AddCredential(cred), ERROR);

                gDoxm->owned = true;
                memcpy(&(gDoxm->owner), &(newDoxm->owner), sizeof(OicUuid_t));

                // Update new state in persistent storage
                if (true == UpdatePersistentStorage(gDoxm))
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
    }

exit:

    //Send payload to request originator
    if(OC_STACK_OK != SendSRMResponse(ehRequest, ehRet, NULL))
    {
        OC_LOG (ERROR, TAG, PCF("SendSRMResponse failed in HandlePstatPostRequest"));
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
void CheckDeviceID()
{
    if(strcmp((char *)gDoxm->deviceID.id, "") == 0 )
    {
        OCFillRandomMem(gDoxm->deviceID.id, sizeof(gDoxm->deviceID.id));
        UpdatePersistentStorage(gDoxm);
    }
}

/**
 * Get the default value.
 * @retval  the gDefaultDoxm pointer;
 */
static OicSecDoxm_t* GetDoxmDefault()
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
 * Initialize DOXM resource by loading data from persistent storage.
 *
 * @retval  OC_STACK_OK for Success, otherwise some error value
 */
OCStackResult InitDoxmResource()
{
    OCStackResult ret = OC_STACK_ERROR;

    //Read DOXM resource from PS
    char* jsonSVRDatabase = GetSVRDatabase();
    if(jsonSVRDatabase)
    {
        //Convert JSON DOXM into binary format
        gDoxm = JSONToDoxmBin(jsonSVRDatabase);
    }
    /*
     * If SVR database in persistent storage got corrupted or
     * is not available for some reason, a default doxm is created
     * which allows user to initiate doxm provisioning again.
     */
    if(!jsonSVRDatabase || !gDoxm)
    {
        gDoxm = GetDoxmDefault();
    }
    CheckDeviceID();
    //Instantiate 'oic.sec.doxm'
    ret = CreateDoxmResource();
    OICFree(jsonSVRDatabase);
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
    if(gDoxm  != &gDefaultDoxm)
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
