//******************************************************************
//
// Copyright 2016 Intel Corporation All Rights Reserved.
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

#include "platform_features.h"
#include <stdlib.h>
#include "oic_malloc.h"
#include "oic_string.h"
#include "ocrandom.h"
#include "ocresourcehandler.h"
#include "cbor.h"
#include "setjmp.h"
#include "ocresource.h"
#include "ocserverrequest.h"
#include "ocpayload.h"
#include "ocpayloadcbor.h"
#include "ocpayloadint.h"

#define TAG "OCPayloadConvert"
// Arbitrarily chosen size that seems to contain the majority of packages
#define INIT_SIZE (255)

// CBOR Array Length
#define DISCOVERY_CBOR_ARRAY_LEN 1
// CBOR Res Map Length
#define DISCOVERY_CBOR_RES_MAP_LEN 2
// CBOR Links Map Length
#define DISCOVERY_CBOR_LINKS_MAP_LEN 4

#define savememory
#ifdef savememory
#define ENCODE_STRING(a, b) cbor_encode_text_string(a, b, sizeof (b) -1);
#define ADD_STRING_TO_MAP(a, b, c) AddTextStringToMap(a, b, c);
#define MAYBE_ADD_STRING_TO_MAP(a, b, c) ConditionalAddTextStringToMap(a, b, c);

static void AddTextStringToMap(CborEncoder *map, const char *key, const char *value)
{
    cbor_encode_text_string(map, key, strlen(key));
    cbor_encode_text_string(map, value, strlen(value));
}

static void ConditionalAddTextStringToMap(CborEncoder *map, const char *key, const char *value)
{
    if (value) {
        AddTextStringToMap(map, key, value);
    }
}
#else
#define ENCODE_STRING(a, b) cbor_encode_text_string(a, b, sizeof (b) - 1);
#define ADD_STRING_TO_MAP(a, b, c) AddTextStringToMap(a, b, sizeof (b) -1, c);
#define MAYBE_ADD_STRING_TO_MAP(a, b, c) ConditionalAddTextStringToMap(a, b, sizeof (b), c);

static void AddTextStringToMap(CborEncoder *map, const char *key, size_t keylen,
        const char *value)
{
    cbor_encode_text_string(map, key, keylen);
    cbor_encode_text_string(map, value, strlen(value));
}

static void ConditionalAddTextStringToMap(CborEncoder *map, const char *key, size_t keylen,
        const char *value)
{
    return value ? AddTextStringToMap(map, key, keylen, value) : 0;
}
#endif // savememory

// Functions all return either a CborError, or a negative version of the OC_STACK return values
static OCStackResult OCConvertRepPayload(OCRepPayload* payload, uint8_t* outPayload, size_t* size);
static OCStackResult OCConvertSecurityPayload(OCSecurityPayload* payload,
                                            uint8_t* outPayload, size_t* size);
static OCStackResult OCConvertSingleRepPayload(CborEncoder* parent, const OCRepPayload* payload);

OCStackResult OCConvertPayload(OCPayload *payload, uint8_t *outPayload, size_t *size)
{
    // TinyCbor Version 47a78569c0 or better on master is required for the re-allocation
    // strategy to work.  If you receive the following assertion error, please do a git-pull
    // from the extlibs/tinycbor/tinycbor directory
    #define CborNeedsUpdating  (CborErrorOutOfMemory < CborErrorDataTooLarge)
    //OC_STATIC_ASSERT(!CborNeedsUpdating, "tinycbor needs to be updated to at least 47a78569c0");
    #undef CborNeedsUpdating

    switch (payload->type)
    {
        case PAYLOAD_TYPE_REPRESENTATION:
            return OCConvertRepPayload((OCRepPayload*)payload, outPayload, size);
        case PAYLOAD_TYPE_SECURITY:
            return OCConvertSecurityPayload((OCSecurityPayload*)payload, outPayload, size);
        default:
            OC_LOG_V(INFO,TAG, "ConvertPayload default %d", payload->type);
            return OC_STACK_NOTIMPL;
    }
    return OC_STACK_ERROR;
}

