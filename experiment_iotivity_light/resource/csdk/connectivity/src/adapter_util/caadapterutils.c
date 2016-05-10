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

#define TAG "CA_ADAPTER_UTILS"

/*
 * These two conversion functions return void because errors can't happen
 * (because of NI_NUMERIC), and there's nothing to do if they do happen.
 */
void CAConvertAddrToName(const struct sockaddr_storage *sockAddr, char *host, uint16_t *port)
{
    int r = getnameinfo((struct sockaddr *)sockAddr,
                        sizeof (struct sockaddr_storage),
                        host, MAX_ADDR_STR_SIZE, NULL, 0,
                        NI_NUMERICHOST|NI_NUMERICSERV);
    if (r)
    {
        if (EAI_SYSTEM == r)
        {
            OIC_LOG_V(ERROR, TAG,
                            "getaddrinfo failed: errno %s", strerror(errno));
        }
        else
        {
            OIC_LOG_V(ERROR, TAG, "getaddrinfo failed: %s", gai_strerror(r));
        }
        return;
    }
    *port = ntohs(((struct sockaddr_in *)sockAddr)->sin_port); // IPv4 and IPv6
}

void CAConvertNameToAddr(const char *host, uint16_t port, struct sockaddr_storage *sockaddr)
{
    VERIFY_NON_NULL_VOID(host, TAG, "host is null");
    VERIFY_NON_NULL_VOID(sockaddr, TAG, "sockaddr is null");

    struct addrinfo *addrs;
    struct addrinfo hints = { .ai_family = AF_UNSPEC,
                              .ai_socktype = SOCK_DGRAM,
                              .ai_flags = AI_NUMERICHOST };

    int r = getaddrinfo(host, NULL, &hints, &addrs);
    if (r)
    {
        if (EAI_SYSTEM == r)
        {
            OIC_LOG_V(ERROR, TAG, "getaddrinfo failed: errno %s", strerror(errno));
        }
        else
        {
            OIC_LOG_V(ERROR, TAG, "getaddrinfo failed: %s", gai_strerror(r));
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

CAEndpoint_t *CACloneEndpoint(const CAEndpoint_t *rep)
{
    CAEndpoint_t *clone = (CAEndpoint_t *)OICMalloc(sizeof (CAEndpoint_t));

    *clone = *rep;
    return clone;
}

