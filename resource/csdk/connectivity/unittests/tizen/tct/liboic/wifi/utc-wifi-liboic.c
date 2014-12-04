//#include <glib.h>
#include "assert.h"
//#include "oic-core/logger.h"
//#include "oic-core/ocstack.h"
#include "interfaceHeaders/cacommon.h"
#include "interfaceHeaders/caadapterinterface.h"
#include "interfaceHeaders/cawifiethernetadapter.h"

//& set: Liboic

//Wifi callbacks

CALocalConnectivity* localWifiEndpoint = NULL;
CARemoteEndpoint remoteMulticastEndpoint;
CARemoteEndpoint remoteEndpoint[10]; /* 10 RemoteEndpoints are currently kept */
char remoteIPAddress[CA_IPADDR_SIZE] = "192.168.1.103";  //Change the Corresponding IP address during testing.
char localIPAddress[CA_IPADDR_SIZE] = {0};
int localPort = -1;
int32_t serverId;
//static GMainLoop *mainloop;
//int gMainloopStatus = 0;

//Hardcoded values to Test
typedef struct ConnectivityHandlerList {
    CAConnectivityType type;
    CAConnectivityHandler handler;
    struct ConnectivityHandlerList* nextHandler;
} ConnectivityHandlerList;

char coapData[500] = "{\"oc:\[{href\":\"/a/light\",\"ref\":{\"power\":\"20\",\"state\":\"true\"}}\]}";

#define CA_PORT         5283
#define CA_MCAST_PORT   5298

/**
 * @def CA_MULTICAST_IP
 * @brief Multicast IP Address
 */
#define CA_MULTICAST_IP "224.0.1.187"


CAConnectivityHandler* gCAConnectivityHandlers = NULL;

int interfaceInitializeEndpoint(int d)
{
	int i = 0;
    /* As of initializing one endpoint */
    for (i = 0; i < 1; i++)
    {
        remoteEndpoint[i].connectivityType = CA_WIFI;
        strncpy(remoteEndpoint[i].addressInfo.IP.ipAddress, remoteIPAddress, CA_IPADDR_SIZE);
        remoteEndpoint[i].addressInfo.IP.port = 5283; /* Send the corresponding port here */
    }

    remoteMulticastEndpoint.connectivityType = CA_WIFI;
}

void storeInterfaceCallbacks(CAConnectivityHandler* newHandler)
{
    printf("\nstoreInterfaceCallbacks Entry in Sample");
	#if 0
    newHandler->nextHandler = NULL;

    CAConnectivityHandler* tempCAConnectivityHandlers = gCAConnectivityHandlers;

    if (!tempCAConnectivityHandlers) {
        gCAConnectivityHandlers = newHandler;
        return;
    }
    while (tempCAConnectivityHandlers->nextHandler) {
        tempCAConnectivityHandlers = tempCAConnectivityHandlers->nextHandler;
    }

    tempCAConnectivityHandlers->nextHandler = newHandler;
	#endif
    printf("\nstoreInterfaceCallbacks Exit in Sample");
}

/*
void interfaceRegisterCallback(CAConnectivityHandler handler , CAConnectivityType cType)
{
    printf("\ninterfaceRegisterCallback Entry in Sample");
    CAConnectivityHandler newCAConnectivityHandler;
	newCAConnectivityHandler.start = handler.start;
	newCAConnectivityHandler.stop = handler.stop;
	newCAConnectivityHandler.startAdapter = handler.startAdapter;
	newCAConnectivityHandler.sendData= handler.sendData;
	newCAConnectivityHandler.sendDataToAll= handler.sendDataToAll;
	newCAConnectivityHandler.statNotifyServer= handler.statNotifyServer;
	newCAConnectivityHandler.sendNotification= handler.sendNotification;
	newCAConnectivityHandler.GetnetInfo= handler.GetnetInfo;
	//newCAConnectivityHandler.cType= cType;
	storeInterfaceCallbacks(&newCAConnectivityHandler);
    printf("\ninterfaceRegisterCallback Exit in Sample");
}
*/

