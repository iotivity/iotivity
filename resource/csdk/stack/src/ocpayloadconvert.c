//******************************************************************
//
// Copyright 2015 Intel Mobile Communications GmbH All Rights Reserved.
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

#include "ocpayloadcbor.h"
#include <stdlib.h>
#include "oic_malloc.h"
#include "logger.h"
#include "ocpayload.h"
#include "ocrandom.h"
#include "ocresourcehandler.h"

#define TAG PCF("OCPayloadConvert")

OCStackResult ConvertDiscoveryPayload(OCDiscoveryPayload* payload, uint8_t** outPayload,
        size_t* size);
OCStackResult ConvertDevicePayload(OCDevicePayload* payload, uint8_t** outPayload,
        size_t* size);
OCStackResult ConvertPlatformPayload(OCPlatformPayload* payload, uint8_t** outPayload,
        size_t* size);
OCStackResult ConvertRepPayload(OCRepPayload* payload, uint8_t** outPayload, size_t* size);
OCStackResult ConvertPresencePayload(OCPresencePayload* payload, uint8_t** outPayload,
        size_t* size);

CborError AddTextStringToMap(CborEncoder* map, const char* key, size_t keylen,
        const char* value);

CborError ConditionalAddTextStringToMap(CborEncoder* map, const char* key, size_t keylen,
        const char* value);


OCStackResult ConvertPayload(OCPayload* payload, uint8_t** outPayload, size_t* size)
{
    printf("Converting payload of type %d\n", payload->type);
    switch(payload->type)
    {
        case PAYLOAD_TYPE_DISCOVERY:
            return ConvertDiscoveryPayload((OCDiscoveryPayload*)payload, outPayload, size);
        case PAYLOAD_TYPE_DEVICE:
            return ConvertDevicePayload((OCDevicePayload*)payload, outPayload, size);
        case PAYLOAD_TYPE_PLATFORM:
            return ConvertPlatformPayload((OCPlatformPayload*)payload, outPayload, size);
        case PAYLOAD_TYPE_REPRESENTATION:
            return ConvertRepPayload((OCRepPayload*)payload, outPayload, size);
        case PAYLOAD_TYPE_PRESENCE:
            return ConvertPresencePayload((OCPresencePayload*)payload, outPayload, size);
        default:
            printf("ConvertPayload default %d\n", payload->type);
            exit(-1);
            return OC_STACK_NOTIMPL;
    }
}