static OCStackResult checkError(CborEncoder *encoder, uint8_t *outPayload, size_t *size)
{
    size_t needed = 0; // cbor_encoder_get_extra_bytes_needed(encoder);
    if (needed)
    {
        OC_LOG_V(ERROR, TAG, "Convert Payload failed: needs more memory %d", needed);
        return OC_STACK_NO_MEMORY;
    }
    if (size)
    {
        *size = encoder->ptr - outPayload;
    }
    return OC_STACK_OK;
}

static OCStackResult OCConvertSecurityPayload(OCSecurityPayload *payload, uint8_t *outPayload, size_t *size)
{
    CborEncoder encoder;

    cbor_encoder_init(&encoder, outPayload, *size, 0);

    CborEncoder rootArray;
    cbor_encoder_create_array(&encoder, &rootArray, 1);
    CborEncoder map;

    cbor_encoder_create_map(&rootArray, &map, CborIndefiniteLength);

    if (payload->securityData)
    {
        ADD_STRING_TO_MAP(&map, OC_RSRVD_REPRESENTATION, payload->securityData);
    }

    cbor_encoder_close_container(&rootArray, &map);

    cbor_encoder_close_container(&encoder, &rootArray);
    return checkError(&encoder, outPayload, size);
}

#ifdef RT_MAX
static bool OCStringJoin(char *buffer, size_t bufsize, OCResourceText_t *val, int n)
{
    size_t size = 0;

    for (int i = 0; i < n; i++)
    {
        size += 1 + strlen(val[i].name);
    }

    if (size >= bufsize)
    {
        return false;
    }

    buffer[0] = '\0';
    for (int i = 0; i < n; i++)
    {
        if (i)
        {
            OICStrcat(buffer, size + 1, " ");
        }
        OICStrcat(buffer, size + 1, val[i].name);
    }

    return true;
}
#else
static bool OCStringJoin(char *buffer, size_t bufsize, OCResourceText_t *val)
{
    size_t size = 0;
    for (OCResourceText_t *temp = val; temp; temp = temp->next)
    {
        size += 1 + strlen(temp->name);
    }

    if (size >= bufsize)
    {
        return false;
    }

    buffer[0] = '\0';
    for (int i = 0; val; val = val->next, i++)
    {
        if (i)
        {
            OICStrcat(buffer, size + 1, " ");
        }
        OICStrcat(buffer, size + 1, val->name);
    }
    return true;
}
#endif // RT_MAX

static bool OCStringLLJoin(char *buffer, size_t bufsize, OCStringLL *val)
{
    size_t size = 0;

    for (OCStringLL* temp = val; temp; temp = temp->next)
    {
        size += strlen(temp->value);
        if (temp->next)
        {
            size++;
        }
    }

    if (size > bufsize)
    {
        return false;
    }

    for (; val; val = val->next)
    {
        OICStrcat(buffer, bufsize, val->value);
        if (val->next)
        {
            OICStrcat(buffer, bufsize, " ");
        }
    }
    return true;
}

