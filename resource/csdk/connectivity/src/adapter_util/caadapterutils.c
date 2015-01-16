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

#include "caadapterutils.h"

#include <string.h>

#include "oic_malloc.h"
#include "oic_string.h"

#define CA_ADAPTER_UTILS_TAG "CA_ADAPTER_UTILS"

CALocalConnectivity_t *CAAdapterCreateLocalEndpoint(CAConnectivityType_t type,
        const char *address)
{
    CALocalConnectivity_t *info = (CALocalConnectivity_t *)
                                  OICMalloc(sizeof(CALocalConnectivity_t));
    if (NULL == info)
    {
        OIC_LOG(ERROR, CA_ADAPTER_UTILS_TAG, "Memory allocation failed !");
        return NULL;
    }
    memset(info, 0, sizeof(CALocalConnectivity_t));

    info->type = type;
    if (address && strlen(address))
    {
        if (CA_EDR == type)
        {
            strncpy(info->addressInfo.BT.btMacAddress, address, CA_MACADDR_SIZE - 1);
            info->addressInfo.BT.btMacAddress[CA_MACADDR_SIZE - 1] = '\0';
        }
        else if (CA_LE == type)
        {
            strncpy(info->addressInfo.LE.leMacAddress, address, CA_MACADDR_SIZE - 1);
            info->addressInfo.LE.leMacAddress[CA_MACADDR_SIZE - 1] = '\0';
        }
        else if (CA_WIFI == type || CA_ETHERNET == type)
        {
            strncpy(info->addressInfo.IP.ipAddress, address, CA_IPADDR_SIZE - 1);
            info->addressInfo.IP.ipAddress[CA_IPADDR_SIZE - 1] = '\0';
        }
    }

    return info;
}

CALocalConnectivity_t *CAAdapterCopyLocalEndpoint(CALocalConnectivity_t *connectivity)
{
    VERIFY_NON_NULL_RET(connectivity, CA_ADAPTER_UTILS_TAG, "connectivity is NULL", NULL);

    CALocalConnectivity_t *info = (CALocalConnectivity_t *)
                                  OICMalloc(sizeof(CALocalConnectivity_t));
    if (NULL == info)
    {
        OIC_LOG(ERROR, CA_ADAPTER_UTILS_TAG, "Memory allocation failed !");
        return NULL;
    }
    memset(info, 0, sizeof(CALocalConnectivity_t));

    info->type = connectivity->type;
    if (CA_EDR == info->type && strlen(connectivity->addressInfo.BT.btMacAddress))
    {
        strncpy(info->addressInfo.BT.btMacAddress, connectivity->addressInfo.BT.btMacAddress,
                CA_MACADDR_SIZE - 1);
        info->addressInfo.BT.btMacAddress[CA_MACADDR_SIZE - 1] = '\0';
    }
    else if (CA_LE == info->type && strlen(connectivity->addressInfo.LE.leMacAddress))
    {
        strncpy(info->addressInfo.LE.leMacAddress, connectivity->addressInfo.LE.leMacAddress,
                CA_MACADDR_SIZE - 1);
        info->addressInfo.LE.leMacAddress[CA_MACADDR_SIZE - 1] = '\0';
    }
    else if ((CA_WIFI == info->type || CA_ETHERNET == info->type)
             && strlen(connectivity->addressInfo.IP.ipAddress))
    {
        strncpy(info->addressInfo.IP.ipAddress, connectivity->addressInfo.IP.ipAddress,
                CA_IPADDR_SIZE - 1);
        info->addressInfo.IP.ipAddress[CA_IPADDR_SIZE - 1] = '\0';
        info->addressInfo.IP.port = connectivity->addressInfo.IP.port;
    }

    info->isSecured = connectivity->isSecured;
    return info;
}

void CAAdapterFreeLocalEndpoint(CALocalConnectivity_t *localEndpoint)
{
    if (localEndpoint)
    {
        OICFree(localEndpoint);
    }
}