OCStackResult ConvertDiscoveryPayload(OCDiscoveryPayload* payload, uint8_t** outPayload,
        size_t* size)
{
    *outPayload = (uint8_t*)OICCalloc(1, MAX_REQUEST_LENGTH);
    *size = MAX_REQUEST_LENGTH;

    if(!*outPayload)
    {
        return OC_STACK_NO_MEMORY;
    }

    CborEncoder encoder = {};
    CborError err = CborNoError;
    size_t resourceCount =  OCDiscoveryPayloadGetResourceCount(payload);

    cbor_encoder_init(&encoder, *outPayload, *size, 0);

    CborEncoder rootArray;
    err = err | cbor_encoder_create_array(&encoder, &rootArray, 1 + resourceCount);
    err = err | cbor_encode_uint(&rootArray, PAYLOAD_TYPE_DISCOVERY);

    for(size_t i = 0; i < resourceCount; ++i)
    {
        CborEncoder map;
        OCResourcePayload* resource = OCDiscoveryPayloadGetResource(payload, i);
        err = err | cbor_encoder_create_map(&rootArray, &map, 3);
        // Uri
        err = err | AddTextStringToMap(&map, OC_RSRVD_HREF,
                sizeof(OC_RSRVD_HREF) - 1,
                resource->uri);

        // Server ID
        err = err | cbor_encode_text_string(&map, OC_RSRVD_SERVER_INSTANCE_ID,
                sizeof(OC_RSRVD_SERVER_INSTANCE_ID) - 1);
        err = err | cbor_encode_byte_string(&map, resource->sid, UUID_SIZE);
        // Prop Tag
        {
            CborEncoder propMap;
            err = err | cbor_encode_text_string(&map, OC_RSRVD_PROPERTY,
                    sizeof(OC_RSRVD_PROPERTY) -1 );
            err = err | cbor_encoder_create_map(&map, &propMap, 3);

            // Resource Type
            {
                CborEncoder rtArray;
                err = err | cbor_encode_text_string(&propMap, OC_RSRVD_RESOURCE_TYPE,
                    sizeof(OC_RSRVD_RESOURCE_TYPE) - 1);
                err = err | cbor_encoder_create_array(&propMap, &rtArray, CborIndefiniteLength);

                OCStringLL* rtPtr = resource->types;
                while(rtPtr)
                {
                    err = err | cbor_encode_text_string(&rtArray, rtPtr->value,
                            strlen(rtPtr->value));
                    rtPtr = rtPtr->next;
                }

                err = err | cbor_encoder_close_container(&propMap, &rtArray);
            }

            // Interface Types
            {
                CborEncoder ifArray;
                err = err | cbor_encode_text_string(&propMap, OC_RSRVD_INTERFACE,
                        sizeof(OC_RSRVD_INTERFACE) - 1);
                err = err | cbor_encoder_create_array(&propMap, &ifArray, CborIndefiniteLength);
                OCStringLL* ifPtr = resource->interfaces;

                while(ifPtr)
                {
                    err = err | cbor_encode_text_string(&ifArray, ifPtr->value,
                        strlen(ifPtr->value));
                    ifPtr= ifPtr->next;
                }

                err = err | cbor_encoder_close_container(&propMap, &ifArray);
            }
            // Policy
            {
                CborEncoder policyMap;
                err = err | cbor_encode_text_string(&propMap, OC_RSRVD_POLICY,
                        sizeof(OC_RSRVD_POLICY) - 1);
                err = err | cbor_encoder_create_map(&propMap, &policyMap, CborIndefiniteLength);

                // Bitmap
                err = err | cbor_encode_text_string(&policyMap, OC_RSRVD_BITMAP,
                        sizeof(OC_RSRVD_BITMAP) - 1);
                err = err | cbor_encode_uint(&policyMap, resource->bitmap);

                if(resource->secure)
                {
                    err = err | cbor_encode_text_string(&policyMap, OC_RSRVD_SECURE,
                            sizeof(OC_RSRVD_SECURE) - 1);
                    err = err | cbor_encode_boolean(&policyMap, OC_RESOURCE_SECURE);

                    if(resource->port != 0)
                    {
                        err = err | cbor_encode_text_string(&policyMap, OC_RSRVD_HOSTING_PORT,
                                sizeof(OC_RSRVD_HOSTING_PORT) - 1);
                        err = err | cbor_encode_uint(&policyMap, resource->port);
                    }
                }

                err = err | cbor_encoder_close_container(&propMap, &policyMap);
            }
            // Close
            err = err | cbor_encoder_close_container(&map, &propMap);
        }
        // Close Item
        err = err | cbor_encoder_close_container(&rootArray, &map);
    }
    // Close main array
    err = err | cbor_encoder_close_container(&encoder, &rootArray);

    if(err != CborNoError)
    {
        printf("Convert Discovery Payload failed with : %d\n", err);
        OC_LOG_V(ERROR, TAG, "Convert Discovery Payload failed with : %d\n", err);
        // TODO: remove printf and exit
        exit(-1);
        return OC_STACK_ERROR;
    }

    *size = encoder.ptr - *outPayload;
    uint8_t* tempPayload = (uint8_t*)OICRealloc(*outPayload, *size);

    if(!tempPayload)
    {
        OC_LOG_V(ERROR, TAG, PCF("Payload realloc failed!\n"));
        OICFree(*outPayload);
        return OC_STACK_ERROR;
    }

    *outPayload = tempPayload;
    return OC_STACK_OK;
}

