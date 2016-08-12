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

class CAMulticastTest_stc: public ::testing::Test
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
 * @since 2014-12-31
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
 * @objective Test multicast message sending functionality by sending out GET request messages
 * @target  CAResult_t CASendRequest(const CAEndpoint_t *object, const CARequestInfo_t *requestInfo)
 * @test_data Request messages with method GET
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CAStartListeningServer to start server
 *                 5. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 6. Repeat steps 1~5 for multiple times in multiple devices
 *                 7. [Client] Call CAInitialize to initialize CA
 *                 8. [Client] Select Network by CASelectNetwork API
 *                 9. [Client] Call CARegisterHandler to register handler
 *                 10. [Client] Call CAStartDiscoveryServer
 *                 11. [Client] Call CAGenerateToken to generate token
 * @procedure  1. [Client] Set CA_GET for CAMethod_t while forming CARequestInfo_t
 *             2. [Client] Attempt to send a certain amount of request messages to the server
 *             3. [Client] Attempt to receive acknowledgments for all the sent messages
 *             4. [Client] Check and compare the amount of received acknowledgments with the amount of sent messages
 * @post_condition  1. [Client] Destroy token
 *                  2. [CLient] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages for each devices
 */
#if (defined(__LINUX__) && defined(__IP__)) || (defined(__TIZEN__) && defined(__ALL_TRANSPORT__)) || defined(__ANDROID__)
TEST_F(CAMulticastTest_stc, SendMulticastGetRequest_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_MULTICAST);

    if (!m_caHelper.initClientNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if (!m_caHelper.sendRequestToAll(CA_GET, CA_MSG_NONCONFIRM, TOTAL_MESSAGE))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.countMulticastReceiveMessage(REC_ACK))
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
 * @objective Test multicast message sending functionality by sending out POST request messages
 * @target  CAResult_t CASendRequest(const CAEndpoint_t *object, const CARequestInfo_t *requestInfo)
 * @test_data Request messages with method POST
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CAStartListeningServer to start server
 *                 5. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 6. Repeat steps 1~5 for multiple times in multiple devices
 *                 7. [Client] Call CAInitialize to initialize CA
 *                 8. [Client] Select Network by CASelectNetwork API
 *                 9. [Client] Call CARegisterHandler to register handler
 *                 10. [Client] Call CAStartDiscoveryServer
 *                 11.[Client] Call CAGenerateToken to generate token
 * @procedure  1. [Client] Set CA_POST for CAMethod_t while forming CARequestInfo_t
 *             2. [Client] Attempt to send a certain amount of request messages to the server
 *             3. [Client] Attempt to receive acknowledgments for all the sent messages
 *             4. [Client] Check and compare the amount of received acknowledgments with the amount of sent messages
 * @post_condition  1. [Client] Destroy token
 *                  2. [CLient] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages for each devices
 */
#if (defined(__LINUX__) && defined(__IP__)) || (defined(__TIZEN__) && defined(__ALL_TRANSPORT__)) || defined(__ANDROID__)
TEST_F(CAMulticastTest_stc, SendMulticastPostRequest_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_MULTICAST);

    if (!m_caHelper.initClientNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if (!m_caHelper.sendRequestToAll(CA_POST, CA_MSG_NONCONFIRM, TOTAL_MESSAGE))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.countMulticastReceiveMessage(REC_ACK))
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
 * @objective Test multicast message sending functionality by sending out PUT request messages
 * @target  CAResult_t CASendRequest(const CAEndpoint_t *object, const CARequestInfo_t *requestInfo)
 * @test_data Request messages with method PUT
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CAStartListeningServer to start server
 *                 5. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 6. Repeat steps 1~5 for multiple times in multiple devices
 *                 7. [Client] Call CAInitialize to initialize CA
 *                 8. [Client] Select Network by CASelectNetwork API
 *                 9. [Client] Call CARegisterHandler to register handler
 *                 10. [Client] Call CAStartDiscoveryServer
 *                 11.[Client] Call CAGenerateToken to generate token
 * @procedure  1. [Client] Set CA_PUT for CAMethod_t while forming CARequestInfo_t
 *             2. [Client] Attempt to send a certain amount of request messages to the server
 *             3. [Client] Attempt to receive acknowledgments for all the sent messages
 *             4. [Client] Check and compare the amount of received acknowledgments with the amount of sent messages
 * @post_condition  1. [Client] Destroy token
 *                  2. [CLient] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages for each devices
 */