OCStackResult OCBuildDiscoveryPayload(uint8_t *outBuf, size_t *outSize, OCResource *resourceList[], int resourceCount)
{
    CborEncoder encoder;

    cbor_encoder_init(&encoder, outBuf, *outSize, 0);

    CborEncoder rootArray;
    cbor_encoder_create_array(&encoder, &rootArray, resourceCount);

    for (int i = 0; i < resourceCount; i++)
    {
        CborEncoder map;
        OCResource *res = resourceList[i];

        cbor_encoder_create_map(&rootArray, &map, DISCOVERY_CBOR_RES_MAP_LEN);

        // Device ID
        ENCODE_STRING(&map, OC_RSRVD_DEVICE_ID);
        cbor_encode_byte_string(&map, OCGetServerInstanceID(), UUID_SIZE);
        {
            CborEncoder linkArray;
            ENCODE_STRING(&map, OC_RSRVD_LINKS);
            cbor_encoder_create_array(&map, &linkArray, CborIndefiniteLength);
            // Link Map
            {
                // Resource Type
                CborEncoder linkMap;
                char string[128];
                cbor_encoder_create_map(&linkArray, &linkMap, DISCOVERY_CBOR_LINKS_MAP_LEN);

                // Uri
                ADD_STRING_TO_MAP(&linkMap, OC_RSRVD_HREF, res->uri);

                #ifdef RT_MAX
                if (res->nTypes)
                #else
                if (res->rsrcType)
                #endif
                {
                #ifdef RT_MAX
                    if (!OCStringJoin(string, sizeof (string), res->rsrcType, res->nTypes))
                #else
                    if (!OCStringJoin(string, sizeof (string), res->rsrcType))
                #endif
                    {
                        return OC_STACK_NO_MEMORY;
                    }
                    ENCODE_STRING(&linkMap, OC_RSRVD_RESOURCE_TYPE);
                    ENCODE_STRING(&linkMap, string);
                }

                // Interface Types
                #ifdef RI_MAX
                if (res->nInterfaces)
                #else
                if (res->rsrcInterface)
                #endif
                {
                #ifdef RI_MAX
                    if (!OCStringJoin(string, sizeof (string), res->rsrcInterface, res->nInterfaces))
                #else
                    if (!OCStringJoin(string, sizeof (string), res->rsrcInterface))
                #endif
                    {
                        return OC_STACK_NO_MEMORY;
                    }
                    ENCODE_STRING(&linkMap, OC_RSRVD_INTERFACE);
                    ENCODE_STRING(&linkMap, string);
                }

                // Policy
                {
                    CborEncoder policyMap;
                    ENCODE_STRING(&linkMap, OC_RSRVD_POLICY);
                    cbor_encoder_create_map(&linkMap, &policyMap,
                                                          CborIndefiniteLength);

                    // Bitmap
                    ENCODE_STRING(&policyMap, OC_RSRVD_BITMAP);
                    cbor_encode_uint(&policyMap,
                     res->resourceProperties & (OC_OBSERVABLE|OC_DISCOVERABLE));

                    if (res->resourceProperties & OC_SECURE)
                    {
                        ENCODE_STRING(&policyMap, OC_RSRVD_SECURE);
                        cbor_encode_boolean(&policyMap, OC_RESOURCE_SECURE);
                    }

                    cbor_encoder_close_container(&linkMap, &policyMap);
                }
                // Close
                cbor_encoder_close_container(&linkArray, &linkMap);
            }
            cbor_encoder_close_container(&map, &linkArray);
        }
        // Close Item
        cbor_encoder_close_container(&rootArray, &map);
    }
    // Close main array
    cbor_encoder_close_container(&encoder, &rootArray);

    return checkError(&encoder, outBuf, outSize);
}

OCStackResult OCBuildDevicePayload(uint8_t *outBuf, size_t *outSize, OCDeviceInfo *deviceInfo)
{
    CborEncoder encoder;

    jmp_buf jb;
    if (setjmp(jb))
    {
    }

    cbor_encoder_init(&encoder, outBuf, *outSize, 0);
    CborEncoder rootArray;
    cbor_encoder_create_array(&encoder, &rootArray, 1);

    {
        CborEncoder map;
        cbor_encoder_create_map(&rootArray, &map, 2);

        // uri
        ADD_STRING_TO_MAP(&map, OC_RSRVD_HREF, OC_RSRVD_DEVICE_URI);

        // Rep Map
        {
            CborEncoder repMap;
            ENCODE_STRING(&map, OC_RSRVD_REPRESENTATION);
            cbor_encoder_create_map(&map, &repMap, CborIndefiniteLength);

            // Device ID
            ENCODE_STRING(&repMap, OC_RSRVD_DEVICE_ID);
            cbor_encode_byte_string(&repMap, OCGetServerInstanceID(), UUID_SIZE);

            // Device Name
            MAYBE_ADD_STRING_TO_MAP(&repMap, OC_RSRVD_DEVICE_NAME, deviceInfo->deviceName);

            // Device Spec Version
            MAYBE_ADD_STRING_TO_MAP(&repMap, OC_RSRVD_SPEC_VERSION, OC_SPEC_VERSION);

            // Device data Model Version
            MAYBE_ADD_STRING_TO_MAP(&repMap, OC_RSRVD_DATA_MODEL_VERSION, OC_DATA_MODEL_VERSION);

            cbor_encoder_close_container(&map, &repMap);
        }

        // Close Map
        cbor_encoder_close_container(&rootArray, &map);
    }

    // Close main array
    cbor_encoder_close_container(&encoder, &rootArray);

    return checkError(&encoder, outBuf, outSize);
}