OCStackResult ConvertDevicePayload(OCDevicePayload* payload, uint8_t** outPayload,
        size_t* size)
{
    *outPayload = (uint8_t*)OICCalloc(1, MAX_REQUEST_LENGTH);
    *size = MAX_REQUEST_LENGTH;

    if(!*outPayload)
    {
        return OC_STACK_NO_MEMORY;
    }

    CborEncoder encoder = {};
    CborError err = CborNoError;

    cbor_encoder_init(&encoder, *outPayload, *size, 0);
    CborEncoder rootArray;
    err = err | cbor_encoder_create_array(&encoder, &rootArray, 2);
    err = err | cbor_encode_uint(&rootArray, PAYLOAD_TYPE_DEVICE);

    {
        CborEncoder map;
        err = err | cbor_encoder_create_map(&rootArray, &map, 2);

        // uri
        err = err | AddTextStringToMap(&map, OC_RSRVD_HREF, sizeof(OC_RSRVD_HREF) - 1,
                payload->uri);

        // Rep Map
        {
            CborEncoder repMap;
            err = err | cbor_encode_text_string(&map, OC_RSRVD_REPRESENTATION,
                    sizeof(OC_RSRVD_REPRESENTATION) - 1);
            err = err | cbor_encoder_create_map(&map, &repMap, 4);

            // Device ID
            err = err | cbor_encode_text_string(&repMap, OC_RSRVD_DEVICE_ID,
                    sizeof(OC_RSRVD_DEVICE_ID) - 1);
            err = err | cbor_encode_byte_string(&repMap, payload->sid, UUID_SIZE);

            // Device Name
            err = err | AddTextStringToMap(&repMap, OC_RSRVD_DEVICE_NAME,
                    sizeof(OC_RSRVD_DEVICE_NAME) - 1,
                    payload->deviceName);

            // Device Spec Version
            err = err | AddTextStringToMap(&repMap, OC_RSRVD_SPEC_VERSION,
                    sizeof(OC_RSRVD_SPEC_VERSION) - 1,
                    payload->specVersion);

            // Device data Model Version
            err = err | AddTextStringToMap(&repMap, OC_RSRVD_DATA_MODEL_VERSION,
                    sizeof(OC_RSRVD_DATA_MODEL_VERSION) - 1,
                    payload->dataModelVersion);

            err = err | cbor_encoder_close_container(&map, &repMap);
        }

        // Close Map
        err = err | cbor_encoder_close_container(&rootArray, &map);
    }

    // Close main array
    err = err | cbor_encoder_close_container(&encoder, &rootArray);

    if(err != CborNoError)
    {
        printf("Convert Device Payload failed with : %d\n", err);
        OC_LOG_V(ERROR, TAG, "Convert Device Payload failed with : %d\n", err);
        // TODO: remove printf and exit
        exit(-1);
        return OC_STACK_ERROR;
    }

    *size = encoder.ptr - *outPayload;
    uint8_t* tempPayload = (uint8_t*)OICRealloc(*outPayload, *size);

    if(!tempPayload)
    {
        OC_LOG_V(ERROR, TAG, PCF("Payload realloc failed!\n"));
        OICFree(*outPayload);
        return OC_STACK_ERROR;
    }

    *outPayload = tempPayload;
    return OC_STACK_OK;
}

