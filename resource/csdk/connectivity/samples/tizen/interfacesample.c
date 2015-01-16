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

//***************************************************************
//
// This file is for internal test only.
// Do NOT use this file to test CA on Tizen.
//
//***************************************************************

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <wifi.h>
#include <glib.h>

#include "cacommon.h"
#include "caadapterinterface.h"
#include "uthreadpool.h"

#define MOD_NAME "TizenSample"

#if defined(WIFI_ADAPTER_TEST)
#include "cawifiadapter.h"
static u_thread_pool_t gWiFiThreadPool = NULL;
#endif
#if defined(BT_ADAPTER_TEST)
#include "caedradapter.h"
static  u_thread_pool_t gBTThreadPool = NULL;
#endif
#if defined(BLE_ADAPTER_TEST)
#include "caleadapter.h"
static u_thread_pool_t gLEThreadPool = NULL;
#endif

static GMainLoop *mainloop = NULL;
static GIOChannel *channel = NULL;
static guint g_test_io_watch_id = 0;
static GError *g_err_Sample = NULL;

//Hardcoded coap data for Test
static char coapData[500] =
    "{\"oc:\[{href\":\"/a/light\",\"ref\":{\"power\":\"20\",\"state\":\"true\"}}]}";

void testInitializeBTInterface(void);
void testTerminateBTInterface(void);
void testInitializeWIFIInterface(void);
void testTerminateWIFIInterface(void);
void testInitializeBLEInterface(void);
void testTerminateBLEInterface(void);

typedef struct ConnectivityHandlerList
{
    CAConnectivityType_t type;
    CAConnectivityHandler_t handler;
    struct ConnectivityHandlerList *nextHandler;
} ConnectivityHandlerList;

static ConnectivityHandlerList *gConnectivityHandlers = NULL;

void initializeThreadPool(CAConnectivityType_t type)
{
#ifdef BT_ADAPTER_TEST
    if (CA_EDR == type && NULL == gBTThreadPool)
    {
        if (CA_STATUS_OK != u_thread_pool_init(3, &gBTThreadPool))
        {
            printf("Failed to create thread pool for BT adapter!\n");
            return;
        }
    }
#endif
#ifdef WIFI_ADAPTER_TEST
    if (CA_WIFI == type && NULL == gWiFiThreadPool)
    {
        if (CA_STATUS_OK != u_thread_pool_init(3, &gWiFiThreadPool))
        {
            printf("Failed to create thread pool for WIFI adapter!\n");
            return;
        }
    }
#endif
#ifdef BLE_ADAPTER_TEST
    if (CA_LE == type && NULL == gLEThreadPool)
    {
        if (CA_STATUS_OK != u_thread_pool_init(5, &gLEThreadPool))
        {
            printf("Failed to create thread pool for BLE adapter!\n");
            return;
        }
    }
#endif
}


void storeInterfaceCallbacks(ConnectivityHandlerList *newHandler)
{
    printf("storeInterfaceCallbacks Entry in Sample\n");
    newHandler->nextHandler = NULL;
    ConnectivityHandlerList *tempConnectivityHandlers = gConnectivityHandlers;

    if (!tempConnectivityHandlers)
    {
        gConnectivityHandlers = newHandler;
        printf("storeInterfaceCallbacks Exit in Sample\n");
        return;
    }

    while (tempConnectivityHandlers->nextHandler)
    {
        tempConnectivityHandlers = tempConnectivityHandlers->nextHandler;
    }

    tempConnectivityHandlers->nextHandler = newHandler;
    printf("storeInterfaceCallbacks Exit in Sample\n");
}

