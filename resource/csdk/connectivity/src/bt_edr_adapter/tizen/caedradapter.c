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

/**
 * @file caedradapter.c
 * @brief This file contains the APIs for EDR adapters to be implemented
 */

#include "caedradapter.h"
#include "cabtmanager.h"
#include "cabtutils.h"
#include "caadapterutils.h"
#include "logger.h"

static int32_t gDiscoveryServerID = -1;
static int32_t gListeningServerID = -1;
static int32_t gNotificationServerID = -1;

static CAResult_t CAStartServer(const char *serviceUUID, int32_t *serverID);

CAResult_t CAInitializeEDR(CARegisterConnectivityCallback registerCallback,
                           CANetworkPacketReceivedCallback networkPacketCallback,
                           CANetworkChangeCallback netChangeCallback,
                           u_thread_pool_t handle)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    CAResult_t err = CA_STATUS_OK;

    //Input validation
    VERIFY_NON_NULL(registerCallback, BLUETOOTH_ADAPTER_TAG, "register callback is NULL");
    VERIFY_NON_NULL(networkPacketCallback, BLUETOOTH_ADAPTER_TAG, "data receive callback is NULL");
    VERIFY_NON_NULL(netChangeCallback, BLUETOOTH_ADAPTER_TAG, "network changge callback is NULL");
    VERIFY_NON_NULL(handle, BLUETOOTH_ADAPTER_TAG, "Thread pool hanlde is NULL");

    //Register the callbacks with BT Manager
    CABTManagerSetPacketReceivedCallback(networkPacketCallback);
    CABTManagerSetNetworkChangeCallback(netChangeCallback);

    //Initialize BT Manager
    err = CABTManagerIntialize(handle);
    if (CA_STATUS_OK != err && CA_ADAPTER_NOT_ENABLED != err)
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "BT Manger initialize failed!, error number [%d]",
                  err);
        return err;
    }

    CAConnectivityHandler_t handler;
    handler.startAdapter = CAStartEDR;
    handler.startListenServer = CAStartEDRListeningServer;
    handler.startDiscoverServer = CAStartEDRDiscoveryServer;
    handler.sendData = CASendEDRUnicastData;
    handler.sendDataToAll = CASendEDRMulticastData;
    handler.startNotifyServer = CAStartEDRNotifyServer;
    handler.sendNotification = CASendEDRNotification;
    handler.GetnetInfo = CAGetEDRInterfaceInformation;
    handler.readData = CAReadEDRData;
    handler.stopAdapter = CAStopEDR;
    handler.terminate = CATerminateEDR;
    registerCallback(handler, CA_EDR);

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
    return err;
}

CAResult_t CAStartEDR(void)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    CAResult_t err = CA_STATUS_OK;

    if (CA_STATUS_OK != (err = CABTManagerStart()))
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "BT Manger start failed!, error number [%d] ",
                  err);
    }

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
    return err;
}

CAResult_t CAStartEDRListeningServer(void)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    return CAStartServer(OIC_BT_SERVICE_ID, &gListeningServerID);
}

CAResult_t CAStartEDRDiscoveryServer(void)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    return CAStartServer(OIC_BT_SERVICE_ID, &gDiscoveryServerID);
}

uint32_t CASendEDRUnicastData(const CARemoteEndpoint_t *endpoint, void *data, uint32_t dataLen)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    CAResult_t err = CA_STATUS_OK;
    uint32_t sentLen = 0;
    const char *serviceUUID = OIC_BT_SERVICE_ID;

    //Input validation
    VERIFY_NON_NULL_RET(endpoint, BLUETOOTH_ADAPTER_TAG, "Remote endpoint is null", 0);
    VERIFY_NON_NULL_RET(data, BLUETOOTH_ADAPTER_TAG, "Data is null", 0);

    if (0 == strlen(endpoint->addressInfo.BT.btMacAddress))
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Invalid input: Address is empty!");
        return 0;
    }

    if (0 == dataLen)
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Invalid input: data length is zero!");
        return 0;
    }

    if (CA_STATUS_OK != (err = CABTManagerSendData(endpoint->addressInfo.BT.btMacAddress,
                               serviceUUID, data,
                               dataLen, &sentLen)))
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Send unicast data failed!, error num [%d]", err);
        return 0;
    }

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
    return sentLen;
}

uint32_t CASendEDRMulticastData(void *data, uint32_t dataLen)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    CAResult_t err = CA_STATUS_OK;
    uint32_t sentLen = 0;
    const char *serviceUUID = OIC_BT_SERVICE_ID;

    //Input validation
    VERIFY_NON_NULL_RET(data, BLUETOOTH_ADAPTER_TAG, "Data is null", 0);

    if (0 == dataLen)
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Invalid input: data length is zero!");
        return 0;
    }

    if (CA_STATUS_OK != (err = CABTManagerSendData(NULL, serviceUUID, data, dataLen,
                               &sentLen)))
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, " Send multicast data failed!, error num [%d]",
                  err);
        return 0;
    }

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
    return sentLen;
}

CAResult_t CAStartEDRNotifyServer(void)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    return CAStartServer(OIC_BT_SERVICE_ID, &gNotificationServerID);
}

uint32_t CASendEDRNotification(const CARemoteEndpoint_t *endpoint, void *data, uint32_t dataLen)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    return CASendEDRUnicastData(endpoint, data, dataLen);
}

CAResult_t CAGetEDRInterfaceInformation(CALocalConnectivity_t **info, uint32_t *size)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    VERIFY_NON_NULL(info, BLUETOOTH_ADAPTER_TAG, "LocalConnectivity info is null");

    CAResult_t err = CA_STATUS_OK;
    *size = 0;
    if (CA_STATUS_OK != (err = CABTManagerGetInterface(info)))
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG,
                  "Failed to get local interface information!, error num [%d]", err);
        return err;
    }

    *size = 1;
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
    return err;
}

CAResult_t CAReadEDRData(void)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    return CABTManagerReadData();
}

CAResult_t CAStopEDR(void)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    //Stop the Discovery server
    if (-1 < gDiscoveryServerID)
    {
        CABTManagerStopServer(gDiscoveryServerID);
    }

    //Stop the Listening server
    if (-1 < gListeningServerID)
    {
        CABTManagerStopServer(gListeningServerID);
    }

    //Stop the Notification server
    if (-1 < gNotificationServerID)
    {
        CABTManagerStopServer(gNotificationServerID);
    }

    //Stop the adapter
    CABTManagerStop();

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CATerminateEDR(void)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    //Terminate BT Manager
    CABTManagerTerminate();

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
}

CAResult_t CAStartServer(const char *serviceUUID, int32_t *serverID)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    CAResult_t err = CA_STATUS_OK;

    //Input validation
    VERIFY_NON_NULL(serviceUUID, BLUETOOTH_ADAPTER_TAG,  "Service UUID is NULL");
    VERIFY_NON_NULL(serverID, BLUETOOTH_ADAPTER_TAG,  "Server ID is NULL");
    if (0 == strlen(serviceUUID))
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Invalid input: Service UUID is empty!");
        return CA_STATUS_INVALID_PARAM;
    }

    if (CA_STATUS_OK != (err = CABTManagerStartServer(serviceUUID, serverID)))
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Failed to start RFCOMM server!, error num [%d]",
                  err);
        return err;
    }

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
    return err;
}
