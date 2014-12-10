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
#include "caethernetadapter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "caethernetinterface.h"
#include "caqueueingthread.h"
#include "caadapterutils.h"
#include "umutex.h"
#include "logger.h"
#include "oic_malloc.h"

/**
 * @def WIFI_ETHERNET_ADAPTER_TAG
 * @brief Logging tag for module name
 */
#define ETHERNET_ADAPTER_TAG "ETHERNET_ADAP"

/**
 * @def CA_PORT
 * @brief Port to listen for incoming data
 */
#define CA_PORT   5283

/**
 * @def CA_MCAST_PORT
 * @brief Multicast Port Number
 */
#define CA_MCAST_PORT   5298

/**
 * @def CA_MULTICAST_IP
 * @brief Multicast IP Address
 */
#define CA_MULTICAST_IP "224.0.1.187"


typedef struct
{
    CARemoteEndpoint_t *remoteEndpoint;
    void *data;
    uint32_t dataLen;
} CAEthernetData;

/**
 * @var gNetworkPacketCallback
 * @brief Network Packet Received Callback to CA
 */
static CANetworkPacketReceivedCallback gNetworkPacketCallback = NULL;

/**
 * @var gEthernetNetworkChangeCallback
 * @brief Network Changed Callback to CA
 */
CANetworkChangeCallback gEthernetNetworkChangeCallback = NULL;

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
static int16_t gUnicastServerport = 0;

/**
 * @var gIsStartServerCalled
 * @brief Flag to check if server start requested by CA.
 */
static bool gStartMulticastServerRequested = false;

/**
 * @var gSendQueueHandle
 * @brief Queue handle for Send Data
 */
static CAQueueingThread_t *gSendQueueHandle = NULL;

/**
 * @var gThreadPool
 * @brief ThreadPool for storing u_thread_pool_t handle passed from CA
 */
static u_thread_pool_t gThreadPool = NULL;

static CAResult_t CAEthernetInitializeQueueHandles();
static void CAEthernetDeinitializeQueueHandles();
static void CAEthernetNotifyNetworkChange(const char *address, const int16_t port,
                                      const CANetworkStatus_t status);
static void CAEthernetConnectionStateCB(const char *ipAddress,
                                    const CANetworkStatus_t status);
static void CAEthernetPacketReceivedCB(const char *ipAddress, const uint32_t port,
                                   const void *data, const uint32_t dataLength);
static CAResult_t CAEthernetStopServers();
static void CAEthernetSendDataThread(void *threadData);
static CAEthernetData *CACreateEthernetData(const CARemoteEndpoint_t *remoteEndpoint, void *data,
                                    uint32_t dataLength);
void CAFreeEthernetData(CAEthernetData *ethernetData);


CAResult_t CAEthernetInitializeQueueHandles()
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");

    // Check if the message queue is already initialized
    if (gSendQueueHandle)
    {
        OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "Already queue is initialized!");
        return CA_STATUS_OK;
    }

    // Create send message queue
    gSendQueueHandle = OICMalloc(sizeof(CAQueueingThread_t));
    if (!gSendQueueHandle)
    {
        OIC_LOG(ERROR, ETHERNET_ADAPTER_TAG, "Memory allocation failed!");
        return CA_MEMORY_ALLOC_FAILED;
    }

    if (CA_STATUS_OK != CAQueueingThreadInitialize(gSendQueueHandle, gThreadPool,
            CAEthernetSendDataThread))
    {
        OIC_LOG(ERROR, ETHERNET_ADAPTER_TAG, "Failed to Initialize send queue thread");
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CAEthernetDeinitializeQueueHandles()
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");

    CAQueueingThreadDestroy(gSendQueueHandle);
    OICFree(gSendQueueHandle);
    gSendQueueHandle = NULL;

    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "OUT");
}

