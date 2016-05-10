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


#include "octypes.h"
#include <string.h>
#include "ocadaptation.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "ocresource.h"
#include "ocserverrequest.h"
#include "ocstackinternal.h"
#include "oclogger.h"
#include "ocpayload.h"
#include "ocpayloadint.h"

#define TAG "OCPayload"

static OCPayload *plCreate(OCPayloadType type, size_t size);

static OCPayload *currentPayload;
static char *currentMemory = NULL;
static size_t currentRemaining = 0;

void OCPayloadDestroy(void *payload)
{
    OCFree(payload, MAT_payload);
}

OCRepPayload* OCRepPayloadCreate()
{
    return (OCRepPayload*)plCreate(PAYLOAD_TYPE_REPRESENTATION, sizeof (OCRepPayload));
}

void OCRepPayloadAppend(OCRepPayload* parent, OCRepPayload* child)
{
    for (; parent->next; parent = parent->next)
    {
    }

    parent->next= child;
    child->next = NULL;
}

#ifdef FULL_REPRESENTATION
static OCRepPayloadValue* OCRepPayloadFindValue(const OCRepPayload* payload, const char* name)
{
    if(!payload || !name)
    {
        return NULL;
    }

    for (OCRepPayloadValue* val = payload->values; val;val = val->next)
    {
        if (!strcmp(val->name, name))
        {
            return val;
        }
    }

    return NULL;

}
#endif // FULL_REPRESENTATION

static OCRepPayloadValue* OCRepPayloadFindAndSetValue(OCRepPayload* payload, const char* name,
        OCRepPayloadPropType type)
{
    OCRepPayloadValue* val = payload->values;
    if (val == NULL)
    {
        payload->values = (OCRepPayloadValue*)plCalloc(1, sizeof(OCRepPayloadValue));
        if (!payload->values)
        {
            return NULL;
        }
        payload->values->name = plStrdup(name);
        if (!payload->values->name)
        {
            payload->values = NULL;
            return NULL;
        }
        payload->values->type =type;
        return payload->values;
    }

    for (; val; val = val->next)
    {
        if (0 == strcmp(val->name, name))
        {
            val->type = type;
            return val;
        }

        if (val->next == NULL)
        {
            val->next = (OCRepPayloadValue*)plCalloc(1, sizeof(OCRepPayloadValue));
            if(!val->next)
            {
                return NULL;
            }
            val->next->name = plStrdup(name);
            if(!val->next->name)
            {
                val->next = NULL;
                return NULL;
            }
            val->next->type =type;
            return val->next;
        }
    }

    OC_LOG(ERROR, TAG, "FindAndSetValue reached point after while loop, pointer corruption?");
    return NULL;
}

bool OCRepPayloadAddResourceType(OCRepPayload* payload, const char* resourceType)
{
    return OCRepPayloadAddResourceTypeAsOwner(payload, plStrdup(resourceType));
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
        cur->next = (OCStringLL*)plCalloc(1, sizeof(OCStringLL));

        if(!cur->next)
        {
            return false;
        }

        cur->next->value = resourceType;
        return true;
    }
    else
    {
        payload->types = (OCStringLL*)plCalloc(1, sizeof(OCStringLL));
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
    return OCRepPayloadAddInterfaceAsOwner(payload, plStrdup(interface));
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
        cur->next = (OCStringLL*)plCalloc(1, sizeof(OCStringLL));

        if(!cur->next)
        {
            return false;
        }
        cur->next->value = interface;
        return true;
    }
    else
    {
        payload->interfaces = (OCStringLL*)plCalloc(1, sizeof(OCStringLL));
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

    payload->uri = plStrdup(uri);
    return payload->uri != NULL;
}

#ifdef FULL_REPRESENTATION
bool OCRepPayloadIsNull(const OCRepPayload* payload, const char* name)
{
    OCRepPayloadValue* val = OCRepPayloadFindValue(payload, name);

    if(!val)
    {
        return false;
    }

    return val->type == OCREP_PROP_NULL;
}

