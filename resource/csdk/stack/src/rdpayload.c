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
#include "rdpayload.h"

#include "oic_malloc.h"
#include "oic_string.h"
#include "octypes.h"
#include "ocstack.h"
#include "ocpayload.h"
#include "payload_logging.h"

#define TAG "OIC_RI_RDPAYLOAD"

#define CBOR_ROOT_ARRAY_LENGTH 1

static int64_t OCTagsPayloadToCbor(OCTagsPayload *tags, CborEncoder *setMap);
static int64_t OCLinksPayloadToCbor(OCLinksPayload *links, CborEncoder *setMap);
static int64_t ConditionalAddTextStringToMap(CborEncoder* map, const char* key, const char *value);
static int64_t ConditionalAddIntToMap(CborEncoder *map, const char *tags, const uint64_t *value);
static int64_t AddStringLLToMap(CborEncoder *map, const char *tag, const OCStringLL *value);
static CborError OCTagsCborToPayload(CborValue *tagsMap, OCTagsPayload **tagsPayload);
static CborError OCLinksCborToPayload(CborValue *linksArray, OCLinksPayload **linksPayload);
static CborError FindStringInMap(const CborValue *map, const char *tags, char **value);
static CborError FindIntInMap(const CborValue *map, const char *tags, uint64_t *value);
static CborError FindStringLLInMap(const CborValue *linksMap, const char *tag, OCStringLL **links);
static OCStackResult CreateStringLL(uint8_t numElements, OCResourceHandle handle,
                                    const char* (*getValue)(OCResourceHandle handle, uint8_t i),
                                    OCStringLL **stringLL);

int64_t OCRDPayloadToCbor(const OCRDPayload *rdPayload, uint8_t *outPayload, size_t *size)
{
    int64_t cborEncoderResult = CborNoError;
    int flags = 0;
    CborEncoder encoder;
    VERIFY_PARAM_NON_NULL(TAG, rdPayload, "Invalid input parameter rdPayload");
    VERIFY_PARAM_NON_NULL(TAG, outPayload, "Invalid input parameter outPayload");
    VERIFY_PARAM_NON_NULL(TAG, size, "Invalid input parameter size");

    cbor_encoder_init(&encoder, outPayload, *size, flags);

    if (rdPayload->rdDiscovery)
    {
        CborEncoder map;
        cborEncoderResult |= cbor_encoder_create_map(&encoder, &map, CborIndefiniteLength);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed to create discovery map");

        cborEncoderResult |= ConditionalAddTextStringToMap(&map, OC_RSRVD_DEVICE_NAME,
            rdPayload->rdDiscovery->n.deviceName);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed to add OC_RSRVD_DEVICE_NAME in map");

        cborEncoderResult |= ConditionalAddTextStringToMap(&map, OC_RSRVD_DEVICE_ID,
            (char *)rdPayload->rdDiscovery->di.id);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed to add OC_RSRVD_DEVICE_ID in map");

        {
            uint64_t value = rdPayload->rdDiscovery->sel;
            cborEncoderResult |= ConditionalAddIntToMap(&map, OC_RSRVD_RD_DISCOVERY_SEL, &value);
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed to add RD_DISCOVERY_SEL in map");
        }
        cborEncoderResult |= cbor_encoder_close_container(&encoder, &map);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed closing discovery map");
    }
    else if (rdPayload->rdPublish)
    {
        CborEncoder collMap;
        cborEncoderResult |= cbor_encoder_create_map(&encoder, &collMap, CborIndefiniteLength);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed to create collection map");

        OCResourceCollectionPayload *rdPublish = rdPayload->rdPublish;
        cborEncoderResult |= OCTagsPayloadToCbor(rdPublish->tags, &collMap);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed adding tags payload");
        cborEncoderResult |= OCLinksPayloadToCbor(rdPublish->setLinks, &collMap);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed adding setLinks payload");

        cborEncoderResult |= cbor_encoder_close_container(&encoder, &collMap);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed closing collection map");
    }
    else
    {
        CborEncoder map;
        cborEncoderResult |= cbor_encoder_create_map(&encoder, &map, CborIndefiniteLength);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed entering discovery map");
        cborEncoderResult |= cbor_encoder_close_container(&encoder, &map);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed closing discovery map");
    }

    if (cborEncoderResult == CborErrorOutOfMemory)
    {
        *size += encoder.ptr - encoder.end;
    }
    else
    {
        *size = encoder.ptr - outPayload;
    }

    return cborEncoderResult;

exit:
    OICFree(outPayload);
    return cborEncoderResult;
}

