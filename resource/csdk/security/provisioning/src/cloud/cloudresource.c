/* *****************************************************************
 *
 * Copyright 2018 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * *****************************************************************/

#include "iotivity_config.h"
#include "iotivity_debug.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "octypes.h"
#include "oic_malloc.h"
#include "utlist.h"
#include "experimental/logger.h"
#include "ocpayload.h"
#include "ocpayloadcbor.h"
#include "srmutility.h"
#include "psinterface.h"
#include "deviceonboardingstate.h"
#include "srmresourcestrings.h"
#include "resourcemanager.h"
#include "cloud/cloudresource.h"
#include "cloud/auth.h"

#define TAG  "OIC_CLOUD_RESOURCE"

static OicCloud_t           *gCloud = NULL;
static OCResourceHandle     gCloudHandle = NULL;

static oc_mutex     gCloudMutex;

#define OC_CLOUD_PROVISIONING_APN   "apn"
#define OC_CLOUD_PROVISIONING_CIS   "cis"
#define OC_CLOUD_PROVISIONING_AT    "at"
#define OC_CLOUD_PROVISIONING_SID   "sid"

static OicCloud_t gDefaultCloud =
{
    NULL,
    NULL,
    NULL,
    NULL,
    OC_CLOUD_OK,
    NULL,
    NULL,
    NULL
};

static bool ValidCloud(OicCloud_t *cloud)
{
    OIC_LOG_V(DEBUG, TAG, "%s: IN", __func__);
    VERIFY_NOT_NULL_RETURN(TAG, cloud, ERROR, false);

    bool ret = true;

    if (!cloud->apn)
    {
        OIC_LOG_V(ERROR, TAG, "%s: Authorization Provider Name validate: %s", __func__,
                  cloud->apn ? cloud->apn : "NULL");
        ret = false;
    }
    if (!cloud->cis || 0 != strncmp(cloud->cis, "coaps+tcp://", 11))
    {
        OIC_LOG_V(ERROR, TAG, "%s: OCF Cloud URL validate: %s", __func__, cloud->cis ? cloud->cis : "NULL");
        ret = false;
    }
    if (!cloud->at)
    {
        OIC_LOG_V(ERROR, TAG, "%s: Access token validate: %s", __func__, cloud->at ? cloud->at : "NULL");
        ret = false;
    }

    OIC_LOG_V(DEBUG, TAG, "%s: OUT", __func__);
    return ret;
}

static void DeleteCloudList(OicCloud_t *clouds)
{
    OIC_LOG_V(DEBUG, TAG, "%s: IN", __func__);

    if (!clouds)
    {
        OIC_LOG_V(WARNING, TAG, "%s: cloud is NULL", __func__);
        return;
    }

    OicCloud_t *p1 = NULL, *p2 = NULL;
    oc_mutex_lock(gCloudMutex);
    LL_FOREACH_SAFE(clouds, p1, p2)
    {
        LL_DELETE(clouds, p1);
        FreeCloud(p1);
        p1 = NULL;
    }
    oc_mutex_unlock(gCloudMutex);

    OIC_LOG_V(DEBUG, TAG, "%s: OUT", __func__);
}

static void *CloudWaitForRFNOP(void *data)
{
    OIC_LOG_V(DEBUG, TAG, "%s: IN", __func__);
    OicSecDostype_t dos;

    OicCloud_t *cloud = (OicCloud_t *)data;
    VERIFY_NOT_NULL(TAG, cloud, ERROR);

    VERIFY_SUCCESS(TAG, OC_STACK_OK == GetDos(&dos), ERROR);

    while (DOS_RFNOP != dos.state && OC_CLOUD_EXIT != cloud->stat)
    {
        VERIFY_SUCCESS(TAG, OC_STACK_OK == GetDos(&dos), ERROR);
        sleep(1);
    }

    if (OC_STACK_OK != OCCloudSignUp(cloud))
    {
        OIC_LOG_V(ERROR, TAG, "%s: cloud sign up", __func__);
    }
exit:
    OIC_LOG_V(DEBUG, TAG, "%s: OUT", __func__);
    return NULL;
}

