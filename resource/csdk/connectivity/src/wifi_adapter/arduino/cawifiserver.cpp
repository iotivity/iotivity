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

#include "cawifiinterface_singlethread.h"

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <SPI.h>
#include <utility/server_drv.h>
#include <utility/wifi_drv.h>
#include <IPAddress.h>
#include <TimedAction.h>

#include "logger.h"
#include "cacommon.h"
#include "cainterface.h"
#include "caadapterinterface.h"
#include "cawifiadapter_singlethread.h"
#include "caadapterutils.h"
#include "oic_malloc.h"

#define COAP_MAX_PDU_SIZE 320
#define MOD_NAME "WiFiServer"

char ssid[] = "NETGEAR99";         // your network SSID (name)
char pass[] = "jollysky325";            // your network password
int16_t status = WL_IDLE_STATUS;    // the Wifi radio's status

// Length of the IP address decimal notation string
#define IPNAMESIZE (16)

// Start offsets based on end of received data buffer
#define WIFI_RECBUF_IPADDR_OFFSET  (6)
#define WIFI_RECBUF_PORT_OFFSET    (2)

#define WIFI_RECBUF_IPADDR_SIZE    (WIFI_RECBUF_IPADDR_OFFSET - WIFI_RECBUF_PORT_OFFSET)
#define WIFI_RECBUF_PORT_SIZE      (WIFI_RECBUF_PORT_OFFSET - 0)
#define WIFI_RECBUF_FOOTER_SIZE    (WIFI_RECBUF_IPADDR_SIZE + WIFI_RECBUF_PORT_SIZE)

static CAResult_t CAArduinoInitUdpSocket(int16_t *port, int32_t *socketID);
static int32_t CAArduinoRecvData(int32_t sockFd, uint8_t *buf, uint32_t bufLen,
                                 uint8_t *senderAddr, uint16_t *senderPort);
static CAResult_t CAArduinoGetInterfaceAddress(char *address, int32_t addrLen);
static void CAArduinoCheckData();
void CAPacketReceivedCallback(const char *ipAddress, const uint32_t port,
                              const void *data, const uint32_t dataLength);

static CAWiFiPacketReceivedCallback gPacketReceivedCallback = NULL;
static int32_t gUnicastSocket = 0;
static bool gServerRunning = false;
static TimedAction gRcvAction = TimedAction(3000, CAArduinoCheckData);
static WiFiUDP Udp;

CAResult_t CAWiFiInitializeServer(void)
{
    return CA_STATUS_OK;
}

void CAWiFiTerminateServer(void)
{

}

CAResult_t CAWiFiGetUnicastServerInfo(char **ipAddress, int *port, int32_t *serverID)
{
    return CA_STATUS_OK;
}

