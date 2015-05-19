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

#include "caipinterface.h"

#include <string.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>

#include "caadapterutils.h"
#include "camutex.h"
#include "logger.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "org_iotivity_ca_CaIpInterface.h"

#define IP_MONITOR_TAG "IP_MONITOR"
#define MAX_INTERFACE_INFO_LENGTH (1024)

/**
 * @var g_stopNetworkMonitor
 * @brief  Used to stop the network monitor thread.
 */
static bool g_stopNetworkMonitor = false;

/**
 * @var g_stopNetworkMonitorMutex
 * @brief  Mutex for synchronizing access to g_stopNetworkMonitor flag.
 */
static ca_mutex g_stopNetworkMonitorMutex = NULL;

/**
 * @struct CAIPNwMonitorContext
 * @brief  Used for storing network monitor context information.
 */
typedef struct
{
    u_arraylist_t *netInterfaceList;
    ca_thread_pool_t threadPool;
    CANetworkStatus_t nwConnectivityStatus;
    CAIPConnectionStateChangeCallback networkChangeCb;
} CAIPNetworkMonitorContext;

/**
 * @var g_networkMonitorContext
 * @brief  network monitor context.
 */
static CAIPNetworkMonitorContext *g_networkMonitorContext = NULL;

/**
 * @var g_networkMonitorContextMutex
 * @brief  Mutex for synchronizing access to cached interface and IP address information.
 */
static ca_mutex g_networkMonitorContextMutex = NULL;

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
 * @fn CAIPUpdateInterfaceInformation
 * @brief This methods gets local interface name and IP address information.
 */
static CAResult_t CAIPUpdateInterfaceInformation(u_arraylist_t **netInterfaceList);
/**
 * @fn CACreateIPJNIInterfaceObject
 * @brief creates new instance of caipinterface through JNI
 */
static CAResult_t CACreateIPJNIInterfaceObject(jobject context);

/**
 * @fn CAIPSendNetworkChangeCallback
 * @brief updates network status to IP adapter
 */
static void CAIPSendNetworkChangeCallback(CANetworkStatus_t currNetworkStatus);

