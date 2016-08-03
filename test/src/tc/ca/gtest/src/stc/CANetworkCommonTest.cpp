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

class CANetworkCommonTest_stc: public ::testing::Test
{
protected:
    CAHelper m_caHelper;
    CAResult_t m_result;

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
 * @since 2015-01-15
 * @see CAResult_t CAInitialize()
 * @objective Without making any prior call to CAInitialize API, check the return value of other CA APIs
 * @target CAResult_t CAStartListeningServer()
 * @target CAResult_t CAStopListeningServer()
 * @target CAResult_t CAStartDiscoveryServer()
 * @target CAResult_t CASendRequest(const CAEndpoint_t *object, const CARequestInfo_t *requestInfo)
 * @target CAResult_t CASendResponse(const CAEndpoint_t *object, const CAResponseInfo_t *responseInfo)
 * @target CAResult_t CASelectNetwork(CATransportAdapter_t interestedNetwork)
 * @target CAResult_t CAUnSelectNetwork(CATransportAdapter_t nonInterestedNetwork)
 * @target CAResult_t CAGetNetworkInformation(CAEndpoint_t **info, uint32_t *size)
 * @target CAResult_t CAHandleRequestResponse()
 * @test_data none
 * @pre_condition Do not call CAInitialize
 * @procedure call 1. Call CAStartListeningServer
 *                 2. Check it's return value
 *                 3. Call CAStopListeningServer
 *                 4. Check it's return value
 *                 5. Call CAStartDiscoveryServer
 *                 6. Check it's return value
 *                 7. Call CASendRequest
 *                 8. Check it's return value
 *                 9. Call CASendResponse
 *                 10. Check it's return value
 *                 11. Call CASelectNetwork
 *                 12. Check it's return value
 *                 13. Call CAUnSelectNetwork
 *                 14. Check it's return value
 *                 15. Call CAGetNetworkInformation
 *                 16. Check it's return value
 *                 17. Call CAHandleRequestResponse
 *                 18. Check it's return value
 * @post_condition none
 * @expected All of them will return CA_STATUS_NOT_INITIALIZED
 */
#if ((defined(__LINUX__) || defined(__TIZEN__)) && defined(__ALL_TRANSPORT__)) || (defined(__ANDROID__) && (defined(__IP__) || defined(__TCP__)))
TEST_F(CANetworkCommonTest_stc, CallOtherCAApiWithoutCallingCAInitialize_N)
{
    CAEndpoint_t* endpoint = NULL;
    uint32_t size = 0;
    CARequestInfo_t requestInfo =
    {   0};
    CAResponseInfo_t responseInfo =
    {   0};

    m_result = CAStartListeningServer();
    if (m_result != CA_STATUS_NOT_INITIALIZED)
    {
        SET_FAILURE(m_caHelper.getFailureMessage("CAStartListeningServer", m_result, CA_STATUS_NOT_INITIALIZED));
    }

    m_result = CAStopListeningServer();
    if (m_result != CA_STATUS_NOT_INITIALIZED)
    {
        SET_FAILURE(m_caHelper.getFailureMessage("CAStopListeningServer", m_result, CA_STATUS_NOT_INITIALIZED));
    }

    m_result = CAStartDiscoveryServer();
    if (m_result != CA_STATUS_NOT_INITIALIZED)
    {
        SET_FAILURE(m_caHelper.getFailureMessage("CAStartDiscoveryServer", m_result, CA_STATUS_NOT_INITIALIZED));
    }

    m_result = CASendRequest(endpoint, &requestInfo);
    if (m_result != CA_STATUS_NOT_INITIALIZED)
    {
        SET_FAILURE(m_caHelper.getFailureMessage("CASendRequest", m_result, CA_STATUS_NOT_INITIALIZED));
    }

    m_result = CASendResponse(endpoint, &responseInfo);
    if (m_result != CA_STATUS_NOT_INITIALIZED)
    {
        SET_FAILURE(m_caHelper.getFailureMessage("CASendResponse", m_result, CA_STATUS_NOT_INITIALIZED));
    }

    m_result = CASelectNetwork(m_caHelper.m_availableNetwork);
    if (m_result != CA_STATUS_NOT_INITIALIZED)
    {
        SET_FAILURE(m_caHelper.getFailureMessage("CASelectNetwork", m_result, CA_STATUS_NOT_INITIALIZED));
    }

    m_result = CAUnSelectNetwork(m_caHelper.m_availableNetwork);
    if (m_result != CA_STATUS_NOT_INITIALIZED)
    {
        SET_FAILURE(m_caHelper.getFailureMessage("CAUnSelectNetwork", m_result, CA_STATUS_NOT_INITIALIZED));
    }

    m_result = CAGetNetworkInformation(&endpoint, &size);
    if (m_result != CA_STATUS_NOT_INITIALIZED)
    {
        SET_FAILURE(m_caHelper.getFailureMessage("CAGetNetworkInformation", m_result, CA_STATUS_NOT_INITIALIZED));
    }

    if(endpoint != NULL)
    {
        free(endpoint);
    }

    m_result = CAHandleRequestResponse();
    if (m_result != CA_STATUS_NOT_INITIALIZED)
    {
        SET_FAILURE(m_caHelper.getFailureMessage("CAHandleRequestResponse", m_result, CA_STATUS_NOT_INITIALIZED));
    }
}
#endif

/**
 * @since 2014-12-15
 * @see none
 * @objective Test 'CAInitialize', 'CATerminate' and other CA API in loops
 * @target  CAResult_t CAInitialize()
 * @target CAResult_t CAStartListeningServer()
 * @target CAResult_t CASelectNetwork(CATransportAdapter_t interestedNetwork)
 * @target  void CATerminate()
 * @test_data none
 * @pre_condition none
 * @procedure 1. Call CAInitialize API
 *            2. Check it's return value
 *            3. Call CASelectNetwork API
 *            4. Check it's return value
 *            5. Call CAStartListeningServer API
 *            6. Check it's return value
 *            7. Call CATerminate API
 *            8. Repeat step 1-7 multiple times
 * @post_condition none
 * @expected All APIs will return CA_STATUS_OK
 */
#if ((defined(__LINUX__) || defined(__TIZEN__)) && defined(__ALL_TRANSPORT__)) || (defined(__ANDROID__) && (defined(__IP__) || defined(__TCP__)))
TEST_F(CANetworkCommonTest_stc, CAInitializeAndCATerminateLoopWithOtherFn_P)
{
    for (int count = 0; count < TRY_COUNT; count++)
    {
        if (!m_caHelper.initialize())
        {
            SET_FAILURE(m_caHelper.getFailureMessage());
            return;
        }

        if (!m_caHelper.selectNetwork())
        {
            SET_FAILURE(m_caHelper.getFailureMessage());
            CATerminate();
            return;
        }

        if (!m_caHelper.startListeningServer())
        {
            SET_FAILURE(m_caHelper.getFailureMessage());
            CATerminate();
            return;
        }

        CATerminate();
    }
}
#endif

/**
 * @since 2016-02-22
 * @see CAResult_t CAInitialize()
 * @see void CATerminate()
 * @objective Test 'CASelectNetwork' and 'CAUnSelectNetwork' with state transition condition check [STCC]
 * @target CAResult_t CASelectNetwork(CATransportAdapter_t interestedNetwork)
 * @target CAResult_t CAUnSelectNetwork(CATransportAdapter_t nonInterestedNetwork)
 * @test_data try to unselect-select-unselect network
 * @pre_condition call CAinitialize
 * @procedure  1. Call CAUnSelectNetwork
 *             2. Check it's return value as CA_STATUS_FAILED
 *             3. Call CASelectNetwork
 *             4. Check it's return value
 *             5. Call CAUnSelectNetwork
 *             6. Check it's return value
 * @post_condition Terminate CA using CATerminate function
 * @expected 2. It will fail to unselect network initially
 *           4. It will select network
 *           6. It will unselect network
 */
#if ((defined(__LINUX__) || defined(__TIZEN__)) && defined(__ALL_TRANSPORT__)) || (defined(__ANDROID__) && (defined(__IP__) || defined(__TCP__)))
TEST_F(CANetworkCommonTest_stc, CASelectNetworkUnSelectNetwor_STCC_P)
{
    if (!m_caHelper.initialize())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if (!m_caHelper.unselectNetwork(m_caHelper.m_availableNetwork, CA_STATUS_FAILED))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.selectNetwork(m_caHelper.m_availableNetwork, CA_STATUS_OK))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.unselectNetwork(m_caHelper.m_availableNetwork, CA_STATUS_OK))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    CATerminate();
}
#endif

