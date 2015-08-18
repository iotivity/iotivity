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
#include "logger.h"
#include "oic_malloc.h"
#include "ocstackinternal.h"
#include "ocpayload.h"
#include "cbor.h"

#define TAG PCF("OCPayloadParse")

static OCStackResult OCParseDiscoveryPayload(OCPayload** outPayload, CborValue* arrayVal);
static OCStackResult OCParseDevicePayload(OCPayload** outPayload, CborValue* arrayVal);
static OCStackResult OCParsePlatformPayload(OCPayload** outPayload, CborValue* arrayVal);
static bool OCParseSingleRepPayload(OCRepPayload** outPayload, CborValue* repParent);
static OCStackResult OCParseRepPayload(OCPayload** outPayload, CborValue* arrayVal);
static OCStackResult OCParsePresencePayload(OCPayload** outPayload, CborValue* arrayVal);
static OCStackResult OCParseSecurityPayload(OCPayload** outPayload, CborValue* arrayVal);

OCStackResult OCParsePayload(OCPayload** outPayload, const uint8_t* payload, size_t payloadSize)
{
    CborParser parser;
    CborValue rootValue;
    bool err = false;

    OC_LOG_V(INFO, TAG, "CBOR Parsing size: %d", payloadSize, payload);
    if((err = cbor_parser_init(payload, payloadSize, 0, &parser, &rootValue)) != false)
    {
        OC_LOG_V(ERROR, TAG, "CBOR Parser init failed: %d", err);
        return OC_STACK_ERROR;
    }

    if(!cbor_value_is_array(&rootValue))
    {
        OC_LOG_V(ERROR, TAG, "CBOR payload root object is not an array :%x", rootValue.type);
        return OC_STACK_MALFORMED_RESPONSE;
    }

    CborValue arrayValue;
    // enter the array
    err = err || cbor_value_enter_container(&rootValue, &arrayValue);

    int payloadType = 0;
    err = err || cbor_value_get_int(&arrayValue, &payloadType);
    err = err || cbor_value_advance_fixed(&arrayValue);

    if(err)
    {
        OC_LOG_V(ERROR, TAG, "CBOR payload parse failed :%d", err);
        return OC_STACK_MALFORMED_RESPONSE;
    }

    OCStackResult result = OC_STACK_ERROR;
    switch(payloadType)
    {
        case PAYLOAD_TYPE_DISCOVERY:
            result = OCParseDiscoveryPayload(outPayload, &arrayValue);
            break;
        case PAYLOAD_TYPE_DEVICE:
            result = OCParseDevicePayload(outPayload, &arrayValue);
            break;
        case PAYLOAD_TYPE_PLATFORM:
            result = OCParsePlatformPayload(outPayload, &arrayValue);
            break;
        case PAYLOAD_TYPE_REPRESENTATION:
            result = OCParseRepPayload(outPayload, &arrayValue);
            break;
        case PAYLOAD_TYPE_PRESENCE:
            result = OCParsePresencePayload(outPayload, &arrayValue);
            break;
        case PAYLOAD_TYPE_SECURITY:
            result = OCParseSecurityPayload(outPayload, &arrayValue);
            break;
        default:
            OC_LOG_V(ERROR, TAG, "ParsePayload Type default: %d", payloadType);
            result = OC_STACK_ERROR;
            break;
    }

    if(result == OC_STACK_OK)
    {
        err = err || cbor_value_leave_container(&rootValue, &arrayValue);
        if(err != CborNoError)
        {
            return OC_STACK_MALFORMED_RESPONSE;
        }
    }
    else
    {
        OC_LOG_V(INFO, TAG, "Finished parse payload, result is %d", result);
    }

    return result;
}

void OCFreeOCStringLL(OCStringLL* ll);

