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

#include "caleinterface_singlethread.h"
#include "cableserver.h"
#include "logger.h"
#include "caadapterutils.h"
#include "camsgparser.h"

static CANetworkChangeCallback networkCallback = NULL;
static bool gServerRunning = false;
#define TAG "LAD"

#define COAP_MAX_PDU_SIZE 320

static CANetworkPacketReceivedCallback gRespCallback;
static char *gCoapBuffer = NULL;
static uint32_t dataLen = 0;
static uint32_t packetDataLen = 0;

/**
 * @brief API to store the networkcallback passed from top layer, register BLE network notification
 * @param net_callback - network notification callback
 * @return - Error Code
 */
CAResult_t LERegisterNetworkNotifications(CANetworkChangeCallback netCallback);

/**
 * @brief API to send received data to upper layer
 * @param[in] data - data received from BLE characteristics
 * @param[in] dataLen - received data Length
 * @param[in] senderAdrs - sender Address.
 * @param[in] senderPort - sender port
 * @return - Error Code
 */
void CANotifyCallback(void *data, int32_t dataLen, char *senderAdrs, int32_t senderPort);

/**
 * @brief API to read the data from characteristics and invoke notifyCallback
 * @return - void
 */
void CACheckData();

CAResult_t CAInitializeLE(CARegisterConnectivityCallback registerCallback,
                          CANetworkPacketReceivedCallback reqRespCallback,
                          CANetworkChangeCallback netCallback)
{
    OIC_LOG(DEBUG, TAG, "IN");
    if (NULL == registerCallback || NULL == reqRespCallback || NULL == netCallback)
    {
        OIC_LOG(ERROR, TAG, "error");
        return CA_STATUS_INVALID_PARAM;
    }

    CAResult_t result = CALEInitializeNetworkMonitor();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, TAG, "error");
        return CA_STATUS_FAILED;
    }

    gRespCallback = reqRespCallback;
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
    if (true == gServerRunning)
    {
        CACheckData();
    }
    return CA_STATUS_OK;
}

void CATerminateLE()
{
    OIC_LOG(DEBUG, TAG, "IN");
    gRespCallback = NULL;
    LERegisterNetworkNotifications(NULL);
    CAStopBleGattServer();
    CALETerminateNetworkMonitor();
    gServerRunning = false;
    OIC_LOG(DEBUG, TAG, "OUT");
    return;
}
CAResult_t CAStartLEListeningServer()
{
    OIC_LOG(DEBUG, TAG, "IN");
    uint32_t iter = 0;
    CAResult_t result = CAInitializeBle();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, TAG, "error");
        return CA_STATUS_FAILED;
    }

    /**
     * Below for loop is to process the BLE Events received from BLE Shield.
     * BLE Events includes BLE Shield Address Added as a patch to RBL Library.
     */
    for (iter = 0; iter < 20; iter++)
    {
        CACheckData();
    }

    gServerRunning = true;
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

uint32_t CASendLENotification(const CARemoteEndpoint_t *endpoint, void *data,
                              uint32_t dataLen)
{
    OIC_LOG(DEBUG, TAG, "IN");
    OIC_LOG(DEBUG, TAG, "OUT");
    return 1;
}

uint32_t CASendLEUnicastData(const CARemoteEndpoint_t *remoteEndpoint, void *data,
                             uint32_t dataLen)
{
    OIC_LOG(DEBUG, TAG, "IN");
    if (NULL == remoteEndpoint)
    {
        OIC_LOG(ERROR, TAG, "error");
        return 0;
    }

    char *char_value = (char *)data;
    if (NULL == char_value || dataLen == 0)
    {
        OIC_LOG(ERROR, TAG, "error");
        return 0;
    }

    char header[CA_HEADER_LENGTH] = "";
    memset(header, 0, sizeof(char) * CA_HEADER_LENGTH);

    CAResult_t result = CAGenerateHeader(header, dataLen);

    if (CA_STATUS_OK != result)
    {
        return 0;
    }

    int32_t index = 0;

    if (!CAIsBleConnected())
    {
        OIC_LOG(DEBUG, TAG, "le not conn");
        return 0;
    }

    CAUpdateCharacteristicsInGattServer(header, CA_HEADER_LENGTH);
    int32_t iter = dataLen / CA_SUPPORTED_BLE_MTU_SIZE;

    for (index = 0; index < iter; index++)
    {
        CAUpdateCharacteristicsInGattServer((char_value + (index * CA_SUPPORTED_BLE_MTU_SIZE)),
                                                CA_SUPPORTED_BLE_MTU_SIZE);
        CABleDoEvents();
    }

    CAUpdateCharacteristicsInGattServer((char_value + (index * CA_SUPPORTED_BLE_MTU_SIZE)),
                                            dataLen % CA_SUPPORTED_BLE_MTU_SIZE);
    CABleDoEvents();
    OIC_LOG(DEBUG, TAG, "writebytes done");
    OIC_LOG(DEBUG, TAG, "OUT");
    // Arduino BLEWrite doesnot return value. So, Return the received DataLength
    return dataLen;
}

