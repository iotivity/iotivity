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

class CALeClientTest_stc: public ::testing::Test
{
protected:
    CAHelper m_caHelper;

    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();
        m_caHelper.setAvailableNetwork(CA_ADAPTER_GATT_BTLE);
    }

    virtual void TearDown()
    {
        CommonUtil::runCommonTCTearDownPart();
    }
};

/**
 * @since 2015-09-08
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @see  CAResult_t CACreateEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CAEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Check the functionality of CASelectNetwork to receive response message when available network is selected
 * @target  CAResult_t CASelectNetwork(const CATransportAdapter_t interestedNetwork)
 * @test_data Response Messages
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                 2. [Server] Register the handlers by CARegisterHandler
 *                 3. [Server] Select BLE network
 *                 4. [Server] Start Server
 *                 5. [Client] Initialize CA by CAInitialize
 *                 6. [Client] Register the handlers by CARegisterHandler
 *                 7. [Client] Select BLE network
 *                 8. [Client] Start Client
 *                 9. [Client] Send multicast request to find server
 *                10. [Server] server will receive the multicast request and return a response to client
 *                11. [Client] Create endpoint for server to send unicast request to server
 *                12. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] A client sends a request to server
 *             2. [Server] Server receives the request and return a response message to client
 *             3. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client] Terminate CA
 * @expected  The requested amount of response messages should be received
 */
#if defined(__TIZEN__) && defined(__BLE__)
TEST_F(CALeClientTest_stc, LeSelectNetworkForIncomingMessages_P)
{
    m_caHelper.setupTestCase(MESSAGE_INCOMING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.establishConnectionWithServer())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if (!m_caHelper.sendConfigurationRequest(SEND_MESSAGE, MESSAGE_RESPONSE, CA_PUT))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.countReceiveMessage(REC_NOR))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    CATerminate();
}
#endif

/**
 * @since 2015-09-08
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @see  CAResult_t CACreateEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CAEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Check the functionality of CASelectNetwork to receive response message when unavailable network is selected
 * @target   CAResult_t CASelectNetwork(const CATransportAdapter_t interestedNetwork)
 * @test_data Response Messages
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                 2. [Server] Register the handlers by CARegisterHandler
 *                 3. [Server] Select BLE network
 *                 4. [Server] Start Server
 *                 5. [Client] Initialize CA by CAInitialize
 *                 6. [Client] Register the handlers by CARegisterHandler
 *                 7. [Client] Select BLE network
 *                 8. [Client] Start Client
 *                 9. [Client] Send multicast request to find server
 *                10. [Server] server will receive the multicast request and return a response to client
 *                11. [Client] Create endpoint for server to send unicast request to server
 *                12. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] A client sends a request to server
 *             2. [Server] Server receives the request and return a response message to client
 *             3. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client]Terminate CA
 * @expected  The received amount should be zero
 */
#if defined(__TIZEN__) && defined(__BLE__)
TEST_F(CALeClientTest_stc, LeSelectUnAvailableNetworkForIncomingMessages_N)
{
    m_caHelper.setupTestCase(MESSAGE_INCOMING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.establishConnectionWithServer())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if (!m_caHelper.sendConfigurationRequest(SEND_MESSAGE, MESSAGE_RESPONSE, CA_PUT))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.unselectNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.selectNetwork(m_caHelper.m_unAvailableNetwork, CA_NOT_SUPPORTED))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.countReceiveMessage(REC_NOR, 0))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    CATerminate();
}
#endif
/**
 * @since 2015-09-08
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @see  CAResult_t CACreateEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CAEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Check the functionality of CASelectNetwork to send message when available network is selected
 * @target   CAResult_t CASelectNetwork(const CATransportAdapter_t interestedNetwork)
 * @test_data Request Messages
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                 2. [Server] Register the handlers by CARegisterHandler
 *                 3. [Server] Select BLE network
 *                 4. [Server] Start Server
 *                 5. [Client] Initialize CA by CAInitialize
 *                 6. [Client] Register the handlers by CARegisterHandler
 *                 7. [Client] Select BLE network
 *                 8. [Client] Start Client
 *                 9. [Client] Send multicast request to find server
 *                10. [Server] server will receive the multicast request and return a response to client
 *                11. [Client] Create endpoint for server to send unicast request to server
 *                12. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] A client sends a request to server
 *             2. [Server] Server receives the request and return a response message to client
 *             3. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages
 */
#if defined(__TIZEN__) && defined(__BLE__)
TEST_F(CALeClientTest_stc, LeSelectNetworkForOutgoingMessages_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.establishConnectionWithServer())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if (!m_caHelper.sendRequest(CA_PUT, CA_MSG_NONCONFIRM, TOTAL_MESSAGE))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.countReceiveMessage(REC_ACK))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    CATerminate();
}
#endif

/**
 * @since 2015-09-08
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @see  CAResult_t CACreateEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CAEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Check the functionality of CASelectNetwork to send request message when unavailable network is selected
 * @target   CAResult_t CASelectNetwork(const CATransportAdapter_t interestedNetwork)
 * @test_data Request Messages
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                 2. [Server] Register the handlers by CARegisterHandler
 *                 3. [Server] Select BLE network
 *                 4. [Server] Start Server
 *                 5. [Client] Initialize CA by CAInitialize
 *                 6. [Client] Register the handlers by CARegisterHandler
 *                 7. [Client] Select BLE network
 *                 8. [Client] Start Client
 *                 9. [Client] Send multicast request to find server
 *                10. [Server] server will receive the multicast request and return a response to client
 *                11. [Client] Create endpoint for server to send unicast request to server
 *                12. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] A client sends a request to server
 *             2. [Server] Server receives the request and return a response message to client
 *             3. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client]Terminate CA
 * @expected The number of acknowledgments should be equal to zero
 */