void interfaceRegisterCallback(CAConnectivityHandler_t handler,
                               CAConnectivityType_t connType)
{
    printf("interfaceRegisterCallback Entry in Sample\n");
    ConnectivityHandlerList *newConnectivityHandler = (ConnectivityHandlerList *) malloc(sizeof(
                ConnectivityHandlerList));
    if (NULL == newConnectivityHandler)
    {
        printf("Memory allocation failed\n");
        return;
    }

    newConnectivityHandler->type = connType;
    newConnectivityHandler->handler = handler;
    storeInterfaceCallbacks(newConnectivityHandler);
    printf("interfaceRegisterCallback Exit in Sample\n");
}

void networkPacketHandler(CARemoteEndpoint_t *object, void *data, uint32_t dataLength)
{
    printf("networkPacketHandler Entry in Sample\n");
    if (object == NULL || data == NULL)
    {
        printf("NULL Object\n");
        return;
    }

    printf("Data Received from: ");
    if (CA_EDR == object->connectivityType)
    {
        printf(object->addressInfo.BT.btMacAddress);
    }
    else if (CA_LE == object->connectivityType)
    {
        printf(object->addressInfo.LE.leMacAddress);
    }
    else if (CA_WIFI == object->connectivityType || CA_ETHERNET == object->connectivityType)
    {
        printf(object->addressInfo.IP.ipAddress);
    }

    printf("\nReceived Data [Length: %d]: %s\n", dataLength, (char *)data);
    printf("networkPacketHandler Exit in Sample\n");
}

void networkInterfaceCallback(CALocalConnectivity_t *localEndPoint,
                              CANetworkStatus_t networkConnectivityState)
{
    printf("networkInterfaceCallback Entry in Sample\n");
    if (localEndPoint == NULL)
    {
        printf("NULL Object\n");
        return;
    }

    if (networkConnectivityState == CA_INTERFACE_UP)
    {
        printf("Network Status is UP\n");
    }
    else
    {
        printf("Network Status is DOWN\n");
    }

    printf("Address: ");
    if (CA_EDR == localEndPoint->type)
    {
        printf("%s\n", localEndPoint->addressInfo.BT.btMacAddress);
    }
    else if (CA_LE == localEndPoint->type)
    {
        printf("%s\n", localEndPoint->addressInfo.LE.leMacAddress);
    }
    else if (CA_WIFI == localEndPoint->type || CA_ETHERNET == localEndPoint->type)
    {
        printf("%s\n", localEndPoint->addressInfo.IP.ipAddress);
    }

    printf("networkInterfaceCallback Exit in Sample\n");
}


void freeData(void *data)
{
    printf("freeData Entry in Sample\n");
    if (data)
    {
        free(data);
    }
    printf("freeData Exit in Sample\n");
}

int16_t selectConnectivityType()
{
    int32_t cType;
    printf("*******Select the Connectivity Type*******\n");
    printf("  [1] WIFI \n");
    printf("  [2] BT \n");
    printf("  [3] BLE \n");

    fflush(stdin);
    scanf("%d", &cType);
    if (cType < 1 && cType > 3)
    {
        printf("Invalid Selection!!!!\n");
        return 0;
    }
    return (int16_t)cType;
}

int16_t interfaceStartAdapter(CAConnectivityType_t connType)
{
    ConnectivityHandlerList *tempConnectivityHandlers = gConnectivityHandlers;
    if (NULL == tempConnectivityHandlers)
    {
        printf("None of the interface is initialized\n");
        return 0;
    }

    while (tempConnectivityHandlers && tempConnectivityHandlers->type != connType)
    {
        tempConnectivityHandlers = tempConnectivityHandlers->nextHandler;
    }

    if (NULL == tempConnectivityHandlers)
    {
        printf( "No interface handler for type %d", connType);
        return 0;
    }

    if (CA_STATUS_OK != tempConnectivityHandlers->handler.startAdapter())
    {
        printf("Failed to Start adapter\n");
        return 0;
    }

    return 1;
}

