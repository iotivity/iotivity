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
 * @file  cabtnwmonitor.c
 * @brief  This file provides the APIs for EDR Network Monitor
 */

#include <string.h>
#include <bluetooth.h>
#include "caedrinterface.h"
#include "caedrdevicelist.h"
#include "caedrutils.h"
#include "caadapterutils.h"
#include "caqueueingthread.h"

/**
 * @var gEDRNetworkChangeCallback
 * @brief Maintains the callback to be notified on local bluetooth adapter status change.
 */
static CAEDRNetworkStatusCallback gEDRNetworkChangeCallback = NULL;

/**
 * @fn CAEDRAdapterStateChangeCallback
 * @brief This callback is registered to receive bluetooth adapter state changes.
 */
static void CAEDRAdapterStateChangeCallback(int result, bt_adapter_state_e adapterState,
                                            void *userData);

CAResult_t CAEDRInitializeNetworkMonitor()
{
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN");

    // Initialize Bluetooth service
    int32_t err = bt_initialize();

    if (BT_ERROR_NONE != err)
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Bluetooth initialization failed!, error num [%x]",
                  err);
        return CA_STATUS_FAILED;
    }

    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CAEDRTerminateNetworkMonitor(void)
{
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN");

    gEDRNetworkChangeCallback = NULL;

    // Terminate Bluetooth service
    bt_deinitialize();

    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "OUT");
}

CAResult_t CAEDRStartNetworkMonitor()
{
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN");

    int32_t ret = bt_adapter_set_state_changed_cb(CAEDRAdapterStateChangeCallback, NULL);
    if(BT_ERROR_NONE != ret)
    {
       OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "bt_adapter_set_state_changed_cb failed");
       return CA_STATUS_FAILED;
    }

    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAEDRStopNetworkMonitor()
{
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN");
    // Unset bluetooth adapter callbacks
    int32_t ret = bt_adapter_unset_state_changed_cb();
    if(BT_ERROR_NONE != ret)
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "bt_adapter_set_state_changed_cb failed");
        return CA_STATUS_FAILED;
    }
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CAEDRSetNetworkChangeCallback(
    CAEDRNetworkStatusCallback networkChangeCallback)
{
    gEDRNetworkChangeCallback = networkChangeCallback;
}

CAResult_t CAEDRGetInterfaceInformation(CALocalConnectivity_t **info)
{
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN");

    int32_t err = BT_ERROR_NONE;
    char *localAddress = NULL;

    // Input validation
    VERIFY_NON_NULL(info, EDR_ADAPTER_TAG, "LocalConnectivity info is null");

    // Get the bluetooth adapter local address
    if (BT_ERROR_NONE != (err = bt_adapter_get_address(&localAddress)))
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG,
                  "Getting local adapter address failed!, error num [%x]",
                  err);
        return CA_STATUS_FAILED;
    }

    // Create network info
    *info = CAAdapterCreateLocalEndpoint(CA_EDR, localAddress);
    if (NULL == *info)
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Failed to create LocalConnectivity instance!");

        OICFree(localAddress);
        return CA_MEMORY_ALLOC_FAILED;
    }

    OICFree(localAddress);

    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAEDRGetAdapterEnableState(CABool_t *state)
{
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN");

    // Input validation
    VERIFY_NON_NULL(state, EDR_ADAPTER_TAG, "state holder is NULL!");

    bt_error_e err = BT_ERROR_NONE;
    bt_adapter_state_e adapterState;

    // Get Bluetooth adapter state
    if (BT_ERROR_NONE != (err = bt_adapter_get_state(&adapterState)))
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Bluetooth get state failed!, error num [%x]",
                  err);

        return CA_STATUS_FAILED;
    }

    *state = CA_FALSE;
    if (BT_ADAPTER_ENABLED == adapterState)
    {
        *state = CA_TRUE;
    }

    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CAEDRAdapterStateChangeCallback(int result, bt_adapter_state_e adapterState,
                                     void *userData)
{
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN");

    if (BT_ADAPTER_ENABLED == adapterState)
    {
        // Notity to upper layer
        if (gEDRNetworkChangeCallback)
        {
            gEDRNetworkChangeCallback(CA_INTERFACE_UP);
        }
    }
    else if (BT_ADAPTER_DISABLED == adapterState)
    {
        // Notity to upper layer
        if (gEDRNetworkChangeCallback)
        {
            gEDRNetworkChangeCallback(CA_INTERFACE_DOWN);
        }
    }

    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "OUT");
}