uint32_t CASendLEMulticastData(void *data, uint32_t dataLen)
{
    OIC_LOG(DEBUG, TAG, "IN");
    if (NULL == data || 0 == dataLen)
    {
        OIC_LOG(ERROR, TAG, "error");
        return 0;
    }

    char *char_value = (char *)data;

    char header[CA_HEADER_LENGTH] = "";
    memset(header, 0, sizeof(char) * CA_HEADER_LENGTH);

    CAResult_t result = CAGenerateHeader(header, dataLen);

    if (CA_STATUS_OK != result)
    {
        return 0;
    }

    int32_t index = 0;

    if (!CAIsBleConnected())
    {
        OIC_LOG(DEBUG, TAG, "le not conn");
        return 0;
    }

    CAUpdateCharacteristicsInGattServer(header, CA_HEADER_LENGTH);
    int32_t iter = dataLen / CA_SUPPORTED_BLE_MTU_SIZE;

    for (index = 0; index < iter; index++)
    {
        CAUpdateCharacteristicsInGattServer((char_value + (index * CA_SUPPORTED_BLE_MTU_SIZE)),
                                                CA_SUPPORTED_BLE_MTU_SIZE);
        CABleDoEvents();
    }

    CAUpdateCharacteristicsInGattServer((char_value + (index * CA_SUPPORTED_BLE_MTU_SIZE)),
                                            dataLen % CA_SUPPORTED_BLE_MTU_SIZE);
    CABleDoEvents();
    OIC_LOG(DEBUG, TAG, "writebytes done");

    OIC_LOG(DEBUG, TAG, "OUT");
    // Arduino BLEWrite doesnot return value. So, Return the received DataLength
    return dataLen;
}

CAResult_t CAGetLEInterfaceInformation(CALocalConnectivity_t **info, uint32_t *size)
{
    OIC_LOG(DEBUG, TAG, "IN");

    if (NULL == info || NULL == size)
    {
        OIC_LOG(DEBUG, TAG, "error");
        return CA_STATUS_INVALID_PARAM;
    }

    char *leAddress = NULL;
    CAGetLEAddress(&leAddress);
    OIC_LOG_V(DEBUG, TAG, "leAddress = %s", leAddress);

    /**
     * Create local endpoint using util function
     */
    (*info) = CAAdapterCreateLocalEndpoint(CA_LE, leAddress);
    if (NULL == (*info))
    {
        OIC_LOG(DEBUG, TAG, "error");
        return CA_MEMORY_ALLOC_FAILED;
    }

    (*size) = 1;
    if (*leAddress)
    {
        OICFree(leAddress);
    }
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t LERegisterNetworkNotifications(CANetworkChangeCallback netCallback)
{
    OIC_LOG(DEBUG, TAG, "IN");
    networkCallback = netCallback;
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAStartBleGattServer()
{
    OIC_LOG(DEBUG, TAG, "IN");
    // Done at time of setup i.e. in initializeBle api
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAStopBleGattServer()
{
    // Not Supported
    CAResult_t result = CATerminateBle();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, TAG, "error");
        return CA_STATUS_FAILED;
    }
    return CA_STATUS_OK;
}

void CANotifyCallback(void *data, int32_t dataLen, char *senderAdrs, int32_t senderPort)
{
    OIC_LOG(DEBUG, TAG, "IN");
    if (gRespCallback)
    {

        /* Cannot get Address as of now */
        CARemoteEndpoint_t endPoint;
        endPoint.resourceUri = "";     // will be filled by upper layer
        endPoint.connectivityType = CA_LE;

        gRespCallback(&endPoint, data, dataLen);
    }
    OIC_LOG(DEBUG, TAG, "OUT");
}

void CACheckData()
{
    CABleDoEvents();

    if (CAIsBleDataAvailable())
    {
        // Allocate Memory for COAP Buffer and do ParseHeader
        if (NULL == gCoapBuffer)
        {
            OIC_LOG(DEBUG, TAG, "IN");
            char headerArray[CA_HEADER_LENGTH] = "";
            while (CAIsBleDataAvailable() && dataLen < CA_HEADER_LENGTH)
            {
                headerArray[dataLen++] = CAReadBleData();
            }

            packetDataLen = CAParseHeader(headerArray);

            if (packetDataLen > COAP_MAX_PDU_SIZE)
            {
                OIC_LOG(ERROR, TAG, "error");
                return;
            }

            gCoapBuffer = (char *)OICMalloc(packetDataLen);
            if (NULL == gCoapBuffer)
            {
                OIC_LOG(DEBUG, TAG, "error");
                return;
            }

            OIC_LOG(DEBUG, TAG, "OUT");
            memset(gCoapBuffer, 0, packetDataLen);
            dataLen = 0;
        }

        OIC_LOG(DEBUG, TAG, "IN");
        while (CAIsBleDataAvailable())
        {
            OIC_LOG(DEBUG, TAG, "In While loop");
            gCoapBuffer[dataLen++] = CAReadBleData();
            if (dataLen == packetDataLen)
            {
                OIC_LOG(DEBUG, TAG, "Read Comp BLE Pckt");
                gCoapBuffer[dataLen] = '\0';
                if (dataLen > 0)
                {
                    OIC_LOG_V(DEBUG, TAG, "recv dataLen=%d", dataLen);
                    CANotifyCallback((void *)gCoapBuffer, dataLen, "", 0);
                }
                dataLen = 0;
                OICFree(gCoapBuffer);
                gCoapBuffer = NULL;
                break;
            }
        }
        OIC_LOG(DEBUG, TAG, "OUT");
    }
    else
    {
        OIC_LOG(DEBUG, TAG, "NoData");
    }
    return;
}