int16_t interfaceMulticastStartServer(CAConnectivityType_t connType, int serverType)
{
    printf("interfaceMulticastStartServer Starting...\n");
    ConnectivityHandlerList *tempConnectivityHandlers = gConnectivityHandlers;
    if (NULL == tempConnectivityHandlers)
    {
        printf("None of the interface is initialized\n");
        return 0;
    }

    while (tempConnectivityHandlers && tempConnectivityHandlers->type != connType)
    {
        tempConnectivityHandlers = tempConnectivityHandlers->nextHandler;
    }

    if (NULL == tempConnectivityHandlers)
    {
        printf( "No interface handler for type %d", connType);
        return 0;
    }

    CAAdapterStartDiscoveryServer startServer = NULL;
    switch (serverType)
    {
        case 1: //Discovery server
            startServer = tempConnectivityHandlers->handler.startDiscoverServer;
            break;
        case 2: //Listening server
            startServer = tempConnectivityHandlers->handler.startListenServer;
            break;
    }

    if (startServer)
    {
        printf("Invoking start server method\n");
        startServer();
    }
    return 0;
}

int16_t interfaceSendUnicastData(CAConnectivityType_t connType)
{
    ConnectivityHandlerList *tempConnectivityHandlers = gConnectivityHandlers;
    if (NULL == tempConnectivityHandlers)
    {
        printf(" None of the interface is initialized \n");
        return 0;
    }

    while (tempConnectivityHandlers && tempConnectivityHandlers->type != connType)
    {
        tempConnectivityHandlers = tempConnectivityHandlers->nextHandler;
    }

    if (NULL == tempConnectivityHandlers)
    {
        printf( "No interface handler for type %d", connType);
        return 0;
    }

    if (CA_WIFI == connType)
    {
        CARemoteEndpoint_t endpoint;
        char remoteIPAddress[CA_IPADDR_SIZE] = {0};
        printf("\nEnter the Remote Endpoint IP: ");
        scanf("%s", remoteIPAddress);
        if (strlen(remoteIPAddress) == 0)
        {
            printf("Invalid device address\n");
            return -1;
        }
        endpoint.connectivityType = CA_WIFI;
        strncpy(endpoint.addressInfo.IP.ipAddress, remoteIPAddress, CA_IPADDR_SIZE);
        endpoint.addressInfo.IP.port = 5683; /* Send the corresponding port here */

        int sdatalen = tempConnectivityHandlers->handler.sendData(&endpoint, coapData,
                       strlen(coapData));
        if (sdatalen == strlen(coapData))
        {
            printf("Send Unicast data success\n");
        }
        else
        {
            printf("Send Unicast data failed\n");
        }
    }
    else if (CA_EDR == connType)
    {
        //create endpoint
        CARemoteEndpoint_t endpoint;

        //Get the device address from user
        char deviceaddress[100] = {0};
        printf("Enter the device address: \n");
        scanf("%s", deviceaddress);

        if (strlen(deviceaddress) == 0)
        {
            printf("Invlaid device address\n");
            return -1;
        }

        endpoint.connectivityType = CA_EDR;
        strncpy(endpoint.addressInfo.BT.btMacAddress, deviceaddress, CA_MACADDR_SIZE - 1);
        endpoint.addressInfo.BT.btMacAddress[CA_MACADDR_SIZE - 1] = '\0';
        endpoint.resourceUri = NULL;

        printf("Sent Unicast data to device: %s\n", endpoint.addressInfo.BT.btMacAddress);
        tempConnectivityHandlers->handler.sendData(&endpoint, coapData, strlen(coapData) + 1);
    }
    else if (CA_LE == connType)
    {
        //create endpoint
        CARemoteEndpoint_t endpoint;

        //Get the device address from user
        char deviceaddress[100] = {0};
        printf("Enter the device address: \n");
        scanf("%s", deviceaddress);

        if (strlen(deviceaddress) == 0)
        {
            printf("Invlaid device address\n");
            return -1;
        }

        //Get the service uuid from user
        char uuid[100] = {0};
        printf("Enter the service uuid: \n");
        scanf("%s", uuid);

        if (strlen(uuid) == 0)
        {
            printf("Invlaid service uuid\n");
            return -1;
        }

        endpoint.connectivityType = CA_LE;
        strncpy(endpoint.addressInfo.LE.leMacAddress, deviceaddress, CA_MACADDR_SIZE - 1);
        endpoint.addressInfo.LE.leMacAddress[CA_MACADDR_SIZE - 1] = '\0';
        endpoint.resourceUri = strdup(uuid);

        tempConnectivityHandlers->handler.sendData(&endpoint, coapData, strlen(coapData));
        printf("Sent Unicast data \n");
        free(endpoint.resourceUri);
    }

    return 1;
}

