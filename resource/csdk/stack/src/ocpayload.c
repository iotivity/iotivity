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


#include "ocpayload.h"
#include "octypes.h"
#include <string.h>
#include "oic_malloc.h"
#include "oic_string.h"
#include "ocstackinternal.h"
#include "ocresource.h"


void OCPayloadDestroy(OCPayload* payload)
{
    if(!payload)
    {
        return;
    }

    switch(payload->type)
    {
        case PAYLOAD_TYPE_REPRESENTATION:
            OCRepPayloadDestroy((OCRepPayload*)payload);
            break;
        case PAYLOAD_TYPE_DISCOVERY:
            OCDiscoveryPayloadDestroy((OCDiscoveryPayload*)payload);
            break;
        case PAYLOAD_TYPE_DEVICE:
            OCDevicePayloadDestroy((OCDevicePayload*)payload);
            break;
        case PAYLOAD_TYPE_PLATFORM:
            OCPlatformPayloadDestroy((OCPlatformPayload*)payload);
            break;
        case PAYLOAD_TYPE_PRESENCE:
            OCPresencePayloadDestroy((OCPresencePayload*)payload);
        default:
            printf("Not supported payload type: %d\n", payload->type);
            printf("OCPayloadDestroy default\n");
            exit(-1);
            break;
    }
}
OCRepPayload* OCRepPayloadCreate()
{
    OCRepPayload* payload = (OCRepPayload*)OICCalloc(1, sizeof(OCRepPayload));

    if(!payload)
    {
        return NULL;
    }

    payload->type = PAYLOAD_TYPE_REPRESENTATION;

    return payload;
}

void OCRepPayloadAppend(OCRepPayload* parent, OCRepPayload* child)
{
    if(!parent)
    {
        return;
    }

    while(parent->next)
    {
        parent = parent->next;
    }

    parent->next= child;
}

OCRepPayloadValue* OCRepPayloadFindValue(OCRepPayload* payload, const char* name)
{
    if(!payload || !name)
    {
        return NULL;
    }

    OCRepPayloadValue* val = payload->values;
    while(val)
    {
        if(0 == strcmp(val->name, name))
        {
            return val;
        }
        val = val->next;
    }

    return NULL;

}

void CopyPropertyValue (OCRepPayloadValue *dest, OCRepPayloadValue *source)
{
    if (!source || !dest)
    {
        return;
    }
    if (source->type == OCREP_PROP_STRING)
    {
        dest->str = OICStrdup (source->str);
    }
    else
    {
        // TODO: Copy other types.
    }
}

void FreeRepPayloadValue(OCRepPayloadValue* val)
{
    if(!val)
    {
        return;
    }

    if(val->type == OCREP_PROP_STRING)
    {
        OICFree(val->str);
    }

    OICFree(val->name);
    FreeRepPayloadValue(val->next);
    OICFree(val);

}

OCRepPayloadValue* OCRepPayloadValueClone (OCRepPayloadValue* source)
{
    if (!source)
    {
        return NULL;
    }

    OCRepPayloadValue *sourceIter = source;
    OCRepPayloadValue *destIter = (OCRepPayloadValue*) OICCalloc(1, sizeof(OCRepPayloadValue));
    if (!destIter)
    {
        return NULL;
    }

    OCRepPayloadValue *headOfClone = destIter;

    // Copy payload type and non pointer types in union.
    *destIter = *sourceIter;
    destIter->name = OICStrdup (sourceIter->name);
    CopyPropertyValue (destIter, sourceIter);

    sourceIter = sourceIter->next;

    while (sourceIter)
    {
        destIter->next = (OCRepPayloadValue*) OICCalloc(1, sizeof(OCRepPayloadValue));
        if (!destIter->next)
        {
            FreeRepPayloadValue (headOfClone);
            return NULL;
        }

        *(destIter->next) = *sourceIter;
        destIter->next->name = OICStrdup (sourceIter->name);
        CopyPropertyValue (destIter->next, sourceIter);

        sourceIter = sourceIter->next;
        destIter = destIter->next;
    }
    return headOfClone;
}

