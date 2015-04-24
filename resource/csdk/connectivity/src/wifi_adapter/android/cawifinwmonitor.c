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
#include "org_iotivity_ca_CaWiFiInterface.h"

#define WIFI_MONITOR_TAG "WIFI_MONITOR"
#define MAX_INTERFACE_INFO_LENGTH (1024)
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
static CAResult_t CAWiFiUpdateInterfaceInformation(char **interfaceName, char **ipAddress,
                                                   char **subnetMask);
/**
 * @fn CACreateWiFiJNIInterfaceObject
 * @brief creates new instance of cawifiinterface through JNI
 */
static CAResult_t CACreateWiFiJNIInterfaceObject(jobject context);

/**
 * @fn CASendNetworkChangeCallback
 * @brief updates network status to wifi adapter
 */
void CASendNetworkChangeCallback(CANetworkStatus_t currNetworkStatus);

CAResult_t CAWiFiJniInit()
{
    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "IN");

    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "CAWiFiJniInit");
    g_jvm = CANativeJNIGetJavaVM();

    if (!g_jvm)
    {
        OIC_LOG(ERROR, WIFI_MONITOR_TAG, "JNI initialize error");
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAJniSetContext()
{
    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "CAJniSetContext");
    g_context = (jobject) CANativeJNIGetContext();

    if (!g_context)
    {
        OIC_LOG(ERROR, WIFI_MONITOR_TAG, "unable to get application context");
        return CA_STATUS_FAILED;
    }

    return CA_STATUS_OK;
}

CAResult_t CAWiFiInitializeNetworkMonitor(const u_thread_pool_t threadPool)
{
    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "IN");

    CAResult_t ret = CAWiFiJniInit();
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG(ERROR, WIFI_MONITOR_TAG, "Init failed");
        return ret;
    }

    ret = CAJniSetContext();
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG(ERROR, WIFI_MONITOR_TAG, "set context failed");
        return ret;
    }

    g_threadPool = threadPool;

    if (!g_wifiNetInfoMutex)
    {
        g_wifiNetInfoMutex = u_mutex_new();
        if (!g_wifiNetInfoMutex)
        {
            OIC_LOG(ERROR, WIFI_MONITOR_TAG, "unable to create mutex");
            return CA_STATUS_FAILED;
        }
    }

    ret = CACreateWiFiJNIInterfaceObject(g_context);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG(ERROR, WIFI_MONITOR_TAG, "unable to create wifi interface instance");
        u_mutex_free(g_wifiNetInfoMutex);
        return ret;
    }

    u_mutex_lock(g_wifiNetInfoMutex);
    ret = CAWiFiUpdateInterfaceInformation(&g_wifiInterfaceName, &g_wifiIPAddress,
                                           &g_wifiSubnetMask);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG(ERROR, WIFI_MONITOR_TAG, "could not update network information during init");
    }

    u_mutex_unlock(g_wifiNetInfoMutex);

    g_nwConnectivityStatus = (g_wifiIPAddress) ? CA_INTERFACE_UP : CA_INTERFACE_DOWN;

    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

void CAWiFiTerminateNetworkMonitor()
{
    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "IN");

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

    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "OUT");
}

