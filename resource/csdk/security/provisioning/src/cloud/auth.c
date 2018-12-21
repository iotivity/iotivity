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

#include "octypes.h"
#include "experimental/logger.h"
#include "experimental/payload_logging.h"
#include "ocstack.h"
#include "ocpayload.h"
#include "psinterface.h"
#include "experimental/securevirtualresourcetypes.h"
#include "experimental/doxmresource.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "pmutility.h"
#include "credresource.h"
#include "experimental/payload_logging.h"
#include "cacommonutil.h"
#include "srmutility.h"

#include "coap/utlist.h"

#include "utils.h"
#include "cloud/auth.h"
#include "cloud/cloudresource.h"

#include <time.h>

#define TAG "OIC_CLOUD_RESOURCE_AUTH"

#define LOGIN_OK 4

#define SESSION_CBOR_CLOUD_MAP_SIZE 3
#define MAX_STR_LEN 2048

const char *OIC_JSON_CLOUD_SESSION_ACCESS_TOKEN = "accesstoken";
const char *OIC_JSON_CLOUD_SESSION_REFRESH_TOKEN = "refreshtoken";
const char *OIC_JSON_CLOUD_SESSION_UID = "uid";

#if !defined(__MANDATORY__)
#define CBOR_CLOUD_MAP_SIZE 5
const char *OIC_JSON_CLOUD_SESSION_REDIRECT_URL = "redirecturl";
#else
#define CBOR_CLOUD_MAP_SIZE 4
#endif // __MANDATORY__

static const size_t CBOR_DEFAULT_SIZE = MAX_STR_LEN * (CBOR_CLOUD_MAP_SIZE +
                                        SESSION_CBOR_CLOUD_MAP_SIZE);

#define MAX_URI_QUERY MAX_URI_LENGTH + MAX_QUERY_LENGTH

void SessionFree(session_t *session)
{
    if (NULL == session)
    {
        OIC_LOG_V(ERROR, TAG, "%s: session is NULL", __func__);
        return;
    }
    if (session->accessToken)
    {
        memset(session->accessToken, 0, strlen(session->accessToken));
        OICFree(session->accessToken);
        session->accessToken = NULL;
    }
    if (session->refreshToken)
    {
        memset(session->refreshToken, 0, strlen(session->refreshToken));
        OICFree(session->refreshToken);
        session->refreshToken = NULL;
    }
    OICFree(session->uid);
    session->uid = NULL;

    OICFree(session);
    session = NULL;
}

OicCloud_t *FreeCloud(OicCloud_t *cloud)
{
    if (!cloud)
    {
        OIC_LOG_V(WARNING, TAG, "%s:Cloud is null", __func__);
        return NULL;
    }

    StopCloudRefresh(cloud);

    OicCloud_t *ret = cloud->next;
    cloud->next = NULL;

    if (cloud->apn)
    {
        OICFree(cloud->apn);
        cloud->apn = NULL;
    }
    if (cloud->cis)
    {
        OICFree(cloud->cis);
        cloud->cis = NULL;
    }
    if (cloud->at)
    {
        OICFree(cloud->at);
        cloud->at = NULL;
    }
    if (cloud->sid)
    {
        OICFree(cloud->sid);
        cloud->sid = NULL;
    }
    if (cloud->session)
    {
        SessionFree(cloud->session);
        cloud->session = NULL;
    }
#if !defined(__MANDATORY__)
    OICFree(cloud->redirectUri);
    cloud->redirectUri = NULL;
#endif // __MANDATORY__

    OICFree(cloud);
    cloud = NULL;

    return ret;
}

bool strCopy(const char *src, char **dst)
{
    if (!src || !dst)
    {
        return false;
    }
    OICFree(*dst);

    size_t len = strlen(src) + 1;
    *dst = (char *)OICCalloc(1, len);
    if (*dst)
    {
        OICStrcpy(*dst, len, src);
        return true;
    }
    return false;
}

bool CloudCopy(const OicCloud_t *src, OicCloud_t *dst)
{
    if (!strCopy(src->apn, &dst->apn)
        || !strCopy(src->cis, &dst->cis)
        || !strCopy(src->at, &dst->at)
        || !strCopy(src->sid, &dst->sid))
    {
        return false;
    }

    return true;
}

OicCloud_t *CloudFind(OicCloud_t *list, const OicCloud_t *cloud)
{
    if (!list || !cloud || !cloud->cis)
    {
        return NULL;
    }

    OicCloud_t *p1 = NULL, *p2 = NULL;
    LL_FOREACH_SAFE(list, p1, p2)
    {
        if (p1 && p1->cis && 0 == strcmp(p1->cis, cloud->cis))
        {
            return p1;
        }
    }
    return NULL;
}


static size_t CloudsCount(const OicCloud_t *clouds)
{
    size_t size = 0;
    for (const OicCloud_t *cloud = clouds; cloud; cloud = cloud->next)
    {
        size++;
    }
    return size;
}

bool ValidCloud(OicCloud_t *cloud)
{
    OIC_LOG_V(DEBUG, TAG, "%s: IN", __func__);
    VERIFY_NOT_NULL_RETURN(TAG, cloud, ERROR, false);

    bool ret = true;

    if (!cloud->apn)
    {
        OIC_LOG_V(INFO, TAG, "%s: Authorization Provider Name validate: %s", __func__,
                  cloud->apn ? cloud->apn : "NULL");
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

#if !defined(__MANDATORY__)
    if (cloud->redirectUri && 0 != strncmp(cloud->redirectUri, "coaps+tcp://", 11))
    {
        OIC_LOG_V(ERROR, TAG, "%s: OCF Redirect URI validate: %s", __func__,
                  cloud->redirectUri ? cloud->redirectUri : "NULL");
        ret = false;
    }
#endif // __MANDATORY__
    OIC_LOG_V(DEBUG, TAG, "%s: OUT", __func__);
    return ret;
}

OCStackResult CloudSessionToCBORPayload(const session_t *session, CborEncoder *map)
{
    OIC_LOG_V(DEBUG, TAG, "%s: IN", __func__);
    VERIFY_NOT_NULL_RETURN(TAG, session, ERROR, OC_STACK_INVALID_PARAM);
    VERIFY_NOT_NULL_RETURN(TAG, map, ERROR, OC_STACK_INVALID_PARAM);

    OCStackResult ret = OC_STACK_ERROR;

    CborError cborError = CborNoError;

    cborError = cbor_encode_text_string(map, OIC_JSON_CLOUD_SESSION_ACCESS_TOKEN,
                                        sizeof(OIC_JSON_CLOUD_SESSION_ACCESS_TOKEN) + 1);
    VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborError, "Failed add tag: access token");
    cborError = cbor_encode_text_string(map, session->accessToken, strnlen(session->accessToken,
                                        MAX_STR_LEN));
    VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborError, "Failed add value: access token");

    cborError = cbor_encode_text_string(map, OIC_JSON_CLOUD_SESSION_REFRESH_TOKEN,
                                        sizeof(OIC_JSON_CLOUD_SESSION_REFRESH_TOKEN) + 1);
    VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborError, "Failed add tag: refresh token");
    cborError = cbor_encode_text_string(map, session->refreshToken, strnlen(session->refreshToken,
                                        MAX_STR_LEN));
    VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborError, "Failed add value: refresh token");

    cborError = cbor_encode_text_string(map, OIC_JSON_CLOUD_SESSION_UID,
                                        sizeof(OIC_JSON_CLOUD_SESSION_UID) + 1);
    VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborError, "Failed add tag: uid");
    cborError = cbor_encode_text_string(map, session->uid, strnlen(session->uid, MAX_STR_LEN));
    VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborError, "Failed add value: uid");

    ret = OC_STACK_OK;
