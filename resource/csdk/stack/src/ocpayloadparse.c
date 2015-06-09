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

#define TAG PCF("OCPayloadParse")

OCStackResult ParseDiscoveryPayload(OCPayload** outPayload, CborValue* arrayVal);
OCStackResult ParseDevicePayload(OCPayload** outPayload, CborValue* arrayVal);
OCStackResult ParsePlatformPayload(OCPayload** outPayload, CborValue* arrayVal);
OCStackResult ParseRepPayload(OCPayload** outPayload, CborValue* arrayVal);
OCStackResult ParsePresencePayload(OCPayload** outPayload, CborValue* arrayVal);

OCStackResult ParsePayload(OCPayload** outPayload, const uint8_t* payload, size_t payloadSize)
{
    CborParser parser;
    CborValue rootValue;
    CborError err = CborNoError;

    OC_LOG_V(INFO, TAG, "CBOR Parsing size: %d", payloadSize, payload);
    if((err = cbor_parser_init(payload, payloadSize, 0, &parser, &rootValue)) != CborNoError)
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
    err = err | cbor_value_enter_container(&rootValue, &arrayValue);

    int payloadType;
    err = err | cbor_value_get_int(&arrayValue, &payloadType);
    err = err | cbor_value_advance_fixed(&arrayValue);

    if(err != CborNoError)
    {
        OC_LOG_V(ERROR, TAG, "CBOR payload parse failed :%d", err);
        return OC_STACK_MALFORMED_RESPONSE;
    }

    OCStackResult result = OC_STACK_ERROR;
    switch(payloadType)
    {
        case PAYLOAD_TYPE_DISCOVERY:
            result = ParseDiscoveryPayload(outPayload, &arrayValue);
            break;
        case PAYLOAD_TYPE_DEVICE:
            result = ParseDevicePayload(outPayload, &arrayValue);
            break;
        case PAYLOAD_TYPE_PLATFORM:
            result = ParsePlatformPayload(outPayload, &arrayValue);
            break;
        case PAYLOAD_TYPE_REPRESENTATION:
            result = ParseRepPayload(outPayload, &arrayValue);
            break;
        case PAYLOAD_TYPE_PRESENCE:
            result = ParsePresencePayload(outPayload, &arrayValue);
            break;
        default:
            printf("ERICH: ParsePayload Type default: %d\n", payloadType);
            exit(-1);
    }

    if(result == OC_STACK_OK)
    {
        err = err | cbor_value_leave_container(&rootValue, &arrayValue);
        if(err != CborNoError)
        {
            return OC_STACK_MALFORMED_RESPONSE;
        }
    }
    else
    {
        printf("Erich: Finished parse payload, result is %d\n", result);
        exit(-1);
    }

    return result;
}

