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
#define RANDOM_STRING_LENGTH 1500
#define TOTAL_MESSAGE 0
#define RECEIVE_ERROR_COUNT 0
#define EXPECTED_ERROR_MESSAGE_COUNT 0
#define TESTLOG_START "APIs Called So far: ";

CAHelper caHelper;
CAResult_t result;
String testLog;

void setup()
{
    Serial.begin(SPEED);
    Serial.println("================================");
    Serial.println("TestSuite: CAClientTest");
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
 * @since 2015-03-30
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @see  CAResult_t CACreateRemoteEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CARemoteEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyRemoteEndpoint(CARemoteEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Check the functionality of CASelectNetwork to receive response message when available network is selected
 * @target  CAResult_t CASelectNetwork(const CATransportAdapter_t interestedNetwork)
 * @test_data Response Messages
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                 2. [Server] Register the handlers by CARegisterHandler
 *                 3. [Server] Select network
 *                 4. [Server] Start Server
 *                 5. [Client] Initialize CA by CAInitialize
 *                 6. [Client] Register the handlers by CARegisterHandler
 *                 7. [Client] Select network
 *                 8. [Client] Start Client
 *                 9. [Client] Send Request to find server
 *                10. [Client] Create Endpoint for server to send request to server
 *                11. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] A client sends a request to server
 *             2. [Server] Server receives the request and return a response message to client
 *             3. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client] Terminate CA
 * @expected  The requested amount of response messages should be received
 */
test(SelectNetworkForIncomingMessages_P)
{
    caHelper.setupTestCase(MESSAGE_INCOMING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);
    testLog = TESTLOG_START;

    assertEqual(caHelper.initClientNetwork(), true, "Initialization");
    testLog += "CAInitialize, CASelectNetwork, CARegisterHandler, CAStartDiscoveryServer";

    result = caHelper.sendConfigurationRequest(SEND_MESSAGE, MESSAGE_RESPONSE, CA_PUT);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CASendRequest";

    result = caHelper.receiveMessage(REC_CLI_NOR);
    testLog += ", CAHandleRequestResponse";
    
    if (caHelper.isReceiveError(result, REC_CLI_NOR))
    {
        caHelper.printReceiveError(testLog, REC_CLI_NOR);
        fail();
    }
    
    CATerminate();
}

/**
 * @since 2014-04-02
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @see  CAResult_t CACreateRemoteEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CARemoteEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyRemoteEndpoint(CARemoteEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Check the functionality of CASelectNetwork to receive response message when unavailable network is selected
 * @target   CAResult_t CASelectNetwork(const CATransportAdapter_t interestedNetwork)
 * @test_data Response Messages
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                 2. [Server] Register the handlers by CARegisterHandler
 *                 3. [Server] Select network
 *                 4. [Server] Start Server
 *                 5. [Client] Initialize CA by CAInitialize
 *                 6. [Client] Register the handlers by CARegisterHandler
 *                 7. [Client] Select network
 *                 8. [Client] Start Client
 *                 9. [Client] Send Request to find server
 *                10. [Client] Create Endpoint for server to send request to server
 *                11. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] A client sends a request to server
 *             2. [Server] Server receives the request and return a response message to client
 *             3. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client] Terminate CA
  * @expected  The received amount should be zero
 */
test(SelectUnAvailableNetworkForIncomingMessages_N)
{
    caHelper.setupTestCase(MESSAGE_INCOMING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);
    testLog = TESTLOG_START;

    assertEqual(caHelper.initClientNetwork(), true, "Initialization");
    testLog += "CAInitialize, CASelectNetwork, CARegisterHandler, CAStartDiscoveryServer";

    result = caHelper.sendConfigurationRequest(SEND_MESSAGE, MESSAGE_RESPONSE, CA_PUT);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CASendRequest";

    result = CAUnSelectNetwork(caHelper.m_availableNetwork);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CAUnSelectNetwork";

    result = CASelectNetwork(caHelper.m_unAvailableNetwork);
    assertEqual(result, CA_NOT_SUPPORTED, testLog);
    testLog += ", CASelectNetwork";

    result = caHelper.receiveMessage(TOTAL_MESSAGE, REC_CLI_NOR);
    testLog += ", CAHandleRequestResponse";
    
    if (caHelper.isReceiveError(result, REC_CLI_NOR, RECEIVE_ERROR_COUNT))
    {
        caHelper.printReceiveError(testLog, EXPECTED_ERROR_MESSAGE_COUNT, REC_CLI_NOR);
        fail();
    }
    
    CATerminate();
}