int16_t interfaceSendMulticastData(CAConnectivityType_t connType)
{
    ConnectivityHandlerList *tempConnectivityHandlers = gConnectivityHandlers;
    if (NULL == tempConnectivityHandlers)
    {
        printf("None of the interface is initialized\n");
        return 0;
    }

    while (tempConnectivityHandlers && tempConnectivityHandlers->type != connType)
    {
        tempConnectivityHandlers = tempConnectivityHandlers->nextHandler;
    }

    if (NULL == tempConnectivityHandlers)
    {
        printf( "No interface handler for type %d", connType);
        return 0;
    }

    if (connType == CA_WIFI)
    {
        tempConnectivityHandlers->handler.sendDataToAll(coapData,
                strlen(coapData) + 1);
    }
    else if (connType == CA_EDR || connType == CA_LE)
    {
        tempConnectivityHandlers->handler.sendDataToAll(coapData, strlen(coapData));
    }
    return 0;
}

void interfaceReadData(CAConnectivityType_t connType)
{
    ConnectivityHandlerList *tempConnectivityHandlers = gConnectivityHandlers;
    if (NULL == tempConnectivityHandlers)
    {
        printf("None of the interface is initialized\n");
        return;
    }

    while (tempConnectivityHandlers && tempConnectivityHandlers->type != connType)
    {
        tempConnectivityHandlers = tempConnectivityHandlers->nextHandler;
    }

    if (NULL == tempConnectivityHandlers)
    {
        printf( "No interface handler for type %d", connType);
        return;
    }

    if (CA_STATUS_OK != tempConnectivityHandlers->handler.readData())
    {
        printf("Failed to Read Data\n");
        return;
    }

    printf("Read Data is successfull\n");
    return;
}

void interfaceGetNetworkInfo(CAConnectivityType_t connType)
{
    int i = 0;

    ConnectivityHandlerList *tempConnectivityHandlers = gConnectivityHandlers;
    if (NULL == tempConnectivityHandlers)
    {
        printf("None of the interface is initialized\n");
        return;
    }

    while (tempConnectivityHandlers && tempConnectivityHandlers->type != connType)
    {
        tempConnectivityHandlers = tempConnectivityHandlers->nextHandler;
    }

    if (NULL == tempConnectivityHandlers)
    {
        printf( "No interface handler for type %d", connType);
        return;
    }

    //Get the network interface info
    CALocalConnectivity_t *info = NULL;
    uint32_t size = 0;
    if (CA_STATUS_OK != tempConnectivityHandlers->handler.GetnetInfo(&info, &size))
    {
        printf("Failed to get network info\n");
        return;
    }

    if (0 >= size || info == NULL)
    {
        printf("No network found !!!\n");
        return;
    }

    printf("Network Information: \n");
    for (; i < size; i++)
    {
        if (connType == CA_WIFI)
        {
            printf("Type : %s\n", (connType == CA_ETHERNET) ? "CA_ETHERNET" : "CA_WIFI");
            printf("Address : %s\n", info[i].addressInfo.IP.ipAddress);
        }
        else
        {
            printf("Type : %s\n", (connType == CA_EDR) ? "CA_EDR" : "CA_LE");
            printf("Address : %s\n\n", info[i].addressInfo.BT.btMacAddress);
        }
    }
}

