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

/**
 * This file is mainly intended for writing and reading the messages over the pipe,
 * This is the common code functionality where either client or MPM can call these
 * functions to write to the pipe and read from the pipe.
 */

#include <string.h>
#include <errno.h>
#include "messageHandler.h"
#include "iotivity_config.h"
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include "platform_features.h"
#include "oic_malloc.h"
#include "experimental/logger.h"

#define TAG "PIPE_HANDLER"

MPMResult MPMWritePipeMessage(int fd, const MPMPipeMessage *pipe_message)
{
    ssize_t ret = 0;
    OIC_LOG(DEBUG, TAG, "writing message over pipe");

    OIC_LOG_V(DEBUG, TAG, "Message type = %d, payload size = %" PRIuPTR, pipe_message->msgType,
              pipe_message->payloadSize);

    ret = write(fd, &pipe_message->payloadSize, sizeof(size_t));
    if (ret < 0)
    {
        OIC_LOG_V(ERROR, TAG, "Error writing message over the pipe - [%s]", strerror(errno));
        return MPM_RESULT_INTERNAL_ERROR;
    }

    ret = write(fd, &pipe_message->msgType, sizeof(MPMMessageType));
    if (ret < 0)
    {
        OIC_LOG_V(ERROR, TAG, "Error writing message over the pipe - [%s]", strerror(errno));
        return MPM_RESULT_INTERNAL_ERROR;
    }

    if (pipe_message->payloadSize > 0)
    {
        ret = write(fd, pipe_message->payload, pipe_message->payloadSize);

        if (ret < 0)
        {
            OIC_LOG_V(ERROR, TAG, "Error writing message over the pipe - [%s]", strerror(errno));
            return MPM_RESULT_INTERNAL_ERROR;
        }
    }

    return MPM_RESULT_OK;
}


ssize_t MPMReadPipeMessage(int fd, MPMPipeMessage *pipe_message)
{
    ssize_t ret = 0, bytesRead =0;
    OIC_LOG(DEBUG, TAG, "reading message from pipe");
    OIC_LOG_V(DEBUG, TAG, "Message type = %d, payload size = %" PRIuPTR , pipe_message->msgType,
                  pipe_message->payloadSize);

    ret = read(fd, &pipe_message->payloadSize, sizeof(size_t));
    if (ret < 0)
    {
        OIC_LOG_V(ERROR, TAG, "Error Reading message from the pipe - [%s]", strerror(errno));
        return ret;
    }
    bytesRead = ret;

    ret = read(fd, &pipe_message->msgType, sizeof(MPMMessageType));
    if (ret < 0)
    {
        OIC_LOG_V(ERROR, TAG, "Error Reading message from the pipe - [%s]", strerror(errno));
        return ret;
    }
    bytesRead += ret;


    if (pipe_message->msgType == MPM_NOMSG)
    {
        bytesRead = 0;
    }
    else if (pipe_message->payloadSize > 0 && pipe_message->payloadSize <= SIZE_MAX)
    {
        pipe_message->payload = (uint8_t *) OICCalloc(1, pipe_message->payloadSize);
        if (!pipe_message->payload)
        {
            OIC_LOG(ERROR, TAG, "failed to allocate memory");
            bytesRead = 0;
        }
        else
        {
            ret = read(fd, (void*)pipe_message->payload, pipe_message->payloadSize);
            if (ret < 0)
            {
                OIC_LOG_V(ERROR, TAG, "Error Reading message from the pipe - [%s]", strerror(errno));
                return ret;
            }
            bytesRead += ret;
        }
    }
    else
    {
        pipe_message->payload = NULL;
        OIC_LOG(DEBUG, TAG, "no payload received");
    }
    return bytesRead;
}
