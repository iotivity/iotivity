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
 * @file caleadapter.h
 * @brief This file contains the APIs for LE adapters to be implemented
 */
#ifndef _CA_LEADAPTER_H_
#define _CA_LEADAPTER_H_

#include "cacommon.h"
#include "caadapterinterface.h"
#ifndef ARDUINO
#include "uthreadpool.h" /* for thread pool */
#endif  //ARDUINO

/**
 * BLE Interface APIs.
 */
#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @Structure CABLEData
 * @brief Stores the information of the Data to be sent from the queues.
 *           This structure will be pushed to the sender/receiver queue for processing.
 */
typedef struct
{
    CARemoteEndpoint_t
    *remoteEndpoint;    /**< Remote endpoint contains the inforamtion of remote device */
    void *data;                                               /**< Data to be transmitted over LE tranport */
    uint32_t dataLen;                                     /**< Length of the data being transmitted */
} CABLEData;

/** @brief Initialize LE connectivity interface.
 * @param registerCallback [IN] To register LE interfaces to Connectivity Abstraction Layer
 * @param reqRespCallback [IN] sending responses and discovery messages from unicast , \
 *                                             multicast servers
 * @param netCallback [IN] Intimate the network additions to Connectivity Abstraction Layer.
 * @return CA_STATUS_OK or ERROR CODES ( CAResult_t error codes in cacommon.h)
 */
#ifdef ARDUINO
CAResult_t CAInitializeLE(CARegisterConnectivityCallback registerCallback,
                          CANetworkPacketReceivedCallback reqRespCallback,
                          CANetworkChangeCallback netCallback);
#else
CAResult_t CAInitializeLE(CARegisterConnectivityCallback registerCallback,
                          CANetworkPacketReceivedCallback reqRespCallback,
                          CANetworkChangeCallback netCallback,
                          u_thread_pool_t handle);
#endif //#ifdef ARDUINO

/**
 * @brief Starting LE connectivity adapters . \
 *           As its peer to peer it doesnot require to start any servers
 * @return CA_STATUS_OK or ERROR CODES ( CAResult_t error codes in cacommon.h)
 */
CAResult_t CAStartLE();

/**
 * @brief Starting listening server for receiving multicast search requests
 * Transport Specific Behavior:
 *   LE  Starts GATT Server with prefixed UUID and Characteristics as per OIC Specification.
 * @return CA_STATUS_OK or ERROR CODES ( CAResult_t error codes in cacommon.h)
 */
CAResult_t CAStartLEListeningServer();

/**
 * @brief for starting discovery servers for receiving multicast advertisements
 * Transport Specific Behavior:
 *   LE  Starts GATT Server with prefixed UUID and Characteristics as per OIC Specification.
 * @return CA_STATUS_OK or ERROR CODES ( CAResult_t error codes in cacommon.h)
 */
CAResult_t CAStartLEDiscoveryServer();

/**
 * @brief Sends data to the endpoint using the adapter connectivity.
 * Note: length must be > 0.
 * @param   endpoint  [IN]  Remote Endpoint information (like ipaddress , port, reference uri \
 *                          and connectivity type) to which the unicast data has to be sent.
 * @param   data        [IN]  Data which required to be sent.
 * @param   dataLen   [IN]  Size of data to be sent.
 * @return - The number of bytes sent on the network. Return value equal to zero indicates error.
 */
uint32_t CASendLEUnicastData(const CARemoteEndpoint_t *endpoint, void *data,
                             uint32_t dataLen);

/**
 * @brief Sends Multicast data to the endpoint using the LE connectivity.
 * Note: length must be > 0.
 * @param   data        [IN]    Data which required to be sent.
 * @param   dataLen     [IN]    Size of data to be sent.
 * @return - The number of bytes sent on the network. Return value equal to zero indicates error.
 */
uint32_t CASendLEMulticastData(void *data, uint32_t dataLen);

/**
 * @brief Starts notification server on EDR adapters.
 * @return CA_STATUS_OK or ERROR CODES ( CAResult_t error codes in cacommon.h)
 */
CAResult_t CAStartLENotifyServer();