OCRepPayloadValue* OCRepPayloadFindAndSetValue(OCRepPayload* payload, const char* name,
        OCRepPayloadPropType type)
{
    if(!payload || !name)
    {
        return NULL;
    }

    OCRepPayloadValue* val = payload->values;
    if(val == NULL)
    {
        payload->values = (OCRepPayloadValue*)OICCalloc(1, sizeof(OCRepPayloadValue));
        payload->values->name = OICStrdup(name);
        payload->values->type =type;
        return payload->values;
    }

    while(val)
    {
        if(0 == strcmp(val->name, name))
        {
            val->type = type;
            return val;
        }
        else if(val->next == NULL)
        {
            val->next = (OCRepPayloadValue*)OICCalloc(1, sizeof(OCRepPayloadValue));
            val->next->name = OICStrdup(name);
            val->next->type =type;
            return val->next;
        }

        val = val->next;
    }

    printf("Find and set value, unreachable?!");
    exit(-1);
    return NULL;
}

bool OCRepPayloadAddResourceType(OCRepPayload* payload, const char* resourceType)
{
    return OCRepPayloadAddResourceTypeAsOwner(payload, OICStrdup(resourceType));
}

bool OCRepPayloadAddResourceTypeAsOwner(OCRepPayload* payload, char* resourceType)
{
    if(!payload || !resourceType)
    {
        return false;
    }

    if(payload->types)
    {
        OCStringLL* cur = payload->types;
        while(cur->next)
        {
            cur = cur->next;
        }
        cur->next = (OCStringLL*)OICCalloc(1, sizeof(OCStringLL));

        if(!cur->next)
        {
            return false;
        }

        cur->next->value = resourceType;
        return true;
    }
    else
    {
        payload->types = (OCStringLL*)OICCalloc(1, sizeof(OCStringLL));
        if(!payload->types)
        {
            return false;
        }
        payload->types->value = resourceType;
        return true;
    }
}

bool OCRepPayloadAddInterface(OCRepPayload* payload, const char* interface)
{
    return OCRepPayloadAddInterfaceAsOwner(payload, OICStrdup(interface));
}

bool OCRepPayloadAddInterfaceAsOwner(OCRepPayload* payload, char* interface)
{
    if(!payload || !interface)
    {
        return false;
    }

    if(payload->interfaces)
    {
        OCStringLL* cur = payload->interfaces;
        while(cur->next)
        {
            cur = cur->next;
        }
        cur->next = (OCStringLL*)OICCalloc(1, sizeof(OCStringLL));

        if(!cur->next)
        {
            return false;
        }
        cur->next->value = interface;
        return true;
    }
    else
    {
        payload->interfaces = (OCStringLL*)OICCalloc(1, sizeof(OCStringLL));
        if(!payload->interfaces)
        {
            return false;
        }
        payload->interfaces->value = interface;
        return true;
    }
}

bool OCRepPayloadSetUri(OCRepPayload* payload, const char*  uri)
{
    if(!payload)
    {
        return false;
    }

    payload->uri = OICStrdup(uri);
    return payload->uri != NULL;
}

bool OCRepPayloadSetPropInt(OCRepPayload* payload,
        const char* name, int64_t value)
{
    OCRepPayloadValue* val = OCRepPayloadFindAndSetValue(payload, name, OCREP_PROP_INT);

    if(!val)
    {
        return false;
    }

    val->i = value;
    return true;
}

bool OCRepPayloadSetPropBool(OCRepPayload* payload,
        const char* name, bool value)
{
    OCRepPayloadValue* val = OCRepPayloadFindAndSetValue(payload, name, OCREP_PROP_BOOL);

    if(!val )
    {
        return false;
    }

    val->b = value;
    return true;
}

