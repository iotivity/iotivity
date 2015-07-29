//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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
#include "rd_payload.h"

#include <cbor.h>

#include "oic_malloc.h"
#include "oic_string.h"

#ifdef WITH_RD
#include "rd_payload.h"
#endif

#define TAG PCF("OCRDPayload")

OCStackResult OCRDPayloadToCbor(const OCRDPayload *rdPayload, uint8_t **outPayload, size_t *size)
{
    if (!outPayload || !size)
    {
        OC_LOG_V(ERROR, TAG, "Invalid parameters.");
        return OC_STACK_ERROR;
    }

    *outPayload = (uint8_t *)OICCalloc(1, MAX_REQUEST_LENGTH);
    if (!*outPayload)
    {
        goto no_memory;
    }

    *size = MAX_REQUEST_LENGTH;

    CborEncoder encoder;
    cbor_encoder_init(&encoder, *outPayload, *size, 0);

    OC_LOG_V(DEBUG, TAG, "RD Payload : %d", rdPayload->base.type);
    OC_LOG_V(DEBUG, TAG, "RD Payload Type: %d", rdPayload->payloadType);

    CborEncoder rootArray;
    CborError cborEncoderResult;
    cborEncoderResult = cbor_encoder_create_array(&encoder, &rootArray, 2);
    if (CborNoError != cborEncoderResult)
    {
        OC_LOG_V(ERROR, TAG, "Failed creating cbor array.");
        goto exit;
    }

    cborEncoderResult = cbor_encode_uint(&rootArray, rdPayload->base.type);
    if (CborNoError != cborEncoderResult)
    {
        OC_LOG_V(ERROR, TAG, "Failed setting rdPayload->base.type.");
        goto exit;
    }

    CborEncoder map;
    cborEncoderResult = cbor_encoder_create_map(&rootArray, &map, CborIndefiniteLength);
    if (CborNoError != cborEncoderResult)
    {
        OC_LOG_V(ERROR, TAG, "Failed creating cbor map.");
        goto exit;
    }

    cborEncoderResult = cbor_encode_text_string(&map, OC_RSRVD_CONTENT_TYPE,
        sizeof(OC_RSRVD_CONTENT_TYPE) - 1);
    if (CborNoError != cborEncoderResult)
    {
        OC_LOG_V(ERROR, TAG, "Failed setting content type.");
        goto exit;
    }

    cborEncoderResult = cbor_encode_uint(&map, rdPayload->payloadType);
    if (CborNoError != cborEncoderResult)
    {
        OC_LOG_V(ERROR, TAG, "Failed setting rdPayload->payloadType.");
        goto exit;
    }

    if (rdPayload->payloadType == RD_PAYLOAD_TYPE_DISCOVERY)
    {
        if (rdPayload->rdDiscovery && rdPayload->rdDiscovery->sel)
        {
            cborEncoderResult = cbor_encode_text_string(&map,
                OC_RSRVD_RD_DISCOVERY_SEL, sizeof(OC_RSRVD_RD_DISCOVERY_SEL) -1);
            if (CborNoError !=  cborEncoderResult)
            {
                OC_LOG_V(ERROR, TAG, "Failed setting discovery sel type.");
                goto exit;
            }
            cborEncoderResult = cbor_encode_uint(&map, rdPayload->rdDiscovery->sel);
            if (CborNoError != cborEncoderResult)
            {
                OC_LOG_V(ERROR, TAG, "Failed setting discovery sel value.");
                goto exit;
            }
            OC_LOG_V(DEBUG, TAG, "RD Payload bias factor: %d", rdPayload->rdDiscovery->sel);
        }
    }
    cborEncoderResult = cbor_encoder_close_container(&rootArray, &map);
    if (CborNoError != cborEncoderResult)
    {
        OC_LOG_V(ERROR, TAG, "Failed closing rootArray container.");
        goto exit;
    }

    cborEncoderResult = cbor_encoder_close_container(&encoder, &rootArray);
    if (CborNoError != cborEncoderResult)
    {
        OC_LOG_V(ERROR, TAG, "Failed closing encoder container.");
        goto exit;
    }

    *size = encoder.ptr - *outPayload;
    uint8_t *tempPayload = (uint8_t *)OICRealloc(*outPayload, *size);
    if (!tempPayload)
    {
        goto no_memory;
    }

    *outPayload = tempPayload;

    return OC_STACK_OK;

no_memory:
    OC_LOG_V(ERROR, TAG, "Memory allocation failed.");
    OICFree(*outPayload);
    return OC_STACK_NO_MEMORY;

exit:
    OICFree(*outPayload);
    return OC_STACK_ERROR;
}

