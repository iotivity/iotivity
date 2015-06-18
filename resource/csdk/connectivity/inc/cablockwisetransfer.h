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
 * @file cablockwisetransfer.h
 * @brief This file contains common function for block messages.
 */

#ifndef CA_BLOCKWISETRANSFER_H_
#define CA_BLOCKWISETRANSFER_H_

#include <stdint.h>

#include "coap.h"
#include "cathreadpool.h"
#include "camutex.h"
#include "uarraylist.h"
#include "cacommon.h"
#include "caprotocolmessage.h"

/**
 * @brief   Callback to send block data
 * @param   data        [IN] send data
 * @return  NONE
 */
typedef void (*CASendThreadFunc)(CAData_t *data);

/**
 * @brief   Callback to notify received data from the remote endpoint
 * @param   data        [IN] received data
 * @return  NONE
 */
typedef void (*CAReceiveThreadFunc)(CAData_t *data);

typedef struct
{
    /** send method for block data **/
    CASendThreadFunc sendThreadFunc;

    /** callback function for received message **/
    CAReceiveThreadFunc receivedThreadFunc;

    /** array list on which the thread is operating. **/
    u_arraylist_t *dataList;

} CABlockWiseContext_t;

typedef struct
{
    coap_block_t block;                 /**< block option */
    unsigned short type;                /**< block option type */
    CAToken_t token;                    /**< token for CA */
    uint8_t tokenLength;                /**< token length*/
    CAData_t *sentData;                 /**< sent request or response data information */
    CAPayload_t payload;                /**< payload buffer  */
    uint32_t payloadLength;             /**< the total payload length to be received  */
} CABlockData_t;

/**
 * @enum  CABlockState_t
 * @brief state of received block message from remote endpoint
 */
typedef enum
{
    CA_BLOCK_UNKNOWN = 0,
    CA_OPTION1_ACK,
    CA_OPTION1_NO_ACK_LAST_BLOCK,
    CA_OPTION1_NO_ACK_BLOCK,
    CA_OPTION2_FIRST_BLOCK,
    CA_OPTION2_LAST_BLOCK,
    CA_OPTION2_ACK,
    CA_OPTION2_NON,
    CA_OPTION2_CON,
    CA_SENT_PREVIOUS_NON_MSG,
    CA_BLOCK_INCOMPLETE,
    CA_BLOCK_TOO_LARGE,
    CA_BLOCK_RECEIVED_ALREADY
} CABlockState_t;

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief   Initializes the block-wise transfer context
 * @param   CASendThreadFunc    [IN] function point to add data in send queue thread
 * @param   CAReceiveThreadFunc [IN] function point to add data in receive queue thread
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CAInitializeBlockWiseTransfer(CASendThreadFunc blockSendMethod,
                                         CAReceiveThreadFunc receivedDataCallback);

/**
 * @brief   Terminate the block-wise transfer context
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CATerminateBlockWiseTransfer();

/**
 * @brief   initialize mutex
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CAInitBlockWiseMutexVaraibles();

/**
 * @brief   terminate mutex
 * @return  None
 */
void CATerminateBlockWiseMutexVaraibles();

