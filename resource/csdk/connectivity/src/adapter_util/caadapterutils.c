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
#include <ctype.h>

#ifdef __ANDROID__
#include <jni.h>
#endif

#include "oic_malloc.h"
#include "oic_string.h"

#define CA_ADAPTER_UTILS_TAG "CA_ADAPTER_UTILS"

#ifdef __ANDROID__
/**
 * @var g_jvm
 * @brief pointer to store JavaVM
 */
static JavaVM *g_jvm = NULL;

/**
 * @var gContext
 * @brief pointer to store context for android callback interface
 */
static jobject g_Context = NULL;
#endif

void CALogPDUData(coap_pdu_t *pdu)
{
    VERIFY_NON_NULL_VOID(pdu, CA_ADAPTER_UTILS_TAG, "pdu");
    OIC_LOG_V(DEBUG, CA_ADAPTER_UTILS_TAG, "PDU Maker - payload : %s", pdu->data);

    OIC_LOG_V(DEBUG, CA_ADAPTER_UTILS_TAG, "PDU Maker - type : %d", pdu->hdr->type);

    OIC_LOG_V(DEBUG, CA_ADAPTER_UTILS_TAG, "PDU Maker - code : %d", pdu->hdr->code);

    OIC_LOG_V(DEBUG, CA_ADAPTER_UTILS_TAG, "PDU Maker - id : %d", pdu->hdr->id);

    OIC_LOG_V(DEBUG, CA_ADAPTER_UTILS_TAG, "PDU Maker - token : %s", pdu->hdr->token);
}

CALocalConnectivity_t *CAAdapterCreateLocalEndpoint(CATransportType_t type, const char *address)
{
    CALocalConnectivity_t *info = (CALocalConnectivity_t *)
                                  OICCalloc(1, sizeof(CALocalConnectivity_t));
    if (NULL == info)
    {
        OIC_LOG(ERROR, CA_ADAPTER_UTILS_TAG, "Memory allocation failed !");
        return NULL;
    }

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
        else if (CA_IPV4 == type)
        {
            strncpy(info->addressInfo.IP.ipAddress, address, CA_IPADDR_SIZE - 1);
            info->addressInfo.IP.ipAddress[CA_IPADDR_SIZE - 1] = '\0';
        }
        else if (CA_IPV6 == type)
        {
            OIC_LOG(ERROR, CA_ADAPTER_UTILS_TAG, "Currently IPV6 is not supported");
            OICFree(info);
            return NULL;
        }
        else
        {
            OIC_LOG(ERROR, CA_ADAPTER_UTILS_TAG, "type is not matched with any transport!");
            OICFree(info);
            return NULL;
        }
    }

    return info;
}

CALocalConnectivity_t *CAAdapterCopyLocalEndpoint(const CALocalConnectivity_t *connectivity)
{
    VERIFY_NON_NULL_RET(connectivity, CA_ADAPTER_UTILS_TAG, "connectivity is NULL", NULL);

    CALocalConnectivity_t *info = (CALocalConnectivity_t *)
                                  OICCalloc(1, sizeof(CALocalConnectivity_t));
    if (NULL == info)
    {
        OIC_LOG(ERROR, CA_ADAPTER_UTILS_TAG, "Memory allocation failed !");
        return NULL;
    }

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
    else if ((CA_IPV4 == info->type)

            && strlen(connectivity->addressInfo.IP.ipAddress))
    {
        strncpy(info->addressInfo.IP.ipAddress, connectivity->addressInfo.IP.ipAddress,
                CA_IPADDR_SIZE - 1);
        info->addressInfo.IP.ipAddress[CA_IPADDR_SIZE - 1] = '\0';
        info->addressInfo.IP.port = connectivity->addressInfo.IP.port;
    }
    else if (CA_IPV6 == info->type)
    {
        OIC_LOG(ERROR, CA_ADAPTER_UTILS_TAG, "Currently IPV6 is not supported");
        OICFree(info);
        return NULL;
    }
    else
    {
        OIC_LOG(ERROR, CA_ADAPTER_UTILS_TAG, "type is not matched with any transport!");
        OICFree(info);
        return NULL;
    }

    info->isSecured = connectivity->isSecured;
    return info;
}

