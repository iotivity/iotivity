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

/* This file contains the "C" plugin interface definition.  This header file
 * and its corresponding implementation file is intended to be shared among
 * ALL plugins.  Modification of this file is not necessary for the construction
 * of a new plugin.
 */

#ifndef _PLUGINIF_H_
#define _PLUGINIF_H_

#include <stdint.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include "messageHandler.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This is a clear way to store file descriptors for unnamed pipes.  Each pipe
 * has a read end and a write end.
 */
struct MPMPipe
{
    int read_fd;
    int write_fd;
};

/**
 * structure to represent the plugin context
 *  which is common between all the plugins.
 */
struct MPMCommonPluginCtx
{
    /**
     * Unnamed pipes are used in this implementation of the common plugin code,
     * however the unnamed pipes are NOT exposed outside of the common plugin code.
     * One of the unnamed pipes is used by the child to tell parent that the
     * child has created and started the plugin content section successfully or not.
     * The other unnamed pipe is used by the parent to stop the child which in turn
     * stops and destroys the plugin content. As you know the unnamed pipes may be
     * only used with related processes (i.e. having the same parent). The pipes exist
     * as long as their descriptors are open.
     */
    MPMPipe parent_reads_fds;
    MPMPipe child_reads_fds;

    /**
     * The "started" variable is used by the parent process to not permit
     * more than one fork to happen per instance of the plugin.
     */
    bool started;

    /**
     * The main thread in the child process needs to know when it should
     * leave the OCF process loop.
     */
    bool exit_process_loop;
    char reconnect_file_name[MPM_MAX_FILE_NAME_LENGTH];

    /**
     * Save the child's process handle to wait on it being signaled later
     */
    pid_t child_pid;

};

/** time out value */
#define MPM_TIMEOUT_VAL_IN_SEC  60

/**
 * This function is a OCF server.  The function does not return unless there is an
 * error or this main thread was signaled by the parent process main thread.
 * @param[in] plugin_ctx            plugin specific context
 *
 * @return MPM_RESULT_OK if no errors, MPM_RESULT_INTERNAL_ERROR if error
 */
MPMResult MPMPluginService(MPMCommonPluginCtx *plugin_ctx);

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif /* __PLUGIN_IF_H__ */