CARemoteEndpoint_t *CAAdapterCreateRemoteEndpoint(CAConnectivityType_t type,
        const char *address,
        const char *resourceUri)
{
    CARemoteEndpoint_t *info = (CARemoteEndpoint_t *)
                               OICMalloc(sizeof(CARemoteEndpoint_t));
    if (NULL == info)
    {
        OIC_LOG(ERROR, CA_ADAPTER_UTILS_TAG, "Memory allocation failed !");
        return NULL;
    }
    memset(info, 0, sizeof(CARemoteEndpoint_t));

    info->connectivityType = type;
    if (address && strlen(address))
    {
        if (CA_EDR == type)
        {
            strncpy(info->addressInfo.BT.btMacAddress, address, CA_MACADDR_SIZE - 1);
            info->addressInfo.BT.btMacAddress[CA_MACADDR_SIZE - 1] = '\0';
        }
        else if (CA_LE == info->connectivityType)
        {
            strncpy(info->addressInfo.LE.leMacAddress, address, CA_MACADDR_SIZE - 1);
            info->addressInfo.LE.leMacAddress[CA_MACADDR_SIZE - 1] = '\0';
        }
        else if (CA_WIFI == type || CA_ETHERNET == type)
        {
            strncpy(info->addressInfo.IP.ipAddress, address, CA_IPADDR_SIZE - 1);
            info->addressInfo.IP.ipAddress[CA_IPADDR_SIZE - 1] = '\0';
        }
    }

    if (resourceUri && strlen(resourceUri))
    {
        info->resourceUri = OICStrdup(resourceUri);
    }

    return info;
}

CARemoteEndpoint_t *CAAdapterCopyRemoteEndpoint(const CARemoteEndpoint_t *remoteEndpoint)
{
    VERIFY_NON_NULL_RET(remoteEndpoint, CA_ADAPTER_UTILS_TAG, "Remote endpoint is NULL", NULL);

    CARemoteEndpoint_t *info = (CARemoteEndpoint_t *)
                               OICMalloc(sizeof(CARemoteEndpoint_t));
    if (NULL == info)
    {
        OIC_LOG(ERROR, CA_ADAPTER_UTILS_TAG, "Memory allocation failed !");
        return NULL;
    }
    memset(info, 0, sizeof(CARemoteEndpoint_t));

    info->connectivityType = remoteEndpoint->connectivityType;
    if (CA_EDR == info->connectivityType && strlen(remoteEndpoint->addressInfo.BT.btMacAddress))
    {
        strncpy(info->addressInfo.BT.btMacAddress, remoteEndpoint->addressInfo.BT.btMacAddress,
                CA_MACADDR_SIZE - 1);
        info->addressInfo.BT.btMacAddress[CA_MACADDR_SIZE - 1] = '\0';
    }
    else if (CA_LE == info->connectivityType && strlen(remoteEndpoint->addressInfo.LE.leMacAddress))
    {
        strncpy(info->addressInfo.LE.leMacAddress, remoteEndpoint->addressInfo.LE.leMacAddress,
                CA_MACADDR_SIZE - 1);
        info->addressInfo.LE.leMacAddress[CA_MACADDR_SIZE - 1] = '\0';
    }
    else if ((CA_WIFI == info->connectivityType || CA_ETHERNET == info->connectivityType)
             && strlen(remoteEndpoint->addressInfo.IP.ipAddress))
    {
        strncpy(info->addressInfo.IP.ipAddress, remoteEndpoint->addressInfo.IP.ipAddress,
                CA_IPADDR_SIZE - 1);
        info->addressInfo.IP.ipAddress[CA_IPADDR_SIZE - 1] = '\0';
        info->addressInfo.IP.port = remoteEndpoint->addressInfo.IP.port;
    }

    if (remoteEndpoint->resourceUri && strlen(remoteEndpoint->resourceUri))
    {
        info->resourceUri = OICStrdup(remoteEndpoint->resourceUri);
    }

    info->isSecured = remoteEndpoint->isSecured;
    return info;
}

void CAAdapterFreeRemoteEndpoint(CARemoteEndpoint_t *remoteEndpoint)
{
    if (remoteEndpoint)
    {
        if (remoteEndpoint->resourceUri)
        {
            OICFree(remoteEndpoint->resourceUri);
        }

        OICFree(remoteEndpoint);
    }
}

