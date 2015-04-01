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
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "caadapterutils.h"
#include "logger.h"
#include "oic_malloc.h"
#include "cawifiinterface_singlethread.h"

/**
 * @def WIFI_ETHERNET_ADAPTER_TAG
 * @brief Logging tag for module name
 */
#define WIFI_ADAPTER_TAG "WAST"

/**
 * @def CA_PORT
 * @brief Port to listen for incoming data. Port 5683 is as per COAP RFC.
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
static int16_t g_unicastServerport = 0;

/**
 * @var g_startMulticastServerRequested
 * @brief Flag to check if server start requested by CA.
 */
static bool g_startMulticastServerRequested = false;


static void CAWiFiNotifyNetworkChange(const char *address, int16_t port,
                                      CANetworkStatus_t status);
static void CAWiFiConnectionStateCB(const char *ipAddress,
                                    CANetworkStatus_t status);
static void CAWiFiPacketReceivedCB(const char *ipAddress, uint32_t port,
                                   const void *data, uint32_t dataLength);
static CAResult_t CAWiFiStopServers();

void CAWiFiNotifyNetworkChange(const char *address, int16_t port,
                               CANetworkStatus_t status)
{
    CALocalConnectivity_t *localEndpoint = CAAdapterCreateLocalEndpoint(CA_WIFI, address);
    if (!localEndpoint)
    {
        OIC_LOG(ERROR, WIFI_ADAPTER_TAG, "Out of memory");
        return;
    }
    localEndpoint->addressInfo.IP.port = port;

    if (NULL != g_networkChangeCallback)
    {
        g_networkChangeCallback(localEndpoint, status);
    }

    CAAdapterFreeLocalEndpoint(localEndpoint);
}

void CAWiFiConnectionStateCB(const char *ipAddr,
                             CANetworkStatus_t status)
{
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "IN");

    CAResult_t ret = CA_STATUS_FAILED;
    /* If Wifi is connected, then get the latest IP from the WIFI Interface
      * and start unicast and multicast servers if requested earlier */
    if (CA_INTERFACE_UP == status)
    {
        int16_t port = CA_PORT;
        int32_t serverFd = -1;
        /* Start Unicast server if requested earlier */
        if (g_startUnicastServerRequested)
        {
            ret = CAWiFiStartUnicastServer("0.0.0.0", &port, false, &serverFd);
            if (CA_STATUS_OK == ret)
            {
                OIC_LOG_V(DEBUG, WIFI_ADAPTER_TAG, "Unicast server started on %d port", port);
                CAWiFiSetUnicastSocket(serverFd);
                g_unicastServerport = port;
            }
            else
            {
                OIC_LOG_V(ERROR, WIFI_ADAPTER_TAG, "strt ucast fail:%d", ret);
            }
        }

        /* Start Multicast server if requested earlier */
        if (g_startMulticastServerRequested)
        {
            int16_t multicastPort = CA_MCAST_PORT;
            ret = CAWiFiStartMulticastServer("0.0.0.0", CA_MULTICAST_IP, multicastPort, &serverFd);
            if (CA_STATUS_OK == ret)
            {
                OIC_LOG_V(DEBUG, WIFI_ADAPTER_TAG, "Multicast server started on %d port", multicastPort);
                g_isMulticastServerStarted = true;
            }
            else
            {
                OIC_LOG_V(ERROR, WIFI_ADAPTER_TAG, "strt mcast fail:%d", ret);
            }
        }

        /* Notify network change to CA */
        CAWiFiNotifyNetworkChange(ipAddr, port, status);
    }
    else
    {
        CAWiFiNotifyNetworkChange("", 0, status);
        /* Stop both Unicast and Multicast servers */
        ret = CAWiFiStopServers();
        if (CA_STATUS_OK != ret)
        {
            OIC_LOG_V(ERROR, WIFI_ADAPTER_TAG, "Failed to Stop Servers![%d]", ret);
            return;
        }
    }

    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "OUT");
}

void CAWiFiPacketReceivedCB(const char *ipAddress, uint32_t port,
                            const void *data, uint32_t dataLength)
{
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "IN");
    OIC_LOG_V(DEBUG, WIFI_ADAPTER_TAG, "Address: %s, port:%d ", ipAddress, port);
    OIC_LOG_V(DEBUG, WIFI_ADAPTER_TAG, "Data:%s", data);

    /* CA is freeing this memory */
    CARemoteEndpoint_t *endPoint = CAAdapterCreateRemoteEndpoint(CA_WIFI, ipAddress, NULL);
    if (NULL == endPoint)
    {
        OIC_LOG(ERROR, WIFI_ADAPTER_TAG, "Out of memory");
        return;
    }
    endPoint->addressInfo.IP.port = port;

    if (g_networkPacketCallback)
    {
        g_networkPacketCallback(endPoint, data, dataLength);
    }
    CAAdapterFreeRemoteEndpoint(endPoint);
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "OUT");
}