OCStackResult OCBuildPlatformPayload(uint8_t *outBuf, size_t *outSize, OCPlatformInfo *platform)
{
    CborEncoder encoder;

    cbor_encoder_init(&encoder, outBuf, *outSize, 0);
    CborEncoder rootArray;
    cbor_encoder_create_array(&encoder, &rootArray, 1);
    {
        CborEncoder map;
        cbor_encoder_create_map(&rootArray, &map, CborIndefiniteLength);

        // uri
        ADD_STRING_TO_MAP(&map, OC_RSRVD_HREF, OC_RSRVD_PLATFORM_URI);

        // Rep Map
        {
            CborEncoder repMap;
            ENCODE_STRING(&map, OC_RSRVD_REPRESENTATION);
            cbor_encoder_create_map(&map, &repMap, CborIndefiniteLength);

            // Platform ID
            MAYBE_ADD_STRING_TO_MAP(&repMap, OC_RSRVD_PLATFORM_ID, platform->platformID);

            // MFG Name
            MAYBE_ADD_STRING_TO_MAP(&repMap, OC_RSRVD_MFG_NAME, platform->manufacturerName);

            // MFG Url
            MAYBE_ADD_STRING_TO_MAP(&repMap, OC_RSRVD_MFG_URL, platform->manufacturerUrl);

            // Model Num
            MAYBE_ADD_STRING_TO_MAP(&repMap, OC_RSRVD_MODEL_NUM, platform->modelNumber);

            // Date of Mfg
            MAYBE_ADD_STRING_TO_MAP(&repMap, OC_RSRVD_MFG_DATE, platform->dateOfManufacture);

            // Platform Version
            MAYBE_ADD_STRING_TO_MAP(&repMap, OC_RSRVD_PLATFORM_VERSION, platform->platformVersion);

            // OS Version
            MAYBE_ADD_STRING_TO_MAP(&repMap, OC_RSRVD_OS_VERSION, platform->operatingSystemVersion);

            // Hardware Version
            MAYBE_ADD_STRING_TO_MAP(&repMap, OC_RSRVD_HARDWARE_VERSION, platform->hardwareVersion);

            // Firmware Version
            MAYBE_ADD_STRING_TO_MAP(&repMap, OC_RSRVD_FIRMWARE_VERSION, platform->firmwareVersion);

            // Support URL
            MAYBE_ADD_STRING_TO_MAP(&repMap, OC_RSRVD_SUPPORT_URL, platform->supportUrl);

            // System Time
            MAYBE_ADD_STRING_TO_MAP(&repMap, OC_RSRVD_SYSTEM_TIME, platform->systemTime);

            cbor_encoder_close_container(&map, &repMap);
        }

        // Close Map
        cbor_encoder_close_container(&rootArray, &map);
    }

    // Close main array
    cbor_encoder_close_container(&encoder, &rootArray);

    return checkError(&encoder, outBuf, outSize);
}

static OCStackResult OCConvertRepMap(CborEncoder *map, const OCRepPayload* payload)
{
    CborEncoder repMap;
    OCStackResult err;

    err = cbor_encoder_create_map(map, &repMap, CborIndefiniteLength);
    if (err == OC_STACK_OK)
    {
        err = OCConvertSingleRepPayload(&repMap, payload);
    }
    cbor_encoder_close_container(map, &repMap);
    return err;
}