OCStackResult OCRDCborToPayload(const CborValue *cborPayload, OCPayload **outPayload)
{
    CborValue *rdCBORPayload = (CborValue *)cborPayload;
    OCRDPayload *rdPayload = NULL;

    if (cbor_value_is_map(rdCBORPayload))
    {
        CborValue curVal;
        CborError cborFindResult;
        cborFindResult = cbor_value_map_find_value(rdCBORPayload,
            OC_RSRVD_CONTENT_TYPE, &curVal);
        if (CborNoError != cborFindResult)
        {
            OC_LOG_V(ERROR, TAG, "Failed finding OC_RSRVD_CONTENT_TYPE type in the payload.");
            goto exit;
        }

        int payloadType = 0 ;
        if (cbor_value_is_valid(&curVal))
        {
            cborFindResult = cbor_value_get_int(&curVal, &payloadType);
            if (CborNoError != cborFindResult)
            {
                OC_LOG_V(ERROR, TAG, "Failed finding OC_RSRVD_CONTENT_TYPE value in the payload.");
                goto exit;
            }
            OC_LOG_V(DEBUG, TAG, "RD Payload Type : %d ", payloadType);
        }

        rdPayload = OCRDPayloadCreate(payloadType);
        if (!rdPayload)
        {
            goto no_memory;
        }

        if (RD_PAYLOAD_TYPE_DISCOVERY == payloadType)
        {
            cborFindResult = cbor_value_map_find_value(rdCBORPayload,
                OC_RSRVD_RD_DISCOVERY_SEL, &curVal);
            if (CborNoError != cborFindResult)
            {
                OC_LOG_V(ERROR, TAG, "Failed finding OC_RSRVD_RD_DISCOVERY_SEL type in the payload.");
                goto exit;
            }

            int biasFactor = 0;
            if (cbor_value_is_valid(&curVal))
            {
                cborFindResult = cbor_value_get_int(&curVal, &biasFactor);
                if (CborNoError != cborFindResult)
                {
                    OC_LOG_V(ERROR, TAG, "Failed finding OC_RSRVD_RD_DISCOVERY_SEL value in the payload.");
                    goto exit;
                }
            }
            OC_LOG_V(DEBUG, TAG, "Bias Factor : %d ", biasFactor);

            rdPayload->rdDiscovery = OCRDDiscoveryPayloadCreate(biasFactor);
            if (!rdPayload->rdDiscovery)
            {
                goto no_memory;
            }
        }

        cborFindResult = cbor_value_advance(rdCBORPayload);
        if (CborNoError != cborFindResult)
        {
            OC_LOG_V(ERROR, TAG, "Failed advancing the payload.");
            goto exit;
        }

        *outPayload = (OCPayload *)rdPayload;
    }

    return OC_STACK_OK;

no_memory:
    OC_LOG_V(ERROR, TAG, "Failed allocating memory.");
    OCRDPayloadDestroy(rdPayload);
    return OC_STACK_NO_MEMORY;

exit:
    OCRDPayloadDestroy(rdPayload);
    return OC_STACK_ERROR;
}

OCRDPayload *OCRDPayloadCreate(OCRDPayloadType payloadType)
{
    OCRDPayload *rdPayload = (OCRDPayload *)OICCalloc(1, sizeof(OCRDPayload));

    if (!rdPayload)
    {
        return NULL;
    }

    rdPayload->base.type = PAYLOAD_TYPE_RD;
    rdPayload->payloadType = payloadType;

    return rdPayload;
}

OCRDDiscoveryPayload *OCRDDiscoveryPayloadCreate(int biasFactor)
{
    OCRDDiscoveryPayload *discoveryPayload = (OCRDDiscoveryPayload *)
        OICCalloc(1, sizeof(OCRDDiscoveryPayload));

    if (!discoveryPayload)
    {
        return NULL;
    }
    discoveryPayload->sel = biasFactor;

    return discoveryPayload;
}

void OCRDPayloadDestroy(OCRDPayload *payload)
{
    if (!payload)
    {
        return;
    }

    if (payload->rdDiscovery)
    {
        OICFree(payload->rdDiscovery);
    }

    OICFree(payload);
}

void OCRDPayloadLog(LogLevel level, const char *tag, const OCRDPayload *payload)
{
    if (!payload)
    {
        return;
    }

    OC_LOG_V(level, tag, "BaseType : %d", payload->base.type);
    OC_LOG_V(level, tag, "RD Payload Type : %d", payload->payloadType);

    if (payload->rdDiscovery)
    {
        OC_LOG_V(level, tag, "RD Payload Discovery BIAS : %d", payload->rdDiscovery->sel);
    }
}
