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
 * @brief This file provides APIs for WIFI adapter - client, server, network monitor modules
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
    CA_UNICAST_SERVER = 0,      /**< Unicast Server */
    CA_MULTICAST_SERVER,        /**< Multicast Server */
    CA_SECURED_UNICAST_SERVER   /**< Secured Unicast Server */
} CAAdapterServerType_t;

/**
 * @brief Callback to be notified on reception of any data from remote OIC devices.
 * @param  ipAddress    [IN] IP address of remote OIC device.
 * @param  port         [IN] Port number on which data is received.
 * @param  data         [IN] Data received from remote OIC device.
 * @param  dataLength   [IN] Length of data in bytes.
 * @param  isSecure     [IN] Indicates the data is secure or not.
 * @return NONE
 * @pre  Callback must be registered using CAWiFiSetPacketReceiveCallback()
 */
typedef void (*CAWiFiPacketReceivedCallback)(const char *ipAddress, const uint32_t port,
        const void *data, const uint32_t dataLength, const CABool_t isSecure);

/**
 * @brief  Callback to be notified when exception occures on multicast/unicast server.
 * @param  type  [IN] Type of server(#CAAdapterServerType_t)
 * @return NONE
 * @pre  Callback must be registered using CAWiFiSetExceptionCallback()
 */
typedef void (*CAWiFiExceptionCallback)(CAAdapterServerType_t type);

/**
 * @brief  Initialize WIFI server
 * @param   threadPool  [IN] Thread pool for managing Unicast/Multicast server threads.
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_INVALID_PARAM Invalid input data
 * @retval  #CA_STATUS_FAILED Initialization failed
 */
CAResult_t CAWiFiInitializeServer(const u_thread_pool_t threadPool);


/**
 * @brief  Terminate WIFI server
 * @return NONE
 */
void CAWiFiTerminateServer(void);

/**
 * @brief  Start multicast server for specified multicast address and port
 *
 * @param   localAddress        [IN]      Local adapter address to which server to be binded.
 * @param   multicastAddress    [IN]      Multicast group address.
 * @param   multicastPort       [IN,OUT]  Port number on which server will be running. If binding
                                          the port failed, server starts in the next available port.
 * @param   serverFD            [OUT]     Multicast server socket FD.
 *
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_INVALID_PARAM Invalid input data
 * @retval  #CA_SERVER_STARTED_ALREADY Multicast server is already started and running.
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAWiFiStartMulticastServer(const char *localAddress, const char *multicastAddress,
                                      const int16_t multicastPort, int32_t *serverFD);

/**
 * @brief  Start unicast server for specified local address and port
 *
 * @param  localAddress [IN]      Local adapter address to which server to be binded.
 * @param  port         [IN,OUT]  Port number on which server will be running. If binding
                                  the port failed, server starts in the next available port.
 * @param  forceStart   [IN]      Indicate whether to start server forcesfully on specified port
 *                                or not.
 * @param  isSecured    [IN]      True if the secure server to be started, otherwise false.
 * @param  serverFD     [OUT]     Unicast server socket FD.
 *
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_INVALID_PARAM Invalid input data
 * @retval  #CA_SERVER_STARTED_ALREADY Unicast server is already started and running.
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAWiFiStartUnicastServer(const char *localAddress, int16_t *port,
                                    const bool forceStart, const CABool_t isSecured,
                                    int32_t *serverFD);

/**
 * @brief  Stop multicast server.
 *
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAWiFiStopMulticastServer(void);

/**
 * @brief  Stop unicast server.
 *
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAWiFiStopUnicastServer();

#ifdef __WITH_DTLS__
/**
 * @brief  Stop secured unicast server.
 *
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAWiFiStopSecureUnicastServer();
#endif

/**
 * @brief  Get the Unicast Server Information if it is started
 *
 * @param  isSecured    [IN] true if the secure server information needed, otherwise false.
 * @param  ipAddress    [OUT] IP address on which server is binded and running.
 * @param  port         [OUT]Port number on which server is running
 * @param  serverFD     [OUT]Server socket fd.
 *
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_INVALID_PARAM Invalid input data
 * @retval  #CA_STATUS_FAILED Operation failed
 * @remarks  ipAddress must be freed using free().
 */
