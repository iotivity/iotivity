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
#include "oic_malloc.h"
#include "caethernetinterface_singlethread.h"

/**
 * @def ETHERNET_ADAPTER_TAG
 * @brief Logging tag for module name
 */
#define ETHERNET_ADAPTER_TAG "EA"

/**
 * @def CA_PORT
 * @brief Unicast port number (to listen for incoming data on unicast server).
 * Note :- Actual port number may differ based on result of bind() operation.
 */
#define CA_PORT   6298

/**
 * @def CA_SECURE_PORT
 * @brief Secured (unicast) port number as defined in COAP Specification, RFC-7252.
 */
#define CA_SECURE_PORT   5684

/**
 * @def CA_MCAST_PORT
 * @brief Multicast port number as defined in COAP Specification, RFC-7252.
 */
#define CA_MCAST_PORT   5683

/**
 * @def CA_MULTICAST_IP
 * @brief Multicast IP Address
 */
#define CA_MULTICAST_IP "224.0.1.187"

/* Skip Queue */
/**
 * @var gNetworkPacketCallback
 * @brief Network Packet Received Callback to CA
 */
static CANetworkPacketReceivedCallback gNetworkPacketCallback = NULL;

/**
 * @var gNetworkChangeCb
 * @brief Network Changed Callback to CA
 */

static CANetworkChangeCallback gNetworkChangeCallback = NULL;

/**
 * @var gIsMulticastServerStarted
 * @brief Flag to check if multicast server is started
 */
static bool gIsMulticastServerStarted = false;

/**
 * @var gIsStartServerCalled
 * @brief Flag to check if server start requested by CA.
 */
static bool gStartUnicastServerRequested = false;

/**
 * @var gUnicastServerport
 * @brief port number on which unicast server is running.
 */
static uint16_t gUnicastServerport = 0;

/**
 * @var gIsStartServerCalled
 * @brief Flag to check if server start requested by CA.
 */
static bool gStartMulticastServerRequested = false;


static void CAEthernetNotifyNetworkChange(const char *address, int16_t port,
                                          CANetworkStatus_t status);
static void CAEthernetConnectionStateCB(const char *ipAddress,
                                        CANetworkStatus_t status);
static void CAEthernetPacketReceivedCB(const char *ipAddress, uint32_t port,
                                       const void *data, uint32_t dataLength);
static CAResult_t CAEthernetStopServers();

void CAEthernetNotifyNetworkChange(const char *address, int16_t port,
                                   CANetworkStatus_t status)
{
    CALocalConnectivity_t *localEndpoint = CAAdapterCreateLocalEndpoint(CA_ETHERNET, address);
    if (!localEndpoint)
    {
        OIC_LOG(ERROR, ETHERNET_ADAPTER_TAG, "Out of memory!");
        return;
    }
    localEndpoint->addressInfo.IP.port = port;

    if (NULL != gNetworkChangeCallback)
    {
        gNetworkChangeCallback(localEndpoint, status);
    }

    CAAdapterFreeLocalEndpoint(localEndpoint);
}

