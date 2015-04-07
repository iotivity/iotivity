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
#ifdef __WITH_DTLS__
#include "caadapternetdtls.h"
#endif
#include "umutex.h"
#include "logger.h"
#include "oic_malloc.h"

/**
 * @def ETHERNET_ADAPTER_TAG
 * @brief Logging tag for module name
 */
#define ETHERNET_ADAPTER_TAG "ETHERNET_ADAP"

/**
 * @def CA_PORT
 * @brief Port to listen for incoming data
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
 * @brief Multicast IP Address as defined in COAP Specification, RFC-7252.
 */
#define CA_MULTICAST_IP "224.0.1.187"

typedef struct
{
    CARemoteEndpoint_t *remoteEndpoint;
    void *data;
    uint32_t dataLen;
} CAEthernetData;

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
 * @var gIsStartServerCalled
 * @brief Flag to check if server start requested by CA.
 */
static bool g_startUnicastServerRequested = false;

/**
 * @var g_unicastServerport
 * @brief port number on which unicast server is running.
 */
static uint16_t g_unicastServerport = 0;

#ifdef __WITH_DTLS__
/**
 * @var g_secureUnicastServerport
 * @brief port number on which secure unicast server is running.
 */
static uint16_t g_secureUnicastServerport = 0;
#endif

/**
 * @var g_startMulticastServerRequested
 * @brief Flag to check if server start requested by CA.
 */
static bool g_startMulticastServerRequested = false;

/**
 * @var g_sendQueueHandle
 * @brief Queue handle for Send Data
 */
static CAQueueingThread_t *g_sendQueueHandle = NULL;

/**
 * @var g_threadPool
 * @brief ThreadPool for storing u_thread_pool_t handle passed from CA
 */
static u_thread_pool_t g_threadPool = NULL;

static CAResult_t CAEthernetInitializeQueueHandles();
static void CAEthernetDeinitializeQueueHandles();
static void CAEthernetNotifyNetworkChange(const char *address, const uint16_t port,
        const CANetworkStatus_t status);
static void CAEthernetConnectionStateCB(const char *ipAddress,
                                    const CANetworkStatus_t status);
static void CAEthernetPacketReceivedCB(const char *ipAddress, const uint16_t port,
                                       const void *data, const uint32_t dataLength, const bool isSecured);
#ifdef __WITH_DTLS__
static uint32_t CAEthernetPacketSendCB(const char *ipAddress, const uint16_t port,
                                       const void *data, const uint32_t dataLength);
#endif

static CAResult_t CAEthernetStopServers();
static void CAEthernetSendDataThread(void *threadData);
static CAEthernetData *CACreateEthernetData(const CARemoteEndpoint_t *remoteEndpoint, const void *data,
        uint32_t dataLength);
void CAFreeEthernetData(CAEthernetData *EthernetData);

static void CADataDestroyer(void *data, uint32_t size);

CAResult_t CAEthernetInitializeQueueHandles()
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");

    // Check if the message queue is already initialized
    if (g_sendQueueHandle)
    {
        OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "send queue handle is already initialized!");
        return CA_STATUS_OK;
    }

    // Create send message queue
    g_sendQueueHandle = OICMalloc(sizeof(CAQueueingThread_t));
    if (!g_sendQueueHandle)
    {
        OIC_LOG(ERROR, ETHERNET_ADAPTER_TAG, "Memory allocation failed!");
        return CA_MEMORY_ALLOC_FAILED;
    }

    if (CA_STATUS_OK != CAQueueingThreadInitialize(g_sendQueueHandle, g_threadPool,
            CAEthernetSendDataThread, CADataDestroyer))
    {
        OIC_LOG(ERROR, ETHERNET_ADAPTER_TAG, "Failed to Initialize send queue thread");
        OICFree(g_sendQueueHandle);
        g_sendQueueHandle = NULL;
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CAEthernetDeinitializeQueueHandles()
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");

    CAQueueingThreadDestroy(g_sendQueueHandle);
    OICFree(g_sendQueueHandle);
    g_sendQueueHandle = NULL;

    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "OUT");
}

