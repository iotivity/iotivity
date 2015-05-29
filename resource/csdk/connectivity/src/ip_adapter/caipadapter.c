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
#include "caipadapter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "caipinterface.h"
#include "caqueueingthread.h"
#include "caadapterutils.h"
#ifdef __WITH_DTLS__
#include "caadapternetdtls.h"
#endif
#include "camutex.h"
#include "uarraylist.h"
#include "logger.h"
#include "oic_malloc.h"

/**
 * @def IP_ADAPTER_TAG
 * @brief Logging tag for module name
 */
#define IP_ADAPTER_TAG "IP_ADAP"

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

/**
 * @var CAIPData
 * @brief Holds inter thread ip data information.
 */
typedef struct
{
    CARemoteEndpoint_t *remoteEndpoint;
    void *data;
    uint32_t dataLen;
} CAIPData;

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
 * @var g_sendQueueHandle
 * @brief Queue handle for Send Data
 */
static CAQueueingThread_t *g_sendQueueHandle = NULL;

/**
 * @var g_threadPool
 * @brief ThreadPool for storing ca_thread_pool_t handle passed from CA
 */
static ca_thread_pool_t g_threadPool = NULL;

static CAResult_t CAIPInitializeQueueHandles();

static void CAIPDeinitializeQueueHandles();

static void CAIPNotifyNetworkChange(const char *address, uint16_t port,
                                          CANetworkStatus_t status);

static void CAIPConnectionStateCB(const char *ipAddress, CANetworkStatus_t status);

static void CAIPPacketReceivedCB(const char *ipAddress, uint16_t port, const void *data,
                                       uint32_t dataLength, bool isSecured);
#ifdef __WITH_DTLS__
static uint32_t CAIPPacketSendCB(const char *ipAddress, uint16_t port,
                                       const void *data, uint32_t dataLength);
#endif

static CAResult_t CAIPStopServers();

static void CAIPSendDataThread(void *threadData);

static CAIPData *CACreateIPData(const CARemoteEndpoint_t *remoteEndpoint,
                                            const void *data, uint32_t dataLength);
void CAFreeIPData(CAIPData *ipData);

static void CADataDestroyer(void *data, uint32_t size);

