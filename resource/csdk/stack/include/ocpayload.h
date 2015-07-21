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

#ifndef OCPAYLOAD_H_
#define OCPAYLOAD_H_

#include <stdbool.h>
#include <inttypes.h>
#include "logger.h"
#include "octypes.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct OCResource OCResource;

#ifdef TB_LOG
    #define OC_LOG_PAYLOAD(level, tag, payload) OCPayloadLog((level),(tag),(payload))
    #define UUID_SIZE (16)
    #define UUID_LENGTH (37)
const char *convertTriggerEnumToString(OCPresenceTrigger trigger);
OCPresenceTrigger convertTriggerStringToEnum(const char * triggerStr);

static inline void OCPayloadLogRep(LogLevel level, const char* tag, OCRepPayload* payload)
{
    OC_LOG(level, tag, PCF("Payload Type: Representation"));
    OCRepPayload* rep = payload;
    int i = 1;
    while(rep)
    {
        OC_LOG_V(level, tag, "\tResource #%d", i);
        OC_LOG_V(level, tag, "\tURL:%s", rep->uri);
        OC_LOG(level, tag, PCF("\tResource Types:"));
        OCStringLL* strll =  rep->types;
        while(strll)
        {
            OC_LOG_V(level, tag, "\t\t%s", strll->value);
            strll = strll->next;
        }
        OC_LOG(level, tag, PCF("\tInterfaces:"));
        strll =  rep->interfaces;
        while(strll)
        {
            OC_LOG_V(level, tag, "\t\t%s", strll->value);
            strll = strll->next;
        }

        // TODO Finish Logging: Values
        OCRepPayloadValue* val = rep->values;

        OC_LOG(level, tag, PCF("\tValues:"));

        while(val)
        {
            switch(val->type)
            {
                case OCREP_PROP_NULL:
                    OC_LOG_V(level, tag, "\t\t%s: NULL", val->name);
                    break;
                case OCREP_PROP_INT:
                    OC_LOG_V(level, tag, "\t\t%s(int):%lld", val->name, val->i);
                    break;
                case OCREP_PROP_DOUBLE:
                    OC_LOG_V(level, tag, "\t\t%s(double):%f", val->name, val->d);
                    break;
                case OCREP_PROP_BOOL:
                    OC_LOG_V(level, tag, "\t\t%s(bool):%s", val->name, val->b ? "true" : "false");
                    break;
                case OCREP_PROP_STRING:
                    OC_LOG_V(level, tag, "\t\t%s(string):%s", val->name, val->str);
                    break;
                case OCREP_PROP_OBJECT:
                    // Note: Only prints the URI (if available), to print further, you'll
                    // need to dig into the object better!
                    OC_LOG_V(level, tag, "\t\t%s(OCRep):%s", val->name, val->obj->uri);
                    break;
                case OCREP_PROP_ARRAY:
                    switch(val->arr.type)
                    {
                        case OCREP_PROP_INT:
                            OC_LOG_V(level, tag, "\t\t%s(int array):%lld x %lld x %lld",
                                    val->name,
                                    val->arr.dimensions[0], val->arr.dimensions[1],
                                    val->arr.dimensions[2]);
                            break;
                        case OCREP_PROP_DOUBLE:
                            OC_LOG_V(level, tag, "\t\t%s(double array):%lld x %lld x %lld",
                                    val->name,
                                    val->arr.dimensions[0], val->arr.dimensions[1],
                                    val->arr.dimensions[2]);
                            break;
                        case OCREP_PROP_BOOL:
                            OC_LOG_V(level, tag, "\t\t%s(bool array):%lld x %lld x %lld",
                                    val->name,
                                    val->arr.dimensions[0], val->arr.dimensions[1],
                                    val->arr.dimensions[2]);
                            break;
                        case OCREP_PROP_STRING:
                            OC_LOG_V(level, tag, "\t\t%s(string array):%lld x %lld x %lld",
                                    val->name,
                                    val->arr.dimensions[0], val->arr.dimensions[1],
                                    val->arr.dimensions[2]);
                            break;
                        case OCREP_PROP_OBJECT:
                            OC_LOG_V(level, tag, "\t\t%s(OCRep array):%lld x %lld x %lld",
                                    val->name,
                                    val->arr.dimensions[0], val->arr.dimensions[1],
                                    val->arr.dimensions[2]);
                            break;
                        default:
                            OC_LOG_V(ERROR, tag, "\t\t%s <-- Unknown/unsupported array type!",
                                    val->name);
                            break;
                    }
                    break;
                default:
                    OC_LOG_V(ERROR, tag, "\t\t%s <-- Unknown type!", val->name);
                    break;
            }
            val = val -> next;
        }

        ++i;
        rep = rep->next;
    }

}