static OCStackResult OCConvertSingleRepPayload(CborEncoder *repMap, const OCRepPayload *payload)
{
    OCStackResult err = OC_STACK_OK;
    OCRepPayloadValue* value = payload->values;

    for (; value && (err == OC_STACK_OK); value = value->next)
    {
        cbor_encode_text_string(repMap, value->name, strlen(value->name));
        switch (value->type)
        {
        case OCREP_PROP_NULL:
            cbor_encode_null(repMap);
            break;
        case OCREP_PROP_INT:
            cbor_encode_int(repMap, value->i);
            break;
        case OCREP_PROP_DOUBLE:
            cbor_encode_double(repMap, value->d);
            break;
        case OCREP_PROP_BOOL:
            cbor_encode_boolean(repMap, value->b);
            break;
        case OCREP_PROP_STRING:
            cbor_encode_text_string(repMap, value->str, strlen(value->str));
            break;
        case OCREP_PROP_OBJECT:
            err = OCConvertRepMap(repMap, value->obj);
            break;
        default:
            OC_LOG_V(ERROR, TAG, "Invalid Prop type: %d", value->type);
            break;
        }
    }

    return (err == OC_STACK_OK) ? checkError(repMap, NULL, NULL) : err;
}

static OCStackResult OCConvertRepPayload(OCRepPayload *payload, uint8_t *outPayload, size_t *size)
{
    OCStackResult err = OC_STACK_OK;
    CborEncoder encoder = {{0},0,0,0};

    cbor_encoder_init(&encoder, outPayload, *size, 0);
    CborEncoder rootMap;
    cbor_encoder_create_map(&encoder, &rootMap, CborIndefiniteLength);

    // resource types, interfaces
    if (payload->types || payload->interfaces)
    {
        OC_LOG(INFO, TAG, "Payload has types or interfaces");
        char str[1024];

        if (payload->types)
        {
            OCStringLLJoin(str, sizeof (str), payload->types);
            ENCODE_STRING(&rootMap, OC_RSRVD_RESOURCE_TYPE);
            cbor_encode_text_string(&rootMap, str, strlen(str));
        }
        if (payload->interfaces)
        {
            OCStringLLJoin(str, sizeof (str), payload->interfaces);
            ENCODE_STRING(&rootMap, OC_RSRVD_INTERFACE);
            cbor_encode_text_string(&rootMap, str, strlen(str));
        }
    }

    for (; (payload != NULL) && (err == OC_STACK_OK); payload = payload->next)
    {
        OCConvertSingleRepPayload(&rootMap, payload);
    }

    // Close main array
    cbor_encoder_close_container(&encoder, &rootMap);

    return (err == OC_STACK_OK) ? checkError(&encoder, outPayload, size) : err;
}

#ifdef WITH_PRESENCE
OCStackResult OCBuildPresencePayload(uint8_t *outBuf, size_t *outSize,
                                     uint32_t seqNum, uint32_t maxAge,
                                     OCPresenceTrigger trigger,
                                     const char *resourceType)
{
    CborEncoder encoder;

    cbor_encoder_init(&encoder, outBuf, *outSize, 0);
    CborEncoder rootArray;

    cbor_encoder_create_array(&encoder, &rootArray, 1);

    CborEncoder map;
    cbor_encoder_create_map(&rootArray, &map, CborIndefiniteLength);

    // Sequence Number
    ENCODE_STRING(&map, OC_RSRVD_NONCE);
    cbor_encode_uint(&map, seqNum);

    // Max Age
    ENCODE_STRING(&map, OC_RSRVD_TTL);
    cbor_encode_uint(&map, maxAge);

    // Trigger
    const char* triggerStr = convertTriggerEnumToString(trigger);
    ADD_STRING_TO_MAP(&map, OC_RSRVD_TRIGGER, triggerStr);

    // Resource type name
    if (trigger != OC_PRESENCE_TRIGGER_DELETE)
    {
        MAYBE_ADD_STRING_TO_MAP(&map, OC_RSRVD_RESOURCE_TYPE, resourceType);
    }

    // Close Map
    cbor_encoder_close_container(&rootArray, &map);
    cbor_encoder_close_container(&encoder, &rootArray);

    return checkError(&encoder, outBuf, outSize);
}
#endif // WITH_PRESENCE

