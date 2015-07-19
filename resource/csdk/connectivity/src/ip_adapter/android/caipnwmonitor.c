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

#include "caipinterface.h"

#include <sys/types.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <linux/if.h>

#include "caadapterutils.h"
#include "logger.h"
#include "oic_malloc.h"
#include "oic_string.h"

#define TAG "IP_MONITOR"

char *getHostIPAddress(const char *ifa_name) {
    static char address[INET_ADDRSTRLEN] = {};
    memset(&address, 0, INET_ADDRSTRLEN);
    struct ifreq ifr;
    int sck, status, len = sizeof(ifr.ifr_name) - 1;
    char *ip;

    if ((sck = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return NULL;
    }

    strncpy(ifr.ifr_name, ifa_name, len);
    ifr.ifr_name[len] = '\0';
    if ((status = ioctl(sck, SIOCGIFADDR, &ifr)) < 0) {
        close(sck);
        return NULL;
    }
    close(sck);
    ip = inet_ntoa(((struct sockaddr_in *)(&ifr.ifr_addr))->sin_addr);
    len = sizeof(address) - 1;
    strncpy(address, ip, len);
    address[len] = '\0';
    return address;
}

u_arraylist_t *CAIPGetInterfaceInformation(int desiredIndex)
{
    u_arraylist_t *iflist = u_arraylist_create();
    if (!iflist)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to create iflist: %s", strerror(errno));
        return NULL;
    }

    char* ipAddr = getHostIPAddress("wlan0");
    if (NULL == ipAddr)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to get ifaddrs: %s", strerror(errno));
        u_arraylist_destroy(iflist);
        return NULL;
    } 
    OIC_LOG_V(DEBUG, TAG, "Got ifaddrs:: %s", ipAddr);

    struct in_addr inaddr;
    inet_pton(AF_INET, ipAddr, &(inaddr));

    CAInterface_t *ifitem = (CAInterface_t *)OICCalloc(1, sizeof(CAInterface_t));;
    OICStrcpy(ifitem->name, INTERFACE_NAME_MAX, "wlan0");
    ifitem->index = 0; //if_nametoindex("wlan0");
    ifitem->family = AF_INET; //we support ipv4 only
    ifitem->ipv4addr = inaddr.s_addr;
    ifitem->flags = IFF_UP|IFF_RUNNING;

    CAResult_t result = u_arraylist_add(iflist, ifitem);
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, TAG, "u_arraylist_add failed.");
        goto exit;
    }

    OIC_LOG_V(ERROR, TAG, "Added interface: %s (%d)", ifitem->name, ifitem->family);

    return iflist;

exit:
    u_arraylist_destroy(iflist);
    return NULL;
}