void interfaceRegisterCallback(CAConnectivityHandler handler,
                               CAConnectivityType connType)
{
    printf("interfaceRegisterCallback Entry in Sample\n");
// connectivity Handlerlist is not used
#if 0
    ConnectivityHandlerList* newConnectivityHandler = (ConnectivityHandlerList*) malloc(sizeof(ConnectivityHandlerList));
    if (NULL == newConnectivityHandler)
    {
        printf("Memory allocation failed!\n");
        return;
    }
    
    newConnectivityHandler->type = connType;
    newConnectivityHandler->handler = handler;
    storeInterfaceCallbacks(newConnectivityHandler);
#endif //#if 0
    printf("interfaceRegisterCallback Exit in Sample\n");
}


void requestResponseHandler(CARemoteEndpoint* object, void* data)
{
    printf("\nrequestResponseHandler Entry in Sample");
    if (object == NULL || data == NULL) {
        printf("\nNULL Object");
        return;
    }

    if (object->addressInfo.IP.ipAddress)
        printf("\nData Received from %s\n", object->addressInfo.IP.ipAddress);
    if (data)
        printf("\nReceived Data : %s \n", (char*)data);
    /*
    gMainloopStatus = 1;
    if (mainloop)
    {
    g_main_loop_quit(mainloop);
    mainloop = NULL;
    }
    */
    printf("\nrequestResponseHandler Exit in Sample");
}


void networkInterfaceCallback(CALocalConnectivity* localEndPoint, CANetworkStatus networkConnectivityState)
{
    printf("\nnetworkInterfaceCallback Entry in Sample");
    if (localEndPoint == NULL) {
        printf("\nNULL Object");
        return;
    }

    if (localEndPoint->addressInfo.IP.ipAddress)
        printf("\n Local Interface Address%s\n", localEndPoint->addressInfo.IP.ipAddress);

    if (networkConnectivityState == CA_INTERFACE_UP)
        printf("\n WiFi Network Status is UP");
    else
        printf("\n WiFi Network Status is DOWN");
    printf("\nnetworkInterfaceCallback Exit in Sample");
}

void utc_liboic_startup(void)
{

}

void utc_liboic_cleanup(void)
{

}


int wifi_tc_001_initializeWifi_p(void)
{
    CAResult error = CA_STATUS_FAILED;
    error = CAInitializeWifi(interfaceRegisterCallback, requestResponseHandler, networkInterfaceCallback);
    assert_eq(error, CA_STATUS_OK);
    return 0;
}

int wifi_tc_002_initializeWifi_n(void)
{
    CAResult error = CA_STATUS_FAILED;
    error = CAInitializeWifi(NULL, NULL, NULL);
    assert_eq(error, CA_STATUS_INVALID_PARAM);  //Note: initializeWifi always return OC_STACK_OK
    return 0;
}

int wifi_tc_003_getWIFIInterfaceInformation_p(void)
{
    CAResult error = CA_STATUS_FAILED;
    uint32_t size = 0;
    error = CAGetWIFIInterfaceInformation(&localWifiEndpoint, &size);
    assert_eq(error, CA_STATUS_OK);

    // Now, check local endpoint information
    //assert_neq(localWifiEndpoint->endpoint_info, NULL);
    strncpy(localIPAddress, localWifiEndpoint->addressInfo.IP.ipAddress, CA_IPADDR_SIZE);
    localPort = localWifiEndpoint->addressInfo.IP.port;
    assert_neq(localPort, -1);
    return 0;
}

int wifi_tc_004_getWIFIInterfaceInformation_n(void)
{
    CAResult error = CA_STATUS_FAILED;
    uint32_t size = 0;
    error = CAGetWIFIInterfaceInformation(NULL, &size);
    assert_eq(error, CA_STATUS_INVALID_PARAM);
    return 0;
}