static OCStackResult OCParseSecurityPayload(OCPayload** outPayload, CborValue* arrayVal)
{
    bool err = false;
    char * securityData = NULL;

    if(cbor_value_is_map(arrayVal))
    {
        CborValue curVal;
        err = err || cbor_value_map_find_value(arrayVal, OC_RSRVD_REPRESENTATION, &curVal);

        if(cbor_value_is_valid(&curVal))
        {
            size_t len;
            err = err || cbor_value_dup_text_string(&curVal, &securityData, &len, NULL);
        }
    }
    else
    {
        OC_LOG_V(ERROR, TAG, PCF("Cbor main value not a map"));
        return OC_STACK_MALFORMED_RESPONSE;
    }

    err = err || cbor_value_advance(arrayVal);

    if(err)
    {
        OC_LOG_V(ERROR, TAG, "Cbor in error condition");
        OICFree(securityData);
        return OC_STACK_MALFORMED_RESPONSE;
    }

    *outPayload = (OCPayload*)OCSecurityPayloadCreate(securityData);
    OICFree(securityData);

    return OC_STACK_OK;

}

static OCStackResult OCParseDiscoveryPayload(OCPayload** outPayload, CborValue* arrayVal)
{
    bool err = false;

    OCDiscoveryPayload* out = OCDiscoveryPayloadCreate();

    if(!out)
    {
        return OC_STACK_NO_MEMORY;
    }

    size_t resourceCount = 0;
    while(!err &&
            cbor_value_is_map(arrayVal))
    {
        OCResourcePayload* resource = (OCResourcePayload*)OICCalloc(1, sizeof(OCResourcePayload));
        if(!resource)
        {
            OC_LOG_V(ERROR, TAG, "Memory allocation failed");
            OCDiscoveryPayloadDestroy(out);
            return OC_STACK_NO_MEMORY;
        }
        CborValue curVal;

        // Uri
        err = err || cbor_value_map_find_value(arrayVal, OC_RSRVD_HREF, &curVal);
        size_t len;
        err = err || cbor_value_dup_text_string(&curVal, &(resource->uri), &len, NULL);

        // SID
        err = err || cbor_value_map_find_value(arrayVal, OC_RSRVD_SERVER_INSTANCE_ID, &curVal);
        err = err || cbor_value_dup_byte_string(&curVal, &(resource->sid), &len, NULL);

        // Prop Tag
        {
             err = err || cbor_value_map_find_value(arrayVal, OC_RSRVD_PROPERTY, &curVal);
            // ResourceTypes
            CborValue rtArray;
             err = err || cbor_value_map_find_value(&curVal, OC_RSRVD_RESOURCE_TYPE, &rtArray);

            CborValue rtVal;
             err = err || cbor_value_enter_container(&rtArray, &rtVal);

            OCStringLL* llPtr = NULL;
            while(!err && cbor_value_is_text_string(&rtVal))
            {
                if(resource->types == NULL)
                {
                    resource->types = (OCStringLL*)OICCalloc(1, sizeof(OCStringLL));
                    llPtr = resource->types;
                    if(!llPtr)
                    {
                        OC_LOG(ERROR, TAG, PCF("Memory allocation failed"));
                        OICFree(resource->uri);
                        OICFree(resource->sid);
                        OICFree(resource);
                        OCDiscoveryPayloadDestroy(out);
                        return OC_STACK_NO_MEMORY;
                    }
                }
                else if(llPtr)
                {
                    llPtr->next = (OCStringLL*)OICCalloc(1, sizeof(OCStringLL));
                    llPtr = llPtr->next;
                    if(!llPtr)
                    {
                        OC_LOG(ERROR, TAG, PCF("Memory allocation failed"));
                        OICFree(resource->uri);
                        OICFree(resource->sid);
                        OCFreeOCStringLL(resource->types);
                        OICFree(resource);
                        OCDiscoveryPayloadDestroy(out);
                        return OC_STACK_NO_MEMORY;
                    }
                }
                else
                {
                        OC_LOG(ERROR, TAG, PCF("Unknown state in resource type copying"));
                        OICFree(resource->uri);
                        OICFree(resource->sid);
                        OCFreeOCStringLL(resource->types);
                        OICFree(resource);
                        OCDiscoveryPayloadDestroy(out);
                        return OC_STACK_NO_MEMORY;
                }

                 err = err || cbor_value_dup_text_string(&rtVal, &(llPtr->value), &len, NULL);
                 err = err || cbor_value_advance(&rtVal);
            }

             err = err || cbor_value_leave_container(&rtArray, &rtVal);
            //
            // Interface Types
            CborValue ifArray;
             err = err || cbor_value_map_find_value(&curVal, OC_RSRVD_INTERFACE, &ifArray);
            CborValue ifVal;
             err = err || cbor_value_enter_container(&ifArray, &ifVal);

            llPtr = NULL;
            while(!err && cbor_value_is_text_string(&ifVal))
            {
                if(resource->interfaces == NULL)
                {
                    resource->interfaces = (OCStringLL*)OICCalloc(1, sizeof(OCStringLL));
                    llPtr = resource->interfaces;
                    if(!llPtr)
                    {
                        OC_LOG_V(ERROR, TAG, "Memory allocation failed");
                        OICFree(resource->uri);
                        OICFree(resource->sid);
                        OCFreeOCStringLL(resource->types);
                        OICFree(resource);
                        OCDiscoveryPayloadDestroy(out);
                        return OC_STACK_NO_MEMORY;
                    }
                }
                else if (llPtr)
                {
                    llPtr->next = (OCStringLL*)OICCalloc(1, sizeof(OCStringLL));
                    llPtr = llPtr->next;
                    if(!llPtr)
                    {
                        OC_LOG_V(ERROR, TAG, "Memory allocation failed");
                        OICFree(resource->uri);
                        OICFree(resource->sid);
                        OCFreeOCStringLL(resource->types);
                        OCFreeOCStringLL(resource->interfaces);
                        OICFree(resource);
                        OCDiscoveryPayloadDestroy(out);
                        return OC_STACK_NO_MEMORY;
                    }
                }
                else
                {
                        OC_LOG(ERROR, TAG, PCF("Unknown state in resource interfaces copying"));
                        OICFree(resource->uri);
                        OICFree(resource->sid);
                        OCFreeOCStringLL(resource->types);
                        OICFree(resource);
                        OCDiscoveryPayloadDestroy(out);
                        return OC_STACK_NO_MEMORY;
                }

                 err = err || cbor_value_dup_text_string(&ifVal, &(llPtr->value), &len, NULL);
                 err = err || cbor_value_advance(&ifVal);
            }
             err = err || cbor_value_leave_container(&ifArray, &ifVal);

            // Policy
            {
                CborValue policyMap;
                err = err || cbor_value_map_find_value(&curVal, OC_RSRVD_POLICY, &policyMap);

                // Bitmap
                CborValue val;
                err = err || cbor_value_map_find_value(&policyMap, OC_RSRVD_BITMAP, &val);
                uint64_t temp = 0;
                err = err || cbor_value_get_uint64(&val, &temp);
                resource->bitmap = (uint8_t)temp;
                // Secure Flag
                err = err || cbor_value_map_find_value(&policyMap, OC_RSRVD_SECURE, &val);
                if(cbor_value_is_valid(&val))
                {
                    err = err || cbor_value_get_boolean(&val, &(resource->secure));
                    // Port
                    CborValue port;
                    err = err || cbor_value_map_find_value(&policyMap, OC_RSRVD_HOSTING_PORT,
                                    &port);
                    if(cbor_value_is_valid(&port))
                    {
                        err = err || cbor_value_get_uint64(&port, &temp);
                        resource->port = (uint16_t)temp;
                    }
                }
            }
        }

        err = err || cbor_value_advance(arrayVal);
        if(err)
        {
            OICFree(resource->uri);
            OICFree(resource->sid);
            OCFreeOCStringLL(resource->types);
            OCFreeOCStringLL(resource->interfaces);
            OICFree(resource);
            OCDiscoveryPayloadDestroy(out);
            OC_LOG_V(ERROR, TAG, "CBOR in error condition", err);
            return OC_STACK_MALFORMED_RESPONSE;
        }
        ++resourceCount;
        OCDiscoveryPayloadAddNewResource(out, resource);
    }

    if(err)
    {
        OCDiscoveryPayloadDestroy(out);
        return OC_STACK_MALFORMED_RESPONSE;
    }
    else
    {
        *outPayload = (OCPayload*)out;
        return OC_STACK_OK;
    }
}

