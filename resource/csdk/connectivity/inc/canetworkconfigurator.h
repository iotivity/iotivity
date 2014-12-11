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
 * @file canetworkconfigurator.h
 * @brief This file contains  utility function for network configurations.
 */

#ifndef _NETWORK_CONFIGURATOR_H_
#define _NETWORK_CONFIGURATOR_H_

#include "cacommon.h"
#include "uarraylist.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief   Add network type to the selected networks for network packets reception
 * @param   CAConnectivityType       [IN]    connectivity type that needs to be added
 * @return  CA_STATUS_OK or ERROR CODES ( CAResult_t error codes in cacommon.h)
 */
CAResult_t CAAddNetworkType(uint32_t CAConnectivityType);

/**
 * @brief   Add network type to the un selected for network packets reception
 * @param   CAConnectivityType       [IN]    connectivity type that needs to be removed
 * @return  CA_STATUS_OK or ERROR CODES ( CAResult_t error codes in cacommon.h)
 */
CAResult_t CARemoveNetworkType(uint32_t CAConnectivityType);

/**
 * @brief   Get selected network information
 * @return array list having the connectivity types
 */
u_arraylist_t *CAGetSelectedNetworkList();

/**
 * @brief  Getnetwork informations of the selected networks
 * @param   info    [OUT]   LocalConnectivity objects
 * @param   size    [OUT]   No Of Array objects
 * @return  CA_STATUS_OK or ERROR CODES ( CAResult_t error codes in cacommon.h)
 */
CAResult_t CAGetNetworkInformationInternal(CALocalConnectivity_t **info, uint32_t *size);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif //#ifndef _NETWORK_CONFIGURATOR_H_
