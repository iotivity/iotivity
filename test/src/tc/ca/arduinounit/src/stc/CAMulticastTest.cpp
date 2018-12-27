/******************************************************************
*
* Copyright 2016 Samsung Electronics All Rights Reserved.
*
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/

#include "CAHelper.h"

#define SPEED 115200
#define TESTLOG_START "APIs Called So far: "

CAHelper caHelper;
CAResult_t result;
String testLog;

void setup()
{
    Serial.begin(SPEED);
    Serial.println("================================");
    Serial.println("TestSuite: CAMulticastTest");
    Serial.println("================================");

    CAHelper::addFilter();

    caHelper.Connect();

    Serial.println("------------------------------------BEGIN------------------------------------");
}

void loop()
{
    Test::run();
}

/**
 * @since 2015-06-30
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @see  CAResult_t CACreateRemoteEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CARemoteEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyRemoteEndpoint(CARemoteEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test CASendRequestToAll positively to send multicast messages
 * @target CAResult_t CASendRequestToAll(const CAGroupEndpoint_t *object, const CARequestInfo_t *requestInfo)
 * @test_data Request messages
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                 2. [Server] Register the handlers by CARegisterHandler
 *                 3. [Server] Select network
 *                 4. [Server] Start Server
 *                 5. [Client] Initialize CA by CAInitialize
 *                 6. [Client] Register the handlers by CARegisterHandler
 *                 7. [Client] Select network
 *                 8. [Client] Start Client
 *                 9. [Client] Send multicast request to find server
 *                10. [Client] server will receive the multicast request and return a response to client
 *                11. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] A client sends a request to server
 *             2. [Server] Server receives the request and return a response message to client
 *             3. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client] Terminate CA
 * @expected  The requested amount of response messages should be received
 */
test(SendMulticastRequest_P)
{
    caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_MULTICAST);
    testLog = TESTLOG_START;

    assertEqual(caHelper.initClientNetwork(), true, "Initialization");
    testLog += "CAInitialize, CASelectNetwork, CARegisterHandler, CAStartDiscoveryServer";

    result = caHelper.sendRequestToAll(CA_GET, CA_MSG_NONCONFIRM, TOTAL_MESSAGE);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CASendRequestToAll";

    result = caHelper.receiveMulticastMessage(REC_CLI_ACK);
    testLog += ", CAHandleRequestResponse";
    
    if (caHelper.isMulticastReceiveError(result, REC_CLI_ACK))
    {
        caHelper.printReceiveError(testLog, REC_CLI_ACK);
        fail();
    }
    
    CATerminate();
}

/**
 * @since 2015-04-06
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @see  CAResult_t CACreateRemoteEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CARemoteEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyRemoteEndpoint(CARemoteEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test CASendRequestToAll positively to send multicast messages when payload is empty
 * @target CASendRequestToAll
 * @test_data Request messages
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                  2. [Server] Register the handlers by CARegisterHandler
 *                  3. [Server] Select network
 *                  4. [Server] Start Server
 *                  5. [Client] Initialize CA by CAInitialize
 *                  6. [Client] Register the handlers by CARegisterHandler
 *                  7. [Client] Select network
 *                  8. [Client] Start Client
 *                  9. [Client] Send multicast request to find server
 *                 10. [Client] server will receive the multicast request and return a response to client
 *                 11. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] A client sends a request to server
 *             2. [Server] Server receives the empty payload request and return a response message to client
 *             3. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client] Terminate CA
 * @expected  The received amount of message should be equal to the requested amount with empty payload response
 */
