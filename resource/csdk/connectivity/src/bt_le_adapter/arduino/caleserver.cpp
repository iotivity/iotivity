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

#include "caleserver.h"
#include <logger.h>
#include "oic_malloc.h"
#include <camsgparser.h>

#define COAP_MAX_PDU_SIZE 320

static CANetworkPacketReceivedCallback gRespCallback;
static bool gServerRunning = false;
static TimedAction gRcvAction = TimedAction(2000, CACheckData);
static char *gCoapBuffer = NULL;
static int32_t dataLen = 0;
static uint16_t packetDataLen = 0;

#define TAG "CLS"

CAResult_t CAInitializeBle()
{
    OIC_LOG(DEBUG, TAG, "IN");

    // Set your BLE Shield name here, max. length 10
    ble_set_name("SAMSUNG");

    OIC_LOG(DEBUG, TAG, "BLE Name Set is completed");

    ble_begin();
    gRcvAction.enable();

    OIC_LOG(DEBUG, TAG, "OUT");
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
    dataLen = 0;
    ble_do_events();

    if (ble_available())
    {
        // Allocate Memory for COAP Buffer and do ParseHeader
        if (NULL == gCoapBuffer)
        {
            OIC_LOG(DEBUG, TAG, "IN");
            gCoapBuffer = (char *)OICMalloc(COAP_MAX_PDU_SIZE);
            while (ble_available() && dataLen < 2)
            {
                gCoapBuffer[dataLen++] = (char)ble_read();
            }

            packetDataLen = CAParseHeader(gCoapBuffer);
            OIC_LOG(DEBUG, TAG, "OUT");
            memset(gCoapBuffer, 0, COAP_MAX_PDU_SIZE);
            dataLen = 0;
        }

        OIC_LOG(DEBUG, TAG, "IN");
        while (ble_available())
        {
            OIC_LOG(DEBUG, TAG, "Inside While loop");
            gCoapBuffer[dataLen++] = (char)ble_read();
            if (dataLen == packetDataLen)
            {
                OIC_LOG(DEBUG, TAG, "Read Comp BLE Pckt");
                gCoapBuffer[dataLen] = '\0';
                if (dataLen > 0)
                {
                    OIC_LOG_V(DEBUG, TAG, "recv dataLen=%d", dataLen);
                    CANotifyCallback((void *)gCoapBuffer, dataLen, "", 0);
                }
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
}

void CAArduoinoCheckServerData()
{
    ble_do_events();
    gRcvAction.check();
}

void CASetBLEReqRescallback(CANetworkPacketReceivedCallback callback)
{
    OIC_LOG(DEBUG, TAG, "IN");
    gRespCallback = callback;
    OIC_LOG(DEBUG, TAG, "OUT");
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

CAResult_t CAAddNewBleServiceInGattServer(const char *service_uuid)
{
    // Not Supported - Done at compile time
    return CA_STATUS_OK;
}

CAResult_t CARemoveBleServiceFromGattServer(const char *svc_path)
{
    // Not Supported - Done at compile time
    return CA_STATUS_OK;
}

CAResult_t CARemoveAllBleServicesFromGattServer()
{
    // Not Supported
    return CA_STATUS_OK;
}

CAResult_t CARegisterBleServicewithGattServer(const char *svc_path)
{
    // Not Supported - Done at compile time
    return CA_STATUS_OK;
}

CAResult_t CAAddNewCharacteristicsToGattServer(const char *svc_path, const char *char_uuid,
        const char *char_value,
        int char_value_len,
        int read)
{
    // Not Supported - Done at compile time
    return CA_STATUS_OK;
}

CAResult_t CARemoveCharacteristicsFromGattServer(const char *char_path)
{
    // Not Supported
    return CA_STATUS_OK;
}

CAResult_t CAUpdateCharacteristicsInGattServer(const char *char_path,
                            const char *char_value, int value_length)
{
    // Only 1 characteristic will be used for sending data from server
    OIC_LOG(DEBUG, TAG, "IN");

    char *header = NULL;
    header = (char *) OICMalloc(sizeof(char) * CA_HEADER_LENGTH);
    memset(header, 0, sizeof(char) * CA_HEADER_LENGTH);
    CAResult_t result = CAGenerateHeader(header, value_length);
    if (CA_STATUS_OK != result)
    {
         return CA_STATUS_FAILED;
    }
    int index = 0;
    if (ble_connected())
    {
        ble_write_bytes((unsigned char *)header, CA_HEADER_LENGTH);
        int iter = value_length/CA_SUPPORTED_BLE_MTU_SIZE;

        for (index = 0; index < iter; index++)
        {
            ble_write_bytes((unsigned char *)(char_value + (index * CA_SUPPORTED_BLE_MTU_SIZE)),
                                                       (unsigned char) CA_SUPPORTED_BLE_MTU_SIZE);
            ble_do_events();
        }

        ble_write_bytes((unsigned char *)(char_value + (index * CA_SUPPORTED_BLE_MTU_SIZE)),
                                        (unsigned char) value_length%CA_SUPPORTED_BLE_MTU_SIZE);
        ble_do_events();
        OIC_LOG(DEBUG, TAG,
                "ble_write_bytes had been done");
    }
    else
    {
        OIC_LOG(DEBUG, TAG, "ble is not connected");
        return CA_STATUS_FAILED;
    }
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}
