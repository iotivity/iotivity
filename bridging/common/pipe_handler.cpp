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

/* The purpose of this file is to simulate the Iotivity protocol plugin manager
 * this file contains a plugin loader for a 32 bit plugin targeted for the X3
 * platform.  It has exactly the same interface as the Iotivity protocol plugin
 * manager.
 */

#include "message_handler.h"
#include <unistd.h>
#include <stdlib.h>
#include "logger.h"

#ifndef SIZE_MAX
/* Some systems fail to define SIZE_MAX in <stdint.h>, even though C99 requires it...
 * Conversion from signed to unsigned is defined in 6.3.1.3 (Signed and unsigned integers) p2,
 * which says: "the value is converted by repeatedly adding or subtracting one more than the
 * maximum value that can be represented in the new type until the value is in the range of the
 * new type."
 * So -1 gets converted to size_t by adding SIZE_MAX + 1, which results in SIZE_MAX.
 */
#  define SIZE_MAX ((size_t)-1)
#endif

#define TAG "PIPE_HANDLER"
/*
 *   MPMWritePipeMessage : Write messages to the pipe
 *   Input parameters    : fd - file descriptor
 *                         pipe_message - message to be written
 *
 *   Output Parametets   : GW_RESULT_OK - Success
 *                          GW_RESULT_INTERNAL_PARAMETER - FAILURE
*/

GW_RESULT MPMWritePipeMessage(int32_t fd, mpm_pipe_message_t * pipe_message)
{
     int ret = 0;
     OIC_LOG(DEBUG, TAG, "writing message over pipe");

     OIC_LOG_V(DEBUG, TAG, "Message type = %d, payload size = %zu", pipe_message->msgType, pipe_message->payloadSize);
     ret = write(fd, &pipe_message->payloadSize, sizeof(size_t));
     ret = write(fd, &pipe_message->msgType, sizeof(messageType));
     if(pipe_message->payloadSize > 0)
     {
         ret = write(fd, pipe_message->payload, pipe_message->payloadSize);
     }

     if(ret < 0)
     {
          OIC_LOG(ERROR, TAG, "Write failed!");
        return GW_RESULT_INTERNAL_ERROR;
    }

    return GW_RESULT_OK;
}

/*
 *   MPMWritePipeMessage : Write messages to the pipe
 *   Input parameters    : fd - file descriptor
 *                         pipe_message - for storing the read message.
 *
 *   Output Parametets   : no of bytes read
*/

int MPMReadPipeMessage(int32_t fd, mpm_pipe_message_t * pipe_message)
{
    int ret = 0;
    OIC_LOG(DEBUG, TAG, "reading message from pipe");

    ret = read(fd, &pipe_message->payloadSize, sizeof(size_t));
    ret += read(fd, &pipe_message->msgType, sizeof(messageType));
    OIC_LOG_V(DEBUG, TAG, "Message type = %d, payload size = %zu", pipe_message->msgType, pipe_message->payloadSize);

    if(pipe_message->msgType == MPM_NOMSG)
    {
        ret = 0;
    }
    else if(pipe_message->payloadSize > 0 && pipe_message->payloadSize < SIZE_MAX)
    {
        pipe_message->payload = (uint8_t *) malloc(pipe_message->payloadSize);
        if(!pipe_message->payload)
        {
            OIC_LOG(ERROR, TAG, "alloc failed");
            ret = 0;
        }
        else
        {
            ret += read(fd, pipe_message->payload, pipe_message->payloadSize);
        }
    }
    else
    {
        pipe_message->payload = NULL;
        OIC_LOG(DEBUG, TAG, "no payload received");
    }
    return ret;
}

