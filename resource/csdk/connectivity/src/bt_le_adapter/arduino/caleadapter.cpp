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
#include "caleadapter_singlethread.h"
#include "caleserver.h"
#include "logger.h"
#include "caadapterutils.h"

static CANetworkChangeCallback networkCallback = NULL;
#define BLE_ADDRESS "DB:F7:EB:B5:0F:07"
#define TAG "CALEADAPTER"

/**
 * @brief API to store the networkcallback passed from top layer, register BLE network notification
 * @param net_callback - network notification callback
 * @return - int
 */
int LERegisterNetworkNotifications(CANetworkChangeCallback netCallback);

CAResult_t CAInitializeLE(CARegisterConnectivityCallback registerCallback,
                          CANetworkPacketReceivedCallback reqRespCallback, CANetworkChangeCallback netCallback)
{
    OIC_LOG(DEBUG, TAG, "IN");
    if (NULL == registerCallback || NULL == reqRespCallback || NULL == netCallback)
    {
        OIC_LOG(ERROR, TAG, "Invalid Parameter");
        return CA_STATUS_INVALID_PARAM;
    }

    CASetBLEReqRescallback(reqRespCallback);
    LERegisterNetworkNotifications(netCallback);
    CAConnectivityHandler_t connHandler;
    connHandler.startAdapter = NULL;
    connHandler.startListenServer = CAStartLEListeningServer;
    connHandler.startDiscoverServer = CAStartLEDiscoveryServer;
    connHandler.sendData = CASendLEUnicastData;
    connHandler.sendDataToAll = CASendLEMulticastData;
    connHandler.GetnetInfo = CAGetLEInterfaceInformation;
    connHandler.readData = CAReadLEData;
    connHandler.terminate = CATerminateLE;
    registerCallback(connHandler, CA_LE);
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAReadLEData()
{
    CAArduoinoCheckServerData();
}

void CATerminateLE()
{
    OIC_LOG(DEBUG, TAG, "IN");
    CASetBLEReqRescallback(NULL);
    LERegisterNetworkNotifications(NULL);
    CAStopBleGattServer();
    OIC_LOG(DEBUG, TAG, "OUT");
    return;
}
CAResult_t CAStartLEListeningServer()
{
    OIC_LOG(DEBUG, TAG, "IN");
    CAInitializeBle();
    CAArduoinoCheckServerData();
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAStartLEDiscoveryServer()
{
    OIC_LOG(DEBUG, TAG, "IN");
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAStartLENotifyServer()
{
    OIC_LOG(DEBUG, TAG, "IN");
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

uint32_t CASendLENotification(const CARemoteEndpoint_t *endpoint, void *data, uint32_t dataLen)
{
    OIC_LOG(DEBUG, TAG, "IN");
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

uint32_t CASendLEUnicastData(const CARemoteEndpoint_t *remoteEndpoint, void *data, uint32_t dataLen)
{
    OIC_LOG(DEBUG, TAG, "IN");
    if (NULL == remoteEndpoint)
    {
        OIC_LOG(ERROR, TAG, "Invalid Input Parameter");
        return CA_STATUS_INVALID_PARAM;
    }
    char *tempPath = "a";
    CAUpdateCharacteristicsInGattServer(tempPath, (char *)data, dataLen);
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

uint32_t CASendLEMulticastData(void *data, uint32_t dataLen)
{
    OIC_LOG(DEBUG, TAG, "IN");
    if (NULL == data || 0 == dataLen)
    {
        OIC_LOG(ERROR, TAG, "Invalid Parameter");
        return CA_STATUS_INVALID_PARAM;
    }
    char *tempPath = "a";
    CAUpdateCharacteristicsInGattServer(tempPath, (char *)data, dataLen);
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAGetLEInterfaceInformation(CALocalConnectivity_t **info, uint32_t *size)
{
    OIC_LOG(DEBUG, TAG, "IN");

    if (NULL == info || NULL == size)
    {
        OIC_LOG(DEBUG, TAG, "Invalid Parameter");
        return CA_STATUS_INVALID_PARAM;
    }

    /**
     * Create local endpoint using util function
     */
    (*info) = CAAdapterCreateLocalEndpoint(CA_LE, BLE_ADDRESS, "Le");
    if (NULL == (*info))
    {
        OIC_LOG(DEBUG, TAG, "Out of memory");
        return CA_MEMORY_ALLOC_FAILED;
    }

    (*size) = 1;
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

int LERegisterNetworkNotifications(CANetworkChangeCallback netCallback)
{
    OIC_LOG(DEBUG, TAG, "IN");
    networkCallback = netCallback;
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}