bool OCRepPayloadSetPropString(OCRepPayload* payload, const char* name, const char* value)
{
    OCRepPayloadValue* val = OCRepPayloadFindAndSetValue(payload, name, OCREP_PROP_STRING);

    if(!val)
    {
        return false;
    }

    val->str = OICStrdup(value);
    return val->str != NULL;
}

bool OCRepPayloadGetPropInt(OCRepPayload* payload, const char* name, int64_t* value)
{
    OCRepPayloadValue* val = OCRepPayloadFindValue(payload, name);

    if(!val)
    {
        return false;
    }

    *value = val->i;
    return true;
}

bool OCRepPayloadGetPropBool(OCRepPayload* payload, const char* name, bool* value)
{
    OCRepPayloadValue* val = OCRepPayloadFindValue(payload, name);

    if(!val)
    {
        return false;
    }

    *value = val->b;
    return true;
}

void FreeOCStringLL(OCStringLL* ll)
{
    if(!ll)
    {
        return;
    }

    FreeOCStringLL(ll->next);
    OICFree(ll->value);
    OICFree(ll);
}

OCStringLL* CloneOCStringLL (OCStringLL* ll)
{
    if (!ll)
    {
        return NULL;
    }

    OCStringLL *sourceIter = ll;

    OCStringLL *destIter = OICCalloc (1, sizeof (OCStringLL));
    if (!destIter)
    {
        return NULL;
    }
    destIter->value = OICStrdup (sourceIter->value);

    OCStringLL *headOfClone = destIter;

    sourceIter = sourceIter->next;

    while (sourceIter)
    {
        destIter->next  = OICCalloc (1, sizeof (OCStringLL));
        if (!destIter->next)
        {
            FreeOCStringLL (headOfClone);
            return NULL;
        }
        destIter->next->value = OICStrdup (sourceIter->value);

        destIter = destIter->next;
        sourceIter = sourceIter->next;
    }
    return headOfClone;
}

OCRepPayload* OCRepPayloadClone (const OCRepPayload* payload)
{
    if (!payload)
    {
        return NULL;
    }

    OCRepPayload *clone = OCRepPayloadCreate();

    if (!clone)
    {
        return NULL;
    }

    clone->uri = OICStrdup (payload->uri);
    clone->types = CloneOCStringLL (payload->types);
    clone->interfaces = CloneOCStringLL (payload->interfaces);
    clone->values = OCRepPayloadValueClone (payload->values);

    return clone;
}


void OCRepPayloadDestroy(OCRepPayload* payload)
{
    if(!payload)
    {
        return;
    }

    OICFree(payload->uri);
    FreeOCStringLL(payload->types);
    FreeOCStringLL(payload->interfaces);
    FreeRepPayloadValue(payload->values);
    OCRepPayloadDestroy(payload->next);
    OICFree(payload);
}

OCDiscoveryPayload* OCDiscoveryPayloadCreate()
{
    OCDiscoveryPayload* payload = (OCDiscoveryPayload*)OICCalloc(1, sizeof(OCDiscoveryPayload));

    if(!payload)
    {
        return NULL;
    }

    payload->type = PAYLOAD_TYPE_DISCOVERY;

    return payload;
}

size_t OCDiscoveryPayloadGetResourceCount(OCDiscoveryPayload* payload)
{
    size_t i = 0;
    OCResourcePayload* p = payload->resources;
    while(p)
    {
        ++i;
        p = p->next;
    }
    return i;
}

OCResourcePayload* OCDiscoveryPayloadGetResource(OCDiscoveryPayload* payload, size_t index)
{
    size_t i = 0;
    OCResourcePayload* p = payload->resources;
    while(p)
    {
        if(i == index)
        {
            return p;
        }
        ++i;
        p = p->next;
    }
    return NULL;
}

