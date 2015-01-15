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

#include "caleinterface.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>

#include <bluetooth.h>
#include <bluetooth_type.h>
#include <bluetooth_product.h>


#include "umutex.h"
#include "caleadapter.h"
#include "caadapterutils.h"

/**
 * @def TZ_LE_NWK_MONITOR_TAG
 * @brief Logging tag for module name
 */
#define TZ_LE_NWK_MONITOR_TAG "TZ_BLE_ADAPTER_CONTROLLER"

/**
 * @var gCALEDeviceStateChangedCallback
 * @brief Maintains the callback to be notified on device state changed.
 */
static CALEDeviceStateChangedCallback gCALEDeviceStateChangedCallback = NULL;

/**
 * @var gCALEDeviceStateChangedCbMutex
 * @brief Mutex to synchronize access to the deviceStateChanged Callback when the state
 *           of the LE adapter gets change.
 */
static u_mutex gCALEDeviceStateChangedCbMutex = NULL;

/**
* @fn  CALEAdapterStateChangedCb
* @brief  This is the callback which will be called when the adapter state gets changed.
*
* @param[in]  result result of the query done to the platform.
* @param[in]  adapter_state State of the LE adapter.
* @param[in]  user_data Any user_data passed by the caller when querying for the state changed cb.
*
* @return  None.
*
*/
void CALEAdapterStateChangedCb(int32_t result, bt_adapter_state_e adapter_state, void *user_data);

CAResult_t CAInitializeLENwkMonitor()
{
    OIC_LOG(DEBUG, TZ_LE_NWK_MONITOR_TAG, "IN");

    CAResult_t res =    CAInitLENwkMonitorMutexVaraibles();
    if (CA_STATUS_OK != res)
    {
        OIC_LOG_V(ERROR, TZ_LE_NWK_MONITOR_TAG, "CAInitLENwkMonitorMutexVaraibles() failed");
        return CA_STATUS_FAILED;
    }
    OIC_LOG(DEBUG, TZ_LE_NWK_MONITOR_TAG, "OUT");

    return CA_STATUS_OK;

}

void CATerminateLENwkMonitorMutexVariables()
{
    OIC_LOG(DEBUG,  TZ_LE_NWK_MONITOR_TAG, "IN");

    u_mutex_free(gCALEDeviceStateChangedCbMutex);
    gCALEDeviceStateChangedCbMutex = NULL;

    OIC_LOG(DEBUG,  TZ_LE_NWK_MONITOR_TAG, "OUT");
    return;
}

void CATerminateLENwkMonitor()
{
    OIC_LOG(DEBUG, TZ_LE_NWK_MONITOR_TAG, "IN");

    CATerminateLENwkMonitorMutexVariables();

    OIC_LOG(DEBUG, TZ_LE_NWK_MONITOR_TAG, "OUT");

    return ;

}

