/* ****************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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

// Defining _BSD_SOURCE or _DEFAULT_SOURCE causes header files to expose
// definitions that may otherwise be skipped. Skipping can cause implicit
// declaration warnings and/or bugs and subtle problems in code execution.
// For glibc information on feature test macros,
// Refer http://www.gnu.org/software/libc/manual/html_node/Feature-Test-Macros.html
//
// This file requires #define use due to random()
// For details on compatibility and glibc support,
// Refer http://www.gnu.org/software/libc/manual/html_node/BSD-Random.html
#define _DEFAULT_SOURCE
#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "caadapterutils.h"
#include "cainterface.h"
#include "camessagehandler.h"
#include "caremotehandler.h"
#include "cablockwisetransfer.h"
#include "oic_malloc.h"
#include "camutex.h"
#include "logger.h"

#define TAG "CA_BWT"

#define BLOCKWISE_OPTION_BUFFER    (sizeof(unsigned int))
#define BLOCK_NUMBER_IDX           4
#define BLOCK_M_BIT_IDX            3
#define PORT_LENGTH                2

#define BLOCK_SIZE(arg) (1 << ((arg) + 4))

// context for block-wise transfer
static CABlockWiseContext_t g_context = { 0 };

CAResult_t CAInitializeBlockWiseTransfer(CASendThreadFunc sendThreadFunc,
                                         CAReceiveThreadFunc receivedThreadFunc)
{
    OIC_LOG(DEBUG, TAG, "initialize");

    // set block-wise transfer context
    if (!g_context.sendThreadFunc)
    {
        g_context.sendThreadFunc = sendThreadFunc;
    }

    if (!g_context.receivedThreadFunc)
    {
        g_context.receivedThreadFunc = receivedThreadFunc;
    }

    if (!g_context.dataList)
    {
        g_context.dataList = u_arraylist_create();
    }

    CAResult_t res = CAInitBlockWiseMutexVariables();
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "init has failed");
    }

    return res;
}

CAResult_t CATerminateBlockWiseTransfer()
{
    OIC_LOG(DEBUG, TAG, "terminate");

    if (g_context.dataList)
    {
        u_arraylist_free(&g_context.dataList);
    }

    CATerminateBlockWiseMutexVariables();

    return CA_STATUS_OK;
}

CAResult_t CAInitBlockWiseMutexVariables()
{
    OIC_LOG(DEBUG, TAG, "IN");

    if (NULL == g_context.blockDataListMutex)
    {
        g_context.blockDataListMutex = ca_mutex_new();
        if (NULL == g_context.blockDataListMutex)
        {
            OIC_LOG(ERROR, TAG, "ca_mutex_new has failed");
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == g_context.blockDataSenderMutex)
    {
        g_context.blockDataSenderMutex = ca_mutex_new();
        if (NULL == g_context.blockDataSenderMutex)
        {
            OIC_LOG(ERROR, TAG, "ca_mutex_new has failed");
            CATerminateBlockWiseMutexVariables();
            return CA_STATUS_FAILED;
        }
    }

    return CA_STATUS_OK;
}

void CATerminateBlockWiseMutexVariables()
{
    OIC_LOG(DEBUG, TAG, "IN");

    if (g_context.blockDataListMutex)
    {
        ca_mutex_free(g_context.blockDataListMutex);
        g_context.blockDataListMutex = NULL;
    }

    if (g_context.blockDataSenderMutex)
    {
        ca_mutex_free(g_context.blockDataSenderMutex);
        g_context.blockDataSenderMutex = NULL;
    }
}

CAResult_t CASendBlockWiseData(const CAMessageInfo_t *messageInfo)
{
    // check if message type is CA_MSG_RESET
    if (CA_RESPONSE_DATA != messageInfo->type)
    {
        if (CA_MSG_RESET == messageInfo->info.type)
        {
            OIC_LOG(DEBUG, TAG, "reset message can't be sent to the block");
            return CA_NOT_SUPPORTED;
        }
    }

    // #1. check if it is already included in block data list
    CABlockData_t *currData = NULL;
    CAResult_t res = CACheckBlockDataValidation(messageInfo, &currData);
    if (CA_STATUS_OK != res)
    {
        // #2. if it is not included, add the data into list
        if (NULL == currData)
        {
            OIC_LOG(DEBUG, TAG, "There is no block data");
            currData = CACreateNewBlockData(messageInfo);
            if (NULL == currData)
            {
                OIC_LOG(ERROR, TAG, "failed to create block data");
                return CA_MEMORY_ALLOC_FAILED;
            }
        }
    }

    // #3. check request/response block option type and payload length
    res = CACheckBlockOptionType(currData);
    if (CA_STATUS_OK == res)
    {
        // #4. send block message
        OIC_LOG(DEBUG, TAG, "send first block msg");
        res = CAAddSendThreadQueue(currData->sentData, (const CABlockDataID_t *)&currData->blockDataId);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "add has failed");
            return res;
        }
    }

    return res;
}

CAResult_t CAAddSendThreadQueue(const CAMessageInfo_t *sendData, const CABlockDataID_t *blockID)
{
    VERIFY_NON_NULL(sendData, TAG, "sendData");
    VERIFY_NON_NULL(blockID, TAG, "blockID");

    CAMessageInfo_t *cloneData = CACloneMessageInfo(sendData);
    if (NULL == cloneData)
    {
        OIC_LOG(ERROR, TAG, "clone has failed");
        CARemoveBlockDataFromList(blockID);
        return CA_STATUS_FAILED;
    }

    if (g_context.sendThreadFunc)
    {
        ca_mutex_lock(g_context.blockDataSenderMutex);
        g_context.sendThreadFunc(cloneData);
        ca_mutex_unlock(g_context.blockDataSenderMutex);
    }
    else
    {
        CADestroyDataSet(cloneData);
    }
    return CA_STATUS_OK;
}

CAResult_t CACheckBlockOptionType(CABlockData_t *currData)
{
    VERIFY_NON_NULL(currData, TAG, "currData");
    VERIFY_NON_NULL(currData->sentData, TAG, "currData->sentData");

    size_t payloadLen = 0;
    CAGetPayloadInfo(currData->sentData, &payloadLen);

    // check if message has to be transfered to a block
    size_t maxBlockSize = BLOCK_SIZE(CA_DEFAULT_BLOCK_SIZE);
    if (payloadLen <= maxBlockSize)
    {
        OIC_LOG_V(DEBUG, TAG, "payloadLen=%d, maxBlockSize=%d", payloadLen, maxBlockSize);
        return CA_NOT_SUPPORTED;
    }

    // set block option (COAP_OPTION_BLOCK2 or COAP_OPTION_BLOCK1)
    if (CA_REQUEST_DATA == currData->sentData->type)
    {
        currData->type = COAP_OPTION_BLOCK1;
    }
    else if (CA_RESPONSE_DATA == currData->sentData->type)
    {
        currData->type = COAP_OPTION_BLOCK2;
    }
    else
    {
        return CA_NOT_SUPPORTED;
    }

    return CA_STATUS_OK;
}

// TODO make pdu const after libcoap is updated to support that.
CAResult_t CAReceiveBlockWiseData(coap_pdu_t *pdu, CAMessageInfo_t *receivedData, size_t dataLen)
{
    CAEndpoint_t *endpoint = &(receivedData->endpoint);
    // check if received message type is CA_MSG_RESET
    if (CA_EMPTY == pdu->hdr->code)
    {
        OIC_LOG(DEBUG, TAG, "code is CA_EMPTY..");

        // get token from block-wise transfer list when CA_EMPTY(RST/ACK) is received
        CAResult_t res = CAGetTokenFromBlockDataList(pdu, endpoint, receivedData);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "fail to get token");
            return res;
        }

        CABlockDataID_t* blockDataID = CACreateBlockDatablockId(
                receivedData->info.token.t.chars,
                receivedData->info.token.size,
                endpoint->port);
        if (NULL == blockDataID || NULL == blockDataID->id || blockDataID->idLength < 1)
        {
            OIC_LOG(ERROR, TAG, "blockId is null");
            CADestroyBlockID(blockDataID);
            return CA_STATUS_FAILED;
        }

        CARemoveBlockDataFromList(blockDataID);
        CADestroyBlockID(blockDataID);
        return CA_NOT_SUPPORTED;
    }

    // check if block option is set and get block data
    coap_block_t block = {0, 0, 0};

    // get block1 option
    int isBlock1 = coap_get_block(pdu, COAP_OPTION_BLOCK1, &block);
    if (isBlock1)
    {
        CAResult_t res = CASetNextBlockOption1(pdu, endpoint, receivedData, block, dataLen);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "setting has failed");
            return res;
        }
    }

    // get block2 option
    int isBlock2 = coap_get_block(pdu, COAP_OPTION_BLOCK2, &block);
    if (isBlock2)
    {
        CAResult_t res = CASetNextBlockOption2(pdu, endpoint, receivedData, block, dataLen);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "setting has failed");
            return res;
        }
    }

    // check if there is error code
    if (!isBlock1 && !isBlock2)
    {
        uint32_t code = CA_RESPONSE_CODE(pdu->hdr->code);
        if (CA_REQUEST_ENTITY_INCOMPLETE == code)
        {
            CABlockDataID_t* blockDataID = CACreateBlockDatablockId(
                     pdu->hdr->token,
                     pdu->hdr->token_length,
                     endpoint->port);

            if(NULL == blockDataID || NULL == blockDataID->id || blockDataID->idLength < 1)
            {
                OIC_LOG(ERROR, TAG, "blockId is null");
                CADestroyBlockID(blockDataID);
                return CA_STATUS_FAILED;
            }

            CABlockData_t *data = CAGetBlockDataFromBlockDataList(blockDataID);
            if (NULL == data)
            {
                OIC_LOG(ERROR, TAG, "getting has failed");
                CADestroyBlockID(blockDataID);
                return CA_STATUS_FAILED;
            }

            if (COAP_OPTION_BLOCK2 == data->type)
            {
                coap_block_t *block2 = CAGetBlockOption(blockDataID,
                                                        COAP_OPTION_BLOCK2);
                if (NULL == block2)
                {
                    OIC_LOG(ERROR, TAG, "block is null");
                    CADestroyBlockID(blockDataID);
                    return CA_STATUS_FAILED;
                }

                CAResult_t res = CASetNextBlockOption2(pdu, endpoint, receivedData, *block2,
                                                       dataLen);
                if (CA_STATUS_OK != res)
                {
                    OIC_LOG(ERROR, TAG, "setting has failed");
                    CADestroyBlockID(blockDataID);
                    return res;
                }
            }
            else if (COAP_OPTION_BLOCK1 == data->type)
            {
                coap_block_t *block1 = CAGetBlockOption(blockDataID,
                                                        COAP_OPTION_BLOCK1);
                if (NULL == block1)
                {
                    OIC_LOG(ERROR, TAG, "block is null");
                    CADestroyBlockID(blockDataID);
                    return CA_STATUS_FAILED;
                }

                CAResult_t res = CASetNextBlockOption1(pdu, endpoint, receivedData, *block1,
                                                       dataLen);
                if (CA_STATUS_OK != res)
                {
                    OIC_LOG(ERROR, TAG, "setting has failed");
                    CADestroyBlockID(blockDataID);
                    return res;
                }
            }
        }
        else
        {
            // normal pdu data
            OIC_LOG(DEBUG, TAG, "it's normal pdu");

            // if received data is response message
            // and sent data remain in block data list, remove block data
            if (CA_RESPONSE_DATA == receivedData->type)
            {
                CAResult_t res = CAGetTokenFromBlockDataList(pdu, endpoint, receivedData);
                if (CA_STATUS_OK != res)
                {
                    OIC_LOG(ERROR, TAG, "fail to get token");
                    return res;
                }
            }
            return CA_NOT_SUPPORTED;
        }
    }

    return CA_STATUS_OK;
}

CAResult_t CAProcessNextStep(const coap_pdu_t *pdu, const CAMessageInfo_t *receivedData,
                             uint8_t blockWiseStatus, const CABlockDataID_t *blockID)
{
    CAResult_t res = CA_STATUS_OK;
    CAMessageInfo_t *data = NULL;

    // process blockWiseStatus
    switch (blockWiseStatus)
    {
        case CA_OPTION2_FIRST_BLOCK:
            res = CAAddSendThreadQueue(receivedData, blockID);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG(ERROR, TAG, "add has failed");
                return res;
            }
            break;

        case CA_OPTION2_CON:
            // add data to send thread
            data = CAGetDataSetFromBlockDataList(blockID);
            if (NULL == data)
            {
                OIC_LOG(ERROR, TAG, "it's unavailable");
                return CA_STATUS_FAILED;
            }

            data->info.messageId = pdu->hdr->id;

            res = CAAddSendThreadQueue(data, blockID);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG(ERROR, TAG, "add has failed");
                return res;
            }

            break;

        case CA_OPTION1_ACK:
        case CA_OPTION2_ACK:
        case CA_SENT_PREVIOUS_NON_MSG:
            res = CASendBlockMessage(pdu, CA_MSG_CONFIRM, blockWiseStatus,
                                     blockID);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG(ERROR, TAG, "send has failed");
                return res;
            }
            break;

        case CA_OPTION2_LAST_BLOCK:
            // process last block and send upper layer
            res = CAReceiveLastBlock(blockID, receivedData);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG(ERROR, TAG, "receive has failed");
                return res;
            }

            // remove data from list
            res = CARemoveBlockDataFromList(blockID);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG(ERROR, TAG, "remove has failed");
                return res;
            }
            break;

        case CA_OPTION1_NO_ACK_LAST_BLOCK:
            // process last block and send upper layer
            res = CAReceiveLastBlock(blockID, receivedData);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG(ERROR, TAG, "receive has failed");
                return res;
            }

            if (CA_MSG_NONCONFIRM == pdu->hdr->type)
            {
                // remove data from list
                res = CARemoveBlockDataFromList(blockID);
                if (CA_STATUS_OK != res)
                {
                    OIC_LOG(ERROR, TAG, "remove has failed");
                    return res;
                }
            }
            break;

        case CA_OPTION1_NO_ACK_BLOCK:
            if (CA_MSG_CONFIRM == pdu->hdr->type)
            {
                // add data to send thread
                res = CASendBlockMessage(pdu, CA_MSG_ACKNOWLEDGE, blockWiseStatus,
                                         blockID);
                if (CA_STATUS_OK != res)
                {
                    OIC_LOG(ERROR, TAG, "send has failed");
                    return res;
                }
            }
            break;

        case CA_BLOCK_INCOMPLETE:
            if (CA_MSG_CONFIRM == pdu->hdr->type || CA_MSG_ACKNOWLEDGE == pdu->hdr->type)
            {
                // add data to send thread
                res = CASendErrorMessage(pdu, blockWiseStatus,
                                         CA_REQUEST_ENTITY_INCOMPLETE,
                                         blockID);
                if (CA_STATUS_OK != res)
                {
                    OIC_LOG(ERROR, TAG, "send has failed");
                    return res;
                }
            }
            break;

        case CA_BLOCK_TOO_LARGE:
            if (CA_MSG_ACKNOWLEDGE == pdu->hdr->type)
            {
                res = CASendBlockMessage(pdu, CA_MSG_CONFIRM, blockWiseStatus,
                                         blockID);
                if (CA_STATUS_OK != res)
                {
                    OIC_LOG(ERROR, TAG, "send has failed");
                    return res;
                }
            }
            else if (CA_MSG_CONFIRM == pdu->hdr->type)
            {
                res = CASendErrorMessage(pdu, blockWiseStatus,
                                         CA_REQUEST_ENTITY_TOO_LARGE,
                                         blockID);
                if (CA_STATUS_OK != res)
                {
                    OIC_LOG(ERROR, TAG, "send has failed");
                    return res;
                }
            }
            break;
        default:
            OIC_LOG_V(ERROR, TAG, "no logic [%d]", blockWiseStatus);
    }
    return CA_STATUS_OK;
}

CAResult_t CASendBlockMessage(const coap_pdu_t *pdu, CAMessageType_t msgType,
                              uint8_t status, const CABlockDataID_t *blockID)
{
    VERIFY_NON_NULL(pdu, TAG, "pdu");
    VERIFY_NON_NULL(pdu->hdr, TAG, "pdu->hdr");
    VERIFY_NON_NULL(blockID, TAG, "blockID");

    CAMessageInfo_t *data = CAGetDataSetFromBlockDataList(blockID);
    if (NULL == data)
    {
        OIC_LOG(ERROR, TAG, "CAData is unavailable");
        return CA_STATUS_FAILED;
    }

    if (CA_MSG_CONFIRM == msgType)
    {
        data->info.messageId = 0;
    }
    else if (CA_MSG_ACKNOWLEDGE == msgType)
    {
        if (CA_RESPONSE_DATA == data->type)
        {
            OIC_LOG(DEBUG, TAG, "set ACK message");
            data->info.messageId = pdu->hdr->id;
            data->info.type = CA_MSG_ACKNOWLEDGE;
            if (CA_OPTION1_NO_ACK_LAST_BLOCK == status)
            {
                data->responseResult = CA_CHANGED;
            }
            else if (CA_OPTION1_NO_ACK_BLOCK == status)
            {
                data->responseResult = CA_CONTINUE;
            }
        }
    }

    // add data to send thread
    CAResult_t res = CAAddSendThreadQueue(data, blockID);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "add has failed");
    }

    return res;
}

CAResult_t CASendErrorMessage(const coap_pdu_t *pdu, uint8_t status,
                              CAResponseResult_t responseResult,
                              const CABlockDataID_t *blockID)
{
    // create error responseInfo
    CABlockData_t *data = CAGetBlockDataFromBlockDataList(blockID);
    if (NULL == data)
    {
        OIC_LOG(ERROR, TAG, "data is unavailable");
        return CA_STATUS_FAILED;
    }

    CAMessageInfo_t *cloneData = NULL;
    if (data->sentData && (CA_RESPONSE_DATA == data->sentData->type))
    {
        data->sentData->info.messageId = pdu->hdr->id;
        data->sentData->info.type = CA_MSG_ACKNOWLEDGE;
        data->sentData->responseResult = responseResult;
        cloneData = CACloneMessageInfo(data->sentData);
        if (NULL == cloneData)
        {
            OIC_LOG(ERROR, TAG, "clone has failed");
            return CA_MEMORY_ALLOC_FAILED;
        }
        OIC_LOG(DEBUG, TAG, "set ACK message");
    }
    else if (data->sentData)
    {
        cloneData = CACreateNewDataSet(pdu, &(data->sentData->endpoint));

        if(!cloneData)
        {
            OIC_LOG(ERROR, TAG, PCF("CACreateNewDataSet failed"));
            return CA_MEMORY_ALLOC_FAILED;
        }

        cloneData->info.type = CA_MSG_CONFIRM;
        cloneData->responseResult = responseResult;
        OIC_LOG(DEBUG, TAG, "set CON message");
    }
    else
    {
        OIC_LOG(ERROR, TAG, "data has no sent-data");
        return CA_MEMORY_ALLOC_FAILED;
    }

    // add data to send thread
    if (g_context.sendThreadFunc)
    {
        ca_mutex_lock(g_context.blockDataSenderMutex);
        g_context.sendThreadFunc(cloneData);
        ca_mutex_unlock(g_context.blockDataSenderMutex);
    }
    else
    {
        CADestroyDataSet(cloneData);
    }

    // if error code is 4.08, remove the stored payload and initialize block number
    if (CA_BLOCK_INCOMPLETE == status)
    {
        OICFree(data->payload);
        data->payload = NULL;
        data->payloadLength = 0;
        data->receivedPayloadLen = 0;
        data->block1.num = 0;
        data->block2.num = 0;
    }

    return CA_STATUS_OK;
}

CAResult_t CAReceiveLastBlock(const CABlockDataID_t *blockID,
                              const CAMessageInfo_t *receivedData)
{
    // total block data have to notify to Application
    CAMessageInfo_t *cloneData = CACloneMessageInfo(receivedData);
    if (NULL == cloneData)
    {
        OIC_LOG(ERROR, TAG, "clone has failed");
        return CA_MEMORY_ALLOC_FAILED;
    }

    // update payload
    size_t fullPayloadLen = 0;
    CAPayload_t fullPayload = CAGetPayloadFromBlockDataList(blockID,
                                                            &fullPayloadLen);
    if (NULL != fullPayload)
    {
        CAResult_t res = CAUpdatePayloadToCAMessageInfo(cloneData, fullPayload, fullPayloadLen);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "update has failed");
            CADestroyDataSet(cloneData);
            return CA_STATUS_FAILED;
        }
    }

    if (g_context.receivedThreadFunc)
    {
        g_context.receivedThreadFunc(cloneData);
    }
    else
    {
        CADestroyDataSet(cloneData);
    }

    return CA_STATUS_OK;
}

// TODO make pdu const after libcoap is updated to support that.
CAResult_t CASetNextBlockOption1(coap_pdu_t *pdu, const CAEndpoint_t *endpoint,
                                 const CAMessageInfo_t *receivedData, coap_block_t block,
                                 size_t dataLen)
{
    OIC_LOG(INFO, TAG, "CASetNextBlockOption1");
    VERIFY_NON_NULL(pdu, TAG, "pdu");
    VERIFY_NON_NULL(pdu->hdr, TAG, "pdu->hdr");
    VERIFY_NON_NULL(endpoint, TAG, "endpoint");
    VERIFY_NON_NULL(receivedData, TAG, "receivedData");

    OIC_LOG_V(INFO, TAG, "num:%d, M:%d, sze:%d", block.num, block.m, block.szx);

    CABlockDataID_t* blockDataID = CACreateBlockDatablockId(
            pdu->hdr->token,
            pdu->hdr->token_length,
            endpoint->port);

    if(NULL == blockDataID || NULL == blockDataID->id || blockDataID->idLength < 1)
    {
        OIC_LOG(ERROR, TAG, "blockId is null");
        CADestroyBlockID(blockDataID);
        return CA_STATUS_FAILED;
    }

    // BlockData data is created if it not existed
    if (!CAIsBlockDataInList(blockDataID))
    {
        OIC_LOG(DEBUG, TAG, "no message in list");

        CAMessageInfo_t *data = CACreateNewDataSet(pdu, endpoint);
        if (NULL == data)
        {
            OIC_LOG(ERROR, TAG, "data is null");
            CADestroyBlockID(blockDataID);
            return CA_STATUS_FAILED;
        }

        CABlockData_t *currData = CACreateNewBlockData(data);
        if (NULL == currData)
        {
            OIC_LOG(ERROR, TAG, "currData is null");
            CADestroyDataSet(data);
            CADestroyBlockID(blockDataID);
            return CA_STATUS_FAILED;
        }
    }

    // update BLOCK OPTION1 type
    CAResult_t res = CAUpdateBlockOptionType(blockDataID,
                                             COAP_OPTION_BLOCK1);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "update has failed");
        CARemoveBlockDataFromList(blockDataID);
        CADestroyBlockID(blockDataID);
        return res;
    }

    CABlockData_t *data = CAGetBlockDataFromBlockDataList(blockDataID);
    if (NULL == data)
    {
        OIC_LOG(ERROR, TAG, "getting has failed");
        CADestroyBlockID(blockDataID);
        return CA_STATUS_FAILED;
    }

    uint8_t blockWiseStatus = CA_BLOCK_UNKNOWN;
    // received type from remote device
    if (CA_MSG_ACKNOWLEDGE == pdu->hdr->type)
    {
        uint32_t code = CA_RESPONSE_CODE(pdu->hdr->code);
        if (0 == block.m &&
                (CA_REQUEST_ENTITY_INCOMPLETE != code && CA_REQUEST_ENTITY_TOO_LARGE != code))
        {
            OIC_LOG(INFO, TAG, "Data has sent");
            // initialize block number for response message
            data->block1.num = 0;
            CADestroyBlockID(blockDataID);
            return CA_STATUS_OK;
        }

        blockWiseStatus = CA_OPTION1_ACK;
        res = CAUpdateBlockOptionItems(data, pdu, &block, COAP_OPTION_BLOCK1, blockWiseStatus);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "update has failed");
            CADestroyBlockID(blockDataID);
            return res;
        }

        res = CAUpdateBlockData(data, block, COAP_OPTION_BLOCK1);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "update has failed");
            CARemoveBlockDataFromList(blockDataID);
            CADestroyBlockID(blockDataID);
            return res;
        }
    }
    else // CON or NON message
    {
        OIC_LOG_V(INFO, TAG, "num:%d, M:%d", block.num, block.m);

        // check the size option
        bool isSizeOption = CAIsPayloadLengthInPduWithBlockSizeOption(pdu,
                                                                      COAP_OPTION_SIZE1,
                                                                      &(data->payloadLength));

        // check if received payload is exact
        if (CA_MSG_CONFIRM == pdu->hdr->type)
        {
            blockWiseStatus = CACheckBlockErrorType(data, &block, receivedData,
                                                    COAP_OPTION_BLOCK1, dataLen);
        }

        if (CA_BLOCK_RECEIVED_ALREADY != blockWiseStatus)
        {
            // store the received payload and merge
            res = CAUpdatePayloadData(data, receivedData, blockWiseStatus,
                                      isSizeOption, COAP_OPTION_BLOCK1);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG(ERROR, TAG, "update has failed");
                CARemoveBlockDataFromList(blockDataID);
                CADestroyBlockID(blockDataID);
                return res;
            }

            res = CAUpdateBlockOptionItems(data, pdu, &block, COAP_OPTION_BLOCK1, blockWiseStatus);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG(ERROR, TAG, "update has failed");
                CARemoveBlockDataFromList(blockDataID);
                CADestroyBlockID(blockDataID);
                return res;
            }

            // update block data
            res = CAUpdateBlockData(data, block, COAP_OPTION_BLOCK1);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG(ERROR, TAG, "update has failed");
                CARemoveBlockDataFromList(blockDataID);
                CADestroyBlockID(blockDataID);
                return res;
            }
        }

        // check the blcok-wise transfer status for next step
        if (CA_BLOCK_UNKNOWN == blockWiseStatus || CA_BLOCK_RECEIVED_ALREADY == blockWiseStatus)
        {
            if (0 == block.m) // Last block is received
            {
                OIC_LOG(DEBUG, TAG, "M bit is 0");
                blockWiseStatus = CA_OPTION1_NO_ACK_LAST_BLOCK;
            }
            else
            {
                OIC_LOG(DEBUG, TAG, "M bit is 1");
                blockWiseStatus = CA_OPTION1_NO_ACK_BLOCK;
            }
        }
    }

    res = CAProcessNextStep(pdu, receivedData, blockWiseStatus, blockDataID);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "setting has failed");
        CARemoveBlockDataFromList(blockDataID);
    }

    CADestroyBlockID(blockDataID);
    return res;
}

// TODO make pdu const after libcoap is updated to support that.
CAResult_t CASetNextBlockOption2(coap_pdu_t *pdu, const CAEndpoint_t *endpoint,
                                 const CAMessageInfo_t *receivedData, coap_block_t block,
                                 size_t dataLen)
{
    OIC_LOG(DEBUG, TAG, "CASetNextBlockOption2");
    OIC_LOG_V(INFO, TAG, "num:%d, M:%d, sze:%d", block.num, block.m, block.szx);

    VERIFY_NON_NULL(pdu, TAG, "pdu");
    VERIFY_NON_NULL(pdu->hdr, TAG, "pdu->hdr");
    VERIFY_NON_NULL(endpoint, TAG, "endpoint");
    VERIFY_NON_NULL(receivedData, TAG, "receivedData");

    CABlockDataID_t* blockDataID = CACreateBlockDatablockId(
            pdu->hdr->token,
            pdu->hdr->token_length,
            endpoint->port);

    if(NULL == blockDataID || NULL == blockDataID->id || blockDataID->idLength < 1)
    {
        OIC_LOG(ERROR, TAG, "blockId is null");
        CADestroyBlockID(blockDataID);
        return CA_STATUS_FAILED;
    }

    // BlockData data is created if it not existed
    if (!CAIsBlockDataInList(blockDataID))
    {
        OIC_LOG(DEBUG, TAG, "no msg in list.");

        CAMessageInfo_t *data = CACreateNewDataSet(pdu, endpoint);
        if (NULL == data)
        {
            OIC_LOG(ERROR, TAG, "data is null");
            CADestroyBlockID(blockDataID);
            return CA_STATUS_FAILED;
        }

        CABlockData_t *currData = CACreateNewBlockData(data);
        if (NULL == currData)
        {
            OIC_LOG(ERROR, TAG, "data is null");
            CADestroyDataSet(data);
            CADestroyBlockID(blockDataID);
            return CA_STATUS_FAILED;
        }
    }

    // set Block Option Type
    CAResult_t res = CAUpdateBlockOptionType(blockDataID,
                                             COAP_OPTION_BLOCK2);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "update has failed");
        CARemoveBlockDataFromList(blockDataID);
        CADestroyBlockID(blockDataID);
        return res;
    }

    CABlockData_t *data = CAGetBlockDataFromBlockDataList(blockDataID);
    if (NULL == data)
    {
        OIC_LOG(ERROR, TAG, "getting has failed");
        CARemoveBlockDataFromList(blockDataID);
        CADestroyBlockID(blockDataID);
        return CA_STATUS_FAILED;
    }

    uint8_t blockWiseStatus = CA_BLOCK_UNKNOWN;
    if (0 == block.num && CA_GET == pdu->hdr->code && 0 == block.m)
    {
        OIC_LOG(INFO, TAG, "first block number");

        res = CAUpdateBlockOptionItems(data, pdu, &block, COAP_OPTION_BLOCK2, blockWiseStatus);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "update has failed");
            CARemoveBlockDataFromList(blockDataID);
            CADestroyBlockID(blockDataID);
            return res;
        }

        // first block data have to notify to Application
        res = CAUpdateBlockData(data, block, COAP_OPTION_BLOCK2);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "update has failed");
            CARemoveBlockDataFromList(blockDataID);
            CADestroyBlockID(blockDataID);
            return res;
        }
        blockWiseStatus = CA_OPTION2_FIRST_BLOCK;
    }
    else
    {
        // received type from remote device
        if (CA_MSG_ACKNOWLEDGE == pdu->hdr->type ||
                (CA_MSG_NONCONFIRM == pdu->hdr->type && (CA_RESPONSE_DATA == receivedData->type)))
        {
            OIC_LOG(DEBUG, TAG, "received ACK or NON");

            // check the size option
            bool isSizeOption = CAIsPayloadLengthInPduWithBlockSizeOption(pdu,
                                                                          COAP_OPTION_SIZE2,
                                                                          &(data->payloadLength));

            // check if received payload is exact
            if (CA_MSG_ACKNOWLEDGE == pdu->hdr->type)
            {
                blockWiseStatus = CACheckBlockErrorType(data, &block, receivedData,
                                                        COAP_OPTION_BLOCK2, dataLen);
            }

            if (CA_BLOCK_RECEIVED_ALREADY != blockWiseStatus)
            {
                // store the received payload and merge
                res = CAUpdatePayloadData(data, receivedData, blockWiseStatus,
                                          isSizeOption, COAP_OPTION_BLOCK2);
                if (CA_STATUS_OK != res)
                {
                    OIC_LOG(ERROR, TAG, "update has failed");
                    CARemoveBlockDataFromList(blockDataID);
                    CADestroyBlockID(blockDataID);
                    return res;
                }
            }

            if (0 == block.m && CA_BLOCK_UNKNOWN == blockWiseStatus) // Last block is received
            {
                OIC_LOG(DEBUG, TAG, "M bit is 0");
                blockWiseStatus = CA_OPTION2_LAST_BLOCK;
            }
            else
            {
                if (CA_BLOCK_UNKNOWN == blockWiseStatus ||
                        CA_BLOCK_RECEIVED_ALREADY == blockWiseStatus)
                {
                    OIC_LOG(DEBUG, TAG, "M bit is 1");

                    if (CA_MSG_ACKNOWLEDGE == pdu->hdr->type)
                    {
                        blockWiseStatus = CA_OPTION2_ACK;
                    }
                    else
                    {
                        blockWiseStatus = CA_OPTION2_NON;
                    }
                }

                res = CAUpdateBlockOptionItems(data, pdu, &block, COAP_OPTION_BLOCK2,
                                               blockWiseStatus);
                if (CA_STATUS_OK != res)
                {
                    OIC_LOG(ERROR, TAG, "update has failed");
                    CARemoveBlockDataFromList(blockDataID);
                    CADestroyBlockID(blockDataID);
                    return res;
                }

                res = CAUpdateBlockData(data, block, COAP_OPTION_BLOCK2);
                if (CA_STATUS_OK != res)
                {
                    OIC_LOG(ERROR, TAG, "update has failed");
                    CARemoveBlockDataFromList(blockDataID);
                    CADestroyBlockID(blockDataID);
                    return res;
                }
            }
        }
        else // CON message and so on.
        {
            OIC_LOG_V(INFO, TAG, "num:%d, M:%d", block.num, block.m);

            blockWiseStatus = CA_OPTION2_CON;

            res = CAUpdateBlockOptionItems(data, pdu, &block, COAP_OPTION_BLOCK2, blockWiseStatus);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG(ERROR, TAG, "update has failed");
                CARemoveBlockDataFromList(blockDataID);
                CADestroyBlockID(blockDataID);
                return res;
            }

            res = CAUpdateBlockData(data, block, COAP_OPTION_BLOCK2);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG(ERROR, TAG, "update has failed");
                CARemoveBlockDataFromList(blockDataID);
                CADestroyBlockID(blockDataID);
                return res;
            }
        }
    }

    res = CAProcessNextStep(pdu, receivedData, blockWiseStatus, blockDataID);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "setting has failed");
        CARemoveBlockDataFromList(blockDataID);
        CADestroyBlockID(blockDataID);
        return res;
    }

    CADestroyBlockID(blockDataID);
    return CA_STATUS_OK;
}

CAResult_t CAUpdateBlockOptionItems(CABlockData_t *currData, const coap_pdu_t *pdu,
                                    coap_block_t *block, uint16_t blockType,
                                    uint32_t status)
{
    VERIFY_NON_NULL(currData, TAG, "currData");
    VERIFY_NON_NULL(pdu, TAG, "pdu");
    VERIFY_NON_NULL(block, TAG, "block");

    // update block data
    CAResult_t res = CA_STATUS_OK;
    uint32_t code = CA_RESPONSE_CODE(pdu->hdr->code);

    if (CA_REQUEST_ENTITY_INCOMPLETE == code || CA_REQUEST_ENTITY_TOO_LARGE == code)
    {
        // response error code of the received block message
        res = CAHandleBlockErrorResponse(block, blockType, code);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "error handle has failed");
            return res;
        }
    }
    else
    {
        // update block option items
        switch (status)
        {
            case CA_OPTION1_ACK:
                if (currData->block1.num > block->num)
                {
                    OIC_LOG(ERROR, TAG, "received incorrect block num");
                    return CA_STATUS_FAILED;
                }
                block->num++;
                break;
            case CA_OPTION2_NON:
                block->num++;
                block->m = 0;
                break;
            case CA_OPTION2_CON:
                block->m = 0;
                break;
            case CA_OPTION2_ACK:
                if (currData->block2.num > block->num)
                {
                    OIC_LOG(ERROR, TAG, "received incorrect block num");
                    return CA_STATUS_FAILED;
                }
                block->num++;
                block->m = 0;
                break;
            case CA_BLOCK_TOO_LARGE:
                // if state of received block message is CA_BLOCK_TOO_LARGE or CA_BLOCK_INCOMPLETE
                // we set the response error code appropriately and send
                if (COAP_OPTION_BLOCK2 == blockType)
                {
                    block->num++;
                    block->m = 0;
                    block->szx = currData->block2.szx;
                }
                else
                {
                    block->szx = currData->block1.szx;
                }
                break;
            default:
                OIC_LOG_V(ERROR, TAG, "no logic [%d]", status);
        }

        if (CA_BLOCK_INCOMPLETE != status && CA_BLOCK_TOO_LARGE != status)
        {
            // negotiate block size
            res = CANegotiateBlockSize(currData, block, pdu->hdr->type, blockType);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG(ERROR, TAG, "negotiation has failed");
                return res;
            }
        }
    }
    return res;
}

CAResult_t CAGetMoreBitFromBlock(size_t payloadLen, coap_block_t *block)
{
    VERIFY_NON_NULL(block, TAG, "block");

    if ((size_t)((block->num + 1) << (block->szx + BLOCK_NUMBER_IDX))
        < payloadLen)
    {
        OIC_LOG(DEBUG, TAG, "Set the M-bit(1)");
        block->m = 1;
    }
    else
    {
        OIC_LOG(DEBUG, TAG, "Set the M-bit(0)");
        block->m = 0;
    }

    return CA_STATUS_OK;
}

CAResult_t CANegotiateBlockSize(CABlockData_t *currData, coap_block_t *block,
                                CAMessageType_t msgType, uint16_t blockType)
{
    OIC_LOG(DEBUG, TAG, "IN-NegotiateBlockSize");

    VERIFY_NON_NULL(currData, TAG, "currData");
    VERIFY_NON_NULL(block, TAG, "block");

    // #1. check the block option type
    if (COAP_OPTION_BLOCK2 == blockType)
    {
        // #2. check the message type
        if (CA_MSG_ACKNOWLEDGE == msgType)
        {
            if (block->szx > currData->block2.szx)
            {
                OIC_LOG(DEBUG, TAG, "sze is big");

                // #3. calculate new block number from block size
                unsigned int blockNum = BLOCK_SIZE(block->szx) /
                                        BLOCK_SIZE(currData->block2.szx) - 1;
                OIC_LOG(DEBUG, TAG, "num is set as Negotiation");
                block->num += blockNum;
                block->szx = currData->block2.szx;
                OIC_LOG_V(DEBUG, TAG, "updated block num: %d", block->num);
            }
        }
        else
        {
            if (block->szx > currData->block2.szx)
            {
                OIC_LOG(DEBUG, TAG, "sze is big");
                block->szx = currData->block2.szx;
            }
        }
    }
    else if (COAP_OPTION_BLOCK1 == blockType)
    {
        if (CA_MSG_ACKNOWLEDGE == msgType)
        {
            if (block->szx < currData->block1.szx)
            {
                OIC_LOG(DEBUG, TAG, "sze is small");

                unsigned int blockNum = BLOCK_SIZE(currData->block1.szx) /
                                        BLOCK_SIZE(block->szx) - 1;
                block->num += blockNum;
                OIC_LOG_V(DEBUG, TAG, "updated block num: %d", block->num);
            }
        }
        else
        {
            if (block->szx > currData->block1.szx)
            {
                OIC_LOG(DEBUG, TAG, "sze is big");
                block->szx = currData->block1.szx;
            }
        }
    }
    else
    {
        OIC_LOG(DEBUG, TAG, "Invalid block option");
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TAG, "OUT-NegotiateBlockSize");

    return CA_STATUS_OK;
}

CAResult_t CAUpdateBlockData(CABlockData_t *currData, coap_block_t block,
                             uint16_t blockType)
{
    VERIFY_NON_NULL(currData, TAG, "currData");

    // check if block size is bigger than CABlockSize_t
    if (block.szx > CA_BLOCK_SIZE_1024_BYTE)
    {
        OIC_LOG(DEBUG, TAG, "invalid block szx");
        return CA_STATUS_FAILED;
    }

    // update block option
    if (COAP_OPTION_BLOCK2 == blockType)
    {
        currData->block2 = block;
    }
    else
    {
        currData->block1 = block;
    }

    OIC_LOG(DEBUG, TAG, "data has updated");
    return CA_STATUS_OK;
}

CAResult_t CAUpdateMessageId(coap_pdu_t *pdu, const CABlockDataID_t *blockID)
{
    // if CON message is sent, update messageId in block-wise transfer list
    if (CA_MSG_CONFIRM == pdu->hdr->type)
    {
        CAMessageInfo_t *cadata = CAGetDataSetFromBlockDataList(blockID);
        if (NULL == cadata)
        {
            OIC_LOG(ERROR, TAG, "CAData is unavailable");
            return CA_STATUS_FAILED;
        }

        if (CA_REQUEST_DATA == cadata->type)
        {
            cadata->info.messageId = pdu->hdr->id;
        }
    }

    return CA_STATUS_OK;
}

CAResult_t CAAddBlockOption(coap_pdu_t **pdu, const CAMessageInfo_t *messageInfo)
{
    size_t dataLength = 0;
    if (messageInfo->info.payload)
    {
        dataLength = messageInfo->info.payloadSize;
        OIC_LOG_V(DEBUG, TAG, "dataLength - %d", dataLength);
    }

    OIC_LOG_V(DEBUG, TAG, "previous payload - %s", (*pdu)->data);

    uint32_t code = CA_RESPONSE_CODE((*pdu)->hdr->code);
    if (CA_REQUEST_ENTITY_INCOMPLETE == code)
    {
        OIC_LOG(INFO, TAG, "don't use option");
        return CA_STATUS_OK;
    }

    CABlockDataID_t* blockDataID = CACreateBlockDatablockId(
            (*pdu)->hdr->token,
            (*pdu)->hdr->token_length,
            messageInfo->endpoint.port);

    if(NULL == blockDataID || NULL == blockDataID->id || blockDataID->idLength < 1)
    {
        OIC_LOG(ERROR, TAG, "blockId is null");
        CADestroyBlockID(blockDataID);
        return CA_STATUS_FAILED;
    }

    uint8_t blockType = CAGetBlockOptionType(blockDataID);
    if (COAP_OPTION_BLOCK2 == blockType)
    {
        CAResult_t res = CAAddBlockOption2(pdu, &(messageInfo->info), dataLength,
                                           blockDataID);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "add has failed");
            CADestroyBlockID(blockDataID);
            return res;
        }
    }
    else if (COAP_OPTION_BLOCK1 == blockType)
    {
        CAResult_t res = CAAddBlockOption1(pdu, &(messageInfo->info), dataLength,
                                           blockDataID);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "add has failed");
            CADestroyBlockID(blockDataID);
            return res;
        }
    }
    else
    {
        OIC_LOG(DEBUG, TAG, "no BLOCK option");
        // if response data is so large. it have to send as block transfer
        if (!coap_add_data(*pdu, dataLength, (const unsigned char *)(&messageInfo->info.payload)))
        {
            OIC_LOG(INFO, TAG, "it have to use block");
        }
        else
        {
            OIC_LOG(INFO, TAG, "not Blockwise Transfer");
        }
    }

    CAResult_t res = CAUpdateMessageId(*pdu, blockDataID);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "fail to update CON message id ");
        CADestroyBlockID(blockDataID);
        return res;
    }

    CADestroyBlockID(blockDataID);
    OIC_LOG(DEBUG, TAG, "OUT-AddBlockOption");
    return CA_STATUS_OK;
}

CAResult_t CAAddBlockOption2(coap_pdu_t **pdu, const CAInfo_t *info, size_t dataLength,
                             const CABlockDataID_t *blockID)
{
    OIC_LOG(DEBUG, TAG, "IN-AddBlockOption2");
    VERIFY_NON_NULL(pdu, TAG, "pdu");
    VERIFY_NON_NULL((*pdu), TAG, "(*pdu)");
    VERIFY_NON_NULL((*pdu)->hdr, TAG, "(*pdu)->hdr");
    VERIFY_NON_NULL(blockID, TAG, "blockID");

    // get set block data from CABlock list-set.
    coap_block_t *block1 = CAGetBlockOption(blockID,
                                            COAP_OPTION_BLOCK1);
    coap_block_t *block2 = CAGetBlockOption(blockID,
                                            COAP_OPTION_BLOCK2);
    if (!block1 || !block2)
    {
        OIC_LOG(ERROR, TAG, "getting has failed");
        return CA_STATUS_FAILED;
    }

    CALogBlockInfo(block2);

    uint8_t code = 0;
    if (CA_MSG_ACKNOWLEDGE == (*pdu)->hdr->type ||
            (CA_MSG_NONCONFIRM == (*pdu)->hdr->type && CA_GET != (*pdu)->hdr->code))
    {
        int32_t res = coap_write_block_opt(block2, COAP_OPTION_BLOCK2, *pdu, dataLength);
        switch (res)
        {
            case -2: /* illegal block */
                code = COAP_RESPONSE_CODE(CA_BAD_REQ);
                OIC_LOG(ERROR, TAG, "write block option : -2");
                goto error;
            case -1: /* should really not happen */
                OIC_LOG(ERROR, TAG, "write block option : -1");
                break;
            case -3: /* cannot handle request */
                code = COAP_RESPONSE_CODE(CA_INTERNAL_SERVER_ERROR);
                OIC_LOG(ERROR, TAG, "write block option : -3");
                goto error;
            default:
                OIC_LOG(INFO, TAG, "success write block option");
        }
        CALogBlockInfo(block2);

        if (block1->num)
        {
            OIC_LOG(DEBUG, TAG, "combining block1 and block2");
            CAResult_t res = CAAddBlockOptionImpl(*pdu, block1, COAP_OPTION_BLOCK1);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG(ERROR, TAG, "add has failed");
                CARemoveBlockDataFromList(blockID);
                return res;
            }
            // initialize block number
            block1->num = 0;
        }

        // if block number is 0, add size2 option
        if (0 == block2->num)
        {
            res = CAAddBlockSizeOption(*pdu, COAP_OPTION_SIZE2, dataLength);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG(ERROR, TAG, "add has failed");
                CARemoveBlockDataFromList(blockID);
                return res;
            }
        }

        if (!coap_add_block(*pdu, dataLength, (const unsigned char *)info->payload,
                            block2->num, block2->szx))
        {
            OIC_LOG(ERROR, TAG, "Data length is smaller than the start index");
            return CA_STATUS_FAILED;
        }

        if (!block2->m)
        {
            // if sent message is last response block message, remove data
            CARemoveBlockDataFromList(blockID);
        }
        else
        {
            if (CA_MSG_NONCONFIRM == (*pdu)->hdr->type)
            {
                OIC_LOG(DEBUG, TAG, "NON, send next block..");
                // update block data
                block2->num++;
                CAResult_t res = CAProcessNextStep(*pdu, NULL,
                                                   CA_SENT_PREVIOUS_NON_MSG,
                                                   blockID);
                if (CA_STATUS_OK != res)
                {
                    OIC_LOG(ERROR, TAG, "failed to process next step");
                    CARemoveBlockDataFromList(blockID);
                    return res;
                }
            }
        }
    }
    else
    {
        OIC_LOG(DEBUG, TAG, "option2, not ACK msg");
        CAResult_t res = CAAddBlockOptionImpl(*pdu, block2, COAP_OPTION_BLOCK2);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "add has failed");
            CARemoveBlockDataFromList(blockID);
            return res;
        }
    }

    return CA_STATUS_OK;

