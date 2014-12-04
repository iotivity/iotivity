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
 * @file    cabtserver.c
 * @brief   This    file provides the APIs to start and stop RFCOMM server.
 */

#include "cabtserver.h"
#include "caadapterutils.h"
#include "cabtutils.h"
#include "logger.h"

static int32_t gMaxPendingConnections = 10;

CAResult_t CABTServerStart(const char *serviceUUID, int32_t *serverFD)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    int err = BT_ERROR_NONE;
    bool isRunning = false;
    int socketFD;

    VERIFY_NON_NULL(serviceUUID, BLUETOOTH_ADAPTER_TAG, "Service UUID is null");
    VERIFY_NON_NULL(serverFD, BLUETOOTH_ADAPTER_TAG, "Server fd holder is null");

    if (0 >= strlen(serviceUUID))
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Invalid input: Empty service uuid!");
        return CA_STATUS_INVALID_PARAM;
    }

    if (BT_ERROR_NONE != bt_adapter_is_service_used(serviceUUID, &isRunning))
    {
        OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG,
                  "Unable to find whether service is already running or not!");
        return CA_STATUS_FAILED;
    }

    if (true == isRunning)
    {
        OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "Service is already running with this UUID!");
        return CA_SERVER_STARTED_ALREADY;
    }

    //Registers a rfcomm socket with a specific service_uuid .
    if (BT_ERROR_NONE != (err = bt_socket_create_rfcomm(serviceUUID, &socketFD)))
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Failed to create rfcomm socket!, error num [%x]",
                  err);
        return CA_STATUS_FAILED;
    }

    //Start listening and accepting
    if (BT_ERROR_NONE != (err = bt_socket_listen_and_accept_rfcomm(socketFD,
                                gMaxPendingConnections)))
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Failed in listen rfcomm socket!, error num [%x]",
                  err);

        bt_socket_destroy_rfcomm(socketFD);
        return CA_STATUS_FAILED;
    }

    *serverFD = socketFD;

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CABTServerStop(const int32_t serverFD)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    int err = BT_ERROR_NONE;
    if (BT_ERROR_NONE != (err = bt_socket_destroy_rfcomm(serverFD)))
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Failed close server socket!, error num [%x]",
                  err);
        return CA_STATUS_FAILED;
    }

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

