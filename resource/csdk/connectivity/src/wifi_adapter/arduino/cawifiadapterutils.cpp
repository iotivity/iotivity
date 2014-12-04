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

#include "cawifiadapterutils.h"

#include <Arduino.h>
#include <WiFi.h>
#include <SPI.h>
#include <utility/server_drv.h>
#include <utility/wifi_drv.h>
#include <IPAddress.h>
#include <TimedAction.h>

#include "logger.h"
#include "cacommon.h"
#include "cainterface.h"
#include "caadapterinterface.h"
#include "caadapterutils.h"
#include "oic_malloc.h"

#define COAP_MAX_PDU_SIZE 320
#define MOD_NAME "WAU"

char ssid[] = "network_ssid";     //  your network SSID (name)
char pass[] = "network_pswd";  // your network password

int16_t status = WL_IDLE_STATUS;     // the Wifi radio's status

/// Length of the IP address decimal notation string
#define IPNAMESIZE (16)
/// This is the max buffer size between Arduino and WiFi Shield
#define ARDUINO_WIFI_SPI_RECV_BUFFERSIZE (64)

// Start offsets based on end of received data buffer
#define WIFI_RECBUF_IPADDR_OFFSET  (6)
#define WIFI_RECBUF_PORT_OFFSET    (2)

#define WIFI_RECBUF_IPADDR_SIZE    (WIFI_RECBUF_IPADDR_OFFSET - WIFI_RECBUF_PORT_OFFSET)
#define WIFI_RECBUF_PORT_SIZE      (WIFI_RECBUF_PORT_OFFSET - 0)
#define WIFI_RECBUF_FOOTER_SIZE    (WIFI_RECBUF_IPADDR_SIZE + WIFI_RECBUF_PORT_SIZE)

static CANetworkPacketReceivedCallback gNetworkPacketCallback;
static int32_t gUnicastSocket = 0;
static bool gServerRunning = false;
static TimedAction gRcvAction = TimedAction(3000, CACheckData);
static WiFiUDP Udp;