CAResult_t CAWiFiStartNetworkMonitor()
{
    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "IN");

    u_mutex_lock(g_wifiNetInfoMutex);
    g_stopNetworkMonitor = false;
    u_mutex_unlock(g_wifiNetInfoMutex);

    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAWiFiStopNetworkMonitor()
{
    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "IN");

    u_mutex_lock(g_wifiNetInfoMutex);

    if (g_stopNetworkMonitor)
    {
        OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "CAWiFiStopNetworkMonitor, already stopped");
        u_mutex_unlock(g_wifiNetInfoMutex);
        return CA_STATUS_OK;
    }

    g_stopNetworkMonitor = true;

    u_mutex_unlock(g_wifiNetInfoMutex);

    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAWiFiGetInterfaceInfo(char **interfaceName, char **ipAddress)
{
    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "IN");

    VERIFY_NON_NULL(interfaceName, WIFI_MONITOR_TAG, "interface name");
    VERIFY_NON_NULL(ipAddress, WIFI_MONITOR_TAG, "ip address");

    // Get the interface and ipaddress information from cache
    u_mutex_lock(g_wifiNetInfoMutex);

    if (!g_wifiInterfaceName || !g_wifiIPAddress)
    {
        OIC_LOG(ERROR, WIFI_MONITOR_TAG, "Network not enabled");
        u_mutex_unlock(g_wifiNetInfoMutex);
        return CA_ADAPTER_NOT_ENABLED;
    }

    *interfaceName = (g_wifiInterfaceName) ? OICStrdup((const char *) g_wifiInterfaceName) : NULL;
    *ipAddress = (g_wifiIPAddress) ? OICStrdup((const char *) g_wifiIPAddress) : NULL;

    u_mutex_unlock(g_wifiNetInfoMutex);

    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAWiFiGetInterfaceSubnetMask(char **subnetMask)
{
    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "IN");

    VERIFY_NON_NULL(subnetMask, WIFI_MONITOR_TAG, "subnet mask");

    u_mutex_lock(g_wifiNetInfoMutex);
    if (NULL == g_wifiSubnetMask)
    {
        OIC_LOG(ERROR, WIFI_MONITOR_TAG, "There is no subnet mask information!");
        u_mutex_unlock(g_wifiNetInfoMutex);
        return CA_STATUS_FAILED;
    }

    *subnetMask = (g_wifiSubnetMask) ? OICStrdup((const char *) g_wifiSubnetMask) : NULL;
    u_mutex_unlock(g_wifiNetInfoMutex);

    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

bool CAWiFiIsConnected()
{
    return (g_nwConnectivityStatus == CA_INTERFACE_UP);
}

void CAWiFiSetConnectionStateChangeCallback(CAWiFiConnectionStateChangeCallback callback)
{
    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "IN");

    g_networkChangeCb = callback;

    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "OUT");
}