test(SendMulticastWithEmptyPayload_P)
{
    caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_URI, MESSAGE_MULTICAST);
    testLog = TESTLOG_START;

    assertEqual(caHelper.initClientNetwork(), true, "Initialization");
    testLog += "CAInitialize, CASelectNetwork, CARegisterHandler, CAStartDiscoveryServer";

    result = caHelper.sendRequestToAllWithCustomPayload((char *) "", CA_PUT, CA_MSG_NONCONFIRM,
            TOTAL_MESSAGE);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CASendRequestToAll";

    result = caHelper.receiveMulticastMessage(REC_CLI_ACK);
    testLog += ", CAHandleRequestResponse";
    
    if (caHelper.isMulticastReceiveError(result, REC_CLI_ACK))
    {
        caHelper.printReceiveError(testLog, REC_CLI_ACK);
        fail();
    }
    
    CATerminate();
}

/**
 * @since 2015-04-06
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @see  CAResult_t CACreateRemoteEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CARemoteEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyRemoteEndpoint(CARemoteEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test CASendRequestToAll positively to send multicast messages when payload is null
 * @target CASendRequestToAll
 * @test_data Request messages
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                  2. [Server] Register the handlers by CARegisterHandler
 *                  3. [Server] Select network
 *                  4. [Server] Start Server
 *                  5. [Client] Initialize CA by CAInitialize
 *                  6. [Client] Register the handlers by CARegisterHandler
 *                  7. [Client] Select network
 *                  8. [Client] Start Client
 *                  9. [Client] Send multicast request to find server
 *                 10. [Client] server will receive the multicast request and return a response to client
 *                 11. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] A client sends a request to server
 *             2. [Server] Server receives the null payload request and return a response message to client
 *             3. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client] Terminate CA
 * @expected  The received amount of message should be equal to the requested amount with null payload response
 */
test(SendMulticastWithNullPayload_P)
{
    caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_URI, MESSAGE_MULTICAST);
    testLog = TESTLOG_START;

    assertEqual(caHelper.initClientNetwork(), true, "Initialization");
    testLog += "CAInitialize, CASelectNetwork, CARegisterHandler, CAStartDiscoveryServer";

    result = caHelper.sendRequestToAllWithCustomPayload(NULL, CA_PUT, CA_MSG_NONCONFIRM,
            TOTAL_MESSAGE);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CASendRequestToAll";

    result = caHelper.receiveMulticastMessage(REC_CLI_ACK);
    testLog += ", CAHandleRequestResponse";
    
    if (caHelper.isMulticastReceiveError(result, REC_CLI_ACK))
    {
        caHelper.printReceiveError(testLog, REC_CLI_ACK);
        fail();
    }
    
    CATerminate();
}

/**
 * @since 2015-04-06
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @see  CAResult_t CACreateRemoteEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CARemoteEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyRemoteEndpoint(CARemoteEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test CASendRequestToAll negatively to send multicast messages using null endpoint
 * @target CASendRequestToAll
 * @test_data Request messages
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                  2. [Server] Register the handlers by CARegisterHandler
 *                  3. [Server] Select network
 *                  4. [Server] Start Server
 *                  5. [Client] Initialize CA by CAInitialize
 *                  6. [Client] Register the handlers by CARegisterHandler
 *                  7. [Client] Select network
 *                  8. [Client] Start Client
 *                  9. [Client] Send multicast request to find server
 *                 10. [Client] server will receive the multicast request and return a response to client
 *                 11. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] A client sends a request to server
 *             2. [Server] Server receives the request and return a response message to client
 *             3. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client] Terminate CA
 * @expected  The received amount should be zero
 */
test(SendRequestToAllWithNullEndpoint_N)
{
    caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_MULTICAST);
    testLog = TESTLOG_START;

    assertEqual(caHelper.initClientNetwork(), true, "Initialization");
    testLog += "CAInitialize, CASelectNetwork, CARegisterHandler, CAStartDiscoveryServer";

    result = caHelper.sendRequestToAllWithCustomUri(NULL, CA_PUT, CA_MSG_NONCONFIRM,
            TOTAL_MESSAGE);
    assertEqual(result, CA_STATUS_OK, testLog);

    CATerminate();
}