void CAEthernetNotifyNetworkChange(const char *address, const uint16_t port,
                               const CANetworkStatus_t status)
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");

    CALocalConnectivity_t *localEndpoint = CAAdapterCreateLocalEndpoint(CA_ETHERNET, address);
    if (!localEndpoint)
    {
        OIC_LOG(ERROR, ETHERNET_ADAPTER_TAG, "Out of memory");
        return;
    }

    if (NULL != g_networkChangeCallback)
    {
        g_networkChangeCallback(localEndpoint, status);
    }

    CAAdapterFreeLocalEndpoint(localEndpoint);
}

void CAEthernetConnectionStateCB(const char *ipAddress,
                             const CANetworkStatus_t status)
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");

    CAResult_t ret = CA_STATUS_FAILED;

    /**
     * If Ethernet is connected, then get the latest IP from the Ethernet Interface
     * and start unicast and multicast servers if requested earlier
     */
    if (CA_INTERFACE_UP == status)
    {
        uint16_t port = CA_PORT;
        int32_t serverFd = -1;

        // Start Unicast server if requested earlier
        if (g_startUnicastServerRequested)
        {
            ret = CAEthernetStartUnicastServer("0.0.0.0", &port, false, false, &serverFd);
            if (CA_STATUS_OK == ret)
            {
                OIC_LOG_V(DEBUG, ETHERNET_ADAPTER_TAG, "Unicast server started on %d port", port);
                CAEthernetSetUnicastSocket(serverFd);
                g_unicastServerport = port;
            }
            else
            {
                OIC_LOG_V(ERROR, ETHERNET_ADAPTER_TAG, "Failed to start Unicast server [%d]", ret);
            }

#ifdef __WITH_DTLS__
            port = CA_SECURE_PORT;
            ret = CAEthernetStartUnicastServer("0.0.0.0", &port, false, true, &serverFd);
            if (CA_STATUS_OK == ret)
            {
                OIC_LOG_V(DEBUG, ETHERNET_ADAPTER_TAG, "Secure Unicast server started on %d", port);
                CAEthernetSetSecureUnicastSocket(serverFd);
                g_secureUnicastServerport = port;
            }
            else
            {
                OIC_LOG_V(ERROR, ETHERNET_ADAPTER_TAG, "Failed to start secure Unicast server [%d]",
                          ret);
            }
#endif
        }

        // Start Multicast server if requested earlier
        if (g_startMulticastServerRequested)
        {
            uint16_t multicastPort = CA_MCAST_PORT;
            int32_t multicastFd = 0;
            ret = CAEthernetStartMulticastServer("0.0.0.0", CA_MULTICAST_IP, multicastPort,
                                             &multicastFd);
            if (CA_STATUS_OK == ret)
            {
                OIC_LOG_V(DEBUG, ETHERNET_ADAPTER_TAG, "Multicast server started on %d port",
                          multicastPort);
                g_isMulticastServerStarted = true;
            }
            else
            {
                OIC_LOG_V(ERROR, ETHERNET_ADAPTER_TAG, "Failed to start Multicast server [%d]", ret);
            }
        }

        char *address = NULL;
        char *ifcName = NULL;
        ret = CAEthernetGetInterfaceInfo(&ifcName, &address);
        if (CA_STATUS_OK != ret || NULL == address)
        {
            OIC_LOG_V(ERROR, ETHERNET_ADAPTER_TAG, "Failed to get interface info [%d]", ret);
            OICFree(address);
            OICFree(ifcName);
            return;
        }

        // Notify network change to CA
        CAEthernetNotifyNetworkChange(address, port, status);
        OICFree(address);
        OICFree(ifcName);
    }
    else
    {
        CAEthernetNotifyNetworkChange("", 0, status);

        // Stop Unicast, Secured unicast and Multicast servers
        CAEthernetStopServers();
    }

    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "OUT");
}

#ifdef __WITH_DTLS__
uint32_t CAEthernetPacketSendCB(const char *ipAddress, const uint16_t port,
                                const void *data, const uint32_t dataLength)
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");

    uint32_t sentLength = CAEthernetSendData(ipAddress, port, data, dataLength, false, true);

    OIC_LOG_V(DEBUG, ETHERNET_ADAPTER_TAG, "Successfully sent %d of encrypted data!", sentLength);

    return sentLength;
}
#endif