OCResourcePayload* copyResource(const OCResource* res, uint16_t port)
{
    OCResourcePayload* pl = (OCResourcePayload*)OICCalloc(1, sizeof(OCResourcePayload));
    if(!pl)
    {
        return NULL;
    }

    pl->uri = OICStrdup(res->uri);
    pl->sid = OICCalloc(1, UUID_SIZE);
    memcpy(pl->sid, OCGetServerInstanceID(), UUID_SIZE);

    // types
    OCResourceType* typePtr = res->rsrcType;

    if(typePtr != NULL)
    {
        pl->types = (OCStringLL*)OICCalloc(1, sizeof(OCStringLL));
        pl->types->value = OICStrdup(typePtr->resourcetypename);

        OCStringLL* cur = pl->types;
        typePtr = typePtr->next;
        while(typePtr)
        {
            cur->next = (OCStringLL*)OICCalloc(1, sizeof(OCStringLL));
            cur->next->value = OICStrdup(typePtr->resourcetypename);
            cur = cur->next;
            typePtr = typePtr->next;
        }
    }

    // interfaces
    OCResourceInterface* ifPtr = res->rsrcInterface;
    if(ifPtr != NULL)
    {
        pl->interfaces = (OCStringLL*)OICCalloc(1, sizeof(OCStringLL));
        pl->interfaces->value = OICStrdup(ifPtr->name);

        OCStringLL* cur = pl->types;
        ifPtr = ifPtr->next;
        while(ifPtr)
        {
            cur->next = (OCStringLL*)OICCalloc(1, sizeof(OCStringLL));
            cur->next->value = OICStrdup(ifPtr->name);
            cur = cur->next;
            ifPtr = ifPtr->next;
        }
    }

    pl->bitmap = res->resourceProperties & (OC_OBSERVABLE | OC_DISCOVERABLE);
    pl->secure = (res->resourceProperties & OC_SECURE) != 0;
    pl->port = port;

    return pl;
}

void OCDiscoveryPayloadAddResource(OCDiscoveryPayload* payload, const OCResource* res,
        uint16_t port)
{
    OCDiscoveryPayloadAddNewResource(payload, copyResource(res, port));
}

void OCDiscoveryPayloadAddNewResource(OCDiscoveryPayload* payload, OCResourcePayload* res)
{
    if(!payload->resources)
    {
        payload->resources = res;
    }
    else
    {
        OCResourcePayload* p = payload->resources;
        while(p->next)
        {
            p = p->next;
        }
        p->next = res;
    }
}

void FreeOCDiscoveryResource(OCResourcePayload* payload)
{
    if(!payload)
    {
        return;
    }

    OICFree(payload->uri);
    OICFree(payload->sid);
    FreeOCStringLL(payload->types);
    FreeOCStringLL(payload->interfaces);
    FreeOCDiscoveryResource(payload->next);
    OICFree(payload);

}
void OCDiscoveryPayloadDestroy(OCDiscoveryPayload* payload)
{
    if(!payload)
    {
        return;
    }

    FreeOCDiscoveryResource(payload->resources);
    OICFree(payload);
}

OCDevicePayload* OCDevicePayloadCreate(const char* uri, const uint8_t* sid, const char* dname,
        const char* specVer, const char* dmVer)
{

    OCDevicePayload* payload = (OCDevicePayload*)OICCalloc(1, sizeof(OCDevicePayload));

    if(!payload)
    {
        return NULL;
    }

    // TODO: we can likely find a way on the 'send' to not allocate memory for this, and just
    // reuse valid pointers, just need to make sure we don't delete them after, but still do
    // on the receiver side
    payload->type = PAYLOAD_TYPE_DEVICE;
    payload->uri = OICStrdup(uri);
    payload->sid = (uint8_t*)OICMalloc(UUID_SIZE);
    memcpy(payload->sid, sid, UUID_SIZE);
    payload->deviceName = OICStrdup(dname);
    payload->specVersion = OICStrdup(specVer);
    payload->dataModelVersion = OICStrdup(dmVer);
    // TODO: Check each allocation, return NULL and cleanup if any fail
    return payload;
}