#if (defined(__LINUX__) && defined(__IP__)) || (defined(__TIZEN__) && defined(__ALL_TRANSPORT__)) || defined(__ANDROID__)
TEST_F(CAMulticastTest_stc, SendMulticastPutRequest_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_MULTICAST);

    if (!m_caHelper.initClientNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if (!m_caHelper.sendRequestToAll(CA_PUT, CA_MSG_NONCONFIRM, TOTAL_MESSAGE))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.countMulticastReceiveMessage(REC_ACK))
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
 * @objective Test multicast message sending functionality by sending out DELETE request messages
 * @target  CAResult_t CASendRequest(const CAEndpoint_t *object, const CARequestInfo_t *requestInfo)
 * @test_data Request messages with method DELETE
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CAStartListeningServer to start server
 *                 5. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 6. Repeat steps 1~5 for multiple times in multiple devices
 *                 7. [Client] Call CAInitialize to initialize CA
 *                 8. [Client] Select Network by CASelectNetwork API
 *                 9. [Client] Call CARegisterHandler to register handler
 *                 10. [Client] Call CAStartDiscoveryServer
 *                 11.[Client] Call CAGenerateToken to generate token
 * @procedure  1. [Client] Set CA_DELETE for CAMethod_t while forming CARequestInfo_t
 *             2. [Client] Attempt to send a certain amount of request messages to the server
 *             3. [Client] Attempt to receive acknowledgments for all the sent messages
 *             4. [Client] Check and compare the amount of received acknowledgments with the amount of sent messages
 * @post_condition  1. [Client] Destroy token
 *                  2. [CLient] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages for each devices
 */
#if (defined(__LINUX__) && defined(__IP__)) || (defined(__TIZEN__) && defined(__ALL_TRANSPORT__)) || defined(__ANDROID__)
TEST_F(CAMulticastTest_stc, SendMulticastDeleteRequest_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_MULTICAST);

    if (!m_caHelper.initClientNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if (!m_caHelper.sendRequestToAll(CA_DELETE, CA_MSG_NONCONFIRM, TOTAL_MESSAGE))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.countMulticastReceiveMessage(REC_ACK))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    CATerminate();
}
#endif

/**
 * @since 2015-06-30
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
 *                 6. Repeat steps 1~5 for multiple times in multiple devices
 *                 7. [Client] Call CAInitialize to initialize CA
 *                 8. [Client] Select Network by CASelectNetwork API
 *                 9. [Client] Call CARegisterHandler to register handler
 *                 10. [Client] Call CAStartDiscoveryServer
 *                 11.[Client] Call CAGenerateToken to generate token
 * @procedure  1. [Client] Attempt to send a certain amount of request messages to the server simulator using no header option
 *             2. [Client] Attempt to receive acknowledgments for all the sent messages
 *             3. [Client] Check and compare the amount of received acknowledgments with the amount of sent messages
 * @post_condition  1. [Client] Destroy token
 *                  2. [CLient] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages and received header options should be same NULL for each devices
 */
#if (defined(__LINUX__) && defined(__IP__)) || (defined(__TIZEN__) && defined(__ALL_TRANSPORT__)) || defined(__ANDROID__)
TEST_F(CAMulticastTest_stc, SendMulticastRequestWithoutHeaderOption_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_HEADER, MESSAGE_MULTICAST);

    if (!m_caHelper.initClientNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    CAHelper::s_tcInfo.numOptions = 0;

    if (!m_caHelper.sendRequestToAll(CA_GET, CA_MSG_NONCONFIRM, TOTAL_MESSAGE))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.countMulticastReceiveMessage(REC_ACK))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    CATerminate();
}
#endif

