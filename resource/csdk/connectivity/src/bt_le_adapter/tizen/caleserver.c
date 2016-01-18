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

#include <bluetooth.h>
#include <bluetooth_type.h>

#include "caleserver.h"
#include <pthread.h>
#include "cacommon.h"
#include "caadapterutils.h"
#include <gio/gio.h>
#include "camutex.h"
#include "caqueueingthread.h"
#include "caadapterutils.h"
#include "cafragmentation.h"
#include "cagattservice.h"
#include "caleutil.h"
#include "oic_string.h"
#include "oic_malloc.h"

/**
 * @def TZ_BLE_SERVER_TAG
 * @brief Logging tag for module name
 */
#define TZ_BLE_SERVER_TAG "OIC_CA_LE_SERVER"

void CABleGattServerConnectionStateChangedCb(int result, bool connected,
                                             const char *remoteAddress, void *userData)
{
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "IN");

    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "OUT");
}

CAResult_t CAStartLEGattServer()
{
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "IN");
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CAStartBleGattServerThread(void *data)
{
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "IN");

    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "OUT");
}

CAResult_t CAStopLEGattServer()
{
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "IN");

    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CATerminateLEGattServer()
{
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "IN");

    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "OUT");
}

CAResult_t CAInitializeLEGattServer()
{
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "IN");

    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAInitBleGattService()
{
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "IN");

    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CADeInitBleGattService()
{
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "IN");

    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CASetLEServerThreadPoolHandle(ca_thread_pool_t handle)
{
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "IN");
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "OUT");
}

CAResult_t CAAddNewBleServiceInGattServer(const char *serviceUUID)
{
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "IN");
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CARemoveBleServiceFromGattServer(const char *svcPath)
{
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "IN");
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CARemoveAllBleServicesFromGattServer()
{
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "IN");
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CABleGattRemoteCharacteristicWriteCb(char *charPath,
                                          unsigned char *charValue,
                                          int charValueLen,
                                          const char *remoteAddress,
                                          void *userData)
{
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "IN");
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "OUT");
}

CAResult_t CARegisterBleServicewithGattServer(const char *svcPath)
{
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "IN");
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAAddNewCharacteristicsToGattServer(const char *svcPath, const char *charUUID,
        const uint8_t *charValue, int charValueLen, bool read)
{

    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "IN");
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CARemoveCharacteristicsFromGattServer(const char *charPath)
{
    ///TODO: There is no api provided in bluetooth.h for removing characteristics.
    return CA_STATUS_OK;
}

CAResult_t CAUpdateCharacteristicsToGattClient(const char *address,
                                               const uint8_t *charValue,
                                               uint32_t charValueLen)
{
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "IN");

    OIC_LOG(ERROR, TZ_BLE_SERVER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAUpdateCharacteristicsToAllGattClients(const uint8_t *charValue, uint32_t charValueLen)
{
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "IN");

    OIC_LOG(ERROR, TZ_BLE_SERVER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CASetLEReqRespServerCallback(CABLEDataReceivedCallback callback)
{
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "IN");

    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "OUT");
}

void CASetBLEServerErrorHandleCallback(CABLEErrorHandleCallback callback)
{

}
