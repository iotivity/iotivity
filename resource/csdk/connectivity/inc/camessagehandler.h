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
 * @file
 * This file contains message functionality.
 */

#ifndef CA_MESSAGE_HANDLER_H_
#define CA_MESSAGE_HANDLER_H_

#include "cacommon.h"
#include <coap/coap.h>

#define CA_MEMORY_ALLOC_CHECK(arg) { if (NULL == arg) {OIC_LOG(ERROR, TAG, "Out of memory"); \
goto memory_error_exit;} }

typedef enum
{
    SEND_TYPE_MULTICAST = 0,  /**< multicast type */
    SEND_TYPE_UNICAST         /**< unicast type */
} CASendDataType_t;

typedef struct
{
    CASendDataType_t type;            /**< data type */
    CAEndpoint_t *remoteEndpoint;     /**< remote endpoint */
    CARequestInfo_t *requestInfo;     /**< request information */
    CAResponseInfo_t *responseInfo;   /**< response information */
    CAErrorInfo_t *errorInfo;         /**< error information */
    CASignalingInfo_t *signalingInfo; /**< signaling information */
    CADataType_t dataType;            /**< data type */
} CAData_t;

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Detaches control from the caller for sending message.
 * @param[in] endpoint    endpoint information where the data has to be sent.
 * @param[in] sendMsg     message that needs to be sent.
 * @param[in] dataType    type of the message(request/response).
 * @return  ::CA_STATUS_OK or ERROR CODES (::CAResult_t error codes in cacommon.h).
 */
CAResult_t CADetachSendMessage(const CAEndpoint_t *endpoint,
                               const void *sendMsg,
                               CADataType_t dataType);

/**
 * Setting the request and response callbacks for network packets.
 * @param[in] ReqHandler      callback for receiving the requests.
 * @param[in] RespHandler     callback for receiving the response.
 * @param[in] ErrorHandler    callback for receiving error response.
 */
void CASetInterfaceCallbacks(CARequestCallback ReqHandler, CAResponseCallback RespHandler,
                             CAErrorCallback ErrorHandler);

/**
 * Initialize the message handler by starting thread pool and initializing the
 * send and receive queue.
 * @param[in]   transportType  transport type to initialize.
 * @return  ::CA_STATUS_OK or ERROR CODES (::CAResult_t error codes in cacommon.h).
 */
CAResult_t CAInitializeMessageHandler(CATransportAdapter_t transportType);

/**
 * Terminate the message handler by stopping  the thread pool and destroying the queues.
 */
void CATerminateMessageHandler(void);

/**
 * Handler for receiving request and response callback in single thread model.
 */
void CAHandleRequestResponseCallbacks(void);

/**
 * Setting the Callback funtion for network state change callback.
 * @param[in] nwMonitorHandler    callback for network state change.
 */
void CASetNetworkMonitorCallback(CANetworkMonitorCallback nwMonitorHandler);

#if defined(WITH_BWT) || defined(TCP_ADAPTER)
/**
 * Add the data to the send queue thread.
 * @param[in] data    send data.
 */
void CAAddDataToSendThread(CAData_t *data);

/**
 * Add the data to the receive queue thread to notify received data.
 * @param[in] data    received data.
 */
void CAAddDataToReceiveThread(CAData_t *data);
#endif

#ifdef TCP_ADAPTER
/**
 * Add a header option to the given header option array.
 * @param[in,out] hdrOpt            Pointer to existing options.
 * @param[in,out] numOptions        Number of existing options.
 * @param[in]     optionID          COAP option ID.
 * @param[in]     optionData        Option data value.
 * @param[in]     optionDataLength  Size of Option data value.
 * @return  ::CA_STATUS_OK or ERROR CODES (::CAResult_t error codes in cacommon.h).
 */
CAResult_t CAAddHeaderOption(CAHeaderOption_t **hdrOpt, uint8_t *numOptions,
                             uint16_t optionID, void* optionData,
                             size_t optionDataLength);

/**
 * Get data value of the option with specified option ID from given header option array.
 * @param[in] hdrOpt                Pointer to existing options.
 * @param[in] numOptions            Number of existing options.
 * @param[in] optionID              COAP option ID.
 * @param[out] optionData           Pointer to option data.
 * @param[in] optionDataLength      Size of option data value.
 * @param[out] receivedDataLen      Pointer to the actual length of received data.
 * @return  ::CA_STATUS_OK or ERROR CODES (::CAResult_t error codes in cacommon.h).
 */
CAResult_t CAGetHeaderOption(CAHeaderOption_t *hdrOpt, size_t numOptions, uint16_t optionID,
                             void *optionData, size_t optionDataLength, uint16_t *receivedDataLen);

/**
 * Generate signaling message from the given information.
 * @param[in] endpoint      endpoint information where the data has to be sent.
 * @param[in] code          signaling code has to be sent.
 * @param[in] headerOpt     Pointer to existing options.
 * @param[in] numOptions    Number of existing options.
 * @return  generated signaling message object.
 */
CAData_t *CAGenerateSignalingMessage(const CAEndpoint_t *endpoint, CASignalingCode_t code,
                                     CAHeaderOption_t *headerOpt, uint8_t numOptions);

/**
 * Generate signaling message from the given information.
 * @param[in] endpoint      endpoint information where the data has to be sent.
 * @param[in] code          signaling code has to be sent.
 * @param[in] headerOpt     Pointer to existing options.
 * @param[in] numOptions    Number of existing options.
 * @param[in] token         The token to be used for the message (required for pong messages)
 * @param[in] tokenLength   The length of the token
 * @return  generated signaling message object.
 */
CAData_t *CAGenerateSignalingMessageUsingToken(const CAEndpoint_t *endpoint, CASignalingCode_t code,
                                               CAHeaderOption_t *headerOpt, uint8_t numOptions,
                                               const CAToken_t token, uint8_t tokenLength);

#endif //TCP_ADAPTER

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* CA_MESSAGE_HANDLER_H_ */