CAResult_t CAIPJniInit()
{
    OIC_LOG(DEBUG, IP_MONITOR_TAG, "IN");

    OIC_LOG(DEBUG, IP_MONITOR_TAG, "CAIPJniInit");
    g_jvm = CANativeJNIGetJavaVM();

    if (!g_jvm)
    {
        OIC_LOG(ERROR, IP_MONITOR_TAG, "JNI initialize error");
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, IP_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAIPJniSetContext()
{
    OIC_LOG(DEBUG, IP_MONITOR_TAG, "CAIPJniSetContext");
    g_context = (jobject) CANativeJNIGetContext();

    if (!g_context)
    {
        OIC_LOG(ERROR, IP_MONITOR_TAG, "unable to get application context");
        return CA_STATUS_FAILED;
    }

    return CA_STATUS_OK;
}

CAResult_t CACreateIPJNIInterfaceObject(jobject context)
{
    OIC_LOG(DEBUG, IP_MONITOR_TAG, "IN");

    OIC_LOG(DEBUG, IP_MONITOR_TAG, "CACreateIPJNIInterfaceObject");

    VERIFY_NON_NULL(context, IP_MONITOR_TAG, "context");

    JNIEnv* env;

    if ((*g_jvm)->GetEnv(g_jvm, (void**) &env, JNI_VERSION_1_6) != JNI_OK)
    {
        OIC_LOG(ERROR, IP_MONITOR_TAG, "Could not get JNIEnv pointer");
        return CA_STATUS_FAILED;
    }

    //getApplicationContext
    jclass contextClass = (*env)->FindClass(env, "android/content/Context");
    if (!contextClass)
    {
        OIC_LOG(ERROR, IP_MONITOR_TAG, "Could not get context object class");
        return CA_STATUS_FAILED;
    }

    jmethodID getApplicationContextMethod = (*env)->GetMethodID(env, contextClass,
                                                                "getApplicationContext",
                                                                "()Landroid/content/Context;");
    if (!getApplicationContextMethod)
    {
        OIC_LOG(ERROR, IP_MONITOR_TAG, "Could not get getApplicationContext method");
        return CA_STATUS_FAILED;
    }

    jobject gApplicationContext = (*env)->CallObjectMethod(env, context,
                                                           getApplicationContextMethod);
    if (!getApplicationContextMethod)
    {
        OIC_LOG(ERROR, IP_MONITOR_TAG, "Could not get getApplicationContext");
        return CA_STATUS_FAILED;
    }

    //Create caipinterface jni instance
    jclass IPJniInterface = (*env)->FindClass(env, "org/iotivity/ca/CaIpInterface");
    if (!IPJniInterface)
    {
        OIC_LOG(ERROR, IP_MONITOR_TAG, "Could not get caipinterface class");
        return CA_STATUS_FAILED;
    }

    jmethodID IPInterfaceConstructorMethod = (*env)->GetMethodID(env, IPJniInterface, "<init>",
                                                                   "(Landroid/content/Context;)V");
    if (!IPInterfaceConstructorMethod)
    {
        OIC_LOG(ERROR, IP_MONITOR_TAG, "Could not get caipinterface constructor method");
        return CA_STATUS_FAILED;
    }

    (*env)->NewObject(env, IPJniInterface, IPInterfaceConstructorMethod, gApplicationContext);
    OIC_LOG(DEBUG, IP_MONITOR_TAG, "Create caipinterface instance, success");

    OIC_LOG(DEBUG, IP_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

static CAResult_t CAIPUpdateInterfaceInformation(u_arraylist_t **netInterfaceList)
{
    OIC_LOG(DEBUG, IP_MONITOR_TAG, "IN");

    VERIFY_NON_NULL(netInterfaceList, IP_MONITOR_TAG, "netInterfaceList is null");

    /* Get a socket handle. */
    int sck = -1;
#ifdef SOCK_CLOEXEC
    sck = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, IPPROTO_UDP);
#endif

    if ( -1 == sck)
    {
        sck=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    }

    if (sck < 0)
    {
        OIC_LOG(ERROR, IP_MONITOR_TAG, "Error in socket creation");
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
        OIC_LOG(ERROR, IP_MONITOR_TAG, "Failed to get interface info");
        return CA_STATUS_FAILED;
    }

    /* Iterate through the list of interfaces. */
    struct ifreq* ifr = ifc.ifc_req;
    int32_t interfaces = ifc.ifc_len / sizeof(struct ifreq);

    OIC_LOG_V(DEBUG, IP_MONITOR_TAG, "CAIPUpdateInterfaceInformation : %d", interfaces);

    for (int32_t i = 0; i < interfaces; i++)
    {
        struct ifreq temp_ifr = { 0 };
        struct ifreq* item = &ifr[i];

        char interfaceAddress[CA_IPADDR_SIZE] = { 0 };
        char interfaceSubnetMask[CA_IPADDR_SIZE] = { 0 };
        socklen_t len = sizeof(struct sockaddr_in);

        strcpy(temp_ifr.ifr_name, item->ifr_name);

        if (ioctl(sck, SIOCGIFFLAGS, &temp_ifr))
        {
            OIC_LOG(ERROR, IP_MONITOR_TAG,
                    "CAIPUpdateInterfaceInformation, SIOCGIFFLAGS Failed");
            close(sck);
            return CA_STATUS_FAILED;
        }

        if ((temp_ifr.ifr_flags & IFF_LOOPBACK)
            || !(temp_ifr.ifr_flags & IFF_UP) || !(temp_ifr.ifr_flags & IFF_RUNNING))
        {
            continue;
        }

        if (AF_INET != ((struct sockaddr_in*) &item->ifr_addr)->sin_family)
        {
            continue;
        }

        //get the interface ip address
        if (0 != getnameinfo(&item->ifr_addr, len, interfaceAddress, sizeof(interfaceAddress),
                             NULL, 0, NI_NUMERICHOST))
        {
            OIC_LOG_V(ERROR, IP_MONITOR_TAG, "Failed to get IPAddress, Error code: %s",
                    strerror(errno));
            close(sck);
            return CA_STATUS_FAILED;
        }

        if (ioctl((int) sck, SIOCGIFNETMASK, item) < 0)
        {
            OIC_LOG(ERROR, IP_MONITOR_TAG,
                    "CAIPUpdateInterfaceInformation, SIOCGIFNETMASK Failed");
            close(sck);
            return CA_STATUS_FAILED;
        }

        // get the interface subnet mask
        if (0 != getnameinfo(&item->ifr_netmask, len, interfaceSubnetMask,
                             sizeof(interfaceSubnetMask), NULL, 0, NI_NUMERICHOST))
        {
            OIC_LOG_V(ERROR, IP_MONITOR_TAG, "Failed to get subnet mask, Error code: %s",
                    strerror(errno));
            close(sck);
            return CA_STATUS_FAILED;
        }

        CANetInfo_t *netInfo = (CANetInfo_t *) OICCalloc(1, sizeof(CANetInfo_t));
        if (!netInfo)
        {
            OIC_LOG(ERROR, IP_MONITOR_TAG, "Malloc failed");
            close(sck);
            return CA_MEMORY_ALLOC_FAILED;
        }

        // set interface name
        strncpy(netInfo->interfaceName, item->ifr_name, strlen(item->ifr_name));

        // set local ip address
        strncpy(netInfo->ipAddress, interfaceAddress, strlen(interfaceAddress));

        // set subnet mask
        strncpy(netInfo->subnetMask, interfaceSubnetMask, strlen(interfaceSubnetMask));

        CAResult_t result = u_arraylist_add(*netInterfaceList, (void *) netInfo);
        if (CA_STATUS_OK != result)
        {
            OIC_LOG(ERROR, IP_MONITOR_TAG, "u_arraylist_add failed!Thread exiting.");
            close(sck);
            return CA_STATUS_FAILED;
        }

        OIC_LOG_V(DEBUG, IP_MONITOR_TAG, "ipAddress : %s, interfaceName : %s, subnetmask : %s",
                netInfo->ipAddress, netInfo->interfaceName, netInfo->subnetMask);
        close(sck);
        return CA_STATUS_OK;

        break;
    }

    close(sck);
    OIC_LOG(DEBUG, IP_MONITOR_TAG, "OUT");
    return CA_STATUS_FAILED;
}

static bool CACheckIsAnyInterfaceDown(const u_arraylist_t *netInterfaceList,
                                      const CANetInfo_t *info)
{
    VERIFY_NON_NULL_RET(netInterfaceList, IP_MONITOR_TAG, "netInterfaceList is null", false);
    VERIFY_NON_NULL_RET(info, IP_MONITOR_TAG, "info is null", false);

    uint32_t list_length = u_arraylist_length(netInterfaceList);
    for (uint32_t list_index = 0; list_index < list_length; list_index++)
    {
        CANetInfo_t *netInfo = (CANetInfo_t *) u_arraylist_get(netInterfaceList, list_index);
        if (!netInfo)
        {
            continue;
        }
        if (strncmp(netInfo->interfaceName, info->interfaceName, strlen(info->interfaceName)) == 0)
        {
            return false;
        }
    }
    OIC_LOG(DEBUG, IP_MONITOR_TAG, "Interface is down");
    return true;
}

static bool CACheckIsInterfaceInfoChanged(const CANetInfo_t *info)
{
    VERIFY_NON_NULL_RET(info, IP_MONITOR_TAG, "info is null", false);

    ca_mutex_lock(g_networkMonitorContextMutex);

    uint32_t list_length = u_arraylist_length(g_networkMonitorContext->netInterfaceList);
    for (uint32_t list_index = 0; list_index < list_length; list_index++)
    {
        CANetInfo_t *netInfo = (CANetInfo_t *) u_arraylist_get(
                               g_networkMonitorContext->netInterfaceList, list_index);
        if (!netInfo)
        {
            continue;
        }
        if (strncmp(netInfo->interfaceName, info->interfaceName, strlen(info->interfaceName)) == 0)
        {
            if (strncmp(netInfo->ipAddress, info->ipAddress, strlen(info->ipAddress)) == 0)
            {
                ca_mutex_unlock(g_networkMonitorContextMutex);
                return false;
            }
            else
            {
                OIC_LOG(DEBUG, IP_MONITOR_TAG, "Network interface info changed");
                if (u_arraylist_remove(g_networkMonitorContext->netInterfaceList, list_index))
                {
                    if (g_networkMonitorContext->networkChangeCb)
                    {
                        g_networkMonitorContext->networkChangeCb(netInfo->ipAddress,
                                                                 CA_INTERFACE_DOWN);
                    }
                    OICFree(netInfo);
                }
                else
                {
                    OIC_LOG(ERROR, IP_MONITOR_TAG, "u_arraylist_remove failed");
                }
                break;
            }
        }
    }

    CANetInfo_t *newNetInfo = (CANetInfo_t *) OICMalloc(sizeof(CANetInfo_t));
    if (!newNetInfo)
    {
        OIC_LOG(ERROR, IP_MONITOR_TAG, "newNetInfo malloc failed");
        ca_mutex_unlock(g_networkMonitorContextMutex);
        return false;
    }
    memcpy(newNetInfo, info, sizeof(*newNetInfo));

    OIC_LOG(DEBUG, IP_MONITOR_TAG, "New Interface found");

    CAResult_t result = u_arraylist_add(g_networkMonitorContext->netInterfaceList,
                                        (void *) newNetInfo);
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, IP_MONITOR_TAG, "u_arraylist_add failed!");
        OICFree(newNetInfo);
        ca_mutex_unlock(g_networkMonitorContextMutex);
        return false;
    }
    ca_mutex_unlock(g_networkMonitorContextMutex);

    /*Callback will be unset only at the time of termination. By that time, all the threads will be
      stopped gracefully. This callback is properly protected*/
    if (g_networkMonitorContext->networkChangeCb)
    {
        g_networkMonitorContext->networkChangeCb(newNetInfo->ipAddress, CA_INTERFACE_UP);
    }

    return true;
}

static CAResult_t CAInitializeNetworkMonitorMutexes()
{
    if (!g_networkMonitorContextMutex)
    {
        g_networkMonitorContextMutex = ca_mutex_new();
        if (!g_networkMonitorContextMutex)
        {
            OIC_LOG(ERROR, IP_MONITOR_TAG, "g_networkMonitorContextMutex Malloc  failed");
            return CA_MEMORY_ALLOC_FAILED;
        }
    }

    if (!g_stopNetworkMonitorMutex)
    {
        g_stopNetworkMonitorMutex = ca_mutex_new();
        if (!g_stopNetworkMonitorMutex)
        {
            OIC_LOG(ERROR, IP_MONITOR_TAG, "g_stopNetworkMonitorMutex Malloc  failed");
            ca_mutex_free(g_networkMonitorContextMutex);
            return CA_MEMORY_ALLOC_FAILED;
        }
    }
    return CA_STATUS_OK;
}

static void CADestroyNetworkMonitorMutexes()
{
    ca_mutex_free(g_networkMonitorContextMutex);
    g_networkMonitorContextMutex = NULL;

    ca_mutex_free(g_stopNetworkMonitorMutex);
    g_stopNetworkMonitorMutex = NULL;
}

CAResult_t CAIPInitializeNetworkMonitor(const ca_thread_pool_t threadPool)
{
    OIC_LOG(DEBUG, IP_MONITOR_TAG, "CAIPInitializeNetworkMonitor IN");

    VERIFY_NON_NULL(threadPool, IP_MONITOR_TAG, "threadPool is null");

    CAResult_t ret = CAIPJniInit();
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG(ERROR, IP_MONITOR_TAG, "Initialization failed");
        return ret;
    }

    ret = CAIPJniSetContext();
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG(ERROR, IP_MONITOR_TAG, "CAIPJniSetContext failed");
        return ret;
    }

    ret = CACreateIPJNIInterfaceObject(g_context);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG(ERROR, IP_MONITOR_TAG, "unable to create caipinterface instance");
        return ret;
    }

    ret = CAInitializeNetworkMonitorMutexes();

    if (CA_STATUS_OK != ret)
    {
        OIC_LOG(ERROR, IP_MONITOR_TAG, "CAInitializeNetworkMonitorMutexes failed");
        return CA_STATUS_FAILED;
    }

    ca_mutex_lock(g_networkMonitorContextMutex);

    g_networkMonitorContext = (CAIPNetworkMonitorContext *) OICCalloc(
            1, sizeof(*g_networkMonitorContext));
    if (!g_networkMonitorContext)
    {
        OIC_LOG(ERROR, IP_MONITOR_TAG, "g_networkMonitorContext Malloc  failed");
        ca_mutex_unlock(g_networkMonitorContextMutex);
        CADestroyNetworkMonitorMutexes();
        return CA_MEMORY_ALLOC_FAILED;
    }
    g_networkMonitorContext->threadPool = threadPool;

    g_networkMonitorContext->netInterfaceList = u_arraylist_create();
    if (!g_networkMonitorContext->netInterfaceList)
    {
        OIC_LOG(ERROR, IP_MONITOR_TAG, "u_arraylist_create failed");
        OICFree(g_networkMonitorContext);
        ca_mutex_unlock(g_networkMonitorContextMutex);
        CADestroyNetworkMonitorMutexes();
        return CA_MEMORY_ALLOC_FAILED;
    }

    CAIPUpdateInterfaceInformation(&g_networkMonitorContext->netInterfaceList);

    if (u_arraylist_length(g_networkMonitorContext->netInterfaceList))
    {
        g_networkMonitorContext->nwConnectivityStatus = CA_INTERFACE_UP;
    }
    else
    {
        g_networkMonitorContext->nwConnectivityStatus = CA_INTERFACE_DOWN;
    }

    ca_mutex_unlock(g_networkMonitorContextMutex);

    OIC_LOG(DEBUG, IP_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

void CAIPTerminateNetworkMonitor()
{
    OIC_LOG(DEBUG, IP_MONITOR_TAG, "IN");

    ca_mutex_lock(g_networkMonitorContextMutex);
    g_networkMonitorContext->threadPool = NULL;

    CAClearNetInterfaceInfoList(g_networkMonitorContext->netInterfaceList);

    g_networkMonitorContext->netInterfaceList = NULL;
    g_networkMonitorContext->nwConnectivityStatus = CA_INTERFACE_DOWN;
    g_networkMonitorContext->networkChangeCb = NULL;
    g_networkMonitorContext->threadPool = NULL;

    OICFree(g_networkMonitorContext);
    g_networkMonitorContext = NULL;

    ca_mutex_unlock(g_networkMonitorContextMutex);

    ca_mutex_lock(g_stopNetworkMonitorMutex);
    g_stopNetworkMonitor = true;
    ca_mutex_unlock(g_stopNetworkMonitorMutex);

    CADestroyNetworkMonitorMutexes();

    OIC_LOG(DEBUG, IP_MONITOR_TAG, "OUT");
}

CAResult_t CAIPStartNetworkMonitor()
{
    OIC_LOG(DEBUG, IP_MONITOR_TAG, "IN");

    ca_mutex_lock(g_stopNetworkMonitorMutex);
    g_stopNetworkMonitor = false;
    ca_mutex_unlock(g_stopNetworkMonitorMutex);

    OIC_LOG(DEBUG, IP_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAIPStopNetworkMonitor()
{
    OIC_LOG(DEBUG, IP_MONITOR_TAG, "IN");

    ca_mutex_lock(g_networkMonitorContextMutex);
    if (!g_networkMonitorContext)
    {
        OIC_LOG(ERROR, IP_MONITOR_TAG, "g_networkMonitorContext is null");
        ca_mutex_unlock(g_networkMonitorContextMutex);
        return CA_STATUS_FAILED;
    }

    ca_mutex_unlock(g_networkMonitorContextMutex);

    ca_mutex_lock(g_stopNetworkMonitorMutex);
    if (!g_stopNetworkMonitor)
    {
        g_stopNetworkMonitor = true;
    }
    else
    {
        OIC_LOG(DEBUG, IP_MONITOR_TAG, "CAIPStopNetworkMonitor, already stopped!");
    }
    ca_mutex_unlock(g_stopNetworkMonitorMutex);

    OIC_LOG(DEBUG, IP_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAIPGetInterfaceInfo(u_arraylist_t **netInterfaceList)
{
    OIC_LOG(DEBUG, IP_MONITOR_TAG, "IN");

    VERIFY_NON_NULL(netInterfaceList, IP_MONITOR_TAG, "u_array_list is null");
    VERIFY_NON_NULL(g_networkMonitorContext, IP_MONITOR_TAG, "g_networkMonitorContext is null");
    VERIFY_NON_NULL(g_networkMonitorContextMutex, IP_MONITOR_TAG,
                    "g_networkMonitorContextMutex is null");

    // Get the interface and ipaddress information from cache
    ca_mutex_lock(g_networkMonitorContextMutex);
    if (!g_networkMonitorContext->netInterfaceList
        || !(u_arraylist_length(g_networkMonitorContext->netInterfaceList)))
    {
        OIC_LOG(ERROR, IP_MONITOR_TAG, "Network not enabled");
        ca_mutex_unlock(g_networkMonitorContextMutex);
        return CA_ADAPTER_NOT_ENABLED;
    }

    uint32_t list_length = u_arraylist_length(g_networkMonitorContext->netInterfaceList);
    OIC_LOG_V(DEBUG, IP_MONITOR_TAG, "CAIPGetInterfaceInfo list length [%d]",
            list_length);
    for (uint32_t list_index = 0; list_index < list_length; list_index++)
    {
        CANetInfo_t *info = (CANetInfo_t *) u_arraylist_get(
                g_networkMonitorContext->netInterfaceList, list_index);
        if (!info)
        {
            continue;
        }
        OIC_LOG_V(DEBUG, IP_MONITOR_TAG, "CAIPGetInterfaceInfo ip [%s]",
                  info->ipAddress);
        CANetInfo_t *newNetinfo = (CANetInfo_t *) OICMalloc(sizeof(CANetInfo_t));
        if (!newNetinfo)
        {
            OIC_LOG(ERROR, IP_MONITOR_TAG, "Malloc failed!");
            ca_mutex_unlock(g_networkMonitorContextMutex);
            return CA_MEMORY_ALLOC_FAILED;
        }

        memcpy(newNetinfo, info, sizeof(*info));

        CAResult_t result = u_arraylist_add(*netInterfaceList, (void *) newNetinfo);
        if (CA_STATUS_OK != result)
        {
            OIC_LOG(ERROR, IP_MONITOR_TAG, "u_arraylist_add failed!");
            ca_mutex_unlock(g_networkMonitorContextMutex);
            return CA_STATUS_FAILED;
        }
    }

    ca_mutex_unlock(g_networkMonitorContextMutex);

    OIC_LOG(DEBUG, IP_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAIPGetInterfaceSubnetMask(const char *ipAddress, char **subnetMask)
{
    OIC_LOG(DEBUG, IP_MONITOR_TAG, "IN");

    VERIFY_NON_NULL(subnetMask, IP_MONITOR_TAG, "subnet mask");
    VERIFY_NON_NULL(ipAddress, IP_MONITOR_TAG, "ipAddress is null");
    VERIFY_NON_NULL(g_networkMonitorContext, IP_MONITOR_TAG, "g_networkMonitorContext is null");
    VERIFY_NON_NULL(g_networkMonitorContextMutex, IP_MONITOR_TAG,
                    "g_networkMonitorContextMutex is null");

    // Get the interface and ipaddress information from cache
    ca_mutex_lock(g_networkMonitorContextMutex);
    if (!g_networkMonitorContext->netInterfaceList
        || (0 == u_arraylist_length(g_networkMonitorContext->netInterfaceList)))
    {
        OIC_LOG(DEBUG, IP_MONITOR_TAG, "Network not enabled");
        ca_mutex_unlock(g_networkMonitorContextMutex);
        return CA_ADAPTER_NOT_ENABLED;
    }

    uint32_t list_length = u_arraylist_length(g_networkMonitorContext->netInterfaceList);
    OIC_LOG_V(DEBUG, IP_MONITOR_TAG, "list lenght [%d]", list_length);
    for (uint32_t list_index = 0; list_index < list_length; list_index++)
    {
        CANetInfo_t *info = (CANetInfo_t *) u_arraylist_get(
                g_networkMonitorContext->netInterfaceList, list_index);
        if (!info)
        {
            continue;
        }

        if (strncmp(info->ipAddress, ipAddress, strlen(ipAddress)) == 0)
        {
            OIC_LOG_V(DEBUG, IP_MONITOR_TAG,
                      "CAIPGetInterfaceSubnetMask subnetmask is %s", info->subnetMask);
            *subnetMask = OICStrdup(info->subnetMask);
            break;
        }
    }
    ca_mutex_unlock(g_networkMonitorContextMutex);

    OIC_LOG(DEBUG, IP_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

bool CAIPIsConnected()
{
    OIC_LOG(DEBUG, IP_MONITOR_TAG, "IN");
    if (!g_networkMonitorContextMutex || !g_networkMonitorContext)
    {
        OIC_LOG(ERROR, IP_MONITOR_TAG, "IP is not connected");
        return false;
    }

    ca_mutex_lock(g_networkMonitorContextMutex);
    if (0 == u_arraylist_length(g_networkMonitorContext->netInterfaceList))
    {
        OIC_LOG(ERROR, IP_MONITOR_TAG, "IP is not connected");
        ca_mutex_unlock(g_networkMonitorContextMutex);
        return false;
    }
    ca_mutex_unlock(g_networkMonitorContextMutex);

    OIC_LOG(DEBUG, IP_MONITOR_TAG, "OUT");
    return true;
}

void CAIPSetConnectionStateChangeCallback(CAIPConnectionStateChangeCallback callback)
{
    OIC_LOG(DEBUG, IP_MONITOR_TAG, "IN");
    if (!g_networkMonitorContextMutex || !g_networkMonitorContext)
    {
        OIC_LOG(ERROR, IP_MONITOR_TAG, "CAIPSetConnectionStateChangeCallback failed");
        return;
    }
    ca_mutex_lock(g_networkMonitorContextMutex);

    g_networkMonitorContext->networkChangeCb = callback;

    ca_mutex_unlock(g_networkMonitorContextMutex);

    OIC_LOG(DEBUG, IP_MONITOR_TAG, "OUT");
}

void CAIPSendNetworkChangeCallback(CANetworkStatus_t currNetworkStatus)
{
    OIC_LOG(DEBUG, IP_MONITOR_TAG, "IN");
    ca_mutex_lock(g_stopNetworkMonitorMutex);

    if (g_stopNetworkMonitor)
    {
        OIC_LOG(DEBUG, IP_MONITOR_TAG, "Stop Network Monitor Thread is called");
        ca_mutex_unlock(g_stopNetworkMonitorMutex);
        return;
    }
    ca_mutex_unlock(g_stopNetworkMonitorMutex);
    ca_mutex_lock(g_networkMonitorContextMutex);
    if(!g_networkMonitorContext)
    {
	OIC_LOG(DEBUG, IP_MONITOR_TAG, "g_networkChangeCb is NULL");
	ca_mutex_unlock(g_networkMonitorContextMutex);
	return;
    }
    if (!g_networkMonitorContext->networkChangeCb)
    {    
        OIC_LOG(ERROR, IP_MONITOR_TAG, "g_networkChangeCb->networkChangeCb is NULL");
        ca_mutex_unlock(g_networkMonitorContextMutex);
        return;
    }

    ca_mutex_unlock(g_networkMonitorContextMutex);
  
    u_arraylist_t *netInterfaceList = u_arraylist_create();

    VERIFY_NON_NULL_VOID(netInterfaceList, IP_MONITOR_TAG,
                         "memory allocation failed for netInterfaceList");

    // if network status is changed
    CAResult_t ret = CAIPUpdateInterfaceInformation(&netInterfaceList);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG(ERROR, IP_MONITOR_TAG, "could not update interface information");
    }

    ca_mutex_lock(g_networkMonitorContextMutex);
    if (!g_networkMonitorContext->netInterfaceList)
    {
        OIC_LOG(ERROR, IP_MONITOR_TAG,
                "u_arraylist_create failed. Network Monitor thread stopped");
        CAClearNetInterfaceInfoList(netInterfaceList);
        ca_mutex_unlock(g_networkMonitorContextMutex);
        return;
    }

    uint32_t listLength = u_arraylist_length(g_networkMonitorContext->netInterfaceList);
    for (uint32_t listIndex = 0; listIndex < listLength;)
    {
        CANetInfo_t *info = (CANetInfo_t *) u_arraylist_get(
                g_networkMonitorContext->netInterfaceList, listIndex);
        if (!info)
        {
            listIndex++;
            continue;
        }

        bool ret = CACheckIsAnyInterfaceDown(netInterfaceList, info);
        if (ret)
        {
            OIC_LOG(DEBUG, IP_MONITOR_TAG, "Interface is down");
            if (u_arraylist_remove(g_networkMonitorContext->netInterfaceList, listIndex))
            {
                OIC_LOG(DEBUG, IP_MONITOR_TAG, "u_arraylist_remove success");
                if (g_networkMonitorContext->networkChangeCb)
                {
                    g_networkMonitorContext->networkChangeCb(info->ipAddress, CA_INTERFACE_DOWN);
                }
                OICFree(info);
                listLength--;
            }
            else
            {
                OIC_LOG(ERROR, IP_MONITOR_TAG, "u_arraylist_remove failed");
                break;
            }
        }
        else
        {
            listIndex++;
        }
    }

    ca_mutex_unlock(g_networkMonitorContextMutex);

    listLength = u_arraylist_length(netInterfaceList);
    for (uint32_t listIndex = 0; listIndex < listLength; listIndex++)
    {
        CANetInfo_t *info = (CANetInfo_t *) u_arraylist_get(netInterfaceList, listIndex);
        if (!info)
        {
            continue;
        }
        bool ret = CACheckIsInterfaceInfoChanged(info);
        if (ret)
        {
            OIC_LOG(DEBUG, IP_MONITOR_TAG, "CACheckIsInterfaceInfoChanged true");
        }
    }

    CAClearNetInterfaceInfoList(netInterfaceList);
    OIC_LOG(DEBUG, IP_MONITOR_TAG, "OUT");
}

JNIEXPORT void JNICALL Java_org_iotivity_ca_CaIpInterface_stateEnabled
  (JNIEnv *env, jclass clazz)
{
    CANetworkStatus_t currNetworkStatus = CA_INTERFACE_UP;
    OIC_LOG(DEBUG, IP_MONITOR_TAG, "CAIPStateEnabled");

    CAIPSendNetworkChangeCallback(currNetworkStatus);
}

JNIEXPORT void JNICALL Java_org_iotivity_ca_CaIpInterface_stateDisabled
  (JNIEnv *env, jclass clazz)
{
    CANetworkStatus_t currNetworkStatus = CA_INTERFACE_DOWN;
    OIC_LOG(DEBUG, IP_MONITOR_TAG, "CAIPStateDisabled");

    CAIPSendNetworkChangeCallback(currNetworkStatus);
}

