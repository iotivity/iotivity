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

package org.iotivity.service.ca.test.btc;

import android.content.Context;
import android.test.AndroidTestCase;
import org.iotivity.CAJni;
import org.iotivity.service.ca.test.helper.*;

import static org.iotivity.service.ca.test.helper.TestInfo.*;

public class CATest extends AndroidTestCase {

    public CATest() {
    }

    protected void setUp() throws Exception {
        super.setUp();
        CAJni.setContext(getContext(), MainActivity.getActivity());
    }

    protected void tearDown() throws Exception {
        super.tearDown();
    }

    /**
     * @since 2014-09-17
     * @see void CATerminate()
     * @objective Test 'CAInitialize' positively to initialize CA module
     * @target CAResult_t CAInitialize()
     * @test_data none
     * @pre_condition none
     * @procedure call CAInitialize() API
     * @post_condition Terminate CA using CATerminate() API
     * @expected Initialization will succeed and return CA_STATUS_OK
     */
    public void testCAInitialize_P() {
        boolean status = CAJni.initialize();

        CAJni.terminate();

        assertTrue(status);
    }

    /**
     * @since 2014-09-17
     * @see CAResult_t CAInitialize()
     * @see void CATerminate()
     * @objective Test 'CARegisterHandler' positively to register request &
     *            response handlers
     * @target void CARegisterHandler(CARequestCallback ReqHandler,
     *         CAResponseCallback RespHandler, CAErrorCallback ErrorHandler)
     * @test_data 1. A request handler 2. A response handler 3. A error handler
     * @pre_condition Initialize CA using CAInitialize()
     * @procedure call Call CARegisterHandler() API with request,response and
     *            error handlers as arguments
     * @post_condition Terminate CA using CATerminate() API
     * @expected CARegisterHandler() will register the handlers
     */
    public void testCARegisterHandler_P() {
        if (!CAJni.initialize()) {
            fail();
            return;
        }

        if (!CAJni.registerHandler()) {
            fail();
        }

        CAJni.terminate();
    }

    /**
     * @since 2014-09-17
     * @see void CADestroyToken(CAToken_t token)
     * @objective Test 'CAGenerateToken' positively to generate a token
     * @target CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
     * @test_data a pointer of the type CAToken_t and token length
     * @pre_condition None
     * @procedure call the CAGenerateToken() API with passing a reference to the
     *            token and uint8_t length to tokenLength as argument
     * @post_condition Destroy token using CADestroyToken.
     * @expected 1. It will generate a token and return CA_STATUS_OK 2. Token
     *           variable is not NULL
     */
    public void testCAGenerateToken_P() {

        if (!CAJni.generateToken(TOKEN_TYPE_VALID, TOKEN_LENGTH_NORMAL, CA_STATUS_OK)) {
            fail();
            CAJni.terminate();
            return;
        }

        CAJni.destroyToken();
    }

    /**
     * @since 2016-02-29
     * @see void CADestroyToken(CAToken_t token)
     * @objective Test 'CAGenerateToken' positively with lower boundary value in
     *            Token Length parameter [LBV]
     * @target CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
     * @test_data Lower boundary value (1) as token length argument
     * @pre_condition None
     * @procedure Call CAGenerateToken with lower boundary value (1) as token
     *            length argument
     * @post_condition Destroy token using CADestroyToken.
     * @expected 1. It will generate a token and return CA_STATUS_OK 2. Token
     *           variable is not NULL
     */
    public void testCAGenerateToken_LBV_P() {

        if (!CAJni.generateToken(TOKEN_TYPE_VALID, TOKEN_LENGTH_LBV_VALID,
                CA_STATUS_OK)) {
            fail();
            return;
        }
        CAJni.destroyToken();
    }

    /**
     * @since 2016-02-29
     * @see CAResult_t CAInitialize()
     * @see void CATerminate()
     * @objective Test 'CAGenerateToken' negatively with out of lower bundary
     *            value in Token Length parameter [LOBV]
     * @target CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
     * @test_data out of lower bundary value as token length argument
     * @pre_condition None
     * @procedure Call CAGenerateToken with out of lower boundary value in token
     *            length parameter
     * @post_condition None
     * @expected It will return CA_STATUS_INVALID_PARAM
     */
    public void testCAGenerateToken_LOBV_N() {

        if (!CAJni.generateToken(TOKEN_TYPE_VALID, 
                TOKEN_LENGTH_LOBV_INVALID, CA_STATUS_INVALID_PARAM)) {
            fail();
        }
    }

