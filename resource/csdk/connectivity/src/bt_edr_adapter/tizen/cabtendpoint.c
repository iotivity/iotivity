/******************************************************************
 *
 * Copyright 2014 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

/**
 * @file  cabtendpoint.c
 * @brief  This file provides the APIs to send data on established RFCOMM connections.
 */

#include "cabtendpoint.h"
#include "caadapterutils.h"
#include "cabtutils.h"
#include "logger.h"

CAResult_t CABTSendData(const int32_t serverFD, const void *data, const uint32_t dataLength,
                        uint32_t *sentDataLen)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    VERIFY_NON_NULL(data, BLUETOOTH_ADAPTER_TAG, "Data is null");
    VERIFY_NON_NULL(sentDataLen, BLUETOOTH_ADAPTER_TAG, "Sent data length holder is null");

    if (0 > serverFD)
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Invalid input: Negative socket id");
        return CA_STATUS_INVALID_PARAM;
    }

    int dataLen = 0;
    dataLen = bt_socket_send_data(serverFD, (const char *)data, dataLength);
    if (dataLen == -1)
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "sending data failed!, soketid [%d]", serverFD);
        *sentDataLen = 0;
        return CA_STATUS_FAILED;
    }

    *sentDataLen = dataLen;
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}
