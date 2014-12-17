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
 * @file caethernetinterface_singlethread.h
 * @brief This file provides APIs ethernet client/server/network monitor modules
 */

#ifndef _CA_ETHERNET_INTERFACE_SINGLETHREAD_H_
#define _CA_ETHERNET_INTERFACE_SINGLETHREAD_H_

#include <stdbool.h>

#include "cacommon.h"

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
    CA_UNICAST_SERVER = 0,
    CA_MULTICAST_SERVER,
    CA_SECURED_UNICAST_SERVER
} CAAdapterServerType_t;

/**
 * @fn  CAEthernetPacketReceivedCallback
 * @brief  Callback to be notified on receival of any data from remote OIC devices.
 *
 * @param[in]  ipAddress  IP address of remote OIC device.
 * @param[in]  port  Port number on which data is received.
 * @param[in]  data  Data received from remote OIC device.
 * @param[in]  dataLength  Length of data in bytes.
 * @param[in]  secure  Indicates the data is secure or not.
 *
 * @pre  Callback must be registered using CAEthernetSetPacketReceiveCallback()
 */
typedef void (*CAEthernetPacketReceivedCallback)(const char *ipAddress, const uint32_t port,
        const void *data, const uint32_t dataLength);

/**
 * @fn  CAEthernetExceptionCallback
 * @brief  Callback to be notified when exception occures on multicast/unicast server.
 *
 * @param[in]  type  Type of server either #CA_UNICAST_SERVER or $CA_MULTICAST_SERVER
 *
 * @pre  Callback must be registered using CAEthernetSetExceptionCallback()
 */
typedef void (*CAEthernetExceptionCallback)(CAAdapterServerType_t type);

/**
 * @fn  CAEthernetInitializeServer
 * @brief  API to initialize Ethernet server
 *
 * @return  #CA_STATUS_OK on success otherwise proper error code.
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Initialization failed
 */
CAResult_t CAEthernetInitializeServer(void);

/**
 * @fn  CAEthernetTerminateServer
 * @brief  API to terminate Ethernet server
 */
void CAEthernetTerminateServer(void);

/**
 * @fn  CAEthernetStartMulticastServer
 * @brief  API to start multicast server for specified multicast address and port
 *
 * @param[in]  localAddress  Local adapter address to which server to be binded.
 * @param[in]  multicastAddress  Multicast group address.
 * @param[in]  multicastPort  Port number on which server to be running.
 * @param[out]  serverFD  Multicast server socket FD.
 *
 * @return  #CA_STATUS_OK on success otherwise proper error code.
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_INVALID_PARAM Invalid input data
 * @retval  #CA_SERVER_STARTED_ALREADY Multicast server is already started and running.
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAEthernetStartMulticastServer(const char *localAddress, const char *multicastAddress,
        const int16_t multicastPort, int32_t *serverFD);

/**
 * @fn  CAEthernetStartUnicastServer
 * @brief  API to start unicast server for specified local address and port
 *
 * @param[in]  localAddress  Local adapter address to which server to be binded.
 * @param[in][out]  port  Port number on which server to be running.
 * Port number on which server actually started will be returned.
 * @param[in]  forceStart  Indicate whether to start server forcesfully on specified port or not.
 * @param[in]  secured  true if the secure server to be started, otherwise false.
 * @param[out]  serverFD  Unicast server socket FD.
 *
 * @return  #CA_STATUS_OK on success otherwise proper error code.
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_INVALID_PARAM Invalid input data
 * @retval  #CA_SERVER_STARTED_ALREADY Unicast server is already started and running.
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAEthernetStartUnicastServer(const char *localAddress, int16_t *port,
                                        const bool forceStart, int32_t *serverFD);

/**
 * @fn  CAEthernetStopMulticastServer
 * @brief  API to stop multicast server.
 *
 * @return  #CA_STATUS_OK on success otherwise proper error code.
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAEthernetStopMulticastServer(void);

/**
 * @fn  CAEthernetStopUnicastServer
 * @brief  API to stop unicast server.
 *
 * @return  #CA_STATUS_OK on success otherwise proper error code.
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAEthernetStopUnicastServer();

/**
 * @fn  CAEthernetStopSecureUnicastServer
 * @brief  API to stop secured unicast server.
 *
 * @return  #CA_STATUS_OK on success otherwise proper error code.
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAEthernetStopSecureUnicastServer();

/**
 * @fn  CAEthernetGetUnicastServerInfo
 * @brief  API to get running unicast server information.
 * @remarks  @ipAddress must be freed using free().
 *
 * @param[in]  secure  true if the secure server information needed, otherwise false.
 * @param[in]  ipAddress  IP address on which server is binded and running.
 * @param[out]  port  Port number on which server is running
 * @param[out]  serverFD  Server socket fd.
 *
 * @return  #CA_STATUS_OK on success otherwise proper error code.
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_INVALID_PARAM Invalid input data
 * @retval  #CA_STATUS_FAILED Operation failed
 */

CAResult_t CAEthernetGetUnicastServerInfo(char **ipAddress, int16_t *port, int32_t *serverFD);