void CAEthernetConnectionStateCB(const char *ipAddr,
                                 CANetworkStatus_t status)
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");

    CAResult_t ret = CA_STATUS_FAILED;
    /* If Ethernet is connected, then get the latest IP from the Ethernet Interface
      * and start unicast and multicast servers if requested earlier */
    if (CA_INTERFACE_UP == status)
    {
        uint16_t port = CA_PORT;
        int32_t serverFd = -1;
        /* Start Unicast server if requested earlier */
        if (gStartUnicastServerRequested)
        {
            ret = CAEthernetStartUnicastServer("0.0.0.0", &port, false, &serverFd);
            if (CA_STATUS_OK == ret)
            {
                OIC_LOG_V(DEBUG, ETHERNET_ADAPTER_TAG, "unicast started:%d", port);
                CAEthernetSetUnicastSocket(serverFd);
                CAEthernetSetUnicastPort(port);
                gUnicastServerport = port;
            }
            else
            {
                OIC_LOG_V(ERROR, ETHERNET_ADAPTER_TAG, "FAILED:%d", ret);
            }
        }

        /* Start Multicast server if requested earlier */
        if (gStartMulticastServerRequested)
        {
            int16_t multicastPort = CA_MCAST_PORT;
            ret = CAEthernetStartMulticastServer("0.0.0.0", CA_MULTICAST_IP, multicastPort, &serverFd);
            if (CA_STATUS_OK == ret)
            {
                OIC_LOG_V(DEBUG, ETHERNET_ADAPTER_TAG, "multicast started:%d", multicastPort);
                gIsMulticastServerStarted = true;
            }
            else
            {
                OIC_LOG_V(ERROR, ETHERNET_ADAPTER_TAG, "strt mcast srv fail:%d", ret);
            }
        }

        char *ipAddress = NULL;
        char *ifcName = NULL;
        CAResult_t ret = CAEthernetGetInterfaceInfo(&ifcName, &ipAddress);
        if (CA_STATUS_OK != ret)
        {
            OIC_LOG_V(ERROR, ETHERNET_ADAPTER_TAG, "get interface info fail:%d", ret);
            OICFree(ipAddress);
            OICFree(ifcName);
            return;
        }
        /* Notify network change to CA */
        CAEthernetNotifyNetworkChange(ipAddress, port, status);
        OICFree(ipAddress);
        OICFree(ifcName);
    }
    else
    {
        CAEthernetNotifyNetworkChange("", 0, status);
        /* Stop both Unicast and Multicast servers */
        ret = CAEthernetStopServers();
        if (CA_STATUS_OK != ret)
        {
            OIC_LOG_V(ERROR, ETHERNET_ADAPTER_TAG, "stop srv fail:%d", ret);
            return;
        }
    }

    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "OUT");
}

void CAEthernetPacketReceivedCB(const char *ipAddress, uint32_t port,
                                const void *data, uint32_t dataLength)
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");
    OIC_LOG_V(DEBUG, ETHERNET_ADAPTER_TAG, "sddress:%s", ipAddress);
    OIC_LOG_V(DEBUG, ETHERNET_ADAPTER_TAG, "port:%s", port);
    OIC_LOG_V(DEBUG, ETHERNET_ADAPTER_TAG, "data:%s", data);

    /* CA is freeing this memory */
    CARemoteEndpoint_t *endPoint = CAAdapterCreateRemoteEndpoint(CA_ETHERNET, ipAddress, NULL);
    if (NULL == endPoint)
    {
        OIC_LOG(ERROR, ETHERNET_ADAPTER_TAG, "Out of memory!");
        return;
    }
    endPoint->addressInfo.IP.port = port;

    if (gNetworkPacketCallback)
    {
        gNetworkPacketCallback(endPoint, data, dataLength);
    }
    CAAdapterFreeRemoteEndpoint(endPoint);
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "OUT");
}

