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
#include "caremotehandler.h"
#include "logger.h"

#define TAG "CA"

CARemoteEndpoint_t *CACloneRemoteEndpoint(const CARemoteEndpoint_t *rep)
{
    if (NULL == rep)
    {
        OIC_LOG(ERROR, TAG, "parameter is null");
        return NULL;
    }

    // allocate the remote end point structure.
    CARemoteEndpoint_t *clone = (CARemoteEndpoint_t *) OICMalloc(sizeof(CARemoteEndpoint_t));
    if (NULL == clone)
    {
        OIC_LOG(ERROR, TAG, "CACloneRemoteEndpoint Out of memory");
        return NULL;
    }
    memcpy(clone, rep, sizeof(CARemoteEndpoint_t));

    if (NULL != rep->resourceUri)
    {
        // allocate reference uri field
        size_t len = strlen(rep->resourceUri);

        char *temp = (char *) OICCalloc(len + 1, sizeof(char));
        if (NULL == temp)
        {
            OIC_LOG(ERROR, TAG, "CACloneRemoteEndpoint Out of memory");

            CADestroyRemoteEndpointInternal(clone);

            return NULL;
        }

        strncpy(temp, rep->resourceUri, len);

        // save the uri
        clone->resourceUri = temp;
    }

    return clone;
}

#define COAP_PREFIX         "coap://"
#define COAP_PREFIX_LEN     7
#define COAPS_PREFIX         "coaps://"
#define COAPS_PREFIX_LEN     8


// return 1 : ip
// return 0 : mac
static int32_t getCAAddress(const char *pAddress, CAAddress_t *outAddress)
{
    if (NULL == pAddress || NULL == outAddress)
    {
        OIC_LOG(ERROR, TAG, "parameter is null");
        return -1;
    }

    // simple parse, it will be change.
    // 10.11.12.13:4545 (ip)
    // 10:11:12:13:45:45 (mac)

    int32_t len = strlen(pAddress);

    int32_t isIp = 0;
    int32_t ipLen = 0;

    int i = 0;
    for (i = 0; i < len; i++)
    {
        if (pAddress[i] == '.')
        {
            isIp = 1;
        }

        // found port number start index
        if (isIp && pAddress[i] == ':')
        {
            ipLen = i;
            break;
        }
    }

    if (isIp)
    {
        if(ipLen && ipLen < sizeof(outAddress->IP.ipAddress))
        {
            strncpy(outAddress->IP.ipAddress, pAddress, ipLen);
            outAddress->IP.ipAddress[ipLen] = '\0';
        }
        else if (!ipLen && len < sizeof(outAddress->IP.ipAddress))
        {
            strncpy(outAddress->IP.ipAddress, pAddress, len);
            outAddress->IP.ipAddress[len] = '\0';
        }
        else
        {
            OIC_LOG_V(ERROR, TAG, "IP Address too long: %d", ipLen==0 ? len : ipLen);
            return -1;
        }


        if (ipLen > 0)
        {
            outAddress->IP.port = atoi(pAddress + ipLen + 1);
        }

        OIC_LOG_V(DEBUG, TAG, "ip: %s, port: %d", outAddress->IP.ipAddress, outAddress->IP.port);
    }
    else
    {
        strncpy(outAddress->BT.btMacAddress, pAddress, CA_MACADDR_SIZE - 1);

        OIC_LOG_V(DEBUG, TAG, "mac address : %s", outAddress->BT.btMacAddress);
    }

    return isIp;
}