    /**
     * @since 2016-02-29
     * @see CAResult_t CAInitialize()
     * @see void CATerminate()
     * @objective Test 'CAGenerateToken' negatively with out of upper boundary
     *            value in Token Length parameter [UOBV]
     * @target CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
     * @test_data out of upper boundary value as token length argument
     * @pre_condition None
     * @procedure Call CAGenerateToken with out of upper boundary value in token
     *            length parameter
     * @post_condition None
     * @expected It will return CA_STATUS_INVALID_PARAM
     */
    public void testCAGenerateToken_UOBV_N() {

        if (!CAJni.generateToken(TOKEN_TYPE_VALID, 
                TOKEN_LENGTH_UOBV_INVALID, CA_STATUS_INVALID_PARAM)) {
            fail();
        }
    }

    /**
     * @since 2016-02-29
     * @see CAResult_t CAInitialize()
     * @see void CATerminate()
     * @objective Test 'CAGenerateToken' negatively with passing invalid
     *            refrerence in token parameter [UFRV]
     * @target CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
     * @test_data Invalid reference as token argument
     * @pre_condition None
     * @procedure Call CAGenerateToken with unformatted reference value in token
     *            type parameter
     * @post_condition None
     * @expected It will not generate token & will return
     *           CA_STATUS_INVALID_PARAM
     */
    public void testCAGenerateToken_UFRV_N() {

        if (!CAJni.generateToken(TOKEN_TYPE_INVALID_REFERENCE,
                TOKEN_LENGTH_NORMAL, CA_STATUS_INVALID_PARAM)) {
            fail();
        }
    }

    /**
     * @since 2014-09-17
     * @see CAResult_t CAInitialize()
     * @see void CATerminate()
     * @objective Test 'CAGenerateToken' negatively with passing NULL value as
     *            token argument
     * @target CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
     * @test_data NULL as token argument
     * @pre_condition None
     * @procedure Call CAGenerateToken with NULL parameter.
     * @post_condition none
     * @expected It will return CA_STATUS_INVALID_PARAM
     */
    public void testCAGenerateToken_N() {

        if (!CAJni.generateToken(TOKEN_TYPE_NULL, 
                TOKEN_LENGTH_NORMAL, CA_STATUS_INVALID_PARAM)) {
            fail();
        }
    }

    /**
     * @since 2016-03-01
     * @see CAResult_t CAInitialize()
     * @see void CATerminate()
     * @objective Test 'CASelectNetwork' positively with lower boundary value of
     *            selected network [LBV]
     * @target CAResult_t CASelectNetwork(CATransportAdapter_t
     *         interestedNetwork)
     * @test_data lower boundary value as network argument
     * @pre_condition Initialize CA using CAInitialize
     * @procedure 1. Call CASelectNetwork using lower boundary value as network
     *            argument 2. Check it's return value
     * @post_condition Terminate CA using CATerminate function
     * @expected It will return CA_NOT_SUPPORTED
     */
    public void testCASelectNetwork_LBV_P() {
        if (!CAJni.initialize()) {
            fail();
            return;
        }

        if (!CAJni.selectCustomNetwork(
                CA_DEFAULT_ADAPTER, CA_NOT_SUPPORTED)) {
            fail();
        }

        CAJni.terminate();
    }

    /**
     * @since 2016-03-01
     * @see CAResult_t CAInitialize()
     * @see void CATerminate()
     * @objective Test 'CASelectNetwork' positively with upper boundary value of
     *            selected network [UBV]
     * @target CAResult_t CASelectNetwork(CATransportAdapter_t
     *         interestedNetwork)
     * @test_data upper boundary value as network argument
     * @pre_condition Initialize CA using CAInitialize
     * @procedure 1. Call CASelectNetwork using upper boundary value as network
     *            argument 2. Check it's return value
     * @post_condition Terminate CA using CATerminate function
     * @expected It will return CA_STATUS_OK
     */
    public void testCASelectNetwork_UBV_P() {
        if (!CAJni.initialize()) {
            fail();
            return;
        }

        if (!CAJni.selectCustomNetwork(
                CA_ALL_ADAPTERS, CA_STATUS_OK)) {
            fail();
        }

        CAJni.terminate();
    }

    /**
     * @since 2014-09-17
     * @see CAResult_t CAInitialize()
     * @see void CATerminate()
     * @objective Test 'CASelectNetwork' negatively to select invalid network
     * @target CAResult_t CASelectNetwork(const uint32_t interestedNetwork);
     * @test_data invalid value for which network interface is not defined
     * @pre_condition Initialize CA using CAInitialize
     * @procedure 1. Call CASelectNetwork using invalid network value 2. Check
     *            it's return value
     * @post_condition Terminate CA using CATerminate function
     * @expected It will fail to select a network and won't return CA_STATUS_OK
     */