static OCEntityHandlerResult HandleCloudPostRequest(OCEntityHandlerRequest *ehRequest)
{
    OIC_LOG_V(DEBUG, TAG, "%s: IN", __func__);
    OCEntityHandlerResult ehRet = OC_EH_INTERNAL_SERVER_ERROR;
    OicSecDostype_t dos;
    OicCloud_t *newCloud = NULL;
    OCRepPayload *payload = NULL;
    bool isDeviceOwned = false;

    VERIFY_NOT_NULL(TAG, ehRequest, ERROR);
    VERIFY_NOT_NULL(TAG, ehRequest->payload, ERROR);

    VERIFY_SUCCESS(TAG, OC_STACK_OK == GetDos(&dos), ERROR);

    if (DOS_RFNOP != dos.state && DOS_RFPRO != dos.state)
    {
        OIC_LOG_V(ERROR, TAG, "%s %s resource is read-only in not RFNOP or DOS_RFPRO", __func__,
                  OIC_RSRC_TYPE_SEC_CLOUDCONF);
        ehRet = OC_EH_NOT_ACCEPTABLE;
        goto exit;
    }

    OCGetDeviceOwnedState(&isDeviceOwned);

    if (!isDeviceOwned)
    {
        OIC_LOG_V(ERROR, TAG, "%s: device is not owned", __func__);
        ehRet = OC_EH_NOT_ACCEPTABLE;
        goto exit;
    }

    newCloud = (OicCloud_t *) OICCalloc(1, sizeof(OicCloud_t));
    VERIFY_NOT_NULL(TAG, newCloud, ERROR);

    payload = (OCRepPayload *)ehRequest->payload;

    if (!OCRepPayloadGetPropString(payload, OC_CLOUD_PROVISIONING_APN, &newCloud->apn))
    {
        OIC_LOG_V(ERROR, TAG, "%s: Can't get: %s", __func__, OC_CLOUD_PROVISIONING_APN);
        goto exit;
    }
    if (!OCRepPayloadGetPropString(payload, OC_CLOUD_PROVISIONING_CIS, &newCloud->cis))
    {
        OIC_LOG_V(ERROR, TAG, "%s: Can't get: %s", __func__, OC_CLOUD_PROVISIONING_CIS);
        goto exit;
    }
    if (!OCRepPayloadGetPropString(payload, OC_CLOUD_PROVISIONING_AT, &newCloud->at))
    {
        OIC_LOG_V(ERROR, TAG, "%s: Can't get: %s", __func__, OC_CLOUD_PROVISIONING_AT);
        goto exit;
    }
    if (!OCRepPayloadGetPropString(payload, OC_CLOUD_PROVISIONING_SID, &newCloud->sid))
    {
        OIC_LOG_V(ERROR, TAG, "%s: Can't get: %s", __func__, OC_CLOUD_PROVISIONING_SID);
        goto exit;
    }

    if (!ValidCloud(newCloud))
    {
        OIC_LOG_V(ERROR, TAG, "%s: Error validate the new cloud", __func__);
        goto exit;
    }

    if (CloudFind(gCloud, newCloud))
    {
        OIC_LOG_V(ERROR, TAG, "%s: cloud: %s exist", __func__, newCloud->cis);
        goto exit;
    }

    newCloud->stat = OC_CLOUD_PROV;

    oc_mutex_lock(gCloudMutex);
    LL_APPEND(gCloud, newCloud);
    oc_mutex_unlock(gCloudMutex);

    if (DOS_RFNOP == dos.state)
    {
        if (OC_STACK_OK != OCCloudSignUp(newCloud))
        {
            OIC_LOG_V(ERROR, TAG, "%s: cloud sign up", __func__);
        }
    }
    else
    {
        OCThreadResult_t res = OC_THREAD_SUCCESS;
        res = oc_thread_new(&newCloud->pid, CloudWaitForRFNOP, newCloud);
        if (OC_THREAD_SUCCESS != res)
        {
            OIC_LOG_V(ERROR, TAG, "%s: create thread: %d", __func__, res);
        }
    }

    ehRet = OC_EH_OK;
exit:
    ehRet = ((SendSRMResponse(ehRequest, ehRet, NULL, 0)) == OC_STACK_OK) ?
            OC_EH_OK : OC_EH_ERROR;

    OIC_LOG_V(DEBUG, TAG, "%s: OUT", __func__);

    return ehRet;
}