/**
 * @since 2015-06-30
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
 *                 6. Repeat steps 1~5 for multiple times in multiple devices
 *                 7. [Client] Call CAInitialize to initialize CA
 *                 8. [Client] Select Network by CASelectNetwork API
 *                 9. [Client] Call CARegisterHandler to register handler
 *                 10. [Client] Call CAStartDiscoveryServer
 *                 11. [Client] Call CAGenerateToken to generate token
 * @procedure  1. [Client] Attempt to send a certain amount of request messages to the server simulator using header option
 *             2. [Client] Attempt to receive acknowledgments for all the sent messages
 *             3. [Client] Check and compare the amount of received acknowledgments with the amount of sent messages
 * @post_condition  1. [Client] Destroy token
 *                  2. [CLient] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages and received header option should be same as send header option for each devices
 */
#if (defined(__LINUX__) && defined(__IP__)) || (defined(__TIZEN__) && defined(__ALL_TRANSPORT__)) || defined(__ANDROID__)
TEST_F(CAMulticastTest_stc, SendMulticastRequestWithHeaderOption_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_HEADER, MESSAGE_MULTICAST);

    if (!m_caHelper.initClientNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    CAHelper::s_tcInfo.numOptions = 1;

    if (!m_caHelper.sendRequestToAll(CA_GET, CA_MSG_NONCONFIRM, TOTAL_MESSAGE))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.countMulticastReceiveMessage(REC_ACK))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    CATerminate();
}
#endif

/**
 * @since 2015-06-30
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
 *                 6. Repeat steps 1~5 for multiple times in multiple devices
 *                 7. [Client] Call CAInitialize to initialize CA
 *                 8. [Client] Select Network by CASelectNetwork API
 *                 9. [Client] Call CARegisterHandler to register handler
 *                 10. [Client] Call CAStartDiscoveryServer
 *                 11. [Client] Call CAGenerateToken to generate token
 * @procedure  1. [Client] Attempt to send a certain amount of request messages to the server simulator using header options
 *             2. [Client] Attempt to receive acknowledgments for all the sent messages
 *             3. [Client] Check and compare the amount of received acknowledgments with the amount of sent messages
 * @post_condition  1. [Client] Destroy token
 *                  2. [CLient] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages and received header option should be same as send header options for each devices
 */
#if (defined(__LINUX__) && defined(__IP__)) || (defined(__TIZEN__) && defined(__ALL_TRANSPORT__)) || defined(__ANDROID__)
TEST_F(CAMulticastTest_stc, SendMulticastRequestWithHeaderOptions_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_HEADER, MESSAGE_MULTICAST);

    if (!m_caHelper.initClientNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    CAHelper::s_tcInfo.numOptions = 2;

    if (!m_caHelper.sendRequestToAll(CA_GET, CA_MSG_NONCONFIRM, TOTAL_MESSAGE))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.countMulticastReceiveMessage(REC_ACK))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    CATerminate();
}
#endif

/**
 * @since 2014-12-31
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
 *                 6. Repeat steps 1~5 for multiple times in multiple devices
 *                 7. [Client] Call CAInitialize to initialize CA
 *                 8. [Client] Select Network by CASelectNetwork API
 *                 9. [Client] Call CARegisterHandler to register handler
 *                 10. [Client] Call CAStartDiscoveryServer
 *                 11. [Client] Call CAGenerateToken to generate token
 * @procedure  1. [Client] Keep the payload empty while forming the CARequestInfo_t
 *             2. [Client] Attempt to send a certain amount of request messages to the server simulator using header options
 *             3. [Client] Attempt to receive acknowledgments for all the sent messages
 *             4. [Client] Check and compare the amount of received acknowledgments with the amount of sent messages
 * @post_condition  1. [Client] Destroy token
 *                  2. [CLient] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages
 */
#if (defined(__LINUX__) && defined(__IP__)) || (defined(__TIZEN__) && defined(__ALL_TRANSPORT__)) || defined(__ANDROID__)
TEST_F(CAMulticastTest_stc, SendMulticastRequestWithEmptyPayload_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_URI, MESSAGE_MULTICAST);

    if (!m_caHelper.initClientNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    char* uri = (char*) m_caHelper.m_simulatorReqAckUri.c_str();
    char* payload = (char*)"";

    if (!m_caHelper.sendRequestToAll(uri, payload, CA_PUT, CA_MSG_NONCONFIRM, TOTAL_MESSAGE))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.countMulticastReceiveMessage(REC_ACK))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    CATerminate();
}
#endif