error:
    OIC_LOG_V(ERROR, TAG, "error : %d", code);

    char* phrase = coap_response_phrase(code);
    if(phrase)
    {
        coap_add_data(*pdu, strlen(phrase),
                      (unsigned char *) phrase);
    }
    return CA_STATUS_FAILED;
}

CAResult_t CAAddBlockOption1(coap_pdu_t **pdu, const CAInfo_t *info, size_t dataLength,
                             const CABlockDataID_t *blockID)
{
    OIC_LOG(DEBUG, TAG, "IN-AddBlockOption1");
    VERIFY_NON_NULL(pdu, TAG, "pdu");
    VERIFY_NON_NULL((*pdu), TAG, "(*pdu)");
    VERIFY_NON_NULL((*pdu)->hdr, TAG, "(*pdu)->hdr");
    VERIFY_NON_NULL(blockID, TAG, "blockID");

    // get set block data from CABlock list-set.
    coap_block_t *block1 = CAGetBlockOption(blockID,
                                            COAP_OPTION_BLOCK1);
    if (NULL == block1)
    {
        OIC_LOG(ERROR, TAG, "getting has failed");
        return CA_STATUS_FAILED;
    }

    CALogBlockInfo(block1);

    if (CA_MSG_ACKNOWLEDGE == (*pdu)->hdr->type)
    {
        OIC_LOG(DEBUG, TAG, "option1 and ACK msg..");
        CAResult_t res = CAAddBlockOptionImpl(*pdu, block1, COAP_OPTION_BLOCK1);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "add has failed");
            CARemoveBlockDataFromList(blockID);
            return res;
        }

        // reset block-list after write block
        if (0 == block1->m)
        {
            // remove data from list
            CAResult_t res = CARemoveBlockDataFromList(blockID);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG(ERROR, TAG, "remove has failed");
                return res;
            }
        }
    }
    else
    {
        CAGetMoreBitFromBlock(dataLength, block1);
        CAResult_t res = CAAddBlockOptionImpl(*pdu, block1, COAP_OPTION_BLOCK1);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "add has failed");
            CARemoveBlockDataFromList(blockID);
            return res;
        }
        CALogBlockInfo(block1);

        // if block number is 0, add size1 option
        if (0 == block1->num)
        {
            res = CAAddBlockSizeOption(*pdu, COAP_OPTION_SIZE1, dataLength);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG(ERROR, TAG, "add has failed");
                CARemoveBlockDataFromList(blockID);
                return res;
            }
        }

        if (!coap_add_block(*pdu, dataLength, (const unsigned char *)info->payload,
                            block1->num, block1->szx))
        {
            OIC_LOG(ERROR, TAG, "Data length is smaller than the start index");
            return CA_STATUS_FAILED;
        }

        // check the message type and if message type is NON, next block message will be sent
        if (CA_MSG_NONCONFIRM == (*pdu)->hdr->type)
        {
            if (block1->m)
            {
                OIC_LOG(DEBUG, TAG, "NON, send next block..");
                // update block data
                block1->num++;
                CAResult_t res = CAProcessNextStep(*pdu, NULL,
                                                   CA_SENT_PREVIOUS_NON_MSG,
                                                   blockID);
                if (CA_STATUS_OK != res)
                {
                    OIC_LOG(ERROR, TAG, "failed to process next step");
                    CARemoveBlockDataFromList(blockID);
                    return res;
                }
            }
            else
            {
                CARemoveBlockDataFromList(blockID);
            }
        }
    }

    OIC_LOG(DEBUG, TAG, "OUT-AddBlockOption1");

    return CA_STATUS_OK;
}