#endif // FULL_REPRESENTATION

static bool OCRepPayloadSetProp(OCRepPayload* payload, const char *name,
        const void *value, OCRepPayloadPropType type)
{
    OCRepPayloadValue* val = OCRepPayloadFindAndSetValue(payload, name, type);
    if(!val)
    {
        return false;
    }
    switch(type)
    {
        case OCREP_PROP_INT:
               val->i = *(int64_t*)value;
               break;
        case OCREP_PROP_DOUBLE:
               val->d = *(double*)value;
               break;
        case OCREP_PROP_BOOL:
               val->b = *(bool*)value;
               break;
        case OCREP_PROP_OBJECT:
               val->obj = (OCRepPayload*)value;
               break;
        case OCREP_PROP_STRING:
               val->str = (char*)value;
               return val->str != NULL;
        case OCREP_PROP_NULL:
               return val != NULL;
        case OCREP_PROP_ARRAY:
        default:
               return false;
    }

    return true;
}

bool OCRepPayloadSetNull(OCRepPayload* payload, const char* name)
{
    return OCRepPayloadSetProp(payload, name, NULL, OCREP_PROP_NULL);
}

bool OCRepPayloadSetPropInt(OCRepPayload* payload,
        const char* name, int64_t value)
{
    return OCRepPayloadSetProp(payload, name, &value, OCREP_PROP_INT);
}

#ifdef FULL_REPRESENTATION

bool OCRepPayloadGetPropInt(const OCRepPayload* payload, const char* name, int64_t* value)
{
    OCRepPayloadValue* val = OCRepPayloadFindValue(payload, name);

    if(!val || val->type != OCREP_PROP_INT)
    {
        return false;
    }

    *value = val->i;
    return true;
}
#endif // FULL_REPRESENTATION

bool OCRepPayloadSetPropDouble(OCRepPayload* payload,
        const char* name, double value)
{
    return OCRepPayloadSetProp(payload, name, &value, OCREP_PROP_DOUBLE);
}

#ifdef FULL_REPRESENTATION
bool OCRepPayloadGetPropDouble(const OCRepPayload* payload, const char* name, double* value)
{
    OCRepPayloadValue* val = OCRepPayloadFindValue(payload, name);

    if(!val || val->type != OCREP_PROP_DOUBLE)
    {
        return false;
    }

    *value = val->d;
    return true;
}
#endif // FULL_REPRESENTATION

bool OCRepPayloadSetPropString(OCRepPayload* payload, const char* name, const char* value)
{
    char* temp = plStrdup(value);
    return OCRepPayloadSetPropStringAsOwner(payload, name, temp);
}

bool OCRepPayloadSetPropStringAsOwner(OCRepPayload* payload, const char* name, char* value)
{
    return OCRepPayloadSetProp(payload, name, value, OCREP_PROP_STRING);
}

#ifdef FULL_REPRESENTATION
bool OCRepPayloadGetPropString(const OCRepPayload* payload, const char* name, char** value)
{
    OCRepPayloadValue* val = OCRepPayloadFindValue(payload, name);

    if(!val || val->type != OCREP_PROP_STRING)
    {
        return false;
    }

    *value = plStrdup(val->str);
    return *value != NULL;
}
#endif // FULL_REPRESENTATION

bool OCRepPayloadSetPropBool(OCRepPayload* payload,
        const char* name, bool value)
{
    return OCRepPayloadSetProp(payload, name, &value, OCREP_PROP_BOOL);
}

#ifdef FULL_REPRESENTATION
bool OCRepPayloadGetPropBool(const OCRepPayload* payload, const char* name, bool* value)
{
    OCRepPayloadValue* val = OCRepPayloadFindValue(payload, name);

    if(!val || val->type != OCREP_PROP_BOOL)
    {
        return false;
    }

    *value = val->b;
    return true;
}
#endif // FULL_REPRESENTATION

#ifdef NESTED_PAYLOADS
bool OCRepPayloadSetPropObject(OCRepPayload* payload, const char* name, const OCRepPayload* value)
{
    return OCRepPayloadSetPropObjectAsOwner(payload, name, value);
}

