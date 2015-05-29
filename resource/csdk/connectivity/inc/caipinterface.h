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
    CA_UNICAST_SERVER = 0,    /**< Unicast Server */
    CA_MULTICAST_SERVER,      /**< Multicast Server */
    CA_SECURED_UNICAST_SERVER /**< Secured Unicast Server */
} CAAdapterServerType_t;

/**
 * @brief Callback to be notified on reception of any data from remote OIC devices.
 *
 * @param  ipAddress    [IN] IP address of remote OIC device.
 * @param  port         [IN] Port number on which data is received.
 * @param  data         [IN] Data received from remote OIC device.
 * @param  dataLength   [IN] Length of data in bytes.
 * @param  isSecured    [IN] Indicates the data is secure or not.
 *
 * @return NONE
 * @pre  Callback must be registered using CAIPSetPacketReceiveCallback()
 */
typedef void (*CAIPPacketReceivedCallback)(const char *ipAddress, uint16_t port,
                                           const void *data, uint32_t dataLength,
                                           bool isSecured);

/**
 * @brief  Callback to be notified when exception occures on multicast/unicast server.
 * @param  type  [IN] Type of server(#CAAdapterServerType_t)
 * @return NONE
 * @pre  Callback must be registered using CAIPSetExceptionCallback()
 */
typedef void (*CAIPExceptionCallback)(CAAdapterServerType_t type);

/**
 * @brief  Initialize IP server
 *
 * @param   threadPool  [IN] Thread pool for managing Unicast/Multicast server threads.
 *
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_INVALID_PARAM Invalid input data
 * @retval  #CA_STATUS_FAILED Initialization failed
 */
CAResult_t CAIPInitializeServer(const ca_thread_pool_t threadPool);

/**
 * @brief  Terminate IP server
 * @return NONE
 */
void CAIPTerminateServer();

/**
 * @brief  Start multicast server for specified multicast address and port
 *
 * @param   localAddress        [IN]      Local adapter address to which server to be binded.
 * @param   multicastAddress    [IN]      Multicast group address.
 * @param   multicastPort       [IN,OUT]  Port number on which server will be running. If binding
 *                                        the port failed, server starts in the next available port.
 *
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_INVALID_PARAM Invalid input data
 * @retval  #CA_SERVER_STARTED_ALREADY Multicast server is already started and running.
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAIPStartMulticastServer(const char *localAddress, const char *multicastAddress,
                                    uint16_t multicastPort);

/**
 * @brief  Start unicast server for specified local address and port
 *
 * @param  localAddress [IN]      Local adapter address to which server to be binded.
 * @param  port         [IN,OUT]  Port number on which server will be running. If binding
 *                                the port failed, server starts in the next available port.
 * @param  forceStart   [IN]      Indicate whether to start server forcesfully on specified port
 *                                or not.
 * @param  secured      [IN]      True if the secure server to be started, otherwise false.
 *
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_INVALID_PARAM Invalid input data
 * @retval  #CA_SERVER_STARTED_ALREADY Unicast server is already started and running.
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAIPStartUnicastServer(const char *localAddress, uint16_t *port, bool forceStart,
                                  bool secured);

/**
 * @brief  Stop servers that are running in particular interface address.
 *
 * @param   interfaceAddress  [IN] interface address in which servers are running.
 *
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAIPStopServer(const char *interfaceAddress);

/**
 * @brief  Used to stop all unicast and multicast servers.
 *
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAIPStopAllServers();

/**
 * @brief  Used to get the socket fd based on index value of server info list.
 *
 * @param   index      [IN] Index where we need socket fd value.
 * @param   isSecured  [IN] For secured unicast server or normal server.
 *
 * @return  positive value on success and -1 on error.
 */
int CAGetSocketFdFromUnicastIPServerbyIndex(int16_t index, bool isSecured);

/**
 * @brief  Used to get the number of unicast server currently running.
 *
 * @param   isSecured  [IN] To identify whether its secured unicast server or normal server.
 *
 * @return  positive value on success and -1 on error.
 */
int16_t CAGetNumberOfUnicastIPServers(bool isSecured);

/**
 * @brief  Used to get the stored socket fd for corresponding ipAddress.
 *
 * @param   ipAddress    [IN] IpAddress of server.
 * @param   isSecured    [IN] Used to check the server is secured or not.
 * @param   isMulticast  [IN] To identify whether its for multicast or unicast.
 *
 * @return  socket fd on success and -1 on error.
 */
