/*

Copyright (c) 2012-2014 RedBearLab

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

/*
 *    HelloWorldl
 *
 *    HelloWorld sketch, work with the Chat iOS/Android App.
 *    It will send "Hello World" string to the App every 1 sec.
 *
 */

#ifdef __ARDUINO_BLE__
//"services.h/spi.h/boards.h" is needed in every new project
#include <SPI.h>
#include <boards.h>
#include <RBL_nRF8001.h>
#include <services.h>

#define PCF(str) ((PROGMEM const char *)(F(str)))
#define printf Serial.println
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "Arduino.h"
#include "caadapterinterface.h"
#include "cacommon.h"
#include "caleadapter_singlethread.h"
#include <TimedAction.h>
#include <logger.h>

char localBTAddress[20] = "DB:F7:EB:B5:0F:07"; //BT Address is not used internally by OIC API.
int number = 0;
char coapData[30] = "Arduino Test Sample :  \0";

CAConnectivityHandler_t *gConnectivityHandlers = NULL;
CALocalConnectivity_t localEndpoint;
CARemoteEndpoint_t remoteMulticastEndpoint;
CARemoteEndpoint_t remoteEndpoint[2];

int CAInterfaceInitializeEndpoint()
{
    int i = 0;
    /* As of initializing one endpoint */
    for (i = 1; i < /*10*/2; i++)
    {
        if (i == 0)
        {
            /*
            remoteEndpoint[i].connectivityType = WiFi;
            remoteEndpoint[i].endpoint_info = (Address*) malloc (sizeof(Address));
            if (NULL == remoteEndpoint[i].endpoint_info)
            {
              return -1;
            }
              memset(remoteEndpoint[i].endpoint_info, 0, sizeof(Address));
            remoteEndpoint[i].endpoint_info->address = (char*) malloc (sizeof(remoteIPAddress) + 1);
            if (NULL == remoteEndpoint[i].endpoint_info->address)
            {
              return -1;
            }
            memset((void*)remoteEndpoint[i].endpoint_info->address, 0, sizeof(remoteIPAddress) + 1);
            strncpy((char*)remoteEndpoint[i].endpoint_info->address, remoteIPAddress, strlen(remoteIPAddress));
            remoteEndpoint[i].endpoint_info->port = 8254; // Send the corresponding port here
                      remoteMulticastEndpoint.connectivityType = WiFi;
                      */
        }
        else if (i == 1) //BLE
        {
            /*localEndpoint.endpoint_info = (Address*) malloc (sizeof(Address));
            if (NULL == localEndpoint.endpoint_info)
            {
            return -1;
            }
            memset(localEndpoint.endpoint_info, 0, sizeof(Address));
            localEndpoint.interface.address = (char*) malloc (sizeof(localBTAddress) + 1);
            if (NULL == localEndpoint.interface.address)
            {
            return -1;
            }
            memset((void*)localEndpoint.interface.address, 0, sizeof(localBTAddress) + 1);
            strncpy((char*)localEndpoint.interface.address, localBTAddress, strlen(localBTAddress));*/

            remoteEndpoint[i].connectivityType = CA_LE;
            remoteMulticastEndpoint.connectivityType = CA_LE;
        }

    }
    return true;
}


void CAInterfaceRegisterCallback(CAConnectivityHandler_t handler , CAConnectivityType_t cType)
{
    //printf("\ninterfaceRegisterCallback Entry in Sample");
    gConnectivityHandlers = (CAConnectivityHandler_t *) malloc(sizeof(CAConnectivityHandler_t));
    //gConnectivityHandlers->cType = cType;
    gConnectivityHandlers->startListenServer = handler.startListenServer;
    gConnectivityHandlers->terminate = handler.terminate;
    gConnectivityHandlers->sendData = handler.sendData;
    gConnectivityHandlers->sendDataToAll = handler.sendDataToAll;
    gConnectivityHandlers->GetnetInfo = handler.GetnetInfo;
    gConnectivityHandlers->readData = handler.readData;
    gConnectivityHandlers->startListenServer();
    printf("Control comes here");
}