CAResult_t CAAddBlockOptionImpl(coap_pdu_t *pdu, coap_block_t *block, uint8_t blockType)
{
    OIC_LOG(DEBUG, TAG, "IN-AddBlockOptionImpl");
    VERIFY_NON_NULL(pdu, TAG, "pdu");
    VERIFY_NON_NULL(block, TAG, "block");

    coap_option *option = (coap_option *) OICMalloc(sizeof(coap_option));
    if (NULL == option)
    {
        OIC_LOG(ERROR, TAG, "out of memory");
        return CA_MEMORY_ALLOC_FAILED;
    }

    unsigned char buf[BLOCKWISE_OPTION_BUFFER] = { 0 };
    option->key = blockType;
    option->length = coap_encode_var_bytes(buf,
                                           ((block->num << BLOCK_NUMBER_IDX)
                                            | (block->m << BLOCK_M_BIT_IDX) | block->szx));
    if (!coap_add_option(pdu, option->key, option->length, buf))
    {
        OIC_LOG(ERROR, TAG, "coap_add_option has failed");
        OICFree(option);
        return CA_STATUS_FAILED;
    }

    OICFree(option);

    OIC_LOG(DEBUG, TAG, "OUT-AddBlockOptionImpl");
    return CA_STATUS_OK;
}

