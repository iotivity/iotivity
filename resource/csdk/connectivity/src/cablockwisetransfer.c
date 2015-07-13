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

#define BLOCKWISE_OPTION_BUFFER    3
#define BLOCK_NUMBER_IDX           4
#define BLOCK_M_BIT_IDX            3

#define BLOCK_SIZE(arg) (1 << (arg + 4))

// context for block-wise transfer
static CABlockWiseContext_t g_context = { 0 };

static ca_mutex g_blockDataListMutex = NULL;

CAResult_t CAInitializeBlockWiseTransfer(CASendThreadFunc sendThreadFunc,
                                         CAReceiveThreadFunc receivedThreadFunc)
{
    OIC_LOG(DEBUG, TAG, "initialize");

    // set block-wise transfer context
    g_context.sendThreadFunc = sendThreadFunc;
    g_context.receivedThreadFunc = receivedThreadFunc;

    if (!g_context.dataList)
    {
        g_context.dataList = u_arraylist_create();
    }

    CAResult_t res = CAInitBlockWiseMutexVaraibles();
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "init has failed");
        return res;
    }

    return CA_STATUS_OK;
}

CAResult_t CATerminateBlockWiseTransfer()
{
    OIC_LOG(DEBUG, TAG, "terminate");

    if (g_context.dataList)
    {
        u_arraylist_free(&g_context.dataList);
    }

    CATerminateBlockWiseMutexVaraibles();

    return CA_STATUS_OK;
}

CAResult_t CAInitBlockWiseMutexVaraibles()
{
    OIC_LOG(DEBUG, TAG, "IN");

    if (NULL == g_blockDataListMutex)
    {
        g_blockDataListMutex = ca_mutex_new();
        if (NULL == g_blockDataListMutex)
        {
            OIC_LOG(ERROR, TAG, "ca_mutex_new has failed");
            return CA_STATUS_FAILED;
        }
    }

    return CA_STATUS_OK;
}

void CATerminateBlockWiseMutexVaraibles()
{
    OIC_LOG(DEBUG, TAG, "IN");

    ca_mutex_free(g_blockDataListMutex);
    g_blockDataListMutex = NULL;
}

CAResult_t CASendBlockWiseData(CAData_t *sendData)
{
    VERIFY_NON_NULL(sendData, TAG, "sendData");

    // check if message type is CA_MSG_RESET
    if (NULL != sendData->responseInfo)
    {
        if (CA_MSG_RESET == sendData->responseInfo->info.type)
        {
            OIC_LOG(DEBUG, TAG, "reset message can't be sent to the block");
            return CA_NOT_SUPPORTED;
        }
    }

    // #1. check if it is already included in block data list
    CABlockData_t *currData = NULL;
    CAResult_t res = CACheckBlockDataValidation(sendData, &currData);
    if (CA_STATUS_OK != res)
    {
        // #2. if it is not included, add the data into list
        if (NULL == currData)
        {
            OIC_LOG(DEBUG, TAG, "There is no block data");
            currData = CACreateNewBlockData(sendData);
        }
    }

    // #3. check request/response block option type and payload length
    res = CACheckBlockOptionType(currData);
    if (CA_STATUS_OK == res)
    {
        // #4. send block message
        OIC_LOG(DEBUG, TAG, "send first block msg");
        CALogBlockInfo(&currData->block);

        res = CAAddSendThreadQueue(currData->sentData);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "add has failed");
            return res;
        }
    }

    return res;
}

CAResult_t CAAddSendThreadQueue(CAData_t *sendData)
{
    CAData_t *cloneData = CACloneCAData(sendData);
    if (NULL == cloneData)
    {
        OIC_LOG(ERROR, TAG, "clone has failed");
        CADestroyDataSet(sendData);
        return CA_STATUS_FAILED;
    }

    if (g_context.sendThreadFunc)
    {
        g_context.sendThreadFunc(cloneData);
    }
    return CA_STATUS_OK;
}

CAResult_t CACheckBlockOptionType(CABlockData_t *currData)
{
    VERIFY_NON_NULL(currData, TAG, "currData");
    VERIFY_NON_NULL(currData->sentData, TAG, "currData->sentData");

    uint32_t payloadLen = 0;
    CAGetPayloadInfo(currData->sentData, &payloadLen);

    // check if message has to be transfered to a block
    uint32_t maxBlockSize = BLOCK_SIZE(CA_DEFAULT_BLOCK_SIZE);
    if (payloadLen <= maxBlockSize)
    {
        OIC_LOG_V(DEBUG, TAG, "payloadLen=%d, maxBlockSize=%d", payloadLen, maxBlockSize);
        return CA_NOT_SUPPORTED;
    }

    // check if next block are required to transfer
    CAGetMoreBitFromBlock(payloadLen, &currData->block);

    // set block option (COAP_OPTION_BLOCK2 or COAP_OPTION_BLOCK1)
    if (NULL != currData->sentData->requestInfo) // request message
    {
        if (currData->block.m)
        {
            OIC_LOG(DEBUG, TAG, "no ACK, option1");
            currData->type = COAP_OPTION_BLOCK1;
        }
        else
        {
            OIC_LOG(DEBUG, TAG, "no ACK, normal req");
        }
    }
    else // response message
    {
        if (currData->block.m)
        {
            OIC_LOG(DEBUG, TAG, "ACK, option2");
            currData->type = COAP_OPTION_BLOCK2;
        }
        else
        {
            OIC_LOG(DEBUG, TAG, "ACK, normal res");
        }
    }

    return CA_STATUS_OK;
}

CAResult_t CAReceiveBlockWiseData(const coap_pdu_t *pdu, const CAEndpoint_t *endpoint,
                                  CAData_t *receivedData, uint32_t dataLen)
{
    OIC_LOG(DEBUG, TAG, "CAReceiveBlockWiseData");
    VERIFY_NON_NULL(pdu, TAG, "pdu");
    VERIFY_NON_NULL(endpoint, TAG, "endpoint");
    VERIFY_NON_NULL(receivedData, TAG, "receivedData");

    // check if received message type is CA_MSG_RESET
    if (CA_EMPTY == pdu->hdr->code)
    {
        OIC_LOG(DEBUG, TAG, "code is CA_EMPTY..");

        // get token from block-wise transfer list when CA_EMPTY(RST/ACK) is received
        CAResult_t res = CAGetTokenFromBlockDataList(pdu, endpoint, receivedData->responseInfo);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "fail to get token");
            return res;
        }

        CARemoveBlockDataFromList((const unsigned char*) receivedData->responseInfo->info.token);
        return CA_NOT_SUPPORTED;
    }

    // check if block option is set and get block data
    coap_block_t block;
    if (coap_get_block((coap_pdu_t *) pdu, COAP_OPTION_BLOCK2, &block))
    {
        CAResult_t res = CASetNextBlockOption2(pdu, endpoint, receivedData, block, dataLen);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "setting has failed");
            return res;
        }
    }
    else if (coap_get_block((coap_pdu_t *) pdu, COAP_OPTION_BLOCK1, &block)) // block1 option
    {
        CAResult_t res = CASetNextBlockOption1(pdu, endpoint, receivedData, block, dataLen);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "setting has failed");
            return res;
        }
    }
    else
    {
        uint32_t code = (uint32_t) CA_RESPONSE_CODE(pdu->hdr->code);
        if (CA_REQUEST_ENTITY_INCOMPLETE == code)
        {
            CABlockData_t *data = CAGetBlockDataFromBlockDataList(pdu->hdr->token);
            if (NULL == data)
            {
                OIC_LOG(ERROR, TAG, "getting has failed");
                return CA_STATUS_FAILED;
            }

            coap_block_t *block = CAGetBlockOption(pdu->hdr->token);
            if (NULL == block)
            {
                OIC_LOG(ERROR, TAG, "block is null");
                return CA_STATUS_FAILED;
            }

            if (COAP_OPTION_BLOCK2 == data->type)
            {
                CAResult_t res = CASetNextBlockOption2(pdu, endpoint, receivedData, *block,
                                                       dataLen);
                if (CA_STATUS_OK != res)
                {
                    OIC_LOG(ERROR, TAG, "setting has failed");
                    return res;
                }
            }
            else if (COAP_OPTION_BLOCK1 == data->type)
            {
                CAResult_t res = CASetNextBlockOption1(pdu, endpoint, receivedData, *block,
                                                       dataLen);
                if (CA_STATUS_OK != res)
                {
                    OIC_LOG(ERROR, TAG, "setting has failed");
                    return res;
                }
            }
        }
        else
        {
            // normal pdu data
            OIC_LOG(DEBUG, TAG, "it's normal pdu");
            return CA_NOT_SUPPORTED;
        }
    }

    return CA_STATUS_OK;
}

