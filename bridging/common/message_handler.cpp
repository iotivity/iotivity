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

#include <unistd.h>
#include <stdlib.h>
#include "plugin_if.h"
#include "plugin_server.h"
#include "cbor.h"
#include "logger.h"

#define TAG "MESSAGE_HANDLER"

#define RESOURCE_TYPE           "rt"
#define HREF                    "href"
#define INTERFACE               "if"
#define BITMAP                  "Bm"
#define NAME                    "NAME"
#define MANUFACTURER            "MF"
#define DEVICETYPE              "DEVICE_TYPE"
#define PLUGINSPECIFICDETAILS   "PluginSpecificDetails"
#define RESOURCES               "RESOURCES"

#define VERIFY_CBOR_SUCCESS(log_tag, err, log_message) \
    if ((CborNoError != (err)) && (CborErrorOutOfMemory != (err))) \
    { \
        if ((log_tag) && (log_message)) \
        { \
              printf("cbor error - %s \n",log_message); \
        } \
    } \

common_plugin_ctx_t *g_com_ctx;

/*******************************************************************************
 * Request Handler : Handles the requests from mpm to plugin
 * IN Params :
 * OUT Params :
 *
 ******************************************************************************/
void request_handler(mpm_pipe_message_t *pipe_message, plugin_ctx_t *ctx )
{
    OIC_LOG(DEBUG, TAG,"Inside request_handler");

    switch(pipe_message->msgType)
    {
        case MPM_SCAN:
            OIC_LOG(DEBUG, TAG,"plugin_scan called");
            plugin_scan(ctx, pipe_message);
            break;
        case MPM_ADD:
            OIC_LOG(DEBUG, TAG,"plugin_add called");
            plugin_add(ctx, pipe_message);
            break;
        case MPM_DELETE:
            break;
        case MPM_REMOVE:
            OIC_LOG(DEBUG, TAG,"plugin_remove called");
            plugin_remove(ctx, pipe_message);
            break;
        case MPM_RECONNECT:
            OIC_LOG(DEBUG, TAG,"Plugin reconnect called");
            plugin_reconnect(ctx,pipe_message);
            break;
        default:
            break;
    }
}

GW_RESULT send_response(void *response, size_t size, messageType type)
{
    GW_RESULT result = GW_RESULT_INTERNAL_ERROR;

    OIC_LOG_V(DEBUG, TAG,"Inside response_handler and size of payload = %d", (int)size);

    mpm_pipe_message_t pipe_message;

    pipe_message.payloadSize = size;
    pipe_message.msgType = type;
    pipe_message.payload = (uint8_t *)response;

    result = MPMWritePipeMessage(g_com_ctx->parent_reads_fds.write_fd, &pipe_message);

    pipe_message.msgType = MPM_NOMSG;
    pipe_message.payloadSize = 0;

    return result;
}

static int64_t AddTextStringToMap(CborEncoder* map, const char* key, size_t keylen,
        const char* value)
{
    int64_t err = cbor_encode_text_string(map, key, keylen);
    if (CborNoError != err)
    {
        return err;
    }
    return cbor_encode_text_string(map, value, strlen(value));
}

static int64_t AddstructureToMap(CborEncoder* map, const char* key, size_t keylen,
        const char* value, size_t valueSize)
{
    int64_t err = cbor_encode_text_string(map, key, keylen);
    if (CborNoError != err)
    {
        return err;
    }
    return cbor_encode_text_string(map, value, valueSize);
}

/* This function encodes the metadata received from
 * the plugin
 * Inputs:
 *        list- a list of resources supported by the device
 *        buff- The metadata stream to be filled with encoded data
 *        size- size of the metadata stream to be encoded
 *        details- to hold plugin specific device details to be encoded
 *        payloadSize- size of the plugin specific device details
 * Return value:
 *        err- error code,
 *                       zero, if no error
 *                       non-zero, if some error has occured
 */
