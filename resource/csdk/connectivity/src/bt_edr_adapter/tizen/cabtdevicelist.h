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
 * @file  cabtdevicelist.h
 * @brief  This file provides APIs to manage discovered bluetooth device list.
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
    void *data;             /**< Data to be send to peer Bluetooth device. */
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
    int32_t socketFD;           /**< RfComm connection socket FD. */
    BTDataList *pendingDataList;/**< List of data needs to send to peer Bluetooth device. */
    uint32_t serviceSearched;   /**< Flag to indicate the status of service search. */
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

/**
 * @fn  CACreateAndAddToDeviceList
 * @brief  Creates #BTDevice for specified remote address and uuid and to device list.
 *
 * @param[in][out]  deviceList Device list which created device add to.
 * @param[in]  deviceAddress  Bluetooth device address.
 * @param[in]  uuid  service uuid.
 * @param[in]  device  created #BTDevice.
 *
 * @return  #CA_STATUS_OK on success otherwise proper error code.
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_INVALID_PARAM Invalid input parameters
 * @retval  #CA_STATUS_FAILED Failed to create device and add to list
 *
 */
CAResult_t CACreateAndAddToDeviceList(BTDeviceList **deviceList, const char *deviceAddress,
                                      const char *uuid, BTDevice **device);

/**
 * @fn  CAAddBTDeviceToList
 * @brief  Insert device to specified list.
 *
 * @param[in][out]  deviceList  Device list to which specifed @device to be added.
 * @param[in]  device  Device to be added to list.
 *
 * @return  #CA_STATUS_OK on success otherwise proper error code.
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_INVALID_PARAM Invalid input parameters
 * @retval  #CA_MEMORY_ALLOC_FAILED Memory allocation failed
 *
 */
CAResult_t CAAddBTDeviceToList(BTDeviceList **deviceList, BTDevice *device);

/**
 * @fn  CAGetBTDevice
 * @brief  Get the device from list which matches specified device address.
 *
 * @param[in]  deviceList  Device list to search for the device.
 * @param[in]  deviceAddress  Device address used for matching.
 * @param[out]  device  #BTDevice which has matching device address.
 *
 * @return  #CA_STATUS_OK on success otherwise proper error code.
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_INVALID_PARAM Invalid input parameters
 * @retval  #CA_STATUS_FAILED Device is not found in the list
 *
 */
CAResult_t CAGetBTDevice(BTDeviceList *deviceList, const char *deviceAddress, BTDevice **device);

/**
 * @fn  CAGetBTDeviceBySocketId
 * @brief  Get the device from list which matches specified RFCOMM socket id.
 *
 * @param[in]  deviceList  Device list to search for the device.
 * @param[in]  socketID  RFCOMM socket id.
 * @param[out]  device  #BTDevice which has matching RFCOMM socket id .
 *
 * @return  #CA_STATUS_OK on success otherwise proper error code.
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_INVALID_PARAM Invalid input parameters
 * @retval  #CA_STATUS_FAILED Device is not found in the list
 *
 */
CAResult_t CAGetBTDeviceBySocketId(BTDeviceList *deviceList, int32_t socketID, BTDevice **device);

/**
 * @fn  CARemoveBTDeviceFromList
 * @brief  Remove and delete the device matching specified device address from list.
 *
 * @param[in][out]  deviceList  Device list to search for the device.
 * @param[in]  deviceAddress  Bluetooth device address.
 *
 * @return  #CA_STATUS_OK on success otherwise proper error code.
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_INVALID_PARAM Invalid input parameters
 * @retval  #CA_STATUS_FAILED Device is not found in the list
 *
 */
CAResult_t CARemoveBTDeviceFromList(BTDeviceList **deviceList, const char *deviceAddress);

/**
 * @fn  CADestroyBTDeviceList
 * @brief  Destroy the specified device list. Removes and delete all the devices in the list.
 *
 * @param[in][out]  deviceList  Device list to be destroyed.
 *
 */
void CADestroyBTDeviceList(BTDeviceList **deviceList);

/**
 * @fn  CAAddBTDataToList
 * @brief  Insert data to specified list.
 *
 * @param[in][out]  dataList  Data list to which data will be add.
 * @param[in]  data  Data to be stored.
 * @param[in]  data_len  Length of the data.
 *
 * @return  #CA_STATUS_OK on success otherwise proper error code.
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_INVALID_PARAM Invalid input parameters
 * @retval  #CA_MEMORY_ALLOC_FAILED Memory allocation failed
 *
 */
CAResult_t CAAddBTDataToList(BTDataList **dataList, void *data, uint32_t dataLength);

/**
 * @fn  CARemoveBTDataFromList
 * @brief  Remove and delete data from front end of list.
 *
 * @param[in][out]  dataList  Data list from which data will be removed.
 *
 * @return  #CA_STATUS_OK on success otherwise proper error code.
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_INVALID_PARAM Invalid input parameters
 *
 */
CAResult_t CARemoveBTDataFromList(BTDataList **dataList);

/**
 * @fn  CADestroyBTDataList
 * @brief  Destroy the specified data list. Removes and deletes all the data in the list.
 *
 * @param[in]  dataList  Data list to be destroyed.
 *
 */
void CADestroyBTDataList(BTDataList **dataList);

#ifdef __cplusplus
} /* extern "C" */
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif //__CA_BT_DEVICE_LIST_H_