CARemoteEndpoint_t *CACreateRemoteEndpointUriInternal(const CAURI_t uri,
                                                      const CATransportType_t transportType)
{
    // support URI type
    // coap://10.11.12.13:4545/resource_uri
    // coap://10:11:12:13:45:45/resource_uri

    if (NULL == uri)
    {
        OIC_LOG(ERROR, TAG, "parameter is null");
        return NULL;
    }

    // parse uri
    // #1. check prefix
    int startIndex = 0;
    bool secured = false;
    if (strncmp(COAP_PREFIX, uri, COAP_PREFIX_LEN) == 0)
    {
        OIC_LOG_V(DEBUG, TAG, "uri has '%s' prefix.", COAP_PREFIX);
        startIndex = COAP_PREFIX_LEN;
    }

    if (strncmp(COAPS_PREFIX, uri, COAPS_PREFIX_LEN) == 0)
    {
        OIC_LOG_V(DEBUG, TAG, "uri has '%s' prefix.", COAPS_PREFIX);
        startIndex = COAPS_PREFIX_LEN;
        secured = true;
    }

    // #2. copy uri for parse
    int32_t len = strlen(uri) - startIndex;

    if (len <= 0)
    {
        OIC_LOG(ERROR, TAG, "uri length is 0!");
        return NULL;
    }

    char *cloneUri = (char *) OICCalloc(len + 1, sizeof(char));
    if (NULL == cloneUri)
    {
        OIC_LOG(ERROR, TAG, "CACreateRemoteEndpointUriInternal Out of memory");
        return NULL;
    }

    memcpy(cloneUri, &uri[startIndex], sizeof(char) * len);
    cloneUri[len] = '\0';

    // #3. parse address
    // #4. parse resource uri
    char *pAddress = cloneUri;
    char *pResourceUri = NULL;

    int32_t i = 0;
    for (i = 0; i < len; i++)
    {
        if (cloneUri[i] == '/')
        {
            // separate
            cloneUri[i] = 0;

            pResourceUri = &cloneUri[i + 1];

            break;
        }

    }

    OIC_LOG_V(DEBUG, TAG, "pAddress : %s", pAddress);

    OIC_LOG_V(DEBUG, TAG, "pResourceUri : %s", pResourceUri == NULL ? "" : pResourceUri);

    // address
    CAAddress_t address = {};

    int resType = getCAAddress(pAddress, &address);
    if (resType == -1)
    {
        OIC_LOG(DEBUG, TAG, "address parse error");

        OICFree(cloneUri);
        return NULL;
    }

    // resource uri
    CAURI_t resourceUri = pResourceUri;

    CARemoteEndpoint_t *remoteEndpoint = CACreateRemoteEndpointInternal(resourceUri, address,
                                                                        transportType);
    if (NULL == remoteEndpoint)
    {
        OIC_LOG(ERROR, TAG, "create remote endpoint fail");

        OICFree(cloneUri);
        return NULL;
    }
    remoteEndpoint->isSecured = secured;

    OICFree(cloneUri);

    OIC_LOG_V(DEBUG, TAG, "Remote endpoint successfully created [%d]!", remoteEndpoint->isSecured);
    return remoteEndpoint;
}

CARemoteEndpoint_t *CACreateRemoteEndpointInternal(const CAURI_t resourceUri,
                                                   const CAAddress_t addr,
                                                   const CATransportType_t type)
{
    if (NULL == resourceUri)
    {
        OIC_LOG(ERROR, TAG, "uri is null value");
        return NULL;
    }

    // allocate the remote end point structure.
    CARemoteEndpoint_t *rep = (CARemoteEndpoint_t *) OICCalloc(1, sizeof(CARemoteEndpoint_t));

    if (NULL == rep)
    {
        OIC_LOG(ERROR, TAG, "CACreateRemoteEndpointInternal of memory");
        return NULL;
    }

    // allocate reference uri field
    size_t len = strlen(resourceUri);

    char *temp = (char *) OICMalloc(sizeof(char) * (len + 1));
    if (NULL == temp)
    {
        OIC_LOG(ERROR, TAG, "CACreateRemoteEndpointInternal Out of memory");

        CADestroyRemoteEndpointInternal(rep);

        return NULL;
    }
    strncpy(temp, resourceUri, len);
    temp[len] = '\0';

    // save the uri
    rep->resourceUri = temp;

    // save the addressInfo
    memcpy(&(rep->addressInfo), &addr, sizeof(CAAddress_t));

    // save the type
    rep->transportType = type;

    return rep;
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

    memcpy(clone, rep, sizeof(CARequestInfo_t));

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
        size_t len = strlen(rep->info.payload);

        char *temp = (char *) OICMalloc(sizeof(char) * (len + 1));
        if (NULL == temp)
        {
            OIC_LOG(ERROR, TAG, "CACloneRequestInfo Out of memory");

            CADestroyRequestInfoInternal(clone);

            return NULL;
        }
        strncpy(temp, rep->info.payload, len);
        temp[len] = '\0';

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

    // allocate the response info structure.
    CAResponseInfo_t *clone = (CAResponseInfo_t *) OICCalloc(1, sizeof(CAResponseInfo_t));
    if (NULL == clone)
    {
        OIC_LOG(ERROR, TAG, "CACloneResponseInfo Out of memory");
        return NULL;
    }
    memcpy(clone, rep, sizeof(CAResponseInfo_t));

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
        int32_t len = strlen(rep->info.payload);

        char *temp = (char *) OICCalloc(len + 1, sizeof(char));
        if (NULL == temp)
        {
            OIC_LOG(ERROR, TAG, "CACloneResponseInfo Out of memory");

            CADestroyResponseInfoInternal(clone);

            return NULL;
        }
        strncpy(temp, rep->info.payload, len);

        // save the payload
        clone->info.payload = temp;
    }

    return clone;
}

void CADestroyRemoteEndpointInternal(CARemoteEndpoint_t *rep)
{
    if (NULL == rep)
    {
        OIC_LOG(ERROR, TAG, "parameter is null");
        return;
    }

    // free uri field
    OICFree((char *) rep->resourceUri);

    // free remote end point structure.
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