OCRepPayload *CreateCloudGetPayload(const OicCloud_t *cloud)
{
    OIC_LOG_V(DEBUG, TAG, "%s: IN", __func__);

    OCRepPayload *payload = NULL;
    payload = OCRepPayloadCreate();
    VERIFY_NOT_NULL(TAG, payload, ERROR);

    VERIFY_NOT_NULL_RETURN(TAG, cloud, WARNING, payload);

    OCRepPayloadSetPropString(payload, OIC_JSON_CLOUD_APN, cloud->apn);
    OCRepPayloadSetPropString(payload, OIC_JSON_CLOUD_CIS, cloud->cis);
    OCRepPayloadSetPropString(payload, OIC_JSON_CLOUD_SID, cloud->sid);
    OCRepPayloadSetPropInt(payload, OIC_JSON_CLOUD_CLEC, (int64_t)cloud->stat);

exit:
    OIC_LOG_V(DEBUG, TAG, "%s: OUT", __func__);
    return payload;
}

static OCEntityHandlerResult HandleCloudGetRequest(OCEntityHandlerRequest *ehRequest)
{
    OIC_LOG_V(DEBUG, TAG, "%s: IN", __func__);
    OCEntityHandlerResult ehRet = OC_EH_INTERNAL_SERVER_ERROR;
    OicCloud_t *cloud = NULL;
    OCRepPayload *payload = NULL;
    bool isDeviceOwned = false;
    OicCloud_t *p1 = NULL, *p2 = NULL;
    OCEntityHandlerResponse response;

    VERIFY_NOT_NULL(TAG, ehRequest, ERROR);
    VERIFY_NOT_NULL(TAG, gCloud, ERROR);

    OCGetDeviceOwnedState(&isDeviceOwned);

    if (!isDeviceOwned)
    {
        OIC_LOG_V(ERROR, TAG, "%s: device is not owned", __func__);
        ehRet = OC_EH_NOT_ACCEPTABLE;
        goto exit;
    }

    cloud = (OicCloud_t *) OICCalloc(1, sizeof(OicCloud_t));
    VERIFY_NOT_NULL(TAG, cloud, ERROR);

    payload = (OCRepPayload *)ehRequest->payload;

    if (!payload || !OCRepPayloadGetPropString(payload, OC_CLOUD_PROVISIONING_CIS, &cloud->cis))
    {
        OIC_LOG_V(ERROR, TAG, "%s: Can't get: %s", __func__, OC_CLOUD_PROVISIONING_CIS);
        p1 = gCloud;
        ehRet = OC_EH_OK;
        goto exit;
    }
    if (!OCRepPayloadGetPropString(payload, OC_CLOUD_PROVISIONING_APN, &cloud->apn))
    {
        OIC_LOG_V(WARNING, TAG, "%s: Can't get: %s", __func__, OC_CLOUD_PROVISIONING_APN);
    }
    if (!OCRepPayloadGetPropString(payload, OC_CLOUD_PROVISIONING_AT, &cloud->at))
    {
        OIC_LOG_V(WARNING, TAG, "%s: Can't get: %s", __func__, OC_CLOUD_PROVISIONING_AT);
    }

    LL_FOREACH_SAFE(gCloud, p1, p2)
    {
        if (0 == strcmp(p1->cis, cloud->cis))
        {
            ehRet = OC_EH_OK;
            break;
        }
    }
exit:
    payload = CreateCloudGetPayload(p1);

    response.requestHandle = ehRequest->requestHandle;
    response.ehResult = payload ? ehRet : OC_EH_INTERNAL_SERVER_ERROR;
    response.payload = (OCPayload *)payload;
    response.payload->type = PAYLOAD_TYPE_REPRESENTATION;
    response.persistentBufferFlag = 0;

    if (OC_STACK_OK != OCDoResponse(&response))
    {
        OIC_LOG_V(ERROR, TAG, "%s: send response", __func__);
        ehRet = OC_EH_ERROR;
    }

    FreeCloud(cloud);

    OIC_LOG_V(DEBUG, TAG, "%s: OUT", __func__);

    return ehRet;
}