OCStackResult ConvertPlatformPayload(OCPlatformPayload* payload, uint8_t** outPayload,
        size_t* size)
{
    *outPayload = (uint8_t*)OICCalloc(1, MAX_REQUEST_LENGTH);
    *size = MAX_REQUEST_LENGTH;

    if(!*outPayload)
    {
        return OC_STACK_NO_MEMORY;
    }

    CborEncoder encoder = {};
    CborError err = CborNoError;

    cbor_encoder_init(&encoder, *outPayload, *size, 0);
    CborEncoder rootArray;
    err = err | cbor_encoder_create_array(&encoder, &rootArray, 2);
    err = err | cbor_encode_uint(&rootArray, PAYLOAD_TYPE_PLATFORM);
    {
        CborEncoder map;
        err = err | cbor_encoder_create_map(&rootArray, &map, CborIndefiniteLength);

        // uri
        err = err | AddTextStringToMap(&map, OC_RSRVD_HREF, sizeof(OC_RSRVD_HREF) - 1,
                payload->uri);

        // Rep Map
        {
            CborEncoder repMap;
            err = err | cbor_encode_text_string(&map, OC_RSRVD_REPRESENTATION,
                    sizeof(OC_RSRVD_REPRESENTATION) - 1);
            err = err | cbor_encoder_create_map(&map, &repMap, 4);

            // Platform ID
            err = err | AddTextStringToMap(&repMap, OC_RSRVD_PLATFORM_ID,
                    sizeof(OC_RSRVD_PLATFORM_ID) - 1,
                    payload->info.platformID);

            // MFG Name
            err = err | AddTextStringToMap(&repMap, OC_RSRVD_MFG_NAME,
                    sizeof(OC_RSRVD_MFG_NAME) - 1,
                    payload->info.manufacturerName);

            // MFG Url
            err = err | ConditionalAddTextStringToMap(&repMap, OC_RSRVD_MFG_URL,
                    sizeof(OC_RSRVD_MFG_URL) - 1,
                    payload->info.manufacturerUrl);

            // Model Num
            err = err | ConditionalAddTextStringToMap(&repMap, OC_RSRVD_MODEL_NUM,
                    sizeof(OC_RSRVD_MODEL_NUM) - 1,
                    payload->info.modelNumber);

            // Date of Mfg
            err = err | ConditionalAddTextStringToMap(&repMap, OC_RSRVD_MFG_DATE,
                    sizeof(OC_RSRVD_MFG_DATE) - 1,
                    payload->info.dateOfManufacture);

            // Platform Version
            err = err | ConditionalAddTextStringToMap(&repMap, OC_RSRVD_PLATFORM_VERSION,
                    sizeof(OC_RSRVD_PLATFORM_VERSION) - 1,
                    payload->info.platformVersion);

            // OS Version
            err = err | ConditionalAddTextStringToMap(&repMap, OC_RSRVD_OS_VERSION,
                    sizeof(OC_RSRVD_OS_VERSION) - 1,
                    payload->info.operatingSystemVersion);

            // Hardware Version
            err = err | ConditionalAddTextStringToMap(&repMap, OC_RSRVD_HARDWARE_VERSION,
                    sizeof(OC_RSRVD_HARDWARE_VERSION) - 1,
                    payload->info.hardwareVersion);

            // Firmware Version
            err = err | ConditionalAddTextStringToMap(&repMap, OC_RSRVD_FIRMWARE_VERSION,
                    sizeof(OC_RSRVD_FIRMWARE_VERSION) - 1,
                    payload->info.firmwareVersion);

            // Support URL
            err = err | ConditionalAddTextStringToMap(&repMap, OC_RSRVD_SUPPORT_URL,
                    sizeof(OC_RSRVD_SUPPORT_URL) - 1,
                    payload->info.supportUrl);

            // System Time
            err = err | ConditionalAddTextStringToMap(&repMap, OC_RSRVD_SYSTEM_TIME,
                    sizeof(OC_RSRVD_SYSTEM_TIME) - 1,
                    payload->info.systemTime);
            err = err | cbor_encoder_close_container(&map, &repMap);
        }

        // Close Map
        err = err | cbor_encoder_close_container(&rootArray, &map);
    }

    // Close main array
    err = err | cbor_encoder_close_container(&encoder, &rootArray);

    if(err != CborNoError)
    {
        printf("Convert Platform Payload failed with : %d\n", err);
        OC_LOG_V(ERROR, TAG, "Convert Platform Payload failed with : %d\n", err);
        // TODO: remove printf and exit
        exit(-1);
        return OC_STACK_ERROR;
    }

    *size = encoder.ptr - *outPayload;
    uint8_t* tempPayload = (uint8_t*)OICRealloc(*outPayload, *size);

    if(!tempPayload)
    {
        OC_LOG_V(ERROR, TAG, PCF("Payload realloc failed!\n"));
        OICFree(*outPayload);
        return OC_STACK_ERROR;
    }

    *outPayload = tempPayload;

    return OC_STACK_OK;
}