void CARequestResponseHandler(CARemoteEndpoint_t *object, void *data, uint32_t dataLength)
{
#ifdef INTERFACESAMPLE_TIZEN
    printf("\nData Received from");
    printf("\nReceived Data : %s \n", (char *)data);
#else
    printf("\nData Received from ");
    //printf(object->endpointAddress.address);
    printf("\nReceived Data :");
    printf((char *)data);
#endif //#ifdef INTERFACESAMPLE_TIZEN

}


void CANetworkInterfaceCallback(CALocalConnectivity_t *localEndPoint,
                                CANetworkStatus_t networkConnectivityState)
{
    printf("\nnetworkInterfaceCallback Entry in Sample");
    /*if (localEndPoint == NULL)
    {
        printf("\nNULL Object");
    }
    if (localEndPoint->interface.address){
           printf("Local Interface Address");
           printf(localEndPoint->interface.address);
      }

    if (networkConnectivityState == INTERFACE_UP)
      printf("\n WiFi Network Status is UP");
    else
      printf("\n WiFi Network Status is DOWN");*/
    printf("\nnetworkInterfaceCallback Exit in Sample");
}


void setup()
{
    Serial.begin (115200);
    CAInterfaceInitializeEndpoint();
    CAInitializeLE(CAInterfaceRegisterCallback, CARequestResponseHandler, CANetworkInterfaceCallback);
}
unsigned char buf[16] = {0};
unsigned char len = 0;

void loop()
{
    if (Serial.available() > 0)
    {
        Serial.println("Serial available.");
        switch (Serial.read())
        {
            case 's':
                {
                    Serial.println("sending data");
                    if ( ble_connected() )
                    {
                        Serial.println("Sending Data");
                        gConnectivityHandlers->sendData(&remoteEndpoint[1], coapData, strlen(coapData));
                        Serial.println("Sent Data");
                    }
                }
                break;
        }
    }
    gConnectivityHandlers->readData();
}
#else
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
//#include <fcntl.h>
#include <errno.h>

#include "Arduino.h"

#include "cawifiadapter_singlethread.h"

#include <logger.h>
#include "caadapterinterface.h"
//#include "cawifiethernetadapter.h"
#include "cacommon.h"
#include <TimedAction.h>

#define MOD_NAME "ArduinoSample"
static CALocalConnectivity_t *localWifiEndpoint = NULL;
static CARemoteEndpoint_t remoteMulticastEndpoint;
static CARemoteEndpoint_t remoteEndpoint[10]; /* 10 RemoteEndpoints are currently kept */
static char remoteIPAddress[CA_IPADDR_SIZE] =
    "192.168.1.105";  //Change the receiver IP address during testing.
static char readIpAddress[20];
//Hardcoded coap data for Test
static char coapData[500] =
    "{\"oc:\[{href\":\"/a/light\",\"ref\":{\"power\":\"20\",\"state\":\"true\"}}]}";

typedef struct ConnectivityHandlerList
{
    CAConnectivityType_t type;
    CAConnectivityHandler_t handler;
    struct ConnectivityHandlerList *nextHandler;
} ConnectivityHandlerList;

static ConnectivityHandlerList *gConnectivityHandlers = NULL;

int16_t CAInterfaceInitializeEndpoint()
{
    int16_t i = 0;
    /* As of initializing one endpoint */
    for (i = 0; i < 1; i++)
    {
        remoteEndpoint[i].connectivityType = CA_WIFI;
        memset((void *)remoteEndpoint[i].addressInfo.IP.ipAddress, 0, sizeof(remoteIPAddress) + 1);
        strncpy((char *)remoteEndpoint[i].addressInfo.IP.ipAddress, remoteIPAddress,
                strlen(remoteIPAddress));
        remoteEndpoint[i].addressInfo.IP.port = 5683; /* Send the corresponding port here */
    }

    remoteMulticastEndpoint.connectivityType = CA_WIFI;
    return true;
}

void CAStoreInterfaceCallbacks(ConnectivityHandlerList *newHandler)
{
    OIC_LOG(DEBUG, MOD_NAME, "storeInterfaceCallbacks Entry in Sample");
    newHandler->nextHandler = NULL;
    ConnectivityHandlerList *tempConnectivityHandlers = gConnectivityHandlers;

    if (!tempConnectivityHandlers)
    {
        gConnectivityHandlers = newHandler;
        OIC_LOG(DEBUG, MOD_NAME, "storeInterfaceCallbacks Exit in Sample");
        return;
    }

    while (tempConnectivityHandlers->nextHandler)
    {
        tempConnectivityHandlers = tempConnectivityHandlers->nextHandler;
    }

    tempConnectivityHandlers->nextHandler = newHandler;
    OIC_LOG(DEBUG, MOD_NAME, "storeInterfaceCallbacks Exit in Sample");
}

