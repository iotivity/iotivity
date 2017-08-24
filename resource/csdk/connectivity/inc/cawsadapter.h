/* ****************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
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
 * This file contains the APIs for WS Adapter.
 */
#ifndef CA_WS_ADAPTER_H_
#define CA_WS_ADAPTER_H_

#include "cacommon.h"
#include "caadapterinterface.h"
#include "cathreadpool.h"
#include "cainterface.h"
#include <coap/pdu.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * API to initialize WS Interface.
 * @param[in] registerCallback      Callback to register WS interfaces to
 *                                  Connectivity Abstraction Layer.
 * @param[in] networkPacketCallback Callback to notify request and
 *                                  response messages from server(s)
 *                                  started at Connectivity Abstraction Layer.
 * @param[in] netCallback           Callback to notify the adapter changes
 *                                  to Connectivity Abstraction Layer.
 * @param[in] connCallback          Callback to notify the connection changes
 *                                  to Connectivity Abstraction Layer.
 * @param[in] errorCallback         Callback to notify the network errors to
 *                                  Connectivity Abstraction Layer.
 * @param[in] handle                Threadpool Handle.
 * @return  ::CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAInitializeWS(CARegisterConnectivityCallback registerCallback,
                          CANetworkPacketReceivedCallback networkPacketCallback,
                          CAAdapterChangeCallback netCallback,
                          CAConnectionChangeCallback connCallback,
                          CAErrorHandleCallback errorCallback, ca_thread_pool_t handle);

/**
 * Start WS Interface adapter.
 * @return  ::CA_STATUS_OK or Appropriate error code.
 */
CAResult_t CAStartWS();

/**
 * Start listening server for receiving connect requests.
 * Transport Specific Behavior:
 * WS adapter does not support this functionality
 * @return  ::CA_STATUS_OK
 */
CAResult_t CAStartWSListeningServer();

/**
 * Stops listening server from receiving connect requests.
 * Transport Specific Behavior:
 * WS adapter does not support this functionality
 * @return  ::CA_STATUS_OK
 */
CAResult_t CAStopWSListeningServer();

/**
 * Start discovery servers for receiving advertisements.
 * Transport Specific Behavior:
 * WS adapter does not support this functionality
 * number as per OIC Specification.
 * @return  ::CA_STATUS_OK
 */
CAResult_t CAStartWSDiscoveryServer();

/**
 * Sends data to the endpoint using the adapter connectivity.
 * @param[in]   endpoint       Remote Endpoint information (like ipaddress,
 *                              port, reference uri and transport type) to
 *                              which the unicast data has to be sent.
 * @param[in]   data           Data which is required to be sent.
 * @param[in]   dataLen        Size of data to be sent.
 * @param[in]   dataType       Data type which is REQUEST or RESPONSE.
 * @note  dataLen must be > 0 and <= INT_MAX.
 * @return  The number of bytes sent on the network, or -1 upon error.
 */
int32_t CASendWSUnicastData(const CAEndpoint_t *endpoint,
                            const void *data, uint32_t dataLen,
                            CADataType_t dataType);

/**
 * Send multicast data to the endpoint
 * Note: This functionality is not supported by WS adapter
 * @param[in]   endpoint       Remote Endpoint information (like ipaddress,
 *                              port)
 * @param[in]   data           Data which is required to be sent.
 * @param[in]   dataLen        Size of data to be sent.
 * @param[in]   dataType       Data type which is REQUEST or RESPONSE.
 * @note  dataLen must be > 0 and <= INT_MAX.
 * @return  The number of bytes sent on the network, or -1 upon error.
 */
int32_t CASendWSMulticastData(const CAEndpoint_t *endpoint,
                              const void *data, uint32_t dataLen,
                              CADataType_t dataType);

/**
 * Get WS Connectivity network information.
 * @param[out]   info        Local connectivity information structures.
 * @note info is allocated in this API and should be freed by the caller.
 * @param[out]   size        Number of local connectivity structures.
 * @return  ::CA_STATUS_OK or Appropriate error code.
 */
CAResult_t CAGetWSInterfaceInformation(CAEndpoint_t **info, size_t *size);

/**
 * Read Synchronous API callback.
 * Note: This functionality is not supported by WS adapter
 * @return  ::CA_STATUS_OK or Appropriate error code.
 */
CAResult_t CAReadWSData();

/**
 * Stops Unicast, servers and close the sockets.
 * @return  ::CA_STATUS_OK or Appropriate error code.
 */
CAResult_t CAStopWS();

/**
 * Terminate the WS connectivity adapter.
 * Configuration information will be deleted from further use.
 */
void CATerminateWS();

/*
 * For unit test purpose: Get the packet received callback
 * used by CA layer so that it can be invoked from unit tests
 * to check the stack response when a packet is received
 */
CANetworkPacketReceivedCallback CAGetWSNetworkPacketReceivedCallback();

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // CA_WS_ADAPTER_H_