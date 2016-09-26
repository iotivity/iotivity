#include "utils.h"

#include "oic_malloc.h"
#include "logger.h"
#include "ocstack.h"
#include "ocpayload.h"
#include "pmutility.h"
#include "cacommonutil.h"
#include "aclresource.h"
#include "ocpayloadcbor.h"
#include "payload_logging.h"

#define TAG "CLOUD-ACL-ID"

/**
 * ACL Id parse from received response
 *
 * @param[in] ctx       context
 * @param[out] data     data required to external application
 * @param[in] response  peer response
 * @return  OCStackResult application result
 */
static OCStackResult getAclIdFromResponse(void *ctx, void **data, OCClientResponse *response)
{
    OC_UNUSED(ctx);
    if (NULL == response->payload)
    {
        OIC_LOG(ERROR, TAG, "Receive NULL payload");
        return OC_STACK_INVALID_PARAM;
    }

    char *aclid = NULL;

    if (!OCRepPayloadGetPropString((const OCRepPayload *)response->payload, OC_RSRVD_ACL_ID, &aclid))
    {
        OIC_LOG_V(ERROR, TAG, "Can't get: %s", OC_RSRVD_ACL_ID);
        return OC_STACK_MALFORMED_RESPONSE;
    }

    OIC_LOG_V(INFO, TAG, "Received Acl id = %s", aclid);

    *data = aclid;
    return OC_STACK_OK;
}

/**
 * ACL Id by device request received data handler
 *
 * @param[in] ctx       context
 * @param[out] data     data required to external application
 * @param[in] response  peer response
 * @return  OCStackResult application result
 */
static OCStackResult handleGetAclIdByDeviceResponse(void *ctx, void **data,
                                                    OCClientResponse *response)
{
    return getAclIdFromResponse(ctx, data, response);
}

/**
 * ACL Id create request received data handler
 *
 * @param[in] ctx       context
 * @param[out] data     data required to external application
 * @param[in] response  peer response
 * @return  OCStackResult application result
 */
static OCStackResult handleAclIdCreateResponse(void *ctx, void **data, OCClientResponse *response)
{
    return getAclIdFromResponse(ctx, data, response);
}

OCStackResult OCCloudGetAclIdByDevice(void* ctx,
                                      const char *deviceId,
                                      const OCDevAddr *endPoint,
                                      OCCloudResponseCB callback)
{
    char uri[MAX_URI_LENGTH] = { 0 };

    VERIFY_NON_NULL_RET(deviceId, TAG, "NULL input param", OC_STACK_INVALID_PARAM);

    snprintf(uri, MAX_URI_LENGTH, "%s%s:%d%s?%s=%s", DEFAULT_PREFIX,
            endPoint->addr, endPoint->port, OC_RSRVD_ACL_ID_URL, OC_RSRVD_DEVICE_ID, deviceId);

    OCCallbackData cbData;
    fillCallbackData(&cbData, ctx, callback, handleGetAclIdByDeviceResponse, NULL);

    return OCDoResource(NULL, OC_REST_GET, uri, NULL, NULL,
                        CT_ADAPTER_TCP, OC_LOW_QOS, &cbData, NULL, 0);
}

OCStackResult OCCloudAclIdCreate(void* ctx,
                                 const char *ownerId,
                                 const char *deviceId,
                                 const OCDevAddr *endPoint,
                                 OCCloudResponseCB callback)
{
    char uri[MAX_URI_LENGTH] = { 0 };

    VERIFY_NON_NULL_RET(ownerId, TAG, "NULL input param", OC_STACK_INVALID_PARAM);
    VERIFY_NON_NULL_RET(deviceId, TAG, "NULL input param", OC_STACK_INVALID_PARAM);

    snprintf(uri, MAX_URI_LENGTH, "%s%s:%d%s?%s=%s&%s=%s", DEFAULT_PREFIX,
            endPoint->addr, endPoint->port, OC_RSRVD_ACL_ID_URL,
            OC_RSRVD_OWNER_ID, ownerId, OC_RSRVD_DEVICE_ID, deviceId);

    OCCallbackData cbData;
    fillCallbackData(&cbData, ctx, callback, handleAclIdCreateResponse, NULL);

    return OCDoResource(NULL, OC_REST_PUT, uri, NULL, NULL,
                        CT_ADAPTER_TCP, OC_LOW_QOS, &cbData, NULL, 0);
}