static int64_t OCTagsPayloadToCbor(OCTagsPayload *tags, CborEncoder *setMap)
{
    int64_t cborEncoderResult = CborNoError;

    cborEncoderResult |= ConditionalAddTextStringToMap(setMap, OC_RSRVD_DEVICE_NAME,
        tags->n.deviceName);
    VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed to add OC_RSRVD_DEVICE_NAME in tags map");

    cborEncoderResult |= ConditionalAddTextStringToMap(setMap, OC_RSRVD_DEVICE_ID,
        (char *)tags->di.id);
    VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed to add OC_RSRVD_DEVICE_ID in tags map");

    cborEncoderResult |= ConditionalAddIntToMap(setMap, OC_RSRVD_DEVICE_TTL, &tags->ttl);
    VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed to add OC_RSRVD_TTL in tags map");
exit:
    return cborEncoderResult;
}

static int64_t OCLinksPayloadToCbor(OCLinksPayload *links, CborEncoder *setMap)
{
    CborEncoder linksArray;
    int64_t cborEncoderResult = CborNoError;

    cborEncoderResult |= cbor_encode_text_string(setMap, OC_RSRVD_LINKS,
        sizeof(OC_RSRVD_LINKS) - 1);

    cborEncoderResult |= cbor_encoder_create_array(setMap, &linksArray, CborIndefiniteLength);
    VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed to create Links array");

    while (links)
    {
        CborEncoder linksMap;
        cborEncoderResult |= cbor_encoder_create_map(&linksArray, &linksMap, CborIndefiniteLength);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed to create links map");

        cborEncoderResult |= ConditionalAddTextStringToMap(&linksMap, OC_RSRVD_HREF, links->href);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed to add OC_RSRVD_HREF in links map");

        cborEncoderResult |= ConditionalAddTextStringToMap(&linksMap, OC_RSRVD_REL, links->rel);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed to add OC_RSRVD_REL in links map");

        cborEncoderResult |= AddStringLLToMap(&linksMap, OC_RSRVD_RESOURCE_TYPE, links->rt);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed to add OC_RSRVD_RT in links map");

        cborEncoderResult |= AddStringLLToMap(&linksMap, OC_RSRVD_INTERFACE, links->itf);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed to add OC_RSRVD_ITF in links map");

        // Policy
        CborEncoder policyMap;
        cborEncoderResult |= cbor_encode_text_string(&linksMap, OC_RSRVD_POLICY,
            sizeof(OC_RSRVD_POLICY) - 1);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed adding policy tag to links map");
        cborEncoderResult |= cbor_encoder_create_map(&linksMap, &policyMap, CborIndefiniteLength);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed adding policy map to links map");

        // Bitmap
        cborEncoderResult |= cbor_encode_text_string(&policyMap, OC_RSRVD_BITMAP,
            sizeof(OC_RSRVD_BITMAP) - 1);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed adding bitmap tag to policy map");
        cborEncoderResult |= cbor_encode_uint(&policyMap, links->p);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed adding bitmap value to policy map");

        cborEncoderResult |= cbor_encoder_close_container(&linksMap, &policyMap);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed closing policy map");

        cborEncoderResult |= ConditionalAddTextStringToMap(&linksMap, OC_RSRVD_TITLE, links->title);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed to add OC_RSRVD_TITLE in links map");

        cborEncoderResult |= ConditionalAddTextStringToMap(&linksMap, OC_RSRVD_URI, links->anchor);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed to add OC_RSRVD_URI in links map");

        cborEncoderResult |= ConditionalAddIntToMap(&linksMap, OC_RSRVD_INS, (uint64_t *) &links->ins);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed to add OC_RSRVD_INS in links map");

        cborEncoderResult |= ConditionalAddIntToMap(&linksMap, OC_RSRVD_TTL, &links->ttl);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed to add OC_RSRVD_TTL in links map");

        cborEncoderResult |= AddStringLLToMap(&linksMap, OC_RSRVD_MEDIA_TYPE, links->type);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed to add OC_RSRVD_MT in links map");

        // Finsihed encoding a resource, close the map.
        cborEncoderResult |= cbor_encoder_close_container(&linksArray, &linksMap);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed closing links map");

        links = links->next;
    }
    cborEncoderResult |= cbor_encoder_close_container(setMap, &linksArray);
    VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed closing links array");

exit:
    return cborEncoderResult;
}