CAResult_t CAIPInitializeQueueHandles()
{
    OIC_LOG(DEBUG, IP_ADAPTER_TAG, "IN");

    // Check if the message queue is already initialized
    if (g_sendQueueHandle)
    {
        OIC_LOG(DEBUG, IP_ADAPTER_TAG, "send queue handle is already initialized!");
        return CA_STATUS_OK;
    }

    // Create send message queue
    g_sendQueueHandle = OICMalloc(sizeof(CAQueueingThread_t));
    if (!g_sendQueueHandle)
    {
        OIC_LOG(ERROR, IP_ADAPTER_TAG, "Memory allocation failed!");
        return CA_MEMORY_ALLOC_FAILED;
    }

    if (CA_STATUS_OK != CAQueueingThreadInitialize(g_sendQueueHandle, g_threadPool,
                                                   CAIPSendDataThread, CADataDestroyer))
    {
        OIC_LOG(ERROR, IP_ADAPTER_TAG, "Failed to Initialize send queue thread");
        OICFree(g_sendQueueHandle);
        g_sendQueueHandle = NULL;
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, IP_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CAIPDeinitializeQueueHandles()
{
    OIC_LOG(DEBUG, IP_ADAPTER_TAG, "IN");

    CAQueueingThreadDestroy(g_sendQueueHandle);
    OICFree(g_sendQueueHandle);
    g_sendQueueHandle = NULL;

    OIC_LOG(DEBUG, IP_ADAPTER_TAG, "OUT");
}

void CAIPNotifyNetworkChange(const char *address, uint16_t port, CANetworkStatus_t status)
{
    OIC_LOG(DEBUG, IP_ADAPTER_TAG, "IN");

    VERIFY_NON_NULL_VOID(address, IP_ADAPTER_TAG, "address is NULL");

    CALocalConnectivity_t *localEndpoint = CAAdapterCreateLocalEndpoint(CA_IPV4, address);
    if (!localEndpoint)
    {
        OIC_LOG(ERROR, IP_ADAPTER_TAG, "localEndpoint creation failed!");
        return;
    }

    localEndpoint->addressInfo.IP.port = port;

    if (g_networkChangeCallback)
    {
        g_networkChangeCallback(localEndpoint, status);
    }
    else
    {
        OIC_LOG(ERROR, IP_ADAPTER_TAG, "g_networkChangeCallback is NULL");
    }

    CAAdapterFreeLocalEndpoint(localEndpoint);

    OIC_LOG(DEBUG, IP_ADAPTER_TAG, "OUT");
}

void CAIPConnectionStateCB(const char *ipAddress, CANetworkStatus_t status)
{
    OIC_LOG(DEBUG, IP_ADAPTER_TAG, "IN");

    VERIFY_NON_NULL_VOID(ipAddress, IP_ADAPTER_TAG, "ipAddress is NULL");

    if (CA_INTERFACE_UP == status)
    {
        uint16_t port = CA_PORT;
        CAResult_t ret = CAIPStartUnicastServer(ipAddress, &port, false, false);
        if (CA_STATUS_OK == ret)
        {
            OIC_LOG_V(DEBUG, IP_ADAPTER_TAG, "Unicast server started on %d port", port);
        }
        else
        {
            OIC_LOG_V(ERROR, IP_ADAPTER_TAG, "Failed to start Unicast server port[%d]", ret);
        }

#ifdef __WITH_DTLS__
        port = CA_SECURE_PORT;
        ret = CAIPStartUnicastServer(ipAddress, &port, false, true);
        if (CA_STATUS_OK == ret)
        {
            OIC_LOG_V(DEBUG, IP_ADAPTER_TAG, "Secure Unicast server started on %d", port);
        }
        else
        {
            OIC_LOG_V(ERROR, IP_ADAPTER_TAG, "Failed to start secure Unicast server [%d]",
                      ret);
        }
#endif
        ret = CAIPStartMulticastServer(ipAddress, CA_MULTICAST_IP, CA_MCAST_PORT);
        if (CA_STATUS_OK == ret)
        {
            OIC_LOG_V(DEBUG, IP_ADAPTER_TAG, "Multicast server started on port[%d]",
                      CA_MCAST_PORT);
        }
        else
        {
            OIC_LOG_V(ERROR, IP_ADAPTER_TAG, "Failed to start Multicast server port[%d]",
                      ret);
        }

        // Notify network change to CA
        CAIPNotifyNetworkChange(ipAddress, port, status);
    }
    else
    {
        CAIPNotifyNetworkChange(ipAddress, 0, status);

        // Stop Unicast, Secured unicast and Multicast servers
        CAIPStopServer(ipAddress);
    }

    OIC_LOG(DEBUG, IP_ADAPTER_TAG, "OUT");
}

#ifdef __WITH_DTLS__
uint32_t CAIPPacketSendCB(const char *ipAddress, uint16_t port,
        const void *data, uint32_t dataLength)
{
    OIC_LOG(DEBUG, IP_ADAPTER_TAG, "IN");

    VERIFY_NON_NULL_RET(ipAddress, IP_ADAPTER_TAG, "ipAddress is NULL", 0);

    VERIFY_NON_NULL_RET(data, IP_ADAPTER_TAG, "data is NULL", 0);

    uint32_t sentLength = CAIPSendData(ipAddress, port, data, dataLength, false, true);

    OIC_LOG_V(DEBUG, IP_ADAPTER_TAG, "Successfully sent %d of encrypted data!", sentLength);

    OIC_LOG(DEBUG, IP_ADAPTER_TAG, "OUT");

    return sentLength;
}
#endif

void CAIPPacketReceivedCB(const char *ipAddress, uint16_t port, const void *data,
                                uint32_t dataLength, bool isSecured)
{
    OIC_LOG(DEBUG, IP_ADAPTER_TAG, "IN");

    VERIFY_NON_NULL_VOID(ipAddress, IP_ADAPTER_TAG, "ipAddress is NULL");

    VERIFY_NON_NULL_VOID(data, IP_ADAPTER_TAG, "data is NULL");

    OIC_LOG_V(DEBUG, IP_ADAPTER_TAG, "Address: %s, port:%d", ipAddress, port);

    // CA is freeing this memory
    CARemoteEndpoint_t *endPoint = CAAdapterCreateRemoteEndpoint(CA_IPV4, ipAddress, NULL );
    if (!endPoint)
    {
        OIC_LOG(ERROR, IP_ADAPTER_TAG, "EndPoint creation failed!");
        return;
    }
    endPoint->addressInfo.IP.port = port;
    endPoint->isSecured = isSecured;

    void *buf = OICCalloc(dataLength + 1, sizeof(char));
    if (!buf)
    {
        OIC_LOG(ERROR, IP_ADAPTER_TAG, "Memory Allocation failed!");
        CAAdapterFreeRemoteEndpoint(endPoint);
        return;
    }
    memcpy(buf, data, dataLength);
    if (g_networkPacketCallback)
    {
        g_networkPacketCallback(endPoint, buf, dataLength);
    }
    else
    {
        OICFree(buf);
        CAAdapterFreeRemoteEndpoint(endPoint);
    }

    OIC_LOG(DEBUG, IP_ADAPTER_TAG, "OUT");
}

CAResult_t CAInitializeIP(CARegisterConnectivityCallback registerCallback,
                                CANetworkPacketReceivedCallback networkPacketCallback,
                                CANetworkChangeCallback netCallback, ca_thread_pool_t handle)
{
    OIC_LOG(DEBUG, IP_ADAPTER_TAG, "IN");
    VERIFY_NON_NULL(registerCallback, IP_ADAPTER_TAG, "registerCallback");
    VERIFY_NON_NULL(networkPacketCallback, IP_ADAPTER_TAG, "networkPacketCallback");
    VERIFY_NON_NULL(netCallback, IP_ADAPTER_TAG, "netCallback");
    VERIFY_NON_NULL(handle, IP_ADAPTER_TAG, "thread pool handle");

    g_threadPool = handle;
    g_networkChangeCallback = netCallback;
    g_networkPacketCallback = networkPacketCallback;

    CAResult_t ret = CAIPInitializeNetworkMonitor(g_threadPool);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(ERROR, IP_ADAPTER_TAG, "Failed to initialize n/w monitor![%d]", ret);
        return ret;
    }
    CAIPSetConnectionStateChangeCallback(CAIPConnectionStateCB);

    ret = CAIPInitializeServer(g_threadPool);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(ERROR, IP_ADAPTER_TAG, "Failed to initialize server![%d]", ret);
        CATerminateIP();
        return ret;
    }

    CAIPSetPacketReceiveCallback(CAIPPacketReceivedCB);
#ifdef __WITH_DTLS__
    CAAdapterNetDtlsInit();

    CADTLSSetAdapterCallbacks(CAIPPacketReceivedCB, CAIPPacketSendCB, DTLS_IP);
#endif

    CAConnectivityHandler_t ipHandler;
    ipHandler.startAdapter = CAStartIP;
    ipHandler.startListenServer = CAStartIPListeningServer;
    ipHandler.startDiscoveryServer = CAStartIPDiscoveryServer;
    ipHandler.sendData = CASendIPUnicastData;
    ipHandler.sendDataToAll = CASendIPMulticastData;
    ipHandler.GetnetInfo = CAGetIPInterfaceInformation;
    ipHandler.readData = CAReadIPData;
    ipHandler.stopAdapter = CAStopIP;
    ipHandler.terminate = CATerminateIP;
    registerCallback(ipHandler, CA_IPV4);

    if (CA_STATUS_OK != CAIPInitializeQueueHandles())
    {
        OIC_LOG(ERROR, IP_ADAPTER_TAG, "Failed to Initialize Queue Handle");
        CATerminateIP();
        return CA_STATUS_FAILED;
    }

    OIC_LOG(INFO, IP_ADAPTER_TAG, "OUT IntializeIP is Success");
    return CA_STATUS_OK;
}