/**
 * @brief   Pass the bulk data. if block-wise transfer process need,
 *          bulk data will be sent to block messages.
 * @param   data                [IN] data for sending
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CASendBlockWiseData(CAData_t *data);

/**
 * @brief   Add the data to send thread queue
 * @param   sendData            [IN] data for sending
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CAAddSendThreadQueue(CAData_t *sendData);

/**
 * @brief   Check the block option type. If it has to be sent to a block,
 *          block option will be set.
 * @param   currData            [IN] block data
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CACheckBlockOptionType(CABlockData_t *currData);

/**
 * @brief   Pass the received pdu data. and check if block option is set.
 * @param   pdu                 [IN] received pdu binary data
 * @param   endpoint            [IN] information of remote device
 * @param   receivedData        [IN] received CAData
 * @param   dataLen             [IN] received data length
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CAReceiveBlockWiseData(const coap_pdu_t *pdu, CARemoteEndpoint_t *endpoint,
                                  CAData_t *receivedData, uint32_t dataLen);

/**
 * @brief   process next step by block-wise state
 * @param   pdu                 [IN] received pdu binary data
 * @param   receivedData        [IN] received CAData
 * @param   blockWiseStatus     [IN] block-wise state to move next step
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CAProcessNextStep(const coap_pdu_t *pdu, CAData_t *receivedData,
                             uint8_t blockWiseStatus);

/**
 * @brief   send block message to remote device
 * @param   pdu                 [IN] received pdu binary data
 * @param   msgType             [IN] the message type of the block
 * @param   status              [IN] block-wise state to move next step
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CASendBlockMessage(const coap_pdu_t *pdu, CAMessageType_t msgType, uint8_t status);

/**
 * @brief   send error message to remote device
 * @param   pdu                 [IN] received pdu binary data
 * @param   status              [IN] block-wise state to move next step
 * @param   responseResult      [IN] response result code
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CASendErrorMessage(const coap_pdu_t *pdu, uint8_t status,
                              CAResponseResult_t responseResult);

/**
 * @brief   receive last block data
 * @param   pdu                 [IN] received pdu binary data
 * @param   receivedData        [IN] received CAData
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CAReceiveLastBlock(const coap_pdu_t *pdu, CAData_t *receivedData);

/**
 * @brief   set next block option 1
 * @param   pdu                 [IN] received pdu binary data
 * @param   endpoint            [IN] information of remote device
 * @param   receivedData        [IN] received CAData
 * @param   block               [IN] block option data
 * @param   dataLen             [IN] received data length
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CASetNextBlockOption1(const coap_pdu_t *pdu, CARemoteEndpoint_t *endpoint,
                                 CAData_t *receivedData, coap_block_t block, uint32_t dataLen);

/**
 * @brief   set next block option 2
 * @param   pdu                 [IN] received pdu binary data
 * @param   endpoint            [IN] information of remote device
 * @param   receivedData        [IN] received CAData
 * @param   block               [IN] block option data
 * @param   dataLen             [IN] received data length
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CASetNextBlockOption2(const coap_pdu_t *pdu, CARemoteEndpoint_t *endpoint,
                                 CAData_t *receivedData, coap_block_t block, uint32_t dataLen);

/**
 * @brief   Update the block option in block-wise transfer list
 * @param   currData            [IN] stored block data information
 * @param   block               [IN] block option to update
 * @param   msgType             [IN] message type of pdu
 * @param   blockType           [IN] block option type
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CANegotiateBlockSize(CABlockData_t *currData, coap_block_t *block,
                                CAMessageType_t msgType, unsigned short blockType);

/**
 * @brief   Update the block option in block-wise transfer list
 * @param   currData            [IN] stored block data information
 * @param   block               [IN] block option of current message
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CAUpdateBlockData(CABlockData_t *currData, coap_block_t block);

/**
 * @brief   Update the messageId in block-wise transfer list
 * @param   currData            [IN] stored block data information
 * @param   block               [IN] block option of current message
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CAUpdateMessageId(coap_pdu_t *pdu);

/**
 * @brief   Update the block option items
 * @param   currData            [IN] stored block data information
 * @param   pdu                 [IN] received pdu binary data
 * @param   block               [IN/OUT] block option of current message
 * @param   blockType           [IN] block option type
 * @param   status              [IN] current flow status for block-wise transfer
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CAUpdateBlockOptionItems(CABlockData_t *currData, const coap_pdu_t *pdu,
                                    coap_block_t *block, unsigned short blockType,
                                    uint32_t status);
/**
 * @brief   Set the M-bit of block option
 * @param   payloadLen          [IN] payload length of current bulk data
 * @param   block               [OUT] block option
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CAGetMoreBitFromBlock(uint32_t payloadLen, coap_block_t *block);

/**
 * @brief   check the block option what kind of option have to set.
 * @param   pdu                 [OUT] pdu object
 * @param   info                [IN] information of the request/response
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CAAddBlockOption(coap_pdu_t **pdu, CAInfo_t info);

/**
 * @brief   Write the block option2 in pdu binary data.
 * @param   pdu                 [OUT] pdu object
 * @param   info                [IN] information of the request/response
 * @param   dataLength          [IN] length of payload
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CAAddBlockOption2(coap_pdu_t **pdu, CAInfo_t info, uint32_t dataLength);

/**
 * @brief   Write the block option1 in pdu binary data.
 * @param   pdu                 [OUT] pdu object
 * @param   info                [IN] information of the request/response
 * @param   dataLength          [IN] length of payload
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CAAddBlockOption1(coap_pdu_t **pdu, CAInfo_t info, uint32_t dataLength);

/**
 * @brief   Add the block option in pdu data
 * @param   pdu                 [IN] pdu object
 * @param   block               [OUT] block data
 * @param   blockType           [IN] block option type
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CAAddBlockOptionImpl(coap_pdu_t *pdu, coap_block_t *block, uint8_t blockType);

/**
 * @brief   Add the size option in pdu data
 * @param   pdu                 [IN/OUT] pdu object
 * @param   sizeType            [IN] size option type
 * @param   dataLength          [IN] the total payload length to be sent
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CAAddBlockSizeOption(coap_pdu_t *pdu, unsigned short sizeType, uint32_t dataLength);

/**
 * @brief   Get the size option from pdu data
 * @param   pdu                 [IN] pdu object
 * @param   sizeType            [IN] size option type
 * @param   totalPayloadLen     [OUT] the total payload length to be received
 * @return  true or false
 */
bool CAIsPayloadLengthInPduWithBlockSizeOption(const coap_pdu_t *pdu,
                                               unsigned short sizeType,
                                               uint32_t *totalPayloadLen);

