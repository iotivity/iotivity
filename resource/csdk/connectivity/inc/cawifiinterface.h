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
 * @file cawifiinterface.h
 * @brief This file provides APIs wifi client/server/network monitor modules
 */

#ifndef _CA_WIFI_INTERFACE_H_
#define _CA_WIFI_INTERFACE_H_

#include <stdbool.h>

#include "cacommon.h"
#include "uthreadpool.h"

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
 * @fn  CAWiFiPacketReceivedCallback
 * @brief  Callback to be notified on receival of any data from remote OIC devices.
 *
 * @param[in]  ipAddress  IP address of remote OIC device.
 * @param[in]  port  Port number on which data is received.
 * @param[in]  data  Data received from remote OIC device.
 * @param[in]  dataLength  Length of data in bytes.
 * @param[in]  secure  Indicates the data is secure or not.
 *
 * @pre  Callback must be registered using CAWiFiSetPacketReceiveCallback()
 */
typedef void (*CAWiFiPacketReceivedCallback)(const char *ipAddress, const uint32_t port,
        const void *data, const uint32_t dataLength, const CABool_t isSecure);

/**
 * @fn  CAWiFiExceptionCallback
 * @brief  Callback to be notified when exception occures on multicast/unicast server.
 *
 * @param[in]  type  Type of server either #CA_UNICAST_SERVER or $CA_MULTICAST_SERVER
 *
 * @pre  Callback must be registered using CAWiFiSetExceptionCallback()
 */
typedef void (*CAWiFiExceptionCallback)(CAAdapterServerType_t type);

/**
 * @fn  CAWiFiInitializeServer
 * @brief  API to initialize Wifi server
 *
 * @param[in]  threadPool  Thread pool for managing Unicast/Multicast server threads.
 *
 * @return  #CA_STATUS_OK on success otherwise proper error code.
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_INVALID_PARAM Invalid input data
 * @retval  #CA_STATUS_FAILED Initialization failed
 */
CAResult_t CAWiFiInitializeServer(const u_thread_pool_t threadPool);


/**
 * @fn  CAWiFiTerminateServer
 * @brief  API to terminate Wifi server
 */
void CAWiFiTerminateServer(void);

/**
 * @fn  CAWiFiStartMulticastServer
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
CAResult_t CAWiFiStartMulticastServer(const char *localAddress, const char *multicastAddress,
                                      const int16_t multicastPort, int32_t *serverFD);

/**
 * @fn  CAWiFiStartUnicastServer
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
CAResult_t CAWiFiStartUnicastServer(const char *localAddress, int16_t *port,
                                    const bool forceStart, const CABool_t isSecured, int32_t *serverFD);

/**
 * @fn  CAWiFiStopMulticastServer
 * @brief  API to stop multicast server.
 *
 * @return  #CA_STATUS_OK on success otherwise proper error code.
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAWiFiStopMulticastServer(void);

/**
 * @fn  CAWiFiStopUnicastServer
 * @brief  API to stop unicast server.
 *
 * @return  #CA_STATUS_OK on success otherwise proper error code.
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAWiFiStopUnicastServer();

#ifdef __WITH_DTLS__
/**
 * @fn  CAWiFiStopSecureUnicastServer
 * @brief  API to stop secured unicast server.
 *
 * @return  #CA_STATUS_OK on success otherwise proper error code.
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAWiFiStopSecureUnicastServer();
#endif

/**
 * @fn  CAWiFiGetUnicastServerInfo
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
CAResult_t CAWiFiGetUnicastServerInfo(const CABool_t isSecured, char **ipAddress, int16_t *port,
                                      int32_t *serverFD);


/**
 * @fn  CAWiFiSetPacketReceiveCallback
 * @brief  API to set callback for receiving data packets from peer devices.
 *
 * @param[in]  callback Callback to be notified on receival of unicast/multicast data packets.
 *
 * @return  #CA_STATUS_OK on success otherwise proper error code.
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
void CAWiFiSetPacketReceiveCallback(CAWiFiPacketReceivedCallback callback);

/**
 * @fn  CAWiFiSetExceptionCallback
 * @brief  API to set callback for receiving exception notifications.
 *
 * @param[in]  callback  Callback to be notified on occurance of exception running servers.
 *
 * @return  #CA_STATUS_OK on success otherwise proper error code.
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
void CAWiFiSetExceptionCallback(CAWiFiExceptionCallback callback);

/**
 * @fn  CAWiFiSetUnicastSocket
 * @brief  API to set socket description for sending unicast UDP data
 *
 * @param[in]  socketFD  Socket descriptor used for sending UDP data.
 *
 */