CAResult_t CAStartIP()
{
    OIC_LOG(DEBUG, IP_ADAPTER_TAG, "IN");

    // Start monitoring IP network
    CAResult_t ret = CAIPStartNetworkMonitor();
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG(ERROR, IP_ADAPTER_TAG, "Failed to Start n/w monitor");
        return ret;
    }

    // Start send queue thread
    if (CA_STATUS_OK != CAQueueingThreadStart(g_sendQueueHandle))
    {
        OIC_LOG(ERROR, IP_ADAPTER_TAG, "Failed to Start Send Data Thread");
        return CA_STATUS_FAILED;
    }

    bool retVal = CAIPIsConnected();
    if (false == retVal)
    {
        OIC_LOG(DEBUG, IP_ADAPTER_TAG, "IP is not Connected");
        return CA_STATUS_OK;
    }

    u_arraylist_t *netInterfaceList = u_arraylist_create();

    VERIFY_NON_NULL(netInterfaceList, IP_ADAPTER_TAG, "netInterfaceList is NULL");

    ret = CAIPGetInterfaceInfo(&netInterfaceList);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(ERROR, IP_ADAPTER_TAG, "Failed to get IP interface info [%d]", ret);
        CAClearNetInterfaceInfoList(netInterfaceList);
        return ret;
    }

    uint32_t listIndex = 0;
    uint32_t listLength = u_arraylist_length(netInterfaceList);
    for (listIndex = 0; listIndex < listLength; listIndex++)
    {
        CANetInfo_t *netInfo = (CANetInfo_t *) u_arraylist_get(netInterfaceList, listIndex);
        if (!netInfo)
        {
            continue;
        }
        uint16_t unicastPort = CA_PORT;
        ret = CAIPStartUnicastServer(netInfo->ipAddress, &unicastPort, false, false);
        if (CA_STATUS_OK == ret)
        {
            OIC_LOG_V(DEBUG, IP_ADAPTER_TAG, "Unicast server started on %d port",
                      unicastPort);
        }

#ifdef __WITH_DTLS__
        unicastPort = CA_SECURE_PORT;
        ret = CAIPStartUnicastServer(netInfo->ipAddress, &unicastPort, false, true);

        if (CA_STATUS_OK == ret)
        {
            OIC_LOG_V(DEBUG, IP_ADAPTER_TAG,
                      "Secure Unicast server started on %d port", unicastPort);
        }
#endif
    }
    CAClearNetInterfaceInfoList(netInterfaceList);
    OIC_LOG(DEBUG, IP_ADAPTER_TAG, "OUT");
    return ret;
}

