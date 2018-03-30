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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

#include "experimental/logger.h"
#include "ocstack.h"
#include "ocpayload.h"
#include "pmutility.h"
#include "oic_malloc.h"
#include "crlresource.h"
#include "ocpayloadcbor.h"
#include "experimental/payload_logging.h"
#include "cacommonutil.h"
#include "secureresourceprovider.h"
#include "srmutility.h"
#include "occloudprovisioning.h"
#include "cloudresource.h"
#include "srmresourcestrings.h"
#include "doxmresource.h"

#define TAG "OIC_CLOUD_CONFIG"

OCStackResult OCProvisionCloudConfig(void *ctx,
                                     const OCProvisionDev_t *pDev,
                                     const OicCloud_t *cloud,
                                     OCClientResponseHandler resultCallback)
{
    OIC_LOG_V(INFO, TAG, "IN %s", __func__);
    OCStackResult ret = OC_STACK_ERROR;
    char *query = NULL;
    OCDoHandle lHandle = NULL;

    VERIFY_NOT_NULL_RETURN(TAG, ctx, ERROR,  OC_STACK_INVALID_PARAM);
    VERIFY_NOT_NULL_RETURN(TAG, pDev, ERROR,  OC_STACK_INVALID_PARAM);
    VERIFY_NOT_NULL_RETURN(TAG, cloud, ERROR,  OC_STACK_INVALID_PARAM);
    VERIFY_NOT_NULL_RETURN(TAG, cloud->cis, ERROR,  OC_STACK_INVALID_PARAM);
    VERIFY_NOT_NULL_RETURN(TAG, cloud->apn, ERROR,  OC_STACK_INVALID_PARAM);
    VERIFY_NOT_NULL_RETURN(TAG, cloud->at, ERROR,  OC_STACK_INVALID_PARAM);
    VERIFY_NOT_NULL_RETURN(TAG, cloud->sid, ERROR,  OC_STACK_INVALID_PARAM);
    VERIFY_NOT_NULL_RETURN(TAG, resultCallback, ERROR,  OC_STACK_INVALID_PARAM);

    OCCallbackData cbData =  {.context = ctx, .cb = resultCallback, .cd = NULL};

    OCRepPayload *payload = OCRepPayloadCreate();
    VERIFY_NOT_NULL(TAG, payload, ERROR);

    OCRepPayloadSetPropString(payload, OIC_JSON_CLOUD_CIS, cloud->cis);
    OCRepPayloadSetPropString(payload, OIC_JSON_CLOUD_APN, cloud->apn);
    OCRepPayloadSetPropString(payload, OIC_JSON_CLOUD_AT, cloud->at);
    OCRepPayloadSetPropString(payload, OIC_JSON_CLOUD_SID, cloud->sid);

    query = (char *)OICCalloc(1, DEFAULT_URI_LENGTH);
    VERIFY_NOT_NULL(TAG, query, ERROR);

    VERIFY_SUCCESS(TAG, PMGenerateQuery(true,
                                        pDev->endpoint.addr,
                                        pDev->securePort,
                                        pDev->connType,
                                        query, DEFAULT_URI_LENGTH, OIC_RSRC_CLOUDCONF_URI), ERROR);


    ret = OCDoResource(&lHandle, OC_REST_POST, query,
                       &pDev->endpoint, (OCPayload *)payload,
                       pDev->connType, OC_HIGH_QOS, &cbData, NULL, 0);
    OIC_LOG_V(DEBUG, TAG, "POST:%s ret:%d", query, ret);
exit:
    OICFree(query);
    if (OC_STACK_OK != ret)
    {
        if (NULL != payload)
        {
            OCPayloadDestroy((OCPayload *)payload);
        }
    }

    OIC_LOG_V(INFO, TAG, "OUT %s", __func__);

    return ret;
}

/**
 * Cloud status response to cloud handle
 *
 * @param[in] ctx                    context
 * @param[in] handle                 handle
 * @param[in] response               response from peer
 * @return  OCStackApplicationResult application result
 */