/**
 * @since 2016-02-22
 * @see CAResult_t CAInitialize()
 * @see void CATerminate()
 * @objective Test 'CAUnSelectNetwork' as a loop check without selecting any network [ITLC]
 * @target CAResult_t CAUnSelectNetwork(CATransportAdapter_t nonInterestedNetwork)
 * @test_data try to unselect network multiple times
 * @pre_condition do not call CASelectNetwork
 * @procedure  1. Call CAInitialize
 *             2. Check it's return value
 *             3. Call CAUnSelectNetwork
 *             4. Check it's return value as CA_STAUS_FAILED
 *             5. Repeat steps 3-4 for multiple times
 * @post_condition Terminate CA using CATerminate function
 * @expected It will always return CA_STATUS_FAILED
 */
#if ((defined(__LINUX__) || defined(__TIZEN__)) && defined(__ALL_TRANSPORT__)) || (defined(__ANDROID__) && (defined(__IP__) || defined(__TCP__)))
TEST_F(CANetworkCommonTest_stc, CAUnSelectNetworkLoop_ITLC_N)
{
    if (!m_caHelper.initialize())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    for (int count = 0; count < TRY_COUNT; count++)
    {
        if (!m_caHelper.unselectNetwork(m_caHelper.m_availableNetwork, CA_STATUS_FAILED))
        {
            SET_FAILURE(m_caHelper.getFailureMessage());
            CATerminate();
            return;
        }
    }

    CATerminate();
}
#endif