void CAAdapterFreeLocalEndpoint(CALocalConnectivity_t *localEndpoint)
{
    OICFree(localEndpoint);
}

CARemoteEndpoint_t *CAAdapterCreateRemoteEndpoint(CATransportType_t type, const char *address,
                                                  const char *resourceUri)
{
    CARemoteEndpoint_t *info = (CARemoteEndpoint_t *)
                               OICCalloc(1, sizeof(CARemoteEndpoint_t));
    if (NULL == info)
    {
        OIC_LOG(ERROR, CA_ADAPTER_UTILS_TAG, "Memory allocation failed !");
        return NULL;
    }

    info->transportType = type;
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
        else if (CA_IPV4 == type)
        {
            strncpy(info->addressInfo.IP.ipAddress, address, CA_IPADDR_SIZE - 1);
            info->addressInfo.IP.ipAddress[CA_IPADDR_SIZE - 1] = '\0';
        }
        else if (CA_IPV6 == type)
        {
            OIC_LOG(ERROR, CA_ADAPTER_UTILS_TAG, "Currently IPV6 is not supported");
            OICFree(info);
            return NULL;
        }
        else
        {
            OIC_LOG(ERROR, CA_ADAPTER_UTILS_TAG, "type is not matched with any transport!");
            OICFree(info);
            return NULL;
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
                               OICCalloc(1, sizeof(CARemoteEndpoint_t));
    if (NULL == info)
    {
        OIC_LOG(ERROR, CA_ADAPTER_UTILS_TAG, "Memory allocation failed !");
        return NULL;
    }

    info->transportType = remoteEndpoint->transportType;
    if (CA_EDR == info->transportType && ('\0' != remoteEndpoint->addressInfo.BT.btMacAddress[0]))
    {
        strncpy(info->addressInfo.BT.btMacAddress, remoteEndpoint->addressInfo.BT.btMacAddress,
                CA_MACADDR_SIZE - 1);
        info->addressInfo.BT.btMacAddress[CA_MACADDR_SIZE - 1] = '\0';
    }
    else if (CA_LE == info->transportType
             && ('\0' != remoteEndpoint->addressInfo.LE.leMacAddress[0]))
    {
        strncpy(info->addressInfo.LE.leMacAddress, remoteEndpoint->addressInfo.LE.leMacAddress,
                CA_MACADDR_SIZE - 1);
        info->addressInfo.LE.leMacAddress[CA_MACADDR_SIZE - 1] = '\0';
    }
    else if ((CA_IPV4 == info->transportType)
            && ('\0' != remoteEndpoint->addressInfo.IP.ipAddress[0]))
    {
        strncpy(info->addressInfo.IP.ipAddress, remoteEndpoint->addressInfo.IP.ipAddress,
                CA_IPADDR_SIZE - 1);
        info->addressInfo.IP.ipAddress[CA_IPADDR_SIZE - 1] = '\0';
        info->addressInfo.IP.port = remoteEndpoint->addressInfo.IP.port;
    }
    else if (CA_IPV6 == info->transportType)
    {
        OIC_LOG(ERROR, CA_ADAPTER_UTILS_TAG, "Currently IPV6 is not supported");
    }
    else
    {
        OIC_LOG(DEBUG, CA_ADAPTER_UTILS_TAG, "Its not matching. May be multicast.");
    }

    //For Multicast, remote address will be null while resourceUri will have the service UUID

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
        OICFree(remoteEndpoint->resourceUri);
        OICFree(remoteEndpoint);
    }
}

