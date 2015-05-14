/* ****************************************************************
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
 * This file contains the APIs for LE adapters to be implemented.
 */

#ifndef CA_LEADAPTER_H_
#define CA_LEADAPTER_H_

#include "cacommon.h"
#include "caadapterinterface.h"
#include "cathreadpool.h" /* for thread pool */

/**
 * BLE Interface APIs.
 */
#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @struct CALEData_t
 * @brief Stores the information of the Data to be sent from the queues.
 *        This structure will be pushed to the sender/receiver queue for processing.
 */
typedef struct
{
    CARemoteEndpoint_t
    *remoteEndpoint;   /**< Remote endpoint contains the inforamtion of remote device */
    void *data;        /**< Data to be transmitted over LE tranport */
    uint32_t dataLen;  /**< Length of the data being transmitted */
} CALEData_t;

/**
 * @brief  Initialize LE connectivity interface.
 * @param  registerCallback [IN] Callback to register LE interfaces to Connectivity
 *                               Abstraction Layer
 * @param  reqRespCallback  [IN] Callback to notify request and response messages from server(s)
 *                               started at Connectivity Abstraction Layer.
 * @param  netCallback      [IN] Callback to notify the network additions to Connectivity
 *                               Abstraction Layer.
 * @param  handle           [IN] Threadpool Handle
 * @return #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAInitializeLE(CARegisterConnectivityCallback registerCallback,
                          CANetworkPacketReceivedCallback reqRespCallback,
                          CANetworkChangeCallback netCallback,
                          ca_thread_pool_t handle);

/**
 * @brief Starting LE connectivity adapters.
 *        As its peer to peer it doesnot require to start any servers
 * @return  #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAStartLE();

/**
 * @brief Starting listening server for receiving multicast search requests
 * Transport Specific Behavior:
 *   LE  Starts GATT Server with prefixed UUID and Characteristics as per OIC Specification.
 * @return  #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAStartLEListeningServer();

/**
 * @brief for starting discovery servers for receiving multicast advertisements
 * Transport Specific Behavior:
 *   LE  Starts GATT Server with prefixed UUID and Characteristics as per OIC Specification.
 * @return  #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAStartLEDiscoveryServer();

/**
 * @brief Sends data to the endpoint using the adapter connectivity.
 * @param   endpoint  [IN]  Remote Endpoint information (like ipaddress , port, reference uri \
 *                          and connectivity type) to which the unicast data has to be sent.
 * @param   data      [IN]  Data which required to be sent.
 * @param   dataLen   [IN]  Size of data to be sent.
 * @return  The number of bytes sent on the network. Returns -1 on error.
 * @remarks  dataLen must be > 0.
 */
int32_t CASendLEUnicastData(const CARemoteEndpoint_t *endpoint, const void *data,
                             uint32_t dataLen);

/**
 * @brief Sends Multicast data to the endpoint using the LE connectivity.
 * @param   data        [IN]    Data which required to be sent.
 * @param   dataLen     [IN]    Size of data to be sent.
 * @return  The number of bytes sent on the network. Returns -1 on error.
 * @remarks  dataLen must be > 0.
 */
int32_t CASendLEMulticastData(const void *data, uint32_t dataLen);

/**
 * @brief Starts notification server on EDR adapters.
 * @return  #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAStartLENotifyServer();

/**
 * @brief   Send notification information.
 * @param   endpoint  [IN]    Remote Endpoint information (like ipaddress , port, reference uri \
 *                            and connectivity type) to which the unicast data has to be sent.
 * @param   data      [IN]    Data which required to be sent.
 * @param   dataLen   [IN]    Size of data to be sent.
 * @return  The number of bytes sent on the network. Returns 0 on error.
 * @remarks dataLen must be > 0.
 */
uint32_t CASendLENotification(const CARemoteEndpoint_t *endpoint, const void *data,
                              uint32_t dataLen);

/**
 * @brief  Get LE Connectivity network information
 * @param  info        [OUT]   Local connectivity information structures
 * @param  size        [OUT]   Number of local connectivity structures.
 * @return #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAGetLEInterfaceInformation(CALocalConnectivity_t **info, uint32_t *size);

/**
 * @brief Read Synchronous API callback.
 * @return  #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAReadLEData();

/**
 * @brief Stopping the adapters and close socket connections
 *   LE Stops all GATT servers and GATT Clients.
 * @return  #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAStopLE();

/**
 * @brief Terminate the LE connectivity adapter.
 * Configuration information will be deleted from further use
 */
