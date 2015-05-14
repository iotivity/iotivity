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
 * This file provides APIs to manage discovered bluetooth device list.
 */

#ifndef CA_EDR_DEVICE_LIST_H_
#define CA_EDR_DEVICE_LIST_H_

#include "cacommon.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @struct EDRData
 * @brief Structure to maintain the data needs to send to peer Bluetooth device.
 */
typedef struct
{
    void *data;             /**< Data to be sent to peer Bluetooth device. */
    uint32_t dataLength;    /**< Length of the data. */
} EDRData;

/**
 * @struct EDRDataList
 * @brief Structure to maintain list of data needs to send to peer Bluetooth device.
 */
typedef struct _EDRDataList
{
    EDRData *data;            /**< Data to be sent to peer Bluetooth device. */
    struct _EDRDataList *next;/**< Reference to next data in list. */
} EDRDataList;

/**
 * @struct EDRDevice
 * @brief Structure to maintain information of peer Bluetooth device.
 */
typedef struct
{
    char *remoteAddress;        /**< Address of peer Bluetooth device. */
    char *serviceUUID;          /**< OIC service UUID running in peer Bluetooth device. */
    int socketFD;           /**< RfComm connection socket FD. */
    EDRDataList *pendingDataList;/**< List of data needs to send to peer Bluetooth device. */
    bool serviceSearched;   /**< Flag to indicate the status of service search. */
} EDRDevice;

/**
 * @struct EDRDeviceList
 * @brief Structure to maintain list of peer Bluetooth device information.
 */
typedef struct _EDRDeviceList
{
    EDRDevice *device;            /**< Bluetooth device information. */
    struct _EDRDeviceList *next;  /**< Reference to next device information. */
} EDRDeviceList;

/**
 * @brief  Creates #EDRDevice for specified remote address and uuid and to device list.
 *
 * @param  deviceList       [IN,OUT] Device list which created device add to.
 * @param  deviceAddress    [IN]     Bluetooth device address.
 * @param  uuid             [IN]     Service uuid.
 * @param  device           [IN]     Created #EDRDevice.
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM Invalid input parameters
 * @retval #CA_STATUS_FAILED Failed to create device and add to list
 */
CAResult_t CACreateAndAddToDeviceList(EDRDeviceList **deviceList, const char *deviceAddress,
                                      const char *uuid, EDRDevice **device);

/**
 * @brief  Insert device to specified list.
 *
 * @param  deviceList [IN,OUT] Device list to which specifed @device to be added.
 * @param  device     [IN]     Device to be added to list.
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM Invalid input parameters
 * @retval #CA_MEMORY_ALLOC_FAILED Memory allocation failed
 */
CAResult_t CAAddEDRDeviceToList(EDRDeviceList **deviceList, EDRDevice *device);

/**
 * @brief  Get the device from list which matches specified device address.
 *
 * @param  deviceList    [IN]  Device list to search for the device.
 * @param  deviceAddress [IN]  Device address used for matching.
 * @param  device        [OUT] #EDRDevice which has matching device address.
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM Invalid input parameters
 * @retval #CA_STATUS_FAILED Device is not found in the list
 */
CAResult_t CAGetEDRDevice(EDRDeviceList *deviceList,
                           const char *deviceAddress, EDRDevice **device);

/**
 * @brief  Get the device from list which matches specified RFCOMM socket id.
 *
 * @param  deviceList [IN]  Device list to search for the device.
 * @param  socketID   [IN]  RFCOMM socket id.
 * @param  device     [OUT] #EDRDevice which has matching RFCOMM socket id .
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM Invalid input parameters
 * @retval #CA_STATUS_FAILED Device is not found in the list
 */
CAResult_t CAGetEDRDeviceBySocketId(EDRDeviceList *deviceList, int32_t socketID,
                                    EDRDevice **device);

/**
 * @brief  Remove and delete the device matching specified device address from list.
 *
 * @param  deviceList    [IN,OUT] Device list to search for the device.
 * @param  deviceAddress [IN]     Bluetooth device address.
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM Invalid input parameters
 * @retval #CA_STATUS_FAILED Device is not found in the list
 */
CAResult_t CARemoveEDRDeviceFromList(EDRDeviceList **deviceList,
                                    const char *deviceAddress);

/**
 * @brief  Destroy the specified device list. Removes and delete all the devices in the list.
 * @param  deviceList [IN,OUT] Device list to be destroyed.
 * @return NONE
 */
void CADestroyEDRDeviceList(EDRDeviceList **deviceList);

/**
 * @brief  Insert data to specified list.
 *
 * @param  dataList   [IN,OUT] Data list to which data will be add.
 * @param  data       [IN]      Data to be stored.
 * @param  dataLength [IN]      Length of the data.
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM Invalid input parameters
 * @retval #CA_MEMORY_ALLOC_FAILED Memory allocation failed
 */
CAResult_t CAAddEDRDataToList(EDRDataList **dataList, const void *data, uint32_t dataLength);

/**
 * @brief  Remove and delete data from front end of list.
 * @param  dataList [IN,OUT] Data list from which data will be removed.
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM Invalid input parameters
 */
CAResult_t CARemoveEDRDataFromList(EDRDataList **dataList);

/**
 * @brief  Destroy the specified data list. Removes and deletes all the data in the list.
 * @param  dataList [IN] Data list to be destroyed.
 * @return NONE
 */
void CADestroyEDRDataList(EDRDataList **dataList);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* CA_EDR_DEVICE_LIST_H_ */