CAResult_t CAArduinoInitUdpSocket(const int16_t *port, int32_t *socketID)
{
    uint8_t sock;
    OIC_LOG(DEBUG, MOD_NAME, "IN");
    VERIFY_NON_NULL(port, MOD_NAME, "port");
    VERIFY_NON_NULL(socketID, MOD_NAME, "socket");

    //Is any socket available to work with ?
    *socketID = -1;

    sock = WiFiClass::getSocket();
    if (sock != NO_SOCKET_AVAIL)
    {
        *socketID = (int32_t)sock;
        OIC_LOG_V(ERROR, MOD_NAME, "Set SOCKETID=%d", *socketID);
    }
    else
    {
        OIC_LOG(DEBUG, MOD_NAME, "Error");
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, MOD_NAME, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAStartUnicastServer(const char *localAddress, const int16_t *port)
{
    OIC_LOG(DEBUG, MOD_NAME, "IN");
    VERIFY_NON_NULL(port, MOD_NAME, "port");

    // WiFiClass WiFi;
    if (WiFi.status() == WL_NO_SHIELD)
    {
        OIC_LOG(DEBUG, MOD_NAME, "Error");
        return CA_STATUS_FAILED;
    }

    while (status != WL_CONNECTED)
    {
        OIC_LOG_V(ERROR, MOD_NAME, "connecting: %s", ssid);
        status = WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network:

        // wait 10 seconds for connection:
        delay(10000);
    }

    char localIpAddress[CA_IPADDR_SIZE];
    int32_t localIpAddressLen = sizeof(localIpAddress);
    CAArduinoGetInterfaceAddress(localIpAddress, localIpAddressLen);
    OIC_LOG_V(DEBUG, MOD_NAME, "address: %s", localIpAddress);

    if (gServerRunning)
    {
        // already running
        OIC_LOG(DEBUG, MOD_NAME, "Error");
        return CA_STATUS_FAILED;
    }

    OIC_LOG_V(DEBUG, MOD_NAME, "port: %d", *port);
    Udp.begin((unsigned int16_t)*port);

    // start thread to monitor socket here
    if (!gServerRunning)
    {
        gRcvAction.enable();
        gServerRunning = true;
    }
    OIC_LOG(DEBUG, MOD_NAME, "OUT");
    return CA_STATUS_OK;
}

/// Retrieves the IP address assigned to Arduino Ethernet shield
CAResult_t CAArduinoGetInterfaceAddress(char *address, int32_t addrLen)
{
    OIC_LOG(DEBUG, MOD_NAME, "IN");
    VERIFY_NON_NULL(address, MOD_NAME, "address");
    if (addrLen < IPNAMESIZE)
    {
        OIC_LOG_V(ERROR, MOD_NAME, "min addrLen %d", IPNAMESIZE);
        return CA_STATUS_FAILED;
    }

    IPAddress ip = WiFi.localIP();
    sprintf(address, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

    OIC_LOG_V(DEBUG, MOD_NAME, "address: %s", address);
    OIC_LOG(DEBUG, MOD_NAME, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAStartMulticastServer(const char *mcastAddress, const char *localAddress, int16_t *port)
{
    // wifi shield do not support multicast
    OIC_LOG(DEBUG, MOD_NAME, "IN");
    if (gServerRunning)
    {
        // already running
        OIC_LOG(DEBUG, MOD_NAME, "failed");
        return CA_STATUS_FAILED;
    }
    return CAStartUnicastServer(localAddress, port);
}

CAResult_t CAStopUnicastServer()
{
    OIC_LOG(DEBUG, MOD_NAME, "IN");
    if (gUnicastSocket >= MAX_SOCK_NUM)
    {
        OIC_LOG(DEBUG, MOD_NAME, "Invalid sockfd");
        return CA_STATUS_FAILED;
    }

    ServerDrv::stopClient(gUnicastSocket);
    WiFiClass::_server_port[gUnicastSocket] = 0;
    WiFiClass::_state[gUnicastSocket] = NA_STATE;
    gUnicastSocket = 0;

    // Terminate server thread
    gRcvAction.disable();
    gServerRunning = false;
    OIC_LOG(DEBUG, MOD_NAME, "OUT");
    return CA_STATUS_OK;
}

void CAArduoinoCheckServerData()
{
    gRcvAction.check();
}

CAResult_t CAStopMulticastServer()
{
    return CAStopUnicastServer();
}

uint32_t CAWIFISendData(const char *remoteIpAddress, const int16_t port, const char *buf,
                        uint32_t bufLen,
                        int16_t isMulticast)
{
    OIC_LOG(DEBUG, MOD_NAME, "IN");
    int32_t ret = 1;
    OIC_LOG_V(DEBUG, MOD_NAME, "remoteip: %s, port: %d", remoteIpAddress, port);
    Udp.beginPacket(remoteIpAddress, port);
    ret = (int32_t)Udp.write((char *)buf);
    Udp.endPacket();
    OIC_LOG(DEBUG, MOD_NAME, "OUT");
    return ret;
}

void CACheckData()
{
    char *data = (char *)OICMalloc(COAP_MAX_PDU_SIZE);
    int32_t dataLen = 0;
    char addr[IPNAMESIZE] = {0};
    uint16_t senderPort = 0;
    int16_t packetSize = Udp.parsePacket();
    OIC_LOG_V(DEBUG, MOD_NAME, "Rcv packet of size:%d ", packetSize);
    if (packetSize)
    {
        IPAddress remoteIp = Udp.remoteIP();
        senderPort = Udp.remotePort();
        sprintf(addr, "%d.%d.%d.%d", remoteIp[0], remoteIp[1], remoteIp[2], remoteIp[3]);
        OIC_LOG_V(DEBUG, MOD_NAME, "remoteip: %s, port: %d", addr, senderPort);
        // read the packet into packetBufffer
        int16_t len = Udp.read(data, COAP_MAX_PDU_SIZE);
        if (len > 0)
        {
            data[len] = 0;
        }
        CANotifyCallback(data, len, addr, senderPort);
    }
    OICFree(data);
}

void CANotifyCallback(void *data, int32_t dataLen, char *senderIp, int32_t senderPort)
{
    OIC_LOG(DEBUG, MOD_NAME, "IN");
    if (gNetworkPacketCallback)
    {
        CARemoteEndpoint_t endPoint;
        endPoint.resourceUri = NULL;     // will be filled by upper layer
        endPoint.connectivityType = CA_WIFI;
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
    VERIFY_NON_NULL(buf, MOD_NAME, "buf");
    VERIFY_NON_NULL(senderAddr, MOD_NAME, "senderAddr");
    VERIFY_NON_NULL(senderPort, MOD_NAME, "senderPort");

    uint16_t recvLen = 0;
    if (sockFd >= MAX_SOCK_NUM)
    {
        OIC_LOG(ERROR, MOD_NAME, "Invalid sockfd");
        return -1;
    }

    recvLen = (int32_t)ServerDrv::availData((uint8_t)sockFd);
    if (recvLen == 0)
    {
        return recvLen;
    }

    // Make sure buf is large enough for received data
    if ((uint32_t)recvLen > bufLen)
    {
        OIC_LOG(ERROR, MOD_NAME, "Receive buffer too small");
        return -1;
    }

    if (!ServerDrv::getDataBuf((uint8_t)sockFd, (uint8_t *)buf, &recvLen))
    {
        OIC_LOG(ERROR, MOD_NAME, "getDataBuf error");
        return -1;
    }

    // Read IP Address and Port from end of receive buffer
    memcpy(senderAddr, &buf[recvLen - WIFI_RECBUF_IPADDR_OFFSET], WIFI_RECBUF_IPADDR_SIZE);
    // Change the endianness of the port number
    *((uint8_t *)senderPort) = buf[recvLen - (WIFI_RECBUF_PORT_OFFSET - 1)];
    *((uint8_t *)senderPort + 1) = buf[recvLen - (WIFI_RECBUF_PORT_OFFSET)];

    recvLen -= WIFI_RECBUF_FOOTER_SIZE;
    OIC_LOG(DEBUG, MOD_NAME, "OUT");
    return (int32_t)recvLen;
}


void CASetWIFINetworkPacketCallback(CANetworkPacketReceivedCallback callback)
{
    OIC_LOG(DEBUG, MOD_NAME, "IN");
    gNetworkPacketCallback = callback;
    OIC_LOG(DEBUG, MOD_NAME, "OUT");
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