int wifi_tc_005_sendWifiUnicastData_p(void)
{
    uint32_t dataSize;
    CAResult error = CA_STATUS_FAILED;
    
    //Startup
    CAInitializeWifi(interfaceRegisterCallback, requestResponseHandler, networkInterfaceCallback);
    uint32_t size = 0;
    error = CAGetWIFIInterfaceInformation(&localWifiEndpoint, &size);
    assert_eq(error, CA_STATUS_OK);
    
    localWifiEndpoint->addressInfo.IP.port = 5283;
    //StartWifiAdapter(localWifiEndpoint);

    interfaceInitializeEndpoint(0);
    dataSize = CASendWIFIUnicastData(&remoteEndpoint[0], coapData, strlen(coapData));
    sleep(5);

    assert_eq(dataSize, strlen(coapData));
    CATerminateWifi();
    return 0;
}

int wifi_tc_006_sendWifiUnicastData_n(void)
{
    int dataSize = 0;
    interfaceInitializeEndpoint(0);
    dataSize = CASendWIFIUnicastData(NULL, NULL, NULL);
    assert_eq(dataSize, 0);
    return 0;
}

int wifi_tc_007_sendWifiMulticastData_p(void)
{
    uint32_t dataSize = 0;
    interfaceInitializeEndpoint(0);
    dataSize = CASendWIFIMulticastData(coapData, strlen(coapData));
    assert_eq(dataSize, strlen(coapData));
    return 0;
}

int wifi_tc_008_sendWifiMulticastData_n(void)
{
    int dataSize = 0;
    interfaceInitializeEndpoint(0);
    dataSize = CASendWIFIMulticastData(NULL, NULL);
    assert_eq(dataSize, 0);
    return 0;
}

int wifi_tc_009_startUnicastServer_p(void)
{
    CAResult error = CA_STATUS_FAILED;
    int16_t unicastPort = CA_PORT;
    //Startup
    CAInitializeWifi(interfaceRegisterCallback, requestResponseHandler, networkInterfaceCallback);
    uint32_t size = 0;
    error = CAGetWIFIInterfaceInformation(&localWifiEndpoint, &size);

    error = CAStartUnicastServer("0.0.0.0", &unicastPort);
    assert_eq(error, CA_STATUS_OK);
    //Cleanup
    CAStopUnicastServer();
    CATerminateWifi();
    return 0;
}

int wifi_tc_010_startUnicastServer_n(void)
{
    CAResult error = CA_STATUS_FAILED;
    //Startup
    CAInitializeWifi(interfaceRegisterCallback, requestResponseHandler, networkInterfaceCallback);
    uint32_t size = 0;
    error = CAGetWIFIInterfaceInformation(&localWifiEndpoint, &size);
    assert_eq(error, CA_STATUS_OK);

    error = CAStartUnicastServer(NULL, localWifiEndpoint->addressInfo.IP.port);
    assert_eq(error, CA_STATUS_INVALID_PARAM);
    //Cleanup
    CAStopUnicastServer();
    CATerminateWifi();
    return 0;
}

int wifi_tc_011_startUnicastServer_n(void)
{
    CAResult error = CA_STATUS_FAILED;
    //Startup
    CAInitializeWifi(interfaceRegisterCallback, requestResponseHandler, networkInterfaceCallback);
    uint32_t size = 0;
    error = CAGetWIFIInterfaceInformation(&localWifiEndpoint, &size);
    assert_eq(error, CA_STATUS_OK);
    
    error = CAStartUnicastServer(localWifiEndpoint->addressInfo.IP.ipAddress, NULL);
    assert_eq(error, CA_STATUS_INVALID_PARAM);
    //Cleanup
    CAStopUnicastServer();
    CATerminateWifi();
    return 0;
}