OCStackResult ParseDiscoveryPayload(OCPayload** outPayload, CborValue* arrayVal)
{
    CborError err = CborNoError;

    OCDiscoveryPayload* out = OCDiscoveryPayloadCreate();

    if(!out)
    {
        return OC_STACK_NO_MEMORY;
    }

    size_t resourceCount = 0;
    while(err == CborNoError &&
            cbor_value_is_map(arrayVal))
    {
        OCResourcePayload* resource = (OCResourcePayload*)OICCalloc(1, sizeof(OCResourcePayload));
        if(!resource)
        {
            printf("Erich no mem #0\n");
            exit(-1);
        }
        CborValue curVal;

        // Uri
        err = err | cbor_value_map_find_value(arrayVal, OC_RSRVD_HREF, &curVal);
        size_t len;
        err = err | cbor_value_dup_text_string(&curVal, &(resource->uri), &len, NULL);

        // SID
        err = err | cbor_value_map_find_value(arrayVal, OC_RSRVD_SERVER_INSTANCE_ID, &curVal);
        err = err | cbor_value_dup_byte_string(&curVal, &(resource->sid), &len, NULL);

        // Prop Tag
        {
            err = err | cbor_value_map_find_value(arrayVal, OC_RSRVD_PROPERTY, &curVal);
            // ResourceTypes
            CborValue rtArray;
            err = err | cbor_value_map_find_value(&curVal, OC_RSRVD_RESOURCE_TYPE, &rtArray);

            CborValue rtVal;
            err = err | cbor_value_enter_container(&rtArray, &rtVal);

            OCStringLL* llPtr = NULL;
            while(err == CborNoError && cbor_value_is_text_string(&rtVal))
            {
                if(resource->types == NULL)
                {
                    resource->types = (OCStringLL*)OICCalloc(1, sizeof(OCStringLL));
                    llPtr = resource->types;
                    if(!llPtr)
                    {
                        printf("ERICH: alloc failed #1\n");
                        exit(-1);
                    }
                }
                else
                {
                    llPtr->next = (OCStringLL*)OICCalloc(1, sizeof(OCStringLL));
                    llPtr = llPtr->next;
                    if(!llPtr)
                    {
                        printf("ERICH: alloc failed #2\n");
                        exit(-1);
                    }

                }

                err = err | cbor_value_dup_text_string(&rtVal, &(llPtr->value), &len, NULL);
                err = err | cbor_value_advance(&rtVal);
            }

            err = err | cbor_value_leave_container(&rtArray, &rtVal);
            //
            // Interface Types
            CborValue ifArray;
            err = err | cbor_value_map_find_value(&curVal, OC_RSRVD_INTERFACE, &ifArray);
            CborValue ifVal;
            err = err | cbor_value_enter_container(&ifArray, &ifVal);

            llPtr = NULL;
            while(err == CborNoError && cbor_value_is_text_string(&ifVal))
            {
                if(resource->interfaces == NULL)
                {
                    resource->interfaces = (OCStringLL*)OICCalloc(1, sizeof(OCStringLL));
                    llPtr = resource->interfaces;
                    if(!llPtr)
                    {
                        printf("ERICH: alloc failed #3\n");
                        exit(-1);
                    }
                }
                else
                {
                    llPtr->next = (OCStringLL*)OICCalloc(1, sizeof(OCStringLL));
                    llPtr = llPtr->next;
                    if(!llPtr)
                    {
                        printf("ERICH: alloc failed #4\n");
                        exit(-1);
                    }
                }

                err = err | cbor_value_dup_text_string(&ifVal, &(llPtr->value), &len, NULL);
                err = err | cbor_value_advance(&ifVal);
            }
            err = err | cbor_value_leave_container(&ifArray, &ifVal);

            // Policy
            {
                CborValue policyMap;
                err = err | cbor_value_map_find_value(&curVal, OC_RSRVD_POLICY, &policyMap);

                // Bitmap
                CborValue val;
                err= err | cbor_value_map_find_value(&policyMap, OC_RSRVD_BITMAP, &val);
                uint64_t temp;
                err = err | cbor_value_get_uint64(&val, &temp);
                resource->bitmap = (uint8_t)temp;
                // Secure Flag
                err= err | cbor_value_map_find_value(&policyMap, OC_RSRVD_SECURE, &val);
                if(cbor_value_is_valid(&val))
                {
                    err = err | cbor_value_get_boolean(&val, &(resource->secure));
                    // Port
                    err = err | cbor_value_map_find_value(&policyMap, OC_RSRVD_SECURE, &val);
                    if(cbor_value_is_valid(&val))
                    {
                        err = err | cbor_value_get_uint64(&val, &temp);
                        resource->port = (uint16_t)temp;
                    }
                }
            }
        }

        err = err | cbor_value_advance(arrayVal);
        if(err != CborNoError)
        {
            // TODO: clean up payload objects
            printf("ERICH: Cbor in error condition: %d\n", err);
            return OC_STACK_MALFORMED_RESPONSE;
        }
        ++resourceCount;
        OCDiscoveryPayloadAddNewResource(out, resource);
    }

    *outPayload = (OCPayload*)out;

    return OC_STACK_OK;
}

