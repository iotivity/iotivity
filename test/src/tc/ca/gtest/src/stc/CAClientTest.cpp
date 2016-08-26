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

class CAClientTest_stc: public ::testing::Test
{
protected:
    CAHelper m_caHelper;

    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();
        if (!m_caHelper.setConfigFile())
        {
            SET_FAILURE("Unable to read configuration file");
            return;
        }
    }

    virtual void TearDown()
    {
        CommonUtil::runCommonTCTearDownPart();
    }
};

/**
 * @since 2014-12-11
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @see  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Check the functionality of CASelectNetwork to receive response message when available network is selected
 * @target  CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @test_data Response Messages
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CAStartListeningServer to start server
 *                 5. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 6. [Client] Call CAInitialize to initialize CA
 *                 7. [Client] Select Network by CASelectNetwork API
 *                 8. [Client] Call CARegisterHandler to register handler
 *                 9. [Client] Call CAStartDiscoveryServer
 *                 10.[Client] Call CAGenerateToken to generate token
 *                 11.[Client] Call CACreateEndpoint to create endpoint for server
 * @procedure  1. [Server] Configure the server simulator with a request to send back a certain amount of response messages
 *             2. [Client] Attempt to receive the requested amount of unicast response messages from the server simulator
 *             3. [Client] Check and compare the received amount with the requested amount
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [CLient] Terminate CA
 * @expected  The requested amount of response messages should be received in client side
 */
