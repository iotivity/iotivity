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

#include "caipadapter_singlethread.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "caadapterutils.h"
#include "logger.h"
#include "oic_malloc.h"
#include "caipinterface_singlethread.h"

/**
 * @def TAG
 * @brief Logging tag for module name
 */
#define TAG "IPAD"

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
 * @var g_networkPacketCallback
 * @brief Network Packet Received Callback to CA
 */
static CANetworkPacketReceivedCallback g_networkPacketCallback = NULL;

/**
 * @var g_networkChangeCallback
 * @brief Network Changed Callback to CA
 */

static CANetworkChangeCallback g_networkChangeCallback = NULL;

/**
 * @var g_isMulticastServerStarted
 * @brief Flag to check if multicast server is started
 */
static bool g_isMulticastServerStarted = false;

/**
 * @var g_startUnicastServerRequested
 * @brief Flag to check if server start requested by CA.
 */
static bool g_startUnicastServerRequested = false;

/**
 * @var g_unicastServerport
 * @brief port number on which unicast server is running.
 */
static uint16_t g_unicastServerport = 0;

/**
 * @var g_startMulticastServerRequested
 * @brief Flag to check if server start requested by CA.
 */
static bool g_startMulticastServerRequested = false;


static void CAIPNotifyNetworkChange(const char *address, uint16_t port,
                                          CANetworkStatus_t status);
static void CAIPConnectionStateCB(const char *ipAddress,
                                        CANetworkStatus_t status);
static void CAIPPacketReceivedCB(const char *ipAddress, uint16_t port,
                                       const void *data, uint32_t dataLength);
static CAResult_t CAIPStopServers();

void CAIPNotifyNetworkChange(const char *address, uint16_t port, CANetworkStatus_t status)
{
    CALocalConnectivity_t *localEndpoint = CAAdapterCreateLocalEndpoint(CA_IPV4, address);
    if (!localEndpoint)
    {
        OIC_LOG(ERROR, TAG, "Out of memory!");
        return;
    }
    localEndpoint->addressInfo.IP.port = port;

    if (NULL != g_networkChangeCallback)
    {
        g_networkChangeCallback(localEndpoint, status);
    }

    CAAdapterFreeLocalEndpoint(localEndpoint);
}

void CAIPConnectionStateCB(const char *ipAddr,
                                 CANetworkStatus_t status)
{
    OIC_LOG(DEBUG, TAG, "IN");

    CAResult_t ret = CA_STATUS_FAILED;
    /* If IP is connected, then get the latest IP from the IP Interface
      * and start unicast and multicast servers if requested earlier */
    if (CA_INTERFACE_UP == status)
    {
        uint16_t port = CA_PORT;
        int32_t serverFd = -1;
        /* Start Unicast server if requested earlier */
        if (g_startUnicastServerRequested)
        {
            ret = CAIPStartUnicastServer("0.0.0.0", &port, false, &serverFd);
            if (CA_STATUS_OK == ret)
            {
                OIC_LOG_V(DEBUG, TAG, "unicast started:%d", port);
                CAIPSetUnicastSocket(serverFd);
                CAIPSetUnicastPort(port);
                g_unicastServerport = port;
            }
            else
            {
                OIC_LOG_V(ERROR, TAG, "FAILED:%d", ret);
            }
        }

        /* Start Multicast server if requested earlier */
        if (g_startMulticastServerRequested)
        {
            uint16_t multicastPort = CA_MCAST_PORT;
            ret = CAIPStartMulticastServer("0.0.0.0", CA_MULTICAST_IP, multicastPort, &serverFd);
            if (CA_STATUS_OK == ret)
            {
                OIC_LOG_V(DEBUG, TAG, "multicast started:%d", multicastPort);
                g_isMulticastServerStarted = true;
            }
            else
            {
                OIC_LOG_V(ERROR, TAG, "strt mcast srv fail:%d", ret);
            }
        }

        char *ipAddress = NULL;
        char *ifcName = NULL;
        CAResult_t ret = CAIPGetInterfaceInfo(&ifcName, &ipAddress);
        if (CA_STATUS_OK != ret)
        {
            OIC_LOG_V(ERROR, TAG, "get interface info fail:%d", ret);
            OICFree(ipAddress);
            OICFree(ifcName);
            return;
        }
        /* Notify network change to CA */
        CAIPNotifyNetworkChange(ipAddress, port, status);
        OICFree(ipAddress);
        OICFree(ifcName);
    }
    else
    {
        CAIPNotifyNetworkChange("", 0, status);
        /* Stop both Unicast and Multicast servers */
        ret = CAIPStopServers();
        if (CA_STATUS_OK != ret)
        {
            OIC_LOG_V(ERROR, TAG, "stop srv fail:%d", ret);
            return;
        }
    }

    OIC_LOG(DEBUG, TAG, "OUT");
}