#if defined(__TIZEN__) && defined(__BLE__)
TEST_F(CALeClientTest_stc, LeSelectUnAvailableNetworkForOutgoingMessages_N)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.initClientNetwork(false))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if(!m_caHelper.selectNetwork(m_caHelper.m_unAvailableNetwork, CA_NOT_SUPPORTED))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.sendRequest((char*) m_caHelper.m_simulatorResAckUri.c_str(), m_caHelper.s_tcInfo.identifier,
                    CA_PUT, CA_MSG_NONCONFIRM, TOTAL_MESSAGE, CA_STATUS_FAILED))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if(!m_caHelper.selectNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.countReceiveMessage(REC_ACK, 0))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    CATerminate();
}
#endif

/**
 * @since 2015-09-08
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @see  CAResult_t CACreateEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CAEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test the effect of select/unselect network on incoming unicast messages in a loop
 * @target    CAResult_t CASelectNetwork(const CATransportAdapter_t interestedNetwork)
 * @test_data Response Messages
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                 2. [Server] Register the handlers by CARegisterHandler
 * @procedure  1. [Server] Select BLE network
 *             2. [Server] Start Server
 *             3. [Client] Initialize CA by CAInitialize
 *             4. [Client] Register the handlers by CARegisterHandler
 *             5. [Client] Select BLE network
 *             6. [Client] Start Client
 *             7. [Client] Send multicast request to find server
 *             8. [Server] server will receive the multicast request and return a response to client
 *             9. [Client] Create endpoint for server to send unicast request to server
 *            10. [Client] Generate a token and set this token in request message
 *            11. [Client] A client sends a request to server
 *            12. [Server] Server receives the request and return a response message to client
 *            13. [Client] Client receive the server's response
 *            14. [Client] Destroy token
 *            15. [Client] Destroy endpoint
 *            16. [Client] Unselect the available network
 *            17. Repeat procedure 1 to 16 multiple times
 * @post_condition Terminate CA
 * @expected  The requested amount of response messages should be received each time
 */
#if defined(__TIZEN__) && defined(__BLE__)
TEST_F(CALeClientTest_stc, LeSelectNetworkSequentiallyForIncomingMessages_P)
{
    m_caHelper.setupTestCase(MESSAGE_INCOMING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.establishConnectionWithServer())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    for (int i = 0; i < TRY_COUNT; i++)
    {
        if(i > 0)
        {
            if(!m_caHelper.selectNetwork())
            {
                SET_FAILURE(m_caHelper.getFailureMessage());
                CATerminate();
                return;
            }
        }

        if (!m_caHelper.sendConfigurationRequest(SEND_MESSAGE, MESSAGE_RESPONSE, CA_PUT))
        {
            SET_FAILURE(m_caHelper.getFailureMessage());
            CATerminate();
            return;
        }

        if (!m_caHelper.countReceiveMessage(REC_NOR))
        {
            SET_FAILURE(m_caHelper.getFailureMessage());
            CATerminate();
            return;
        }

        if(!m_caHelper.unselectNetwork())
        {
            SET_FAILURE(m_caHelper.getFailureMessage());
            CATerminate();
            return;
        }

        m_caHelper.s_mapReceiveCount.clear();
    }

    CATerminate();
}
#endif

/**
 * @since 2015-09-08
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @see  CAResult_t CACreateEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CAEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test the effect of selecting available network multiple times on incoming unicast messages
 * @target    CAResult_t CASelectNetwork(const CATransportAdapter_t interestedNetwork)
 * @test_data Response Messages
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                 2. [Server] Register the handlers by CARegisterHandler
 * @procedure  1. [Server] Select BLE network
 *             2. [Server] Start Server
 *             3. [Client] Initialize CA by CAInitialize
 *             4. [Client] Register the handlers by CARegisterHandler
 *             5. [Client] Select BLE network
 *             6. [Client] Start Client
 *             7. [Client] Send multicast request to find server
 *             8. [Server] server will receive the multicast request and return a response to client
 *             9. [Client] Create endpoint for server to send unicast request to server
 *            10. [Client] Generate a token and set this token in request message
 *            11. [Client] A client sends a request to server
 *            12. [Server] Server receives the request and return a response message to client
 *            13. [Client] Client receive the server's response
 *            14. [Client] Destroy token
 *            15. [Client] Destroy endpoint
 *            16. Repeat procedure 1 to 15 multiple times
 * @post_condition Terminate CA
 * @expected  The requested amount of response messages should be received each time
 */
#if defined(__TIZEN__) && defined(__BLE__)
TEST_F(CALeClientTest_stc, LeSelectNetworkMultipleTimesForIncomingMessages_P)
{
    m_caHelper.setupTestCase(MESSAGE_INCOMING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.establishConnectionWithServer())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    for (int i = 0; i < TRY_COUNT; i++)
    {
        if(!m_caHelper.selectNetwork())
        {
            SET_FAILURE(m_caHelper.getFailureMessage());
            CATerminate();
            return;
        }

        if (!m_caHelper.sendConfigurationRequest(SEND_MESSAGE, MESSAGE_RESPONSE, CA_PUT))
        {
            SET_FAILURE(m_caHelper.getFailureMessage());
            CATerminate();
            return;
        }

        if (!m_caHelper.countReceiveMessage(REC_NOR))
        {
            SET_FAILURE(m_caHelper.getFailureMessage());
            CATerminate();
            return;
        }

        m_caHelper.s_mapReceiveCount.clear();
    }

    CATerminate();
}
#endif

