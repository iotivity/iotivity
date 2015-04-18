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
#include "ocmalloc.h"
#include "cJSON.h"
#include "resourcemanager.h"
#include "doxmresource.h"
#include "psinterface.h"
#include "utlist.h"
#include "srmresourcestrings.h"
#include "securevirtualresourcetypes.h"
#include "base64.h"
#include "ocrandom.h"
#include <stdlib.h>
#include <string.h>

#define TAG  PCF("SRM-DOXM")

#define VERIFY_SUCCESS(op, logLevel) { if ((!op)) \
            {OC_LOG((logLevel), TAG, PCF(#op " failed!!")); goto exit;} }

#define VERIFY_NON_NULL(arg, logLevel) { if (!(arg)) { OC_LOG((logLevel), \
             TAG, PCF(#arg " is NULL")); goto exit; } }

OicSecDoxm_t        *gDoxm = NULL;
OCResourceHandle    gDoxmHandle = NULL;

static void DeleteDoxmBinData(OicSecDoxm_t* doxm)
{
    if (doxm)
    {
        //Clean oxmType
        for(int i = 0; i < doxm->oxmTypeLen; i++)
        {
            OCFree(doxm->oxmType[i]);
        }
        OCFree(doxm->oxmType);

        //clean oxm
        OCFree(doxm->oxm);

        //Clean doxm itself
        OCFree(doxm);
    }
}

/*
 * This internal method converts DOXM data into JSON format.
 *
 * Note: Caller needs to invoke 'free' when finished done using
 * return string.
 */
char * BinToDoxmJSON(const OicSecDoxm_t * doxm)
{
    if (NULL == doxm)
    {
        return NULL;
    }

    char *jsonStr = NULL;
    cJSON *jsonRoot = cJSON_CreateObject();
    VERIFY_NON_NULL(jsonRoot, ERROR);

    cJSON *jsonDoxm = cJSON_CreateObject();
    VERIFY_NON_NULL(jsonDoxm, ERROR);
    cJSON_AddItemToObject(jsonRoot, OIC_JSON_DOXM_NAME, jsonDoxm );

    char base64Buff[B64ENCODE_OUT_SAFESIZE(sizeof(((OicUuid_t*)0)->id)) + 1] = {};
    uint32_t outLen = 0;
    B64Result b64Ret = B64_OK;

    //OxmType -- Not Mandatory
    if(doxm->oxmTypeLen > 0)
    {
        cJSON *jsonOxmTyArray = cJSON_CreateArray();
        VERIFY_NON_NULL(jsonOxmTyArray, ERROR);
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
        VERIFY_NON_NULL(jsonOxmArray, ERROR);
        cJSON_AddItemToObject (jsonDoxm, OIC_JSON_OXM_NAME,jsonOxmArray );
        for (size_t i = 0; i < doxm->oxmLen; i++)
        {
            cJSON_AddItemToArray (jsonOxmArray, cJSON_CreateNumber(doxm->oxm[i]));
        }
    }

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
    VERIFY_SUCCESS(b64Ret == B64_OK, ERROR);
    cJSON_AddStringToObject(jsonDoxm, OIC_JSON_DEVICE_ID_NAME, base64Buff);

    //Owner -- Mandatory
    outLen = 0;
    b64Ret = b64Encode(doxm->owner.id, sizeof(doxm->owner.id), base64Buff,
                    sizeof(base64Buff), &outLen);
    VERIFY_SUCCESS(b64Ret == B64_OK, ERROR);
    cJSON_AddStringToObject(jsonDoxm, OIC_JSON_OWNER_NAME, base64Buff);

    jsonStr = cJSON_PrintUnformatted(jsonRoot);

exit:
    if (jsonRoot)
    {
        cJSON_Delete(jsonRoot);
    }
    return jsonStr;
}

/*
 * This internal method converts JSON DOXM into binary DOXM.
 */
OicSecDoxm_t * JSONToDoxmBin(const char * jsonStr)
{
    OCStackResult ret = OC_STACK_ERROR;
    OicSecDoxm_t *doxm =  NULL;
    OicSecDoxm_t initDoxm = {};

    cJSON *jsonRoot = cJSON_Parse(jsonStr);
    VERIFY_NON_NULL(jsonRoot, ERROR);

    cJSON *jsonDoxm = cJSON_GetObjectItem(jsonRoot, OIC_JSON_DOXM_NAME);
    VERIFY_NON_NULL(jsonDoxm, ERROR);

    doxm = (OicSecDoxm_t*)OCCalloc(1, sizeof(OicSecDoxm_t));
    VERIFY_NON_NULL(doxm, ERROR);
    doxm = &initDoxm;

    size_t jsonObjLen = 0;
    cJSON *jsonObj = NULL;

    unsigned char base64Buff[sizeof(((OicUuid_t*)0)->id)] = {};
    uint32_t outLen = 0;
    B64Result b64Ret = B64_OK;

    //OxmType -- not Mandatory
    jsonObj = cJSON_GetObjectItem(jsonDoxm, OIC_JSON_OXM_TYPE_NAME);
    if ((jsonObj) && (cJSON_Array == jsonObj->type))
    {
        doxm->oxmTypeLen = cJSON_GetArraySize(jsonObj);
        VERIFY_SUCCESS(doxm->oxmTypeLen > 0, ERROR);

        doxm->oxmType = (OicUrn_t *)OCCalloc(doxm->oxmTypeLen, sizeof(char *));
        VERIFY_NON_NULL(doxm->oxmType, FATAL);

        for(int i  = 0; i < doxm->oxmTypeLen ; i++)
        {
            cJSON *jsonOxmTy = cJSON_GetArrayItem(jsonObj, i);
            VERIFY_NON_NULL(jsonOxmTy, ERROR);

            jsonObjLen = strlen(jsonOxmTy->valuestring) + 1;
            doxm->oxmType[i] = (char*)OCMalloc(jsonObjLen);
            VERIFY_NON_NULL(doxm->oxmType[i], FATAL);
            strncpy((char *)doxm->oxmType[i], (char *)jsonOxmTy->valuestring, jsonObjLen);
        }
    }

    //Oxm -- not Mandatory
    jsonObj = cJSON_GetObjectItem(jsonDoxm, OIC_JSON_OXM_NAME);
    if (jsonObj && cJSON_Array == jsonObj->type)
    {
        doxm->oxmLen = cJSON_GetArraySize(jsonObj);
        VERIFY_SUCCESS(doxm->oxmLen > 0, ERROR);

        doxm->oxm = (OicSecOxm_t*)OCCalloc(doxm->oxmLen, sizeof(short));
        VERIFY_NON_NULL(doxm->oxm, FATAL);

        for(int i  = 0; i < doxm->oxmLen ; i++)
        {
            cJSON *jsonOxm = cJSON_GetArrayItem(jsonObj, i);
            VERIFY_NON_NULL(jsonOxm, ERROR);
            doxm->oxm[i] = jsonOxm->valueint;
        }
    }

    //Owned -- Mandatory
    jsonObj = cJSON_GetObjectItem(jsonDoxm, OIC_JSON_OWNED_NAME);
    VERIFY_NON_NULL(jsonObj, ERROR);
    VERIFY_SUCCESS((cJSON_True || cJSON_False) == jsonObj->type, ERROR)
    doxm->owned = jsonObj->valueint;

    //TODO: Need more clarification on deviceIDFormat field type.
#if 0
    //DeviceIdFormat -- Mandatory
    jsonObj = cJSON_GetObjectItem(jsonDoxm, OIC_JSON_DEVICE_ID_FORMAT_NAME);
    VERIFY_NON_NULL(jsonObj, ERROR);
    VERIFY_SUCCESS(cJSON_Number == jsonObj->type, ERROR)
    doxm->deviceIDFormat = jsonObj->valueint;
#endif

    //DeviceId -- Mandatory
    jsonObj = cJSON_GetObjectItem(jsonDoxm, OIC_JSON_DEVICE_ID_NAME);
    VERIFY_NON_NULL(jsonObj, ERROR);
    VERIFY_SUCCESS(cJSON_String == jsonObj->type, ERROR);
    outLen = 0;
    b64Ret = b64Decode(jsonObj->valuestring, strlen(jsonObj->valuestring), base64Buff,
            sizeof(base64Buff), &outLen);
    VERIFY_SUCCESS((b64Ret == B64_OK) && (outLen <= sizeof(doxm->deviceID.id)), ERROR);
    memcpy(doxm->deviceID.id, base64Buff, outLen);

    //Owner -- Mandatory
    jsonObj = cJSON_GetObjectItem(jsonDoxm, OIC_JSON_OWNER_NAME);
    VERIFY_NON_NULL(jsonObj, ERROR);
    VERIFY_SUCCESS(cJSON_String == jsonObj->type, ERROR)
    outLen = 0;
    b64Ret = b64Decode(jsonObj->valuestring, strlen(jsonObj->valuestring), base64Buff,
                sizeof(base64Buff), &outLen);
    VERIFY_SUCCESS((b64Ret == B64_OK) && (outLen <= sizeof(doxm->owner.id)), ERROR);
    memcpy(doxm->owner.id, base64Buff, outLen);

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

/*
 * This internal method is the entity handler for DOXM resources.
 */
OCEntityHandlerResult DoxmEntityHandler (OCEntityHandlerFlag flag,
                                        OCEntityHandlerRequest * ehRequest)
{
    OCEntityHandlerResult ret = OC_EH_ERROR;
    char *jsonRsp = NULL;

    if(NULL == ehRequest)
    {
        return ret;
    }

    /*
     * This method will handle REST request (GET/PUT/POST/DEL) for
     * virtual resources such as: /oic/sec/cred, /oic/sec/doxm etc
     */

    if (flag & OC_REQUEST_FLAG)
    {
        //TODO :  Handle PUT/POST/DEL methods
        OC_LOG (INFO, TAG, PCF("Flag includes OC_REQUEST_FLAG"));
        if (OC_REST_GET == ehRequest->method)
        {
            //Convert DOXM data into JSON for transmission
            jsonRsp = BinToDoxmJSON(gDoxm);

            // Send payload to request originator
            ret = (SendSRMResponse(ehRequest, jsonRsp) == OC_STACK_OK ?
                   OC_EH_OK : OC_EH_ERROR);
        }
    }
    OCFree(jsonRsp);
    return ret;
}

/*
 * This internal method is used to create '/oic/sec/doxm' resource.
 */
OCStackResult CreateDoxmResource()
{
    OCStackResult ret;

    //TODO : Does these resources needs to be OC_DISCOVERABLE
    ret = OCCreateResource(&gDoxmHandle,
                           OIC_RSRC_TYPE_SEC_DOXM,
                           OIC_MI_DEF,
                           OIC_RSRC_DOXM_URI,
                           DoxmEntityHandler,
                           OC_DISCOVERABLE | OC_OBSERVABLE);

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
    //TODO: Save this deviceID at secure location so that we can retrieve it if the
    //JSON gets corrupted.
    if(strcmp((char *)gDoxm->deviceID.id, "") == 0 )
    {
        OCFillRandomMem(gDoxm->deviceID.id, sizeof(gDoxm->deviceID.id));
    }
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
    VERIFY_NON_NULL(jsonSVRDatabase, FATAL);

    //Convert JSON DOXM into binary format
    gDoxm = JSONToDoxmBin(jsonSVRDatabase);
    VERIFY_NON_NULL(gDoxm, FATAL);
    CheckDeviceID();

    //Instantiate 'oic.sec.doxm'
    ret = CreateDoxmResource();

exit:
    OCFree(jsonSVRDatabase);
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
    DeleteDoxmBinData(gDoxm);
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
 * This method is used by PolicyEngine to retrieve DOXM resource data.
 *
 * @retval  reference to @ref OicSecDoxm_t, binary format of Doxm resource data
 */
const OicSecDoxm_t* GetDoxmResourceData()
{
    return gDoxm;
}
