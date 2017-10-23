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

#include "cawsutil.h"
#include "caadapterutils.h"
#include "experimental/logger.h"
#include "oic_malloc.h"

#include <string.h>
#include <errno.h>

#define TAG "OIC_CA_WS_UTIL"

WSData *CAWSPrepareWSData(const void *data, uint32_t dataLength)
{
    OIC_LOG_V(DEBUG, TAG, "%s IN", __func__);

    WSData *wsData = (WSData *)OICMalloc(sizeof(WSData));

    if (!wsData)
    {
        OIC_LOG(DEBUG, TAG, "Could not allocate wsData");
        return NULL;
    }

    wsData->data = OICMalloc(LWS_PRE + dataLength);
    wsData->dataLength = dataLength;

    if (!wsData->data)
    {
        OICFree(wsData);
        OIC_LOG(DEBUG, TAG, "Could not allocate wsData->data");
        return NULL;
    }

    memcpy((uint8_t *)wsData->data + LWS_PRE, data, dataLength);
    return wsData;
}

void CAWSDestroyWSData(WSData *wsData)
{
    OIC_LOG_V(DEBUG, TAG, "%s IN", __func__);

    if (wsData)
    {
        if (wsData->data)
        {
            OICFree(wsData->data);
        }
        OICFree(wsData);
    }
}

CAResult_t CAWSAddWSDataToList(WSDataList **dataList, WSData *data)
{
    OIC_LOG_V(DEBUG, TAG, "%s IN", __func__);

    WSDataList *node = (WSDataList *)OICMalloc(sizeof(WSDataList));

    if (!node)
    {
        OIC_LOG(ERROR, TAG, "Could not allocate ws data list node");
        return CA_MEMORY_ALLOC_FAILED;
    }

    node->data = data;
    node->next = NULL;

    if (*dataList)
    {
        WSDataList *cur = *dataList;
        while (cur->next)
        {
            cur = cur->next;
        }
        cur->next = node;
    }
    else
    {
        *dataList = node;
    }

    return CA_STATUS_OK;
}

WSData *CAWSRemoveWSDataFromList(WSDataList **dataList)
{
    OIC_LOG_V(DEBUG, TAG, "%s IN", __func__);

    if (!(*dataList))
    {
        OIC_LOG(DEBUG, TAG, "List is empty");
        return NULL;
    }

    WSData *data = (*dataList)->data;
    WSDataList *node = (*dataList);
    *dataList = node->next;
    OICFree(node);
    return data;
}

void CAWSDestroyWSDataList(WSDataList *dataList)
{
    OIC_LOG_V(DEBUG, TAG, "%s IN", __func__);

    if (!dataList)
    {
        OIC_LOG(DEBUG, TAG, "List is empty");
        return;
    }

    WSDataList *cur = dataList;
    while (cur)
    {
        if (cur->data)
        {
            CAWSDestroyWSData(cur->data);
        }

        WSDataList *prev = cur;
        cur = cur->next;
        OICFree(prev);
    }
}

CAResult_t CAWSAddWSConnInfoToList(WSConnInfoList **connInfoList, WSConnInfo *connInfo)
{
    OIC_LOG_V(DEBUG, TAG, "%s IN", __func__);

    WSConnInfoList *node = (WSConnInfoList *)OICMalloc(sizeof(WSConnInfoList));

    if (!node)
    {
        OIC_LOG(ERROR, TAG, "Could not allocate ws conninfo list node");
        return CA_MEMORY_ALLOC_FAILED;
    }

    node->connInfo = connInfo;
    node->next = *connInfoList;
    *connInfoList = node;

    return CA_STATUS_OK;
}

bool CAWSRemoveWSConnInfoFromList(WSConnInfoList **connInfoList, WSConnInfo *connInfo)
{
    OIC_LOG_V(DEBUG, TAG, "%s IN", __func__);

    if (NULL == *connInfoList)
    {
        OIC_LOG(ERROR, TAG, "Connection info list is empty");
        return false;
    }

    WSConnInfoList *cur = *connInfoList;
    WSConnInfoList *prev = NULL;
    while (cur)
    {
        if (cur->connInfo == connInfo)
        {
            if (NULL != prev)
            {
                prev->next = cur->next;
            }
            else
            {
                *connInfoList = cur->next;
            }
            OICFree(cur);
            return true;
        }
        prev = cur;
        cur = cur->next;
    }

    OIC_LOG(ERROR, TAG, "Connection info not found in list");
    return false;
}
WSConnInfo *CAWSFindWSConnInfoFromList(WSConnInfoList *connInfoList, const CAEndpoint_t *endpoint)
{
    OIC_LOG_V(DEBUG, TAG, "%s IN", __func__);

    WSConnInfoList *cur = connInfoList;
    while (cur)
    {
        if (!strcmp(cur->connInfo->secureEndpoint.endpoint.addr, endpoint->addr) &&
            cur->connInfo->secureEndpoint.endpoint.port == endpoint->port)
        {
            return cur->connInfo;
        }
        cur = cur->next;
    }

    return NULL;
}

void CAWSDestroyWSConnInfo(WSConnInfo *connInfo)
{
    OIC_LOG_V(DEBUG, TAG, "%s IN", __func__);

    if (connInfo)
    {
        CAWSDestroyWSDataList(connInfo->pendingDataList);
        CAWSDestroyWSData(connInfo->recvData);
        OICFree(connInfo);
    }
}

void CAWSDestroyWSConnInfoList(WSConnInfoList *connInfoList)
{
    OIC_LOG_V(DEBUG, TAG, "%s IN", __func__);

    if (!connInfoList)
    {
        OIC_LOG(DEBUG, TAG, "List is empty");
        return;
    }

    WSConnInfoList *cur = connInfoList;
    while (cur)
    {
        if (cur->connInfo)
        {
            CAWSDestroyWSConnInfo(cur->connInfo);
        }

        WSConnInfoList *prev = cur;
        cur = cur->next;
        OICFree(prev);
    }
}

CAResult_t CAWSGetPeerAddress(int sockfd, char *host,
                              uint16_t *port, CATransportFlags_t *flag)
{
    struct sockaddr addr;
    socklen_t addrLen = sizeof(addr);

    if (0 != getpeername(sockfd, &addr, &addrLen))
    {
        OIC_LOG_V(ERROR, TAG, "Failed to get peer information (errno=%d)", errno);
        return CA_STATUS_FAILED;
    }

    if (flag && addr.sa_family == AF_INET)
    {
        *flag |= CA_IPV4;
    }
    else if (flag && addr.sa_family == AF_INET6)
    {
        *flag |= CA_IPV6;
    }

    return CAConvertAddrToName((struct sockaddr_storage *)&addr, addrLen, host, port);
}