CborError ConvertSingleRepPayload(CborEncoder* parent, const OCRepPayload* payload);

CborError ConvertArray(CborEncoder* parent, const OCRepPayloadValueArray* valArray)
{
    CborEncoder array;
    CborError err = CborNoError;

    err = err | cbor_encoder_create_array(parent, &array, CborIndefiniteLength);
    err = err | cbor_encode_uint(&array, valArray->type);
    for(int i = 0; i < MAX_REP_ARRAY_DEPTH; ++i)
    {
        err = err | cbor_encode_uint(&array, valArray->dimensions[i]);
    }

    size_t dimTotal = calcDimTotal(valArray->dimensions);

    for(size_t i = 0; i < dimTotal; ++i)
    {
        switch(valArray->type)
        {
            case OCREP_PROP_NULL:
                OC_LOG(ERROR, TAG, PCF("ConvertArray Invalid NULL"));
                err = CborUnknownError;
                break;
            case OCREP_PROP_INT:
                err = err | cbor_encode_int(&array, valArray->iArray[i]);
                break;
            case OCREP_PROP_DOUBLE:
                err = err | cbor_encode_double(&array, &(valArray->dArray[i]));
                break;
            case OCREP_PROP_BOOL:
                err = err | cbor_encode_boolean(&array, valArray->bArray[i]);
                break;
            case OCREP_PROP_STRING:
                err = err | cbor_encode_text_string(&array, valArray->strArray[i],
                        strlen(valArray->strArray[i]));
                break;
            case OCREP_PROP_OBJECT:
                err = ConvertSingleRepPayload(&array, valArray->objArray[i]);
                break;
            case OCREP_PROP_ARRAY:
                OC_LOG(ERROR, TAG, PCF("ConvertArray Invalid child array"));
                err = CborUnknownError;
                break;
        }
    }

    err = err | cbor_encoder_close_container(parent, &array);
    return err;
}

