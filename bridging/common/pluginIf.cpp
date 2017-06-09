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

/* This file contains the "C" plugin interface implementation */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <spawn.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "messageHandler.h"
#include "experimental/logger.h"
#include "oic_malloc.h"
#include "mpmErrorCode.h"
#include "pluginIf.h"
#include "pluginServer.h"

#define TAG "PLUGIN_IF"

/* this function waits for a child process to signal that it is complete
 * @param[in] child_pid child process id
 * @param[in] timeout time to wait for child to complete
 */
static void waitForChildProcessToComplete(pid_t child_pid, int32_t timeout);


/* This is a timed wait for pipe write; the written value is returned in the passed
 * in message buffer.
 * @param[in]  fd_to_parent       File descriptor
 * @param[out] message            Message from the child
 * @param[in]  timeout            Time to wait for pipe write in seconds
 */
static void timedWaitForPipeWrite(int fd_to_parent, MPMPipeMessage *message,
                                  int32_t timeout);


/**
 * The purpose of this function is to create a context or instance of the
 * plugin.  There is nothing stopping one from creating more than one
 * instance of the plugin.  Currently it is expected that only one instance
 * is created.
 *
 * @return  A pointer to the context data of this plugin that is managed
 *          by this plugin.
 */
static MPMCommonPluginCtx *create()
{
    /* always try to create a new instance */
    MPMCommonPluginCtx *ctx = (MPMCommonPluginCtx *) OICCalloc(1, sizeof(MPMCommonPluginCtx));

    if (ctx == NULL)
    {
        OIC_LOG(ERROR, TAG, "Unable to allocate context.");
    }
    return ctx;
}

/**
 * This function forks a new process and starts an OCF server in that
 * process.  The plan is to only allow one fork per instance of
 * the plugin.  There is a check for this, the start will fail if you
 * call start twice on the same instance.
 *
 * @param[in] ctx        the plugin context created with the "create" function
 *
 * @return 0 on success, 1 on error
 */
static int start(MPMCommonPluginCtx *ctx)
{
    int result = 1;

    if (ctx && !ctx->started)
    {
        pid_t pid;
        MPMPipeMessage pipe_message;

        pipe_message.payloadSize = 0;
        pipe_message.msgType = MPM_NOMSG;
        pipe_message.payload = NULL;

        /* Create unnamed pipes for IPC prior to the fork so that both
         * parent and child have the same information on the unnamed
         * pipes.
         */
        int parent_result = pipe(&(ctx->parent_reads_fds.read_fd));
        if (parent_result == -1)
        {
            OIC_LOG(ERROR, TAG, "Failed to create IPC unnamed pipe for parent.");
            return result;
        }
        int child_result = pipe(&(ctx->child_reads_fds.read_fd));

        if (child_result == -1)
        {
            OIC_LOG(ERROR, TAG, "Failed to create IPC unnamed pipe for child.");
            close(ctx->parent_reads_fds.read_fd);
            close(ctx->parent_reads_fds.write_fd);
            return result;
        }

        switch (pid = fork())
        {
            case 0:
                /* Child(plugin) process.
                 * Close unused edges of unnamed pipes from the
                 * child's perspective. Child is not going
                 * write to the child's read pipe nor is the child
                 * going to read from the parent's read pipe
                 */
                close(ctx->child_reads_fds.write_fd);
                close(ctx->parent_reads_fds.read_fd);

                /* Start the OCF server. This is a blocking call and will
                   return only when the plugin stops*/
                MPMPluginService(ctx);

                OIC_LOG(INFO, TAG, "Child process complete.");

                /* Close the other sides of the pipes from
                 * the child's perspective
                 */
                close(ctx->child_reads_fds.read_fd);
                close(ctx->parent_reads_fds.write_fd);

                exit(0);
                break;

            default:

                /* Parent process */
                ctx->child_pid = pid;

                /* The parent is not going to read
                 * from the child's read pipe and the parent is not
                 * going to write the parents read pipe.
                 */
                close(ctx->child_reads_fds.read_fd);
                close(ctx->parent_reads_fds.write_fd);

                /* The plugin may fail to create or start.
                 * The parent must wait here for some time to
                 * learn what happened.
                 */
                timedWaitForPipeWrite(ctx->parent_reads_fds.read_fd,
                                      &pipe_message,
                                      MPM_TIMEOUT_VAL_IN_SEC);
                if (pipe_message.msgType == MPM_DONE)
                {
                    /* plugin was successful with its create and start */
                    OIC_LOG_V(INFO, TAG, "Child: %d started successfully", ctx->child_pid);
                    result = 0;
                    ctx->started = true;
                }
                else
                {
                    /* We have a problem, and the plugin is NOT going to load.
                     */
                    OIC_LOG_V(ERROR, TAG, "Child: %d failed, either plugin create or start.",
                              ctx->child_pid);

                    OIC_LOG(ERROR, TAG, "Forced completion of the child");

                    /* The parent must wait here until the child process is dead.
                     * but this time we are not going to wait, we are only going
                     * to force completion
                     */
                    waitForChildProcessToComplete(ctx->child_pid, 0);

                    /* Let's close the rest of the pipe interfaces. Sides of the pipes
                     * from the parent's perspective
                     */
                    close(ctx->child_reads_fds.write_fd);
                    close(ctx->parent_reads_fds.read_fd);
                }

                OICFree((void*)pipe_message.payload);

                break;

            case -1:
                perror("fork");
                OIC_LOG(ERROR, TAG, "Fork returned error.");
                break;
        }
    }
    else
    {
        OIC_LOG(ERROR, TAG, "Plugin instance already forked or has not been created.");
    }
    return (result);
}