exit:
    if (CborNoError != cborError)
    {
        OIC_LOG_V(ERROR, TAG, "%s: Failed", __func__);
        ret = OC_STACK_ERROR;
    }

    OIC_LOG_V(DEBUG, TAG, "%s: OUT", __func__);
    return ret;
}

static OCStackResult CloudToCBORPayloadInternal(const OicCloud_t *clouds, uint8_t **payload,
        size_t *size, bool secure)
{
    OIC_LOG_V(DEBUG, TAG, "%s: IN", __func__);

    VERIFY_NOT_NULL_RETURN(TAG, clouds, ERROR, OC_STACK_INVALID_PARAM);
    VERIFY_NOT_NULL_RETURN(TAG, size, ERROR, OC_STACK_INVALID_PARAM);
    VERIFY_NOT_NULL_RETURN(TAG, payload, ERROR, OC_STACK_INVALID_PARAM);

    if (NULL != *payload)
    {
        OIC_LOG_V(ERROR, TAG, "%s: *payload is not NULL", __func__);
        return OC_STACK_INVALID_PARAM;
    }

    size_t cborLen = *size;
    if (0 == cborLen)
    {
        cborLen = CBOR_DEFAULT_SIZE;
    }
    *payload = NULL;
    *size = 0;
    OicCloud_t *cloud = (OicCloud_t *)clouds;

    OCStackResult ret = OC_STACK_ERROR;

    CborError cborError = CborNoError;

    uint8_t *outPayload = (uint8_t *)OICCalloc(1, cborLen);
    VERIFY_NOT_NULL_RETURN(TAG, outPayload, ERROR, OC_STACK_ERROR);

    CborEncoder encoder;
    cbor_encoder_init(&encoder, outPayload, cborLen, 0);

    CborEncoder rootMap;
    cborError = cbor_encoder_create_map(&encoder, &rootMap, 1);
    VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborError, "Failed add root map");

    cborError = cbor_encode_text_string(&rootMap, OIC_JSON_CLOUDS_NAME,
                                        sizeof(OIC_JSON_CLOUDS_NAME) + 1);
    VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborError, "Failed add tag clouds");

    CborEncoder cloudArray;
    cborError = cbor_encoder_create_array(&rootMap, &cloudArray, CloudsCount(clouds));
    VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborError, "Failed add array");

    while (cloud)
    {
        CborEncoder map;
        size_t mapSize = CBOR_CLOUD_MAP_SIZE;
        if (secure)
        {
            mapSize += SESSION_CBOR_CLOUD_MAP_SIZE + 1;
        }

        if (NULL == cloud->apn)
        {
            mapSize--;
        }
#if !defined(__MANDATORY__)
        if (NULL == cloud->redirectUri)
        {
            mapSize--;
        }
#endif

        cborError = cbor_encoder_create_map(&cloudArray, &map, mapSize);
        VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborError, "Failed add map");

        if (NULL != cloud->apn)
        {
            cborError = cbor_encode_text_string(&map, OIC_JSON_CLOUD_APN,
                                            sizeof(OIC_JSON_CLOUD_APN) + 1);
            VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborError, "Failed add tag: apn");
            cborError = cbor_encode_text_string(&map, cloud->apn, strnlen(cloud->apn, MAX_STR_LEN));
            VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborError, "Failed add value: apn");
        }
        cborError = cbor_encode_text_string(&map, OIC_JSON_CLOUD_CIS,
                                            sizeof(OIC_JSON_CLOUD_CIS) + 1);
        VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborError, "Failed add tag: cis");
        cborError = cbor_encode_text_string(&map, cloud->cis, strnlen(cloud->cis, MAX_STR_LEN));
        VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborError, "Failed add value: cis");

        if (secure)
        {
            cborError = cbor_encode_text_string(&map, OIC_JSON_CLOUD_AT,
                                                sizeof(OIC_JSON_CLOUD_AT) + 1);
            VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborError, "Failed add tag: at");
            cborError = cbor_encode_text_string(&map, cloud->at, strnlen(cloud->at, MAX_STR_LEN));
            VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborError, "Failed add value: at");
        }

        cborError = cbor_encode_text_string(&map, OIC_JSON_CLOUD_SID,
                                            sizeof(OIC_JSON_CLOUD_SID) + 1);
        VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborError, "Failed add tag: sid");
        cborError = cbor_encode_text_string(&map, cloud->sid, strnlen(cloud->sid, MAX_STR_LEN));
        VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborError, "Failed add value: sid");

        cborError = cbor_encode_text_string(&map, OIC_JSON_CLOUD_CLEC,
                                            sizeof(OIC_JSON_CLOUD_CLEC) + 1);
        VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborError, "Failed add tag: clec");
        cborError = cbor_encode_int(&map, cloud->stat);
        VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborError, "Failed add value: clec");

        if (secure)
        {
            if (cloud->session)
            {
                ret = CloudSessionToCBORPayload(cloud->session, &map);
                if (OC_STACK_OK != ret)
                {
                    OIC_LOG_V(ERROR, TAG, "%s: Failed convert session to cbor", __func__);
                    goto exit;
                }
            }
        }

#if !defined(__MANDATORY__)
        if (NULL != cloud->redirectUri)
        {
            cborError = cbor_encode_text_string(&map, OIC_JSON_CLOUD_SESSION_REDIRECT_URL,
                                    sizeof(OIC_JSON_CLOUD_SESSION_REDIRECT_URL) + 1);
            VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborError, "Failed add tag: redirect uri");
            cborError = cbor_encode_text_string(&map, cloud->redirectUri, strnlen(cloud->redirectUri, MAX_STR_LEN));
            VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborError, "Failed add value: redirect uri");
        }
#endif // __MANDATORY__

        cborError = cbor_encoder_close_container(&cloudArray, &map);
        VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborError, "Failed close cloud map");

        cloud = cloud->next;
    }
    cborError = cbor_encoder_close_container(&rootMap, &cloudArray);
    VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborError, "Failed close array");

    cborError = cbor_encoder_close_container(&encoder, &rootMap);
    VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborError, "Failed close root map");

    if (CborNoError == cborError)
    {
        *size = cbor_encoder_get_buffer_size(&encoder, outPayload);
        *payload = outPayload;
        ret = OC_STACK_OK;
    }
