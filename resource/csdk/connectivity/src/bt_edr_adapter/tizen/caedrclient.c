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
 * @file  cabtclient.c
 * @brief  This file provides the APIs to establish RFCOMM connection with remote bluetooth device
 */
#include <string.h>
#include <bluetooth.h>

#include "caedrinterface.h"
#include "umutex.h"
#include "caedrendpoint.h"
#include "caadapterutils.h"
#include "caedrutils.h"
#include "logger.h"
#include "cacommon.h"
#include "caedrdevicelist.h"

/**
 * @var gEDRDeviceListMutex
 * @brief Mutex to synchronize the access to Bluetooth device information list.
 */
static u_mutex gEDRDeviceListMutex = NULL;

/**
 * @var gEDRDeviceList
 * @brief Peer Bluetooth device information list.
 */
static EDRDeviceList *gEDRDeviceList = NULL;

/**
 * @var gEDRNetworkChangeCallback
 * @brief Maintains the callback to be notified when data received from remote Bluetooth device
 */
static CAEDRDataReceivedCallback gEDRPacketReceivedCallback = NULL;

/**
 * @fn CAEDRManagerInitializeMutex
 * @brief This function creates mutex.
 */
static void CAEDRManagerInitializeMutex(void);

/**
 * @fn CAEDRManagerTerminateMutex
 * @brief This function frees mutex.
 */
static void CAEDRManagerTerminateMutex(void);

/**
 * @fn CAEDRDataRecvCallback
 * @brief This callback is registered to recieve data on any open RFCOMM connection.
 */
static void CAEDRDataRecvCallback(bt_socket_received_data_s *data, void *userData);

/**
 * @brief This function starts device discovery.
 * @return NONE
 */
static CAResult_t CAEDRStartDeviceDiscovery(void);

/**
 * @fn CAEDRStopServiceSearch
 * @brief This function stops any ongoing service sevice search.
 */
static CAResult_t CAEDRStopServiceSearch(void);

/**
 * @fn CAEDRStopDeviceDiscovery
 * @brief This function stops device discovery.
 */
static CAResult_t CAEDRStopDeviceDiscovery(void);

/**
 * @fn CAEDRStartServiceSearch
 * @brief This function searches for OIC service for remote Bluetooth device.
 */
static CAResult_t CAEDRStartServiceSearch(const char *remoteAddress);

/**
 * @fn CAEDRDeviceDiscoveryCallback
 * @brief This callback is registered to recieve all bluetooth nearby devices when device
 *           scan is initiated.
 */
static void CAEDRDeviceDiscoveryCallback(int result,
                                         bt_adapter_device_discovery_state_e state,
                                         bt_adapter_device_discovery_info_s *discoveryInfo,
                                         void *userData);

/**
 * @fn CAEDRServiceSearchedCallback
 * @brief This callback is registered to recieve all the services remote bluetooth device supports
 *           when service search initiated.
 */
static void CAEDRServiceSearchedCallback(int result, bt_device_sdp_info_s *sdpInfo,
                                        void *userData);

/**
 * @fn CAEDRSocketConnectionStateCallback
 * @brief This callback is registered to receive bluetooth RFCOMM connection state changes.
 */
static void CAEDRSocketConnectionStateCallback(int result, bt_socket_connection_state_e state,
                                              bt_socket_connection_s *connection, void *userData);

/**
 * @fn CAEDRClientConnect
 * @brief Establishes RFCOMM connection with remote bluetooth device
 */
static CAResult_t CAEDRClientConnect(const char *remoteAddress, const char *serviceUUID);

/**
 * @fn CAEDRClientDisconnect
 * @brief  Disconnect RFCOMM client socket connection
 */
static CAResult_t CAEDRClientDisconnect(const int32_t clientID);

void CAEDRSetPacketReceivedCallback(CAEDRDataReceivedCallback packetReceivedCallback)
{
    gEDRPacketReceivedCallback = packetReceivedCallback;
}