int wifi_tc_012_startUnicastServer_n(void)
{
    CAResult error = CA_STATUS_FAILED;
    //Startup
    CAInitializeWifi(interfaceRegisterCallback, requestResponseHandler, networkInterfaceCallback);
    uint32_t size = 0;
    error = CAGetWIFIInterfaceInformation(&localWifiEndpoint, &size);
    assert_eq(error, CA_STATUS_OK);
    
    error = CAStartUnicastServer(NULL, NULL);
    assert_eq(error, CA_STATUS_INVALID_PARAM);
    //Cleanup
    CAStopUnicastServer();
    CATerminateWifi();
    return 0;
}

int wifi_tc_013_startMulticastServer_p(void)
{
    CAResult error = CA_STATUS_FAILED;
    int16_t multicastPort = CA_MCAST_PORT;
    //Startup
    CAInitializeWifi(interfaceRegisterCallback, requestResponseHandler, networkInterfaceCallback);
    uint32_t size = 0;
    error = CAGetWIFIInterfaceInformation(&localWifiEndpoint, &size);

    error = CAStartMulticastServer(CA_MULTICAST_IP, "0.0.0.0", &multicastPort);
    assert_eq(error, CA_STATUS_OK);
    //Cleanup
    CAStopMulticastServer();
    CATerminateWifi();
    return 0;
}

int wifi_tc_014_startMulticastServer_n(void)
{
    CAResult error = CA_STATUS_FAILED;
    int16_t multicastPort = CA_MCAST_PORT;
    //Startup
    CAInitializeWifi(interfaceRegisterCallback, requestResponseHandler, networkInterfaceCallback);
    uint32_t size = 0;
    error = CAGetWIFIInterfaceInformation(&localWifiEndpoint, &size);

    error = CAStartMulticastServer(NULL, "0.0.0.0", &multicastPort);
    assert_eq(error, CA_STATUS_INVALID_PARAM);
    //Cleanup
    CAStopMulticastServer();
    CATerminateWifi();
    return 0;
}

int wifi_tc_015_startMulticastServer_n(void)
{
    CAResult error = CA_STATUS_FAILED;
    int16_t multicastPort = CA_MCAST_PORT;
    //Startup
    CAInitializeWifi(interfaceRegisterCallback, requestResponseHandler, networkInterfaceCallback);
    uint32_t size = 0;
    error = CAGetWIFIInterfaceInformation(&localWifiEndpoint, &size);

    error = CAStartMulticastServer(CA_MULTICAST_IP, NULL, &multicastPort);
    assert_eq(error, CA_STATUS_INVALID_PARAM);
    //Cleanup
    CAStopMulticastServer();
    CATerminateWifi();
    return 0;
}

int wifi_tc_016_startMulticastServer_n(void)
{
    CAResult error = CA_STATUS_FAILED;
    int16_t multicastPort = CA_MCAST_PORT;
    //Startup
    CAInitializeWifi(interfaceRegisterCallback, requestResponseHandler, networkInterfaceCallback);
    uint32_t size = 0;
    error = CAGetWIFIInterfaceInformation(&localWifiEndpoint, &size);

    error = CAStartMulticastServer(CA_MULTICAST_IP, "0.0.0.0", NULL);
    assert_eq(error, CA_STATUS_INVALID_PARAM);
    //Cleanup
    CAStopMulticastServer();
    CATerminateWifi();
    return 0;
}

int wifi_tc_017_startMulticastServer_n(void)
{
    CAResult error = CA_STATUS_FAILED;
    int16_t multicastPort = CA_MCAST_PORT;
    //Startup
    CAInitializeWifi(interfaceRegisterCallback, requestResponseHandler, networkInterfaceCallback);
    uint32_t size = 0;
    error = CAGetWIFIInterfaceInformation(&localWifiEndpoint, &size);

    error = CAStartMulticastServer(NULL, NULL, NULL);
    assert_eq(error, CA_STATUS_INVALID_PARAM);
    //Cleanup
    CAStopMulticastServer();
    CATerminateWifi();
    return 0;
}