CAResult_t CAInitializeEthernet(CARegisterConnectivityCallback registerCallback,
                                CANetworkPacketReceivedCallback networkPacketCallback,
                                CANetworkChangeCallback netCallback)
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");
    VERIFY_NON_NULL(registerCallback, ETHERNET_ADAPTER_TAG, "registerCallback");
    VERIFY_NON_NULL(networkPacketCallback, ETHERNET_ADAPTER_TAG, "networkPacketCallback");
    VERIFY_NON_NULL(netCallback, ETHERNET_ADAPTER_TAG, "netCallback");

    gNetworkChangeCallback = netCallback;
    gNetworkPacketCallback = networkPacketCallback;

    CAResult_t ret = CAEthernetInitializeNetworkMonitor();
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(ERROR, ETHERNET_ADAPTER_TAG, "init n/w fail:%d", ret);
        return ret;
    }
    CAEthernetSetConnectionStateChangeCallback(CAEthernetConnectionStateCB);

    ret = CAEthernetInitializeServer();
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(ERROR, ETHERNET_ADAPTER_TAG, "init fail:%d", ret);
        CATerminateEthernet();
        return ret;
    }
    CAEthernetSetPacketReceiveCallback(CAEthernetPacketReceivedCB);

    CAConnectivityHandler_t EthernetHandler;
    EthernetHandler.startAdapter = CAStartEthernet;
    EthernetHandler.startListenServer = CAStartEthernetListeningServer;
    EthernetHandler.startDiscoveryServer = CAStartEthernetDiscoveryServer;
    EthernetHandler.sendData = CASendEthernetUnicastData;
    EthernetHandler.sendDataToAll = CASendEthernetMulticastData;
    EthernetHandler.GetnetInfo = CAGetEthernetInterfaceInformation;
    EthernetHandler.readData = CAReadEthernetData;
    EthernetHandler.stopAdapter = CAStopEthernet;
    EthernetHandler.terminate = CATerminateEthernet;
    registerCallback(EthernetHandler, CA_ETHERNET);

    OIC_LOG(INFO, ETHERNET_ADAPTER_TAG, "success");
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAStartEthernet()
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");

    /* Start monitoring Ethernet network */
    CAResult_t ret = CAEthernetStartNetworkMonitor();
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG(ERROR, ETHERNET_ADAPTER_TAG, "strt n/w monitor fail");
    }

    gStartUnicastServerRequested = true;
    bool retVal = CAEthernetIsConnected();
    if (false == retVal)
    {
        OIC_LOG(ERROR, ETHERNET_ADAPTER_TAG, "not connected");
        return ret;
    }

    uint16_t unicastPort = CA_PORT;
    int32_t serverFd = 0;
    // Address is hardcoded as we are using Single Interface
    ret = CAEthernetStartUnicastServer("0.0.0.0", &unicastPort, false, &serverFd);
    if (CA_STATUS_OK == ret)
    {
        OIC_LOG_V(DEBUG, ETHERNET_ADAPTER_TAG, "unicast started:%d", unicastPort);
        CAEthernetSetUnicastSocket(serverFd);
        CAEthernetSetUnicastPort(unicastPort);
        gUnicastServerport = unicastPort;
    }

    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "OUT");
    return ret;
}

CAResult_t CAStartEthernetListeningServer()
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");

    CAResult_t ret = CA_STATUS_OK;
    int16_t multicastPort = CA_MCAST_PORT;
    int32_t serverFD = 1;
    if (gIsMulticastServerStarted == true)
    {
        OIC_LOG(ERROR, ETHERNET_ADAPTER_TAG, "Already Started!");
        return CA_SERVER_STARTED_ALREADY;
    }

    gStartMulticastServerRequested = true;
    bool retVal = CAEthernetIsConnected();
    if (false == retVal)
    {
        OIC_LOG(ERROR, ETHERNET_ADAPTER_TAG,"Not connected");
        return CA_ADAPTER_NOT_ENABLED;
    }

    ret = CAEthernetStartMulticastServer("0.0.0.0", CA_MULTICAST_IP, multicastPort, &serverFD);
    if (CA_STATUS_OK == ret)
    {
        OIC_LOG(INFO, ETHERNET_ADAPTER_TAG, "multicast success");
        gIsMulticastServerStarted = true;
    }

    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "OUT");
    return ret;
}

CAResult_t CAStartEthernetDiscoveryServer()
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");
    /* Both listening and discovery server are same */
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "OUT");
    return CAStartEthernetListeningServer();
}

int32_t CASendEthernetUnicastData(const CARemoteEndpoint_t *remoteEndpoint, const void *data,
                                  uint32_t dataLength)
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");

    VERIFY_NON_NULL_RET(remoteEndpoint, ETHERNET_ADAPTER_TAG, "remoteEndpoint", -1);
    VERIFY_NON_NULL_RET(data, ETHERNET_ADAPTER_TAG, "data", -1);
    if (dataLength == 0)
    {
        OIC_LOG(ERROR, ETHERNET_ADAPTER_TAG, "Invalid length");
        return -1;
    }

    CAEthernetSendData(remoteEndpoint->addressInfo.IP.ipAddress,
                       remoteEndpoint->addressInfo.IP.port, data, dataLength, false);
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "OUT");
    return dataLength;
}

