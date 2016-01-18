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

/**
 * Number of services connected.
 */
static int32_t g_numberOfServiceConnected = 0;

void CAIncrementRegisteredServiceCount()
{
    g_numberOfServiceConnected++;
}

void CADecrementRegisteredServiceCount()
{
    g_numberOfServiceConnected--;
}

void CAResetRegisteredServiceCount()
{
    g_numberOfServiceConnected = 0;
}

int32_t  CAGetRegisteredServiceCount()
{
    return g_numberOfServiceConnected ;
}

CAResult_t CAAddLEServerInfoToList(LEServerInfoList **serverList,
                                   LEServerInfo *leServerInfo)
{

    OIC_LOG(DEBUG, TAG, "IN");

    VERIFY_NON_NULL(serverList, TAG, "clientList");
    VERIFY_NON_NULL(leServerInfo, TAG, "leClientInfo");

    LEServerInfoList *node = (LEServerInfoList *) OICCalloc(1, sizeof(LEServerInfoList));
    if (NULL == node)
    {
        OIC_LOG(ERROR, TAG, "Malloc failed!");
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

    CAIncrementRegisteredServiceCount();

    OIC_LOG_V(DEBUG, TAG, "Device [%s] added to list",
              leServerInfo->remoteAddress);

    OIC_LOG(DEBUG, TAG, "OUT");

    return CA_STATUS_OK;
}

CAResult_t CAGetLEServerInfo(LEServerInfoList *serverList, const char *leAddress,
                             LEServerInfo **leServerInfo)
{

    OIC_LOG(DEBUG, TAG, "IN");

    VERIFY_NON_NULL(serverList, TAG, "clientList");
    VERIFY_NON_NULL(leServerInfo, TAG, "leClientInfo");
    VERIFY_NON_NULL(leAddress, TAG, "leAddress");

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

CAResult_t CAGetLEServerInfoByPosition(LEServerInfoList *serverList, int32_t position,
                                       LEServerInfo **leServerInfo)
{
    OIC_LOG(DEBUG, TAG, "IN");

    VERIFY_NON_NULL(serverList, TAG, "clientList");
    VERIFY_NON_NULL(leServerInfo, TAG, "leClientInfo");

    if (0 > position)
    {
        OIC_LOG(ERROR, TAG, "Position Invalid input !");
        return CA_STATUS_INVALID_PARAM;
    }

    *leServerInfo = NULL;
    int32_t count = 0;
    LEServerInfoList *cur = serverList;
    while (cur != NULL)
    {
        if (position == count)
        {
            *leServerInfo = cur->serverInfo;
            OIC_LOG(DEBUG, TAG, "OUT");
            return CA_STATUS_OK;
        }
        count++;
        cur = cur->next;
    }
    OIC_LOG(DEBUG, TAG, "Client info not found for the position");
    return CA_STATUS_FAILED;
}

void CAFreeLEServerList(LEServerInfoList *clientList)
{
    OIC_LOG(DEBUG, TAG, "IN");
    while (clientList)
    {
        LEServerInfoList *temp = clientList;
        clientList = clientList->next;
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
        if (leServerInfo->remoteAddress)
        {
            bt_gatt_client_destroy(leServerInfo->clientHandle);
            int32_t ret = bt_gatt_disconnect(leServerInfo->remoteAddress);

            if (BT_ERROR_NONE != ret)
            {
                OIC_LOG_V(ERROR, TAG,
                          "bt_gatt_disconnect Failed with ret value [%d]",
                          ret);
                return;
            }
            OICFree(leServerInfo->remoteAddress);
        }
        OICFree(leServerInfo);
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
        default:
            errStr = "NOT Defined";
            break;
    }

    return errStr;
}


