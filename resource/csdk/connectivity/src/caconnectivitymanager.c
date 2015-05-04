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
#include "camessagehandler.h"
#include "caprotocolmessage.h"
#include "canetworkconfigurator.h"
#include "cainterfacecontroller.h"
#include "logger.h"

#define TAG PCF("CA")

#ifdef __WITH_DTLS__
// CAAdapterNetDTLS will register the callback.
// Taking callback all the way through adapters not the right approach, hence calling here.
extern void CADTLSSetCredentialsCallback(CAGetDTLSCredentialsHandler credCallback);
#endif

CAResult_t CAInitialize()
{
    OIC_LOG(DEBUG, TAG, "CAInitialize");

    return CAInitializeMessageHandler();;
}

void CATerminate()
{
    OIC_LOG(DEBUG, TAG, "CATerminate");

    CATerminateMessageHandler();

    CATerminateNetworkType();
}

CAResult_t CAStartListeningServer()
{
    OIC_LOG(DEBUG, TAG, "CAStartListeningServer");

    return CAStartListeningServerAdapters();
}

CAResult_t CAStartDiscoveryServer()
{
    OIC_LOG(DEBUG, TAG, "CAStartDiscoveryServer");

    return CAStartDiscoveryServerAdapters();
}

void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
{
    OIC_LOG(DEBUG, TAG, "CARegisterHandler");

    CASetRequestResponseCallbacks(ReqHandler, RespHandler);
}

#ifdef __WITH_DTLS__
CAResult_t CARegisterDTLSCredentialsHandler(
                                             CAGetDTLSCredentialsHandler GetDTLSCredentialsHandler)
{
    OIC_LOG(DEBUG, TAG, "CARegisterDTLSCredentialsHandler");
    CADTLSSetCredentialsCallback(GetDTLSCredentialsHandler);
    return CA_STATUS_OK;
}
#endif //__WITH_DTLS__

CAResult_t CACreateRemoteEndpoint(const CAURI_t uri, const CATransportType_t transportType,
                                  CARemoteEndpoint_t **remoteEndpoint)
{
    OIC_LOG(DEBUG, TAG, "CACreateRemoteEndpoint");

    CARemoteEndpoint_t *remote = CACreateRemoteEndpointUriInternal(uri, transportType);

    if (remote == NULL)
    {
        OIC_LOG(DEBUG, TAG, "remote is NULL!");
        return CA_STATUS_FAILED;
    }

    *remoteEndpoint = remote;

    return CA_STATUS_OK;
}

void CADestroyRemoteEndpoint(CARemoteEndpoint_t *rep)
{
    OIC_LOG(DEBUG, TAG, "CADestroyRemoteEndpoint");

    CADestroyRemoteEndpointInternal(rep);
}

CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
{
    OIC_LOG(DEBUG, TAG, "CAGenerateToken");

    return CAGenerateTokenInternal(token, tokenLength);
}

void CADestroyToken(CAToken_t token)
{
    OIC_LOG(DEBUG, TAG, "CADestroyToken");

    CADestroyTokenInternal(token);
}

CAResult_t CAGetNetworkInformation(CALocalConnectivity_t **info, uint32_t *size)
{
    OIC_LOG(DEBUG, TAG, "CAGetNetworkInformation");

    return CAGetNetworkInformationInternal(info, size);
}

CAResult_t CAFindResource(const CAURI_t resourceUri, const CAToken_t token, uint8_t tokenLength)
{
    OIC_LOG(DEBUG, TAG, "CAFindResource");

    return CADetachMessageResourceUri(resourceUri, token, tokenLength, NULL, 0);

}

CAResult_t CASendRequest(const CARemoteEndpoint_t *object,const CARequestInfo_t *requestInfo)
{
    OIC_LOG(DEBUG, TAG, "CASendGetRequest");

    return CADetachRequestMessage(object, requestInfo);
}

CAResult_t CASendRequestToAll(const CAGroupEndpoint_t *object,
                              const CARequestInfo_t *requestInfo)
{
    OIC_LOG(DEBUG, TAG, "CASendRequestToAll");

    return CADetachRequestToAllMessage(object, requestInfo);
}

CAResult_t CASendNotification(const CARemoteEndpoint_t *object,
    const CAResponseInfo_t *responseInfo)
{
    OIC_LOG(DEBUG, TAG, "CASendNotification");

    return CADetachResponseMessage(object, responseInfo);

}

CAResult_t CASendResponse(const CARemoteEndpoint_t *object,
    const CAResponseInfo_t *responseInfo)
{
    OIC_LOG(DEBUG, TAG, "CASendResponse");

    return CADetachResponseMessage(object, responseInfo);

}

CAResult_t CAAdvertiseResource(const CAURI_t resourceUri,const CAToken_t token,
                               uint8_t tokenLength, const CAHeaderOption_t *options,
                               const uint8_t numOptions)
{
    OIC_LOG(DEBUG, TAG, "CAAdvertiseResource");

    return CADetachMessageResourceUri(resourceUri, token, tokenLength, options, numOptions);

}

CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
{
    OIC_LOG_V(DEBUG, TAG, "Selected network : %d", interestedNetwork);

    if (!(interestedNetwork & 0xf))
    {
        return CA_NOT_SUPPORTED;
    }

    CAResult_t res = CA_STATUS_OK;

    if (interestedNetwork & CA_IPV4)
    {
        res = CAAddNetworkType(CA_IPV4);
        OIC_LOG_V(ERROR, TAG, "CAAddNetworkType(CA_IPV4) function returns error : %d", res);
    }

    if (interestedNetwork & CA_EDR)
    {
        res = CAAddNetworkType(CA_EDR);
        OIC_LOG_V(ERROR, TAG, "CAAddNetworkType(CA_EDR) function returns error : %d", res);
    }

    if (interestedNetwork & CA_LE)
    {
        res = CAAddNetworkType(CA_LE);
        OIC_LOG_V(ERROR, TAG, "CAAddNetworkType(CA_LE) function returns error : %d", res);
    }

    return res;
}

CAResult_t CAUnSelectNetwork(const uint32_t nonInterestedNetwork)
{
    OIC_LOG_V(DEBUG, TAG, "unselected network : %d", nonInterestedNetwork);

    if (!(nonInterestedNetwork & 0xf))
    {
        return CA_NOT_SUPPORTED;
    }

    CAResult_t res = CA_STATUS_OK;

    if (nonInterestedNetwork & CA_IPV4)
    {
        res = CARemoveNetworkType(CA_IPV4);
        OIC_LOG_V(ERROR, TAG, "CARemoveNetworkType(CA_IPV4) function returns error : %d", res);
    }

    if (nonInterestedNetwork & CA_EDR)
    {
        res = CARemoveNetworkType(CA_EDR);
        OIC_LOG_V(ERROR, TAG, "CARemoveNetworkType(CA_EDR) function returns error : %d", res);
    }

    if (nonInterestedNetwork & CA_LE)
    {
        res = CARemoveNetworkType(CA_LE);
        OIC_LOG_V(ERROR, TAG, "CARemoveNetworkType(CA_LE) function returns error : %d", res);
    }

    return res;
}

CAResult_t CAHandleRequestResponse()
{
    OIC_LOG(DEBUG, TAG, "CAHandleRequestResponse");

    CAHandleRequestResponseCallbacks();

    return CA_STATUS_OK;
}