/**
 * @since 2015-04-02
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @see  CAResult_t CACreateRemoteEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CARemoteEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyRemoteEndpoint(CARemoteEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Check the functionality of CASelectNetwork to send message when available network is selected
 * @target   CAResult_t CASelectNetwork(const CATransportAdapter_t interestedNetwork)
 * @test_data Request Messages
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                 2. [Server] Register the handlers by CARegisterHandler
 *                 3. [Server] Select network
 *                 4. [Server] Start Server
 *                 5. [Client] Initialize CA by CAInitialize
 *                 6. [Client] Register the handlers by CARegisterHandler
 *                 7. [Client] Select network
 *                 8. [Client] Start Client
 *                 9. [Client] Send Request to find server
 *                10. [Client] Create Endpoint for server to send request to server
 *                11. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] A client sends a request to server
 *             2. [Server] Server receives the request and return a response message to client
 *             3. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages
 */
test(SelectNetworkForOutgoingMessages_P)
{
    caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);
    testLog = "APIs Called So far: ";

    assertEqual(caHelper.initClientNetwork(), true, "Initialization");
    testLog += "CAInitialize, CASelectNetwork, CARegisterHandler, CAStartDiscoveryServer";

    result = caHelper.sendRequest(CA_PUT, CA_MSG_NONCONFIRM, TOTAL_MESSAGE);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CASendRequest";

    result = caHelper.receiveMessage(REC_CLI_ACK);
    testLog += ", CAHandleRequestResponse";
    
    if (caHelper.isReceiveError(result, REC_CLI_ACK))
    {
        caHelper.printReceiveError(testLog, REC_CLI_ACK);
        fail();
    }
    
    CATerminate();
}

/**
 * @since 2015-04-02
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @see  CAResult_t CACreateRemoteEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CARemoteEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyRemoteEndpoint(CARemoteEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Check the functionality of CASelectNetwork to send request message when unavailable network is selected
 * @target   CAResult_t CASelectNetwork(const CATransportAdapter_t interestedNetwork)
 * @test_data Request Messages
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                 2. [Server] Register the handlers by CARegisterHandler
 *                 3. [Server] Select network
 *                 4. [Server] Start Server
 *                 5. [Client] Initialize CA by CAInitialize
 *                 6. [Client] Register the handlers by CARegisterHandler
 *                 7. [Client] Select network
 *                 8. [Client] Start Client
 *                 9. [Client] Send Request to find server
 *                10. [Client] Create Endpoint for server to send request to server
 *                11. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] A client sends a request to server
 *             2. [Server] Server receives the request and return a response message to client
 *             3. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client] Terminate CA
 * @expected The number of acknowledgments should be equal to zero
 */

test(SelectUnAvailableNetworkForOutgoingMessages_N)
{
    caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);
    testLog = TESTLOG_START;

    assertEqual(caHelper.initClientNetwork(false), true, "Initialization");
    testLog += "CAInitialize, CARegisterHandler";

    result = CASelectNetwork(caHelper.m_unAvailableNetwork);
    assertEqual(result, CA_NOT_SUPPORTED, testLog);
    testLog += ", CASelectNetwork";

    result = caHelper.sendRequest(CA_PUT, CA_MSG_NONCONFIRM, TOTAL_MESSAGE);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CASendRequest";

    result = CASelectNetwork(caHelper.m_availableNetwork);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CASelectNetwork";

    result = caHelper.receiveMessage(TOTAL_MESSAGE, REC_CLI_ACK);
    testLog += ", CAHandleRequestResponse";
    
    if (caHelper.isReceiveError(result, REC_CLI_ACK, RECEIVE_ERROR_COUNT))
    {
        caHelper.printReceiveError(testLog, EXPECTED_ERROR_MESSAGE_COUNT, REC_CLI_ACK);
        fail();
    }
    
    CATerminate();
}
/**
 * @since 2015-04-02
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @see  CAResult_t CACreateRemoteEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CARemoteEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyRemoteEndpoint(CARemoteEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test the effect of selecting available network multiple times on incoming unicast messages
 * @target    CAResult_t CASelectNetwork(const CATransportAdapter_t interestedNetwork)
 * @test_data Response Messages
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                 2. [Server] Register the handlers by CARegisterHandler
 * @procedure  1. [Server] Select network
 *             2. [Server] Start Server
 *             3. [Client] Initialize CA by CAInitialize
 *             4. [Client] Register the handlers by CARegisterHandler
 *             5. [Client] Select network
 *             6. [Client] Start Client
 *             7. [Client] Send Request to find server
 *             8. [Client] Create Endpoint for server to send request to server
 *             9. [Client] Generate a token and set this token in request message
 *            10. [Client] A client sends a request to server
 *            11. [Server] Server receives the request and return a response message to client
 *            12. [Client] Client receive the server's response
 *            13. [Client] Destroy token
 *            14. [Client] Destroy endpoint
 *            15. Repeat procedure 1 to 14 multiple times
 * @post_condition  [Client] Terminate CA
 * @expected  The requested amount of response messages should be received each time
 */

