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

package org.iotivity.service.ca.test.stc;

import android.content.Context;
import android.test.AndroidTestCase;
import org.iotivity.CAJni;
import org.iotivity.service.ca.test.helper.*;

import static org.iotivity.service.ca.test.helper.TestInfo.*;

public class CAMulticastTest extends AndroidTestCase {

    public CAMulticastTest() {
    }

    protected void setUp() throws Exception {
        super.setUp();
        CAJni.readConfig();
        CAJni.setContext(getContext(), MainActivity.getActivity());
    }

    protected void tearDown() throws Exception {
        super.tearDown();
    }

    /**
     *         2015-09-18
     * @since 2015-09-18
     * @see CAResult_t CAInitialize()
     * @see void CARegisterHandler(CARequestCallback ReqHandler,
     *      CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
     * @see CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
     * @see CAResult_t CACreateEndpoint(CATransportFlags_t flags,
     *      CATransportAdapter_t adapter, const char *addr, uint16_t port,
     *      CAEndpoint_t **object)
     * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
     * @see void CADestroyEndpoint(CAEndpoint_t *object)
     * @see void CADestroyToken(CAToken_t token)
     * @see CAResult_t CAHandleRequestResponse()
     * @see void CATerminate()
     * @objective Test multicast message sending functionality by sending out
     *            GET request messages
     * @target CAResult_t CASendRequest(const CAEndpoint_t *object, const
     *         CARequestInfo_t *requestInfo)
     * @test_data Request messages with method GET
     * @pre_condition 1. [Server] Call CAInitialize to initialize CA 2. [Server]
     *                Select Network by CASelectNetwork API 3. [Server] Call
     *                CARegisterHandler to register handler 4. [Server] Call
     *                CAStartListeningServer to start server 5. [Server] Call
     *                CAHandleRequestResponse periodically to recieve client
     *                request 6. Repeat steps 1~5 for multiple times in multiple
     *                devices 7. [Client] Call CAInitialize to initialize CA 8.
     *                [Client] Select Network by CASelectNetwork API 9. [Client]
     *                Call CARegisterHandler to register handler 10. [Client]
     *                Call CAStartDiscoveryServer 11. [Client] Call
     *                CAGenerateToken to generate token
     * @procedure 1. [Client] Set CA_GET for CAMethod_t while forming
     *            CARequestInfo_t 2. [Client] Attempt to send a certain amount
     *            of request messages to the server 3. [Client] Attempt to
     *            receive acknowledgments for all the sent messages 4. [Client]
     *            Check and compare the amount of received acknowledgments with
     *            the amount of sent messages
     * @post_condition 1. [Client] Destroy token 2. [CLient] Terminate CA
     * @expected The number of acknowledgments should be equal to the amount of
     *           sent messages for each devices
     */
    public void testSendMulticastGetRequest_P() {
        CAJni.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD,
                MESSAGE_MULTICAST);

        if (!CAJni.initClientNetwork()) {
            fail();
            return;
        }

        if (!CAJni.sendRequestToAll(URI_TYPE_NORMAL,
                PAYLOAD_TYPE_NORMAL, CA_MSG_NONCONFIRM, CA_GET, HEADER_NONE)) {
            fail();
            CAJni.terminate();
            return;
        }

        if (!CAJni.checkReceiveCount(TOTAL_MESSAGE)) {
            fail();
        }

