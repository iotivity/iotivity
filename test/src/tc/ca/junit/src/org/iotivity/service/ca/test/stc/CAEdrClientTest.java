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
import org.iotivity.service.ca.test.helper.*;

import static org.iotivity.service.ca.test.helper.TestInfo.*;

public class CAEdrClientTest extends AndroidTestCase {

	private static final int NETWORK = 4;
	private static final String ADDRESS = "68:05:71:C3:EA:03";

	public CAEdrClientTest() {
	}

	protected void setUp() throws Exception {
		super.setUp();
		MainActivity.RM.setNetwork(NETWORK);
		MainActivity.RM.setContext(getContext(), MainActivity.getActivity());
		MainActivity.RM.setRemoteIp(ADDRESS);
	}

	protected void tearDown() throws Exception {
		super.tearDown();
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
	 * @see void CADestroyEndpoint(CAEndpoint_t *object)
	 * @see void CADestroyToken(CAToken_t token)
	 * @see CAResult_t CAHandleRequestResponse()
	 * @see void CATerminate()
	 * @objective Check the functionality of CASelectNetwork to receive response
	 *            message when available network is selected
	 * @target CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
	 * @test_data Response Messages
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
	 * @procedure 1. [Server] Configure the server simulator with a request to
	 *            send back a certain amount of response messages 2. [Client]
	 *            Attempt to receive the requested amount of unicast response
	 *            messages from the server simulator 3. [Client] Check and
	 *            compare the received amount with the requested amount
	 * @post_condition 1. [Client] Destroy token 2. [Client] Destroy endpoint 3.
	 *                 [CLient] Terminate CA
	 * @expected The requested amount of response messages should be received in
	 *           client side
	 */
	public void testEdrSelectNetworkForIncomingMessages_P() {
		MainActivity.RM.setupTestCase(MESSAGE_INCOMING, MESSAGE_PAYLOAD,
				MESSAGE_UNICAST);

		if (!MainActivity.RM.initClientNetwork()) {
			fail();
			return;
		}

		if (!MainActivity.RM.sendConfigurationRequest(SEND_MESSAGE,
				MESSAGE_RESPONSE, CA_PUT)) {
			fail();
			MainActivity.RM.terminate();
			return;
		}

		if (!MainActivity.RM.checkReceiveCount(TOTAL_MESSAGE)) {
			fail();
		}

		MainActivity.RM.terminate();
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
	 * @see void CADestroyEndpoint(CAEndpoint_t *object)
	 * @see void CADestroyToken(CAToken_t token)
	 * @see CAResult_t CAHandleRequestResponse()
	 * @see void CATerminate()
	 * @objective Check the functionality of CASelectNetwork & CAUnSelectNetwork
	 *            to receive response message when no network is selected
	 * @target CAResult_t CASelectNetwork(CATransportAdapter_t
	 *         interestedNetwork)
	 * @target CAResult_t CAUnSelectNetwork(CATransportAdapter_t
	 *         nonInterestedNetwork)
	 * @test_data Response Messages
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
	 * @procedure 1. [Server] Configure the server simulator with a request to
	 *            send back a certain amount of response messages 2. [Client]
	 *            UnSelect the network 3. [Client] Attempt to receive the
	 *            requested amount of unicast response messages from the server
	 *            simulator 4. [Client] Check and compare the received amount
	 *            with zero
	 * @post_condition 1. [Client] Destroy token 2. [Client] Destroy endpoint 3.
	 *                 [Client] Terminate CA
	 * @expected The received amount should be zero in client side
	 */
	public void testEdrSelectUnAvailableNetworkForIncomingMessages_N() {
		MainActivity.RM.setupTestCase(MESSAGE_INCOMING, MESSAGE_PAYLOAD,
				MESSAGE_UNICAST);

		if (!MainActivity.RM.initClientNetwork()) {
			fail();
			return;
		}

		if (!MainActivity.RM.sendConfigurationRequest(SEND_MESSAGE,
				MESSAGE_RESPONSE, CA_PUT)) {
			fail();
			MainActivity.RM.terminate();
			return;
		}

		if (!MainActivity.RM.unSelectCustomNetwork(NETWORK)) {
			fail();
			MainActivity.RM.terminate();
			return;
		}

		if (MainActivity.RM.selectCustomNetwork(INVALID_NETWORK)) {
			fail();
			MainActivity.RM.terminate();
			return;
		}

		if (!MainActivity.RM.checkReceiveCount(0)) {
			fail();
		}

		MainActivity.RM.terminate();
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
	 * @see CAResult_t CASendRequest(const CAEndpoint_t *object, const
	 *      CARequestInfo_t *requestInfo)
	 * @see void CADestroyEndpoint(CAEndpoint_t *object)
	 * @see void CADestroyToken(CAToken_t token)
	 * @see CAResult_t CAHandleRequestResponse()
	 * @see void CATerminate()
	 * @objective Check the functionality of CASelectNetwork to send message
	 *            when available network is selected
	 * @target CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
	 * @test_data Request Messages
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
	 * @procedure 1. [Client] Attempt to send a certain amount of request
	 *            messages to the server simulator 2. [Client] Attempt to
	 *            receive acknowledgments for all the sent messages
	 * @post_condition 1. [Client] Destroy token 2. [Client] Destroy endpoint 3.
	 *                 [CLient] Terminate CA
	 * @expected The number of acknowledgments should be equal to the amount of
	 *           sent messages
	 */
	public void testEdrSelectNetworkForOutgoingMessages_P() {
		MainActivity.RM.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD,
				MESSAGE_UNICAST);

		if (!MainActivity.RM.initClientNetwork()) {
			fail();
			return;
		}

		if (!MainActivity.RM.sendRequest(URI_TYPE_NORMAL, PAYLOAD_TYPE_NORMAL,
				CA_MSG_NONCONFIRM, CA_PUT, HEADER_NONE, false)) {
			fail();
			MainActivity.RM.terminate();
			return;
		}

		if (!MainActivity.RM.checkReceiveCount(TOTAL_MESSAGE)) {
			fail();
		}

		MainActivity.RM.terminate();
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
	 * @see void CADestroyEndpoint(CAEndpoint_t *object)
	 * @see void CADestroyToken(CAToken_t token)
	 * @see CAResult_t CAHandleRequestResponse()
	 * @see void CATerminate()
	 * @objective Test the effect of select/unselect network on incoming unicast
	 *            messages in a loop
	 * @target CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
	 * @target CAResult_t CAUnSelectNetwork(CATransportAdapter_t
	 *         nonInterestedNetwork)
	 * @test_data Response Messages
	 * @pre_condition 1. [Server] Call CAInitialize to initialize CA 2. [Server]
	 *                Select Network by CASelectNetwork API 3. [Server] Call
	 *                CARegisterHandler to register handler 4. [Server] Call
	 *                CAStartListeningServer to start server 5. [Server] Call
	 *                CAHandleRequestResponse periodically to recieve client
	 *                request 6. [Client] Call CAInitialize to initialize CA 7.
	 *                [Client] Call CARegisterHandler to register handler 8.
	 *                [Client] Call CAStartDiscoveryServer
	 * @procedure 1. [Client] Select Network by CASelectNetwork API 2. [Client]
	 *            Call CAGenerateToken to generate token 3. [Client] Call
	 *            CACreateEndpoint to create endpoint for server 4. [Server]
	 *            Configure the server simulator with a request to send back a
	 *            certain amount of response messages 5. [Client] Attempt to
	 *            receive the requested amount of unicast response messages from
	 *            the server simulator 6. [Client] Check and compare the
	 *            received amount with the requested amount 7. [Client] Destroy
	 *            token 8. [Client] Destroy endpoint 9. [CLient] Unselect the
	 *            network 10.Repeat procedure 1 to 8 multiple times
	 * @post_condition [CLient] Terminate CA
	 * @expected The requested amount of response messages should be received
	 *           each time in client side
	 */
	public void testEdrSelectNetworkSequentiallyForIncomingMessages_P() {
		MainActivity.RM.setupTestCase(MESSAGE_INCOMING, MESSAGE_PAYLOAD,
				MESSAGE_UNICAST);

		if (!MainActivity.RM.initClientNetwork()) {
			fail();
			return;
		}

		for (int i = 0; i < TRY_COUNT; i++) {
			if (i > 0) {
				if (!MainActivity.RM.selectCustomNetwork(NETWORK)) {
					fail();
					MainActivity.RM.terminate();
					return;
				}
			}

			if (!MainActivity.RM.sendConfigurationRequest(SEND_MESSAGE,
					MESSAGE_RESPONSE, CA_PUT)) {
				fail();
				MainActivity.RM.terminate();
				return;
			}

			if (!MainActivity.RM.checkReceiveCount(TOTAL_MESSAGE)) {
				fail();
				MainActivity.RM.terminate();
				return;
			}

			if (!MainActivity.RM.unSelectCustomNetwork(NETWORK)) {
				fail();
				MainActivity.RM.terminate();
				return;
			}
		}

		MainActivity.RM.terminate();
	}

	/**
	 * @since 2015-09-18
	 * @see CAResult_t CAInitialize()
	 * @see void CATerminate()
	 * @objective Test the effect of selecting network and receving incoming
	 *            unicast messages for multiple times
	 * @target CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
	 * @target CAResult_t CAUnSelectNetwork(CATransportAdapter_t
	 *         nonInterestedNetwork)
	 * @target void CARegisterHandler(CARequestCallback ReqHandler,
	 *         CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
	 * @target CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
	 * @target CAResult_t CACreateEndpoint(CATransportFlags_t flags,
	 *         CATransportAdapter_t adapter, const char *addr, uint16_t port,
	 *         CAEndpoint_t **object)
	 * @target void CADestroyEndpoint(CAEndpoint_t *object)
	 * @target void CADestroyToken(CAToken_t token)
	 * @target CAResult_t CAHandleRequestResponse()
	 * @test_data Response Messages
	 * @pre_condition 1. [Server] Call CAInitialize to initialize CA 2. [Server]
	 *                Select Network by CASelectNetwork API 3. [Server] Call
	 *                CARegisterHandler to register handler 4. [Server] Call
	 *                CAStartListeningServer to start server 5. [Server] Call
	 *                CAHandleRequestResponse periodically to recieve client
	 *                request 6. [Client] Call CAInitialize to initialize CA 7.
	 *                [Client] Call CARegisterHandler to register handler 8.
	 *                [Client] Call CAStartDiscoveryServer
	 * @procedure 1. [Client] Select Network by CASelectNetwork API 2. [Client]
	 *            Call CAGenerateToken to generate token 3. [Client] Call
	 *            CACreateEndpoint to create endpoint for server 4. [Server]
	 *            Configure the server simulator with a request to send back a
	 *            certain amount of response messages 5. [Client] Attempt to
	 *            receive the requested amount of unicast response messages from
	 *            the server simulator 6. [Client] Check and compare the
	 *            received amount with the requested amount 7. [Client] Destroy
	 *            token 8. [Client] Destroy endpoint 9. Repeat procedure 1 to 8
	 *            multiple times
	 * @post_condition [CLient] Terminate CA
	 * @expected The requested amount of response messages should be received
	 *           each time in client side
	 */
	public void testEdrSelectNetworkMultipleTimesForIncomingMessages_P() {
		MainActivity.RM.setupTestCase(MESSAGE_INCOMING, MESSAGE_PAYLOAD,
				MESSAGE_UNICAST);

		if (!MainActivity.RM.initClientNetwork()) {
			fail();
			return;
		}

		for (int i = 0; i < TRY_COUNT; i++) {
			if (!MainActivity.RM.selectNetwork()) {
				fail();
				MainActivity.RM.terminate();
				return;
			}

			if (!MainActivity.RM.sendConfigurationRequest(SEND_MESSAGE,
					MESSAGE_RESPONSE, CA_PUT)) {
				fail();
				MainActivity.RM.terminate();
				return;
			}

			if (!MainActivity.RM.checkReceiveCount(TOTAL_MESSAGE)) {
				fail();
				MainActivity.RM.terminate();
				return;
			}
		}

		MainActivity.RM.terminate();
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
	 *            request messages
	 * @target CAResult_t CASendRequest(const CAEndpoint_t *object, const
	 *         CARequestInfo_t *requestInfo)
	 * @test_data Request Messages
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
	 * @procedure 1. [Client] Attempt to send a certain amount of request
	 *            messages to the server simulator using no header option 2.
	 *            [Client] Attempt to receive acknowledgments for all the sent
	 *            messages 3. [Client] Check and compare the amount of received
	 *            acknowledgments with the amount of sent messages
	 * @post_condition 1. [Client] Destroy token 2. [Client] Destroy endpoint 3.
	 *                 [CLient] Terminate CA
	 * @expected The number of acknowledgments should be equal to the amount of
	 *           sent messages and received header options should be same NULL
	 */
	public void testEdrSendRequest_P() {
		for (int i = 0; i < TRY_COUNT; i++) {
			MainActivity.RM.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD,
					MESSAGE_UNICAST);

			if (!MainActivity.RM.initClientNetwork()) {
				fail();
				return;
			}

			if (!MainActivity.RM.sendRequest(URI_TYPE_NORMAL,
					PAYLOAD_TYPE_NORMAL, CA_MSG_NONCONFIRM, CA_PUT,
					HEADER_NONE, false)) {
				fail();
				MainActivity.RM.terminate();
				return;
			}

			if (!MainActivity.RM.checkReceiveCount(TOTAL_MESSAGE)) {
				fail();
			}

			MainActivity.RM.terminate();
		}
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
	 *                request 6. [Client] Call CAInitialize to initialize CA 7.
	 *                [Client] Select Network by CASelectNetwork API 8. [Client]
	 *                Call CARegisterHandler to register handler 9. [Client]
	 *                Call CAStartDiscoveryServer 10.[Client] Call
	 *                CAGenerateToken to generate token 11.[Client] Call
	 *                CACreateEndpoint to create endpoint for server
	 * @procedure 1. [Client] Attempt to send a certain amount of request
	 *            messages to the server simulator using no header option 2.
	 *            [Client] Attempt to receive acknowledgments for all the sent
	 *            messages 3. [Client] Check and compare the amount of received
	 *            acknowledgments with the amount of sent messages
	 * @post_condition 1. [Client] Destroy token 2. [Client] Destroy endpoint 3.
	 *                 [CLient] Terminate CA
	 * @expected The number of acknowledgments should be equal to the amount of
	 *           sent messages and received header options should be same NULL
	 */
	public void testEdrSendRequestWithoutHeaderOption_P() {
		MainActivity.RM.setupTestCase(MESSAGE_OUTGOING, MESSAGE_HEADER,
				MESSAGE_UNICAST);

		if (!MainActivity.RM.initClientNetwork()) {
			fail();
			return;
		}

		if (!MainActivity.RM.sendRequest(URI_TYPE_NORMAL, PAYLOAD_TYPE_NORMAL,
				CA_MSG_NONCONFIRM, CA_PUT, HEADER_NONE, false)) {
			fail();
			MainActivity.RM.terminate();
			return;
		}

		if (!MainActivity.RM.checkReceiveCount(TOTAL_MESSAGE)) {
			fail();
		}

		MainActivity.RM.terminate();
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
	 *                request 6. [Client] Call CAInitialize to initialize CA 7.
	 *                [Client] Select Network by CASelectNetwork API 8. [Client]
	 *                Call CARegisterHandler to register handler 9. [Client]
	 *                Call CAStartDiscoveryServer 10.[Client] Call
	 *                CAGenerateToken to generate token 11.[Client] Call
	 *                CACreateEndpoint to create endpoint for server
	 * @procedure 1. [Client] Attempt to send a certain amount of request
	 *            messages to the server simulator using header option 2.
	 *            [Client] Attempt to receive acknowledgments for all the sent
	 *            messages 3. [Client] Check and compare the amount of received
	 *            acknowledgments with the amount of sent messages
	 * @post_condition 1. [Client] Destroy token 2. [Client] Destroy endpoint 3.
	 *                 [CLient] Terminate CA
	 * @expected The number of acknowledgments should be equal to the amount of
	 *           sent messages and received header option should be same as send
	 *           header option
	 */
	public void testEdrSendRequestWithHeaderOption_P() {
		MainActivity.RM.setupTestCase(MESSAGE_OUTGOING, MESSAGE_HEADER,
				MESSAGE_UNICAST);

		if (!MainActivity.RM.initClientNetwork()) {
			fail();
			return;
		}

		if (!MainActivity.RM.sendRequest(URI_TYPE_NORMAL, PAYLOAD_TYPE_NORMAL,
				CA_MSG_NONCONFIRM, CA_PUT, HEADER_ONE, false)) {
			fail();
			MainActivity.RM.terminate();
			return;
		}

		if (!MainActivity.RM.checkReceiveCount(TOTAL_MESSAGE)) {
			fail();
		}

		MainActivity.RM.terminate();
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
	 *                request 6. [Client] Call CAInitialize to initialize CA 7.
	 *                [Client] Select Network by CASelectNetwork API 8. [Client]
	 *                Call CARegisterHandler to register handler 9. [Client]
	 *                Call CAStartDiscoveryServer 10.[Client] Call
	 *                CAGenerateToken to generate token 11.[Client] Call
	 *                CACreateEndpoint to create endpoint for server
	 * @procedure 1. [Client] Attempt to send a certain amount of request
	 *            messages to the server simulator using header options 2.
	 *            [Client] Attempt to receive acknowledgments for all the sent
	 *            messages 3. [Client] Check and compare the amount of received
	 *            acknowledgments with the amount of sent messages
	 * @post_condition 1. [Client] Destroy token 2. [Client] Destroy endpoint 3.
	 *                 [CLient] Terminate CA
	 * @expected The number of acknowledgments should be equal to the amount of
	 *           sent messages and received header option should be same as send
	 *           header options
	 */
	public void testEdrSendRequestWithHeaderOptions_P() {
		MainActivity.RM.setupTestCase(MESSAGE_OUTGOING, MESSAGE_HEADER,
				MESSAGE_UNICAST);

		if (!MainActivity.RM.initClientNetwork()) {
			fail();
			return;
		}

		if (!MainActivity.RM.sendRequest(URI_TYPE_NORMAL, PAYLOAD_TYPE_NORMAL,
				CA_MSG_NONCONFIRM, CA_PUT, HEADER_TWO, false)) {
			fail();
			MainActivity.RM.terminate();
			return;
		}

		if (!MainActivity.RM.checkReceiveCount(TOTAL_MESSAGE)) {
			fail();
		}

		MainActivity.RM.terminate();
	}

	/**
	 * @since 2014-12-24
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
	 *                request 6. [Client] Call CAInitialize to initialize CA 7.
	 *                [Client] Select Network by CASelectNetwork API 8. [Client]
	 *                Call CARegisterHandler to register handler 9. [Client]
	 *                Call CAStartDiscoveryServer 10.[Client] Call
	 *                CAGenerateToken to generate token 11.[Client] Call
	 *                CACreateEndpoint to create endpoint for server
	 * @procedure 1. [Client] Keep the payload empty while forming the
	 *            CARequestInfo_t 2. [Client] Attempt to send a certain amount
	 *            of request messages to the server simulator using header
	 *            options 3. [Client] Attempt to receive acknowledgments for all
	 *            the sent messages 4. [Client] Check and compare the amount of
	 *            received acknowledgments with the amount of sent messages
	 * @post_condition 1. [Client] Destroy token 2. [Client] Destroy endpoint 3.
	 *                 [CLient] Terminate CA
	 * @expected The number of acknowledgments should be equal to the amount of
	 *           sent messages
	 */
	public void testEdrSendRequestWithEmptyPayload_P() {
		MainActivity.RM.setupTestCase(MESSAGE_OUTGOING, MESSAGE_URI,
				MESSAGE_UNICAST);

		if (!MainActivity.RM.initClientNetwork()) {
			fail();
			return;
		}

		if (!MainActivity.RM.sendRequest(URI_TYPE_NORMAL, PAYLOAD_TYPE_EMPTY,
				CA_MSG_NONCONFIRM, CA_PUT, HEADER_NONE, false)) {
			fail();
			MainActivity.RM.terminate();
			return;
		}

		if (!MainActivity.RM.checkReceiveCount(TOTAL_MESSAGE)) {
			fail();
		}

		MainActivity.RM.terminate();
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
	 *            request messages with null payloads
	 * @target CAResult_t CASendRequest(const CAEndpoint_t *object, const
	 *         CARequestInfo_t *requestInfo)
	 * @test_data NULL Payloads
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
	 * @procedure 1. [Client] Keep the payload null while forming the
	 *            CARequestInfo_t 2. [Client] Attempt to send a certain amount
	 *            of request messages to the server simulator using header
	 *            options 3. [Client] Attempt to receive acknowledgments for all
	 *            the sent messages 4. [Client] Check and compare the amount of
	 *            received acknowledgments with the amount of sent messages
	 * @post_condition 1. [Client] Destroy token 2. [Client] Destroy endpoint 3.
	 *                 [CLient] Terminate CA
	 * @expected The number of acknowledgments should be equal to the amount of
	 *           sent messages
	 */
	public void testEdrSendRequestWithNullPayload_P() {
		MainActivity.RM.setupTestCase(MESSAGE_OUTGOING, MESSAGE_URI,
				MESSAGE_UNICAST);

		if (!MainActivity.RM.initClientNetwork()) {
			fail();
			return;
		}

		if (!MainActivity.RM.sendRequest(URI_TYPE_NORMAL, PAYLOAD_TYPE_NULL,
				CA_MSG_NONCONFIRM, CA_PUT, HEADER_NONE, false)) {
			fail();
			MainActivity.RM.terminate();
			return;
		}

		if (!MainActivity.RM.checkReceiveCount(TOTAL_MESSAGE)) {
			fail();
		}

		MainActivity.RM.terminate();
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
	 * @objective Test unicast message retransmission functionality using
	 *            message type CA_MSG_CONFIRM
	 * @target CAResult_t CASendRequest(const CAEndpoint_t *object, const
	 *         CARequestInfo_t *requestInfo)
	 * @test_data Request message of type CA_MSG_CONFIRM
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
	 * @procedure 1. [Client] Set CAMessageType_t as CA_MSG_CONFIRM while
	 *            forming CAInfo_t 2. [Client] Attempt to send 1 request, the
	 *            server simulator will ignore it 3. [Client] Auto
	 *            retransmission should occur 4. [Server] The server simulator
	 *            is expected to respond to it with type CA_MSG_ACKNOWLEDGE 5.
	 *            [Client] Attempt to receive the acknowledgement from server
	 * @post_condition 1. [Client] Destroy token 2. [Client] Destroy endpoint 3.
	 *                 [CLient] Terminate CA
	 * @expected The request message should be acknowledged
	 */
	public void testEdrSendRequestWithMessageTypeConfirm_P() {
		MainActivity.RM.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD,
				MESSAGE_UNICAST);

		if (!MainActivity.RM.initClientNetwork()) {
			fail();
			return;
		}

		if (!MainActivity.RM.sendRequest(URI_TYPE_NORMAL, PAYLOAD_TYPE_NORMAL,
				CA_MSG_CONFIRM, CA_PUT, HEADER_NONE, false)) {
			fail();
			MainActivity.RM.terminate();
			return;
		}

		if (!MainActivity.RM.checkReceiveCount(1)) {
			fail();
		}

		MainActivity.RM.terminate();
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
	 * @objective Test unicast message retransmission functionality using
	 *            message type CA_MSG_RESET
	 * @target CAResult_t CASendRequest(const CAEndpoint_t *object, const
	 *         CARequestInfo_t *requestInfo)
	 * @test_data Request message of type CA_MSG_RESET
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
	 * @procedure 1. [Client] Set CAMessageType_t as CA_MSG_RESET while forming
	 *            CAInfo_t 2. [Client] Attempt to send 1 request, the server
	 *            simulator will ignore it 3. [Client] Auto retransmission
	 *            should occur 4. [Server] The server simulator is expected to
	 *            respond to it with type CA_MSG_ACKNOWLEDGE 5. [Client] Attempt
	 *            to receive the acknowledgement from server
	 * @post_condition 1. [Client] Destroy token 2. [Client] Destroy endpoint 3.
	 *                 [CLient] Terminate CA
	 * @expected The request message should be acknowledged
	 */
	public void testEdrSendRequestWithMessageTypeReset_P() {
		MainActivity.RM.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD,
				MESSAGE_UNICAST);

		if (!MainActivity.RM.initClientNetwork()) {
			fail();
			return;
		}

		if (!MainActivity.RM.sendRequest(URI_TYPE_NORMAL, PAYLOAD_TYPE_NORMAL,
				CA_MSG_RESET, CA_PUT, HEADER_NONE, false)) {
			fail();
			MainActivity.RM.terminate();
			return;
		}

		if (!MainActivity.RM.checkReceiveCount(TOTAL_MESSAGE)) {
			fail();
		}

		MainActivity.RM.terminate();
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
	 * @objective Test unicast message sending functionality using wrong
	 *            endpoint
	 * @target CAResult_t CASendRequest(const CAEndpoint_t *object, const
	 *         CARequestInfo_t *requestInfo)
	 * @test_data Wrong endpoint URI
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
	 *                CACreateEndpoint to create endpoint from a wrong URI which
	 *                does not belong to the server
	 * @procedure 1. [Client] Attempt to send a certain amount of request
	 *            messages to the server 2. [Client] Attempt to receive
	 *            acknowledgments for all the sent messages 3. [Client] Check
	 *            and compare the amount of received acknowledgments with zero
	 * @post_condition 1. [Client] Destroy token 2. [Client] Destroy endpoint 3.
	 *                 [CLient] Terminate CA
	 * @expected The received amount should be zero
	 */
	public void testEdrSendRequestWithWrongEndpoint_N() {
		MainActivity.RM.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD,
				MESSAGE_UNICAST);

		if (!MainActivity.RM.initClientNetwork()) {
			fail();
			return;
		}

		if (!MainActivity.RM.sendRequest(URI_TYPE_WRONG_ENDPOINT,
				PAYLOAD_TYPE_NORMAL, CA_MSG_NONCONFIRM, CA_PUT, HEADER_NONE,
				false)) {
			fail();
			MainActivity.RM.terminate();
			return;
		}

		if (!MainActivity.RM.checkReceiveCount(0)) {
			fail();
		}

		MainActivity.RM.terminate();
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
	 * @objective Test unicast message retransmission functionality using
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
	public void testEdrSendRequestWithInvalidMethod_N() {
		MainActivity.RM.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD,
				MESSAGE_UNICAST);

		if (!MainActivity.RM.initClientNetwork()) {
			fail();
			return;
		}

		if (!MainActivity.RM.sendRequest(URI_TYPE_NORMAL, PAYLOAD_TYPE_NORMAL,
				CA_MSG_NONCONFIRM, CA_INVALID, HEADER_NONE, false)) {
			fail();
			MainActivity.RM.terminate();
			return;
		}

		if (!MainActivity.RM.checkReceiveCount(0)) {
			fail();
		}

		MainActivity.RM.terminate();
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
	 * @objective Test unicast message sending functionality by sending out GET
	 *            request messages
	 * @target CAResult_t CASendRequest(const CAEndpoint_t *object, const
	 *         CARequestInfo_t *requestInfo)
	 * @test_data Request messages with method GET
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
	 * @procedure 1. [Client] Set CA_GET for CAMethod_t while forming
	 *            CARequestInfo_t 2. [Client] Attempt to send a certain amount
	 *            of request messages to the server 3. [Client] Attempt to
	 *            receive acknowledgments for all the sent messages 4. [Client]
	 *            Check and compare the amount of received acknowledgments with
	 *            the amount of sent messages
	 * @post_condition 1. [Client] Destroy token 2. [Client] Destroy endpoint 3.
	 *                 [CLient] Terminate CA
	 * @expected The number of acknowledgments should be equal to the amount of
	 *           sent messages
	 */
	public void testEdrSendGetRequest_P() {
		MainActivity.RM.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD,
				MESSAGE_UNICAST);

		if (!MainActivity.RM.initClientNetwork()) {
			fail();
			return;
		}

		if (!MainActivity.RM.sendRequest(URI_TYPE_NORMAL, PAYLOAD_TYPE_NORMAL,
				CA_MSG_NONCONFIRM, CA_GET, HEADER_NONE, false)) {
			fail();
			MainActivity.RM.terminate();
			return;
		}

		if (!MainActivity.RM.checkReceiveCount(TOTAL_MESSAGE)) {
			fail();
		}

		MainActivity.RM.terminate();
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
	 * @objective Test unicast message sending functionality by sending out PUT
	 *            request messages
	 * @target CAResult_t CASendRequest(const CAEndpoint_t *object, const
	 *         CARequestInfo_t *requestInfo)
	 * @test_data Request messages with method PUT
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
	 * @procedure 1. [Client] Set CA_PUT for CAMethod_t while forming
	 *            CARequestInfo_t 2. [Client] Attempt to send a certain amount
	 *            of request messages to the server 3. [Client] Attempt to
	 *            receive acknowledgments for all the sent messages 4. [Client]
	 *            Check and compare the amount of received acknowledgments with
	 *            the amount of sent messages
	 * @post_condition 1. [Client] Destroy token 2. [Client] Destroy endpoint 3.
	 *                 [CLient] Terminate CA
	 * @expected The number of acknowledgments should be equal to the amount of
	 *           sent messages
	 */
	public void testEdrSendPutRequest_P() {
		MainActivity.RM.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD,
				MESSAGE_UNICAST);

		if (!MainActivity.RM.initClientNetwork()) {
			fail();
			return;
		}

		if (!MainActivity.RM.sendRequest(URI_TYPE_NORMAL, PAYLOAD_TYPE_NORMAL,
				CA_MSG_NONCONFIRM, CA_PUT, HEADER_NONE, false)) {
			fail();
			MainActivity.RM.terminate();
			return;
		}

		if (!MainActivity.RM.checkReceiveCount(TOTAL_MESSAGE)) {
			fail();
		}

		MainActivity.RM.terminate();
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
	 * @objective Test unicast message sending functionality by sending out POST
	 *            request messages
	 * @target CAResult_t CASendRequest(const CAEndpoint_t *object, const
	 *         CARequestInfo_t *requestInfo)
	 * @test_data Request messages with method POST
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
	 * @procedure 1. [Client] Set CA_POST for CAMethod_t while forming
	 *            CARequestInfo_t 2. [Client] Attempt to send a certain amount
	 *            of request messages to the server 3. [Client] Attempt to
	 *            receive acknowledgments for all the sent messages 4. [Client]
	 *            Check and compare the amount of received acknowledgments with
	 *            the amount of sent messages
	 * @post_condition 1. [Client] Destroy token 2. [Client] Destroy endpoint 3.
	 *                 [CLient] Terminate CA
	 * @expected The number of acknowledgments should be equal to the amount of
	 *           sent messages
	 */
	public void testEdrSendPostRequest_P() {
		MainActivity.RM.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD,
				MESSAGE_UNICAST);

		if (!MainActivity.RM.initClientNetwork()) {
			fail();
			return;
		}

		if (!MainActivity.RM.sendRequest(URI_TYPE_NORMAL, PAYLOAD_TYPE_NORMAL,
				CA_MSG_NONCONFIRM, CA_POST, HEADER_NONE, false)) {
			fail();
			MainActivity.RM.terminate();
			return;
		}

		if (!MainActivity.RM.checkReceiveCount(TOTAL_MESSAGE)) {
			fail();
		}

		MainActivity.RM.terminate();
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
	 *            DELETE request messages
	 * @target CAResult_t CASendRequest(const CAEndpoint_t *object, const
	 *         CARequestInfo_t *requestInfo)
	 * @test_data Request messages with method DELETE
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
	 * @procedure 1. [Client] Set CA_DELETE for CAMethod_t while forming
	 *            CARequestInfo_t 2. [Client] Attempt to send a certain amount
	 *            of request messages to the server 3. [Client] Attempt to
	 *            receive acknowledgments for all the sent messages 4. [Client]
	 *            Check and compare the amount of received acknowledgments with
	 *            the amount of sent messages
	 * @post_condition 1. [Client] Destroy token 2. [Client] Destroy endpoint 3.
	 *                 [CLient] Terminate CA
	 * @expected The number of acknowledgments should be equal to the amount of
	 *           sent messages
	 */
	public void testEdrSendDeleteRequest_P() {
		MainActivity.RM.setupTestCase(MESSAGE_OUTGOING, MESSAGE_PAYLOAD,
				MESSAGE_UNICAST);

		if (!MainActivity.RM.initClientNetwork()) {
			fail();
			return;
		}

		if (!MainActivity.RM.sendRequest(URI_TYPE_NORMAL, PAYLOAD_TYPE_NORMAL,
				CA_MSG_NONCONFIRM, CA_DELETE, HEADER_NONE, false)) {
			fail();
			MainActivity.RM.terminate();
			return;
		}

		if (!MainActivity.RM.checkReceiveCount(TOTAL_MESSAGE)) {
			fail();
		}

		MainActivity.RM.terminate();
	}

	/**
	 * @since 2015-09-18
	 * @see CAResult_t CAInitialize()
	 * @see CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
	 * @see CAResult_t CACreateEndpoint(CATransportFlags_t flags,
	 *      CATransportAdapter_t adapter, const char *addr, uint16_t port,
	 *      CAEndpoint_t **object)
	 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
	 * @see CAResult_t CASendRequest(const CAEndpoint_t *object, const
	 *      CARequestInfo_t *requestInfo)
	 * @see void CADestroyEndpoint(CAEndpoint_t *object)
	 * @see void CADestroyToken(CAToken_t token)
	 * @see CAResult_t CAHandleRequestResponse()
	 * @see void CATerminate()
	 * @objective Test 'CARegisterHandler' by registering null response handler
	 * @target void CARegisterHandler(CARequestCallback ReqHandler,
	 *         CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
	 * @test_data null response handler
	 * @pre_condition 1. [Server] Call CAInitialize to initialize CA 2. [Server]
	 *                Select Network by CASelectNetwork API 3. [Server] Call
	 *                CARegisterHandler to register handler 4. [Server] Call
	 *                CAStartListeningServer to start server 5. [Server] Call
	 *                CAHandleRequestResponse periodically to recieve client
	 *                request 6. [Client] Call CAInitialize to initialize CA 7.
	 *                [Client] Select Network by CASelectNetwork API 8. [Client]
	 *                Call CAGenerateToken to generate token 9. [Client] Call
	 *                CACreateEndpoint to create endpoint for server
	 * @procedure 1. [Client] Call CARegisterHandler with Null Response Handler
	 *            2. [Server] Configure the server with a request to send back a
	 *            certain amount of response messages 3. [Client] Attempt to
	 *            receive a certain amount of unicast response messages from the
	 *            server 4. [Client] Check and compare the received amount with
	 *            zero
	 * @post_condition 1. [Client] Destroy token 2. [Client] Destroy endpoint 3.
	 *                 [CLient] Terminate CA
	 * @expected No response messages should be received
	 */
	public void testEdrReceiveWithNullResponseHandler_N() {
		MainActivity.RM.setupTestCase(MESSAGE_INCOMING, MESSAGE_PAYLOAD,
				MESSAGE_UNICAST);

		if (!MainActivity.RM.initNetwork()) {
			fail();
			return;
		}

		MainActivity.RM.registerClientHandler(HANDLER_NORMAL, HANDLER_NULL);

		if (!MainActivity.RM.sendConfigurationRequest(SEND_MESSAGE,
				MESSAGE_RESPONSE, CA_PUT)) {
			fail();
			MainActivity.RM.terminate();
			return;
		}

		if (!MainActivity.RM.checkReceiveCount(0)) {
			fail();
		}

		MainActivity.RM.terminate();
	}

	/**
	 * @since 2015-09-18
	 * @see CAResult_t CAInitialize()
	 * @see CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
	 * @see CAResult_t CACreateEndpoint(CATransportFlags_t flags,
	 *      CATransportAdapter_t adapter, const char *addr, uint16_t port,
	 *      CAEndpoint_t **object)
	 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
	 * @see CAResult_t CASendRequest(const CAEndpoint_t *object, const
	 *      CARequestInfo_t *requestInfo)
	 * @see void CADestroyEndpoint(CAEndpoint_t *object)
	 * @see void CADestroyToken(CAToken_t token)
	 * @see void CATerminate()
	 * @objective Test the impact or consequence of not calling
	 *            CARegisterHandler Test in receiving response
	 * @target CAResult_t CAHandleRequestResponse()
	 * @test_data none
	 * @pre_condition 1. [Server] Call CAInitialize to initialize CA 2. [Server]
	 *                Select Network by CASelectNetwork API 3. [Server] Call
	 *                CARegisterHandler to register handler 4. [Server] Call
	 *                CAStartListeningServer to start server 5. [Server] Call
	 *                CAHandleRequestResponse periodically to recieve client
	 *                request 6. [Client] Call CAInitialize to initialize CA 7.
	 *                [Client] Select Network by CASelectNetwork API 8. [Client]
	 *                Do not Register the handler 9. [Client] Call
	 *                CAGenerateToken to generate token 10. [Client] Call
	 *                CACreateEndpoint to create endpoint for server
	 * @procedure 1. [Server] Configure the server with a request to send back a
	 *            certain amount of response messages 2. [Client] Attempt to
	 *            receive the requested amount of unicast response messages from
	 *            the server 3. [Client] Check and compare the received amount
	 *            with zero
	 * @post_condition 1. [Client] Destroy token 2. [Client] Destroy endpoint 3.
	 *                 [CLient] Terminate CA
	 * @expected No response messages should be received
	 */
	public void testEdrReceiveResponseWithoutCallingCARegisterHandler_N() {
		MainActivity.RM.setupTestCase(MESSAGE_INCOMING, MESSAGE_PAYLOAD,
				MESSAGE_UNICAST);

		if (!MainActivity.RM.initNetwork()) {
			fail();
			return;
		}

		if (!MainActivity.RM.sendConfigurationRequest(SEND_MESSAGE,
				MESSAGE_RESPONSE, CA_PUT)) {
			fail();
			MainActivity.RM.terminate();
			return;
		}

		if (!MainActivity.RM.checkReceiveCount(0)) {
			fail();
		}

		MainActivity.RM.terminate();
	}

	/**
	 * @since 2015-09-18
	 * @see CAResult_t CAInitialize()
	 * @see CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
	 * @see CAResult_t CACreateEndpoint(CATransportFlags_t flags,
	 *      CATransportAdapter_t adapter, const char *addr, uint16_t port,
	 *      CAEndpoint_t **object)
	 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
	 * @see CAResult_t CASendRequest(const CAEndpoint_t *object, const
	 *      CARequestInfo_t *requestInfo)
	 * @see void CADestroyEndpoint(CAEndpoint_t *object)
	 * @see void CADestroyToken(CAToken_t token)
	 * @see CAResult_t CAHandleRequestResponse()
	 * @see void CATerminate()
	 * @objective Test 'CARegisterHandler' when registration is done twice by
	 *            using two different response handler
	 * @target void CARegisterHandler(CARequestCallback ReqHandler,
	 *         CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
	 * @test_data two different response handler
	 * @pre_condition 1. [Server] Call CAInitialize to initialize CA 2. [Server]
	 *                Select Network by CASelectNetwork API 3. [Server] Call
	 *                CARegisterHandler to register handler 4. [Server] Call
	 *                CAStartListeningServer to start server 5. [Server] Call
	 *                CAHandleRequestResponse periodically to recieve client
	 *                request 6. [Client] Call CAInitialize to initialize CA 7.
	 *                [Client] Select Network by CASelectNetwork API
	 * @procedure 1. [Client] Call CARegisterHandler twice with two different
	 *            Response Handler 2. [Client] Call CAStartDiscoveryServer 3.
	 *            [Client] Call CAGenerateToken to generate token 4. [Client]
	 *            Call CACreateEndpoint to create endpoint for server 5.
	 *            [Server] Configure the server with a request to send back a
	 *            certain amount of response messages 6. [Client] Attempt to
	 *            receive the requested amount of unicast response messages from
	 *            the server 7. [Client] Check and compare the received amount
	 *            by the both handler with the requested amount
	 * @post_condition 1. [Client] Destroy token 2. [Client] Destroy endpoint 3.
	 *                 [CLient] Terminate CA
	 * @expected Second handler will receive requested amount of response
	 *           messages and first handler will receive no response message
	 */
	public void testEdrReceiveWithSecondResponseHandler_P() {
		MainActivity.RM.setupTestCase(MESSAGE_INCOMING, MESSAGE_PAYLOAD,
				MESSAGE_UNICAST);

		if (!MainActivity.RM.initNetwork()) {
			fail();
			return;
		}

		MainActivity.RM.registerClientHandler(HANDLER_NORMAL, HANDLER_NORMAL);
		MainActivity.RM.registerClientHandler(HANDLER_NORMAL, HANDLER_SECOND);

		if (!MainActivity.RM.startDiscoveryServer()) {
			fail();
			MainActivity.RM.terminate();
			return;
		}

		if (!MainActivity.RM.sendConfigurationRequest(SEND_MESSAGE,
				MESSAGE_RESPONSE, CA_PUT)) {
			fail();
			MainActivity.RM.terminate();
			return;
		}

		if (!MainActivity.RM.checkReceiveCount(TOTAL_MESSAGE)) {
			fail();
		}

		MainActivity.RM.terminate();
	}

	/**
	 * @since 2015-09-18
	 * @see CAResult_t CAInitialize()
	 * @see CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
	 * @see CAResult_t CACreateEndpoint(CATransportFlags_t flags,
	 *      CATransportAdapter_t adapter, const char *addr, uint16_t port,
	 *      CAEndpoint_t **object)
	 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
	 * @see CAResult_t CASendRequest(const CAEndpoint_t *object, const
	 *      CARequestInfo_t *requestInfo)
	 * @see void CADestroyEndpoint(CAEndpoint_t *object)
	 * @see void CADestroyToken(CAToken_t token)
	 * @see CAResult_t CAHandleRequestResponse()
	 * @see void CATerminate()
	 * @objective Test 'CARegisterHandler' by doing registration twice using
	 *            first a valid response handler then a null response handler
	 * @target void CARegisterHandler(CARequestCallback ReqHandler,
	 *         CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
	 * @test_data valid response handler & null response handler
	 * @pre_condition 1. [Server] Call CAInitialize to initialize CA 2. [Server]
	 *                Select Network by CASelectNetwork API 3. [Server] Call
	 *                CARegisterHandler to register handler 4. [Server] Call
	 *                CAStartListeningServer to start server 5. [Server] Call
	 *                CAHandleRequestResponse periodically to recieve client
	 *                request 6. [Client] Call CAInitialize to initialize CA 7.
	 *                [Client] Select Network by CASelectNetwork API
	 * @procedure 1. [Client] Call CARegisterHandler twice by using first a
	 *            valid response handler then a null response handler 2.
	 *            [Client] Call CAStartDiscoveryServer 3. [Client] Call
	 *            CAGenerateToken to generate token 4. [Client] Call
	 *            CACreateEndpoint to create endpoint for server 5. [Server]
	 *            Configure the server with a request to send back a certain
	 *            amount of response messages 6. [Client] Attempt to receive the
	 *            requested amount of unicast response messages from the server
	 *            7. [Client] Check and compare the received amount wit zero
	 * @post_condition 1. [Client] Destroy token 2. [Client] Destroy endpoint 3.
	 *                 [CLient] Terminate CA
	 * @expected No response messages should be received
	 */
	public void testEdrReceiveAfterRegisteringWithValidFollowedByNullResponseHandler_N() {
		MainActivity.RM.setupTestCase(MESSAGE_INCOMING, MESSAGE_PAYLOAD,
				MESSAGE_UNICAST);

		if (!MainActivity.RM.initNetwork()) {
			fail();
			return;
		}

		MainActivity.RM.registerClientHandler(HANDLER_NORMAL, HANDLER_NORMAL);

		MainActivity.RM.registerClientHandler(HANDLER_NORMAL, HANDLER_NULL);

		if (!MainActivity.RM.sendConfigurationRequest(SEND_MESSAGE,
				MESSAGE_RESPONSE, CA_PUT)) {
			fail();
			MainActivity.RM.terminate();
			return;
		}

		if (!MainActivity.RM.checkReceiveCount(0)) {
			fail();
		}

		MainActivity.RM.terminate();
	}
}