void CAEthernetNotifyNetworkChange(const char *address, const int16_t port,
                               const CANetworkStatus_t status)
{
    CALocalConnectivity_t *localEndpoint = CAAdapterCreateLocalEndpoint(CA_ETHERNET, address, "Ethernet");
    if (!localEndpoint)
    {
        OIC_LOG_V(ERROR, ETHERNET_ADAPTER_TAG, "Out of memory");
        return;
    }
    localEndpoint->addressInfo.IP.port = port;

    if (NULL != gEthernetNetworkChangeCallback)
    {
        gEthernetNetworkChangeCallback(localEndpoint, status);
    }

    CAAdapterFreeLocalEndpoint(localEndpoint);
}

void CAEthernetConnectionStateCB(const char *ipAddress,
                             const CANetworkStatus_t status)
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");

    CAResult_t ret = CA_STATUS_FAILED;
    /* If Ethernet is connected, then get the latest IP from the Ethernet Interface
      * and start unicast and multicast servers if requested earlier */
    if (CA_INTERFACE_UP == status)
    {
        int16_t port = CA_PORT;
        int32_t serverFd = -1;
        /* Start Unicast server if requested earlier */
        if (gStartUnicastServerRequested)
        {
            ret = CAEthernetStartUnicastServer("0.0.0.0", &port, false, &serverFd);
            if (CA_STATUS_OK == ret)
            {
                OIC_LOG_V(DEBUG, ETHERNET_ADAPTER_TAG, "Unicast server started on %d port", port);
                CAEthernetSetUnicastSocket(serverFd);
                gUnicastServerport = port;
            }
            else
            {
                OIC_LOG_V(ERROR, ETHERNET_ADAPTER_TAG, "Failed to start Unicast server [%d]", ret);
            }
        }

        /* Start Multicast server if requested earlier */
        if (gStartMulticastServerRequested)
        {
            int16_t multicastPort = CA_MCAST_PORT;
            int32_t multicastFd = 0;
            ret = CAEthernetStartMulticastServer("0.0.0.0", CA_MULTICAST_IP, multicastPort,
                &multicastFd);
            if (CA_STATUS_OK == ret)
            {
                OIC_LOG_V(DEBUG, ETHERNET_ADAPTER_TAG, "Multicast server started on %d port",
                    multicastPort);
                gIsMulticastServerStarted = true;
            }
            else
            {
                OIC_LOG_V(ERROR, ETHERNET_ADAPTER_TAG, "Failed to start Multicast server [%d]", ret);
            }
        }

        char *ipAddress = NULL;
        char *ifcName = NULL;
        ret = CAEthernetGetInterfaceInfo(&ifcName, &ipAddress);
        if (CA_STATUS_OK != ret)
        {
            OIC_LOG_V(ERROR, ETHERNET_ADAPTER_TAG, "Failed to get interface info [%d]", ret);
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
            OIC_LOG_V(ERROR, ETHERNET_ADAPTER_TAG, "Failed to Stop Servers![%d]", ret);
            return;
        }
    }

    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "OUT");
}

void CAEthernetPacketReceivedCB(const char *ipAddress, const uint32_t port,
                            const void *data, const uint32_t dataLength)
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");
    OIC_LOG_V(DEBUG, ETHERNET_ADAPTER_TAG, "Address: %s, port:%d, data:%s", ipAddress, port, data);

    /* CA is freeing this memory */
    CARemoteEndpoint_t *endPoint = CAAdapterCreateRemoteEndpoint(CA_ETHERNET, ipAddress, NULL);
    if (NULL == endPoint)
    {
        OIC_LOG(ERROR, ETHERNET_ADAPTER_TAG, "Out of memory");
        return;
    }
    endPoint->addressInfo.IP.port = port;

    void *buf = OICMalloc(dataLength + 1);
    if (NULL == buf)
    {
        OIC_LOG(ERROR, ETHERNET_ADAPTER_TAG, "Out of memory");
        CAAdapterFreeRemoteEndpoint(endPoint);
        return;
    }
    memcpy(buf, data, dataLength);
    memset(buf + dataLength, 0, 1);
    if (gNetworkPacketCallback)
    {
        gNetworkPacketCallback(endPoint, buf, dataLength);
    }
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "OUT");
}