CAResult_t CAAddBlockSizeOption(coap_pdu_t *pdu, uint16_t sizeType, size_t dataLength)
{
    OIC_LOG(DEBUG, TAG, "IN-CAAddBlockSizeOption");
    VERIFY_NON_NULL(pdu, TAG, "pdu");

    if (sizeType != COAP_OPTION_SIZE1 && sizeType != COAP_OPTION_SIZE2)
    {
        OIC_LOG(ERROR, TAG, "unknown option type");
        return CA_STATUS_FAILED;
    }

    unsigned char value[BLOCKWISE_OPTION_BUFFER] = { 0 };
    unsigned int optionLength = coap_encode_var_bytes(value, dataLength);

    if (!coap_add_option(pdu, sizeType, optionLength, value))
    {
        OIC_LOG(ERROR, TAG, "failed to add size option");
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TAG, "OUT-CAAddBlockSizeOption");

    return CA_STATUS_OK;
}

// TODO make pdu const after libcoap is updated to support that.
bool CAIsPayloadLengthInPduWithBlockSizeOption(coap_pdu_t *pdu,
                                               uint16_t sizeType,
                                               size_t *totalPayloadLen)
{
    OIC_LOG(DEBUG, TAG, "IN-CAIsPayloadLengthInPduWithBlockSizeOption");
    VERIFY_NON_NULL(pdu, TAG, "pdu");
    VERIFY_NON_NULL(totalPayloadLen, TAG, "totalPayloadLen");

    if (sizeType != COAP_OPTION_SIZE1 && sizeType != COAP_OPTION_SIZE2)
    {
        OIC_LOG(ERROR, TAG, "unknown option type");
        return CA_STATUS_FAILED;
    }

    coap_opt_iterator_t opt_iter;
    coap_opt_t *option = coap_check_option(pdu, sizeType, &opt_iter);
    if (option)
    {
        OIC_LOG(DEBUG, TAG, "get size option from pdu");
        *totalPayloadLen = coap_decode_var_bytes(COAP_OPT_VALUE(option),
                                                 COAP_OPT_LENGTH(option));

        OIC_LOG_V(DEBUG, TAG, "the total payload length to be received is [%d]bytes",
                  *totalPayloadLen);

        return true;
    }

    OIC_LOG(DEBUG, TAG, "OUT-CAIsPayloadLengthInPduWithBlockSizeOption");

    return false;
}

