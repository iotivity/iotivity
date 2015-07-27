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

// BLE Interface APIs.
#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Stores the information of the Data to be sent from the queues.
 * This structure will be pushed to the sender/receiver queue for processing.
 */
typedef struct
{
    CAEndpoint_t *remoteEndpoint;   /**< Remote endpoint contains the
                                       information of remote device. */
    void *data;        /**< Data to be transmitted over LE tranport. */
    uint32_t dataLen;  /**< Length of the data being transmitted. */
} CALEData_t;

/**
 * Stores information of all the senders.
 * This structure will be used to track and defragment all incoming data packets.
 */
typedef struct
{
    uint32_t recvDataLen;
    uint32_t totalDataLen;
    char *defragData;
    CAEndpoint_t *remoteEndpoint;
}CABLESenderInfo_t;

/**
 * Initialize LE connectivity interface.
 * @param[in]  registerCallback Callback to register LE interfaces to
 *                               Connectivity Abstraction Layer.
 * @param[in]  reqRespCallback  Callback to notify request and response
 *                               messages from server(s) started at
 *                               Connectivity Abstraction Layer.
 * @param[in]  netCallback      Callback to notify the network additions
 *                               to Connectivity Abstraction Layer.
 * @param[in]  errorCallback    errorCallback to notify error to
 *                               connectivity common logic layer from adapter.
 * @param[in]  handle           Threadpool Handle.
 * @return ::CA_STATUS_OK or Appropriate error code.
 */
CAResult_t CAInitializeLE(CARegisterConnectivityCallback registerCallback,
                          CANetworkPacketReceivedCallback reqRespCallback,
                          CANetworkChangeCallback netCallback,
                          CAErrorHandleCallback errorCallback, ca_thread_pool_t handle);

/**
 * Starting LE connectivity adapters.
 * As its peer to peer it doesnot require to start any servers.
 * @return  ::CA_STATUS_OK or Appropriate error code.
 */
CAResult_t CAStartLE();

/**
 * Starting listening server for receiving multicast search requests.
 * Transport Specific Behavior:
 *   LE  Starts GATT Server with prefixed UUID and Characteristics as per
 *   OIC Specification.
 * @return  ::CA_STATUS_OK or Appropriate error code.
 */
CAResult_t CAStartLEListeningServer();

/**
 * for starting discovery servers for receiving multicast advertisements.
 * Transport Specific Behavior:
 *   LE  Starts GATT Server with prefixed UUID and Characteristics as per
 *   OIC Specification.
 * @return  ::CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAStartLEDiscoveryServer();

/**
 * Sends data to the endpoint using the adapter connectivity.
 * @param[in]   endpoint   Remote Endpoint information (like ipaddress ,
 *                          port, reference uri and connectivity type) to
 *                          which the unicast data has to be sent.
 * @param[in]   data       Data which required to be sent.
 * @param[in]   dataLen    Size of data to be sent.
 * @note  dataLen must be > 0.
 * @return  The number of bytes sent on the network, or -1 on error.
 */
int32_t CASendLEUnicastData(const CAEndpoint_t *endpoint, const void *data,
                             uint32_t dataLen);

/**
 * Sends Multicast data to the endpoint using the LE connectivity.
 * @param[in]   endpoint     Remote Endpoint information to which the
 *                            unicast data has to be sent.
 * @param[in]   data         Data which required to be sent.
 * @param[in]   dataLen      Size of data to be sent.
 * @note  dataLen must be > 0.
 * @return  The number of bytes sent on the network, or -1 on error.
 */
int32_t CASendLEMulticastData(const CAEndpoint_t *endpoint, const void *data, uint32_t dataLen);

/**
 * Starts notification server on EDR adapters.
 * @return  ::CA_STATUS_OK or Appropriate error code.
 */
CAResult_t CAStartLENotifyServer();