CAResult_t CAArduinoInitUdpSocket(int16_t *port, int32_t *socketID)
{
    OIC_LOG(DEBUG, MOD_NAME, "IN");
    VERIFY_NON_NULL(port, MOD_NAME, "port");
    VERIFY_NON_NULL(socketID, MOD_NAME, "socketID");

    uint8_t sock;
    //Is any socket available to work with ?
    *socketID = -1;

    sock = WiFiClass::getSocket();
    if (sock != NO_SOCKET_AVAIL)
    {
        *socketID = (int32_t)sock;
        OIC_LOG_V(ERROR, MOD_NAME, "socket: %d", *socketID);
    }
    else
    {
        OIC_LOG(DEBUG, MOD_NAME, "Error");
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, MOD_NAME, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAWiFiStartUnicastServer(const char *localAddress, int16_t *port,
                                    const bool forceStart, int32_t *serverFD)
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
    Udp.begin((uint16_t ) *port);

    // start thread to monitor socket here
    if (!gServerRunning)
    {
        gRcvAction.enable();
        gServerRunning = true;
    }
    OIC_LOG(DEBUG, MOD_NAME, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAWiFiStartMulticastServer(const char *localAddress, const char *multicastAddress,
                                      const int16_t multicastPort, int32_t *serverFD)
{
    // wifi shield do not support multicast
    OIC_LOG(DEBUG, MOD_NAME, "IN");
    OIC_LOG(DEBUG, MOD_NAME, "IN");
    return CA_NOT_SUPPORTED;
}

CAResult_t CAWiFiStopUnicastServer()
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

CAResult_t CAWiFiStopMulticastServer()
{
    return CAWiFiStopUnicastServer();
}

void CAPacketReceivedCallback(const char *ipAddress, const uint32_t port,
                              const void *data, const uint32_t dataLength)
{
    OIC_LOG(DEBUG, MOD_NAME, "notifyCallback Entry");
    if (gPacketReceivedCallback)
    {
        gPacketReceivedCallback(ipAddress, port, data, dataLength);
        OIC_LOG(DEBUG, MOD_NAME, "Notified network packet");
    }
    OIC_LOG(DEBUG, MOD_NAME, "notifyCallback Exit");
}

void CAArduinoCheckData()
{
    OIC_LOG(DEBUG, MOD_NAME, "CAACD Being called");
    char *data = (char *)OICMalloc(COAP_MAX_PDU_SIZE);
    if (NULL == data)
    {
        return;
    }
    int32_t dataLen = 0;
    char addr[IPNAMESIZE] = {0};
    uint16_t senderPort = 0;
    int16_t packetSize = Udp.parsePacket();
    OIC_LOG_V(DEBUG, MOD_NAME, "Rcv packet of size:%d ", packetSize);
    senderPort = Udp.remotePort();
    OIC_LOG_V(DEBUG, MOD_NAME, "senderport: %d", senderPort);
    if (packetSize)
    {
        IPAddress remoteIp = Udp.remoteIP();
        senderPort = Udp.remotePort();
        OIC_LOG_V(DEBUG, MOD_NAME, "senderport: %d", senderPort);
        sprintf(addr, "%d.%d.%d.%d", remoteIp[0], remoteIp[1], remoteIp[2], remoteIp[3]);
        OIC_LOG_V(DEBUG, MOD_NAME, "remoteip: %s, port: %d", addr, senderPort);
        // read the packet into packetBufffer
        int16_t len = Udp.read(data, COAP_MAX_PDU_SIZE);
        if (len > 0)
        {
            data[len] = 0;
        }
        CAPacketReceivedCallback(addr, senderPort, data, dataLen);
    }
    OICFree(data);
}

/// Retrieve any available data from UDP socket. This is a non-blocking call.
int32_t CAArduinoRecvData(int32_t sockFd, uint8_t *buf, uint32_t bufLen, uint8_t *senderAddr,
                          uint16_t *senderPort)
{
    OIC_LOG(DEBUG, MOD_NAME, "arduinoRecvData Entry");
    /**Bug : When there are multiple UDP packets in Wiznet buffer, W5100.getRXReceivedSize
     * will not return correct length of the first packet.
     * Fix : Use the patch provided for arduino/libraries/Ethernet/utility/socket.cpp
     */
    //int32_t ret = 0;
    uint16_t recvLen = 0;

    VERIFY_NON_NULL(buf, MOD_NAME, "Invalid buf");
    VERIFY_NON_NULL(senderAddr, MOD_NAME, "Invalid senderAddr");
    VERIFY_NON_NULL(senderPort, MOD_NAME, "Invalid senderPort");

    OIC_LOG(DEBUG, MOD_NAME, "arduinoRecvData Begin");
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
    OIC_LOG(DEBUG, MOD_NAME, "arduinoRecvData End");
    return (int32_t)recvLen;
}

void CAWiFiSetPacketReceiveCallback(CAWiFiPacketReceivedCallback callback)
{
    OIC_LOG(DEBUG, MOD_NAME, "CAWSPRC Entry");
    gPacketReceivedCallback = callback;
    OIC_LOG(DEBUG, MOD_NAME, "CAWSPRC Exit");
}

void CAWiFiSetExceptionCallback(CAWiFiExceptionCallback callback)
{
    // TODO
}

void CAWiFiPullData()
{
    gRcvAction.check();
}

/// Retrieves the IP address assigned to Arduino Ethernet shield
CAResult_t CAArduinoGetInterfaceAddress(char *address, int32_t addrLen)
{
    OIC_LOG(DEBUG, MOD_NAME, "IN");
    // WiFiClass WiFi;
    if (WiFi.status() == WL_NO_SHIELD)
    {
        OIC_LOG(DEBUG, MOD_NAME, "WIFI SHIELD NOT PRESENT");
        return CA_STATUS_FAILED;
    }

    while ( status != WL_CONNECTED)
    {
        OIC_LOG_V(ERROR, MOD_NAME, "Attempting to connect to WPA SSID: %s", ssid);
        status = WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network:

        // wait 10 seconds for connection:
        delay(10000);
        OIC_LOG(DEBUG, MOD_NAME, "Attempting connection again");
    }

    VERIFY_NON_NULL(address, MOD_NAME, "Invalid address");
    if (addrLen < IPNAMESIZE)
    {
        OIC_LOG_V(ERROR, MOD_NAME, "arduinoGetInterfaceAddress: addrLen MUST be atleast %d", IPNAMESIZE);
        return CA_STATUS_FAILED;
    }

    IPAddress ip = WiFi.localIP();
    sprintf((char *)address, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

    OIC_LOG_V(DEBUG, MOD_NAME, "Wifi shield address is: %s", address);
    OIC_LOG(DEBUG, MOD_NAME, "arduinoGetInterfaceAddress::Exit");
    return CA_STATUS_OK;
}