CAResult_t CAParseIPv4AddressInternal(const char *ipAddrStr, uint8_t *ipAddr,
                                   size_t ipAddrLen, uint16_t *port)
{
    if (!ipAddr || !isdigit(ipAddrStr[0]) || !port)
    {
        OIC_LOG(ERROR, CA_ADAPTER_UTILS_TAG, "invalid param");
        return CA_STATUS_INVALID_PARAM;
    }

    size_t index = 0;
    uint8_t dotCount = 0;

    ipAddr[index] = 0;
    *port = 0;
    while (*ipAddrStr)
    {
        if (isdigit(*ipAddrStr))
        {
            if(index >= ipAddrLen)
            {
                OIC_LOG(ERROR, CA_ADAPTER_UTILS_TAG, "invalid param");
                return CA_STATUS_INVALID_PARAM;
            }
            ipAddr[index] *= 10;
            ipAddr[index] += *ipAddrStr - '0';
        }
        else if (*ipAddrStr == '.')
        {
            index++;
            dotCount++;
            ipAddr[index] = 0;
        }
        else
        {
            break;
        }
        ipAddrStr++;
    }

    if (*ipAddrStr == ':')
    {
        ipAddrStr++;
        while (*ipAddrStr)
        {
            if (isdigit(*ipAddrStr))
            {
                *port *= 10;
                *port += *ipAddrStr - '0';
            }
            else
            {
                break;
            }
            ipAddrStr++;
        }
    }

    if (dotCount == 3)
    {
        return CA_STATUS_OK;
    }
    return CA_STATUS_FAILED;
}

bool CAAdapterIsSameSubnet(const char *ipAddress1, const char *ipAddress2, const char *netMask)
{
    VERIFY_NON_NULL_RET(ipAddress1, CA_ADAPTER_UTILS_TAG, "First address", false);
    VERIFY_NON_NULL_RET(ipAddress2, CA_ADAPTER_UTILS_TAG, "Second address", false);
    VERIFY_NON_NULL_RET(netMask, CA_ADAPTER_UTILS_TAG, "netMask", false);

    uint8_t ipList1[IPV4_ADDR_ONE_OCTECT_LEN] = { 0 };
    uint8_t ipList2[IPV4_ADDR_ONE_OCTECT_LEN] = { 0 };
    uint8_t maskList[IPV4_ADDR_ONE_OCTECT_LEN] = { 0 };
    CAResult_t ret = CA_STATUS_OK;

    /* Local Loopback Address */
    if (0 == strncmp(ipAddress1, "127.", 4) || 0 == strncmp(ipAddress2, "127.", 4))
    {
        return true;
    }

    uint16_t parsedPort = 0;
    ret = CAParseIPv4AddressInternal(ipAddress1, ipList1, sizeof(ipList1), &parsedPort);
    if (ret != CA_STATUS_OK)
    {
        OIC_LOG_V(ERROR, CA_ADAPTER_UTILS_TAG, "First ip address parse fail %d", ret);
        return false;
    }

    ret = CAParseIPv4AddressInternal(ipAddress2, ipList2, sizeof(ipList2), &parsedPort);
    if (ret != CA_STATUS_OK)
    {
        OIC_LOG_V(ERROR, CA_ADAPTER_UTILS_TAG, "Second ip address parse fail %d", ret);
        return false;
    }

    ret = CAParseIPv4AddressInternal(netMask, maskList, sizeof(maskList), &parsedPort);
    if (ret != CA_STATUS_OK)
    {
        OIC_LOG_V(ERROR, CA_ADAPTER_UTILS_TAG, "Net mask parse fail %d", ret);
        return false;
    }

    return ((ipList1[0] & maskList[0]) == (ipList2[0] & maskList[0])) && ((ipList1[1] & maskList[1])
            == (ipList2[1] & maskList[1]))
           && ((ipList1[2] & maskList[2]) == (ipList2[2] & maskList[2]))
           && ((ipList1[3] & maskList[3]) == (ipList2[3] & maskList[3]));
}


