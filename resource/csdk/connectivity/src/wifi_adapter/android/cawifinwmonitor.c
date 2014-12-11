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
//#include <ifaddrs.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <netdb.h>

#include "caadapterutils.h"
#include "umutex.h"
#include "logger.h"
#include "oic_malloc.h"
#include "com_iotivity_jar_CAWiFiInterface.h"

#define WIFI_MONITOR_TAG "WIFI_MONITOR"

/**
 * @var nwConnectivityStatus
 * @brief  Maintains network status.
 */
static CANetworkStatus_t nwConnectivityStatus;

/**
 * @var gWifiNetInfoMutex
 * @brief  Mutex for synchronizing access to cached interface and IP address information.
 */
static u_mutex gWifiNetInfoMutex = NULL;

/**
 * @var gWifiInterfaceName
 * @brief  Maintains interface name.
 */
static char *gWifiInterfaceName = NULL;

/**
 * @var gWifiIPAddress
 * @brief  Maintains interface IP address.
 */
static char *gWifiIPAddress = NULL;

/**
 * @var gThreadPool
 * @brief ThreadPool for storing u_thread_pool_t handle passed from adapter
 */
static u_thread_pool_t gThreadPool = NULL;

/**
 * @var gStopNetworkMonitor
 * @brief Flag to control the Network Monitor Thread
 */
static bool gStopNetworkMonitor = false;

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
 * @var gNetworkChangeCb
 * @brief  Maintains network connection state change callback.
 */
static CAWiFiConnectionStateChangeCallback gNetworkChangeCb = NULL;

/**
 * @fn CAWiFiGetInterfaceInformation
 * @brief Gets local interface name and IP address information.
 */
static void CAWiFiGetInterfaceInformation(char **interfaceName, char **ipAddress);

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

    gThreadPool = threadPool;

    if (!gWifiNetInfoMutex)
    {
        gWifiNetInfoMutex = u_mutex_new();
    }

    CACreateWiFiJNIInterfaceObject(g_context);

    u_mutex_lock(gWifiNetInfoMutex);
    CAWiFiGetInterfaceInformation(&gWifiInterfaceName, &gWifiIPAddress);
    u_mutex_unlock(gWifiNetInfoMutex);

    nwConnectivityStatus = (gWifiIPAddress) ? CA_INTERFACE_UP : CA_INTERFACE_DOWN;

    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

void CAWiFiTerminateNetworkMonitor(void)
{
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "IN");

    gThreadPool = NULL;

    if (gWifiInterfaceName)
    {
        OICFree(gWifiInterfaceName);
        gWifiInterfaceName = NULL;
    }

    if (gWifiIPAddress)
    {
        OICFree(gWifiIPAddress);
        gWifiIPAddress = NULL;
    }

    if (gWifiNetInfoMutex)
    {
        u_mutex_free(gWifiNetInfoMutex);
        gWifiNetInfoMutex = NULL;
    }

    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "OUT");
}

CAResult_t CAWiFiStartNetworkMonitor(void)
{
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "IN");

    u_mutex_lock(gWifiNetInfoMutex);
    gStopNetworkMonitor = false;
    u_mutex_unlock(gWifiNetInfoMutex);

    if (gStopNetworkMonitor)
    {
        OIC_LOG_V(ERROR, WIFI_MONITOR_TAG, "Network Monitor Thread is already running!");
        return CA_SERVER_STARTED_ALREADY;
    }

    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAWiFiStopNetworkMonitor(void)
{
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "IN");

    if (gStopNetworkMonitor)
    {
        OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "CAWiFiStopNetworkMonitor, already stopped");

        return CA_STATUS_OK;
    }

    u_mutex_lock(gWifiNetInfoMutex);
    gStopNetworkMonitor = true;
    u_mutex_unlock(gWifiNetInfoMutex);

    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAWiFiGetInterfaceInfo(char **interfaceName, char **ipAddress)
{
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "IN");

    u_mutex_lock(gWifiNetInfoMutex);

    if (gWifiInterfaceName && strlen(gWifiInterfaceName))
    {
        *interfaceName = (gWifiInterfaceName) ? strndup(gWifiInterfaceName, strlen(gWifiInterfaceName)) :
                                NULL;
    }

    if (gWifiIPAddress && strlen(gWifiIPAddress))
    {
        *ipAddress = (gWifiIPAddress) ? strndup(gWifiIPAddress, strlen(gWifiIPAddress)) :
                                NULL;
    }

    u_mutex_unlock(gWifiNetInfoMutex);

    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

