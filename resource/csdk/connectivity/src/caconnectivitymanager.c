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
#include <stdbool.h>

#include "cainterface.h"
#include "caremotehandler.h"
#include "camessagehandler.h"
#include "caprotocolmessage.h"
#include "canetworkconfigurator.h"
#include "cainterfacecontroller.h"
#include "logger.h"
#ifdef __WITH_DTLS__
#include "caadapternetdtls.h"
#endif

#define TAG PCF("CA")

static bool g_isInitialized = false;

#ifdef __WITH_DTLS__
// CAAdapterNetDTLS will register the callback.
// Taking callback all the way through adapters not the right approach, hence calling here.
extern void CADTLSSetCredentialsCallback(CAGetDTLSCredentialsHandler credCallback);
#endif

CAResult_t CAInitialize()
{
    OIC_LOG(DEBUG, TAG, "CAInitialize");

    if (!g_isInitialized)
    {
        CAResult_t res = CAInitializeMessageHandler();
        if (res != CA_STATUS_OK)
        {
            OIC_LOG(ERROR, TAG, "CAInitialize has failed");
            return res;
        }
        g_isInitialized = true;
    }
    return CA_STATUS_OK;
}

void CATerminate()
{
    OIC_LOG(DEBUG, TAG, "CATerminate");

    if (g_isInitialized)
    {
        CATerminateMessageHandler();
        CATerminateNetworkType();

        g_isInitialized = false;
    }
}

CAResult_t CAStartListeningServer()
{
    OIC_LOG(DEBUG, TAG, "CAStartListeningServer");

    if(!g_isInitialized)
    {
        return CA_STATUS_NOT_INITIALIZED;
    }

    return CAStartListeningServerAdapters();
}

CAResult_t CAStartDiscoveryServer()
{
    OIC_LOG(DEBUG, TAG, "CAStartDiscoveryServer");

    if(!g_isInitialized)
    {
        return CA_STATUS_NOT_INITIALIZED;
    }

    return CAStartDiscoveryServerAdapters();
}

void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler,
                       CAErrorCallback ErrorHandler)
{
    OIC_LOG(DEBUG, TAG, "CARegisterHandler");

    if(!g_isInitialized)
    {
        OIC_LOG(DEBUG, TAG, "CA is not initialized");
        return;
    }

    CASetInterfaceCallbacks(ReqHandler, RespHandler, ErrorHandler);
}

#ifdef __WITH_DTLS__
CAResult_t CARegisterDTLSCredentialsHandler(
                                             CAGetDTLSCredentialsHandler GetDTLSCredentialsHandler)
{
    OIC_LOG(DEBUG, TAG, "CARegisterDTLSCredentialsHandler");

    if(!g_isInitialized)
    {
        return CA_STATUS_NOT_INITIALIZED;
    }

    CADTLSSetCredentialsCallback(GetDTLSCredentialsHandler);
    return CA_STATUS_OK;
}
#endif //__WITH_DTLS__

CAResult_t CACreateRemoteEndpoint(const CAURI_t uri, const CATransportType_t transportType,
                                  CARemoteEndpoint_t **remoteEndpoint)
{
    OIC_LOG(DEBUG, TAG, "CACreateRemoteEndpoint");

    if(!g_isInitialized)
    {
        return CA_STATUS_NOT_INITIALIZED;
    }

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

    if(!g_isInitialized)
    {
        return CA_STATUS_NOT_INITIALIZED;
    }
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

    if(!g_isInitialized)
    {
        return CA_STATUS_NOT_INITIALIZED;
    }

    return CAGetNetworkInformationInternal(info, size);
}

CAResult_t CAFindResource(const CAURI_t resourceUri, const CAToken_t token, uint8_t tokenLength)
{
    OIC_LOG(DEBUG, TAG, "CAFindResource");

    if(!g_isInitialized)
    {
        return CA_STATUS_NOT_INITIALIZED;
    }
    return CADetachMessageResourceUri(resourceUri, token, tokenLength, NULL, 0);

}

CAResult_t CASendRequest(const CARemoteEndpoint_t *object,const CARequestInfo_t *requestInfo)
{
    OIC_LOG(DEBUG, TAG, "CASendGetRequest");

    if(!g_isInitialized)
    {
        return CA_STATUS_NOT_INITIALIZED;
    }

    return CADetachRequestMessage(object, requestInfo);
}