/**
 * Send notification information.
 * @param[in]   endpoint     Remote Endpoint information (like ipaddress ,
 *                            port, reference uri and connectivity type)
 *                            to which the unicast data has to be sent.
 * @param[in]   data         Data which required to be sent.
 * @param[in]   dataLen      Size of data to be sent.
 * @note dataLen must be > 0.
 * @return  The number of bytes sent on the network, or 0 on error.
 */
uint32_t CASendLENotification(const CAEndpoint_t *endpoint, const void *data,
                              uint32_t dataLen);

/**
 * Get LE Connectivity network information.
 * @param[out]  info          Local connectivity information structures.
 * @param[out]  size          Number of local connectivity structures.
 * @return ::CA_STATUS_OK or Appropriate error code.
 */
CAResult_t CAGetLEInterfaceInformation(CAEndpoint_t **info, uint32_t *size);

/**
 * Read Synchronous API callback.
 * @return  ::CA_STATUS_OK or Appropriate error code.
 */
CAResult_t CAReadLEData();

/**
 * Stopping the adapters and close socket connections.
 *   LE Stops all GATT servers and GATT Clients.
 * @return  ::CA_STATUS_OK or Appropriate error code.
 */
CAResult_t CAStopLE();

/**
 * Terminate the LE connectivity adapter.
 * Configuration information will be deleted from further use.
 */
void CATerminateLE();

/**
 * This function will receive the data from the GattServer and add the data to
 *         the Server receiver queue.
 * @param[in]  remoteAddress Remote address of the device from where data
 *                            is received.
 * @param[in]  serviceUUID   Uuid of the OIC service running on the remote
 *                            device.
 * @param[in]  data          Actual data recevied from the remote device.
 * @param[in]  dataLength    Length of the data received from the remote device.
 * @param[in]  sentLength    Length of the data sent from the remote device.
 * @return ::CA_STATUS_OK or Appropriate error code.
 * @retval ::CA_STATUS_OK  Successful.
 * @retval ::CA_STATUS_INVALID_PARAM  Invalid input argumets.
 * @retval ::CA_STATUS_FAILED Operation failed.
 *
 */
CAResult_t CALEAdapterServerReceivedData(const char *remoteAddress, const char *serviceUUID,
                                         const void *data, uint32_t dataLength,
                                         uint32_t *sentLength);

/**
 * This function will receive the data from the GattClient and add the
 * data into the Client receiver queue.
 * @param[in]  remoteAddress Remote address of the device from where data
 *                            is received.
 * @param[in]  serviceUUID   Uuid of the OIC service running on the remote
 *                            device.
 * @param[in]  data          Actual data recevied from the remote device.
 * @param[in]  dataLength    Length of the data received from the remote device.
 * @param[in]  sentLength    Length of the data sent from the remote device.
 * @return ::CA_STATUS_OK or Appropriate error code.
 * @retval ::CA_STATUS_OK  Successful.
 * @retval ::CA_STATUS_INVALID_PARAM  Invalid input argumets.
 * @retval ::CA_STATUS_FAILED Operation failed.
 */
CAResult_t CALEAdapterClientReceivedData(const char *remoteAddress, const char *serviceUUID,
                                         const void *data, uint32_t dataLength,
                                         uint32_t *sentLength);

/**
 * This function is used to set the NetworkPacket received callback to CA
 * layer from adapter layer.
 * @param[in]  callback callback handle sent from the upper layer.
 */
void CASetLEReqRespAdapterCallback(CANetworkPacketReceivedCallback callback);

/**
 * This function will push the data from CA layer to the Sender processor queue.
 *
 * @param[in]  remoteEndpoint Remote endpoint information of the server.
 * @param[in]  data           Data to be transmitted from LE.
 * @param[in]  dataLen        length of the Data being transmitted.
 *
 * @return ::CA_STATUS_OK or Appropriate error code.
 * @retval ::CA_STATUS_OK  Successful.
 * @retval ::CA_STATUS_INVALID_PARAM  Invalid input argumets.
 * @retval ::CA_STATUS_FAILED Operation failed.
 */
