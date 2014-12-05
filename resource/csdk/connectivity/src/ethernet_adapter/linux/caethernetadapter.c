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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "caethernetadapter.h"
//#include "config.h"
//#include "coap.h"
#include "caethernetcore.h"
#include "logger.h"
#include "oic_malloc.h"

#define TAG PCF("CA")

// received packet callback
static CANetworkPacketReceivedCallback gEthernetReceivedCallback = NULL;
static u_thread_pool_t gThreadPoolHandle = NULL;

static void CAEthernetPacketReceiveCallback(char* address, const char* data, uint32_t dataLen)
{
    OIC_LOG_V(DEBUG, TAG, "CAethernetPacketReceiveCallback, from: %s, data: %s", address, data);

    // call the callback
    if (gEthernetReceivedCallback != NULL)
    {
        CARemoteEndpoint_t* endpoint = NULL;
        endpoint = (CARemoteEndpoint_t*) OICMalloc(sizeof(CARemoteEndpoint_t));

        // set address
        memset((void*) endpoint->addressInfo.IP.ipAddress, 0, CA_IPADDR_SIZE);
        if (CA_IPADDR_SIZE > strlen(address))
        {
            strcpy((char*) endpoint->addressInfo.IP.ipAddress, address);
        }
        OICFree(address);

        // set connectivity type
        endpoint->connectivityType = CA_ETHERNET;

        gEthernetReceivedCallback(endpoint, (void *) data, dataLen);
    }
}

CAResult_t CAInitializeEthernet(CARegisterConnectivityCallback registerCallback,
        CANetworkPacketReceivedCallback reqRespCallback, CANetworkChangeCallback netCallback,
        u_thread_pool_t handle)
{
    OIC_LOG(DEBUG, TAG, "IntializeEthernet");

    gEthernetReceivedCallback = reqRespCallback;
    gThreadPoolHandle = handle;

    // register handlers
    CAConnectivityHandler_t handler;
    memset(&handler, 0, sizeof(CAConnectivityHandler_t));

    handler.startAdapter = CAStartEthernet;
    handler.startListenServer = CAStartEthernetListeningServer;
    handler.startDiscoverServer = CAStartEthernetDiscoveryServer;
    handler.sendData = CASendEthernetUnicastData;
    handler.sendDataToAll = CASendEthernetMulticastData;
    handler.GetnetInfo = CAGetEthernetInterfaceInformation;
    handler.readData = CAReadEthernetData;
    handler.stopAdapter = CAStopEthernet;
    handler.terminate = CATerminateEthernet;

    registerCallback(handler, CA_ETHERNET);

    CAEthernetSetCallback(CAEthernetPacketReceiveCallback);

    return CA_STATUS_OK;
}

void CATerminateEthernet()
{
    OIC_LOG(DEBUG, TAG, "TerminateEthernet");

    CAEthernetTerminate();
}

CAResult_t CAStartEthernet()
{
    OIC_LOG(DEBUG, TAG, "CAStartEthernet");
    //CAEthernetInitialize();
    CAEthernetInitialize(gThreadPoolHandle);

    OIC_LOG(DEBUG, TAG, "CAEthernetStartUnicastServer");
    int32_t res = CAEthernetStartUnicastServer();

    if (res < 0)
        return CA_STATUS_FAILED;

    return CA_STATUS_OK;
}

CAResult_t CAStopEthernet()
{
    OIC_LOG(DEBUG, TAG, "CAStopEthernet");

    // ToDo:

    return CA_STATUS_OK;
}

CAResult_t CAStartEthernetListeningServer()
{
    OIC_LOG(DEBUG, TAG, "StartEthernetListeningServer");

    int32_t res = CAEthernetStartMulticastServer();

    if (res < 0)
        return CA_STATUS_FAILED;

    return CA_STATUS_OK;
}

CAResult_t CAStartEthernetDiscoveryServer()
{
    OIC_LOG(DEBUG, TAG, "StartEthernetDiscoveryServer");

    int32_t res = CAEthernetStartMulticastServer();

    if (res < 0)
        return CA_STATUS_FAILED;

    return CA_STATUS_OK;
}

uint32_t CASendEthernetUnicastData(const CARemoteEndpoint_t* endpoint, void* data, uint32_t dataLen)
{
    OIC_LOG(DEBUG, TAG, "SendEthernetUnicastData");

    CAEthernetSendUnicastMessage(endpoint->addressInfo.IP.ipAddress, data, dataLen);

    return 0;
}

uint32_t CASendEthernetMulticastData(void* data, uint32_t dataLen)
{
    OIC_LOG(DEBUG, TAG, "CASendEthernetMulticastData");

    CAEthernetSendMulticastMessage("0.0.0.0", (char*) data, dataLen);

    return 0;
}

CAResult_t CAGetEthernetInterfaceInformation(CALocalConnectivity_t** info, uint32_t* size)
{
    OIC_LOG(DEBUG, TAG, "GetEthernetInterfaceInformation");

    CAGetEthernetInterfaceInfo(info, size);

    return CA_STATUS_OK;
}

CAResult_t CAReadEthernetData()
{
    OIC_LOG(DEBUG, TAG, "Read Ethernet Data");

    // ToDo:

    return CA_STATUS_OK;
}