uint8_t CACheckBlockErrorType(CABlockData_t *currData, coap_block_t *receivedBlock,
                              const CAMessageInfo_t *receivedData, uint16_t blockType,
                              size_t dataLen)
{
    OIC_LOG(DEBUG, TAG, "IN-CheckBlockError");

    VERIFY_NON_NULL(currData, TAG, "currData is NULL");
    VERIFY_NON_NULL(receivedBlock, TAG, "receivedBlock is NULL");
    VERIFY_NON_NULL(receivedData, TAG, "receivedData is NULL");

    // #1. check the received payload length
    size_t blockPayloadLen = 0;
    CAGetPayloadInfo(receivedData, &blockPayloadLen);

    // #2. check if the block sequence is right
    if (COAP_OPTION_BLOCK1 == blockType)
    {
        size_t prePayloadLen = currData->receivedPayloadLen;
        if (prePayloadLen != (size_t)BLOCK_SIZE(receivedBlock->szx)
            * receivedBlock->num)
        {
            if (receivedBlock->num > currData->block1.num + 1)
            {
                // 408 Error handling of block loss
                OIC_LOG(ERROR, TAG, "option1: error 4.08");
                OIC_LOG(ERROR, TAG, "it didn't order");
                return CA_BLOCK_INCOMPLETE;
            }
            return CA_BLOCK_RECEIVED_ALREADY;
        }
    }
    else if (COAP_OPTION_BLOCK2 == blockType)
    {
        if (receivedBlock->num != currData->block2.num)
        {
            if (receivedBlock->num > currData->block2.num)
            {
                // 408 Error handling of block loss
                OIC_LOG(ERROR, TAG, "option2: error 4.08");
                OIC_LOG(ERROR, TAG, "it didn't order");
                return CA_BLOCK_INCOMPLETE;
            }
            return CA_BLOCK_RECEIVED_ALREADY;
        }
    }

    // #3. check if error check logic is required
    size_t optionLen = dataLen - blockPayloadLen;
    if (receivedBlock->m && blockPayloadLen !=
        (size_t)BLOCK_SIZE(receivedBlock->szx))
    {
        // 413 Error handling of too large entity
        if (COAP_MAX_PDU_SIZE < BLOCK_SIZE(receivedBlock->szx) + optionLen)
        {
            // buffer size is smaller than received block size
            OIC_LOG(ERROR, TAG, "error type 4.13");
            OIC_LOG(ERROR, TAG, "too large size");

            // set the block size to be smaller than COAP_MAX_PDU_SIZE
            for (int32_t size = CA_DEFAULT_BLOCK_SIZE; size >= 0; size--)
            {
                if (COAP_MAX_PDU_SIZE >= BLOCK_SIZE(size) + optionLen)
                {
                    OIC_LOG_V(ERROR, TAG, "replace sze with %d", size);
                    if (COAP_OPTION_BLOCK2 == blockType)
                    {
                        currData->block2.szx = size;
                    }
                    else
                    {
                        currData->block1.szx = size;
                    }
                    break;
                }
            }
            return CA_BLOCK_TOO_LARGE;
        }
        else
        {
            // 408 Error handling of payload loss
            OIC_LOG(ERROR, TAG, "error type 4.08");
            OIC_LOG(ERROR, TAG, "payload len != block sze");
            return CA_BLOCK_INCOMPLETE;
        }
    }
    else if (0 == receivedBlock->m && 0 != currData->payloadLength)
    {
        // if the received block is last block, check the total payload length
        size_t receivedPayloadLen = currData->receivedPayloadLen;
        receivedPayloadLen += blockPayloadLen;

        if (receivedPayloadLen != currData->payloadLength)
        {
            OIC_LOG(ERROR, TAG, "error type 4.08");
            OIC_LOG(ERROR, TAG, "total payload length is wrong");
            return CA_BLOCK_INCOMPLETE;
        }
    }

    OIC_LOG(DEBUG, TAG, "received all data normally");

    OIC_LOG(DEBUG, TAG, "OUT-CheckBlockError");

    return CA_BLOCK_UNKNOWN;
}

