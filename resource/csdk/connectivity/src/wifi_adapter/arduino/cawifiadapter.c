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

#include "cawifiadapter_singlethread.h"

#include <stdio.h>
#include <stdint.h>
#include "caadapterutils.h"
#include "logger.h"

#include "cawifiadapterutils.h"

/**
 * @def WIFI_ETHERNET_ADAPTER_TAG
 * @brief Logging tag for module name
 */
#define WIFI_ADAPTER_TAG "WIFIADAP"

/**
 * @def CA_PORT
 * @brief Port to listen for incoming data
 */
#define CA_PORT   5383

#define CA_MCAST_PORT   5683

/**
 * @def CA_MULTICAST_IP
 * @brief Multicast IP Address
 */
#define CA_MULTICAST_IP "224.0.1.187"

/**
 * @var gNetworkCallback
 * @brief Network callback
 */
static CANetworkChangeCallback gNetworkCallback = NULL;

/**
 * @var gIsMulticastServerStarted
 * @brief Flag to check if multicast server is started
 */
static bool gIsMulticastServerStarted = false;

CAResult_t CAWIFIRegisterNetworkNotifications(CANetworkChangeCallback netCallback)
{
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "IN");

    gNetworkCallback = netCallback;

    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAInitializeWifi(CARegisterConnectivityCallback registerCallback,
                            CANetworkPacketReceivedCallback networkPacketCallback, CANetworkChangeCallback netCallback)
{
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "IN");
    VERIFY_NON_NULL(registerCallback, WIFI_ADAPTER_TAG, "registerCallback");
    VERIFY_NON_NULL(networkPacketCallback, WIFI_ADAPTER_TAG, "networkPacketCallback");
    VERIFY_NON_NULL(netCallback, WIFI_ADAPTER_TAG, "netCallback");

    CAConnectivityHandler_t wifiHandler;
    wifiHandler.startAdapter = CAStartWIFI;
    wifiHandler.startListenServer = CAStartWIFIListeningServer;
    wifiHandler.startDiscoverServer = CAStartWIFIDiscoveryServer;
    wifiHandler.sendData = CASendWIFIUnicastData;
    wifiHandler.sendDataToAll = CASendWIFIMulticastData;
    wifiHandler.GetnetInfo = CAGetWIFIInterfaceInformation;
    wifiHandler.readData = CAReadWIFIData;
    wifiHandler.stopAdapter = CAStopWIFI;
    wifiHandler.terminate = CATerminateWIfI;
    registerCallback(wifiHandler, CA_WIFI);

    CASetWIFINetworkPacketCallback(networkPacketCallback);
    CAWIFIRegisterNetworkNotifications(netCallback);

    OIC_LOG(INFO, WIFI_ADAPTER_TAG, "IntializeWifi is Success");

    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAStartWIFI()
{
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "IN");
    int16_t unicastPort = CA_PORT;
    CAResult_t ret = CA_STATUS_OK;

    /*Address is hardcoded as we are using Single Interface*/
    ret = CAStartUnicastServer("0.0.0.0", &unicastPort);

    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "OUT");
    return ret;
}

CAResult_t CAStartWIFIListeningServer()
{
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "IN");

    CAResult_t ret = CA_STATUS_OK;
    int16_t multicastPort = CA_MCAST_PORT;

    if (gIsMulticastServerStarted == true)
    {
        OIC_LOG(ERROR, WIFI_ADAPTER_TAG, "Server is already Started");
        return CA_SERVER_STARTED_ALREADY;
    }

    ret = CAStartMulticastServer(CA_MULTICAST_IP, "0.0.0.0", &multicastPort);
    if (ret == CA_STATUS_OK)
    {
        OIC_LOG(INFO, WIFI_ADAPTER_TAG, "Multicast Server is Started Successfully");
        gIsMulticastServerStarted = true;
    }

    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "OUT");
    return ret;
}