CAResult_t CASendRequestToAll(const CAGroupEndpoint_t *object,
                              const CARequestInfo_t *requestInfo)
{
    OIC_LOG(DEBUG, TAG, "CASendRequestToAll");

    if(!g_isInitialized)
    {
        return CA_STATUS_NOT_INITIALIZED;
    }

    return CADetachRequestToAllMessage(object, requestInfo);
}

CAResult_t CASendNotification(const CARemoteEndpoint_t *object,
    const CAResponseInfo_t *responseInfo)
{
    OIC_LOG(DEBUG, TAG, "CASendNotification");

    if(!g_isInitialized)
    {
        return CA_STATUS_NOT_INITIALIZED;
    }

    return CADetachResponseMessage(object, responseInfo);

}

CAResult_t CASendResponse(const CARemoteEndpoint_t *object,
    const CAResponseInfo_t *responseInfo)
{
    OIC_LOG(DEBUG, TAG, "CASendResponse");

    if(!g_isInitialized)
    {
        return CA_STATUS_NOT_INITIALIZED;
    }

    return CADetachResponseMessage(object, responseInfo);

}

CAResult_t CAAdvertiseResource(const CAURI_t resourceUri,const CAToken_t token,
                               uint8_t tokenLength, const CAHeaderOption_t *options,
                               const uint8_t numOptions)
{
    OIC_LOG(DEBUG, TAG, "CAAdvertiseResource");

    if(!g_isInitialized)
    {
        return CA_STATUS_NOT_INITIALIZED;
    }
    return CADetachMessageResourceUri(resourceUri, token, tokenLength, options, numOptions);

}

CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
{
    OIC_LOG_V(DEBUG, TAG, "Selected network : %d", interestedNetwork);

    if(!g_isInitialized)
    {
        return CA_STATUS_NOT_INITIALIZED;
    }

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

    if(!g_isInitialized)
    {
        return CA_STATUS_NOT_INITIALIZED;
    }

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

    if (!g_isInitialized)
    {
        OIC_LOG(ERROR, TAG, "not initialized");
        return CA_STATUS_NOT_INITIALIZED;
    }

    CAHandleRequestResponseCallbacks();

    return CA_STATUS_OK;
}

#ifdef __WITH_DTLS__

CAResult_t CASelectCipherSuite(const uint16_t cipher)
{
    OIC_LOG_V(DEBUG, TAG, "CASelectCipherSuite");

    return CADtlsSelectCipherSuite(cipher);
}

CAResult_t CAEnableAnonECDHCipherSuite(const bool enable)
{
    OIC_LOG_V(DEBUG, TAG, "CAEnableAnonECDHCipherSuite");

    return CADtlsEnableAnonECDHCipherSuite(enable);
}

CAResult_t CAGenerateOwnerPSK(const CAAddress_t* addrInfo,
                    const CATransportType_t transportType,
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

    res = CADtlsGenerateOwnerPSK(addrInfo, transportType, label, labelLen,
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

CAResult_t CAInitiateHandshake(const CAAddress_t* addrInfo,
                               const CATransportType_t transportType)
{
    OIC_LOG_V(DEBUG, TAG, "IN : CAInitiateHandshake");
    CAResult_t res = CA_STATUS_OK;

    if(!addrInfo)
    {
        return CA_STATUS_INVALID_PARAM;
    }

    res = CADtlsInitiateHandshake(addrInfo, transportType);
    if(CA_STATUS_OK != res)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to CADtlsInitiateHandshake : %d", res);
    }

    OIC_LOG_V(DEBUG, TAG, "OUT : CAInitiateHandshake");

    return res;
}

CAResult_t CACloseDtlsSession(const CAAddress_t* addrInfo,
                              const CATransportType_t transportType)
{
    OIC_LOG_V(DEBUG, TAG, "IN : CACloseDtlsSession");
    CAResult_t res = CA_STATUS_OK;

    if(!addrInfo)
    {
        return CA_STATUS_INVALID_PARAM;
    }

    res = CADtlsClose(addrInfo, transportType);
    if(CA_STATUS_OK != res)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to CADtlsClose : %d", res);
    }

    OIC_LOG_V(DEBUG, TAG, "OUT : CACloseDtlsSession");

    return res;
}

#endif /* __WITH_DTLS__ */
