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

#include "caleutil.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>


#include "caadapterutils.h"
#include "oic_string.h"
#include "oic_malloc.h"

/**
 * Logging tag for module name
 */
#define TAG "OIC_CA_LE_UTIL"

CAResult_t CAAddLEDataToList(LEDataList **dataList, const void *data, uint32_t dataLength)
{
    OIC_LOG(DEBUG, TAG, "IN");

    VERIFY_NON_NULL(dataList, TAG, "Data list is null");
    VERIFY_NON_NULL(data, TAG, "data is null");

    if (0 == dataLength)
    {
        OIC_LOG(ERROR, TAG, "Invalid input: data length is zero!");
        return CA_STATUS_INVALID_PARAM;
    }

    LEDataList *pending_data = (LEDataList *) OICMalloc(sizeof(LEDataList));
    if (NULL == pending_data)
    {
        OIC_LOG(ERROR, TAG, "OICMalloc failed (data list)!");
        return CA_MEMORY_ALLOC_FAILED;
    }

    pending_data->data = (LEData *) OICMalloc(sizeof(LEData));
    if (NULL == pending_data->data)
    {
        OIC_LOG(ERROR, TAG, "OICMalloc failed (data node)!");
        OICFree(pending_data);
        return CA_MEMORY_ALLOC_FAILED;
    }

    pending_data->next = NULL;
    pending_data->data->data = (void *) OICMalloc(dataLength); //data
    if (NULL == pending_data->data->data)
    {
        OIC_LOG(ERROR, TAG, "OICMalloc failed (data)!");
        OICFree(pending_data->data);
        OICFree(pending_data);
        return CA_MEMORY_ALLOC_FAILED;
    }

    memcpy(pending_data->data->data, data, dataLength);
    pending_data->data->dataLength = dataLength;

    if (NULL == *dataList)
    {
        *dataList = pending_data;
    }
    else
    {
        LEDataList *curNode = *dataList;
        while (curNode->next != NULL)
        {
            curNode = curNode->next;
        }
        curNode->next = pending_data;
    }

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

void CARemoveLEDataFromList(LEDataList **dataList)
{
    OIC_LOG(DEBUG, TAG, "IN");

    VERIFY_NON_NULL(dataList, TAG, "Data list is null");

    if (*dataList)
    {
        LEDataList *curNode = *dataList;
        *dataList = (*dataList)->next;

        //Delete the first node
        CADestroyLEData(curNode->data);
        OICFree(curNode);
    }

    OIC_LOG(DEBUG, TAG, "OUT");
}

void CADestroyLEDataList(LEDataList **dataList)
{
    OIC_LOG(DEBUG, TAG, "IN");

    VERIFY_NON_NULL_VOID(dataList, TAG, "Data list is null");

    while (*dataList)
    {
        LEDataList *curNode = *dataList;
        *dataList = (*dataList)->next;

        CADestroyLEData(curNode->data);
        OICFree(curNode);
    }

    *dataList = NULL;

    OIC_LOG(DEBUG, TAG, "OUT");
}

void CADestroyLEData(LEData *data)
{
    if (data)
    {
        OICFree(data->data);
        OICFree(data);
    }
}

CAResult_t CAAddLEServerInfoToList(LEServerInfoList **serverList,
                                   LEServerInfo *leServerInfo)
{
    OIC_LOG(DEBUG, TAG, "IN");

    VERIFY_NON_NULL(serverList, TAG, "serverList");
    VERIFY_NON_NULL(leServerInfo, TAG, "leServerInfo");

    LEServerInfoList *node = (LEServerInfoList *) OICCalloc(1, sizeof(LEServerInfoList));
    if (NULL == node)
    {
        OIC_LOG(ERROR, TAG, "Calloc failed!");
        return CA_STATUS_FAILED;
    }

    node->serverInfo = leServerInfo;
    node->next = NULL;

    if (*serverList == NULL)   // Empty list
    {
        *serverList = node;
    }
    else     // Add at front end
    {
        node->next = *serverList;
        *serverList = node;
    }

    OIC_LOG_V(DEBUG, TAG, "Device [%s] added to list",
              leServerInfo->remoteAddress);

    OIC_LOG(DEBUG, TAG, "OUT");

    return CA_STATUS_OK;
}

void CARemoveLEServerInfoFromList(LEServerInfoList **serverList,
                                  const char *remoteAddress)
{
    OIC_LOG(DEBUG, TAG, "IN");
    VERIFY_NON_NULL_VOID(serverList, TAG, "serverList");
    VERIFY_NON_NULL_VOID(remoteAddress, TAG, "remoteAddress");

    LEServerInfoList *temp = *serverList;
    LEServerInfoList *prev = NULL;
    while (temp)
    {
        if (!strcasecmp(temp->serverInfo->remoteAddress, remoteAddress))
        {
            if (NULL == prev)
            {
                *serverList = temp->next;
            }
            else
            {
                prev->next = temp->next;
            }
            CAFreeLEServerInfo(temp->serverInfo);
            OICFree(temp);
            OIC_LOG_V(DEBUG, TAG, "Device [%s] removed from list", remoteAddress);
            break;
        }
        prev = temp;
        temp = temp->next;
    }

    OIC_LOG(DEBUG, TAG, "OUT");
}

CAResult_t CAGetLEServerInfo(LEServerInfoList *serverList, const char *leAddress,
                             LEServerInfo **leServerInfo)
{

    OIC_LOG(DEBUG, TAG, "IN");

    VERIFY_NON_NULL(leServerInfo, TAG, "leClientInfo");
    VERIFY_NON_NULL(leAddress, TAG, "leAddress");

    if (NULL == serverList)
    {
        OIC_LOG(DEBUG, TAG, "Server list is empty");
        return CA_STATUS_FAILED;
    }

    LEServerInfoList *cur = serverList;
    *leServerInfo = NULL;
    while (cur != NULL)
    {
        if (!strcasecmp(cur->serverInfo->remoteAddress , leAddress))
        {
            *leServerInfo = cur->serverInfo;
            OIC_LOG(DEBUG, TAG, "OUT");
            return CA_STATUS_OK;
        }

        cur = cur->next;
    }

    OIC_LOG(DEBUG, TAG, " OUT");
    return CA_STATUS_FAILED;
}

void CAFreeLEServerList(LEServerInfoList *serverList)
{
    OIC_LOG(DEBUG, TAG, "IN");
    while (serverList)
    {
        LEServerInfoList *temp = serverList;
        serverList = serverList->next;
        CAFreeLEServerInfo(temp->serverInfo);
        OICFree(temp);
    }
    OIC_LOG(DEBUG, TAG, "OUT");
}

void CAFreeLEServerInfo(LEServerInfo *leServerInfo)
{
    OIC_LOG(DEBUG, TAG, "IN");

    if (leServerInfo)
    {
        if (leServerInfo->clientHandle)
        {
            bt_gatt_client_destroy(leServerInfo->clientHandle);
        }

        if (leServerInfo->pendingDataList)
        {
            CADestroyLEDataList(&(leServerInfo->pendingDataList));
        }

        if (leServerInfo->status > LE_STATUS_CONNECTED)
        {
            int32_t ret = bt_gatt_disconnect(leServerInfo->remoteAddress);

            if (BT_ERROR_NONE != ret)
            {
                OIC_LOG_V(ERROR, TAG,
                          "bt_gatt_disconnect Failed with ret value [%d]",
                          ret);
                return;
            }
        }

        OICFree(leServerInfo->remoteAddress);
        OICFree(leServerInfo);
    }

    OIC_LOG(DEBUG, TAG, "OUT");
}

CAResult_t CAAddLEClientInfoToList(LEClientInfoList **clientList,
                                   char *clientAddress)
{
    OIC_LOG(DEBUG, TAG, "IN");
    VERIFY_NON_NULL(clientList, TAG, "clientList");
    VERIFY_NON_NULL(clientAddress, TAG, "clientAddress");

    LEClientInfoList *node = (LEClientInfoList *) OICCalloc(1, sizeof(LEClientInfoList));
    if (NULL == node)
    {
        OIC_LOG(ERROR, TAG, "Malloc failed!");
        return CA_STATUS_FAILED;
    }

    node->remoteAddress = clientAddress;
    node->next = NULL;

    if (*clientList == NULL)   // Empty list
    {
        *clientList = node;
    }
    else     // Add at front end
    {
        node->next = *clientList;
        *clientList = node;
    }

    OIC_LOG_V(DEBUG, TAG, "Device [%s] added to list", clientAddress);
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

void CARemoveLEClientInfoFromList(LEClientInfoList **clientList,
                                  const char *clientAddress)
{
    OIC_LOG(DEBUG, TAG, "IN");
    VERIFY_NON_NULL_VOID(clientAddress, TAG, "clientAddress");

    LEClientInfoList *temp = *clientList;
    LEClientInfoList *prev = NULL;
    while (temp)
    {
        if (!strcasecmp(temp->remoteAddress, clientAddress))
        {
            if (NULL == prev)
            {
                *clientList = temp->next;
            }
            else
            {
                prev->next = temp->next;
            }
            OICFree(temp->remoteAddress);
            OICFree(temp);
            OIC_LOG_V(DEBUG, TAG, "Device [%s] removed from list", clientAddress);
            break;
        }
        prev = temp;
        temp = temp->next;
    }

    OIC_LOG(DEBUG, TAG, "OUT");
}

void CADisconnectAllClient(LEClientInfoList *clientList)
{
    OIC_LOG(DEBUG, TAG, "IN");
    while (clientList)
    {
        LEClientInfoList *temp = clientList;
        clientList = clientList->next;
        if (temp->remoteAddress)
        {
            int32_t ret = bt_gatt_disconnect(temp->remoteAddress);

            if (BT_ERROR_NONE != ret)
            {
                OIC_LOG_V(ERROR, TAG,
                          "bt_gatt_disconnect Failed with ret value [%d]",
                          ret);
                return;
            }
            OICFree(temp->remoteAddress);
        }
        OICFree(temp);
    }
    OIC_LOG(DEBUG, TAG, "OUT");
}

const char *CALEGetErrorMsg(bt_error_e err)
{
    const char *errStr = NULL;

    switch (err)
    {
        case BT_ERROR_NONE:
            errStr = "BT_ERROR_NONE";
            break;
        case BT_ERROR_CANCELLED:
            errStr = "BT_ERROR_CANCELLED";
            break;
        case BT_ERROR_INVALID_PARAMETER:
            errStr = "BT_ERROR_INVALID_PARAMETER";
            break;
        case BT_ERROR_OUT_OF_MEMORY:
            errStr = "BT_ERROR_OUT_OF_MEMORY";
            break;
        case BT_ERROR_RESOURCE_BUSY:
            errStr = "BT_ERROR_RESOURCE_BUSY";
            break;
        case BT_ERROR_TIMED_OUT:
            errStr = "BT_ERROR_TIMED_OUT";
            break;
        case BT_ERROR_NOW_IN_PROGRESS:
            errStr = "BT_ERROR_NOW_IN_PROGRESS";
            break;
        case BT_ERROR_NOT_INITIALIZED:
            errStr = "BT_ERROR_NOT_INITIALIZED";
            break;
        case BT_ERROR_NOT_ENABLED:
            errStr = "BT_ERROR_NOT_ENABLED";
            break;
        case BT_ERROR_ALREADY_DONE:
            errStr = "BT_ERROR_ALREADY_DONE";
            break;
        case BT_ERROR_OPERATION_FAILED:
            errStr = "BT_ERROR_OPERATION_FAILED";
            break;
        case BT_ERROR_NOT_IN_PROGRESS:
            errStr = "BT_ERROR_NOT_IN_PROGRESS";
            break;
        case BT_ERROR_REMOTE_DEVICE_NOT_BONDED:
            errStr = "BT_ERROR_REMOTE_DEVICE_NOT_BONDED";
            break;
        case BT_ERROR_AUTH_REJECTED:
            errStr = "BT_ERROR_AUTH_REJECTED";
            break;
        case BT_ERROR_AUTH_FAILED:
            errStr = "BT_ERROR_AUTH_FAILED";
            break;
        case BT_ERROR_REMOTE_DEVICE_NOT_FOUND:
            errStr = "BT_ERROR_REMOTE_DEVICE_NOT_FOUND";
            break;
        case BT_ERROR_SERVICE_SEARCH_FAILED:
            errStr = "BT_ERROR_SERVICE_SEARCH_FAILED";
            break;
        case BT_ERROR_REMOTE_DEVICE_NOT_CONNECTED:
            errStr = "BT_ERROR_REMOTE_DEVICE_NOT_CONNECTED";
            break;
        case BT_ERROR_PERMISSION_DENIED:
            errStr = "BT_ERROR_PERMISSION_DENIED";
            break;
        case BT_ERROR_SERVICE_NOT_FOUND:
            errStr = "BT_ERROR_SERVICE_NOT_FOUND";
            break;
        case BT_ERROR_NOT_SUPPORTED:
            errStr = "BT_ERROR_NOT_SUPPORTED";
            break;
        case BT_ERROR_QUOTA_EXCEEDED:
            errStr = "BT_ERROR_QUOTA_EXCEEDED";
            break;
        case BT_ERROR_NO_DATA:
            errStr = "BT_ERROR_NO_DATA";
            break;
        case BT_ERROR_AGAIN:
            errStr = "BT_ERROR_AGAIN";
            break;
        default:
            errStr = "NOT Defined";
            break;
    }

    return errStr;
}