/**
 * @since 2015-09-08
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @see  CAResult_t CACreateEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CAEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test unicast message receiving functionality by receiving response messages
 * @target CAResult_t CASendRequest(const CAEndpoint_t *object, CARequestInfo_t *requestInfo)
 * @test_data Response Messages
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                 2. [Server] Register the handlers by CARegisterHandler
 *                 3. [Server] Select BLE network
 *                 4. [Server] Start Server
 *                 5. [Client] Initialize CA by CAInitialize
 *                 6. [Client] Register the handlers by CARegisterHandler
 *                 7. [Client] Select BLE network
 *                 8. [Client] Start Client
 *                 9. [Client] Send multicast request to find server
 *                10. [Server] server will receive the multicast request and return a response to client
 *                11. [Client] Create endpoint for server to send unicast request to server
 *                12. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] A client sends a request to server
 *             2. [Server] Server receives the request and return a response message to client
 *             3. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client]Terminate CA
 * @expected  The requested amount of response messages should be received
 */
#if defined(__TIZEN__) && defined(__BLE__)
TEST_F(CALeClientTest_stc, LeReceiveResponse_P)
{
    m_caHelper.setupTestCase(MESSAGE_INCOMING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.establishConnectionWithServer())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if (!m_caHelper.sendConfigurationRequest(SEND_MESSAGE, MESSAGE_RESPONSE, CA_PUT))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.countReceiveMessage(REC_NOR))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    CATerminate();
}
#endif

/**
 * @since 2015-09-18
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @see  CAResult_t CACreateEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CAEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test unicast message sending functionality by sending out request messages
 * @target CAResult_t CASendRequest(const CAEndpoint_t *object, CARequestInfo_t *requestInfo)
 * @test_data Request Messages
 * @pre_condition  none
 * 
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                 2. [Server] Register the handlers by CARegisterHandler
 * @procedure  1. [Server] Select BLE network
 *             2. [Server] Start Server
 *             3. [Client] Initialize CA by CAInitialize
 *             4. [Client] Register the handlers by CARegisterHandler
 *             5. [Client] Select BLE network
 *             6. [Client] Start Client
 *             7. [Client] Send multicast request to find server
 *             8. [Server] server will receive the multicast request and return a response to client
 *             9. [Client] Create endpoint for server to send unicast request to server
 *            10. [Client] Generate a token and set this token in request message
 *            11. [Client] A client sends a request to server
 *            12. [Server] Server receives the request and return a response message to client
 *            13. [Client] Client receive the server's response
 *            14. [Client] Destroy token
 *            15. [Client] Destroy endpoint
 *            16. [Client] Unselect the available network
 *            17. [Client] Terminate CA
 *            18. Repeat procedure 1 to 17 multiple times
 * @post_condition none
 * @expected  The number of acknowledgments should be equal to the amount of sent messages each times
 */
#if defined(__TIZEN__) && defined(__BLE__)
TEST_F(CALeClientTest_stc, LeSendRequestMultipleTimes_P)
{
    for(int i = 0; i < TRY_COUNT; i++)
    {
        m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

        if (!m_caHelper.establishConnectionWithServer())
        {
            SET_FAILURE(m_caHelper.getFailureMessage());
            return;
        }

        if (!m_caHelper.sendRequest(CA_PUT, CA_MSG_NONCONFIRM, TOTAL_MESSAGE))
        {
            SET_FAILURE(m_caHelper.getFailureMessage());
            CATerminate();
            return;
        }

        if (!m_caHelper.countReceiveMessage(REC_ACK))
        {
            SET_FAILURE(m_caHelper.getFailureMessage());
        }

        CATerminate();
    }
}
#endif

/**
 * @since 2015-09-08
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @see  CAResult_t CACreateEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CAEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test unicast message sending functionality by sending out request messages
 * @target CAResult_t CASendRequest(const CAEndpoint_t *object, CARequestInfo_t *requestInfo)
 * @test_data Request Messages
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                 2. [Server] Register the handlers by CARegisterHandler
 *                 3. [Server] Select BLE network
 *                 4. [Server] Start Server
 *                 5. [Client] Initialize CA by CAInitialize
 *                 6. [Client] Register the handlers by CARegisterHandler
 *                 7. [Client] Select BLE network
 *                 8. [Client] Start Client
 *                 9. [Client] Send multicast request to find server
 *                10. [Server] server will receive the multicast request and return a response to client
 *                11. [Client] Create endpoint for server to send unicast request to server
 *                12. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] A client sends a request to server
 *             2. [Server] Server receives the without header request and return a response message to client
 *             3. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client]Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages and received response without header
 */
#if defined(__TIZEN__) && defined(__BLE__)
TEST_F(CALeClientTest_stc, LeSendRequestWithoutHeaderOption_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_HEADER, MESSAGE_UNICAST);

    if (!m_caHelper.establishConnectionWithServer())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    CAHelper::s_tcInfo.numOptions = 0;

    if (!m_caHelper.sendRequest(CA_PUT, CA_MSG_NONCONFIRM, TOTAL_MESSAGE))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.countReceiveMessage(REC_ACK))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    CATerminate();
}
#endif

