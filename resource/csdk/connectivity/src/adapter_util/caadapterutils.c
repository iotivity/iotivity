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
#include "oic_string.h"
#include "oic_malloc.h"
#include <errno.h>

#ifndef WITH_ARDUINO
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#endif

#ifdef __ANDROID__
#include <jni.h>
#endif

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

        if (info->isMulticastServer && (strncmp(info->endpoint.addr, multicastAddress,
                                                strlen(multicastAddress)) == 0)
            && (info->endpoint.port == port) && (strncmp(info->ifAddr, ipAddress, strlen(ipAddress)) == 0))
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

        if (!info->isMulticastServer && (strncmp(info->endpoint.addr, ipAddress,
                                                 strlen(ipAddress)) == 0)
            && (info->endpoint.port == port))
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
        bool ifSecured = info->endpoint.flags & CA_SECURE;
        if ((strncmp(info->endpoint.addr, ipAddress, strlen(ipAddress)) == 0) &&
                    (ifSecured == isSecured))
        {
            return info->endpoint.port;
        }
    }

    return 0;
}

int CAGetSocketFdForUnicastServer(const u_arraylist_t *serverInfoList,
                                bool isMulticast, const CAEndpoint_t *endpoint)
{
    VERIFY_NON_NULL_RET(serverInfoList, CA_ADAPTER_UTILS_TAG, "serverInfoList is null", -1);
    VERIFY_NON_NULL_RET(endpoint, CA_ADAPTER_UTILS_TAG, "endpoint is null", -1);

    bool isSecured = (endpoint->flags & CA_SECURE) != 0;

    uint32_t listLength = u_arraylist_length(serverInfoList);

    for (uint32_t listIndex = 0; listIndex < listLength; listIndex++)
    {
        CAServerInfo_t *info = (CAServerInfo_t *) u_arraylist_get(serverInfoList, listIndex);
        if (!info)
        {
            continue;
        }

        if (!CAAdapterIsSameSubnet(info->endpoint.addr, endpoint->addr, info->subNetMask))
        {
            continue;
        }

        bool ifSecured = info->endpoint.flags & CA_SECURE;
        if (!info->isMulticastServer && (ifSecured == isSecured))
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

#ifndef WITH_ARDUINO
/*
 * These two conversion functions return void because errors can't happen
 * (because of NI_NUMERIC), and there's nothing to do if they do happen.
 */
void CAConvertAddrToName(const struct sockaddr_storage *sockAddr, char *host, uint16_t *port)
{
    VERIFY_NON_NULL_VOID(sockAddr, CA_ADAPTER_UTILS_TAG, "sockAddr is null");
    VERIFY_NON_NULL_VOID(host, CA_ADAPTER_UTILS_TAG, "host is null");
    VERIFY_NON_NULL_VOID(port, CA_ADAPTER_UTILS_TAG, "port is null");

    int r = getnameinfo((struct sockaddr *)sockAddr,
                        sizeof (struct sockaddr_storage),
                        host, CA_IPADDR_SIZE,
                        NULL, 0,
                        NI_NUMERICHOST|NI_NUMERICSERV);
    if (r)
    {
        if (EAI_SYSTEM == r)
        {
            OIC_LOG_V(ERROR, CA_ADAPTER_UTILS_TAG,
                            "getaddrinfo failed: errno %s", strerror(errno));
        }
        else
        {
            OIC_LOG_V(ERROR, CA_ADAPTER_UTILS_TAG,
                            "getaddrinfo failed: %s", gai_strerror(r));
        }
        return;
    }
    *port = ntohs(((struct sockaddr_in *)sockAddr)->sin_port); // IPv4 and IPv6
}

void CAConvertNameToAddr(const char *host, uint16_t port, struct sockaddr_storage *sockaddr)
{
    VERIFY_NON_NULL_VOID(host, CA_ADAPTER_UTILS_TAG, "host is null");
    VERIFY_NON_NULL_VOID(sockaddr, CA_ADAPTER_UTILS_TAG, "sockaddr is null");

    struct addrinfo *addrs;
    struct addrinfo hints = { 0 };
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_NUMERICHOST;

    int r = getaddrinfo(host, NULL, &hints, &addrs);
    if (r)
    {
        if (EAI_SYSTEM == r)
        {
            OIC_LOG_V(ERROR, CA_ADAPTER_UTILS_TAG,
                            "getaddrinfo failed: errno %s", strerror(errno));
        }
        else
        {
            OIC_LOG_V(ERROR, CA_ADAPTER_UTILS_TAG,
                            "getaddrinfo failed: %s", gai_strerror(r));
        }
        return;
    }
    // assumption: in this case, getaddrinfo will only return one addrinfo
    // or first is the one we want.
    if (addrs[0].ai_family == AF_INET6)
    {
        memcpy(sockaddr, addrs[0].ai_addr, sizeof (struct sockaddr_in6));
        ((struct sockaddr_in6 *)sockaddr)->sin6_port = htons(port);
    }
    else
    {
        memcpy(sockaddr, addrs[0].ai_addr, sizeof (struct sockaddr_in));
        ((struct sockaddr_in *)sockaddr)->sin_port = htons(port);
    }
    freeaddrinfo(addrs);
}
#endif // WITH_ARDUINO

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
