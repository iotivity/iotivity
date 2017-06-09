//******************************************************************
//
// Copyright 2017 Intel Mobile Communications GmbH All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//

#include <stdio.h>
#include <dlfcn.h>
#include "iotivity_config.h"
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <signal.h>
#include <pthread.h>
#include <sys/wait.h>
#include <iostream>
#include <vector>

#define TAG "MINI_PLUGIN_MANAGER"

#include "oic_malloc.h"
#include "pluginIf.h"
#include "miniPluginManager.h"
#include "messageHandler.h"
#include "experimental/logger.h"

pthread_t readResponsethreadhandle;

bool exitResponseThread = false;

/*******************************************************************************
 * type defines and structure definitions go here
 ******************************************************************************/

typedef MPMCommonPluginCtx *(*create_t)();

typedef int32_t (*start_t)(MPMCommonPluginCtx *ctx);

typedef void (*stop_t)(MPMCommonPluginCtx *ctx);

typedef void (*destroy_t)(MPMCommonPluginCtx *ctx);

typedef struct plugin_runtime_tag
{
    create_t create;
    start_t start;
    stop_t stop;
    destroy_t destroy;
} MPMPluginRunTime;

/**
 * structure representing plugin context
 */
typedef struct plugin_context_tag
{
    /** function pointers to the plugin life cycle methods */
    MPMPluginRunTime lifecycle;

    /** plugin handle */
    void *handle;

    /** common plugin context */
    MPMCommonPluginCtx *plugin_ctx;

    /** plugin shared object name */
    char shared_object_name[MAX_SHARED_OBJECT_NAME_LENGTH];

    MPMCallback callbackClient;
} MPMPluginContext;

std::vector<MPMPluginContext> g_LoadedPlugins;

/**
 * This is MPM library API to start the thread for handling response
 * messages from the plugins (child processes)
 */

void startReadResponseThread();

/**
 * This is MPM library API to terminate the thread for handling response
 * messages from the plugins (child processes)
 */
void stopReadResponseThread();

/**
 * This function runs as a thread which is running to handle
 * the response messages from the plugins(child processes)
 */
static void *readResponse(void *)
{
    int status = 0;
    fd_set readfds;
    struct timeval tv;

    std::vector<MPMPluginContext>::iterator loadedPluginsItr;

    std::vector<MPMPluginContext> *loadedPlugins = &g_LoadedPlugins;

    tv.tv_sec = 5;
    tv.tv_usec = 0;

    while (true)
    {
        if (exitResponseThread == true)
        {
            OIC_LOG(DEBUG, TAG, "Exiting readResponse thread");
            break;
        }
        int maxFd = -1;

        FD_ZERO(&(readfds));

        loadedPluginsItr = loadedPlugins->begin();

        for ( ; loadedPluginsItr != loadedPlugins->end(); )
        {
            MPMCommonPluginCtx *ctx = (*loadedPluginsItr).plugin_ctx;
            if (ctx->started)
            {
                FD_SET(ctx->parent_reads_fds.read_fd, &(readfds));
                if (maxFd < ctx->parent_reads_fds.read_fd)
                {
                    maxFd = ctx->parent_reads_fds.read_fd;
                }
            }
            loadedPluginsItr++;
        }

        if (-1 == select(maxFd + 1, &(readfds), NULL, NULL, &tv))
        {
            continue;
        }

        loadedPluginsItr = loadedPlugins->begin();

        for ( ; loadedPluginsItr != loadedPlugins->end(); )
        {
            MPMCommonPluginCtx *ctx = (*loadedPluginsItr).plugin_ctx;
            if (ctx == NULL)
            {
                loadedPluginsItr++;
                continue;
            }
            if (ctx->started)
            {
                if (FD_ISSET(ctx->parent_reads_fds.read_fd, &(readfds)))
                {
                    pid_t childStat = waitpid(ctx->child_pid, &status, WNOHANG);
                    MPMPipeMessage pipe_message;
                    ssize_t readbytes = 0;

                    pipe_message.payloadSize = 0;
                    pipe_message.msgType = MPM_NOMSG;
                    pipe_message.payload = NULL;
                    readbytes = MPMReadPipeMessage(ctx->parent_reads_fds.read_fd, &pipe_message);
                    if ((childStat != 0) || (readbytes <= 0))
                    {
                        OIC_LOG_V(DEBUG, TAG, "Plugin %s is exited",
                                  (*loadedPluginsItr).shared_object_name);
                        if (pipe_message.payloadSize > 0)
                        {
                            OICFree((void*)pipe_message.payload);
                            pipe_message.payload = NULL;
                            pipe_message.payloadSize = 0;
                        }
                        ctx->started = false;
                    }
                    else
                    {
                        (*loadedPluginsItr).callbackClient((uint32_t)pipe_message.msgType,
                                                           (MPMMessage)pipe_message.payload,
                                                           pipe_message.payloadSize,
                                                           (*loadedPluginsItr).shared_object_name);

                        pipe_message.msgType = MPM_NOMSG;
                        if (pipe_message.payloadSize > 0)
                        {
                            OICFree((void*)pipe_message.payload);
                            pipe_message.payload = NULL;
                            pipe_message.payloadSize = 0;
                        }

                    }
                }
            }
            loadedPluginsItr++;
        }

        sleep(1);
    }

    return (void *)loadedPlugins;
}

