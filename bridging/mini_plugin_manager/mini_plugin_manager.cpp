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

#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <signal.h>
#include <pthread.h>
#include <sys/wait.h>
#include <iostream>
#include <vector>

#define TAG "MINI_PLUGIN_MANAGER"

#include "plugin_if.h"
#include "mini_plugin_manager.h"
#include "message_handler.h"
#include "logger.h"

pthread_t readResponsethreadhandle;

bool exit_response_thread = false;

/*******************************************************************************
 * type defines and structure definitions go here
 ******************************************************************************/

typedef common_plugin_ctx_t *(*create_t)();

typedef int32_t (*start_t)(common_plugin_ctx_t *ctx);

typedef void (*stop_t)(common_plugin_ctx_t *ctx);

typedef void (*destroy_t)(common_plugin_ctx_t *ctx);

typedef struct plugin_runtime_tag
{
    create_t create;
    start_t start;
    stop_t stop;
    destroy_t destroy;
} plugin_runtime_t;

typedef struct plugin_context_tag
{
    /* function pointers for each shared object loaded */
    plugin_runtime_t lifecycle;
    void *handle;
    common_plugin_ctx_t *plugin_ctx;
    char shared_object_name[MAX_SHARED_OBJECT_NAME_LENGTH];
    MPMCallback callbackClient;
} plugin_context_t;

std::vector<plugin_context_t> g_loaded_plugin_list;

/* This is MPM library API to start the thread for handling response
 * messages from the plugins (child processes)
 */

void startReadResponseThread();

/* This is MPM library API to terminate the thread for handling response
 * messages from the plugins (child processes)
 */
void stopReadResponseThread();

/* readResponse :  Is a thread function which is running to handle
 *                 the response messages from the plugins(child processes)
 * Parameters   :
 *                 plugin context : Available plugins in the list.
 * returns      :  VOID
 *
 */
void *readResponse(void *unused)
{
    (void)unused;
    int status = 0;
    fd_set readfds;
    struct timeval tv;

    FD_ZERO(&(readfds));

    std::vector<plugin_context_t>::iterator p_loaded_plugin_list_itr;

    std::vector<plugin_context_t> *p_loaded_plugin_list = &g_loaded_plugin_list;

    tv.tv_sec = 5;
    tv.tv_usec = 0;

    while(true)
    {
        if(exit_response_thread == true)
        {
            OIC_LOG(DEBUG, TAG, "Exiting readResponse thread");
            break;
        }
        int maxFd = -1;

        FD_ZERO(&(readfds));

        p_loaded_plugin_list_itr = p_loaded_plugin_list->begin();

        for ( ;p_loaded_plugin_list_itr != p_loaded_plugin_list->end(); )
        {
            common_plugin_ctx_t *ctx = (*p_loaded_plugin_list_itr).plugin_ctx;
            if (ctx->started )
            {
                FD_SET(ctx->parent_reads_fds.read_fd, &(readfds));
                if(maxFd < ctx->parent_reads_fds.read_fd)
                {
                    maxFd = ctx->parent_reads_fds.read_fd;
                }
            }
            p_loaded_plugin_list_itr++;
        }

        if(-1 == select(maxFd+1, &(readfds), NULL, NULL, &tv))
        {
            continue;
        }

        p_loaded_plugin_list_itr = p_loaded_plugin_list->begin();

        for ( ; p_loaded_plugin_list_itr != p_loaded_plugin_list->end(); )
        {
            common_plugin_ctx_t *ctx = (*p_loaded_plugin_list_itr).plugin_ctx;
            if(ctx == NULL)
            {
                p_loaded_plugin_list_itr++;
                continue;
            }
            if (ctx->started)
            {
                if(FD_ISSET(ctx->parent_reads_fds.read_fd, &(readfds)))
                {
                    int childStat = waitpid(ctx->child_pid, &status, WNOHANG);
                    mpm_pipe_message_t pipe_message;
                    int readbytes = 0;

                    pipe_message.payloadSize = 0;
                    pipe_message.msgType = MPM_NOMSG;
                    pipe_message.payload = NULL;
                    readbytes = MPMReadPipeMessage(ctx->parent_reads_fds.read_fd, &pipe_message);
                    if ((childStat != 0) || (readbytes <= 0))
                    {
                        OIC_LOG_V(DEBUG, TAG, "Plugin %s is exited", (*p_loaded_plugin_list_itr).shared_object_name);
                        if(pipe_message.payloadSize > 0)
                        {
                            free(pipe_message.payload);
                            pipe_message.payload = NULL;
                            pipe_message.payloadSize = 0;
                        }
                        ctx->started = false;
                    }
                    else
                    {
                        (*p_loaded_plugin_list_itr).callbackClient((uint32_t)pipe_message.msgType, (MPMMessage)pipe_message.payload,
                                pipe_message.payloadSize, (*p_loaded_plugin_list_itr).shared_object_name);

                        pipe_message.msgType = MPM_NOMSG;
                        if(pipe_message.payloadSize > 0)
                        {
                            free(pipe_message.payload);
                            pipe_message.payload = NULL;
                            pipe_message.payloadSize = 0;
                        }

                    }
                }
            }
            p_loaded_plugin_list_itr++;
        }

        sleep(1);
    }

    pthread_exit(NULL);
    return (void *)p_loaded_plugin_list;
}