/**
 * @since 2014-12-31
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
 *                 6. Repeat steps 1~5 for multiple times in multiple devices
 *                 7. [Client] Call CAInitialize to initialize CA
 *                 8. [Client] Select Network by CASelectNetwork API
 *                 9. [Client] Call CARegisterHandler to register handler
 *                 10. [Client] Call CAStartDiscoveryServer
 *                 11. [Client] Call CAGenerateToken to generate token
 * @procedure  1. [Client] Keep the payload null while forming the CARequestInfo_t
 *             2. [Client] Attempt to send a certain amount of request messages to the server simulator using header options
 *             3. [Client] Attempt to receive acknowledgments for all the sent messages
 *             4. [Client] Check and compare the amount of received acknowledgments with the amount of sent messages
 * @post_condition  1. [Client] Destroy token
 *                  2. [CLient] Terminate CA
 * @expected  The number of acknowledgments should be equal to the amount of sent messages for each devices
 */
#if (defined(__LINUX__) && defined(__IP__)) || (defined(__TIZEN__) && defined(__ALL_TRANSPORT__)) || defined(__ANDROID__)
TEST_F(CAMulticastTest_stc, SendMulticastRequestWithNullPayload_P)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_URI, MESSAGE_MULTICAST);

    if (!m_caHelper.initClientNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    char* uri = (char*) m_caHelper.m_simulatorReqAckUri.c_str();
    char* payload = NULL;

    if (!m_caHelper.sendRequestToAll(uri, payload, CA_PUT, CA_MSG_NONCONFIRM, TOTAL_MESSAGE))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.countMulticastReceiveMessage(REC_ACK))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    CATerminate();
}
#endif

/**
 * @since 2014-12-31
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
 * @objective Test CASendRequest negatively to send multicast messages using invalid message type
 * @target CAResult_t CASendRequest(const CAEndpoint_t *object, const CARequestInfo_t *requestInfo)
 * @test_data Request messages
 * @pre_condition  1. [Server] Call CAInitialize to initialize CA
 *                 2. [Server] Select Network by CASelectNetwork API
 *                 3. [Server] Call CARegisterHandler to register handler
 *                 4. [Server] Call CAStartListeningServer to start server
 *                 5. [Server] Call CAHandleRequestResponse periodically to recieve client request
 *                 6. Repeat steps 1~5 for multiple times in multiple devices
 *                 7. [Client] Call CAInitialize to initialize CA
 *                 8. [Client] Select Network by CASelectNetwork API
 *                 9. [Client] Call CARegisterHandler to register handler
 *                 10. [Client] Call CAStartDiscoveryServer
 *                 11. [Client] Call CAGenerateToken to generate token
 * @procedure  1. [Client] Attempt to send a certain amount of request messages to the server simulator with invalid message type
 *             2. [Client] Attempt to receive acknowledgments for all the sent messages
 *             3. [Client] Check and compare the amount of received acknowledgments with the amount of sent messages
 * @post_condition  1. [Client] Destroy token
 *                  2. [CLient] Terminate CA
 * @expected  The number of acknowledgments should be zero for each devices
 */
#if (defined(__LINUX__) && defined(__IP__)) || (defined(__TIZEN__) && defined(__ALL_TRANSPORT__)) || defined(__ANDROID__)
TEST_F(CAMulticastTest_stc, SendMulticastRequestWithInvalidMethod_N)
{
    m_caHelper.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD, MESSAGE_MULTICAST);

    if (!m_caHelper.initClientNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    char* uri = (char*) m_caHelper.m_simulatorReqAckUri.c_str();
    char* payload = m_caHelper.s_tcInfo.identifier;

    if (!m_caHelper.sendRequestToAll(uri, payload, CA_INVALID_METHOD, CA_MSG_NONCONFIRM, 1, CA_MEMORY_ALLOC_FAILED))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    CATerminate();
}
#endif