CAResult_t CAProcessNextStep(const coap_pdu_t *pdu, CAData_t *receivedData,
                             uint8_t blockWiseStatus)
{
    VERIFY_NON_NULL(pdu, TAG, "pdu");
    VERIFY_NON_NULL(pdu->hdr, TAG, "pdu->hdr");

    CAResult_t res = CA_STATUS_OK;
    CAData_t *data = NULL;

    // process blockWiseStatus
    switch (blockWiseStatus)
    {
        case CA_OPTION2_FIRST_BLOCK:
            res = CAAddSendThreadQueue(receivedData);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG(ERROR, TAG, "add has failed");
                return res;
            }
            break;

        case CA_OPTION2_CON:
            // add data to send thread
            data = CAGetDataSetFromBlockDataList(pdu->hdr->token);
            if (NULL == data)
            {
                OIC_LOG(ERROR, TAG, "it's unavailable");
                return CA_STATUS_FAILED;
            }

            if (data->requestInfo)
            {
                data->requestInfo->info.messageId = pdu->hdr->id;
            }

            if (data->responseInfo)
            {
                data->responseInfo->info.messageId = pdu->hdr->id;
            }

            res = CAAddSendThreadQueue(data);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG(ERROR, TAG, "add has failed");
                return res;
            }

            break;

        case CA_OPTION1_ACK:
        case CA_OPTION2_ACK:
        case CA_SENT_PREVIOUS_NON_MSG:
            res = CASendBlockMessage(pdu, CA_MSG_CONFIRM, blockWiseStatus);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG(ERROR, TAG, "send has failed");
                return res;
            }
            break;

        case CA_OPTION2_LAST_BLOCK:
            // process last block and send upper layer
            res = CAReceiveLastBlock(pdu, receivedData);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG(ERROR, TAG, "receive has failed");
                return res;
            }

            // remove data from list
            res = CARemoveBlockDataFromList(pdu->hdr->token);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG(ERROR, TAG, "remove has failed");
                return res;
            }
            break;

        case CA_OPTION1_NO_ACK_LAST_BLOCK:
            // process last block and send upper layer
            res = CAReceiveLastBlock(pdu, receivedData);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG(ERROR, TAG, "receive has failed");
                return res;
            }

            if (CA_MSG_CONFIRM == pdu->hdr->type)
            {
                // send ack message to remote device
                res = CASendBlockMessage(pdu, CA_MSG_ACKNOWLEDGE, blockWiseStatus);
                if (CA_STATUS_OK != res)
                {
                    OIC_LOG(ERROR, TAG, "send has failed");
                    return res;
                }
            }
            break;

        case CA_OPTION1_NO_ACK_BLOCK:
            if (CA_MSG_CONFIRM == pdu->hdr->type)
            {
                // add data to send thread
                res = CASendBlockMessage(pdu, CA_MSG_ACKNOWLEDGE, blockWiseStatus);
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
                res = CASendErrorMessage(pdu, blockWiseStatus, CA_REQUEST_ENTITY_INCOMPLETE);
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
                res = CASendBlockMessage(pdu, CA_MSG_CONFIRM, blockWiseStatus);
                if (CA_STATUS_OK != res)
                {
                    OIC_LOG(ERROR, TAG, "send has failed");
                    return res;
                }
            }
            else if (CA_MSG_CONFIRM == pdu->hdr->type)
            {
                res = CASendErrorMessage(pdu, blockWiseStatus, CA_REQUEST_ENTITY_TOO_LARGE);
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

CAResult_t CASendBlockMessage(const coap_pdu_t *pdu, CAMessageType_t msgType, uint8_t status)
{
    VERIFY_NON_NULL(pdu, TAG, "pdu");
    VERIFY_NON_NULL(pdu->hdr, TAG, "pdu->hdr");

    CAData_t *data = CAGetDataSetFromBlockDataList(pdu->hdr->token);
    if (NULL == data)
    {
        OIC_LOG(ERROR, TAG, "CAData is unavailable");
        return CA_STATUS_FAILED;
    }

    if (CA_MSG_CONFIRM == msgType)
    {
        OIC_LOG(DEBUG, TAG, "need new msgID");
        if (data->requestInfo)
        {
            data->requestInfo->info.messageId = 0;
        }

        if (data->responseInfo)
        {
            data->responseInfo->info.messageId = 0;
        }
    }
    else if (CA_MSG_ACKNOWLEDGE == msgType)
    {
        if (data->responseInfo)
        {
            OIC_LOG(DEBUG, TAG, "set ACK message");
            data->responseInfo->info.messageId = pdu->hdr->id;
            data->responseInfo->info.type = CA_MSG_ACKNOWLEDGE;
            if (CA_OPTION1_NO_ACK_LAST_BLOCK == status)
            {
                data->responseInfo->result = CA_CHANGED;
            }
            else if (CA_OPTION1_NO_ACK_BLOCK == status)
            {
                data->responseInfo->result = CA_CONTINUE;
            }
        }
    }

    // add data to send thread
    CAResult_t res = CAAddSendThreadQueue(data);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "add has failed");
        return res;
    }

    return CA_STATUS_OK;
}

CAResult_t CASendErrorMessage(const coap_pdu_t *pdu, uint8_t status,
                              CAResponseResult_t responseResult)
{
    VERIFY_NON_NULL(pdu, TAG, "pdu");
    VERIFY_NON_NULL(pdu->hdr, TAG, "pdu->hdr");

    // create error responseInfo
    CABlockData_t *data = CAGetBlockDataFromBlockDataList(pdu->hdr->token);
    if (NULL == data)
    {
        OIC_LOG(ERROR, TAG, "data is unavailable");
        return CA_STATUS_FAILED;
    }

    CAData_t *cloneData = NULL;
    if (data->sentData->responseInfo)
    {
        data->sentData->responseInfo->info.messageId = pdu->hdr->id;
        data->sentData->responseInfo->info.type = CA_MSG_ACKNOWLEDGE;
        data->sentData->responseInfo->result = responseResult;
        cloneData = CACloneCAData(data->sentData);
        if (NULL == cloneData)
        {
            OIC_LOG(ERROR, TAG, "clone has failed");
            return CA_MEMORY_ALLOC_FAILED;
        }
        OIC_LOG(DEBUG, TAG, "set ACK message");
    }
    else
    {
        cloneData = CACreateNewDataSet(pdu, CACloneEndpoint(data->sentData->remoteEndpoint));
        cloneData->responseInfo->info.type = CA_MSG_CONFIRM;
        cloneData->responseInfo->result = responseResult;
        OIC_LOG(DEBUG, TAG, "set CON message");
    }

    // add data to send thread
    if (g_context.sendThreadFunc)
    {
        g_context.sendThreadFunc(cloneData);
    }

    // if error code is 4.08, remove the stored payload and initialize block number
    if (CA_BLOCK_INCOMPLETE == status)
    {
        OICFree(data->payload);
        data->payload = NULL;
        data->payloadLength = 0;
        data->block.num = 0;
    }

    return CA_STATUS_OK;
}