/**
 * @fn  CAEthernetSetPacketReceiveCallback
 * @brief  API to set callback for receiving data packets from peer devices.
 *
 * @param[in]  callback Callback to be notified on receival of unicast/multicast data packets.
 *
 * @return  #CA_STATUS_OK on success otherwise proper error code.
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
void CAEthernetSetPacketReceiveCallback(CAEthernetPacketReceivedCallback callback);

/**
 * @fn  CAEthernetReadData
 * @brief  API to pull data
 */
void CAEthernetPullData();

/**
 * @fn  CAEthernetSetExceptionCallback
 * @brief  API to set callback for receiving exception notifications.
 *
 * @param[in]  callback  Callback to be notified on occurance of exception running servers.
 *
 * @return  #CA_STATUS_OK on success otherwise proper error code.
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
void CAEthernetSetExceptionCallback(CAEthernetExceptionCallback callback);

/**
 * @fn  CAEthernetSetUnicastSocket
 * @brief  API to set socket description for sending unicast UDP data
 *
 * @param[in]  socketFD  Socket descriptor used for sending UDP data.
 *
 */
void CAEthernetSetUnicastSocket(const int32_t socketFD);

/**
 * @fn  CAEthernetSetUnicastPort
 * @brief  API to set port description for sending unicast UDP data
 *
 * @param[in]  port  Port descriptor used for sending UDP data.
 *
 */
void CAEthernetSetUnicastPort(const int32_t port);

/**
 * @fn  CAEthernetSetSecureUnicastSocket
 * @brief  API to set socket description for sending secured (encrypted) unicast UDP data
 *
 * @param[in]  socketFD  Socket descriptor used for sending secured (encrypted) UDP data.
 *
 */
void CAEthernetSetSecureUnicastSocket(const int32_t socketFD);

/**
 * @fn  CAEthernetSendUnicastData
 * @brief  API to send unicast UDP data
 *
 * @param[in]  remoteAddress  IP address to which data needs to be send.
 * @param[in]  port  Port to which data needs to be send.
 * @param[in]  data  Data to be send.
 * @param[in]  dataLength  Length of data in bytes
 * @param[in]  isMulticast  whether data needs to be sent to multicast ip
 * @param[in]  isSecure  Indicate the whether data needs to be send on secure channel.
 * @isSecure will be ignored when @isMulticast is true.
 *
 * @return  #CA_STATUS_OK on success otherwise proper error code.
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_INVALID_PARAM Invalid input data
 * @retval  #CA_STATUS_FAILED Operation failed
 */

uint32_t CAEthernetSendData(const char *remoteAddress, const int16_t port,
                            const char *buf, const uint32_t bufLen, bool isMulticast);

/**
 * @fn  CAEthernetConnectionStateChangeCallback
 * @brief  Callback to be notified when ethernet adapter connection state changes.
 *
 * @param[in]  ipAddress  IP address of remote OIC device.
 * @param[in]  status  Connection status either #CA_INTERFACE_UP or #CA_INTERFACE_DOWN.
 *
 * @pre  Callback must be registered using CAEthernetSetConnectionStateChangeCallback()
 */
typedef void (*CAEthernetConnectionStateChangeCallback)(const char *ipAddress,
        const CANetworkStatus_t status);

/**
* @fn  CAEthernetInitializeServer
* @brief  API to initialize Ethernet server
*
* @return  #CA_STATUS_OK on success otherwise proper error code.
* @retval  #CA_STATUS_OK  Successful
* @retval  #CA_STATUS_FAILED Initialization failed
*/
CAResult_t CAEthernetInitializeNetworkMonitor(void);

/**
 * @fn  CAEthernetTerminateNetworkMonitor
 * @brief  API to terminate Ethernet network monitor
 */
void CAEthernetTerminateNetworkMonitor(void);

/**
 * @fn  CAEthernetStartNetworkMonitor
 * @brief  API to start network monitoring process.
 *
 * @return  #CA_STATUS_OK on success otherwise proper error code.
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAEthernetStartNetworkMonitor(void);

/**
 * @fn  CAEthernetStopNetworkMonitor
 * @brief  API to stop network monitoring process.
 *
 * @return  #CA_STATUS_OK on success otherwise proper error code.
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAEthernetStopNetworkMonitor(void);

/**
 * @fn  CAEthernetGetInterfaceInfo
 * @brief  API to get local adapter network information.
 * @remarks  @interfaceName and @ipAddress must be freed using free().
 *
 * @param[out]  interfaceName  Local adapter interface name
 * @param[out]  ipAddress  IP address
 *
 * @return  #CA_STATUS_OK on success otherwise proper error code.
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_INVALID_PARAM Invalid input data
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAEthernetGetInterfaceInfo(char **interfaceName, char **ipAddress);

/**
 * @fn  CAEthernetIsConnected
 * @brief  API to get ethernet adapter connection state.
 *
 * @return  true if ethernet adapter is connected, otherwise false
 */
bool CAEthernetIsConnected(void);

/**
 * @fn  CAEthernetSetConnectionStateChangeCallback
 * @brief  API to set callback for receiving local ethernet adapter connection status.
 *
 * @param[in]  callback  Callback to be notified when local ethernet adapter connection state changes.
 *
 */
void CAEthernetSetConnectionStateChangeCallback(CAEthernetConnectionStateChangeCallback callback);

#ifdef __cplusplus
}
#endif

#endif //_CA_ETHERNET_INTERFACE_SINGLETHREAD_H_