/**
 * @since 2016-02-22
 * @see CAResult_t CAInitialize()
 * @see void CATerminate()
 * @objective Test 'CAUnSelectNetwork' with network type which is not selected in 'CASelectNetwork' [EG]
 * @target CAResult_t CASelectNetwork(CATransportAdapter_t interestedNetwork)
 * @target CAResult_t CAUnSelectNetwork(CATransportAdapter_t nonInterestedNetwork)
 * @test_data try to unselect network
 * @pre_condition call CAinitialize
 * @procedure  1. Call CASelectNetwork
 *             2. Check it's return value
 *             3. Call CAUnSelectNetwork with another network type that is not selected in step 1
 *             4. Check it's return value as CA_STATUS_FAILED
 *             5. Call CAUnselectNetwork with network type as selected in step 1
 *             6. Check it's return value as CA_STATUS_OK
 * @post_condition Terminate CA using CATerminate function
 * @expected 4. It will return CA_STATUS_FAILED
 */
#if ((defined(__LINUX__) || defined(__TIZEN__)) && defined(__ALL_TRANSPORT__)) || (defined(__ANDROID__) && (defined(__IP__) || defined(__TCP__)))
TEST_F(CANetworkCommonTest_stc, CASelectNetworkUnSelectNetwork_EG_N)
{
    if (!m_caHelper.initialize())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if (!m_caHelper.selectNetwork(m_caHelper.m_availableNetwork, CA_STATUS_OK))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.unselectNetwork(CA_DEFAULT_ADAPTER, CA_STATUS_FAILED))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    if (!m_caHelper.unselectNetwork(m_caHelper.m_availableNetwork, CA_STATUS_OK))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    CATerminate();
}
#endif