static OCStackResult OCParseDevicePayload(OCPayload** outPayload, CborValue* arrayVal)
{
    bool err = false;

    if(cbor_value_is_map(arrayVal))
    {
        char* uri = NULL;
        uint8_t* sid = NULL;
        char* dname = NULL;
        char* specVer = NULL;
        char* dmVer = NULL;
        CborValue curVal;
         err = err || cbor_value_map_find_value(arrayVal, OC_RSRVD_HREF, &curVal);
        size_t len;
         err = err || cbor_value_dup_text_string(&curVal, &uri, &len, NULL);

        // Representation
        {
             err = err || cbor_value_map_find_value(arrayVal, OC_RSRVD_REPRESENTATION, &curVal);

            CborValue repVal;
            // Device ID
             err = err || cbor_value_map_find_value(&curVal, OC_RSRVD_DEVICE_ID, &repVal);
             err = err || cbor_value_dup_byte_string(&repVal, &sid, &len, NULL);
            // Device Name
             err = err || cbor_value_map_find_value(&curVal, OC_RSRVD_DEVICE_NAME, &repVal);
             err = err || cbor_value_dup_text_string(&repVal, &dname, &len, NULL);
            // Device Spec Version
             err = err || cbor_value_map_find_value(&curVal, OC_RSRVD_SPEC_VERSION, &repVal);
             err = err || cbor_value_dup_text_string(&repVal, &specVer, &len, NULL);
            // Data Model Version
             err = err || cbor_value_map_find_value(&curVal, OC_RSRVD_DATA_MODEL_VERSION, &repVal);
             err = err || cbor_value_dup_text_string(&repVal, &dmVer, &len, NULL);

        }

         err = err || cbor_value_advance(arrayVal);

        if(err)
        {
            OICFree(uri);
            OICFree(sid);
            OICFree(dname);
            OICFree(specVer);
            OICFree(dmVer);
            OC_LOG_V(ERROR, TAG, "CBOR in error condition %d", err);
            return OC_STACK_MALFORMED_RESPONSE;
        }

        *outPayload = (OCPayload*)OCDevicePayloadCreate(uri, sid, dname, specVer, dmVer);

        OICFree(uri);
        OICFree(sid);
        OICFree(dname);
        OICFree(specVer);
        OICFree(dmVer);
        if(!*outPayload)
        {
            return OC_STACK_NO_MEMORY;
        }

        return OC_STACK_OK;
    }
    else
    {
        OC_LOG(ERROR, TAG, PCF("Root device node was not a map"));
        return OC_STACK_MALFORMED_RESPONSE;
    }

}