int16_t interfaceStopAdapter(CAConnectivityType_t connType)
{
    ConnectivityHandlerList *tempConnectivityHandlers = gConnectivityHandlers;
    if (NULL == tempConnectivityHandlers)
    {
        printf("None of the interface is initialized\n");
        return 0;
    }

    while (tempConnectivityHandlers && tempConnectivityHandlers->type != connType)
    {
        tempConnectivityHandlers = tempConnectivityHandlers->nextHandler;
    }

    if (NULL == tempConnectivityHandlers)
    {
        printf( "No interface handler for type %d", connType);
        return 0;
    }

    if (CA_STATUS_OK != tempConnectivityHandlers->handler.stopAdapter())
    {
        printf("Failed to Stop adapter\n");
        return 0;
    }

    printf("Stopped the adapter\n");
    return 1;
}

void testInitializeInterface()
{
    printf("testInitializeInterface Entry\n");

    int16_t type = selectConnectivityType();
    if (0 >= type || 3 < type)
    {
        printf("Invalid selection...\n");
        return;
    }

    switch (type)
    {
#ifdef WIFI_ADAPTER_TEST
        case 1: //WIFI
            {
                testInitializeWIFIInterface();
            }
            break;
#endif
#ifdef BT_ADAPTER_TEST
        case 2:   //BT
            {
                testInitializeBTInterface();
            }
            break;
#endif
#ifdef BLE_ADAPTER_TEST
        case 3: //BLE
            {
                testInitializeBLEInterface();
            }
            break;
#endif
        default:
            printf("Feature is not enabled or not implemented\n");
    }
}

void testTerminateInterface()
{
    int16_t type = selectConnectivityType();
    if (0 >= type || 3 < type)
    {
        printf("Invalid selection...\n");
        return;
    }

#ifdef WIFI_ADAPTER_TEST
    if (1 == type)   /* WIFI */
    {
        testTerminateWIFIInterface();
    }
#endif
#ifdef BT_ADAPTER_TEST
    if (2 == type)   /*BT*/
    {
        testTerminateBTInterface();
    }
#endif
#ifdef BLE_ADAPTER_TEST
    if (3 == type)   /*BLE*/
    {
        testTerminateBLEInterface();
    }
#endif

    ConnectivityHandlerList *currentConnectivityHandler = gConnectivityHandlers;
    ConnectivityHandlerList *prevConnectivityHandler = NULL;

    printf("Linked list delete start\n");
    while (currentConnectivityHandler != NULL)
    {
        printf("Iterating through the list to find the matching interface\n");
        if (currentConnectivityHandler->type == type)
        {
            printf("Matching interface found\n");
            if (prevConnectivityHandler == NULL)
            {
                currentConnectivityHandler = currentConnectivityHandler->nextHandler;
                freeData(gConnectivityHandlers);
                gConnectivityHandlers = NULL;
                printf( "Node deleted with interface type %d", type);
            }
            else
            {
                prevConnectivityHandler->nextHandler = currentConnectivityHandler->nextHandler;
                freeData(currentConnectivityHandler);
                currentConnectivityHandler = prevConnectivityHandler->nextHandler;
                printf( "Node deleted with interface type %d from linked list", type);
            }
        }
        else
        {
            prevConnectivityHandler = currentConnectivityHandler;
            currentConnectivityHandler = currentConnectivityHandler->nextHandler;
        }
    }
    gConnectivityHandlers = NULL;
    return;
}

