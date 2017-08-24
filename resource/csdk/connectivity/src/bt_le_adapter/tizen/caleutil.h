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
 * This file contains the util function for LE adapter. This maintains the
 * list of services an individual GATT Client connected to and operations on
 * that list, such as getting the service info with BD address or with
 * position etc.
 */

#ifndef TZ_BLE_UTIL_H_
#define TZ_BLE_UTIL_H_

#include <bluetooth.h>

#include "cacommon.h"

typedef struct
{
    void *data;
    uint32_t dataLength;
} LEData;

typedef struct _LEDataList
{
    LEData *data;
    struct _LEDataList *next;
} LEDataList;

typedef enum
{
    LE_STATUS_INVALID = 0,
    LE_STATUS_UNICAST_PENDING,
    LE_STATUS_DISCOVERED,
    LE_STATUS_CONNECTION_INITIATED,
    LE_STATUS_CONNECTED,
    LE_STATUS_SERVICES_DISCOVERED
} LEDeviceStatus;

typedef struct
{
    bt_gatt_client_h clientHandle;
    bt_gatt_h serviceHandle;
    bt_gatt_h readChar;
    bt_gatt_h writeChar;
    char *remoteAddress;
    LEDataList *pendingDataList;
    LEDeviceStatus status;
} LEServerInfo;

typedef struct _LEServerInfoList
{
    LEServerInfo *serverInfo;
    struct _LEServerInfoList *next;
} LEServerInfoList;

typedef struct _LEClientInfoList
{
    char *remoteAddress;
    struct _LEClientInfoList *next;
} LEClientInfoList;

/**
 * Different characteristics types.
 *
 * This provides information of different characteristics
 * which will be added to OIC service.
 */
typedef enum
{
    BLE_GATT_WRITE_CHAR = 0, /**< write_char This will be used to get the unicast response. */
    BLE_GATT_READ_CHAR,      /**< read_char This will be used update value to OIC server. */
    BLE_GATT_NOTIFY_CHAR     /**< Reserved char for the time being. */
} CHAR_TYPE;

/**
 * Used to add LE data to the list
 *
 * @param[in] dataList      Pointer to the list to which data needs to be added
 * @param[in] data          Pointer to the data that needs to be added
 * @param[in] dataLength    The size of the data
 *
 * @return ::CA_STATUS_OK or appropriate error code
 * @retval ::CA_STATUS_OK   Successful
 * @retval ::CA_STATUS_INVALID_PARAM Invalid input arguments
 * @retval ::CA_MEMORY_ALLOC_FAILED Memory allocation failed
 */
CAResult_t CAAddLEDataToList(LEDataList **dataList, const void *data, uint32_t dataLength);

/**
 * Used to remove LE data from the front of the list.
 * The data is also destroyed
 *
 * @param[in] dataList  Pointer to the list
 */
void CARemoveLEDataFromList(LEDataList **dataList);

/**
 * Destroys the LE data list
 *
 * @param[in] dataList  Pointer to the list the needs to be destroyed
 */
void CADestroyLEDataList(LEDataList **dataList);

/**
 * Destroys LE data
 *
 * param[in] data   Pointer to the LE data that needs to be destroyed
 */
void CADestroyLEData(LEData *data);

/**
 * Used to add the serverListInfo structure to the Server List.
 *
 * @param[in] serverList     Pointer to the ble server list which holds the info of list of
 *                           servers registered by the client.
 * @param[in] leServerInfo   Pointer where serverInfo structure needs to be appended with
 *                           serverList.
 * @return ::CA_STATUS_OK or Appropriate error code.
 * @retval ::CA_STATUS_OK  Successful.
 * @retval ::CA_STATUS_INVALID_PARAM  Invalid input arguments.
 * @retval ::CA_STATUS_FAILED Operation failed.
 */
CAResult_t CAAddLEServerInfoToList(LEServerInfoList **serverList,
                                   LEServerInfo *leServerInfo);

/**
 * Used to remove the serverListInfo structure from the Server List.
 *
 * @param[in,out] serverList     Pointer to the ble server list which holds the info of list of
 *                               servers registered by the client.
 * @param[in]     remoteAddress  Remote address to be removed from the client list.
 */
void CARemoveLEServerInfoFromList(LEServerInfoList **serverList,
                                  const char *remoteAddress);

/**
 * Used to get the serviceInfo from the list.
 *
 * @param[in]  serverList       Pointer to the ble service list which holds the info of list
 *                              of servers registered by the client.
 * @param[in]  leAddress        BD address of the device where GATTServer information is required.
 * @param[out] leServerInfo     Info of service and characteristic handle of the given BD address
 *                              registered by client.
 * @return ::CA_STATUS_OK or Appropriate error code.
 * @retval ::CA_STATUS_OK  Successful.
 * @retval ::CA_STATUS_INVALID_PARAM  Invalid input arguments.
 * @retval ::CA_STATUS_FAILED Operation failed.
 */
CAResult_t CAGetLEServerInfo(LEServerInfoList *serverList, const char *leAddress,
                             LEServerInfo **leServerInfo);

/**
 * Used to clear BLE service list.
 *
 * @param[in]  serverList   Pointer to the ble service list which holds the info of list of
 *                          servers registered by the client.
 */
void CAFreeLEServerList(LEServerInfoList *serverList);

/**
 * @brief Used to get remove particular BLE service info from list
 * @param[in] bleServerInfo Pointer to the structure which needs to be cleared.
 */
void CAFreeLEServerInfo(LEServerInfo *bleServerInfo);

/**
 * Used to add the client address to the Client List.
 *
 * @param[in] clientList     Pointer to the ble client list which holds the info of list of
 *                           clients connected to the server.
 * @param[in] clientAddress  Client remote address.
 * @return ::CA_STATUS_OK or Appropriate error code.
 * @retval ::CA_STATUS_OK  Successful.
 * @retval ::CA_STATUS_INVALID_PARAM  Invalid input arguments.
 * @retval ::CA_STATUS_FAILED Operation failed.
 */
CAResult_t CAAddLEClientInfoToList(LEClientInfoList **clientList,
                                   char *clientAddress);

/**
 * Used to remove the client address from the Client List.
 *
 * @param[in,out] clientList     Pointer to the ble client list which holds the info of list of
 *                               clients connected by the server.
 * @param[in]     clientAddress  Remote address to be removed from the client list.
 */
void CARemoveLEClientInfoFromList(LEClientInfoList **clientList,
                                  const char *clientAddress);

/**
 * Used to disconnect all the clients connected to the server.
 *
 * @param[in,out] clientList     Pointer to the ble client list which holds the info of list of
 *                               clients connected by the server.
 */
void CADisconnectAllClient(LEClientInfoList *clientList);

/**
 * Used to get the Error message.
 * @param[in] err   Error code(::bt_error_e).
 * @return  Error string corresponding to the BT error code.
 */
const char *CALEGetErrorMsg(bt_error_e err);

#endif /* TZ_BLE_UTIL_H_ */