CAResult_t CAInitializeLEAdapter()
{
    OIC_LOG(DEBUG, TZ_LE_NWK_MONITOR_TAG, "IN");

    CAResult_t res = bt_initialize();
    if (CA_STATUS_OK != res)
    {
        OIC_LOG_V(ERROR, TZ_LE_NWK_MONITOR_TAG, "bt_initialize failed");
        return CA_STATUS_FAILED;
    }

    int32_t ret = bt_adapter_set_visibility(BT_ADAPTER_VISIBILITY_MODE_GENERAL_DISCOVERABLE, 0);
    if (0 != ret)
    {
        OIC_LOG_V(ERROR, TZ_LE_NWK_MONITOR_TAG, "bt_adapter_set_visibility failed");
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TZ_LE_NWK_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAGetLEAdapterState()
{
    OIC_LOG(DEBUG, TZ_LE_NWK_MONITOR_TAG, "IN");
    bt_error_e err = BT_ERROR_NONE;
    bt_adapter_state_e adapterState;

    //Get Bluetooth adapter state
    if (BT_ERROR_NONE != (err = bt_adapter_get_state(&adapterState)))
    {
        OIC_LOG_V(ERROR, TZ_LE_NWK_MONITOR_TAG, "Bluetooth get state failed!, error num [%x]",
                  err);
        return CA_STATUS_FAILED;
    }

    if (BT_ADAPTER_ENABLED != adapterState)
    {
        OIC_LOG(DEBUG, TZ_LE_NWK_MONITOR_TAG, "BT Adapter is not enabled");
        return CA_ADAPTER_NOT_ENABLED;
    }

    OIC_LOG(DEBUG, TZ_LE_NWK_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

void CAGetLEAddress(char **local_address)
{
    OIC_LOG(DEBUG, TZ_LE_NWK_MONITOR_TAG, "IN");

    char *address = NULL;

    bt_adapter_get_address(&address);

    OIC_LOG_V(DEBUG, TZ_LE_NWK_MONITOR_TAG, "bd address[%s]", address);

    *local_address = address;

    OIC_LOG(DEBUG, TZ_LE_NWK_MONITOR_TAG, "OUT");
    return ;
}

CAResult_t CASetLEAdapterStateChangedCb(CALEDeviceStateChangedCallback callback)
{
    OIC_LOG(DEBUG, TZ_LE_NWK_MONITOR_TAG, "IN");

    OIC_LOG(DEBUG, TZ_LE_NWK_MONITOR_TAG, "Setting CALEDeviceStateChangedCallback");

    u_mutex_lock(gCALEDeviceStateChangedCbMutex);
    gCALEDeviceStateChangedCallback = callback;
    u_mutex_unlock(gCALEDeviceStateChangedCbMutex);

    int32_t ret = bt_adapter_set_state_changed_cb(CALEAdapterStateChangedCb, NULL);
    if (0 != ret)
    {
        OIC_LOG(DEBUG, TZ_LE_NWK_MONITOR_TAG, "bt_adapter_set_state_changed_cb failed");
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TZ_LE_NWK_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

void CALEAdapterStateChangedCb(int32_t result, bt_adapter_state_e adapter_state, void *user_data)
{
    OIC_LOG(DEBUG, TZ_LE_NWK_MONITOR_TAG, "IN");

    u_mutex_lock(gCALEDeviceStateChangedCbMutex);

    if ( BT_ADAPTER_DISABLED == adapter_state)
    {
        OIC_LOG(DEBUG, TZ_LE_NWK_MONITOR_TAG, "Adapter is disabled");
        if (NULL == gCALEDeviceStateChangedCallback)
        {
            OIC_LOG(ERROR, TZ_LE_NWK_MONITOR_TAG, "gReqRespCallback is NULL!");
            u_mutex_unlock(gCALEDeviceStateChangedCbMutex);
            return;
        }

        gCALEDeviceStateChangedCallback(CA_ADAPTER_DISABLED);
        u_mutex_unlock(gCALEDeviceStateChangedCbMutex);
        return;
    }

    OIC_LOG(DEBUG, TZ_LE_NWK_MONITOR_TAG, "Adapter is Enabled");
    gCALEDeviceStateChangedCallback(CA_ADAPTER_ENABLED);
    u_mutex_unlock(gCALEDeviceStateChangedCbMutex);

    OIC_LOG(DEBUG, TZ_LE_NWK_MONITOR_TAG, "OUT");
    return;
}


CAResult_t CAUnSetLEAdapterStateChangedCb()
{
    OIC_LOG(DEBUG, TZ_LE_NWK_MONITOR_TAG, "IN");

    int32_t ret = bt_adapter_unset_state_changed_cb();
    if (ret != 0)
    {
        OIC_LOG(DEBUG, TZ_LE_NWK_MONITOR_TAG, "bt_adapter_set_state_changed_cb failed");
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TZ_LE_NWK_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;

}

CAResult_t CAInitLENwkMonitorMutexVaraibles()
{
    OIC_LOG(DEBUG,  TZ_LE_NWK_MONITOR_TAG, "IN");
    u_mutex_init();
    if (NULL == gCALEDeviceStateChangedCbMutex)
    {
        gCALEDeviceStateChangedCbMutex = u_mutex_new();
        if (NULL == gCALEDeviceStateChangedCbMutex)
        {
            OIC_LOG(ERROR, TZ_LE_NWK_MONITOR_TAG, "u_mutex_new failed");
            return CA_STATUS_FAILED;
        }
    }

    OIC_LOG(DEBUG, TZ_LE_NWK_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;

}