void CAIPPacketReceivedCB(const char *ipAddress, uint16_t port,
                                const void *data, uint32_t dataLength)
{
    OIC_LOG(DEBUG, TAG, "IN");
    OIC_LOG_V(DEBUG, TAG, "sddress:%s", ipAddress);
    OIC_LOG_V(DEBUG, TAG, "port:%d", port);
    OIC_LOG_V(DEBUG, TAG, "data:%s", data);

    /* CA is freeing this memory */
    CARemoteEndpoint_t *endPoint = CAAdapterCreateRemoteEndpoint(CA_IPV4, ipAddress, NULL);
    if (NULL == endPoint)
    {
        OIC_LOG(ERROR, TAG, "Out of memory!");
        return;
    }
    endPoint->addressInfo.IP.port = port;

    if (g_networkPacketCallback)
    {
        g_networkPacketCallback(endPoint, data, dataLength);
    }
    CAAdapterFreeRemoteEndpoint(endPoint);
    OIC_LOG(DEBUG, TAG, "OUT");
}

CAResult_t CAInitializeIP(CARegisterConnectivityCallback registerCallback,
                                CANetworkPacketReceivedCallback networkPacketCallback,
                                CANetworkChangeCallback netCallback)
{
    OIC_LOG(DEBUG, TAG, "IN");
    VERIFY_NON_NULL(registerCallback, TAG, "registerCallback");
    VERIFY_NON_NULL(networkPacketCallback, TAG, "networkPacketCallback");
    VERIFY_NON_NULL(netCallback, TAG, "netCallback");

    g_networkChangeCallback = netCallback;
    g_networkPacketCallback = networkPacketCallback;

    CAResult_t ret = CAIPInitializeNetworkMonitor();
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(ERROR, TAG, "init n/w fail:%d", ret);
        return ret;
    }
    CAIPSetConnectionStateChangeCallback(CAIPConnectionStateCB);

    ret = CAIPInitializeServer();
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(ERROR, TAG, "init fail:%d", ret);
        CATerminateIP();
        return ret;
    }
    CAIPSetPacketReceiveCallback(CAIPPacketReceivedCB);

    CAConnectivityHandler_t IPHandler;
    IPHandler.startAdapter = CAStartIP;
    IPHandler.startListenServer = CAStartIPListeningServer;
    IPHandler.startDiscoveryServer = CAStartIPDiscoveryServer;
    IPHandler.sendData = CASendIPUnicastData;
    IPHandler.sendDataToAll = CASendIPMulticastData;
    IPHandler.GetnetInfo = CAGetIPInterfaceInformation;
    IPHandler.readData = CAReadIPData;
    IPHandler.stopAdapter = CAStopIP;
    IPHandler.terminate = CATerminateIP;
    registerCallback(IPHandler, CA_IPV4);

    OIC_LOG(INFO, TAG, "success");
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAStartIP()
{
    OIC_LOG(DEBUG, TAG, "IN");

    /* Start monitoring IP network */
    CAResult_t ret = CAIPStartNetworkMonitor();
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG(ERROR, TAG, "strt n/w monitor fail");
    }

    g_startUnicastServerRequested = true;
    bool retVal = CAIPIsConnected();
    if (false == retVal)
    {
        OIC_LOG(ERROR, TAG, "not connected");
        return ret;
    }

    uint16_t unicastPort = CA_PORT;
    int32_t serverFd = 0;
    // Address is hardcoded as we are using Single Interface
    ret = CAIPStartUnicastServer("0.0.0.0", &unicastPort, false, &serverFd);
    if (CA_STATUS_OK == ret)
    {
        OIC_LOG_V(DEBUG, TAG, "unicast started:%d", unicastPort);
        CAIPSetUnicastSocket(serverFd);
        CAIPSetUnicastPort(unicastPort);
        g_unicastServerport = unicastPort;
    }

    OIC_LOG(DEBUG, TAG, "OUT");
    return ret;
}

CAResult_t CAStartIPListeningServer()
{
    OIC_LOG(DEBUG, TAG, "IN");

    CAResult_t ret = CA_STATUS_OK;
    uint16_t multicastPort = CA_MCAST_PORT;
    int32_t serverFD = 1;
    if (g_isMulticastServerStarted == true)
    {
        OIC_LOG(ERROR, TAG, "Already Started!");
        return CA_SERVER_STARTED_ALREADY;
    }

    g_startMulticastServerRequested = true;
    bool retVal = CAIPIsConnected();
    if (false == retVal)
    {
        OIC_LOG(ERROR, TAG,"Not connected");
        return CA_ADAPTER_NOT_ENABLED;
    }

    ret = CAIPStartMulticastServer("0.0.0.0", CA_MULTICAST_IP, multicastPort, &serverFD);
    if (CA_STATUS_OK == ret)
    {
        OIC_LOG(INFO, TAG, "multicast success");
        g_isMulticastServerStarted = true;
    }

    OIC_LOG(DEBUG, TAG, "OUT");
    return ret;
}