exit:
    if ((CborNoError != cborError) || (OC_STACK_OK != ret))
    {
        OIC_LOG_V(ERROR, TAG, "%s: Failed", __func__);
        OICFree(outPayload);
        outPayload = NULL;
        *payload = NULL;
        *size = 0;
        ret = OC_STACK_ERROR;
    }

    OIC_LOG_V(DEBUG, TAG, "%s: OUT", __func__);
    return ret;
}

OCStackResult CloudToCBORPayload(const OicCloud_t *clouds, uint8_t **payload, size_t *size)
{
    return CloudToCBORPayloadInternal(clouds, payload, size, true);
}

OCStackResult CloudToCBORPayloadResource(const OicCloud_t *clouds, uint8_t **payload, size_t *size)
{
    return CloudToCBORPayloadInternal(clouds, payload, size, false);
}


OCStackResult CBORPayloadToCloud(const uint8_t *cborPayload, size_t size, OicCloud_t **clouds)
{
    OIC_LOG_V(DEBUG, TAG, "%s: IN", __func__);

    VERIFY_NOT_NULL_RETURN(TAG, cborPayload, ERROR, OC_STACK_INVALID_PARAM);
    VERIFY_NOT_NULL_RETURN(TAG, clouds, ERROR, OC_STACK_INVALID_PARAM);

    if (NULL != *clouds)
    {
        OIC_LOG_V(ERROR, TAG, "%s: *payload is not NULL", __func__);
        return OC_STACK_INVALID_PARAM;
    }

    if (0 == size)
    {
        OIC_LOG_V(ERROR, TAG, "%s: zero size", __func__);
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult ret = OC_STACK_ERROR;
    CborValue cloudCbor = { NULL, NULL, 0, 0, 0, 0};
    CborParser parser = { NULL, 0 };
    CborError cborError = CborNoError;
    char *mainSecName = NULL;
    size_t len = 0;

    cbor_parser_init(cborPayload, size, 0, &parser, &cloudCbor);

    if (!cbor_value_is_container(&cloudCbor))
    {
        OIC_LOG_V(ERROR, TAG, "%s: it's not container", __func__);
        return OC_STACK_ERROR;
    }

    CborValue rootMap = { NULL, NULL, 0, 0, 0, 0 };
    cborError = cbor_value_enter_container(&cloudCbor, &rootMap);
    VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborError, "Cant't find root map");

    while (cbor_value_is_valid(&rootMap))
    {
        CborType type = cbor_value_get_type(&rootMap);
        if (type == CborTextStringType && cbor_value_is_text_string(&rootMap))
        {
            cborError = cbor_value_dup_text_string(&rootMap, &mainSecName, &len, NULL);
            VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborError, "Failed find name root map");
            cborError = cbor_value_advance(&rootMap);
            VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborError, "Failed get value in root map");
        }
        else
        {
            OIC_LOG_V(ERROR, TAG, "%s: it's not string type", __func__);
        }

        if (0 == len || !mainSecName || strcmp(mainSecName, OIC_JSON_CLOUDS_NAME) != 0)
        {
            OIC_LOG_V(ERROR, TAG, "%s: Failed get main section", __func__);
            continue;
        }

        OICFree(mainSecName);

        CborValue cloudArray = { NULL, NULL, 0, 0, 0, 0 };
        cborError = cbor_value_enter_container(&rootMap, &cloudArray);
        VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborError, "Failed find cloud array");

        while (cbor_value_is_valid(&cloudArray))
        {

            CborValue map = { NULL, NULL, 0, 0, 0, 0 };
            cborError = cbor_value_enter_container(&cloudArray, &map);
            VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborError, "Failed find cloud map");

            OicCloud_t *cloud = (OicCloud_t *) OICCalloc(1, sizeof(OicCloud_t));
            VERIFY_NOT_NULL(TAG, cloud, ERROR);

            cloud->session = (session_t *) OICCalloc(1, sizeof(session_t));
            VERIFY_NOT_NULL(TAG, cloud->session, ERROR);

            cloud->stat = OC_CLOUD_OK;

            LL_APPEND(*clouds, cloud);

            char *name = NULL;
            while (cbor_value_is_valid(&map) && cbor_value_is_text_string(&map))
            {
                CborType cmType = cbor_value_get_type(&map);
                if (cmType == CborTextStringType)
                {
                    cborError = cbor_value_dup_text_string(&map, &name, &len, NULL);
                    VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborError, "Failed find name in map");
                    cborError = cbor_value_advance(&map);
                    VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborError, "Failed advance map");
                }

                if (!name)
                {
                    if (cbor_value_is_valid(&map))
                    {
                        cborError = cbor_value_advance(&map);
                        VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborError, "Failed find root map");
                    }
                    continue;
                }

                if (strncmp(name, OIC_JSON_CLOUD_APN, len)  == 0)
                {
                    cborError = cbor_value_dup_text_string(&map, &cloud->apn, &len, NULL);
                    VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborError, "Failed get apn value");
                    goto next;
                }
                else if (strncmp(name, OIC_JSON_CLOUD_CIS, len)  == 0)
                {
                    cborError = cbor_value_dup_text_string(&map, &cloud->cis, &len, NULL);
                    VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborError, "Failed get cis value");
                    goto next;
                }
                else if (strncmp(name, OIC_JSON_CLOUD_AT, len)  == 0)
                {
                    cborError = cbor_value_dup_text_string(&map, &cloud->at, &len, NULL);
                    VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborError, "Failed get at value");
                    goto next;
                }
                else if (strncmp(name, OIC_JSON_CLOUD_SID, len)  == 0)
                {
                    cborError = cbor_value_dup_text_string(&map, &cloud->sid, &len, NULL);
                    VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborError, "Failed get sid value");
                    goto next;
                }
                else if (strncmp(name, OIC_JSON_CLOUD_SESSION_ACCESS_TOKEN, len)  == 0)
                {
                    cborError = cbor_value_dup_text_string(&map, &cloud->session->accessToken, &len, NULL);
                    VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborError, "Failed get access token value");
                    goto next;
                }
                else if (strncmp(name, OIC_JSON_CLOUD_SESSION_REFRESH_TOKEN, len)  == 0)
                {
                    cborError = cbor_value_dup_text_string(&map, &cloud->session->refreshToken, &len, NULL);
                    VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborError, "Failed get refresh token value");
                    goto next;
                }
                else if (strncmp(name, OIC_JSON_CLOUD_SESSION_UID, len)  == 0)
                {
                    cborError = cbor_value_dup_text_string(&map, &cloud->session->uid, &len, NULL);
                    VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborError, "Failed get uid value");
                    goto next;
                }
#if !defined(__MANDATORY__)
                else if (strncmp(name, OIC_JSON_CLOUD_SESSION_REDIRECT_URL, len)  == 0)
                {
                    cborError = cbor_value_dup_text_string(&map, &cloud->redirectUri, &len, NULL);
                    VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborError, "Failed get redirect uri");
                    goto next;
                }