void CAEthernetPacketReceivedCB(const char *ipAddress, const uint16_t port,
                                const void *data, const uint32_t dataLength, const bool isSecured)
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");
    OIC_LOG_V(DEBUG, ETHERNET_ADAPTER_TAG, "Address: %s, port:%d, data:%s", ipAddress, port, data);

    // CA is freeing this memory
    CARemoteEndpoint_t *endPoint = CAAdapterCreateRemoteEndpoint(CA_ETHERNET, ipAddress, NULL);
    if (NULL == endPoint)
    {
        OIC_LOG(ERROR, ETHERNET_ADAPTER_TAG, "Out of memory");
        return;
    }
    endPoint->addressInfo.IP.port = port;
    endPoint->isSecured = isSecured;

    void *buf = OICMalloc(dataLength + 1);
    if (NULL == buf)
    {
        OIC_LOG(ERROR, ETHERNET_ADAPTER_TAG, "Out of memory");
        CAAdapterFreeRemoteEndpoint(endPoint);
        return;
    }
    memcpy(buf, data, dataLength);
    memset(buf + dataLength, 0, 1);
    if (g_networkPacketCallback)
    {
        g_networkPacketCallback(endPoint, buf, dataLength);
    }
    else
    {
        OICFree(buf);
        CAAdapterFreeRemoteEndpoint(endPoint);
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

    g_threadPool  = handle;
    g_networkChangeCallback = netCallback;
    g_networkPacketCallback = networkPacketCallback;

    CAResult_t ret = CAEthernetInitializeNetworkMonitor(g_threadPool);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(ERROR, ETHERNET_ADAPTER_TAG, "Failed to initialize n/w monitor![%d]", ret);
        return ret;
    }
    CAEthernetSetConnectionStateChangeCallback(CAEthernetConnectionStateCB);

    ret = CAEthernetInitializeServer(g_threadPool);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(ERROR, ETHERNET_ADAPTER_TAG, "Failed to initialize server![%d]", ret);
        CATerminateEthernet();
        return ret;
    }

    CAEthernetSetPacketReceiveCallback(CAEthernetPacketReceivedCB);
#ifdef __WITH_DTLS__
    CAAdapterNetDtlsInit();

    CADTLSSetAdapterCallbacks(CAEthernetPacketReceivedCB, CAEthernetPacketSendCB, DTLS_ETHERNET);
#endif

    CAConnectivityHandler_t ethernetHandler;
    ethernetHandler.startAdapter = CAStartEthernet;
    ethernetHandler.startListenServer = CAStartEthernetListeningServer;
    ethernetHandler.startDiscoveryServer = CAStartEthernetDiscoveryServer;
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
    return CA_STATUS_OK;
}

CAResult_t CAStartEthernet()
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");

    // Start monitoring Ethernet network
    CAResult_t ret = CAEthernetStartNetworkMonitor();
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG(ERROR, ETHERNET_ADAPTER_TAG, "Failed to Start n/w monitor");
        return ret;
    }

    // Start send queue thread
    if (CA_STATUS_OK != CAQueueingThreadStart(g_sendQueueHandle))
    {
        OIC_LOG(ERROR, ETHERNET_ADAPTER_TAG, "Failed to Start Send Data Thread");
        CAStopEthernet();
        return CA_STATUS_FAILED;
    }

    g_startUnicastServerRequested = true;
    bool retVal = CAEthernetIsConnected();
    if (false == retVal)
    {
        OIC_LOG(ERROR, ETHERNET_ADAPTER_TAG, "Ethernet is not Connected");
        return CA_STATUS_OK;
    }

    char *ifcName = NULL;
    char *ifcAdrs = NULL;
    ret = CAEthernetGetInterfaceInfo(&ifcName, &ifcAdrs);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(DEBUG, ETHERNET_ADAPTER_TAG, "Failed to get ethernet interface info [%d]", ret);
        return ret;
    }
    uint16_t unicastPort = CA_PORT;
    int32_t serverFd = 0;
    // Address is hardcoded as we are using Single Interface
    ret = CAEthernetStartUnicastServer(ifcAdrs, &unicastPort, false, false, &serverFd);
    if (CA_STATUS_OK == ret)
    {
        OIC_LOG_V(DEBUG, ETHERNET_ADAPTER_TAG, "Unicast server started on %d port", unicastPort);
        CAEthernetSetUnicastSocket(serverFd);
        g_unicastServerport = unicastPort;
    }

