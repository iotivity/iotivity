/* ****************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
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
#include <SHPlatform_Impl.h>
#include <SHPlatformConfig.h>
#include <CommonException.h>

#include <octypes.h>
#include <ocstack.h>
#include <octhread.h>
#include "logger.h"

#include <climits>
#include <unistd.h>

#define TAG "OIC_SH_COMMON_SHMGR_IMPL"

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            namespace SH_Impl
            {
                namespace
                {
                    static bool g_initialized = false;

                    static uint32_t g_startCount = 0;

                    static oc_mutex g_startMutex = NULL;

                    static bool g_processThreadRun = false;

                    static oc_thread g_processThread = NULL;

                    static oc_mutex g_csdkMutex = NULL;
                }

                SHPlatformConfig& getConfig()
                {
                    static SHPlatformConfig g_config;
                    return g_config;
                }

                void setConfig(const SHPlatformConfig &config)
                {
                    OCRegisterPersistentStorageHandler(config.ps);
                    getConfig() = config;
                }

                oc_mutex& getCsdkMutex()
                {
                    if (NULL == g_csdkMutex)
                    {
                        g_csdkMutex = oc_mutex_new();
                        if (NULL == g_csdkMutex)
                        {
                            throw CommonException("oc_mutex_new failed");
                        }
                    }

                    return g_csdkMutex;
                }

                static void destroy()
                {
                    OIC_LOG(DEBUG, TAG, "IN");

                    if (g_startMutex)
                    {
                        oc_mutex_free(g_startMutex);
                        g_startMutex = NULL;
                    }

                    if (g_csdkMutex)
                    {
                        oc_mutex_free(g_csdkMutex);
                        g_csdkMutex = NULL;
                    }

                    g_initialized = false;
                    OIC_LOG(DEBUG, TAG, "OUT");
                }

                static bool init()
                {
                    OIC_LOG(DEBUG, TAG, "IN");

                    if (NULL == g_startMutex)
                    {
                        g_startMutex = oc_mutex_new();
                        if (NULL == g_startMutex)
                        {
                            OIC_LOG(ERROR, TAG, "oc_mutex_new failed");
                            return false;
                        }
                    }

                    if (NULL == g_csdkMutex)
                    {
                        g_csdkMutex = oc_mutex_new();
                        if (NULL == g_csdkMutex)
                        {
                            OIC_LOG(ERROR, TAG, "oc_mutex_new failed");
                            destroy();
                            return false;
                        }
                    }

                    g_initialized = true;

                    OIC_LOG(DEBUG, TAG, "OUT");
                    return true;
                }

                //TODO
                //To be removed.(RI layer can handle it soon.)
                //Related JIRA ticket - IOT-1828
                static void* process(void* data)
                {
                    OIC_LOG(DEBUG, TAG, "IN");

                    while (g_processThreadRun)
                    {
                        //TODO
                        //need recursive lock.(or RI layer can handle lock.)
                        //oc_mutex_lock(g_csdkMutex);
                        OCStackResult result = OCProcess();
                        //oc_mutex_unlock(g_csdkMutex);

                        if (OC_STACK_ERROR == result)
                        {
                            //TODO
                        }

                        //To minimize CPU utilization we may with to do this with sleep
                        usleep(10 * 1000);
                    }

                    OIC_LOG(DEBUG, TAG, "OUT");
                    return NULL;
                }

                bool start()
                {
                    OIC_LOG(DEBUG, TAG, "IN");

                    if (!g_initialized)
                    {
                        bool ret = init();
                        if (!ret)
                        {
                            OIC_LOG(ERROR, TAG, "init failed");
                            return ret;
                        }
                    }

                    oc_mutex_lock(g_startMutex);

                    OIC_LOG_V(DEBUG, TAG, "Start count: %d", g_startCount);
                    if (0 != g_startCount)
                    {
                        if (UINT_MAX == g_startCount)
                        {
                            oc_mutex_unlock(g_startMutex);
                            throw CommonException("Unexpected situation!");
                            return false;
                        }

                        g_startCount++;

                        oc_mutex_unlock(g_startMutex);
                        OIC_LOG(DEBUG, TAG, "OUT");

                        return true;
                    }

                    SHPlatformConfig m_config = getConfig();

                    OCTransportFlags serverFlags = OC_DEFAULT_FLAGS;
                    OCTransportFlags clientFlags = OC_DEFAULT_FLAGS;

                    if (m_config.ipv4enabled)
                    {
                        serverFlags = static_cast<OCTransportFlags>(serverFlags | OC_IP_USE_V4);
                        clientFlags = static_cast<OCTransportFlags>(clientFlags | OC_IP_USE_V4);
                    }

                    if (m_config.ipv6enabled)
                    {
                        serverFlags = static_cast<OCTransportFlags>(serverFlags | OC_IP_USE_V6);
                        clientFlags = static_cast<OCTransportFlags>(clientFlags | OC_IP_USE_V6);
                    }

                    OCStackResult res = OCInit2(static_cast<OCMode>(m_config.mode), serverFlags,
                            clientFlags, static_cast<OCTransportAdapter>(m_config.transport));

                    if (OC_STACK_OK != res)
                    {
                        oc_mutex_unlock(g_startMutex);
                        return false;
                    }

                    if (false == g_processThreadRun)
                    {
                        g_processThreadRun = true;
                        OCThreadResult_t ret = oc_thread_new(&g_processThread, process, NULL);

                        if (OC_THREAD_SUCCESS != ret)
                        {
                            OIC_LOG(ERROR, TAG, "can't start process thread");
                            OCStop();
                            oc_mutex_unlock(g_startMutex);
                            return false;
                        }
                    }

                    g_startCount++;

                    oc_mutex_unlock(g_startMutex);
                    OIC_LOG(DEBUG, TAG, "OUT");

                    return true;
                }

                bool stop()
                {
                    OIC_LOG(DEBUG, TAG, "IN");

                    if (!g_initialized)
                    {
                        OIC_LOG(ERROR, TAG, "is not initialized!");
                        return false;
                    }

                    oc_mutex_lock(g_startMutex);

                    OIC_LOG_V(DEBUG, TAG, "count : %d", g_startCount);
                    if (1 < g_startCount)
                    {
                        g_startCount--;
                        oc_mutex_unlock(g_startMutex);
                        OIC_LOG(DEBUG, TAG, "OUT");
                        return true;
                    }

                    if (g_processThreadRun)
                    {
                        g_processThreadRun = false;
                        oc_thread_wait(g_processThread);
                        OCThreadResult_t ret = oc_thread_free(g_processThread);

                        if (OC_THREAD_SUCCESS != ret)
                        {
                            OIC_LOG(ERROR, TAG, "Stop process thread if failed");
                            oc_mutex_unlock(g_startMutex);
                            return false;
                        }
                    }

                    OCStop();

                    g_startCount--;

                    oc_mutex_unlock(g_startMutex);

                    destroy();

                    OIC_LOG(DEBUG, TAG, "OUT");
                    return true;
                }
            }
        }
    }
}