bool CAWiFiIsConnected(void)
{
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "IN");

    if (nwConnectivityStatus == CA_INTERFACE_DOWN)
        return false;

    return true;
}

void CAWiFiSetConnectionStateChangeCallback(CAWiFiConnectionStateChangeCallback callback)
{
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "IN");

    gNetworkChangeCb = callback;
}

void CAWiFiGetInterfaceInformation(char **interfaceName, char **ipAddress)
{
    char buf[1024] =  { 0, };
    struct ifconf ifc;
    struct ifreq* ifr;
    struct ifreq* item;
    int32_t sck;
    int32_t interfaces;
    int32_t i;

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

    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "CAWiFiGetInterfaceInformation : %d", interfaces);

    for (i = 0; i < interfaces; i++)
    {
        struct ifreq temp_ifr;
        struct ifreq* item = &ifr[i];

        memset(&temp_ifr, 0, sizeof(temp_ifr));
        strcpy(temp_ifr.ifr_name, item->ifr_name);

        if (ioctl((int)sck, SIOCGIFFLAGS, &temp_ifr))
        {
            OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "CAWiFiGetInterfaceInformation, SIOCGIFFLAGS Failed");
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

        if(strcmp(ip, "127.0.0.1") == 0)
        {
            continue;
        }

        // set interface name
        *interfaceName = strndup(item->ifr_name, strlen(item->ifr_name));

        // set local ip address
        *ipAddress = strndup(ip, strlen(ip));

       OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "ipAddress : %s, interfaceName : %s", *ipAddress, *interfaceName);
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

    if (gStopNetworkMonitor)
    {
        OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "Stop Network Monitor Thread is called");
        return;
    }

    if(NULL == gNetworkChangeCb)
    {
        OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "[WiFiCore] gNetworkChangeCb is NULL");
        return;
    }

    // if network status is changed
    if (currNetworkStatus != nwConnectivityStatus)
    {
        CAWiFiGetInterfaceInformation(&interfaceName, &ipAddress);

        // set current network information
        u_mutex_lock(gWifiNetInfoMutex);

        nwConnectivityStatus = currNetworkStatus;

        gWifiInterfaceName = (interfaceName) ? strndup(interfaceName, strlen(interfaceName)) : NULL;
        gWifiIPAddress = (ipAddress) ? strndup(ipAddress, strlen(ipAddress)) : NULL;

        u_mutex_unlock(gWifiNetInfoMutex);

        gNetworkChangeCb(gWifiIPAddress, nwConnectivityStatus);
    }
}

JNIEXPORT void JNICALL Java_com_iotivity_jar_CAWiFiInterface_CAWiFiStateEnabled
  (JNIEnv *env, jclass class)
{
    CALocalConnectivity_t info;

    CANetworkStatus_t currNetworkStatus = CA_INTERFACE_UP;
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "[WiFiCore] CAWiFiStateEnabled");

    CASendNetworkChangeCallback(currNetworkStatus);
    return;
}


JNIEXPORT void JNICALL Java_com_iotivity_jar_CAWiFiInterface_CAWiFiStateDisabled
  (JNIEnv *env, jclass class)
{
    CALocalConnectivity_t info;

    CANetworkStatus_t currNetworkStatus = CA_INTERFACE_DOWN;
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "[WiFiCore] CAWiFiStateDisabled");

    CASendNetworkChangeCallback(currNetworkStatus);
    return;
}