test(SelectNetworkMultipleTimesForIncomingMessages_P)
{
    caHelper.setupTestCase(MESSAGE_INCOMING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);
    testLog = TESTLOG_START;

    assertEqual(caHelper.initClientNetwork(), true, "Initialization");
    testLog += "CAInitialize, CASelectNetwork, CARegisterHandler, CAStartDiscoveryServer";

    for (int i = 0; i < TRY_COUNT; i++)
    {
        result = caHelper.sendConfigurationRequest(SEND_MESSAGE, MESSAGE_RESPONSE, CA_PUT);
        assertEqual(result, CA_STATUS_OK, testLog);
        testLog += ", CASendRequest";

        result = caHelper.receiveMessage(REC_CLI_NOR);
        testLog += ", CAHandleRequestResponse";
        
        if (caHelper.isReceiveError(result, REC_CLI_NOR))
        {
            caHelper.printReceiveError(testLog, REC_CLI_NOR);
            fail();
        }
        
        caHelper.s_receiveCount[REC_CLI_NOR] = 0;
    }
    CATerminate();
}

/**
 * @since 2015-04-02
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @see  CAResult_t CACreateRemoteEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CARemoteEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyRemoteEndpoint(CARemoteEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test unicast message receiving functionality by receiving response messages
 * @target CAResult_t CASendRequest(const CARemoteEndpoint_t *object, CARequestInfo_t *requestInfo)
 * @test_data Response Messages
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                 2. [Server] Register the handlers by CARegisterHandler
 *                 3. [Server] Select network
 *                 4. [Server] Start Server
 *                 5. [Client] Initialize CA by CAInitialize
 *                 6. [Client] Register the handlers by CARegisterHandler
 *                 7. [Client] Select network
 *                 8. [Client] Start Client
 *                 9. [Client] Send Request to find server
 *                10. [Client] Create Endpoint for server to send request to server
 *                11. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] A client sends a request to server
 *             2. [Server] Server receives the request and return a response message to client
 *             3. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client] Terminate CA
 * @expected  The requested amount of response messages should be received
 */
test(ReceiveResponse_P)
{
    caHelper.setupTestCase(MESSAGE_INCOMING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);
    testLog = TESTLOG_START;

    assertEqual(caHelper.initClientNetwork(), true, "Initialization");
    testLog += "CAInitialize, CASelectNetwork, CARegisterHandler, CAStartDiscoveryServer";

    result = caHelper.sendConfigurationRequest(SEND_MESSAGE, MESSAGE_RESPONSE, CA_PUT);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CASendRequest";

    result = caHelper.receiveMessage(REC_CLI_NOR);
    testLog += ", CAHandleRequestResponse";
    
    if (caHelper.isReceiveError(result, REC_CLI_NOR))
    {
        caHelper.printReceiveError(testLog, REC_CLI_NOR);
        fail();
    }
    
    CATerminate();
}

/**
 * @since 2015-04-02
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @see  CAResult_t CACreateRemoteEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CARemoteEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyRemoteEndpoint(CARemoteEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test unicast message sending functionality by sending out request messages
 * @target CAResult_t CASendRequest(const CARemoteEndpoint_t *object, CARequestInfo_t *requestInfo)
 * @test_data Request Messages
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                 2. [Server] Register the handlers by CARegisterHandler
 *                 3. [Server] Select network
 *                 4. [Server] Start Server
 *                 5. [Client] Initialize CA by CAInitialize
 *                 6. [Client] Register the handlers by CARegisterHandler
 *                 7. [Client] Select network
 *                 8. [Client] Start Client
 *                 9. [Client] Send Request to find server
 *                10. [Client] Create Endpoint for server to send request to server
 *                11. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] A client sends a request to server
 *             2. [Server] Server receives the request and return a response message to client
 *             3. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages
 */
test(SendRequest_P)
{
    caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);
    testLog = TESTLOG_START;

    assertEqual(caHelper.initClientNetwork(), true, "Initialization");
    testLog += "CAInitialize, CASelectNetwork, CARegisterHandler, CAStartDiscoveryServer";

    result = caHelper.sendRequest(CA_PUT, CA_MSG_NONCONFIRM, TOTAL_MESSAGE);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CASendRequest";

    result = caHelper.receiveMessage(REC_CLI_ACK);
    testLog += ", CAHandleRequestResponse";
    
    if (caHelper.isReceiveError(result, REC_CLI_ACK))
    {
        caHelper.printReceiveError(testLog, REC_CLI_ACK);
        fail();
    }
    
    CATerminate();
}