void CAInterfaceRegisterCallback(CAConnectivityHandler_t handler,
                                 CAConnectivityType_t connType)
{
    OIC_LOG(DEBUG, MOD_NAME, "interfaceRegisterCallback Entry in Sample");
    ConnectivityHandlerList *newConnectivityHandler = (ConnectivityHandlerList *) malloc(sizeof(
                ConnectivityHandlerList));
    if (NULL == newConnectivityHandler)
    {
        printf("Failed to allocate memory !\n");
        return;
    }

    newConnectivityHandler->type = connType;
    newConnectivityHandler->handler = handler;
    CAStoreInterfaceCallbacks(newConnectivityHandler);
    OIC_LOG(DEBUG, MOD_NAME, "interfaceRegisterCallback Exit in Sample");
}

void CANetworkPacketHandler(CARemoteEndpoint_t *object, void *data, uint32_t dataLength)
{
    OIC_LOG(DEBUG, MOD_NAME, "networkPacketHandler Entry in Sample");
    if (object == NULL || data == NULL)
    {
        printf("\nNULL Object");
        return;
    }

    OIC_LOG(DEBUG, MOD_NAME, "Data Received from ");
    OIC_LOG_V(DEBUG, MOD_NAME, object->addressInfo.IP.ipAddress);
    OIC_LOG(DEBUG, MOD_NAME, "Received Data :");
    OIC_LOG_V(DEBUG, MOD_NAME, (char *)data);
    OIC_LOG(DEBUG, MOD_NAME, "networkPacketHandler Exit in Sample");
}

void CANetworkInterfaceCallback(CALocalConnectivity_t *localEndPoint,
                                CANetworkStatus_t networkConnectivityState)
{
    OIC_LOG(DEBUG, MOD_NAME, "networkInterfaceCallback Entry in Sample");
    if (localEndPoint == NULL)
    {
        OIC_LOG(DEBUG, MOD_NAME, "NULL Object");
        return;
    }

    OIC_LOG(DEBUG, MOD_NAME, "Local Interface Address: ");
    OIC_LOG(DEBUG, MOD_NAME, localEndPoint->addressInfo.IP.ipAddress);

    if (networkConnectivityState == CA_INTERFACE_UP)
    {
        OIC_LOG(DEBUG, MOD_NAME, " WiFi Network Status is UP");
    }
    else
    {
        OIC_LOG(DEBUG, MOD_NAME, " WiFi Network Status is DOWN");
    }
    OIC_LOG(DEBUG, MOD_NAME, "networkInterfaceCallback Exit in Sample");
}


void CAFreeData(void *data)
{
    OIC_LOG(DEBUG, MOD_NAME, "freeData Entry in Sample");
    if (data)
    {
        free(data);
    }
    OIC_LOG(DEBUG, MOD_NAME, "freeData Exit in Sample");
}

int16_t CASelectConnectivityType()
{
    int16_t cType;
    OIC_LOG(DEBUG, MOD_NAME, "*******Select the Connectivity Type*******");
    OIC_LOG(DEBUG, MOD_NAME, "  [1] WiFi ");
    OIC_LOG(DEBUG, MOD_NAME, "  [2] BT ");
    OIC_LOG(DEBUG, MOD_NAME, "  [3] BLE ");
    cType = 1;
    return cType;
}