void CAEDRSocketConnectionStateCallback(int result, bt_socket_connection_state_e state,
                                       bt_socket_connection_s *connection, void *userData)
{
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN");

    EDRDevice *device = NULL;

    if (BT_ERROR_NONE != result || NULL == connection)
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Invalid connection state!, error num [%x]",
                  result);
        return;
    }

    switch (state)
    {
        case BT_SOCKET_CONNECTED:
            {
                u_mutex_lock(gEDRDeviceListMutex);
                if (CA_STATUS_OK != CAGetEDRDevice(gEDRDeviceList, connection->remote_address,
                                                  &device))
                {
                    // Create the deviceinfo and add to list
                    if (CA_STATUS_OK != CACreateAndAddToDeviceList(&gEDRDeviceList,
                            connection->remote_address, OIC_EDR_SERVICE_ID, &device))
                    {
                        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Failed add device to list!");
                        u_mutex_unlock(gEDRDeviceListMutex);
                        return;
                    }

                    device->socketFD = connection->socket_fd;
                    u_mutex_unlock(gEDRDeviceListMutex);
                    return;
                }

                device->socketFD = connection->socket_fd;
                while (device->pendingDataList)
                {
                    uint32_t sentData = 0;
                    EDRData *edrData = device->pendingDataList->data;
                    if (CA_STATUS_OK != CAEDRSendData(device->socketFD, edrData->data,
                                                     edrData->dataLength, &sentData))
                    {
                        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Failed to send pending data [%s]",
                                  device->remoteAddress);

                        // Remove all the data from pending list
                        CADestroyEDRDataList(&device->pendingDataList);
                        break;
                    }

                    // Remove the data which send from pending list
                    CARemoveEDRDataFromList(&device->pendingDataList);
                }
                u_mutex_unlock(gEDRDeviceListMutex);
            }
            break;

        case BT_SOCKET_DISCONNECTED:
            {
                u_mutex_lock(gEDRDeviceListMutex);
                CARemoveEDRDeviceFromList(&gEDRDeviceList, connection->remote_address);
                u_mutex_unlock(gEDRDeviceListMutex);
            }
            break;
    }

    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "OUT");
}


void CAEDRDeviceDiscoveryCallback(int result, bt_adapter_device_discovery_state_e state,
                                 bt_adapter_device_discovery_info_s *discoveryInfo, void *userData)
{
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN");

    EDRDevice *device = NULL;

    if (BT_ERROR_NONE != result)
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Received bad state!, error num [%x]",
                  result);
        return;
    }

    switch (state)
    {
        case BT_ADAPTER_DEVICE_DISCOVERY_STARTED:
            {
                OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "Discovery started!");
            }
            break;

        case BT_ADAPTER_DEVICE_DISCOVERY_FINISHED:
            {
                OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "Discovery finished!");
            }
            break;

        case BT_ADAPTER_DEVICE_DISCOVERY_FOUND:
            {
                OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "Device discovered [%s]!",
                          discoveryInfo->remote_name);
                if (CA_TRUE == CAEDRIsServiceSupported((const char **)discoveryInfo->service_uuid,
                                                        discoveryInfo->service_count,
                                                        OIC_EDR_SERVICE_ID))
                {
                    // Check if the deivce is already in the list
                    u_mutex_lock(gEDRDeviceListMutex);
                    if (CA_STATUS_OK == CAGetEDRDevice(gEDRDeviceList, discoveryInfo->remote_address,
                                                       &device))
                    {
                        device->serviceSearched = 1;
                        u_mutex_unlock(gEDRDeviceListMutex);
                        return;
                    }

                    // Create the deviceinfo and add to list
                    if (CA_STATUS_OK != CACreateAndAddToDeviceList(&gEDRDeviceList,
                            discoveryInfo->remote_address, OIC_EDR_SERVICE_ID, &device))
                    {
                        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Failed to add device to list!");
                        u_mutex_unlock(gEDRDeviceListMutex);
                        return;
                    }

                    device->serviceSearched = 1;
                    u_mutex_unlock(gEDRDeviceListMutex);
                }
                else
                {
                    OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Device does not support OIC service!");
                }
            }
            break;
    }

    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "OUT");
}

void CAEDRServiceSearchedCallback(int32_t result, bt_device_sdp_info_s *sdpInfo, void *userData)
{
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN");

    if (NULL == sdpInfo)
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "SDP info is null!");
        return;
    }

    u_mutex_lock(gEDRDeviceListMutex);

    EDRDevice *device = NULL;
    if (CA_STATUS_OK == CAGetEDRDevice(gEDRDeviceList, sdpInfo->remote_address, &device)
        && NULL != device)
    {
        if (1 == device->serviceSearched)
        {
            OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "Service is already searched for this device!");
            u_mutex_unlock(gEDRDeviceListMutex);
            return;
        }

        if (CA_TRUE == CAEDRIsServiceSupported((const char **)sdpInfo->service_uuid,
                                           sdpInfo->service_count, OIC_EDR_SERVICE_ID))
        {
            device->serviceSearched = 1;
            if (CA_STATUS_OK != CAEDRClientConnect(sdpInfo->remote_address, OIC_EDR_SERVICE_ID))
            {
                OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Failed to make rfcomm connection!");

                // Remove the device from device list
                CARemoveEDRDeviceFromList(&gEDRDeviceList, sdpInfo->remote_address);
            }
        }
        else
        {
            OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "Device does not contain OIC service!");

            // Remove device from list as it does not support OIC service
            CARemoveEDRDeviceFromList(&gEDRDeviceList, sdpInfo->remote_address);
        }
    }

    u_mutex_unlock(gEDRDeviceListMutex);

    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "OUT");
}

