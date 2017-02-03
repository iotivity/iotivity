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

/* This file contains the plugin server implementation.  Most of this
 * implementation is reused for other plugins.  There is NO customization
 * required of functions in this file to accommodate plugin specific code.
 */

#ifndef __MESSAGE_HANDLER_H
#define __MESSAGE_HANDLER_H

#include <stdint.h>
#include <stdio.h>
#include "gw_error_code.h"

#define MAX_CALLBACK_PARAM_LENGTH 64
#define MAX_URI_LEN       256
#define MAX_FILE_NAME_LENGTH           300
#define MAX_LENGTH_32     32
#define MAX_LENGTH_64     64
#define MAX_LENGTH_256    256
#define MAX_UNIQUE_ID_LEN 128
#define MAX_METADATA_LEN  3000

/* Enum to specify the action type*/
typedef enum {
        MPM_NOMSG = 0,
        MPM_SCAN,
        MPM_ADD,
        MPM_DELETE,
        MPM_REMOVE,
        MPM_RECONNECT,
        MPM_STOP,
        MPM_DONE,
        MPM_ERROR
} messageType;

/* This structure represents the response message coming from
 * the plugins to mpm library
 */
typedef struct mpm_pipe_message_tag
{
    size_t payloadSize;
    messageType msgType;
    uint8_t * payload;
}mpm_pipe_message_t;

typedef struct mpm_add_tag
{
    char uri[MAX_URI_LEN];
    char metadata[MAX_METADATA_LEN];
}add_response;

typedef struct resource_list_tag
{
   char href[MAX_URI_LEN];
   char relative[MAX_LENGTH_64];
   char interfaces[MAX_LENGTH_64];
   char rt[MAX_LENGTH_64];
   int bitmap;
   struct resource_list_tag *next;
}resourceList;

typedef struct {
        char devName[MAX_LENGTH_64];
        char devType[MAX_LENGTH_64];
        char manufacturerName[MAX_LENGTH_256];
}device_specific_data_t;

GW_RESULT MPMWritePipeMessage(int32_t fd, mpm_pipe_message_t * pipe_message);
int MPMReadPipeMessage(int32_t fd, mpm_pipe_message_t * pipe_message);
int64_t formMetaData(resourceList *list, device_specific_data_t *deviceDetails, uint8_t *buf, size_t size, void *details, size_t payloadSize);
void ParseMetaData(uint8_t *buffer, size_t size, resourceList **list, void **details);

#ifdef __cplusplus
extern "C"{
GW_RESULT send_response(void *response, size_t size, messageType type);
}
#else
GW_RESULT send_response(void *response, size_t size, messageType type);
#endif

#endif