static OCStackApplicationResult handleCloudStatusResponse(void *ctx,
        OCDoHandle handle,
        OCClientResponse *response)
{
    OIC_LOG_V(DEBUG, TAG, "%s: IN", __func__);

    VERIFY_NOT_NULL_RETURN(TAG, response, ERROR, OC_STACK_DELETE_TRANSACTION);
    VERIFY_NOT_NULL_RETURN(TAG, response->payload, ERROR, OC_STACK_DELETE_TRANSACTION);

    OCRepPayload *payload = (OCRepPayload *)response->payload;
    OicCloud_t *cloud = NULL;

#if !defined(NDEBUG)
    OIC_LOG_PAYLOAD(DEBUG, response->payload);
#endif

    if (PAYLOAD_TYPE_REPRESENTATION == response->payload->type)
    {
        cloud = (OicCloud_t *)OICCalloc(1, sizeof(OicCloud_t));
        if (cloud)
        {
            if (!OCRepPayloadGetPropString(payload, OIC_JSON_CLOUD_APN, &cloud->apn))
            {
                OIC_LOG_V(ERROR, TAG, "%s parse %s", __func__, OIC_JSON_CLOUD_APN);
                goto parse;
            }
            if (!OCRepPayloadGetPropString(payload, OIC_JSON_CLOUD_CIS, &cloud->cis))
            {
                OIC_LOG_V(ERROR, TAG, "%s parse %s", __func__, OIC_JSON_CLOUD_CIS);
                goto parse;
            }
            if (!OCRepPayloadGetPropString(payload, OIC_JSON_CLOUD_SID, &cloud->sid))
            {
                OIC_LOG_V(ERROR, TAG, "%s parse %s", __func__, OIC_JSON_CLOUD_SID);
                goto parse;
            }
            if (!OCRepPayloadGetPropInt(payload, OIC_JSON_CLOUD_CLEC, (int64_t *)(&cloud->stat)))
            {
                OIC_LOG_V(ERROR, TAG, "%s parse %s", __func__, OIC_JSON_CLOUD_CLEC);
                goto parse;
            }

            OIC_LOG_V(DEBUG, TAG, "%s: OUT: cloud status(%d): %s", __func__, cloud->stat,
                      GetCloudStatus(cloud));
parse:
            FreeCloud(cloud);
        }
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "%s unknown payload type: %d", __func__, response->payload->type);
    }

    ((OCClientResponseHandler)ctx)(NULL, handle, response);

    return OC_STACK_DELETE_TRANSACTION;
}

OCStackResult OCGetCloudStatusRequest(void *ctx,
                                      const OCProvisionDev_t *pDev,
                                      const OicCloud_t *cloud,
                                      OCClientResponseHandler resultCallback)
{
    OIC_LOG_V(INFO, TAG, "IN %s", __func__);
    OCStackResult ret = OC_STACK_ERROR;
    char *query = NULL;
    OCDoHandle lHandle = NULL;

    VERIFY_NOT_NULL_RETURN(TAG, ctx, ERROR,  OC_STACK_INVALID_PARAM);
    VERIFY_NOT_NULL_RETURN(TAG, pDev, ERROR,  OC_STACK_INVALID_PARAM);
    VERIFY_NOT_NULL_RETURN(TAG, cloud, ERROR,  OC_STACK_INVALID_PARAM);
    VERIFY_NOT_NULL_RETURN(TAG, cloud->cis, ERROR,  OC_STACK_INVALID_PARAM);
    VERIFY_NOT_NULL_RETURN(TAG, resultCallback, ERROR,  OC_STACK_INVALID_PARAM);

    OCCallbackData cbData =  {.context = (void *)resultCallback, .cb = handleCloudStatusResponse, .cd = NULL};

    query = (char *)OICCalloc(1, DEFAULT_URI_LENGTH);
    VERIFY_NOT_NULL(TAG, query, ERROR);

    VERIFY_SUCCESS(TAG, PMGenerateQuery(true,
                                        pDev->endpoint.addr,
                                        pDev->securePort,
                                        pDev->connType,
                                        query, DEFAULT_URI_LENGTH, OIC_RSRC_CLOUDCONF_URI), ERROR);

    ret = OCDoResource(&lHandle, OC_REST_GET, query,
                       &pDev->endpoint, NULL,
                       pDev->connType, OC_HIGH_QOS, &cbData, NULL, 0);
    OIC_LOG_V(DEBUG, TAG, "GET:%s ret:%d", query, ret);

exit:
    OICFree(query);

    OIC_LOG_V(INFO, TAG, "OUT %s", __func__);

    return ret;
}

/**
 * Callback for CoAPCloudConfig remove.
 */
