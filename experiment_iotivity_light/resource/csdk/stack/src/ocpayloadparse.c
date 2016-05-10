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

// Defining _POSIX_C_SOURCE macro with 200112L (or greater) as value
// causes header files to expose definitions
// corresponding to the POSIX.1-2001 base
// specification (excluding the XSI extension).
// For POSIX.1-2001 base specification,
// Refer http://pubs.opengroup.org/onlinepubs/009695399/
// Required for strok_r
#define _POSIX_C_SOURCE 200112L
#include <string.h>
#include <stdlib.h>
#include "oic_malloc.h"
#include "oic_string.h"
#include "ocstackinternal.h"
#include "ocpayload.h"
#include "cbor.h"
#include "ocresource.h"
#include "ocserverrequest.h"
#include "ocpayloadcbor.h"
#include "ocpayloadint.h"

#define TAG "OCPayloadParse"

#ifdef WITH_CLIENT
static OCStackResult OCParseDiscoveryPayload(OCPayload** outPayload, CborValue* arrayVal);
static OCStackResult OCParseDevicePayload(OCPayload** outPayload, CborValue* arrayVal);
static OCStackResult OCParsePlatformPayload(OCPayload** outPayload, CborValue* arrayVal);
static OCStackResult OCParsePresencePayload(OCPayload** outPayload, CborValue* arrayVal);
static OCStackResult OCParseSecurityPayload(OCPayload** outPayload, CborValue* arrayVal);
#endif // WITH_CLIENT

static bool OCParseSingleRepPayload(OCRepPayload** outPayload, CborValue* repParent);
static OCStackResult OCParseRepPayload(OCPayload** outPayload, CborValue* arrayVal);