CAResult_t CAReceiveLastBlock(const coap_pdu_t *pdu, CAData_t *receivedData)
{
    VERIFY_NON_NULL(pdu, TAG, "pdu");
    VERIFY_NON_NULL(pdu->hdr, TAG, "pdu->hdr");
    VERIFY_NON_NULL(receivedData, TAG, "receivedData");

    // total block data have to notify to Application
    CAData_t *cloneData = CACloneCAData(receivedData);
    if (NULL == cloneData)
    {
        OIC_LOG(ERROR, TAG, "clone has failed");
        CADestroyDataSet(receivedData);
        return CA_MEMORY_ALLOC_FAILED;
    }

    // update payload
    uint32_t fullPayloadLen = 0;
    CAPayload_t fullPayload = CAGetPayloadFromBlockDataList(pdu->hdr->token, &fullPayloadLen);
    if (NULL != fullPayload)
    {
        CAResult_t res = CAUpdatePayloadToCAData(cloneData, fullPayload, fullPayloadLen);
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

    return CA_STATUS_OK;
}

CAResult_t CASetNextBlockOption1(const coap_pdu_t *pdu, const CAEndpoint_t *endpoint,
                                 CAData_t *receivedData, coap_block_t block, uint32_t dataLen)
{
    OIC_LOG(INFO, TAG, "CASetNextBlockOption1");
    VERIFY_NON_NULL(pdu, TAG, "pdu");
    VERIFY_NON_NULL(pdu->hdr, TAG, "pdu->hdr");
    VERIFY_NON_NULL(endpoint, TAG, "endpoint");
    VERIFY_NON_NULL(receivedData, TAG, "receivedData");

    OIC_LOG_V(INFO, TAG, "num:%d, M:%d, sze:%d", block.num, block.m, block.szx);

    // BlockData data is created if it not existed
    if (!CAIsBlockDataInList(pdu))
    {
        OIC_LOG(DEBUG, TAG, "no message in list");

        CAData_t *data = CACreateNewDataSet(pdu, endpoint);
        if (NULL == data)
        {
            OIC_LOG(ERROR, TAG, "data is null");
            return CA_STATUS_FAILED;
        }

        CABlockData_t *currData = CACreateNewBlockData(data);
        if (NULL == currData)
        {
            OIC_LOG(ERROR, TAG, "currData is null");
            CADestroyDataSet(data);
            return CA_STATUS_FAILED;
        }
    }

    // update BLOCK OPTION1 type
    CAResult_t res = CAUpdateBlockOptionType(pdu->hdr->token, COAP_OPTION_BLOCK1);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "update has failed");
        CARemoveBlockDataFromList(pdu->hdr->token);
        return res;
    }

    CABlockData_t *data = CAGetBlockDataFromBlockDataList(pdu->hdr->token);
    if (NULL == data)
    {
        OIC_LOG(ERROR, TAG, "getting has failed");
        return CA_STATUS_FAILED;
    }

    uint8_t blockWiseStatus = CA_BLOCK_UNKNOWN;
    // received type from remote device
    if (CA_MSG_ACKNOWLEDGE == pdu->hdr->type)
    {
        uint32_t code = (uint32_t) CA_RESPONSE_CODE(pdu->hdr->code);
        if (0 == block.m &&
                (CA_REQUEST_ENTITY_INCOMPLETE != code && CA_REQUEST_ENTITY_TOO_LARGE != code))
        {
            OIC_LOG(INFO, TAG, "Data has sent");
            // initialize block number for response message
            data->block.num = 0;
            return CA_STATUS_OK;
        }

        blockWiseStatus = CA_OPTION1_ACK;
        res = CAUpdateBlockOptionItems(data, pdu, &block, COAP_OPTION_BLOCK1, blockWiseStatus);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "update has failed");
            return res;
        }

        res = CAUpdateBlockData(data, block);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "update has failed");
            CARemoveBlockDataFromList(pdu->hdr->token);
            return res;
        }
    }
    else // CON or NON message
    {
        OIC_LOG_V(INFO, TAG, "num:%d, M:%d", block.num, block.m);

        // check the size option
        bool isSizeOption = CAIsPayloadLengthInPduWithBlockSizeOption((coap_pdu_t *) pdu,
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
            res = CAUpdatePayloadData(data, receivedData, blockWiseStatus, isSizeOption);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG(ERROR, TAG, "update has failed");
                CARemoveBlockDataFromList(pdu->hdr->token);
                return res;
            }

            res = CAUpdateBlockOptionItems(data, pdu, &block, COAP_OPTION_BLOCK1, blockWiseStatus);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG(ERROR, TAG, "update has failed");
                return res;
            }

            // update block data
            res = CAUpdateBlockData(data, block);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG(ERROR, TAG, "update has failed");
                CARemoveBlockDataFromList(pdu->hdr->token);
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

    res = CAProcessNextStep(pdu, receivedData, blockWiseStatus);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "setting has failed");
        return res;
    }

    CADestroyDataSet(receivedData);

    return CA_STATUS_OK;
}