OCStackResult OCRDCborToPayload(const CborValue *cborPayload, OCPayload **outPayload)
{
    CborValue *rdCBORPayload = (CborValue *)cborPayload;
    OCStackResult ret = OC_STACK_NO_MEMORY;
    CborError cborFindResult;

    OCRDPayload *rdPayload = OCRDPayloadCreate();
    VERIFY_PARAM_NON_NULL(TAG, rdPayload, "Failed allocating rdPayload");

    ret = OC_STACK_MALFORMED_RESPONSE;

    if (cbor_value_is_map(rdCBORPayload))
    {
        // rdCBORPayload is already inside the main root map.
        OCTagsPayload *tagsPayload = NULL;
        OCLinksPayload *linksPayload = NULL;

        cborFindResult = OCTagsCborToPayload(rdCBORPayload, &tagsPayload);
        VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed parsing tags payload.");

        cborFindResult = OCLinksCborToPayload(rdCBORPayload, &linksPayload);
        VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed parsing links payload.");

        // Move from tags payload to links array.
        cborFindResult = cbor_value_advance(rdCBORPayload);
        VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed advancing rdCborPayload.");

        rdPayload->rdPublish = OCCopyCollectionResource(tagsPayload, linksPayload);
        VERIFY_PARAM_NON_NULL(TAG, rdPayload->rdPublish, "Failed allocating rdPayload->rdPublish");
    }
    // TODO: This logic needs to be modified to check the payload type exactly..
    else if (cbor_value_is_map(rdCBORPayload))
    {
        rdPayload->rdDiscovery = (OCRDDiscoveryPayload *)OICCalloc(1, sizeof(OCRDDiscoveryPayload));
        VERIFY_PARAM_NON_NULL(TAG, rdPayload->rdDiscovery, "Failed allocating discoveryPayload");

        cborFindResult = FindStringInMap(rdCBORPayload, OC_RSRVD_DEVICE_NAME,
                                         &rdPayload->rdDiscovery->n.deviceName);
        VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed finding OC_RSRVD_DEVICE_NAME.");
        char *deviceId = NULL;
        cborFindResult = FindStringInMap(rdCBORPayload, OC_RSRVD_DEVICE_ID, &deviceId);
        if (deviceId)
        {
            memcpy(rdPayload->rdDiscovery->di.id, deviceId, strlen(deviceId));
            OICFree(deviceId);
        }
        VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed finding OC_RSRVD_DEVICE_ID.");

        {
            uint64_t value = 0;
            cborFindResult = FindIntInMap(rdCBORPayload, OC_RSRVD_RD_DISCOVERY_SEL, &value);
            VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed finding OC_RSRVD_RD_DISCOVERY_SEL.");
            rdPayload->rdDiscovery->sel = value;
        }

        cborFindResult =  cbor_value_advance(rdCBORPayload);
        VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed advancing rdCBORPayload.");
    }
    OIC_LOG_PAYLOAD(DEBUG, (OCPayload *) rdPayload);
    *outPayload = (OCPayload *)rdPayload;
    return OC_STACK_OK;

exit:
    OCRDPayloadDestroy(rdPayload);
    return ret;
}

