#include <assert.h>
#include <stdbool.h>
#include <unistd.h>

#include "interfaceHeaders/cacommon.h"
#include "interfaceHeaders/caadapterinterface.h"
#include "interfaceHeaders/cawifiadapter.h"
#include "interfaceHeaders/uthreadpool.h"

// & set: Liboic

// Wifi callbacks

CALocalConnectivity_t* localWifiEndpoint = NULL;
CARemoteEndpoint_t remoteMulticastEndpoint;
CARemoteEndpoint_t remoteEndpoint[10]; /* 10 RemoteEndpoints are currently kept */
char remoteIPAddress[CA_IPADDR_SIZE] = "192.168.1.103";  // Change the Corresponding IP address during testing.
char localIPAddress[CA_IPADDR_SIZE] = {0};
static u_thread_pool_t gWiFiThreadPool = NULL;

typedef struct ConnectivityHandlerList {
    CAConnectivityType_t type;
    CAConnectivityHandler_t handler;
    struct ConnectivityHandlerList* nextHandler;
} ConnectivityHandlerList;

// Hardcoded values to Test
char coapData[500] = "{\"oc:\[{href\":\"/a/light\",\"ref\":{\"power\":\"20\",\"state\":\"true\"}}\]}";

/**
 * @def CA_PORT
 * @brief Unicast port number (to listen for incoming data on unicast server).
 * Note :- Actual port number may differ based on result of bind() operation.
 */
#define CA_PORT   5298

/**
 * @def CA_MCAST_PORT
 * @brief Multicast port number as defined in COAP Specification, RFC-7252.
 */
#define CA_MCAST_PORT   5683

/**
 * @def CA_MULTICAST_IP
 * @brief Multicast IP Address
 */
#define CA_MULTICAST_IP "224.0.1.187"


CAConnectivityHandler_t* gCAConnectivityHandlers = NULL;

void initializeThreadPool(int d)
{
    if (CA_STATUS_OK != u_thread_pool_init(3, &gWiFiThreadPool))
        printf("Failed to create thread pool for WiFi adapter!\n");
}

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

void interfaceRegisterCallback(CAConnectivityHandler_t handler,
                               CAConnectivityType_t connType)
{
    printf("interfaceRegisterCallback Entry in Sample\n");
    ConnectivityHandlerList* newConnectivityHandler = (ConnectivityHandlerList*) malloc(sizeof(ConnectivityHandlerList));
    newConnectivityHandler->type = connType;
    newConnectivityHandler->handler = handler;
    printf("interfaceRegisterCallback Exit in Sample\n");
}


void requestResponseHandler(CARemoteEndpoint_t* object, void* data)
{
    printf("\nrequestResponseHandler Entry in Sample");
    if (NULL == object || NULL == data) {
        printf("\nNULL Object");
    }
    if (object->addressInfo.IP.ipAddress)
        printf("\nData Received from %s\n", object->addressInfo.IP.ipAddress);
    if (data)
        printf("\nReceived Data : %s \n", (char*)data);

    printf("\nrequestResponseHandler Exit in Sample");
}