bool CAIsMulticastServerStarted(const u_arraylist_t *serverInfoList, const char *ipAddress,
                                const char *multicastAddress, uint16_t port)
{
    VERIFY_NON_NULL_RET(serverInfoList, CA_ADAPTER_UTILS_TAG, "serverInfoList is null", false);
    VERIFY_NON_NULL_RET(ipAddress, CA_ADAPTER_UTILS_TAG, "ipAddress is null", false);
    VERIFY_NON_NULL_RET(multicastAddress, CA_ADAPTER_UTILS_TAG, "multicastAddress is null", false);

    uint32_t listLength = u_arraylist_length(serverInfoList);
    for (uint32_t listIndex = 0; listIndex < listLength; listIndex++)
    {
        CAServerInfo_t *info = (CAServerInfo_t *) u_arraylist_get(serverInfoList, listIndex);
        if (!info)
        {
            OIC_LOG(ERROR, CA_ADAPTER_UTILS_TAG, "Info is NULL");
            return false;
        }

        if (info->isMulticastServer && (strncmp(info->ipAddress, multicastAddress,
                                                strlen(multicastAddress) == 0))
            && (info->port == port) && (strncmp(info->ifAddr, ipAddress, strlen(ipAddress)) == 0))
        {
            return info->isServerStarted;
        }
    }
    return false;
}

bool CAIsUnicastServerStarted(const u_arraylist_t *serverInfoList, const char *ipAddress,
                              uint16_t port)
{
    VERIFY_NON_NULL_RET(serverInfoList, CA_ADAPTER_UTILS_TAG, "serverInfoList is null", false);
    VERIFY_NON_NULL_RET(ipAddress, CA_ADAPTER_UTILS_TAG, "ipAddress is null", false);

    uint32_t listLength = u_arraylist_length(serverInfoList);
    for (uint32_t listIndex = 0; listIndex < listLength; listIndex++)
    {
        CAServerInfo_t *info = (CAServerInfo_t *) u_arraylist_get(serverInfoList, listIndex);
        if (!info)
        {
            continue;
        }

        if (!info->isMulticastServer && (strncmp(info->ipAddress, ipAddress,
                                                 strlen(ipAddress)) == 0)
            && (info->port == port))
        {
            return info->isServerStarted;
        }
    }
    return false;
}

uint16_t CAGetServerPort(const u_arraylist_t *serverInfoList, const char *ipAddress, bool isSecured)
{
    VERIFY_NON_NULL_RET(serverInfoList, CA_ADAPTER_UTILS_TAG, "serverInfoList is null", 0);
    VERIFY_NON_NULL_RET(ipAddress, CA_ADAPTER_UTILS_TAG, "ipAddress is null", 0);

    uint32_t listLength = u_arraylist_length(serverInfoList);
    for (uint32_t listIndex = 0; listIndex < listLength; listIndex++)
    {
        CAServerInfo_t *info = (CAServerInfo_t *) u_arraylist_get(serverInfoList, listIndex);
        if (!info)
        {
            continue;
        }
        if ((strncmp(info->ipAddress, ipAddress, strlen(ipAddress)) == 0) &&
                    (info->isSecured == isSecured))
        {
            return info->port;
        }
    }

    return 0;
}

int CAGetSocketFdForUnicastServer(const u_arraylist_t *serverInfoList, const char *ipAddress,
                                  bool isSecured, bool isMulticast, CATransportType_t type)
{
    VERIFY_NON_NULL_RET(serverInfoList, CA_ADAPTER_UTILS_TAG, "serverInfoList is null", -1);
    VERIFY_NON_NULL_RET(ipAddress, CA_ADAPTER_UTILS_TAG, "ipAddress is null", -1);

    uint32_t listLength = u_arraylist_length(serverInfoList);

    for (uint32_t listIndex = 0; listIndex < listLength; listIndex++)
    {
        CAServerInfo_t *info = (CAServerInfo_t *) u_arraylist_get(serverInfoList, listIndex);
        if (!info)
        {
            continue;
        }

        if (!CAAdapterIsSameSubnet(info->ipAddress, ipAddress, info->subNetMask))
        {
            continue;
        }

        if (!info->isMulticastServer && (info->isSecured == isSecured))
        {
            OIC_LOG_V(DEBUG, CA_ADAPTER_UTILS_TAG,
                      "CAGetSocketFdForServer found socket [%d]", info->socketFd);
            return info->socketFd;
        }

    }

    OIC_LOG(ERROR, CA_ADAPTER_UTILS_TAG,
            "CAGetSocketFdForServer socket fd is not found");
    return -1;
}