/* MPMLoad : API to Load Plugins
 * Input Parameters : plugin handle
 *                    plugin name
 *                    callback
 *                    file name for reconnect
 * Output parameters: GW_RESULT_OK - Success
 *                     GW_RESULT_INTERNAL_ERROR - Failure
*/
GW_RESULT MPMLoad(MPMPluginHandle *pluginHandle, const char *pluginName, MPMCallback callback,const char *reconnect_file_name)
{
    GW_RESULT result = GW_RESULT_INTERNAL_ERROR;
    plugin_runtime_t *functionSymbolTable;

    if ((pluginName == NULL))
    {
        OIC_LOG(ERROR, TAG, "PluginName is NULL");
        return result;
    }

    plugin_context_t *plugin_instance = (plugin_context_t *)calloc(1, sizeof(plugin_context_t));

    if (plugin_instance == NULL)
    {
        OIC_LOG(ERROR, TAG, "Unable to allocate context.");
        return GW_RESULT_NOT_CREATED;
    }
    plugin_instance->plugin_ctx = NULL;

    /*
     * Now lets load the plugin and resolve the exported functions of the plugin
     * The exported functions of interest: create, start, stop, destroy
     */
    plugin_instance->handle = dlopen(pluginName, RTLD_LAZY);

    if (!plugin_instance->handle)
    {
        OIC_LOG_V(ERROR, TAG, "Error loading %s", pluginName);
        OIC_LOG_V(ERROR, TAG, "Error message %s", dlerror());
        goto CLEANUP;
    }

    strncpy(plugin_instance->shared_object_name, pluginName, MAX_SHARED_OBJECT_NAME_LENGTH - 1);

    OIC_LOG(DEBUG, TAG, "Resolving function symbols");
    functionSymbolTable = (plugin_runtime_t *) dlsym(plugin_instance->handle, "plugin_funcs");

    if (functionSymbolTable == NULL)
    {
        OIC_LOG_V(ERROR, TAG, "Error loading function symbols from %s", pluginName);
        goto CLEANUP;
    }

    plugin_instance->lifecycle.create  = functionSymbolTable->create;
    plugin_instance->lifecycle.start   = functionSymbolTable->start;
    plugin_instance->lifecycle.stop    = functionSymbolTable->stop;
    plugin_instance->lifecycle.destroy = functionSymbolTable->destroy;

    /* Time to call the entry points (create and start) */
    OIC_LOG_V(INFO, TAG,"Calling create on \"%s\":", pluginName);

    plugin_instance->plugin_ctx = (*(plugin_instance->lifecycle.create))();

    if (NULL == plugin_instance->plugin_ctx)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to create %s", pluginName);
        goto CLEANUP;
    }
    if(reconnect_file_name != NULL)
    {
        strncpy(plugin_instance->plugin_ctx->reconnect_file_name, reconnect_file_name, MAX_FILE_NAME_LENGTH-1);
    }
    else
    {
        memset(plugin_instance->plugin_ctx->reconnect_file_name, 0, MAX_FILE_NAME_LENGTH);
    }
    OIC_LOG_V(INFO, TAG, "Calling start on \"%s\":", pluginName);

    result = (*(plugin_instance->lifecycle.start))(plugin_instance->plugin_ctx);
    if (GW_RESULT_OK == result)
    {
        OIC_LOG(INFO, TAG, "Plugin start successful.");
        *pluginHandle = (MPMPluginHandle)(plugin_instance);

        plugin_context_t p_context;
        p_context.lifecycle.create  = plugin_instance->lifecycle.create;
        p_context.lifecycle.start   = plugin_instance->lifecycle.start;
        p_context.lifecycle.stop    = plugin_instance->lifecycle.stop;
        p_context.lifecycle.destroy = plugin_instance->lifecycle.destroy;
        p_context.handle = plugin_instance->handle;

        p_context.plugin_ctx = plugin_instance->plugin_ctx;

        strncpy(p_context.shared_object_name, plugin_instance->shared_object_name, MAX_SHARED_OBJECT_NAME_LENGTH - 1);
        p_context.callbackClient = callback;

        g_loaded_plugin_list.push_back(p_context);

        if(g_loaded_plugin_list.size() == 1)
        {
            startReadResponseThread();
        }

        return result;
    }

    OIC_LOG_V(ERROR, TAG, "Failed to start \"%s\":", pluginName);
    OIC_LOG(ERROR, TAG,"Are the authorization files correct?");

    CLEANUP:
    if(plugin_instance->plugin_ctx)
    {
        free(plugin_instance->plugin_ctx);
    }
    free(plugin_instance);
    return GW_RESULT_CREATED_FAILED;
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
    exit_response_thread = true;
    //terminate readResopnse thread
    pthread_join(readResponsethreadhandle, NULL);
}