CAResult_t CASetNextBlockOption2(const coap_pdu_t *pdu, const CAEndpoint_t *endpoint,
                                 CAData_t *receivedData, coap_block_t block, uint32_t dataLen)
{
    OIC_LOG(DEBUG, TAG, "CASetNextBlockOption2");
    OIC_LOG_V(INFO, TAG, "num:%d, M:%d, sze:%d", block.num, block.m, block.szx);

    VERIFY_NON_NULL(pdu, TAG, "pdu");
    VERIFY_NON_NULL(pdu->hdr, TAG, "pdu->hdr");
    VERIFY_NON_NULL(receivedData, TAG, "receivedData");

    // BlockData data is created if it not existed
    if (!CAIsBlockDataInList(pdu))
    {
        OIC_LOG(DEBUG, TAG, "no msg in list.");

        CAData_t *data = CACreateNewDataSet(pdu, endpoint);
        if (NULL == data)
        {
            OIC_LOG(ERROR, TAG, "data is null");
            return CA_STATUS_FAILED;
        }

        CABlockData_t *currData = CACreateNewBlockData(data);
        if (NULL == currData)
        {
            OIC_LOG(ERROR, TAG, "data is null");
            CADestroyDataSet(data);
            return CA_STATUS_FAILED;
        }
    }

    // set Block Option Type
    CAResult_t res = CAUpdateBlockOptionType(pdu->hdr->token, COAP_OPTION_BLOCK2);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "update has failed");
        CARemoveBlockDataFromList(pdu->hdr->token);
        return res;
    }

    CABlockData_t *data = CAGetBlockDataFromBlockDataList(pdu->hdr->token);
    if (NULL == data)
    {
        OIC_LOG(ERROR, TAG, "getting has failed");
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
            return res;
        }

        // first block data have to notify to Application
        res = CAUpdateBlockData(data, block);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "update has failed");
            CARemoveBlockDataFromList(pdu->hdr->token);
            return res;
        }
        blockWiseStatus = CA_OPTION2_FIRST_BLOCK;
    }
    else
    {
        // received type from remote device
        if (CA_MSG_ACKNOWLEDGE == pdu->hdr->type ||
                (CA_MSG_NONCONFIRM == pdu->hdr->type && NULL != receivedData->responseInfo))
        {
            OIC_LOG(DEBUG, TAG, "received ACK or NON");

            // check the size option
            bool isSizeOption = CAIsPayloadLengthInPduWithBlockSizeOption((coap_pdu_t *) pdu,
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
                res = CAUpdatePayloadData(data, receivedData, blockWiseStatus, isSizeOption);
                if (CA_STATUS_OK != res)
                {
                    OIC_LOG(ERROR, TAG, "update has failed");
                    CARemoveBlockDataFromList(pdu->hdr->token);
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
                    CARemoveBlockDataFromList(pdu->hdr->token);
                    return res;
                }

                // first block data have to notify to Application
                res = CAUpdateBlockData(data, block);
                if (CA_STATUS_OK != res)
                {
                    OIC_LOG(ERROR, TAG, "update has failed");
                    CARemoveBlockDataFromList(pdu->hdr->token);
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
                return res;
            }

            res = CAUpdateBlockData(data, block);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG(ERROR, TAG, "update has failed");
                CARemoveBlockDataFromList(pdu->hdr->token);
                return res;
            }
        }
    }

    res = CAProcessNextStep(pdu, receivedData, blockWiseStatus);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "setting has failed");
        return res;
    }

    CADestroyDataSet(receivedData);

    return CA_STATUS_OK;
}

CAResult_t CAUpdateBlockOptionItems(CABlockData_t *currData, const coap_pdu_t *pdu,
                                    coap_block_t *block, unsigned short blockType,
                                    uint32_t status)
{
    VERIFY_NON_NULL(currData, TAG, "currData");
    VERIFY_NON_NULL(pdu, TAG, "pdu");
    VERIFY_NON_NULL(block, TAG, "block");

    // update block data
    CAResult_t res = CA_STATUS_OK;
    uint32_t code = (uint32_t) CA_RESPONSE_CODE(pdu->hdr->code);
    switch (code)
    {
        // response error code of the received block message
        case CA_REQUEST_ENTITY_INCOMPLETE:
        case CA_REQUEST_ENTITY_TOO_LARGE:
            res = CAHandleBlockErrorResponse(block, blockType, code);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG(ERROR, TAG, "error handle has failed");
                return res;
            }
            break;

        default:
            // update block option items
            switch (status)
            {
                case CA_OPTION1_ACK:
                    if (currData->block.num > block->num)
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
                    if (currData->block.num > block->num)
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
                    }
                    block->szx = currData->block.szx;
                    break;
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

CAResult_t CAGetMoreBitFromBlock(uint32_t payloadLen, coap_block_t *block)
{
    VERIFY_NON_NULL(block, TAG, "block");

    if (((block->num + 1) << (block->szx + 4)) < payloadLen)
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
                                CAMessageType_t msgType, unsigned short blockType)
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
            if (block->szx > currData->block.szx)
            {
                OIC_LOG(DEBUG, TAG, "sze is big");

                // #3. calculate new block number from block size
                unsigned int blockNum = BLOCK_SIZE(block->szx) /
                                        BLOCK_SIZE(currData->block.szx) - 1;
                OIC_LOG(DEBUG, TAG, "num is set as Negotiation");
                block->num += blockNum;
                block->szx = currData->block.szx;
                OIC_LOG_V(DEBUG, TAG, "updated block num: %d", block->num);
            }
        }
        else
        {
            if (block->szx > currData->block.szx)
            {
                OIC_LOG(DEBUG, TAG, "sze is big");
                block->szx = currData->block.szx;
            }
        }
    }
    else if (COAP_OPTION_BLOCK1 == blockType)
    {
        if (CA_MSG_ACKNOWLEDGE == msgType)
        {
            if (block->szx < currData->block.szx)
            {
                OIC_LOG(DEBUG, TAG, "sze is small");

                unsigned int blockNum = BLOCK_SIZE(currData->block.szx) /
                                        BLOCK_SIZE(block->szx) - 1;
                block->num += blockNum;
                OIC_LOG_V(DEBUG, TAG, "updated block num: %d", block->num);
            }
        }
        else
        {
            if (block->szx > currData->block.szx)
            {
                OIC_LOG(DEBUG, TAG, "sze is big");
                block->szx = currData->block.szx;
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

CAResult_t CAUpdateBlockData(CABlockData_t *currData, coap_block_t block)
{
    VERIFY_NON_NULL(currData, TAG, "currData");

    // check if block size is bigger than CABlockSize_t
    if (block.szx > CA_BLOCK_SIZE_1_KBYTE)
    {
        OIC_LOG(DEBUG, TAG, "invalid block szx");
        return CA_STATUS_FAILED;
    }

    // update block option
    currData->block = block;

    OIC_LOG(DEBUG, TAG, "data has updated");
    return CA_STATUS_OK;
}

CAResult_t CAUpdateMessageId(coap_pdu_t *pdu)
{
    VERIFY_NON_NULL(pdu, TAG, "pdu");

    // if CON message is sent, update messageId in block-wise transfer list
    if (CA_MSG_CONFIRM == pdu->hdr->type)
    {
        CAData_t * cadata = CAGetDataSetFromBlockDataList(pdu->hdr->token);
        if (NULL == cadata)
        {
            OIC_LOG(ERROR, TAG, "CAData is unavailable");
            return CA_STATUS_FAILED;
        }

        if (NULL != cadata->requestInfo)
        {
            cadata->requestInfo->info.messageId = pdu->hdr->id;
        }
    }

    return CA_STATUS_OK;
}

CAResult_t CAAddBlockOption(coap_pdu_t **pdu, CAInfo_t info)
{
    OIC_LOG(DEBUG, TAG, "IN-AddBlockOption");
    VERIFY_NON_NULL(pdu, TAG, "pdu");
    VERIFY_NON_NULL((*pdu), TAG, "(*pdu)");
    VERIFY_NON_NULL((*pdu)->hdr, TAG, "(*pdu)->hdr");

    uint32_t dataLength = 0;
    if (info.payload)
    {
        dataLength = info.payloadSize;
        OIC_LOG_V(DEBUG, TAG, "dataLength - %d", dataLength);
    }

    OIC_LOG_V(DEBUG, TAG, "previous payload - %s", (*pdu)->data);

    if (!(*pdu))
    {
        OIC_LOG(ERROR, TAG, "pdu data is null");
        return CA_STATUS_FAILED;
    }

    uint32_t code = (uint32_t) CA_RESPONSE_CODE((*pdu)->hdr->code);
    if (CA_REQUEST_ENTITY_INCOMPLETE == code)
    {
        OIC_LOG(INFO, TAG, "don't use option");
        return CA_STATUS_OK;
    }

    uint8_t blockType = CAGetBlockOptionType((*pdu)->hdr->token);
    if (COAP_OPTION_BLOCK2 == blockType)
    {
        CAResult_t res = CAAddBlockOption2(pdu, info, dataLength);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "add has failed");
            return res;
        }
    }
    else if (COAP_OPTION_BLOCK1 == blockType)
    {
        CAResult_t res = CAAddBlockOption1(pdu, info, dataLength);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "add has failed");
            return res;
        }
    }
    else
    {
        OIC_LOG(DEBUG, TAG, "no BLOCK option");
        // if response data is so large. it have to send as block transfer
        if (!coap_add_data(*pdu, dataLength, (const unsigned char *) info.payload))
        {
            OIC_LOG(INFO, TAG, "it have to use block");
        }
        else
        {
            OIC_LOG(INFO, TAG, "not Blockwise Transfer");
        }
    }

    CAResult_t res = CAUpdateMessageId(*pdu);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "fail to update CON message id ");
        return res;
    }

    OIC_LOG(DEBUG, TAG, "OUT-AddBlockOption");
    return CA_STATUS_OK;
}