/**
 * @brief Send notification information.
 * Note: length must be > 0.
 * @param   endpoint  [IN]    Remote Endpoint information (like ipaddress , port, reference uri \
 *                            and connectivity type) to which the unicast data has to be sent.
 * @param   data        [IN]    Data which required to be sent.
 * @param   dataLen   [IN]    Size of data to be sent.
 * @return - The number of bytes sent on the network. Return value equal to zero indicates error.
 */
uint32_t CASendLENotification(const CARemoteEndpoint_t *endpoint, void *data,
                              uint32_t dataLen);

/**
 * @brief Get LE Connectivity network information
 * @param   info        [OUT]   Local connectivity information structures
 * @param   size        [OUT]   Number of local connectivity structures.
 * @return CA_STATUS_OK or ERROR CODES ( CAResult_t error codes in cacommon.h)
 */
CAResult_t CAGetLEInterfaceInformation(CALocalConnectivity_t **info, uint32_t *size);

/**
 * @brief Read Synchronous API callback.
 * @return CA_STATUS_OK or ERROR CODES ( CAResult_t error codes in cacommon.h)
 */
CAResult_t CAReadLEData();

/**
 * @brief Stopping the adapters and close socket connections
 *   LE Stops all GATT servers and close sockets.
 * @return CA_STATUS_OK or ERROR CODES ( CAResult_t error codes in cacommon.h)
 */
CAResult_t CAStopLE();

/**
 * @brief Terminate the LE connectivity adapter.
 * Configuration information will be deleted from further use
 */
void CATerminateLE();

CAResult_t CABLEServerReceivedData(const char *remoteAddress, const char *serviceUUID,
                                        void *data, uint32_t dataLength, uint32_t *sentLength);

CAResult_t CABLEClientReceivedData(const char *remoteAddress, const char *serviceUUID,
                                        void *data, uint32_t dataLength, uint32_t *sentLength);

void CASetBLEReqRespAdapterCallback(CANetworkPacketReceivedCallback callback);

CAResult_t CABLEServerSendData(const CARemoteEndpoint_t *remoteEndpoint,
        void *data, uint32_t dataLen);

CAResult_t CABLEClientSendData(const CARemoteEndpoint_t *remoteEndpoint,
        void *data,  uint32_t dataLen);

void CABLEClientSendDataThread(void *threadData);

void CABLEClientDataReceiverHandler(void *threadData);

void CATerminateBleQueues();

CAResult_t CAInitBleClientReceiverQueue();

CAResult_t CAInitBleServerReceiverQueue();

void CAInitBleQueues();

void CATerminateBleQueues();

CAResult_t CAInitBleServerQueues();

CAResult_t CAInitBleClientQueues();

CAResult_t CAInitBleServerSenderQueue();

CAResult_t CAInitBleClientSenderQueue();



/**
 * @fn CABLEDataReceiverHandler
 * @brief This function handles data from recv message queue.
 */
void CABLEServerDataReceiverHandler(void *context);

/**
 * @fn CABLESendDataThread
 * @brief This function handles data from sender message queue.
 */
void CABLEServerSendDataThread(void *threadData);

/**
* @fn  CACreateBLEData
* @brief  This function will create the Data required to send it in the queue.
*
* @param[in]  remoteEndpoint  Remote endpoint information of the server.
* @param[in]  data Data to be transmitted from LE.
* @param[in]  length of the Data being transmitted.
*
* @return  0 on success otherwise a positive error value.
* @retval  CA_STATUS_OK  Successful
* @retval  CA_STATUS_INVALID_PARAM  Invalid input argumets
* @retval  CA_STATUS_FAILED Operation failed
*
*/
CABLEData *CACreateBLEData(const CARemoteEndpoint_t *remoteEndpoint, void *data,
                                  uint32_t dataLength);

/**
* @fn  CAFreeBLEData
* @brief  Used to free the BLE information stored in the sender/receiver queues.
*
* @param[in]  bleData  Structure contains the information of a particular data segment.
*
* @return  none.
*/
void CAFreeBLEData(CABLEData *bleData);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif //#ifndef _CA_LEADAPTER_H_
