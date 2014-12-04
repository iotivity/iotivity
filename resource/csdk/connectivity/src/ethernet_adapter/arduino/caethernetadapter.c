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

#include "caethernetadapter_singlethread.h"

#include <stdio.h>
#include <stdint.h>
#include "caadapterutils.h"
#include "logger.h"

#include "caethernetadapterutils.h"

/**
 * @def ETHERNET_ADAPTER_TAG
 * @brief Logging tag for module name
 */
#define ETHERNET_ADAPTER_TAG "EA"

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
static int gIsMulticastServerStarted = 0;

int CAEthernetRegisterNetworkNotifications(CANetworkChangeCallback netCallback)
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");

    gNetworkCallback = netCallback;

    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAInitializeEthernet(CARegisterConnectivityCallback registerCallback,
                                CANetworkPacketReceivedCallback networkPacketCallback, CANetworkChangeCallback netCallback)
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");
    VERIFY_NON_NULL(registerCallback, ETHERNET_ADAPTER_TAG, "registercallback");
    VERIFY_NON_NULL(networkPacketCallback, ETHERNET_ADAPTER_TAG, "networkpacketcallback");
    VERIFY_NON_NULL(netCallback, ETHERNET_ADAPTER_TAG, "netcallback");

    CAConnectivityHandler_t ethernetHandler;
    ethernetHandler.startAdapter = CAStartEthernet;
    ethernetHandler.startListenServer = CAStartEthernetListeningServer;
    ethernetHandler.startDiscoverServer = CAStartEthernetDiscoveryServer;
    ethernetHandler.sendData = CASendEthernetUnicastData;
    ethernetHandler.sendDataToAll = CASendEthernetMulticastData;
    ethernetHandler.GetnetInfo = CAGetEthernetInterfaceInformation;
    ethernetHandler.readData = CAReadEthernetData;
    ethernetHandler.stopAdapter = CAStopEthernet;
    ethernetHandler.terminate = CATerminateEthernet;
    registerCallback(ethernetHandler, CA_ETHERNET);

    CASetEthernetNetworkPacketCallback(networkPacketCallback);
    CAEthernetRegisterNetworkNotifications(netCallback);

    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAStartEthernet()
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");
    int16_t unicastPort = CA_PORT;
    CAResult_t ret = CA_STATUS_OK;

    /*Address is hardcoded as we are using Single Interface*/
    ret = CAStartUnicastServer("0.0.0.0", &unicastPort);

    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "OUT");
    return ret;
}

CAResult_t CAStartEthernetListeningServer()
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");

    CAResult_t ret = CA_STATUS_OK;
    int16_t multicastPort = CA_MCAST_PORT;

    if (gIsMulticastServerStarted == 1)
    {
        OIC_LOG(ERROR, ETHERNET_ADAPTER_TAG, "Already started");
        return CA_SERVER_STARTED_ALREADY;
    }

    ret = CAStartMulticastServer(CA_MULTICAST_IP, "0.0.0.0", &multicastPort);
    if (ret == CA_STATUS_OK)
    {
        OIC_LOG(INFO, ETHERNET_ADAPTER_TAG, "success");
        gIsMulticastServerStarted = 1;
    }

    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "OUT");
    return ret;
}

CAResult_t CAStartEthernetDiscoveryServer()
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "OUT");
    return CAStartEthernetListeningServer();
}

uint32_t CASendEthernetUnicastData(const CARemoteEndpoint_t *remoteEndpoint, void *data,
                                   uint32_t dataLen)
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");

    uint32_t dataSize = 0;

    VERIFY_NON_NULL_RET(remoteEndpoint, ETHERNET_ADAPTER_TAG, "remoteEndpoint",
                        dataSize);
    VERIFY_NON_NULL_RET(data, ETHERNET_ADAPTER_TAG, "data", dataSize);

    if (dataLen <= 0)
    {
        OIC_LOG(ERROR, ETHERNET_ADAPTER_TAG, "data");
        return dataSize;

    }

    dataSize = CAEthernetSendData(remoteEndpoint->addressInfo.IP.ipAddress,
                                  remoteEndpoint->addressInfo.IP.port, data, dataLen, 0);

    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "OUT");
    return dataSize;
}

uint32_t CASendEthernetMulticastData(void *data, uint32_t dataLength)
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");

    uint32_t dataSize = 0;

    VERIFY_NON_NULL_RET(data, ETHERNET_ADAPTER_TAG, "data", dataSize);

    if (dataLength <= 0)
    {
        OIC_LOG(ERROR, ETHERNET_ADAPTER_TAG, "data");
        return 0;

    }
    dataSize = CAEthernetSendData(CA_MULTICAST_IP, CA_MCAST_PORT, data, dataLength, 1);

    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "OUT");
    return dataSize;
}

CAResult_t CAStartEthernetNotifyRecvServers()
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");

    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

uint32_t CASendEthernetNotification(const CARemoteEndpoint_t *endpoint, void *data,
                                    uint32_t dataLen)
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");

    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAGetEthernetInterfaceInformation(CALocalConnectivity_t **info, uint32_t *size)
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");

    VERIFY_NON_NULL(info, ETHERNET_ADAPTER_TAG, "info");
    char localIpAddress[CA_IPADDR_SIZE];
    int32_t localIpAddressLen = sizeof(localIpAddress);

    CAArduinoGetInterfaceAddress(localIpAddress, localIpAddressLen);
    /**
     * Create local endpoint using util function
     */
    (*info) = CAAdapterCreateLocalEndpoint(CA_ETHERNET, localIpAddress, "Ethernet");
    if (NULL == (*info))
    {
        OIC_LOG(ERROR, ETHERNET_ADAPTER_TAG, "Out of memory");
        return CA_MEMORY_ALLOC_FAILED;
    }

    (*size) = 1;

    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAReadEthernetData()
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");
    CAArduoinoCheckServerData();
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAStopEthernet()
{
    CAResult_t result = CA_STATUS_FAILED;
    result = CAStopUnicastServer();
    if (result != CA_STATUS_OK)
    {
        OIC_LOG(ERROR, ETHERNET_ADAPTER_TAG, "unicast");
        return result;
    }

    result = CAStopMulticastServer();
    if (result != CA_STATUS_OK)
    {
        OIC_LOG(ERROR, ETHERNET_ADAPTER_TAG, "multicast");
        return result;
    }
    else
    {
        gIsMulticastServerStarted = 0;
    }
    return CA_STATUS_OK;
}

void CATerminateEthernet()
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");
    CAResult_t result = CA_STATUS_FAILED;

    CASetEthernetNetworkPacketCallback(NULL);
    result = CAEthernetRegisterNetworkNotifications(NULL);
    if (result != CA_STATUS_OK)
    {
        OIC_LOG(ERROR, ETHERNET_ADAPTER_TAG, "register failed");
    }

    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "OUT");
    return;
}