int16_t CAInterfaceStartServer(CAConnectivityType_t connType)
{
    OIC_LOG(DEBUG, MOD_NAME, "interfaceStartServer Starting...");
    ConnectivityHandlerList *tempConnectivityHandlers = gConnectivityHandlers;
    if (NULL == tempConnectivityHandlers)
    {
        OIC_LOG(DEBUG, MOD_NAME, "None of the interface is initialized");
        return 0;
    }
    while (tempConnectivityHandlers && tempConnectivityHandlers->type == connType)
    {
        if (connType == CA_WIFI)
        {
            OIC_LOG(DEBUG, MOD_NAME, "Connection Type = WiFi.");
            uint32_t size = 0;
            tempConnectivityHandlers->handler.GetnetInfo(&localWifiEndpoint, &size);
            OIC_LOG(DEBUG, MOD_NAME, "tempConnectivityHandlers ->NetInfoCallback");
            localWifiEndpoint->addressInfo.IP.port = 5683;
            OIC_LOG(DEBUG, MOD_NAME, "LocalWiFiEndPoint Port is 5683");
            OIC_LOG(DEBUG, MOD_NAME, "Local Address = ");
            OIC_LOG(DEBUG, MOD_NAME, localWifiEndpoint->addressInfo.IP.ipAddress);
            tempConnectivityHandlers->handler.startAdapter();
            tempConnectivityHandlers->handler.startDiscoverServer();
        }
        tempConnectivityHandlers = tempConnectivityHandlers->nextHandler;
    }
    return true;
}

int16_t CAInterfaceStopServer(CAConnectivityType_t connType)
{
    OIC_LOG(DEBUG, MOD_NAME, "interfaceStopServer Starting...");
    ConnectivityHandlerList *tempConnectivityHandlers = gConnectivityHandlers;
    if (NULL == tempConnectivityHandlers)
    {
        OIC_LOG(DEBUG, MOD_NAME, "None of the interface is initialized");
        return 0;
    }
    while (tempConnectivityHandlers && tempConnectivityHandlers->type == connType)
    {
        tempConnectivityHandlers->handler.terminate();
        tempConnectivityHandlers = tempConnectivityHandlers->nextHandler;
    }
    return true;
}

int16_t CAInterfaceSendUnicastData(CAConnectivityType_t connType)
{
    OIC_LOG(DEBUG, MOD_NAME, "PLEASE ENTER THE IP ADDRESS ");
    while (!Serial.available());
    strncpy(readIpAddress, (Serial.readStringUntil('\n')).c_str(), 20);
    strncpy((char *)remoteEndpoint[0].addressInfo.IP.ipAddress, readIpAddress,
            strlen(readIpAddress));
    remoteEndpoint[0].addressInfo.IP.port = 5683;

    ConnectivityHandlerList *tempConnectivityHandlers = gConnectivityHandlers;
    if (NULL == tempConnectivityHandlers)
    {
        OIC_LOG(DEBUG, MOD_NAME, " None of the interface is initialized ");
        return 0;
    }
    while (tempConnectivityHandlers && tempConnectivityHandlers->type == connType)
    {
        tempConnectivityHandlers->handler.sendData(&remoteEndpoint[0], coapData, strlen(coapData));
        tempConnectivityHandlers = tempConnectivityHandlers->nextHandler;
    }
    return true;
}

int16_t CAInterfaceSendMulticastData(CAConnectivityType_t connType)
{
    ConnectivityHandlerList *tempConnectivityHandlers = gConnectivityHandlers;
    if (NULL == tempConnectivityHandlers)
    {
        OIC_LOG(DEBUG, MOD_NAME, "None of the interface is initialized");
        return 0;
    }
    while (tempConnectivityHandlers && tempConnectivityHandlers->type == connType)
    {
        if (connType == CA_WIFI)
        {
            tempConnectivityHandlers->handler.sendDataToAll(coapData, strlen(coapData));
        }
        else if (connType == CA_EDR || connType == CA_LE)
        {
            //iterate all endpoints, check ConnectivityType for BT, BLE and call SendDataToAll
        }

        tempConnectivityHandlers = tempConnectivityHandlers->nextHandler;
    }
    return true;
}

void CATestInitializeInterface()
{
    OIC_LOG(DEBUG, MOD_NAME, "testInitializeInterface Entry");
    int16_t interface = CASelectConnectivityType();
    OIC_LOG_V(DEBUG, MOD_NAME, "Interface : %d", interface);
    if (interface == 0)
    {
        return;
    }

    if (1 == interface)   /* WIFI */
    {
        CAInitializeWifi(CAInterfaceRegisterCallback, CANetworkPacketHandler,
                         CANetworkInterfaceCallback);
        CAInterfaceStartServer(CA_WIFI);
    }
    else if (2 == interface)     /*BT*/
    {
    }
    else if (3 == interface)     /*BLE*/
    {
    }

    OIC_LOG(DEBUG, MOD_NAME, "testInitializeInterface Exit");
}

