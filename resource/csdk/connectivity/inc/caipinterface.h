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
 * @file caipinterface.h
 * @brief This file provides APIs IP client/server/network monitor modules
 */

#ifndef CA_IP_INTERFACE_H_
#define CA_IP_INTERFACE_H_

#include <stdbool.h>

#include "cacommon.h"
#include "cathreadpool.h"
#include "uarraylist.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @enum CAAdapterServerType_t
 * @brief Enum for defining different server types.
 */
typedef enum
{
    CA_UNICAST_SERVER = 0,      /**< Unicast Server */
    CA_MULTICAST_SERVER,        /**< Multicast Server */
    CA_SECURED_UNICAST_SERVER   /**< Secured Unicast Server */
} CAAdapterServerType_t;

/**
 * @brief Callback to be notified on reception of any data from remote OIC devices.
 *
 * @param  endpoint     [IN] network endpoint description
 * @param  data         [IN] Data received from remote OIC device.
 * @param  dataLength   [IN] Length of data in bytes.
 *
 * @return NONE
 * @pre  Callback must be registered using CAIPSetPacketReceiveCallback()
 */
typedef void (*CAIPPacketReceivedCallback)(const CAEndpoint_t *endpoint,
                                           const void *data,
                                           uint32_t dataLength);

/**
  * @brief Callback to notify error in the IP adapter
  *
  * @param  endpoint     [IN] [IN] network endpoint description
  * @param  data         [IN] Data sent/received
  * @param  dataLength   [IN] Length of data in bytes.
  * @param  result       [IN] result of request from R.I
  * @return NONE
  * @pre  Callback must be registered using CAIPSetPacketReceiveCallback()
 */
typedef void (*CAIPErrorHandleCallback)(const CAEndpoint_t *endpoint, const void *data,
                                        uint32_t dataLength, CAResult_t result);

/**
 * @brief  Callback to be notified when exception occures on multicast/unicast server.
 * @param  type  [IN] Type of server(#CAAdapterServerType_t)
 * @return NONE
 * @pre  Callback must be registered using CAIPSetExceptionCallback()
 */
typedef void (*CAIPExceptionCallback)(CAAdapterServerType_t type);

/**
 * @brief  Start IP server
 *
 * @param   threadPool  [IN] Thread pool for managing Unicast/Multicast server threads.
 *
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_INVALID_PARAM Invalid input data
 * @retval  #CA_STATUS_FAILED Initialization failed
 */
#ifdef SINGLE_THREAD
CAResult_t CAIPStartServer();
#else
CAResult_t CAIPStartServer(const ca_thread_pool_t threadPool);
#endif

/**
 * @brief  Stop IP server
 * @return NONE
 */
void CAIPStopServer();

/**
 * @brief  Set this callback for receiving data packets from peer devices.
 *
 * @param  callback   [IN] Callback to be notified on reception of unicast/multicast data packets.
 *
 * @return  NONE
 */
void CAIPSetPacketReceiveCallback(CAIPPacketReceivedCallback callback);

/**
 * @brief  Set this callback for receiving exception notifications.
 *
 * @param  callback [IN] Callback to be notified on occurance of exception on running servers.
 *
 * @return  NONE
 */
void CAIPSetExceptionCallback(CAIPExceptionCallback callback);

/**
 * @brief  Set socket description for sending unicast UDP data. Once the Unicast server is started,
 *         the same socket descriptor is used for sending the Unicast UDP data.
 *
 * @param  socketFD [IN]  Socket descriptor used for sending UDP data.
 * @return  NONE
 */
void CAIPSetUnicastSocket(int socketFD);

/**
 * @brief  Set the port number for sending unicast UDP data
 * @param  port [IN] Port number used for sending UDP data.
 * @return NONE
 */
void CAIPSetUnicastPort(uint16_t port);

/**
 * @brief  API to send unicast UDP data
 *
 * @param  endpoint         [IN] complete network address to send to
 * @param  data             [IN] Data to be send.
 * @param  dataLength       [IN] Length of data in bytes
 * @param  isMulticast      [IN] Whether data needs to be sent to multicast ip
 */
void CAIPSendData(CAEndpoint_t *endpoint,
                  const void *data,
                  uint32_t dataLength,
                  bool isMulticast);

/**
 * @brief  Get IP adapter connection state.
 *
 * @return  True if IP adapter is connected, otherwise false
 */
bool CAIPIsConnected();

/**
 * @brief  Pull the Received Data
 * @return NONE
 */
void CAIPPullData();

#define CA_COAP        5683
#define CA_SECURE_COAP 5684
#define INTERFACE_NAME_MAX 16

typedef struct
{
    char name[INTERFACE_NAME_MAX];
    uint32_t index;
    uint32_t flags;
    uint16_t family;
    uint32_t ipv4addr;        // used for IPv4 only
} CAInterface_t;

/**
 * @brief  Get a list of CAInterface_t items
 *
 * @return  List of CAInterface_t items
 */
u_arraylist_t *CAIPGetInterfaceInformation(int desiredIndex);

/**
 * @brief  Set callback for error handling
 *
 * @param  ipErrorCallback [IN] callback to notify error to the ipadapter
 * @return NONE
 */
void CAIPSetErrorHandleCallback(CAIPErrorHandleCallback ipErrorCallback);

#ifdef __cplusplus
}
#endif

#endif /* CA_IP_INTERFACE_H_ */
