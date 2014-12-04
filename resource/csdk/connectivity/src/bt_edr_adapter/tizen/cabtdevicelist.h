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
 * @file    cabtdevicelist.h
 * @brief    This file provides APIs to access the discovered bluetooth device list
 */

#ifndef __CA_BT_DEVICE_LIST_H_
#define __CA_BT_DEVICE_LIST_H_

#include "cacommon.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @struct BTData
 * @brief Structure to maintain the data needs to send to peer Bluetooth device.
 */
typedef struct
{
    void *data;        /**< Data to be send to peer Bluetooth device. */
    uint32_t dataLength;    /**< Length of the data. */
} BTData;

/**
 * @struct BTDataList
 * @brief Structure to maintain list of data needs to send to peer Bluetooth device.
 */
typedef struct _BTDataList
{
    BTData *data;            /**< Data to be send to peer Bluetooth device. */
    struct _BTDataList *next;/**< Reference to next data in list. */
} BTDataList;

/**
 * @struct BTDevice
 * @brief Structure to maintain information of peer Bluetooth device.
 */
typedef struct
{
    char *remoteAddress;        /**< Address of peer Bluetooth device. */
    char *serviceUUID;          /**< OIC service UUID running in peer Bluetooth device. */
    int32_t socketFD;               /**< RfComm connection socket FD. */
    BTDataList *pendingDataList;/**< List of data needs to send to peer Bluetooth device. */
    uint32_t serviceSearched;        /**< Flag to indicate the status of service search. */
} BTDevice;

/**
 * @struct BTDeviceList
 * @brief Structure to maintain list of peer Bluetooth device information.
 */
typedef struct _BTDeviceList
{
    BTDevice *device;            /**< Bluetooth device information. */
    struct _BTDeviceList *next;  /**< Reference to next device information. */
} BTDeviceList;

CAResult_t CAAddBTDeviceToList(BTDeviceList **deviceList, BTDevice *device);
CAResult_t CARemoveBTDeviceFromList(BTDeviceList **deviceList, const char *remoteAddress);
CAResult_t CAGetBTDevice(BTDeviceList *deviceList, const char *remoteAddress, BTDevice **device);
CAResult_t CAGetBTDeviceBySocketId(BTDeviceList *deviceList, int32_t socket_id, BTDevice **device);
CAResult_t CACreateBTDevice(const char *remoteAddress, const char *uuid, BTDevice **device);
CAResult_t CACreateAndAddToDeviceList(BTDeviceList **deviceList, const char *remoteAddress,
                                      const char *serviceUUID, BTDevice **device);
void CAFreeBTDeviceList(BTDeviceList *deviceList);
void CAFreeBTDevice(BTDevice *device);

CAResult_t CAAddDataToDevicePendingList(BTDataList **dataList, void *data, uint32_t data_len);
CAResult_t CARemoveDataFromDevicePendingList(BTDataList **dataList);
CAResult_t CARemoveAllDataFromDevicePendingList(BTDataList **dataList);
void CAFreeDataFromBTDataList(BTDataList *dataList);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif //__CA_BT_DEVICE_LIST_H_

