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
#include <TimedAction.h>

#include "logger.h"
#include "cacommon.h"
#include "cainterface.h"
#include "caadapterinterface.h"
#include "caadapterutils.h"
#include "oic_malloc.h"

#define COAP_MAX_PDU_SIZE 320
#define MOD_NAME "EAU"

/// Length of the IP address decimal notation string
#define IPNAMESIZE (16)

static CANetworkPacketReceivedCallback gNetworkPacketCallback;
static int32_t gUnicastSocket = 0;
static int32_t gMulticastSocket = 0;
static bool gServerRunning = false;
static TimedAction gRcvAction = TimedAction(3000, CACheckData);

// Retrieves a empty socket and bind it for UDP with the input port
/**
 * @brief API to start unicast server.
 * @param localAddress - Local Unicast IP address to bind the socket.
 * @param port - Local port number where socket will listen for incoming request.
 * @return - Error Code
 */
CAResult_t CAStartUnicastServer(const char *localAddress, int16_t *port)
{
    OIC_LOG(DEBUG, MOD_NAME, "IN");
    VERIFY_NON_NULL(port, MOD_NAME, "port");

    // Note: ****Update the MAC address here with your shield's MAC address****
    uint8_t ETHERNET_MAC[] = {0x90, 0xA2, 0xDA, 0x0E, 0xC4, 0x05};
    uint8_t error = Ethernet.begin(ETHERNET_MAC);
    if (error  == 0)
    {
        OIC_LOG_V(ERROR, MOD_NAME, "cant connect:%d", error);
        return CA_STATUS_FAILED;
    }
    OIC_LOG(DEBUG, MOD_NAME, "success");

    uint8_t rawIPAddr[4];
    char address[16];
    W5100.getIPAddress(rawIPAddr);
    sprintf(address, "%d.%d.%d.%d", rawIPAddr[0], rawIPAddr[1], rawIPAddr[2], rawIPAddr[3]);
    OIC_LOG_V(DEBUG, MOD_NAME, "address:%s", address);

    if (CAArduinoInitUdpSocket(port, &gUnicastSocket) != CA_STATUS_OK)
    {
        OIC_LOG(DEBUG, MOD_NAME, "failed");
        return CA_STATUS_FAILED;
    }

    // start thread to monitor socket here
    if (!gServerRunning)
    {
        gRcvAction.enable();
        gServerRunning = true;
    }
    OIC_LOG(DEBUG, MOD_NAME, "OUT");
    return CA_STATUS_OK;
}