static OCStackApplicationResult OCRemoveCloudConfigCB(void *ctx, OCDoHandle handle,
        OCClientResponse *clientResponse)
{
    OIC_LOG_V(INFO, TAG, "IN %s", __func__);

    OC_UNUSED(handle);
    OCStackResult ret = OC_STACK_ERROR;
    char *query = NULL;
    const OCProvisionDev_t *pDev = NULL;
    const char *sid = NULL;
    char *buf = NULL;
    OCDoHandle lHandle = NULL;
    int res = 0, bufLen = 0;

    VERIFY_NOT_NULL_RETURN(TAG, ctx, ERROR, OC_STACK_DELETE_TRANSACTION);
    VERIFY_NOT_NULL_RETURN(TAG, clientResponse, ERROR, OC_STACK_DELETE_TRANSACTION);

    VERIFY_SUCCESS_RETURN(TAG, (OC_STACK_RESOURCE_CHANGED == clientResponse->result), ERROR,
                          OC_STACK_DELETE_TRANSACTION);

    Data_t *data = (Data_t *) ctx;
    VERIFY_SUCCESS_RETURN(TAG, (CLOUD_TYPE == data->type), ERROR, OC_STACK_DELETE_TRANSACTION);

    OCCallbackData cbData =  {.context = ctx, .cb = ProvisionCB, .cd = NULL};

    CloudData_t *cloudData = (CloudData_t *) (data->ctx);
    VERIFY_NOT_NULL(TAG, cloudData, ERROR);
    pDev = cloudData->targetDev;
    VERIFY_NOT_NULL(TAG, pDev, ERROR);
    sid = cloudData->sid;
    VERIFY_NOT_NULL(TAG, sid, ERROR);

    query = (char *)OICCalloc(1, DEFAULT_URI_LENGTH);
    VERIFY_NOT_NULL(TAG, query, ERROR);

    bufLen = strlen(OIC_RSRC_ACL2_URI) + strlen("?Subject=") + strlen(sid) + 1;
    buf = (char *)OICCalloc(1, bufLen);
    VERIFY_NOT_NULL(TAG, buf, ERROR);

    res = snprintf(buf, bufLen, "%s%s%s", OIC_RSRC_ACL2_URI, "?Subject=", sid);
    if (res + 1 != bufLen)
    {
        OIC_LOG_V(ERROR, TAG, "%s: error assembling query", __func__);
        goto exit;
    }

    VERIFY_SUCCESS(TAG, PMGenerateQuery(true,
                                        pDev->endpoint.addr,
                                        pDev->securePort,
                                        pDev->connType,
                                        query, DEFAULT_URI_LENGTH, buf), ERROR);

    ret = OCDoResource(&lHandle, OC_REST_DELETE, query,
                       &pDev->endpoint, NULL,
                       pDev->connType, OC_HIGH_QOS, &cbData, NULL, 0);
    OIC_LOG_V(DEBUG, TAG, "%s:query: %s, do resource return: %d", __func__, query, (int)ret);
exit:
    if (ret == OC_STACK_ERROR)
    {
        OIC_LOG_V(ERROR, TAG, "%s exit by error: %d", query, (int)ret);
    }

    OICFree(query);
    OICFree(buf);

    OIC_LOG_V(INFO, TAG, "OUT %s", __func__);

    return OC_STACK_DELETE_TRANSACTION;
}

OCStackResult OCRemoveCloudConfig(void *ctx,
                                  const OCProvisionDev_t *pDev,
                                  const OicCloud_t *cloud,
                                  OCProvisionResultCB resultCallback)
{
    OIC_LOG_V(INFO, TAG, "IN %s", __func__);

    VERIFY_NOT_NULL_RETURN(TAG, pDev, ERROR,  OC_STACK_INVALID_PARAM);
    VERIFY_NOT_NULL_RETURN(TAG, resultCallback, ERROR,  OC_STACK_INVALID_CALLBACK);
    VERIFY_NOT_NULL_RETURN(TAG, cloud, ERROR,  OC_STACK_INVALID_PARAM);

    OCStackResult ret = OC_STACK_ERROR;
    Data_t *data = NULL;
    CloudData_t *cloudData =  NULL;

    OicUuid_t provTooldeviceID =   {{0,}};
    if (OC_STACK_OK != GetDoxmDeviceID(&provTooldeviceID))
    {
        OIC_LOG(ERROR, TAG, "Error while retrieving provisioning tool's device ID");
        return OC_STACK_ERROR;
    }

    data = (Data_t *)OICCalloc(1, sizeof(Data_t));
    VERIFY_NOT_NULL(TAG, data, ERROR);
    data->type = CLOUD_TYPE;

    cloudData = (CloudData_t *)OICCalloc(1, sizeof(CloudData_t));
    VERIFY_NOT_NULL(TAG, cloudData, ERROR);
    data->ctx = cloudData;
    cloudData->ctx = ctx;
    cloudData->targetDev = (OCProvisionDev_t *)pDev;
    cloudData->resultCallback = resultCallback;
    cloudData->numOfResults = 0;
    cloudData->credInfo = NULL;

    cloudData->resArr = (OCProvisionResult_t *)OICCalloc(1, sizeof(OCProvisionResult_t));
    VERIFY_NOT_NULL(TAG, cloudData->resArr, ERROR);

    cloudData->sid = cloud->sid;

    ret = SetDOS(data, DOS_RFPRO, OCRemoveCloudConfigCB);
exit:
    if (OC_STACK_OK != ret)
    {
        FreeData(data);
    }

    OIC_LOG_V(INFO, TAG, "OUT %s", __func__);

    return ret;
}
