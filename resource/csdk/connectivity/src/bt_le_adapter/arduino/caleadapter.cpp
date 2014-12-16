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

#include <TimedAction.h>

#include "caleinterface_singlethread.h"
#include "caleserver.h"
#include "logger.h"
#include "caadapterutils.h"
#include "camsgparser.h"

static CANetworkChangeCallback networkCallback = NULL;
static bool gServerRunning = false;
#define BLE_ADDRESS "DB:F7:EB:B5:0F:07"
#define TAG "CALEADAPTER"

#define COAP_MAX_PDU_SIZE 320

static CANetworkPacketReceivedCallback gRespCallback;
static char *gCoapBuffer = NULL;
static int32_t dataLen = 0;
static uint16_t packetDataLen = 0;

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

static TimedAction gRcvAction = TimedAction(2000, CACheckData);


CAResult_t CAInitializeLE(CARegisterConnectivityCallback registerCallback,
                          CANetworkPacketReceivedCallback reqRespCallback,
                          CANetworkChangeCallback netCallback)
{
    OIC_LOG(DEBUG, TAG, "IN");
    if (NULL == registerCallback || NULL == reqRespCallback || NULL == netCallback)
    {
        OIC_LOG(ERROR, TAG, "Invalid Parameter");
        return CA_STATUS_INVALID_PARAM;
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
    CABleDoEvents();
    gRcvAction.check();
    return CA_STATUS_OK;
}

void CATerminateLE()
{
    OIC_LOG(DEBUG, TAG, "IN");
    gRespCallback = NULL;
    LERegisterNetworkNotifications(NULL);
    gRcvAction.disable();
    CAStopBleGattServer();
    OIC_LOG(DEBUG, TAG, "OUT");
    return;
}
CAResult_t CAStartLEListeningServer()
{
    OIC_LOG(DEBUG, TAG, "IN");
    CAInitializeBle();
    gRcvAction.enable();
    CABleDoEvents();
    gRcvAction.check();
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
    return CA_STATUS_OK;
}

CAResult_t CAUpdateCharacteristicsInGattServer(const char *char_value,
        const uint32_t value_length)
{
    // Only 1 characteristic will be used for sending data from server
    OIC_LOG(DEBUG, TAG, "IN");

    char *header = NULL;
    header = (char *) OICMalloc(sizeof(char) * CA_HEADER_LENGTH);
    if (NULL == header)
    {
        OIC_LOG(DEBUG, TAG, "error");
        return CA_STATUS_FAILED;
    }

    memset(header, 0, sizeof(char) * CA_HEADER_LENGTH);
    CAResult_t result = CAGenerateHeader(header, value_length);

    if (CA_STATUS_OK != result)
    {
         return CA_STATUS_FAILED;
    }

    int32_t index = 0;

    if(!CAIsBleConnected())
    {
        OIC_LOG(DEBUG, TAG, "ble is not connected");
        return CA_STATUS_FAILED;
    }

    CAWriteBleData((unsigned char *)header, CA_HEADER_LENGTH);
    int32_t iter = value_length/CA_SUPPORTED_BLE_MTU_SIZE;

    for (index = 0; index < iter; index++)
    {
        CAWriteBleData((unsigned char *)(char_value + (index * CA_SUPPORTED_BLE_MTU_SIZE)),
                                                   (unsigned char) CA_SUPPORTED_BLE_MTU_SIZE);
        CABleDoEvents();
    }

    CAWriteBleData((unsigned char *)(char_value + (index * CA_SUPPORTED_BLE_MTU_SIZE)),
                                    (unsigned char) value_length%CA_SUPPORTED_BLE_MTU_SIZE);
    CABleDoEvents();
    OIC_LOG(DEBUG, TAG, "ble_write_bytes had been done");
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

uint32_t CASendLEUnicastData(const CARemoteEndpoint_t *remoteEndpoint, void *data,
                                        uint32_t dataLen)
{
    OIC_LOG(DEBUG, TAG, "IN");
    if (NULL == remoteEndpoint)
    {
        OIC_LOG(ERROR, TAG, "Invalid Input Parameter");
        return CA_STATUS_INVALID_PARAM;
    }
    CAUpdateCharacteristicsInGattServer((char *)data, dataLen);
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
    CAUpdateCharacteristicsInGattServer((char *)data, dataLen);
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
    (*info) = CAAdapterCreateLocalEndpoint(CA_LE, BLE_ADDRESS);
    if (NULL == (*info))
    {
        OIC_LOG(DEBUG, TAG, "Out of memory");
        return CA_MEMORY_ALLOC_FAILED;
    }

    (*size) = 1;
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
    gServerRunning = true;
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAStopBleGattServer()
{
    // Not Supported
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
    OIC_LOG(DEBUG, TAG, "IN");
    CABleDoEvents();

    if (CAIsBleDataAvailable())
    {
        // Allocate Memory for COAP Buffer and do ParseHeader
        if (NULL == gCoapBuffer)
        {
            OIC_LOG(DEBUG, TAG, "IN");
            gCoapBuffer = (char *)OICMalloc(COAP_MAX_PDU_SIZE);
            if (NULL == gCoapBuffer)
            {
                OIC_LOG(DEBUG, TAG, "error");
                return;
            }
            while (CAIsBleDataAvailable() && dataLen < 2)
            {
                gCoapBuffer[dataLen++] = CAReadBleData();
            }

            packetDataLen = CAParseHeader(gCoapBuffer);
            OIC_LOG(DEBUG, TAG, "OUT");
            memset(gCoapBuffer, 0, COAP_MAX_PDU_SIZE);
            dataLen = 0;
        }

        OIC_LOG(DEBUG, TAG, "IN");
        while (CAIsBleDataAvailable())
        {
            OIC_LOG(DEBUG, TAG, "Inside While loop");
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
        OIC_LOG(DEBUG, TAG, "No Data on BLE server");
    }
    return;
}