CAResult_t CAInitializeEthernet(CARegisterConnectivityCallback registerCallback,
                            CANetworkPacketReceivedCallback networkPacketCallback,
                            CANetworkChangeCallback netCallback, u_thread_pool_t handle)
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");
    VERIFY_NON_NULL(registerCallback, ETHERNET_ADAPTER_TAG, "registerCallback");
    VERIFY_NON_NULL(networkPacketCallback, ETHERNET_ADAPTER_TAG, "networkPacketCallback");
    VERIFY_NON_NULL(netCallback, ETHERNET_ADAPTER_TAG, "netCallback");
    VERIFY_NON_NULL(handle, ETHERNET_ADAPTER_TAG, "thread pool handle");

    gThreadPool  = handle;
    gEthernetNetworkChangeCallback = netCallback;
    gNetworkPacketCallback = networkPacketCallback;

    CAResult_t ret = CAEthernetInitializeNetworkMonitor(gThreadPool);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(ERROR, ETHERNET_ADAPTER_TAG, "Failed to initialize n/w monitor![%d]", ret);
        return ret;
    }
    CAEthernetSetConnectionStateChangeCallback(CAEthernetConnectionStateCB);

    ret = CAEthernetInitializeServer(gThreadPool);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(ERROR, ETHERNET_ADAPTER_TAG, "Failed to initialize server![%d]", ret);
        CATerminateEthernet();
        return ret;
    }
    CAEthernetSetPacketReceiveCallback(CAEthernetPacketReceivedCB);

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

    if (CA_STATUS_OK != CAEthernetInitializeQueueHandles())
    {
        OIC_LOG(ERROR, ETHERNET_ADAPTER_TAG, "Failed to Initialize Queue Handle");
        CATerminateEthernet();
        return CA_STATUS_FAILED;
    }

    OIC_LOG(INFO, ETHERNET_ADAPTER_TAG, "IntializeEthernet is Success");
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
        OIC_LOG(ERROR, ETHERNET_ADAPTER_TAG, "Failed to Start n/w monitor");
    }

    gStartUnicastServerRequested = true;
    bool retVal = CAEthernetIsConnected();
    if (false == retVal)
    {
        OIC_LOG(ERROR, ETHERNET_ADAPTER_TAG, "Ethernet is not Connected");
        return ret;
    }

    int16_t unicastPort = CA_PORT;
    int32_t serverFd = 0;
    // Address is hardcoded as we are using Single Interface
    ret = CAEthernetStartUnicastServer("0.0.0.0", &unicastPort, false, &serverFd);
    if (CA_STATUS_OK == ret)
    {
        OIC_LOG_V(DEBUG, ETHERNET_ADAPTER_TAG, "Unicast server started on %d port", unicastPort);
        CAEthernetSetUnicastSocket(serverFd);
        gUnicastServerport = unicastPort;
    }

    // Start send queue thread
    if (CA_STATUS_OK != CAQueueingThreadStart(gSendQueueHandle))
    {
        OIC_LOG(ERROR, ETHERNET_ADAPTER_TAG, "Failed to Start Send Data Thread");
        CAStopEthernet();
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "OUT");
    return ret;
}