void OCDevicePayloadDestroy(OCDevicePayload* payload)
{
    if(!payload)
    {
        return;
    }

    OICFree(payload->uri);
    OICFree(payload->sid);
    OICFree(payload->deviceName);
    OICFree(payload->specVersion);
    OICFree(payload->dataModelVersion);
    OICFree(payload);
}

void CopyPlatformInfo(const OCPlatformInfo* platformInfo, OCPlatformPayload* target)
{
    target->info.platformID = OICStrdup(platformInfo->platformID);
    target->info.manufacturerName = OICStrdup(platformInfo->manufacturerName);
    target->info.manufacturerUrl = OICStrdup(platformInfo->manufacturerUrl);
    target->info.modelNumber = OICStrdup(platformInfo->modelNumber);
    target->info.dateOfManufacture = OICStrdup(platformInfo->dateOfManufacture);
    target->info.platformVersion = OICStrdup(platformInfo->platformVersion);
    target->info.operatingSystemVersion = OICStrdup(platformInfo->operatingSystemVersion);
    target->info.hardwareVersion = OICStrdup(platformInfo->hardwareVersion);
    target->info.firmwareVersion = OICStrdup(platformInfo->firmwareVersion);
    target->info.supportUrl = OICStrdup(platformInfo->supportUrl);
    target->info.systemTime = OICStrdup(platformInfo->systemTime);
}

OCPlatformPayload* OCPlatformPayloadCreateAsOwner(char* uri, OCPlatformInfo* platformInfo)
{
    OCPlatformPayload* payload = (OCPlatformPayload*)OICCalloc(1, sizeof(OCPlatformPayload));
    if(!payload)
    {
        return NULL;
    }

    payload->type = PAYLOAD_TYPE_PLATFORM;
    payload->uri = uri;
    payload->info = *platformInfo;

    return payload;
}

OCPlatformPayload* OCPlatformPayloadCreate(const char* uri, const OCPlatformInfo* platformInfo)
{
    OCPlatformPayload* payload = (OCPlatformPayload*)OICCalloc(1, sizeof(OCPlatformPayload));

    if(!payload)
    {
        return NULL;
    }

    // TODO: we can likely find a way on the 'send' to not allocate memory for this, and just
    // reuse valid pointers, just need to make sure we don't delete them after, but still do
    // on the receiver side
    payload->type = PAYLOAD_TYPE_PLATFORM;
    payload->uri = OICStrdup(uri);
    CopyPlatformInfo(platformInfo, payload);

    return payload;
}

void OCPlatformPayloadDestroy(OCPlatformPayload* payload)
{
    if(!payload)
    {
        return;
    }
    OICFree(payload->uri);
    OICFree(payload->info.platformID);
    OICFree(payload->info.manufacturerName);
    OICFree(payload->info.manufacturerUrl);
    OICFree(payload->info.modelNumber);
    OICFree(payload->info.dateOfManufacture);
    OICFree(payload->info.platformVersion);
    OICFree(payload->info.operatingSystemVersion);
    OICFree(payload->info.hardwareVersion);
    OICFree(payload->info.firmwareVersion);
    OICFree(payload->info.supportUrl);
    OICFree(payload->info.systemTime);
    OICFree(payload);
}

OCPresencePayload* OCPresencePayloadCreate(uint32_t seqNum, uint32_t maxAge,
        OCPresenceTrigger trigger, const char* resourceType)
{
    OCPresencePayload* payload = (OCPresencePayload*)OICCalloc(1, sizeof(OCPresencePayload));
    if(!payload)
    {
        return NULL;
    }

    payload->type = PAYLOAD_TYPE_PRESENCE;
    payload->sequenceNumber = seqNum;
    payload->maxAge = maxAge;
    payload->trigger = trigger;
    payload->resourceType = OICStrdup(resourceType);
    return payload;
}

void OCPresencePayloadDestroy(OCPresencePayload* payload)
{
    if(!payload)
    {
        return;
    }
    OICFree(payload->resourceType);
    OICFree(payload);
}
