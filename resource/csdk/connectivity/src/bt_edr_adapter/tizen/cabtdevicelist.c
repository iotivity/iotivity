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
 * @file  cabtdevicelist.c
 * @brief  This file provides APIs to access the discovered bluetooth device list
 */

#include "cabtdevicelist.h"
#include "caadapterutils.h"
#include "cabtutils.h"
#include "logger.h"


/**
 * @fn  CACreateBTDevice
 * @brief  Creates #BTDevice for specified remote address and uuid.
 *
 */
static CAResult_t CACreateBTDevice(const char *deviceAddress, const char *uuid, BTDevice **device);


/**
 * @fn  CADestroyBTDevice
 * @brief  Free all the memory associated with specified device.
 *
 */
static void CADestroyBTDevice(BTDevice *device);


/**
 * @fn  CADestroyBTData
 * @brief  Free all the memory associated with specified data.
 *
 */
static void CADestroyBTData(BTData *data);


CAResult_t CACreateAndAddToDeviceList(BTDeviceList **deviceList, const char *deviceAddress,
        const char *uuid, BTDevice **device)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    VERIFY_NON_NULL(deviceList, BLUETOOTH_ADAPTER_TAG, "Device list is null");
    VERIFY_NON_NULL(deviceAddress, BLUETOOTH_ADAPTER_TAG, "Remote address is null");
    VERIFY_NON_NULL(device, BLUETOOTH_ADAPTER_TAG, "Device is null");

    if (CA_STATUS_OK != CACreateBTDevice(deviceAddress, uuid, device) || NULL == *device)
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Invalid or Not bonded device!");
        return CA_STATUS_FAILED;
    }

    if (CA_STATUS_OK != CAAddBTDeviceToList(deviceList, *device))
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Failed to add in list!");

        //Remove created BTDevice
        CADestroyBTDevice(*device);
        *device = NULL;

        return CA_STATUS_FAILED;
    }

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CACreateBTDevice(const char *deviceAddress, const char *uuid, BTDevice **device)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    VERIFY_NON_NULL(deviceAddress, BLUETOOTH_ADAPTER_TAG, "Remote address is null");
    VERIFY_NON_NULL(uuid, BLUETOOTH_ADAPTER_TAG, "uuid is null");
    VERIFY_NON_NULL(device, BLUETOOTH_ADAPTER_TAG, "Device is null");

    *device = (BTDevice *) OICMalloc(sizeof(BTDevice));
    if (NULL == *device)
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Out of memory (device)!");
        return CA_MEMORY_ALLOC_FAILED;
    }

    //Copy bluetooth address
    if (strlen(deviceAddress))
    {
        (*device)->remoteAddress = strndup(deviceAddress, strlen(deviceAddress));
        if (NULL == (*device)->remoteAddress)
        {
            OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Out of memory (remote address)!");

            OICFree(*device);
            *device = NULL;
            return CA_MEMORY_ALLOC_FAILED;
        }
    }

    //Copy OIC service uuid
    if (strlen(uuid))
    {
        (*device)->serviceUUID = strndup(uuid, strlen(uuid));
        if (NULL == (*device)->serviceUUID)
        {
            OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG,
                    "[createBTDevice] Out of memory (service uuid)!");

            OICFree((*device)->remoteAddress);
            OICFree(*device);
            *device = NULL;
            return CA_MEMORY_ALLOC_FAILED;
        }
    }

    (*device)->socketFD = -1;
    (*device)->pendingDataList = NULL;
    (*device)->serviceSearched = 0;

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAAddBTDeviceToList(BTDeviceList **deviceList, BTDevice *device)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    VERIFY_NON_NULL(deviceList, BLUETOOTH_ADAPTER_TAG, "Device list is null");
    VERIFY_NON_NULL(device, BLUETOOTH_ADAPTER_TAG, "Device is null");

    BTDeviceList *node = (BTDeviceList *) OICMalloc(sizeof(BTDeviceList));
    if (NULL == node)
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Out of memory (device list)!");
        return CA_MEMORY_ALLOC_FAILED;
    }

    node->device = device;
    node->next = NULL;

    if (NULL == *deviceList) //Empty list
    {
        *deviceList = node;
    }
    else //Add at front end
    {
        node->next = *deviceList;
        *deviceList = node;
    }

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAGetBTDevice(BTDeviceList *deviceList, const char *deviceAddress, BTDevice **device)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    VERIFY_NON_NULL(deviceList, BLUETOOTH_ADAPTER_TAG, "Device list is null");
    VERIFY_NON_NULL(deviceAddress, BLUETOOTH_ADAPTER_TAG, "Remote address is null");
    VERIFY_NON_NULL(device, BLUETOOTH_ADAPTER_TAG, "Device is null");

    BTDeviceList *curNode = deviceList;
    *device = NULL;
    while (curNode != NULL)
    {
        if (!strcasecmp(curNode->device->remoteAddress, deviceAddress))
        {
            *device = curNode->device;
            return CA_STATUS_OK;
        }

        curNode = curNode->next;
    }

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT [Device not found!]");
    return CA_STATUS_FAILED;
}