int CAGetSocketFdFromUnicastIPServer(const char *ipAddress, bool isSecured, bool isMulticast);

/**
 * @brief  Used to get the port number to the corresponding ip for giving interface info.
 *
 * @param   ipAddress  [IN] IpAddress of server.
 * @param   isSecured  [IN] Used to check the server is secured or not.
 *
 * @return  port number on success and -1 on error.
 */
uint16_t CAGetServerPortNum(const char *ipAddress, bool isSecured);

/**
 * @brief  Used to get the port number for corresponding ipAddress.
 *
 * @param   serverInfoList  [OUT] ServerInfoList holds unicast and multicast server informations.
 *
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_INVALID_PARAM Invalid input data
 * @retval  #CA_STATUS_FAILED Initialization failed
 */
CAResult_t CAGetIPServerInfoList(u_arraylist_t **serverInfoList);

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
 * @brief  API to send unicast UDP data
 *
 * @param  remoteAddress    [IN] IP address to which data needs to be sent.
 * @param  port             [IN] Port to which data needs to be send.
 * @param  data             [IN] Data to be send.
 * @param  dataLength       [IN] Length of data in bytes
 * @param  isMulticast      [IN] Whether data needs to be sent to multicast ip
 * @param  isSecured        [IN] Whether data to be sent on secured channel.
 *
 * @return  The number of bytes sent on the network. Returns 0 on error.
 * @remarks isSecure will be ignored when isMulticast is true.
 */
uint32_t CAIPSendData(const char *remoteAddress, uint16_t port, const void *data,
                      uint32_t dataLength, bool isMulticast, bool isSecure);

/**
 * @brief  Callback to be notified when IP adapter connection state changes.
 *
 * @param  ipAddress    [IN] IP address of remote OIC device.
 * @param  status       [IN] Connection status either #CA_INTERFACE_UP or #CA_INTERFACE_DOWN.
 * @return  NONE
 * @pre  Callback must be registered using CAIPSetConnectionStateChangeCallback()
 */
typedef void (*CAIPConnectionStateChangeCallback)(const char *ipAddress,
                                                  CANetworkStatus_t status);

/**
 * @brief Initialize IP network monitor
 *
 * @param  threadPool [IN] Thread pool for managing network monitor thread.
 *
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_INVALID_PARAM Invalid input data
 * @retval  #CA_STATUS_FAILED Initialization failed
 */
CAResult_t CAIPInitializeNetworkMonitor(const ca_thread_pool_t threadPool);

/**
 * @brief Terminate IP network monitor by removing interface list.
 * @return  NONE
 */
void CAIPTerminateNetworkMonitor();

/**
 * @brief  Start network monitoring process. It will start the monitor thread.
 *
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAIPStartNetworkMonitor();

/**
 * @brief  Stop network monitoring process. It will stop the monitor thread.
 *
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAIPStopNetworkMonitor();

/**
 * @brief  Get local adapter network information.
 *
 * @param  netInterfaceList [OUT] network interface information list
 *
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_INVALID_PARAM Invalid input data
 * @retval  #CA_STATUS_FAILED Operation failed
 * @remarks  interfaceName and ipAddress must be freed using free().
 */
CAResult_t CAIPGetInterfaceInfo(u_arraylist_t **netInterfaceList);

/**
 * @brief  Get local adapter network subnet mask.
 *
 * @param  ipAddress  [IN]  IpAddress which is used for getting subnet mask.
 * @param  subnetMask [OUT] Local adapter interface subnet mask
 *
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_INVALID_PARAM Invalid input data
 * @retval  #CA_STATUS_FAILED Operation failed
 * @remarks subnetMask must be freed using free().
 */
CAResult_t CAIPGetInterfaceSubnetMask(const char *ipAddress, char **subnetMask);

/**
 * @brief  Get IP adapter connection state.
 *
 * @return  True if IP adapter is connected, otherwise false
 */
 bool CAIPIsConnected();

/**
 * @brief  Set callback for receiving local IP adapter connection status.
 *
 * @param  callback [IN] Callback to be notified when IP adapter connection state changes.
 * @return NONE
 */
void CAIPSetConnectionStateChangeCallback(CAIPConnectionStateChangeCallback callback);

#ifdef __cplusplus
}
#endif

#endif /* CA_IP_INTERFACE_H_ */