static OCStackResult OCParsePlatformPayload(OCPayload** outPayload, CborValue* arrayVal)
{
    bool err = false;

    if(cbor_value_is_map(arrayVal))
    {
        char* uri = NULL;
        OCPlatformInfo info = {0};
        CborValue curVal;
         err = err || cbor_value_map_find_value(arrayVal, OC_RSRVD_HREF, &curVal);
        size_t len;
         err = err || cbor_value_dup_text_string(&curVal, &uri, &len, NULL);

        // Representation
        {
             err = err || cbor_value_map_find_value(arrayVal, OC_RSRVD_REPRESENTATION, &curVal);

            CborValue repVal;
            // Platform ID
             err = err || cbor_value_map_find_value(&curVal, OC_RSRVD_PLATFORM_ID, &repVal);
             err = err || cbor_value_dup_text_string(&repVal, &(info.platformID), &len, NULL);

            // MFG Name
             err = err || cbor_value_map_find_value(&curVal, OC_RSRVD_MFG_NAME, &repVal);
             err = err || cbor_value_dup_text_string(&repVal, &(info.manufacturerName), &len, NULL);

            // MFG URL
             err = err || cbor_value_map_find_value(&curVal, OC_RSRVD_MFG_URL, &repVal);
            if(cbor_value_is_valid(&repVal))
            {
                 err = err || cbor_value_dup_text_string(&repVal, &(info.manufacturerUrl), &len, NULL);
            }

            // Model Num
             err = err || cbor_value_map_find_value(&curVal, OC_RSRVD_MODEL_NUM, &repVal);
            if(cbor_value_is_valid(&repVal))
            {
                 err = err || cbor_value_dup_text_string(&repVal, &(info.modelNumber), &len, NULL);
            }

            // Date of Mfg
             err = err || cbor_value_map_find_value(&curVal, OC_RSRVD_MFG_DATE, &repVal);
            if(cbor_value_is_valid(&repVal))
            {
                 err = err || cbor_value_dup_text_string(&repVal, &(info.dateOfManufacture), &len,
                        NULL);
            }

            // Platform Version
             err = err || cbor_value_map_find_value(&curVal, OC_RSRVD_PLATFORM_VERSION, &repVal);
            if(cbor_value_is_valid(&repVal))
            {
                 err = err || cbor_value_dup_text_string(&repVal, &(info.platformVersion), &len,
                        NULL);
            }

            // OS Version
             err = err || cbor_value_map_find_value(&curVal, OC_RSRVD_OS_VERSION, &repVal);
            if(cbor_value_is_valid(&repVal))
            {
                 err = err || cbor_value_dup_text_string(&repVal, &(info.operatingSystemVersion),
                        &len, NULL);
            }

            // Hardware Version
             err = err || cbor_value_map_find_value(&curVal, OC_RSRVD_HARDWARE_VERSION, &repVal);
            if(cbor_value_is_valid(&repVal))
            {
                 err = err || cbor_value_dup_text_string(&repVal, &(info.hardwareVersion), &len,
                        NULL);
            }

            // Firmware Version
             err = err || cbor_value_map_find_value(&curVal, OC_RSRVD_FIRMWARE_VERSION, &repVal);
            if(cbor_value_is_valid(&repVal))
            {
                 err = err || cbor_value_dup_text_string(&repVal, &(info.firmwareVersion), &len,
                        NULL);
            }

            // Support URL
             err = err || cbor_value_map_find_value(&curVal, OC_RSRVD_SUPPORT_URL, &repVal);
            if(cbor_value_is_valid(&repVal))
            {
                 err = err || cbor_value_dup_text_string(&repVal, &(info.supportUrl), &len, NULL);
            }

            // System Time
             err = err || cbor_value_map_find_value(&curVal, OC_RSRVD_SYSTEM_TIME, &repVal);
            if(cbor_value_is_valid(&repVal))
            {
                 err = err || cbor_value_dup_text_string(&repVal, &(info.systemTime), &len, NULL);
            }
        }

         err = err || cbor_value_advance(arrayVal);

        if(err)
        {
            OICFree(info.dateOfManufacture);
            OICFree(info.firmwareVersion);
            OICFree(info.hardwareVersion);
            OICFree(info.manufacturerName);
            OICFree(info.manufacturerUrl);
            OICFree(info.modelNumber);
            OICFree(info.operatingSystemVersion);
            OICFree(info.platformID);
            OICFree(info.platformVersion);
            OICFree(info.supportUrl);
            OICFree(info.systemTime);
            OC_LOG(ERROR, TAG, PCF("CBOR error In ParsePlatformPayload"));
            return OC_STACK_MALFORMED_RESPONSE;
        }

        *outPayload = (OCPayload*)OCPlatformPayloadCreateAsOwner(uri, &info);

        if(!*outPayload)
        {
            return OC_STACK_NO_MEMORY;
        }

        return OC_STACK_OK;
    }
    else
    {
        OC_LOG(ERROR, TAG, PCF("Root device node was not a map"));
        return OC_STACK_MALFORMED_RESPONSE;
    }
}

