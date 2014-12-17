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
#include "caethernetadapterutils.h"

#include <Arduino.h>
#include <Ethernet.h>
#include <socket.h>
#include <w5100.h>
#include <EthernetUdp.h>
#include <IPAddress.h>

#include "logger.h"
#include "cacommon.h"
#include "caadapterinterface.h"
#include "caethernetadapter_singlethread.h"
#include "caadapterutils.h"

#define MOD_NAME "EU"

CAResult_t CAArduinoInitUdpSocket(int16_t *port, int32_t *socketID)
{
    OIC_LOG(DEBUG, MOD_NAME, "IN");
    VERIFY_NON_NULL(port, MOD_NAME, "port");
    VERIFY_NON_NULL(socketID, MOD_NAME, "socketID");

    uint8_t state;
    //Is any socket available to work with ?
    int32_t sockID = 0;
    for (int i = 1; i < MAX_SOCK_NUM; i++)
    {
        state = W5100.readSnSR(i);
        if (state == SnSR::CLOSED || state == SnSR::FIN_WAIT)
        {
            sockID = i;
            break;
        }
    }

    if (sockID == 0)
    {
        OIC_LOG(ERROR, MOD_NAME, "failed");
        return CA_STATUS_FAILED;
    }

    //Create a datagram socket on which to recv/send.
    if (!socket(sockID, SnMR::UDP, *port, 0))
    {
        OIC_LOG(ERROR, MOD_NAME, "failed");
        return CA_STATUS_FAILED;
    }

    *socketID = sockID;
    OIC_LOG_V(DEBUG, MOD_NAME, "socketId:%d", sockID);
    OIC_LOG(DEBUG, MOD_NAME, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAArduinoInitMulticastUdpSocket(const char *mcastAddress, const int16_t *mport,
        const int16_t *lport, int32_t *socketID)
{
    OIC_LOG(DEBUG, MOD_NAME, "IN");
    VERIFY_NON_NULL(mcastAddress, MOD_NAME, "address");
    VERIFY_NON_NULL(socketID, MOD_NAME, "socket");
    VERIFY_NON_NULL(lport, MOD_NAME, "port");
    VERIFY_NON_NULL(mport, MOD_NAME, "port");
    uint8_t state;
    uint8_t mcastMacAddr[] = { 0x01, 0x00, 0x5E, 0x00, 0x00, 0x00};
    uint8_t ipAddr[4] = { 0 };
    uint16_t parsedPort = 0;
    if (!CAParseIPv4AddressLocal((unsigned char *) mcastAddress, ipAddr, &parsedPort))
    {
        OIC_LOG(ERROR, MOD_NAME, "failed");
        return CA_STATUS_FAILED;
    }

    int32_t sockID = 0;
    //Is any socket available to work with ?
    for (int i = 1; i < MAX_SOCK_NUM; i++)
    {
        state = W5100.readSnSR(i);
        if (state == SnSR::CLOSED || state == SnSR::FIN_WAIT)
        {
            sockID = i;
            break;
        }
    }

    if (sockID == 0)
    {
        OIC_LOG(ERROR, MOD_NAME, "failed");
        return CA_STATUS_FAILED;
    }


    //Calculate Multicast MAC address
    mcastMacAddr[3] = ipAddr[1] & 0x7F;
    mcastMacAddr[4] = ipAddr[2];
    mcastMacAddr[5] = ipAddr[3];
    W5100.writeSnDIPR(sockID, (uint8_t *)ipAddr);
    W5100.writeSnDHAR(sockID, mcastMacAddr);
    W5100.writeSnDPORT(sockID, *mport);

    //Create a datagram socket on which to recv/send.
    if (!socket(sockID, SnMR::UDP, *lport, SnMR::MULTI))
    {
        OIC_LOG(ERROR, MOD_NAME, "failed");
        return CA_STATUS_FAILED;
    }

    *socketID = sockID;
    OIC_LOG_V(DEBUG, MOD_NAME, "socketId:%d", sockID);
    OIC_LOG(DEBUG, MOD_NAME, "OUT");
    return CA_STATUS_OK;
}

int16_t CAParseIPv4AddressLocal(unsigned char *ipAddrStr, uint8_t *ipAddr, uint16_t *port)
{
    size_t index = 0;
    unsigned char *itr;
    uint8_t dotCount = 0;

    ipAddr[index] = 0;
    *port = 0;
    itr = ipAddrStr;
    if (!isdigit((unsigned char) *ipAddrStr))
    {
        return -1;
    }
    ipAddrStr = itr;

    while (*ipAddrStr)
    {
        if (isdigit((unsigned char) *ipAddrStr))
        {
            ipAddr[index] *= 10;
            ipAddr[index] += *ipAddrStr - '0';
        }
        else if ((unsigned char) *ipAddrStr == '.')
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
            if (isdigit((unsigned char) *ipAddrStr))
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
    if (ipAddr[0] < 255 && ipAddr[1] < 255 && ipAddr[2] < 255 && ipAddr[3] < 255
        && dotCount == 3)
    {
        return 1;
    }
    return 0;
}