CAResult_t CAStartWIFIDiscoveryServer()
{
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "IN");

    CAResult_t ret = CA_STATUS_OK;
    int16_t multicastPort = CA_MCAST_PORT;

    if (gIsMulticastServerStarted == true)
    {
        OIC_LOG(ERROR, WIFI_ADAPTER_TAG, "Server is already Started");
        return CA_SERVER_STARTED_ALREADY;
    }

    ret = CAStartMulticastServer(CA_MULTICAST_IP, "0.0.0.0", &multicastPort);
    if (ret == CA_STATUS_OK)
    {
        OIC_LOG(INFO, WIFI_ADAPTER_TAG, "Multicast Server is Started Successfully");
        gIsMulticastServerStarted = true;
    }

    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "OUT");
    return ret;
}

uint32_t CASendWIFIUnicastData(const CARemoteEndpoint_t *remoteEndpoint, void *data,
                               uint32_t dataLen)
{
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "IN");

    uint32_t dataSize = 0;

    VERIFY_NON_NULL_RET(remoteEndpoint, WIFI_ADAPTER_TAG, "remoteEndpoint",
                        dataSize);
    VERIFY_NON_NULL_RET(data, WIFI_ADAPTER_TAG, "data", dataSize);

    if (dataLen == 0)
    {
        OIC_LOG(ERROR, WIFI_ADAPTER_TAG, "DataLen is 0");
        return dataSize;

    }

    dataSize = CAWIFISendData(remoteEndpoint->addressInfo.IP.ipAddress,
                              remoteEndpoint->addressInfo.IP.port, data, dataLen, 0);

    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "OUT");
    return dataSize;
}

uint32_t CASendWIFIMulticastData(void *data, uint32_t dataLength)
{
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "IN");
    VERIFY_NON_NULL_RET(data, WIFI_ADAPTER_TAG, "data", 0);

    uint32_t dataSize = 0;
    if (dataLength == 0)
    {
        OIC_LOG(ERROR, WIFI_ADAPTER_TAG, "DataLen is 0");
        return 0;

    }
    dataSize = CAWIFISendData(CA_MULTICAST_IP, CA_MCAST_PORT, data, dataLength, 1);

    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "OUT");
    return dataSize;
}

CAResult_t CAStartWIFINotifyRecvServers()
{
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "IN");

    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

uint32_t CASendWIFINotification(const CARemoteEndpoint_t *endpoint, void *data, uint32_t dataLen)
{
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "IN");

    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAGetWIFIInterfaceInformation(CALocalConnectivity_t **info, uint32_t *size)
{
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "IN");

    VERIFY_NON_NULL(info, WIFI_ADAPTER_TAG, "info");
    char localIpAddress[CA_IPADDR_SIZE];
    int32_t localIpAddressLen = sizeof(localIpAddress);

    CAArduinoGetInterfaceAddress(localIpAddress, localIpAddressLen);
    /**
     * Create local endpoint using util function
     */
    (*info) = CAAdapterCreateLocalEndpoint(CA_WIFI, localIpAddress, "WiFi");
    if (NULL == (*info))
    {
        OIC_LOG(ERROR, WIFI_ADAPTER_TAG, "Out of memory");
        return CA_MEMORY_ALLOC_FAILED;
    }

    (*size) = 1;

    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAReadWIFIData()
{
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "IN");
    CAArduoinoCheckServerData();
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAStopWIFI()
{
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "IN");
    CAResult_t result = CA_STATUS_FAILED;
    result = CAStopUnicastServer();
    if (result != CA_STATUS_OK)
    {
        OIC_LOG(ERROR, WIFI_ADAPTER_TAG, "Error");
        return result;
    }

    result = CAStopMulticastServer();
    if (result != CA_STATUS_OK)
    {
        OIC_LOG(ERROR, WIFI_ADAPTER_TAG, "Error");
        return result;
    }
    else
    {
        gIsMulticastServerStarted = false;
    }
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CATerminateWIfI()
{
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "IN");

    CASetWIFINetworkPacketCallback(NULL);
    CAResult_t result = CAWIFIRegisterNetworkNotifications(NULL);
    if (result != CA_STATUS_OK)
    {
        OIC_LOG(ERROR, WIFI_ADAPTER_TAG, "Error");
    }

    OIC_LOG(INFO, WIFI_ADAPTER_TAG, "TerminateWifi Success");

    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "OUT");
    return;
}
