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
 * @file    cabtdevicelist.c
 * @brief    This file provides APIs to access the discovered bluetooth device list
 */

#include "cabtdevicelist.h"
#include "caadapterutils.h"
#include "cabtutils.h"
#include "logger.h"

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

CAResult_t CARemoveBTDeviceFromList(BTDeviceList **deviceList, const char *remoteAddress)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    VERIFY_NON_NULL(deviceList, BLUETOOTH_ADAPTER_TAG, "Device list is null");
    VERIFY_NON_NULL(remoteAddress, BLUETOOTH_ADAPTER_TAG, "Remote address is null");

    BTDeviceList *cur = NULL;
    BTDeviceList *prev = NULL;

    cur = *deviceList;
    while (cur != NULL)
    {
        if (!strcasecmp(cur->device->remoteAddress, remoteAddress))
        {
            if (cur == *deviceList)
            {
                *deviceList = cur->next;

                cur->next = NULL;
                CAFreeBTDeviceList(cur);
                return CA_STATUS_OK;
            }
            else
            {
                prev->next = cur->next;

                cur->next = NULL;
                CAFreeBTDeviceList(cur);
                return CA_STATUS_OK;
            }
        }
        else
        {
            prev = cur;
            cur = cur->next;
        }
    }

    OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Device not in the list !");
    return CA_STATUS_FAILED;
}

CAResult_t CAGetBTDevice(BTDeviceList *deviceList, const char *remoteAddress, BTDevice **device)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    VERIFY_NON_NULL(deviceList, BLUETOOTH_ADAPTER_TAG, "Device list is null");
    VERIFY_NON_NULL(remoteAddress, BLUETOOTH_ADAPTER_TAG, "Remote address is null");
    VERIFY_NON_NULL(device, BLUETOOTH_ADAPTER_TAG, "Device is null");

    BTDeviceList *cur = deviceList;
    *device = NULL;
    while (cur != NULL)
    {
        if (!strcasecmp(cur->device->remoteAddress, remoteAddress))
        {
            *device = cur->device;
            return CA_STATUS_OK;
        }

        cur = cur->next;
    }

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT [Device not found!]");
    return CA_STATUS_FAILED;
}

CAResult_t CAGetBTDeviceBySocketId(BTDeviceList *deviceList, int32_t socketFd, BTDevice **device)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    VERIFY_NON_NULL(deviceList, BLUETOOTH_ADAPTER_TAG, "Device list is null");
    VERIFY_NON_NULL(device, BLUETOOTH_ADAPTER_TAG, "Device is null");

    BTDeviceList *cur = deviceList;
    *device = NULL;
    while (cur != NULL)
    {
        if (cur->device->socketFD == socketFd)
        {
            *device = cur->device;
            return CA_STATUS_OK;
        }

        cur = cur->next;
    }

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
    return CA_STATUS_FAILED;
}

CAResult_t CACreateBTDevice(const char *remoteAddress, const char *uuid, BTDevice **device)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    VERIFY_NON_NULL(remoteAddress, BLUETOOTH_ADAPTER_TAG, "Remote address is null");
    VERIFY_NON_NULL(uuid, BLUETOOTH_ADAPTER_TAG, "uuid is null");
    VERIFY_NON_NULL(device, BLUETOOTH_ADAPTER_TAG, "Device is null");

    *device = (BTDevice *) OICMalloc(sizeof(BTDevice));
    if (NULL == *device)
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Out of memory (device)!");
        return CA_MEMORY_ALLOC_FAILED;
    }

    //Copy bluetooth address
    if (remoteAddress && strlen(remoteAddress))
    {
        (*device)->remoteAddress = strndup(remoteAddress, strlen(remoteAddress));
        if (NULL == (*device)->remoteAddress)
        {
            OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Out of memory (remote address)!");

            OICFree(*device);
            *device = NULL;
            return CA_MEMORY_ALLOC_FAILED;
        }
    }

    //Copy OIC service uuid
    if (uuid && strlen(uuid))
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