CborError ConvertSingleRepPayload(CborEncoder* parent, const OCRepPayload* payload)
{
    CborError err = CborNoError;
    CborEncoder map;
    err = err | cbor_encoder_create_map(parent, &map, CborIndefiniteLength);

    // Uri
    err = err | ConditionalAddTextStringToMap(&map, OC_RSRVD_HREF,
            sizeof(OC_RSRVD_HREF) - 1,
            payload->uri);

    // Prop Map
    // resource types, interfaces
    if(payload->types || payload->interfaces)
    {
        printf("Has types or interfaces\n");
        err = err | cbor_encode_text_string(&map,
                OC_RSRVD_PROPERTY,
                sizeof(OC_RSRVD_PROPERTY) - 1);
        CborEncoder propMap;
        err = err | cbor_encoder_create_map(&map, &propMap, 2);

        CborEncoder curArray;
        if(payload->types)
        {
            err = err | cbor_encode_text_string(&propMap,
                    OC_RSRVD_RESOURCE_TYPE,
                    sizeof(OC_RSRVD_RESOURCE_TYPE) - 1);
            err = err | cbor_encoder_create_array(&propMap, &curArray, CborIndefiniteLength);
            OCStringLL* val = payload->types;
            while(val)
            {
                err = err | cbor_encode_text_string(&curArray, val->value, strlen(val->value));
                val = val->next;
            }
            err = err | cbor_encoder_close_container(&propMap, &curArray);
        }
        if(payload->interfaces)
        {
            err = err | cbor_encode_text_string(&propMap,
                    OC_RSRVD_INTERFACE,
                    sizeof(OC_RSRVD_INTERFACE) - 1);
            err = err | cbor_encoder_create_array(&propMap, &curArray, CborIndefiniteLength);
            OCStringLL* val = payload->interfaces;
            while(val)
            {
                err = err | cbor_encode_text_string(&curArray, val->value, strlen(val->value));
                val = val->next;
            }
            err = err | cbor_encoder_close_container(&propMap, &curArray);
        }
        err = err | cbor_encoder_close_container(&map, &propMap);
    }

    // Rep Map
    {
        CborEncoder repMap;
        err = err | cbor_encode_text_string(&map,
                OC_RSRVD_REPRESENTATION,
                sizeof(OC_RSRVD_REPRESENTATION) - 1);
        err = err | cbor_encoder_create_map(&map, &repMap, CborIndefiniteLength);
        OCRepPayloadValue* value = payload->values;
        while(value)
        {
            err = err | cbor_encode_text_string(&repMap,
                    value->name,
                    strlen(value->name));
            switch(value->type)
            {
                case OCREP_PROP_NULL:
                    err = err | cbor_encode_null(&repMap);
                    break;
                case OCREP_PROP_INT:
                    err = err | cbor_encode_int(&repMap,
                            value->i);
                    break;
                case OCREP_PROP_DOUBLE:
                    err = err | cbor_encode_double(&repMap,
                            &(value->d));
                    break;
                case OCREP_PROP_BOOL:
                    err = err | cbor_encode_boolean(&repMap,
                            value->b);
                    break;
                case OCREP_PROP_STRING:
                    err = err | cbor_encode_text_string(&repMap,
                            value->str, strlen(value->str));
                    break;
                case OCREP_PROP_OBJECT:
                    err = err | ConvertSingleRepPayload(&repMap, value->obj);
                    break;
                case OCREP_PROP_ARRAY:
                    err = err | ConvertArray(&repMap, &value->arr);
                    break;
                default:
                    OC_LOG_V(ERROR, TAG, "Invalid Prop type: %d",
                            value->type);
                    printf("ERICH:Invalid prop type on convert: %d\n",
                            value->type);
                    exit(-1);
                    break;
            }
            value = value->next;
        }

        err = err | cbor_encoder_close_container(&map, &repMap);
    }

    // Close Map
    err = err | cbor_encoder_close_container(parent, &map);

    return err;
}

OCStackResult ConvertRepPayload(OCRepPayload* payload, uint8_t** outPayload, size_t* size)
{
    *outPayload = (uint8_t*)OICCalloc(1, MAX_REQUEST_LENGTH);
    *size = MAX_REQUEST_LENGTH;

    if(!*outPayload)
    {
        return OC_STACK_NO_MEMORY;
    }

    CborEncoder encoder = {};
    CborError err = CborNoError;

    cbor_encoder_init(&encoder, *outPayload, *size, 0);
    CborEncoder rootArray;
    err = err | cbor_encoder_create_array(&encoder, &rootArray, CborIndefiniteLength);
    err = err | cbor_encode_uint(&rootArray, PAYLOAD_TYPE_REPRESENTATION);

    while(payload != NULL && err == CborNoError)
    {
        err = err | ConvertSingleRepPayload(&rootArray, payload);
        payload = payload->next;
    }

    // Close main array
    err = err | cbor_encoder_close_container(&encoder, &rootArray);

    if(err != CborNoError)
    {
        printf("Convert Rep Payload failed with : %d\n", err);
        OC_LOG_V(ERROR, TAG, "Convert Rep Payload failed with : %d\n", err);
        // TODO: remove printf and exit
        exit(-1);
        return OC_STACK_ERROR;
    }

    *size = encoder.ptr - *outPayload;
    uint8_t* tempPayload = (uint8_t*)OICRealloc(*outPayload, *size);

    if(!tempPayload)
    {
        OC_LOG_V(ERROR, TAG, PCF("Payload realloc failed!\n"));
        OICFree(*outPayload);
        return OC_STACK_ERROR;
    }

    *outPayload = tempPayload;

    return OC_STACK_OK;
}