/* MPMUnload : API to Unlooad Plugins
 * Input Parameters : plugin handle
 * Output parameters: GW_RESULT_OK - Success
 *                    GW_RESULT_INTERNAL_ERROR - Failure
*/

GW_RESULT MPMUnload(MPMPluginHandle pluginHandle)
{
    if(pluginHandle == NULL)
    {
        OIC_LOG(ERROR, TAG, "plugin_ctx in NULL");
        return GW_RESULT_INTERNAL_ERROR;
    }

    plugin_context_t *plugin_instance = (plugin_context_t *)(pluginHandle);

    /* stop and destroy the plugin */
    OIC_LOG_V(INFO, TAG, "Calling stop on \"%s\":", plugin_instance->shared_object_name);
    (*(plugin_instance->lifecycle.stop))(plugin_instance->plugin_ctx);

    OIC_LOG_V(INFO, TAG, "Calling destroy on \"%s\":", plugin_instance->shared_object_name);
    (*(plugin_instance->lifecycle.destroy))(plugin_instance->plugin_ctx);
    dlclose(plugin_instance->handle);

    std::vector<plugin_context_t>::iterator p_itr;
    for(p_itr = g_loaded_plugin_list.begin(); p_itr != g_loaded_plugin_list.end(); )
    {
        if (strncmp((*p_itr).shared_object_name, plugin_instance->shared_object_name, strlen((*p_itr).shared_object_name)) == 0)
        {
            OIC_LOG_V(INFO, TAG, "plugin name %s", plugin_instance->shared_object_name);
            p_itr = g_loaded_plugin_list.erase(p_itr);
            break;
        }
        else
        {
            p_itr++;
        }
    }

    free(plugin_instance);

    if(g_loaded_plugin_list.size() == 0)
    {
        stopReadResponseThread();
    }

    return GW_RESULT_OK;
}

/* MPMSendPipeMessage - Perform SCAN, ADD, REMOVE and RECONNECT functionality dependin on messageType paramaeter.
 * Input Parameters : plugin handle
 *                    Message
 * Output parameters: GW_RESULT_OK - Success
 *                    GW_RESULT_INTERNAL_ERROR - Failure
*/

GW_RESULT MPMSendPipeMessage(MPMPluginHandle pluginHandle, MPMMessage message, size_t size, messageType type)
{
    GW_RESULT result = GW_RESULT_INTERNAL_ERROR;
    if(pluginHandle == NULL)
    {
        OIC_LOG(ERROR, TAG, "plugin_ctx in NULL");
        return result;
    }

    plugin_context_t *plugin_instance = (plugin_context_t *)(pluginHandle);

    if (plugin_instance->plugin_ctx->started)
    {
        OIC_LOG_V(DEBUG, TAG, "the plugin %s is started", plugin_instance->shared_object_name);
        mpm_pipe_message_t pipe_message;

        pipe_message.msgType = type;
        pipe_message.payloadSize = size;
        pipe_message.payload = (uint8_t *)message;
        common_plugin_ctx_t *ctx = (common_plugin_ctx_t *) (plugin_instance->plugin_ctx);
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


/* API to Initiate Scan */
GW_RESULT MPMScan(MPMPluginHandle pluginHandle, MPMMessage message, size_t size)
{
    GW_RESULT result = GW_RESULT_INTERNAL_ERROR;
    result = MPMSendPipeMessage(pluginHandle, message, size, MPM_SCAN);
    return result;
}

/* API to Add device */
GW_RESULT MPMAddDevice(MPMPluginHandle pluginHandle, MPMMessage message, size_t size)
{
    GW_RESULT result = GW_RESULT_INTERNAL_ERROR;
    result = MPMSendPipeMessage(pluginHandle, message, size, MPM_ADD);
    return result;
}

/* API to Remove device */
GW_RESULT MPMRemoveDevice(MPMPluginHandle pluginHandle, MPMMessage message, size_t size)
{
    GW_RESULT result = GW_RESULT_INTERNAL_ERROR;
    result = MPMSendPipeMessage(pluginHandle, message, size, MPM_REMOVE);
    return result;
}

/* API to Reconnect devices */
GW_RESULT MPMReconnectDevice(MPMPluginHandle pluginHandle, MPMMessage message, size_t size)
{
    GW_RESULT result = GW_RESULT_INTERNAL_ERROR;
    result = MPMSendPipeMessage(pluginHandle, message, size, MPM_RECONNECT);
    return result;
}
