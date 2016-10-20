#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

#include "logger.h"
#include "ocstack.h"
#include "ocpayload.h"
#include "pmutility.h"
#include "oic_malloc.h"
#include "crlresource.h"
#include "ocpayloadcbor.h"
#include "payload_logging.h"
#include "cacommonutil.h"

#define TAG "CLOUD-CRL"

/**
 * CRL GET request received data handler
 *
 * @param[in] ctx       context
 * @param[out] data     data required to external application
 * @param[in] response  peer response
 * @return  OCStackResult application result
 */
static OCStackResult handleCrlGetResponse(void *ctx, void **data, OCClientResponse *response)
{
    OCStackResult result = OC_STACK_OK;
    OicSecCrl_t *crl = NULL;

    OC_UNUSED(ctx);
    OC_UNUSED(data);

    if (NULL == response->payload)
    {
        OIC_LOG(ERROR, TAG, "Receive NULL payload\n");
        return OC_STACK_INVALID_PARAM;
    }

    OCPayloadType type = response->payload->type;

    if (type != PAYLOAD_TYPE_SECURITY)
    {
        OIC_LOG(ERROR, TAG, "Receive NOT security payload, can't parse it!\n");
        return OC_STACK_INVALID_PARAM;
    }

    OCSecurityPayload *payload = (OCSecurityPayload *)response->payload;

    result = CBORPayloadToCrl(payload->securityData, payload->payloadSize, &crl);
    if (result != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "Can't convert cbor to Crl");
        goto exit;
    }

    result = UpdateCRLResource(crl);
    if (result != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "Can't update Crl resource");
    }

    *data = crl;
exit:
    return result;
}

OCStackResult OCCloudGetCRL(void* ctx,
                            const OCDevAddr *endPoint,
                            OCCloudResponseCB callback)
{
    char uri[MAX_URI_LENGTH] = { 0 };
    char *lastUpdate = NULL;

    VERIFY_NON_NULL_RET(endPoint, TAG, "NULL endpoint", OC_STACK_INVALID_PARAM);

    getLastUpdateFromDB(&lastUpdate);

    snprintf(uri, MAX_URI_LENGTH, "%s%s:%d%s?%s=%s", DEFAULT_PREFIX,
            endPoint->addr, endPoint->port,
            OC_RSRVD_PROV_CRL_URL, OC_RSRVD_LAST_UPDATE, lastUpdate);

    OICFree(lastUpdate);

    OCCallbackData cbData;
    fillCallbackData(&cbData, ctx, callback, handleCrlGetResponse, NULL);

    return OCDoResource(NULL, OC_REST_GET, uri, NULL, NULL,
                        CT_ADAPTER_TCP, OC_LOW_QOS, &cbData, NULL, 0);
}

OCStackResult OCCloudPostCRL(void* ctx,
                             const char *thisUpdate,
                             const char *nextUpdate,
                             const OCByteString *crl,
                             const stringArray_t *serialNumbers,
                             const OCDevAddr *endPoint,
                             OCCloudResponseCB callback)
{
    OCStackResult result = OC_STACK_ERROR;
    char uri[MAX_URI_LENGTH] = { 0 };
    OCRepPayload *payload = NULL;
    OCSecurityPayload *secPayload = NULL;
    uint8_t *cbor = NULL;
    size_t cbor_len = 0;
    OicSecKey_t crl1;

    VERIFY_NON_NULL_RET(endPoint, TAG, "NULL endpoint", OC_STACK_INVALID_PARAM);
    VERIFY_NON_NULL_RET(thisUpdate, TAG, "NULL input param", OC_STACK_INVALID_PARAM);
    VERIFY_NON_NULL_RET(nextUpdate, TAG, "NULL input param", OC_STACK_INVALID_PARAM);

    if (crl)
    {
        crl1.data = crl->bytes;
        crl1.len  = crl->len;
        crl1.encoding = OIC_ENCODING_DER;
    }

    snprintf(uri, MAX_URI_LENGTH, "%s%s:%d%s", DEFAULT_PREFIX,
            endPoint->addr, endPoint->port, OC_RSRVD_PROV_CRL_URL);

    OCCallbackData cbData;
    fillCallbackData(&cbData, ctx, callback, NULL, NULL);

    payload = OCRepPayloadCreate();
    if (!payload)
    {
        OIC_LOG(ERROR, TAG, "Can't allocate payload");
        goto exit;
    }

    OCRepPayloadSetPropString(payload, OC_RSRVD_THIS_UPDATE, thisUpdate);
    OCRepPayloadSetPropString(payload, OC_RSRVD_NEXT_UPDATE, nextUpdate);

    //add next fields if they were filled
    if (serialNumbers)
    {
        size_t dimensions[MAX_REP_ARRAY_DEPTH] = {serialNumbers->length, 0 ,0};
        OCRepPayloadSetStringArray(payload, OC_RSRVD_SERIAL_NUMBERS,
                                   (const char**)serialNumbers->array, dimensions);

    }
    if (crl) OCRepPayloadSetPropPubDataType(payload, OC_RSRVD_CRL, &crl1);

    OIC_LOG(DEBUG, TAG, "Next payload created:");
    OIC_LOG_PAYLOAD(DEBUG, (OCPayload *)payload);

    secPayload = OICCalloc(1, sizeof(OCSecurityPayload));
    if (!secPayload)
    {
        OIC_LOG(ERROR, TAG, "Can't allocate secPayload");
        goto exit;
    }

    result = OCConvertPayload((OCPayload *)payload, &cbor, &cbor_len);
    if (result != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "Can't convert OCPayload to cbor");
        OICFree(secPayload);
        goto exit;
    }

    secPayload->base.type = PAYLOAD_TYPE_SECURITY;
    secPayload->securityData = cbor;
    secPayload->payloadSize = cbor_len;

    result = OCDoResource(NULL, OC_REST_POST, uri, NULL, (OCPayload *)secPayload,
                        CT_ADAPTER_TCP, OC_LOW_QOS, &cbData, NULL, 0);
exit:
    OCPayloadDestroy((OCPayload *)payload);
    return result;
}