CAResult_t CAWiFiGetUnicastServerInfo(const CABool_t isSecured, char **ipAddress, int16_t *port,
                                      int32_t *serverFD);

/**
 * @brief  Set this callback for receiving data packets from peer devices.
 *
 * @param  callback   [IN] Callback to be notified on reception of unicast/multicast data packets.
 *
 * @return  NONE
 */
void CAWiFiSetPacketReceiveCallback(CAWiFiPacketReceivedCallback callback);

/**
 * @brief  Set this callback for receiving exception notifications.
 *
 * @param  callback [IN] Callback to be notified on occurance of exception on running servers.
 *
 * @return  NONE
 */
void CAWiFiSetExceptionCallback(CAWiFiExceptionCallback callback);

/**
 * @brief  Set socket description for sending unicast UDP data. Once the Unicast server is started,
 *         the same socket descriptor is used for sending the Unicast UDP data.
 *
 * @param  socketFD [IN]  Socket descriptor used for sending UDP data.
 * @return  NONE
 */
void CAWiFiSetUnicastSocket(const int32_t socketFD);

#ifdef __WITH_DTLS__
/**
 * @brief  Set socket description for sending secured (encrypted) unicast UDP data
 *
 * @param socketFD [IN] Socket descriptor used for sending secured (encrypted) UDP data.
 * @return  NONE
 */
void CAWiFiSetSecureUnicastSocket(const int32_t socketFD);
#endif

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
uint32_t CAWiFiSendData(const char *remoteAddress, const uint32_t port,
                        const void *data, const uint32_t dataLength,
                        CABool_t isMulticast, CABool_t isSecured);

/**
 * @brief  Callback to be notified when wifi adapter connection state changes.
 *
 * @param  ipAddress    [IN] IP address of remote OIC device.
 * @param  status       [IN] Connection status either #CA_INTERFACE_UP or #CA_INTERFACE_DOWN.
 * @return  NONE
 * @pre  Callback must be registered using CAWiFiSetConnectionStateChangeCallback()
 */
typedef void (*CAWiFiConnectionStateChangeCallback)(const char *ipAddress,
        const CANetworkStatus_t status);

/**
 * @brief Initialize Wifi network monitor
 *
 * @param  threadPool [IN] Thread pool for managing network monitor thread.
 *
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_INVALID_PARAM Invalid input data
 * @retval  #CA_STATUS_FAILED Initialization failed
 */
CAResult_t CAWiFiInitializeNetworkMonitor(const u_thread_pool_t threadPool);

/**
 * @brief Terminate WIFI network monitor
 * @return  NONE
 */
void CAWiFiTerminateNetworkMonitor(void);

/**
 * @brief  Start network monitoring process.
 *
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAWiFiStartNetworkMonitor(void);

/**
 * @brief  Stop network monitoring process.
 *
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAWiFiStopNetworkMonitor(void);

/**
 * @brief  Get local adapter network information.
 *
 * @param  interfaceName [OUT] Local adapter interface name
 * @param  ipAddress     [OUT] IP address
 *
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_INVALID_PARAM Invalid input data
 * @retval  #CA_STATUS_FAILED Operation failed
 * @remarks  interfaceName and ipAddress must be freed using free().
 */
CAResult_t CAWiFiGetInterfaceInfo(char **interfaceName, char **ipAddress);

/**
 * @brief  Get local adapter network subnet mask.
 *
 * @param  subnetMask [OUT] Local adapter interface subnet mask
 *
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_INVALID_PARAM Invalid input data
 * @retval  #CA_STATUS_FAILED Operation failed
 * @remarks subnetMask must be freed using free().
 */
CAResult_t CAWiFiGetInterfaceSubnetMask(char **subnetMask);

/**
 * @brief  Get WIFI adapter connection state.
 *
 * @return  True if WIFI adapter is connected, otherwise false
 */
bool CAWiFiIsConnected(void);

/**
 * @brief  Set callback for receiving local wifi adapter connection status.
 *
 * @param  callback [IN] Callback to be notified when local WIFI adapter connection state changes.
 * @return NONE
 */
void CAWiFiSetConnectionStateChangeCallback(CAWiFiConnectionStateChangeCallback callback);

#ifdef __cplusplus
}
#endif

#endif //_CA_WIFI_INTERFACE_H_