CAResult_t CAWiFiUpdateInterfaceInformation(char **interfaceName, char **ipAddress,
                                            char **subnetMask)
{
    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "IN");

    VERIFY_NON_NULL(interfaceName, WIFI_MONITOR_TAG, "interfaceName");
    VERIFY_NON_NULL(ipAddress, WIFI_MONITOR_TAG, "ipAddress");
    VERIFY_NON_NULL(subnetMask, WIFI_MONITOR_TAG, "subnetMask");

    /* Get a socket handle. */
    int sck = socket(AF_INET, SOCK_DGRAM, 0);
    if (sck < 0)
    {
        OIC_LOG(ERROR, WIFI_MONITOR_TAG, "Error in socket creation");
        return CA_STATUS_FAILED;
    }

    char buf[MAX_INTERFACE_INFO_LENGTH] = { 0 };
    struct ifconf ifc;

    /* Query available interfaces. */
    ifc.ifc_len = MAX_INTERFACE_INFO_LENGTH;
    ifc.ifc_buf = buf;

    if (ioctl(sck, SIOCGIFCONF, &ifc) < 0)
    {
        close(sck);
        OIC_LOG(ERROR, WIFI_MONITOR_TAG, "Failed to get interface info");
        return CA_STATUS_FAILED;
    }

    /* Iterate through the list of interfaces. */
    struct ifreq* ifr = ifc.ifc_req;
    int32_t interfaces = ifc.ifc_len / sizeof(struct ifreq);

    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "CAWiFiUpdateInterfaceInformation : %d", interfaces);

    int32_t i;
    for (i = 0; i < interfaces; i++)
    {
        struct ifreq temp_ifr = { 0, };
        struct ifreq* item = &ifr[i];

        char interfaceAddress[CA_IPADDR_SIZE] = { 0 };
        char interfaceSubnetMask[CA_IPADDR_SIZE] = { 0 };
        strcpy(temp_ifr.ifr_name, item->ifr_name);

        if (ioctl(sck, SIOCGIFFLAGS, &temp_ifr))
        {
            OIC_LOG(ERROR, WIFI_MONITOR_TAG,
                    "CAWiFiUpdateInterfaceInformation, SIOCGIFFLAGS Failed");
            close(sck);
            return CA_STATUS_FAILED;
        }

        if ((temp_ifr.ifr_flags & IFF_LOOPBACK)
            || (!((temp_ifr.ifr_flags & IFF_UP) && (temp_ifr.ifr_flags & IFF_RUNNING))))
        {
            continue;
        }

        if (AF_INET != ((struct sockaddr_in*) &item->ifr_addr)->sin_family)
        {
            continue;
        }

        const char *matchName = "wlan";

        if (!strncasecmp(item->ifr_name, matchName, strlen(matchName)))
        {
            socklen_t len = sizeof(struct sockaddr_in);

            //get the interface ip address
            if (0 != getnameinfo(&item->ifr_addr, len, interfaceAddress, sizeof(interfaceAddress),
                                 NULL, 0, NI_NUMERICHOST))
            {
                OIC_LOG_V(ERROR, WIFI_MONITOR_TAG, "Failed to get IPAddress, Error code: %s",
                          strerror(errno));
                close(sck);
                return CA_STATUS_FAILED;
            }

            // set interface name
            *interfaceName = OICStrdup(item->ifr_name);

            // set local ip address
            *ipAddress = OICStrdup(interfaceAddress);

            if (ioctl((int) sck, SIOCGIFNETMASK, item) < 0)
            {
                OIC_LOG(ERROR, WIFI_MONITOR_TAG,
                        "CAWiFiUpdateInterfaceInformation, SIOCGIFNETMASK Failed");
                close(sck);
                return CA_STATUS_FAILED;
            }

            // get the interface subnet mask
            if (0
                != getnameinfo(&item->ifr_netmask, len, interfaceSubnetMask,
                               sizeof(interfaceSubnetMask), NULL, 0, NI_NUMERICHOST))
            {
                OIC_LOG_V(ERROR, WIFI_MONITOR_TAG, "Failed to get subnet mask, Error code: %s",
                          strerror(errno));
                close(sck);
                return CA_STATUS_FAILED;
            }

            *subnetMask = strndup(interfaceSubnetMask, strlen(interfaceSubnetMask));

            OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "ipAddress : %s, interfaceName : %s, subnetmask : %s",
                      *ipAddress, *interfaceName, *subnetMask);
            close(sck);
            return CA_STATUS_OK;
        }
        else
        {
            OIC_LOG(ERROR, WIFI_MONITOR_TAG, "did not match name : wlan");
        }

        break;
    }

    close(sck);
    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "OUT");
    return CA_STATUS_FAILED;
}