/**
 * @since 2015-09-08
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @see  CAResult_t CACreateEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CAEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test unicast message sending functionality by sending out request messages
 * @target CAResult_t CASendRequest(const CAEndpoint_t *object, CARequestInfo_t *requestInfo)
 * @test_data Request Messages
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                 2. [Server] Register the handlers by CARegisterHandler
 *                 3. [Server] Select BLE network
 *                 4. [Server] Start Server
 *                 5. [Client] Initialize CA by CAInitialize
 *                 6. [Client] Register the handlers by CARegisterHandler
 *                 7. [Client] Select BLE network
 *                 8. [Client] Start Client
 *                 9. [Client] Send multicast request to find server
 *                10. [Server] server will receive the multicast request and return a response to client
 *                11. [Client] Create endpoint for server to send unicast request to server
 *                12. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] A client sends a request to server
 *             2. [Server] Server receives the with header request and return a response message to client
 *             3. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client]Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages and received response with one header
 */
#if defined(__TIZEN__) && defined(__BLE__)
TEST_F(CALeClientTest_stc, LeSendRequestWithHeaderOption_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_HEADER, MESSAGE_UNICAST);

    if (!m_caHelper.establishConnectionWithServer())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    CAHelper::s_tcInfo.numOptions = 1;

    if (!m_caHelper.sendRequest(CA_PUT, CA_MSG_NONCONFIRM, TOTAL_MESSAGE))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.countReceiveMessage(REC_ACK))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    CATerminate();
}
#endif

/**
 * @since 2015-09-08
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @see  CAResult_t CACreateEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CAEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test unicast message sending functionality by sending out request messages
 * @target CAResult_t CASendRequest(const CAEndpoint_t *object, CARequestInfo_t *requestInfo)
 * @test_data Request Messages
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                 2. [Server] Register the handlers by CARegisterHandler
 *                 3. [Server] Select BLE network
 *                 4. [Server] Start Server
 *                 5. [Client] Initialize CA by CAInitialize
 *                 6. [Client] Register the handlers by CARegisterHandler
 *                 7. [Client] Select BLE network
 *                 8. [Client] Start Client
 *                 9. [Client] Send multicast request to find server
 *                10. [Server] server will receive the multicast request and return a response to client
 *                11. [Client] Create endpoint for server to send unicast request to server
 *                12. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] A client sends a request to server
 *             2. [Server] Server receives the with header request and return a response message to client
 *             3. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client]Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages and received response with one header
 */
#if defined(__TIZEN__) && defined(__BLE__)
TEST_F(CALeClientTest_stc, LeSendRequestWithHeaderOptions_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_HEADER, MESSAGE_UNICAST);

    if (!m_caHelper.establishConnectionWithServer())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    CAHelper::s_tcInfo.numOptions = 2;

    if (!m_caHelper.sendRequest(CA_PUT, CA_MSG_NONCONFIRM, TOTAL_MESSAGE))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.countReceiveMessage(REC_ACK))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    CATerminate();
}
#endif

/**
 * @since 2015-09-08
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @see  CAResult_t CACreateEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CAEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test unicast message sending functionality using request messages with empty payloads
 * @target CAResult_t CASendRequest(const CAEndpoint_t *object, CARequestInfo_t *requestInfo)
 * @test_data Empty Payload
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                 2. [Server] Register the handlers by CARegisterHandler
 *                 3. [Server] Select BLE network
 *                 4. [Server] Start Server
 *                 5. [Client] Initialize CA by CAInitialize
 *                 6. [Client] Register the handlers by CARegisterHandler
 *                 7. [Client] Select BLE network
 *                 8. [Client] Start Client
 *                 9. [Client] Send multicast request to find server
 *                10. [Server] server will receive the multicast request and return a response to client
 *                11. [Client] Create endpoint for server to send unicast request to server
 *                12. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] A client sends a request to server
 *             2. [Server] Server receives the empty payload request and return a response message to client
 *             3. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client]Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages and received a response empty payload
 */
#if defined(__TIZEN__) && defined(__BLE__)
TEST_F(CALeClientTest_stc, LeSendRequestWithEmptyPayload_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_URI, MESSAGE_UNICAST);

    if (!m_caHelper.establishConnectionWithServer())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    char* uri = (char*) m_caHelper.m_simulatorReqAckUri.c_str();
    char* payload = (char*) "";

    if (!m_caHelper.sendRequest(uri, payload, CA_PUT, CA_MSG_NONCONFIRM,
                    TOTAL_MESSAGE))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.countReceiveMessage(REC_ACK))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    CATerminate();
}
#endif
/**
 * @since 2014-12-24
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @see  CAResult_t CACreateEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CAEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test unicast message sending functionality using request messages with null payloads
 * @target CAResult_t CASendRequest(const CAEndpoint_t *object, CARequestInfo_t *requestInfo)
 * @test_data Null Payload
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                 2. [Server] Register the handlers by CARegisterHandler
 *                 3. [Server] Select BLE network
 *                 4. [Server] Start Server
 *                 5. [Client] Initialize CA by CAInitialize
 *                 6. [Client] Register the handlers by CARegisterHandler
 *                 7. [Client] Select BLE network
 *                 8. [Client] Start Client
 *                 9. [Client] Send multicast request to find server
 *                10. [Server] server will receive the multicast request and return a response to client
 *                11. [Client] Create endpoint for server to send unicast request to server
 *                12. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] A client sends a request to server
 *             2. [Server] Server receives the null payload request and return a response message to client
 *             3. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client]Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages and received a response with null payload
 */
