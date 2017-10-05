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


#include "iotivity_config.h"
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include "oic_malloc.h"
#include "pluginIf.h"
#include "pluginServer.h"
#include "cbor.h"
#include "experimental/logger.h"
#include "StringConstants.h"

#define TAG "MESSAGE_HANDLER"

#define NAME                    "NAME"
#define MANUFACTURER            "MF"
#define DEVICETYPE              "DEVICE_TYPE"
#define PLUGINSPECIFICDETAILS   "PluginSpecificDetails"
#define RESOURCES               "RESOURCES"

#define VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(log_tag, err, log_message) \
    if ((CborNoError != (err)) && (CborErrorOutOfMemory != (err))) \
    { \
        if ((log_tag) && (log_message)) \
        { \
            OIC_LOG_V(ERROR, TAG, "cbor error - %s \n", log_message); \
        } \
    } \

MPMCommonPluginCtx *g_com_ctx;

void MPMRequestHandler(MPMPipeMessage *pipe_message, MPMPluginCtx *ctx)
{
    OIC_LOG(DEBUG, TAG, "Inside request_handler");

    switch (pipe_message->msgType)
    {
        case MPM_SCAN:
            OIC_LOG(DEBUG, TAG, "plugin_scan called");
            pluginScan(ctx, pipe_message);
            break;
        case MPM_ADD:
            OIC_LOG(DEBUG, TAG, "plugin_add called");
            pluginAdd(ctx, pipe_message);
            break;
        case MPM_REMOVE:
            OIC_LOG(DEBUG, TAG, "plugin_remove called");
            pluginRemove(ctx, pipe_message);
            break;
        case MPM_RECONNECT:
            OIC_LOG(DEBUG, TAG, "Plugin reconnect called");
            pluginReconnect(ctx, pipe_message);
            break;
        default:OIC_LOG(DEBUG, TAG, "Currently not supported");
            break;
    }
}

MPMResult MPMSendResponse(const void *response, size_t size, MPMMessageType type)
{
    MPMResult result = MPM_RESULT_INTERNAL_ERROR;

    OIC_LOG_V(DEBUG, TAG, "Inside response_handler and size of payload = %d", (int)size);

    MPMPipeMessage pipe_message;

    pipe_message.payloadSize = size;
    pipe_message.msgType = type;
    pipe_message.payload = (uint8_t *)response;

    result = MPMWritePipeMessage(g_com_ctx->parent_reads_fds.write_fd, &pipe_message);

    return result;
}

static int64_t AddTextStringToMap(CborEncoder *map, const char *key, size_t keylen,
                                  const char *value)
{
    int64_t err = cbor_encode_text_string(map, key, keylen);
    if (CborNoError != err)
    {
        return err;
    }
    return cbor_encode_text_string(map, value, strlen(value));
}

static int64_t AddstructureToMap(CborEncoder *map, const char *key, size_t keylen,
                                 const char *value, size_t valueSize)
{
    int64_t err = cbor_encode_text_string(map, key, keylen);
    if (CborNoError != err)
    {
        return err;
    }
    return cbor_encode_text_string(map, value, valueSize);
}

