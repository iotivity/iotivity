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
 * @file cawifiutils.h
 * @brief This file contains the APIs for WiFi Specific Calls
 */

#ifndef _CA_WIFI_MONITOR_
#define _CA_WIFI_MONITOR_

#include <stdint.h>
#include <wifi.h>
#include "caadapterinterface.h"
#include "cacommon.h"

/**
 * @brief  To send Unicast or Multicast Data
 * @param  remoteIpAddress      [OUT] IP Address of the device
 * @param  port                 [IN]  Length of the IP address
 * @return Return  Error Code
 */
CAResult_t CAGetInterfaceAddress(char *address, int32_t addrLen);

/**
 * @brief  To Initialize the WIFI Adapter
 * @return Return  Error Code
 */
CAResult_t CAInitializeWIFIAdapter();

/**
 * @brief  To Deinitialize WIFI Adapter
 * @return Return  Error Code
 */
CAResult_t CADeinitializeWIFIAdapter();

/**
 * @brief  To Check if WIFI is connected
 * @return Return  True or False
 */
CABool_t CAIsWIFIConnected();

/**
 * @brief  WIFI connection state Changed Callback
 * @param  state              [IN] Connection state of the Device
 * @param  ap                 [IN] Access Point handler
 * @param  userData           [IN] User Data Passed on Setting the Callback
 * @return Return  None
 */
void CAWIFIConnectionStateChangedCb(wifi_connection_state_e state, wifi_ap_h ap, void *userData);

/**
 * @brief  WIFI Device state Changed Callback
 * @param  state              [IN] Device state
 * @param  userData           [IN] User Data Passed on Setting the Callback
 * @return Return  None
 */
void CAWIFIDeviceStateChangedCb(wifi_device_state_e state, void *userData);

/**
 * @brief  To set the IsStartServerCalled global variable
 * @return Return  None
 */
void CASetIsStartServerInvoked();

/**
 * @brief  To unset the IsStartServerCalled global variable
 * @return Return  None
 */
void CAUnsetIsStartServerInvoked();

/**
 * @brief API to set Network Changed callback to upper layer.
 * @param netCallback - upper layer callback function to notify the change in WIFI connection state.
 * @return - Error Code
 */
void CASetWIFINetworkChangeCallback(CANetworkChangeCallback netCallback);
#endif