CAResult_t CAEDRStartDeviceDiscovery(void)
{
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN");

    bt_error_e err = BT_ERROR_NONE;
    bool isDiscoveryStarted = false;

    // Check the device discovery state
    if (BT_ERROR_NONE != (err = bt_adapter_is_discovering(&isDiscoveryStarted)))
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Failed to get discovery state!, error num [%x]",
                  err);
        return CA_STATUS_FAILED;
    }

    //Start device discovery if its not started
    if (false == isDiscoveryStarted)
    {
        if (BT_ERROR_NONE != (err = bt_adapter_start_device_discovery()))
        {
            OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Device discovery failed!, error num [%x]",
                      err);
            return CA_STATUS_FAILED;
        }
    }

    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAEDRStopServiceSearch(void)
{
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN");

    bt_error_e err = BT_ERROR_NONE;

    // Stop ongoing service search
    if (BT_ERROR_NONE != (err = bt_device_cancel_service_search()))
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Get bonded device failed!, error num [%x]",
                  err);
        return CA_STATUS_FAILED;
    }

    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAEDRStopDeviceDiscovery(void)
{
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN");

    bt_error_e err = BT_ERROR_NONE;
    bool isDiscoveryStarted = false;

    // Check the device discovery state
    if (BT_ERROR_NONE != (err = bt_adapter_is_discovering(&isDiscoveryStarted)))
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Failed to get discovery state!, error num [%x]",
                  err);
        return CA_STATUS_FAILED;
    }

    //stop the device discovery process
    if (true == isDiscoveryStarted)
    {
        OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "Stopping the device search process");
        if (BT_ERROR_NONE != (err = bt_adapter_stop_device_discovery()))
        {
            OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Failed to stop discovery!, error num [%x]",
                      err);
        }
    }

    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAEDRStartServiceSearch(const char *remoteAddress)
{
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN");

    bt_error_e err = BT_ERROR_NONE;

    // Input validation
    VERIFY_NON_NULL(remoteAddress, EDR_ADAPTER_TAG, "Remote address is null");
    if (0 == strlen(remoteAddress))
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Remote address is empty!");
        return CA_STATUS_INVALID_PARAM;
    }

    // Start searching for OIC service
    if (BT_ERROR_NONE != (err = bt_device_start_service_search(remoteAddress)))
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Get bonded device failed!, error num [%x]",
                  err);
        return CA_STATUS_FAILED;
    }

    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAEDRClientSetCallbacks(void)
{
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN");

    // Register for discovery and rfcomm socket connection callbacks
    bt_adapter_set_device_discovery_state_changed_cb(CAEDRDeviceDiscoveryCallback, NULL);
    bt_device_set_service_searched_cb(CAEDRServiceSearchedCallback, NULL);
    bt_socket_set_connection_state_changed_cb(CAEDRSocketConnectionStateCallback, NULL);
    bt_socket_set_data_received_cb(CAEDRDataRecvCallback, NULL);

    // Start device discovery
    if( CA_STATUS_OK != CAEDRStartDeviceDiscovery())
    {
        OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "Failed to Start Device discovery");
        return CA_STATUS_FAILED;
    }

    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}


void CAEDRClientUnsetCallbacks(void)
{
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN");

    // Stop service search
    CAEDRStopServiceSearch();

    // Stop the device discovery process
    CAEDRStopDeviceDiscovery();

    // reset bluetooth adapter callbacks
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "Resetting the callbacks");
    bt_adapter_unset_device_discovery_state_changed_cb();
    bt_device_unset_service_searched_cb();
    bt_socket_unset_connection_state_changed_cb();
    bt_socket_unset_data_received_cb();

    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "OUT");
}

void CAEDRManagerInitializeMutex(void)
{
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN");

    u_mutex_init();

    if (!gEDRDeviceListMutex)
    {
        gEDRDeviceListMutex = u_mutex_new();
    }

    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "OUT");
}

