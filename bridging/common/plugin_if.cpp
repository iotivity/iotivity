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
#include "message_handler.h"
#include "logger.h"

#define TAG "PLUGIN_IF"

#include "gw_error_code.h"
#include "plugin_if.h"
#include "plugin_server.h"

#ifdef __cplusplus
extern "C"
{
#endif

void wait_for_child_process_to_complete(pid_t child_pid, int32_t timeout);
void timed_wait_for_pipe_write(int32_t fd_to_parent, mpm_pipe_message_t *message,
                               int32_t timeout);


/*  The purpose of this function is to create a context or instance of the
 *  plugin.  There is nothing stopping one from creating more than one
 *  instance of the plugin.  Currently it is expected that only one instance
 *  is created.
 *
 *  output - a pointer to the context data of this plugin that is managed
 *           by this plugin.
 */
static common_plugin_ctx_t *create()
{
    /* always try to create a new instance */
    common_plugin_ctx_t *ctx = (common_plugin_ctx_t *) malloc(sizeof(common_plugin_ctx_t));

    if (ctx != NULL)
    {
        memset(ctx, 0, sizeof(common_plugin_ctx_t));
    }
    else
    {
        OIC_LOG(ERROR, TAG,"Unable to allocate context.");
    }
    return ctx;
}

/*  This function forks a new process and starts an OIC server in that
 *  process.  The plan is to only allow one fork per instance of
 *  the plugin.  There is a check for this, the start will fail if you
 *  call start twice on the same instance.
 *
 *  input - plugin_ctx - the protocol plugin manager hands back the plugin
 *          context created with the "create" function
 *
 *  returns - 0 for success
 *            1 for error
 */
static int start(common_plugin_ctx_t *ctx)
{
    pid_t pid;
    int result = 1;

    mpm_pipe_message_t pipe_message;

    pipe_message.payloadSize = 0;
    pipe_message.msgType = MPM_NOMSG;
    pipe_message.payload = NULL;

    if (ctx && (ctx->started == 0))
    {
        /* Create unnamed pipes for IPC prior to the fork so that both
         * parent and child have the same information on the unnamed
         * pipes.
         */
        int32_t parent_result = pipe(&(ctx->parent_reads_fds.read_fd));
        int32_t child_result = pipe(&(ctx->child_reads_fds.read_fd));

        if (parent_result == -1 || child_result == -1)
        {
            OIC_LOG(ERROR, TAG,"Failed to create IPC unnamed pipes.");
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

                /* Start the OIC server. This is a blocking call and will
                   return only when the plugin stops*/
                plugin_service(ctx);

                OIC_LOG(INFO, TAG,"Child process complete.");

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
                timed_wait_for_pipe_write(ctx->parent_reads_fds.read_fd,
                                          &pipe_message,
                                          TIMEOUT_VAL_IN_SEC);
                if (pipe_message.msgType == MPM_DONE)
                {
                    /* plugin was successful with it's create and start */
                    OIC_LOG_V(INFO, TAG,"Child: %d started successfully", ctx->child_pid);
                    result = 0;
                    ctx->started = 1;
                }
                else
                {
                    /* We have a problem, and the plugin is NOT going to load.
                     */
                    OIC_LOG_V(ERROR, TAG,"Child: %d failed, either plugin create or start.",
                          ctx->child_pid);

                    OIC_LOG(ERROR, TAG,"Forced completion of the child");

                    /* The parent must wait here until the child process is dead.
                     * but this time we are not going to wait, we are only going
                     * to force completion
                     */
                    wait_for_child_process_to_complete(ctx->child_pid, 0);

                    /* Lets close the rest of the pipe interfaces. Sides of the pipes
                     * from the parents perspective
                     */
                    close(ctx->child_reads_fds.write_fd);
                    close(ctx->parent_reads_fds.read_fd);
                }
                if(pipe_message.payload != NULL)
                {
                    free(pipe_message.payload);
                }
                break;

            case -1:
                perror("fork");
                OIC_LOG(ERROR, TAG,"Fork returned error.");
                break;
        }
    }
    else
    {
        OIC_LOG(ERROR, TAG,"Plugin instance already forked or has not been created.");
    }
    return (result);
}


static void stop(common_plugin_ctx_t *ctx)
{
    GW_RESULT result = GW_RESULT_INTERNAL_ERROR;
    /* IPC message variable */
    mpm_pipe_message_t pipe_message;

    pipe_message.payloadSize = 0;
    pipe_message.msgType = MPM_STOP;
    pipe_message.payload = NULL;

    if (ctx && (ctx->started == 1))
    {
        result = MPMWritePipeMessage(ctx->child_reads_fds.write_fd, &pipe_message);
        if (result != GW_RESULT_OK)
        {
            OIC_LOG(ERROR, TAG,"Failed to write to pipe for stop");
            return;
        }
        OIC_LOG_V(INFO, TAG,"Parent telling the child process pid: %d to stop.", ctx->child_pid);

        /* the parent must wait here until the child process is dead */
        wait_for_child_process_to_complete(ctx->child_pid, TIMEOUT_VAL_IN_SEC);

        ctx->started = 0;
    }
    else
    {
        OIC_LOG(ERROR, TAG,"Stop has no effect; Plugin has not been started");
    }
    return;
}


static void destroy(common_plugin_ctx_t *ctx)
{
    if (ctx && (ctx->started == 1))
    {
        stop(ctx);
    }

    if (ctx)
    {
        free(ctx);
    }
}


/* this function waits for a child process to signal that it is complete
 */
void wait_for_child_process_to_complete(pid_t child_pid, int32_t timeout)
{
    int32_t waittime = 0;
    int32_t status = 0;
    int32_t wpid = 0;

    do
    {
        wpid = waitpid(child_pid, &status, WNOHANG);
        if (wpid == 0)
        {
            if (waittime < timeout)
            {
                OIC_LOG_V(INFO, TAG,"Parent waiting on child: %d second(s): %d",
                      child_pid, waittime);
                sleep(1);
                waittime++;
            }
            else
            {
                OIC_LOG_V(INFO, TAG,"Parent forced stopping of child: %d", child_pid);
                kill(child_pid, SIGKILL);
                break;
            }
        }
    } while ((wpid == 0) && (waittime <= timeout));

    if (WIFEXITED(status))
    {
        OIC_LOG_V(INFO, TAG,"Child: %d completed \"exited\" with status: %d",
              child_pid, WEXITSTATUS(status));
    }
    else if (WIFSIGNALED(status))
    {
        OIC_LOG_V(INFO, TAG,"Child: %d completed \"signaled\" with status: %d",
              child_pid, WTERMSIG(status));
    }
}


/* this is a timed wait for pipe write; the written value is returned in the passed
 * in message buffer.
 */
void timed_wait_for_pipe_write(int32_t fd, mpm_pipe_message_t *msg, int32_t timeout)
{
    struct timeval tv;
    fd_set fdset;
    int32_t nfd = -1;
    int32_t waittime = 0;
    int32_t nbytes = 0;

    /* wait for 1 second on each time through the loop for up to timeout */
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    FD_ZERO(&(fdset));

    if (NULL != msg)
    {
        do
        {
            FD_ZERO(&(fdset));
            FD_SET(fd, &(fdset));
            sleep(1);  /* tried to set the timeout on select and it was not reliable */
            nfd = select(fd + 1, &(fdset), NULL, NULL, &tv);
            if (nfd == -1)
            {
                perror("select error");
                nbytes = 1;  /* break out of loop by setting this */
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
                OIC_LOG_V(INFO, TAG,"Parent waiting on child seconds(s): %d", waittime);
            }

            waittime++;

        } while ((nbytes == 0) && (waittime <= timeout));
    }
}

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

// This is the symbol table that will be loaded by the mini plugin manager.
// This is the "interface" between the mini plugin manager and the plugin.
plugin_runtime_t plugin_funcs =
        {
                create,
                start,
                stop,
                destroy
        };

#ifdef __cplusplus
}
#endif
