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
#include "resourcemanager.h"
#include "pstatresource.h"
#include "psinterface.h"
#include "utlist.h"
#include "srmresourcestrings.h"
#include "srmutility.h"
#include "ocpayload.h"
#include "ocpayloadcbor.h"
#include "resourcemanager.h"
#include "payload_logging.h"
#include <stdlib.h>
#include <string.h>

#define TAG  PCF("SRM-PSTAT")

static OicSecDpom_t gSm = SINGLE_SERVICE_CLIENT_DRIVEN;
static OicSecPstat_t gDefaultPstat =
{
    false,                                    // bool isOwned
    (OicSecDpm_t)(TAKE_OWNER | BOOTSTRAP_SERVICE | SECURITY_MANAGEMENT_SERVICES |
    PROVISION_CREDENTIALS | PROVISION_ACLS),   // OicSecDpm_t cm
    (OicSecDpm_t)(TAKE_OWNER | BOOTSTRAP_SERVICE | SECURITY_MANAGEMENT_SERVICES |
    PROVISION_CREDENTIALS | PROVISION_ACLS),   // OicSecDpm_t tm
    {.id = {0}},                              // OicUuid_t deviceID
    SINGLE_SERVICE_CLIENT_DRIVEN,             // OicSecDpom_t om */
    1,                                        // the number of elts in Sms
    &gSm,                                     // OicSecDpom_t *sm
    0,                                        // uint16_t commitHash
};
OicSecPstat_t *gPstat = NULL;
static OCResourceHandle gPstatHandle = NULL;

void DeletePstatBinData(OicSecPstat_t* pstat)
{
    if (pstat)
    {
        //Clean 'supported modes' field
        OICFree(pstat->sm);

        //Clean pstat itself
        OICFree(pstat);
    }
}

/**
 * This method converts pstat data into OCRepPayload.
 */
OCRepPayload* PstatToPayload(const OicSecPstat_t * pstat)
{
    if(NULL == pstat)
    {
        return NULL;
    }

    size_t dimensions[MAX_REP_ARRAY_DEPTH] = {0};
    bool result = false;
    OCRepPayload* payload = OCRepPayloadCreate();
    if(!payload)
    {
        OC_LOG(ERROR, TAG, PCF("Failed to allocate Payload"));
        return NULL;
    }

    result = OCRepPayloadSetUri(payload, OIC_RSRC_PSTAT_URI);
    VERIFY_SUCCESS(TAG, result, ERROR);
    //isOp
    result = OCRepPayloadSetPropBool(payload, OIC_JSON_ISOP_NAME, pstat->isOp);
    VERIFY_SUCCESS(TAG, result, ERROR);
    //deviceID
    dimensions[0] = SVR_UUID_LENGTH;
    result = OCRepPayloadSetIntArray(payload, OIC_JSON_DEVICE_ID_NAME,
            (int64_t*) pstat->deviceID.id, dimensions);
    VERIFY_SUCCESS(TAG, result, ERROR);
    //cm
    result = OCRepPayloadSetPropInt(payload, OIC_JSON_COMMIT_HASH_NAME, (int) pstat->cm);
    VERIFY_SUCCESS(TAG, result, ERROR);
    //tm
    result = OCRepPayloadSetPropInt(payload, OIC_JSON_TM_NAME, (int) pstat->tm);
    VERIFY_SUCCESS(TAG, result, ERROR);
    //om
    result = OCRepPayloadSetPropInt(payload, OIC_JSON_OM_NAME, (int) pstat->om);
    VERIFY_SUCCESS(TAG, result, ERROR);
    //sm
    dimensions[0] = pstat->smLen;
    OCRepPayloadSetIntArray(payload, OIC_JSON_SM_NAME, (int64_t*) pstat->sm, dimensions);
    OC_LOG_PAYLOAD(INFO, TAG, (OCPayload*) payload);

exit:
    if(!result)
    {
        OCRepPayloadDestroy(payload);
        payload = NULL;
    }
    return payload;
}

/**
 * This method converts OCRepPayload into pstat.
 */