/**
 * @since 2015-04-02
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @see  CAResult_t CACreateRemoteEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CARemoteEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyRemoteEndpoint(CARemoteEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test unicast message sending functionality using request messages with empty payloads
 * @target CAResult_t CASendRequest(const CARemoteEndpoint_t *object, CARequestInfo_t *requestInfo)
 * @test_data Empty Payload
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                 2. [Server] Register the handlers by CARegisterHandler
 *                 3. [Server] Select network
 *                 4. [Server] Start Server
 *                 5. [Client] Initialize CA by CAInitialize
 *                 6. [Client] Register the handlers by CARegisterHandler
 *                 7. [Client] Select network
 *                 8. [Client] Start Client
 *                 9. [Client] Send Request to find server
 *                10. [Client] Create Endpoint for server to send request to server
 *                11. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] A client sends a request to server
 *             2. [Server] Server receives the empty payload request and return a response message to client
 *             3. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages and received a response empty payload
 */

test(SendRequestWithEmptyPayload_P)
{
    caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_URI, MESSAGE_UNICAST);
    testLog = TESTLOG_START;

    assertEqual(caHelper.initClientNetwork(), true, "Initialization");
    testLog += "CAInitialize, CASelectNetwork, CARegisterHandler, CAStartDiscoveryServer";

    result = caHelper.sendRequestWithCustomPayload("", CA_PUT, CA_MSG_NONCONFIRM, TOTAL_MESSAGE);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CASendRequest";

    result = caHelper.receiveMessage(REC_CLI_ACK);
    testLog += ", CAHandleRequestResponse";
    
    if (caHelper.isReceiveError(result, REC_CLI_ACK))
    {
        caHelper.printReceiveError(testLog, REC_CLI_ACK);
        fail();
    }
    
    CATerminate();
}

/**
 * @since 2015-04-02
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @see  CAResult_t CACreateRemoteEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CARemoteEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyRemoteEndpoint(CARemoteEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test unicast message sending functionality using request messages with null payloads
 * @target CAResult_t CASendRequest(const CARemoteEndpoint_t *object, CARequestInfo_t *requestInfo)
 * @test_data Null Payload
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                 2. [Server] Register the handlers by CARegisterHandler
 *                 3. [Server] Select network
 *                 4. [Server] Start Server
 *                 5. [Client] Initialize CA by CAInitialize
 *                 6. [Client] Register the handlers by CARegisterHandler
 *                 7. [Client] Select network
 *                 8. [Client] Start Client
 *                 9. [Client] Send Request to find server
 *                10. [Client] Create Endpoint for server to send request to server
 *                11. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] A client sends a request to server
 *             2. [Server] Server receives the null payload request and return a response message to client
 *             3. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages and received a response null payload
 */
test(SendRequestWithNullPayload_P)
{
    caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_URI, MESSAGE_UNICAST);
    testLog = TESTLOG_START;

    assertEqual(caHelper.initClientNetwork(), true, "Initialization");
    testLog += "CAInitialize, CASelectNetwork, CARegisterHandler, CAStartDiscoveryServer";

    result = caHelper.sendRequestWithCustomPayload(NULL, CA_PUT, CA_MSG_NONCONFIRM, TOTAL_MESSAGE);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CASendRequest";

    result = caHelper.receiveMessage(REC_CLI_ACK);
    
    if (caHelper.isReceiveError(result, REC_CLI_ACK))
    {
        caHelper.printReceiveError(testLog, REC_CLI_ACK);
        fail();
    }
    
    CATerminate();
}

/**
 * @since 2015-04-02
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @see  CAResult_t CACreateRemoteEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CARemoteEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyRemoteEndpoint(CARemoteEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test unicast message retransmission functionality using message type CA_MSG_CONFIRM
 * @target CAResult_t CASendRequest(const CARemoteEndpoint_t *object, CARequestInfo_t *requestInfo)
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                 2. [Server] Register the handlers by CARegisterHandler
 *                 3. [Server] Select network
 *                 4. [Server] Start Server
 *                 5. [Client] Initialize CA by CAInitialize
 *                 6. [Client] Register the handlers by CARegisterHandler
 *                 7. [Client] Select network
 *                 8. [Client] Start Client
 *                 9. [Client] Send Request to find server
 *                10. [Client] Create Endpoint for server to send request to server
 *                11. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] A client sends a Get request to server
 *             2. [Server] Server receives the Get request and return a response message to client
 *             3. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client] Terminate CA
 * @expected  The request message should be acknowledged
 */
test(SendRequestWithMessageTypeConfirm_P)
{
    caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);
    testLog = TESTLOG_START;

    assertEqual(caHelper.initClientNetwork(), true, "Initialization");
    testLog += "CAInitialize, CASelectNetwork, CARegisterHandler, CAStartDiscoveryServer";

    result = caHelper.sendRequest(CA_PUT, CA_MSG_CONFIRM, 1);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CASendRequest";

    result = caHelper.receiveMessage(TOTAL_MESSAGE+1, MAX_ATTEMPT, REC_CLI_ACK);
    testLog += ", CAHandleRequestResponse";
    
    if (caHelper.isReceiveError(result, REC_CLI_ACK, RECEIVE_ERROR_COUNT+1))
    {
        caHelper.printReceiveError(testLog, EXPECTED_ERROR_MESSAGE_COUNT+1, REC_CLI_ACK);
        fail();
    }
    
    CATerminate();
}

