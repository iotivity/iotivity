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

#include "cawifiinterface.h"

#include <string.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>

#include "caadapterutils.h"
#include "umutex.h"
#include "logger.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "com_iotivity_jar_cawifiinterface.h"

#define WIFI_MONITOR_TAG "WIFI_MONITOR"

/**
 * @var g_nwConnectivityStatus
 * @brief  Maintains network status.
 */
static CANetworkStatus_t g_nwConnectivityStatus;

/**
 * @var g_wifiNetInfoMutex
 * @brief  Mutex for synchronizing access to cached interface and IP address information.
 */
static u_mutex g_wifiNetInfoMutex = NULL;

/**
 * @var g_wifiInterfaceName
 * @brief  Maintains interface name.
 */
static char *g_wifiInterfaceName = NULL;

/**
 * @var g_wifiIPAddress
 * @brief  Maintains interface IP address.
 */
static char *g_wifiIPAddress = NULL;

/**
 * @var g_wifiSubnetMask
 * @brief  Maintains interface subnetmask.
 */
static char *g_wifiSubnetMask = NULL;

/**
 * @var g_threadPool
 * @brief ThreadPool for storing u_thread_pool_t handle passed from adapter
 */
static u_thread_pool_t g_threadPool = NULL;

/**
 * @var g_stopNetworkMonitor
 * @brief Flag to control the Network Monitor Thread
 */
static bool g_stopNetworkMonitor = false;

/**
 * @var g_jvm
 * @brief pointer to store JavaVM
 */
static JavaVM *g_jvm = NULL;

/**
 * @var g_context
 * @brief pointer to store Application Context
 */
static jobject g_context = NULL;

/**
 * @var g_networkChangeCb
 * @brief  Maintains network connection state change callback.
 */
static CAWiFiConnectionStateChangeCallback g_networkChangeCb = NULL;

/**
 * @fn CAWiFiUpdateInterfaceInformation
 * @brief This methods gets local interface name and IP address information.
 */
static void CAWiFiUpdateInterfaceInformation(char **interfaceName,
                    char **ipAddress, char **subnetMask);
/**
 * @fn CACreateWiFiJNIInterfaceObject
 * @brief creates new instance of CAWiFiInterface through JNI
 */
static void CACreateWiFiJNIInterfaceObject(jobject context);

/**
 * @fn CASendNetworkChangeCallback
 * @brief updates network status to wifi adapter
 */
void CASendNetworkChangeCallback(CANetworkStatus_t currNetworkStatus);

CAResult_t CAWiFiInitializeNetworkMonitor(const u_thread_pool_t threadPool)
{
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "IN");

    g_threadPool = threadPool;

    if (!g_wifiNetInfoMutex)
    {
        g_wifiNetInfoMutex = u_mutex_new();
    }

    CACreateWiFiJNIInterfaceObject(g_context);

    u_mutex_lock(g_wifiNetInfoMutex);
    CAWiFiUpdateInterfaceInformation(&g_wifiInterfaceName, &g_wifiIPAddress,
                                        &g_wifiSubnetMask);
    u_mutex_unlock(g_wifiNetInfoMutex);

    g_nwConnectivityStatus = (g_wifiIPAddress) ? CA_INTERFACE_UP : CA_INTERFACE_DOWN;

    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

void CAWiFiTerminateNetworkMonitor(void)
{
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "IN");

    g_threadPool = NULL;

    u_mutex_lock(g_wifiNetInfoMutex);

    if (g_wifiInterfaceName)
    {
        OICFree(g_wifiInterfaceName);
        g_wifiInterfaceName = NULL;
    }

    if (g_wifiIPAddress)
    {
        OICFree(g_wifiIPAddress);
        g_wifiIPAddress = NULL;
    }

    if (g_wifiSubnetMask)
    {
        OICFree(g_wifiSubnetMask);
        g_wifiSubnetMask = NULL;
    }

    u_mutex_unlock(g_wifiNetInfoMutex);

    if (g_wifiNetInfoMutex)
    {
        u_mutex_free(g_wifiNetInfoMutex);
        g_wifiNetInfoMutex = NULL;
    }

    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "OUT");
}