#ifdef __WITH_DTLS__
    // Address is hardcoded as we are using Single Interface
    unicastPort = CA_SECURE_PORT;
    ret = CAEthernetStartUnicastServer(ifcAdrs, &unicastPort, false, true, &serverFd);

    if (CA_STATUS_OK == ret)
    {
        OIC_LOG_V(DEBUG, ETHERNET_ADAPTER_TAG, "Secure Unicast server started on %d port", unicastPort);
        CAEthernetSetSecureUnicastSocket(serverFd);
        g_secureUnicastServerport = unicastPort;
    }
#endif

    OICFree(ifcName);
    OICFree(ifcAdrs);

    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "OUT");
    return ret;
}

CAResult_t CAStartEthernetListeningServer()
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");

    CAResult_t ret = CA_STATUS_OK;
    uint16_t multicastPort = CA_MCAST_PORT;

    if (g_isMulticastServerStarted == true)
    {
        OIC_LOG_V(ERROR, ETHERNET_ADAPTER_TAG,
                  "Failed to Start Multicast Server, Already Started!");
        return CA_SERVER_STARTED_ALREADY;
    }

    g_startMulticastServerRequested = true;
    bool retVal = CAEthernetIsConnected();
    if (false == retVal)
    {
        OIC_LOG_V(ERROR, ETHERNET_ADAPTER_TAG,
                  "Failed to Start Multicast Server, Ethernet not Connected");
        return CA_STATUS_OK;
    }

    char *ifcName = NULL;
    char *ifcAdrs = NULL;
    ret = CAEthernetGetInterfaceInfo(&ifcName, &ifcAdrs);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(DEBUG, ETHERNET_ADAPTER_TAG, "Failed to get ethernet interface info [%d]", ret);
        return ret;
    }
    int32_t multicastFd = 0;
    ret = CAEthernetStartMulticastServer(ifcAdrs, CA_MULTICAST_IP, multicastPort, &multicastFd);
    if (CA_STATUS_OK == ret)
    {
        OIC_LOG(INFO, ETHERNET_ADAPTER_TAG, "Multicast Server is Started Successfully");
        g_isMulticastServerStarted = true;
    }

    OICFree(ifcName);
    OICFree(ifcAdrs);

    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "OUT");
    return ret;
}

CAResult_t CAStartEthernetDiscoveryServer()
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");
    return CAStartEthernetListeningServer();
}

int32_t CASendEthernetUnicastData(const CARemoteEndpoint_t *remoteEndpoint, const void *data,
                               uint32_t dataLength)
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");

    VERIFY_NON_NULL_RET(remoteEndpoint, ETHERNET_ADAPTER_TAG, "remoteEndpoint", -1);
    VERIFY_NON_NULL_RET(data, ETHERNET_ADAPTER_TAG, "data", -1);
    VERIFY_NON_NULL_RET(g_sendQueueHandle, ETHERNET_ADAPTER_TAG, "sendQueueHandle", -1);

    if (0 == dataLength)
    {
        OIC_LOG(ERROR, ETHERNET_ADAPTER_TAG, "Invalid Data Length");
        return -1;
    }

    // Create EthernetData to add to queue
    CAEthernetData *ethernetData = CACreateEthernetData(remoteEndpoint, data, dataLength);
    if (!ethernetData)
    {
        OIC_LOG(ERROR, ETHERNET_ADAPTER_TAG, "Failed to create ethernetData!");
        return -1;
    }
    else
    {
        // Add message to send queue
        CAQueueingThreadAddData(g_sendQueueHandle, ethernetData, sizeof(CAEthernetData));

        OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "OUT");
        return dataLength;
    }
}