static OCEntityHandlerResult HandleCloudDeleteRequest(OCEntityHandlerRequest *ehRequest)
{
    OIC_LOG_V(DEBUG, TAG, "%s: IN", __func__);
    OCEntityHandlerResult ehRet = OC_EH_INTERNAL_SERVER_ERROR;
    OicCloud_t *cloud = NULL;
    OCRepPayload *payload = NULL;
    bool isDeviceOwned = false;
    OicCloud_t *p1 = NULL, *p2 = NULL;
    OCEntityHandlerResponse response;

    VERIFY_NOT_NULL(TAG, ehRequest, ERROR);
    VERIFY_NOT_NULL(TAG, ehRequest->payload, ERROR);

    OCGetDeviceOwnedState(&isDeviceOwned);

    if (!isDeviceOwned)
    {
        OIC_LOG_V(ERROR, TAG, "%s: device is not owned", __func__);
        ehRet = OC_EH_NOT_ACCEPTABLE;
        goto exit;
    }

    cloud = (OicCloud_t *) OICCalloc(1, sizeof(OicCloud_t));
    VERIFY_NOT_NULL(TAG, cloud, ERROR);

    payload = (OCRepPayload *)ehRequest->payload;

    if (!OCRepPayloadGetPropString(payload, OC_CLOUD_PROVISIONING_CIS, &cloud->cis))
    {
        OIC_LOG_V(ERROR, TAG, "%s: Can't get: %s", __func__, OC_CLOUD_PROVISIONING_CIS);
        goto exit;
    }
    if (!OCRepPayloadGetPropString(payload, OC_CLOUD_PROVISIONING_APN, &cloud->apn))
    {
        OIC_LOG_V(ERROR, TAG, "%s: Can't get: %s", __func__, OC_CLOUD_PROVISIONING_APN);
    }
    if (!OCRepPayloadGetPropString(payload, OC_CLOUD_PROVISIONING_AT, &cloud->at))
    {
        OIC_LOG_V(ERROR, TAG, "%s: Can't get: %s", __func__, OC_CLOUD_PROVISIONING_AT);
    }

    LL_FOREACH_SAFE(gCloud, p1, p2)
    {
        if (0 == strcmp(p1->cis, cloud->cis))
        {
            OIC_LOG_V(INFO, TAG, "%s: delete cloud: %s", __func__, p1->cis);
            p1->stat = OC_CLOUD_EXIT;
            LL_DELETE(gCloud, p1);
            FreeCloud(p1);
            ehRet = OC_EH_OK;
            break;
        }
    }
exit:
    response.requestHandle = ehRequest->requestHandle;
    response.ehResult = ehRet;
    response.payload = (OCPayload *)OCRepPayloadCreate();
    response.payload->type = PAYLOAD_TYPE_REPRESENTATION;
    response.persistentBufferFlag = 0;

    if (OC_STACK_OK != OCDoResponse(&response))
    {
        OIC_LOG_V(ERROR, TAG, "%s: send response", __func__);
        ehRet = OC_EH_ERROR;
    }

    FreeCloud(cloud);

    OIC_LOG_V(DEBUG, TAG, "%s: OUT", __func__);

    return ehRet;
}


OCEntityHandlerResult CloudEntityHandler(OCEntityHandlerFlag flag,
        OCEntityHandlerRequest *ehRequest, void *callbackParam)
{
    OIC_LOG_V(DEBUG, TAG, "%s: IN", __func__);

    OC_UNUSED(callbackParam);

    VERIFY_NOT_NULL_RETURN(TAG, ehRequest, ERROR, OC_EH_ERROR);

    OCEntityHandlerResult ehRet = OC_EH_ERROR;

    if (flag & OC_REQUEST_FLAG)
    {
        switch (ehRequest->method)
        {
            case OC_REST_POST:
                ehRet = HandleCloudPostRequest(ehRequest);
                break;
            case OC_REST_GET:
                ehRet = HandleCloudGetRequest(ehRequest);
                break;
            case OC_REST_DELETE:
                ehRet = HandleCloudDeleteRequest(ehRequest);
                break;
            default:
                OIC_LOG_V(WARNING, TAG, "%s: Unknow request: %d", __func__, ehRequest->method);
                usleep(300000);
                ehRet = ((SendSRMResponse(ehRequest, ehRet, NULL, 0)) == OC_STACK_OK) ?
                        OC_EH_OK : OC_EH_ERROR;
                break;
        }
    }
    else
    {
        OIC_LOG_V(WARNING, TAG, "%s: Flag  does not includes OC_REQUEST_FLAG", __func__);
    }

    OIC_LOG_V(DEBUG, TAG, "%s: OUT", __func__);
    return ehRet;
}