OCStackResult OCCloudAclIdDelete(void* ctx,
                                 const char *aclId,
                                 const OCDevAddr *endPoint,
                                 OCCloudResponseCB callback)
{
    char uri[MAX_URI_LENGTH]  = { 0 };

    VERIFY_NON_NULL_RET(aclId, TAG, "NULL input param", OC_STACK_INVALID_PARAM);

    snprintf(uri, MAX_URI_LENGTH, "%s%s:%d%s?%s=%s", DEFAULT_PREFIX,
            endPoint->addr, endPoint->port, OC_RSRVD_ACL_ID_URL, OC_RSRVD_ACL_ID, aclId);

    OCCallbackData cbData;
    fillCallbackData(&cbData, ctx, callback, NULL, NULL);

    return OCDoResource(NULL, OC_REST_DELETE, uri, NULL, NULL,
                        CT_ADAPTER_TCP, OC_LOW_QOS, &cbData, NULL, 0);
}

/**
 * ACL Get Info received data handler
 *
 * @param[in] ctx       context
 * @param[out] data     data required to external application
 * @param[in] response  peer response
 * @return  OCStackResult application result
 */
static OCStackResult handleAclGetInfoResponse(void *ctx, void **data, OCClientResponse *response)
{
    OCStackResult result = OC_STACK_OK;
    uint8_t *cbor = NULL;
    size_t size   = 0;

    OC_UNUSED(ctx);
    OC_UNUSED(data);

    if (NULL == response->payload)
    {
        OIC_LOG(ERROR, TAG, "Receive NULL payload\n");
        return OC_STACK_INVALID_PARAM;
    }

    result = OCConvertPayload(response->payload, &cbor, &size);
    if (result != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "Can't convert OCPayload to cbor");
        goto exit;
    }

    OicSecAcl_t* acl = CBORPayloadToAcl2(cbor, size);
    if (NULL == acl)
    {
        OIC_LOG(ERROR, TAG, "Can't parse CBOR payload");
        goto exit;
    }

    result = InstallNewACL2(acl);
    if (result != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "Can't update ACL resource");
    }

exit:
    //can't delete acl because aces was added to gAcl
    OICFree(cbor);
    return result;
}

OCStackResult OCCloudAclIndividualGetInfo(void* ctx,
                                          const char *aclId,
                                          const OCDevAddr *endPoint,
                                          OCCloudResponseCB callback)
{
    char uri[MAX_URI_LENGTH]  = { 0 };

    VERIFY_NON_NULL_RET(aclId, TAG, "NULL input param", OC_STACK_INVALID_PARAM);

    snprintf(uri, MAX_URI_LENGTH, "%s%s:%d%s/%s", DEFAULT_PREFIX,
            endPoint->addr, endPoint->port, OC_RSRVD_ACL_ID_URL, aclId);

    OCCallbackData cbData;
    fillCallbackData(&cbData, ctx, callback, handleAclGetInfoResponse, NULL);

    return OCDoResource(NULL, OC_REST_GET, uri, NULL, NULL,
                        CT_ADAPTER_TCP, OC_LOW_QOS, &cbData, NULL, 0);
}