CAResult_t CAStartEthernetListeningServer()
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");

    CAResult_t ret = CA_STATUS_OK;
    int16_t multicastPort = CA_MCAST_PORT;

    if (gIsMulticastServerStarted == true)
    {
        OIC_LOG_V(ERROR, ETHERNET_ADAPTER_TAG,
                  "Failed to Start Multicast Server, Already Started!");
        return CA_SERVER_STARTED_ALREADY;
    }

    gStartMulticastServerRequested = true;
    bool retVal = CAEthernetIsConnected();
    if (false == retVal)
    {
        OIC_LOG_V(ERROR, ETHERNET_ADAPTER_TAG,
                  "Failed to Start Multicast Server, Ethernet not Connected");
        return CA_ADAPTER_NOT_ENABLED;
    }

    int32_t multicastFd = 0;
    ret = CAEthernetStartMulticastServer("0.0.0.0", CA_MULTICAST_IP, multicastPort, &multicastFd);
    if (CA_STATUS_OK == ret)
    {
        OIC_LOG(INFO, ETHERNET_ADAPTER_TAG, "Multicast Server is Started Successfully");
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

uint32_t CASendEthernetUnicastData(const CARemoteEndpoint_t *remoteEndpoint, void *data,
                               uint32_t dataLength)
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");

    uint32_t dataSize = 0;
    VERIFY_NON_NULL_RET(remoteEndpoint, ETHERNET_ADAPTER_TAG, "remoteEndpoint", dataSize);
    VERIFY_NON_NULL_RET(data, ETHERNET_ADAPTER_TAG, "data", dataSize);
    VERIFY_NON_NULL_RET(gSendQueueHandle, ETHERNET_ADAPTER_TAG, "sendQueueHandle", dataSize);

    if (0 == dataLength)
    {
        OIC_LOG_V(ERROR, ETHERNET_ADAPTER_TAG, "Invalid Data Length");
        return dataSize;
    }

    // Create EthernetData to add to queue
    CAEthernetData *ethernetData = CACreateEthernetData(remoteEndpoint, data, dataLength);
    if (!ethernetData)
    {
        OIC_LOG_V(ERROR, ETHERNET_ADAPTER_TAG, "Failed to create ethernetData!");
        return CA_MEMORY_ALLOC_FAILED;
    }

    // Add message to send queue
    CAQueueingThreadAddData(gSendQueueHandle, ethernetData, sizeof(CAEthernetData));

    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "OUT");
    return dataLength;
}

uint32_t CASendEthernetMulticastData(void *data, uint32_t dataLength)
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");

    uint32_t dataSize = 0;
    VERIFY_NON_NULL_RET(data, ETHERNET_ADAPTER_TAG, "data", dataSize);
    VERIFY_NON_NULL_RET(gSendQueueHandle, ETHERNET_ADAPTER_TAG, "sendQueueHandle", dataSize);

    if (0 == dataLength)
    {
        OIC_LOG_V(ERROR, ETHERNET_ADAPTER_TAG, "Invalid Data Length");
        return dataSize;
    }

    // Create ethernetData to add to queue
    CAEthernetData *ethernetData = CACreateEthernetData(NULL, data, dataLength);
    if (!ethernetData)
    {
        OIC_LOG_V(ERROR, ETHERNET_ADAPTER_TAG, "Failed to create ethernetData!");
        return CA_MEMORY_ALLOC_FAILED;
    }

    // Add message to send queue
    CAQueueingThreadAddData(gSendQueueHandle, ethernetData, sizeof(CAEthernetData));

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
        OIC_LOG_V(ERROR, ETHERNET_ADAPTER_TAG,
                  "Failed to get interface address, Ethernet not Connected", CA_ADAPTER_NOT_ENABLED);
        return CA_ADAPTER_NOT_ENABLED;
    }

    char *ipAddress = NULL;
    char *ifcName = NULL;
    CAResult_t ret = CAEthernetGetInterfaceInfo(&ifcName, &ipAddress);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(ERROR, ETHERNET_ADAPTER_TAG, "Failed to get interface info [%d]", ret);
        return ret;
    }

    // Create local endpoint using util function
    (*info) = CAAdapterCreateLocalEndpoint(CA_ETHERNET, ipAddress, ifcName);
    if (NULL == (*info))
    {
        OIC_LOG_V(ERROR, ETHERNET_ADAPTER_TAG, "Failed to create Local Endpoint!",
                  CA_MEMORY_ALLOC_FAILED);
        OICFree(ipAddress);
        OICFree(ifcName);
        return CA_MEMORY_ALLOC_FAILED;
    }

    (*info)->addressInfo.IP.port = gUnicastServerport;
    (*size) = 1;

    OICFree(ipAddress);
    OICFree(ifcName);

    OIC_LOG(INFO, ETHERNET_ADAPTER_TAG, "GetEthernetInterfaceInformation success");
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAReadEthernetData()
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAEthernetStopServers()
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");
    CAResult_t result = CAEthernetStopUnicastServer();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG_V(ERROR, ETHERNET_ADAPTER_TAG, "Failed to Stop Unicast Server![%d]", result);
        return result;
    }
    CAEthernetSetUnicastSocket(-1);
    gUnicastServerport = -1;

    result = CAEthernetStopMulticastServer();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG_V(ERROR, ETHERNET_ADAPTER_TAG, "Failed to Stop Multicast Server![%d]", result);
        return result;
    }
    gIsMulticastServerStarted = false;
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "OUT");
    return result;
}