/**
 * This function writes a stop message over the pipe to indicate the child
 * to stop
 *
 * @param[in] ctx        the plugin context created with the "create" function
 */
static void stop(MPMCommonPluginCtx *ctx)
{
    MPMResult result = MPM_RESULT_INTERNAL_ERROR;

    if (ctx && ctx->started)
    {
        /* IPC message variable */
        MPMPipeMessage pipe_message;

        pipe_message.payloadSize = 0;
        pipe_message.msgType = MPM_STOP;
        pipe_message.payload = NULL;

        result = MPMWritePipeMessage(ctx->child_reads_fds.write_fd, &pipe_message);
        if (result != MPM_RESULT_OK)
        {
            OIC_LOG(ERROR, TAG, "Failed to write to pipe for stop");
            return;
        }
        OIC_LOG_V(INFO, TAG, "Parent telling the child process pid: %d to stop.", ctx->child_pid);

        /* the parent must wait here until the child process is dead */
        waitForChildProcessToComplete(ctx->child_pid, MPM_TIMEOUT_VAL_IN_SEC);

        ctx->started = false;
    }
    else
    {
        OIC_LOG(INFO, TAG, "Stop has no effect; Plugin has not been started");
    }
    return;
}

/**
 * This function calls the "stop" function follwed by destruction of the
 * context created with the "create" function
 *
 * @param[in] ctx        the plugin context created with the "create" function
 */
static void destroy(MPMCommonPluginCtx *ctx)
{
    if (ctx && ctx->started)
    {
        stop(ctx);
    }
    OICFree(ctx);
}

static void waitForChildProcessToComplete(pid_t child_pid, int32_t timeout)
{
    int32_t waittime = 0;
    int status = 0;
    pid_t wpid = 0;

    do
    {
        wpid = waitpid(child_pid, &status, WNOHANG);
        if (wpid == 0)
        {
            if (waittime < timeout)
            {
                OIC_LOG_V(INFO, TAG, "Parent waiting on child: %d second(s): %d",
                          child_pid, waittime);
                sleep(1);
                waittime++;
            }
            else
            {
                OIC_LOG_V(INFO, TAG, "Parent forced stopping of child: %d", child_pid);
                kill(child_pid, SIGKILL);
                break;
            }
        }
    }
    while ((wpid == 0) && (waittime <= timeout));

    if (WIFEXITED(status))
    {
        OIC_LOG_V(INFO, TAG, "Child: %d completed \"exited\" with status: %d",
                  child_pid, WEXITSTATUS(status));
    }
    else if (WIFSIGNALED(status))
    {
        OIC_LOG_V(INFO, TAG, "Child: %d completed \"signaled\" with status: %d",
                  child_pid, WTERMSIG(status));
    }
}

static void timedWaitForPipeWrite(int fd, MPMPipeMessage *msg, int32_t timeout)
{
    if (NULL != msg)
    {
        struct timeval tv;
        fd_set fdset;
        int nfd = -1;
        int32_t waittime = 0;
        ssize_t nbytes = 0;

        /* wait for 1 second on each time through the loop for up to timeout */
        tv.tv_sec = 0;
        tv.tv_usec = 0;

        do
        {
            FD_ZERO(&(fdset));
            FD_SET(fd, &(fdset));
            sleep(1);  /* tried to set the timeout on select and it was not reliable */
            nfd = select(fd + 1, &(fdset), NULL, NULL, &tv);
            if (nfd == -1)
            {
                OIC_LOG_V(ERROR, TAG, "select error :[%s]", strerror(errno));
                break;
            }
            else if (nfd)
            {
                if (FD_ISSET(fd, &(fdset)))
                {
                    nbytes = MPMReadPipeMessage(fd, msg);
                }
            }
            else
            {
                /* got nothing case */
                OIC_LOG_V(INFO, TAG, "Parent waiting on child seconds(s): %d", waittime);
            }

            waittime++;

        }
        while ((nbytes == 0) && (waittime <= timeout));
    }
}

typedef MPMCommonPluginCtx *(*create_t)();
typedef int (*start_t)(MPMCommonPluginCtx *ctx);
typedef void (*stop_t)(MPMCommonPluginCtx *ctx);
typedef void (*destroy_t)(MPMCommonPluginCtx *ctx);

typedef struct plugin_runtime_tag
{
    create_t create;
    start_t start;
    stop_t stop;
    destroy_t destroy;
} MPMPluginRuntime;

// This is the symbol table that will be loaded by the mini plugin manager.
// This is the "interface" between the mini plugin manager and the plugin.
MPMPluginRuntime plugin_funcs =
{
    create,
    start,
    stop,
    destroy
};

