/*******************************************************************
 *
 * Copyright (c) 2018 LG Electronics, Inc.
 * Copyright 2015 Intel Mobile Communications GmbH All Rights Reserved.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

#ifndef SAMPLE_PAYLOAD_LOGGING_H_
#define SAMPLE_PAYLOAD_LOGGING_H_

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif

#include "experimental/logger.h"
#include "oic_malloc.h"
#include "ocpayload.h"
#include "ocstack.h"

#include "logging.h"

#ifdef __cplusplus
extern "C"
{
#endif

// PL_TAG is made as generic predefined tag because of build problems in arduino for using logging
#define PL_TAG "PayloadLog"

#ifdef TB_LOG
    #define OCSAMPLE_LOG_PAYLOAD(level, payload) OCPayloadLog((level),(payload))
    #define UUID_SIZE (16)

const char *OC_CALL convertTriggerEnumToString(OCPresenceTrigger trigger);
OCPresenceTrigger OC_CALL convertTriggerStringToEnum(const char * triggerStr);

INLINE_API void OCPayloadLogRep(LogLevel level, OCRepPayload* payload);

INLINE_API void OCPayloadLogRepValues(LogLevel level, OCRepPayloadValue* val)
{
    while (val)
    {
        switch(val->type)
        {
            case OCREP_PROP_NULL:
                OCSAMPLE_LOG_INFO(PL_TAG, 0, "        %s: NULL", val->name);
                break;
            case OCREP_PROP_INT:
                OCSAMPLE_LOG_INFO(PL_TAG, 0, "        %s(int):%" PRId64, val->name, val->i);
                break;
            case OCREP_PROP_DOUBLE:
                OCSAMPLE_LOG_INFO(PL_TAG, 0, "        %s(double):%f", val->name, val->d);
                break;
            case OCREP_PROP_BOOL:
                OCSAMPLE_LOG_INFO(PL_TAG, 0, "        %s(bool):%s", val->name, val->b ? "true" : "false");
                break;
            case OCREP_PROP_STRING:
                OCSAMPLE_LOG_INFO(PL_TAG, 0, "        %s(string):%s", val->name, val->str);
                break;
            case OCREP_PROP_BYTE_STRING:
                OCSAMPLE_LOG_INFO(PL_TAG, 0, "        %s(binary):", val->name);
                OIC_LOG_BUFFER(level, PL_TAG, val->ocByteStr.bytes, val->ocByteStr.len);
                break;
            case OCREP_PROP_OBJECT:
                // Note: Only prints the URI (if available), to print further, you'll
                // need to dig into the object better!
                OCSAMPLE_LOG_INFO(PL_TAG, 0, "        %s(object):", val->name);
                OCPayloadLogRep(level, val->obj);
                break;
            case OCREP_PROP_ARRAY:
                switch(val->arr.type)
                {
                    case OCREP_PROP_INT:
                        OCSAMPLE_LOG_INFO(PL_TAG, 0, "        %s(int array):%" PRIuPTR " x %" PRIuPTR " x %" PRIuPTR ": ",
                                val->name,
                                val->arr.dimensions[0], val->arr.dimensions[1],
                                val->arr.dimensions[2]);
                        OCSAMPLE_LOG_INFO(PL_TAG, 0,  "         Values:");
                        for (size_t i = 0; i < val->arr.dimensions[0]; i++)
                        {
                            OCSAMPLE_LOG_INFO(PL_TAG, 0, "             %" PRId64, val->arr.iArray[i]);
                        }
                        break;
                    case OCREP_PROP_DOUBLE:
                        OCSAMPLE_LOG_INFO(PL_TAG, 0, "        %s(double array):%" PRIuPTR " x %" PRIuPTR " x %" PRIuPTR ": ",
                                val->name,
                                val->arr.dimensions[0], val->arr.dimensions[1],
                                val->arr.dimensions[2]);
                        OCSAMPLE_LOG_INFO(PL_TAG, 0,  "         Values:");
                        for (size_t i = 0; i < val->arr.dimensions[0]; i++)
                        {
                            OCSAMPLE_LOG_INFO(PL_TAG, 0, "             %lf", val->arr.dArray[i]);
                        }
                        break;
                    case OCREP_PROP_BOOL:
                        OCSAMPLE_LOG_INFO(PL_TAG, 0, "        %s(bool array):%" PRIuPTR " x %" PRIuPTR " x %" PRIuPTR ": ",
                                val->name,
                                val->arr.dimensions[0], val->arr.dimensions[1],
                                val->arr.dimensions[2]);
                        OCSAMPLE_LOG_INFO(PL_TAG, 0,  "         Values:");
                        for (size_t i = 0; i < val->arr.dimensions[0]; i++)
                        {
                            OCSAMPLE_LOG_INFO(PL_TAG, 0, "             %d", val->arr.bArray[i]);
                        }
                        break;
                    case OCREP_PROP_STRING:
                        OCSAMPLE_LOG_INFO(PL_TAG, 0, "        %s(string array):%" PRIuPTR " x %" PRIuPTR " x %" PRIuPTR ": ",
                                val->name,
                                val->arr.dimensions[0], val->arr.dimensions[1],
                                val->arr.dimensions[2]);
                        OCSAMPLE_LOG_INFO(PL_TAG, 0,  "         Values:");
                        for (size_t i = 0; i < val->arr.dimensions[0]; i++)
                        {
                            OCSAMPLE_LOG_INFO(PL_TAG, 0, "             %s", val->arr.strArray[i]);
                        }
                        break;
                    case OCREP_PROP_BYTE_STRING:
                        OCSAMPLE_LOG_INFO(PL_TAG, 0, "        %s(byte array):%" PRIuPTR " x %" PRIuPTR " x %" PRIuPTR ": ",
                                val->name,
                                val->arr.dimensions[0], val->arr.dimensions[1],
                                val->arr.dimensions[2]);
                        OCSAMPLE_LOG_INFO(PL_TAG, 0,  "         Values:");
                        for (size_t i = 0; i < val->arr.dimensions[0]; i++)
                        {
                            OIC_LOG_BUFFER(level, PL_TAG, val->arr.ocByteStrArray[i].bytes, val->arr.ocByteStrArray[i].len);
                        }
                        break;
                    case OCREP_PROP_OBJECT:
                        OCSAMPLE_LOG_INFO(PL_TAG, 0, "        %s(object array):%" PRIuPTR " x %" PRIuPTR " x %" PRIuPTR ": ",
                                val->name,
                                val->arr.dimensions[0], val->arr.dimensions[1],
                                val->arr.dimensions[2]);
                        OCSAMPLE_LOG_INFO(PL_TAG, 0,  "         Values:");

                        for (size_t i = 0; i < val->arr.dimensions[0]; i++)
                        {
                            OCPayloadLogRep(level, val->arr.objArray[i]);
                        }
                        break;
                    case OCREP_PROP_ARRAY: //Seems as nested arrays doesn't not supported in API
                    default:
                        OCSAMPLE_LOG_ERROR(PL_TAG, 0, "%s <-- Unknown/unsupported array type!",
                                val->name);
                        break;
                }
                break;
            default:
                OCSAMPLE_LOG_ERROR(PL_TAG, 0, "%s <-- Unknown type!", val->name);
                break;
        }
        val = val -> next;
    }
}

INLINE_API void OCPayloadLogRep(LogLevel level, OCRepPayload* payload)
{
    OCSAMPLE_LOG_INFO(PL_TAG, 0, "Payload Type: Representation");
    uint32_t i = 1;
    for (OCRepPayload* rep = payload; rep; rep = rep->next, ++i)
    {
        OCSAMPLE_LOG_INFO(PL_TAG, 0, "    Resource #%d", i);
        if (rep->uri)
        {
            OCSAMPLE_LOG_INFO(PL_TAG, 0, "    URI:%s", rep->uri);
        }
        if (rep->types)
        {
            OCSAMPLE_LOG_INFO(PL_TAG, 0,  "    Resource Types:");
            for (OCStringLL* strll = rep->types; strll; strll = strll->next)
            {
                OCSAMPLE_LOG_INFO(PL_TAG, 0, "        %s", strll->value);
            }
        }
        if (rep->interfaces)
        {
            OCSAMPLE_LOG_INFO(PL_TAG, 0,  "    Interfaces:");
            for (OCStringLL* strll = rep->interfaces; strll; strll = strll->next)
            {
                OCSAMPLE_LOG_INFO(PL_TAG, 0, "        %s", strll->value);
            }
        }
        OCSAMPLE_LOG_INFO(PL_TAG, 0,  "    Values:");
        OCPayloadLogRepValues(level, rep->values);
    }
}

static void OCStringLLPrint(LogLevel level, OCStringLL *type)
{
    for (OCStringLL *strll = type; strll; strll = strll->next)
    {
        OCSAMPLE_LOG_INFO(PL_TAG, 0, "         %s", strll->value);
    }
}

INLINE_API void OCPayloadLogDiscovery(LogLevel level, OCDiscoveryPayload* payload)
{
    OCSAMPLE_LOG_INFO(PL_TAG, 0,  "Payload Type: Discovery");

    while(payload && payload->resources)
    {
        OCSAMPLE_LOG_INFO(PL_TAG, 0, "    DI: %s", payload->sid);
        if (payload->name)
        {
            OCSAMPLE_LOG_INFO(PL_TAG, 0, "    NAME: %s", payload->name);
        }

        if (payload->type)
        {
            OCSAMPLE_LOG_INFO(PL_TAG, 0,  "    Resource Type:");
            OCStringLLPrint(level, payload->type);
        }

        if (payload->iface)
        {
            OCSAMPLE_LOG_INFO(PL_TAG, 0,  "    Interface:");
            OCStringLLPrint(level, payload->iface);
        }

        OCResourcePayload* res = payload->resources;

        uint32_t i = 1;
        while(res)
        {
            OCSAMPLE_LOG_INFO(PL_TAG, 0, "    Link#%d", i);
            OCSAMPLE_LOG_INFO(PL_TAG, 0, "    URI:%s", res->uri);
            if (res->rel)
            {
                OCSAMPLE_LOG_INFO(PL_TAG, 0, "    Relation:%s", res->rel);
            }
            if (res->anchor)
            {
                OCSAMPLE_LOG_INFO(PL_TAG, 0, "    Anchor:%s", res->anchor);
            }
            OCSAMPLE_LOG_INFO(PL_TAG, 0,  "    Resource Types:");
            OCStringLL* strll =  res->types;
            while(strll)
            {
                OCSAMPLE_LOG_INFO(PL_TAG, 0, "        %s", strll->value);
                strll = strll->next;
            }
            OCSAMPLE_LOG_INFO(PL_TAG, 0,  "    Interfaces:");
            strll =  res->interfaces;
            while(strll)
            {
                OCSAMPLE_LOG_INFO(PL_TAG, 0, "        %s", strll->value);
                strll = strll->next;
            }

            OCSAMPLE_LOG_INFO(PL_TAG, 0, "    Bitmap: %u", res->bitmap);
            OCSAMPLE_LOG_INFO(PL_TAG, 0, "    Secure?: %s", res->secure ? "true" : "false");
            OCSAMPLE_LOG_INFO(PL_TAG, 0, "    Port: %u", res->port);

            uint32_t j = 1;
            OCEndpointPayload* eps = res->eps;
            while (eps)
            {
                OCSAMPLE_LOG_INFO(PL_TAG, 0, "    Endpoint #%d", j);
                OCSAMPLE_LOG_INFO(PL_TAG, 0, "        tps: %s", eps->tps);
                OCSAMPLE_LOG_INFO(PL_TAG, 0, "        addr: %s", eps->addr);
                OCSAMPLE_LOG_INFO(PL_TAG, 0, "        port: %d", eps->port);
                OCSAMPLE_LOG_INFO(PL_TAG, 0, "        pri: %d", eps->pri);
                eps = eps->next;
                ++j;
            }

            OCSAMPLE_LOG_INFO(PL_TAG, 0,  "");
            res = res->next;
            ++i;
        }
        payload = payload->next;
    }
}

INLINE_API void OCPayloadLogPresence(LogLevel level, OCPresencePayload* payload)
{
    OCSAMPLE_LOG_INFO(PL_TAG, 0,  "Payload Type: Presence");
    OCSAMPLE_LOG_INFO(PL_TAG, 0, "    Sequence Number:%u", payload->sequenceNumber);
    OCSAMPLE_LOG_INFO(PL_TAG, 0, "    Max Age:%d", payload->maxAge);
    OCSAMPLE_LOG_INFO(PL_TAG, 0, "    Trigger:%s", convertTriggerEnumToString(payload->trigger));
    OCSAMPLE_LOG_INFO(PL_TAG, 0, "    Resource Type:%s", payload->resourceType);
}

INLINE_API void OCPayloadLogSecurity(LogLevel level, OCSecurityPayload* payload)
{
    size_t payloadSize = payload->payloadSize;
    OCSAMPLE_LOG_INFO(PL_TAG, 0,  "Payload Type: Security");

    if (payloadSize > 0)
    {
        // Add a zero-character string terminator.
        char *securityData = (char *)OICMalloc(payloadSize + 1);

        if (securityData)
        {
            memcpy(securityData, payload->securityData, payloadSize);
            // assert(securityData[payloadSize - 1] != '\0');
            securityData[payloadSize] = '\0';
            OCSAMPLE_LOG_INFO(PL_TAG, 0, "    Security Data: %s", securityData);
            OICFree(securityData);
        }
    }
}

INLINE_API void OCPayloadLog(LogLevel level, OCPayload* payload)
{
    if(!payload)
    {
        OCSAMPLE_LOG_INFO(PL_TAG, 0,  "NULL Payload");
        return;
    }
    switch(payload->type)
    {
        case PAYLOAD_TYPE_REPRESENTATION:
            OCPayloadLogRep(level, (OCRepPayload*)payload);
            break;
        case PAYLOAD_TYPE_DISCOVERY:
            OCPayloadLogDiscovery(level, (OCDiscoveryPayload*)payload);
            break;
        case PAYLOAD_TYPE_PRESENCE:
            OCPayloadLogPresence(level, (OCPresencePayload*)payload);
            break;
        case PAYLOAD_TYPE_SECURITY:
            OCPayloadLogSecurity(level, (OCSecurityPayload*)payload);
            break;
        default:
            OCSAMPLE_LOG_INFO(PL_TAG, 0, "Unknown Payload Type: %d", payload->type);
            break;
    }
}
#else
    #define OCSAMPLE_LOG_PAYLOAD(level, payload)
#endif

#ifdef __cplusplus
}
#endif

#endif
