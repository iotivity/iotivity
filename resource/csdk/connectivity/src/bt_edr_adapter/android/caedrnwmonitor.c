#include <stdio.h>
#include <string.h>
#include <jni.h>

#include "caedrinterface.h"
#include "logger.h"
#include "oic_malloc.h"
#include "uthreadpool.h" /* for thread pool */
#include "umutex.h"
#include "uarraylist.h"
#include "caadapterutils.h"

//#define DEBUG_MODE
#define TAG PCF("CA_EDR_MONITOR")

// temp method

CAResult_t CAEDRInitializeNetworkMonitor()
{
    OIC_LOG_V(DEBUG, TAG, "IN");

    OIC_LOG_V(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

void CAEDRSetNetworkChangeCallback(
    CAEDRNetworkStatusCallback networkChangeCallback)
{
    OIC_LOG_V(DEBUG, TAG, "IN");

    OIC_LOG_V(DEBUG, TAG, "OUT");
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