static inline void OCPayloadLogDiscovery(LogLevel level, const char* tag,
        OCDiscoveryPayload* payload)
{
    OC_LOG(level, tag, PCF("Payload Type: Discovery"));
    int i = 1;

    if(!payload->resources)
    {
        OC_LOG(level, tag, PCF("\tNO Resources"));
        return;
    }

    OCResourcePayload* res = payload->resources;

    while(res)
    {
        OC_LOG_V(level, tag, "\tResource #%d", i);
        OC_LOG_V(level, tag, "\tURI:%s", res->uri);
        OC_LOG(level, tag, PCF("\tSID:"));
        OC_LOG_BUFFER(level, tag, res->sid, UUID_SIZE);
        OC_LOG(level, tag, PCF("\tResource Types:"));
        OCStringLL* strll =  res->types;
        while(strll)
        {
            OC_LOG_V(level, tag, "\t\t%s", strll->value);
            strll = strll->next;
        }
        OC_LOG(level, tag, PCF("\tInterfaces:"));
        strll =  res->interfaces;
        while(strll)
        {
            OC_LOG_V(level, tag, "\t\t%s", strll->value);
            strll = strll->next;
        }

        OC_LOG_V(level, tag, "\tBitmap: %u", res->bitmap);
        OC_LOG_V(level, tag, "\tSecure?: %s", res->secure ? "true" : "false");
        OC_LOG_V(level, tag, "\tPort: %u", res->port);
        OC_LOG(level, tag, PCF(""));
        res = res->next;
        ++i;
    }
}

static inline void OCPayloadLogDevice(LogLevel level, const char* tag, OCDevicePayload* payload)
{
    OC_LOG(level, tag, PCF("Payload Type: Device"));
    OC_LOG_V(level, tag, "\tURI:%s", payload->uri);
    OC_LOG(level, tag, PCF("\tSID:"));
    OC_LOG_BUFFER(level, tag, payload->sid, UUID_SIZE);
    OC_LOG_V(level, tag, "\tDevice Name:%s", payload->deviceName);
    OC_LOG_V(level, tag, "\tSpec Version%s", payload->specVersion);
    OC_LOG_V(level, tag, "\tData Model Version:%s", payload->dataModelVersion);
}

static inline void OCPayloadLogPlatform(LogLevel level, const char* tag, OCPlatformPayload* payload)
{
    OC_LOG(level, tag, PCF("Payload Type: Platform"));
    OC_LOG_V(level, tag, "\tURI:%s", payload->uri);
    OC_LOG_V(level, tag, "\tPlatform ID:%s", payload->info.platformID);
    OC_LOG_V(level, tag, "\tMfg Name:%s", payload->info.manufacturerName);
    OC_LOG_V(level, tag, "\tMfg URL:%s", payload->info.manufacturerUrl);
    OC_LOG_V(level, tag, "\tModel Number:%s", payload->info.modelNumber);
    OC_LOG_V(level, tag, "\tDate of Mfg:%s", payload->info.dateOfManufacture);
    OC_LOG_V(level, tag, "\tPlatform Version:%s", payload->info.platformVersion);
    OC_LOG_V(level, tag, "\tOS Version:%s", payload->info.operatingSystemVersion);
    OC_LOG_V(level, tag, "\tHardware Version:%s", payload->info.hardwareVersion);
    OC_LOG_V(level, tag, "\tFirmware Version:%s", payload->info.firmwareVersion);
    OC_LOG_V(level, tag, "\tSupport URL:%s", payload->info.supportUrl);
    OC_LOG_V(level, tag, "\tSystem Time:%s", payload->info.systemTime);
}

static inline void OCPayloadLogPresence(LogLevel level, const char* tag, OCPresencePayload* payload)
{
    OC_LOG(level, tag, PCF("Payload Type: Presence"));
    OC_LOG_V(level, tag, "\tSequence Number:%u", payload->sequenceNumber);
    OC_LOG_V(level, tag, "\tMax Age:%d", payload->maxAge);
    OC_LOG_V(level, tag, "\tTrigger:%s", convertTriggerEnumToString(payload->trigger));
    OC_LOG_V(level, tag, "\tResource Type:%s", payload->resourceType);
}