#endif // __MANDATORY__
                else
                {
                    OIC_LOG_V(ERROR, TAG, "%s: not found: %s", __func__, name);
                }

next:
                if (cbor_value_is_valid(&map))
                {
                    cborError = cbor_value_advance(&map);
                    VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborError, "Failed find map");
                }
                free(name);
                name = NULL;
            }

            cloud->next = NULL;
            if (cbor_value_is_valid(&cloudArray))
            {
                cborError = cbor_value_advance(&cloudArray);
                VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborError, "Failed advance cloud array");
            }
        }

        if (cbor_value_is_valid(&rootMap))
        {
            cborError = cbor_value_advance(&rootMap);
            VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborError, "Failed advance root map");
        }
    }
    ret = OC_STACK_OK;
exit:
    if ((CborNoError != cborError) || (OC_STACK_OK != ret))
    {
        OIC_LOG_V(ERROR, TAG, "%s: Failed", __func__);
        FreeCloud(*clouds);
        *clouds = NULL;
    }
    OIC_LOG_V(DEBUG, TAG, "%s: OUT", __func__);
    return ret;
}


extern OCStackResult OCCloudTokenRefresh(OicCloud_t *cloud);
/**
 * Sends token refreshto cloud
 *
 * @param[in] ctx                    context
 * @param[in] handle                 handle
 * @param[in] response               response from peer
 * @return  OCStackApplicationResult application result
 */
static OCStackApplicationResult handleCloudTokenRefreshResponse(void *ctx,
        OCDoHandle handle,
        OCClientResponse *response)
{
    OIC_LOG_V(DEBUG, TAG, "%s: IN", __func__);

    OC_UNUSED(handle);

    VERIFY_NOT_NULL_RETURN(TAG, response, ERROR, OC_STACK_DELETE_TRANSACTION);

#if !defined(NDEBUG)
    OIC_LOG_PAYLOAD(DEBUG, response->payload);
#endif

    OicCloud_t *cloud = (OicCloud_t *)ctx;
    VERIFY_NOT_NULL_RETURN(TAG, cloud, ERROR, OC_STACK_DELETE_TRANSACTION);
    VERIFY_NOT_NULL_RETURN(TAG, cloud->session, ERROR, OC_STACK_DELETE_TRANSACTION);

    if (response->result)
    {
        OIC_LOG_V(INFO, TAG, "%s: result response: %d", __func__, response->result);
        if (OC_STACK_RESOURCE_CHANGED < response->result)
        {
            OIC_LOG_V(ERROR, TAG, "%s: refresh token error", __func__);
            cloud->stat = OC_CLOUD_ERROR_REFRESHTOKEN;
        }
    }

    VERIFY_NOT_NULL_RETURN(TAG, response->payload, ERROR, OC_STACK_DELETE_TRANSACTION);

    if (!OCRepPayloadGetPropString((OCRepPayload *)response->payload, OC_RSRVD_REFRESH_TOKEN,
                                   &cloud->session->refreshToken))
    {
        OIC_LOG_V(ERROR, TAG, "Can't get: %s", OC_RSRVD_REFRESH_TOKEN);
    }
    if (!OCRepPayloadGetPropString((OCRepPayload *)response->payload, OC_RSRVD_ACCESS_TOKEN,
                                   &cloud->session->accessToken))
    {
        OIC_LOG_V(ERROR, TAG, "Can't get: %s", OC_RSRVD_ACCESS_TOKEN);
    }
    if (!OCRepPayloadGetPropInt((OCRepPayload *)response->payload, OC_RSRVD_EXPIRES_IN,
                                &cloud->session->expireSin))
    {
        OIC_LOG_V(ERROR, TAG, "Can't get: %s", OC_RSRVD_EXPIRES_IN);
    }

    OIC_LOG_V(DEBUG, TAG, "%s: OUT", __func__);
    return OC_STACK_DELETE_TRANSACTION;
}

OCStackResult OCCloudTokenRefresh(OicCloud_t *cloud)
{
    OIC_LOG_V(DEBUG, TAG, "%s: IN", __func__);

    char uri[MAX_URI_LENGTH] = { 0 };
    OCStackResult ret = OC_STACK_OK;

    VERIFY_NOT_NULL_RETURN(TAG, cloud, ERROR, OC_STACK_INVALID_PARAM);
    if (OC_CLOUD_TOKEN_REFRESH4 < cloud->stat)
    {
        OIC_LOG_V(DEBUG, TAG, "%s: invalid cloud status: %s", __func__, GetCloudStatus(cloud));
        return OC_STACK_INVALID_PARAM;
    }
    VERIFY_NOT_NULL_RETURN(TAG, cloud->cis, ERROR, OC_STACK_INVALID_PARAM);
    VERIFY_NOT_NULL_RETURN(TAG, cloud->session, ERROR, OC_STACK_INVALID_PARAM);
    VERIFY_NOT_NULL_RETURN(TAG, cloud->session->uid, ERROR, OC_STACK_INVALID_PARAM);
    VERIFY_NOT_NULL_RETURN(TAG, cloud->session->refreshToken, ERROR, OC_STACK_INVALID_PARAM);

    char *deviceId = getDeviceId();
    VERIFY_NON_NULL_RET(deviceId, TAG, "Can't get the device id", OC_STACK_ERROR);

    snprintf(uri, MAX_URI_LENGTH, "%s%s", cloud->cis, OC_RSRVD_ACCOUNT_TOKEN_REFRESH_URI);

    OCRepPayload *payload = OCRepPayloadCreate();
    VERIFY_NON_NULL_RET(payload, TAG, "Failed to allocate payload", OC_STACK_NO_MEMORY);

    OCRepPayloadSetPropString(payload, OC_RSRVD_USER_UUID, cloud->session->uid);
    OCRepPayloadSetPropString(payload, OC_RSRVD_DEVICE_ID, deviceId);
    OCRepPayloadSetPropString(payload, OC_RSRVD_REFRESH_TOKEN, cloud->session->refreshToken);

    OICFree(deviceId);

    OCCallbackData cbData =
    {
        .context = (void *)cloud,
        .cb = handleCloudTokenRefreshResponse,
        .cd = NULL
    };

    if (OC_CLOUD_TOKEN_REFRESH4 < cloud->stat)
    {
        OIC_LOG_V(DEBUG, TAG, "%s: token refresh cloud: %s break", __func__, cloud->apn);
    }
    else
    {
        ret = OCDoResource(NULL, OC_REST_POST, uri, NULL, (OCPayload *)payload,
                           CT_ADAPTER_TCP, OC_LOW_QOS, &cbData, NULL, 0);
    }

    OIC_LOG_V(DEBUG, TAG, "%s: OUT: %d", __func__, (int)ret);

    return ret;
}