void CATerminateLE();

/**
 * @brief  This function will receive the data from the GattServer and add the data to
 *         the Server receiver queue.
 * @param  remoteAddress [IN] Remote address of the device from where data is received.
 * @param  serviceUUID   [IN] Uuid of the OIC service running on the remote device
 * @param  data          [IN] Actual data recevied from the remote device.
 * @param  dataLength    [IN] Length of the data received from the remote device.
 * @param  sentLength    [IN] Length of the data sent from the remote device.
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 *
 */
CAResult_t CABLEServerReceivedData(const char *remoteAddress, const char *serviceUUID,
                                   const void *data, uint32_t dataLength, uint32_t *sentLength);

/**
 * @brief  This function will receive the data from the GattClient and add the data into the
 *         Client receiver queue.
 * @param  remoteAddress [IN] Remote address of the device from where data is received.
 * @param  serviceUUID   [IN] Uuid of the OIC service running on the remote device
 * @param  data          [IN] Actual data recevied from the remote device.
 * @param  dataLength    [IN] Length of the data received from the remote device.
 * @param  sentLength    [IN] Length of the data sent from the remote device.
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
CAResult_t CABLEClientReceivedData(const char *remoteAddress, const char *serviceUUID,
                                   const void *data, uint32_t dataLength, uint32_t *sentLength);

/**
 * @brief  This function is used to set the NetworkPacket received callback to CA layer from
 *         adapter layer.
 * @param  callback [IN] callback handle sent from the upper layer.
 * @return NONE
 */
void CASetBLEReqRespAdapterCallback(CANetworkPacketReceivedCallback callback);

/**
 * @brief  This function will push the data from CA layer to the Sender processor queue.
 *
 * @param  remoteEndpoint [IN] Remote endpoint information of the server.
 * @param  data           [IN] Data to be transmitted from LE.
 * @param  dataLen        [IN] length of the Data being transmitted.
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
CAResult_t CABLEServerSendData(const CARemoteEndpoint_t *remoteEndpoint,
                               const void *data, uint32_t dataLen);

/**
 * @brief  This function will push the data from CA layer to the Sender processor queue.
 *
 * @param  remoteEndpoint [IN] Remote endpoint information of the server.
 * @param  data           [IN] Data to be transmitted from LE.
 * @param  dataLen        [IN] length of the Data being transmitted.
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
CAResult_t CABLEClientSendData(const CARemoteEndpoint_t *remoteEndpoint,
                               const void *data,  uint32_t dataLen);

/**
 * @brief  This function will be associated with the sender queue for GattServer.This function will
 *         fragment the data to the MTU of the transport and send the data in fragments to the
 *         adapters. The function will be blocked untill all data is sent out from the adapter.
 *
 * @param threadData [IN] Data pushed to the queue which contains the info about RemoteEndpoint
 *                        and Data.
 *
 * @return  NONE
 */
void CABLEServerSendDataThread(void *threadData);

/**
 * @brief  This function will be associated with the sender queue for GattClient.This function will
 *         fragment the data to the MTU of the transport and send the data in fragments to the
 *         adapters. The function will be blocked untill all data is sent out from the adapter.
 *
 * @param  threadData [IN] Data pushed to the queue which contains the info about RemoteEndpoint
 *                         and Data.
 *
 * @return NONE
 */
void CABLEClientSendDataThread(void *threadData);

/**
 * @brief  This function will be associated with the receiver queue of GattServer. This function
 *         will defragment the data received and will send the data UP to the CA layer only after
 *         it collects all the data from the adapter layer. Adapter Header will provide the
 *         length of the data sent from the server.
 *
 * @param  threadData [IN] Data pushed to the queue which contains the info about RemoteEndpoint
 *                         and Data.
 *
 * @return  NONE
 */
void CABLEServerDataReceiverHandler(void *threadData);

/**
 * @brief  This function will be associated with the receiver queue of GattClient. This function
 *         will defragment the data received and will send the data UP to the CA layer only after
 *         it collects all the data from the adapter layer. Adapter Header will provide the length
 *         of the data sent from the server.
 *
 * @param  threadData [IN] Data pushed to the queue which contains the info about RemoteEndpoint
 *                         and Data.
 * @return NONE
 */
