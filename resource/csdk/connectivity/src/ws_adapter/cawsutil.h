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
 * Structure to hold the data to be sent over websocket connection
 */
typedef struct
{
    void        *data;
    uint32_t    dataLength;
} WSData;

/**
 * Linked list of pending data to be sent to websocket endpoint
 */
typedef struct _WSDataList
{
    WSData              *data;
    struct _WSDataList  *next;
} WSDataList;

/**
 * Indicates connection status of a websocket connection
 */
typedef enum
{
    WS_CONN_STATUS_DISCONNECTED         = 0,
    WS_CONN_STATUS_PENDING              = 1,
    WS_CONN_STATUS_CONNECTED            = 2
} WSConnStatus;

/**
 * Holds information related to a websocket connection
 */
typedef struct
{
    CASecureEndpoint_t  secureEndpoint;
    struct lws          *wsi;
    WSDataList          *pendingDataList;
    WSData              *recvData;
    WSConnStatus        status;
#ifdef __WITH_TLS__
    int secured;
#endif
} WSConnInfo;

/**
 * Linked list of websocket connection information
 */
typedef struct _WSConnInfoList
{
    WSConnInfo              *connInfo;
    struct _WSConnInfoList  *next;
} WSConnInfoList;

/**
 * Prepares the data to be sent in a WSData structure
 */
WSData *CAWSPrepareWSData(const void *data, uint32_t dataLength);

/**
 * Destroy the data in the WSData structure
 */
void CAWSDestroyWSData(WSData *data);

/**
 * Adds the WS data to the list
 */
CAResult_t CAWSAddWSDataToList(WSDataList **dataList, WSData *data);

/**
 * Removes the WS data from the list
 */
WSData *CAWSRemoveWSDataFromList(WSDataList **dataList);

/**
 * Destroys the WS data list
 */
void CAWSDestroyWSDataList(WSDataList *dataList);

/**
 * Destroys the information associated with a connection
 */
void CAWSDestroyWSConnInfo(WSConnInfo *connInfo);

/**
 * Adds connection information to the connection list
 */
CAResult_t CAWSAddWSConnInfoToList(WSConnInfoList **connInfoList, WSConnInfo *connInfo);

/**
 * Removes the connection information from the list
 */
bool CAWSRemoveWSConnInfoFromList(WSConnInfoList **connInfoList, WSConnInfo *connInfo);

/**
 * Given the endpoint finds the connection information
 * associated with this connection in the connection
 * information list
 */
WSConnInfo *CAWSFindWSConnInfoFromList(WSConnInfoList *connInfoList, const CAEndpoint_t *endpoint);

/**
 * Destroys the connection information list
 */
void CAWSDestroyWSConnInfoList(WSConnInfoList *connInfoList);

CAResult_t CAWSGetPeerAddress(int sockfd, char *host, uint16_t *port, CATransportFlags_t *flag);

#ifdef __cplusplus
}
#endif

#endif /* CA_WS_ADAPTER_UTIL_H_ */