/**
 * @since 2015-04-02
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @see  CAResult_t CACreateRemoteEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CARemoteEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyRemoteEndpoint(CARemoteEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test unicast message functionality using message type CA_MSG_RESET
 * @target CAResult_t CASendRequest(const CARemoteEndpoint_t *object, CARequestInfo_t *requestInfo)
 * @test_data Request message of type CA_MSG_RESET
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                 2. [Server] Register the handlers by CARegisterHandler
 *                 3. [Server] Select network
 *                 4. [Server] Start Server
 *                 5. [Client] Initialize CA by CAInitialize
 *                 6. [Client] Register the handlers by CARegisterHandler
 *                 7. [Client] Select network
 *                 8. [Client] Start Client
 *                 9. [Client] Send Request to find server
 *                10. [Client] Create Endpoint for server to send request to server
 *                11. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] A client sends a request to server
 *             2. [Server] Server receives the request and return a response message to client
 *             3. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages
 */

test(SendRequestWithMessageTypeReset_P)
{
    caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_EMPTY, MESSAGE_UNICAST);
    testLog = TESTLOG_START;

    assertEqual(caHelper.initClientNetwork(), true, "Initialization");
    testLog += "CAInitialize, CASelectNetwork, CARegisterHandler, CAStartDiscoveryServer";

    result = caHelper.sendRequest(CA_PUT, CA_MSG_RESET, TOTAL_MESSAGE);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CASendRequest";

    result = caHelper.receiveMessage(REC_CLI_ACK);
    testLog += ", CAHandleRequestResponse";
    
    if (caHelper.isReceiveError(result, REC_CLI_ACK))
    {
        caHelper.printReceiveError(testLog, REC_CLI_ACK);
        fail();
    }
    
    CATerminate();
}

/**
 * @since 2014-04-02
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @see  CAResult_t CACreateRemoteEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CARemoteEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyRemoteEndpoint(CARemoteEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test unicast message sending functionality using wrong endpoint
 * @target CAResult_t CASendRequest(const CARemoteEndpoint_t *object, CARequestInfo_t *requestInfo)
 * @test_data Wrong endpoint URI
* @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                 2. [Server] Register the handlers by CARegisterHandler
 *                 3. [Server] Select network
 *                 4. [Server] Start Server
 *                 5. [Client] Initialize CA by CAInitialize
 *                 6. [Client] Register the handlers by CARegisterHandler
 *                 7. [Client] Select network
 *                 8. [Client] Start Client
 *                 9. [Client] Send Request to find server
 *                10. [Client] Create Endpoint for server to send request to server
 *                11. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] A client sends a request to server
 *             2. [Server] Server receives the Get request and return a response message to client
 *             3. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client] Terminate CA
 * @expected  The received amount should be zero
 */
test(SendRequestWithWrongEndpoint_N)
{
    caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);
    testLog = TESTLOG_START;

    assertEqual(caHelper.initClientNetwork(), true, "Initialization");
    testLog += "CAInitialize, CASelectNetwork, CARegisterHandler, CAStartDiscoveryServer";
    
    result = caHelper.sendRequest((char *) WRONG_IP, (char *) SIM_REQ_ACK,
            CAHelper::s_tcInfo.identifier, CA_PUT, CA_MSG_NONCONFIRM, TOTAL_MESSAGE);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CASendRequest";

    result = caHelper.receiveMessage(TOTAL_MESSAGE, REC_CLI_ACK);
    testLog += ", CAHandleRequestResponse";
    
    if (caHelper.isReceiveError(result, REC_CLI_ACK, RECEIVE_ERROR_COUNT))
    {
        caHelper.printReceiveError(testLog, EXPECTED_ERROR_MESSAGE_COUNT, REC_CLI_ACK);
        fail();
    }
    
    CATerminate();
}

/**
 * @since 2015-04-02
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @see  CAResult_t CACreateRemoteEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CARemoteEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyRemoteEndpoint(CARemoteEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test unicast message sending functionality by sending out GET request messages
 * @target CAResult_t CASendRequest(const CARemoteEndpoint_t *object, CARequestInfo_t *requestInfo)
 * @test_data Request messages with method GET
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                 2. [Server] Register the handlers by CARegisterHandler
 *                 3. [Server] Select network
 *                 4. [Server] Start Server
 *                 5. [Client] Initialize CA by CAInitialize
 *                 6. [Client] Register the handlers by CARegisterHandler
 *                 7. [Client] Select network
 *                 8. [Client] Start Client
 *                 9. [Client] Send Request to find server
 *                10. [Client] Create Endpoint for server to send request to server
 *                11. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] A client sends a Get request to server
 *             2. [Server] Server receives Get the request and return a response message to client
 *             3. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages
 */