static void *CloudTokenRefresh(void *data)
{
    OIC_LOG_V(DEBUG, TAG, "%s: IN", __func__);

    long ts_sec_eta = 0L;

    struct timespec second =
    {
        .tv_sec  = 1L,
        .tv_nsec = 0L
    };

    OicCloud_t *cloud = (OicCloud_t *)data;
    VERIFY_NOT_NULL(TAG, cloud, ERROR);
    VERIFY_NOT_NULL(TAG, cloud->session, ERROR);

    OIC_LOG_V(INFO, TAG, "%s: cloud(%s) refresh start", __func__, cloud->cis);

    ts_sec_eta = cloud->session->expireSin;
    cloud->stat = OC_CLOUD_TOKEN_REFRESH0;

    while (OC_CLOUD_TOKEN_REFRESH4 > cloud->stat)
    {
        long ts_sec = ts_sec_eta;;
        while (ts_sec--)
        {
            nanosleep(&second, NULL);

            VERIFY_NOT_NULL(TAG, cloud->session, ERROR);

            if (cloud->session->expireSin != ts_sec_eta)
            {
                if (-1 == cloud->session->expireSin)
                {
                    OIC_LOG_V(DEBUG, TAG, "%s: cloud(%s) expireSin: -1", __func__, cloud->cis);
                    goto exit;
                }
                ts_sec = ts_sec_eta = cloud->session->expireSin;
            }

            if (OC_CLOUD_TOKEN_REFRESH4 < cloud->stat)
            {
                break;
            }
        }

        if (OC_STACK_OK != OCCloudTokenRefresh(cloud))
        {
            OIC_LOG_V(ERROR, TAG, "%s: token refresh", __func__);

            if (OC_CLOUD_TOKEN_REFRESH4 >= cloud->stat)
            {
                cloud->stat = (CloudStatus)(cloud->stat + 1);
            }
        }
    }

exit:
    OIC_LOG_V(DEBUG, TAG, "%s: OUT", __func__);
    return NULL;
}

void StopCloudRefresh(OicCloud_t *cloud)
{
    if (!cloud)
    {
        OIC_LOG_V(WARNING, TAG, "%s:Cloud is null", __func__);
        return;
    }

    OIC_LOG_V(DEBUG, TAG, "%s: cloud (%s) state: %s", __func__, cloud->cis, GetCloudStatus(cloud));
    cloud->stat = OC_CLOUD_EXIT;

    if (cloud->pid)
    {
        OIC_LOG_V(DEBUG, TAG, "oc_thread_waiting for cloud %s/%s", cloud->cis, cloud->apn);
        OCThreadResult_t res = oc_thread_wait(cloud->pid);

        if (OC_THREAD_SUCCESS != res)
        {
            OIC_LOG_V(ERROR, TAG, "oc_thread_wait failed - error %u", res);
        }

        res = oc_thread_free(cloud->pid);
        cloud->pid = NULL;

        if (OC_THREAD_SUCCESS != res)
        {
            OIC_LOG_V(ERROR, TAG, "oc_thread_free failed - error %u", res);
        }
        OIC_LOG_V(DEBUG, TAG, "thread for cloud %s/%s is stopped", cloud->cis, cloud->apn);
    }
}

/**
 * Sign In request to cloud handle
 *
 * @param[in] ctx                    context
 * @param[in] handle                 handle
 * @param[in] response               response from peer
 * @return  OCStackApplicationResult application result
 */
static OCStackApplicationResult handleCloudSignInResponse(void *ctx,
        OCDoHandle handle,
        OCClientResponse *response)
{
    OIC_LOG_V(DEBUG, TAG, "%s: IN", __func__);

    OC_UNUSED(handle);

    OicCloud_t *cloud = (OicCloud_t *)ctx;

    char *ipv6End = NULL;
    char *percentChar = NULL;
    char *cis = NULL;
    char *ipv6Intf = NULL;

    VERIFY_NOT_NULL_RETURN(TAG, cloud, ERROR, OC_STACK_DELETE_TRANSACTION);
    VERIFY_NOT_NULL_RETURN(TAG, response, ERROR, OC_STACK_DELETE_TRANSACTION);

    if (LOGIN_OK != response->result)
    {
        OIC_LOG_V(ERROR, TAG, "%s: Login error: %d", __func__, response->result);
        if ( OC_STACK_COMM_ERROR == response->result || OC_STACK_TIMEOUT == response->result )
        {
            OIC_LOG_V(ERROR, TAG, "%s: Unreachable host or network time out", __func__);
            cloud->stat = OC_CLOUD_ERROR_UNREACHABLE;
        }
        else if ( OC_STACK_ERROR == response->result )
        {
            OIC_LOG_V(ERROR, TAG, "%s: TLS error or other network stack error", __func__);
            cloud->stat = OC_CLOUD_ERROR_TLS;
        }
        else
        {
            OIC_LOG_V(ERROR, TAG, "%s: Authorization error", __func__);
            cloud->stat = OC_CLOUD_ERROR_SIGNUP;
        }
        goto exit;
    }

    VERIFY_NOT_NULL_RETURN(TAG, response->payload, ERROR, OC_STACK_DELETE_TRANSACTION);

#if !defined(NDEBUG)
    OIC_LOG_PAYLOAD(DEBUG, response->payload);
#endif

    VERIFY_NOT_NULL_RETURN(TAG, cloud->session, ERROR, OC_STACK_DELETE_TRANSACTION);

    if (!OCRepPayloadGetPropInt((OCRepPayload *)response->payload, OC_RSRVD_EXPIRES_IN,
                                &cloud->session->expireSin))
    {
        OIC_LOG_V(ERROR, TAG, "%s: Can't get: %s", __func__, OC_RSRVD_EXPIRES_IN);
    }

    OIC_LOG(INFO, TAG, "Sign In OK");

    if (!UpdateCloudPersistentStorage())
    {
        OIC_LOG_V(WARNING, TAG, "%s: update cloud's persistent storage", __func__);
    }

    if (OC_CLOUD_SIGNIN == cloud->stat)
    {
        // find the IPv6 address end bracket
        ipv6End = strchr(cloud->cis, ']');
        if (NULL != ipv6End)
        {
            ipv6Intf = strchr(cloud->cis, '%');
            if (ipv6Intf == NULL)
            {
                // find the interface name from UDP address of sender
                percentChar = strchr(response->devAddr.addr, '%');
                size_t ifLen = strlen(percentChar);
                size_t addrLen = strlen(cloud->cis);
                size_t cisLen = addrLen + ifLen + 3;

                // fill the cloud uri with interface name inserted
                cis = (char *)OICMalloc(sizeof(char) * cisLen);
                OICStrcpy(cis, ipv6End - cloud->cis + 1, cloud->cis);
                OICStrcat(cis, cisLen, "%25");
                OICStrcat(cis, cisLen, percentChar + 1);
                OICStrcat(cis, cisLen, ipv6End);

                OICFree(cloud->cis);
                cloud->cis = cis;
            }
        }

        OCThreadResult_t res = OC_THREAD_SUCCESS;
        res = oc_thread_new(&cloud->pid, CloudTokenRefresh, cloud);
        if (OC_THREAD_SUCCESS != res)
        {
            OIC_LOG_V(ERROR, TAG, "%s: create thread: %d", __func__, res);
        }
    }
    else
    {
        cloud->stat = OC_CLOUD_ERROR_SIGNIN;
        OIC_LOG_V(ERROR, TAG, "%s: cloud status: %s", __func__, GetCloudStatus(cloud));
    }
exit:
    OIC_LOG_V(DEBUG, TAG, "%s: OUT", __func__);
    return OC_STACK_DELETE_TRANSACTION;
}