CAResult_t CACreateAndAddToDeviceList(BTDeviceList **devicList, const char *remoteAddress,
                                      const char *serviceUUID, BTDevice **device)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    VERIFY_NON_NULL(devicList, BLUETOOTH_ADAPTER_TAG, "Device list is null");
    VERIFY_NON_NULL(remoteAddress, BLUETOOTH_ADAPTER_TAG, "Remote address is null");
    VERIFY_NON_NULL(device, BLUETOOTH_ADAPTER_TAG, "Device is null");

    if (CA_STATUS_OK != CACreateBTDevice(remoteAddress, serviceUUID, device) || NULL == *device)
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Invalid or Not bonded device!");
        return CA_STATUS_FAILED;
    }

    if (CA_STATUS_OK != CAAddBTDeviceToList(devicList, *device))
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Failed to add in list!");

        //Remove created BTDevice
        CAFreeBTDevice(*device);
        *device = NULL;

        return CA_STATUS_FAILED;
    }

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CAFreeBTDeviceList(BTDeviceList *deviceList)
{
    while (deviceList)
    {
        BTDeviceList *curList = deviceList;
        deviceList = deviceList->next;

        CAFreeBTDevice(curList->device);
        OICFree(curList);
    }
}

void CAFreeBTDevice(BTDevice *device)
{
    if (device)
    {
        if (device->remoteAddress)
        {
            OICFree(device->remoteAddress);
        }

        if (device->serviceUUID)
        {
            OICFree(device->serviceUUID);
        }

        if (device->pendingDataList)
        {
            CARemoveAllDataFromDevicePendingList(&device->pendingDataList);
        }

        OICFree(device);
    }
}

CAResult_t CAAddDataToDevicePendingList(BTDataList **dataList, void *data, uint32_t dataLen)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    VERIFY_NON_NULL(dataList, BLUETOOTH_ADAPTER_TAG, "Data list is null");
    VERIFY_NON_NULL(data, BLUETOOTH_ADAPTER_TAG, "Data is null");

    if (0 == dataLen)
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

    pending_data->data->data = (void *) OICMalloc(dataLen); //data
    if (NULL == pending_data->data->data)
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "OICMalloc failed (data)!");

        OICFree(pending_data->data);
        OICFree(pending_data);
        return CA_MEMORY_ALLOC_FAILED;
    }

    memcpy(pending_data->data->data, data, dataLen);
    pending_data->data->dataLength = dataLen;

    if (NULL == *dataList) //Empty list
    {
        *dataList = pending_data;
    }
    else //Add at rear end
    {
        BTDataList *curList = *dataList;
        while (curList->next != NULL)
        {
            curList = curList->next;
        }

        curList->next = pending_data;
    }

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CARemoveDataFromDevicePendingList(BTDataList **dataList)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    VERIFY_NON_NULL(dataList, BLUETOOTH_ADAPTER_TAG, "Data list is null");

    BTDataList *cur = NULL;
    BTDataList *prev = NULL;

    cur = *dataList;
    if (NULL != cur->next) //next node present in list
    {
        prev = cur;
        cur = cur->next;
        CAFreeDataFromBTDataList(prev);
        *dataList = cur;
    }
    else //last node
    {
        CAFreeDataFromBTDataList(cur);
        *dataList = NULL;
    }

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CARemoveAllDataFromDevicePendingList(BTDataList **dataList)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    VERIFY_NON_NULL(dataList, BLUETOOTH_ADAPTER_TAG, "Data list is null");

    BTDataList *cur = NULL;
    BTDataList *prev = NULL;

    cur = *dataList;
    while (cur)
    {
        prev = cur;
        cur = cur->next;
        CAFreeDataFromBTDataList(prev);
    }

    *dataList = NULL;

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CAFreeDataFromBTDataList(BTDataList *dataList)
{
    if (dataList)
    {
        if (dataList->data)
        {
            if (dataList->data->data)
            {
                OICFree(dataList->data->data);
            }
            OICFree(dataList->data);
        }
        OICFree(dataList);
        dataList = NULL;
    }
}

