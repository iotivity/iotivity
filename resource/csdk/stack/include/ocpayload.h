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
#include "logger.h"
#include "octypes.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct rsrc_t OCResource;

#ifdef TB_LOG
    #define OC_LOG_PAYLOAD(level, tag, payload) OCPayloadLog((level),(tag),(payload))
    void OCPayloadLog(LogLevel level, const char* tag, OCPayload* payload);
#else
    #define OC_LOG_PAYLOAD(level, tag, payload)
#endif

void OCPayloadDestroy(OCPayload* payload);

// Representation Payload
OCRepPayload* OCRepPayloadCreate();

bool OCRepPayloadSetUri(OCRepPayload* payload, const char*  uri);

bool OCRepPayloadAddResourceType(OCRepPayload* payload, char* resourceType);
bool OCRepPayloadAddInterface(OCRepPayload* payload, char* interface);
bool OCRepPayloadAddResourceTypeAsOwner(OCRepPayload* payload, char* resourceType);
bool OCRepPayloadAddInterfaceAsOwner(OCRepPayload* payload, char* interface);

bool OCRepPayloadSetPropInt(OCRepPayload* payload, const char* name, int64_t value);
bool OCRepPayloadGetPropInt(OCRepPayload* payload, const char* name, int64_t* value);
bool OCRepPayloadSetPropBool(OCRepPayload* payload, const char* name, bool value);
bool OCRepPayloadGetPropBool(OCRepPayload* payload, const char* name, bool* value);
bool OCRepPayloadSetPropString(OCRepPayload* payload, const char* name, const char* value);

void OCRepPayloadDestroy(OCRepPayload* payload);

// Discovery Payload
OCDiscoveryPayload* OCDiscoveryPayloadCreate();

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