/**
 * @since 2016-02-23
 * @see CAResult_t CAInitialize()
 * @see void CATerminate()
 * @see CAResult_t CAStartListeningServer()
 * @objective Test CASelectNetwork and CAUnselectNetwork with checking the network status [SCV]
 * @target CAResult_t CASelectNetwork(CATransportAdapter_t interestedNetwork)
 * @target CAResult_t CAUnSelectNetwork(CATransportAdapter_t nonInterestedNetwork)
 * @test_data none
 * @pre_condition Initialize CA using CAInitialize
 * @procedure  1. Call CASelectNetwork
 *             2. Call CAStartListeningServer
 *             3. Check its return value
 *             4. Call CAUnSelectNetwork
 *             5. Call CAStartListeningServer
 *             6. Check its return value
 * @post_condition Terminate CA using CATerminate
 * @expected 2. It will return CA_STATUS_OK
 *           6. It will return CA_STATUS_FAILED
 */
#if ((defined(__LINUX__) || defined(__TIZEN__)) && defined(__ALL_TRANSPORT__)) || (defined(__ANDROID__) && (defined(__IP__) || defined(__TCP__)))
TEST_F(CANetworkCommonTest_stc, CASelectNetworkUnSelectNetwork_SCV_N)
{
    if (!m_caHelper.initServerNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if (!m_caHelper.unselectNetwork())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    if (!m_caHelper.startListeningServer(CA_STATUS_FAILED))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    CATerminate();
}
#endif

/**
 * @since 2016-02-23
 * @see CAResult_t CAInitialize()
 * @see void CATerminate()
 * @objective Test CASelectNetwork and CAUnselectNetwork in a loop [SLCC]
 * @target CAResult_t CASelectNetwork(CATransportAdapter_t interestedNetwork)
 * @target CAResult_t CAUnSelectNetwork(CATransportAdapter_t nonInterestedNetwork)
 * @test_data none
 * @pre_condition Initialize CA using CAInitialize
 * @procedure  1. Call CASelectNetwork
 *             2. Call CAUnselectNetwork
 *             3. Repeat Steps 1~2 for multiple times
 * @post_condition Terminate CA using CATerminate
 * @expected It will select and unselect network in each time and will return CA_STATUS_OK
 */
#if ((defined(__LINUX__) || defined(__TIZEN__)) && defined(__ALL_TRANSPORT__)) || (defined(__ANDROID__) && (defined(__IP__) || defined(__TCP__)))
TEST_F(CANetworkCommonTest_stc, CASelectNetworkUnSelectNetworkLoop_SLCC_P)
{
    if (!m_caHelper.initialize())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    for (int count = 0; count < TRY_COUNT; count++)
    {
        if (!m_caHelper.selectNetwork())
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
    }

    CATerminate();
}
#endif

/**
 * @since 2016-02-23
 * @see CAResult_t CAInitialize()
 * @see void CATerminate()
 * @objective Test functionality of 'CAStartListeningServer' and 'CAStopListeningServer' in a loop [VLCC]
 * @target CAResult_t CASelectNetwork(CATransportAdapter_t interestedNetwork)
 * @target CAResult_t CAStartListeningServer()
 * @target CAResult_t CAStopListeningServer()
 * @test_data none
 * @pre_condition Initialize CA using CAInitialize
 * @procedure  1. Select one of the available networks
 *             2. Start the listening server
 *             3. Call the CAStopListeningServer API
 *             4. Check it's return value
 *             5. Repeat Steps 1~4 for multiple times
 * @post_condition Terminate CA using CATerminate
 * @expected It will start and stop the listening server and will return CA_STATUS_OK
 */
#if ((defined(__LINUX__) || defined(__TIZEN__)) && defined(__ALL_TRANSPORT__)) || (defined(__ANDROID__) && (defined(__IP__) || defined(__TCP__)))
TEST_F(CANetworkCommonTest_stc, CAStartListeningServerStopListeningServer_VLCC_P)
{

    CAResult_t m_result=NULL;

    for (int count = 0; count < TRY_COUNT; count++)
    {
        if (!m_caHelper.initServerNetwork())
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
    }

    CATerminate();
}
#endif

/**
 * @since 2016-02-24
 * @see CAResult_t CAInitialize()
 * @see CAResult_t CASelectNetwork(CATransportAdapter_t interestedNetwork)
 * @see void CATerminate()
 * @objective Test 'CAStartDiscoveryServer' positively to start the discovery service in a loop [VLCC]
 * @target CAResult_t CAStartDiscoveryServer()
 * @test_data none
 * @pre_condition 1. Initialize CA using CAInitialize
 *                2. Select network
 * @procedure  1. Call the CAStartDiscoveryServer API
 *             2. Check it's return value
 *             3. Repeat Steps 1~2 for multiple times
 * @post_condition Terminate CA using CATerminate function
 * @expected It will start the discovery server and will return CA_STATUS_OK
 */
#if ((defined(__LINUX__) || defined(__TIZEN__)) && defined(__ALL_TRANSPORT__)) || (defined(__ANDROID__) && (defined(__IP__) || defined(__TCP__)))
TEST_F(CANetworkCommonTest_stc, CAStartDiscoveryServer_VLCC_P)
{
    for (int count = 0; count < TRY_COUNT; count++)
    {

        if (!m_caHelper.initClientNetwork())
        {
            SET_FAILURE(m_caHelper.getFailureMessage());
            return;
        }

        CommonUtil::waitInSecond(SLEEP_TIME);
    }

    CATerminate();
}
#endif

/**
 * @since 2016-02-25
 * @see CAResult_t CAInitialize()
 * @see void CATerminate()
 * @objective Test functionality of 'CACreateEndpoint' and 'CADestroyEndpoint' in a loop [VLCC]
 * @target CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @target void CADestroyToken(CAToken_t token)
 * @test_data none
 * @pre_condition Initialize CA using CAInitialize
 * @procedure  1. Select network
 *             2. Call CACreateEndpoint API
 *             3. Check it's return value
 *             4. Call CADestroyEndpoint API
 *             5. Repeat Steps 1~4 for multiple times
 * @post_condition Terminate CA using CATerminate
 * @expected It will create and destroy end point and will return CA_STATUS_OK
 */
#if ((defined(__LINUX__) || defined(__TIZEN__)) && defined(__ALL_TRANSPORT__)) || (defined(__ANDROID__) && (defined(__IP__) || defined(__TCP__)))
TEST_F(CANetworkCommonTest_stc, CACreateEndpointDestroyEndpoint_VLCC_P)
{
    CAResult_t m_result=NULL;

    for (int count = 0; count < TRY_COUNT; count++)
    {
        if (!m_caHelper.initialize())
        {
            SET_FAILURE(m_caHelper.getFailureMessage());
            return;
        }

        CAEndpoint_t* m_endpoint = NULL;

        CAResult_t m_result = CACreateEndpoint(CA_DEFAULT_FLAGS, m_caHelper.m_availableNetwork, ENDPOINT_IP, ENDPOINT_PORT, &m_endpoint);
        if (m_result != CA_STATUS_OK || m_endpoint == NULL)
        {
            SET_FAILURE(m_caHelper.getFailureMessage("CACreateEndpoint", CA_STATUS_OK));
            CATerminate();
            return;
        }

       CADestroyEndpoint(m_endpoint);
    }

    CATerminate();
}
#endif
