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
 * @file cawifiserver.h
 * @brief This file contains the APIs for WiFi Server module
 */

#ifndef _CA_WIFI_SERVER_H_
#define _CA_WIFI_SERVER_H_

#include "caadapterinterface.h"
#include "logger.h"
#include "uthreadpool.h"
#include "camessagequeue.h"

/**
 * @brief API to Initialize Server Mutex
 * @return - None
 */
void CAInitializeServerMutex();

/**
 * @brief API to Set the Receive queue handle
 * @param  recvQueueHandle          [IN] Queue Handle to Receive Data
 * @return - None
 */
void CASetRecvQueueHandle(CAAdapterMessageQueue_t *recvQueueHandle);

/**
 * @brief API to Get thread pool handle initialized in interface sample.
 * @return - None
 */
void CASetThreadHandle(u_thread_pool_t gThreadPool);

/**
 * @brief API to start unicast server.
 * @param localAddress - Local Unicast IP address to bind the socket.
 * @param port - Local port number where socket will listen for incoming request.
 * @return - Error Code
 */
CAResult_t CAStartUnicastServer(const char *localAddress, int16_t *port);

/**
 * @brief API to start multicast server.
 * @param mcastAddress - IP address to join multicast group.
 * @param localAddress - Local Unicast IP address to bind the socket.
 * @param port - multicast port number where socket will listen for incoming request.
 * @return - Error Code
 */
CAResult_t CAStartMulticastServer(const char *mcastAddress, const char *localAddress,
                                  int16_t *port);

/**
 * @brief API to stop unicast server.
 * @return - Error Code
 */
CAResult_t CAStopUnicastServer();

/**
 * @brief API to stop multicast server.
 * @return - Error Code
 */
CAResult_t CAStopMulticastServer();

/**
 * @brief API to receive the data in unicast server socket thread.
 * @param data - data received in server from lower layer.
 * @return - NULL
 */
void *CAReceiveThreadForUnicast(void *data);

/**
 * @brief API to receive the data in multicast server socket thread.
 * @param data - data received in server from lower layer.
 * @return - NULL
 */
void *CAReceiveThreadForMulticast(void *data);
/* Skip Queue */
/**
 * @brief API to set request response callback to upper layer.
 * @param RequestResponseCallback - upper layer callback function to pass the data received in the server.
 * @return - Error Code
 */
void CASetWIFINetworkPacketCallback(CANetworkPacketReceivedCallback callback);

#endif  // #ifndef _CA_WIFI_SERVER_H_