/**
 * Sign Out request to cloud handle
 *
 * @param[in] ctx                    context
 * @param[in] handle                 handle
 * @param[in] response               response from peer
 * @return  OCStackApplicationResult application result
 */
static OCStackApplicationResult handleCloudSignOutResponse(void *ctx,
        OCDoHandle handle,
        OCClientResponse *response)
{
    OIC_LOG_V(DEBUG, TAG, "%s: IN", __func__);

    OC_UNUSED(handle);

    OicCloud_t *cloud = (OicCloud_t *)ctx;

    OCStackApplicationResult ret = OC_STACK_DELETE_TRANSACTION;

    VERIFY_NOT_NULL_RETURN(TAG, response, ERROR, OC_STACK_DELETE_TRANSACTION);
    VERIFY_NOT_NULL_RETURN(TAG, response->payload, ERROR, OC_STACK_DELETE_TRANSACTION);

//TODO clarify response result code from cloud
    /*    if (response->result != LOGIN_OK)
        {
            OIC_LOG(ERROR, TAG, "Sign Out error");
            cloud->stat = OC_CLOUD_ERROR_SIGNOUT;
        }
    */
    OIC_LOG(INFO, TAG, "Sign Out OK");

    cloud->accessToken = (char*)OICCalloc(1, strlen(cloud->session->accessToken) + 1);
    OICStrcpy(cloud->accessToken, strlen(cloud->session->accessToken) + 1, cloud->session->accessToken);

    SessionFree(cloud->session);
    cloud->session = NULL;
    cloud->stat = OC_CLOUD_PROV;

    OIC_LOG_V(DEBUG, TAG, "%s: OUT", __func__);

    return ret;
}

/**
 * Sends Sign In/Out request to cloud
 *
 * @param[in] cloud
 * @param[in] signIn                 is it Sign In or Sign Out request
 * @return  OCStackApplicationResult application result
 */
static OCStackResult CloudSign(OicCloud_t *cloud, bool signIn)
{
    OIC_LOG_V(DEBUG, TAG, "%s: IN", __func__);

    VERIFY_NOT_NULL_RETURN(TAG, cloud, ERROR, OC_STACK_INVALID_PARAM);
    VERIFY_NOT_NULL_RETURN(TAG, cloud->cis, ERROR, OC_STACK_INVALID_PARAM);
    VERIFY_NOT_NULL_RETURN(TAG, cloud->session, ERROR, OC_STACK_INVALID_PARAM);
    VERIFY_NOT_NULL_RETURN(TAG, cloud->session->uid, ERROR, OC_STACK_INVALID_PARAM);
    VERIFY_NOT_NULL_RETURN(TAG, cloud->session->accessToken, ERROR, OC_STACK_INVALID_PARAM);

    OCStackResult ret = OC_STACK_OK;
    char uri[MAX_URI_QUERY] = { 0 };

    char *deviceId = getDeviceId();
    VERIFY_NOT_NULL_RETURN(TAG, deviceId, ERROR, OC_STACK_INVALID_PARAM);

    OCRepPayload *payload = OCRepPayloadCreate();
    VERIFY_NOT_NULL_RETURN(TAG, payload, ERROR, OC_STACK_INVALID_PARAM);

    OCRepPayloadSetPropString(payload, OC_RSRVD_DEVICE_ID, deviceId);
    OCRepPayloadSetPropString(payload, OC_RSRVD_USER_UUID, cloud->session->uid);
    OCRepPayloadSetPropString(payload, OC_RSRVD_ACCESS_TOKEN, cloud->session->accessToken);
    OCRepPayloadSetPropBool(payload, OC_RSRVD_LOGIN, signIn);

    OICFree(deviceId);

    snprintf(uri, MAX_URI_QUERY, "%s/%s", cloud->cis, OC_RSRVD_ACCOUNT_SESSION_URI);

    OCCallbackData cbData =
    {
        .context = (void *)cloud,
        .cb = signIn ? handleCloudSignInResponse : handleCloudSignOutResponse,
        .cd = NULL
    };

    if (signIn && OC_CLOUD_SIGNUP != cloud->stat)
    {
        OIC_LOG_V(ERROR, TAG, "%s: cloud status: %s", __func__, GetCloudStatus(cloud));
        cloud->stat = OC_CLOUD_ERROR_SIGNIN;
        return OC_STACK_ERROR;
    }

    cloud->stat = signIn ? OC_CLOUD_SIGNIN : OC_CLOUD_SIGNOUT;

    ret = OCDoResource(NULL, OC_REST_POST, uri, NULL, (OCPayload *)payload,
                       CT_ADAPTER_TCP, OC_LOW_QOS, &cbData, NULL, 0);

    OIC_LOG_V(DEBUG, TAG, "%s: OUT", __func__);
    return ret;
}

OCStackResult OCCloudSignIn(OicCloud_t *cloud)
{
    return CloudSign(cloud, true);
}

OCStackResult OCCloudSignOut(OicCloud_t *cloud)
{
    return CloudSign(cloud, false);
}

/**
 * SignUp parse payload
 *
 * @param[in] payload
 * @param[out] pointer to cloud
 * @return  bool: true if Ok
 */
static bool CloudSignUpParsePayload(const OCRepPayload *payload, OicCloud_t *cloud)
{
    OIC_LOG_V(DEBUG, TAG, "%s: IN", __func__);
    bool ret = true;
    VERIFY_NOT_NULL_RETURN(TAG, payload, ERROR, false);
    VERIFY_NOT_NULL_RETURN(TAG, cloud, ERROR, false);
    VERIFY_NOT_NULL_RETURN(TAG, cloud->session, ERROR, false);

    if (!OCRepPayloadGetPropString(payload, OC_RSRVD_ACCESS_TOKEN,
                                   &cloud->session->accessToken))
    {
        OIC_LOG_V(ERROR, TAG, "Can't get: %s", OC_RSRVD_ACCESS_TOKEN);
        ret = false;
    }
    if (!OCRepPayloadGetPropString(payload, OC_RSRVD_REFRESH_TOKEN,
                                   &cloud->session->refreshToken))
    {
        OIC_LOG_V(ERROR, TAG, "Can't get: %s", OC_RSRVD_REFRESH_TOKEN);
        ret = false;
    }
    if (!OCRepPayloadGetPropInt(payload, OC_RSRVD_EXPIRES_IN, &cloud->session->expireSin))
    {
        OIC_LOG_V(ERROR, TAG, "Can't get: %s", OC_RSRVD_EXPIRES_IN);
        ret = false;
    }
    if (!OCRepPayloadGetPropString(payload, OC_RSRVD_USER_UUID,
                                   &cloud->session->uid))
    {
        OIC_LOG_V(ERROR, TAG, "Can't get: %s", OC_RSRVD_USER_UUID);
        ret = false;
    }
#if !defined(__MANDATORY__)
    if (!OCRepPayloadGetPropString(payload, OC_RSRVD_REDIRECT_URI,
                                   &cloud->redirectUri))
    {
        OIC_LOG_V(WARNING, TAG, "Can't get: %s", OC_RSRVD_REDIRECT_URI);
    }
#endif // __MANDATORY__
    OIC_LOG_V(DEBUG, TAG, "%s: OUT", __func__);
    return ret;
}