CAResult_t CAWiFiStartNetworkMonitor(void)
{
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "IN");

    u_mutex_lock(g_wifiNetInfoMutex);
    g_stopNetworkMonitor = false;
    u_mutex_unlock(g_wifiNetInfoMutex);

    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAWiFiStopNetworkMonitor(void)
{
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "IN");

    u_mutex_lock(g_wifiNetInfoMutex);

    if (g_stopNetworkMonitor)
    {
        OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "CAWiFiStopNetworkMonitor, already stopped");
        u_mutex_unlock(g_wifiNetInfoMutex);
        return CA_STATUS_OK;
    }

    g_stopNetworkMonitor = true;

    u_mutex_unlock(g_wifiNetInfoMutex);

    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAWiFiGetInterfaceInfo(char **interfaceName, char **ipAddress)
{
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "IN");

    VERIFY_NON_NULL(interfaceName, WIFI_MONITOR_TAG, "interface name");
    VERIFY_NON_NULL(ipAddress, WIFI_MONITOR_TAG, "ip address");

    // Get the interface and ipaddress information from cache
    u_mutex_lock(g_wifiNetInfoMutex);

    if(g_wifiInterfaceName == NULL || g_wifiIPAddress == NULL)
    {
        OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "Network not enabled");
        return CA_ADAPTER_NOT_ENABLED;

    }

    *interfaceName = (g_wifiInterfaceName) ? OICStrdup((const char *)g_wifiInterfaceName)
                               : NULL;
    *ipAddress = (g_wifiIPAddress) ? OICStrdup((const char *)g_wifiIPAddress)
                               : NULL;

    u_mutex_unlock(g_wifiNetInfoMutex);

    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAWiFiGetInterfaceSubnetMask(char **subnetMask)
{
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "IN");

    VERIFY_NON_NULL(subnetMask, WIFI_MONITOR_TAG, "subnet mask");

    u_mutex_lock(g_wifiNetInfoMutex);
    if(NULL == g_wifiSubnetMask)
    {
        OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "There is no subnet mask information!");
        u_mutex_unlock(g_wifiNetInfoMutex);
        return CA_STATUS_FAILED;
    }

    *subnetMask = (g_wifiSubnetMask) ? OICStrdup((const char *)g_wifiSubnetMask)
                               : NULL;
    u_mutex_unlock(g_wifiNetInfoMutex);

    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

bool CAWiFiIsConnected(void)
{
    return(g_nwConnectivityStatus == CA_INTERFACE_UP);
}

void CAWiFiSetConnectionStateChangeCallback(
    CAWiFiConnectionStateChangeCallback callback)
{
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "IN");

    g_networkChangeCb = callback;

    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "OUT");
}

void CAWiFiUpdateInterfaceInformation(char **interfaceName, char **ipAddress,
                                      char **subnetMask)
{
    char buf[1024] =  { 0, };
    struct ifconf ifc;
    struct ifreq* ifr;
    int sck;
    int32_t interfaces;
    int32_t i;
    const char *matchName = "wlan";

    if (!interfaceName || !ipAddress || !subnetMask)
    {
        OIC_LOG(ERROR, WIFI_MONITOR_TAG, "Invalid input: interface/ipaddress holder is NULL!");
        return;
    }

   /* Get a socket handle. */
    sck = socket(AF_INET, SOCK_DGRAM, 0);
    if (sck < 0)
    {
        return;
    }

    /* Query available interfaces. */
    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;

    if (ioctl(sck, SIOCGIFCONF, &ifc) < 0)
    {
        close(sck);
        return;
    }

    /* Iterate through the list of interfaces. */
    ifr = ifc.ifc_req;
    interfaces = ifc.ifc_len / sizeof(struct ifreq);

    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "CAWiFiUpdateInterfaceInformation : %d", interfaces);

    for (i = 0; i < interfaces; i++)
    {
        struct ifreq temp_ifr = {};
        struct ifreq* item = &ifr[i];

        char interfaceAddress[CA_IPADDR_SIZE] = {0};
        char interfaceSubnetMask[CA_IPADDR_SIZE] = {0};
        strcpy(temp_ifr.ifr_name, item->ifr_name);

        if (ioctl(sck, SIOCGIFFLAGS, &temp_ifr))
        {
            OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG,
                    "CAWiFiUpdateInterfaceInformation, SIOCGIFFLAGS Failed");
            continue;
        }

        if (!((temp_ifr.ifr_flags & IFF_UP) && (temp_ifr.ifr_flags & IFF_RUNNING)))
        {
            continue;
        }

        if(((struct sockaddr_in*) &item->ifr_addr)->sin_family != AF_INET)
        {
            continue;
        }

        char* ip = inet_ntoa(((struct sockaddr_in*) &item->ifr_addr)->sin_addr);
        OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "ip : %s", ip);
        if(strcmp(ip, "127.0.0.1") == 0)
        {
            continue;
        }

        if (!strncasecmp(item->ifr_name, matchName, strlen(matchName))) {

            socklen_t len = sizeof(struct sockaddr_in);

           // get the interface ip address
            if (0 != getnameinfo(&item->ifr_addr, len, interfaceAddress,
                            sizeof(interfaceAddress), NULL, 0, NI_NUMERICHOST))
            {
                    OIC_LOG_V(ERROR, WIFI_MONITOR_TAG, "Failed to get IPAddress, Error code: %s",
                              strerror(errno));
                    break;
            }

            // set interface name
            *interfaceName = OICStrdup(item->ifr_name);

            // set local ip address
            *ipAddress = OICStrdup(interfaceAddress);

            if (ioctl((int)sck, SIOCGIFNETMASK, item) < 0)
            {
                OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG,
                         "CAWiFiUpdateInterfaceInformation, SIOCGIFNETMASK Failed");
                continue;
            }

            // get the interface subnet mask
            if (0 != getnameinfo(&item->ifr_netmask, len, interfaceSubnetMask,
                            sizeof(interfaceSubnetMask), NULL, 0, NI_NUMERICHOST))
            {
                    OIC_LOG_V(ERROR, WIFI_MONITOR_TAG, "Failed to get subnet mask, Error code: %s",
                              strerror(errno));
                    break;
            }

            *subnetMask = strndup(interfaceSubnetMask, strlen(interfaceSubnetMask));
        }
        else
        {
            OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "did not match name : wlan");
        }

        OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG,
                        "ipAddress : %s, interfaceName : %s, subnetmask : %s",
                        *ipAddress, *interfaceName, *subnetMask);
       break;
    }

    close(sck);
    return;
}