CAResult_t CAUpdatePayloadData(CABlockData_t *currData, const CAMessageInfo_t *receivedData,
                               uint8_t status, bool isSizeOption, uint16_t blockType)
{
    OIC_LOG(DEBUG, TAG, "IN-UpdatePayloadData");

    VERIFY_NON_NULL(currData, TAG, "currData");
    VERIFY_NON_NULL(receivedData, TAG, "receivedData");

    // if error code is 4.08, do not update payload
    if (CA_BLOCK_INCOMPLETE == status)
    {
        OIC_LOG(ERROR, TAG, "no require to update");
        return CA_STATUS_OK;
    }

    size_t blockPayloadLen = 0;
    CAPayload_t blockPayload = CAGetPayloadInfo(receivedData, &blockPayloadLen);

    if (CA_BLOCK_TOO_LARGE == status)
    {
        blockPayloadLen = (COAP_OPTION_BLOCK2 == blockType) ?
                BLOCK_SIZE(currData->block2.szx) : BLOCK_SIZE(currData->block1.szx);
    }

    // memory allocation for the received block payload
    size_t prePayloadLen = currData->receivedPayloadLen;
    if (NULL != blockPayload)
    {
        if (0 != currData->payloadLength)
        {
            // in case the block message has the size option
            // allocate the memory for the total payload
            if (true == isSizeOption)
            {
                CAPayload_t prePayload = currData->payload;

                OIC_LOG(DEBUG, TAG, "allocate memory for the total payload");
                currData->payload = (CAPayload_t) OICCalloc(currData->payloadLength + 1,
                                                            sizeof(char));
                if (NULL == currData->payload)
                {
                    OIC_LOG(ERROR, TAG, "out of memory");
                    return CA_MEMORY_ALLOC_FAILED;
                }
                memcpy(currData->payload, prePayload, prePayloadLen);
                OICFree(prePayload);
            }

            // update the total payload
            memcpy(currData->payload + prePayloadLen, blockPayload, blockPayloadLen);
        }
        else
        {
            OIC_LOG(DEBUG, TAG, "allocate memory for the received block payload");

            size_t totalPayloadLen = prePayloadLen + blockPayloadLen + 1;
            CAPayload_t newPayload = OICRealloc(currData->payload, totalPayloadLen);
            if (NULL == newPayload)
            {
                OIC_LOG(ERROR, TAG, "out of memory");
                return CA_MEMORY_ALLOC_FAILED;
            }

            // update the total payload
            memset(newPayload + prePayloadLen, 0, blockPayloadLen + 1);
            currData->payload = newPayload;
            memcpy(currData->payload + prePayloadLen, blockPayload, blockPayloadLen);
        }

        // update received payload length
        currData->receivedPayloadLen += blockPayloadLen;

        OIC_LOG_V(DEBUG, TAG, "updated payload: %s, len: %d", currData->payload,
                  currData->receivedPayloadLen);
    }

    return CA_STATUS_OK;
}

