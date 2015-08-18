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

#include "oic_malloc.h"
#include "oic_string.h"

#include "octypes.h"
#include "ocstack.h"
#include "logger.h"

#define TAG PCF("OCRDPayload")

#define CBOR_ROOT_ARRAY_LENGTH 2

static void linksPayloadDestroy(OCRDLinksPayload *linkPayload)
{
    OCRDLinksPayload *links = linkPayload;

    while (links)
    {
        OICFree(links->href);
        OICFree(links->rt);
        OICFree(links->itf);
        OCRDLinksPayload *tmp = links;
        links = links->next;
        OICFree(tmp);
    }
}

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
    int flags = 0;
    cbor_encoder_init(&encoder, *outPayload, *size, flags);

    OC_LOG_V(DEBUG, TAG, "RD Payload : %d", rdPayload->base.type);
    OC_LOG_V(DEBUG, TAG, "RD Payload Type: %d", rdPayload->payloadType);

    CborEncoder rootArray;
    CborError cborEncoderResult;
    cborEncoderResult = cbor_encoder_create_array(&encoder, &rootArray, CBOR_ROOT_ARRAY_LENGTH);
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
            if (CborNoError != cborEncoderResult)
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

    if (!cbor_value_is_map(rdCBORPayload))
    {
        OC_LOG_V(ERROR, TAG, "RD CBOR Payload is not in map format.");
        return OC_STACK_ERROR;
    }
    else
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

            rdPayload->rdDiscovery = OCRDDiscoveryPayloadCreate(biasFactor);
            if (!rdPayload->rdDiscovery)
            {
                goto no_memory;
            }
        }
        else
        {    // TTL
            int ttl = 0;
            cborFindResult = cbor_value_map_find_value(rdCBORPayload, OC_RSRVD_TTL, &curVal);
            if (CborNoError != cborFindResult)
            {
                OC_LOG_V(ERROR, TAG, "Failed finding OC_RSRVD_TTL type in the payload.");
                goto exit;
            }

            cborFindResult = cbor_value_get_int(&curVal, &ttl);
            if (CborNoError != cborFindResult)
            {
                OC_LOG_V(ERROR, TAG, "Failed finding OC_RSRVD_TTL value in the payload.");
                goto exit;
            }

            // Link Array
            CborValue linkArray;
            cborFindResult = cbor_value_map_find_value(rdCBORPayload, OC_RSRVD_LINKS, &linkArray);
            if (CborNoError != cborFindResult)
            {
                OC_LOG_V(ERROR, TAG, "Failed finding OC_RSRVD_LINKS type in the payload.");
                goto exit;
            }

            CborValue linkVal;
            cborFindResult = cbor_value_enter_container(&linkArray, &linkVal);
            if (CborNoError != cborFindResult)
            {
                OC_LOG_V(ERROR, TAG, "Failed entering linkArray container in the payload.");
                goto exit;
            }

            OCRDLinksPayload *links = NULL;
            while(cbor_value_is_map(&linkVal))
            {
                char *href = NULL;
                char *itf = NULL;
                char *rt = NULL;
                size_t len;

                cborFindResult = cbor_value_map_find_value(&linkVal, OC_RSRVD_HREF, &curVal);
                if (CborNoError != cborFindResult)
                {
                    OC_LOG_V(ERROR, TAG, "Failed finding link value OC_RSRVD_HREF type in the payload.");
                    goto exit;
                }

                cborFindResult = cbor_value_dup_text_string(&curVal, &href, &len, NULL);
                if (CborNoError != cborFindResult)
                {
                    OC_LOG_V(ERROR, TAG, "Failed finding link value OC_RSRVD_HREF value in the payload.");
                    goto exit;
                }

                cborFindResult = cbor_value_map_find_value(&linkVal, OC_RSRVD_INTERFACE, &curVal);
                if (CborNoError != cborFindResult)
                {
                    OC_LOG_V(ERROR, TAG, "Failed finding link value OC_RSRVD_INTERFACE type in the payload.");
                    goto exit;
                }

                cborFindResult = cbor_value_dup_text_string(&curVal, &itf, &len, NULL);
                if (CborNoError != cborFindResult)
                {
                    OC_LOG_V(ERROR, TAG, "Failed finding link value OC_RSRVD_INTERFACE value in the payload.");
                    goto exit;
                }

                cborFindResult = cbor_value_map_find_value(&linkVal, OC_RSRVD_RESOURCE_TYPE, &curVal);
                if (CborNoError != cborFindResult)
                {
                    OC_LOG_V(ERROR, TAG, "Failed finding link value OC_RSRVD_RESOURCE_TYPE type in the payload.");
                    goto exit;
                }

                cborFindResult = cbor_value_dup_text_string(&curVal, &rt, &len, NULL);
                if (CborNoError != cborFindResult)
                {
                    OC_LOG_V(ERROR, TAG, "Failed finding link value OC_RSRVD_RESOURCE_TYPE value in the payload.");
                    goto exit;
                }

                OCRDLinksPayloadCreate(href, itf, rt, &links);
                if (!links)
                {
                    goto no_memory;
                }

                cborFindResult = cbor_value_advance(&linkVal);
                if (CborNoError != cborFindResult)
                {
                        OC_LOG_V(ERROR, TAG, "Failed advancing the linkVal payload.");
                        goto exit;
                }
            }
            cborFindResult = cbor_value_advance(&linkArray);
            if (CborNoError != cborFindResult)
            {
                OC_LOG_V(ERROR, TAG, "Failed advancing the linkArray payload.");
                goto exit;
            }
            rdPayload->rdPublish = OCRDPublishPayloadCreate(ttl, links);
            if (!rdPayload->rdPublish)
            {
                goto no_memory;
            }

            OCRDPayloadLog(DEBUG, TAG, rdPayload);
            cborFindResult = cbor_value_advance(rdCBORPayload);
            if (CborNoError != cborFindResult)
            {
                OC_LOG_V(ERROR, TAG, "Failed advancing the payload.");
                goto exit;
            }
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

void OCRDLinksPayloadCreate(const char *uri, const char *rt, const char *itf,
        OCRDLinksPayload **linksPayload)
{
    OCRDLinksPayload *payload = OICCalloc(1, sizeof(OCRDLinksPayload));
    if (!payload)
    {
        goto no_memory;
    }

    payload->href = OICStrdup(uri);
    if (!payload->href)
    {
        goto no_memory;
    }

    payload->rt = OICStrdup(rt);
    if (!payload->rt)
    {
        goto no_memory;
    }

    payload->itf = OICStrdup(itf);
    if (!payload->itf)
    {
        goto no_memory;
    }

    payload->next = NULL;

    if (*linksPayload == NULL)
    {
        *linksPayload = payload;
    }
    else
    {
        OCRDLinksPayload *temp = *linksPayload;
        while (temp->next)
        {
            temp = temp->next;
        }
        temp->next = payload;
    }
    return;

no_memory:
    OC_LOG_V(ERROR, TAG, "Memory allocation failed.");
    linksPayloadDestroy(payload);
}

OCRDDiscoveryPayload *OCRDDiscoveryPayloadCreate(int biasFactor)
{
    OCRDDiscoveryPayload *discoveryPayload = OICCalloc(1, sizeof(OCRDDiscoveryPayload));

    if (!discoveryPayload)
    {
        return NULL;
    }
    discoveryPayload->sel = biasFactor;

    return discoveryPayload;
}


OCRDPublishPayload* OCRDPublishPayloadCreate(int ttl,
        OCRDLinksPayload *linksPayload)
{
    OCRDPublishPayload *rdPublish = OICCalloc(1, sizeof(OCRDPublishPayload));
    if (!rdPublish)
    {
        return NULL;
    }

    //TODO: Find way of device device id.
    // rdPayload->rdPublish->id = (uint8_t *)OICCalloc(1, UUID_SIZE);
    // memcpy(rdPayload->rdPublish->id, , UUID_SIZE);
    //TODO: Find way of device device name.
    // rdPayload->rdPublish->n = (char*)OICCalloc(1, strlen(name));
    // memcpy(rdPayload->rdPublish->n, , strlen(name));
    rdPublish->ttl = ttl; // TODO Expose API to allow user to set this value.
    rdPublish->links = linksPayload;

    return rdPublish;
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

    if (payload->rdPublish)
    {
        if (payload->rdPublish->links)
        {
            linksPayloadDestroy(payload->rdPublish->links);
        }

        if (payload->rdPublish->deviceName.deviceName)
        {
            OICFree(payload->rdPublish->deviceName.deviceName);
        }

        OICFree(payload->rdPublish);
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
    OCRDPublishPayloadLog(level, tag, payload->rdPublish);
}

void OCRDPublishPayloadLog(LogLevel level, const char *tag, const OCRDPublishPayload *rdPublish)
{
    if (rdPublish)
    {
        if (payload->rdPublish->deviceName.deviceName)
        {
            OC_LOG_V(level, tag, "RD Payload Pulish Name : %s", payload->rdPublish->deviceName.deviceName);
        }

        if (payload->rdPublish->deviceId.id)
        {
            OC_LOG_V(level, tag, "RD Payload Publish ID : %s",  payload->rdPublish->deviceId.id);
        }

        OC_LOG_V(level, tag, "RD Payload Publish TTL : %d", rdPublish->ttl);

        if (rdPublish->links)
        {
            for (OCRDLinksPayload *temp = payload->rdPublish->links; temp; temp = temp->next)
            {
                OC_LOG_V(level, tag, "RD Payload Publish Link RT : %s", temp->rt);
                OC_LOG_V(level, tag, "RD Payload Publish Link ITF : %s", temp->itf);
                OC_LOG_V(level, tag, "RD Payload Publish Link HREF : %s", temp->href);
            }
        }
    }
}