int64_t MPMFormMetaData(MPMResourceList *list, MPMDeviceSpecificData *deviceDetails,
                        uint8_t *buff, size_t size, void *details, size_t payloadSize)
{

    CborEncoder encoder;
    int64_t err = CborNoError;
    CborEncoder rootArray, rootMap, linkMap;
    CborEncoder linkArray;
    MPMResourceList *temp = NULL;

    cbor_encoder_init(&encoder, buff, size, 0);

    err = cbor_encoder_create_array(&encoder, &rootArray, 1);
    VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, err, " Creating Root Array");

    err = cbor_encoder_create_map(&rootArray, &rootMap, CborIndefiniteLength);
    VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, err, "Creating Root MAP");

    if (deviceDetails)
    {
        err = AddTextStringToMap(&rootMap, NAME, sizeof(NAME) - 1,
                                 deviceDetails->devName);
        VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, err, "Adding device name");

        err = AddTextStringToMap(&rootMap, MANUFACTURER, sizeof(MANUFACTURER) - 1,
                                 deviceDetails->manufacturerName);
        VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, err, "Adding Manufacture name");

        err = AddTextStringToMap(&rootMap, DEVICETYPE, sizeof(DEVICETYPE) - 1,
                                 deviceDetails->devType);
        VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, err, "Adding Device Type");
    }

    if (details)
    {
        err = AddstructureToMap(&rootMap, PLUGINSPECIFICDETAILS, sizeof(PLUGINSPECIFICDETAILS) - 1,
                                (const char *)details, payloadSize);
        VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, err, "Adding Plugin specific Details");
    }

    err = cbor_encode_text_string(&rootMap, RESOURCES, sizeof(RESOURCES) - 1);
    VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, err, "Encoding Resources string");

    err = cbor_encoder_create_array(&rootMap, &linkArray, CborIndefiniteLength);
    VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, err, "Creating Link Array");

    for ( ; list ; )
    {
        temp = list;
        OIC_LOG_V(DEBUG, TAG, " href - %s\n rt -  %s\n if - %s\n bm - %d\n", list->href, list->rt,
                  list->interfaces, list->bitmap);
        // resource map inside the links array.
        err = cbor_encoder_create_map(&linkArray, &linkMap, 4);
        VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, err, "Creating Link Map");

        err = AddTextStringToMap(&linkMap, OC::Key::RESOURCETYPESKEY.c_str(),
                                 OC::Key::RESOURCETYPESKEY.size(),
                                 list->rt);
        VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, err, "Adding Resource type");

        err = AddTextStringToMap(&linkMap, OC::Key::URIKEY.c_str(), OC::Key::URIKEY.size(),
                                 list->href);
        VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, err, "Adding OC::Key::URIKEY");

        err = AddTextStringToMap(&linkMap, OC::Key::INTERFACESKEY.c_str(),
                                 OC::Key::INTERFACESKEY.size(), list->interfaces);
        VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, err, "Adding Resource Interface");

        err = cbor_encode_text_string(&linkMap, OC::Key::BMKEY.c_str(), OC::Key::BMKEY.size());
        VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, err, "Encoding Bitmap string");

        err = cbor_encode_int(&linkMap, list->bitmap);
        VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, err, "encoding bit map");

        // close link map inside link array
        err = cbor_encoder_close_container(&linkArray, &linkMap);
        VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, err, "Closing link map");

        list = list -> next;
        OICFree(temp);
    }

    // Close links array inside the root map.
    err = cbor_encoder_close_container(&rootMap, &linkArray);
    VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, err, "Closing link array");

    // close root map inside the root array.
    err = cbor_encoder_close_container(&rootArray, &rootMap);
    VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, err, "Closing Root Map");

    // Close the final root array.
    err = cbor_encoder_close_container(&encoder, &rootArray);
    VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, err, "Closing root Array");

    return err;
}

