/******************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
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
 * This file contains the functions for COAP ping and pong over reliable
 * transports i.e TCP and websocket
 */

#ifndef CA_PING_H_
#define CA_PING_H_

#include "cacommon.h"

typedef struct PingInfo_t
{
    CAEndpoint_t        endpoint;       // remote endpoint that was pinged
    bool                withCustody;    // true if ping was sent with custody option
    CAToken_t           token;          // token used in the ping message
    uint8_t             tokenLength;    // length of the token
    uint64_t            timeStamp;      // time stamp at which the ping was sent
    struct PingInfo_t   *next;          // pointer to next ping info
} PingInfo;

// Default ping timeout value in ms. Note: RFC 8323 does not specify a default timeout
#define CA_DEFAULT_PING_TIMEOUT     10000

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Initialize the ping service
 * @return  ::CA_STATUS_OK or ERROR CODES (::CAResult_t error codes in cacommon.h).
 */
CAResult_t CAInitializePing();

/**
 * Send a ping message to the given remote endpoint
 * @param[in] endpoint    endpoint information where the data has to be sent.
 * @param[in] withCustody true if the message needs to be sent with custody option
 * @return  ::CA_STATUS_OK or ERROR CODES (::CAResult_t error codes in cacommon.h).
 */
CAResult_t CASendPingMessage(const CAEndpoint_t *endpoint, bool withCustody);

/**
 * Send a pong message to the given remote endpoint
 * @param[in] endpoint    endpoint information where the data has to be sent.
 * @param[in] withCustody true if the message needs to be sent with custody option
 * @param[in] token       The token of the corresponding ping message (per RFC 8323
 *                        section 5.4 the same token as the ping message should be
 *                        used for the corresponding pong message)
 * @param[in] tokenLength length of the token
 * @return  ::CA_STATUS_OK or ERROR CODES (::CAResult_t error codes in cacommon.h).
 */
CAResult_t CASendPongMessage(const CAEndpoint_t *endpoint, bool withCustody,
                             const CAToken_t token, uint8_t tokenLength);

/**
 * Checks if the timeout for each ping message has expired and
 * if so disconnects the corresponding connection since a pong response
 * was not received in time.
 */
void CAProcessPing();

/**
 * Sets the timeout for a ping message
 * @param[in] timeout   the timeout for the ping message (in ms). If this
 *                      method is not invoked to set the timeout then the
 *                      default timeout CA_DEFAULT_PING_TIMEOUT will be used.
 */
void CASetPingTimeout(uint64_t timeout);

/**
 * Callback that is invoked when a pong message is received by CA layer
 * It clear the state of the corresponding ping message if it exists
 * @param[in] endpoint    endpoint information where the data has to be sent.
 * @param[in] token       The token of the pong message
 * @param[in] tokenLength length of the token
 */
void CAPongReceivedCallback(const CAEndpoint_t *endpoint, const CAToken_t token,
                            uint8_t tokenLength);

/**
 * Terminate the ping service
 */
void CATerminatePing();

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* CA_PING_H_ */