static inline void OCPayloadLogSecurity(LogLevel level, const char* tag,
                                        OCSecurityPayload* payload)
{
    OC_LOG(level, tag, PCF("Payload Type: Security"));
    OC_LOG_V(level, tag, "\tSecurity Data: %s", payload->securityData);
}

static inline void OCPayloadLog(LogLevel level, const char* tag, OCPayload* payload)
{
    if(!payload)
    {
        OC_LOG(level, tag, PCF("NULL Payload"));
        return;
    }
    switch(payload->type)
    {
        case PAYLOAD_TYPE_REPRESENTATION:
            OCPayloadLogRep(level, tag, (OCRepPayload*)payload);
            break;
        case PAYLOAD_TYPE_DISCOVERY:
            OCPayloadLogDiscovery(level, tag, (OCDiscoveryPayload*)payload);
            break;
        case PAYLOAD_TYPE_DEVICE:
            OCPayloadLogDevice(level, tag, (OCDevicePayload*)payload);
            break;
        case PAYLOAD_TYPE_PLATFORM:
            OCPayloadLogPlatform(level, tag, (OCPlatformPayload*)payload);
            break;
        case PAYLOAD_TYPE_PRESENCE:
            OCPayloadLogPresence(level, tag, (OCPresencePayload*)payload);
            break;
        case PAYLOAD_TYPE_SECURITY:
            OCPayloadLogSecurity(level, tag, (OCSecurityPayload*)payload);
            break;
        default:
            OC_LOG_V(level, tag, "Unknown Payload Type: %d", payload->type);
            break;
    }
}
#else
    #define OC_LOG_PAYLOAD(level, tag, payload)
#endif

void OCPayloadDestroy(OCPayload* payload);

// Representation Payload
OCRepPayload* OCRepPayloadCreate();

size_t calcDimTotal(const size_t dimensions[MAX_REP_ARRAY_DEPTH]);

OCRepPayload* OCRepPayloadClone(const OCRepPayload* payload);

void OCRepPayloadAppend(OCRepPayload* parent, OCRepPayload* child);

bool OCRepPayloadSetUri(OCRepPayload* payload, const char* uri);

bool OCRepPayloadAddResourceType(OCRepPayload* payload, const char* resourceType);
bool OCRepPayloadAddInterface(OCRepPayload* payload, const char* interface);

bool OCRepPayloadAddResourceTypeAsOwner(OCRepPayload* payload, char* resourceType);
bool OCRepPayloadAddInterfaceAsOwner(OCRepPayload* payload, char* interface);

bool OCRepPayloadIsNull(const OCRepPayload* payload, const char* name);
bool OCRepPayloadSetNull(OCRepPayload* payload, const char* name);

bool OCRepPayloadSetPropInt(OCRepPayload* payload, const char* name, int64_t value);
bool OCRepPayloadGetPropInt(const OCRepPayload* payload, const char* name, int64_t* value);

bool OCRepPayloadSetPropDouble(OCRepPayload* payload, const char* name, double value);
bool OCRepPayloadGetPropDouble(const OCRepPayload* payload, const char* name, double* value);

bool OCRepPayloadSetPropString(OCRepPayload* payload, const char* name, const char* value);
bool OCRepPayloadSetPropStringAsOwner(OCRepPayload* payload, const char* name, char* value);
bool OCRepPayloadGetPropString(const OCRepPayload* payload, const char* name, const char** value);

bool OCRepPayloadSetPropBool(OCRepPayload* payload, const char* name, bool value);
bool OCRepPayloadGetPropBool(const OCRepPayload* payload, const char* name, bool* value);

bool OCRepPayloadSetPropObject(OCRepPayload* payload, const char* name, const OCRepPayload* value);
bool OCRepPayloadSetPropObjectAsOwner(OCRepPayload* payload, const char* name,
        OCRepPayload* value);
bool OCRepPayloadGetPropObject(const OCRepPayload* payload, const char* name, OCRepPayload** value);

bool OCRepPayloadSetIntArrayAsOwner(OCRepPayload* payload, const char* name,
        int64_t* array, size_t dimensions[MAX_REP_ARRAY_DEPTH]);