CAResult_t CAStopEthernet()
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");

    gStartUnicastServerRequested = false;
    gStartMulticastServerRequested = false;

    // Stop Ethernet network monitor
    CAEthernetStopNetworkMonitor();

    //Stop send queue thread
    CAQueueingThreadStop(gSendQueueHandle);

    // Stop unicast/multicast servers running
    CAResult_t result = CAEthernetStopServers();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG_V(ERROR, ETHERNET_ADAPTER_TAG, "Failed to Stop Servers![%d]", result);
    }

    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "OUT");
    return result;
}

void CATerminateEthernet()
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");

    CAEthernetSetConnectionStateChangeCallback(NULL);
    CAEthernetTerminateNetworkMonitor();
    OIC_LOG(INFO, ETHERNET_ADAPTER_TAG, "nw monitor terminated");
    CAEthernetDeinitializeQueueHandles();
    OIC_LOG(INFO, ETHERNET_ADAPTER_TAG, "sendQueue terminated");

    CAEthernetSetPacketReceiveCallback(NULL);
    OIC_LOG(INFO, ETHERNET_ADAPTER_TAG, "TerminateEthernet Success");
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "OUT");
    return;
}

void CAEthernetSendDataThread(void *threadData)
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");

    CAEthernetData *ethernetData = (CAEthernetData *) threadData;
    if (!ethernetData)
    {
        OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "Invalid ethernetdata!");
        return;
    }

    if (NULL != ethernetData->remoteEndpoint)
    {
        OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "Send Unicast Data is called");
        CAEthernetSendData(ethernetData->remoteEndpoint->addressInfo.IP.ipAddress,
                            ethernetData->remoteEndpoint->addressInfo.IP.port, ethernetData->data,
                            ethernetData->dataLen, false);
    }
    else
    {
        OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "Send Multicast Data is called");
        CAEthernetSendData(CA_MULTICAST_IP, CA_MCAST_PORT, ethernetData->data, ethernetData->dataLen, true);
    }

    //Free Ethernet data
    CAFreeEthernetData(ethernetData);

    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "OUT");
    return ;
}

CAEthernetData *CACreateEthernetData(const CARemoteEndpoint_t *remoteEndpoint, void *data,
                             uint32_t dataLength)
{
    CAEthernetData *ethernetData = (CAEthernetData *) OICMalloc(sizeof(CAEthernetData));
    if (!ethernetData)
    {
        OIC_LOG(ERROR, ETHERNET_ADAPTER_TAG, "Memory allocation failed!");
        return NULL;
    }

    ethernetData->remoteEndpoint = CAAdapterCopyRemoteEndpoint(remoteEndpoint);
    ethernetData->data = (void *)OICMalloc(dataLength);
    if (NULL == ethernetData->data)
    {
        OIC_LOG(ERROR, ETHERNET_ADAPTER_TAG, "Memory allocation failed!");
        CAFreeEthernetData(ethernetData);
        return NULL;
    }
    memcpy(ethernetData->data, data, dataLength);
    ethernetData->dataLen = dataLength;

    return ethernetData;
}

void CAFreeEthernetData(CAEthernetData *ethernetData)
{
    if (!ethernetData)
        return;

    CAAdapterFreeRemoteEndpoint(ethernetData->remoteEndpoint);
    OICFree(ethernetData->data);
    OICFree(ethernetData);
}