void MPMParseMetaData(const uint8_t *buff, size_t size, MPMResourceList **list, void **details)
{
    int64_t err = CborNoError;
    CborValue rootMapValue, linkMapValue;
    CborValue resourceMapValue;
    CborValue curVal;
    CborValue rootValue;
    CborParser parser;
    int bitmap;

    err = cbor_parser_init(buff, size, 0, &parser, &rootValue);
    VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, err, "Parser cbor init");

    if (cbor_value_is_array(&rootValue))
    {
        OIC_LOG_V(DEBUG, TAG, "ENCODED DATA - %s ", (char *)buff);
        err = cbor_value_enter_container(&rootValue, &rootMapValue);
        VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, err, "Entering root array");
        if (!cbor_value_is_map(&rootMapValue))
        {
            OIC_LOG(ERROR, TAG, "ERROR, Malformed packet");
            return ;
        }

        if (cbor_value_is_map(&rootMapValue))
        {
            // Parsing device details
            err = cbor_value_map_find_value(&rootMapValue, NAME, &curVal);
            VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, err, "finding Name in map");
            if (cbor_value_is_valid(&curVal))
            {
                if (cbor_value_is_text_string(&curVal))
                {
                    size_t len = 0;
                    char *input = NULL;
                    err = cbor_value_dup_text_string(&curVal, &input, &len, NULL);
                    VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, err, "Duplicating name string");
                    OIC_LOG_V(DEBUG, TAG, "\"NAME\":%s\n", input);
                    free(input);
                }
            }
        }

        err = cbor_value_map_find_value(&rootMapValue, MANUFACTURER, &curVal);
        VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, err, "Finding Manufacturer details in map");
        if (cbor_value_is_valid(&curVal))
        {
            if (cbor_value_is_text_string(&curVal))
            {
                size_t len = 0;
                char *input = NULL;
                err = cbor_value_dup_text_string(&curVal, &input, &len, NULL);
                VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, err, " Copying Text string");
                OIC_LOG_V(DEBUG, TAG, "\"MF\":%s\n", input);
                free(input);
            }
        }

        err = cbor_value_map_find_value(&rootMapValue, PLUGINSPECIFICDETAILS, &curVal);
        VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, err, " Finding PLUGINSPECIFICDETAILS in map ");
        if (cbor_value_is_valid(&curVal))
        {
            if (cbor_value_is_text_string(&curVal))
            {
                size_t len = 0;
                char *input = NULL;
                err = cbor_value_dup_text_string(&curVal, &input, &len, NULL);
                VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, err, " Copying Text string");
                *details = (void *)input;
            }
        }

        err = cbor_value_map_find_value(&rootMapValue, RESOURCES, &linkMapValue);
        VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, err, " Finding RESOURCES in map ");
        // Enter the links array and start iterating through the array processing
        // each resource which shows up as a map.
        if (cbor_value_is_valid(&linkMapValue))
        {

            err = cbor_value_enter_container(&linkMapValue, &resourceMapValue);
            VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, err, " Entering Link map ");
            while (cbor_value_is_map(&resourceMapValue))
            {
                MPMResourceList *tempPtr;
                tempPtr = (MPMResourceList *) OICCalloc(1, sizeof(MPMResourceList));
                if (tempPtr == NULL)
                {
                    OIC_LOG(ERROR, TAG, "calloc failed");
                    return;
                }
                size_t len = 0;
                char *input = NULL;
                err = cbor_value_map_find_value(&resourceMapValue, OC::Key::URIKEY.c_str(), &curVal);
                VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, err, " Finding Uri in map ");

                err = cbor_value_dup_text_string(&curVal, &input, &len, NULL);
                VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, err, " Copying Text string");
                strncpy(tempPtr->href, input, MPM_MAX_LENGTH_64);
                OIC_LOG_V(DEBUG, TAG, "\"ref\":%s\n", input);
                free(input);
                input = NULL;

                err = cbor_value_map_find_value(&resourceMapValue, OC::Key::RESOURCETYPESKEY.c_str(), &curVal);
                VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, err, " Finding Rt in link map ");
                err = cbor_value_dup_text_string(&curVal, &input, &len, NULL);
                VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, err, " Copying Text string");
                strncpy(tempPtr->rt, input, MPM_MAX_LENGTH_64);
                OIC_LOG_V(DEBUG, TAG, "\"rt\":%s\n", input);
                free(input);
                input = NULL;

                err = cbor_value_map_find_value(&resourceMapValue, OC::Key::INTERFACESKEY.c_str(), &curVal);
                VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, err, " Finding If's in link map ");
                err = cbor_value_dup_text_string(&curVal, &input, &len, NULL);
                VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, err, " Copying Text string");
                strncpy(tempPtr->interfaces, input, MPM_MAX_LENGTH_64);
                OIC_LOG_V(DEBUG, TAG, "\"if\":%s\n", input);
                free(input);
                input = NULL;

                err = cbor_value_map_find_value(&resourceMapValue, OC::Key::BMKEY.c_str(), &curVal);
                VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, err, " Finding Bms in link map ");
                if (cbor_value_is_integer(&curVal))
                {
                    err = cbor_value_get_int(&curVal, &bitmap);
                    VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, err, " Getting bit map value fromx link map ");
                    tempPtr->bitmap = bitmap;
                    OIC_LOG_V(DEBUG, TAG, "\"bm\":%d\n", bitmap);
                }

                tempPtr->next = *list;
                *list  = tempPtr;
                err = cbor_value_advance(&resourceMapValue);
                VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, err, "in resource map value advance");
            }
        }
    }
}