bool OCRepPayloadSetPropObjectAsOwner(OCRepPayload* payload, const char* name, const OCRepPayload* value)
{
    return OCRepPayloadSetProp(payload, name, value, OCREP_PROP_OBJECT);
}
#endif // NESTED_PAYLOADS

#ifdef FULL_REPRESENTATION
#ifdef NESTED_PAYLOADS
bool OCRepPayloadGetPropObject(const OCRepPayload* payload, const char* name, OCRepPayload** value)
{
    OCRepPayloadValue* val = OCRepPayloadFindValue(payload, name);

    if(!val || val->type != OCREP_PROP_OBJECT)
    {
        return false;
    }

    *value = val->obj;
    return *value != NULL;
}
#endif // NESTED_PAYLOADS

#endif // FULL_REPRESENTATION

void OCFreeOCStringLL(OCStringLL* ll)
{
    if (!ll)
    {
        return;
    }

    OCFreeOCStringLL(ll->next);
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

    OCStringLL *destIter = (OCStringLL*)plCalloc (1, sizeof (OCStringLL));
    if (!destIter)
    {
        return NULL;
    }
    destIter->value = plStrdup (sourceIter->value);

    OCStringLL *headOfClone = destIter;

    sourceIter = sourceIter->next;

    while (sourceIter)
    {
        destIter->next  = (OCStringLL*)plCalloc (1, sizeof (OCStringLL));
        if (!destIter->next)
        {
            return NULL;
        }
        destIter->next->value = plStrdup (sourceIter->value);

        destIter = destIter->next;
        sourceIter = sourceIter->next;
    }
    return headOfClone;
}

#ifdef FULL_REPRESENTATION
#ifdef WITH_CLIENT

OCDiscoveryPayload* OCDiscoveryPayloadCreate()
{
    return (OCDiscoveryPayload *)plCreate(PAYLOAD_TYPE_DISCOVERY, sizeof(OCDiscoveryPayload));
}

size_t OCDiscoveryPayloadGetResourceCount(OCDiscoveryPayload* payload)
{
    size_t i = 0;
    for (OCResourcePayload* p = payload->resources; p; p = p->next)
    {
        ++i;
    }
    return i;
}

OCResourcePayload* OCDiscoveryPayloadGetResource(OCDiscoveryPayload* payload, size_t index)
{
    size_t i = 0;
    for (OCResourcePayload* p = payload->resources; p; p = p->next)
    {
        if (i++ == index)
        {
            return p;
        }
    }
    return NULL;
}

bool OCResourcePayloadAddResourceType(OCResourcePayload* payload, const char* resourceType)
{
    char* dup = plStrdup(resourceType);
    if (!dup)
    {
        return false;
    }

    if (!payload->types)
    {
        payload->types = (OCStringLL*)plCalloc(1, sizeof (OCStringLL));

        if (!payload->types)
        {
            return false;
        }

        payload->types->value = dup;
        return true;
    }

    else
    {
        OCStringLL* temp = payload->types;

        while(temp->next)
        {
            temp = temp->next;
        }

        temp->next = (OCStringLL*)plCalloc(1, sizeof (OCStringLL));
        if (!temp->next)
        {
            return false;
        }

        temp->next->value = dup;
        return true;
    }
}