OCStackResult OCCloudAclIndividualUpdateAce(void* ctx,
                                            const char *aclId,
                                            const cloudAce_t *aces,
                                            const OCDevAddr *endPoint,
                                            OCCloudResponseCB callback)
{
    size_t dimensions[MAX_REP_ARRAY_DEPTH] = { 0 };
    char uri[MAX_URI_LENGTH]  = { 0 };

    int i = 0, j = 0;

    OCRepPayload **helperPayload  = NULL;
    OCRepPayload **helperPayload2 = NULL;

    VERIFY_NON_NULL_RET(aclId, TAG, "NULL input param", OC_STACK_INVALID_PARAM);
    VERIFY_NON_NULL_RET(aces, TAG, "NULL input param", OC_STACK_INVALID_PARAM);

    snprintf(uri, MAX_URI_LENGTH, "%s%s:%d%s/%s", DEFAULT_PREFIX,
            endPoint->addr, endPoint->port, OC_RSRVD_ACL_ID_URL, aclId);

    OCRepPayload *payload = OCRepPayloadCreate();
    if (!payload)
    {
        OIC_LOG_V(DEBUG, TAG, "Can't allocate memory for payload");
        goto no_memory;
    }

    int acllist_count = 0;
    if (aces)
    {
        cloudAce_t *ace = (cloudAce_t *)&aces[acllist_count++];
        while (ace->next)
        {
            ace = ace->next;
            acllist_count++;
        }
    }

    helperPayload = OICCalloc(acllist_count, sizeof(OCRepPayload *));
    if (!helperPayload)
    {
        OIC_LOG_V(DEBUG, TAG, "Can't allocate memory for helperPayload");
        goto no_memory;
    }

    for (int i = 0; i < acllist_count; i++)
    {
        OCRepPayload *payload = OCRepPayloadCreate();
        if (!payload)
        {
            OIC_LOG_V(DEBUG, TAG, "Can't allocate memory for helperPayload[i]");
            goto no_memory;
        }
        helperPayload[i] = payload;

        const cloudAce_t *ace = &aces[i];

        OCRepPayloadSetPropString(payload, OC_RSRVD_ACE_ID, ace->aceId);
        OCRepPayloadSetPropString(payload, OC_RSRVD_SUBJECT_UUID, (const char *)ace->subjectuuid.id);
        OCRepPayloadSetPropInt(payload, OC_RSRVD_SUBJECT_TYPE, ace->stype);
        OCRepPayloadSetPropInt(payload, OC_RSRVD_PERMISSION_MASK, ace->permission);

        int reslist_count = 0;
        if (ace->resources)
        {
            OicSecRsrc_t *res = &ace->resources[reslist_count++];
            while (res->next)
            {
                res = res->next;
                reslist_count++;
            }
        }

        helperPayload2 = OICCalloc(reslist_count, sizeof(OCRepPayload *));
        if (!helperPayload2)
        {
            goto no_memory;
        }

        for (int j = 0; j < reslist_count; j++)
        {
            OCRepPayload *payload = OCRepPayloadCreate();
            if (!payload)
            {
                OIC_LOG_V(DEBUG, TAG, "Can't allocate memory for payload");
                goto no_memory;
            }
            helperPayload2[j] = payload;

            const OicSecRsrc_t *res = &ace->resources[j];

            OCRepPayloadSetPropString(payload, OC_RSRVD_HREF, res->href);

            dimensions[0] = res->typeLen;
            OCRepPayloadSetStringArray(payload, OC_RSRVD_RESOURCE_TYPE,
                                       (const char **)res->types, dimensions);

            dimensions[0] = res->interfaceLen;
            OCRepPayloadSetStringArray(payload, OC_RSRVD_INTERFACE,
                                       (const char **)res->interfaces, dimensions);
        }
        dimensions[0] = reslist_count;
        OCRepPayloadSetPropObjectArray(payload, OC_RSRVD_RESOURCES,
                (const OCRepPayload **)helperPayload2, dimensions);
    }
    dimensions[0] = acllist_count;
    OCRepPayloadSetPropObjectArray(payload, OC_RSRVD_ACCESS_CONTROL_LIST,
            (const OCRepPayload **)helperPayload, dimensions);

    OCCallbackData cbData;
    fillCallbackData(&cbData, ctx, callback, NULL, NULL);

    OIC_LOG(DEBUG, TAG, "Next payload created:");
    OIC_LOG_PAYLOAD(DEBUG, (OCPayload *)payload);

    return OCDoResource(NULL, OC_REST_POST, uri, NULL, (OCPayload *)payload,
                        CT_ADAPTER_TCP, OC_LOW_QOS, &cbData, NULL, 0);
no_memory:
    if (helperPayload2)
    {
        for (int k = 0; k < j; k++) OCRepPayloadDestroy(helperPayload2[k]);
        OICFree(helperPayload2);
    }
    if (helperPayload)
    {
        for (int k = 0; k < i; k++) OCRepPayloadDestroy(helperPayload[k]);
        OICFree(helperPayload);
    }
    OCRepPayloadDestroy(payload);
    return OC_STACK_NO_MEMORY;
}

OCStackResult OCCloudAclIndividualDelete(void* ctx,
                                         const char *aclId,
                                         const OCDevAddr *endPoint,
                                         OCCloudResponseCB callback)
{
    char uri[MAX_URI_LENGTH]  = { 0 };

    VERIFY_NON_NULL_RET(aclId, TAG, "NULL input param", OC_STACK_INVALID_PARAM);

    snprintf(uri, MAX_URI_LENGTH, "%s%s:%d%s/%s", DEFAULT_PREFIX,
            endPoint->addr, endPoint->port, OC_RSRVD_ACL_ID_URL, aclId);

    OCCallbackData cbData;
    fillCallbackData(&cbData, ctx, callback, NULL, NULL);

    return OCDoResource(NULL, OC_REST_DELETE, uri, NULL, NULL,
                        CT_ADAPTER_TCP, OC_LOW_QOS, &cbData, NULL, 0);
}