CAResult_t CAAddServerInfo(u_arraylist_t *serverInfoList, CAServerInfo_t *info)
{
    VERIFY_NON_NULL(serverInfoList, CA_ADAPTER_UTILS_TAG, "serverInfoList is null");
    VERIFY_NON_NULL(info, CA_ADAPTER_UTILS_TAG, "info is null");

    CAResult_t result = u_arraylist_add(serverInfoList, (void *) info);
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, CA_ADAPTER_UTILS_TAG, "u_arraylist_add failed!");
    }
    return result;
}

void CARemoveServerInfo(u_arraylist_t *serverInfoList, int sockFd)
{
    VERIFY_NON_NULL_VOID(serverInfoList, CA_ADAPTER_UTILS_TAG, "serverInfoList is null");

    uint32_t listLength = u_arraylist_length(serverInfoList);
    for (uint32_t listIndex = 0; listIndex < listLength;)
    {
        CAServerInfo_t *info = (CAServerInfo_t *) u_arraylist_get(serverInfoList, listIndex);
        if (!info)
        {
            listIndex++;
            continue;
        }

        if (info->socketFd == sockFd)
        {
            if (u_arraylist_remove(serverInfoList, listIndex))
            {
                OICFree(info);
                listLength--;
            }
            else
            {
                OIC_LOG(ERROR, CA_ADAPTER_UTILS_TAG, "u_arraylist_remove failed!");
                break;
            }
        }
        else
        {
            listIndex++;
        }
    }
}

void CAClearNetInterfaceInfoList(u_arraylist_t *infoList)
{
    uint32_t listLength = u_arraylist_length(infoList);
    for (uint32_t listIndex = 0; listIndex < listLength; listIndex++)
    {
        CANetInfo_t *netInfo = (CANetInfo_t *) u_arraylist_get(infoList, listIndex);
        if (!netInfo)
        {
            continue;
        }
        OICFree(netInfo);
    }
    u_arraylist_free(&infoList);
}

void CAClearServerInfoList(u_arraylist_t *serverInfoList)
{
    uint32_t listLength = u_arraylist_length(serverInfoList);
    for (uint32_t listIndex = 0; listIndex < listLength; listIndex++)
    {
        CAServerInfo_t *serverInfo = (CAServerInfo_t *) u_arraylist_get(serverInfoList, listIndex);
        if (!serverInfo)
        {
            continue;
        }
        OICFree(serverInfo);
    }
    u_arraylist_free(&serverInfoList);
}

#ifdef __ANDROID__
void CANativeJNISetContext(JNIEnv *env, jobject context)
{
    OIC_LOG_V(DEBUG, CA_ADAPTER_UTILS_TAG, "CANativeJNISetContext");

    if (!context)
    {
        OIC_LOG(DEBUG, CA_ADAPTER_UTILS_TAG, "context is null");

    }

    g_Context = (*env)->NewGlobalRef(env, context);
}

void CANativeJNISetJavaVM(JavaVM *jvm)
{
    OIC_LOG_V(DEBUG, CA_ADAPTER_UTILS_TAG, "CANativeJNISetJavaVM");
    g_jvm = jvm;
}

jobject CANativeJNIGetContext()
{
    return g_Context;
}

JavaVM *CANativeJNIGetJavaVM()
{
    return g_jvm;
}
#endif