#if defined(__TIZEN__) && defined(__BLE__)
TEST_F(CALeClientTest_stc, LeSendRequestWithNullPayload_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_URI, MESSAGE_UNICAST);

    if (!m_caHelper.establishConnectionWithServer())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    char* uri = (char*) m_caHelper.m_simulatorReqAckUri.c_str();

    if (!m_caHelper.sendRequest(uri, NULL, CA_PUT, CA_MSG_NONCONFIRM,
                    TOTAL_MESSAGE))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.countReceiveMessage(REC_ACK))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    CATerminate();
}
#endif
/**
 * @since 2015-09-08
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @see  CAResult_t CACreateEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CAEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test unicast message retransmission functionality using message type CA_MSG_CONFIRM
 * @target CAResult_t CASendRequest(const CAEndpoint_t *object, CARequestInfo_t *requestInfo)
 * @test_data Request message of type CA_MSG_CONFIRM
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                 2. [Server] Register the handlers by CARegisterHandler
 *                 3. [Server] Select BLE network
 *                 4. [Server] Start Server
 *                 5. [Client] Initialize CA by CAInitialize
 *                 6. [Client] Register the handlers by CARegisterHandler
 *                 7. [Client] Select BLE network
 *                 8. [Client] Start Client
 *                 9. [Client] Send multicast request to find server
 *                10. [Server] server will receive the multicast request and return a response to client
 *                11. [Client] Create endpoint for server to send unicast request to server
 *                12. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] A client sends a confirm Get request to server
 *             2. [Server] Server receives request and return a response message to client
 *             3. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client]Terminate CA
 * @expected  The request message should be acknowledged
 */
#if defined(__TIZEN__) && defined(__BLE__)
TEST_F(CALeClientTest_stc, LeSendRequestWithMessageTypeConfirm_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.establishConnectionWithServer())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if (!m_caHelper.sendRequest(CA_PUT, CA_MSG_CONFIRM, 1))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.countReceiveMessage(REC_ACK, 1))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    CATerminate();
}
#endif

/**
 * @since 2015-09-08
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @see  CAResult_t CACreateEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CAEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test unicast message sending functionality using wrong endpoint
 * @target CAResult_t CASendRequest(const CAEndpoint_t *object, CARequestInfo_t *requestInfo)
 * @test_data Wrong endpoint URI
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                 2. [Server] Register the handlers by CARegisterHandler
 *                 3. [Server] Select BLE network
 *                 4. [Server] Start Server
 *                 5. [Client] Initialize CA by CAInitialize
 *                 6. [Client] Register the handlers by CARegisterHandler
 *                 7. [Client] Select BLE network
 *                 8. [Client] Start Client
 *                 9. [Client] Send multicast request to find server
 *                10. [Server] server will receive the multicast request and return a response to client
 *                11. [Client] Create endpoint for server to send unicast request to server
 *                12. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] A client sends a request to server
 *             2. [Server] Server receives the request and return a response message to client
 *             3. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client]Terminate CA
 * @expected  The received amount should be zero
 */
#if defined(__TIZEN__) && defined(__BLE__)
TEST_F(CALeClientTest_stc, LeSendRequestWithWrongEndpoint_N)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.establishConnectionWithServer())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    char* uri = (char *) SIM_REQ_ACK_WRONG_EP_URI;
    char* payload = m_caHelper.s_tcInfo.identifier;

    if (!m_caHelper.sendRequest(uri, payload, CA_PUT, CA_MSG_NONCONFIRM, TOTAL_MESSAGE))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.countReceiveMessage(REC_ACK, 0))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    CATerminate();
}
#endif

/**
 * @since 2015-09-08
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @see  CAResult_t CACreateEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CAEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test unicast message sending functionality using invalid method
 * @target CAResult_t CASendRequest(const CAEndpoint_t *object, CARequestInfo_t *requestInfo)
 * @test_data Invalid method code in CAMethod_t
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                 2. [Server] Register the handlers by CARegisterHandler
 *                 3. [Server] Select BLE network
 *                 4. [Server] Start Server
 *                 5. [Client] Initialize CA by CAInitialize
 *                 6. [Client] Register the handlers by CARegisterHandler
 *                 7. [Client] Select BLE network
 *                 8. [Client] Start Client
 *                 9. [Client] Send multicast request to find server
 *                10. [Server] server will receive the multicast request and return a response to client
 *                11. [Client] Create endpoint for server to send unicast request to server
 *                12. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] A client sends a request to server
 *             2. [Server] Server receives the request and return a response message to client
 *             3. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client]Terminate CA
 * @expected  The received amount should be zero
 */
#if defined(__TIZEN__) && defined(__BLE__)
TEST_F(CALeClientTest_stc, LeSendRequestWithInvalidMethod_N)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.establishConnectionWithServer())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    CAMethod_t invalidMethod = -1;

    char* uri = (char*) m_caHelper.m_simulatorReqAckUri.c_str();
    char* payload = m_caHelper.s_tcInfo.identifier;

    if (!m_caHelper.sendRequest(uri, payload, invalidMethod, CA_MSG_NONCONFIRM, TOTAL_MESSAGE, CA_MEMORY_ALLOC_FAILED))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.countReceiveMessage(REC_ACK, 0))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    CATerminate();
}
#endif

/**
 * @since 2015-09-08
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @see  CAResult_t CACreateEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CAEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test unicast message sending functionality by sending out GET request messages
 * @target CAResult_t CASendRequest(const CAEndpoint_t *object, CARequestInfo_t *requestInfo)
 * @test_data Request messages with method GET
 * 
 * * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                   2. [Server] Register the handlers by CARegisterHandler
 *                   3. [Server] Select BLE network
 *                   4. [Server] Start Server
 *                   5. [Client] Initialize CA by CAInitialize
 *                   6. [Client] Register the handlers by CARegisterHandler
 *                   7. [Client] Select BLE network
 *                   8. [Client] Start Client
 *                   9. [Client] Send multicast request to find server
 *                  10. [Server] server will receive the multicast request and return a response to client
 *                  11. [Client] Create endpoint for server to send unicast request to server
 *                  12. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] A client sends a get request to server
 *             2. [Server] Server receives the request and return a response message to client
 *             3. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client]Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages
 */