OCStackResult OCParsePayload(OCPayload** outPayload, OCPayloadType payloadType,
        const uint8_t* payload, size_t payloadSize)
{
    CborParser parser;
    CborValue rootValue;
    CborValue arrayValue;
    bool err = false;

    OC_LOG_V(INFO, TAG, "CBOR Parsing size: %d", payloadSize, payload);
    err = cbor_parser_init(payload, payloadSize, 0, &parser, &rootValue);
    if (err != false)
    {
        OC_LOG_V(ERROR, TAG, "CBOR Parser init failed: %d", err);
        return OC_STACK_ERROR;
    }

    // enter the array
    err = cbor_value_enter_container(&rootValue, &arrayValue);
    if (err || arrayValue.type != CborMapType)
    {
        OC_LOG_V(ERROR, TAG, "CBOR payload parse failed :%d", err);
        return OC_STACK_MALFORMED_RESPONSE;
    }

    OCStackResult result = OC_STACK_ERROR;
    switch (payloadType)
    {
        case PAYLOAD_TYPE_DISCOVERY:
#ifdef WITH_CLIENT
            result = OCParseDiscoveryPayload(outPayload, &arrayValue);
#endif
            break;
        case PAYLOAD_TYPE_DEVICE:
#ifdef WITH_CLIENT
            result = OCParseDevicePayload(outPayload, &arrayValue);
#endif
            break;
        case PAYLOAD_TYPE_PLATFORM:
#ifdef WITH_CLIENT
            result = OCParsePlatformPayload(outPayload, &arrayValue);
#endif
            break;
        case PAYLOAD_TYPE_REPRESENTATION:
            result = OCParseRepPayload(outPayload, &arrayValue);
            break;
#ifdef WITH_PRESENCE
#ifdef WITH_CLIENT
        case PAYLOAD_TYPE_PRESENCE:
            result = OCParsePresencePayload(outPayload, &arrayValue);
            break;
#endif
#endif
        case PAYLOAD_TYPE_SECURITY:
#ifdef WITH_CLIENT
            result = OCParseSecurityPayload(outPayload, &arrayValue);
#endif
            break;
        default:
            OC_LOG_V(ERROR, TAG, "ParsePayload Type default: %d", payloadType);
            result = OC_STACK_ERROR;
            break;
    }

    if (result == OC_STACK_OK)
    {
        err = err || cbor_value_leave_container(&rootValue, &arrayValue);
        if (err != CborNoError)
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

static CborError OCPayloadString(CborValue *value, char **str)
{
    char buf[256];
    size_t len = sizeof (buf);
    CborError err = cbor_value_copy_text_string(value, buf, &len, NULL);
    if (err)
    {
        return err;
    }
    char *b = plMalloc(++len);
    OICStrcpy(b, len, buf);
    *str = b;
    return CborNoError;
}

#ifdef WITH_CLIENT
static OCStackResult OCParseSecurityPayload(OCPayload** outPayload, CborValue* arrayVal)
{
    if (!outPayload)
    {
        return OC_STACK_INVALID_PARAM;
    }

    bool err = false;
    char *securityData = NULL;

    if (!cbor_value_is_map(arrayVal))
    {
        OC_LOG(ERROR, TAG, "Cbor main value not a map");
        return OC_STACK_MALFORMED_RESPONSE;
    }

    CborValue curVal;
    err = err || cbor_value_map_find_value(arrayVal, OC_RSRVD_REPRESENTATION, &curVal);

    if (cbor_value_is_valid(&curVal))
    {
        err = err || OCPayloadString(&curVal, &securityData);
    }

    err = err || cbor_value_advance(arrayVal);

    if (err)
    {
        OC_LOG(ERROR, TAG, "Cbor in error condition");
        OICFree(securityData);
        return OC_STACK_MALFORMED_RESPONSE;
    }

    *outPayload = (OCPayload*)OCSecurityPayloadCreate(securityData);
    OICFree(securityData);

    return OC_STACK_OK;

}
#endif // WITH_CLIENT

static char* InPlaceStringTrim(char* str)
{
    while (str[0] == ' ')
    {
        ++str;
    }

    size_t lastchar = strlen(str);

    while (str[lastchar] == ' ')
    {
        str[lastchar] = '\0';
        --lastchar;
    }

    return str;
}

#ifdef WITH_CLIENT
static OCStackResult OCParseDiscoveryPayload(OCPayload** outPayload, CborValue* arrayVal)
{
    if (!outPayload)
    {
        return OC_STACK_INVALID_PARAM;
    }

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
            OC_LOG(ERROR, TAG, "Memory allocation failed");
            OICFree(out);
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
            CborValue rtVal;
            err = err || cbor_value_map_find_value(&curVal, OC_RSRVD_RESOURCE_TYPE, &rtVal);

            if (!err && cbor_value_is_text_string(&rtVal))
            {
                char* input = NULL;
                char* savePtr;
                err = err || cbor_value_dup_text_string(&rtVal, &input, &len, NULL);

                if (input)
                {
                    char* curPtr = strtok_r(input, " ", &savePtr);

                    while (curPtr)
                    {
                        char* trimmed = InPlaceStringTrim(curPtr);
                        if (trimmed[0] !='\0')
                        {
                            if (!OCResourcePayloadAddResourceType(resource, trimmed))
                            {
                                OICFree(resource->uri);
                                OICFree(resource->sid);
                                OCFreeOCStringLL(resource->types);
                                OICFree(resource);
                                OICFree(out);
                                return OC_STACK_NO_MEMORY;
                            }
                        }
                        curPtr = strtok_r(NULL, " ", &savePtr);
                    }
                    OICFree(input);
                }
            }

            // Interface Types
            CborValue ifVal;
            err = err || cbor_value_map_find_value(&curVal, OC_RSRVD_INTERFACE, &ifVal);

            if (!err && cbor_value_is_text_string(&ifVal))
            {
                char* input = NULL;
                char* savePtr;
                err = err || cbor_value_dup_text_string(&ifVal, &input, &len, NULL);

                if (input)
                {
                    char* curPtr = strtok_r(input, " ", &savePtr);

                    while (curPtr)
                    {
                        char* trimmed = InPlaceStringTrim(curPtr);
                        if (trimmed[0] !='\0')
                        {
                            if (!OCResourcePayloadAddInterface(resource, trimmed))
                            {
                                OICFree(resource->uri);
                                OICFree(resource->sid);
                                OCFreeOCStringLL(resource->types);
                                OICFree(resource);
                                OICFree(out);
                                return OC_STACK_NO_MEMORY;
                            }
                        }
                        curPtr = strtok_r(NULL, " ", &savePtr);
                    }
                    OICFree(input);
                }
            }

            // Policy
            {
                CborValue policyMap;
                err = err || cbor_value_map_find_value(&curVal, OC_RSRVD_POLICY, &policyMap);

                // Bitmap
                CborValue val;
                err = err || cbor_value_map_find_value(&policyMap, OC_RSRVD_BITMAP, &val);
                uint64_t temp = 0;
                err = err || cbor_value_get_uint64(&val, &temp);
                resource->flags = (uint8_t)temp;
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
            OICFree(out);
            OC_LOG_V(ERROR, TAG, "CBOR in error condition", err);
            return OC_STACK_MALFORMED_RESPONSE;
        }
        ++resourceCount;
        OCDiscoveryPayloadAddNewResource(out, resource);
    }

    if(err)
    {
        OICFree(out);
        return OC_STACK_MALFORMED_RESPONSE;
    }
    else
    {
        *outPayload = (OCPayload*)out;
        return OC_STACK_OK;
    }
}
#endif // WITH_CLIENT

#ifdef WITH_CLIENT
static OCStackResult OCParseDevicePayload(OCPayload** outPayload, CborValue* arrayVal)
{
    if (!outPayload)
    {
        return OC_STACK_INVALID_PARAM;
    }

    bool err = false;

    if (cbor_value_is_map(arrayVal))
    {
        OC_LOG(ERROR, TAG, "Root device node was not a map");
        return OC_STACK_MALFORMED_RESPONSE;
    }

    char *uri = NULL;
    uint8_t *sid = NULL;
    char *dname = NULL;
    char *specVer = NULL;
    char *dmVer = NULL;
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
#endif // WITH_CLIENT

#ifdef WITH_CLIENT
static OCStackResult OCParsePlatformPayload(OCPayload** outPayload, CborValue* arrayVal)
{
    if (!outPayload)
    {
        return OC_STACK_INVALID_PARAM;
    }

    bool err = false;

    if (cbor_value_is_map(arrayVal))
    {
        char* uri = NULL;
        OCPlatformInfo info;
        memset(&info, 0, sizeof (info));
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
            len = sizeof (info.platformID);
            err = err || cbor_value_copy_text_string(&repVal, info.platformID, &len, NULL);

            // MFG Name
            err = err || cbor_value_map_find_value(&curVal, OC_RSRVD_MFG_NAME, &repVal);
            len = sizeof (info.manufacturerName);
            err = err || cbor_value_copy_text_string(&repVal, info.manufacturerName, &len, NULL);

            // MFG URL
            err = err || cbor_value_map_find_value(&curVal, OC_RSRVD_MFG_URL, &repVal);
            if (cbor_value_is_valid(&repVal))
            {
                len = sizeof (info.manufacturerUrl);
                err = err || cbor_value_copy_text_string(&repVal, info.manufacturerUrl, &len, NULL);
            }

            // Model Num
            err = err || cbor_value_map_find_value(&curVal, OC_RSRVD_MODEL_NUM, &repVal);
            if (cbor_value_is_valid(&repVal))
            {
                len = sizeof (info.modelNumber);
                err = err || cbor_value_copy_text_string(&repVal, info.modelNumber, &len, NULL);
            }

            // Date of Mfg
            err = err || cbor_value_map_find_value(&curVal, OC_RSRVD_MFG_DATE, &repVal);
            if (cbor_value_is_valid(&repVal))
            {
                len = sizeof (info.dateOfManufacture);
                err = err || cbor_value_copy_text_string(&repVal, info.dateOfManufacture, &len, NULL);
            }

            // Platform Version
            err = err || cbor_value_map_find_value(&curVal, OC_RSRVD_PLATFORM_VERSION, &repVal);
            if (cbor_value_is_valid(&repVal))
            {
                len = sizeof (info.platformVersion);
                err = err || cbor_value_copy_text_string(&repVal, info.platformVersion, &len, NULL);
            }

            // OS Version
            err = err || cbor_value_map_find_value(&curVal, OC_RSRVD_OS_VERSION, &repVal);
            if (cbor_value_is_valid(&repVal))
            {
                len = sizeof (info.operatingSystemVersion);
                err = err || cbor_value_copy_text_string(&repVal, info.operatingSystemVersion, &len, NULL);
            }

            // Hardware Version
             err = err || cbor_value_map_find_value(&curVal, OC_RSRVD_HARDWARE_VERSION, &repVal);
            if (cbor_value_is_valid(&repVal))
            {
                len = sizeof (info.hardwareVersion);
                err = err || cbor_value_copy_text_string(&repVal, info.hardwareVersion, &len, NULL);
            }

            // Firmware Version
             err = err || cbor_value_map_find_value(&curVal, OC_RSRVD_FIRMWARE_VERSION, &repVal);
            if (cbor_value_is_valid(&repVal))
            {
                len = sizeof (info.firmwareVersion);
                err = err || cbor_value_copy_text_string(&repVal, info.firmwareVersion, &len, NULL);
            }

            // Support URL
             err = err || cbor_value_map_find_value(&curVal, OC_RSRVD_SUPPORT_URL, &repVal);
            if (cbor_value_is_valid(&repVal))
            {
                len = sizeof (info.supportUrl);
                err = err || cbor_value_copy_text_string(&repVal, info.supportUrl, &len, NULL);
            }

            // System Time
             err = err || cbor_value_map_find_value(&curVal, OC_RSRVD_SYSTEM_TIME, &repVal);
            if (cbor_value_is_valid(&repVal))
            {
                len = sizeof (info.systemTime);
                err = err || cbor_value_copy_text_string(&repVal, info.systemTime, &len, NULL);
            }
        }

         err = err || cbor_value_advance(arrayVal);

        if (err)
        {
            OC_LOG(ERROR, TAG, "CBOR error In ParsePlatformPayload");
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
        OC_LOG(ERROR, TAG, "Root device node was not a map");
        return OC_STACK_MALFORMED_RESPONSE;
    }
}
#endif // WITH_CLIENT

static bool OCParseSingleRepPayload(OCRepPayload** outPayload, CborValue* repParent)
{
    *outPayload = OCRepPayloadCreate();
    OCRepPayload* curPayload = *outPayload;
    bool err = false;
    char strbuf[256];

    size_t len;
    CborValue curVal;
    err = err || cbor_value_map_find_value(repParent, OC_RSRVD_HREF, &curVal);
    if (cbor_value_is_valid(&curVal))
    {
        err = err || OCPayloadString(&curVal, &curPayload->uri);
    }

    err = err || cbor_value_map_find_value(repParent, OC_RSRVD_PROPERTY, &curVal);
    if (cbor_value_is_valid(&curVal))
    {
        CborValue insidePropValue = {0};
        err = err || cbor_value_map_find_value(&curVal, OC_RSRVD_RESOURCE_TYPE,
                &insidePropValue);

        if (cbor_value_is_text_string(&insidePropValue))
        {
            size_t len = sizeof (strbuf);
            err = err || cbor_value_copy_text_string(&insidePropValue, strbuf, &len, NULL);

            char* savePtr;

            if (!err)
            {
                char* curPtr = strtok_r(strbuf, " ", &savePtr);

                while (curPtr)
                {
                    char* trimmed = InPlaceStringTrim(curPtr);
                    if (trimmed[0] != '\0')
                    {
                        OCRepPayloadAddResourceType(curPayload, curPtr);
                    }
                    curPtr = strtok_r(NULL, " ", &savePtr);
                }
            }
        }

        err = err || cbor_value_map_find_value(&curVal, OC_RSRVD_INTERFACE, &insidePropValue);

        if (cbor_value_is_text_string(&insidePropValue))
        {
            size_t len = sizeof (strbuf);
            err = err || cbor_value_copy_text_string(&insidePropValue, strbuf, &len, NULL);

            char* savePtr;

            if (!err)
            {
                char* curPtr = strtok_r(strbuf, " ", &savePtr);

                while (curPtr)
                {
                    char* trimmed = InPlaceStringTrim(curPtr);
                    if (trimmed[0] != '\0')
                    {
                        OCRepPayloadAddInterface(curPayload, curPtr);
                    }
                    curPtr = strtok_r(NULL, " ", &savePtr);
                }
            }
        }
    }

    err = err || cbor_value_map_find_value(repParent, OC_RSRVD_REPRESENTATION, &curVal);
    if (cbor_value_is_map(&curVal))
    {
        CborValue repMap;
        err = err || cbor_value_enter_container(&curVal, &repMap);

        while (!err && cbor_value_is_valid(&repMap))
        {
            char* name;
            err = err || OCPayloadString(&repMap, &name);

            err = err || cbor_value_advance(&repMap);

            int64_t intval = 0;
            bool boolval = false;
            double doubleval = 0;
#ifdef NESTED_PAYLOADS
            OCRepPayload* pl;
#endif // NESTED_PAYLOADS

            switch (cbor_value_get_type(&repMap))
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
                len = sizeof (strbuf);
                err = err || cbor_value_copy_text_string(&repMap, strbuf, &len, NULL);
                if (!err)
                {
                    err = !OCRepPayloadSetPropString(curPayload, name, strbuf);
                }
                break;
#ifdef NESTED_PAYLOADS
            case CborMapType:
                err = err || OCParseSingleRepPayload(&pl, &repMap);
                if (!err)
                {
                    err = !OCRepPayloadSetPropObjectAsOwner(curPayload, name, pl);
                }
                break;
#endif // NESTED_PAYLOADS
            default:
                OC_LOG_V(ERROR, TAG, "Parsing rep property, unknown type %d", repMap.type);
                err = true;
            }

            err = err || cbor_value_advance(&repMap);
        }
        err = err || cbor_value_leave_container(&curVal, &repMap);
    }

    if (err)
    {
        OICFree(*outPayload);
        *outPayload = NULL;
    }

    return err;
}

static OCStackResult OCParseRepPayload(OCPayload **outPayload, CborValue *arrayVal)
{
    bool err = false;

    OCRepPayload *rootPayload = NULL;
    OCRepPayload *curPayload = NULL;
    OCRepPayload *temp = NULL;

    while (!err && cbor_value_is_map(arrayVal))
    {
        err = err || OCParseSingleRepPayload(&temp, arrayVal);

        if (rootPayload == NULL)
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
        if (err)
        {
            OICFree(rootPayload);
            OC_LOG(ERROR, TAG, "CBOR error in ParseRepPayload");
            return OC_STACK_MALFORMED_RESPONSE;
        }
    }

    *outPayload = (OCPayload*)rootPayload;

    return OC_STACK_OK;
}

#ifdef WITH_PRESENCE
#ifdef WITH_CLIENT
static OCStackResult OCParsePresencePayload(OCPayload** outPayload, CborValue* arrayVal)
{
    if (!outPayload)
    {
        return OC_STACK_INVALID_PARAM;
    }

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
            OICFree(*outPayload);
            OC_LOG(ERROR, TAG, "CBOR error Parse Presence Payload");
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
        OC_LOG(ERROR, TAG, "Root presence node was not a map");
        return OC_STACK_MALFORMED_RESPONSE;
    }
}

#endif // WITH_CLIENT
#endif // WITH_PRESENCE
