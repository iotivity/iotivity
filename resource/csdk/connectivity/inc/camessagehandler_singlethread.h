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
 * @file camessagehandler_singlethread.h
 * @brief This file contains message functionality.
 */

#ifndef __CA_MESSAGE_HANDLER_SINGLETHREAD_H_
#define __CA_MESSAGE_HANDLER_SINGLETHREAD_H_

#include <stdint.h>
#include "cacommon.h"
#include "cainterface.h"
#include "caretransmission_singlethread.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Function pointer for message handler callback
 */
typedef void (*CAMessageHandlerCallback)(CAToken_t token, CAResult_t res);


/**
 * @brief   Detaches control from the caller for sending unicast request
 * @param   endpoint   [IN]    endpoint information where the data has to be sent
 * @param   request   [IN]    request that needs to be sent
 * @return   CA_STATUS_OK or ERROR CODES ( CAResult_t error codes in cacommon.h)
 */
CAResult_t CADetachRequestMessage(const CARemoteEndpoint_t *object,
                                  const CARequestInfo_t *request);

/**
 * @brief   Detaches control from the caller for sending multicast request
 * @param   object    [IN]    Group endpoint information where the data has to be sent
 * @param   request   [IN]    request that needs to be sent
 * @return   CA_STATUS_OK or ERROR CODES ( CAResult_t error codes in cacommon.h)
 */
CAResult_t CADetachRequestToAllMessage(const CAGroupEndpoint_t *object,
                                       const CARequestInfo_t *request);

/**
 * @brief   Detaches control from the caller for sending response
 * @param   endpoint   [IN]    endpoint information where the data has to be sent
 * @param   response       [IN]    request that needs to be sent
 * @return   CA_STATUS_OK or ERROR CODES ( CAResult_t error codes in cacommon.h)
 */
CAResult_t CADetachResponseMessage(const CARemoteEndpoint_t *object,
                                   const CAResponseInfo_t *response);

/**
 * @brief   Detaches control from the caller for sending request
 * @param   resourceUri   [IN]   resource uri that needs to  be sent in the request
 * @param   token   [IN]    token information of the request
 * @param   options   [IN]    header options that need to be append in the request
 * @param   numOptions   [IN]    number of options be appended
 * @return   CA_STATUS_OK or ERROR CODES ( CAResult_t error codes in cacommon.h)
 */
CAResult_t CADetachMessageResourceUri(const CAURI_t resourceUri, const CAToken_t token,
                                      const CAHeaderOption_t *options, uint8_t numOptions);


/**
 * @brief   Setting the message callback
 * @param   callback   [IN]   callback will be called after data is processed
 * @return   void
 */
void CASetMessageHandlerCallback(CAMessageHandlerCallback callback);

/**
 * @brief   Setting the request and response callbacks for network packets
 * @param   ReqHandler   [IN]    callback for receiving the requests
 * @param   RespHandler  [IN]    callback for receiving the response
 * @return   void
 */
void CASetRequestResponseCallbacks(CARequestCallback ReqHandler, 
                                                   CAResponseCallback RespHandler);

/**
 * @brief  Initialize the message handler by starting thread pool and initializing the 
 *  send and reive queue
 * @return  CA_STATUS_OK or ERROR CODES ( CAResult_t error codes in cacommon.h)
 */
CAResult_t CAInitializeMessageHandler();

/**
 * @brief  Terminate the message handler by stopping  the thread pool and destroying the queues
 * @return  CA_STATUS_OK or ERROR CODES ( CAResult_t error codes in cacommon.h)
 */
void CATerminateMessageHandler();

/**
 * @brief Handler for receiving request and response callback in singled thread model
 */
void CAHandleRequestResponseCallbacks();

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif //#ifndef __CA_MESSAGE_HANDLER_SINGLETHREAD_H_