test(SendGetRequest_P)
{
    caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);
    testLog = TESTLOG_START;

    assertEqual(caHelper.initClientNetwork(), true, "Initialization");
    testLog += "CAInitialize, CASelectNetwork, CARegisterHandler, CAStartDiscoveryServer";

    result = caHelper.sendRequest(CA_GET, CA_MSG_NONCONFIRM, TOTAL_MESSAGE);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CASendRequest";

    result = caHelper.receiveMessage(REC_CLI_ACK);
    testLog += ", CAHandleRequestResponse";
    
    if (caHelper.isReceiveError(result, REC_CLI_ACK))
    {
        caHelper.printReceiveError(testLog, REC_CLI_ACK);
        fail();
    }
    
    CATerminate();
}

/**
 * @since 2015-04-02
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @see  CAResult_t CACreateRemoteEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CARemoteEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyRemoteEndpoint(CARemoteEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test unicast message sending functionality by sending out PUT request messages
 * @target CAResult_t CASendRequest(const CARemoteEndpoint_t *object, CARequestInfo_t *requestInfo)
 * @test_data Request messages with method PUT
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                 2. [Server] Register the handlers by CARegisterHandler
 *                 3. [Server] Select network
 *                 4. [Server] Start Server
 *                 5. [Client] Initialize CA by CAInitialize
 *                 6. [Client] Register the handlers by CARegisterHandler
 *                 7. [Client] Select network
 *                 8. [Client] Start Client
 *                 9. [Client] Send Request to find server
 *                10. [Client] Create Endpoint for server to send request to server
 *                11. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] A client sends a Put request to server
 *             2. [Server] Server receives the Put request and return a response message to client
 *             3. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages
 */
test(SendPutRequest_P)
{
    caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);
    testLog = TESTLOG_START;

    assertEqual(caHelper.initClientNetwork(), true, "Initialization");
    testLog += "CAInitialize, CASelectNetwork, CARegisterHandler, CAStartDiscoveryServer";

    result = caHelper.sendRequest(CA_PUT, CA_MSG_NONCONFIRM, TOTAL_MESSAGE);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CASendRequest";

    result = caHelper.receiveMessage(REC_CLI_ACK);
    testLog += ", CAHandleRequestResponse";
    
    if (caHelper.isReceiveError(result, REC_CLI_ACK))
    {
        caHelper.printReceiveError(testLog, REC_CLI_ACK);
        fail();
    }
    
    CATerminate();
}

/**
 * @since 2015-04-02
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @see  CAResult_t CACreateRemoteEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CARemoteEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyRemoteEndpoint(CARemoteEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test unicast message sending functionality by sending out POST request messages
 * @target CAResult_t CASendRequest(const CARemoteEndpoint_t *object, CARequestInfo_t *requestInfo)
 * @test_data Request messages with method POST
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                 2. [Server] Register the handlers by CARegisterHandler
 *                 3. [Server] Select network
 *                 4. [Server] Start Server
 *                 5. [Client] Initialize CA by CAInitialize
 *                 6. [Client] Register the handlers by CARegisterHandler
 *                 7. [Client] Select network
 *                 8. [Client] Start Client
 *                 9. [Client] Send Request to find server
 *                10. [Client] Create Endpoint for server to send request to server
 *                11. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] A client sends a Post request to server
 *             2. [Server] Server receives the Post request and return a response message to client
 *             3. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages
 */
test(SendPostRequest_P)
{
    caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);
    testLog = TESTLOG_START;

    assertEqual(caHelper.initClientNetwork(), true, "Initialization");
    testLog += "CAInitialize, CASelectNetwork, CARegisterHandler, CAStartDiscoveryServer";

    result = caHelper.sendRequest(CA_POST, CA_MSG_NONCONFIRM, TOTAL_MESSAGE);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CASendRequest";

    result = caHelper.receiveMessage(REC_CLI_ACK);
    testLog += ", CAHandleRequestResponse";
    
    if (caHelper.isReceiveError(result, REC_CLI_ACK))
    {
        caHelper.printReceiveError(testLog, REC_CLI_ACK);
        fail();
    }
    
    CATerminate();
}

/**
 * @since 2015-04-02
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @see  CAResult_t CACreateRemoteEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CARemoteEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyRemoteEndpoint(CARemoteEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test unicast message sending functionality by sending out DELETE request messages
 * @target CAResult_t CASendRequest(const CARemoteEndpoint_t *object, CARequestInfo_t *requestInfo)
 * @test_data Request messages with method DELETE
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                 2. [Server] Register the handlers by CARegisterHandler
 *                 3. [Server] Select network
 *                 4. [Server] Start Server
 *                 5. [Client] Initialize CA by CAInitialize
 *                 6. [Client] Register the handlers by CARegisterHandler
 *                 7. [Client] Select network
 *                 8. [Client] Start Client
 *                 9. [Client] Send Request to find server
 *                10. [Client] Create Endpoint for server to send request to server
 *                11. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] A client sends a Delete request to server
 *             2. [Server] Server receives the Delete request and return a response message to client
 *             3. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages
 */