CAResult_t CALEAdapterServerSendData(const CAEndpoint_t *remoteEndpoint,
                                     const void *data, uint32_t dataLen);

/**
 * This function will push the data from CA layer to the Sender processor queue.
 *
 * @param[in]  remoteEndpoint Remote endpoint information of the server.
 * @param[in]  data           Data to be transmitted from LE.
 * @param[in]  dataLen        length of the Data being transmitted.
 *
 * @return ::CA_STATUS_OK or Appropriate error code.
 * @retval ::CA_STATUS_OK  Successful.
 * @retval ::CA_STATUS_INVALID_PARAM  Invalid input argumets.
 * @retval ::CA_STATUS_FAILED Operation failed.
 */
CAResult_t CALEAdapterClientSendData(const CAEndpoint_t *remoteEndpoint,
                                     const void *data,  uint32_t dataLen);

/**
 * This function will be associated with the sender queue for GattServer.
 * This function will fragment the data to the MTU of the transport and
 * send the data in fragments to the adapters. The function will be
 * blocked untill all data is sent out from the adapter.
 *
 * @param[in] threadData Data pushed to the queue which contains the info
 * about RemoteEndpoint and Data.
 */
void CALEServerSendDataThread(void *threadData);

/**
 * This function will be associated with the sender queue for GattClient.
 * This function will fragment the data to the MTU of the transport and
 * send the data in fragments to the adapters. The function will be
 * blocked untill all data is sent out from the adapter.
 *
 * @param[in]  threadData Data pushed to the queue which contains the info
 * about RemoteEndpoint and Data.
 */
void CALEClientSendDataThread(void *threadData);

/**
 * This function will be associated with the receiver queue. This function will defragment
 * the received data from each sender respectively and will send it up to CA layer.
 * Respective sender's header will provide the length of the data sent.
 *
 * @param [IN]  threadData Data pushed to the queue which contains the info about RemoteEndpoint
 * and Data.
 */
void CALEDataReceiverHandler(void *threadData);

/**
 * This function is used to Initalize both GattServer and GattClient
 * queues. All four queues will be initialized with this function invocations.
 */
void CAInitLEQueues();

/**
 * This function will stop all queues created for GattServer and GattClient. All
 * four queues will be be stopped with this function invocations.
 */
void CAStopLEQueues();

/**
 * This function will terminate all queues created for GattServer and
 * GattClient. All four queues will be be terminated with this function
 * invocations.
 */
void CATerminateLEQueues();

/**
 * This function will initalize the Receiver and Sender queues for
 * GattServer. This function will inturn call the functions
 * CAInitBleServerReceiverQueue() and  CAInitBleServerSenderQueue() to
 * initialize the queues.
 * @return ::CA_STATUS_OK or Appropriate error code.
 * @retval ::CA_STATUS_OK  Successful.
 * @retval ::CA_STATUS_INVALID_PARAM  Invalid input argumets.
 * @retval ::CA_STATUS_FAILED Operation failed.
 */
CAResult_t CAInitLEServerQueues();

/**
 * This function will initalize the Receiver and Sender queues for
 * GattClient. This function will inturn call the functions
 * CAInitBleClientReceiverQueue() and CAInitBleClientSenderQueue() to
 * initialize the queues.
 *
 * @return ::CA_STATUS_OK or Appropriate error code.
 * @retval ::CA_STATUS_OK  Successful.
 * @retval ::CA_STATUS_INVALID_PARAM  Invalid input argumets.
 * @retval ::CA_STATUS_FAILED Operation failed.
 *
 */
CAResult_t CAInitLEClientQueues();

/**
 * This function will initalize the Receiver queue for GattServer. This
 * will initialize the queue to process the function
 * CABLEServerSendDataThread() when ever the task is added to this queue.
 *
 * @return ::CA_STATUS_OK or Appropriate error code.
 * @retval ::CA_STATUS_OK  Successful.
 * @retval ::CA_STATUS_INVALID_PARAM  Invalid input argumets.
 * @retval ::CA_STATUS_FAILED Operation failed.
 */