static CborError FindStringInMap(const CborValue *map, const char *tags, char **value)
{
    CborValue curVal;
    CborError cborFindResult = cbor_value_map_find_value(map, tags, &curVal);
    if (CborNoError == cborFindResult && cbor_value_is_text_string(&curVal))
    {
        size_t len = 0;
        cborFindResult = cbor_value_dup_text_string(&curVal, value, &len, NULL);
    }
    return cborFindResult;
}

static CborError FindIntInMap(const CborValue *map, const char *tags, uint64_t *value)
{
    CborValue curVal;
    CborError cborFindResult = cbor_value_map_find_value(map, tags, &curVal);
    if (CborNoError == cborFindResult && cbor_value_is_unsigned_integer(&curVal))
    {
        cborFindResult = cbor_value_get_uint64(&curVal, value);
    }
    return cborFindResult;
}

static CborError FindStringLLInMap(const CborValue *linksMap, const char *tag, OCStringLL **links)
{
    size_t len;
    CborValue rtArray;
    OCStringLL* llPtr = *links;
    CborError cborFindResult = cbor_value_map_find_value(linksMap, tag, &rtArray);
    VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed finding tag");

    CborValue rtVal;
    cborFindResult = cbor_value_enter_container(&rtArray, &rtVal);
    VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed entering container");

    while (cbor_value_is_text_string(&rtVal))
    {
        if (llPtr == NULL)
        {
            llPtr = (OCStringLL *)OICCalloc(1, sizeof(OCStringLL));
            VERIFY_PARAM_NON_NULL(TAG, llPtr, "Failed allocating OCStringLL");
            *links = llPtr;
        }
        else if (llPtr)
        {
            while (llPtr->next)
            {
                llPtr = llPtr->next;
            }
            llPtr->next = (OCStringLL *)OICCalloc(1, sizeof(OCStringLL));
            llPtr = llPtr->next;
            VERIFY_PARAM_NON_NULL(TAG, llPtr, "Failed allocating OCStringLL->next");
        }
        cborFindResult = cbor_value_dup_text_string(&rtVal, &(llPtr->value), &len, NULL);
        VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed duplicating value");
        cborFindResult = cbor_value_advance(&rtVal);
        VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed advancing OCStringLL");
    }

    cborFindResult = cbor_value_leave_container(&rtArray, &rtVal);
    VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed leaving container");

exit:
    return cborFindResult;
}

static CborError OCTagsCborToPayload(CborValue *tagsMap, OCTagsPayload **tagsPayload)
{
    CborError cborFindResult = CborErrorOutOfMemory;
    OCTagsPayload *tags = (OCTagsPayload *)OICCalloc(1, sizeof(OCTagsPayload));
    VERIFY_PARAM_NON_NULL(TAG, tags, "Failed allocating tags");

    cborFindResult = FindStringInMap(tagsMap, OC_RSRVD_DEVICE_NAME, &tags->n.deviceName);
    VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed finding deviceName");

    {
        char *deviceId = NULL;
        cborFindResult = FindStringInMap(tagsMap, OC_RSRVD_DEVICE_ID, &deviceId);
        if (deviceId)
        {
            memcpy(tags->di.id, deviceId, strlen(deviceId));
            OICFree(deviceId);
        }
        VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed finding deviceId");
    }

    cborFindResult = FindIntInMap(tagsMap, OC_RSRVD_DEVICE_TTL, &tags->ttl);
    VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed finding ttl");

    *tagsPayload = tags;
    return cborFindResult;

exit:
    OCFreeTagsResource(tags);
    return cborFindResult;
}