CAResult_t CAStartIPListeningServer()
{
    OIC_LOG(DEBUG, IP_ADAPTER_TAG, "IN");

    bool retVal = CAIPIsConnected();
    if (false == retVal)
    {
        OIC_LOG(DEBUG, IP_ADAPTER_TAG,
                  "IP not Connected. Couldn't start multicast server");
        return CA_STATUS_OK;
    }

    u_arraylist_t *netInterfaceList = u_arraylist_create();

    VERIFY_NON_NULL(netInterfaceList, IP_ADAPTER_TAG, "netInterfaceList is NULL");

    CAResult_t ret = CAIPGetInterfaceInfo(&netInterfaceList);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(ERROR, IP_ADAPTER_TAG, "Failed to get IP interface info [%d]", ret);
        CAClearNetInterfaceInfoList(netInterfaceList);
        return ret;
    }

    uint32_t listIndex = 0;
    uint32_t listLength = u_arraylist_length(netInterfaceList);
    for (listIndex = 0; listIndex < listLength; listIndex++)
    {

        CANetInfo_t *netInfo = (CANetInfo_t *) u_arraylist_get(netInterfaceList, listIndex);
        if (!netInfo)
        {
            continue;
        }

        OIC_LOG_V(DEBUG, IP_ADAPTER_TAG, "Ip address for multicast interface %s",
                  netInfo->ipAddress);
        ret = CAIPStartMulticastServer(netInfo->ipAddress, CA_MULTICAST_IP, CA_MCAST_PORT);
        if (CA_STATUS_OK == ret)
        {
            OIC_LOG(INFO, IP_ADAPTER_TAG, "Multicast Server is Started Successfully");
        }
    }

    CAClearNetInterfaceInfoList(netInterfaceList);
    OIC_LOG(DEBUG, IP_ADAPTER_TAG, "OUT");
    return ret;
}