test(SendDeleteRequest_P)
{
    caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);
    testLog = TESTLOG_START;

    assertEqual(caHelper.initClientNetwork(), true, "Initialization");
    testLog += "CAInitialize, CASelectNetwork, CARegisterHandler, CAStartDiscoveryServer";

    result = caHelper.sendRequest(CA_DELETE, CA_MSG_NONCONFIRM, TOTAL_MESSAGE);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CASendRequest";

    result = caHelper.receiveMessage(REC_CLI_ACK);
    testLog += ", CAHandleRequestResponse";
    
    if (caHelper.isReceiveError(result, REC_CLI_ACK))
    {
        caHelper.printReceiveError(testLog, REC_CLI_ACK);
        fail();
    }
    
    CATerminate();
}

/**
 * @since 2015-04-02
 * @see  CAResult_t CAInitialize()
 * @see  CAResult_t CACreateRemoteEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CARemoteEndpoint_t **object)
 * @see  CAResult_t CACreateRemoteEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CARemoteEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyRemoteEndpoint(CARemoteEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test 'CARegisterHandler' by registering null response handler
 * @target CAResult_t CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @test_data null response handler
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                 2. [Server] Register the handlers by CARegisterHandler
 *                 3. [Server] Select network
 *                 4. [Server] Start Server
 *                 5. [Client] Initialize CA by CAInitialize
 *                 6. [Client] Register the response handlers with a null handler
 *                 7. [Client] Select network
 *                 8. [Client] Start Client
 *                 9. [Client] Send Request to find server
 *                10. [Client] Create Endpoint for server to send request to server
 *                11. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] A client sends a request to server
 *             2. [Server] Server receives the request and return a response message to client
 *             3. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client]Terminate CA
 * @expected No response messages should be received
 */
test(ReceiveWithNullResponseHandler_N)
{
    caHelper.setupTestCase(MESSAGE_INCOMING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);
    testLog = TESTLOG_START;

    assertEqual(caHelper.initNetwork(), true, "Initialization");
    testLog += "CAInitialize, CASelectNetwork";

    CARegisterHandler(CAHelper::handleClientRequest, NULL, CAHelper::handleError);
    testLog += ", CARegisterHandler";

    result = caHelper.sendConfigurationRequest(SEND_MESSAGE, MESSAGE_RESPONSE, CA_PUT);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CASendRequest";

    result = caHelper.receiveMessage(TOTAL_MESSAGE, REC_CLI_NOR);
    testLog += ", CAHandleRequestResponse";
    
    if (caHelper.isReceiveError(result, REC_CLI_NOR, RECEIVE_ERROR_COUNT))
    {
        caHelper.printReceiveError(testLog, EXPECTED_ERROR_MESSAGE_COUNT, REC_CLI_NOR);
        fail();
    }
    
    CATerminate();
}

/**
 * @since 2015-04-02
 * @see  CAResult_t CAInitialize()
 * @see  CAResult_t CACreateRemoteEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CARemoteEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyRemoteEndpoint(CARemoteEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  void CATerminate()
 * @objective Test the impact or consequence of not calling CARegisterHandler Test in receiving response
 * @target CAResult_t CAHandleRequestResponse()
 * @test_data none
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                 2. [Server] Register the handlers by CARegisterHandler
 *                 3. [Server] Select network
 *                 4. [Server] Start Server
 *                 5. [Client] Initialize CA by CAInitialize
 *                 6. [Client] Select network
 *                 7. [Client] Start Client
 *                 8. [Client] Send Request to find server
 *                 9. [Client] Create Endpoint for server to send request to server
 *                10. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] A client sends a request to server
 *             2. [Server] Server receives the request and return a response message to client
 *             3. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client]Terminate CA
 * @expected No response messages should be received
 */
test(ReceiveResponseWithoutCallingCARegisterHandler_N)
{
    caHelper.setupTestCase(MESSAGE_INCOMING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);
    testLog = TESTLOG_START;

    assertEqual(caHelper.initNetwork(), true, "Initialization");
    testLog += "CAInitialize, CASelectNetwork";

    result = caHelper.sendConfigurationRequest(SEND_MESSAGE, MESSAGE_RESPONSE, CA_PUT);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CASendRequest";

    result = caHelper.receiveMessage(TOTAL_MESSAGE, REC_CLI_NOR);
    testLog += ", CAHandleRequestResponse";
    
    if (caHelper.isReceiveError(result, REC_CLI_NOR, RECEIVE_ERROR_COUNT))
    {
        caHelper.printReceiveError(testLog, 0);
        fail();
    }
    
    CATerminate();
}

