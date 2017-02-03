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

#ifndef __PLUGIN_IF_H__
#define __PLUGIN_IF_H__

#include <stdint.h>
#include <stdio.h>
#include <pthread.h>
#include "gw_error_code.h"
#include <stdbool.h>
#include "message_handler.h"

/* This is a clear way to store file descriptors for unnamed pipes.  Each pipe
 * has a read end and a write end.
 */
typedef struct pipe_tag
{
    int32_t read_fd;
    /* descriptor used for reading */
    int32_t write_fd;   /* descriptor used for writing */
} pipe_t;

typedef struct common_plugin_ctx_tag common_plugin_ctx_t;

struct common_plugin_ctx_tag
{
    /* Unnamed pipes are used in this implementation of the common plugin code,
     * however the unnamed pipes are NOT exposed outside of the common plugin code.
     * One of the unnamed pipes is used by the child to tell parent that the
     * child has created and started the plugin content section successfully or not.
     * The other unnamed pipe is used by the parent to stop the child which in turn
     * stops and destroys the plugin content. As you know the unnamed pipes may be
     * only used with related processes (i.e. having the same parent). The pipes exist
     * as long as their descriptors are open.
     */
    pipe_t parent_reads_fds;
    pipe_t child_reads_fds;

    /* The "started" variable is used by the parent process to not permit
     * more than one fork to happen per instance of the plugin.
     */
    int32_t started;

    /* The main thread in the child process needs to know when it should
     * leave the OIC process loop.
     */
    bool exit_process_loop;
    char reconnect_file_name[MAX_FILE_NAME_LENGTH];

    /* Save the child's process handle to wait on it being signaled later */
    pid_t child_pid;

};



/* Pipe Messages that are used to martial APIs inside the common portion of
 * of the plugin.  The messages are defined here.
 */
#define PLUGIN_START_COMPLETE       "DONE"
#define PLUGIN_START_COMPLETE_ERROR "ERRR"
#define PLUGIN_STOP_REQUEST         "STOP"
#define PLUGIN_MESSAGE_SIZE          5


#define TIMEOUT_VAL_IN_SEC  60

GW_RESULT plugin_service(common_plugin_ctx_t *plugin_ctx);

#endif /* __PLUGIN_IF_H__ */
