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

#define TAG "OIC_CLOUD_CONFIG"

OCStackResult OCProvisionCloudConfig(void *ctx,
                                    const OCProvisionDev_t *pDev,
                                    const OicCloud_t *cloud,
                                    OCClientResponseHandler resultCallback)
{
    OIC_LOG_V(INFO, TAG, "IN %s", __func__);
    OCStackResult ret = OC_STACK_ERROR;
    char *query = NULL;

    VERIFY_NOT_NULL_RETURN(TAG, ctx, ERROR,  OC_STACK_INVALID_PARAM);
    VERIFY_NOT_NULL_RETURN(TAG, pDev, ERROR,  OC_STACK_INVALID_PARAM);
    VERIFY_NOT_NULL_RETURN(TAG, cloud, ERROR,  OC_STACK_INVALID_PARAM);
    VERIFY_NOT_NULL_RETURN(TAG, cloud->cis, ERROR,  OC_STACK_INVALID_PARAM);
    VERIFY_NOT_NULL_RETURN(TAG, cloud->apn, ERROR,  OC_STACK_INVALID_PARAM);
    VERIFY_NOT_NULL_RETURN(TAG, cloud->at, ERROR,  OC_STACK_INVALID_PARAM);
    VERIFY_NOT_NULL_RETURN(TAG, resultCallback, ERROR,  OC_STACK_INVALID_PARAM);

    OCRepPayload* payload = OCRepPayloadCreate();
    VERIFY_NOT_NULL(TAG, payload, ERROR);

    OCRepPayloadSetPropString(payload, OIC_JSON_CLOUD_CIS, cloud->cis);
    OCRepPayloadSetPropString(payload, OIC_JSON_CLOUD_APN, cloud->apn);
    OCRepPayloadSetPropString(payload, OIC_JSON_CLOUD_AT, cloud->at);

    query = OICCalloc(1, DEFAULT_URI_LENGTH);
    VERIFY_NOT_NULL(TAG, query, ERROR);

    VERIFY_SUCCESS(TAG, PMGenerateQuery(true,
                             pDev->endpoint.addr,
                             pDev->securePort,
                             pDev->connType,
                             query, DEFAULT_URI_LENGTH, OIC_RSRC_CLOUDCONF_URI), ERROR);

    OCCallbackData cbData =  {.context = ctx, .cb = resultCallback, .cd = NULL};
    OCDoHandle lHandle = NULL;

    ret = OCDoResource(&lHandle, OC_REST_POST, query,
                                &pDev->endpoint, (OCPayload *)payload,
                                pDev->connType, OC_HIGH_QOS, &cbData, NULL, 0);
    OIC_LOG_V(DEBUG, TAG, "POST:%s ret:%d", query, ret);
exit:
    OICFree(query);
    if (OC_STACK_OK != ret)
    {
        if(NULL != payload)
        {
            OCPayloadDestroy((OCPayload *)payload);
        }
    }

    OIC_LOG_V(INFO, TAG, "OUT %s", __func__);

    return ret;
}