OicSecPstat_t* PayloadToPstat(const OCRepPayload* payload)
{
    if(NULL == payload)
    {
        return NULL;
    }
    OC_LOG_PAYLOAD(INFO, TAG, (OCPayload*)payload);

    size_t dimensions[MAX_REP_ARRAY_DEPTH] = {0};
    int64_t value = 0;
    bool result = false;
    OicSecPstat_t *pstat = (OicSecPstat_t*) OICCalloc(1, sizeof(OicSecPstat_t));

    VERIFY_NON_NULL(TAG, pstat, INFO);
    //isOp
    result = OCRepPayloadGetPropBool(payload, OIC_JSON_ISOP_NAME, &pstat->isOp);
    VERIFY_SUCCESS(TAG, result, ERROR);
    //deviceID
    int64_t *id;
    result = OCRepPayloadGetIntArray(payload, OIC_JSON_DEVICE_ID_NAME, (int64_t**) &id, dimensions);
    VERIFY_SUCCESS(TAG, result, ERROR);
    memcpy(&pstat->deviceID.id, id, dimensions[0]);
    OICFree(id);
    //cm
    if (OCRepPayloadGetPropInt(payload, OIC_JSON_CM_NAME, &value))
    {
        pstat->cm = (OicSecDpm_t) value;
    }
    //om
    if (OCRepPayloadGetPropInt(payload, OIC_JSON_OM_NAME, &value))
    {
        pstat->om = (OicSecDpom_t) value;
    }
    //sm
    result = OCRepPayloadGetIntArray(payload, OIC_JSON_SM_NAME, (int64_t**) &pstat->sm, dimensions);
    VERIFY_SUCCESS(TAG, result, ERROR);
    pstat->smLen = dimensions[0];

exit:
    if(!result)
    {
        DeletePstatBinData(pstat);
        pstat = NULL;
    }
    return pstat;
}

/**
 * The entity handler determines how to process a GET request.
 */
static OCEntityHandlerResult HandlePstatGetRequest (const OCEntityHandlerRequest * ehRequest)
{
    OC_LOG (INFO, TAG, PCF("HandlePstatGetRequest processing the request"));
    // Convert pstat data into OCRepPayload.
    OCRepPayload* payload = PstatToPayload(gPstat);
    //TODO: read from DB
    // A device should always have a default pstat. Therefore, payload should never be NULL.
    OCEntityHandlerResult ehRet = (payload ? OC_EH_OK : OC_EH_ERROR);

    // Send response payload to request originator
    SendSRMResponse(ehRequest, ehRet, payload);
    return ehRet;
}

/**
 * The entity handler determines how to process a POST request.
 * Per the REST paradigm, POST can also be used to update representation of existing
 * resource or create a new resource.
 * For pstat, it updates only tm and om.
 */
static OCEntityHandlerResult HandlePstatPutRequest(const OCEntityHandlerRequest *ehRequest)
{
    OC_LOG (INFO, TAG, PCF("HandlePstatPutRequest processing the request"));
    OCEntityHandlerResult ehRet = OC_EH_ERROR;
    if(!ehRequest)
    {
        return ehRet;
    }

    if (ehRequest->resource)
    {
        OCRepPayload* payload = (OCRepPayload*)ehRequest->payload;
        gPstat = PayloadToPstat(payload);
        // Convert data and update it into persistent storage
        if (OC_STACK_OK == UpdateSVRData())
        {
            ehRet = OC_EH_OK;
        }
    }
    //Send payload to request originator
    if(OC_STACK_OK != SendSRMResponse(ehRequest, ehRet, NULL))
    {
        OC_LOG (ERROR, TAG, PCF("SendSRMResponse failed in HandlePstatPostRequest"));
    }
    return ehRet;
}

/**
 * This internal method is the entity handler for pstat resources.
 */
OCEntityHandlerResult PstatEntityHandler(OCEntityHandlerFlag flag,
        OCEntityHandlerRequest * ehRequest,
        void *callbackParam)
{
    (void)callbackParam;
    OCEntityHandlerResult ehRet = OC_EH_ERROR;
    // This method will handle REST request (GET/POST) for /oic/sec/pstat
    if (flag & OC_REQUEST_FLAG)
    {
        OC_LOG (INFO, TAG, PCF("Flag includes OC_REQUEST_FLAG"));
        switch (ehRequest->method)
        {
            case OC_REST_GET:
                ehRet = HandlePstatGetRequest(ehRequest);
                break;
            case OC_REST_PUT:
                ehRet = HandlePstatPutRequest(ehRequest);
                break;
            default:
                ehRet = OC_EH_ERROR;
                SendSRMResponse(ehRequest, ehRet, NULL);
                break;
        }
    }
    return ehRet;
}