CAResult_t CAAddBlockOption2(coap_pdu_t **pdu, CAInfo_t info, uint32_t dataLength)
{
    OIC_LOG(DEBUG, TAG, "IN-AddBlockOption2");
    VERIFY_NON_NULL(pdu, TAG, "pdu");
    VERIFY_NON_NULL((*pdu)->hdr, TAG, "(*pdu)->hdr");

    // get set block data from CABlock list-set.
    coap_block_t *block = CAGetBlockOption((*pdu)->hdr->token);
    if (NULL == block)
    {
        OIC_LOG(ERROR, TAG, "getting has failed");
        return CA_STATUS_FAILED;
    }

    CALogBlockInfo(block);

    uint8_t code;
    if (CA_MSG_ACKNOWLEDGE == (*pdu)->hdr->type ||
            (CA_MSG_NONCONFIRM == (*pdu)->hdr->type && CA_GET != (*pdu)->hdr->code))
    {
        uint32_t res = coap_write_block_opt(block, COAP_OPTION_BLOCK2, *pdu, dataLength);
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
        CALogBlockInfo(block);

        // if block number is 0, add size2 option
        if (0 == block->num)
        {
            res = CAAddBlockSizeOption(*pdu, COAP_OPTION_SIZE2, dataLength);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG(ERROR, TAG, "add has failed");
                CARemoveBlockDataFromList((*pdu)->hdr->token);
                return res;
            }
        }

        coap_add_block(*pdu, dataLength, (const unsigned char *) info.payload, block->num,
                       block->szx);

        if (CA_MSG_NONCONFIRM == (*pdu)->hdr->type)
        {
            if (block->m)
            {
                OIC_LOG(DEBUG, TAG, "NON, send next block..");
                // update block data
                block->num++;
                CAProcessNextStep(*pdu, NULL, CA_SENT_PREVIOUS_NON_MSG);
            }
            else
            {
                CARemoveBlockDataFromList((*pdu)->hdr->token);
            }
        }
    }
    else
    {
        OIC_LOG(DEBUG, TAG, "option2, not ACK msg");
        CAResult_t res = CAAddBlockOptionImpl(*pdu, block, COAP_OPTION_BLOCK2);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "add has failed");
            CARemoveBlockDataFromList((*pdu)->hdr->token);
            return res;
        }
    }

    return CA_STATUS_OK;

error:
    OIC_LOG_V(ERROR, TAG, "error : %d", code);
    coap_add_data(*pdu, strlen(coap_response_phrase(code)),
                  (unsigned char *) coap_response_phrase(code));
    return CA_STATUS_FAILED;
}