#if defined(__TIZEN__) && defined(__BLE__)
TEST_F(CALeClientTest_stc, LeSendGetRequest_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.establishConnectionWithServer())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if (!m_caHelper.sendRequest(CA_GET, CA_MSG_NONCONFIRM, TOTAL_MESSAGE))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.countReceiveMessage(REC_ACK))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    CATerminate();
}
#endif

/**
 * @since 2015-09-08
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @see  CAResult_t CACreateEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CAEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test unicast message sending functionality by sending out PUT request messages
 * @target CAResult_t CASendRequest(const CAEndpoint_t *object, CARequestInfo_t *requestInfo)
 * @test_data Request messages with method PUT
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                 2. [Server] Register the handlers by CARegisterHandler
 *                 3. [Server] Select BLE network
 *                 4. [Server] Start Server
 *                 5. [Client] Initialize CA by CAInitialize
 *                 6. [Client] Register the handlers by CARegisterHandler
 *                 7. [Client] Select BLE network
 *                 8. [Client] Start Client
 *                 9. [Client] Send multicast request to find server
 *                10. [Server] server will receive the multicast request and return a response to client
 *                11. [Client] Create endpoint for server to send unicast request to server
 *                12. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] A client sends a put request to server
 *             2. [Server] Server receives the request and return a response message to client
 *             3. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client]Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages
 */
#if defined(__TIZEN__) && defined(__BLE__)
TEST_F(CALeClientTest_stc, LeSendPutRequest_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.establishConnectionWithServer())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if (!m_caHelper.sendRequest(CA_PUT, CA_MSG_NONCONFIRM, TOTAL_MESSAGE))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.countReceiveMessage(REC_ACK))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    CATerminate();
}
#endif

/**
 * @since 2015-09-08
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @see  CAResult_t CACreateEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CAEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test unicast message sending functionality by sending out POST request messages
 * @target CAResult_t CASendRequest(const CAEndpoint_t *object, CARequestInfo_t *requestInfo)
 * @test_data Request messages with method POST
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                 2. [Server] Register the handlers by CARegisterHandler
 *                 3. [Server] Select BLE network
 *                 4. [Server] Start Server
 *                 5. [Client] Initialize CA by CAInitialize
 *                 6. [Client] Register the handlers by CARegisterHandler
 *                 7. [Client] Select BLE network
 *                 8. [Client] Start Client
 *                 9. [Client] Send multicast request to find server
 *                10. [Server] server will receive the multicast request and return a response to client
 *                11. [Client] Create endpoint for server to send unicast request to server
 *                12. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] A client sends a post request to server
 *             2. [Server] Server receives the request and return a response message to client
 *             3. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client]Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages 
 */
#if defined(__TIZEN__) && defined(__BLE__)
TEST_F(CALeClientTest_stc, LeSendPostRequest_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.establishConnectionWithServer())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if (!m_caHelper.sendRequest(CA_POST, CA_MSG_NONCONFIRM, TOTAL_MESSAGE))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.countReceiveMessage(REC_ACK))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    CATerminate();
}
#endif

/**
 * @since 2015-09-08
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @see  CAResult_t CACreateEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CAEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test unicast message sending functionality by sending out DELETE request messages
 * @target CAResult_t CASendRequest(const CAEndpoint_t *object, CARequestInfo_t *requestInfo)
 * @test_data Request messages with method DELETE
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                 2. [Server] Register the handlers by CARegisterHandler
 *                 3. [Server] Select BLE network
 *                 4. [Server] Start Server
 *                 5. [Client] Initialize CA by CAInitialize
 *                 6. [Client] Register the handlers by CARegisterHandler
 *                 7. [Client] Select BLE network
 *                 8. [Client] Start Client
 *                 9. [Client] Send multicast request to find server
 *                10. [Server] server will receive the multicast request and return a response to client
 *                11. [Client] Create endpoint for server to send unicast request to server
 *                12. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] A client sends a delete request to server
 *             2. [Server] Server receives the request and return a response message to client
 *             3. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client]Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages
 */
#if defined(__TIZEN__) && defined(__BLE__)
TEST_F(CALeClientTest_stc, LeSendDeleteRequest_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.establishConnectionWithServer())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if (!m_caHelper.sendRequest(CA_DELETE, CA_MSG_NONCONFIRM, TOTAL_MESSAGE))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.countReceiveMessage(REC_ACK))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    CATerminate();
}
#endif

/**
 * @since 2015-09-08
 * @see  CAResult_t CAInitialize()
 * @see  CAResult_t CACreateEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CAEndpoint_t **object)
 * @see  CAResult_t CACreateEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CAEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test 'CARegisterHandler' by registering null response handler
 * @target CAResult_t CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @test_data null response handler
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                 2. [Server] Register the handlers by CARegisterHandler
 *                 3. [Server] Select BLE network
 *                 4. [Server] Start Server
 *                 5. [Client] Initialize CA by CAInitialize
 *                 6. [Client] Register the response handlers with a null handler
 *                 7. [Client] Select BLE network
 *                 8. [Client] Start Client
 *                 9. [Client] Send multicast request to find server
 *                10. [Server] server will receive the multicast request and return a response to client
 *                11. [Client] Create endpoint for server to send unicast request to server
 *                12. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] A client sends a request to server
 *             2. [Server] Server receives the request and return a response message to client
 *             3. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client]Terminate CA
 * @expected No response messages should be received
 */
