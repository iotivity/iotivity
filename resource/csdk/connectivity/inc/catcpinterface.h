/* *****************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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
 * This file provides APIs TCP client/server/network monitor modules.
 */

#ifndef CA_TCP_INTERFACE_H_
#define CA_TCP_INTERFACE_H_

#include <stdbool.h>

#include "cacommon.h"
#include "catcpadapter.h"
#include "cathreadpool.h"
#include "uarraylist.h"
#include "oc_refcounter.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Callback to be notified on reception of any data from remote OIC devices.
 *
 * @param[in]  endpoint      network endpoint description.
 * @param[in]  data          Data received from remote OIC device.
 * @param[in]  dataLength    Length of data in bytes.
 * @pre  Callback must be registered using CAIPSetPacketReceiveCallback().
 */
typedef void (*CATCPPacketReceivedCallback)(const CASecureEndpoint_t *endpoint,
                                            const void *data,
                                            size_t dataLength);

/**
  * Callback to notify error in the TCP adapter.
  *
  * @param[in]  endpoint      network endpoint description.
  * @param[in]  data          Data sent/received.
  * @param[in]  dataLength    Length of data in bytes.
  * @param[in]  result        result of request from R.I.
  * @pre  Callback must be registered using CAIPSetPacketReceiveCallback().
 */
typedef void (*CATCPErrorHandleCallback)(const CAEndpoint_t *endpoint, const void *data,
                                         size_t dataLength, CAResult_t result);

/**
  * Callback to notify connection information in the TCP adapter.
  *
  * @param[in]  endpoint        network endpoint description.
  * @param[in]  isConnected     Whether keepalive message needs to be sent.
  * @param[in]  isClient        Host Mode of Operation.
  * @see  Callback must be registered using CATCPSetKeepAliveCallback().
 */
typedef void (*CATCPConnectionHandleCallback)(const CAEndpoint_t *endpoint, bool isConnected,
                                              bool isClient);

/**
 * set error callback to notify error in TCP adapter.
 *
 * @param[in]  errorHandleCallback Callback function to notify the error
 * in the TCP adapter.
 */
void CATCPSetErrorHandler(CATCPErrorHandleCallback errorHandleCallback);

#ifdef TCP_ADAPTER
/**
 * set keepalive callback to notify connection information in TCP adapter.
 *
 * @param[in]  keepaliveHandler Callback function to notify the connection information.
 * in the TCP adapter.
 */
void CATCPSetKeepAliveCallback(CAKeepAliveConnectionCallback keepaliveHandler);
#endif

/**
 * Start TCP server.
 *
 * @param   threadPool   Thread pool for managing Unicast server threads.
 * @return ::CA_STATUS_OK or Appropriate error code.
 * @retval ::CA_STATUS_OK  Successful.
 * @retval ::CA_STATUS_INVALID_PARAM Invalid input data.
 * @retval ::CA_STATUS_FAILED Initialization failed.
 */
CAResult_t CATCPStartServer(const ca_thread_pool_t threadPool);


/**
 * Stop TCP server.
 */
void CATCPStopServer(void);

/**
 * Set this callback for receiving data packets from peer devices.
 *
 * @param[in]  callback    Callback to be notified on reception of unicast data packets.
 */
void CATCPSetPacketReceiveCallback(CATCPPacketReceivedCallback callback);

/**
 * Set this callback for receiving the changed connection information from peer devices.
 *
 * @param[in]  connHandler    Callback to be notified when connection state changes.
 */
void CATCPSetConnectionChangedCallback(CATCPConnectionHandleCallback connHandler);

/**
 * API to send unicast TCP data.
 *
 * @param[in]  endpoint          complete network address to send to.
 * @param[in]  data              Data to be send.
 * @param[in]  dataLength        Length of data in bytes.
 * @return  Sent data length or -1 on error.
 */
ssize_t CATCPSendData(CAEndpoint_t *endpoint, const void *data, size_t dataLength);

/**
 * Get a list of CAInterface_t items.
 *
 * @return  List of CAInterface_t items.
 */
u_arraylist_t *CATCPGetInterfaceInformation(int desiredIndex);

/**
 * Connect to TCP Server.
 *
 * @param[in]   endpoint    remote endpoint information.
 * @return  Created socket file descriptor.
 */
CASocketFd_t CAConnectTCPSession(const CAEndpoint_t *endpoint);

/**
 * Disconnect from TCP Server.
 *
 * @param[in]   session     remove session information from list.
 * @return  ::CA_STATUS_OK or Appropriate error code.
 */
CAResult_t CADisconnectTCPSession(CATCPSessionInfo_t *session);

/**
 * Disconnect all connection from TCP Server.
 */
void CATCPDisconnectAll(void);

/**
 * Get TCP connection information from list. After using, a reference count
 * of information must be decremented by oc_refcounter_dec.
 *
 * @param[in]   endpoint    remote endpoint information.
 * @return  refcounter of TCP Session Information reference(CATCPSessionInfo_t) .
 */
oc_refcounter CAGetTCPSessionInfoRefCountedFromEndpoint(const CAEndpoint_t *endpoint);

/**
 * Get total length from CoAP over TCP header.
 *
 * @param[in]   recvBuffer    received header data.
 * @return  total data length
 */
size_t CAGetTotalLengthFromHeader(const unsigned char *recvBuffer);

/**
 * Get code from CoAP over TCP header.
 *
 * @param[in]   recvBuffer    received header data.
 * @return  method/response code
 */
uint32_t CAGetCodeFromHeader(const unsigned char *recvBuffer);

/**
 * Get socket file descriptor from remote device information.
 *
 * @param[in]   endpoint    Remote Endpoint information (such as ipaddress,
 *                          port, reference URI and transport type) to
 *                          which the unicast data has to be sent.
 * @return  Created socket file descriptor.
 */
CASocketFd_t CAGetSocketFDFromEndpoint(const CAEndpoint_t *endpoint);

/**
 * Find the session with endpoint info and remove it from list.
 *
 * @param[in]   endpoint    Remote Endpoint information (such as ipaddress,
 *                          port, reference URI and transport type) to
 *                          which the unicast data has to be sent.
 * @return  ::CA_STATUS_OK or Appropriate error code.
 */
CAResult_t CASearchAndDeleteTCPSession(const CAEndpoint_t *endpoint);

/**
 * Abort TCP sessions which are in progress of connecting with servers.
 */
void CATCPCloseInProgressConnections();

/**
 * Get total payload length from CoAP over TCP header.
 *
 * @param[in]   data    Data to be send.
 * @param[in]   dlen    Total data length.
 * @return  Payload length or 0 on error.
 */
size_t CACheckPayloadLengthFromHeader(const void *data, size_t dlen);

/**
 * Construct CoAP header and payload from buffer
 *
 * @param[in] svritem - used socket, buffer, current received message length and protocol
 * @param[in,out]  data  - data buffer, this value is updated as data is copied to svritem
 * @param[in,out]  dataLength  - length of data, this value decreased as data is copied to svritem
 * @return             - CA_STATUS_OK or appropriate error code
 */
CAResult_t CAConstructCoAP(CATCPSessionInfo_t *svritem, unsigned char **data,
                          size_t *dataLength);

/**
 * Clean socket state data
 *
 * @param[in,out] svritem - socket state data
 */
void CACleanData(CATCPSessionInfo_t *svritem);

#ifdef __cplusplus
}
#endif

#endif /* CA_TCP_INTERFACE_H_ */