/**
 * Sends Delete request to cloud
 *
 * @param[in] ctx                    context
 * @param[in] handle                 handle
 * @param[in] response               response from peer
 * @return  OCStackApplicationResult application result
 */
static OCStackApplicationResult handleCloudDeleteResponse(void *ctx,
        OCDoHandle handle,
        OCClientResponse *response)
{
    OIC_LOG_V(DEBUG, TAG, "%s: IN", __func__);

    OC_UNUSED(handle);
    OCStackApplicationResult ret = OC_STACK_DELETE_TRANSACTION;
    OicCloud_t *cloud = (OicCloud_t *)ctx;

    VERIFY_NOT_NULL_RETURN(TAG, response, ERROR, OC_STACK_DELETE_TRANSACTION);
    VERIFY_NOT_NULL_RETURN(TAG, cloud, ERROR, OC_STACK_DELETE_TRANSACTION);
    VERIFY_NOT_NULL_RETURN(TAG, cloud->session, ERROR, OC_STACK_DELETE_TRANSACTION);

    OIC_LOG_V(ERROR, TAG, "%s: response result: %d", __func__, response->result);

    cloud->stat = OC_CLOUD_EXIT;

    OIC_LOG_V(DEBUG, TAG, "%s: OUT", __func__);
    return ret;
}
/**
 * Sends Sign Up request to cloud
 *
 * @param[in] ctx                    context
 * @param[in] handle                 handle
 * @param[in] response               response from peer
 * @return  OCStackApplicationResult application result
 */
static OCStackApplicationResult handleCloudSignUpResponse(void *ctx,
        OCDoHandle handle,
        OCClientResponse *response)
{
    OIC_LOG_V(DEBUG, TAG, "%s: IN", __func__);

    OC_UNUSED(handle);
    OCStackApplicationResult ret = OC_STACK_DELETE_TRANSACTION;
    OicCloud_t *cloud = (OicCloud_t *)ctx;

    char *ipv6End = NULL;
    char *percentChar = NULL;
    char *redirectUri = NULL;
    char *cis = NULL;
    char *ipv6Intf = NULL;

    VERIFY_NOT_NULL_RETURN(TAG, response, ERROR, OC_STACK_DELETE_TRANSACTION);
    VERIFY_NOT_NULL_RETURN(TAG, cloud, ERROR, OC_STACK_DELETE_TRANSACTION);

    if (LOGIN_OK != response->result)
    {
        OIC_LOG_V(ERROR, TAG, "%s: Login error: %d", __func__, response->result);
        if ( OC_STACK_COMM_ERROR == response->result || OC_STACK_TIMEOUT == response->result )
        {
            OIC_LOG_V(ERROR, TAG, "%s: Unreachable host or network time out", __func__);
            cloud->stat = OC_CLOUD_ERROR_UNREACHABLE;
        }
        else if ( OC_STACK_ERROR == response->result )
        {
            OIC_LOG_V(ERROR, TAG, "%s: TLS error or other network stack error", __func__);
            cloud->stat = OC_CLOUD_ERROR_TLS;
        }
        else
        {
            OIC_LOG_V(ERROR, TAG, "%s: Authorization error", __func__);
            cloud->stat = OC_CLOUD_ERROR_SIGNUP;
        }
        goto exit;
    }

    if (NULL == response->payload)
    {
        OIC_LOG_V(ERROR, TAG, "%s: Paylod is NULL", __func__);
        cloud->stat = OC_CLOUD_ERROR_SIGNUP;
        goto exit;
    }

#if !defined(NDEBUG)
    OIC_LOG_PAYLOAD(DEBUG, response->payload);
#endif

    if (NULL != cloud->session)
    {
        OIC_LOG_V(WARNING, TAG, "%s: session is running", __func__);
        goto exit;
    }

    cloud->session = (session_t *)OICCalloc(1, sizeof(session_t));

    VERIFY_NOT_NULL_RETURN(TAG, cloud->session, ERROR, OC_STACK_DELETE_TRANSACTION);

    if (!CloudSignUpParsePayload((OCRepPayload *)response->payload, cloud))
    {
        OIC_LOG_V(ERROR, TAG, "%s: parse payload", __func__);
        SessionFree(cloud->session);
        cloud->stat = OC_CLOUD_ERROR_CREATE_SESSION;
        goto exit;
    }

#if !defined(__MANDATORY__)
    if (cloud->redirectUri)
    {
        OIC_LOG_V(WARNING, TAG, "%s: redirect to: %s", __func__, cloud->redirectUri);

        if (ValidCloud(cloud))
        {
            // find the IPv6 address end bracket
            ipv6End = strchr(cloud->redirectUri, ']');
            if (NULL != ipv6End)
            {
                ipv6Intf = strchr(cloud->redirectUri, '%');
                if (ipv6Intf == NULL)
                {
                    // find the interface name from UDP address of sender
                    percentChar = strchr(response->devAddr.addr, '%');
                    size_t ifLen = strlen(percentChar);
                    size_t addrLen = strlen(cloud->redirectUri);
                    size_t uriLen = addrLen + ifLen + 3;

                    // fill the cloud uri with interface name inserted
                    redirectUri = (char *)OICMalloc(sizeof(char) * uriLen);
                    OICStrcpy(redirectUri, ipv6End - cloud->redirectUri + 1, cloud->redirectUri);
                    OICStrcat(redirectUri, uriLen, "%25");
                    OICStrcat(redirectUri, uriLen, percentChar + 1);
                    OICStrcat(redirectUri, uriLen, ipv6End);

                    OICFree(cloud->redirectUri);
                    cloud->redirectUri = redirectUri;
                }
            }

            if (!strCopy(cloud->redirectUri, &cloud->cis))
            {
                OIC_LOG_V(ERROR, TAG, "%s: copy redirect uri", __func__);
                goto exit;
            }
            OICFree(cloud->redirectUri);
            cloud->redirectUri = NULL;
        }
        else
        {
            // find the IPv6 address end bracket
            ipv6End = strchr(cloud->cis, ']');
            if (NULL != ipv6End)
            {
                ipv6Intf = strchr(cloud->cis, '%');
                if (ipv6Intf == NULL)
                {
                    // find the interface name from UDP address of sender
                    percentChar = strchr(response->devAddr.addr, '%');
                    size_t ifLen = strlen(percentChar);
                    size_t addrLen = strlen(cloud->cis);
                    size_t uriLen = addrLen + ifLen + 3;

                    // fill the cloud uri with interface name inserted
                    cis = (char *)OICMalloc(sizeof(char) * uriLen);
                    OICStrcpy(cis, ipv6End - cloud->cis + 1, cloud->cis);
                    OICStrcat(cis, uriLen, "%25");
                    OICStrcat(cis, uriLen, percentChar + 1);
                    OICStrcat(cis, uriLen, ipv6End);

                    OICFree(cloud->cis);
                    cloud->cis = cis;
                }
            }

            OIC_LOG_V(ERROR, TAG, "%s: cannot redirect", __func__);
        }
    }
#endif // __MANDATORY__
    OIC_LOG_V(INFO, TAG, "%s: Sign Up OK", __func__);
    OCCloudSignIn(cloud);
exit:
    OIC_LOG_V(DEBUG, TAG, "%s: OUT", __func__);
    return ret;
}