#if defined(__TIZEN__) && defined(__BLE__)
TEST_F(CALeClientTest_stc, LeReceiveWithNullResponseHandler_N)
{
    m_caHelper.setupTestCase(MESSAGE_INCOMING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.initNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    CARegisterHandler(CAHelper::requestHandler, NULL, CAHelper::errorHandler);

    if (!m_caHelper.sendConfigurationRequest(SEND_MESSAGE, MESSAGE_RESPONSE, CA_PUT))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.countReceiveMessage(REC_NOR, 0))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    CATerminate();
}
#endif

/**
 * @since 2015-09-08
 * @see  CAResult_t CAInitialize()
 * @see  CAResult_t CACreateEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CAEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  void CATerminate()
 * @objective Test the impact or consequence of not calling CARegisterHandler Test in receiving response
 * @target CAResult_t CAHandleRequestResponse()
 * @test_data none
* @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                2. [Server] Register the handlers by CARegisterHandler
 *                3. [Server] Select BLE network
 *                4. [Server] Start Server
 *                5. [Client] Initialize CA by CAInitialize           
 *                6. [Client] Select BLE network
 *                7. [Client] Start Client
 *                8. [Client] Send multicast request to find server
 *                9. [Server] server will receive the multicast request and return a response to client
 *               10. [Client] Create endpoint for server to send unicast request to server
 *               11. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] A client sends a request to server
 *             2. [Server] Server receives the request and return a response message to client
 *             3. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client]Terminate CA
 * @expected No response messages should be received
 */
#if defined(__TIZEN__) && defined(__BLE__)
TEST_F(CALeClientTest_stc, LeReceiveResponseWithoutCallingCARegisterHandler_N)
{
    m_caHelper.setupTestCase(MESSAGE_INCOMING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.initNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if (!m_caHelper.sendConfigurationRequest(SEND_MESSAGE, MESSAGE_RESPONSE, CA_PUT))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.countReceiveMessage(REC_NOR, 0))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    CATerminate();
}
#endif

/**
 * @since 2015-09-08
 * @see  CAResult_t CAInitialize()
 * @see  CAResult_t CACreateEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CAEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  CAResult_t CASendRequest(const CAEndpoint_t *object, CARequestInfo_t *requestInfo)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test 'CARegisterHandler' when registration is done twice by using two different response handler
 * @target CAResult_t CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @test_data two different response handler
 * * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                   2. [Server] Register the handlers by CARegisterHandler
 *                   3. [Server] Select BLE network
 *                   4. [Server] Start Server
 *                   5. [Client] Initialize CA by CAInitialize
 *                   6. [Client] Select BLE network
 *                   7. [Client] Start Client
 *                   8. [Client] Send multicast request to find server
 *                   9. [Server] server will receive the multicast request and return a response to client
 *                  10. [Client] Create endpoint for server to send unicast request to server
 *                  11. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] Call CARegisterHandler twice with two different Response Handler
 *             2. [Client] A client sends a request to server
 *             3. [Server] Server receives the request and return a response message to client
 *             4. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client]Terminate CA
 * @expected Second handler will receive requested amount of response messages and first handler will receive no response message
 */
#if defined(__TIZEN__) && defined(__BLE__)
TEST_F(CALeClientTest_stc, LeReceiveWithSecondResponseHandler_P)
{
    m_caHelper.setupTestCase(MESSAGE_INCOMING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.initNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    CARegisterHandler(CAHelper::requestHandler, CAHelper::responseHandler, CAHelper::errorHandler);

    CARegisterHandler(CAHelper::requestHandler, CAHelper::responseHandlerSecond, CAHelper::errorHandler);

    if (!m_caHelper.startDiscoveryServer())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.sendConfigurationRequest(SEND_MESSAGE, MESSAGE_RESPONSE, CA_PUT))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    m_caHelper.attemptReceiveMessage(REC_SECOND_NOR);

    if (m_caHelper.m_result != CA_STATUS_OK || CAHelper::getReceiveCount(REC_NOR) != 0
            || CAHelper::getReceiveCount(REC_SECOND_NOR) != TOTAL_MESSAGE)
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    CATerminate();
}
#endif

/**
 * @since 2015-09-08
 * @see  CAResult_t CAInitialize()
 * @see  CAResult_t CACreateEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CAEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  CAResult_t CASendRequest(const CAEndpoint_t *object, CARequestInfo_t *requestInfo)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test 'CARegisterHandler' when registration is done twice by using first a valid response handler then a null response handler
 * @target CAResult_t CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @test_data valid response handler & null response handler
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                 2. [Server] Register the handlers by CARegisterHandler
 *                 3. [Server] Select BLE network
 *                 4. [Server] Start Server
 *                 5. [Client] Initialize CA by CAInitialize
 *                 6. [Client] Select BLE network
 *                 7. [Client] Start Client
 *                 8. [Client] Send multicast request to find server
 *                 9. [Server] server will receive the multicast request and return a response to client
 *                10. [Client] Create endpoint for server to send unicast request to server
 *                11. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] Call CARegisterHandler twice by using first a valid response handler then a null response handler
 *             2. [Client] A client sends a request to server
 *             3. [Server] Server receives the request and return a response message to client
 *             4. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client] Terminate CA
 * @expected No response messages should be received
 */
