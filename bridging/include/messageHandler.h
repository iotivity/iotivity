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

#ifndef _MESSAGEHANDLER_H
#define _MESSAGEHANDLER_H

#include <stdint.h>
#include <stdio.h>
#include "mpmErrorCode.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MPM_MAX_URI_LEN       256
#define MPM_MAX_FILE_NAME_LENGTH           300
#define MPM_MAX_LENGTH_32     32
#define MPM_MAX_LENGTH_64     64
#define MPM_MAX_LENGTH_256    256
#define MPM_MAX_LENGTH_1024   1024
#define MPM_MAX_UNIQUE_ID_LEN 128
#define MPM_MAX_METADATA_LEN  3000

/* Enum to specify the action type*/
typedef enum
{
    MPM_NOMSG = 0,
    MPM_SCAN,
    MPM_ADD,
    MPM_DELETE,
    MPM_REMOVE,
    MPM_RECONNECT,
    MPM_STOP,
    MPM_DONE,
    MPM_ERROR
} MPMMessageType;

/**
 * This structure represents the format of the message exchanged
 * between MPM and Plugin over the pipe.
 */
typedef struct
{
    /** size of the payload. */
    size_t payloadSize;

    /** type of the message [MPM_SCAN, MPM_ADD etc] */
    MPMMessageType msgType;

    /** Payload to be sent and received */
    const uint8_t *payload;
} MPMPipeMessage;

/**
 * This structure represents the add response message coming from
 * the plugins to mpm library
 */
typedef struct
{
    char uri[MPM_MAX_URI_LEN];
    char metadata[MPM_MAX_METADATA_LEN];
} MPMAddResponse;

/**
 * This structure represents the resource list used for
 * creating reconnect metadata
 */
typedef struct MPMResourceList
{
    char href[MPM_MAX_URI_LEN];
    char relative[MPM_MAX_LENGTH_64];
    char interfaces[MPM_MAX_LENGTH_64];
    char rt[MPM_MAX_LENGTH_64];
    int bitmap;
    struct MPMResourceList *next;
} MPMResourceList;

/**
 * This structure represents the device specific data of the plugin
 * which is a part of reconnect metadata
 */
typedef struct
{
    char devName[MPM_MAX_LENGTH_64];
    char devType[MPM_MAX_LENGTH_64];
    char manufacturerName[MPM_MAX_LENGTH_256];
} MPMDeviceSpecificData;

/**
 * This function writes messages to the pipe
 * @param[in] fd            file descriptor
 * @param[in] pipe_message  message to be written
 *
 * @return MPM_RESULT_OK on success, MPM_RESULT_INTERNAL_PARAMETER on failure
*/
MPMResult MPMWritePipeMessage(int fd, const MPMPipeMessage *pipe_message);

/**
 * This function reads messages from the pipe
 * @param[in] fd                file descriptor
 * @param[in,out] pipe_message  for storing the read message.
 *
 * @return number of bytes read
*/
ssize_t MPMReadPipeMessage(int fd, MPMPipeMessage *pipe_message);


/**
 * This function encodes the metadata received from the plugin
 * @param[in] list            A list of resources supported by the device
 * @param[in] deviceDetails   Plugin specific details to be encoded
 * @param[in] buff            The metadata stream to be filled with encoded data
 * @param[in] size            Size of the metadata stream to be encoded
 * @param[in] details         To hold plugin specific device details to be encoded
 * @param[in] payloadSize     Size of the plugin specific device details
 *
 * @return zero on no error, non-zero on occurrence of some error
 */
int64_t MPMFormMetaData(MPMResourceList *list, MPMDeviceSpecificData *deviceDetails, uint8_t *buff,
                        size_t size, void *details, size_t payloadSize);

/**
 * This function decodes and parse the metadata received from
 * the client as a part of reconnect request
 * @param[in] buffer           The encoded metadata stream
 * @param[in] size             Size of the encoded metadata stream
 * @param[out] list            Reference to location of resource details
 * @param[out] details         To hold plugin specific device details after parsing
 */
void MPMParseMetaData(const uint8_t *buffer, size_t size, MPMResourceList **list, void **details);

/**
 * This function sends response coming from the plugins to mpm library
 * @param[in] response  Response to be sent
 * @param[in] size      Size of the response.
 * @param[in] type      Type of response (scan response, add response etc)
 * @return MPM_RESULT_OK on success else MPM_RESULT_INTERNAL_ERROR
 */
MPMResult MPMSendResponse(const void *response, size_t size, MPMMessageType type);

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif
