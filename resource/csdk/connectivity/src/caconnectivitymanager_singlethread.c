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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "cainterface.h"
#include "caremotehandler.h"
#include "caprotocolmessage_singlethread.h"
#include "canetworkconfigurator.h"
#include "logger.h"

#include "cainterfacecontroller_singlethread.h"
#include "camessagehandler_singlethread.h"

#define TAG "CM_ST"

static void CAMessageHandler(CAToken_t token, CAResult_t res)
{
    // TODO
    // it will notify the result for the each action.

}

CAResult_t CAInitialize()
{
    OIC_LOG(DEBUG, TAG, "IN");

    CASetMessageHandlerCallback(CAMessageHandler);

    CAResult_t res = CAInitializeMessageHandler();

    if (res != CA_STATUS_OK)
    {
        return res;
    }
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

void CATerminate()
{
    OIC_LOG(DEBUG, TAG, "IN");
    CATerminateMessageHandler();

    CATerminateNetworkType();
    OIC_LOG(DEBUG, TAG, "OUT");
}

CAResult_t CAStartListeningServer()
{
    return CAStartListeningServerAdapters();
}

CAResult_t CAStartDiscoveryServer()
{
    return CAStartDiscoveryServerAdapters();
}

void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
{
    OIC_LOG(DEBUG, TAG, "IN");
    CASetRequestResponseCallbacks(ReqHandler, RespHandler);
    OIC_LOG(DEBUG, TAG, "OUT");
}

CAResult_t CACreateRemoteEndpoint(const CAURI_t uri,
    const CAConnectivityType_t connectivityType,CARemoteEndpoint_t **remoteEndpoint)
{
    OIC_LOG(DEBUG, TAG, "IN");

    CARemoteEndpoint_t *remote = CACreateRemoteEndpointUriInternal(uri,connectivityType);

    *remoteEndpoint = remote;

    if (remote == NULL)
        return CA_STATUS_FAILED;

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

void CADestroyRemoteEndpoint(CARemoteEndpoint_t *rep)
{
    CADestroyRemoteEndpointInternal(rep);
}

CAResult_t CAGenerateToken(CAToken_t *token)
{
    return CAGenerateTokenInternal(token);
}

void CADestroyToken(CAToken_t token)
{
    OIC_LOG(DEBUG, TAG, "IN");
    CADestroyTokenInternal(token);
    OIC_LOG(DEBUG, TAG, "OUT");
}

CAResult_t CAGetNetworkInformation(CALocalConnectivity_t **info, uint32_t *size)
{
    return CAGetNetworkInformationInternal(info, size);
}

CAResult_t CAFindResource(const CAURI_t resourceUri, const CAToken_t token)
{
    OIC_LOG(DEBUG, TAG, "IN");
    CAResult_t res = CADetachMessageResourceUri(resourceUri, token, NULL, 0);
    OIC_LOG(DEBUG, TAG, "OUT");
    return res;
}

CAResult_t CASendRequest(const CARemoteEndpoint_t *object, CARequestInfo_t *requestInfo)
{
    return CADetachRequestMessage(object, requestInfo);
}

CAResult_t CASendRequestToAll(const CAGroupEndpoint_t *object,
                              const CARequestInfo_t *requestInfo)
{
    OIC_LOG_V(DEBUG, TAG, "CASendRequestToAll");

    return CADetachRequestToAllMessage(object, requestInfo);
}

CAResult_t CASendNotification(const CARemoteEndpoint_t *object, CAResponseInfo_t *responseInfo)
{
    return CADetachResponseMessage(object, responseInfo);
}

CAResult_t CASendResponse(const CARemoteEndpoint_t *object, CAResponseInfo_t *responseInfo)
{
    return CADetachResponseMessage(object, responseInfo);
}

CAResult_t CAAdvertiseResource(const CAURI_t resourceUri, CAToken_t token,
                               CAHeaderOption_t *options,
                               uint8_t numOptions)
{
    return CADetachMessageResourceUri(resourceUri, token, options, numOptions);
}

CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
{
    OIC_LOG_V(DEBUG, TAG, "Selected n/W=%d", interestedNetwork);

    if (!(interestedNetwork & 0xf))
    {
        return CA_NOT_SUPPORTED;
    }
    CAResult_t res;

    if (interestedNetwork & CA_ETHERNET)
    {
        res = CAAddNetworkType(CA_ETHERNET);
        if (res != CA_STATUS_OK)
        {
            return res;
        }
    }

    if (interestedNetwork & CA_WIFI)
    {
        res = CAAddNetworkType(CA_WIFI);
        if (res != CA_STATUS_OK)
        {
            return res;
        }
    }

    if (interestedNetwork & CA_EDR)
    {
        res = CAAddNetworkType(CA_EDR);
        if (res != CA_STATUS_OK)
        {
            return res;
        }
    }

    if (interestedNetwork & CA_LE)
    {
        res = CAAddNetworkType(CA_LE);
        if (res != CA_STATUS_OK)
        {
            return res;
        }
    }
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAUnSelectNetwork(const uint32_t nonInterestedNetwork)
{
    OIC_LOG_V(DEBUG, TAG, "unselected n/w=%d", nonInterestedNetwork);

    if (!(nonInterestedNetwork & 0xf))
    {
        return CA_NOT_SUPPORTED;
    }

    CAResult_t res;

    if (nonInterestedNetwork & CA_ETHERNET)
    {
        res = CARemoveNetworkType(CA_ETHERNET);
        if (res != CA_STATUS_OK)
        {
            return res;
        }
    }

    if (nonInterestedNetwork & CA_WIFI)
    {
        res = CARemoveNetworkType(CA_WIFI);
        if (res != CA_STATUS_OK)
        {
            return res;
        }
    }

    if (nonInterestedNetwork & CA_EDR)
    {
        res = CARemoveNetworkType(CA_EDR);
        if (res != CA_STATUS_OK)
        {
            return res;
        }
    }

    if (nonInterestedNetwork & CA_LE)
    {
        res = CARemoveNetworkType(CA_LE);
        if (res != CA_STATUS_OK)
        {
            return res;
        }
    }
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAHandleRequestResponse()
{
    CAHandleRequestResponseCallbacks();
    return CA_STATUS_OK;
}