void CAWiFiJniInit(JavaVM* jvm)
{
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "[WIFICore] CAWiFiJniInit");
    g_jvm = jvm;
}

void CAJniSetContext(jobject context)
{
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "caWifiSetObject");
    g_context = context;
}

void CACreateWiFiJNIInterfaceObject(jobject context)
{
    JNIEnv* env;
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "[WIFICore] CACreateWiFiJNIInterfaceObject");

    if ((*g_jvm)->GetEnv(g_jvm, (void**) &env, JNI_VERSION_1_6) != JNI_OK)
    {
        OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "[WIFICore] Could not get JNIEnv pointer");
        return;
    }

    //getApplicationContext
    jclass contextClass = (*env)->FindClass(env, "android/content/Context");
    if (contextClass == 0)
    {
        OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "[WIFICore] Could not get context object class");
        return;
    }

    jmethodID getApplicationContextMethod = (*env)->GetMethodID(env, contextClass,
            "getApplicationContext", "()Landroid/content/Context;");
    if (getApplicationContextMethod == 0)
    {
        OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "[WIFICore] Could not get getApplicationContext method");
        return;
    }

    jobject gApplicationContext = (*env)->CallObjectMethod(env, context, getApplicationContextMethod);
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "[WIFICore] Saving Android application context object %p", gApplicationContext);

   //Create WiFiInterface instance
    jclass WiFiJniInterface = (*env)->FindClass(env, "com/iotivity/jar/CAWiFiInterface");
    if (!WiFiJniInterface)
    {
        OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "[WIFICore] Could not get CAWiFiInterface class");
        return;
    }

    jmethodID WiFiInterfaceConstructorMethod = (*env)->GetMethodID(env,
            WiFiJniInterface, "<init>", "(Landroid/content/Context;)V");
    if (!WiFiInterfaceConstructorMethod)
    {
        OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "[WIFICore] Could not get CAWiFiInterface constructor method");
        return;
    }

    (*env)->NewObject(env, WiFiJniInterface, WiFiInterfaceConstructorMethod, gApplicationContext);
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "[WIFICore]Create CAWiFiInterface instance");
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "[WIFICore] NewObject Successs");

}

void CASendNetworkChangeCallback(CANetworkStatus_t currNetworkStatus)
{
    // Get network information
    char *interfaceName = NULL;
    char *ipAddress = NULL;
    char *subnetMask = NULL;

    u_mutex_lock(g_wifiNetInfoMutex);

    if (g_stopNetworkMonitor)
    {
        OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "Stop Network Monitor Thread is called");
        u_mutex_unlock(g_wifiNetInfoMutex);
        return;
    }

    if(NULL == g_networkChangeCb)
    {
        OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "[WiFiCore] g_networkChangeCb is NULL");
        u_mutex_lock(g_wifiNetInfoMutex);
        return;
    }

    // if network status is changed
    CAWiFiUpdateInterfaceInformation(&interfaceName, &ipAddress, & subnetMask);

    // set current network information
    g_nwConnectivityStatus = currNetworkStatus;

    g_wifiInterfaceName = (interfaceName) ? OICStrdup(interfaceName) : NULL;
    g_wifiIPAddress = (ipAddress) ? OICStrdup(ipAddress) : NULL;

    u_mutex_unlock(g_wifiNetInfoMutex);

    g_networkChangeCb(g_wifiIPAddress, g_nwConnectivityStatus);

    OICFree(interfaceName);
    OICFree(ipAddress);
    OICFree(subnetMask);
}

JNIEXPORT void JNICALL Java_com_iotivity_jar_cawifiinterface_CAWiFiStateEnabled
  (JNIEnv *env, jclass class)
{
    CANetworkStatus_t currNetworkStatus = CA_INTERFACE_UP;
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "[WiFiCore] CAWiFiStateEnabled");

    if (currNetworkStatus == g_nwConnectivityStatus)
    {
        OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "[WiFiCore] Network State not changed");
        return;
    }

    CASendNetworkChangeCallback(currNetworkStatus);
}


JNIEXPORT void JNICALL Java_com_iotivity_jar_cawifiinterface_CAWiFiStateDisabled
  (JNIEnv *env, jclass class)
{
    CANetworkStatus_t currNetworkStatus = CA_INTERFACE_DOWN;
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "[WiFiCore] CAWiFiStateDisabled");

    if (currNetworkStatus == g_nwConnectivityStatus)
    {
        OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "[WiFiCore] Network State not changed");
        return;
    }
    CASendNetworkChangeCallback(currNetworkStatus);
}


