/* ****************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
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

#include "camanagerleinterface.h"
#include "cabtpairinginterface.h"
#include "cautilinterface.h"
#include "cainterfacecontroller.h"
#include "cacommon.h"
#include "logger.h"

#define TAG "OIC_CA_COMMON_UTILS"

CAResult_t CARegisterNetworkMonitorHandler(CAAdapterStateChangedCB adapterStateCB,
                                           CAConnectionStateChangedCB connStateCB)
{
    OIC_LOG(DEBUG, TAG, "CARegisterNetworkMonitorHandler");

    CASetNetworkMonitorCallbacks(adapterStateCB, connStateCB);
    return CA_STATUS_OK;
}

CAResult_t CASetAutoConnectionDeviceInfo(const char *address)
{
    OIC_LOG(DEBUG, TAG, "CASetAutoConnectionDeviceInfo");

#if defined(__ANDROID__) && defined(LE_ADAPTER)
    return CASetLEClientAutoConnectionDeviceInfo(address);
#else
    (void)address;
    return CA_NOT_SUPPORTED;
#endif
}

CAResult_t CAUnsetAutoConnectionDeviceInfo(const char *address)
{
    OIC_LOG(DEBUG, TAG, "CAUnsetAutoConnectionDeviceInfo");

#if defined(__ANDROID__) && defined(LE_ADAPTER)
    return CAUnsetLEClientAutoConnectionDeviceInfo(address);
#else
    (void)address;
    return CA_NOT_SUPPORTED;
#endif
}

CAResult_t CASetPortNumberToAssign(CATransportAdapter_t adapter,
                                   CATransportFlags_t flag, uint16_t port)
{
    uint16_t *targetPort = 0;

    if (CA_ADAPTER_IP & adapter)
    {
        if (CA_SECURE & flag)
        {
            if (CA_IPV6 & flag)
            {
                targetPort = &caglobals.ports.udp.u6s;
            }
            else if (CA_IPV4 & flag)
            {
                targetPort = &caglobals.ports.udp.u4s;
            }
        }
        else
        {
            if (CA_IPV6 & flag)
            {
                targetPort = &caglobals.ports.udp.u6;
            }
            else if (CA_IPV4 & flag)
            {
                targetPort = &caglobals.ports.udp.u4;
            }
        }
    }
#ifdef TCP_ADAPTER
    if (CA_ADAPTER_TCP & adapter)
    {
        if (CA_IPV6 & flag)
        {
            targetPort = &caglobals.ports.tcp.u6;
        }
        else if (CA_IPV4 & flag)
        {
            targetPort = &caglobals.ports.tcp.u4;
        }
    }
#endif

    if (targetPort)
    {
        *targetPort = port;
        return CA_STATUS_OK;
    }

    return CA_NOT_SUPPORTED;
}

uint16_t CAGetAssignedPortNumber(CATransportAdapter_t adapter, CATransportFlags_t flag)
{
    OIC_LOG(DEBUG, TAG, "CAGetAssignedPortNumber");

    if (CA_ADAPTER_IP & adapter)
    {
        if (CA_SECURE & flag)
        {
            if (CA_IPV6 & flag)
            {
                return caglobals.ip.u6s.port;
            }
            else if (CA_IPV4 & flag)
            {
                return caglobals.ip.u4s.port;
            }
        }
        else
        {
            if (CA_IPV6 & flag)
            {
                return caglobals.ip.u6.port;
            }
            else if (CA_IPV4 & flag)
            {
                return caglobals.ip.u4.port;
            }
        }
    }
#ifdef TCP_ADAPTER
    if (CA_ADAPTER_TCP & adapter)
    {
        if (CA_IPV6 & flag)
        {
            return caglobals.tcp.ipv6.port;
        }
        else if (CA_IPV4 & flag)
        {
            return caglobals.tcp.ipv4.port;
        }
    }
#endif
    return 0;
}

#ifdef __ANDROID__
/**
 * initialize client connection manager
 * @param[in]   env                   JNI interface pointer.
 * @param[in]   jvm                   invocation inferface for JAVA virtual machine.
 * @param[in]   context               application context.
 */
CAResult_t CAUtilClientInitialize(JNIEnv *env, JavaVM *jvm, jobject context)
{
    OIC_LOG(DEBUG, TAG, "CAUtilClientInitialize");

    CAResult_t res = CA_STATUS_OK;
#ifdef LE_ADAPTER
    if (CA_STATUS_OK != CAManagerLEClientInitialize(env, jvm, context))
    {
        OIC_LOG(ERROR, TAG, "CAManagerLEClientInitialize has failed");
        res = CA_STATUS_FAILED;
    }
#endif

#ifdef EDR_ADAPTER
    if (CA_STATUS_OK != CABTPairingInitialize(env, jvm, context))
    {
        OIC_LOG(ERROR, TAG, "CABTPairingInitialize has failed");
        res = CA_STATUS_FAILED;
    }
#endif

#if !defined(LE_ADAPTER) && !defined(EDR_ADAPTER)
    (void)env;
    (void)jvm;
    (void)context;
#endif
    return res;
}

/**
 * terminate client connection manager
 * @param[in]   env                   JNI interface pointer.
 */
CAResult_t CAUtilClientTerminate(JNIEnv *env)
{
    OIC_LOG(DEBUG, TAG, "CAUtilClientTerminate");
#ifdef LE_ADAPTER
    return CAManagerLEClientTerminate(env);
#else
    OIC_LOG(DEBUG, TAG, "it is not supported");
    (void)env;
    return CA_NOT_SUPPORTED;
#endif
}

// BT pairing
CAResult_t CAUtilStartScan(JNIEnv *env)
{
#ifdef EDR_ADAPTER
    return CABTPairingStartScan(env);
#else
    OIC_LOG(DEBUG, TAG, "it is not supported");
    (void)env;
    return CA_NOT_SUPPORTED;
#endif
}

CAResult_t CAUtilStopScan(JNIEnv *env)
{
#ifdef EDR_ADAPTER
    return CABTPairingStopScan(env);
#else
    OIC_LOG(DEBUG, TAG, "it is not supported");
    (void)env;
    return CA_NOT_SUPPORTED;
#endif
}

CAResult_t CAUtilCreateBond(JNIEnv *env, jobject device)
{
#ifdef EDR_ADAPTER
    return CABTPairingCreateBond(env, device);
#else
    OIC_LOG(DEBUG, TAG, "it is not supported");
    (void)env;
    (void)device;
    return CA_NOT_SUPPORTED;
#endif
}

void CAUtilSetFoundDeviceListener(jobject listener)
{
#ifdef EDR_ADAPTER
    CABTPairingSetFoundDeviceListener(listener);
#else
    (void)listener;
#endif
}

CAResult_t CAUtilSetLEScanInterval(jint intervalTime, jint workingCount)
{
    OIC_LOG(DEBUG, TAG, "CAUtilSetLEScanInterval");
#ifdef LE_ADAPTER
    CAManagerLESetScanInterval(intervalTime, workingCount);
    return CA_STATUS_OK;
#else
    (void)intervalTime;
    (void)workingCount;
    OIC_LOG(DEBUG, TAG, "it is not supported");
    return CA_NOT_SUPPORTED;
#endif
}
#endif