int32_t CASendEthernetMulticastData(const void *data, uint32_t dataLength)
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");

    VERIFY_NON_NULL_RET(data, ETHERNET_ADAPTER_TAG, "data", -1);
    VERIFY_NON_NULL_RET(g_sendQueueHandle, ETHERNET_ADAPTER_TAG, "sendQueueHandle", -1);

    if (0 == dataLength)
    {
        OIC_LOG(ERROR, ETHERNET_ADAPTER_TAG, "Invalid Data Length");
        return -1;
    }

    // Create EthernetData to add to queue
    CAEthernetData *EthernetData = CACreateEthernetData(NULL, data, dataLength);
    if (!EthernetData)
    {
        OIC_LOG(ERROR, ETHERNET_ADAPTER_TAG, "Failed to create ethernetData!");
        return -1;
    }
    else
    {
        // Add message to send queue
        CAQueueingThreadAddData(g_sendQueueHandle, EthernetData, sizeof(CAEthernetData));

        OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "OUT");
        return dataLength;
    }
}

CAResult_t CAGetEthernetInterfaceInformation(CALocalConnectivity_t **info, uint32_t *size)
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");

    VERIFY_NON_NULL(info, ETHERNET_ADAPTER_TAG, "info");
    VERIFY_NON_NULL(size, ETHERNET_ADAPTER_TAG, "size");

    if (info == NULL || size == NULL)
    {
        return CA_STATUS_INVALID_PARAM;
    }

    CALocalConnectivity_t *netInfo = NULL;
    bool retVal = CAEthernetIsConnected();
    if (false == retVal)
    {
        OIC_LOG_V(ERROR, ETHERNET_ADAPTER_TAG,
                  "Failed to get interface address, Ethernet not Connected",
                  CA_ADAPTER_NOT_ENABLED);
        return CA_ADAPTER_NOT_ENABLED;
    }

    int32_t netInfoSize = 1;
#ifdef __WITH_DTLS__
    if (g_secureUnicastServerport)
    {
        netInfoSize++;
    }
#endif
    netInfo = (CALocalConnectivity_t *) OICCalloc(netInfoSize, sizeof(CALocalConnectivity_t));
    VERIFY_NON_NULL_RET(netInfo, ETHERNET_ADAPTER_TAG, "malloc failed", CA_MEMORY_ALLOC_FAILED);

    char *ipAddress = NULL;
    char *ifcName = NULL;
    CAResult_t ret = CAEthernetGetInterfaceInfo(&ifcName, &ipAddress);
    if (CA_STATUS_OK != ret || NULL == ipAddress)
    {
        OIC_LOG_V(ERROR, ETHERNET_ADAPTER_TAG, "Failed to get interface info [%d]", ret);

        OICFree(netInfo);
        OICFree(ipAddress);
        OICFree(ifcName);
        return ret;
    }

    // Create local endpoint using util function
    CALocalConnectivity_t *endpoint = CAAdapterCreateLocalEndpoint(CA_ETHERNET, ipAddress);
    if (NULL == endpoint)
    {
        OIC_LOG_V(ERROR, ETHERNET_ADAPTER_TAG, "Failed to create Local Endpoint!",
                  CA_MEMORY_ALLOC_FAILED);
        OICFree(netInfo);
        OICFree(ipAddress);
        OICFree(ifcName);
        return CA_MEMORY_ALLOC_FAILED;
    }

    // copy unicast server information
    endpoint->isSecured = false;
    endpoint->addressInfo.IP.port = g_unicastServerport;
    memcpy(&netInfo[0], endpoint, sizeof(CALocalConnectivity_t));
    *size = 1;
#ifdef __WITH_DTLS__
    // copy secure unicast server information
    if (g_secureUnicastServerport)
    {
        endpoint->isSecured = true;
        endpoint->addressInfo.IP.port = g_secureUnicastServerport;
        memcpy(&netInfo[1], endpoint, sizeof(CALocalConnectivity_t));
        (*size)++;
    }
#endif
    *info = netInfo;

    OICFree(ipAddress);
    OICFree(ifcName);
    CAAdapterFreeLocalEndpoint(endpoint);

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

    // Stop unicast server and set client socket accordingly
    if (CA_STATUS_OK == CAEthernetStopUnicastServer())
    {
        CAEthernetSetUnicastSocket(-1);
        g_unicastServerport = -1;
    }