static bool OCParseArray(OCRepPayload* out, const char* name, CborValue* container)
{
    CborValue insideArray;
    bool err = false;
    uint64_t tempInt = 0;
    OCRepPayloadPropType type;
    size_t dimensions[MAX_REP_ARRAY_DEPTH];
    err = err || cbor_value_enter_container(container, &insideArray);

    err = err || cbor_value_get_uint64(&insideArray, &tempInt);
    err = err || cbor_value_advance_fixed(&insideArray);
    type = (OCRepPayloadPropType)tempInt;

    for(int i = 0; i < MAX_REP_ARRAY_DEPTH; ++ i)
    {
         err = err || cbor_value_get_uint64(&insideArray, &tempInt);
         err = err || cbor_value_advance_fixed(&insideArray);
        dimensions[i] = tempInt;
    }

    size_t dimTotal = calcDimTotal(dimensions);

    void* arr = NULL;
    char* tempStr;
    size_t len;
    OCRepPayload* pl;
    switch(type)
    {
        case OCREP_PROP_INT:
            arr = (int64_t*)OICMalloc(dimTotal * sizeof(int64_t));
            if (arr)
            {
                for(size_t i = 0; i < dimTotal && !err; ++i)
                {
                     err = err || cbor_value_get_int64(&insideArray, &(((int64_t*)arr)[i]));
                     err = err || cbor_value_advance_fixed(&insideArray);
                }
                if(err || !OCRepPayloadSetIntArrayAsOwner(out, name, (int64_t*)arr, dimensions))
                {
                    OICFree(arr);
                    err = true;
                }
            }
            else
            {
                err = true;
            }
            break;
        case OCREP_PROP_DOUBLE:
            arr = (double*)OICMalloc(dimTotal * sizeof(double));
            if(arr)
            {
                for(size_t i = 0; i < dimTotal && !err; ++i)
                {
                     err = err || cbor_value_get_double(&insideArray, &(((double*)arr)[i]));
                     err = err || cbor_value_advance_fixed(&insideArray);
                }
                if(err || !OCRepPayloadSetDoubleArrayAsOwner(out, name, (double*)arr, dimensions))
                {
                    OICFree(arr);
                    err = true;
                }
            }
            else
            {
                err = true;
            }
            break;
        case OCREP_PROP_BOOL:
            arr = (bool*)OICMalloc(dimTotal * sizeof(bool));
            if(arr)
            {
                for(size_t i = 0; i < dimTotal && !err; ++i)
                {
                     err = err || cbor_value_get_boolean(&insideArray, &(((bool*)arr)[i]));
                     err = err || cbor_value_advance_fixed(&insideArray);
                }
                if(err || !OCRepPayloadSetBoolArrayAsOwner(out, name, (bool*)arr, dimensions))
                {
                    OICFree(arr);
                    err = true;
                }
            }
            else
            {
                err = true;
            }
            break;
        case OCREP_PROP_STRING:
            arr = (char**)OICMalloc(dimTotal * sizeof(char*));
            if(arr)
            {
                for(size_t i = 0; i < dimTotal && !err; ++i)
                {
                    err = err || cbor_value_dup_text_string(&insideArray, &tempStr,
                            &len, NULL);
                    err = err || cbor_value_advance(&insideArray);
                    ((char**)arr)[i] = tempStr;
                }
                if(err || !OCRepPayloadSetStringArrayAsOwner(out, name, (char**)arr, dimensions))
                {
                    OICFree(arr);
                    err = true;
                }
            }
            else
            {
                err = true;
            }
            break;
        case OCREP_PROP_OBJECT:
            arr = (OCRepPayload**)OICMalloc(dimTotal * sizeof(OCRepPayload*));
            if(arr)
            {
                for(size_t i = 0; i < dimTotal && !err; ++i)
                {
                    pl = NULL;
                    err = err || OCParseSingleRepPayload(&pl, &insideArray);
                    err = err || cbor_value_advance(&insideArray);
                    ((OCRepPayload**)arr)[i] = pl;
                }
                if(err || !OCRepPayloadSetPropObjectArrayAsOwner(out, name,
                        (OCRepPayload**)arr, dimensions))
                {
                    OICFree(arr);
                    err = true;
                }
            }
            else
            {
                err = true;
            }
            break;
        default:
            OC_LOG(ERROR, TAG, "Invalid Array type in Parse Array");
            err = true;
            break;
    }

    return err;
}

