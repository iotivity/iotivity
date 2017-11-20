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
 *
 * This file contains utility functions used by Websocket adapter
 * including functions to maintain websocket connections in a linked
 * list data structure
 */

#ifndef CA_WS_ADAPTER_UTIL_H_
#define CA_WS_ADAPTER_UTIL_H_

#include "cacommon.h"
#include "libwebsockets.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Structure to hold the data to be sent over websocket connection.
 */
typedef struct
{
    void        *data;          /**< Buffer that holds the data. */
    uint32_t    dataLength;     /**< Length of the data buffer. */
} WSData;

/**
 * Linked list of pending data to be sent to websocket endpoint.
 */
typedef struct _WSDataList
{
    WSData              *data;  /**< Data to be send to peer. */
    struct _WSDataList  *next;  /**< Pointer to the next node in the list. */
} WSDataList;

/**
 * Enum to represent different state of connection.
 */
typedef enum
{
    WS_CONN_STATUS_DISCONNECTED         = 0,    /**< Disconnected state. */
    WS_CONN_STATUS_PENDING              = 1,    /**< Connection initiated but pending. */
    WS_CONN_STATUS_CONNECTED            = 2     /**< Connected state. */
} WSConnStatus;

/**
 * Structure to hold information related to a websocket connection
 */
typedef struct
{
    CASecureEndpoint_t  secureEndpoint;         /**< Endpoint info of the websocket peer. */
    struct lws          *wsi;                   /**< lws struct that represents the connection. */
    WSDataList          *pendingDataList;       /**< List holding the data that needs to be sent. */
    WSData              *recvData;              /**< Data received on this connection. */
    WSConnStatus        status;                 /**< Status of the connection. */
#ifdef __WITH_TLS__
    int secured;                                /**< True if it is a secure connection, false otherwise. */
#endif
} WSConnInfo;

/**
 * Linked list of websocket connection information
 */
typedef struct _WSConnInfoList
{
    WSConnInfo              *connInfo;          /**< WebSocket connection information. */
    struct _WSConnInfoList  *next;              /**< Pointer to the next node in the list */
} WSConnInfoList;

/**
 * Prepare the data to be sent in a WSData structure
 *
 * @param[in] data          Buffer holding the data.
 * @param[in] dataLength    The length of the data buffer.
 *
 * @return A WSData which holds the data.
 */
WSData *CAWSPrepareWSData(const void *data, uint32_t dataLength);

/**
 * Destroy the data in WSData structure.
 *
 * @param[in] data      WSData to be destroyed.
 */
void CAWSDestroyWSData(WSData *data);

/**
 * Add the WS data to data list.
 *
 * @param[in] dataList  Data list to add the data.
 * @param[in] data      WSData to add to the list.
 *
 * @return ::CA_STATUS_OK or appropriate error code.
 */
CAResult_t CAWSAddWSDataToList(WSDataList **dataList, WSData *data);

/**
 * Remove the WS data from list.
 *
 * @param[in] dataList  Data list to remove data from.
 *
 * @return WSData that was removed from the list.
 */
WSData *CAWSRemoveWSDataFromList(WSDataList **dataList);

/**
 * Destroy the WS data list.
 *
 * @param[in] dataList  Data list to be destroyed.
 */
void CAWSDestroyWSDataList(WSDataList *dataList);

/**
 * Destroy the information associated with a connection.
 *
 * @param[in] connInfo  Connection information to be freed.
 */
void CAWSDestroyWSConnInfo(WSConnInfo *connInfo);

/**
 * Add connection information to the connection list.
 *
 * @param[in] connInfoList  Connection info list.
 * @param[in] connInfo      Connection info to be added to the list.
 *
 * @return ::CA_STATUS_OK or appropriate error code.
 */
CAResult_t CAWSAddWSConnInfoToList(WSConnInfoList **connInfoList, WSConnInfo *connInfo);

/**
 * Remove connection information from the list.
 *
 * @param[in] connInfoList  Connection info list.
 * @param[in] connInfo      Connection info to be removed from the list.
 *
 * @return true if removed else false.
 */
bool CAWSRemoveWSConnInfoFromList(WSConnInfoList **connInfoList, WSConnInfo *connInfo);

/**
 * Find connection from connection info list based on given endpoint information.
 *
 * @param[in] connInfoList  Connection info list.
 * @param[in] endpoint      Endpoint to search for.
 *
 * @return Connection info associated with this endpoint if found, otherwise NULL.
 */
WSConnInfo *CAWSFindWSConnInfoFromList(WSConnInfoList *connInfoList, const CAEndpoint_t *endpoint);

/**
 * Destroy the connection information list.
 *
 * @param[in] connInfoList  Connection info list.
 */
void CAWSDestroyWSConnInfoList(WSConnInfoList *connInfoList);

/**
 * Get the peer address of a websocket connection.
 *
 * @param[in]   sockfd   Socket descriptor id.
 * @param[out]  host     Peer address.
 * @param[out]  port     Port used at peer for connection.
 * @param[out]  flag     Transport flags.
 *
 * @return ::CA_STATUS_OK or appropriate error code.
 */
CAResult_t CAWSGetPeerAddress(int sockfd, char *host, uint16_t *port, CATransportFlags_t *flag);

#ifdef __cplusplus
}
#endif

#endif /* CA_WS_ADAPTER_UTIL_H_ */