static CborError OCLinksCborToPayload(CborValue *links, OCLinksPayload **linksPayload)
{
    OCLinksPayload *setLinks = NULL;
    CborValue linksMap;
    CborValue linksArray;
    CborError cborFindResult = CborErrorOutOfMemory;

    cborFindResult = cbor_value_map_find_value(links, OC_RSRVD_LINKS, &linksArray);
    VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed finding links array");

    cborFindResult = cbor_value_enter_container(&linksArray, &linksMap);
    VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed entering links map container");

    while (cbor_value_is_map(&linksMap))
    {
        setLinks = (OCLinksPayload *)OICCalloc(1, sizeof(OCLinksPayload));
        VERIFY_PARAM_NON_NULL(TAG, setLinks, "Failed allocating setLinks");

        cborFindResult = FindStringInMap(&linksMap, OC_RSRVD_HREF, &setLinks->href);
        VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed finding href value");

        cborFindResult = FindStringInMap(&linksMap, OC_RSRVD_REL, &setLinks->rel);
        VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed finding rel value");

        cborFindResult = FindStringLLInMap(&linksMap, OC_RSRVD_RESOURCE_TYPE, &setLinks->rt);
        VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed finding rt value");

        cborFindResult = FindStringLLInMap(&linksMap, OC_RSRVD_INTERFACE, &setLinks->itf);
        VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed finding itf value");

        // Policy
        CborValue policyMap;
        cborFindResult = cbor_value_map_find_value(&linksMap, OC_RSRVD_POLICY, &policyMap);
        VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "to find policy tag");

        // Bitmap
        cborFindResult = FindIntInMap(&policyMap, OC_RSRVD_BITMAP, (uint64_t *) &setLinks->p);
        VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed finding bitmap value");

        cborFindResult = FindStringInMap(&linksMap, OC_RSRVD_TITLE, &setLinks->title);
        VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed finding title value");

        cborFindResult = FindStringInMap(&linksMap, OC_RSRVD_URI, &setLinks->anchor);
        VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed finding uri value");

        cborFindResult = FindIntInMap(&linksMap, OC_RSRVD_INS, (uint64_t *) &setLinks->ins);
        VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed finding ins value");

        cborFindResult = FindIntInMap(&linksMap, OC_RSRVD_TTL, &setLinks->ttl);
        VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed finding ttl");

        cborFindResult = FindStringLLInMap(&linksMap, OC_RSRVD_MEDIA_TYPE, &setLinks->type);
        VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed finding mt value");

        if (!*linksPayload)
        {
            *linksPayload = setLinks;
        }
        else
        {
            OCLinksPayload *temp = *linksPayload;
            while (temp->next)
            {
                temp = temp->next;
            }
            temp->next = setLinks;
        }
        cborFindResult = cbor_value_advance(&linksMap);
        VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed advancing links map");
    }

    return cborFindResult;

exit:
    OCFreeLinksResource(*linksPayload);
    OCFreeLinksResource(setLinks);
    return cborFindResult;
}

static int64_t AddTextStringToMap(CborEncoder* map, const char* key, const char* value)
{
    int64_t err = cbor_encode_text_string(map, key, strlen(key));
    VERIFY_CBOR_SUCCESS(TAG, err, "Failed setting key value");
    err |= cbor_encode_text_string(map, value, strlen(value));
exit:
    return err;
}

static int64_t ConditionalAddTextStringToMap(CborEncoder* map, const char* key, const char* value)
{
    return value ? AddTextStringToMap(map, key, value) : CborNoError;
}

static int64_t ConditionalAddIntToMap(CborEncoder *map, const char *tags, const uint64_t *value)
{
    int64_t err = CborNoError;
    if (*value)
    {
        err |= cbor_encode_text_string(map, tags, strlen(tags));
        VERIFY_CBOR_SUCCESS(TAG, err, "failed setting value");
        err |= cbor_encode_uint(map, *value);
    }
exit:
    return err;
}

