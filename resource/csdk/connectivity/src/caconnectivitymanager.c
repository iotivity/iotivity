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
    OIC_LOG_V(DEBUG, TAG, "CAInitialize");

    CAResult_t res = CAInitializeMessageHandler();

    if (res != CA_STATUS_OK)
    {
        return res;
    }

    return CA_STATUS_OK;
}

void CATerminate()
{
    OIC_LOG_V(DEBUG, TAG, "CATerminate");

    CATerminateMessageHandler();

    CATerminateNetworkType();
}

CAResult_t CAStartListeningServer()
{
    OIC_LOG_V(DEBUG, TAG, "CAStartListeningServer");

    return CAStartListeningServerAdapters();
}

CAResult_t CAStartDiscoveryServer()
{
    OIC_LOG_V(DEBUG, TAG, "CAStartDiscoveryServer");

    return CAStartDiscoveryServerAdapters();
}

void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
{
    OIC_LOG_V(DEBUG, TAG, "CARegisterHandler");

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

CAResult_t CACreateRemoteEndpoint(const CAURI_t uri,
    const CAConnectivityType_t connectivityType,CARemoteEndpoint_t **remoteEndpoint)
{
    OIC_LOG_V(DEBUG, TAG, "CACreateRemoteEndpoint");

    CARemoteEndpoint_t *remote = CACreateRemoteEndpointUriInternal(uri, connectivityType);

    *remoteEndpoint = remote;

    if (remote == NULL)
        return CA_STATUS_FAILED;

    return CA_STATUS_OK;
}

void CADestroyRemoteEndpoint(CARemoteEndpoint_t *rep)
{
    OIC_LOG_V(DEBUG, TAG, "CADestroyRemoteEndpoint");

    CADestroyRemoteEndpointInternal(rep);
}

CAResult_t CAGenerateToken(CAToken_t *token)
{
    OIC_LOG_V(DEBUG, TAG, "CAGenerateToken");

    return CAGenerateTokenInternal(token);
}

void CADestroyToken(CAToken_t token)
{
    OIC_LOG_V(DEBUG, TAG, "CADestroyToken");

    CADestroyTokenInternal(token);
}

CAResult_t CAGetNetworkInformation(CALocalConnectivity_t **info, uint32_t *size)
{
    OIC_LOG_V(DEBUG, TAG, "CAGetNetworkInformation");

    return CAGetNetworkInformationInternal(info, size);
}

CAResult_t CAFindResource(const CAURI_t resourceUri, const CAToken_t token)
{
    OIC_LOG_V(DEBUG, TAG, "CAFindResource");

    return CADetachMessageResourceUri(resourceUri, token, NULL, 0);

}

CAResult_t CASendRequest(const CARemoteEndpoint_t *object, CARequestInfo_t *requestInfo)
{
    OIC_LOG_V(DEBUG, TAG, "CASendGetRequest");

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
    OIC_LOG_V(DEBUG, TAG, "CASendNotification");

    return CADetachResponseMessage(object, responseInfo);

}

CAResult_t CASendResponse(const CARemoteEndpoint_t *object, CAResponseInfo_t *responseInfo)
{
    OIC_LOG_V(DEBUG, TAG, "CASendResponse");

    return CADetachResponseMessage(object, responseInfo);

}

CAResult_t CAAdvertiseResource(const CAURI_t resourceUri, CAToken_t token,
                               CAHeaderOption_t *options, uint8_t numOptions)
{
    OIC_LOG_V(DEBUG, TAG, "CAAdvertiseResource");

    return CADetachMessageResourceUri(resourceUri, token, options, numOptions);

}

CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
{
    OIC_LOG_V(DEBUG, TAG, "Selected network : %d", interestedNetwork);

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

    return CA_STATUS_OK;
}

CAResult_t CAUnSelectNetwork(const uint32_t nonInterestedNetwork)
{
    OIC_LOG_V(DEBUG, TAG, "unselected network : %d", nonInterestedNetwork);

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

    return CA_STATUS_OK;
}

CAResult_t CAHandleRequestResponse()
{
    OIC_LOG_V(DEBUG, TAG, "CAHandleRequestResponse");

    CAHandleRequestResponseCallbacks();

    return CA_STATUS_OK;
}

#ifdef __WITH_DTLS__

CAResult_t CASelectCipherSuite(const uint16_t cipher)
{
    OIC_LOG_V(DEBUG, TAG, "CASelectCipherSuite");

    return CADtlsSelectCipherSuite(cipher);
}

CAResult_t CAEnablesAnonEcdh(const uint8_t enable)
{
    OIC_LOG_V(DEBUG, TAG, "CAEnablesAnonEcdh");

    return CADtlsEnablesAnonEcdh(enable);
}

CAResult_t CAGenerateOwnerPSK(const CAAddress_t* addrInfo,
                    const CAConnectivityType_t connType,
                    const uint8_t* label, const size_t labelLen,
                    const uint8_t* rsrcServerDeviceID, const size_t rsrcServerDeviceIDLen,
                    const uint8_t* provServerDeviceID, const size_t provServerDeviceIDLen,
                    uint8_t* ownerPSK, const size_t ownerPSKSize)
{
    OIC_LOG_V(DEBUG, TAG, "IN : CAGenerateOwnerPSK");

    CAResult_t res = CA_STATUS_OK;

    //newOwnerLabel and prevOwnerLabe can be NULL
    if(!addrInfo || !label || 0 == labelLen || !ownerPSK || 0 == ownerPSKSize)
    {
        return CA_STATUS_INVALID_PARAM;
    }

    res = CADtlsGenerateOwnerPSK(addrInfo, connType, label, labelLen,
                                  rsrcServerDeviceID, rsrcServerDeviceIDLen,
                                  provServerDeviceID, provServerDeviceIDLen,
                                  ownerPSK, ownerPSKSize);
    if(CA_STATUS_OK != res)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to CAGenerateOwnerPSK : %d", res);
    }

    OIC_LOG_V(DEBUG, TAG, "OUT : CAGenerateOwnerPSK");

    return res;
}

CAResult_t CAInitiateHandshake(const CAAddress_t* addrInfo, const CAConnectivityType_t connType)
{
    OIC_LOG_V(DEBUG, TAG, "IN : CAInitiateHandshake");
    CAResult_t res = CA_STATUS_OK;

    if(!addrInfo)
    {
        CA_STATUS_INVALID_PARAM;
    }

    res = CADtlsInitiateHandshake(addrInfo, connType);
    if(CA_STATUS_OK != res)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to CAGenerateOwnerPSK : %d", res);
    }

    return res;
}

#endif /* __WITH_DTLS__ */