void CABLEClientDataReceiverHandler(void *threadData);

/**
 * @brief  This function is used to Initalize both GattServer and GattClient queues. All four
 *         queues will be initialized with this function invocations.
 * @return  NONE
 */
void CAInitBleQueues();

/**
 * @brief  This function will stop all queues created for GattServer and GattClient. All
 *         four queues will be be stopped with this function invocations.
 * @return  NONE
 */
void CAStopBleQueues();

/**
 * @brief  This function will terminate all queues created for GattServer and GattClient. All
 *         four queues will be be terminated with this function invocations.
 * @return  NONE
 */
void CATerminateBleQueues();

/**
 * @brief  This function will initalize the Receiver and Sender queues for GattServer. This
 *         function will inturn call the functions CAInitBleServerReceiverQueue() and
 *         CAInitBleServerSenderQueue() to initialize the queues.
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAInitBleServerQueues();

/**
 * @brief  This function will initalize the Receiver and Sender queues for GattClient. This
 *         function will inturn call the functions CAInitBleClientReceiverQueue() and
 *         CAInitBleClientSenderQueue() to initialize the queues.
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 *
 */
CAResult_t CAInitBleClientQueues();

/**
 * @brief  This function will initalize the Receiver queue for GattServer. This will initialize
 *         the queue to process the function CABLEServerSendDataThread() when ever the task is
 *         added to this queue.
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAInitBleServerSenderQueue();

/**
 * @brief  This function will initalize the Receiver queue for GattClient. This will initialize
 *         the queue to process the function CABLEClientSendDataThread() when ever the task is
 *         added to this queue.
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAInitBleClientSenderQueue();

/**
 * @brief  This function will initalize the Receiver queue for GattServer. This will initialize
 *         the queue to process the function CABLEServerDataReceiverHandler() when ever the task
 *         is added to this queue.
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 *
 */
CAResult_t CAInitBleServerReceiverQueue();

/**
 * @brief  This function will initalize the Receiver queue for GattClient. This will initialize
 *         the queue to process the function CABLEClientDataReceiverHandler() when ever the task
 *         is added to this queue.
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAInitBleClientReceiverQueue();

/**
 * @brief  This function will create the Data required to send it in the queue.
 *
 * @param  remoteEndpoint [IN] Remote endpoint information of the server.
 * @param  data           [IN] Data to be transmitted from LE.
 * @param  dataLength     [IN] length of the Data being transmitted.
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
CALEData_t *CACreateBLEData(const CARemoteEndpoint_t *remoteEndpoint, const void *data,
                           uint32_t dataLength);

/**
 * @brief Used to free the BLE information stored in the sender/receiver queues.
 * @param bleData [IN] Structure contains the information of a particular data segment.
 * @return NONE
 */
void CAFreeBLEData(CALEData_t *bleData);

/**
 * @brief This will be used to notify device status changes to the LE adapter layer
 * @param  adapter_state [IN] State of the adapter
 * @return NONE
 */
typedef void (*CALEDeviceStateChangedCallback)(CAAdapterState_t adapter_state);

/**
 * @brief This will be used to notify that network packet recieved from GATTClient to adapter layer.
 * @param  remoteAddress  [IN] Remote endpoint Address
 * @param  serviceUUID    [IN] Service UUID
 * @param  data           [IN] Data received
 * @param  dataLength     [IN] Length of the data received
 * @param  sentLength     [IN] Length of the data sent
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
typedef CAResult_t (*CABLEClientDataReceivedCallback)(const char *remoteAddress,
                                                      const char *serviceUUID, const void *data,
                                                      uint32_t dataLength, uint32_t *sentLength);

/**
 * @brief This will be used to notify that network packet recieved from GATTServer to adapter layer.
 * @param  remoteAddress  [IN] Remote endpoint Address
 * @param  serviceUUID    [IN] Service UUID
 * @param  data           [IN] Data received
 * @param  dataLength     [IN] Length of the data received
 * @param  sentLength     [IN] Length of the data sent
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
typedef CAResult_t (*CABLEServerDataReceivedCallback)(const char *remoteAddress,
                                                      const char *serviceUUID, const void *data,
                                                      uint32_t dataLength, uint32_t *sentLength);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* CA_LEADAPTER_H_ */