bool OCRepPayloadSetIntArray(OCRepPayload* payload, const char* name,
        const int64_t* array, size_t dimensions[MAX_REP_ARRAY_DEPTH]);
bool OCRepPayloadGetIntArray(const OCRepPayload* payload, const char* name,
        int64_t** array, size_t dimensions[MAX_REP_ARRAY_DEPTH]);

bool OCRepPayloadSetDoubleArrayAsOwner(OCRepPayload* payload, const char* name,
        double* array, size_t dimensions[MAX_REP_ARRAY_DEPTH]);
bool OCRepPayloadSetDoubleArray(OCRepPayload* payload, const char* name,
        const double* array, size_t dimensions[MAX_REP_ARRAY_DEPTH]);
bool OCRepPayloadGetDoubleArray(const OCRepPayload* payload, const char* name,
        double** array, size_t dimensions[MAX_REP_ARRAY_DEPTH]);

bool OCRepPayloadSetStringArrayAsOwner(OCRepPayload* payload, const char* name,
        char** array, size_t dimensions[MAX_REP_ARRAY_DEPTH]);
bool OCRepPayloadSetStringArray(OCRepPayload* payload, const char* name,
        const char** array, size_t dimensions[MAX_REP_ARRAY_DEPTH]);
bool OCRepPayloadGetStringArray(const OCRepPayload* payload, const char* name,
        char*** array, size_t dimensions[MAX_REP_ARRAY_DEPTH]);

bool OCRepPayloadSetBoolArrayAsOwner(OCRepPayload* payload, const char* name,
        bool* array, size_t dimensions[MAX_REP_ARRAY_DEPTH]);
bool OCRepPayloadSetBoolArray(OCRepPayload* payload, const char* name,
        const bool* array, size_t dimensions[MAX_REP_ARRAY_DEPTH]);
bool OCRepPayloadGetBoolArray(const OCRepPayload* payload, const char* name,
        bool** array, size_t dimensions[MAX_REP_ARRAY_DEPTH]);

bool OCRepPayloadSetPropObjectArrayAsOwner(OCRepPayload* payload, const char* name,
        OCRepPayload** array, size_t dimensions[MAX_REP_ARRAY_DEPTH]);
bool OCRepPayloadSetPropObjectArray(OCRepPayload* payload, const char* name,
        const OCRepPayload** array, size_t dimensions[MAX_REP_ARRAY_DEPTH]);
bool OCRepPayloadGetPropObjectArray(const OCRepPayload* payload, const char* name,
        OCRepPayload*** array, size_t dimensions[MAX_REP_ARRAY_DEPTH]);

void OCRepPayloadDestroy(OCRepPayload* payload);

// Discovery Payload
OCDiscoveryPayload* OCDiscoveryPayloadCreate();

OCSecurityPayload* OCSecurityPayloadCreate(char* securityData);
void OCSecurityPayloadDestroy(OCSecurityPayload* payload);

void OCDiscoveryPayloadAddResource(OCDiscoveryPayload* payload, const OCResource* res,
        uint16_t port);
void OCDiscoveryPayloadAddNewResource(OCDiscoveryPayload* payload, OCResourcePayload* res);
size_t OCDiscoveryPayloadGetResourceCount(OCDiscoveryPayload* payload);
OCResourcePayload* OCDiscoveryPayloadGetResource(OCDiscoveryPayload* payload, size_t index);

void OCDiscoveryPayloadDestroy(OCDiscoveryPayload* payload);

// Device Payload
OCDevicePayload* OCDevicePayloadCreate(const char* uri, const uint8_t* sid, const char* dname,
        const char* specVer, const char* dmVer);
void OCDevicePayloadDestroy(OCDevicePayload* payload);

// Platform Payload
OCPlatformPayload* OCPlatformPayloadCreate(const char* uri, const OCPlatformInfo* platformInfo);
OCPlatformPayload* OCPlatformPayloadCreateAsOwner(char* uri, OCPlatformInfo* platformInfo);

void OCPlatformPayloadDestroy(OCPlatformPayload* payload);

// Presence Payload
OCPresencePayload* OCPresencePayloadCreate(uint32_t seqNum, uint32_t maxAge,
        OCPresenceTrigger trigger, const char* resourceType);
void OCPresencePayloadDestroy(OCPresencePayload* payload);

#ifdef __cplusplus
}
#endif

#endif