static OCStackResult OCCloudAccountResource(OicCloud_t *cloud, OCMethod method)
{
    OIC_LOG_V(DEBUG, TAG, "%s: IN", __func__);

    char uri[MAX_URI_LENGTH + MAX_QUERY_LENGTH] = { 0 };
    OCStackResult ret = OC_STACK_OK;

    VERIFY_NOT_NULL_RETURN(TAG, cloud, ERROR, OC_STACK_INVALID_PARAM);
    VERIFY_NOT_NULL_RETURN(TAG, cloud->cis, ERROR, OC_STACK_INVALID_PARAM);
    VERIFY_NOT_NULL_RETURN(TAG, cloud->at, ERROR, OC_STACK_INVALID_PARAM);
    VERIFY_NOT_NULL_RETURN(TAG, cloud->sid, ERROR, OC_STACK_INVALID_PARAM);

    if (OC_CLOUD_PROV != cloud->stat)
    {
        OIC_LOG_V(ERROR, TAG, "%s: cloud state: %s", __func__, GetCloudStatus(cloud));
        return OC_STACK_ERROR;
    }

    if (OC_REST_POST != method && OC_REST_DELETE != method)
    {
        OIC_LOG_V(ERROR, TAG, "%s: unsupported method: %d", __func__, method);
        return OC_STACK_ERROR;
    }

    char *deviceId = getDeviceId();
    VERIFY_NON_NULL_RET(deviceId, TAG, "Can't get the device id", OC_STACK_ERROR);

    OCCallbackData cbData =
    {
        .context = (void *)cloud,
        .cb = NULL,
        .cd = NULL
    };

    OCRepPayload *payload = OCRepPayloadCreate();

    if (OC_REST_POST == method)
    {
        snprintf(uri, MAX_URI_LENGTH, "%s%s", cloud->cis, OC_RSRVD_ACCOUNT_URI);

        VERIFY_NON_NULL_RET(payload, TAG, "Failed to allocate payload", OC_STACK_NO_MEMORY);

        OCRepPayloadSetPropString(payload, OC_RSRVD_DEVICE_ID, deviceId);
        OCRepPayloadSetPropString(payload, OC_RSRVD_ACCESS_TOKEN, cloud->at);

#if !defined(__MANDATORY__)
        if (cloud->apn)
        {
            VERIFY_NOT_NULL_RETURN(TAG, cloud->apn, ERROR, OC_STACK_INVALID_PARAM);
            OCRepPayloadSetPropString(payload, OC_RSRVD_AUTHPROVIDER, cloud->apn);
        }
#endif // __MANDATORY__
        OCRepPayloadSetPropString(payload, OC_RSRVD_SUBJECT_ID, cloud->sid);

        cbData.cb = handleCloudSignUpResponse;

        cloud->stat = OC_CLOUD_SIGNUP;
    }
    else
    {
        snprintf(uri, MAX_URI_LENGTH + MAX_QUERY_LENGTH, "%s%s%s%s%s%s", cloud->cis, OC_RSRVD_ACCOUNT_URI, "?di=", deviceId, "&accesstoken=", cloud->accessToken);
        cloud->stat = OC_CLOUD_EXIT;
        cbData.cb = handleCloudDeleteResponse;
    }

    ret = OCDoResource(NULL, method, uri, NULL, (OCPayload *)payload,
                       CT_ADAPTER_TCP, OC_LOW_QOS, &cbData, NULL, 0);

    OIC_LOG_V(INFO, TAG, "%s: cloud(%s) sign in", __func__, cloud->cis);

    OICFree(deviceId);

    OIC_LOG_V(DEBUG, TAG, "%s: OUT", __func__);

    return ret;
}

OCStackResult OCCloudSignUp(OicCloud_t *cloud)
{
    OIC_LOG_V(DEBUG, TAG, "%s: IN", __func__);

    OCStackResult ret = OCCloudAccountResource(cloud, OC_REST_POST);

    OIC_LOG_V(DEBUG, TAG, "%s: OUT", __func__);
    return ret;
}

OCStackResult OCCloudDelete(OicCloud_t *cloud)
{
    OIC_LOG_V(DEBUG, TAG, "%s: IN", __func__);

    OCStackResult ret = OCCloudAccountResource(cloud, OC_REST_DELETE);

    OIC_LOG_V(DEBUG, TAG, "%s: OUT", __func__);
    return ret;
}

const char *GetCloudStatus(const OicCloud_t *cloud)
{
    switch (cloud->stat)
    {
        case OC_CLOUD_OK:
            return "Ok";
        case OC_CLOUD_PROV:
            return "Cloud provisioned";
        case OC_CLOUD_SIGNUP:
            return "Sign up";
        case OC_CLOUD_SIGNIN:
            return "Sign in";
        case OC_CLOUD_SIGNOUT:
            return "Sign out";
        case OC_CLOUD_TOKEN_REFRESH0:
            return "Token refresh 0";
        case OC_CLOUD_TOKEN_REFRESH1:
            return "Token refresh 1";
        case OC_CLOUD_TOKEN_REFRESH2:
            return "Token refresh 2";
        case OC_CLOUD_TOKEN_REFRESH3:
            return "Token refresh 3";
        case OC_CLOUD_TOKEN_REFRESH4:
            return "Token refresh 4";
        case OC_CLOUD_ERROR_SIGNOUT:
            return "Error sign out";
        case OC_CLOUD_ERROR_SIGNIN:
            return "Error sign in";
        case OC_CLOUD_ERROR_CREATE_SESSION:
            return "Error create session";
        case OC_CLOUD_ERROR_CHECK_SESSION:
            return "Error check session";
        case OC_CLOUD_ERROR_SIGNUP:
            return "Error sign up";
        case OC_CLOUD_ERROR:
            return "Error";
        case OC_CLOUD_EXIT:
            return "Exit";
        default:
            return "unknown";
    }
}