int32_t CASendEthernetMulticastData(const void *data, uint32_t dataLength)
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");

    VERIFY_NON_NULL_RET(data, ETHERNET_ADAPTER_TAG, "data", -1);
    if (dataLength == 0)
    {
        OIC_LOG(ERROR, ETHERNET_ADAPTER_TAG, "Invalid length");
        return -1;
    }

    CAEthernetSendData(CA_MULTICAST_IP, CA_MCAST_PORT, data, dataLength, true);
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "OUT");
    return dataLength;
}

CAResult_t CAGetEthernetInterfaceInformation(CALocalConnectivity_t **info, uint32_t *size)
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");
    VERIFY_NON_NULL(info, ETHERNET_ADAPTER_TAG, "info");
    VERIFY_NON_NULL(size, ETHERNET_ADAPTER_TAG, "size");

    bool retVal = CAEthernetIsConnected();
    if (false == retVal)
    {
        OIC_LOG(ERROR, ETHERNET_ADAPTER_TAG, "Not connected");
        return CA_ADAPTER_NOT_ENABLED;
    }

    char *ipAddress = NULL;
    char *ifcName = NULL;
    CAResult_t ret = CAEthernetGetInterfaceInfo(&ifcName, &ipAddress);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(ERROR, ETHERNET_ADAPTER_TAG, "get interface info fail:%d", ret);
        OICFree(ipAddress);
        OICFree(ifcName);
        return ret;
    }

    // Create local endpoint using util function
    (*info) = CAAdapterCreateLocalEndpoint(CA_ETHERNET, ipAddress);
    if (NULL == (*info))
    {
        OIC_LOG(ERROR, ETHERNET_ADAPTER_TAG, "malloc fail");
        OICFree(ipAddress);
        OICFree(ifcName);
        return CA_MEMORY_ALLOC_FAILED;
    }

    (*info)->addressInfo.IP.port = gUnicastServerport;
    (*size) = 1;

    OICFree(ipAddress);
    OICFree(ifcName);

    OIC_LOG(INFO, ETHERNET_ADAPTER_TAG, "success");
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAReadEthernetData()
{
    CAEthernetPullData();
    return CA_STATUS_OK;
}

CAResult_t CAEthernetStopServers()
{
    CAResult_t result = CAEthernetStopUnicastServer();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG_V(ERROR, ETHERNET_ADAPTER_TAG, "stop ucast srv fail:%d", result);
        return result;
    }
    CAEthernetSetUnicastSocket(-1);
    CAEthernetSetUnicastPort(0);
    gUnicastServerport = 0;

    result = CAEthernetStopMulticastServer();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG_V(ERROR, ETHERNET_ADAPTER_TAG, "stop mcast srv fail:%d", result);
        return result;
    }
    gIsMulticastServerStarted = false;

    return result;
}

CAResult_t CAStopEthernet()
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");

    gStartUnicastServerRequested = false;
    gStartMulticastServerRequested = false;
    CAEthernetStopNetworkMonitor();
    CAResult_t result = CAEthernetStopServers();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG_V(ERROR, ETHERNET_ADAPTER_TAG, "stop srv fail:%d", result);
    }

    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "OUT");
    return result;
}

void CATerminateEthernet()
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");

    CAEthernetSetConnectionStateChangeCallback(NULL);
    CAEthernetTerminateNetworkMonitor();
    CAEthernetSetPacketReceiveCallback(NULL);
    OIC_LOG(INFO, ETHERNET_ADAPTER_TAG, "Terminated Ethernet");
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "OUT");
    return;
}