OCStackResult ParseDevicePayload(OCPayload** outPayload, CborValue* arrayVal)
{
    CborError err = CborNoError;

    if(cbor_value_is_map(arrayVal))
    {
        char* uri = NULL;
        uint8_t* sid = NULL;
        char* dname = NULL;
        char* specVer = NULL;
        char* dmVer = NULL;
        CborValue curVal;
        err = err | cbor_value_map_find_value(arrayVal, OC_RSRVD_HREF, &curVal);
        size_t len;
        err = err | cbor_value_dup_text_string(&curVal, &uri, &len, NULL);

        // Representation
        {
            err = err | cbor_value_map_find_value(arrayVal, OC_RSRVD_REPRESENTATION, &curVal);

            CborValue repVal;
            // Device ID
            err = err | cbor_value_map_find_value(&curVal, OC_RSRVD_DEVICE_ID, &repVal);
            err = err | cbor_value_dup_byte_string(&repVal, &sid, &len, NULL);
            // Device Name
            err = err | cbor_value_map_find_value(&curVal, OC_RSRVD_DEVICE_NAME, &repVal);
            err = err | cbor_value_dup_text_string(&repVal, &dname, &len, NULL);
            // Device Spec Version
            err = err | cbor_value_map_find_value(&curVal, OC_RSRVD_SPEC_VERSION, &repVal);
            err = err | cbor_value_dup_text_string(&repVal, &specVer, &len, NULL);
            // Data Model Version
            err = err | cbor_value_map_find_value(&curVal, OC_RSRVD_DATA_MODEL_VERSION, &repVal);
            err = err | cbor_value_dup_text_string(&repVal, &dmVer, &len, NULL);

        }

        err = err | cbor_value_advance(arrayVal);

        if(err != CborNoError)
        {
            // TODO: clean up payload objects
            printf("ERICH: Cbor in error condition: %d\n", err);
            return OC_STACK_MALFORMED_RESPONSE;
        }

        // TODO: Find a way so that this create doesn't make copies
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

OCStackResult ParsePlatformPayload(OCPayload** outPayload, CborValue* arrayVal)
{
    CborError err = CborNoError;

    if(cbor_value_is_map(arrayVal))
    {
        char* uri = NULL;
        OCPlatformInfo info = {};
        CborValue curVal;
        err = err | cbor_value_map_find_value(arrayVal, OC_RSRVD_HREF, &curVal);
        size_t len;
        err = err | cbor_value_dup_text_string(&curVal, &uri, &len, NULL);

        // Representation
        {
            err = err | cbor_value_map_find_value(arrayVal, OC_RSRVD_REPRESENTATION, &curVal);

            CborValue repVal;
            // Platform ID
            err = err | cbor_value_map_find_value(&curVal, OC_RSRVD_PLATFORM_ID, &repVal);
            err = err | cbor_value_dup_text_string(&repVal, &(info.platformID), &len, NULL);

            // MFG Name
            err = err | cbor_value_map_find_value(&curVal, OC_RSRVD_MFG_NAME, &repVal);
            err = err | cbor_value_dup_text_string(&repVal, &(info.manufacturerName), &len, NULL);

            // MFG URL
            err = err | cbor_value_map_find_value(&curVal, OC_RSRVD_MFG_URL, &repVal);
            if(cbor_value_is_valid(&repVal))
            {
                err = err | cbor_value_dup_text_string(&repVal, &(info.manufacturerUrl), &len, NULL);
            }

            // Model Num
            err = err | cbor_value_map_find_value(&curVal, OC_RSRVD_MODEL_NUM, &repVal);
            if(cbor_value_is_valid(&repVal))
            {
                err = err | cbor_value_dup_text_string(&repVal, &(info.modelNumber), &len, NULL);
            }

            // Date of Mfg
            err = err | cbor_value_map_find_value(&curVal, OC_RSRVD_MFG_DATE, &repVal);
            if(cbor_value_is_valid(&repVal))
            {
                err = err | cbor_value_dup_text_string(&repVal, &(info.dateOfManufacture), &len, NULL);
            }

            // Platform Version
            err = err | cbor_value_map_find_value(&curVal, OC_RSRVD_PLATFORM_VERSION, &repVal);
            if(cbor_value_is_valid(&repVal))
            {
                err = err | cbor_value_dup_text_string(&repVal, &(info.platformVersion), &len, NULL);
            }

            // OS Version
            err = err | cbor_value_map_find_value(&curVal, OC_RSRVD_OS_VERSION, &repVal);
            if(cbor_value_is_valid(&repVal))
            {
                err = err | cbor_value_dup_text_string(&repVal, &(info.operatingSystemVersion), &len, NULL);
            }

            // Hardware Version
            err = err | cbor_value_map_find_value(&curVal, OC_RSRVD_HARDWARE_VERSION, &repVal);
            if(cbor_value_is_valid(&repVal))
            {
                err = err | cbor_value_dup_text_string(&repVal, &(info.hardwareVersion), &len, NULL);
            }

            // Firmware Version
            err = err | cbor_value_map_find_value(&curVal, OC_RSRVD_FIRMWARE_VERSION, &repVal);
            if(cbor_value_is_valid(&repVal))
            {
                err = err | cbor_value_dup_text_string(&repVal, &(info.firmwareVersion), &len, NULL);
            }

            // Support URL
            err = err | cbor_value_map_find_value(&curVal, OC_RSRVD_SUPPORT_URL, &repVal);
            if(cbor_value_is_valid(&repVal))
            {
                err = err | cbor_value_dup_text_string(&repVal, &(info.supportUrl), &len, NULL);
            }

            // System Time
            err = err | cbor_value_map_find_value(&curVal, OC_RSRVD_SYSTEM_TIME, &repVal);
            if(cbor_value_is_valid(&repVal))
            {
                err = err | cbor_value_dup_text_string(&repVal, &(info.systemTime), &len, NULL);
            }
        }

        err = err | cbor_value_advance(arrayVal);

        if(err != CborNoError)
        {
            // TODO: clean up payload objects
            printf("ERICH: Cbor in error condition: %d\n", err);
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

OCStackResult ParseRepPayload(OCPayload** outPayload, CborValue* arrayVal)
{
    CborError err = CborNoError;

    OCRepPayload* rootPayload = NULL;
    OCRepPayload* curPayload = NULL;
    while(err == CborNoError && cbor_value_is_map(arrayVal))
    {
        OCRepPayload* temp = OCRepPayloadCreate();
        if(!temp)
        {
            return OC_STACK_NO_MEMORY;
        }

        if(!rootPayload)
        {
            rootPayload = temp;
            curPayload = temp;
        }
        else
        {
            curPayload->next = temp;
            curPayload = curPayload->next;
        }

        size_t len;
        CborValue curVal;
        err = err | cbor_value_map_find_value(arrayVal, OC_RSRVD_HREF, &curVal);
        if(cbor_value_is_valid(&curVal))
        {
            err = err | cbor_value_dup_text_string(&curVal, &curPayload->uri, &len,
                NULL);
        }

        err = err | cbor_value_map_find_value(arrayVal, OC_RSRVD_PROPERTY, &curVal);
        if(cbor_value_is_valid(&curVal))
        {
            CborValue insidePropArray;
            err = err | cbor_value_map_find_value(&curVal, OC_RSRVD_RESOURCE_TYPE,
                    &insidePropArray);

            if(cbor_value_is_array(&insidePropArray))
            {
                CborValue rtArray;
                err = err | cbor_value_enter_container(&insidePropArray, &rtArray);

                while(err == CborNoError && cbor_value_is_valid(&rtArray))
                {
                    char* curRt;
                    cbor_value_dup_text_string(&rtArray, &curRt, &len, NULL);
                    OCRepPayloadAddResourceTypeAsOwner(curPayload, curRt);
                }

                err = err | cbor_value_leave_container(&insidePropArray, &rtArray);
            }

            err = err | cbor_value_map_find_value(&curVal, OC_RSRVD_INTERFACE, &insidePropArray);

            if(cbor_value_is_array(&insidePropArray))
            {
                CborValue ifArray;
                err = err | cbor_value_enter_container(&insidePropArray, &ifArray);

                while(err == CborNoError && cbor_value_is_valid(&ifArray))
                {
                    char* curIf;
                    err = err | cbor_value_dup_text_string(&ifArray, &curIf, &len, NULL);
                    OCRepPayloadAddInterfaceAsOwner(curPayload, curIf);
                }

                err = err | cbor_value_leave_container(&insidePropArray, &ifArray);
            }
        }

        err = err | cbor_value_map_find_value(arrayVal, OC_RSRVD_REPRESENTATION, &curVal);
        if(cbor_value_is_map(&curVal))
        {
            CborValue repMap;
            err = err | cbor_value_enter_container(&curVal, &repMap);

            while(err == CborNoError && cbor_value_is_valid(&repMap))
            {
                char* name;
                err = err | cbor_value_dup_text_string(&repMap, &name, &len, NULL);

                err = err | cbor_value_advance(&repMap);

                int64_t intval;
                bool boolval;
                char* strval;
                switch(cbor_value_get_type(&repMap))
                {
                    case CborIntegerType:
                        err = err | cbor_value_get_int64(&repMap, &intval);
                        OCRepPayloadSetPropInt(curPayload, name, intval);
                        break;
                    case CborBooleanType:
                        err = err | cbor_value_get_boolean(&repMap, &boolval);
                        OCRepPayloadSetPropBool(curPayload, name, boolval);
                        break;
                    case CborTextStringType:
                        err = err | cbor_value_dup_text_string(&repMap, &strval, &len, NULL);
                        OCRepPayloadSetPropString(curPayload, name, strval);
                        OICFree(strval);
                        break;
                    default:
                        printf("ERICH: Parsing rep property, unknown type %d\n",repMap.type);
                        exit(-1);

                }

                err = err | cbor_value_advance(&repMap);
                OICFree(name);
            }
            err = err | cbor_value_leave_container(&curVal, &repMap);
        }

        err = err | cbor_value_advance(arrayVal);
        if(err != CborNoError)
        {
            // TODO: clean up payload objects
            printf("ERICH: Cbor in error condition: %d\n", err);
            return OC_STACK_MALFORMED_RESPONSE;
        }
    }

    *outPayload = (OCPayload*)rootPayload;

    return OC_STACK_OK;
}

OCStackResult ParsePresencePayload(OCPayload** outPayload, CborValue* arrayVal)
{
    CborError err = CborNoError;
    if(cbor_value_is_map(arrayVal))
    {
        uint64_t seqNum;
        uint64_t maxAge;
        OCPresenceTrigger trigger;
        char* tempStr;
        size_t len;

        CborValue curVal;
        // Sequence Number
        err = err | cbor_value_map_find_value(arrayVal, OC_RSRVD_NONCE, &curVal);
        err = err | cbor_value_get_uint64(&curVal, &seqNum);

        // Max Age
        err = err | cbor_value_map_find_value(arrayVal, OC_RSRVD_TTL, &curVal);
        err = err | cbor_value_get_uint64(&curVal, &maxAge);

        // Trigger
        err = err | cbor_value_map_find_value(arrayVal, OC_RSRVD_TRIGGER, &curVal);
        err = err | cbor_value_dup_text_string(&curVal, &tempStr, &len, NULL);
        trigger = convertTriggerStringToEnum(tempStr);
        OICFree(tempStr);
        tempStr = NULL;

        // Resource type name
        err = err | cbor_value_map_find_value(arrayVal, OC_RSRVD_RESOURCE_TYPE, &curVal);
        if(cbor_value_is_valid(&curVal))
        {
            err = err | cbor_value_dup_text_string(&curVal, &tempStr, &len, NULL);
        }

        // TODO: Find a way so that this create doesn't make copies
        *outPayload = (OCPayload*)OCPresencePayloadCreate(seqNum, maxAge, trigger, tempStr);
        OICFree(tempStr);

        err = err | cbor_value_advance(arrayVal);

        if(err != CborNoError)
        {
            // TODO: clean up payload objects
            printf("ERICH: Cbor in error condition: %d\n", err);
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