MPMResult MPMLoad(MPMPluginHandle *pluginHandle, const char *pluginName, MPMCallback callback,
                  const char *reconnect_file_name)
{
    MPMResult result = MPM_RESULT_INTERNAL_ERROR;
    MPMPluginRunTime *functionSymbolTable;

    if (pluginName == NULL)
    {
        OIC_LOG(ERROR, TAG, "PluginName is NULL");
        return result;
    }

    MPMPluginContext *plugin_instance = (MPMPluginContext *)OICCalloc(1, sizeof(MPMPluginContext));

    if (plugin_instance == NULL)
    {
        OIC_LOG(ERROR, TAG, "Unable to allocate context.");
        return MPM_RESULT_MEMORY_ERROR;
    }
    plugin_instance->plugin_ctx = NULL;

    /*
     * Now let's load the plugin and resolve the exported functions of the plugin
     * The exported functions of interest: create, start, stop, destroy
     */
    plugin_instance->handle = dlopen(pluginName, RTLD_LAZY);

    if (!plugin_instance->handle)
    {
        OIC_LOG_V(ERROR, TAG, "Error loading %s", pluginName);
        OIC_LOG_V(ERROR, TAG, "Error message %s", dlerror());
        goto CLEANUP;
    }

    strncpy(plugin_instance->shared_object_name, pluginName, MAX_SHARED_OBJECT_NAME_LENGTH-1);

    OIC_LOG(DEBUG, TAG, "Resolving function symbols");
    functionSymbolTable = (MPMPluginRunTime *) dlsym(plugin_instance->handle, "plugin_funcs");

    if (functionSymbolTable == NULL)
    {
        OIC_LOG_V(ERROR, TAG, "Error loading function symbols from %s", pluginName);
        goto CLEANUP;
    }

    plugin_instance->lifecycle.create  = functionSymbolTable->create;
    plugin_instance->lifecycle.start   = functionSymbolTable->start;
    plugin_instance->lifecycle.stop    = functionSymbolTable->stop;
    plugin_instance->lifecycle.destroy = functionSymbolTable->destroy;

    // Time to call the entry points (create and start)
    OIC_LOG_V(INFO, TAG, "Calling create on \"%s\":", pluginName);

    plugin_instance->plugin_ctx = (*(plugin_instance->lifecycle.create))();

    if (NULL == plugin_instance->plugin_ctx)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to create %s", pluginName);
        goto CLEANUP;
    }
    if (reconnect_file_name != NULL)
    {
        strncpy(plugin_instance->plugin_ctx->reconnect_file_name, reconnect_file_name,
                MPM_MAX_FILE_NAME_LENGTH - 1);
    }
    else
    {
        plugin_instance->plugin_ctx->reconnect_file_name[0] = '\0';
    }
    OIC_LOG_V(INFO, TAG, "Calling start on \"%s\":", pluginName);

    result = (MPMResult)(*(plugin_instance->lifecycle.start))(plugin_instance->plugin_ctx);
    if (MPM_RESULT_OK == result)
    {
        OIC_LOG(INFO, TAG, "Plugin start successful.");
        *pluginHandle = (MPMPluginHandle)(plugin_instance);

        MPMPluginContext p_context;
        p_context.lifecycle.create  = plugin_instance->lifecycle.create;
        p_context.lifecycle.start   = plugin_instance->lifecycle.start;
        p_context.lifecycle.stop    = plugin_instance->lifecycle.stop;
        p_context.lifecycle.destroy = plugin_instance->lifecycle.destroy;
        p_context.handle = plugin_instance->handle;

        p_context.plugin_ctx = plugin_instance->plugin_ctx;

        strncpy(p_context.shared_object_name, plugin_instance->shared_object_name,
                sizeof(p_context.shared_object_name) - 1);
        p_context.shared_object_name[sizeof(p_context.shared_object_name) - 1] = '\0';

        p_context.callbackClient = callback;

        g_LoadedPlugins.push_back(p_context);

        if (g_LoadedPlugins.size() == 1)
        {
            startReadResponseThread();
        }

        return result;
    }

    OIC_LOG_V(ERROR, TAG, "Failed to start \"%s\":", pluginName);
    OIC_LOG(ERROR, TAG, "Are the authorization files correct?");

CLEANUP:
    if (plugin_instance->plugin_ctx)
    {
        OICFree(plugin_instance->plugin_ctx);
    }
    OICFree(plugin_instance);
    return MPM_RESULT_CREATED_FAILED;
}

void startReadResponseThread()
{
    //Create a thread to handle the responses from plugin processes
    int error = pthread_create(&readResponsethreadhandle, NULL, readResponse, NULL);
    if (error != 0)
    {
        OIC_LOG(ERROR, TAG, "readResponse thread could not be started");
    }
}

