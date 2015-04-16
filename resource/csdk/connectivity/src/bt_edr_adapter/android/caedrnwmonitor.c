#include <stdio.h>
#include <string.h>
#include <jni.h>

#include "caedrinterface.h"
#include "logger.h"
#include "oic_malloc.h"
#include "cathreadpool.h" /* for thread pool */
#include "camutex.h"
#include "uarraylist.h"
#include "caadapterutils.h"

//#define DEBUG_MODE
#define TAG PCF("CA_EDR_MONITOR")

// temp method

CAResult_t CAEDRInitializeNetworkMonitor()
{
    OIC_LOG(DEBUG, TAG, "IN");

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

void CAEDRSetNetworkChangeCallback(
    CAEDRNetworkStatusCallback networkChangeCallback)
{
    OIC_LOG(DEBUG, TAG, "IN");

    OIC_LOG(DEBUG, TAG, "OUT");
}

void CAEDRTerminateNetworkMonitor(void)
{
    OIC_LOG(DEBUG, TAG, "IN");

    OIC_LOG(DEBUG, TAG, "OUT");
}

CAResult_t CAEDRStartNetworkMonitor()
{
    OIC_LOG(DEBUG, TAG, "IN");

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAEDRStopNetworkMonitor()
{
    OIC_LOG(DEBUG, TAG, "IN");

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAEDRClientSetCallbacks(void)
{
    OIC_LOG(DEBUG, TAG, "IN");

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