void testStartAdapter()
{
    int type = selectConnectivityType();
    if (0 >= type || 3 < type)
    {
        printf("Invalid selection...\n");
        return;
    }

    switch (type)
    {
#ifdef WIFI_ADAPTER_TEST
        case 1: //WIFI
            {
                interfaceStartAdapter(CA_WIFI);
            }
            break;
#endif
#ifdef BT_ADAPTER_TEST
        case 2:   //BT
            {
                interfaceStartAdapter(CA_EDR);
            }
            break;
#endif
#ifdef BLE_ADAPTER_TEST
        case 3: //BLE
            {
                interfaceStartAdapter(CA_LE);
            }
            break;
#endif
        default:
            printf("Feature is not enabled or not implemented\n");
    }
}

void testStartServer(int serverType)
{
    int type = selectConnectivityType();
    if (0 >= type || 3 < type)
    {
        printf("Invalid selection...\n");
        return;
    }

    switch (type)
    {
#ifdef WIFI_ADAPTER_TEST
        case 1: //WIFI
            {
                interfaceMulticastStartServer(CA_WIFI, serverType);
            }
            break;
#endif
#ifdef BT_ADAPTER_TEST
        case 2:   //BT
            {
                interfaceMulticastStartServer(CA_EDR, serverType);
            }
            break;
#endif
#ifdef BLE_ADAPTER_TEST
        case 3: //BLE
            {
                interfaceMulticastStartServer(CA_LE, serverType);
            }
            break;
#endif
        default:
            printf("Feature is not enabled or not implemented\n");
    }
}

void testSendUnicastData()
{
    int16_t type = selectConnectivityType();
    if (0 >= type || 3 < type)
    {
        printf("Invalid selection...\n");
        return;
    }

    switch (type)
    {
#ifdef WIFI_ADAPTER_TEST
        case 1: //WIFI
            {
                interfaceSendUnicastData(CA_WIFI);
            }
            break;
#endif
#ifdef BT_ADAPTER_TEST
        case 2:   //BT
            {
                interfaceSendUnicastData(CA_EDR);
            }
            break;
#endif
#ifdef BLE_ADAPTER_TEST
        case 3: //BLE
            {
                interfaceSendUnicastData(CA_LE);
            }
            break;
#endif
        default:
            printf("Feature is not enabled or not implemented\n");
    }
}

void testSendMulticastData()
{
    int16_t type = selectConnectivityType();
    if (0 >= type || 3 < type)
    {
        printf("Invalid selection...\n");
        return;
    }

    switch (type)
    {
#ifdef WIFI_ADAPTER_TEST
        case 1: //WIFI
            {
                interfaceSendMulticastData(CA_WIFI);
            }
            break;
#endif
#ifdef BT_ADAPTER_TEST
        case 2:   //BT
            {
                interfaceSendMulticastData(CA_EDR);
            }
            break;
#endif
#ifdef BLE_ADAPTER_TEST
        case 3: //BLE
            {
                interfaceSendMulticastData(CA_LE);
            }
            break;
#endif
        default:
            printf("Feature is not enabled or not implemented\n");
    }
}

void testReadData(void)
{
    int16_t type = selectConnectivityType();
    if (0 >= type || 3 < type)
    {
        printf("Invalid selection...\n");
        return;
    }

    switch (type)
    {
#ifdef WIFI_ADAPTER_TEST
        case 1: //WIFI
            {
                interfaceReadData(CA_WIFI);
            }
            break;
#endif
#ifdef BT_ADAPTER_TEST
        case 2:   //BT
            {
                interfaceReadData(CA_EDR);
            }
            break;
#endif
#ifdef BLE_ADAPTER_TEST
        case 3: //BLE
            {
                interfaceReadData(CA_LE);
            }
            break;
#endif
        default:
            printf("Feature is not enabled or not implemented\n");
    }
}