#if ((defined(__LINUX__) || defined(__ANDROID__)) && defined(__ALL_TRANSPORT__)) || (defined(__TIZEN__) && (defined(__IP__) || defined(__BT__) || defined(__TCP__)))
TEST_F(CAClientTest_stc, SelectNetworkForIncomingMessages_P)
{
    m_caHelper.setupTestCase(MESSAGE_INCOMING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.initClientNetwork())
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
 * @since 2014-12-11
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @see  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Check the functionality of CASelectNetwork & CAUnSelectNetwork to receive response message when no network is selected
 * @target CAResult_t CASelectNetwork(CATransportAdapter_t interestedNetwork)
 * @target CAResult_t CAUnSelectNetwork(CATransportAdapter_t nonInterestedNetwork)
 * @test_data Response Messages
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CAStartListeningServer to start server
 *                 5. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 6. [Client] Call CAInitialize to initialize CA
 *                 7. [Client] Select Network by CASelectNetwork API
 *                 8. [Client] Call CARegisterHandler to register handler
 *                 9. [Client] Call CAStartDiscoveryServer
 *                 10.[Client] Call CAGenerateToken to generate token
 *                 11.[Client] Call CACreateEndpoint to create endpoint for server
 * @procedure  1. [Server] Configure the server simulator with a request to send back a certain amount of response messages
 *             2. [Client] UnSelect the network
 *             3. [Client] Attempt to receive the requested amount of unicast response messages from the server simulator
 *             4. [Client] Check and compare the received amount with zero
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [Client] Terminate CA
 * @expected  The received amount should be zero in client side
 */
#if ((defined(__LINUX__) || defined(__ANDROID__)) && defined(__ALL_TRANSPORT__)) || (defined(__TIZEN__) && (defined(__IP__) || defined(__BT__) || defined(__TCP__)))
TEST_F(CAClientTest_stc, SelectUnAvailableNetworkForIncomingMessages_N)
{
    m_caHelper.setupTestCase(MESSAGE_INCOMING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.initClientNetwork())
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

    if (!m_caHelper.countReceiveMessage(REC_NOR, 0))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    CATerminate();
}
#endif

/**
 * @since 2014-12-11
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @see  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CASendRequest(const CAEndpoint_t *object, const CARequestInfo_t *requestInfo)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Check the functionality of CASelectNetwork to send message when available network is selected
 * @target  CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @test_data Request Messages
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CAStartListeningServer to start server
 *                 5. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 6. [Client] Call CAInitialize to initialize CA
 *                 7. [Client] Select Network by CASelectNetwork API
 *                 8. [Client] Call CARegisterHandler to register handler
 *                 9. [Client] Call CAStartDiscoveryServer
 *                 10.[Client] Call CAGenerateToken to generate token
 *                 11.[Client] Call CACreateEndpoint to create endpoint for server
 * @procedure  1. [Client] Attempt to send a certain amount of request messages to the server simulator
 *             2. [Client] Attempt to receive acknowledgments for all the sent messages
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [CLient] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages
 */
#if ((defined(__LINUX__) || defined(__ANDROID__)) && defined(__ALL_TRANSPORT__)) || (defined(__TIZEN__) && (defined(__IP__) || defined(__BT__) || defined(__TCP__)))
TEST_F(CAClientTest_stc, SelectNetworkForOutgoingMessages_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.initClientNetwork())
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
 * @since 2015-01-01
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @see  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test the effect of select/unselect network on incoming unicast messages in a loop
 * @target  CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @target CAResult_t CAUnSelectNetwork(CATransportAdapter_t nonInterestedNetwork)
 * @test_data Response Messages
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CAStartListeningServer to start server
 *                 5. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 6. [Client] Call CAInitialize to initialize CA
 *                 7. [Client] Call CARegisterHandler to register handler
 *                 8. [Client] Call CAStartDiscoveryServer
 * @procedure  1. [Client] Select Network by CASelectNetwork API
 *             2. [Client] Call CAGenerateToken to generate token
 *             3. [Client] Call CACreateEndpoint to create endpoint for server
 *             4. [Server] Configure the server simulator with a request to send back a certain amount of response messages
 *             5. [Client] Attempt to receive the requested amount of unicast response messages from the server simulator
 *             6. [Client] Check and compare the received amount with the requested amount
 *             7. [Client] Destroy token
 *             8. [Client] Destroy endpoint
 *             9. [CLient] Unselect the network
 *             10.Repeat procedure 1 to 8 multiple times
 * @post_condition  [CLient] Terminate CA
 * @expected  The requested amount of response messages should be received each time in client side
 */
#if ((defined(__LINUX__) || defined(__ANDROID__)) && defined(__ALL_TRANSPORT__)) || (defined(__TIZEN__) && (defined(__IP__) || defined(__BT__) || defined(__TCP__)))
TEST_F(CAClientTest_stc, SelectNetworkSequentiallyForIncomingMessages_P)
{
    m_caHelper.setupTestCase(MESSAGE_INCOMING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.initClientNetwork())
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

            if(!m_caHelper.startDiscoveryServer())
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
 * @since 2015-01-01
 * @see  CAResult_t CAInitialize()
 * @see  void CATerminate()
 * @objective Test the effect of selecting network and receving incoming unicast messages for multiple times
 * @target  CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @target CAResult_t CAUnSelectNetwork(CATransportAdapter_t nonInterestedNetwork)
 * @target  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
 * @target  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @target  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @target  void CADestroyEndpoint(CAEndpoint_t *object)
 * @target  void CADestroyToken(CAToken_t token)
 * @target  CAResult_t CAHandleRequestResponse()
 * @test_data Response Messages
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CAStartListeningServer to start server
 *                 5. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 6. [Client] Call CAInitialize to initialize CA
 *                 7. [Client] Call CARegisterHandler to register handler
 *                 8. [Client] Call CAStartDiscoveryServer
 * @procedure  1. [Client] Select Network by CASelectNetwork API
 *             2. [Client] Call CAGenerateToken to generate token
 *             3. [Client] Call CACreateEndpoint to create endpoint for server
 *             4. [Server] Configure the server simulator with a request to send back a certain amount of response messages
 *             5. [Client] Attempt to receive the requested amount of unicast response messages from the server simulator
 *             6. [Client] Check and compare the received amount with the requested amount
 *             7. [Client] Destroy token
 *             8. [Client] Destroy endpoint
 *             9. Repeat procedure 1 to 8 multiple times
 * @post_condition  [CLient] Terminate CA
 * @expected  The requested amount of response messages should be received each time in client side
 */
#if ((defined(__LINUX__) || defined(__ANDROID__)) && defined(__ALL_TRANSPORT__)) || (defined(__TIZEN__) && (defined(__IP__) || defined(__BT__) || defined(__TCP__)))
TEST_F(CAClientTest_stc, SelectNetworkMultipleTimesForIncomingMessages_P)
{
    m_caHelper.setupTestCase(MESSAGE_INCOMING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.initClientNetwork())
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
 * @since 2015-09-18
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @see  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test unicast message sending functionality by sending out request messages for multiple times
 * @target  CAResult_t CASendRequest(const CAEndpoint_t *object, const CARequestInfo_t *requestInfo)
 * @test_data Request Messages
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CAStartListeningServer to start server
 *                 5. [Server] Call CAHandleRequestResponse periodically to recieve client request
 * @procedure  1. [Client] Call CAInitialize to initialize CA
 *             2. [Client] Select Network by CASelectNetwork API
 *             3. [Client] Call CARegisterHandler to register handler
 *             4. [Client] Call CAStartDiscoveryServer
 *             5. [Client] Call CAGenerateToken to generate token
 *             6. [Client] Call CACreateEndpoint to create endpoint for server
 *             7. [Client] Attempt to send a certain amount of request messages to the server simulator
 *             8. [Client] Attempt to receive acknowledgments for all the sent messages
 *             9. [Client] Check and compare the amount of received acknowledgments with the amount of sent message
 *             10.[Client] Destroy token
 *             11.[Client] Destroy endpoint
 *             12.[CLient] Terminate CA
 *             13.Repeat steps 1-12 multiple times
 * @post_condition  none
 * @expected  The number of acknowledgments should be equal to the amount of sent messages each times
 */
#if ((defined(__LINUX__) || defined(__ANDROID__)) && defined(__ALL_TRANSPORT__)) || (defined(__TIZEN__) && (defined(__IP__) || defined(__BT__) || defined(__TCP__)))
TEST_F(CAClientTest_stc, SendRequestMultipleTimes_P)
{
    for(int i = 0; i < TRY_COUNT; i++)
    {
        m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

        if (!m_caHelper.initClientNetwork())
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
 * @since 2014-12-05
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @see  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test unicast message sending functionality by sending out request messages without header option
 * @target  CAResult_t CASendRequest(const CAEndpoint_t *object, const CARequestInfo_t *requestInfo)
 * @test_data Request Messages
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CAStartListeningServer to start server
 *                 5. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 6. [Client] Call CAInitialize to initialize CA
 *                 7. [Client] Select Network by CASelectNetwork API
 *                 8. [Client] Call CARegisterHandler to register handler
 *                 9. [Client] Call CAStartDiscoveryServer
 *                 10.[Client] Call CAGenerateToken to generate token
 *                 11.[Client] Call CACreateEndpoint to create endpoint for server
 * @procedure  1. [Client] Attempt to send a certain amount of request messages to the server simulator using no header option
 *             2. [Client] Attempt to receive acknowledgments for all the sent messages
 *             3. [Client] Check and compare the amount of received acknowledgments with the amount of sent messages
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [CLient] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages and received header options should be same NULL
 */
#if ((defined(__LINUX__) || defined(__ANDROID__)) && defined(__ALL_TRANSPORT__)) || (defined(__TIZEN__) && (defined(__IP__) || defined(__BT__) || defined(__TCP__)))
TEST_F(CAClientTest_stc, SendRequestWithoutHeaderOption_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_HEADER, MESSAGE_UNICAST);

    if (!m_caHelper.initClientNetwork())
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
 * @since 2014-12-05
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @see  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test unicast message sending functionality by sending out request messages with header option
 * @target  CAResult_t CASendRequest(const CAEndpoint_t *object, const CARequestInfo_t *requestInfo)
 * @test_data Request Messages
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CAStartListeningServer to start server
 *                 5. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 6. [Client] Call CAInitialize to initialize CA
 *                 7. [Client] Select Network by CASelectNetwork API
 *                 8. [Client] Call CARegisterHandler to register handler
 *                 9. [Client] Call CAStartDiscoveryServer
 *                 10.[Client] Call CAGenerateToken to generate token
 *                 11.[Client] Call CACreateEndpoint to create endpoint for server
 * @procedure  1. [Client] Attempt to send a certain amount of request messages to the server simulator using header option
 *             2. [Client] Attempt to receive acknowledgments for all the sent messages
 *             3. [Client] Check and compare the amount of received acknowledgments with the amount of sent messages
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [CLient] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages and received header option should be same as send header option
 */
#if ((defined(__LINUX__) || defined(__ANDROID__)) && defined(__ALL_TRANSPORT__)) || (defined(__TIZEN__) && (defined(__IP__) || defined(__BT__) || defined(__TCP__)))
TEST_F(CAClientTest_stc, SendRequestWithHeaderOption_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_HEADER, MESSAGE_UNICAST);

    if (!m_caHelper.initClientNetwork())
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
 * @since 2014-12-05
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @see  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test unicast message sending functionality by sending out request messages with header options
 * @target  CAResult_t CASendRequest(const CAEndpoint_t *object, const CARequestInfo_t *requestInfo)
 * @test_data Request Messages
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CAStartListeningServer to start server
 *                 5. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 6. [Client] Call CAInitialize to initialize CA
 *                 7. [Client] Select Network by CASelectNetwork API
 *                 8. [Client] Call CARegisterHandler to register handler
 *                 9. [Client] Call CAStartDiscoveryServer
 *                 10.[Client] Call CAGenerateToken to generate token
 *                 11.[Client] Call CACreateEndpoint to create endpoint for server
 * @procedure  1. [Client] Attempt to send a certain amount of request messages to the server simulator using header options
 *             2. [Client] Attempt to receive acknowledgments for all the sent messages
 *             3. [Client] Check and compare the amount of received acknowledgments with the amount of sent messages
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [CLient] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages and received header option should be same as send header options
 */
#if ((defined(__LINUX__) || defined(__ANDROID__)) && defined(__ALL_TRANSPORT__)) || (defined(__TIZEN__) && (defined(__IP__) || defined(__BT__) || defined(__TCP__)))
TEST_F(CAClientTest_stc, SendRequestWithHeaderOptions_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_HEADER, MESSAGE_UNICAST);

    if (!m_caHelper.initClientNetwork())
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
 * @since 2014-12-24
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @see  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test unicast message sending functionality by sending out request messages with empty payloads
 * @target  CAResult_t CASendRequest(const CAEndpoint_t *object, const CARequestInfo_t *requestInfo)
 * @test_data Empty Payloads
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CAStartListeningServer to start server
 *                 5. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 6. [Client] Call CAInitialize to initialize CA
 *                 7. [Client] Select Network by CASelectNetwork API
 *                 8. [Client] Call CARegisterHandler to register handler
 *                 9. [Client] Call CAStartDiscoveryServer
 *                 10.[Client] Call CAGenerateToken to generate token
 *                 11.[Client] Call CACreateEndpoint to create endpoint for server
 * @procedure  1. [Client] Keep the payload empty while forming the CARequestInfo_t
 *             2. [Client] Attempt to send a certain amount of request messages to the server simulator using header options
 *             3. [Client] Attempt to receive acknowledgments for all the sent messages
 *             4. [Client] Check and compare the amount of received acknowledgments with the amount of sent messages
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [CLient] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages
 */
#if ((defined(__LINUX__) || defined(__ANDROID__)) && defined(__ALL_TRANSPORT__)) || (defined(__TIZEN__) && (defined(__IP__) || defined(__BT__) || defined(__TCP__)))
TEST_F(CAClientTest_stc, SendRequestWithEmptyPayload_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_URI, MESSAGE_UNICAST);

    if (!m_caHelper.initClientNetwork())
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
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @see  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test unicast message sending functionality by sending out request messages with null payloads
 * @target  CAResult_t CASendRequest(const CAEndpoint_t *object, const CARequestInfo_t *requestInfo)
 * @test_data NULL Payloads
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CAStartListeningServer to start server
 *                 5. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 6. [Client] Call CAInitialize to initialize CA
 *                 7. [Client] Select Network by CASelectNetwork API
 *                 8. [Client] Call CARegisterHandler to register handler
 *                 9. [Client] Call CAStartDiscoveryServer
 *                 10.[Client] Call CAGenerateToken to generate token
 *                 11.[Client] Call CACreateEndpoint to create endpoint for server
 * @procedure  1. [Client] Keep the payload null while forming the CARequestInfo_t
 *             2. [Client] Attempt to send a certain amount of request messages to the server simulator using header options
 *             3. [Client] Attempt to receive acknowledgments for all the sent messages
 *             4. [Client] Check and compare the amount of received acknowledgments with the amount of sent messages
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [CLient] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages
 */
#if ((defined(__LINUX__) || defined(__ANDROID__)) && defined(__ALL_TRANSPORT__)) || (defined(__TIZEN__) && (defined(__IP__) || defined(__BT__) || defined(__TCP__)))
TEST_F(CAClientTest_stc, SendRequestWithNullPayload_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_URI, MESSAGE_UNICAST);

    if (!m_caHelper.initClientNetwork())
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
 * @since 2014-12-24
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @see  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test unicast message retransmission functionality using message type CA_MSG_CONFIRM
 * @target  CAResult_t CASendRequest(const CAEndpoint_t *object, const CARequestInfo_t *requestInfo)
 * @test_data Request message of type CA_MSG_CONFIRM
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CAStartListeningServer to start server
 *                 5. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 6. [Client] Call CAInitialize to initialize CA
 *                 7. [Client] Select Network by CASelectNetwork API
 *                 8. [Client] Call CARegisterHandler to register handler
 *                 9. [Client] Call CAStartDiscoveryServer
 *                 10.[Client] Call CAGenerateToken to generate token
 *                 11.[Client] Call CACreateEndpoint to create endpoint for server
 * @procedure  1. [Client] Set CAMessageType_t as CA_MSG_CONFIRM while forming CAInfo_t
 *             2. [Client] Attempt to send 1 request, the server simulator will ignore it
 *             3. [Client] Auto retransmission should occur
 *             4. [Server] The server simulator is expected to respond to it with type CA_MSG_ACKNOWLEDGE
 *             5. [Client] Attempt to receive the acknowledgement from server
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [CLient] Terminate CA
 * @expected  The request message should be acknowledged
 */
#if (defined(__LINUX__) || defined(__ANDROID__)) && defined(__IP__) || (defined(__TIZEN__) && (defined(__IP__) || defined(__BT__)))
TEST_F(CAClientTest_stc, SendRequestWithMessageTypeConfirm_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.initClientNetwork())
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
 * @since 2014-12-11
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @see  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test unicast message sending functionality using wrong endpoint
 * @target  CAResult_t CASendRequest(const CAEndpoint_t *object, const CARequestInfo_t *requestInfo)
 * @test_data Wrong endpoint URI
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CAStartListeningServer to start server
 *                 5. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 6. [Client] Call CAInitialize to initialize CA
 *                 7. [Client] Select Network by CASelectNetwork API
 *                 8. [Client] Call CARegisterHandler to register handler
 *                 9. [Client] Call CAStartDiscoveryServer
 *                 10.[Client] Call CAGenerateToken to generate token
 *                 11.[Client] Call CACreateEndpoint to create endpoint from a wrong URI which does not belong to the server
 * @procedure  1. [Client] Attempt to send a certain amount of request messages to the server
 *             2. [Client] Attempt to receive acknowledgments for all the sent messages
 *             3. [Client] Check and compare the amount of received acknowledgments with zero
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [CLient] Terminate CA
 * @expected  The received amount should be zero
 */
#if ((defined(__LINUX__) || defined(__ANDROID__)) && defined(__ALL_TRANSPORT__)) || (defined(__TIZEN__) && (defined(__IP__) || defined(__BT__) || defined(__TCP__)))
TEST_F(CAClientTest_stc, SendRequestWithWrongEndpoint_N)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.initClientNetwork())
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
 * @since 2014-12-08
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @see  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test unicast message sending functionality by sending out GET request messages
 * @target  CAResult_t CASendRequest(const CAEndpoint_t *object, const CARequestInfo_t *requestInfo)
 * @test_data Request messages with method GET
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CAStartListeningServer to start server
 *                 5. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 6. [Client] Call CAInitialize to initialize CA
 *                 7. [Client] Select Network by CASelectNetwork API
 *                 8. [Client] Call CARegisterHandler to register handler
 *                 9. [Client] Call CAStartDiscoveryServer
 *                 10.[Client] Call CAGenerateToken to generate token
 *                 11.[Client] Call CACreateEndpoint to create endpoint for server
 * @procedure  1. [Client] Set CA_GET for CAMethod_t while forming CARequestInfo_t
 *             2. [Client] Attempt to send a certain amount of request messages to the server
 *             3. [Client] Attempt to receive acknowledgments for all the sent messages
 *             4. [Client] Check and compare the amount of received acknowledgments with the amount of sent messages
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [CLient] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages
 */
#if ((defined(__LINUX__) || defined(__ANDROID__)) && defined(__ALL_TRANSPORT__)) || (defined(__TIZEN__) && (defined(__IP__) || defined(__BT__) || defined(__TCP__)))
TEST_F(CAClientTest_stc, SendGetRequest_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.initClientNetwork())
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
 * @since 2014-12-08
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @see  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test unicast message sending functionality by sending out PUT request messages
 * @target  CAResult_t CASendRequest(const CAEndpoint_t *object, const CARequestInfo_t *requestInfo)
 * @test_data Request messages with method PUT
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CAStartListeningServer to start server
 *                 5. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 6. [Client] Call CAInitialize to initialize CA
 *                 7. [Client] Select Network by CASelectNetwork API
 *                 8. [Client] Call CARegisterHandler to register handler
 *                 9. [Client] Call CAStartDiscoveryServer
 *                 10.[Client] Call CAGenerateToken to generate token
 *                 11.[Client] Call CACreateEndpoint to create endpoint for server
 * @procedure  1. [Client] Set CA_PUT for CAMethod_t while forming CARequestInfo_t
 *             2. [Client] Attempt to send a certain amount of request messages to the server
 *             3. [Client] Attempt to receive acknowledgments for all the sent messages
 *             4. [Client] Check and compare the amount of received acknowledgments with the amount of sent messages
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [CLient] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages
 */
#if ((defined(__LINUX__) || defined(__ANDROID__)) && defined(__ALL_TRANSPORT__)) || (defined(__TIZEN__) && (defined(__IP__) || defined(__BT__) || defined(__TCP__)))
TEST_F(CAClientTest_stc, SendPutRequest_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.initClientNetwork())
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
 * @since 2014-12-24
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @see  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test unicast message sending functionality by sending out POST request messages
 * @target  CAResult_t CASendRequest(const CAEndpoint_t *object, const CARequestInfo_t *requestInfo)
 * @test_data Request messages with method POST
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CAStartListeningServer to start server
 *                 5. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 6. [Client] Call CAInitialize to initialize CA
 *                 7. [Client] Select Network by CASelectNetwork API
 *                 8. [Client] Call CARegisterHandler to register handler
 *                 9. [Client] Call CAStartDiscoveryServer
 *                 10.[Client] Call CAGenerateToken to generate token
 *                 11.[Client] Call CACreateEndpoint to create endpoint for server
 * @procedure  1. [Client] Set CA_POST for CAMethod_t while forming CARequestInfo_t
 *             2. [Client] Attempt to send a certain amount of request messages to the server
 *             3. [Client] Attempt to receive acknowledgments for all the sent messages
 *             4. [Client] Check and compare the amount of received acknowledgments with the amount of sent messages
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [CLient] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages
 */
#if ((defined(__LINUX__) || defined(__ANDROID__)) && defined(__ALL_TRANSPORT__)) || (defined(__TIZEN__) && (defined(__IP__) || defined(__BT__) || defined(__TCP__)))
TEST_F(CAClientTest_stc, SendPostRequest_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.initClientNetwork())
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
 * @since 2014-12-24
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @see  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test unicast message sending functionality by sending out DELETE request messages
 * @target  CAResult_t CASendRequest(const CAEndpoint_t *object, const CARequestInfo_t *requestInfo)
 * @test_data Request messages with method DELETE
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CAStartListeningServer to start server
 *                 5. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 6. [Client] Call CAInitialize to initialize CA
 *                 7. [Client] Select Network by CASelectNetwork API
 *                 8. [Client] Call CARegisterHandler to register handler
 *                 9. [Client] Call CAStartDiscoveryServer
 *                 10.[Client] Call CAGenerateToken to generate token
 *                 11.[Client] Call CACreateEndpoint to create endpoint for server
 * @procedure  1. [Client] Set CA_DELETE for CAMethod_t while forming CARequestInfo_t
 *             2. [Client] Attempt to send a certain amount of request messages to the server
 *             3. [Client] Attempt to receive acknowledgments for all the sent messages
 *             4. [Client] Check and compare the amount of received acknowledgments with the amount of sent messages
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [CLient] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages
 */
#if ((defined(__LINUX__) || defined(__ANDROID__)) && defined(__ALL_TRANSPORT__)) || (defined(__TIZEN__) && (defined(__IP__) || defined(__BT__) || defined(__TCP__)))
TEST_F(CAClientTest_stc, SendDeleteRequest_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.initClientNetwork())
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
 * @since 2015-01-15
 * @see  CAResult_t CAInitialize()
 * @see  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @see  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @see CAResult_t CASendRequest(const CAEndpoint_t *object, const CARequestInfo_t *requestInfo)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test 'CARegisterHandler' by registering null response handler
 * @target  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
 * @test_data null response handler
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CAStartListeningServer to start server
 *                 5. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 6. [Client] Call CAInitialize to initialize CA
 *                 7. [Client] Select Network by CASelectNetwork API
 *                 8. [Client] Call CAGenerateToken to generate token
 *                 9. [Client] Call CACreateEndpoint to create endpoint for server
 * @procedure  1. [Client] Call CARegisterHandler with Null Response Handler
 *             2. [Server] Configure the server with a request to send back a certain amount of response messages
 *             3. [Client] Attempt to receive a certain amount of unicast response messages from the server
 *             4. [Client] Check and compare the received amount with zero
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [CLient] Terminate CA
 * @expected  No response messages should be received
 */
#if ((defined(__LINUX__) || defined(__ANDROID__)) && defined(__ALL_TRANSPORT__)) || (defined(__TIZEN__) && (defined(__IP__) || defined(__BT__) || defined(__TCP__)))
TEST_F(CAClientTest_stc, ReceiveWithNullResponseHandler_N)
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
 * @since 2015-01-15
 * @see  CAResult_t CAInitialize()
 * @see  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @see  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @see CAResult_t CASendRequest(const CAEndpoint_t *object, const CARequestInfo_t *requestInfo)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  void CATerminate()
 * @objective Test the impact or consequence of not calling CARegisterHandler Test in receiving response
 * @target  CAResult_t CAHandleRequestResponse()
 * @test_data none
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CAStartListeningServer to start server
 *                 5. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 6. [Client] Call CAInitialize to initialize CA
 *                 7. [Client] Select Network by CASelectNetwork API
 *                 8. [Client] Do not Register the handler
 *                 9. [Client] Call CAGenerateToken to generate token
 *                 10. [Client] Call CACreateEndpoint to create endpoint for server
 * @procedure  1. [Server] Configure the server with a request to send back a certain amount of response messages
 *             2. [Client] Attempt to receive the requested amount of unicast response messages from the server
 *             3. [Client] Check and compare the received amount with zero
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [CLient] Terminate CA
 * @expected  No response messages should be received
 */
#if ((defined(__LINUX__) || defined(__ANDROID__)) && defined(__ALL_TRANSPORT__)) || (defined(__TIZEN__) && (defined(__IP__) || defined(__BT__) || defined(__TCP__)))
TEST_F(CAClientTest_stc, ReceiveResponseWithoutCallingCARegisterHandler_N)
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
 * @since 2015-01-15
 * @see  CAResult_t CAInitialize()
 * @see  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @see  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @see CAResult_t CASendRequest(const CAEndpoint_t *object, const CARequestInfo_t *requestInfo)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test 'CARegisterHandler' when registration is done twice by using two different response handler
 * @target  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
 * @test_data two different response handler
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CAStartListeningServer to start server
 *                 5. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 6. [Client] Call CAInitialize to initialize CA
 *                 7. [Client] Select Network by CASelectNetwork API
 * @procedure  1. [Client] Call CARegisterHandler twice with two different Response Handler
 *             2. [Client] Call CAStartDiscoveryServer
 *             3. [Client] Call CAGenerateToken to generate token
 *             4. [Client] Call CACreateEndpoint to create endpoint for server
 *             5. [Server] Configure the server with a request to send back a certain amount of response messages
 *             6. [Client] Attempt to receive the requested amount of unicast response messages from the server
 *             7. [Client] Check and compare the received amount by the both handler with the requested amount
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [CLient] Terminate CA
 * @expected  Second handler will receive requested amount of response messages and first handler will receive no response message
 */
#if ((defined(__LINUX__) || defined(__ANDROID__)) && defined(__ALL_TRANSPORT__)) || (defined(__TIZEN__) && (defined(__IP__) || defined(__BT__) || defined(__TCP__)))
TEST_F(CAClientTest_stc, ReceiveWithSecondResponseHandler_P)
{
    m_caHelper.setupTestCase(MESSAGE_INCOMING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.initNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    CARegisterHandler(CAHelper::requestHandler, CAHelper::responseHandler, CAHelper::errorHandler);

    CARegisterHandler(CAHelper::requestHandler, CAHelper::requestHandlerSecond, CAHelper::errorHandler);

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
 * @since 2015-01-15
 * @see  CAResult_t CAInitialize()
 * @see  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @see  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @see CAResult_t CASendRequest(const CAEndpoint_t *object, const CARequestInfo_t *requestInfo)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test 'CARegisterHandler' by doing registration twice using first a valid response handler then a null response handler
 * @target  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
 * @test_data valid response handler & null response handler
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CAStartListeningServer to start server
 *                 5. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 6. [Client] Call CAInitialize to initialize CA
 *                 7. [Client] Select Network by CASelectNetwork API
 * @procedure  1. [Client] Call CARegisterHandler twice by using first a valid response handler then a null response handler
 *             2. [Client] Call CAStartDiscoveryServer
 *             3. [Client] Call CAGenerateToken to generate token
 *             4. [Client] Call CACreateEndpoint to create endpoint for server
 *             5. [Server] Configure the server with a request to send back a certain amount of response messages
 *             6. [Client] Attempt to receive the requested amount of unicast response messages from the server
 *             7. [Client] Check and compare the received amount wit zero
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [CLient] Terminate CA
 * @expected  No response messages should be received
 */
#if ((defined(__LINUX__) || defined(__ANDROID__)) && defined(__ALL_TRANSPORT__)) || (defined(__TIZEN__) && (defined(__IP__) || defined(__BT__) || defined(__TCP__)))
TEST_F(CAClientTest_stc, ReceiveAfterRegisteringWithValidFollowedByNullResponseHandler_N)
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
 * @since 2015-06-29
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @see  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test blockwise unicast message sending functionality by sending out get non confirm request messages with large payload
 * @target  CAResult_t CASendRequest(const CAEndpoint_t *object, const CARequestInfo_t *requestInfo)
 * @test_data Request Messages of CA_GET as method & CA_MSG_NONCONFIRM as message type with payload > 1 kB
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CAStartListeningServer to start server
 *                 5. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 6. [Client] Call CAInitialize to initialize CA
 *                 7. [Client] Select Network by CASelectNetwork API
 *                 8. [Client] Call CARegisterHandler to register handler
 *                 9. [Client] Call CAStartDiscoveryServer
 *                 10.[Client] Call CAGenerateToken to generate token
 *                 11.[Client] Call CACreateEndpoint to create endpoint for server
 * @procedure  1. [Client] Set CA_GET for CAMethod_t and message type as CA_MSG_NONCONFIRM while forming CARequestInfo_t
 *             2. [Client] Attempt to send a certain amount of request messages to the server
 *             3. [Client] Attempt to receive acknowledgments for all the sent messages
 *             4. [Client] Check and compare the amount of received acknowledgments with the amount of sent messages
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [CLient] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages
 */
#if (defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)) && defined(__IP__)
TEST_F(CAClientTest_stc, SendNonGetRequestWithLargePayload_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.initClientNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    char* uri = (char*) m_caHelper.m_simulatorReqAckUri.c_str();
    char* payload = (char*) m_caHelper.getRandomString(BLOCKWISE_PACKET_SIZE).c_str();

    if (!m_caHelper.sendRequest(uri, payload, CA_GET, CA_MSG_NONCONFIRM, TOTAL_MESSAGE))
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
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @see  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test blockwise unicast message sending functionality by sending out post non confirm request messages with large payload
 * @target  CAResult_t CASendRequest(const CAEndpoint_t *object, const CARequestInfo_t *requestInfo)
 * @test_data Request Messages of CA_POST as method & CA_MSG_NONCONFIRM as message type with payload > 1 kB
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CAStartListeningServer to start server
 *                 5. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 6. [Client] Call CAInitialize to initialize CA
 *                 7. [Client] Select Network by CASelectNetwork API
 *                 8. [Client] Call CARegisterHandler to register handler
 *                 9. [Client] Call CAStartDiscoveryServer
 *                 10.[Client] Call CAGenerateToken to generate token
 *                 11.[Client] Call CACreateEndpoint to create endpoint for server
 * @procedure  1. [Client] Set CA_POST for CAMethod_t and message type as CA_MSG_NONCONFIRM while forming CARequestInfo_t
 *             2. [Client] Attempt to send a certain amount of request messages to the server
 *             3. [Client] Attempt to receive acknowledgments for all the sent messages
 *             4. [Client] Check and compare the amount of received acknowledgments with the amount of sent messages
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [CLient] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages
 */
#if (defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)) && defined(__IP__)
TEST_F(CAClientTest_stc, SendNonPostRequestWithLargePayload_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.initClientNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    char* uri = (char*) m_caHelper.m_simulatorReqAckUri.c_str();
    char* payload = (char*) m_caHelper.getRandomString(BLOCKWISE_PACKET_SIZE).c_str();

    if (!m_caHelper.sendRequest(uri, payload, CA_POST, CA_MSG_NONCONFIRM, TOTAL_MESSAGE))
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
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @see  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test blockwise unicast message sending functionality by sending out put non confirm request messages with large payload
 * @target  CAResult_t CASendRequest(const CAEndpoint_t *object, const CARequestInfo_t *requestInfo)
 * @test_data Request Messages of CA_PUT as method and CA_MSG_NONCONFIRM as message type with payload > 1 kB
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CAStartListeningServer to start server
 *                 5. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 6. [Client] Call CAInitialize to initialize CA
 *                 7. [Client] Select Network by CASelectNetwork API
 *                 8. [Client] Call CARegisterHandler to register handler
 *                 9. [Client] Call CAStartDiscoveryServer
 *                 10.[Client] Call CAGenerateToken to generate token
 *                 11.[Client] Call CACreateEndpoint to create endpoint for server
 * @procedure  1. [Client] Set CA_PUT for CAMethod_t and message type as CA_MSG_NONCONFIRM while forming CARequestInfo_t
 *             2. [Client] Attempt to send a certain amount of request messages to the server
 *             3. [Client] Attempt to receive acknowledgments for all the sent messages
 *             4. [Client] Check and compare the amount of received acknowledgments with the amount of sent messages
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [CLient] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages
 */
#if (defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)) && defined(__IP__)
TEST_F(CAClientTest_stc, SendNonPutRequestWithLargePayload_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.initClientNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    char* uri = (char*) m_caHelper.m_simulatorReqAckUri.c_str();
    char* payload = (char*) m_caHelper.getRandomString(BLOCKWISE_PACKET_SIZE).c_str();

    if (!m_caHelper.sendRequest(uri, payload, CA_PUT, CA_MSG_NONCONFIRM, TOTAL_MESSAGE))
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
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @see  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test blockwise unicast message sending functionality by sending out delete non confirm request messages with large payload
 * @target  CAResult_t CASendRequest(const CAEndpoint_t *object, const CARequestInfo_t *requestInfo)
 * @test_data Request Messages of CA_DELETE method and CA_MSG_NONCONFIRM type with payload > 1 kB
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CAStartListeningServer to start server
 *                 5. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 6. [Client] Call CAInitialize to initialize CA
 *                 7. [Client] Select Network by CASelectNetwork API
 *                 8. [Client] Call CARegisterHandler to register handler
 *                 9. [Client] Call CAStartDiscoveryServer
 *                 10.[Client] Call CAGenerateToken to generate token
 *                 11.[Client] Call CACreateEndpoint to create endpoint for server
 * @procedure  1. [Client] Set CA_DELETE for CAMethod_t and message type as CA_MSG_NONCONFIRM while forming CARequestInfo_t
 *             2. [Client] Attempt to send a certain amount of request messages to the server
 *             3. [Client] Attempt to receive acknowledgments for all the sent messages
 *             4. [Client] Check and compare the amount of received acknowledgments with the amount of sent messages
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [CLient] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages
 */
#if (defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)) && defined(__IP__)
TEST_F(CAClientTest_stc, SendNonDeleteRequestWithLargePayload_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.initClientNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    char* uri = (char*) m_caHelper.m_simulatorReqAckUri.c_str();
    char* payload = (char*) m_caHelper.getRandomString(BLOCKWISE_PACKET_SIZE).c_str();

    if (!m_caHelper.sendRequest(uri, payload, CA_DELETE, CA_MSG_NONCONFIRM, TOTAL_MESSAGE))
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
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @see  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test blockwise unicast message retransmission functionality using message type CA_MSG_CONFIRM and large payload
 * @target  CAResult_t CASendRequest(const CAEndpoint_t *object, const CARequestInfo_t *requestInfo)
 * @test_data Request message of type CA_MSG_CONFIRM with payload > 1 kB
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CAStartListeningServer to start server
 *                 5. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 6. [Client] Call CAInitialize to initialize CA
 *                 7. [Client] Select Network by CASelectNetwork API
 *                 8. [Client] Call CARegisterHandler to register handler
 *                 9. [Client] Call CAStartDiscoveryServer
 *                 10.[Client] Call CAGenerateToken to generate token
 *                 11.[Client] Call CACreateEndpoint to create endpoint for server
 * @procedure  1. [Client] Set CAMessageType_t as CA_MSG_CONFIRM while forming CAInfo_t
 *             2. [Client] Attempt to send 1 request, the server simulator will ignore it
 *             3. [Client] Auto retransmission should occur
 *             4. [Server] The server is expected to respond to it with type CA_MSG_ACKNOWLEDGE
 *             5. [Client] Attempt to receive the acknowledgement from server
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [CLient] Terminate CA
 * @expected  The request message should be acknowledged
 */
#if (defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)) && defined(__IP__)
TEST_F(CAClientTest_stc, SendConGetRequestWithLargePayload_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.initClientNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    char* uri = (char*) m_caHelper.m_simulatorReqAckUri.c_str();
    char* payload = (char*) m_caHelper.getRandomString(BLOCKWISE_PACKET_SIZE).c_str();

    if (!m_caHelper.sendRequest(uri, payload, CA_GET, CA_MSG_CONFIRM, 1))
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
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @see  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test blockwise unicast message retransmission functionality using CA_MSG_CONFIRM message & CA_POST method with large payload
 * @target  CAResult_t CASendRequest(const CAEndpoint_t *object, const CARequestInfo_t *requestInfo)
 * @test_data Request message of type CA_MSG_CONFIRM with payload > 1 kB
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CAStartListeningServer to start server
 *                 5. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 6. [Client] Call CAInitialize to initialize CA
 *                 7. [Client] Select Network by CASelectNetwork API
 *                 8. [Client] Call CARegisterHandler to register handler
 *                 9. [Client] Call CAStartDiscoveryServer
 *                 10.[Client] Call CAGenerateToken to generate token
 *                 11.[Client] Call CACreateEndpoint to create endpoint for server
 * @procedure  1. [Client] Set CAMessageType_t as CA_MSG_CONFIRM and CAMethod_t as CA_POST while forming CAInfo_t
 *             2. [Client] Attempt to send 1 request, the server simulator will ignore it
 *             3. [Client] Auto retransmission should occur
 *             4. [Server] The server is expected to respond to it with type CA_MSG_ACKNOWLEDGE
 *             5. [Client] Attempt to receive the acknowledgement from server
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [CLient] Terminate CA
 * @expected  The request message should be acknowledged
 */
#if (defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)) && defined(__IP__)
TEST_F(CAClientTest_stc, SendConPostRequestWithLargePayload_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.initClientNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    char* uri = (char*) m_caHelper.m_simulatorReqAckUri.c_str();
    char* payload = (char*) m_caHelper.getRandomString(BLOCKWISE_PACKET_SIZE).c_str();

    if (!m_caHelper.sendRequest(uri, payload, CA_POST, CA_MSG_CONFIRM, 1))
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
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @see  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test blockwise unicast message retransmission functionality using CA_MSG_CONFIRM message & CA_PUT method with large payload
 * @target  CAResult_t CASendRequest(const CAEndpoint_t *object, const CARequestInfo_t *requestInfo)
 * @test_data Request message of type CA_MSG_CONFIRM with payload > 1 kB
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CAStartListeningServer to start server
 *                 5. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 6. [Client] Call CAInitialize to initialize CA
 *                 7. [Client] Select Network by CASelectNetwork API
 *                 8. [Client] Call CARegisterHandler to register handler
 *                 9. [Client] Call CAStartDiscoveryServer
 *                 10.[Client] Call CAGenerateToken to generate token
 *                 11.[Client] Call CACreateEndpoint to create endpoint for server
 * @procedure  1. [Client] Set CAMessageType_t as CA_MSG_CONFIRM and CAMethod_t as CA_PUT while forming CAInfo_t
 *             2. [Client] Attempt to send 1 request, the server simulator will ignore it
 *             3. [Client] Auto retransmission should occur
 *             4. [Server] The server is expected to respond to it with type CA_MSG_ACKNOWLEDGE
 *             5. [Client] Attempt to receive the acknowledgement from server
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [CLient] Terminate CA
 * @expected  The request message should be acknowledged
 */
#if (defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)) && defined(__IP__)
TEST_F(CAClientTest_stc, SendConPutRequestWithLargePayload_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.initClientNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    char* uri = (char*) m_caHelper.m_simulatorReqAckUri.c_str();
    char* payload = (char*) m_caHelper.getRandomString(BLOCKWISE_PACKET_SIZE).c_str();

    if (!m_caHelper.sendRequest(uri, payload, CA_PUT, CA_MSG_CONFIRM, 1))
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
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @see  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test blockwise unicast message retransmission functionality using confirm delete request message with large payload
 * @target  CAResult_t CASendRequest(const CAEndpoint_t *object, const CARequestInfo_t *requestInfo)
 * @test_data Request message of type CA_MSG_CONFIRM with payload > 1 kB
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CAStartListeningServer to start server
 *                 5. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 6. [Client] Call CAInitialize to initialize CA
 *                 7. [Client] Select Network by CASelectNetwork API
 *                 8. [Client] Call CARegisterHandler to register handler
 *                 9. [Client] Call CAStartDiscoveryServer
 *                 10.[Client] Call CAGenerateToken to generate token
 *                 11.[Client] Call CACreateEndpoint to create endpoint for server
 * @procedure  1. [Client] Set CAMessageType_t as CA_MSG_CONFIRM and CAMethod_t as CA_DELETE while forming CAInfo_t
 *             2. [Client] Attempt to send 1 request, the server simulator will ignore it
 *             3. [Client] Auto retransmission should occur
 *             4. [Server] The server is expected to respond to it with type CA_MSG_ACKNOWLEDGE
 *             5. [Client] Attempt to receive the acknowledgement from server
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [CLient] Terminate CA
 * @expected  The request message should be acknowledged
 */
#if (defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)) && defined(__IP__)
TEST_F(CAClientTest_stc, SendConDeleteRequestWithLargePayload_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.initClientNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    char* uri = (char*) m_caHelper.m_simulatorReqAckUri.c_str();
    char* payload = (char*) m_caHelper.getRandomString(BLOCKWISE_PACKET_SIZE).c_str();

    if (!m_caHelper.sendRequest(uri, payload, CA_DELETE, CA_MSG_CONFIRM, 1))
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

#ifdef __WITH_DTLS__

/**
 * @since 2015-02-13
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @see  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test CARegisterDTLSCredentialsHandler functionality by sending out non confirm GET request messages using a secure port
 * @target CAResult_t CARegisterDTLSCredentialsHandler(CAGetDTLSPskCredentialsHandler GetDTLSCredentials)
 * @target int (*CAGetDTLSPskCredentialsHandler)(CADtlsPskCredType_t type, const uint8_t *desc, size_t desc_len, uint8_t *result, size_t result_length)
 * @test_data Request messages of CA_GET as method
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CARegisterDTLSCredentialsHandler to set handler
 *                 4. [Server] Call CAStartListeningServer to start server
 *                 5. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 6. [Client] Call CAInitialize to initialize CA
 *                 7. [Client] Select Network by CASelectNetwork API
 *                 8. [Client] Call CARegisterHandler to register handler
 *                 9. [Client] Call CAStartDiscoveryServer
 *                 10. [Client] Call CAGenerateToken to generate token
 *                 11. [Client] Call CACreateEndpoint to create endpoint for server
 *                 12. [Client] Call CARegisterDTLSCredentialsHandler to set handler
 *                 13. [Client] Register Request Response Handler
 * @procedure  1. [Client] Attempt to send a certain amount of non confirm GET request messages to the server using secure port
 *             2. [Client] Attempt to receive acknowledgments for all the sent messages using that secure port
 *             3. [Client] Check and compare the amount of received acknowledgments with the amount of sent messages
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [CLient] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages each time
 */
#if (defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)) && defined(__IP__)
TEST_F(CAClientTest_stc, SendSecureGetRequest_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.initClientNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if (!m_caHelper.setDtls())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.sendSecuredRequest(CA_GET, CA_MSG_NONCONFIRM, TOTAL_MESSAGE))
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
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @see  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test CARegisterDTLSCredentialsHandler functionality by sending out non confirm POST request messages using a secure port
 * @target CAResult_t CARegisterDTLSCredentialsHandler(CAGetDTLSPskCredentialsHandler GetDTLSCredentials)
 * @target int (*CAGetDTLSPskCredentialsHandler)(CADtlsPskCredType_t type, const uint8_t *desc, size_t desc_len, uint8_t *result, size_t result_length)
 * @test_data Request messages of CA_POST as method
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CARegisterDTLSCredentialsHandler to set handler
 *                 5. [Server] Call CAStartListeningServer to start server
 *                 6. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 7. [Client] Call CAInitialize to initialize CA
 *                 8. [Client] Select Network by CASelectNetwork API
 *                 9. [Client] Call CARegisterHandler to register handler
 *                 10. [Client] Call CAStartDiscoveryServer
 *                 11. [Client] Call CAGenerateToken to generate token
 *                 12. [Client] Call CACreateEndpoint to create endpoint for server
 *                 13. [Client] Call CARegisterDTLSCredentialsHandler to get DTLS PSK credentials
 *                 14. [Client] Register Request Response Handler
 * @procedure  1. [Client] Attempt to send a certain amount of non confirm POST request messages to the server using secure port
 *             2. [Client] Attempt to receive acknowledgments for all the sent messages using that secure port
 *             3. [Client] Check and compare the amount of received acknowledgments with the amount of sent messages
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [CLient] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages each time
 */
#if (defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)) && defined(__IP__)
TEST_F(CAClientTest_stc, SendSecurePostRequest_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.initClientNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if (!m_caHelper.setDtls())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.sendSecuredRequest(CA_POST, CA_MSG_NONCONFIRM, TOTAL_MESSAGE))
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
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @see  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test CARegisterDTLSCredentialsHandler functionality by sending out non confirm PUT request messages using a secure port
 * @target CAResult_t CARegisterDTLSCredentialsHandler(CAGetDTLSPskCredentialsHandler GetDTLSCredentials)
 * @target int (*CAGetDTLSPskCredentialsHandler)(CADtlsPskCredType_t type, const uint8_t *desc, size_t desc_len, uint8_t *result, size_t result_length)
 * @test_data Request messages of CA_PUT as method
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CARegisterDTLSCredentialsHandler to set handler
 *                 5. [Server] Call CAStartListeningServer to start server
 *                 6. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 7. [Client] Call CAInitialize to initialize CA
 *                 8. [Client] Select Network by CASelectNetwork API
 *                 9. [Client] Call CARegisterHandler to register handler
 *                 10. [Client] Call CAStartDiscoveryServer
 *                 11. [Client] Call CAGenerateToken to generate token
 *                 12. [Client] Call CACreateEndpoint to create endpoint for server
 *                 13. [Client] Call CARegisterDTLSCredentialsHandler to get DTLS PSK credentials
 *                 14. [Client] Register Request Response Handler
 * @procedure  1. [Client] Attempt to send a certain amount of non confirm PUT request messages to the server using secure port
 *             2. [Client] Attempt to receive acknowledgments for all the sent messages using that secure port
 *             3. [Client] Check and compare the amount of received acknowledgments with the amount of sent messages
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [CLient] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages each time
 */
#if (defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)) && defined(__IP__)
TEST_F(CAClientTest_stc, SendSecurePutRequest_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.initClientNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if (!m_caHelper.setDtls())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.sendSecuredRequest(CA_PUT, CA_MSG_NONCONFIRM, TOTAL_MESSAGE))
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
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @see  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test CARegisterDTLSCredentialsHandler functionality by sending out non confirm DELETE request messages using a secure port
 * @target CAResult_t CARegisterDTLSCredentialsHandler(CAGetDTLSPskCredentialsHandler GetDTLSCredentials)
 * @target int (*CAGetDTLSPskCredentialsHandler)(CADtlsPskCredType_t type, const uint8_t *desc, size_t desc_len, uint8_t *result, size_t result_length)
 * @test_data Request messages of CA_DELETE as method
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CARegisterDTLSCredentialsHandler to set handler
 *                 5. [Server] Call CAStartListeningServer to start server
 *                 6. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 7. [Client] Call CAInitialize to initialize CA
 *                 8. [Client] Select Network by CASelectNetwork API
 *                 9. [Client] Call CARegisterHandler to register handler
 *                 10. [Client] Call CAStartDiscoveryServer
 *                 11. [Client] Call CAGenerateToken to generate token
 *                 12. [Client] Call CACreateEndpoint to create endpoint for server
 *                 13. [Client] Call CARegisterDTLSCredentialsHandler to get DTLS PSK credentials
 *                 14. [Client] Register Request Response Handler
 * @procedure  1. [Client] Attempt to send a certain amount of non confirm request messages to the server using secure port
 *             2. [Client] Attempt to receive acknowledgments for all the sent messages using that secure port
 *             3. [Client] Check and compare the amount of received acknowledgments with the amount of sent messages
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [CLient] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages each time
 */
#if (defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)) && defined(__IP__)
TEST_F(CAClientTest_stc, SendSecureDeleteRequest_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.initClientNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if (!m_caHelper.setDtls())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.sendSecuredRequest(CA_DELETE, CA_MSG_NONCONFIRM, TOTAL_MESSAGE))
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
 * @since 2015-02-13
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @see  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test CARegisterDTLSCredentialsHandler functionality by sending out confirm GET request messages using a secure port
 * @target CAResult_t CARegisterDTLSCredentialsHandler(CAGetDTLSPskCredentialsHandler GetDTLSCredentials)
 * @target int (*CAGetDTLSPskCredentialsHandler)(CADtlsPskCredType_t type, const uint8_t *desc, size_t desc_len, uint8_t *result, size_t result_length)
 * @test_data Request messages of CA_GET as method
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CARegisterDTLSCredentialsHandler to set handler
 *                 5. [Server] Call CAStartListeningServer to start server
 *                 6. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 7. [Client] Call CAInitialize to initialize CA
 *                 8. [Client] Select Network by CASelectNetwork API
 *                 9. [Client] Call CARegisterHandler to register handler
 *                 10. [Client] Call CAStartDiscoveryServer
 *                 11. [Client] Call CAGenerateToken to generate token
 *                 12. [Client] Call CACreateEndpoint to create endpoint for server
 *                 13. [Client] Call CARegisterDTLSCredentialsHandler to get DTLS PSK credentials
 *                 14. [Client] Register Request Response Handler
 * @procedure  1. [Client] Attempt to send a certain amount of confirm get request messages to the server using secure port
 *             2. [Client] Attempt to receive acknowledgments for all the sent messages using that secure port
 *             3. [Client] Check and compare the amount of received acknowledgments with the amount of sent messages
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [CLient] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages each time
 */
#if (defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)) && defined(__IP__)
TEST_F(CAClientTest_stc, SendSecureConfirmGetRequest_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.initClientNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if (!m_caHelper.setDtls())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.sendSecuredRequest(CA_GET, CA_MSG_CONFIRM, 1))
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
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @see  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test CARegisterDTLSCredentialsHandler functionality by sending out confirm POST request messages using a secure port
 * @target CAResult_t CARegisterDTLSCredentialsHandler(CAGetDTLSPskCredentialsHandler GetDTLSCredentials)
 * @target int (*CAGetDTLSPskCredentialsHandler)(CADtlsPskCredType_t type, const uint8_t *desc, size_t desc_len, uint8_t *result, size_t result_length)
 * @test_data Request messages of CA_POST as method
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CARegisterDTLSCredentialsHandler to set handler
 *                 5. [Server] Call CAStartListeningServer to start server
 *                 6. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 7. [Client] Call CAInitialize to initialize CA
 *                 8. [Client] Select Network by CASelectNetwork API
 *                 9. [Client] Call CARegisterHandler to register handler
 *                 10. [Client] Call CAStartDiscoveryServer
 *                 11. [Client] Call CAGenerateToken to generate token
 *                 12. [Client] Call CACreateEndpoint to create endpoint for server
 *                 13. [Client] Call CARegisterDTLSCredentialsHandler to get DTLS PSK credentials
 *                 14. [Client] Register Request Response Handler
 * @procedure  1. [Client] Attempt to send a certain amount of confirm POST request messages to the server using secure port
 *             2. [Client] Attempt to receive acknowledgments for all the sent messages using that secure port
 *             3. [Client] Check and compare the amount of received acknowledgments with the amount of sent messages
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [CLient] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages each time
 */
#if (defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)) && defined(__IP__)
TEST_F(CAClientTest_stc, SendSecureConfirmPostRequest_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.initClientNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if (!m_caHelper.setDtls())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.sendSecuredRequest(CA_POST, CA_MSG_CONFIRM, 1))
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
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @see  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test CARegisterDTLSCredentialsHandler functionality by sending out confirm PUT request messages using a secure port
 * @target CAResult_t CARegisterDTLSCredentialsHandler(CAGetDTLSPskCredentialsHandler GetDTLSCredentials)
 * @target int (*CAGetDTLSPskCredentialsHandler)(CADtlsPskCredType_t type, const uint8_t *desc, size_t desc_len, uint8_t *result, size_t result_length)
 * @test_data Request messages of CA_PUT as method
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CARegisterDTLSCredentialsHandler to set handler
 *                 5. [Server] Call CAStartListeningServer to start server
 *                 6. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 7. [Client] Call CAInitialize to initialize CA
 *                 8. [Client] Select Network by CASelectNetwork API
 *                 9. [Client] Call CARegisterHandler to register handler
 *                 10. [Client] Call CAStartDiscoveryServer
 *                 11. [Client] Call CAGenerateToken to generate token
 *                 12. [Client] Call CACreateEndpoint to create endpoint for server
 *                 13. [Client] Call CARegisterDTLSCredentialsHandler to get DTLS PSK credentials
 *                 14. [Client] Register Request Response Handler
 * @procedure  1. [Client] Attempt to send a certain amount of confirm PUT request messages to the server using secure port
 *             2. [Client] Attempt to receive acknowledgments for all the sent messages using that secure port
 *             3. [Client] Check and compare the amount of received acknowledgments with the amount of sent messages
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [CLient] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages each time
 */
#if (defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)) && defined(__IP__)
TEST_F(CAClientTest_stc, SendSecureConfirmPutRequest_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.initClientNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if (!m_caHelper.setDtls())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.sendSecuredRequest(CA_PUT, CA_MSG_CONFIRM, 1))
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
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @see  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test CARegisterDTLSCredentialsHandler functionality by sending out confirm DELETE request messages using a secure port
 * @target CAResult_t CARegisterDTLSCredentialsHandler(CAGetDTLSPskCredentialsHandler GetDTLSCredentials)
 * @target int (*CAGetDTLSPskCredentialsHandler)(CADtlsPskCredType_t type, const uint8_t *desc, size_t desc_len, uint8_t *result, size_t result_length)
 * @test_data Request messages of CA_DELETE as method
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CARegisterDTLSCredentialsHandler to set handler
 *                 5. [Server] Call CAStartListeningServer to start server
 *                 6. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 7. [Client] Call CAInitialize to initialize CA
 *                 8. [Client] Select Network by CASelectNetwork API
 *                 9. [Client] Call CARegisterHandler to register handler
 *                 10. [Client] Call CAStartDiscoveryServer
 *                 11. [Client] Call CAGenerateToken to generate token
 *                 12. [Client] Call CACreateEndpoint to create endpoint for server
 *                 13. [Client] Call CARegisterDTLSCredentialsHandler to get DTLS PSK credentials
 *                 14. [Client] Register Request Response Handler
 * @procedure  1. [Client] Attempt to send a certain amount of confirm DELETE request messages to the server using secure port
 *             2. [Client] Attempt to receive acknowledgments for all the sent messages using that secure port
 *             3. [Client] Check and compare the amount of received acknowledgments with the amount of sent messages
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [CLient] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages each time
 */
#if (defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)) && defined(__IP__)
TEST_F(CAClientTest_stc, SendSecureConfirmDeleteRequest_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.initClientNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if (!m_caHelper.setDtls())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.sendSecuredRequest(CA_DELETE, CA_MSG_CONFIRM, 1))
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
 * @since 2015-02-16
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @see  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test CARegisterDTLSCredentialsHandler functionality by without registering CAGetDTLSCredentialsHandler to send request messages using a secure port
 * @target CAResult_t CARegisterDTLSCredentialsHandler(CAGetDTLSPskCredentialsHandler GetDTLSCredentials)
 * @target int (*CAGetDTLSPskCredentialsHandler)(CADtlsPskCredType_t type, const uint8_t *desc, size_t desc_len, uint8_t *result, size_t result_length)
 * @test_data Request messages
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CARegisterDTLSCredentialsHandler to set handler
 *                 5. [Server] Call CAStartListeningServer to start server
 *                 6. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 7. [Client] Call CAInitialize to initialize CA
 *                 8. [Client] Select Network by CASelectNetwork API
 *                 9. [Client] Call CARegisterHandler to register handler
 *                 10. [Client] Call CAStartDiscoveryServer
 *                 11. [Client] Call CAGenerateToken to generate token
 *                 12. [Client] Call CACreateEndpoint to create endpoint for server
 *                 13. [Client] Register Request Response Handler
 * @procedure  1. [Client] Attempt to send a certain amount of request messages to the server using secure port
 *             2. [Client] Attempt to receive acknowledgments for all the sent messages using that secure port
 *             3. [Client] Check and compare the amount of received acknowledgments with the amount of sent messages
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [CLient] Terminate CA
 * @expected  No acknowledgment should be received
 */
#if (defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)) && defined(__IP__)
TEST_F(CAClientTest_stc, SendSecureRequestWithoutDtlsHandler_N)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.initClientNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if (!m_caHelper.sendSecuredRequest(CA_PUT, CA_MSG_NONCONFIRM, TOTAL_MESSAGE))
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
 * @since 2015-06-29
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @see  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test CARegisterDTLSCredentialsHandler functionality by sending request messages using a secure port using large payload
 * @target CAResult_t CARegisterDTLSCredentialsHandler(CAGetDTLSPskCredentialsHandler GetDTLSCredentials)
 * @target int (*CAGetDTLSPskCredentialsHandler)(CADtlsPskCredType_t type, const uint8_t *desc, size_t desc_len, uint8_t *result, size_t result_length)
 * @test_data Request messages with payload > 1 kB
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CARegisterDTLSCredentialsHandler to set handler
 *                 5. [Server] Call CAStartListeningServer to start server
 *                 6. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 7. [Client] Call CAInitialize to initialize CA
 *                 8. [Client] Select Network by CASelectNetwork API
 *                 9. [Client] Call CARegisterHandler to register handler
 *                 10. [Client] Call CAStartDiscoveryServer
 *                 11. [Client] Call CAGenerateToken to generate token
 *                 12. [Client] Call CACreateEndpoint to create endpoint for server
 *                 13. [Client] Register Request Response Handler
 * @procedure  1. [Client] Attempt to send a certain amount of non confirm get request messages to the server using secure port with large payload
 *             2. [Client] Attempt to receive acknowledgments for all the sent messages using that secure port
 *             3. [Client] Check and compare the amount of received acknowledgments with the amount of sent messages
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [CLient] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages each time
 */
#if (defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)) && defined(__IP__)
TEST_F(CAClientTest_stc, SendSecureNonGetRequestWithLargePayload_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.initClientNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if (!m_caHelper.setDtls())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    char* payload = (char*) m_caHelper.getRandomString(BLOCKWISE_PACKET_SIZE).c_str();

    if (!m_caHelper.sendSecuredRequest(payload, CA_GET, CA_MSG_NONCONFIRM, TOTAL_MESSAGE))
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
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @see  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test CARegisterDTLSCredentialsHandler functionality by sending out POST request messages using a secure port with large payload
 * @target CAResult_t CARegisterDTLSCredentialsHandler(CAGetDTLSPskCredentialsHandler GetDTLSCredentials)
 * @target int (*CAGetDTLSPskCredentialsHandler)(CADtlsPskCredType_t type, const uint8_t *desc, size_t desc_len, uint8_t *result, size_t result_length)
 * @test_data Request messages of CA_GET as method with payload > 1 kB
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CAStartListeningServer to start server
 *                 5. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 6. [Client] Call CAInitialize to initialize CA
 *                 7. [Client] Select Network by CASelectNetwork API
 *                 8. [Client] Call CARegisterHandler to register handler
 *                 9. [Client] Call CAStartDiscoveryServer
 *                 10. [Client] Call CAGenerateToken to generate token
 *                 11. [Client] Call CACreateEndpoint to create endpoint for server
 *                 12. [Client] Call CARegisterDTLSCredentialsHandler to get DTLS PSK credentials
 *                 13. [Client] Register Request Response Handler
 * @procedure  1. [Client] Attempt to send a certain amount of non confirm POST request messages to the server using secure port
 *             2. [Client] Attempt to receive acknowledgments for all the sent messages using that secure port
 *             3. [Client] Check and compare the amount of received acknowledgments with the amount of sent messages
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [CLient] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages each time
 */
#if (defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)) && defined(__IP__)
TEST_F(CAClientTest_stc, SendSecureNonPostRequestWithLargePayload_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.initClientNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if (!m_caHelper.setDtls())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    char* payload = (char*) m_caHelper.getRandomString(BLOCKWISE_PACKET_SIZE).c_str();

    if (!m_caHelper.sendSecuredRequest(payload, CA_POST, CA_MSG_NONCONFIRM, TOTAL_MESSAGE))
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
 * @since 2015-06-29
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @see  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test CARegisterDTLSCredentialsHandler functionality by sending PUT request messages using a secure port and large payload
 * @target CAResult_t CARegisterDTLSCredentialsHandler(CAGetDTLSPskCredentialsHandler GetDTLSCredentials)
 * @target int (*CAGetDTLSPskCredentialsHandler)(CADtlsPskCredType_t type, const uint8_t *desc, size_t desc_len, uint8_t *result, size_t result_length)
 * @test_data Request messages with payload > 1 kB
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CARegisterDTLSCredentialsHandler to set handler
 *                 5. [Server] Call CAStartListeningServer to start server
 *                 6. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 7. [Client] Call CAInitialize to initialize CA
 *                 8. [Client] Select Network by CASelectNetwork API
 *                 9. [Client] Call CARegisterHandler to register handler
 *                 10. [Client] Call CAStartDiscoveryServer
 *                 11. [Client] Call CAGenerateToken to generate token
 *                 12. [Client] Call CACreateEndpoint to create endpoint for server
 *                 13. [Client] Register Request Response Handler
 * @procedure  1. [Client] Attempt to send a certain amount of non confirm PUT request messages to the server using secure port with large payload
 *             2. [Client] Attempt to receive acknowledgments for all the sent messages using that secure port
 *             3. [Client] Check and compare the amount of received acknowledgments with the amount of sent messages
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [CLient] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages each time
 */
#if (defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)) && defined(__IP__)
TEST_F(CAClientTest_stc, SendSecureNonPutRequestWithLargePayload_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.initClientNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if (!m_caHelper.setDtls())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    char* payload = (char*) m_caHelper.getRandomString(BLOCKWISE_PACKET_SIZE).c_str();

    if (!m_caHelper.sendSecuredRequest(payload, CA_PUT, CA_MSG_NONCONFIRM, TOTAL_MESSAGE))
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
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @see  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test CARegisterDTLSCredentialsHandler functionality by sending DELETE request messages using a secure port and large payload
 * @target CAResult_t CARegisterDTLSCredentialsHandler(CAGetDTLSPskCredentialsHandler GetDTLSCredentials)
 * @target int (*CAGetDTLSPskCredentialsHandler)(CADtlsPskCredType_t type, const uint8_t *desc, size_t desc_len, uint8_t *result, size_t result_length)
 * @test_data Request messages with payload > 1 kB
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CARegisterDTLSCredentialsHandler to set handler
 *                 5. [Server] Call CAStartListeningServer to start server
 *                 6. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 7. [Client] Call CAInitialize to initialize CA
 *                 8. [Client] Select Network by CASelectNetwork API
 *                 9. [Client] Call CARegisterHandler to register handler
 *                 10. [Client] Call CAStartDiscoveryServer
 *                 11. [Client] Call CAGenerateToken to generate token
 *                 12. [Client] Call CACreateEndpoint to create endpoint for server
 *                 13. [Client] Register Request Response Handler
 * @procedure  1. [Client] Attempt to send a certain amount of non confirm DELETE request messages to the server using secure port with large payload
 *             2. [Client] Attempt to receive acknowledgments for all the sent messages using that secure port
 *             3. [Client] Check and compare the amount of received acknowledgments with the amount of sent messages
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [CLient] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages each time
 */
#if (defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)) && defined(__IP__)
TEST_F(CAClientTest_stc, SendSecureNonDeleteRequestWithLargePayload_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.initClientNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if (!m_caHelper.setDtls())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    char* payload = (char*) m_caHelper.getRandomString(BLOCKWISE_PACKET_SIZE).c_str();

    if (!m_caHelper.sendSecuredRequest(payload, CA_DELETE, CA_MSG_NONCONFIRM, TOTAL_MESSAGE))
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
 * @since 2015-06-29
 * @see  CAResult_t CAInitialize()
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @see  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test CARegisterDTLSCredentialsHandler functionality by sending confirm GET request messages using a secure port and large payload
 * @target CAResult_t CARegisterDTLSCredentialsHandler(CAGetDTLSPskCredentialsHandler GetDTLSCredentials)
 * @target int (*CAGetDTLSPskCredentialsHandler)(CADtlsPskCredType_t type, const uint8_t *desc, size_t desc_len, uint8_t *result, size_t result_length)
 * @test_data Request messages with payload > 1 kB
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CARegisterDTLSCredentialsHandler to set handler
 *                 5. [Server] Call CAStartListeningServer to start server
 *                 6. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 7. [Client] Call CAInitialize to initialize CA
 *                 8. [Client] Select Network by CASelectNetwork API
 *                 9. [Client] Call CARegisterHandler to register handler
 *                 10. [Client] Call CAStartDiscoveryServer
 *                 11. [Client] Call CAGenerateToken to generate token
 *                 12. [Client] Call CACreateEndpoint to create endpoint for server
 *                 13. [Client] Register Request Response Handler
 * @procedure  1. [Client] Attempt to send a certain amount of confirm GET request messages to the server using secure port with large payload
 *             2. [Client] Attempt to receive acknowledgments for all the sent messages using that secure port
 *             3. [Client] Check and compare the amount of received acknowledgments with the amount of sent messages
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [CLient] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages each time
 */
#if (defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)) && defined(__IP__)
TEST_F(CAClientTest_stc, SendSecureConGetRequestWithLargePayload_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.initClientNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if (!m_caHelper.setDtls())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    char* payload = (char*) m_caHelper.getRandomString(BLOCKWISE_PACKET_SIZE).c_str();

    if (!m_caHelper.sendSecuredRequest(payload, CA_GET, CA_MSG_CONFIRM, 1))
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
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @see  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test CARegisterDTLSCredentialsHandler functionality by sending confirm POST request messages using a secure port and large payload
 * @target CAResult_t CARegisterDTLSCredentialsHandler(CAGetDTLSPskCredentialsHandler GetDTLSCredentials)
 * @target int (*CAGetDTLSPskCredentialsHandler)(CADtlsPskCredType_t type, const uint8_t *desc, size_t desc_len, uint8_t *result, size_t result_length)
 * @test_data Request messages with payload > 1 kB
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CARegisterDTLSCredentialsHandler to set handler
 *                 5. [Server] Call CAStartListeningServer to start server
 *                 6. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 7. [Client] Call CAInitialize to initialize CA
 *                 8. [Client] Select Network by CASelectNetwork API
 *                 9. [Client] Call CARegisterHandler to register handler
 *                 10. [Client] Call CAStartDiscoveryServer
 *                 11. [Client] Call CAGenerateToken to generate token
 *                 12. [Client] Call CACreateEndpoint to create endpoint for server
 *                 13. [Client] Register Request Response Handler
 * @procedure  1. [Client] Attempt to send a certain amount of confirm POST request messages to the server using secure port with large payload
 *             2. [Client] Attempt to receive acknowledgments for all the sent messages using that secure port
 *             3. [Client] Check and compare the amount of received acknowledgments with the amount of sent messages
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [CLient] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages
 */
#if (defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)) && defined(__IP__)
TEST_F(CAClientTest_stc, SendSecureConPostRequestWithLargePayload_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.initClientNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if (!m_caHelper.setDtls())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    char* payload = (char*) m_caHelper.getRandomString(BLOCKWISE_PACKET_SIZE).c_str();

    if (!m_caHelper.sendSecuredRequest(payload, CA_POST, CA_MSG_CONFIRM, 1))
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
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @see  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test CARegisterDTLSCredentialsHandler functionality by sending confirm PUT request messages using a secure port and large payload
 * @target CAResult_t CARegisterDTLSCredentialsHandler(CAGetDTLSPskCredentialsHandler GetDTLSCredentials)
 * @target int (*CAGetDTLSPskCredentialsHandler)(CADtlsPskCredType_t type, const uint8_t *desc, size_t desc_len, uint8_t *result, size_t result_length)
 * @test_data Request messages with payload > 1 kB
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CARegisterDTLSCredentialsHandler to set handler
 *                 5. [Server] Call CAStartListeningServer to start server
 *                 6. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 7. [Client] Call CAInitialize to initialize CA
 *                 8. [Client] Select Network by CASelectNetwork API
 *                 9. [Client] Call CARegisterHandler to register handler
 *                 10. [Client] Call CAStartDiscoveryServer
 *                 11. [Client] Call CAGenerateToken to generate token
 *                 12. [Client] Call CACreateEndpoint to create endpoint for server
 *                 13. [Client] Register Request Response Handler
 * @procedure  1. [Client] Attempt to send a certain amount of confirm PUT request messages to the server using secure port with large payload
 *             2. [Client] Attempt to receive acknowledgments for all the sent messages using that secure port
 *             3. [Client] Check and compare the amount of received acknowledgments with the amount of sent messages
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [CLient] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages
 */
#if (defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)) && defined(__IP__)
TEST_F(CAClientTest_stc, SendSecureConPutRequestWithLargePayload_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.initClientNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if (!m_caHelper.setDtls())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    char* payload = (char*) m_caHelper.getRandomString(BLOCKWISE_PACKET_SIZE).c_str();

    if (!m_caHelper.sendSecuredRequest(payload, CA_PUT, CA_MSG_CONFIRM, 1))
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
 * @see  void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
 * @see  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @see  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  CAResult_t CAHandleRequestResponse()
 * @see  void CATerminate()
 * @objective Test CARegisterDTLSCredentialsHandler functionality by sending confirm DELETE request messages using a secure port and large payload
 * @target CAResult_t CARegisterDTLSCredentialsHandler(CAGetDTLSPskCredentialsHandler GetDTLSCredentials)
 * @target int (*CAGetDTLSPskCredentialsHandler)(CADtlsPskCredType_t type, const uint8_t *desc, size_t desc_len, uint8_t *result, size_t result_length)
 * @test_data Request messages with payload > 1 kB
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CARegisterDTLSCredentialsHandler to set handler
 *                 5. [Server] Call CAStartListeningServer to start server
 *                 6. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 7. [Client] Call CAInitialize to initialize CA
 *                 8. [Client] Select Network by CASelectNetwork API
 *                 9. [Client] Call CARegisterHandler to register handler
 *                 10. [Client] Call CAStartDiscoveryServer
 *                 11. [Client] Call CAGenerateToken to generate token
 *                 12. [Client] Call CACreateEndpoint to create endpoint for server
 *                 13. [Client] Register Request Response Handler
 * @procedure  1. [Client] Attempt to send a certain amount of confirm DELETE request messages to the server using secure port with large payload
 *             2. [Client] Attempt to receive acknowledgments for all the sent messages using that secure port
 *             3. [Client] Check and compare the amount of received acknowledgments with the amount of sent messages
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [CLient] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages
 */
#if (defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)) && defined(__IP__)
TEST_F(CAClientTest_stc, SendSecureConDeleteRequestWithLargePayload_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.initClientNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if (!m_caHelper.setDtls())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    char* payload = (char*) m_caHelper.getRandomString(BLOCKWISE_PACKET_SIZE).c_str();

    if (!m_caHelper.sendSecuredRequest(payload, CA_DELETE, CA_MSG_CONFIRM, 1))
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

#endif

/**
 * @since 2016-08-05
 * @see  CAResult_t CAInitialize()
 * @see  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @see  CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @see  void CADestroyEndpoint(CAEndpoint_t *object)
 * @see  void CADestroyToken(CAToken_t token)
 * @see  void CATerminate()
 * @objective Test CARegisterKeepAliveHandler functionality by sending message
 * @target void CARegisterKeepAliveHandler(CAKeepAliveConnectionCallback ConnHandler)
 * @test_data keepAliveHandler callback function 
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CAStartListeningServer to start server
 *                 5. [Client] Call CAInitialize to initialize CA
 *                 6. [Client] Select Network by CASelectNetwork API
 *                 7. [Client] Call CARegisterKeepAliveHandler to register handler
 *                 8. [Client] Call CAStartDiscoveryServer
 *                 9. [Client] Call CAGenerateToken to generate tokenl
 *                 10. [Client] Call CACreateEndpoint to create endpoint for server
 *                 11. [Client] Set keepAliveCount to zero
 * @procedure  1. [Client] Attempt to send a request messages to the server
 *             2. [Client] Wait few seconds
 *             3. [Client] Increament keepAliveCount in keepAliveHandler callback function
 * @post_condition  1. [Client] Destroy token
 *                  2. [Client] Destroy endpoint
 *                  3. [CLient] Terminate CA
 * @expected  keepAliveCount should be greater than zero
 */
#if (defined(__LINUX__) || defined(__ANDROID__)) && defined(__TCP__)
TEST_F(CAClientTest_stc, CACheckKeepAliveHandler_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_UNICAST);

    if (!m_caHelper.initClientNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    CARegisterKeepAliveHandler(CAHelper::keepAliveHandler);
    
    if (!m_caHelper.sendRequest(CA_GET, CA_MSG_NONCONFIRM, TOTAL_MESSAGE))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    CommonUtil::waitInSecond(2);
        
    if (m_caHelper.getKeepAliveCount()==0)
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }
        
    CATerminate();    
}
#endif