void CAEDRManagerTerminateMutex(void)
{
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN");

    if (gEDRDeviceListMutex)
    {
        u_mutex_free(gEDRDeviceListMutex);
        gEDRDeviceListMutex = NULL;
    }

    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "OUT");
}

void CAEDRInitializeClient(u_thread_pool_t handle)
{
    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "IN");
    CAEDRManagerInitializeMutex();
    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "OUT");
}

void CAEDRTerminateClient()
{
    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "IN");

    // Free EDRDevices list
    if (gEDRDeviceListMutex)
    {
        u_mutex_lock(gEDRDeviceListMutex);
        CADestroyEDRDeviceList(&gEDRDeviceList);
        u_mutex_unlock(gEDRDeviceListMutex);
    }

    // Free the mutex
    CAEDRManagerTerminateMutex();
    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "OUT");
}

void CAEDRClientDisconnectAll(void)
{
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN");

    u_mutex_lock(gEDRDeviceListMutex);

    EDRDeviceList *cur = gEDRDeviceList;
    while (cur != NULL)
    {
        EDRDevice *device = cur->device;
        cur = cur->next;

        if (device && 0 <= device->socketFD)
        {
            if (CA_STATUS_OK != CAEDRClientDisconnect(device->socketFD))
            {
                OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Failed to disconnect with client :%s",
                          device->remoteAddress);
            }

            device->socketFD = -1;
        }
    }

    u_mutex_unlock(gEDRDeviceListMutex);

    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "OUT");
}