CAMessageInfo_t* CACreateNewDataSet(const coap_pdu_t *pdu, const CAEndpoint_t *endpoint)
{
    if (pdu->hdr->token_length > 8)
    {
        OIC_LOG(DEBUG, TAG, "bad pdu token length");
        return NULL;
    }

    CAMessageInfo_t* responseInfo = (CAMessageInfo_t*)OICCalloc(1, sizeof (CAMessageInfo_t));
    memcpy(responseInfo->info.token.t.chars, pdu->hdr->token, pdu->hdr->token_length);
    responseInfo->info.token.size = pdu->hdr->token_length;

    responseInfo->type = CA_RESPONSE_DATA;
    responseInfo->endpoint = *endpoint;

    return responseInfo;
}

CAResult_t CAUpdatePayloadToCAMessageInfo(CAMessageInfo_t *data,
                                   const CAPayload_t payload, size_t payloadLen)
{
    if (NULL == data->info.payload || data->info.payloadSize < payloadLen)
    {
        OICFree(data->info.payload);
        data->info.payload = (uint8_t *)OICMalloc(payloadLen);
    }

    memcpy(data->info.payload, payload, payloadLen);
    data->info.payloadSize = payloadLen;

    return CA_STATUS_OK;
}

CAPayload_t CAGetPayloadInfo(const CAMessageInfo_t *data, size_t *payloadLen)
{
    if (!data->info.payload)
    {
        return NULL;
    }

    *payloadLen = data->info.payloadSize;
    return data->info.payload;
}

CAResult_t CAHandleBlockErrorResponse(coap_block_t *block, uint16_t blockType,
                                      uint32_t responseResult)
{
    OIC_LOG(DEBUG, TAG, "IN-HandleBlockErrorRes");
    VERIFY_NON_NULL(block, TAG, "block is NULL");

    // update block data
    switch (responseResult)
    {
        case CA_REQUEST_ENTITY_INCOMPLETE:
            block->num = 0;
            break;
        case CA_REQUEST_ENTITY_TOO_LARGE:
            if (COAP_OPTION_BLOCK1 == blockType)
            {
                block->num++;
            }
            block->m = 0;
            break;
        default:
            OIC_LOG_V(ERROR, TAG, "there is no Error Code of BWT[%d]", responseResult);
    }

    OIC_LOG(DEBUG, TAG, "OUT-HandleBlockErrorRes");
    return CA_STATUS_OK;
}

CAResult_t CAUpdateBlockOptionType(const CABlockDataID_t *blockID,
                                   uint8_t blockType)
{
    OIC_LOG(DEBUG, TAG, "IN-UpdateBlockOptionType");
    VERIFY_NON_NULL(blockID, TAG, "blockID");

    ca_mutex_lock(g_context.blockDataListMutex);

    size_t len = u_arraylist_length(g_context.dataList);
    for (size_t i = 0; i < len; i++)
    {
        CABlockData_t *currData = (CABlockData_t *) u_arraylist_get(g_context.dataList, i);
        if (CABlockidMatches(currData, blockID))
        {
            currData->type = blockType;
            ca_mutex_unlock(g_context.blockDataListMutex);
            OIC_LOG(DEBUG, TAG, "OUT-UpdateBlockOptionType");
            return CA_STATUS_OK;
        }
    }
    ca_mutex_unlock(g_context.blockDataListMutex);

    OIC_LOG(DEBUG, TAG, "OUT-UpdateBlockOptionType");
    return CA_STATUS_FAILED;
}

uint8_t CAGetBlockOptionType(const CABlockDataID_t *blockID)
{
    OIC_LOG(DEBUG, TAG, "IN-GetBlockOptionType");
    VERIFY_NON_NULL_RET(blockID, TAG, "blockID", 0);

    ca_mutex_lock(g_context.blockDataListMutex);

    size_t len = u_arraylist_length(g_context.dataList);
    for (size_t i = 0; i < len; i++)
    {
        CABlockData_t *currData = (CABlockData_t *) u_arraylist_get(g_context.dataList, i);
        if (CABlockidMatches(currData, blockID))
        {
            ca_mutex_unlock(g_context.blockDataListMutex);
            OIC_LOG(DEBUG, TAG, "OUT-GetBlockOptionType");
            return currData->type;
        }
    }
    ca_mutex_unlock(g_context.blockDataListMutex);

    OIC_LOG(DEBUG, TAG, "OUT-GetBlockOptionType");
    return 0;
}

CAMessageInfo_t *CAGetDataSetFromBlockDataList(const CABlockDataID_t *blockID)
{
    VERIFY_NON_NULL_RET(blockID, TAG, "blockID", NULL);

    ca_mutex_lock(g_context.blockDataListMutex);

    size_t len = u_arraylist_length(g_context.dataList);
    for (size_t i = 0; i < len; i++)
    {
        CABlockData_t *currData = (CABlockData_t *) u_arraylist_get(g_context.dataList, i);
        if (CABlockidMatches(currData, blockID))
        {
            ca_mutex_unlock(g_context.blockDataListMutex);
            return currData->sentData;
        }
    }
    ca_mutex_unlock(g_context.blockDataListMutex);

    return NULL;
}

CAResult_t CAGetTokenFromBlockDataList(const coap_pdu_t *pdu, const CAEndpoint_t *endpoint,
                                       CAMessageInfo_t *responseInfo)
{
    ca_mutex_lock(g_context.blockDataListMutex);

    size_t len = u_arraylist_length(g_context.dataList);
    for (size_t i = 0; i < len; i++)
    {
        CABlockData_t *currData = (CABlockData_t *)u_arraylist_get(g_context.dataList, i);
        if (!currData || !currData->sentData)
        {
            continue;
        }

        CAMessageInfo_t *sentData = currData->sentData;
        if (CA_REQUEST_DATA != sentData->type)
        {
            continue;
        }

        if (pdu->hdr->id == sentData->info.messageId &&
                    endpoint->adapter == currData->sentData->endpoint.adapter)
        {
            responseInfo->info.token = sentData->info.token;
            break;
        }
    }

    ca_mutex_unlock(g_context.blockDataListMutex);

    return CA_STATUS_OK;
}

CAResult_t CACheckBlockDataValidation(const CAMessageInfo_t *sendData, CABlockData_t **blockData)
{
    ca_mutex_lock(g_context.blockDataListMutex);

    size_t len = u_arraylist_length(g_context.dataList);
    for (size_t i = 0; i < len; i++)
    {
        CABlockData_t *currData = (CABlockData_t *) u_arraylist_get(g_context.dataList, i);
        if (NULL == currData
                    || NULL == currData->blockDataId
                    || NULL == currData->blockDataId->id
                    || currData->blockDataId->idLength <= 0)
        {
            continue;
        }

        CABlockDataID_t* blockDataID = CACreateBlockDatablockId(
                        sendData->info.token.t.chars,
                        sendData->info.token.size,
                        sendData->endpoint.port);

        if (NULL == blockDataID || NULL == blockDataID->id || blockDataID->idLength < 1)
        {
            OIC_LOG(ERROR, TAG, "blockId is null");
            CADestroyBlockID(blockDataID);
            return CA_STATUS_FAILED;
        }

        if (CA_REQUEST_DATA == sendData->type)
        {
            OIC_LOG(DEBUG, TAG, "Send request");
            if (CABlockidMatches(currData, blockDataID))
            {
                OIC_LOG(ERROR, TAG, "already sent");
            }
        }
        else if (CA_RESPONSE_DATA == sendData->type)
        {
            OIC_LOG(DEBUG, TAG, "Send response");
            if (CABlockidMatches(currData, blockDataID))
            {
                // set sendData
                if (NULL != currData->sentData)
                {
                    OIC_LOG(DEBUG, TAG, "init block number");
                    CADestroyDataSet(currData->sentData);
                }
                currData->sentData = CACloneMessageInfo(sendData);
                *blockData = currData;
                CADestroyBlockID(blockDataID);
                ca_mutex_unlock(g_context.blockDataListMutex);
                return CA_STATUS_OK;
            }
        }
        CADestroyBlockID(blockDataID);
    }

    ca_mutex_unlock(g_context.blockDataListMutex);

    return CA_STATUS_FAILED;
}