/**
 * @since 2015-04-02
 * @see  CAResult_t CAInitialize()
 * @see  CAResult_t CACreateRemoteEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CARemoteEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  CAResult_t CASendRequest(const CARemoteEndpoint_t *object, CARequestInfo_t *requestInfo)
 * @see  void CADestroyRemoteEndpoint(CARemoteEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test 'CARegisterHandler' when registration is done twice by using two different response handler
 * @target CAResult_t CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @test_data two different response handler
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                 2. [Server] Register the handlers by CARegisterHandler
 *                 3. [Server] Select network
 *                 4. [Server] Start Server
 *                 5. [Client] Initialize CA by CAInitialize
 *                 6. [Client] Register the response handlers
 *                 7. [Client] Select network
 *                 8. [Client] Start Client
 *                 9. [Client] Send Request to find server
 *                10. [Client] Create Endpoint for server to send request to server
 *                11. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] Call CARegisterHandler twice with two different Response Handler
 *             2. [Client] A client sends a request to server
 *             3. [Server] Server receives the request and return a response message to client
 *             4. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client]Terminate CA
 * @expected Second handler will receive requested amount of response messages and first handler will receive no response message
 */
test(ReceiveWithSecondResponseHandler_P)
{
    caHelper.setupTestCase(MESSAGE_INCOMING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);
    testLog = TESTLOG_START;

    assertEqual(caHelper.initNetwork(), true, "Initialization");
    testLog += "CAInitialize, CASelectNetwork";

    CARegisterHandler(CAHelper::handleClientRequest, CAHelper::handleClientResponse, CAHelper::handleError);
    testLog += ", CARegisterHandler";

    CARegisterHandler(CAHelper::handleClientRequest, CAHelper::handleClientResponseSecond, CAHelper::handleError);
    testLog += ", CARegisterHandler";

    result = CAStartDiscoveryServer();
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CAStartDiscoveryServer";

    result = caHelper.sendConfigurationRequest(SEND_MESSAGE, MESSAGE_RESPONSE, CA_PUT);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CASendRequest";

    result = caHelper.receiveMessage(REC_SECOND_CLI_NOR);
    testLog += ", CAHandleRequestResponse";
    
    if (result != CA_STATUS_OK || CAHelper::getReceiveCount(REC_CLI_NOR) != 0
            || CAHelper::getReceiveCount(REC_SECOND_CLI_NOR) != TOTAL_MESSAGE)
    {
        caHelper.printReceiveError(testLog, REC_SECOND_CLI_NOR);
        fail();
    }
    
    CATerminate();
}

/**
 * @since 2015-04-02
 * @see  CAResult_t CAInitialize()
 * @see  CAResult_t CACreateRemoteEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CARemoteEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  CAResult_t CASendRequest(const CARemoteEndpoint_t *object, CARequestInfo_t *requestInfo)
 * @see  void CADestroyRemoteEndpoint(CARemoteEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test 'CARegisterHandler' when registration is done twice by using first a valid response handler then a null response handler
 * @target CAResult_t CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @test_data valid response handler & null response handler
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                 2. [Server] Register the handlers by CARegisterHandler
 *                 3. [Server] Select network
 *                 4. [Server] Start Server
 *                 5. [Client] Initialize CA by CAInitialize
 *                 6. [Client] Register the response handlers
 *                 7. [Client] Select network
 *                 8. [Client] Start Client
 *                 9. [Client] Send Request to find server
 *                10. [Client] Create Endpoint for server to send request to server
 *                11. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] Call CARegisterHandler twice by using first a valid response handler then a null response handler
 *             2. [Client] A client sends a request to server
 *             3. [Server] Server receives the request and return a response message to client
 *             4. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client]Terminate CA
 * @expected No response messages should be received
 */
test(ReceiveWithValidAndNullResponseHandler_N)
{
    caHelper.setupTestCase(MESSAGE_INCOMING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);
    testLog = TESTLOG_START;

    assertEqual(caHelper.initNetwork(), true, "Initialization");
    testLog += "CAInitialize, CASelectNetwork";

    CARegisterHandler(CAHelper::handleClientRequest, CAHelper::handleClientResponse, CAHelper::handleError);
    testLog += ", CARegisterHandler";

    CARegisterHandler(CAHelper::handleClientRequest, NULL, CAHelper::handleError);
    testLog += ", CARegisterHandler";

    result = caHelper.sendConfigurationRequest(SEND_MESSAGE, MESSAGE_RESPONSE, CA_PUT);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CASendRequest";

    result = caHelper.receiveMessage(TOTAL_MESSAGE, REC_CLI_NOR);
    testLog += ", CAHandleRequestResponse";
    
    if (caHelper.isReceiveError(result, REC_CLI_NOR, RECEIVE_ERROR_COUNT))
    {
        caHelper.printReceiveError(testLog, EXPECTED_ERROR_MESSAGE_COUNT, REC_CLI_NOR);
        fail();
    }
    
    CATerminate();
}
