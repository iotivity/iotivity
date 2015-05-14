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
 * @file
 *
 * This file provides APIs for EDR adapter - client, server, network monitor
 * modules.
 */

#ifndef CA_EDR_INTERFACE_H_
#define CA_EDR_INTERFACE_H_

#include "caedradapter.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef OIC_EDR_SERVICE_ID
#define OIC_EDR_SERVICE_ID "12341234-1C25-481F-9DFB-59193D238280"
#endif //OIC_EDR_SERVICE_ID

typedef enum
{
    STATE_DISCONNECTED, /**< State is Disconnected */
    STATE_CONNECTED /**< State is Connected */
} CAConnectedState_t;

typedef struct connected_state
{
    uint8_t address[CA_MACADDR_SIZE];
    CAConnectedState_t state;
} state_t;

/**
 * @enum  CAAdapterServerType_t
 * @brief Enum for defining different server types.
 */
typedef enum
{
    CA_UNICAST_SERVER = 0,    /**< Unicast Server */
    CA_MULTICAST_SERVER,      /**< Multicast Server */
    CA_SECURED_UNICAST_SERVER /**< Secured Unicast Server */
} CAAdapterServerType_t;

/**
 * @struct CAEDRData
 * @brief  Structure to maintain the information of data in message queue.
 */
typedef struct
{
    CARemoteEndpoint_t *remoteEndpoint; /**< Remote Endpoint */
    void *data;                         /**< Data to be sent */
    uint32_t dataLen;                   /**< Length of the data to be sent */
} CAEDRData;

/**
 * @struct CAEDRNetworkEvent
 * @brief  Structure to maintain the adapter information and its status.
 */
typedef struct
{
    CALocalConnectivity_t *info; /**< Local Connectivity Information */
    CANetworkStatus_t status;    /**< Network Status */
} CAEDRNetworkEvent;

/**
 * @brief This will be used during the recive of network requests and response.
 * @param remoteAddress [IN] EDR address of remote OIC device from which data received.
 * @param data          [IN] Data received
 * @param dataLength    [IN] Length of the Data received
 * @param sentLength    [OUT] Length of the sent data
 * @return NONE
 * @pre Callback must be registered using CAEDRSetPacketReceivedCallback()
 */
typedef void (*CAEDRDataReceivedCallback)(const char *remoteAddress, const void *data,
                                          uint32_t dataLength, uint32_t *sentLength);

/**
 * @brief This will be used during change in network status.
 * @param status        [IN] Network Status of the adapter
 * @return NONE
 */
typedef void (*CAEDRNetworkStatusCallback)(CANetworkStatus_t status);

/**
 * @brief  Initialize the network monitor module
 * @param  threadPool   [IN] Threadpool Handle
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_ADAPTER_NOT_ENABLED Initialization is successful, but bluetooth adapter is
 *                                 not enabled.
 * @retval #CA_STATUS_FAILED Operation failed
 * @see  CAEDRTerminateNetworkMonitor()
 */
CAResult_t CAEDRInitializeNetworkMonitor(const ca_thread_pool_t threadPool);

/**
 * @brief  Deinitialize with bluetooth adapter.
 * @return NONE
 * @pre    CAEDRInitializeNetworkMonitor() should be invoked before using this API.
 * @see    CAEDRInitializeNetworkMonitor()
 */
void CAEDRTerminateNetworkMonitor();

/**
 * @brief  Start Network Monitoring Process
 * @return #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAEDRStartNetworkMonitor();

/**
 * @brief  Stop Network Monitoring Process
 * @return #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAEDRStopNetworkMonitor();

/**
 * @brief  Sets the callback and Starts discovery for nearby OIC bluetooth devices.
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAEDRClientSetCallbacks();

/**
 * @brief  Resetting callbacks with bluetooth framework and stop OIC device discovery.
 * @return NONE
 * @pre    CAEDRClientSetCallbacks() should be invoked before using this API.
 * @see    CAEDRClientSetCallbacks()
 */
void CAEDRClientUnsetCallbacks();

/**
 * @brief  Used to initialize the EDR client module where mutex is initialized
 * @return NONE
 */
void CAEDRInitializeClient(ca_thread_pool_t handle);

/**
 * @brief  Destroys the Device list and mutex.
 * @return NONE
 */
void CAEDRClientTerminate();

/**
 * @brief  Closes all the client connection to peer bluetooth devices.
 * @return NONE
 */
void CAEDRClientDisconnectAll();

/**
 * @brief  Register callback to send the received packets from remote bluetooth device to BTAdapter.
 *
 * @param  packetReceivedCallback [IN] Callback function to register for sending network
 *                                     packets to EDR Adapter.
 * @return NONE
 */
void CAEDRSetPacketReceivedCallback(CAEDRDataReceivedCallback packetReceivedCallback);

/**
 * @brief  Register callback for receiving local bluetooth adapter state.
 *
 * @param  networkStateChangeCallback [IN] Callback function to register for receiving local
 *                                         bluetooth adapter status.
 * @return NONE
 */
void CAEDRSetNetworkChangeCallback(CAEDRNetworkStatusCallback networkStateChangeCallback);

/**
 * @brief  Get the local bluetooth adapter information.
 *
 * @param  info [OUT] Local bluetooth adapter information
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 *
 * @see #CALocalConnectivity_t
 *
 */
CAResult_t CAEDRGetInterfaceInformation(CALocalConnectivity_t **info);

/**
 * @brief  Start RFCOMM server for given service UUID
 *
 * @param  serviceUUID  [IN] The UUID of service with which RFCOMM server needs to be started.
 * @param  serverFD     [IN] The RFCOMM server socket file descriptor.
 * @param  handle       [IN] Threadpool Handle
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 *
 */
CAResult_t CAEDRServerStart(const char *serviceUUID, int *serverFD, ca_thread_pool_t handle);

/**
 * @brief  Stop RFCOMM server
 *
 * @param  serverFD [IN] The RFCOMM server socket file descriptor which needs to be stopped.
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAEDRServerStop(int serverFD);

/**
 * @brief   Terminate server for EDR
 * @return  None
 */
void CAEDRServerTerminate();

/**
 * @brief  All received data will be notified to upper layer.
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_FAILED Operation failed
 *
 */
CAResult_t CAEDRManagerReadData();

/**
 * @brief  This function gets bluetooth adapter enable state.
 * @param  state    [OUT] State of the Adapter.
 * @return #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAEDRGetAdapterEnableState(bool *state);

/**
 * @brief  This function sends data to specified remote bluetooth device.
 * @param  remoteAddress   [IN] Remote EDR Address
 * @param  serviceUUID     [IN] Service UUID of the device
 * @param  data            [IN] Data to be sent
 * @param  dataLength      [IN] Length of the data to be sent
 * @param  sentLength      [OUT] Length of the actual sent data
 * @return #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAEDRClientSendUnicastData(const char *remoteAddress, const char *serviceUUID,
                                      const void *data, uint32_t dataLength, uint32_t *sentLength);

/**
 * @brief  This function sends data to all bluetooth devices running OIC service.
 * @param  serviceUUID     [IN] Service UUID of the device
 * @param  data            [IN] Data to be sent
 * @param  dataLength      [IN] Length of the data to be sent
 * @param  sentLength      [OUT] Length of the actual sent data
 * @return #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAEDRClientSendMulticastData(const char *serviceUUID, const void *data,
                                        uint32_t dataLength, uint32_t *sentLength);

/**
 * @brief This function gets bonded bluetooth device list
 * @return #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAEDRGetBondedDeviceList();

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* CA_EDR_INTERFACE_H_ */