void testGetNetworkInfo(void)
{
    int16_t type = selectConnectivityType();
    if (0 >= type || 3 < type)
    {
        printf("Invalid selection...\n");
        return;
    }

    switch (type)
    {
#ifdef WIFI_ADAPTER_TEST
        case 1: //WIFI
            {
                interfaceGetNetworkInfo(CA_WIFI);
            }
            break;
#endif
#ifdef BT_ADAPTER_TEST
        case 2:   //BT
            {
                interfaceGetNetworkInfo(CA_EDR);
            }
            break;
#endif
#ifdef BLE_ADAPTER_TEST
        case 3: //BLE
            {
                interfaceGetNetworkInfo(CA_LE);
            }
            break;
#endif
        default:
            printf("Feature is not enabled or not implemented\n");
    }
}

void testStopAdapter()
{
    int16_t type = selectConnectivityType();
    if (0 >= type || 3 < type)
    {
        printf("Invalid selection...\n");
        return;
    }

    switch (type)
    {
#ifdef WIFI_ADAPTER_TEST
        case 1: //WIFI
            {
                interfaceStopAdapter(CA_WIFI);
            }
            break;
#endif
#ifdef BT_ADAPTER_TEST
        case 2:   //BT
            {
                interfaceStopAdapter(CA_EDR);
            }
            break;
#endif
#ifdef BLE_ADAPTER_TEST
        case 3: //BLE
            {
                interfaceStopAdapter(CA_LE);
            }
            break;
#endif
        default:
            printf("Feature is not enabled or not implemented\n");
    }
}

#ifdef BT_ADAPTER_TEST
void testInitializeBTInterface(void)
{
    printf("Initiazing EDR\n");

    printf("Initializing BT Adapter threadpool\n");
    initializeThreadPool(CA_EDR);

    //Start bluetooth communication adapter
    CAResult_t err = CAInitializeEDR(interfaceRegisterCallback, networkPacketHandler,
                                     networkInterfaceCallback, gBTThreadPool);
    if (CA_STATUS_OK != err && CA_ADAPTER_NOT_ENABLED != err)
    {
        printf("Failed to initialize bluetooth communication adapter!\n");
    }
}

void testTerminateBTInterface(void)
{
    printf("Terminating EDR\n");

    //Terminate the Bluetooth communication adapter
    CATerminateEDR();

    printf( "Terminating BT Adapter thread pool");
    u_thread_pool_free(gBTThreadPool);
    gBTThreadPool = NULL;
}
#endif //BT_ADAPTER_TEST

#ifdef WIFI_ADAPTER_TEST
void testInitializeWIFIInterface(void)
{
    printf("testInitializeWIFIInterface IN\n");

    printf("Initializing WIFI adapter threadpool\n");
    initializeThreadPool(CA_WIFI);

    //Start Wifi communication adapter
    if (0 != CAInitializeWifi(interfaceRegisterCallback, networkPacketHandler,
                              networkInterfaceCallback, gWiFiThreadPool))
    {
        printf("testInitializeWIFIInterface Failed to initialize bluetooth communication adapter\n");
        return;
    }

    printf("testInitializeWIFIInterface OUT\n");
}

void testTerminateWIFIInterface(void)
{
    printf("testTerminateWIFIInterface IN\n");

    // Stop if Wifi communication adapter is running
    interfaceStopAdapter(CA_WIFI);

    // Freeing threadpool for wifi communication adapter
    printf( "Terminating WIFI Adapter thread pool");
    u_thread_pool_free(gWiFiThreadPool);
    gWiFiThreadPool = NULL;

    //Terminate the Wifi communication adapter
    CATerminateWIfI();


    printf("testTerminateWIFIInterface OUT\n");
}
#endif //WIFI_ADAPTER_TEST

#ifdef BLE_ADAPTER_TEST
void testInitializeBLEInterface(void)
{
    printf("testInitializeBLEInterface IN\n");

    printf("Initializing BLE adapter threadpool\n");
    initializeThreadPool(CA_LE);

    //Start bluetooth communication adapter
    if (0 != CAInitializeLE(interfaceRegisterCallback, networkPacketHandler,
                            networkInterfaceCallback, gLEThreadPool))
    {
        printf("testInitializeBLEInterface Failed due to CAInitializeLE\n");
        return;
    }

    printf("testInitializeBLEInterface OUT\n");
}