void CAWiFiSetUnicastSocket(const int32_t socketFD);

#ifdef __WITH_DTLS__
/**
 * @fn  CAWiFiSetSecureUnicastSocket
 * @brief  API to set socket description for sending secured (encrypted) unicast UDP data
 *
 * @param[in]  socketFD  Socket descriptor used for sending secured (encrypted) UDP data.
 *
 */
void CAWiFiSetSecureUnicastSocket(const int32_t socketFD);
#endif

/**
 * @fn  CAWiFiSendUnicastData
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
uint32_t CAWiFiSendData(const char *remoteAddress, const uint32_t port,
                        const void *data, const uint32_t dataLength,
                        CABool_t isMulticast, CABool_t isSecured);

/**
 * @fn  CAWiFiConnectionStateChangeCallback
 * @brief  Callback to be notified when wifi adapter connection state changes.
 *
 * @param[in]  ipAddress  IP address of remote OIC device.
 * @param[in]  status  Connection status either #CA_INTERFACE_UP or #CA_INTERFACE_DOWN.
 *
 * @pre  Callback must be registered using CAWiFiSetConnectionStateChangeCallback()
 */
typedef void (*CAWiFiConnectionStateChangeCallback)(const char *ipAddress,
        const CANetworkStatus_t status);

/**
 * @fn  CAWiFiInitializeNetworkMonitor
 * @brief  API to initialize Wifi network monitor
 *
 * @param[in]  threadPool  Thread pool for managing network monitor thread.
 *
 * @return  #CA_STATUS_OK on success otherwise proper error code.
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_INVALID_PARAM Invalid input data
 * @retval  #CA_STATUS_FAILED Initialization failed
 */
CAResult_t CAWiFiInitializeNetworkMonitor(const u_thread_pool_t threadPool);

/**
 * @fn  CAWiFiTerminateNetworkMonitor
 * @brief  API to terminate Wifi network monitor
 */
void CAWiFiTerminateNetworkMonitor(void);

/**
 * @fn  CAWiFiStartNetworkMonitor
 * @brief  API to start network monitoring process.
 *
 * @return  #CA_STATUS_OK on success otherwise proper error code.
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAWiFiStartNetworkMonitor(void);

/**
 * @fn  CAWiFiStopNetworkMonitor
 * @brief  API to stop network monitoring process.
 *
 * @return  #CA_STATUS_OK on success otherwise proper error code.
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAWiFiStopNetworkMonitor(void);

/**
 * @fn  CAWiFiGetInterfaceInfo
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
CAResult_t CAWiFiGetInterfaceInfo(char **interfaceName, char **ipAddress);

/**
 * @fn  CAWiFiGetInterfaceSubnetMask
 * @brief  API to get local adapter network subnet mask.
 * @remarks  @subnetMaskmust be freed using free().
 *
 * @param[out]  subnetMask  Local adapter interface subnet mask
 *
 * @return  #CA_STATUS_OK on success otherwise proper error code.
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_INVALID_PARAM Invalid input data
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAWiFiGetInterfaceSubnetMask(char **subnetMask);

/**
 * @fn  CAWiFiIsConnected
 * @brief  API to get wifi adapter connection state.
 *
 * @return  true if wifi adapter is connected, otherwise false
 */
bool CAWiFiIsConnected(void);

/**
 * @fn  CAWiFiSetConnectionStateChangeCallback
 * @brief  API to set callback for receiving local wifi adapter connection status.
 *
 * @param[in]  callback  Callback to be notified when local wifi adapter connection state changes.
 *
 */
void CAWiFiSetConnectionStateChangeCallback(CAWiFiConnectionStateChangeCallback callback);

#ifdef __cplusplus
}
#endif

#endif //_CA_WIFI_INTERFACE_H_