OCStackResult ConvertPresencePayload(OCPresencePayload* payload, uint8_t** outPayload, size_t* size)
{
    *outPayload = (uint8_t*)OICCalloc(1, MAX_REQUEST_LENGTH);
    // TODO: figure out how to calculate this so we only allocate the required amount,
    // might need to just do a realloc afterwards
    *size = MAX_REQUEST_LENGTH;

    if(!*outPayload)
    {
        return OC_STACK_NO_MEMORY;
    }

    CborEncoder encoder = {};
    CborError err = CborNoError;

    cbor_encoder_init(&encoder, *outPayload, *size, 0);
    CborEncoder rootArray;

    err = err | cbor_encoder_create_array(&encoder, &rootArray, 2);
    err = err | cbor_encode_uint(&rootArray, PAYLOAD_TYPE_PRESENCE);


    CborEncoder map;
    err = err | cbor_encoder_create_map(&rootArray, &map, CborIndefiniteLength);

    // Sequence Number
    err = err | cbor_encode_text_string(&map,
            OC_RSRVD_NONCE,
            sizeof(OC_RSRVD_NONCE) - 1);
    err = err | cbor_encode_uint(&map, payload->sequenceNumber);

    // Max Age
    err = err | cbor_encode_text_string(&map,
            OC_RSRVD_TTL,
            sizeof(OC_RSRVD_TTL) - 1);
    err = err | cbor_encode_uint(&map, payload->maxAge);

    // Trigger
    const char* triggerStr = convertTriggerEnumToString(payload->trigger);
    err = err | AddTextStringToMap(&map, OC_RSRVD_TRIGGER, sizeof(OC_RSRVD_TRIGGER) - 1,
            triggerStr);

    // Resource type name
    if(payload->trigger != OC_PRESENCE_TRIGGER_DELETE)
    {
        err = err | ConditionalAddTextStringToMap(&map, OC_RSRVD_RESOURCE_TYPE,
                sizeof(OC_RSRVD_RESOURCE_TYPE) - 1, payload->resourceType);
    }

    // Close Map
    err = err | cbor_encoder_close_container(&rootArray, &map);
    err = err | cbor_encoder_close_container(&encoder, &rootArray);

    if(err != CborNoError)
    {
        printf("Convert Presence Payload failed with : %d\n", err);
        OC_LOG_V(ERROR, TAG, "Convert Presence Payload failed with : %d\n", err);
        // TODO: remove printf and exit
        exit(-1);
        return OC_STACK_ERROR;
    }

    *size = encoder.ptr - *outPayload;
    uint8_t* tempPayload = (uint8_t*)OICRealloc(*outPayload, *size);

    if(!tempPayload)
    {
        OC_LOG_V(ERROR, TAG, PCF("Payload realloc failed!\n"));
        OICFree(*outPayload);
        return OC_STACK_ERROR;
    }

    *outPayload = tempPayload;

    return OC_STACK_OK;
}

CborError AddTextStringToMap(CborEncoder* map, const char* key, size_t keylen,
        const char* value)
{
    return cbor_encode_text_string(map, key, keylen) |
           cbor_encode_text_string(map, value, strlen(value));
}

CborError ConditionalAddTextStringToMap(CborEncoder* map, const char* key, size_t keylen,
        const char* value)
{
    return value ? AddTextStringToMap(map, key, keylen, value) : CborNoError;
}
