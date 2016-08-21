//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
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
#include "cloud_connector.h"

#include <stdarg.h>
#include <stdlib.h>

#include "oic_string.h"
#include "oic_malloc.h"

#include "ocpayload.h"

#include "rdpayload.h"

#define OC_RD_PUBLISH_TTL 86400
#define DEFAULT_CONTEXT_VALUE 0x99

#define DEFAULT_COAP_TCP_HOST "coap+tcp://"
#define DEFAULT_COAP_TCP_PORT 5683

#define DEFAULT_COAP_TCP_SECURE_HOST "coaps+tcp://"
#define DEFAULT_COAP_TCP_SECURE_PORT 5864

#define DEFAULT_AUTH_REGISTER_LOGIN "/oic/auth/?reqtype=register"
#define DEFAULT_AUTH_LOGIN "/oic/auth/?reqtype=login"
#define DEFAULT_AUTH_LOGOUT "/oic/auth/?reqtype=logout"

static OCStackResult createStringLL(uint8_t numElements, OCResourceHandle handle,
                                    const char *(*getValue)(OCResourceHandle handle, uint8_t i), OCStringLL **stringLL)
{
    for (uint8_t i = 0; i < numElements; ++i)
    {
        const char *value = getValue(handle, i);
        if (!*stringLL)
        {
            *stringLL = (OCStringLL *)OICCalloc(1, sizeof(OCStringLL));
            if (!*stringLL)
            {
                return OC_STACK_NO_MEMORY;
            }
            (*stringLL)->value = OICStrdup(value);
            if (!(*stringLL)->value)
            {
                return OC_STACK_NO_MEMORY;
            }
        }
        else
        {
            OCStringLL *cur = *stringLL;
            while (cur->next)
            {
                cur = cur->next;
            }
            cur->next = (OCStringLL *)OICCalloc(1, sizeof(OCStringLL));
            if (!cur->next)
            {
                return OC_STACK_NO_MEMORY;
            }
            cur->next->value = OICStrdup(value);
            if (!cur->next->value)
            {
                return OC_STACK_NO_MEMORY;
            }
        }
    }
    return OC_STACK_OK;
}

OCStackResult OCCloudRegisterLogin(const char *host, const char *auth_provider,
                                   const char *auth_code, OCClientResponseHandler response)
{
    char    targetUri[MAX_URI_LENGTH * 2] = { 0, };
    snprintf(targetUri, MAX_URI_LENGTH * 2, "%s%s", host, DEFAULT_AUTH_REGISTER_LOGIN);

    OCCallbackData cbData;
    memset(&cbData, 0, sizeof(OCCallbackData));
    cbData.cb = response;
    cbData.cd = NULL;
    cbData.context = (void *)DEFAULT_CONTEXT_VALUE;

    OCRepPayload *registerPayload = OCRepPayloadCreate();
    if (!registerPayload)
    {
        goto no_memory;
    }

    OCRepPayloadSetPropString(registerPayload, "authprovider", auth_provider);
    OCRepPayloadSetPropString(registerPayload, "authcode", auth_code);

    return OCDoResource(NULL, OC_REST_POST, targetUri, NULL, (OCPayload *)registerPayload,
                        CT_ADAPTER_TCP, OC_LOW_QOS, &cbData, NULL, 0);

no_memory:
    OCRepPayloadDestroy(registerPayload);
    return OC_STACK_NO_MEMORY;
}

OCStackResult OCCloudLoginout(const char *host, const char *query, const char *auth_session,
                              OCClientResponseHandler response)
{
    char    targetUri[MAX_URI_LENGTH * 2] = { 0, };
    snprintf(targetUri, MAX_URI_LENGTH * 2, "%s%s", host, query);

    OCCallbackData cbData;
    memset(&cbData, 0, sizeof(OCCallbackData));
    cbData.cb = response;
    cbData.cd = NULL;
    cbData.context = (void *)DEFAULT_CONTEXT_VALUE;

    OCRepPayload *loginoutPayload = OCRepPayloadCreate();
    if (!loginoutPayload)
    {
        goto no_memory;
    }

    OCRepPayloadSetPropString(loginoutPayload, "session", auth_session);

    return OCDoResource(NULL, OC_REST_POST, targetUri, NULL, (OCPayload *)loginoutPayload,
                        CT_ADAPTER_TCP, OC_LOW_QOS, &cbData, NULL, 0);

no_memory:
    OCRepPayloadDestroy(loginoutPayload);
    return OC_STACK_NO_MEMORY;
}


OCStackResult OCCloudLogin(const char *host, const char *auth_session,
                           OCClientResponseHandler response)
{
    return OCCloudLoginout(host, DEFAULT_AUTH_LOGIN, auth_session, response);
}

OCStackResult OCCloudLogout(const char *host, const char *auth_session,
                            OCClientResponseHandler response)
{
    return OCCloudLoginout(host, DEFAULT_AUTH_LOGOUT, auth_session, response);
}