static int64_t AddStringLLToMap(CborEncoder *map, const char *tag, const OCStringLL *strType)
{
    CborEncoder array;
    int64_t cborEncoderResult = CborNoError;

    cborEncoderResult |= cbor_encode_text_string(map, tag, strlen(tag));
    VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed encoding string tag name");
    cborEncoderResult |= cbor_encoder_create_array(map, &array, CborIndefiniteLength);
    VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed creating stringLL array");
    while (strType)
    {
        cborEncoderResult |= cbor_encode_text_string(&array, strType->value, strlen(strType->value));
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed encoding string value");
        strType = strType->next;
    }
    cborEncoderResult |= cbor_encoder_close_container(map, &array);
    VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed closing string array");
exit:
    return cborEncoderResult;
}

OCRDPayload *OCRDPayloadCreate()
{
    OCRDPayload *rdPayload = (OCRDPayload *)OICCalloc(1, sizeof(OCRDPayload));
    VERIFY_PARAM_NON_NULL(TAG, rdPayload, "Failed allocating rdPayload");
    rdPayload->base.type = PAYLOAD_TYPE_RD;

exit:
    return rdPayload;
}

#ifdef RD_CLIENT
OCRDPayload *OCRDPublishPayloadCreate(const unsigned char *id,
                                      OCResourceHandle *resourceHandles, uint8_t nHandles,
                                      uint64_t ttl)
{
    OCTagsPayload *tagsPayload = NULL;
    OCLinksPayload *linksPayload = NULL;
    OCStringLL *rt = NULL;
    OCStringLL *itf = NULL;
    OCStringLL *mt = NULL;
    OCResourceProperty p = OC_RES_PROP_NONE;
    uint8_t ins = 0;

    OCRDPayload *rdPayload = OCRDPayloadCreate();
    if (!rdPayload)
    {
        return NULL;
    }

    tagsPayload = OCCopyTagsResources(NULL, id, ttl);
    if (!tagsPayload)
    {
        goto exit;
    }

    for (uint8_t j = 0; j < nHandles; j++)
    {
        OCResourceHandle handle = resourceHandles[j];
        if (handle)
        {
            rt = NULL;
            itf = NULL;
            mt = NULL;
            ins = 0;
            const char *uri = OCGetResourceUri(handle);
            uint8_t numElement = 0;
            if (OC_STACK_OK == OCGetNumberOfResourceTypes(handle, &numElement))
            {
                OCStackResult res = CreateStringLL(numElement, handle, OCGetResourceTypeName, &rt);
                if (OC_STACK_OK != res || !rt)
                {
                    goto exit;
                }
            }

            if (OC_STACK_OK == OCGetNumberOfResourceInterfaces(handle, &numElement))
            {
                OCStackResult res = CreateStringLL(numElement, handle, OCGetResourceInterfaceName,
                                                   &itf);
                if (OC_STACK_OK != res || !itf)
                {
                    goto exit;
                }
            }

            p = OCGetResourceProperties(handle);
            p = (OCResourceProperty) ((p & OC_DISCOVERABLE) | (p & OC_OBSERVABLE));

            OCStackResult res = OCGetResourceIns(handle, &ins);
            if (OC_STACK_OK != res)
            {
                goto exit;
            }

            mt = (OCStringLL *)OICCalloc(1, sizeof(OCStringLL));
            if (!mt)
            {
                goto exit;
            }
            mt->value = OICStrdup(DEFAULT_MESSAGE_TYPE);
            if (!mt->value)
            {
                goto exit;
            }

            if (!linksPayload)
            {
                linksPayload = OCCopyLinksResources(uri, NULL, rt, itf, p, NULL,
                                                    NULL, ins, ttl, mt);;
                if (!linksPayload)
                {
                    goto exit;
                }
            }
            else
            {
                OCLinksPayload *temp = linksPayload;
                while (temp->next)
                {
                    temp = temp->next;
                }
                temp->next = OCCopyLinksResources(uri, NULL, rt, itf, p, NULL,
                                                  NULL, ins, ttl, mt);
                if (!temp->next)
                {
                    goto exit;
                }
            }
            OCFreeOCStringLL(rt);
            OCFreeOCStringLL(itf);
            OCFreeOCStringLL(mt);
        }
    }

    rdPayload->rdPublish = OCCopyCollectionResource(tagsPayload, linksPayload);
    if (!rdPayload->rdPublish)
    {
        goto exit;
    }

    return rdPayload;

exit:
    if (rt)
    {
        OCFreeOCStringLL(rt);
    }
    if (itf)
    {
        OCFreeOCStringLL(itf);
    }
    if (mt)
    {
        OCFreeOCStringLL(mt);
    }
    if (tagsPayload)
    {
        OCFreeTagsResource(tagsPayload);
    }
    if (linksPayload)
    {
        OCFreeLinksResource(linksPayload);
    }
    OCRDPayloadDestroy(rdPayload);
    return NULL;
}
#endif
OCRDDiscoveryPayload *OCRDDiscoveryPayloadCreate(const char *deviceName, const char *id, int biasFactor)
{
    OCRDDiscoveryPayload *discoveryPayload = (OCRDDiscoveryPayload *)OICCalloc(1, sizeof(OCRDDiscoveryPayload));
    VERIFY_PARAM_NON_NULL(TAG, discoveryPayload, "Failed allocating memory for discovery payload");

    if (deviceName)
    {
        discoveryPayload->n.deviceName = OICStrdup(deviceName);
        VERIFY_PARAM_NON_NULL(TAG, discoveryPayload->n.deviceName,
                "Failed allocating memory for discovery device name");
    }
    if (id)
    {
        OICStrcpy((char*)discoveryPayload->di.id, MAX_IDENTITY_SIZE, id);
    }

    discoveryPayload->sel = biasFactor;
    return discoveryPayload;
exit:
    OICFree(discoveryPayload);
    discoveryPayload = NULL;
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
        if (payload->rdDiscovery->n.deviceName)
        {
            OICFree(payload->rdDiscovery->n.deviceName);
        }
        OICFree(payload->rdDiscovery);
    }

    if (payload->rdPublish)
    {
        OCResourceCollectionPayload *col = payload->rdPublish;

        if (col->setLinks)
        {
            OCFreeLinksResource(col->setLinks);
        }

        if (col->tags)
        {
            OCFreeTagsResource(col->tags);
        }
        OICFree(col);
    }

    OICFree(payload);
}