static bool OCParseSingleRepPayload(OCRepPayload** outPayload, CborValue* repParent)
{
    *outPayload = OCRepPayloadCreate();
    OCRepPayload* curPayload = *outPayload;
    bool err = false;
    if(!*outPayload)
    {
        return CborErrorOutOfMemory;
    }

    size_t len;
    CborValue curVal;
    err = err || cbor_value_map_find_value(repParent, OC_RSRVD_HREF, &curVal);
    if(cbor_value_is_valid(&curVal))
    {
        err = err || cbor_value_dup_text_string(&curVal, &curPayload->uri, &len,
            NULL);
    }

    err = err || cbor_value_map_find_value(repParent, OC_RSRVD_PROPERTY, &curVal);
    if(cbor_value_is_valid(&curVal))
    {
        CborValue insidePropArray = {0};
        err = err || cbor_value_map_find_value(&curVal, OC_RSRVD_RESOURCE_TYPE,
                &insidePropArray);

        if(cbor_value_is_array(&insidePropArray))
        {
            CborValue rtArray;
            err = err || cbor_value_enter_container(&insidePropArray, &rtArray);

            while(!err && cbor_value_is_valid(&rtArray))
            {
                char* curRt;
                err = err || cbor_value_dup_text_string(&rtArray, &curRt, &len, NULL);
                err = err || cbor_value_advance(&rtArray);
                OCRepPayloadAddResourceTypeAsOwner(curPayload, curRt);
            }

            err = err || cbor_value_leave_container(&insidePropArray, &rtArray);
        }

        err = err || cbor_value_map_find_value(&curVal, OC_RSRVD_INTERFACE, &insidePropArray);

        if(cbor_value_is_array(&insidePropArray))
        {
            CborValue ifArray;
            err = err || cbor_value_enter_container(&insidePropArray, &ifArray);

            while(!err && cbor_value_is_valid(&ifArray))
            {
                char* curIf;
                err = err || cbor_value_dup_text_string(&ifArray, &curIf, &len, NULL);
                err = err || cbor_value_advance(&ifArray);
                OCRepPayloadAddInterfaceAsOwner(curPayload, curIf);
            }

            err = err || cbor_value_leave_container(&insidePropArray, &ifArray);
        }
    }
    err = err || cbor_value_map_find_value(repParent, OC_RSRVD_REPRESENTATION, &curVal);
    if(cbor_value_is_map(&curVal))
    {
        CborValue repMap;
        err = err || cbor_value_enter_container(&curVal, &repMap);

        while(!err && cbor_value_is_valid(&repMap))
        {
            char* name;
             err = err || cbor_value_dup_text_string(&repMap, &name, &len, NULL);

             err = err || cbor_value_advance(&repMap);

            int64_t intval = 0;
            bool boolval = false;
            char* strval = NULL;
            double doubleval = 0;
            OCRepPayload* pl;

            switch(cbor_value_get_type(&repMap))
            {
                case CborNullType:
                    err = !OCRepPayloadSetNull(curPayload, name);
                    break;
                case CborIntegerType:
                    err = err || cbor_value_get_int64(&repMap, &intval);
                    if (!err)
                    {
                        err = !OCRepPayloadSetPropInt(curPayload, name, intval);
                    }
                    break;
                case CborDoubleType:
                    err = err || cbor_value_get_double(&repMap, &doubleval);
                    if (!err)
                    {
                        err = !OCRepPayloadSetPropDouble(curPayload, name, doubleval);
                    }
                    break;
                case CborBooleanType:
                    err = err || cbor_value_get_boolean(&repMap, &boolval);
                    if (!err)
                    {
                        err = !OCRepPayloadSetPropBool(curPayload, name, boolval);
                    }
                    break;
                case CborTextStringType:
                    err = err || cbor_value_dup_text_string(&repMap, &strval, &len, NULL);
                    if (!err)
                    {
                        err = !OCRepPayloadSetPropStringAsOwner(curPayload, name, strval);
                    }
                    break;
                case CborMapType:
                    err = err || OCParseSingleRepPayload(&pl, &repMap);
                    if (!err)
                    {
                        err = !OCRepPayloadSetPropObjectAsOwner(curPayload, name, pl);
                    }
                    break;
                case CborArrayType:
                    err = err || OCParseArray(curPayload, name, &repMap);
                    break;
                default:
                    OC_LOG_V(ERROR, TAG, "Parsing rep property, unknown type %d", repMap.type);
                    err = true;
            }

             err = err || cbor_value_advance(&repMap);
            OICFree(name);
        }
        err = err || cbor_value_leave_container(&curVal, &repMap);
    }

    if(err)
    {
        OCRepPayloadDestroy(*outPayload);
        *outPayload = NULL;
    }

    return err;
}
static OCStackResult OCParseRepPayload(OCPayload** outPayload, CborValue* arrayVal)
{
    bool err = false;

    OCRepPayload* rootPayload = NULL;
    OCRepPayload* curPayload = NULL;
    OCRepPayload* temp = NULL;
    while(!err && cbor_value_is_map(arrayVal))
    {
         err = err || OCParseSingleRepPayload(&temp, arrayVal);

        if(rootPayload == NULL)
        {
            rootPayload = temp;
            curPayload = temp;
        }
        else
        {
            curPayload->next = temp;
            curPayload = curPayload->next;
        }


         err = err || cbor_value_advance(arrayVal);
        if(err)
        {
            OCRepPayloadDestroy(rootPayload);
            OC_LOG_V(ERROR, TAG, PCF("CBOR error in ParseRepPayload"));
            return OC_STACK_MALFORMED_RESPONSE;
        }
    }

    *outPayload = (OCPayload*)rootPayload;

    return OC_STACK_OK;
}