CAResult_t CAStartIPDiscoveryServer()
{
    OIC_LOG(DEBUG, TAG, "IN");
    /* Both listening and discovery server are same */
    OIC_LOG(DEBUG, TAG, "OUT");
    return CAStartIPListeningServer();
}

int32_t CASendIPUnicastData(const CARemoteEndpoint_t *remoteEndpoint, const void *data,
                                  uint32_t dataLength)
{
    OIC_LOG(DEBUG, TAG, "IN");

    VERIFY_NON_NULL_RET(remoteEndpoint, TAG, "remoteEndpoint", -1);
    VERIFY_NON_NULL_RET(data, TAG, "data", -1);
    if (dataLength == 0)
    {
        OIC_LOG(ERROR, TAG, "Invalid length");
        return -1;
    }

    CAIPSendData(remoteEndpoint->addressInfo.IP.ipAddress,
                       remoteEndpoint->addressInfo.IP.port, data, dataLength, false);
    OIC_LOG(DEBUG, TAG, "OUT");
    return dataLength;
}

int32_t CASendIPMulticastData(const void *data, uint32_t dataLength)
{
    OIC_LOG(DEBUG, TAG, "IN");

    VERIFY_NON_NULL_RET(data, TAG, "data", -1);
    if (dataLength == 0)
    {
        OIC_LOG(ERROR, TAG, "Invalid length");
        return -1;
    }

    CAIPSendData(CA_MULTICAST_IP, CA_MCAST_PORT, data, dataLength, true);
    OIC_LOG(DEBUG, TAG, "OUT");
    return dataLength;
}

CAResult_t CAGetIPInterfaceInformation(CALocalConnectivity_t **info, uint32_t *size)
{
    OIC_LOG(DEBUG, TAG, "IN");
    VERIFY_NON_NULL(info, TAG, "info");
    VERIFY_NON_NULL(size, TAG, "size");

    bool retVal = CAIPIsConnected();
    if (false == retVal)
    {
        OIC_LOG(ERROR, TAG, "Not connected");
        return CA_ADAPTER_NOT_ENABLED;
    }

    char *ipAddress = NULL;
    char *ifcName = NULL;
    CAResult_t ret = CAIPGetInterfaceInfo(&ipAddress, &ifcName);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(ERROR, TAG, "get interface info fail:%d", ret);
        OICFree(ipAddress);
        OICFree(ifcName);
        return ret;
    }

    // Create local endpoint using util function
    (*info) = CAAdapterCreateLocalEndpoint(CA_IPV4, ipAddress);
    if (NULL == (*info))
    {
        OIC_LOG(ERROR, TAG, "malloc fail");
        OICFree(ipAddress);
        OICFree(ifcName);
        return CA_MEMORY_ALLOC_FAILED;
    }

    (*info)->addressInfo.IP.port = g_unicastServerport;
    (*size) = 1;

    OICFree(ipAddress);
    OICFree(ifcName);

    OIC_LOG(INFO, TAG, "success");
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAReadIPData()
{
    CAIPPullData();
    return CA_STATUS_OK;
}

CAResult_t CAIPStopServers()
{
    CAResult_t result = CAIPStopUnicastServer();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG_V(ERROR, TAG, "stop ucast srv fail:%d", result);
        return result;
    }
    CAIPSetUnicastSocket(-1);
    CAIPSetUnicastPort(0);
    g_unicastServerport = 0;

    result = CAIPStopMulticastServer();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG_V(ERROR, TAG, "stop mcast srv fail:%d", result);
        return result;
    }
    g_isMulticastServerStarted = false;

    return result;
}

CAResult_t CAStopIP()
{
    OIC_LOG(DEBUG, TAG, "IN");

    g_startUnicastServerRequested = false;
    g_startMulticastServerRequested = false;
    CAIPStopNetworkMonitor();
    CAResult_t result = CAIPStopServers();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG_V(ERROR, TAG, "stop srv fail:%d", result);
    }

    OIC_LOG(DEBUG, TAG, "OUT");
    return result;
}

void CATerminateIP()
{
    OIC_LOG(DEBUG, TAG, "IN");

    CAIPSetConnectionStateChangeCallback(NULL);
    CAIPTerminateNetworkMonitor();
    CAIPSetPacketReceiveCallback(NULL);
    OIC_LOG(INFO, TAG, "Terminated Ethernet");
    OIC_LOG(DEBUG, TAG, "OUT");
    return;
}