int64_t formMetaData(resourceList *list, device_specific_data_t *deviceDetails, uint8_t *buff, size_t size, void *details, size_t payloadSize)
{

    CborEncoder encoder;
    int64_t err = CborNoError;
    CborEncoder rootArray,rootMap,linkMap;
    CborEncoder linkArray;
    resourceList *temp = NULL;

    cbor_encoder_init(&encoder, buff, size, 0);

    err = cbor_encoder_create_array(&encoder, &rootArray, 1);
    VERIFY_CBOR_SUCCESS(TAG, err, " Creating Root Array");

    err = cbor_encoder_create_map(&rootArray, &rootMap, CborIndefiniteLength);
    VERIFY_CBOR_SUCCESS(TAG, err, "Creating Root MAP");

    if(deviceDetails)
    {
        err = AddTextStringToMap(&rootMap, NAME, sizeof(NAME) - 1,
                   deviceDetails->devName);
        VERIFY_CBOR_SUCCESS(TAG, err, "Adding device name");

        err = AddTextStringToMap(&rootMap, MANUFACTURER, sizeof(MANUFACTURER) - 1,
                   deviceDetails->manufacturerName);
        VERIFY_CBOR_SUCCESS(TAG, err, "Adding Manufacture name");

        err = AddTextStringToMap(&rootMap, DEVICETYPE, sizeof(DEVICETYPE) - 1,
               deviceDetails->devType);
        VERIFY_CBOR_SUCCESS(TAG, err, "Adding Device Type");
    }

    if(details)
    {
        err = AddstructureToMap(&rootMap, PLUGINSPECIFICDETAILS, sizeof(PLUGINSPECIFICDETAILS) - 1,
                   (const char*)details, payloadSize);
        VERIFY_CBOR_SUCCESS(TAG, err, "Adding Plugin specific Details");
    }

    err = cbor_encode_text_string(&rootMap, RESOURCES, sizeof(RESOURCES) - 1);
    VERIFY_CBOR_SUCCESS(TAG, err, "Encoding Resources string");

    err = cbor_encoder_create_array(&rootMap, &linkArray,CborIndefiniteLength);
    VERIFY_CBOR_SUCCESS(TAG, err, "Creating Link Array");

    for( ; list ; )
    {
        temp = list;
        OIC_LOG_V(DEBUG, TAG," href - %s\n rt -  %s\n if - %s\n bm - %d\n",list->href,list->rt,list->interfaces,list->bitmap);
        // resource map inside the links array.
        err = cbor_encoder_create_map(&linkArray, &linkMap, 4);
        VERIFY_CBOR_SUCCESS(TAG, err, "Creating Link Map");

        err = AddTextStringToMap(&linkMap, RESOURCE_TYPE, sizeof(RESOURCE_TYPE) - 1,
                   list->rt);
        VERIFY_CBOR_SUCCESS(TAG, err, "Adding Resource type");

        err = AddTextStringToMap(&linkMap, HREF, sizeof(HREF) - 1,
                   list->href);
        VERIFY_CBOR_SUCCESS(TAG, err, "Adding HREF");

        err = AddTextStringToMap(&linkMap, INTERFACE, sizeof(INTERFACE) - 1,
                   list->interfaces);
        VERIFY_CBOR_SUCCESS(TAG, err, "Adding Resource Interface");

        err = cbor_encode_text_string(&linkMap, BITMAP, sizeof(BITMAP) - 1);
        VERIFY_CBOR_SUCCESS(TAG, err, "Encoding Bitmap string");

        err = cbor_encode_int(&linkMap,list->bitmap);
        VERIFY_CBOR_SUCCESS(TAG, err, "encoding bit map");

        // close link map inside link array
        err = cbor_encoder_close_container(&linkArray, &linkMap);
        VERIFY_CBOR_SUCCESS(TAG, err, "Closing link map");

        list = list -> next;
        free(temp);
    }

    // Close links array inside the root map.
    err = cbor_encoder_close_container(&rootMap, &linkArray);
    VERIFY_CBOR_SUCCESS(TAG, err, "Closing link array");

    // close root map inside the root array.
    err = cbor_encoder_close_container(&rootArray, &rootMap);
    VERIFY_CBOR_SUCCESS(TAG, err, "Closing Root Map");

    // Close the final root array.
    err = cbor_encoder_close_container(&encoder, &rootArray);
    VERIFY_CBOR_SUCCESS(TAG, err, "Closing root Array");

    return err;
}

/* This function decodes and parse the metadata received from
 * the client as a part of reconnect request
 * Inputs:
 *        buff- The encoded metadata stream
 *        size- size of the encoded metadata stream
 *        list- reference to location of resource details
 *        details- to hold plugin specific device details after parsing
 * Return value:
 *        none
 */