#if defined(__TIZEN__) && defined(__BLE__)
TEST_F(CALeClientTest_stc, LeReceiveAfterRegisteringWithValidFollowedByNullResponseHandler_N)
{
    m_caHelper.setupTestCase(MESSAGE_INCOMING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.initNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    CARegisterHandler(CAHelper::requestHandler, CAHelper::responseHandler, CAHelper::errorHandler);

    CARegisterHandler(CAHelper::requestHandler, NULL, CAHelper::errorHandler);

    if (!m_caHelper.sendConfigurationRequest(SEND_MESSAGE, MESSAGE_RESPONSE, CA_PUT))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.countReceiveMessage(REC_NOR, 0))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    CATerminate();
}
#endif

/**
 * @since 2016-02-23
 * @see  CAResult_t CAInitialize()
 * @see  CAResult_t CASelectNetwork(const CATransportAdapter_t interestedNetwork)
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @see  CAResult_t CACreateEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CAEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Receive request message when unavailable network is selected
 * @target    CAResult_t CAUnSelectNetwork(CATransportAdapter_t nonInterestedNetwork)
 * @test_data Request Messages
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                 2. [Server] Register the handlers by CARegisterHandler
 *                 3. [Server] Select BLE network
 *                 4. [Server] Start Server
 *                 5. [Client] Initialize CA by CAInitialize
 *                 6. [Client] Register the handlers by CARegisterHandler
 *                 7. [Client] Select BLE network
 *                 8. [Client] Start Client
 *                 9. [Client] Send multicast request to find server
 *                10. [Server] server will receive the multicast request and return a response to client
 *                11. [Client] Create endpoint for server to send unicast request to server
 *                12. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] Stop listening server
 *             2. [Client] Call CAUnselectNetwork
 *             3. [Client] A client sends a request to server
 *             4. [Server] Server receives the request and return a response message to client
 *             5. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client]Terminate CA
 * @expected  The requested amount of request messages should be received
 */
#if defined(__TIZEN__) && defined(__BLE__)
TEST_F(CALeClientTest_stc, LeUnSelectNetworkAndSendMessage_STLSEC_N)
{
    m_caHelper.setupTestCase(MESSAGE_INCOMING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.establishConnectionWithServer())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if (!m_caHelper.stopListeningServer())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.unselectNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    if (m_caHelper.sendConfigurationRequest(SEND_MESSAGE, MESSAGE_RESPONSE, CA_PUT))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    m_caHelper.destroyEndpoint();
    m_caHelper.destroyToken();
    CATerminate();
}
#endif

/**
 * @since 2015-09-08
 * @see  CAResult_t CAInitialize()
 * @see  CAResult_t CACreateEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CAEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test 'CARegisterHandler' effectiveness in sending request by without calling it.
 * @target CAResult_t CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @test_data none
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                 2. [Server] Register the handlers by CARegisterHandler
 *                 3. [Server] Select BLE network
 *                 4. [Server] Start Server
 *                 5. [Client] Initialize CA by CAInitialize
 *                 6. [Client] Select BLE network
 *                 7. [Client] Start Client
 *                 8. [Client] Send multicast request to find server
 *                 9. [Server] server will receive the multicast request and return a response to client
 *                10. [Client] Create endpoint for server to send unicast request to server
 *                11. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] A client sends a request to server
 *             2. [Server] Server receives the request and return a response message to client
 *             3. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client]Terminate CA
 * @expected No request messages should be received
 */
#if defined(__TIZEN__) && defined(__BLE__)
TEST_F(CALeClientTest_stc, LeReceiveRequestWithoutCallingRegisterHandler_N)
{
    m_caHelper.setupTestCase(MESSAGE_INCOMING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.initNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if (!m_caHelper.sendConfigurationRequest(SEND_MESSAGE, MESSAGE_REQUEST, CA_PUT))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.countReceiveMessage(REC_NOR, 0))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    CATerminate();
}
#endif

/**
 * @since 2015-09-08
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
 * @see  CAResult_t CACreateEndpoint(const CAURI_t uri, const CAConnectivityType_t connectivityType, CAEndpoint_t **object)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test unicast message receiving request messages
 * @target CAResult_t CAInitialize()
 * @test_data Request Messages
 * @pre_condition  1. [Server] Initialize CA by CAInitialize
 *                 2. [Server] Register the handlers by CARegisterHandler
 *                 3. [Server] Select BLE network
 *                 4. [Server] Start Server
 *                 5. [Client] Initialize CA by CAInitialize
 *                 6. [Client] Register the handlers by CARegisterHandler
 *                 7. [Client] Select BLE network
 *                 8. [Client] Start Client
 *                 9. [Client] Send multicast request to find server
 *                10. [Server] server will receive the multicast request and return a response to client
 *                11. [Client] Create endpoint for server to send unicast request to server
 *                12. [Client] Generate a token and set this token in request message
 * @procedure  1. [Client] A client sends a request to server
 *             2. [Server] Server receives the request and return a response message to client
 *             3. [Client] Client receive the server's response
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client]Terminate CA
 * @expected  The requested amount of request messages should be received by the simulator
 */
#if defined(__TIZEN__) && defined(__BLE__)
TEST_F(CALeClientTest_stc, LeReceiveRequest_P)
{
    m_caHelper.setupTestCase(MESSAGE_INCOMING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.initClientNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if (!m_caHelper.sendConfigurationRequest(SEND_MESSAGE, MESSAGE_REQUEST, CA_PUT))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.countReceiveMessage(REC_NOR))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    CATerminate();
}
#endif
