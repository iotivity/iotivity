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

public class CANetworkCommonTest extends AndroidTestCase {

    public CANetworkCommonTest() {
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
     * @since 2015-09-17
     * @see CAResult_t CAInitialize()
     * @objective Without making any prior call to CAInitialize API, check the
     *            return value of other CA APIs
     * @target CAResult_t CAStartListeningServer()
     * @target CAResult_t CAStopListeningServer()
     * @target CAResult_t CAStartDiscoveryServer()
     * @target CAResult_t CASendRequest(const CAEndpoint_t *object, const
     *         CARequestInfo_t *requestInfo)
     * @target CAResult_t CASendResponse(const CAEndpoint_t *object, const
     *         CAResponseInfo_t *responseInfo)
     * @target CAResult_t CASelectNetwork(CATransportAdapter_t
     *         interestedNetwork)
     * @target CAResult_t CAUnSelectNetwork(CATransportAdapter_t
     *         nonInterestedNetwork)
     * @target CAResult_t CAGetNetworkInformation(CAEndpoint_t **info, uint32_t
     *         *size)
     * @target CAResult_t CAHandleRequestResponse()
     * @test_data none
     * @pre_condition Do not call CAInitialize
     * @procedure call 1. Call CAStartListeningServer 2. Check it's return value
     *            3. Call CAStopListeningServer 4. Check it's return value 5.
     *            Call CAStartDiscoveryServer 6. Check it's return value 7. Call
     *            CASendRequest 8. Check it's return value 9. Call
     *            CASendResponse 10. Check it's return value 11. Call
     *            CASelectNetwork 12. Check it's return value 13. Call
     *            CAUnSelectNetwork 14. Check it's return value 15. Call
     *            CAGetNetworkInformation 16. Check it's return value 17. Call
     *            CAHandleRequestResponse 18. Check it's return value
     * @post_condition none
     * @expected All of them will return CA_STATUS_NOT_INITIALIZED
     */
    public void testCallOtherCAApiWithoutCallingCAInitialize_N() {

        if (!CAJni
                .startListeningServer(CA_STATUS_NOT_INITIALIZED)) {
            fail();
        }

        if (!CAJni.stopListeningServer(CA_STATUS_NOT_INITIALIZED)) {
            fail();
        }

        if (!CAJni
                .startDiscoveryServer(CA_STATUS_NOT_INITIALIZED)) {
            fail();
        }

        if (!CAJni.selectCustomNetwork(NETWORK,
                CA_STATUS_NOT_INITIALIZED)) {
            fail();
        }

        if (!CAJni.unSelectCustomNetwork(NETWORK,
                CA_STATUS_NOT_INITIALIZED)) {
            fail();
        }

        if (!CAJni.getNetworkInfomation(
                NETWORKINFO_ENDPOINT_VALID, NETWORKINFO_SIZE_VALID, CA_STATUS_NOT_INITIALIZED)) {
            fail();
        }
    }

    /**
     * @since 2015-09-17
     * @see none
     * @objective Test 'CAInitialize', 'CATerminate' and other CA API in loops
     * @target CAResult_t CAInitialize()
     * @target void CATerminate()
     * @test_data none
     * @pre_condition none
     * @procedure 1. Call CAInitialize API 2. Check it's return value 1. Call
     *            CASelectNetwork API 3. Check it's return value 4. Call
     *            CASelectNetwork API 5. Check it's return value 6. Call
     *            CATerminate API 7. Repeat step 1-6 multiple times
     * @post_condition none
     * @expected Initialization will succeed and return CA_STATUS_OK
     */
    public void testCAInitializeAndCATerminateLoopWithOtherFn_P() {
        for (int count = 0; count < TRY_COUNT; count++) {
            if (!CAJni.initialize()) {
                fail();
                return;
            }

            if (!CAJni.selectNetwork()) {
                fail();
            }

            if (!CAJni.unSelectNetwork()) {
                fail();
            }

            CAJni.terminate();
        }
    }

    /**
     * @since 2016-03-04
     * @see CAResult_t CAInitialize()
     * @see void CATerminate()
     * @objective Test 'CASelectNetwork' and 'CAUnSelectNetwork' with state
     *            transition condition check [STCC]
     * @target CAResult_t CASelectNetwork(CATransportAdapter_t
     *         interestedNetwork)
     * @target CAResult_t CAUnSelectNetwork(CATransportAdapter_t
     *         nonInterestedNetwork)
     * @test_data try to unselect-select-unselect network
     * @pre_condition call CAinitialize
     * @procedure 1. Call CAUnSelectNetwork 2. Check it's return value as
     *            CA_STATUS_FAILED 3. Call CASelectNetwork 4. Check it's return
     *            value 5. Call CAUnSelectNetwork 6. Check it's return value
     * @post_condition Terminate CA using CATerminate function
     * @expected 2. It will fail to unselect network initially 4. It will select
     *           network 6. It will unselect network
     */
    public void testCASelectNetworkUnSelectNetwor_STCC_P() {

        if (!CAJni.initialize()) {
            fail();
            return;
        }

        if (!CAJni.unSelectCustomNetwork(NETWORK,
                CA_STATUS_FAILED)) {
            fail();
        }

        if (!CAJni.selectNetwork()) {
            fail();
        }

        if (!CAJni.unSelectNetwork()) {
            fail();
        }

        CAJni.terminate();
    }

    /**
     * @since 2016-02-22
     * @see CAResult_t CAInitialize()
     * @see void CATerminate()
     * @objective Test 'CAUnSelectNetwork' as a loop check without selecting any
     *            network [ITLC]
     * @target CAResult_t CAUnSelectNetwork(CATransportAdapter_t
     *         nonInterestedNetwork)
     * @test_data try to unselect network multiple times
     * @pre_condition do not call CASelectNetwork
     * @procedure 1. Call CAInitialize 2. Check it's return value 3. Call
     *            CAUnSelectNetwork 4. Check it's return value as
     *            CA_STAUS_FAILED 5. Repeat steps 3-4 for multiple times
     * @post_condition Terminate CA using CATerminate function
     * @expected It will always return CA_STATUS_FAILED
     */
    public void testCAUnSelectNetworkLoop_ITLC_N() {
        if (!CAJni.initialize()) {
            fail();
            return;
        }

        for (int count = 0; count < TRY_COUNT; count++) {
            if (!CAJni.unSelectCustomNetwork(NETWORK,
                    CA_STATUS_FAILED)) {
                fail();
                return;
            }
        }

        CAJni.terminate();
    }

    /**
     * @since 2016-02-22
     * @see CAResult_t CAInitialize()
     * @see void CATerminate()
     * @objective Test 'CAUnSelectNetwork' with network type which is not
     *            selected in 'CASelectNetwork' [EG]
     * @target CAResult_t CASelectNetwork(CATransportAdapter_t
     *         interestedNetwork)
     * @target CAResult_t CAUnSelectNetwork(CATransportAdapter_t
     *         nonInterestedNetwork)
     * @test_data try to unselect network
     * @pre_condition call CAinitialize
     * @procedure 1. Call CASelectNetwork 2. Check it's return value 3. Call
     *            CAUnSelectNetwork with another network type that is not
     *            selected in step 1 4. Check it's return value as
     *            CA_STATUS_FAILED 5. Call CAUnselectNetwork with network type
     *            as selected in step 1 6. Check it's return value as
     *            CA_STATUS_OK
     * @post_condition Terminate CA using CATerminate function
     * @expected 4. It will return CA_STATUS_FAILED
     */
    public void testCASelectNetworkUnSelectNetwork_EG_N() {
        if (!CAJni.initialize()) {
            fail();
            return;
        }

        if (!CAJni.selectNetwork()) {
            fail();
            return;
        }

        if (!CAJni.unSelectCustomNetwork(0,
                CA_STATUS_FAILED)) {
            fail();
            return;
        }

        if (!CAJni.unSelectNetwork()) {
            fail();
            return;
        }

        CAJni.terminate();
    }

    /**
     * @since 2016-02-23
     * @see CAResult_t CAInitialize()
     * @see void CATerminate()
     * @see CAResult_t CAStartListeningServer()
     * @objective Test CASelectNetwork and CAUnselectNetwork with checking the
     *            network status [SCV]
     * @target CAResult_t CASelectNetwork(CATransportAdapter_t
     *         interestedNetwork)
     * @target CAResult_t CAUnSelectNetwork(CATransportAdapter_t
     *         nonInterestedNetwork)
     * @test_data none
     * @pre_condition Initialize CA using CAInitialize
     * @procedure 1. Call CASelectNetwork 2. Call CAStartListeningServer 3.
     *            Check its return value 4. Call CAUnSelectNetwork 5. Call
     *            CAStartListeningServer 6. Check its return value
     * @post_condition Terminate CA using CATerminate
     * @expected 2. It will return CA_STATUS_OK 6. It will return
     *           CA_STATUS_FAILED
     */
    public static void testCASelectNetworkUnSelectNetwork_SCV_N() {
        if (!CAJni.initServerNetwork()) {
            fail();
            return;
        }

        if (!CAJni.unSelectNetwork()) {
            fail();
            return;
        }

        if (!CAJni.startListeningServer(CA_STATUS_FAILED)) {
            fail();
            return;
        }

        CAJni.terminate();
    }

    /**
     * @since 2016-03-04
     * @see CAResult_t CAInitialize()
     * @see void CATerminate()
     * @objective Test CASelectNetwork and CAUnselectNetwork in a loop [SLCC]
     * @target CAResult_t CASelectNetwork(CATransportAdapter_t
     *         interestedNetwork)
     * @target CAResult_t CAUnSelectNetwork(CATransportAdapter_t
     *         nonInterestedNetwork)
     * @test_data none
     * @pre_condition Initialize CA using CAInitialize
     * @procedure 1. Call CASelectNetwork 2. Call CAUnselectNetwork 3. Repeat
     *            Steps 1~2 for multiple times
     * @post_condition Terminate CA using CATerminate
     * @expected It will select and unselect network in each time and will
     *           return CA_STATUS_OK
     */
    public void testCASelectNetworkUnSelectNetworkLoop_SLCC_P() {
        if (!CAJni.initialize()) {
            fail();
            return;
        }

        for (int count = 0; count < TRY_COUNT; count++) {

            if (!CAJni.selectNetwork()) {
                fail();
            }

            if (!CAJni.unSelectNetwork()) {
                fail();
            }
        }

        CAJni.terminate();
    }

    /**
     * @since 2016-03-04
     * @see CAResult_t CAInitialize()
     * @see void CATerminate()
     * @objective Test functionality of 'CAStartListeningServer' and
     *            'CAStopListeningServer' in a loop [VLCC]
     * @target CAResult_t CASelectNetwork(CATransportAdapter_t
     *         interestedNetwork)
     * @target CAResult_t CAStartListeningServer()
     * @target CAResult_t CAStopListeningServer()
     * @test_data none
     * @pre_condition Initialize CA using CAInitialize
     * @procedure 1. Select one of the available networks 2. Start the listening
     *            server 3. Call the CAStopListeningServer API 4. Check it's
     *            return value 5. Repeat Steps 1~4 for multiple times
     * @post_condition Terminate CA using CATerminate
     * @expected It will start and stop the listening server and will return
     *           CA_STATUS_OK
     */
    public void testCAStartListeningServerStopListeningServer_VLCC_P() {

        for (int count = 0; count < TRY_COUNT; count++) {

            if (!CAJni.initServerNetwork()) {
                fail();
                return;
            }

            if (!CAJni.stopListeningServer(CA_STATUS_OK)) {
                fail();
            }
        }

        CAJni.terminate();
    }

    /**
     * @since 2016-03-04
     * @see CAResult_t CAInitialize()
     * @see CAResult_t CASelectNetwork(CATransportAdapter_t interestedNetwork)
     * @see void CATerminate()
     * @objective Test 'CAStartDiscoveryServer' positively to start the
     *            discovery service in a loop [VLCC]
     * @target CAResult_t CAStartDiscoveryServer()
     * @test_data none
     * @pre_condition 1. Initialize CA using CAInitialize 2. Select network
     * @procedure 1. Call the CAStartDiscoveryServer API 2. Check it's return
     *            value 3. Repeat Steps 1~2 for multiple times
     * @post_condition Terminate CA using CATerminate function
     * @expected It will start the discovery server and will return CA_STATUS_OK
     * @expected It will start the discovery server and will return CA_STATUS_OK
     */
    public void testCAStartDiscoveryServer_VLCC_P() {
        for (int count = 0; count < TRY_COUNT; count++) {

            if (!CAJni.initClientNetwork()) {
                fail();
                return;
            }

        }

        CAJni.terminate();
    }

    /**
     * @since 2016-03-04
     * @see CAResult_t CAInitialize()
     * @see void CATerminate()
     * @objective Test functionality of 'CACreateEndpoint' and
     *            'CADestroyEndpoint' in a loop [VLCC]
     * @target CAResult_t CACreateEndpoint(CATransportFlags_t flags,
     *         CATransportAdapter_t adapter, const char *addr, uint16_t port,
     *         CAEndpoint_t **object)
     * @target void CADestroyToken(CAToken_t token)
     * @test_data none
     * @pre_condition Initialize CA using CAInitialize
     * @procedure 1. Select network 2. Call CACreateEndpoint API 3. Check it's
     *            return value 4. Call CADestroyEndpoint API 5. Repeat Steps 1~4
     *            for multiple times
     * @post_condition Terminate CA using CATerminate
     * @expected It will create and destroy end point and will return
     *           CA_STATUS_OK
     */
    public void testCACreateEndpointDestroyEndpoint_VLCC_P() {

        for (int count = 0; count < TRY_COUNT; count++) {

            if (!CAJni.initialize()) {
                fail();
                return;
            }

            if (!CAJni.selectNetwork()) {
                fail();
            }

            if (!CAJni.createEndpoint(ENDPOINT_VALID, CA_STATUS_OK)) {
                fail();
            }

            if (!CAJni.destroyEndpoint(ENDPOINT_VALID)) {
                fail();
            }

        }

        CAJni.terminate();
    }
}
