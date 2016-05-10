//******************************************************************
//
// Copyright 2016 Intel Corporation All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

/*
 * global structure
 *
 * This structure gathers all globals to one place.
 */

#ifndef OC_GLOBALS_H_
#define OC_GLOBALS_H_

#include "occonfig.h"
#include "occonstants.h"
#include "ocenums.h"

/**
 * Hold global variables
 */
typedef struct
{
    int fd;        /**< socket fd */
    uint16_t port; /**< socket port */
} CASocket_t;

#define HISTORYSIZE (4)

typedef struct
{
    OCTransportFlags flags;
    uint16_t messageId;
} CAHistoryItem_t;

typedef struct
{
    int nextIndex;
    CAHistoryItem_t items[HISTORYSIZE];
} CAHistory_t;

typedef struct
{
    int ifindex; /**< network interface index */
} CAIfItem_t;

typedef struct
{
    OCTransportFlags clientFlags; /**< flag for client */
    OCTransportFlags serverFlags; /**< flag for server */
    bool client; /**< client mode */
    bool server; /**< server mode */

    struct sockets
    {
        void *threadpool;   /**< threadpool between Initialize and Start */
        CASocket_t u6;      /**< unicast   IPv6 */
        CASocket_t u6s;     /**< unicast   IPv6 secure */
        CASocket_t u4;      /**< unicast   IPv4 */
        CASocket_t u4s;     /**< unicast   IPv4 secure */
        CASocket_t m6;      /**< multicast IPv6 */
        CASocket_t m6s;     /**< multicast IPv6 secure */
        CASocket_t m4;      /**< multicast IPv4 */
        CASocket_t m4s;     /**< multicast IPv4 secure */
        int netlinkFd;      /**< netlink */
        int shutdownFds[2]; /**< shutdown pipe */
        int selectTimeout;  /**< in seconds */
        int maxfd;          /**< highest fd (for select) */
        bool started;       /**< the IP adapter has started */
        bool terminate;     /**< the IP adapter needs to stop */
        bool ipv6enabled;   /**< IPv6 enabled by OCInit flags */
        bool ipv4enabled;   /**< IPv4 enabled by OCInit flags */
        bool dualstack;     /**< IPv6 and IPv4 enabled */

        struct networkmonitors
        {
            CAIfItem_t *ifitems; /**< current network interface index list */
            int numifitems;      /**< number of current network interfaces */
        } nm;
    } ip;

    struct calayer
    {
        CAHistory_t requestHistory;  /**< filter IP family in requests */
        CAHistory_t responseHistory; /**< filter IP family in responses */
        void *transportAdapters[MAX_ADAPTERS]; /**< adapter function vectors */
        bool randomSeeded;           /**< has RNG been seeded? */
    } ca;
} CAGlobals_t;

extern CAGlobals_t caglobals;

#endif // OC_GLOBALS_H_