CAResult_t CAAddBlockOption1(coap_pdu_t **pdu, CAInfo_t info, uint32_t dataLength)
{
    OIC_LOG(DEBUG, TAG, "IN-AddBlockOption1");
    VERIFY_NON_NULL(pdu, TAG, "pdu");
    VERIFY_NON_NULL((*pdu)->hdr, TAG, "(*pdu)->hdr");

    // get set block data from CABlock list-set.
    coap_block_t *block = CAGetBlockOption((*pdu)->hdr->token);
    if (NULL == block)
    {
        OIC_LOG(ERROR, TAG, "getting has failed");
        return CA_STATUS_FAILED;
    }

    CALogBlockInfo(block);

    if (CA_MSG_ACKNOWLEDGE == (*pdu)->hdr->type)
    {
        OIC_LOG(DEBUG, TAG, "option1 and ACK msg..");
        CAResult_t res = CAAddBlockOptionImpl(*pdu, block, COAP_OPTION_BLOCK1);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "add has failed");
            CARemoveBlockDataFromList((*pdu)->hdr->token);
            return res;
        }

        // reset block-list after write block
        if (0 == block->m)
        {
            // remove data from list
            CAResult_t res = CARemoveBlockDataFromList((*pdu)->hdr->token);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG(ERROR, TAG, "remove has failed");
                return res;
            }
        }
    }
    else
    {
        CAGetMoreBitFromBlock(dataLength, block);
        CAResult_t res = CAAddBlockOptionImpl(*pdu, block, COAP_OPTION_BLOCK1);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "add has failed");
            CARemoveBlockDataFromList((*pdu)->hdr->token);
            return res;
        }
        CALogBlockInfo(block);

        // if block number is 0, add size1 option
        if (0 == block->num)
        {
            res = CAAddBlockSizeOption(*pdu, COAP_OPTION_SIZE1, dataLength);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG(ERROR, TAG, "add has failed");
                CARemoveBlockDataFromList((*pdu)->hdr->token);
                return res;
            }
        }

        coap_add_block(*pdu, dataLength, (const unsigned char *) info.payload, block->num,
                       block->szx);

        // check the message type and if message type is NON, next block message will be sent
        if (CA_MSG_NONCONFIRM == (*pdu)->hdr->type)
        {
            if (block->m)
            {
                OIC_LOG(DEBUG, TAG, "NON, send next block..");
                // update block data
                block->num++;
                CAProcessNextStep(*pdu, NULL, CA_SENT_PREVIOUS_NON_MSG);
            }
            else
            {
                CARemoveBlockDataFromList((*pdu)->hdr->token);
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

CAResult_t CAAddBlockSizeOption(coap_pdu_t *pdu, unsigned short sizeType, uint32_t dataLength)
{
    OIC_LOG(DEBUG, TAG, "IN-CAAddBlockSizeOption");
    VERIFY_NON_NULL(pdu, TAG, "pdu");

    if (sizeType != COAP_OPTION_SIZE1 && sizeType != COAP_OPTION_SIZE2)
    {
        OIC_LOG(ERROR, TAG, "unknown option type");
        return CA_STATUS_FAILED;
    }

    unsigned char value[BLOCKWISE_OPTION_BUFFER] = { 0 };
    coap_option *option = (coap_option *) OICMalloc(sizeof(coap_option));
    if (NULL == option)
    {
        OIC_LOG(ERROR, TAG, "out of memory");
        return CA_MEMORY_ALLOC_FAILED;
    }

    option->key = sizeType;
    option->length = coap_encode_var_bytes(value, dataLength);

    if (!coap_add_option(pdu, option->key, option->length, value))
    {
        OIC_LOG(ERROR, TAG, "failed to add size option");
        OICFree(option);
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TAG, "OUT-CAAddBlockSizeOption");

    return CA_STATUS_OK;
}

bool CAIsPayloadLengthInPduWithBlockSizeOption(const coap_pdu_t *pdu,
                                               unsigned short sizeType,
                                               uint32_t *totalPayloadLen)
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
    coap_opt_t *option = coap_check_option((coap_pdu_t *) pdu, sizeType, &opt_iter);
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
                              CAData_t *receivedData, unsigned short blockType, uint32_t dataLen)
{
    OIC_LOG(DEBUG, TAG, "IN-CheckBlockError");

    VERIFY_NON_NULL(currData, TAG, "currData is NULL");
    VERIFY_NON_NULL(receivedBlock, TAG, "receivedBlock is NULL");
    VERIFY_NON_NULL(receivedData, TAG, "receivedData is NULL");

    // #1. check the received payload length
    uint32_t blockPayloadLen = 0;
    CAGetPayloadInfo(receivedData, &blockPayloadLen);

    // #2. check if the block sequence is right
    if (COAP_OPTION_BLOCK1 == blockType)
    {
        uint32_t prePayloadLen = currData->receivedPayloadLen;
        if (prePayloadLen != BLOCK_SIZE(receivedBlock->szx) * receivedBlock->num)
        {
            if (receivedBlock->num > currData->block.num + 1)
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
        if (receivedBlock->num != currData->block.num)
        {
            if (receivedBlock->num > currData->block.num)
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
    uint32_t optionLen = dataLen - blockPayloadLen;
    if (receivedBlock->m && blockPayloadLen != BLOCK_SIZE(receivedBlock->szx))
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
                    currData->block.szx = size;
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
        uint32_t receivedPayloadLen = currData->receivedPayloadLen;
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

CAResult_t CAUpdatePayloadData(CABlockData_t *currData, CAData_t *receivedData,
                               uint8_t status, bool isSizeOption)
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

    uint32_t blockPayloadLen = 0;
    CAPayload_t blockPayload = CAGetPayloadInfo(receivedData, &blockPayloadLen);

    if (CA_BLOCK_TOO_LARGE == status)
    {
        blockPayloadLen = BLOCK_SIZE(currData->block.szx);
    }

    // memory allocation for the received block payload
    uint32_t prePayloadLen = currData->receivedPayloadLen;
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
            void *newPayload = realloc(currData->payload, totalPayloadLen);
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

    OIC_LOG(DEBUG, TAG, "OUT-UpdatePayloadData");
    return CA_STATUS_OK;
}

CAData_t* CACreateNewDataSet(const coap_pdu_t *pdu, const CAEndpoint_t *endpoint)
{
    VERIFY_NON_NULL_RET(pdu, TAG, "pdu", NULL);
    VERIFY_NON_NULL_RET(pdu->hdr, TAG, "pdu->hdr", NULL);
    VERIFY_NON_NULL_RET(endpoint, TAG, "endpoint", NULL);

    CAInfo_t responseData = { 0 };
    responseData.token = (CAToken_t) OICMalloc(pdu->hdr->token_length);
    memcpy(responseData.token, pdu->hdr->token, pdu->hdr->token_length);
    responseData.tokenLength = pdu->hdr->token_length;
    CAResponseInfo_t* responseInfo = (CAResponseInfo_t*) OICMalloc(sizeof(CAResponseInfo_t));
    if (NULL == responseInfo)
    {
        OIC_LOG(ERROR, TAG, "out of memory");
        return NULL;
    }
    responseInfo->info = responseData;

    CAData_t *data = (CAData_t *) OICCalloc(1, sizeof(CAData_t));
    if (NULL == data)
    {
        OIC_LOG(ERROR, TAG, "out of memory");
        OICFree(responseInfo);
        return NULL;
    }

    data->requestInfo = NULL;
    data->responseInfo = responseInfo;
    data->remoteEndpoint = CACloneEndpoint(endpoint);
    data->type = SEND_TYPE_UNICAST;

    return data;
}

CAData_t *CACloneCAData(const CAData_t *data)
{
    VERIFY_NON_NULL_RET(data, TAG, "data", NULL);

    CAData_t *clone = (CAData_t *) OICCalloc(1, sizeof(CAData_t));
    if (NULL == clone)
    {
        OIC_LOG(ERROR, TAG, "out of memory");
        return NULL;
    }
    memcpy(clone, data, sizeof(CAData_t));

    if (data->requestInfo)
    {
        clone->requestInfo = CACloneRequestInfo(data->requestInfo);
    }

    if (data->responseInfo)
    {
        clone->responseInfo = CACloneResponseInfo(data->responseInfo);
    }

    if (data->remoteEndpoint)
    {
        clone->remoteEndpoint = CACloneEndpoint(data->remoteEndpoint);
    }

    if (NULL != data->options && 0 < data->numOptions)
    {
        // copy data
        CAHeaderOption_t *headerOption = (CAHeaderOption_t *) OICMalloc(sizeof(CAHeaderOption_t)
                                                                        * data->numOptions);
        if (NULL == headerOption)
        {
            OIC_LOG(ERROR, TAG, "Out of memory");
            CADestroyDataSet(clone);
            return NULL;
        }
        memcpy(headerOption, data->options, sizeof(CAHeaderOption_t) * data->numOptions);

        clone->options = headerOption;
    }

    return clone;
}

CAResult_t CAUpdatePayloadToCAData(CAData_t *data, const CAPayload_t payload,
                                   uint32_t payloadLen)
{
    OIC_LOG(DEBUG, TAG, "IN-UpdatePayload");

    VERIFY_NON_NULL(data, TAG, "data is NULL");
    VERIFY_NON_NULL(payload, TAG, "payload is NULL");

    if (NULL != data->requestInfo)
    {
        // allocate payload field
        if (NULL != data->requestInfo->info.payload)
        {
            char *temp = (char *) OICCalloc(payloadLen + 1, sizeof(char));
            if (NULL == temp)
            {
                OIC_LOG(ERROR, TAG, "out of memory");
                return CA_STATUS_FAILED;
            }
            memcpy(temp, payload, payloadLen);

            // save the full payload
            OICFree(data->requestInfo->info.payload);
            data->requestInfo->info.payload = temp;
        }
        data->requestInfo->info.payloadSize = payloadLen;
    }

    if (NULL != data->responseInfo)
    {
        // allocate payload field
        if (NULL != data->responseInfo->info.payload)
        {
            char *temp = (char *) OICCalloc(payloadLen + 1, sizeof(char));
            if (NULL == temp)
            {
                OIC_LOG(ERROR, TAG, "out of memory");
                return CA_STATUS_FAILED;
            }
            memcpy(temp, payload, payloadLen);

            // save the full payload
            OICFree(data->responseInfo->info.payload);
            data->responseInfo->info.payload = temp;
        }
        data->responseInfo->info.payloadSize = payloadLen;
    }

    OIC_LOG(DEBUG, TAG, "OUT-UpdatePayload");

    return CA_STATUS_OK;
}

CAPayload_t CAGetPayloadInfo(CAData_t *data, uint32_t *payloadLen)
{
    VERIFY_NON_NULL_RET(data, TAG, "data", NULL);

    if (NULL != data->requestInfo)
    {
        if (NULL != data->requestInfo->info.payload)
        {
            *payloadLen = data->requestInfo->info.payloadSize;
            return data->requestInfo->info.payload;
        }
    }
    else
    {
        if (NULL != data->responseInfo->info.payload)
        {
            *payloadLen = data->responseInfo->info.payloadSize;
            return data->responseInfo->info.payload;
        }
    }

    return NULL;
}

CAResult_t CAHandleBlockErrorResponse(coap_block_t *block, unsigned short blockType,
                                      uint32_t responseResult)
{
    OIC_LOG(DEBUG, TAG, "IN-HandleBlockErrorRes");

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
    }

    OIC_LOG(DEBUG, TAG, "OUT-HandleBlockErrorRes");
    return CA_STATUS_OK;
}

CAResult_t CAUpdateBlockOptionType(const unsigned char* token, uint8_t blockType)
{
    OIC_LOG(DEBUG, TAG, "IN-UpdateBlockOptionType");
    VERIFY_NON_NULL(token, TAG, "token");

    ca_mutex_lock(g_blockDataListMutex);

    CABlockData_t *currData = NULL;
    uint32_t len = u_arraylist_length(g_context.dataList);
    for (uint32_t i = 0; i < len; i++)
    {
        currData = (CABlockData_t *) u_arraylist_get(g_context.dataList, i);

        if (NULL == currData)
        {
            continue;
        }

        if (!strncmp(currData->token, (const char *) token, currData->tokenLength))
        {
            currData->type = blockType;
            ca_mutex_unlock(g_blockDataListMutex);
            return CA_STATUS_OK;
        }
    }
    ca_mutex_unlock(g_blockDataListMutex);

    OIC_LOG(DEBUG, TAG, "OUT-UpdateBlockOptionType");
    return CA_STATUS_FAILED;
}

uint8_t CAGetBlockOptionType(const unsigned char* token)
{
    OIC_LOG(DEBUG, TAG, "IN-GetBlockOptionType");
    VERIFY_NON_NULL(token, TAG, "token");

    ca_mutex_lock(g_blockDataListMutex);

    CABlockData_t *currData = NULL;
    uint32_t len = u_arraylist_length(g_context.dataList);
    for (uint32_t i = 0; i < len; i++)
    {
        currData = (CABlockData_t *) u_arraylist_get(g_context.dataList, i);

        if (NULL == currData)
        {
            continue;
        }

        if (!strncmp(currData->token, (const char *) token, currData->tokenLength))
        {
            ca_mutex_unlock(g_blockDataListMutex);
            return currData->type;
        }
    }
    ca_mutex_unlock(g_blockDataListMutex);

    OIC_LOG(DEBUG, TAG, "OUT-GetBlockOptionType");
    return 0;
}

CAData_t *CAGetDataSetFromBlockDataList(const unsigned char* token)
{
    VERIFY_NON_NULL_RET(token, TAG, "token", NULL);

    ca_mutex_lock(g_blockDataListMutex);

    CABlockData_t *currData = NULL;
    uint32_t len = u_arraylist_length(g_context.dataList);
    for (uint32_t i = 0; i < len; i++)
    {
        currData = (CABlockData_t *) u_arraylist_get(g_context.dataList, i);

        if (NULL == currData)
        {
            continue;
        }

        if (!strncmp(currData->token, (const char *) token, currData->tokenLength))
        {
            ca_mutex_unlock(g_blockDataListMutex);
            return currData->sentData;
        }
    }
    ca_mutex_unlock(g_blockDataListMutex);

    return NULL;
}

CAResult_t CAGetTokenFromBlockDataList(const coap_pdu_t *pdu, const CAEndpoint_t *endpoint,
                                       CAResponseInfo_t *responseInfo)
{
    OIC_LOG(DEBUG, TAG, "IN-CAGetTokenFromBlockDataList");
    VERIFY_NON_NULL(pdu, TAG, "pdu");
    VERIFY_NON_NULL(endpoint, TAG, "endpoint");
    VERIFY_NON_NULL(responseInfo, TAG, "responseInfo");

    ca_mutex_lock(g_blockDataListMutex);

    uint32_t len = u_arraylist_length(g_context.dataList);
    for (uint32_t i = 0; i < len; i++)
    {
        CABlockData_t *currData = (CABlockData_t *) u_arraylist_get(g_context.dataList, i);
        if (NULL == currData)
        {
            continue;
        }

        if (NULL != currData->sentData->requestInfo)
        {
            if (pdu->hdr->id == currData->sentData->requestInfo->info.messageId)// &&
                    //endpoint->transportType == currData->sentData->remoteEndpoint->transportType)
            {
                if (NULL != currData->token)
                {
                    responseInfo->info.tokenLength = currData->tokenLength;
                    responseInfo->info.token = (char *) OICMalloc(currData->tokenLength);
                    if (NULL == responseInfo->info.token)
                    {
                        OIC_LOG(ERROR, TAG, "out of memory");
                        ca_mutex_unlock(g_blockDataListMutex);
                        return CA_MEMORY_ALLOC_FAILED;
                    }
                    memcpy(responseInfo->info.token, currData->token,
                           responseInfo->info.tokenLength);

                    ca_mutex_unlock(g_blockDataListMutex);
                    return CA_STATUS_OK;
                }
            }
        }
    }

    ca_mutex_unlock(g_blockDataListMutex);

    OIC_LOG(DEBUG, TAG, "OUT-CAGetTokenFromBlockDataList");
    return CA_STATUS_FAILED;
}

CAResult_t CACheckBlockDataValidation(CAData_t *sendData, CABlockData_t **blockData)
{
    VERIFY_NON_NULL(sendData, TAG, "sendData");
    VERIFY_NON_NULL(blockData, TAG, "blockData");

    ca_mutex_lock(g_blockDataListMutex);

    uint32_t len = u_arraylist_length(g_context.dataList);
    for (uint32_t i = 0; i < len; i++)
    {
        CABlockData_t *currData = (CABlockData_t *) u_arraylist_get(g_context.dataList, i);

        if (NULL == currData)
        {
            continue;
        }

        if (NULL == currData->token)
        {
            continue;
        }

        if (NULL != sendData->requestInfo) // sendData is requestMessage
        {
            OIC_LOG(DEBUG, TAG, "Send request");
            if (NULL != currData->token && NULL != sendData->requestInfo->info.token)
            {
                if (!strncmp(currData->token, sendData->requestInfo->info.token,
                             currData->tokenLength))
                {
                    OIC_LOG(ERROR, TAG, "already sent");
                    continue;
                }
            }
        }
        else if (NULL != sendData->responseInfo) // sendData is responseMessage
        {
            OIC_LOG(DEBUG, TAG, "Send response");
            if (NULL != currData->token && NULL != sendData->responseInfo->info.token)
            {
                if (!strncmp(currData->token, sendData->responseInfo->info.token,
                             currData->tokenLength))
                {
                    // set sendData
                    if (NULL != currData->sentData)
                    {
                        OIC_LOG(DEBUG, TAG, "init block number");
                        currData->block.num = 0;
                        CADestroyDataSet(currData->sentData);
                    }
                    currData->sentData = sendData;
                    *blockData = currData;
                    ca_mutex_unlock(g_blockDataListMutex);
                    return CA_STATUS_OK;
                }
            }
        }
        else
        {
            OIC_LOG(ERROR, TAG, "no CAInfo data");
            continue;
        }
    }
    ca_mutex_unlock(g_blockDataListMutex);

    return CA_STATUS_FAILED;
}

CABlockData_t *CAGetBlockDataFromBlockDataList(const unsigned char* token)
{
    VERIFY_NON_NULL_RET(token, TAG, "token", NULL);

    ca_mutex_lock(g_blockDataListMutex);

    CABlockData_t *currData = NULL;
    uint32_t len = u_arraylist_length(g_context.dataList);
    for (uint32_t i = 0; i < len; i++)
    {
        currData = (CABlockData_t *) u_arraylist_get(g_context.dataList, i);

        if (NULL == currData)
        {
            continue;
        }

        if (!strncmp(currData->token, (const char *) token, currData->tokenLength))
        {
            ca_mutex_unlock(g_blockDataListMutex);
            return currData;
        }
    }
    ca_mutex_unlock(g_blockDataListMutex);

    return NULL;
}

coap_block_t *CAGetBlockOption(const unsigned char* token)
{
    OIC_LOG(DEBUG, TAG, "IN-GetBlockOption");
    VERIFY_NON_NULL_RET(token, TAG, "token", NULL);

    ca_mutex_lock(g_blockDataListMutex);

    CABlockData_t *currData = NULL;
    uint32_t len = u_arraylist_length(g_context.dataList);
    for (uint32_t i = 0; i < len; i++)
    {
        currData = (CABlockData_t *) u_arraylist_get(g_context.dataList, i);

        if (NULL == currData)
        {
            continue;
        }

        if (!strncmp(currData->token, (const char *) token, currData->tokenLength))
        {
            ca_mutex_unlock(g_blockDataListMutex);
            return &currData->block;
        }
    }
    ca_mutex_unlock(g_blockDataListMutex);

    OIC_LOG(DEBUG, TAG, "OUT-GetBlockOption");
    return NULL;
}

CAPayload_t CAGetPayloadFromBlockDataList(const unsigned char* token, uint32_t *fullPayloadLen)
{
    OIC_LOG(DEBUG, TAG, "IN-GetFullPayload");
    VERIFY_NON_NULL_RET(token, TAG, "token", NULL);

    ca_mutex_lock(g_blockDataListMutex);

    CABlockData_t *currData = NULL;
    uint32_t len = u_arraylist_length(g_context.dataList);
    for (uint32_t i = 0; i < len; i++)
    {
        currData = (CABlockData_t *) u_arraylist_get(g_context.dataList, i);

        if (NULL == currData)
        {
            continue;
        }

        if (!strncmp(currData->token, (const char *) token, currData->tokenLength))
        {
            ca_mutex_unlock(g_blockDataListMutex);
            *fullPayloadLen = currData->receivedPayloadLen;
            return currData->payload;
        }
    }
    ca_mutex_unlock(g_blockDataListMutex);

    OIC_LOG(DEBUG, TAG, "OUT-GetFullPayload");
    return NULL;
}

CABlockData_t *CACreateNewBlockData(CAData_t *sendData)
{
    OIC_LOG(DEBUG, TAG, "IN-CreateBlockData");
    VERIFY_NON_NULL_RET(sendData, TAG, "sendData", NULL);

    // create block data
    CABlockData_t *data = (CABlockData_t *) OICCalloc(1, sizeof(CABlockData_t));
    if (NULL == data)
    {
        OIC_LOG(ERROR, TAG, "memory alloc has failed");
        return NULL;
    }

    data->block.num = 0;
    data->block.m = 0;
    data->block.szx = CA_DEFAULT_BLOCK_SIZE;
    data->type = 0;
    data->sentData = sendData;
    data->payload = NULL;
    data->payloadLength = 0;
    data->receivedPayloadLen = 0;

    if (data->sentData->requestInfo)
    {
        // update token info
        uint8_t tokenLength = data->sentData->requestInfo->info.tokenLength;

        data->tokenLength = tokenLength;
        data->token = (char *) OICMalloc(tokenLength * sizeof(char));
        if (!data->token)
        {
            OIC_LOG(ERROR, TAG, "memory alloc has failed");
            OICFree(data);
            return NULL;
        }
        memcpy(data->token, data->sentData->requestInfo->info.token, tokenLength);
    }
    else
    {
        uint8_t tokenLength = data->sentData->responseInfo->info.tokenLength;

        data->tokenLength = tokenLength;
        data->token = (char *) OICMalloc(tokenLength * sizeof(char));
        if (!data->token)
        {
            OIC_LOG(ERROR, TAG, "memory alloc has failed");
            OICFree(data);
            return NULL;
        }
        memcpy(data->token, data->sentData->responseInfo->info.token, tokenLength);
    }

    ca_mutex_lock(g_blockDataListMutex);

    CAResult_t res = u_arraylist_add(g_context.dataList, (void *) data);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "add has failed");
        OICFree(data);
        ca_mutex_unlock(g_blockDataListMutex);
        return NULL;
    }
    ca_mutex_unlock(g_blockDataListMutex);

    OIC_LOG(DEBUG, TAG, "OUT-CreateBlockData");
    return data;
}

CAResult_t CARemoveBlockDataFromList(const unsigned char* token)
{
    OIC_LOG(DEBUG, TAG, "CARemoveBlockData");
    VERIFY_NON_NULL(token, TAG, "token");

    ca_mutex_lock(g_blockDataListMutex);

    CABlockData_t *currData = NULL;
    uint32_t len = u_arraylist_length(g_context.dataList);
    for (uint32_t i = 0; i < len; i++)
    {
        currData = (CABlockData_t *) u_arraylist_get(g_context.dataList, i);

        if (NULL == currData)
        {
            continue;
        }

        if (!strncmp(currData->token, (const char *) token, currData->tokenLength))
        {
            CABlockData_t *removedData = u_arraylist_remove(g_context.dataList, i);
            if (NULL == removedData)
            {
                OIC_LOG(ERROR, TAG, "data is NULL");
                ca_mutex_unlock(g_blockDataListMutex);
                return CA_STATUS_FAILED;
            }
            CADestroyDataSet(currData->sentData);
            OICFree(currData->payload);
            OICFree(currData->token);
            ca_mutex_unlock(g_blockDataListMutex);
            return CA_STATUS_OK;
        }
    }
    ca_mutex_unlock(g_blockDataListMutex);

    return CA_STATUS_OK;
}

bool CAIsBlockDataInList(const coap_pdu_t *pdu)
{
    OIC_LOG(DEBUG, TAG, "IN-IsBlockDataInList");
    VERIFY_NON_NULL_RET(pdu, TAG, "pdu", false);
    VERIFY_NON_NULL_RET(pdu->hdr, TAG, "pdu->hdr", false);

    ca_mutex_lock(g_blockDataListMutex);

    CABlockData_t *currData = NULL;
    uint32_t len = u_arraylist_length(g_context.dataList);
    for (uint32_t i = 0; i < len; i++)
    {
        currData = (CABlockData_t *) u_arraylist_get(g_context.dataList, i);

        if (NULL == currData)
        {
            continue;
        }

        if (!strncmp(currData->token, (const char *) pdu->hdr->token, currData->tokenLength))
        {
            OIC_LOG(DEBUG, TAG, "found block data");
            ca_mutex_unlock(g_blockDataListMutex);
            return true;
        }
    }
    ca_mutex_unlock(g_blockDataListMutex);

    OIC_LOG(DEBUG, TAG, "OUT-IsBlockDataInList");
    return false;
}

void CADestroyDataSet(CAData_t* data)
{
    VERIFY_NON_NULL_VOID(data, TAG, "data");

    CAFreeEndpoint(data->remoteEndpoint);
    CADestroyRequestInfoInternal(data->requestInfo);
    CADestroyResponseInfoInternal(data->responseInfo);
    OICFree(data->options);
    OICFree(data);
}

void CALogBlockInfo(coap_block_t *block)
{
    VERIFY_NON_NULL_VOID(block, TAG, "block");

    OIC_LOG(DEBUG, TAG, "block option info");

    OIC_LOG_V(DEBUG, TAG, "block option-num : %d", block->num);

    OIC_LOG_V(DEBUG, TAG, "block option-m   : %d", block->m);

    OIC_LOG_V(DEBUG, TAG, "block option-szx : %d", block->szx);
}