void CATestTerminateInterface()
{
    int16_t interface = CASelectConnectivityType();
    if (interface == 0)
    {
        return;
    }
    if (1 == interface)   /* WIFI */
    {
        CAInterfaceStopServer(CA_WIFI);
        //CATerminateWifi();
    }
    else if (2 == interface)     /*BT*/
    {
    }
    else if (3 == interface)     /*BLE*/
    {
    }

    /* Freeing the allocated Memory */
    //freeInterfaceMemory((void*)localWifiEndpoint->interface.address);
    //freeInterfaceMemory(localWifiEndpoint->endpoint_info);
    CAFreeData(localWifiEndpoint);
}

void CATestSendUnicastData()
{
    int16_t interface = CASelectConnectivityType();
    if (interface == 0)
    {
        return;
    }
    if (1 == interface)   /* WIFI */
    {
        CAInterfaceSendUnicastData(CA_WIFI);
    }
    else if (2 == interface)     /*BT*/
    {
    }
    else if (3 == interface)     /*BLE*/
    {
    }
}

void CATestSendMulticastData()
{
    int16_t interface = CASelectConnectivityType();
    if (interface == 0)
    {
        return;
    }

    if (1 == interface)   /* WIFI */
    {
        CAInterfaceSendMulticastData(CA_WIFI);
    }
    else if (2 == interface)     /*BT*/
    {
    }
    else if (3 == interface)     /*BLE*/
    {
    }
}

static void CATestPrintHelp(void)
{
    OIC_LOG(DEBUG, MOD_NAME, " =====================================================================");
    OIC_LOG(DEBUG, MOD_NAME, "|                 Welcome to Connectivity Abstraction                 |");
    OIC_LOG(DEBUG, MOD_NAME, "|                   - CA Unit Test v1.0 -                             |");
    OIC_LOG(DEBUG, MOD_NAME, "|---------------------------------------------------------------------|");
    OIC_LOG(DEBUG, MOD_NAME, "|                           ** Options **                             |");
    OIC_LOG(DEBUG, MOD_NAME, "|  i - Initialize the Interface                                       |");
    OIC_LOG(DEBUG, MOD_NAME, "|  d - Terminate the Interface                                        |");
    OIC_LOG(DEBUG, MOD_NAME, "|  s - Send Unicast Data                                              |");
    OIC_LOG(DEBUG, MOD_NAME, "|  m - Send Multicast Data                                            |");
    OIC_LOG(DEBUG, MOD_NAME, "|  x - quit the test.                                                 |");
    OIC_LOG(DEBUG, MOD_NAME, "|  r - Print the menu again.                                          |");
    OIC_LOG(DEBUG, MOD_NAME, " =====================================================================");
}

void loop()
{
    delay(2000);
    if (gConnectivityHandlers)
    {
        gConnectivityHandlers->handler.readData();
    }
    if (Serial.available() > 0)
    {
        OIC_LOG(DEBUG, MOD_NAME, "Serial available.");
        switch (Serial.read())
        {
            case 'i':
                OIC_LOG(DEBUG, MOD_NAME, "Initializing Interface.");
                CATestInitializeInterface();
                break;
            case 'x':
                OIC_LOG(DEBUG, MOD_NAME, "Terminating Interface.");
                CATestTerminateInterface();
                break;
            case 'd':
                OIC_LOG(DEBUG, MOD_NAME, "Terminating Interface.");
                CATestTerminateInterface();
                break;
            case 's':
                CATestSendUnicastData();
                break;
            case 'm':
                CATestSendMulticastData();
                break;
            case 'r':
                CATestPrintHelp();
                break;
        }
    }
}

void setup()
{
    //OC_LOG_INIT();
    Serial.begin(115200);
    OIC_LOG(DEBUG, MOD_NAME, "Setup Started.");
    CAInterfaceInitializeEndpoint();
    OIC_LOG(DEBUG, MOD_NAME, "After initializing endpoints.");
    CATestPrintHelp();
}

#endif //#ifdef __ARDUINO_BLE__