CAResult_t CACreateWiFiJNIInterfaceObject(jobject context)
{
    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "IN");

    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "CACreateWiFiJNIInterfaceObject");

    VERIFY_NON_NULL(context, WIFI_MONITOR_TAG, "context");

    JNIEnv* env;

    if ((*g_jvm)->GetEnv(g_jvm, (void**) &env, JNI_VERSION_1_6) != JNI_OK)
    {
        OIC_LOG(ERROR, WIFI_MONITOR_TAG, "Could not get JNIEnv pointer");
        return CA_STATUS_FAILED;
    }

    //getApplicationContext
    jclass contextClass = (*env)->FindClass(env, "android/content/Context");
    if (!contextClass)
    {
        OIC_LOG(ERROR, WIFI_MONITOR_TAG, "Could not get context object class");
        return CA_STATUS_FAILED;
    }

    jmethodID getApplicationContextMethod = (*env)->GetMethodID(env, contextClass,
                                                                "getApplicationContext",
                                                                "()Landroid/content/Context;");
    if (!getApplicationContextMethod)
    {
        OIC_LOG(ERROR, WIFI_MONITOR_TAG, "Could not get getApplicationContext method");
        return CA_STATUS_FAILED;
    }

    jobject gApplicationContext = (*env)->CallObjectMethod(env, context,
                                                           getApplicationContextMethod);
    if (!getApplicationContextMethod)
    {
        OIC_LOG(ERROR, WIFI_MONITOR_TAG, "Could not get getApplicationContext");
        return CA_STATUS_FAILED;
    }

    //Create WiFiInterface instance
    jclass WiFiJniInterface = (*env)->FindClass(env, "org/iotivity/ca/CaWiFiInterface");
    if (!WiFiJniInterface)
    {
        OIC_LOG(ERROR, WIFI_MONITOR_TAG, "Could not get CaWiFiInterface class");
        return CA_STATUS_FAILED;
    }

    jmethodID WiFiInterfaceConstructorMethod = (*env)->GetMethodID(env, WiFiJniInterface, "<init>",
                                                                   "(Landroid/content/Context;)V");
    if (!WiFiInterfaceConstructorMethod)
    {
        OIC_LOG(ERROR, WIFI_MONITOR_TAG, "Could not get CaWiFiInterface constructor method");
        return CA_STATUS_FAILED;
    }

    (*env)->NewObject(env, WiFiJniInterface, WiFiInterfaceConstructorMethod, gApplicationContext);
    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "[WIFICore]Create CaWiFiInterface instance, success");

    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

void CASendNetworkChangeCallback(CANetworkStatus_t currNetworkStatus)
{
    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "IN");
    u_mutex_lock(g_wifiNetInfoMutex);

    if (g_stopNetworkMonitor)
    {
        OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "Stop Network Monitor Thread is called");
        u_mutex_unlock(g_wifiNetInfoMutex);
        return;
    }

    if (!g_networkChangeCb)
    {
        OIC_LOG(ERROR, WIFI_MONITOR_TAG, "g_networkChangeCb is NULL");
        u_mutex_unlock(g_wifiNetInfoMutex);
        return;
    }

    // Get network information
    char *interfaceName = NULL;
    char *ipAddress = NULL;
    char *subnetMask = NULL;

    // if network status is changed
    CAResult_t ret = CAWiFiUpdateInterfaceInformation(&interfaceName, &ipAddress, &subnetMask);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG(ERROR, WIFI_MONITOR_TAG, "could not update interface information");
    }

    // set current network information
    g_nwConnectivityStatus = currNetworkStatus;

    g_wifiInterfaceName = (interfaceName) ? OICStrdup(interfaceName) : NULL;
    g_wifiIPAddress = (ipAddress) ? OICStrdup(ipAddress) : NULL;
    g_wifiSubnetMask = subnetMask ? OICStrdup(subnetMask) : NULL;

    u_mutex_unlock(g_wifiNetInfoMutex);

    g_networkChangeCb(g_wifiIPAddress, g_nwConnectivityStatus);

    OICFree(interfaceName);
    OICFree(ipAddress);
    OICFree(subnetMask);
    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "OUT");
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_CaWiFiInterface_CAWiFiStateEnabled(JNIEnv *env, jclass class)
{
    CANetworkStatus_t currNetworkStatus = CA_INTERFACE_UP;
    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "CAWiFiStateEnabled");

    if (currNetworkStatus == g_nwConnectivityStatus)
    {
        OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "Network State not changed");
        return;
    }

    CASendNetworkChangeCallback(currNetworkStatus);
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_CaWiFiInterface_CAWiFiStateDisabled(JNIEnv *env, jclass class)
{
    CANetworkStatus_t currNetworkStatus = CA_INTERFACE_DOWN;
    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "CAWiFiStateDisabled");

    if (currNetworkStatus == g_nwConnectivityStatus)
    {
        OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "Network State not changed");
        return;
    }
    CASendNetworkChangeCallback(currNetworkStatus);
}