bool OCResourcePayloadAddInterface(OCResourcePayload* payload, const char* interface)
{
    char* dup = plStrdup(interface);
    if (!dup)
    {
        return false;
    }

    if (!payload->interfaces)
    {
        payload->interfaces = (OCStringLL*)plCalloc(1, sizeof(OCStringLL));
        if (!payload->interfaces)
        {
            return false;
        }

        payload->interfaces->value = dup;
        return true;
    }

    else
    {
        OCStringLL* temp = payload->interfaces;

        while(temp->next)
        {
            temp = temp->next;
        }

        temp->next = (OCStringLL*)plCalloc(1, sizeof(OCStringLL));
        if (!temp->next)
        {
            return false;
        }

        temp->next->value = dup;
        return true;
    }
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

OCDevicePayload* OCDevicePayloadCreate(const char* uri, const uint8_t* sid, const char* dname,
        const char* specVer, const char* dmVer)
{
    OCDevicePayload* payload = (OCDevicePayload *)plCreate(PAYLOAD_TYPE_DEVICE,
                                                      sizeof (OCDevicePayload));
    payload->uri = plStrdup(uri);
    if (uri && !payload->uri)
    {
        goto exit;
    }

    if(sid)
    {
        payload->sid = (uint8_t*)plMalloc(UUID_SIZE);
        if(!payload->sid)
        {
            goto exit;
        }
        memcpy(payload->sid, sid, UUID_SIZE);
    }

    payload->deviceName = plStrdup(dname);
    if(dname && !payload->deviceName)
    {
        goto exit;
    }

    payload->specVersion = plStrdup(specVer);
    if(specVer && !payload->specVersion)
    {
        goto exit;
    }

    payload->dataModelVersion = plStrdup(dmVer);
    if(dmVer && !payload->dataModelVersion)
    {
        goto exit;
    }

    return payload;

exit:
    OCPayloadDestroy(payload);
    return NULL;
}

OCPlatformPayload* OCPlatformPayloadCreateAsOwner(char* uri, OCPlatformInfo* platformInfo)
{
    OCPlatformPayload *payload = (OCPlatformPayload *)plCreate(
                            PAYLOAD_TYPE_PLATFORM, sizeof (OCPlatformPayload));

    payload->uri = uri;
    payload->info = *platformInfo;

    return payload;
}

#ifdef WITH_PRESENCE
OCPresencePayload* OCPresencePayloadCreate(uint32_t seqNum, uint32_t maxAge,
        OCPresenceTrigger trigger, const char* resourceType)
{
    OCPresencePayload* payload = (OCPresencePayload*)plCreate(
                              PAYLOAD_TYPE_PRESENCE, sizeof(OCPresencePayload));

    payload->sequenceNumber = seqNum;
    payload->maxAge = maxAge;
    payload->trigger = trigger;
    payload->resourceType = plStrdup(resourceType);
    return payload;
}

#endif // WITH_PRESENCE
#endif // WITH_CLIENT

#endif // FULL_REPRESENTATION

OCSecurityPayload* OCSecurityPayloadCreate(const char* securityData)
{
    OCSecurityPayload* payload = (OCSecurityPayload*)plCreate(
                              PAYLOAD_TYPE_SECURITY, sizeof(OCSecurityPayload));

    payload->securityData = plStrdup(securityData);

    return payload;
}

static OCPayload *plCreate(OCPayloadType type, size_t size)
{
    OCPayload* pl = (OCPayload *)OCCalloc(MAT_payload);
    pl->type = type;
    pl->bo = size;
    currentPayload = pl;
    currentMemory = (char *)pl + size;
    currentRemaining = PAYLOAD_STRUCT_SIZE - size;
    return pl;
}

void *plMalloc(size_t size)
{
    if (size > currentRemaining)
    {
        currentRemaining = -1;
        return NULL;
    }
    char *next = currentMemory;
    currentMemory += size;
    currentRemaining -= size;
    return next;
}

void *plCalloc(size_t nmemb, size_t size)
{
    size_t total = nmemb * size;
    void *p = plMalloc(total);
    if (p)
    {
        memset(p, 0, total);
    }
    return p;
}

char *plStrdup(const char *str)
{
    size_t len = strlen(str);
    char *newstr = plMalloc(len + 1);
    memcpy(newstr, str, len + 1);
    return newstr;
}

OCStackResult OCFinishPayload(OCPayload *payload)
{
    OCStackResult result = OC_STACK_OK;
    if (currentPayload != payload || currentRemaining == (size_t)-1)
    {
        result = OC_STACK_ERROR;
    }
    currentPayload = NULL;
    currentMemory = NULL;
    currentRemaining = 0;
    return result;
}