/**
 * This internal method is used to create '/oic/sec/pstat' resource.
 */
OCStackResult CreatePstatResource()
{
    OCStackResult ret;

    ret = OCCreateResource(&gPstatHandle,
                           OIC_RSRC_TYPE_SEC_PSTAT,
                           OIC_MI_DEF,
                           OIC_RSRC_PSTAT_URI,
                           PstatEntityHandler,
                           NULL,
                           OC_RES_PROP_NONE);

    if (ret != OC_STACK_OK)
    {
        OC_LOG (FATAL, TAG, PCF("Unable to instantiate pstat resource"));
        DeInitPstatResource();
    }
    return ret;
}

/**
 * Post ACL hander update the commitHash during ACL provisioning.
 */
void SetCommitHash(uint16_t commitHash)
{
    gPstat->commitHash = commitHash;
}

/**
 * Get the default value
 * @retval  the gDefaultPstat pointer
 */
OicSecPstat_t* GetPstatDefault()
{
    return &gDefaultPstat;
}

/**
 * This method converts SVR buffers into OCRepPayload and updates the persistent storage.
 *
 * @param[out] payload is a pointer of CBOR pstat payload.
 * @param[out] size is CBOR pstat payload size.
 * @retval  OC_STACK_OK for Success, otherwise some error value
 */
OCStackResult ConvertPstatData(uint8_t **payload,  size_t *size)
{
    OCStackResult result = OC_STACK_ERROR;
    VERIFY_NON_NULL(TAG, payload, ERROR);
    VERIFY_NON_NULL(TAG, size, ERROR);

    if (gPstat)
    {
        OCRepPayload* rePayload = PstatToPayload(gPstat);
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
 * @param[in] payload is a pointer of CBOR pstat payload.
 * @param[in] size is CBOR pstat payload size.
 * @retval  OC_STACK_OK for Success, otherwise some error value
 */
OCStackResult ParsePstatPayload(uint8_t *payload,  size_t size)
{
    OCStackResult result = OC_STACK_ERROR;
    VERIFY_NON_NULL(TAG, payload, ERROR);
    if (size)
    {
        OCPayload* outPayload = NULL;
        result = OCParsePayload(&outPayload, payload, size);
        if (result == OC_STACK_OK)
        {
            gPstat = PayloadToPstat((const OCRepPayload*) outPayload);
            OCPayloadDestroy(outPayload);
        }
    }

exit:
    return result;
}

/**
 * Initialize pstat resource by loading data from persistent storage.
 *
 * @param[in] payload CBOR pstat payload pointers.
 * @param[in] size size of CBOR pstat payload.
 * @retval  OC_STACK_OK for Success, otherwise some error value
 */
OCStackResult InitPstatResource(uint8_t *payload,  size_t size)
{
    OCStackResult ret = OC_STACK_ERROR;
    VERIFY_NON_NULL(TAG, payload, ERROR);
    ret = ParsePstatPayload(payload, size);
    if(ret != OC_STACK_OK)
    {
        OC_LOG (ERROR, TAG, PCF("ParsePstatPayload failed"));

    }
    /*
     * If SVR database in persistent storage got corrupted or
     * is not available for some reason, a default pstat is created
     * which allows user to initiate pstat provisioning again.
     */
    if (!gPstat)
    {
        gPstat = GetPstatDefault();
    }
    // Instantiate 'oic.sec.pstat'
    ret = CreatePstatResource();

exit:
    return ret;
}

/**
 * Perform cleanup for pstat resources.
 *
 * @retval  OC_STACK_OK for Success, otherwise some error value
 */
OCStackResult DeInitPstatResource()
{
    if(gPstat != &gDefaultPstat)
    {
        DeletePstatBinData(gPstat);
        gPstat = NULL;
    }
    return OCDeleteResource(gPstatHandle);
}