void ParseMetaData(uint8_t *buff, size_t size, resourceList **list, void **details)
{
    int64_t err = CborNoError;
    CborValue rootMapValue,linkMapValue;
    CborValue resourceMapValue;
    CborValue curVal;
    CborValue rootValue;
    CborParser parser;
    int bitmap ;

    err = cbor_parser_init(buff, size, 0, &parser, &rootValue);
    VERIFY_CBOR_SUCCESS(TAG,err,"Parser cbor init");

    if (cbor_value_is_array(&rootValue))
    {
        OIC_LOG_V(DEBUG, TAG,"ENCODED DATA - %s ",(char *)buff);
        err = cbor_value_enter_container(&rootValue, &rootMapValue);
        if (!cbor_value_is_map(&rootMapValue))
        {
            OIC_LOG(ERROR, TAG,"ERROR, Malformed packet");
            return ;
        }

        if (cbor_value_is_map(&rootMapValue))
        {
            // Parsing device details
            err = cbor_value_map_find_value(&rootMapValue, NAME, &curVal);
            if (cbor_value_is_valid(&curVal))
            {
                if(cbor_value_is_text_string(&curVal))
                {
                    size_t len = 0;
                    char *input = NULL;
                    err = cbor_value_dup_text_string(&curVal, &input, &len, NULL);
                    OIC_LOG_V(DEBUG, TAG,"\"NAME\":%s\n", input);
                    free(input);
                }
            }
        }

        err = cbor_value_map_find_value(&rootMapValue, MANUFACTURER, &curVal);
        if (cbor_value_is_valid(&curVal))
        {
            if(cbor_value_is_text_string(&curVal))
            {
                size_t len = 0;
                char *input = NULL;
                err = cbor_value_dup_text_string(&curVal, &input, &len, NULL);
                OIC_LOG_V(DEBUG, TAG,"\"MF\":%s\n", input);
                free(input);
            }
        }

        err = cbor_value_map_find_value(&rootMapValue, PLUGINSPECIFICDETAILS, &curVal);
        if (cbor_value_is_valid(&curVal))
        {
            if(cbor_value_is_text_string(&curVal))
            {
                size_t len = 0;
                char *input = NULL;
                err = cbor_value_dup_text_string(&curVal, &input, &len, NULL);
                *details = (void *)input;
            }
        }

        err = cbor_value_map_find_value(&rootMapValue, RESOURCES, &linkMapValue);
        // Enter the links array and start iterating through the array processing
        // each resource which shows up as a map.
        if (cbor_value_is_valid(&linkMapValue))
        {

            err = cbor_value_enter_container(&linkMapValue, &resourceMapValue);
            while (cbor_value_is_map(&resourceMapValue))
            {
                resourceList *tempPtr;
                tempPtr = (resourceList *) calloc(1, sizeof(resourceList));
                if(tempPtr == NULL)
                {
                    OIC_LOG(ERROR, TAG, "calloc failed");
                    return;
                }
                size_t len = 0;
                char *input = NULL;
                err = cbor_value_map_find_value(&resourceMapValue, HREF, &curVal);
                err = cbor_value_dup_text_string(&curVal, &input, &len, NULL);
                strncpy(tempPtr->href, input, MAX_LENGTH_64);
                OIC_LOG_V(DEBUG, TAG,"\"ref\":%s\n",input);
                free(input);
                input = NULL;

                err = cbor_value_map_find_value(&resourceMapValue, RESOURCE_TYPE, &curVal);
                err = cbor_value_dup_text_string(&curVal, &input, &len, NULL);
                strncpy(tempPtr->rt, input , MAX_LENGTH_64);
                OIC_LOG_V(DEBUG, TAG,"\"rt\":%s\n",input);
                free(input);
                input = NULL;

                err = cbor_value_map_find_value(&resourceMapValue, INTERFACE, &curVal);
                err = cbor_value_dup_text_string(&curVal, &input, &len, NULL);
                strncpy(tempPtr->interfaces, input , MAX_LENGTH_64);
                OIC_LOG_V(DEBUG, TAG,"\"if\":%s\n",input);
                free(input);
                input = NULL;

                err = cbor_value_map_find_value(&resourceMapValue, BITMAP, &curVal);
                if(cbor_value_is_integer(&curVal))
                {
                    err = cbor_value_get_int(&curVal,&bitmap);
                    tempPtr->bitmap = bitmap;
                    OIC_LOG_V(DEBUG, TAG,"\"bm\":%d\n",bitmap);
                }

                tempPtr->next = *list;
                *list  = tempPtr;
                err = cbor_value_advance(&resourceMapValue);
                VERIFY_CBOR_SUCCESS(TAG ,err,"in resource map value advance");
            }
        }
    }
}