void testTerminateBLEInterface(void)
{
    printf("testTerminateBLEInterface IN\n");

    //Terminate the BLE server & Client
    CATerminateLE();

    printf( "Terminating BLE Adapter thread pool");
    u_thread_pool_free(gLEThreadPool);
    gLEThreadPool = NULL;

    printf("testTerminateBLEInterface OUT\n");
}
#endif //BLE_ADAPTER_TEST

static void testPrintHelp(void)
{
    printf(" =====================================================================\n");
    printf("|                 Welcome to Connectivity Abstraction                 |\n");
    printf("|                   - CA Unit Test v1.0 -                             |\n");
    printf("|---------------------------------------------------------------------|\n");
    printf("|                           ** Options **                             |\n");
    printf("|  i - Initialize the Interface                                       |\n");
    printf("|  d - Terminate the Interface                                        |\n");
    printf("|  a - Start adapter                                                  |\n");
    printf("|  b - Stop adapter                                                   |\n");
    printf("|  sd- Start Discovery Server                                         |\n");
    printf("|  sl- Start Listening Server                                         |\n");
    printf("|  u - Send Unicast Data                                              |\n");
    printf("|  m - Send Multicast Data                                            |\n");
    printf("|  g - Get Network Info                                               |\n");
    printf("|  r - Read data synchronously                                        |\n");
    printf("|  x - quit the test.                                                 |\n");
    printf("|  h - Help menu.                                                     |\n");
    printf(" =====================================================================\n");
}

static gboolean testThread(GIOChannel *source, GIOCondition condition , gpointer data)
{
    gchar buf[10] = {'\0'};
    gsize read = 0;

    if (g_io_channel_read(channel, buf, 10, &read) != G_IO_ERROR_NONE)
    {
        printf("g_io_channel_read error!!!\n");
        return 1;
    }
    buf[read] = '\0';
    g_strstrip(buf);

    /*if ((!has_register) && (((buf[0]!= 'i') && (buf[0]!= 'h') && (buf[0]!= 'q')) || (read != 2))) {
        testPrintHelp();
        printf("***Warning***: You should Register firstly!\n");
        return 1;
    }*/
    switch (buf[0])
    {
        case 'i':
            testInitializeInterface();
            break;
        case 'x':
            testTerminateInterface();
            if (g_source_remove(g_test_io_watch_id))
            {
                printf("g_source_remove() OK!!!\n");
                g_io_channel_shutdown(channel, TRUE, &g_err_Sample);
                g_io_channel_unref(channel);
                g_main_loop_quit(mainloop);
            }
            break;
        case 'd':
            testTerminateInterface();
            break;
        case 'a':
            testStartAdapter();
            break;
        case 'b':
            testStopAdapter();
            break;
        case 's':
            if (read == 3)
            {
                if (buf[1] == 'd')
                {
                    testStartServer(1);
                }
                if (buf[1] == 'l')
                {
                    testStartServer(2);
                }
            }
            break;
        case 'u':
            testSendUnicastData();
            break;
        case 'm':
            testSendMulticastData();
            break;
        case 'r':
            testReadData();
            break;
        case 'g':
            testGetNetworkInfo();
            break;
        case 'h':
            testPrintHelp();
    }
    return 1;
}

int main(int argc, char *argv[])
{
    printf("Starting sample\n");
    mainloop = g_main_loop_new(NULL, FALSE);
    channel = g_io_channel_unix_new(0);/* read from stdin */
    g_test_io_watch_id = g_io_add_watch(channel,
                                        (GIOCondition)(G_IO_IN | G_IO_ERR | G_IO_HUP | G_IO_NVAL), testThread,
                                        NULL);
    printf("CM Test Thread created...\n");
    testPrintHelp();
    g_main_loop_run(mainloop);
    return 0;
}