    public void testCASelectNetwork_N() {
        if (!CAJni.initialize()) {
            fail();
            return;
        }

        if (CAJni.selectCustomNetwork(CA_DEFAULT_ADAPTER, CA_STATUS_OK)) {
            fail();
        }

        CAJni.terminate();
    }

    /**
     * @since 2016-03-01
     * @see CAResult_t CAInitialize()
     * @see void CATerminate()
     * @objective Test 'CASelectNetwork' negatively with out of lower boundary
     *            value of selected network [LOBV]
     * @target CAResult_t CASelectNetwork(CATransportAdapter_t
     *         interestedNetwork)
     * @test_data out of lower boundary value as Network argument
     * @pre_condition Initialize CA using CAInitialize
     * @procedure 1. Call CASelectNetwork using out of lower boundary value as
     *            network argument 2. Check it's return value
     * @post_condition Terminate CA using CATerminate function
     * @expected It will return CA_NOT_SUPPORTED
     */
    public void testCASelectNetwork_LOBV_N() {
        if (!CAJni.initialize()) {
            fail();
            return;
        }

        if (!CAJni.selectCustomNetwork(CA_INVALID_LOBV_ADAPTER, CA_STATUS_OK)) {
            fail();
        }

        CAJni.terminate();
    }

    /**
     * @since 2016-03-01
     * @see CAResult_t CAInitialize()
     * @see void CATerminate()
     * @objective Test 'CASelectNetwork' negatively with out of upper boundary
     *            value of selected network [UOBV]
     * @target CAResult_t CASelectNetwork(CATransportAdapter_t
     *         interestedNetwork)
     * @test_data upper boundary value as network argument
     * @pre_condition Initialize CA using CAInitialize
     * @procedure 1. Call CASelectNetwork using out of upper boundary value as
     *            network argument 2. Check it's return value
     * @post_condition Terminate CA using CATerminate function
     * @expected It will return CA_NOT_SUPPORTED
     */
    public void testCASelectNetwork_UOBV_N() {
        if (!CAJni.initialize()) {
            fail();
            return;
        }

        if (!CAJni.selectCustomNetwork(CA_INVALID_UOBV_ADAPTER,
                CA_NOT_SUPPORTED)) {
            fail();
        }

        CAJni.terminate();
    }

    /**
     * @since 2016-03-01
     * @see CAResult_t CAInitialize()
     * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
     * @see void CATerminate()
     * @objective Test 'CAUnSelectNetwork' positively with upper boundary value
     *            of selected network [UBV]
     * @target CAResult_t CAUnSelectNetwork(CATransportAdapter_t
     *         nonInterestedNetwork)
     * @test_data upper boundary value as Network argument
     * @pre_condition Initialize CA using CAInitialize
     * @procedure 1. Call CASelectNetwork using upper boundary value as network
     *            argument 2. Check it's return value 3. Call CAUnselectNetwork
     *            using upper boundary value value as network argument 4. Check
     *            it's return value
     * @post_condition Terminate CA using CATerminate function
     * @expected It will unselect the network and will return CA_STATUS_OK
     */
    public void testCAUnSelectNetwork_UBV_P() {
        if (!CAJni.initialize()) {
            fail();
            return;
        }

        if (!CAJni.selectCustomNetwork(
                CA_ALL_ADAPTERS, CA_STATUS_OK)) {
            fail();
        }

        if (!CAJni.unSelectCustomNetwork(
                CA_ALL_ADAPTERS, CA_STATUS_OK)) {
            fail();
        }

        CAJni.terminate();
    }

    /**
     * @since 2014-09-17
     * @see CAResult_t CAInitialize()
     * @see void CATerminate()
     * @objective Test 'CAUnSelectNetwork' negatively to un-select invalid
     *            network
     * @target CAResult_t CAUnSelectNetwork(const uint32_t
     *         nonInterestedNetwork);
     * @test_data invalid value for which network interface is not defined
     * @pre_condition Initialize CA using CAInitialize
     * @procedure 1. Call CAUnSelectNetwork with that network 2. Check it's
     *            return value
     * @post_condition Terminate CA using CATerminate
     * @expected It will fail to un-select the network and won't return
     *           CA_STATUS_OK
     */
    public void testCAUnSelectNetwork_N() {
        if (!CAJni.initialize()) {
            fail();
            return;
        }

        if (CAJni.unSelectCustomNetwork(CA_DEFAULT_ADAPTER, CA_STATUS_OK)) {
            fail();
        }

        CAJni.terminate();
    }