CAResult_t CAGetBTDeviceBySocketId(BTDeviceList *deviceList, int32_t socketID, BTDevice **device)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    VERIFY_NON_NULL(deviceList, BLUETOOTH_ADAPTER_TAG, "Device list is null");
    VERIFY_NON_NULL(device, BLUETOOTH_ADAPTER_TAG, "Device is null");
    BTDeviceList *curNode = deviceList;
    *device = NULL;
    while (curNode != NULL)
    {
        if (curNode->device->socketFD == socketID)
        {
            *device = curNode->device;
            return CA_STATUS_OK;
        }

        curNode = curNode->next;
    }

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
    return CA_STATUS_FAILED;
}

CAResult_t CARemoveBTDeviceFromList(BTDeviceList **deviceList, const char *deviceAddress)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    VERIFY_NON_NULL(deviceList, BLUETOOTH_ADAPTER_TAG, "Device list is null");
    VERIFY_NON_NULL(deviceAddress, BLUETOOTH_ADAPTER_TAG, "Remote address is null");

    BTDeviceList *curNode = NULL;
    BTDeviceList *prevNode = NULL;

    curNode = *deviceList;
    while (curNode != NULL)
    {
        if (!strcasecmp(curNode->device->remoteAddress, deviceAddress))
        {
            if (curNode == *deviceList)
            {
                *deviceList = curNode->next;

                curNode->next = NULL;
                CADestroyBTDeviceList(&curNode);
                return CA_STATUS_OK;
            }
            else
            {
                prevNode->next = curNode->next;

                curNode->next = NULL;
                CADestroyBTDeviceList(&curNode);
                return CA_STATUS_OK;
            }
        }
        else
        {
            prevNode = curNode;
            curNode = curNode->next;
        }
    }

    OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Device not in the list !");
    return CA_STATUS_FAILED;
}

void CADestroyBTDeviceList(BTDeviceList **deviceList)
{
    while (*deviceList)
    {
        BTDeviceList *curNode = *deviceList;
        *deviceList = (*deviceList)->next;

        CADestroyBTDevice(curNode->device);
        OICFree(curNode);
    }
}

void CADestroyBTDevice(BTDevice *device)
{
    if (device)
    {
        OICFree(device->remoteAddress);
        OICFree(device->serviceUUID);
        CADestroyBTDataList(&device->pendingDataList);
        OICFree(device);
    }
}

CAResult_t CAAddBTDataToList(BTDataList **dataList, void *data, uint32_t dataLength)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    VERIFY_NON_NULL(dataList, BLUETOOTH_ADAPTER_TAG, "Data list is null");
    VERIFY_NON_NULL(data, BLUETOOTH_ADAPTER_TAG, "Data is null");

    if (0 == dataLength)
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Invalid input: data length is zero!");
        return CA_STATUS_INVALID_PARAM;
    }

    BTDataList *pending_data = (BTDataList *) OICMalloc(sizeof(BTDataList));
    if (NULL == pending_data)
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "OICMalloc failed (data list)!");
        return CA_MEMORY_ALLOC_FAILED;
    }

    pending_data->data = (BTData *) OICMalloc(sizeof(BTData));
    if (NULL == pending_data->data)
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "OICMalloc failed (data node)!");

        OICFree(pending_data);
        return CA_MEMORY_ALLOC_FAILED;
    }
    pending_data->next = NULL;

    pending_data->data->data = (void *) OICMalloc(dataLength); //data
    if (NULL == pending_data->data->data)
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "OICMalloc failed (data)!");

        OICFree(pending_data->data);
        OICFree(pending_data);
        return CA_MEMORY_ALLOC_FAILED;
    }

    memcpy(pending_data->data->data, data, dataLength);
    pending_data->data->dataLength = dataLength;

    if (NULL == *dataList) //Empty list
    {
        *dataList = pending_data;
    }
    else //Add at rear end
    {
        BTDataList *curNode = *dataList;
        while (curNode->next != NULL)
        {
            curNode = curNode->next;
        }

        curNode->next = pending_data;
    }

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CARemoveBTDataFromList(BTDataList **dataList)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    VERIFY_NON_NULL(dataList, BLUETOOTH_ADAPTER_TAG, "Data list is null");

    if (*dataList)
    {
        BTDataList *curNode = *dataList;
        *dataList = (*dataList)->next;

        //Delete the first node
        CADestroyBTData(curNode->data);
        OICFree(curNode);
    }

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CADestroyBTDataList(BTDataList **dataList)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    while (*dataList)
    {
        BTDataList *curNode = *dataList;
        *dataList = (*dataList)->next;
        
        CADestroyBTData(curNode->data);
        OICFree(curNode);
    }

    *dataList = NULL;

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
}

void CADestroyBTData(BTData *data)
{
    if (data)
    {
        OICFree(data->data);
        OICFree(data);
    }
}