CABlockData_t *CAGetBlockDataFromBlockDataList(const CABlockDataID_t *blockID)
{
    VERIFY_NON_NULL_RET(blockID, TAG, "blockID", NULL);

    ca_mutex_lock(g_context.blockDataListMutex);

    size_t len = u_arraylist_length(g_context.dataList);
    for (size_t i = 0; i < len; i++)
    {
        CABlockData_t *currData = (CABlockData_t *) u_arraylist_get(g_context.dataList, i);
        if (CABlockidMatches(currData, blockID))
        {
            ca_mutex_unlock(g_context.blockDataListMutex);
            return currData;
        }
    }
    ca_mutex_unlock(g_context.blockDataListMutex);

    return NULL;
}

coap_block_t *CAGetBlockOption(const CABlockDataID_t *blockID,
                               uint16_t blockType)
{
    OIC_LOG(DEBUG, TAG, "IN-GetBlockOption");
    VERIFY_NON_NULL_RET(blockID, TAG, "blockID", NULL);

    ca_mutex_lock(g_context.blockDataListMutex);

    size_t len = u_arraylist_length(g_context.dataList);
    for (size_t i = 0; i < len; i++)
    {
        CABlockData_t *currData = (CABlockData_t *) u_arraylist_get(g_context.dataList, i);
        if (CABlockidMatches(currData, blockID))
        {
            ca_mutex_unlock(g_context.blockDataListMutex);
            OIC_LOG(DEBUG, TAG, "OUT-GetBlockOption");
            if (COAP_OPTION_BLOCK2 == blockType)
            {
                return &currData->block2;
            }
            else
            {
                return &currData->block1;
            }
        }
    }
    ca_mutex_unlock(g_context.blockDataListMutex);

    OIC_LOG(DEBUG, TAG, "OUT-GetBlockOption");
    return NULL;
}

CAPayload_t CAGetPayloadFromBlockDataList(const CABlockDataID_t *blockID,
                                          size_t *fullPayloadLen)
{
    OIC_LOG(DEBUG, TAG, "IN-GetFullPayload");
    VERIFY_NON_NULL_RET(blockID, TAG, "blockID", NULL);
    VERIFY_NON_NULL_RET(fullPayloadLen, TAG, "fullPayloadLen", NULL);

    ca_mutex_lock(g_context.blockDataListMutex);

    size_t len = u_arraylist_length(g_context.dataList);
    for (size_t i = 0; i < len; i++)
    {
        CABlockData_t *currData = (CABlockData_t *) u_arraylist_get(g_context.dataList, i);
        if (CABlockidMatches(currData, blockID))
        {
            ca_mutex_unlock(g_context.blockDataListMutex);
            *fullPayloadLen = currData->receivedPayloadLen;
            OIC_LOG(DEBUG, TAG, "OUT-GetFullPayload");
            return currData->payload;
        }
    }
    ca_mutex_unlock(g_context.blockDataListMutex);

    OIC_LOG(DEBUG, TAG, "OUT-GetFullPayload");
    return NULL;
}

CABlockData_t *CACreateNewBlockData(const CAMessageInfo_t *sendData)
{
    OIC_LOG(DEBUG, TAG, "IN-CACreateNewBlockData");
    VERIFY_NON_NULL_RET(sendData, TAG, "sendData", NULL);

    // create block data
    CABlockData_t *data = (CABlockData_t *) OICCalloc(1, sizeof(CABlockData_t));
    if (NULL == data)
    {
        OIC_LOG(ERROR, TAG, "memory alloc has failed");
        return NULL;
    }

    data->block1.szx = CA_DEFAULT_BLOCK_SIZE;
    data->block2.szx = CA_DEFAULT_BLOCK_SIZE;
    data->sentData = CACloneMessageInfo(sendData);
    if(!data->sentData)
    {
        OIC_LOG(ERROR, TAG, PCF("memory alloc has failed"));
        OICFree(data);
        return NULL;
    }

    CABlockDataID_t* blockDataID = CACreateBlockDatablockId(
            data->sentData->info.token.t.chars,
            data->sentData->info.token.size,
            data->sentData->endpoint.port);
    if (NULL == blockDataID || NULL == blockDataID->id || blockDataID->idLength < 1)
    {
        OIC_LOG(ERROR, TAG, "blockId is null");
        CADestroyBlockID(blockDataID);
        return NULL;
    }
    data->blockDataId = blockDataID;

    ca_mutex_lock(g_context.blockDataListMutex);

    CAResult_t res = u_arraylist_add(g_context.dataList, (void *) data);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "add has failed");
        CADestroyBlockID(data->blockDataId);
        OICFree(data);
        ca_mutex_unlock(g_context.blockDataListMutex);
        return NULL;
    }
    ca_mutex_unlock(g_context.blockDataListMutex);

    OIC_LOG(DEBUG, TAG, "OUT-CreateBlockData");
    return data;
}

CAResult_t CARemoveBlockDataFromList(const CABlockDataID_t *blockID)
{
    OIC_LOG(DEBUG, TAG, "CARemoveBlockData");
    VERIFY_NON_NULL(blockID, TAG, "blockID");

    ca_mutex_lock(g_context.blockDataListMutex);

    size_t len = u_arraylist_length(g_context.dataList);
    for (size_t i = 0; i < len; i++)
    {
        CABlockData_t *currData = (CABlockData_t *) u_arraylist_get(g_context.dataList, i);
        if (CABlockidMatches(currData, blockID))
        {
            CABlockData_t *removedData = u_arraylist_remove(g_context.dataList, i);
            if (NULL == removedData)
            {
                OIC_LOG(ERROR, TAG, "data is NULL");
                ca_mutex_unlock(g_context.blockDataListMutex);
                return CA_STATUS_FAILED;
            }

            // destroy memory
            if (currData->sentData)
            {
                CADestroyDataSet(currData->sentData);
            }
            OICFree(currData->payload);
            CADestroyBlockID(currData->blockDataId);
            ca_mutex_unlock(g_context.blockDataListMutex);
            return CA_STATUS_OK;
        }
    }
    ca_mutex_unlock(g_context.blockDataListMutex);

    return CA_STATUS_OK;
}

bool CAIsBlockDataInList(const CABlockDataID_t *blockID)
{
    OIC_LOG(DEBUG, TAG, "IN-IsBlockDataInList");
    VERIFY_NON_NULL_RET(blockID, TAG, "blockID", false);

    ca_mutex_lock(g_context.blockDataListMutex);

    size_t len = u_arraylist_length(g_context.dataList);
    for (size_t i = 0; i < len; i++)
    {
        CABlockData_t *currData = (CABlockData_t *) u_arraylist_get(g_context.dataList, i);
        if (CABlockidMatches(currData, blockID))
        {
            OIC_LOG(DEBUG, TAG, "found block data");
            ca_mutex_unlock(g_context.blockDataListMutex);
            return true;
        }
    }
    ca_mutex_unlock(g_context.blockDataListMutex);

    OIC_LOG(DEBUG, TAG, "OUT-IsBlockDataInList");
    return false;
}

void CADestroyDataSet(CAMessageInfo_t* data)
{
    VERIFY_NON_NULL_VOID(data, TAG, "data");

    CADestroyMessageInfoInternal(data);
}

CABlockDataID_t* CACreateBlockDatablockId(const uint8_t *token, int tokenLength,
                                          uint16_t portNumber)
{
    VERIFY_NON_NULL_RET(token, TAG, "token", NULL);

    char port[PORT_LENGTH] = {0,};
    port[0] = (char)((portNumber>>8) & 0xFF);
    port[1] = (char)(portNumber & 0xFF);

    CABlockDataID_t* blockDataID = (CABlockDataID_t *) OICMalloc(sizeof(CABlockDataID_t));
    blockDataID->idLength = tokenLength + sizeof(port);
    blockDataID->id = (uint8_t *) OICMalloc(blockDataID->idLength);
    if (!blockDataID->id)
    {
        OIC_LOG(ERROR, TAG, "memory alloc has failed");
        OICFree(blockDataID);
        return NULL;
    }

    memcpy(blockDataID->id, token, tokenLength);
    memcpy(blockDataID->id + tokenLength, port, sizeof(port));

    OIC_LOG(DEBUG, TAG, "BlockID is ");
    OIC_LOG_BUFFER(DEBUG, TAG, (const uint8_t *)blockDataID->id, blockDataID->idLength);

    return blockDataID;
}

void CADestroyBlockID(CABlockDataID_t *blockID)
{
    VERIFY_NON_NULL_VOID(blockID, TAG, "blockID");
    OICFree(blockID->id);
    OICFree(blockID);
    blockID = NULL;
}

bool CABlockidMatches(const CABlockData_t *currData, const CABlockDataID_t *blockID)
{
    VERIFY_NON_NULL_RET(currData, TAG, "currData", false);
    VERIFY_NON_NULL_RET(blockID, TAG, "blockID", false);
    VERIFY_NON_NULL_RET(blockID->id, TAG, "blockID->id", false);

    if ((currData->blockDataId)
        && (currData->blockDataId->id)
        && (currData->blockDataId->idLength == blockID->idLength)
        && !memcmp(currData->blockDataId->id, blockID->id, currData->blockDataId->idLength))
    {
        return true;
    }
    return false;
}

void CALogBlockInfo(coap_block_t *block)
{
    VERIFY_NON_NULL_VOID(block, TAG, "block");

    OIC_LOG(DEBUG, TAG, "block option info");

    OIC_LOG_V(DEBUG, TAG, "block option-num : %d", block->num);

    OIC_LOG_V(DEBUG, TAG, "block option-m   : %d", block->m);

    OIC_LOG_V(DEBUG, TAG, "block option-szx : %d", block->szx);
}