/**
 * @brief   update the total payload with the received payload
 * @param   currData            [IN] stored block data information
 * @param   receivedData        [IN] received CAData
 * @param   status              [IN] block-wise state
 * @param   isSizeOption        [IN] size option
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CAUpdatePayloadData(CABlockData_t *currData, CAData_t *receivedData,
                               uint8_t status, bool isSizeOption);

/**
 * @brief   Generate CAData structure  from the given information.
 * @param   pdu                 [IN] pdu object
 * @param   endpoint            [IN] information of remote device
 * @return  generated CAData
 */
CAData_t* CACreateNewDataSet(const coap_pdu_t *pdu, CARemoteEndpoint_t *endpoint);

/**
 * @brief   Update the block option items
 * @param   block               [IN/OUT] block option of current message
 * @param   blockType           [IN] block option type
 * @param   responseResult      [IN] result code of pdu
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CAHandleBlockErrorResponse(coap_block_t *block, unsigned short blockType,
                                      uint32_t responseResult);

/**
 * @brief   Check the received payload and if an error happens, return error type
 * @param   currData            [IN/OUT] stored block data information
 * @param   receivedBlock       [IN] received block option
 * @param   receivedData        [IN] message type of pdu
 * @param   blockType           [IN] block option type
 * @param   dataLen             [IN] received data length
 * @return  block state
 */
uint8_t CACheckBlockErrorType(CABlockData_t *currData, coap_block_t *receivedBlock,
                              CAData_t *receivedData, unsigned short blockType, uint32_t dataLen);

/**
 * @brief   Destroys the given CAData
 * @param   data                [IN] CAData to destroy
 * @return  generated CAData
 */
void CADestroyDataSet(CAData_t* data);

/**
 * @brief   Print the given block option information
 * @param   block               [IN] block option information
 * @return  NONE
 */
void CALogBlockInfo(coap_block_t *block);

/**
 * @brief   Create new CAData structure from the input information
 * @param   data                [IN]    CAData information that needs to be duplicated
 * @return  created CAData structure
 */
CAData_t *CACloneCAData(const CAData_t *data);

/**
 * @brief   Update payload from the input information
 * @param   data                [IN]    CAData information that needs to be updated
 * @param   payload             [IN]    received new payload from the remote endpoint
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CAUpdatePayloadToCAData(CAData_t *data, const CAPayload_t payload);

/**
 * @brief   Get payload and payload length from the input information
 * @param   data                [IN]    CAData information
 * @param   payloadLen          [OUT]   The payload length is stored.
 * @return  payload
 */
CAPayload_t CAGetPayloadInfo(CAData_t *data, uint32_t *payloadLen);

/**
 * @brief   Set the block option type
 * @param   token               [IN] token of current message
 * @param   blockType           [IN] block option type
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CAUpdateBlockOptionType(const unsigned char* token, uint8_t blockType);

/**
 * @brief   Get the block option type from block-wise transfer list
 * @param   token               [IN] token of current message
 * @return  block option type
 */
uint8_t CAGetBlockOptionType(const unsigned char* token);

/**
 * @brief   Get the block data from block-wise transfer list
 * @param   token               [IN] token of current message
 * @return  CAData structure
 */
CAData_t *CAGetDataSetFromBlockDataList(const unsigned char* token);


/**
 * @brief   Get token from block-wise transfer list
 * @param   pdu                 [IN] received pdu binary data
 * @param   endpoint            [IN] remote endpoint information
 * @param   responseInfo        [IN] received response information
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CAGetTokenFromBlockDataList(const coap_pdu_t *pdu, const CARemoteEndpoint_t *endpoint,
                                       CAResponseInfo_t *responseInfo);

/**
 * @brief   check whether the block data is valid or not
 * @param   sendData            [IN]    CAData information
 * @param   blockData           [OUT]   block data when it is valid
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CACheckBlockDataValidation(CAData_t *sendData, CABlockData_t **blockData);

/**
 * @brief   Get the block data from block-wise transfer list
 * @param   token               [IN] token of current message
 * @return  CABlockData_t structure
 */
CABlockData_t *CAGetBlockDataFromBlockDataList(const unsigned char* token);

/**
 * @brief   Get the block option from block-wise transfer list
 * @param   token               [IN] token of current message
 * @return  coap_block_t structure
 */
coap_block_t *CAGetBlockOption(const unsigned char* token);

/**
 * @brief   Get the full payload from block-wise list
 * @param   token               [IN] token of current message
 * @return  payload
 */
CAPayload_t CAGetPayloadFromBlockDataList(const unsigned char* token);

/**
 * @brief   Create the block data from given data and add the data in block-wise transfer list
 * @param   sendData            [IN] data to be added to a list
 * @return  created CABlockData_t structure
 */
CABlockData_t *CACreateNewBlockData(CAData_t *sendData);

/**
 * @brief   Remove the block data in block-wise transfer list
 * @param   token               [IN] token of current message
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CARemoveBlockDataFromList(const unsigned char* token);

/**
 * @brief   Check if data exist in block-wise transfer list.
 * @param   pdu                 [IN] pdu object
 * @return  true or false
 */
bool CAIsBlockDataInList(const coap_pdu_t *pdu);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  // CA_BLOCKWISETRANSFER_H_
