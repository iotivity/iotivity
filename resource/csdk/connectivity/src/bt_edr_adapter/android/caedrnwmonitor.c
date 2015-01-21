#include <stdio.h>
#include <string.h>
#include <jni.h>

#include "caedrinterface.h"
#include "caedrnwmonitor.h"
#include "logger.h"
#include "oic_malloc.h"
#include "uthreadpool.h" /* for thread pool */
#include "umutex.h"
#include "uarraylist.h"
#include "caadapterutils.h"

#include "com_iotivity_jar_CAEDRInterface.h"

//#define DEBUG_MODE
#define TAG PCF("CA_EDR_MONITOR")

#define BT_STATE_ON         12
#define BT_STATE_OFF        10

// temp method
static JavaVM *g_jvm;
static jobject gContext;
static CAEDRNetworkStatusCallback gNetworkChangeCb = NULL;

void CAEDRNetworkMonitorJNISetContext()
{
    OIC_LOG_V(DEBUG, TAG, "CAEDRNetworkMonitorJNISetContext");
    gContext = CANativeJNIGetContext();
}

//getting jvm
void CAEDRNetworkMonitorJniInit()
{
    OIC_LOG_V(DEBUG, TAG, "CAEDRNetworkMonitorJniInit");
    g_jvm = CANativeJNIGetJavaVM();
}

CAResult_t CAEDRInitializeNetworkMonitor()
{
    OIC_LOG_V(DEBUG, TAG, "IN");

    CAEDRNetworkMonitorJniInit();
    CANativeJNIGetJavaVM();

    OIC_LOG_V(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

void CAEDRSetNetworkChangeCallback(
    CAEDRNetworkStatusCallback networkChangeCallback)
{
    OIC_LOG_V(DEBUG, TAG, "CAEDRSetNetworkChangeCallback");
    gNetworkChangeCb = networkChangeCallback;
}

void CAEDRTerminateNetworkMonitor(void)
{
    OIC_LOG_V(DEBUG, TAG, "IN");

    OIC_LOG_V(DEBUG, TAG, "OUT");
}

CAResult_t CAEDRStartNetworkMonitor()
{
    OIC_LOG_V(DEBUG, TAG, "IN");

    OIC_LOG_V(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAEDRStopNetworkMonitor()
{
    OIC_LOG_V(DEBUG, TAG, "IN");

    OIC_LOG_V(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAEDRClientSetCallbacks(void)
{
    OIC_LOG_V(DEBUG, TAG, "IN");

    OIC_LOG_V(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

JNIEXPORT void JNICALL
Java_com_iotivity_jar_CAEDRInterface_CAEDRStateChangedCallback
(JNIEnv *env, jobject obj, jint status)
{
    // STATE_ON:12, STATE_OFF:10
    OIC_LOG_V(DEBUG, TAG, "CAEDRInterface - Network State Changed");

    if(gNetworkChangeCb == NULL)
        OIC_LOG_V(DEBUG, TAG, "gNetworkChangeCb is null", status);

    if(BT_STATE_ON == status)
    {
        CANetworkStatus_t newStatus = CA_INTERFACE_UP;
        CAEDRServerStartAccpetThread();
        gNetworkChangeCb(newStatus);
    }
    else if(BT_STATE_OFF == status)
    {
        CANetworkStatus_t newStatus = CA_INTERFACE_DOWN;
        CAEDRNativeRemoveAllDeviceSocket(env);
        CAEDRNativeRemoveAllDeviceState(env);
        gNetworkChangeCb(newStatus);
    }
}

JNIEXPORT void JNICALL
Java_com_iotivity_jar_CAEDRInterface_CAEDRBondStateChangedCallback
(JNIEnv *env, jobject obj, jstring addr)
{
    OIC_LOG_V(DEBUG, TAG, "CAEDRInterface - Bond State Changed");

    if(addr)
    {
        CAEDRNativeRemoveDeviceSocketBaseAddr(env, addr);
        CAEDRNativeRemoveDevice(env, addr);
    }
}