        CAJni.terminate();
    }

    /**
     * @since 2015-09-18
     * @see CAResult_t CAInitialize()
     * @see void CARegisterHandler(CARequestCallback ReqHandler,
     *      CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
     * @see CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
     * @see CAResult_t CACreateEndpoint(CATransportFlags_t flags,
     *      CATransportAdapter_t adapter, const char *addr, uint16_t port,
     *      CAEndpoint_t **object)
     * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
     * @see void CADestroyEndpoint(CAEndpoint_t *object)
     * @see void CADestroyToken(CAToken_t token)
     * @see CAResult_t CAHandleRequestResponse()
     * @see void CATerminate()
     * @objective Test multicast message sending functionality by sending out
     *            POST request messages
     * @target CAResult_t CASendRequest(const CAEndpoint_t *object, const
     *         CARequestInfo_t *requestInfo)
     * @test_data Request messages with method POST
     * @pre_condition 1. [Server] Call CAInitialize to initialize CA 2. [Server]
     *                Select Network by CASelectNetwork API 3. [Server] Call
     *                CARegisterHandler to register handler 4. [Server] Call
     *                CAStartListeningServer to start server 5. [Server] Call
     *                CAHandleRequestResponse periodically to recieve client
     *                request 6. Repeat steps 1~5 for multiple times in multiple
     *                devices 7. [Client] Call CAInitialize to initialize CA 8.
     *                [Client] Select Network by CASelectNetwork API 9. [Client]
     *                Call CARegisterHandler to register handler 10. [Client]
     *                Call CAStartDiscoveryServer 11.[Client] Call
     *                CAGenerateToken to generate token
     * @procedure 1. [Client] Set CA_POST for CAMethod_t while forming
     *            CARequestInfo_t 2. [Client] Attempt to send a certain amount
     *            of request messages to the server 3. [Client] Attempt to
     *            receive acknowledgments for all the sent messages 4. [Client]
     *            Check and compare the amount of received acknowledgments with
     *            the amount of sent messages
     * @post_condition 1. [Client] Destroy token 2. [CLient] Terminate CA
     * @expected The number of acknowledgments should be equal to the amount of
     *           sent messages for each devices
     */
    public void testSendMulticastPostRequest_P() {
        CAJni.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD,
                MESSAGE_MULTICAST);

        if (!CAJni.initClientNetwork()) {
            fail();
            return;
        }

        if (!CAJni.sendRequestToAll(URI_TYPE_NORMAL,
                PAYLOAD_TYPE_NORMAL, CA_MSG_NONCONFIRM, CA_POST, HEADER_NONE)) {
            fail();
            CAJni.terminate();
            return;
        }

        if (!CAJni.checkReceiveCount(TOTAL_MESSAGE)) {
            fail();
        }

        CAJni.terminate();
    }

    /**
     * @since 2015-09-18
     * @see CAResult_t CAInitialize()
     * @see void CARegisterHandler(CARequestCallback ReqHandler,
     *      CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
     * @see CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
     * @see CAResult_t CACreateEndpoint(CATransportFlags_t flags,
     *      CATransportAdapter_t adapter, const char *addr, uint16_t port,
     *      CAEndpoint_t **object)
     * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
     * @see void CADestroyEndpoint(CAEndpoint_t *object)
     * @see void CADestroyToken(CAToken_t token)
     * @see CAResult_t CAHandleRequestResponse()
     * @see void CATerminate()
     * @objective Test multicast message sending functionality by sending out
     *            PUT request messages
     * @target CAResult_t CASendRequest(const CAEndpoint_t *object, const
     *         CARequestInfo_t *requestInfo)
     * @test_data Request messages with method PUT
     * @pre_condition 1. [Server] Call CAInitialize to initialize CA 2. [Server]
     *                Select Network by CASelectNetwork API 3. [Server] Call
     *                CARegisterHandler to register handler 4. [Server] Call
     *                CAStartListeningServer to start server 5. [Server] Call
     *                CAHandleRequestResponse periodically to recieve client
     *                request 6. Repeat steps 1~5 for multiple times in multiple
     *                devices 7. [Client] Call CAInitialize to initialize CA 8.
     *                [Client] Select Network by CASelectNetwork API 9. [Client]
     *                Call CARegisterHandler to register handler 10. [Client]
     *                Call CAStartDiscoveryServer 11.[Client] Call
     *                CAGenerateToken to generate token
     * @procedure 1. [Client] Set CA_PUT for CAMethod_t while forming
     *            CARequestInfo_t 2. [Client] Attempt to send a certain amount
     *            of request messages to the server 3. [Client] Attempt to
     *            receive acknowledgments for all the sent messages 4. [Client]
     *            Check and compare the amount of received acknowledgments with
     *            the amount of sent messages
     * @post_condition 1. [Client] Destroy token 2. [CLient] Terminate CA
     * @expected The number of acknowledgments should be equal to the amount of
     *           sent messages for each devices
     */
    public void testSendMulticastPutRequest_P() {
        CAJni.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD,
                MESSAGE_MULTICAST);

        if (!CAJni.initClientNetwork()) {
            fail();
            return;
        }

        if (!CAJni.sendRequestToAll(URI_TYPE_NORMAL,
                PAYLOAD_TYPE_NORMAL, CA_MSG_NONCONFIRM, CA_PUT, HEADER_NONE)) {
            fail();
            CAJni.terminate();
            return;
        }

        if (!CAJni.checkReceiveCount(TOTAL_MESSAGE)) {
            fail();
        }

        CAJni.terminate();
    }

    /**
     * @since 2015-09-18
     * @see CAResult_t CAInitialize()
     * @see void CARegisterHandler(CARequestCallback ReqHandler,
     *      CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
     * @see CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
     * @see CAResult_t CACreateEndpoint(CATransportFlags_t flags,
     *      CATransportAdapter_t adapter, const char *addr, uint16_t port,
     *      CAEndpoint_t **object)
     * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
     * @see void CADestroyEndpoint(CAEndpoint_t *object)
     * @see void CADestroyToken(CAToken_t token)
     * @see CAResult_t CAHandleRequestResponse()
     * @see void CATerminate()
     * @objective Test multicast message sending functionality by sending out
     *            DELETE request messages
     * @target CAResult_t CASendRequest(const CAEndpoint_t *object, const
     *         CARequestInfo_t *requestInfo)
     * @test_data Request messages with method DELETE
     * @pre_condition 1. [Server] Call CAInitialize to initialize CA 2. [Server]
     *                Select Network by CASelectNetwork API 3. [Server] Call
     *                CARegisterHandler to register handler 4. [Server] Call
     *                CAStartListeningServer to start server 5. [Server] Call
     *                CAHandleRequestResponse periodically to recieve client
     *                request 6. Repeat steps 1~5 for multiple times in multiple
     *                devices 7. [Client] Call CAInitialize to initialize CA 8.
     *                [Client] Select Network by CASelectNetwork API 9. [Client]
     *                Call CARegisterHandler to register handler 10. [Client]
     *                Call CAStartDiscoveryServer 11.[Client] Call
     *                CAGenerateToken to generate token
     * @procedure 1. [Client] Set CA_DELETE for CAMethod_t while forming
     *            CARequestInfo_t 2. [Client] Attempt to send a certain amount
     *            of request messages to the server 3. [Client] Attempt to
     *            receive acknowledgments for all the sent messages 4. [Client]
     *            Check and compare the amount of received acknowledgments with
     *            the amount of sent messages
     * @post_condition 1. [Client] Destroy token 2. [CLient] Terminate CA
     * @expected The number of acknowledgments should be equal to the amount of
     *           sent messages for each devices
     */
    public void testSendMulticastDeleteRequest_P() {
        CAJni.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD,
                MESSAGE_MULTICAST);

        if (!CAJni.initClientNetwork()) {
            fail();
            return;
        }

        if (!CAJni.sendRequestToAll(URI_TYPE_NORMAL,
                PAYLOAD_TYPE_NORMAL, CA_MSG_NONCONFIRM, CA_DELETE, HEADER_NONE)) {
            fail();
            CAJni.terminate();
            return;
        }

        if (!CAJni.checkReceiveCount(TOTAL_MESSAGE)) {
            fail();
        }

        CAJni.terminate();
    }

    /**
     * @since 2015-09-18
     * @see CAResult_t CAInitialize()
     * @see void CARegisterHandler(CARequestCallback ReqHandler,
     *      CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
     * @see CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
     * @see CAResult_t CACreateEndpoint(CATransportFlags_t flags,
     *      CATransportAdapter_t adapter, const char *addr, uint16_t port,
     *      CAEndpoint_t **object)
     * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
     * @see void CADestroyEndpoint(CAEndpoint_t *object)
     * @see void CADestroyToken(CAToken_t token)
     * @see CAResult_t CAHandleRequestResponse()
     * @see void CATerminate()
     * @objective Test unicast message sending functionality by sending out
     *            request messages without header option
     * @target CAResult_t CASendRequest(const CAEndpoint_t *object, const
     *         CARequestInfo_t *requestInfo)
     * @test_data Request Messages
     * @pre_condition 1. [Server] Call CAInitialize to initialize CA 2. [Server]
     *                Select Network by CASelectNetwork API 3. [Server] Call
     *                CARegisterHandler to register handler 4. [Server] Call
     *                CAStartListeningServer to start server 5. [Server] Call
     *                CAHandleRequestResponse periodically to recieve client
     *                request 6. Repeat steps 1~5 for multiple times in multiple
     *                devices 7. [Client] Call CAInitialize to initialize CA 8.
     *                [Client] Select Network by CASelectNetwork API 9. [Client]
     *                Call CARegisterHandler to register handler 10. [Client]
     *                Call CAStartDiscoveryServer 11.[Client] Call
     *                CAGenerateToken to generate token
     * @procedure 1. [Client] Attempt to send a certain amount of request
     *            messages to the server simulator using no header option 2.
     *            [Client] Attempt to receive acknowledgments for all the sent
     *            messages 3. [Client] Check and compare the amount of received
     *            acknowledgments with the amount of sent messages
     * @post_condition 1. [Client] Destroy token 2. [CLient] Terminate CA
     * @expected The number of acknowledgments should be equal to the amount of
     *           sent messages and received header options should be same NULL
     *           for each devices
     */
    public void testSendMulticastRequestWithoutHeaderOption_P() {
        CAJni.setupTestCase(MESSAGE_OUTGOING, MESSAGE_HEADER,
                MESSAGE_MULTICAST);

        if (!CAJni.initClientNetwork()) {
            fail();
            return;
        }

        if (!CAJni.sendRequestToAll(URI_TYPE_NORMAL,
                PAYLOAD_TYPE_NORMAL, CA_MSG_NONCONFIRM, CA_GET, HEADER_NONE)) {
            fail();
            CAJni.terminate();
            return;
        }

        if (!CAJni.checkReceiveCount(TOTAL_MESSAGE)) {
            fail();
        }

        CAJni.terminate();
    }

    /**
     * @since 2015-09-18
     * @see CAResult_t CAInitialize()
     * @see void CARegisterHandler(CARequestCallback ReqHandler,
     *      CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
     * @see CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
     * @see CAResult_t CACreateEndpoint(CATransportFlags_t flags,
     *      CATransportAdapter_t adapter, const char *addr, uint16_t port,
     *      CAEndpoint_t **object)
     * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
     * @see void CADestroyEndpoint(CAEndpoint_t *object)
     * @see void CADestroyToken(CAToken_t token)
     * @see CAResult_t CAHandleRequestResponse()
     * @see void CATerminate()
     * @objective Test unicast message sending functionality by sending out
     *            request messages with header option
     * @target CAResult_t CASendRequest(const CAEndpoint_t *object, const
     *         CARequestInfo_t *requestInfo)
     * @test_data Request Messages
     * @pre_condition 1. [Server] Call CAInitialize to initialize CA 2. [Server]
     *                Select Network by CASelectNetwork API 3. [Server] Call
     *                CARegisterHandler to register handler 4. [Server] Call
     *                CAStartListeningServer to start server 5. [Server] Call
     *                CAHandleRequestResponse periodically to recieve client
     *                request 6. Repeat steps 1~5 for multiple times in multiple
     *                devices 7. [Client] Call CAInitialize to initialize CA 8.
     *                [Client] Select Network by CASelectNetwork API 9. [Client]
     *                Call CARegisterHandler to register handler 10. [Client]
     *                Call CAStartDiscoveryServer 11. [Client] Call
     *                CAGenerateToken to generate token
     * @procedure 1. [Client] Attempt to send a certain amount of request
     *            messages to the server simulator using header option 2.
     *            [Client] Attempt to receive acknowledgments for all the sent
     *            messages 3. [Client] Check and compare the amount of received
     *            acknowledgments with the amount of sent messages
     * @post_condition 1. [Client] Destroy token 2. [CLient] Terminate CA
     * @expected The number of acknowledgments should be equal to the amount of
     *           sent messages and received header option should be same as send
     *           header option for each devices
     */
    public void testSendMulticastRequestWithHeaderOption_P() {
        CAJni.setupTestCase(MESSAGE_OUTGOING, MESSAGE_HEADER,
                MESSAGE_MULTICAST);

        if (!CAJni.initClientNetwork()) {
            fail();
            return;
        }

        if (!CAJni.sendRequestToAll(URI_TYPE_NORMAL,
                PAYLOAD_TYPE_NORMAL, CA_MSG_NONCONFIRM, CA_GET, HEADER_ONE)) {
            fail();
            CAJni.terminate();
            return;
        }

        if (!CAJni.checkReceiveCount(TOTAL_MESSAGE)) {
            fail();
        }

        CAJni.terminate();
    }

    /**
     * @since 2015-09-18
     * @see CAResult_t CAInitialize()
     * @see void CARegisterHandler(CARequestCallback ReqHandler,
     *      CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
     * @see CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
     * @see CAResult_t CACreateEndpoint(CATransportFlags_t flags,
     *      CATransportAdapter_t adapter, const char *addr, uint16_t port,
     *      CAEndpoint_t **object)
     * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
     * @see void CADestroyEndpoint(CAEndpoint_t *object)
     * @see void CADestroyToken(CAToken_t token)
     * @see CAResult_t CAHandleRequestResponse()
     * @see void CATerminate()
     * @objective Test unicast message sending functionality by sending out
     *            request messages with header options
     * @target CAResult_t CASendRequest(const CAEndpoint_t *object, const
     *         CARequestInfo_t *requestInfo)
     * @test_data Request Messages
     * @pre_condition 1. [Server] Call CAInitialize to initialize CA 2. [Server]
     *                Select Network by CASelectNetwork API 3. [Server] Call
     *                CARegisterHandler to register handler 4. [Server] Call
     *                CAStartListeningServer to start server 5. [Server] Call
     *                CAHandleRequestResponse periodically to recieve client
     *                request 6. Repeat steps 1~5 for multiple times in multiple
     *                devices 7. [Client] Call CAInitialize to initialize CA 8.
     *                [Client] Select Network by CASelectNetwork API 9. [Client]
     *                Call CARegisterHandler to register handler 10. [Client]
     *                Call CAStartDiscoveryServer 11. [Client] Call
     *                CAGenerateToken to generate token
     * @procedure 1. [Client] Attempt to send a certain amount of request
     *            messages to the server simulator using header options 2.
     *            [Client] Attempt to receive acknowledgments for all the sent
     *            messages 3. [Client] Check and compare the amount of received
     *            acknowledgments with the amount of sent messages
     * @post_condition 1. [Client] Destroy token 2. [CLient] Terminate CA
     * @expected The number of acknowledgments should be equal to the amount of
     *           sent messages and received header option should be same as send
     *           header options for each devices
     */
    public void testSendMulticastRequestWithHeaderOptions_P() {
        CAJni.setupTestCase(MESSAGE_OUTGOING, MESSAGE_HEADER,
                MESSAGE_MULTICAST);

        if (!CAJni.initClientNetwork()) {
            fail();
            return;
        }

        if (!CAJni.sendRequestToAll(URI_TYPE_NORMAL,
                PAYLOAD_TYPE_NORMAL, CA_MSG_NONCONFIRM, CA_GET, HEADER_TWO)) {
            fail();
            CAJni.terminate();
            return;
        }

        if (!CAJni.checkReceiveCount(TOTAL_MESSAGE)) {
            fail();
        }

        CAJni.terminate();
    }

    /**
     * @since 2015-09-18
     * @see CAResult_t CAInitialize()
     * @see void CARegisterHandler(CARequestCallback ReqHandler,
     *      CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
     * @see CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
     * @see CAResult_t CACreateEndpoint(CATransportFlags_t flags,
     *      CATransportAdapter_t adapter, const char *addr, uint16_t port,
     *      CAEndpoint_t **object)
     * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
     * @see void CADestroyEndpoint(CAEndpoint_t *object)
     * @see void CADestroyToken(CAToken_t token)
     * @see CAResult_t CAHandleRequestResponse()
     * @see void CATerminate()
     * @objective Test unicast message sending functionality by sending out
     *            request messages with empty payloads
     * @target CAResult_t CASendRequest(const CAEndpoint_t *object, const
     *         CARequestInfo_t *requestInfo)
     * @test_data Empty Payloads
     * @pre_condition 1. [Server] Call CAInitialize to initialize CA 2. [Server]
     *                Select Network by CASelectNetwork API 3. [Server] Call
     *                CARegisterHandler to register handler 4. [Server] Call
     *                CAStartListeningServer to start server 5. [Server] Call
     *                CAHandleRequestResponse periodically to recieve client
     *                request 6. Repeat steps 1~5 for multiple times in multiple
     *                devices 7. [Client] Call CAInitialize to initialize CA 8.
     *                [Client] Select Network by CASelectNetwork API 9. [Client]
     *                Call CARegisterHandler to register handler 10. [Client]
     *                Call CAStartDiscoveryServer 11. [Client] Call
     *                CAGenerateToken to generate token
     * @procedure 1. [Client] Keep the payload empty while forming the
     *            CARequestInfo_t 2. [Client] Attempt to send a certain amount
     *            of request messages to the server simulator using header
     *            options 3. [Client] Attempt to receive acknowledgments for all
     *            the sent messages 4. [Client] Check and compare the amount of
     *            received acknowledgments with the amount of sent messages
     * @post_condition 1. [Client] Destroy token 2. [CLient] Terminate CA
     * @expected The number of acknowledgments should be equal to the amount of
     *           sent messages
     */
    public void testSendMulticastRequestWithEmptyPayload_P() {
        CAJni.setupTestCase(MESSAGE_OUTGOING, MESSAGE_URI,
                MESSAGE_MULTICAST);

        if (!CAJni.initClientNetwork()) {
            fail();
            return;
        }

        if (!CAJni.sendRequestToAll(URI_TYPE_NORMAL,
                PAYLOAD_TYPE_EMPTY, CA_MSG_NONCONFIRM, CA_GET, HEADER_NONE)) {
            fail();
            CAJni.terminate();
            return;
        }

        if (!CAJni.checkReceiveCount(TOTAL_MESSAGE)) {
            fail();
        }

        CAJni.terminate();
    }

    /**
     * @since 2014-12-31
     * @see CAResult_t CAInitialize()
     * @see void CARegisterHandler(CARequestCallback ReqHandler,
     *      CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
     * @see CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
     * @see CAResult_t CACreateEndpoint(CATransportFlags_t flags,
     *      CATransportAdapter_t adapter, const char *addr, uint16_t port,
     *      CAEndpoint_t **object)
     * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
     * @see void CADestroyEndpoint(CAEndpoint_t *object)
     * @see void CADestroyToken(CAToken_t token)
     * @see CAResult_t CAHandleRequestResponse()
     * @see void CATerminate()
     * @objective Test unicast message sending functionality by sending out
     *            request messages with null payloads
     * @target CAResult_t CASendRequest(const CAEndpoint_t *object, const
     *         CARequestInfo_t *requestInfo)
     * @test_data NULL Payloads
     * @pre_condition 1. [Server] Call CAInitialize to initialize CA 2. [Server]
     *                Select Network by CASelectNetwork API 3. [Server] Call
     *                CARegisterHandler to register handler 4. [Server] Call
     *                CAStartListeningServer to start server 5. [Server] Call
     *                CAHandleRequestResponse periodically to recieve client
     *                request 6. Repeat steps 1~5 for multiple times in multiple
     *                devices 7. [Client] Call CAInitialize to initialize CA 8.
     *                [Client] Select Network by CASelectNetwork API 9. [Client]
     *                Call CARegisterHandler to register handler 10. [Client]
     *                Call CAStartDiscoveryServer 11. [Client] Call
     *                CAGenerateToken to generate token
     * @procedure 1. [Client] Keep the payload null while forming the
     *            CARequestInfo_t 2. [Client] Attempt to send a certain amount
     *            of request messages to the server simulator using header
     *            options 3. [Client] Attempt to receive acknowledgments for all
     *            the sent messages 4. [Client] Check and compare the amount of
     *            received acknowledgments with the amount of sent messages
     * @post_condition 1. [Client] Destroy token 2. [CLient] Terminate CA
     * @expected The number of acknowledgments should be equal to the amount of
     *           sent messages for each devices
     */
    public void testSendMulticastRequestWithNullPayload_P() {
        CAJni.setupTestCase(MESSAGE_OUTGOING, MESSAGE_URI,
                MESSAGE_MULTICAST);

        if (!CAJni.initClientNetwork()) {
            fail();
            return;
        }

        if (!CAJni.sendRequestToAll(URI_TYPE_NORMAL,
                PAYLOAD_TYPE_NULL, CA_MSG_NONCONFIRM, CA_GET, HEADER_NONE)) {
            fail();
            CAJni.terminate();
            return;
        }

        if (!CAJni.checkReceiveCount(TOTAL_MESSAGE)) {
            fail();
        }

        CAJni.terminate();
    }

    /**
     * @since 2015-09-18
     * @see CAResult_t CAInitialize()
     * @see void CARegisterHandler(CARequestCallback ReqHandler,
     *      CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
     * @see CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
     * @see CAResult_t CACreateEndpoint(CATransportFlags_t flags,
     *      CATransportAdapter_t adapter, const char *addr, uint16_t port,
     *      CAEndpoint_t **object)
     * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
     * @see void CADestroyEndpoint(CAEndpoint_t *object)
     * @see void CADestroyToken(CAToken_t token)
     * @see CAResult_t CAHandleRequestResponse()
     * @see void CATerminate()
     * @objective Test multicast message retransmission functionality using
     *            invalid method
     * @target CAResult_t CASendRequest(const CAEndpoint_t *object, const
     *         CARequestInfo_t *requestInfo)
     * @test_data Request message of invalid method
     * @pre_condition 1. [Server] Call CAInitialize to initialize CA 2. [Server]
     *                Select Network by CASelectNetwork API 3. [Server] Call
     *                CARegisterHandler to register handler 4. [Server] Call
     *                CAStartListeningServer to start server 5. [Server] Call
     *                CAHandleRequestResponse periodically to recieve client
     *                request 6. [Client] Call CAInitialize to initialize CA 7.
     *                [Client] Select Network by CASelectNetwork API 8. [Client]
     *                Call CARegisterHandler to register handler 9. [Client]
     *                Call CAStartDiscoveryServer 10.[Client] Call
     *                CAGenerateToken to generate token 11.[Client] Call
     *                CACreateEndpoint to create endpoint for server
     * @procedure 1. [Client] Set meythodas CA_INVALID while forming CAInfo_t 2.
     *            [Client] Attempt to send 1 request, the server simulator will
     *            ignore it 3. [Client] Auto retransmission should occur 4.
     *            [Server] The server simulator is expected to respond to it
     *            with type CA_MSG_ACKNOWLEDGE 5. [Client] Attempt to receive
     *            the acknowledgement from server
     * @post_condition 1. [Client] Destroy token 2. [Client] Destroy endpoint 3.
     *                 [CLient] Terminate CA
     * @expected The request message should be acknowledged
     */
    public void testSendMulticastRequestWithInvalidMethod_N() {
        CAJni.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD,
                MESSAGE_MULTICAST);

        if (!CAJni.initClientNetwork()) {
            fail();
            return;
        }

        if (CAJni
                .sendRequestToAll(URI_TYPE_NORMAL, PAYLOAD_TYPE_NORMAL,
                        CA_MSG_NONCONFIRM, CA_INVALID, HEADER_NONE)) {
            fail();
        }

        CAJni.terminate();
    }
}