CAResult_t CAInitializeWIFI(CARegisterConnectivityCallback registerCallback,
                            CANetworkPacketReceivedCallback networkPacketCallback,
                            CANetworkChangeCallback netCallback)
{
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "IN");
    VERIFY_NON_NULL(registerCallback, WIFI_ADAPTER_TAG, "registerCallback");
    VERIFY_NON_NULL(networkPacketCallback, WIFI_ADAPTER_TAG, "networkPacketCallback");
    VERIFY_NON_NULL(netCallback, WIFI_ADAPTER_TAG, "netCallback");

    g_networkChangeCallback = netCallback;
    g_networkPacketCallback = networkPacketCallback;

    CAResult_t ret = CAWiFiInitializeNetworkMonitor();
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(ERROR, WIFI_ADAPTER_TAG, "Failed to initialize n/w monitor![%d]", ret);
        return ret;
    }
    CAWiFiSetConnectionStateChangeCallback(CAWiFiConnectionStateCB);

    // network monitor is yet to be implemented
    ret = CAWiFiInitializeServer();
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(ERROR, WIFI_ADAPTER_TAG, "Failed to initialize server![%d]", ret);
        CATerminateWIFI();
        return ret;
    }
    CAWiFiSetPacketReceiveCallback(CAWiFiPacketReceivedCB);

    CAConnectivityHandler_t wifiHandler;
    wifiHandler.startAdapter = CAStartWIFI;
    wifiHandler.startListenServer = CAStartWIFIListeningServer;
    wifiHandler.startDiscoveryServer = CAStartWIFIDiscoveryServer;
    wifiHandler.sendData = CASendWIFIUnicastData;
    wifiHandler.sendDataToAll = CASendWIFIMulticastData;
    wifiHandler.GetnetInfo = CAGetWIFIInterfaceInformation;
    wifiHandler.readData = CAReadWIFIData;
    wifiHandler.stopAdapter = CAStopWIFI;
    wifiHandler.terminate = CATerminateWIFI;
    registerCallback(wifiHandler, CA_WIFI);

    OIC_LOG(INFO, WIFI_ADAPTER_TAG, "Intialize WIFI Success");
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAStartWIFI()
{
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "IN");

    // Start monitoring wifi network
    // network monitor is yet to be implemented
    CAResult_t ret = CAWiFiStartNetworkMonitor();
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG(ERROR, WIFI_ADAPTER_TAG, "Failed to Start n/w monitor");
    }

    g_startUnicastServerRequested = true;
    bool retVal = CAWiFiIsConnected();
    if (false == retVal)
    {
        OIC_LOG(ERROR, WIFI_ADAPTER_TAG, "WIFI is not Connected");
        return ret;
    }

    int16_t unicastPort = CA_PORT;
    int32_t serverFd = 0;
    // Address is hardcoded as we are using Single Interface
    ret = CAWiFiStartUnicastServer("0.0.0.0", &unicastPort, false,  &serverFd);
    if (CA_STATUS_OK == ret)
    {
        OIC_LOG_V(DEBUG, WIFI_ADAPTER_TAG, "Unicast server started on %d port", unicastPort);
        CAWiFiSetUnicastSocket(serverFd);
        g_unicastServerport = unicastPort;
    }

    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "OUT");
    return ret;
}

CAResult_t CAStartWIFIListeningServer()
{
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "IN");

    CAResult_t ret = CA_STATUS_OK;
    int16_t multicastPort = CA_MCAST_PORT;

    if (g_isMulticastServerStarted == true)
    {
        OIC_LOG(ERROR, WIFI_ADAPTER_TAG, "Multicast Server, Already Started!");
        return CA_SERVER_STARTED_ALREADY;
    }

    g_startMulticastServerRequested = true;
    // network monitor is yet to be implemented
    bool retVal = CAWiFiIsConnected();
    if (false == retVal)
    {
        OIC_LOG(ERROR, WIFI_ADAPTER_TAG, "Failed : WIFI not Connected");
        return CA_ADAPTER_NOT_ENABLED;
    }

    ret = CAWiFiStartMulticastServer("0.0.0.0", CA_MULTICAST_IP, multicastPort, NULL);
    if (CA_STATUS_OK == ret)
    {
        OIC_LOG(INFO, WIFI_ADAPTER_TAG, "Multicast Server Started Successfully");
        // iff wifi shield firmware upgrade allows multicast
        g_isMulticastServerStarted = true;
    }

    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "OUT");
    return ret;
}