CAResult_t CAEDRClientSendUnicastData(const char *remoteAddress, const char *serviceUUID,
                                      void *data, uint32_t dataLength, uint32_t *sentLength)
{
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN");

    EDRDevice *device = NULL;

    // Input validation
    VERIFY_NON_NULL(remoteAddress, EDR_ADAPTER_TAG, "Remote address is null");
    VERIFY_NON_NULL(serviceUUID, EDR_ADAPTER_TAG, "service UUID is null");
    VERIFY_NON_NULL(data, EDR_ADAPTER_TAG, "Data is null");
    VERIFY_NON_NULL(sentLength, EDR_ADAPTER_TAG, "Sent data length holder is null");

    if (0 >= dataLength)
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Invalid input: Negative data length!");
        return CA_STATUS_INVALID_PARAM;
    }

    // Check the connection existence with remote device
    u_mutex_lock(gEDRDeviceListMutex);
    if (CA_STATUS_OK != CAGetEDRDevice(gEDRDeviceList, remoteAddress, &device))
    {
        // Create new device and add to list
        if (CA_STATUS_OK != CACreateAndAddToDeviceList(&gEDRDeviceList, remoteAddress,
                OIC_EDR_SERVICE_ID, &device))
        {
            OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Failed create device and add to list!");

            u_mutex_unlock(gEDRDeviceListMutex);
            return CA_STATUS_FAILED;
        }

        // Start the OIC service search newly created device
        if (CA_STATUS_OK != CAEDRStartServiceSearch(remoteAddress))
        {
            OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Failed to initiate service search!");

            // Remove device from list
            CARemoveEDRDeviceFromList(&gEDRDeviceList, remoteAddress);

            u_mutex_unlock(gEDRDeviceListMutex);
            return CA_STATUS_FAILED;
        }
    }
    u_mutex_unlock(gEDRDeviceListMutex);

    if (-1 == device->socketFD)
    {
        // Adding to pending list
        if (CA_STATUS_OK != CAAddEDRDataToList(&device->pendingDataList, data,
                                              dataLength))
        {
            OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Failed to add data to pending list!");

            //Remove device from list
            CARemoveEDRDeviceFromList(&gEDRDeviceList, remoteAddress);
            return CA_STATUS_FAILED;
        }

        // Make a rfcomm connection with remote BT Device
        if (1 == device->serviceSearched &&
            CA_STATUS_OK != CAEDRClientConnect(remoteAddress, serviceUUID))
        {
            OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Failed to make RFCOMM connection!");

            //Remove device from list
            CARemoveEDRDeviceFromList(&gEDRDeviceList, remoteAddress);
            return CA_STATUS_FAILED;
        }
        *sentLength = dataLength;
    }
    else
    {
        if (CA_STATUS_OK != CAEDRSendData(device->socketFD, data, dataLength, sentLength))
        {
            OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Failed to send data!");
            return CA_STATUS_FAILED;
        }
    }

    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAEDRClientSendMulticastData(const char *serviceUUID, void *data,
                                        uint32_t dataLength, uint32_t *sentLength)
{
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN");

    // Input validation
    VERIFY_NON_NULL(serviceUUID, EDR_ADAPTER_TAG, "service UUID is null");
    VERIFY_NON_NULL(data, EDR_ADAPTER_TAG, "Data is null");
    VERIFY_NON_NULL(sentLength, EDR_ADAPTER_TAG, "Sent data length holder is null");

    if (0 >= dataLength)
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Invalid input: Negative data length!");
        return CA_STATUS_INVALID_PARAM;
    }

    *sentLength = dataLength;

    // Send the packet to all OIC devices
    u_mutex_lock(gEDRDeviceListMutex);
    EDRDeviceList *curList = gEDRDeviceList;
    while (curList != NULL)
    {
        EDRDevice *device = curList->device;
        curList = curList->next;

        if (!device)
        {
            OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "There is no device!");
            break;
        }

        if (-1 == device->socketFD)
        {
            OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN1");
            // Check if the device service search is finished
            if (0 == device->serviceSearched)
            {
                OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Device services are still unknown!");
                continue;
            }

            // Adding to pendding list
            if (CA_STATUS_OK != CAAddEDRDataToList(&device->pendingDataList, data,
                                                  dataLength))
            {
                OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Failed to add data to pending list !");
                continue;
            }

            // Make a rfcomm connection with remote BT Device
            if (CA_STATUS_OK != CAEDRClientConnect(device->remoteAddress, device->serviceUUID))
            {
                OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Failed to make RFCOMM connection !");

                //Remove the data which added to pending list
                CARemoveEDRDataFromList(&device->pendingDataList);
                continue;
            }
            OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN2");
        }
        else
        {
            OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN3");
            if (CA_STATUS_OK != CAEDRSendData(device->socketFD, data, dataLength, sentLength))
            {
                OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Failed to send data to [%s] !",
                          device->remoteAddress);
            }
            OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN4");
        }
    }
    u_mutex_unlock(gEDRDeviceListMutex);

    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAEDRClientConnect(const char *remoteAddress, const char *serviceUUID)
{
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN");

    bt_error_e err = BT_ERROR_NONE;
    int32_t addressLen = 0;

    VERIFY_NON_NULL(remoteAddress, EDR_ADAPTER_TAG, "Remote address is null");
    VERIFY_NON_NULL(serviceUUID, EDR_ADAPTER_TAG, "Service UUID is null");

    addressLen = strlen(remoteAddress);
    if (0 == addressLen || CA_MACADDR_SIZE - 1 != addressLen)
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Invalid input: Invalid remote address");
        return  CA_STATUS_INVALID_PARAM;
    }

    if (0 == strlen(serviceUUID))
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Invalid input: Empty service uuid");
        return  CA_STATUS_INVALID_PARAM;
    }

    if (BT_ERROR_NONE != (err = bt_socket_connect_rfcomm(remoteAddress, serviceUUID)))
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG,
                  "Failed to connect!, address [%s] error num [%x]",
                  remoteAddress, err);
        return CA_STATUS_FAILED;
    }

    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAEDRClientDisconnect(const int32_t clientID)
{
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN");

    // Input validation
    if (0 > clientID)
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Invalid input: negative client id");
        return CA_STATUS_INVALID_PARAM;
    }

    bt_error_e err = BT_ERROR_NONE;
    if (BT_ERROR_NONE != (err = bt_socket_disconnect_rfcomm(clientID)))
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Failed close rfcomm client socket!, error num [%x]",
                  err);
        return CA_STATUS_FAILED;
    }

    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CAEDRDataRecvCallback(bt_socket_received_data_s *data, void *userData)
{
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN");

    EDRDevice *device = NULL;

    if (NULL == data || 0 >= data->data_size)
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Data is null!");
        return;
    }

    // Get EDR device from list
    u_mutex_lock(gEDRDeviceListMutex);
    if (CA_STATUS_OK != CAGetEDRDeviceBySocketId(gEDRDeviceList, data->socket_fd, &device))
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Could not find the device!");

        u_mutex_unlock(gEDRDeviceListMutex);
        return;
    }
    u_mutex_unlock(gEDRDeviceListMutex);

    uint32_t sentLength = 0;

    gEDRPacketReceivedCallback(device->remoteAddress, data->data,
                                (uint32_t)data->data_size, &sentLength);

    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "OUT");
}