// Retrieves the IP address assigned to Arduino Ethernet shield
CAResult_t CAArduinoGetInterfaceAddress(char *address, int32_t addrLen)
{
    OIC_LOG(DEBUG, MOD_NAME, "IN");
    VERIFY_NON_NULL(address, MOD_NAME, "address");

    //TODO : Fix this for scenarios when this API is invoked when device is not connected
    uint8_t rawIPAddr[4];
    if (addrLen < IPNAMESIZE)
    {
        OIC_LOG(ERROR, MOD_NAME, "Invalid addrLen");
        return CA_STATUS_FAILED;
    }

    W5100.getIPAddress(rawIPAddr);
    sprintf(address, "%d.%d.%d.%d", rawIPAddr[0], rawIPAddr[1], rawIPAddr[2], rawIPAddr[3]);

    OIC_LOG_V(DEBUG, MOD_NAME, "address:%s", address);
    OIC_LOG(DEBUG, MOD_NAME, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAArduinoInitUdpSocket(int16_t *port, int32_t *socketID)
{
    OIC_LOG(DEBUG, MOD_NAME, "IN");
    VERIFY_NON_NULL(socketID, MOD_NAME, "socket");
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

CAResult_t CAArduinoInitMulticastUdpSocket(const char *mcastAddress, const int16_t *port,
        int32_t *socketID)
{
    OIC_LOG(DEBUG, MOD_NAME, "IN");
    VERIFY_NON_NULL(mcastAddress, MOD_NAME, "address");
    VERIFY_NON_NULL(socketID, MOD_NAME, "socket");
    VERIFY_NON_NULL(port, MOD_NAME, "port");
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
    sockID = -1;
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
    W5100.writeSnDPORT(sockID, *port);

    //Create a datagram socket on which to recv/send.
    if (!socket(sockID, SnMR::UDP, *port, SnMR::MULTI))
    {
        OIC_LOG(ERROR, MOD_NAME, "failed");
        return CA_STATUS_FAILED;
    }

    *socketID = sockID;
    OIC_LOG_V(DEBUG, MOD_NAME, "socketId:%d", sockID);
    OIC_LOG(DEBUG, MOD_NAME, "OUT");
    return CA_STATUS_OK;
}

/**
 * @brief API to start multicast server.
 * @param mcastAddress - IP address to join multicast group.
 * @param localAddress - Local Unicast IP address to bind the socket.
 * @param port - multicast port number where socket will listen for incoming request.
 * @return - Error Code
 */
CAResult_t CAStartMulticastServer(const char *mcastAddress, const char *localAddress,
                                  const int16_t *port)
{
    OIC_LOG(DEBUG, MOD_NAME, "IN");
    if (CAArduinoInitMulticastUdpSocket(mcastAddress, port, &gMulticastSocket) != CA_STATUS_OK)
    {
        OIC_LOG(DEBUG, MOD_NAME, "failed");
        return CA_STATUS_FAILED;
    }

    // start thread to monitor socket here
    if (!gServerRunning)
    {
        gRcvAction.enable();
        gServerRunning = true;
    }

    OIC_LOG(DEBUG, MOD_NAME, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAStopUnicastServer()
{
    OIC_LOG(DEBUG, MOD_NAME, "IN");
    // terminate server thread
    // Stop thread if both server stopped
    if (gMulticastSocket == 0)
    {
        gRcvAction.disable();
        gServerRunning = false;
    }

    close(gUnicastSocket);
    gUnicastSocket = 0;
    OIC_LOG(DEBUG, MOD_NAME, "OUT");
    return CA_STATUS_OK;
}

void CAArduoinoCheckServerData()
{
    gRcvAction.check();
}

CAResult_t CAStopMulticastServer()
{
    OIC_LOG(DEBUG, MOD_NAME, "IN");
    // terminate server thread
    // Stop thread if both server stopped
    if (gUnicastSocket == 0)
    {
        gRcvAction.disable();
        gServerRunning = false;
    }

    close(gMulticastSocket);
    gMulticastSocket = 0;
    OIC_LOG(DEBUG, MOD_NAME, "OUT");
    return CA_STATUS_OK;
}

uint32_t CAEthernetSendData(const char *remoteIpAddress, const int16_t port, const char *buf,
                            uint32_t bufLen,
                            int16_t isMulticast)
{
    OIC_LOG(DEBUG, MOD_NAME, "IN");
    int32_t socketID = 0;
    if (isMulticast)
    {
        if (CAArduinoInitMulticastUdpSocket(remoteIpAddress, &port, &socketID) != CA_STATUS_OK)
        {
            OIC_LOG(ERROR, MOD_NAME, "multicast");
            return 0;
        }
    }
    else
    {
        if (CAArduinoInitUdpSocket((int16_t *)&port, &socketID) != CA_STATUS_OK)
        {
            OIC_LOG(ERROR, MOD_NAME, "unicast");
            return 0;
        }
    }
    OIC_LOG(DEBUG, MOD_NAME, "OUT");
    return CAArduinoSendData(socketID, (uint8_t *)buf, bufLen, remoteIpAddress, port);
}

/// Send data to requested end-point using UDP socket
uint32_t CAArduinoSendData(int32_t sockFd, const uint8_t *buf, uint32_t bufLen,
                           const char *ipAddress,
                           int32_t port)
{
    OIC_LOG(DEBUG, MOD_NAME, "IN");
    uint32_t ret;
    VERIFY_NON_NULL_RET(buf, MOD_NAME, "buffer", 0);
    VERIFY_NON_NULL_RET(ipAddress, MOD_NAME, "address", 0);

    uint8_t ipAddr[4] = { 0 };
    uint16_t parsedPort = 0;
    if (!CAParseIPv4AddressLocal((unsigned char *) ipAddress, ipAddr, &parsedPort))
    {
        OIC_LOG(ERROR, MOD_NAME, "failed");
        return 0;
    }

    ret = sendto(sockFd, buf, bufLen, (uint8_t *)ipAddr, port);
    delay(10);
    close(sockFd);
    OIC_LOG(DEBUG, MOD_NAME, "OUT");
    return ret;
}

void CACheckData()
{
    void *data = OICMalloc(COAP_MAX_PDU_SIZE);
    memset(data, 0, COAP_MAX_PDU_SIZE);
    int32_t dataLen = 0;
    uint8_t senderAddr[4] = { 0 };
    char addr[IPNAMESIZE] = {0};
    uint16_t senderPort = 0;

    if (gUnicastSocket)
    {
        dataLen = CAArduinoRecvData(gUnicastSocket, (uint8_t *)data, COAP_MAX_PDU_SIZE, senderAddr,
                                    &senderPort);
        if (dataLen < 0)
        {
            OIC_LOG(ERROR, MOD_NAME, "unicast");
            CAStopUnicastServer();
        }
        else if (dataLen > 0)
        {
            OIC_LOG(DEBUG, MOD_NAME, "unicast data rcvd");
            sprintf(addr, "%d.%d.%d.%d", senderAddr[0], senderAddr[1], senderAddr[2], senderAddr[3]);
            CANotifyCallback(data, dataLen, addr, senderPort);
        }
    }

    if (gMulticastSocket)
    {
        dataLen = CAArduinoRecvData(gMulticastSocket, (uint8_t *)data, COAP_MAX_PDU_SIZE, senderAddr,
                                    &senderPort);
        if (dataLen < 0)
        {
            OIC_LOG(ERROR, MOD_NAME, "multicast");
            CAStopMulticastServer();
        }
        else if (dataLen > 0)
        {
            OIC_LOG(DEBUG, MOD_NAME, "multicast data recvd");
            sprintf(addr, "%d.%d.%d.%d", senderAddr[0], senderAddr[1], senderAddr[2], senderAddr[3]);
            CANotifyCallback(data, dataLen, addr, senderPort);
        }
    }
    OICFree(data);
}

void CANotifyCallback(void *data, int32_t dataLen, const char *senderIp, int32_t senderPort)
{
    OIC_LOG(DEBUG, MOD_NAME, "IN");
    if (gNetworkPacketCallback)
    {
        CARemoteEndpoint_t endPoint;
        endPoint.resourceUri = NULL;     // will be filled by upper layer
        endPoint.connectivityType = CA_ETHERNET;
        strncpy(endPoint.addressInfo.IP.ipAddress, senderIp, strlen(senderIp));
        endPoint.addressInfo.IP.port = senderPort;
        gNetworkPacketCallback(&endPoint, data, dataLen);
    }
    OIC_LOG(DEBUG, MOD_NAME, "OUT");
}

/// Retrieve any available data from UDP socket. This is a non-blocking call.
int32_t CAArduinoRecvData(int32_t sockFd, uint8_t *buf, uint32_t bufLen, uint8_t *senderAddr,
                          uint16_t *senderPort)
{
    OIC_LOG(DEBUG, MOD_NAME, "IN");
    /**Bug : When there are multiple UDP packets in Wiznet buffer, W5100.getRXReceivedSize
     * will not return correct length of the first packet.
     * Fix : Use the patch provided for arduino/libraries/Ethernet/utility/socket.cpp
     */
    VERIFY_NON_NULL_RET(buf, MOD_NAME, "buffer", 0);
    VERIFY_NON_NULL_RET(senderAddr, MOD_NAME, "address", 0);
    VERIFY_NON_NULL_RET(senderPort, MOD_NAME, "port", 0);
    int32_t ret = 0;
    uint16_t recvLen = W5100.getRXReceivedSize(sockFd);
    if (recvLen == 0)
    {
        OIC_LOG_V(DEBUG, MOD_NAME, "rcvd %d", recvLen);
        return recvLen;
    }

    // Read available data.
    ret = recvfrom(sockFd, buf, bufLen, senderAddr, senderPort);
    OIC_LOG(DEBUG, MOD_NAME, "OUT");
    return ret;
}

void CASetEthernetNetworkPacketCallback(CANetworkPacketReceivedCallback callback)
{
    OIC_LOG(DEBUG, MOD_NAME, "IN");
    gNetworkPacketCallback = callback;
    OIC_LOG(DEBUG, MOD_NAME, "OUT");
}

int16_t CAParseIPv4AddressLocal(unsigned char *ipAddrStr, uint8_t *ipAddr, uint16_t *port)
{
    OIC_LOG(DEBUG, MOD_NAME, "IN");
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
    OIC_LOG(DEBUG, MOD_NAME, "OUT");
    return 0;
}