void stopReadResponseThread()
{
    /* set this variable to terminate the readResponse thread */
    exitResponseThread = true;
    //terminate readResponse thread
    pthread_join(readResponsethreadhandle, NULL);
}

MPMResult MPMUnload(MPMPluginHandle pluginHandle)
{
    if (pluginHandle == NULL)
    {
        OIC_LOG(ERROR, TAG, "plugin_ctx in NULL");
        return MPM_RESULT_INTERNAL_ERROR;
    }

    MPMPluginContext *plugin_instance = (MPMPluginContext *)(pluginHandle);

    /* stop and destroy the plugin */
    OIC_LOG_V(INFO, TAG, "Calling stop on \"%s\":", plugin_instance->shared_object_name);
    (*(plugin_instance->lifecycle.stop))(plugin_instance->plugin_ctx);

    OIC_LOG_V(INFO, TAG, "Calling destroy on \"%s\":", plugin_instance->shared_object_name);
    (*(plugin_instance->lifecycle.destroy))(plugin_instance->plugin_ctx);
    dlclose(plugin_instance->handle);

    std::vector<MPMPluginContext>::iterator p_itr;
    for (p_itr = g_LoadedPlugins.begin(); p_itr != g_LoadedPlugins.end(); )
    {
        if (strncmp((*p_itr).shared_object_name, plugin_instance->shared_object_name,
                    strlen((*p_itr).shared_object_name)) == 0)
        {
            OIC_LOG_V(INFO, TAG, "plugin name %s", plugin_instance->shared_object_name);
            p_itr = g_LoadedPlugins.erase(p_itr);
            break;
        }
        else
        {
            p_itr++;
        }
    }

    OICFree(plugin_instance);

    if (g_LoadedPlugins.size() == 0)
    {
        stopReadResponseThread();
    }

    return MPM_RESULT_OK;
}

/**
 * This function performs SCAN, ADD, REMOVE and RECONNECT functionality depending on
 * MPMMessageType paramaeter.
 * @param[in] pluginHandle            Handle of the plugin to which the message
 *                                     is to be sent
 * @param[in] message                  Message to be sent over the pipe to the plugin
 * @param[in] size                     Size of the message in bytes
 * @param[in] type                     Type of the message (MPM_SCAN, MPM_ADD, etc.)
 *
 * @return MPM_RESULT_OK if success, MPM_RESULT_INTERNAL_ERROR upon failure
*/
MPMResult MPMSendPipeMessage(MPMPluginHandle pluginHandle, MPMMessage message, size_t size,
                             MPMMessageType type)
{
    MPMResult result = MPM_RESULT_INTERNAL_ERROR;
    if (pluginHandle == NULL)
    {
        OIC_LOG(ERROR, TAG, "plugin_ctx in NULL");
        return result;
    }

    MPMPluginContext *plugin_instance = (MPMPluginContext *)(pluginHandle);

    if (plugin_instance->plugin_ctx->started)
    {
        OIC_LOG_V(DEBUG, TAG, "the plugin %s is started", plugin_instance->shared_object_name);
        MPMPipeMessage pipe_message;

        pipe_message.msgType = type;
        pipe_message.payloadSize = size;
        pipe_message.payload = (uint8_t *)message;
        MPMCommonPluginCtx *ctx = (MPMCommonPluginCtx *) (plugin_instance->plugin_ctx);
        result = MPMWritePipeMessage(ctx->child_reads_fds.write_fd, &pipe_message);

        pipe_message.msgType = MPM_NOMSG;
        pipe_message.payloadSize = 0;

    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "the plugin %s is not yet started", plugin_instance->shared_object_name);
    }
    return result;
}

MPMResult MPMScan(MPMPluginHandle pluginHandle, MPMMessage message, size_t size)
{
    MPMResult result = MPM_RESULT_INTERNAL_ERROR;
    result = MPMSendPipeMessage(pluginHandle, message, size, MPM_SCAN);
    return result;
}

MPMResult MPMAddDevice(MPMPluginHandle pluginHandle, MPMMessage message, size_t size)
{
    MPMResult result = MPM_RESULT_INTERNAL_ERROR;
    result = MPMSendPipeMessage(pluginHandle, message, size, MPM_ADD);
    return result;
}

MPMResult MPMRemoveDevice(MPMPluginHandle pluginHandle, MPMMessage message, size_t size)
{
    MPMResult result = MPM_RESULT_INTERNAL_ERROR;
    result = MPMSendPipeMessage(pluginHandle, message, size, MPM_REMOVE);
    return result;
}

MPMResult MPMReconnectDevice(MPMPluginHandle pluginHandle, MPMMessage message, size_t size)
{
    MPMResult result = MPM_RESULT_INTERNAL_ERROR;
    result = MPMSendPipeMessage(pluginHandle, message, size, MPM_RECONNECT);
    return result;
}