OCTagsPayload* OCCopyTagsResources(const char *deviceName, const unsigned char *id, uint64_t ttl)
{
    OCTagsPayload *tags = (OCTagsPayload *)OICCalloc(1, sizeof(OCTagsPayload));
    if (!tags)
    {
        return NULL;
    }
    if (deviceName)
    {
        tags->n.deviceName = OICStrdup(deviceName);
        if (!tags->n.deviceName)
        {
            goto memory_allocation_failed;
        }
    }
    if (id)
    {
        OICStrcpy((char*)tags->di.id, MAX_IDENTITY_SIZE, (char *)id);
    }
    tags->ttl = ttl;
    return tags;

memory_allocation_failed:
    OIC_LOG(ERROR, TAG, "Memory allocation failed.");
    OCFreeTagsResource(tags);
    return NULL;
}

OCLinksPayload* OCCopyLinksResources(const char *href, const char *rel, OCStringLL *rt,
                                     OCStringLL *itf, uint8_t p, const char *title,
                                     const char *anchor, uint8_t ins, uint64_t ttl,
                                     OCStringLL *mt)
{
    OCLinksPayload *links = (OCLinksPayload *)OICCalloc(1, sizeof(OCLinksPayload));
    if (!links)
    {
        OIC_LOG(ERROR, TAG, "Failed allocating memory.");
        return NULL;
    }
    if (href)
    {
        links->href = OICStrdup(href);
        if (!links->href)
        {
            goto memory_allocation_failed;
        }
    }
    if (rel)
    {
        links->rel = OICStrdup(rel);
        if (!links->rel)
        {
            goto memory_allocation_failed;
        }
    }
    if (rt)
    {
        links->rt = CloneOCStringLL(rt);
        if (!links->rt)
        {
            goto memory_allocation_failed;
        }
    }
    if (itf)
    {
        links->itf = CloneOCStringLL(itf);
        if (!links->itf)
        {
            goto memory_allocation_failed;
        }
    }
    links->p = p;
    if (title)
    {
        links->title = OICStrdup(title);
        if (!links->title)
        {
            goto memory_allocation_failed;
        }
    }
    if (anchor)
    {
        links->anchor = OICStrdup(anchor);
        if (!links->anchor)
        {
            goto memory_allocation_failed;
        }
    }
    links->ins = ins;
    links->ttl = ttl;
    if (mt)
    {
        links->type = CloneOCStringLL(mt);
        if (!links->type)
        {
            goto memory_allocation_failed;
        }
    }
    links->next = NULL;
    return links;

memory_allocation_failed:
    OIC_LOG(ERROR, TAG, "Memory allocation failed.");
    OCFreeLinksResource(links);
    return NULL;
}