    /**
     * @since 2014-09-17
     * @see CAResult_t CAInitialize()
     * @see void CATerminate()
     * @objective Test CAHandleRequestResponse() API positively
     * @target CAResult_t CAHandleRequestResponse()
     * @test_data none
     * @pre_condition Initialize CA using CAInitialize()
     * @procedure 1. Call CAHandleRequestResponse() API 2. Check it's return
     *            value
     * @post_condition Terminate CA using CATerminate()
     * @expected CAHandleRequestResponse() will return CA_STATUS_OK
     */
    public void testCAHandleRequestResponse_P() {
        if (!CAJni.initialize()) {
            fail();
            return;
        }

        if (!CAJni.handleRequestResponse()) {
            fail();
        }

        CAJni.terminate();
    }

    /**
     * @since 2016-02-29
     * @see CAResult_t CAInitialize()
     * @see void CATerminate()
     * @objective Test 'CAStartListeningServer' without selecting a network
     *            [ECRC]
     * @target CAResult_t CAStartListeningServer()
     * @test_data none
     * @pre_condition 1. Initialize CA using CAInitialize 2. Do not select
     *                network
     * @procedure 1. Call CAStartListeningServer API 2. Check it's return value
     * @post_condition Terminate CA using CATerminate
     * @expected It will return CA_STATUS_FAILED
     */
    public void testCAStartListeningServerWithoutSelectingNetwork_ECRC_N() {
        if (!CAJni.initialize()) {
            fail();
            return;
        }

        if (!CAJni.startListeningServer(CA_STATUS_FAILED)) {
            fail();
        }

        CAJni.terminate();
    }

    /**
     * @since 2016-02-29
     * @see CAResult_t CAInitialize()
     * @see void CATerminate()
     * @objective Test 'CAStopListeningServer' negatively without
     *            SelectingNetwork [ECRC]
     * @target CAResult_t CAStopListeningServer()
     * @test_data none
     * @pre_condition Initialize CA using CAInitialize
     * @procedure 1. Call the CAStopListeningServer API 2. Check it's return
     *            value
     * @post_condition Terminate CA using CATerminate
     * @expected It will return CA_STATUS_FAILED
     */
    public void testCAStopListeningServerWithoutSelectingNetwork_ECRC_N() {
        if (!CAJni.initialize()) {
            fail();
            return;
        }

        if (!CAJni.stopListeningServer(CA_STATUS_FAILED)) {
            fail();
        }

        CAJni.terminate();
    }

    /**
     * @since 2016-03-01
     * @see CAResult_t CAInitialize()
     * @see void CATerminate()
     * @objective Test 'CAStartDiscoveryServer' without selecting a network
     *            [ECRC]
     * @target CAResult_t CAStartDiscoveryServer()
     * @test_data none
     * @pre_condition 1. Initialize CA using CAInitialize 2. Do not select
     *                network
     * @procedure 1. Call the CAStartDiscoveryServer API 2. Check it's return
     *            value
     * @post_condition Terminate CA using CATerminate
     * @expected It will return CA_STATUS_FAILED
     */
    public void testCAStartDiscoveryServerWithoutSelectingNetwork_ECRC_N() {
        if (!CAJni.initialize()) {
            fail();
            return;
        }

        if (!CAJni.startDiscoveryServer(CA_STATUS_FAILED)) {
            fail();
        }

        CAJni.terminate();
    }

    /**
     * @since 2016-03-02
     * @see none
     * @objective Test 'CACreateEndpoint' negatively to check against inavlid
     *            value in Adapter [adapter-ECRC]
     * @target CAResult_t CACreateEndpoint(CATransportFlags_t flags,
     *         CATransportAdapter_t adapter, const char *addr, uint16_t port,
     *         CAEndpoint_t **object)
     * @test_data Invalid adapter
     * @pre_condition none
     * @procedure 1. call the CACreateEndpoint API and pass the invalid value as
     *            adapter arguments 2. Check it's return value
     * @post_condition none
     * @expected return CA_STATUS_OK
     */
    public void testCACreateEndpoint_Adapter_ECRC_N() {
        if (!CAJni.createEndpoint(
                ENDPOINT_INVALID_ADAPTER, CA_STATUS_OK)) {
            fail();
        }
    }
}