CAResult_t CAInitLEServerSenderQueue();

/**
 * This function will initalize the Receiver queue for GattClient. This
 * will initialize the queue to process the function
 * CABLEClientSendDataThread() when ever the task is added to this queue.
 *
 * @return ::CA_STATUS_OK or Appropriate error code.
 * @retval ::CA_STATUS_OK  Successful.
 * @retval ::CA_STATUS_INVALID_PARAM  Invalid input argumets.
 * @retval ::CA_STATUS_FAILED Operation failed.
 */
CAResult_t CAInitLEClientSenderQueue();

/**
 * This function will initalize the Receiver queue for LEAdapter. This will initialize
 * the queue to process the function CABLEDataReceiverHandler() when ever the task
 * is added to this queue.
 *
 * @return ::CA_STATUS_OK or Appropriate error code.
 * @retval ::CA_STATUS_OK  Successful.
 * @retval ::CA_STATUS_INVALID_PARAM  Invalid input argumets.
 * @retval ::CA_STATUS_FAILED Operation failed.
 *
 */
CAResult_t CAInitLEReceiverQueue();

/**
 * This function will create the Data required to send it in the queue.
 *
 * @param[in]  remoteEndpoint Remote endpoint information of the server.
 * @param[in]  data           Data to be transmitted from LE.
 * @param[in]  dataLength     length of the Data being transmitted.
 *
 * @return ::CA_STATUS_OK or Appropriate error code.
 * @retval ::CA_STATUS_OK  Successful.
 * @retval ::CA_STATUS_INVALID_PARAM  Invalid input argumets.
 * @retval ::CA_STATUS_FAILED Operation failed.
 */
CALEData_t *CACreateLEData(const CAEndpoint_t *remoteEndpoint, const void *data,
                           uint32_t dataLength);

/**
 * Used to free the BLE information stored in the sender/receiver queues.
 * @param[in] bleData Structure contains the information of a particular
 * data segment.
 */
void CAFreeLEData(CALEData_t *bleData);

/**
 * This will be used to notify device status changes to the LE adapter layer.
 * @param[in]  adapter_state State of the adapter.
 */
typedef void (*CALEDeviceStateChangedCallback)(CAAdapterState_t adapter_state);

/**
 * This will be used to notify that network packet recieved from
 * GATTClient to adapter layer.
 * @param[in]  remoteAddress  Remote endpoint Address.
 * @param[in]  serviceUUID    Service UUID.
 * @param[in]  data           Data received.
 * @param[in]  dataLength     Length of the data received.
 * @param[in]  sentLength     Length of the data sent.
 * @return ::CA_STATUS_OK or Appropriate error code.
 * @retval ::CA_STATUS_OK  Successful.
 * @retval ::CA_STATUS_INVALID_PARAM  Invalid input argumets.
 * @retval ::CA_STATUS_FAILED Operation failed.
 */
typedef CAResult_t (*CABLEClientDataReceivedCallback)(const char *remoteAddress,
                                                      const char *serviceUUID, const void *data,
                                                      uint32_t dataLength, uint32_t *sentLength);

/**
 * This will be used to notify that network packet recieved from
 * GATTServer to adapter layer.
 * @param[in]  remoteAddress  Remote endpoint Address.
 * @param[in]  serviceUUID    Service UUID.
 * @param[in]  data           Data received.
 * @param[in]  dataLength     Length of the data received.
 * @param[in]  sentLength     Length of the data sent.
 * @return ::CA_STATUS_OK or Appropriate error code.
 * @retval ::CA_STATUS_OK  Successful.
 * @retval ::CA_STATUS_INVALID_PARAM  Invalid input argumets.
 * @retval ::CA_STATUS_FAILED Operation failed.
 */
typedef CAResult_t (*CABLEServerDataReceivedCallback)(const char *remoteAddress,
                                                      const char *serviceUUID, const void *data,
                                                      uint32_t dataLength, uint32_t *sentLength);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* CA_LEADAPTER_H_ */