OCResourceCollectionPayload* OCCopyCollectionResource(OCTagsPayload *tags, OCLinksPayload *links)
{
    OCResourceCollectionPayload *pl =  NULL;
    VERIFY_PARAM_NON_NULL(TAG, tags, "Invalid param tags");
    VERIFY_PARAM_NON_NULL(TAG, links, "Invalid param links");

    pl = (OCResourceCollectionPayload *)OICCalloc(1, sizeof(OCResourceCollectionPayload));
    VERIFY_PARAM_NON_NULL(TAG, pl, "Failed allocating memory for the OCResourceCollectionPayload");

    pl->tags = tags;
    pl->setLinks = links;

exit:
    return pl;
}

void OCFreeLinksResource(OCLinksPayload *links)
{
    if (!links)
    {
        return;
    }
    OICFree(links->href);
    OICFree(links->rel);
    OCFreeOCStringLL(links->rt);
    OCFreeOCStringLL(links->itf);
    OICFree(links->title);
    OICFree(links->anchor);
    OCFreeOCStringLL(links->type);
    OCFreeLinksResource(links->next);
    OICFree(links);
}

void OCFreeTagsResource(OCTagsPayload *payload)
{
    if (!payload)
    {
        return;
    }
    OICFree(payload->n.deviceName);;
    OICFree(payload);
}

void OCFreeCollectionResource(OCResourceCollectionPayload *payload)
{
    if (!payload)
    {
        return;
    }
    if (payload->tags)
    {
        OCFreeTagsResource(payload->tags);
    }
    if (payload->setLinks)
    {
        OCFreeLinksResource(payload->setLinks);
    }
    OICFree(payload);
}

static OCStackResult CreateStringLL(uint8_t numElements, OCResourceHandle handle,
                                    const char* (*getValue)(OCResourceHandle handle, uint8_t i),
                                    OCStringLL **stringLL)
{
    for (uint8_t i = 0; i < numElements; ++i)
    {
        const char *value = getValue(handle, i);
        OIC_LOG_V(ERROR, TAG, "value: %s", value);
        if (!*stringLL)
        {
            *stringLL = (OCStringLL *)OICCalloc(1, sizeof(OCStringLL));
            if (!*stringLL)
            {
                OIC_LOG(ERROR, TAG, "Failed allocating memory.");
                return OC_STACK_NO_MEMORY;
            }
            (*stringLL)->value = OICStrdup(value);
            if (!(*stringLL)->value)
            {
                OIC_LOG(ERROR, TAG, "Failed copying to OCStringLL.");
                return OC_STACK_NO_MEMORY;
            }
        }
        else
        {
            OCStringLL *cur = *stringLL;
            while (cur->next)
            {
                cur = cur->next;
            }
            cur->next = (OCStringLL *)OICCalloc(1, sizeof(OCStringLL));
            if (!cur->next)
            {
                OIC_LOG(ERROR, TAG, "Failed allocating memory.");
                return OC_STACK_NO_MEMORY;
            }
            cur->next->value = OICStrdup(value);
            if (!cur->next->value)
            {
                OIC_LOG(ERROR, TAG, "Failed copying to OCStringLL.");
                return OC_STACK_NO_MEMORY;
            }
        }
    }
    return OC_STACK_OK;
}