void networkInterfaceCallback(CALocalConnectivity_t* localEndPoint, CANetworkStatus_t networkConnectivityState)
{
    printf("\nnetworkInterfaceCallback Entry in Sample");
    if (NULL == localEndPoint) {
        printf("\nNULL Object");
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
    CAResult_t error = CA_STATUS_FAILED;
    initializeThreadPool(0);
    error = CAInitializeWifi(interfaceRegisterCallback, requestResponseHandler, networkInterfaceCallback, gWiFiThreadPool);
    assert_eq(error, CA_STATUS_OK);
    u_thread_pool_free(gWiFiThreadPool);
    return 0;
}

int wifi_tc_002_initializeWifi_n(void)
{
    CAResult_t error = CA_STATUS_FAILED;
    initializeThreadPool(0);
    error = CAInitializeWifi(NULL, NULL, NULL, NULL);
    assert_eq(error, CA_STATUS_INVALID_PARAM);  // Note: initializeWifi always return OC_STACK_OK
    u_thread_pool_free(gWiFiThreadPool);
    return 0;
}

int wifi_tc_003_getWIFIInterfaceInformation_p(void)
{
    CAResult_t error = CA_STATUS_FAILED;
    uint32_t size = 0;

    //Startup
    initializeThreadPool(0);
    CAInitializeWifi(interfaceRegisterCallback, requestResponseHandler, networkInterfaceCallback, gWiFiThreadPool);
    CAStartWIFI();
    error = CAGetWIFIInterfaceInformation(&localWifiEndpoint, &size);
    assert_eq(error, CA_STATUS_OK);
    return 0;
}

int wifi_tc_004_getWIFIInterfaceInformation_n(void)
{
    CAResult_t error = CA_STATUS_FAILED;
    uint32_t size = 0;
    error = CAGetWIFIInterfaceInformation(NULL, &size);
    assert_eq(error, CA_STATUS_INVALID_PARAM);
    return 0;
}

int wifi_tc_005_sendWifiUnicastData_p(void)
{
    uint32_t dataSize;

    //Startup
    initializeThreadPool(0);
    CAInitializeWifi(interfaceRegisterCallback, requestResponseHandler, networkInterfaceCallback, gWiFiThreadPool);
    CAStartWIFI();
    uint32_t size = 0;
    CAGetWIFIInterfaceInformation(&localWifiEndpoint, &size);
    // localWifiEndpoint->addressInfo.IP.port = 5283;
    interfaceInitializeEndpoint(0);
    dataSize = CASendWIFIUnicastData(&remoteEndpoint[0], coapData, strlen(coapData));
    sleep(5);

    assert_eq(dataSize, strlen(coapData));

    CAStopWIFI();
    u_thread_pool_free(gWiFiThreadPool);
    CATerminateWIfI();
    return 0;
}

int wifi_tc_006_sendWifiUnicastData_n(void)
{
    int dataSize = 0;
    dataSize = CASendWIFIUnicastData(NULL, NULL, NULL);
    assert_eq(dataSize, 0);
    return 0;
}

int wifi_tc_007_sendWifiMulticastData_p(void)
{
    uint32_t dataSize = 0;
     // Startup
    initializeThreadPool(0);
    CAInitializeWifi(interfaceRegisterCallback, requestResponseHandler, networkInterfaceCallback, gWiFiThreadPool);
    CAStartWIFI();

    dataSize = CASendWIFIMulticastData(coapData, strlen(coapData));
    sleep(5);
    assert_eq(dataSize, strlen(coapData));

    CAStopWIFI();
    u_thread_pool_free(gWiFiThreadPool);
    CATerminateWIfI();
    return 0;
}

int wifi_tc_008_sendWifiMulticastData_n(void)
{
    int dataSize = 0;
    dataSize = CASendWIFIMulticastData(NULL, NULL);
    assert_eq(dataSize, 0);
    return 0;
}

int wifi_tc_009_startUnicastServer_p(void)
{
    CAResult_t error = CA_STATUS_FAILED;
    int16_t unicastPort = CA_PORT;
    // Startup
    initializeThreadPool(0);
    CAInitializeWifi(interfaceRegisterCallback, requestResponseHandler, networkInterfaceCallback, gWiFiThreadPool);
    CAStartWIFI();
    uint32_t size = 0;
    int32_t serverFd = 0;
    // error = CAGetWIFIInterfaceInformation(&localWifiEndpoint, &size);
    error = CAWiFiStartUnicastServer("0.0.0.0", &unicastPort, false, false, &serverFd);
    assert_eq(error, CA_STATUS_OK);
    // Cleanup
    CAStopWIFI();
    u_thread_pool_free(gWiFiThreadPool);
    CATerminateWIfI();
    return 0;
}

int wifi_tc_010_startUnicastServer_n(void)
{
    CAResult_t error = CA_STATUS_FAILED;
    // Startup
    initializeThreadPool(0);
    CAInitializeWifi(interfaceRegisterCallback, requestResponseHandler, networkInterfaceCallback, gWiFiThreadPool);
    CAStartWIFI();
    uint32_t size = 0;
    int32_t serverFd = 0;
    error = CAGetWIFIInterfaceInformation(&localWifiEndpoint, &size);

    error = CAWiFiStartUnicastServer(NULL, &localWifiEndpoint->addressInfo.IP.port, false, false, &serverFd);
    assert_eq(error, CA_STATUS_INVALID_PARAM);
    // Cleanup
    CAStopWIFI();
    u_thread_pool_free(gWiFiThreadPool);
    CATerminateWIfI();
    return 0;
}

int wifi_tc_011_startUnicastServer_n(void)
{
    CAResult_t error = CA_STATUS_FAILED;
    // Startup
    initializeThreadPool(0);
    CAInitializeWifi(interfaceRegisterCallback, requestResponseHandler, networkInterfaceCallback, gWiFiThreadPool);
    CAStartWIFI();
    uint32_t size = 0;
    int32_t serverFd = 0;
    error = CAGetWIFIInterfaceInformation(&localWifiEndpoint, &size);

    error = CAWiFiStartUnicastServer(localWifiEndpoint->addressInfo.IP.ipAddress, NULL, false, false, &serverFd);
    assert_eq(error, CA_STATUS_INVALID_PARAM);
    // Cleanup
    CAStopWIFI();
    u_thread_pool_free(gWiFiThreadPool);
    CATerminateWIfI();
    return 0;
}

int wifi_tc_012_startUnicastServer_n(void)
{
    CAResult_t error = CA_STATUS_FAILED;
    // Startup
    initializeThreadPool(0);
    CAInitializeWifi(interfaceRegisterCallback, requestResponseHandler, networkInterfaceCallback, gWiFiThreadPool);
    CAStartWIFI();
    uint32_t size = 0;
    int32_t serverFd = 0;
    error = CAGetWIFIInterfaceInformation(&localWifiEndpoint, &size);

    error = CAWiFiStartUnicastServer(NULL, NULL, false, false, &serverFd);
    assert_eq(error, CA_STATUS_INVALID_PARAM);
    // Cleanup
    CAStopWIFI();
    u_thread_pool_free(gWiFiThreadPool);
    CATerminateWIfI();
    return 0;
}

int wifi_tc_013_startMulticastServer_p(void)
{
    CAResult_t error = CA_STATUS_FAILED;
    int16_t multicastPort = CA_MCAST_PORT;
    // Startup
    initializeThreadPool(0);
    CAInitializeWifi(interfaceRegisterCallback, requestResponseHandler, networkInterfaceCallback, gWiFiThreadPool);
    CAStartWIFI();
    uint32_t size = 0;
    int32_t serverFd = 0;
    error = CAGetWIFIInterfaceInformation(&localWifiEndpoint, &size);

    error = CAWiFiStartMulticastServer("0.0.0.0", CA_MULTICAST_IP, multicastPort, &serverFd);
    assert_eq(error, CA_STATUS_OK);
    // Cleanup
    CAStopWIFI();
    u_thread_pool_free(gWiFiThreadPool);
    CATerminateWIfI();
    return 0;
}

int wifi_tc_014_startMulticastServer_n(void)
{
    CAResult_t error = CA_STATUS_FAILED;
    int16_t multicastPort = CA_MCAST_PORT;
    // Startup
    initializeThreadPool(0);
    CAInitializeWifi(interfaceRegisterCallback, requestResponseHandler, networkInterfaceCallback, gWiFiThreadPool);
    CAStartWIFI();
    uint32_t size = 0;
    int32_t serverFd = 0;
    error = CAGetWIFIInterfaceInformation(&localWifiEndpoint, &size);

    error = CAWiFiStartMulticastServer(NULL, "0.0.0.0", multicastPort, &serverFd);
    assert_eq(error, CA_STATUS_INVALID_PARAM);
    // Cleanup
    CAStopWIFI();
    u_thread_pool_free(gWiFiThreadPool);
    CATerminateWIfI();
    return 0;
}

int wifi_tc_015_startMulticastServer_n(void)
{
    CAResult_t error = CA_STATUS_FAILED;
    int16_t multicastPort = CA_MCAST_PORT;
    // Startup
    initializeThreadPool(0);
    CAInitializeWifi(interfaceRegisterCallback, requestResponseHandler, networkInterfaceCallback, gWiFiThreadPool);
    CAStartWIFI();
    uint32_t size = 0;
    int32_t serverFd = 0;
    error = CAGetWIFIInterfaceInformation(&localWifiEndpoint, &size);

    error = CAWiFiStartMulticastServer(NULL, CA_MULTICAST_IP, multicastPort, &serverFd);
    assert_eq(error, CA_STATUS_INVALID_PARAM);
    // Cleanup
    CAStopWIFI();
    u_thread_pool_free(gWiFiThreadPool);
    CATerminateWIfI();
    return 0;
}

int wifi_tc_016_startMulticastServer_n(void)
{
    CAResult_t error = CA_STATUS_FAILED;
    int16_t multicastPort = CA_MCAST_PORT;
    // Startup
    initializeThreadPool(0);
    CAInitializeWifi(interfaceRegisterCallback, requestResponseHandler, networkInterfaceCallback, gWiFiThreadPool);
    CAStartWIFI();
    uint32_t size = 0;
    int32_t serverFd = 0;
    error = CAGetWIFIInterfaceInformation(&localWifiEndpoint, &size);

    error = CAWiFiStartMulticastServer("0.0.0.0", CA_MULTICAST_IP, NULL, &serverFd);
    assert_eq(error, CA_STATUS_INVALID_PARAM);
    // Cleanup
    CAStopWIFI();
    u_thread_pool_free(gWiFiThreadPool);
    CATerminateWIfI();
    return 0;
}

int wifi_tc_017_startMulticastServer_n(void)
{
    CAResult_t error = CA_STATUS_FAILED;
    int16_t multicastPort = CA_MCAST_PORT;
    // Startup
    initializeThreadPool(0);
    CAInitializeWifi(interfaceRegisterCallback, requestResponseHandler, networkInterfaceCallback, gWiFiThreadPool);
    CAStartWIFI();
    uint32_t size = 0;
    int32_t serverFd = 0;
    error = CAGetWIFIInterfaceInformation(&localWifiEndpoint, &size);

    error = CAWiFiStartMulticastServer(NULL, NULL, NULL, NULL, &serverFd);
    assert_eq(error, CA_STATUS_INVALID_PARAM);
    // Cleanup
    CAStopWIFI();
    u_thread_pool_free(gWiFiThreadPool);
    CATerminateWIfI();
    return 0;
}