OCStackResult CreateCloudResource()
{
    OCStackResult ret = OCCreateResource(&gCloudHandle,
                                         OIC_RSRC_TYPE_SEC_CLOUDCONF,
                                         OC_RSRVD_INTERFACE_DEFAULT,
                                         OIC_RSRC_CLOUDCONF_URI,
                                         CloudEntityHandler,
                                         NULL,
                                         OC_SECURE | OC_NONSECURE |
                                         OC_DISCOVERABLE);

    if (OC_STACK_OK != ret)
    {
        OIC_LOG (FATAL, TAG, "Unable to create cloud config resource");
        DeInitCloudResource();
    }
    return ret;
}

OCStackResult DeInitCloudResource()
{
    OIC_LOG_V(DEBUG, TAG, "%s: IN", __func__);

    OCStackResult ret = OCDeleteResource(gCloudHandle);
    if (gCloud  != &gDefaultCloud)
    {
        DeleteCloudList(gCloud);
    }

    oc_mutex_free(gCloudMutex);

    gCloud = NULL;

    OIC_LOG_V(DEBUG, TAG, "%s: IN", __func__);

    if (OC_STACK_OK == ret)
    {
        return OC_STACK_OK;
    }
    else
    {
        return OC_STACK_ERROR;
    }
}

static void StartClouds()
{
    OIC_LOG_V(DEBUG, TAG, "%s: IN", __func__);
    bool isDeviceOwned = false;

    OCGetDeviceOwnedState(&isDeviceOwned);

    if (!isDeviceOwned)
    {
        OIC_LOG_V(ERROR, TAG, "%s: device is not owned", __func__);
        return;
    }

    OicCloud_t *p1 = NULL, *p2 = NULL;
    LL_FOREACH_SAFE(gCloud, p1, p2)
    {
        if (OC_CLOUD_PROV == p1->stat)
        {
            if (OC_STACK_OK != OCCloudSignUp(p1))
            {
                OIC_LOG_V(ERROR, TAG, "%s: cloud sign up", __func__);
            }
        }
    }

    OIC_LOG_V(DEBUG, TAG, "%s: IN", __func__);
}

OCStackResult InitCloudResource()
{
    uint8_t *data = NULL;
    size_t size = 0;

    gCloudMutex = oc_mutex_new();

    if (OC_STACK_OK != GetSecureVirtualDatabaseFromPS(OIC_JSON_CLOUDS_NAME, &data, &size))
    {
        OIC_LOG (WARNING, TAG, "There is no cloud data in database");
    }
    else
    {
        if (data)
        {
            oc_mutex_lock(gCloudMutex);
            if (OC_STACK_OK != CBORPayloadToCloud(data, size, &gCloud))
            {
                OIC_LOG (WARNING, TAG, "There is no cloud data in database");
            }
            else
            {
                StartClouds();
            }
            oc_mutex_unlock(gCloudMutex);
        }
    }

    return CreateCloudResource();
}

bool UpdateCloudPersistentStorage()
{
    OIC_LOG_V(DEBUG, TAG, "%s: IN", __func__);

    VERIFY_NOT_NULL_RETURN(TAG, gCloud, ERROR, false);

    bool ret = false;
    uint8_t *payload = NULL;
    size_t size = 0;
    oc_mutex_lock(gCloudMutex);
    if (OC_STACK_OK != CloudToCBORPayload(gCloud, &payload, &size))
    {
        OIC_LOG_V(ERROR, TAG, "%s: cloud to cbor", __func__);
        goto exit;
    }

    VERIFY_NOT_NULL(TAG, payload, ERROR);

    if (OC_STACK_OK == UpdateSecureResourceInPS(OIC_JSON_CLOUDS_NAME, payload, size))
    {
        ret = true;
    }

    OICFree(payload);
exit:
    OIC_LOG_V(DEBUG, TAG, "%s: OUT", __func__);
    oc_mutex_unlock(gCloudMutex);
    return ret;
}