static OCStackResult OCParsePresencePayload(OCPayload** outPayload, CborValue* arrayVal)
{
    bool err = false;
    if(cbor_value_is_map(arrayVal))
    {
        uint64_t seqNum = 0;
        uint64_t maxAge = 0;
        OCPresenceTrigger trigger = OC_PRESENCE_TRIGGER_CREATE;
        char* tempStr = NULL;
        size_t len = 0;

        CborValue curVal;
        // Sequence Number
        err = err || cbor_value_map_find_value(arrayVal, OC_RSRVD_NONCE, &curVal);
        err = err || cbor_value_get_uint64(&curVal, &seqNum);

        // Max Age
        err = err || cbor_value_map_find_value(arrayVal, OC_RSRVD_TTL, &curVal);
        err = err || cbor_value_get_uint64(&curVal, &maxAge);

        // Trigger
        err = err || cbor_value_map_find_value(arrayVal, OC_RSRVD_TRIGGER, &curVal);
        err = err || cbor_value_dup_text_string(&curVal, &tempStr, &len, NULL);
        trigger = convertTriggerStringToEnum(tempStr);
        OICFree(tempStr);
        tempStr = NULL;

        // Resource type name
         err = err || cbor_value_map_find_value(arrayVal, OC_RSRVD_RESOURCE_TYPE, &curVal);
        if(cbor_value_is_valid(&curVal))
        {
             err = err || cbor_value_dup_text_string(&curVal, &tempStr, &len, NULL);
        }

        err = err || cbor_value_advance(arrayVal);

        if(!err)
        {
            *outPayload = (OCPayload*)OCPresencePayloadCreate(seqNum, maxAge, trigger, tempStr);
        }
        OICFree(tempStr);

        if(err)
        {
            OCPayloadDestroy(*outPayload);
            OC_LOG_V(ERROR, TAG, PCF("CBOR error Parse Presence Payload"));
            return OC_STACK_MALFORMED_RESPONSE;
        }

        if(!*outPayload)
        {
            return OC_STACK_NO_MEMORY;
        }

        return OC_STACK_OK;
    }
    else
    {
        OC_LOG(ERROR, TAG, PCF("Root presence node was not a map"));
        return OC_STACK_MALFORMED_RESPONSE;
    }
}
