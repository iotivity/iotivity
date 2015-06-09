/******************************************************************
 *
 * Copyright 2014 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#include <string.h>

#include "oic_malloc.h"
#include "oic_string.h"
#include "caremotehandler.h"
#include "logger.h"

#define TAG "CA"

CAEndpoint_t *CACloneEndpoint(const CAEndpoint_t *rep)
{
    if (NULL == rep)
    {
        OIC_LOG(ERROR, TAG, "parameter is null");
        return NULL;
    }

    // allocate the remote end point structure.
    CAEndpoint_t *clone = (CAEndpoint_t *)OICMalloc(sizeof (CAEndpoint_t));
    if (NULL == clone)
    {
        OIC_LOG(ERROR, TAG, "CACloneRemoteEndpoint Out of memory");
        return NULL;
    }
    *clone = *rep;

    return clone;
}

CARequestInfo_t *CACloneRequestInfo(const CARequestInfo_t *rep)
{
    if (NULL == rep)
    {
        OIC_LOG(ERROR, TAG, "parameter is null");
        return NULL;
    }

    // allocate the request info structure.
    CARequestInfo_t *clone = (CARequestInfo_t *) OICMalloc(sizeof(CARequestInfo_t));
    if (!clone)
    {
        OIC_LOG(ERROR, TAG, "CACloneRequestInfo Out of memory");
        return NULL;
    }

    *clone = *rep;

    if (rep->info.token)
    {
        char *temp = NULL;

        // allocate token field
        uint8_t len = rep->info.tokenLength;

        if (len)
        {
            temp = (char *) OICCalloc(len, sizeof(char));
            if (!temp)
            {
                OIC_LOG(ERROR, TAG, "CACloneRequestInfo Out of memory");

                CADestroyRequestInfoInternal(clone);

                return NULL;
            }
            memcpy(temp, rep->info.token, len);
        }

        // save the token
        clone->info.token = temp;
        clone->info.tokenLength = len;
    }

    if (NULL != rep->info.options && 0 < rep->info.numOptions)
    {
        // save the options
        clone->info.options =
            (CAHeaderOption_t *) OICMalloc(sizeof(CAHeaderOption_t) * rep->info.numOptions);
        if (NULL == clone->info.options)
        {
            OIC_LOG(ERROR, TAG, "CACloneRequestInfo Out of memory");
            OICFree(clone->info.token);
            OICFree(clone);
            return NULL;
        }
        memcpy(clone->info.options, rep->info.options,
               sizeof(CAHeaderOption_t) * rep->info.numOptions);
    }
    else
    {
        clone->info.options = NULL;
        clone->info.numOptions = 0;
    }

    if (NULL != rep->info.payload)
    {
        // allocate payload field
        uint8_t *temp = OICMalloc(rep->info.payloadSize);
        if (NULL == temp)
        {
            OIC_LOG(ERROR, TAG, "CACloneRequestInfo Out of memory");

            CADestroyRequestInfoInternal(clone);

            return NULL;
        }
        memcpy(temp, rep->info.payload, rep->info.payloadSize);

        // save the payload
        clone->info.payload = temp;
    }

    return clone;
}

CAResponseInfo_t *CACloneResponseInfo(const CAResponseInfo_t *rep)
{
    if (NULL == rep)
    {
        OIC_LOG(ERROR, TAG, "Response pointer is NULL");
        return NULL;
    }

    // check the result value of response info.
    switch (rep->result)
    {
        case CA_EMPTY:
        case CA_SUCCESS:
        case CA_CREATED:
        case CA_DELETED:
        case CA_BAD_REQ:
        case CA_BAD_OPT:
        case CA_NOT_FOUND:
        case CA_INTERNAL_SERVER_ERROR:
        case CA_RETRANSMIT_TIMEOUT:
            break;

        default:
            OIC_LOG(ERROR, TAG, "Response status code is invalid number");
            return NULL;
    }

    // allocate the response info structure.
    CAResponseInfo_t *clone = (CAResponseInfo_t *) OICCalloc(1, sizeof(CAResponseInfo_t));
    if (NULL == clone)
    {
        OIC_LOG(ERROR, TAG, "CACloneResponseInfo Out of memory");
        return NULL;
    }
    *clone = *rep;

    if (rep->info.token)
    {
        char *temp = NULL;

        // allocate token field
        uint8_t len = rep->info.tokenLength;

        if (len)
        {
            temp = (char *) OICCalloc(len, sizeof(char));
            if (!temp)
            {
                OIC_LOG(ERROR, TAG, "CACloneResponseInfo Out of memory");

                CADestroyResponseInfoInternal(clone);

                return NULL;
            }
            memcpy(temp, rep->info.token, len);
        }
        // save the token
        clone->info.token = temp;
        clone->info.tokenLength = len;
    }

    if (NULL != rep->info.options && rep->info.numOptions)
    {
        // save the options
        clone->info.options =
                (CAHeaderOption_t *) OICMalloc(sizeof(CAHeaderOption_t) * rep->info.numOptions);

        if (NULL == clone->info.options)
        {
            OIC_LOG(ERROR, TAG, "CACloneResponseInfo Out of memory");

            OICFree(clone->info.token);
            OICFree(clone);
            return NULL;
        }
        memcpy(clone->info.options, rep->info.options,
                sizeof(CAHeaderOption_t) * rep->info.numOptions);
    }
    else
    {
        clone->info.options = NULL;
        clone->info.numOptions = 0;
    }

    if (NULL != rep->info.payload)
    {
        // allocate payload field
        uint8_t *temp = (uint8_t *) OICMalloc(rep->info.payloadSize);
        if (NULL == temp)
        {
            OIC_LOG(ERROR, TAG, "CACloneResponseInfo Out of memory");

            CADestroyResponseInfoInternal(clone);

            return NULL;
        }
        memcpy(temp, rep->info.payload, rep->info.payloadSize);

        // save the payload
        clone->info.payload = temp;
    }

    return clone;
}

void CADestroyEndpointInternal(CAEndpoint_t *rep)
{
    OICFree(rep);
}

void CADestroyRequestInfoInternal(CARequestInfo_t *rep)
{
    if (NULL == rep)
    {
        OIC_LOG(ERROR, TAG, "parameter is null");
        return;
    }

    // free token field
    OICFree(rep->info.token);

    // free options field
    OICFree((CAHeaderOption_t *) rep->info.options);

    // free payload field
    OICFree((char *) rep->info.payload);

    OICFree(rep);
}

void CADestroyResponseInfoInternal(CAResponseInfo_t *rep)
{
    if (NULL == rep)
    {
        OIC_LOG(ERROR, TAG, "parameter is null");
        return;
    }

    // free token field
    OICFree(rep->info.token);

    // free options field
    if (rep->info.options != NULL && rep->info.numOptions)
    {
        OICFree((CAHeaderOption_t *) rep->info.options);
    }

    // free payload field
    OICFree((char *) rep->info.payload);

    OICFree(rep);
}