CAResult_t CAStartIPDiscoveryServer()
{
    OIC_LOG(DEBUG, IP_ADAPTER_TAG, "IN");
    return CAStartIPListeningServer();
}

int32_t CASendIPUnicastData(const CARemoteEndpoint_t *remoteEndpoint, const void *data,
                                  uint32_t dataLength)
{
    OIC_LOG(DEBUG, IP_ADAPTER_TAG, "IN");

    VERIFY_NON_NULL_RET(remoteEndpoint, IP_ADAPTER_TAG, "remoteEndpoint", -1);
    VERIFY_NON_NULL_RET(data, IP_ADAPTER_TAG, "data", -1);
    VERIFY_NON_NULL_RET(g_sendQueueHandle, IP_ADAPTER_TAG, "sendQueueHandle", -1);

    if (0 == dataLength)
    {
        OIC_LOG(ERROR, IP_ADAPTER_TAG, "Invalid Data Length");
        return -1;
    }

    // Create IPData to add to queue
    CAIPData *ipData = CACreateIPData(remoteEndpoint, data, dataLength);
    if (!ipData)
    {
        OIC_LOG(ERROR, IP_ADAPTER_TAG, "Failed to create ipData!");
        return -1;
    }
    else
    {
        // Add message to send queue
        CAQueueingThreadAddData(g_sendQueueHandle, ipData, sizeof(CAIPData));

        OIC_LOG(DEBUG, IP_ADAPTER_TAG, "OUT");
        return dataLength;
    }
}

int32_t CASendIPMulticastData(const void *data, uint32_t dataLength)
{
    OIC_LOG(DEBUG, IP_ADAPTER_TAG, "IN");

    VERIFY_NON_NULL_RET(data, IP_ADAPTER_TAG, "data", -1);
    VERIFY_NON_NULL_RET(g_sendQueueHandle, IP_ADAPTER_TAG, "sendQueueHandle", -1);

    if (0 == dataLength)
    {
        OIC_LOG(ERROR, IP_ADAPTER_TAG, "Invalid Data Length");
        return -1;
    }

    // Create IPData to add to queue
    CAIPData *ipData = CACreateIPData(NULL, data, dataLength);
    if (!ipData)
    {
        OIC_LOG(ERROR, IP_ADAPTER_TAG, "Failed to create ipData!");
        return -1;
    }
    else
    {
        // Add message to send queue
        CAQueueingThreadAddData(g_sendQueueHandle, ipData, sizeof(CAIPData));

        OIC_LOG(DEBUG, IP_ADAPTER_TAG, "OUT");
        return dataLength;
    }
}

CAResult_t CAGetIPInterfaceInformation(CALocalConnectivity_t **info, uint32_t *size)
{
    OIC_LOG(DEBUG, IP_ADAPTER_TAG, "IN");

    VERIFY_NON_NULL(info, IP_ADAPTER_TAG, "info is NULL");
    VERIFY_NON_NULL(size, IP_ADAPTER_TAG, "size is NULL");

    bool retVal = CAIPIsConnected();
    if (false == retVal)
    {
        OIC_LOG(ERROR, IP_ADAPTER_TAG,
                "Failed to get interface address, IP not Connected");
        return CA_ADAPTER_NOT_ENABLED;
    }

    u_arraylist_t *netInterfaceList = u_arraylist_create();

    VERIFY_NON_NULL(netInterfaceList, IP_ADAPTER_TAG, "netInterfaceList is NULL");

    CAResult_t ret = CAIPGetInterfaceInfo(&netInterfaceList);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(ERROR, IP_ADAPTER_TAG, "CAIPGetInterfaceInfo failed:%d", ret);
        CAClearNetInterfaceInfoList(netInterfaceList);
        return ret;
    }

    uint32_t listLength = u_arraylist_length(netInterfaceList);
    uint32_t netInfoSize = listLength;