CAResult_t CAStartWIFIDiscoveryServer()
{
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "IN");
    // Both listening and discovery server are same
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "OUT");
    return CAStartWIFIListeningServer();
}

int32_t CASendWIFIUnicastData(const CARemoteEndpoint_t *remoteEndpoint, const void *data,
                              uint32_t dataLength)
{
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "IN");

    VERIFY_NON_NULL_RET(remoteEndpoint, WIFI_ADAPTER_TAG, "remoteEndpoint", -1);
    VERIFY_NON_NULL_RET(data, WIFI_ADAPTER_TAG, "data", -1);
    if (dataLength == 0)
    {
        OIC_LOG(ERROR, WIFI_ADAPTER_TAG, "Invalid Data Length");
        return -1;
    }

    CAWiFiSendData(remoteEndpoint->addressInfo.IP.ipAddress,
                   remoteEndpoint->addressInfo.IP.port, data, dataLength, false);
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "OUT");
    return dataLength;
}

int32_t CASendWIFIMulticastData(const void *data, uint32_t dataLength)
{
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "IN");

    VERIFY_NON_NULL_RET(data, WIFI_ADAPTER_TAG, "data", -1);
    if (dataLength == 0)
    {
        OIC_LOG(ERROR, WIFI_ADAPTER_TAG, "Invalid Data Length");
        return -1;
    }

    CAWiFiSendData(CA_MULTICAST_IP, CA_MCAST_PORT, data, dataLength, true);
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "OUT");
    return dataLength;
}

CAResult_t CAGetWIFIInterfaceInformation(CALocalConnectivity_t **info, uint32_t *size)
{
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "IN");
    VERIFY_NON_NULL(info, WIFI_ADAPTER_TAG, "info");
    VERIFY_NON_NULL(size, WIFI_ADAPTER_TAG, "size");

    bool retVal = CAWiFiIsConnected();
    if (false == retVal)
    {
        OIC_LOG_V(ERROR, WIFI_ADAPTER_TAG,
                  "Failed to get interface address, Error Code: %d", CA_ADAPTER_NOT_ENABLED);
        return CA_ADAPTER_NOT_ENABLED;
    }

    char *ipAddress = NULL;
    char *ifcName = NULL;
    CAResult_t ret = CAWiFiGetInterfaceInfo(&ipAddress, &ifcName);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(ERROR, WIFI_ADAPTER_TAG, "Failed to get interface info [%d]", ret);
        OICFree(ipAddress);
        OICFree(ifcName);
        return ret;
    }

    // Create local endpoint using util function
    (*info) = CAAdapterCreateLocalEndpoint(CA_WIFI, ipAddress);
    if (NULL == (*info))
    {
        OIC_LOG_V(ERROR, WIFI_ADAPTER_TAG, "Failed to create Local Endpoint, Error Code:%d",
                  CA_MEMORY_ALLOC_FAILED);
        OICFree(ipAddress);
        OICFree(ifcName);
        return CA_MEMORY_ALLOC_FAILED;
    }

    (*info)->addressInfo.IP.port = g_unicastServerport;
    (*size) = 1;

    OICFree(ipAddress);
    OICFree(ifcName);

    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAReadWIFIData()
{
    CAWiFiPullData();
    return CA_STATUS_OK;
}

CAResult_t CAWiFiStopServers()
{
    CAResult_t result = CAWiFiStopUnicastServer();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG_V(ERROR, WIFI_ADAPTER_TAG, "Failed to Stop Unicast Server![%d]", result);
        return result;
    }
    CAWiFiSetUnicastSocket(-1);
    g_unicastServerport = -1;

    result = CAWiFiStopMulticastServer();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG_V(ERROR, WIFI_ADAPTER_TAG, "Failed to Stop Multicast Server![%d]", result);
        return result;
    }
    g_isMulticastServerStarted = false;

    return result;
}

CAResult_t CAStopWIFI()
{
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "IN");

    g_startUnicastServerRequested = false;
    g_startMulticastServerRequested = false;
    CAWiFiStopNetworkMonitor();
    CAResult_t result = CAWiFiStopServers();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG_V(ERROR, WIFI_ADAPTER_TAG, "Failed to Stop Servers![%d]", result);
    }

    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "OUT");
    return result;
}

void CATerminateWIFI()
{
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "IN");

    CAWiFiSetConnectionStateChangeCallback(NULL);
    CAWiFiTerminateNetworkMonitor();
    OIC_LOG(INFO, WIFI_ADAPTER_TAG, "nw monitor terminated");
    CAWiFiSetPacketReceiveCallback(NULL);
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "OUT");
    return;
}
