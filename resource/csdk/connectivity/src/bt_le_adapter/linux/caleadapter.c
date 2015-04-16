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

#include "caleadapter.h"
#include "logger.h"

#define TAG PCF("CA")

static CANetworkPacketReceivedCallback g_leReceivedCallback = NULL;
static ca_thread_pool_t g_threadPoolHandle = NULL;

CAResult_t CAInitializeLE(CARegisterConnectivityCallback registerCallback,
                          CANetworkPacketReceivedCallback reqRespCallback, CANetworkChangeCallback netCallback,
                          ca_thread_pool_t handle)
{
    OIC_LOG(DEBUG, TAG, "CAInitializeLE");

    g_leReceivedCallback = reqRespCallback;
    g_threadPoolHandle = handle;

    // register handlers
    CAConnectivityHandler_t handler;
    memset(&handler, 0, sizeof(CAConnectivityHandler_t));

    handler.startAdapter = CAStartLE;
    handler.startListenServer = CAStartLEListeningServer;
    handler.startDiscoveryServer = CAStartLEDiscoveryServer;
    handler.sendData = CASendLEUnicastData;
    handler.sendDataToAll = CASendLEMulticastData;
    handler.GetnetInfo = CAGetLEInterfaceInformation;
    handler.readData = CAReadLEData;
    handler.stopAdapter = CAStopLE;
    handler.terminate = CATerminateLE;

    registerCallback(handler, CA_LE);

    return CA_STATUS_OK;
}

CAResult_t CAStartLE()
{
    OIC_LOG(DEBUG, TAG, "CAStartLE");

    return CA_STATUS_OK;
}

CAResult_t CAStartLEListeningServer()
{
    OIC_LOG(DEBUG, TAG, "CAStartLEListeningServer");

    return CA_STATUS_OK;
}

CAResult_t CAStartLEDiscoveryServer()
{
    OIC_LOG(DEBUG, TAG, "CAStartLEDiscoveryServer");

    return CA_STATUS_OK;
}

int32_t CASendLEUnicastData(const CARemoteEndpoint_t *endpoint, const void *data, uint32_t dataLen)
{
    OIC_LOG(DEBUG, TAG, "CASendLEUnicastData");

    return -1;
}

int32_t CASendLEMulticastData(const void *data, uint32_t dataLen)
{
    OIC_LOG(DEBUG, TAG, "CASendLEMulticastData");

    return -1;
}

CAResult_t CAGetLEInterfaceInformation(CALocalConnectivity_t **info, uint32_t *size)
{
    OIC_LOG(DEBUG, TAG, "CAGetLEInterfaceInformation");

    return CA_STATUS_OK;
}

CAResult_t CAReadLEData()
{
    OIC_LOG(DEBUG, TAG, "Read LE Data");

    return CA_STATUS_OK;
}

CAResult_t CAStopLE()
{
    OIC_LOG(DEBUG, TAG, "CAStopLE");

    return CA_STATUS_OK;
}

void CATerminateLE()
{
    OIC_LOG(DEBUG, TAG, "TerminatLE");
}