#ifdef __WITH_DTLS__
    if (listLength)
    {
        netInfoSize = listLength * 2;
    }
#endif

    CALocalConnectivity_t *conInfo = (CALocalConnectivity_t *) OICCalloc(
                                      netInfoSize, sizeof(CALocalConnectivity_t));
    if (!conInfo)
    {
        OIC_LOG(ERROR, IP_ADAPTER_TAG, "Malloc Failed");
        CAClearNetInterfaceInfoList(netInterfaceList);
        return CA_MEMORY_ALLOC_FAILED;
    }

    uint32_t listIndex = 0;
    uint32_t count = 0;
    for (listIndex = 0; listIndex < listLength; listIndex++)
    {
        CANetInfo_t *netInfo = (CANetInfo_t *) u_arraylist_get(netInterfaceList, listIndex);
        if (!netInfo)
        {
            continue;
        }

        conInfo[count].type = CA_IPV4;
        conInfo[count].isSecured = false;
        conInfo[count].addressInfo.IP.port = CAGetServerPortNum(netInfo->ipAddress, false);
        strncpy(conInfo[count].addressInfo.IP.ipAddress, netInfo->ipAddress,
                strlen(netInfo->ipAddress));

#ifdef __WITH_DTLS__
        // copy secure unicast server information
        {
            count ++;
            conInfo[count].type = CA_IPV4;
            conInfo[count].isSecured = true;
            conInfo[count].addressInfo.IP.port = CAGetServerPortNum(netInfo->ipAddress, true);
            strncpy(conInfo[count].addressInfo.IP.ipAddress, netInfo->ipAddress,
                    strlen(netInfo->ipAddress));
        }
#endif
        count ++;
    }
    *size = count;
    *info = conInfo;
    CAClearNetInterfaceInfoList(netInterfaceList);

    OIC_LOG(DEBUG, IP_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAReadIPData()
{
    OIC_LOG(DEBUG, IP_ADAPTER_TAG, "IN");

    OIC_LOG(DEBUG, IP_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAIPStopServers()
{
    OIC_LOG(DEBUG, IP_ADAPTER_TAG, "IN");

    // Stop all unicast and multicast servers.
    if (CA_STATUS_OK == CAIPStopAllServers())
    {
        OIC_LOG(DEBUG, IP_ADAPTER_TAG, "CAIPStopAllServers success");
    }

    OIC_LOG(DEBUG, IP_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAStopIP()
{
    OIC_LOG(DEBUG, IP_ADAPTER_TAG, "IN");

#ifdef __WITH_DTLS__
    CAAdapterNetDtlsDeInit();
#endif

    // Stop IP network monitor
    CAIPStopNetworkMonitor();

    // Stop send queue thread
    if (g_sendQueueHandle)
    {
        CAQueueingThreadStop(g_sendQueueHandle);
    }

    // Stop Unicast, Secured unicast and Multicast servers running
    CAIPStopServers();

    OIC_LOG(DEBUG, IP_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CATerminateIP()
{
    OIC_LOG(DEBUG, IP_ADAPTER_TAG, "IN");

    // Stop IP adapter
    CAStopIP();

#ifdef __WITH_DTLS__
    CADTLSSetAdapterCallbacks(NULL, NULL, DTLS_IP);
#endif

    CAIPSetPacketReceiveCallback(NULL);

    // Terminate IP server
    CAIPTerminateServer();

    // Terminate network monitor
    CAIPSetConnectionStateChangeCallback(NULL);
    CAIPTerminateNetworkMonitor();

    // Terminate message queue handler
    CAIPDeinitializeQueueHandles();

    OIC_LOG(DEBUG, IP_ADAPTER_TAG, "OUT");
}

void CAIPSendDataThread(void *threadData)
{
    OIC_LOG(DEBUG, IP_ADAPTER_TAG, "IN");

    CAIPData *ipData = (CAIPData *) threadData;
    if (!ipData)
    {
        OIC_LOG(DEBUG, IP_ADAPTER_TAG, "Invalid ip data!");
        return;
    }

    //If remoteEndpoint is NULL, its Multicast, else its Unicast.
    if (ipData->remoteEndpoint)
    {
        //Processing for sending unicast
        char *address = ipData->remoteEndpoint->addressInfo.IP.ipAddress;
        uint16_t port = ipData->remoteEndpoint->addressInfo.IP.port;

#ifdef __WITH_DTLS__
        if (!ipData->remoteEndpoint->isSecured)
        {
            OIC_LOG(DEBUG, IP_ADAPTER_TAG, "Send Unicast Data is called");
            CAIPSendData(address, port, ipData->data, ipData->dataLen, false,
                               ipData->remoteEndpoint->isSecured);
        }
        else
        {
            OIC_LOG(DEBUG, IP_ADAPTER_TAG, "CAAdapterNetDtlsEncrypt called!");
            uint8_t cacheFlag = 0;
            CAResult_t result = CAAdapterNetDtlsEncrypt(address, port, ipData->data,
                                                        ipData->dataLen, &cacheFlag,
                                                        DTLS_IP);

            if (CA_STATUS_OK != result)
            {
                OIC_LOG(ERROR, IP_ADAPTER_TAG, "CAAdapterNetDtlsEncrypt failed!");
            }
            OIC_LOG_V(DEBUG, IP_ADAPTER_TAG,
                      "CAAdapterNetDtlsEncrypt returned with cache[%d]", cacheFlag);
        }
#else
        CAIPSendData(address, port, ipData->data, ipData->dataLen, false,
                           ipData->remoteEndpoint->isSecured);
#endif
    }
    else
    {
        //Processing for sending multicast
        OIC_LOG(DEBUG, IP_ADAPTER_TAG, "Send Multicast Data is called");
        CAIPSendData(CA_MULTICAST_IP, CA_MCAST_PORT, ipData->data,
                           ipData->dataLen, true, false);
    }

    OIC_LOG(DEBUG, IP_ADAPTER_TAG, "OUT");
}

CAIPData *CACreateIPData(const CARemoteEndpoint_t *remoteEndpoint, const void *data,
                                     uint32_t dataLength)
{
    VERIFY_NON_NULL_RET(data, IP_ADAPTER_TAG, "IPData is NULL", NULL);

    CAIPData *ipData = (CAIPData *) OICMalloc(sizeof(CAIPData));
    if (!ipData)
    {
        OIC_LOG(ERROR, IP_ADAPTER_TAG, "Memory allocation failed!");
        return NULL;
    }

    ipData->remoteEndpoint = CAAdapterCopyRemoteEndpoint(remoteEndpoint);
    ipData->data = (void *) OICMalloc(dataLength);
    if (!ipData->data)
    {
        OIC_LOG(ERROR, IP_ADAPTER_TAG, "Memory allocation failed!");
        CAFreeIPData(ipData);
        return NULL;
    }

    memcpy(ipData->data, data, dataLength);
    ipData->dataLen = dataLength;

    return ipData;
}

void CAFreeIPData(CAIPData *ipData)
{
    VERIFY_NON_NULL_VOID(ipData, IP_ADAPTER_TAG, "ipData is NULL");

    CAAdapterFreeRemoteEndpoint(ipData->remoteEndpoint);
    OICFree(ipData->data);
    OICFree(ipData);
}

void CADataDestroyer(void *data, uint32_t size)
{
    CAIPData *etdata = (CAIPData *) data;

    CAFreeIPData(etdata);
}