#ifdef __WITH_DTLS__
    // Stop secure unicast server and set client socket accordingly
    if (CA_STATUS_OK == CAEthernetStopSecureUnicastServer())
    {
        CAEthernetSetSecureUnicastSocket(-1);
        g_secureUnicastServerport = -1;
    }
#endif

    //Stop multicast server and set the state accordingly
    if (CA_STATUS_OK == CAEthernetStopMulticastServer())
    {
        g_isMulticastServerStarted = false;
    }

    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAStopEthernet()
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");

    g_startUnicastServerRequested = false;
    g_startMulticastServerRequested = false;

    // Stop ethernet network monitor
    CAEthernetStopNetworkMonitor();

    // Stop send queue thread
    if (g_sendQueueHandle && false == g_sendQueueHandle->isStop)
    {
        CAQueueingThreadStop(g_sendQueueHandle);
    }

    // Stop Unicast, Secured unicast and Multicast servers running
    CAEthernetStopServers();

    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CATerminateEthernet()
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");

#ifdef __WITH_DTLS__
    CADTLSSetAdapterCallbacks(NULL, NULL, DTLS_ETHERNET);
    CAAdapterNetDtlsDeInit();
#endif

    // Stop ethernet adapter
    CAStopEthernet();

    // Terminate Ethernet server
    CAEthernetTerminateServer();

    // Terminate network monitor
    CAEthernetSetConnectionStateChangeCallback(NULL);
    CAEthernetTerminateNetworkMonitor();

    // Terminate message queue handler
    CAEthernetDeinitializeQueueHandles();
    CAEthernetSetPacketReceiveCallback(NULL);

    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "OUT");
    return;
}

void CAEthernetSendDataThread(void *threadData)
{
    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "IN");

    CAEthernetData *ethernetData = (CAEthernetData *) threadData;
    if (!ethernetData)
    {
        OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "Invalid ethernet data!");
        return;
    }

    if (NULL != ethernetData->remoteEndpoint)
    {
        char *address = ethernetData->remoteEndpoint->addressInfo.IP.ipAddress;
        uint16_t port = ethernetData->remoteEndpoint->addressInfo.IP.port;

#ifdef __WITH_DTLS__
        if (!ethernetData->remoteEndpoint->isSecured)
        {
            CAEthernetSendData(address, port, ethernetData->data, ethernetData->dataLen, false,
                               ethernetData->remoteEndpoint->isSecured);
        }
        else
        {
            OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "CAAdapterNetDtlsEncrypt called!");
            uint8_t cacheFlag = 0;
            CAResult_t  result = CAAdapterNetDtlsEncrypt(address, port, ethernetData->data,
                                 ethernetData->dataLen, &cacheFlag, DTLS_ETHERNET);

            if (CA_STATUS_OK != result)
            {
                OIC_LOG(ERROR, ETHERNET_ADAPTER_TAG, "CAAdapterNetDtlsEncrypt failed!");
            }
            OIC_LOG_V(ERROR, ETHERNET_ADAPTER_TAG,
                "CAAdapterNetDtlsEncrypt returned with cache[%d]",cacheFlag);
        }
#else
        CAEthernetSendData(address, port, ethernetData->data, ethernetData->dataLen, false,
                           ethernetData->remoteEndpoint->isSecured);
#endif
    }
    else
    {
        OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "Send Multicast Data is called");
        CAEthernetSendData(CA_MULTICAST_IP, CA_MCAST_PORT, ethernetData->data,
                           ethernetData->dataLen, true, false);
    }

    OIC_LOG(DEBUG, ETHERNET_ADAPTER_TAG, "OUT");
}

CAEthernetData *CACreateEthernetData(const CARemoteEndpoint_t *remoteEndpoint, const void *data,
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
    {
        return;
    }

    CAAdapterFreeRemoteEndpoint(ethernetData->remoteEndpoint);
    OICFree(ethernetData->data);
    OICFree(ethernetData);
}

void CADataDestroyer(void *data, uint32_t size)
{
    CAEthernetData *etdata = (CAEthernetData *) data;

    CAFreeEthernetData(etdata);
}