bool CAAdapterIsSameSubnet(const char *ipAddress1, const char *ipAddress2,
                           const char *netMask)
{
    VERIFY_NON_NULL_RET(ipAddress1, CA_ADAPTER_UTILS_TAG, "First address", false);
    VERIFY_NON_NULL_RET(ipAddress2, CA_ADAPTER_UTILS_TAG, "Second address", false);
    VERIFY_NON_NULL_RET(netMask, CA_ADAPTER_UTILS_TAG, "netMask", false);

    int32_t ipList1[8] = {0};
    int32_t ipList2[8] = {0};
    int32_t maskList[8] = {0};

    /* Local Loopback Address */
    if (0 == strncmp(ipAddress1, "127.", 4)
        || 0 == strncmp(ipAddress2, "127.", 4))
    {
        return true;
    }

    char *ipAdrs1 = OICStrdup(ipAddress1);
    if (!ipAdrs1)
    {
        OIC_LOG(ERROR, CA_ADAPTER_UTILS_TAG, "Failed to get dup string!");
        return false;
    }

    int16_t index = 0;
    int16_t lastDotIndex = 0;
    int16_t ip1TokenCount = 0;
    while ('\0' != ipAdrs1[index])
    {
        if ('.' == ipAdrs1[index])
        {
            ipAdrs1[index] = '\0';
            ipList1[ip1TokenCount++] = atoi(ipAdrs1 + lastDotIndex);
            lastDotIndex = index + 1;
        }
        index++;
    }
    // Add last touple
    ipList1[ip1TokenCount] = atoi(ipAdrs1 + lastDotIndex);

    char *ipAdrs2 = OICStrdup(ipAddress2);
    if (!ipAdrs2)
    {
        OIC_LOG(ERROR, CA_ADAPTER_UTILS_TAG, "Failed to get dup string!");
        OICFree(ipAdrs1);
        return false;
    }

    index = 0;
    lastDotIndex = 0;
    int16_t ip2TokenCount = 0;
    while ('\0' != ipAdrs2[index])
    {
        if ('.' == ipAdrs2[index])
        {
            ipAdrs2[index] = '\0';
            ipList2[ip2TokenCount++] = atoi(ipAdrs2 + lastDotIndex);
            lastDotIndex = index + 1;
        }
        index++;
    }
    // Add last touple
    ipList2[ip2TokenCount] = atoi(ipAdrs2 + lastDotIndex);

    char *nMask = OICStrdup(netMask);
    if (!nMask)
    {
        OIC_LOG(ERROR, CA_ADAPTER_UTILS_TAG, "Failed to get dup string!");
        OICFree(ipAdrs1);
        OICFree(ipAdrs2);
        return false;
    }

    index = 0;
    lastDotIndex = 0;
    int16_t maskTokenCount = 0;
    while ('\0' != nMask[index])
    {
        if ('.' == nMask[index])
        {
            nMask[index] = '\0';
            maskList[maskTokenCount++] = atoi(nMask + lastDotIndex);
            lastDotIndex = index + 1;
        }
        index++;
    }
    // Add last touple
    maskList[maskTokenCount] = atoi(nMask + lastDotIndex);

    OICFree(ipAdrs1);
    OICFree(ipAdrs2);
    OICFree(nMask);

    if (ip1TokenCount < 3 || ip2TokenCount < 3 || maskTokenCount < 3)
    {
        OIC_LOG(ERROR, CA_ADAPTER_UTILS_TAG, "Address or mask is invalid!");
        return false;
    }

    if (((ipList1[0]& maskList[0]) == (ipList2[0]& maskList[0]))
        && ((ipList1[1]& maskList[1]) == (ipList2[1]& maskList[1]))
        && ((ipList1[2]& maskList[2]) == (ipList2[2]& maskList[2]))
        && ((ipList1[3]& maskList[3]) == (ipList2[3]& maskList[3])))
    {
        return true;
    }
    return false;
}
