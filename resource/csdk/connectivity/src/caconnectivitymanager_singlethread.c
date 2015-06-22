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
#include "caprotocolmessage.h"
#include "canetworkconfigurator.h"
#include "logger.h"

#include "cainterfacecontroller_singlethread.h"
#include "camessagehandler_singlethread.h"

#define TAG "CM_ST"

CAGlobals_t caglobals;

static bool g_isInitialized = false;

CAResult_t CAInitialize()
{
    OIC_LOG(DEBUG, TAG, "IN");

    if (!g_isInitialized)
    {
        CAResult_t res = CAInitializeMessageHandler();
        if (res != CA_STATUS_OK)
        {
            OIC_LOG(ERROR, TAG, "not initialized");
            return res;
        }
        g_isInitialized = true;
    }

    return CA_STATUS_OK;
}

void CATerminate()
{
    OIC_LOG(DEBUG, TAG, "IN");

    if (g_isInitialized)
    {
        CASetInterfaceCallbacks(NULL, NULL, NULL);
        CATerminateMessageHandler();
        CATerminateNetworkType();
        g_isInitialized = false;
    }

    OIC_LOG(DEBUG, TAG, "OUT");
}

CAResult_t CAStartListeningServer()
{
    if (!g_isInitialized)
    {
        OIC_LOG(ERROR, TAG, "not initialized");
        return CA_STATUS_NOT_INITIALIZED;
    }

    return CAStartListeningServerAdapters();
}

CAResult_t CAStartDiscoveryServer()
{
    if (!g_isInitialized)
    {
        OIC_LOG(ERROR, TAG, "not initialized");
        return CA_STATUS_NOT_INITIALIZED;
    }

    return CAStartDiscoveryServerAdapters();
}

void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler,
                       CAErrorCallback errorHandler)
{
    OIC_LOG(DEBUG, TAG, "IN");

    if (!g_isInitialized)
    {
        OIC_LOG(ERROR, TAG, "not initialized");
        return;
    }

    CASetInterfaceCallbacks(ReqHandler, RespHandler, errorHandler);
    OIC_LOG(DEBUG, TAG, "OUT");
}

void CADestroyEndpoint(CAEndpoint_t *rep)
{
    CADestroyEndpointInternal(rep);
}

CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
{
    if (!g_isInitialized)
    {
        OIC_LOG(ERROR, TAG, "not initialized");
        return CA_STATUS_NOT_INITIALIZED;
    }

    return CAGenerateTokenInternal(token, tokenLength);
}

void CADestroyToken(CAToken_t token)
{
    OIC_LOG(DEBUG, TAG, "IN");

    CADestroyTokenInternal(token);
    OIC_LOG(DEBUG, TAG, "OUT");
}

CAResult_t CAGetNetworkInformation(CAEndpoint_t **info, uint32_t *size)
{
    if (!g_isInitialized)
    {
        OIC_LOG(ERROR, TAG, "not initialized");
        return CA_STATUS_NOT_INITIALIZED;
    }

    return CAGetNetworkInformationInternal(info, size);
}

CAResult_t CASendRequest(const CAEndpoint_t *object,const CARequestInfo_t *requestInfo)
{
    OIC_LOG(DEBUG, TAG, "CASendGetRequest");

    return CADetachRequestMessage(object, requestInfo);
}

CAResult_t CASendNotification(const CAEndpoint_t *object,
    const CAResponseInfo_t *responseInfo)
{
    OIC_LOG(DEBUG, TAG, "CASendNotification");

    return CADetachResponseMessage(object, responseInfo);
}

CAResult_t CASendResponse(const CAEndpoint_t *object,
    const CAResponseInfo_t *responseInfo)
{
    OIC_LOG(DEBUG, TAG, "CASendResponse");

    return CADetachResponseMessage(object, responseInfo);
}

CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
{
    OIC_LOG_V(DEBUG, TAG, "Selected n/W=%d", interestedNetwork);

    if (!g_isInitialized)
    {
        OIC_LOG(ERROR, TAG, "not initialized");
        return CA_STATUS_NOT_INITIALIZED;
    }

    if (!(interestedNetwork & 0xf))
    {
        OIC_LOG(ERROR, TAG, "not supported");
        return CA_NOT_SUPPORTED;
    }
    CAResult_t res = CA_STATUS_OK;

    if (interestedNetwork & CA_ADAPTER_IP)
    {
        res = CAAddNetworkType(CA_ADAPTER_IP);
        if (res != CA_STATUS_OK)
        {
            OIC_LOG(ERROR, TAG, "Failed to Add n/w type");
            return res;
        }
    }

    if (interestedNetwork & CA_ADAPTER_RFCOMM_BTEDR)
    {
        res = CAAddNetworkType(CA_ADAPTER_RFCOMM_BTEDR);
        if (res != CA_STATUS_OK)
        {
            OIC_LOG(ERROR, TAG, "Failed to Add n/w type");
            return res;
        }
    }

    if (interestedNetwork & CA_ADAPTER_GATT_BTLE)
    {
        res = CAAddNetworkType(CA_ADAPTER_GATT_BTLE);
        if (res != CA_STATUS_OK)
        {
            OIC_LOG(ERROR, TAG, "Failed to Add n/w type");
            return res;
        }
    }
    OIC_LOG(DEBUG, TAG, "OUT");
    return res;
}

CAResult_t CAUnSelectNetwork(const uint32_t nonInterestedNetwork)
{
    OIC_LOG_V(DEBUG, TAG, "unselected n/w=%d", nonInterestedNetwork);

    if (!g_isInitialized)
    {
        OIC_LOG(ERROR, TAG, "not initialized");
        return CA_STATUS_NOT_INITIALIZED;
    }

    if (!(nonInterestedNetwork & 0xf))
    {
        OIC_LOG(ERROR, TAG, "not supported");
        return CA_NOT_SUPPORTED;
    }

    CAResult_t res = CA_STATUS_OK;

    if (nonInterestedNetwork & CA_ADAPTER_IP)
    {
        res = CARemoveNetworkType(CA_ADAPTER_IP);
        if (res != CA_STATUS_OK)
        {
            OIC_LOG(ERROR, TAG, "Failed to remove n/w type");
            return res;
        }
    }

    if (nonInterestedNetwork & CA_ADAPTER_RFCOMM_BTEDR)
    {
        res = CARemoveNetworkType(CA_ADAPTER_RFCOMM_BTEDR);
        if (res != CA_STATUS_OK)
        {
            OIC_LOG(ERROR, TAG, "Failed to remove n/w type");
            return res;
        }
    }

    if (nonInterestedNetwork & CA_ADAPTER_GATT_BTLE)
    {
        res = CARemoveNetworkType(CA_ADAPTER_GATT_BTLE);
        if (res != CA_STATUS_OK)
        {
            OIC_LOG(ERROR, TAG, "Failed to remove n/w type");
            return res;
        }
    }
    OIC_LOG(DEBUG, TAG, "OUT");
    return res;
}

CAResult_t CAHandleRequestResponse()
{
    if (!g_isInitialized)
    {
        OIC_LOG(ERROR, TAG, "not initialized");
        return CA_STATUS_NOT_INITIALIZED;
    }

    CAHandleRequestResponseCallbacks();
    return CA_STATUS_OK;
}

