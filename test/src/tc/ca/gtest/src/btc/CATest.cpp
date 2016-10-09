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

class CATest_btc: public ::testing::Test
{
protected:
    CAHelper m_caHelper;

    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();
    }

    virtual void TearDown()
    {
        CommonUtil::runCommonTCTearDownPart();
    }
};

/**
 * @since 2014-11-28
 * @see void CATerminate()
 * @objective Test 'CAInitialize' positively to initialize CA module
 * @target CAResult_t CAInitialize()
 * @test_data none
 * @pre_condition none
 * @procedure call CAInitialize() API
 * @post_condition Terminate CA using CATerminate() API
 * @expected Initialization will succeed and return CA_STATUS_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)
TEST_F(CATest_btc , CAInitialize_P)
{
    if (!m_caHelper.initialize())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    CATerminate();
}
#endif

/**
 * @since 2014-12-01
 * @see CAResult_t CAInitialize()
 * @see void CATerminate()
 * @objective Test 'CARegisterHandler' positively to register request & response handlers
 * @target void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler,
 CAErrorCallback ErrorHandler)
 * @test_data  1. A request handler
 *             2. A response handler
 *             2. A error handler
 * @pre_condition Initialize CA using CAInitialize()
 * @procedure call Call CARegisterHandler() API with request,response and error handlers as arguments
 * @post_condition Terminate CA using CATerminate() API
 * @expected CARegisterHandler() will register the handlers
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)
TEST_F(CATest_btc , CARegisterHandler_P)
{
    if (!m_caHelper.initialize())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    CARegisterHandler(CAHelper::requestHandler, CAHelper::responseHandler, CAHelper::errorHandler);

    CATerminate();
}
#endif

/**
 * @since 2014-11-28
 * @see void CADestroyToken(CAToken_t token)
 * @objective Test 'CAGenerateToken' positively to generate a token
 * @target CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @test_data a pointer of the type CAToken_t and token length
 * @pre_condition none
 * @procedure call the CAGenerateToken() API with passing a reference to the token and uint8_t length to tokenLength as argument
 * @post_condition Destroy token using CADestroyToken.
 * @expected 1. It will generate a token and return CA_STATUS_OK
 *           2. Token variable is not NULL
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)
TEST_F(CATest_btc , CAGenerateToken_P)
{
    if (!m_caHelper.generateToken())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    m_caHelper.destroyToken();
}
#endif

/**
 * @since 2016-02-18
 * @see void CADestroyToken(CAToken_t token)
 * @objective Test 'CAGenerateToken' positively with lower boundary value in Token Length parameter [LBV]
 * @target CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @test_data Lower boundary value (1) as token length argument
 * @pre_condition none
 * @procedure Call CAGenerateToken with lower boundary value (1) as token length argument
 * @post_condition Destroy token using CADestroyToken.
 * @expected 1. It will generate a token and return CA_STATUS_OK
 *           2. Token variable is not NULL
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)
TEST_F(CATest_btc , CAGenerateToken_LBV_P)
{
    CAToken_t token = NULL;
    CAResult_t m_result = CAGenerateToken(&token, TOKEN_MIN_LENGTH );
    if (token == NULL)
    {
        m_caHelper.m_failureMessage = "Failed to generate token.";
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    if (m_result != CA_STATUS_OK)
    {
        SET_FAILURE(m_caHelper.getFailureMessage("CAGenerateToken", m_result, CA_STATUS_OK ));
        return;
    }

    CADestroyToken(token);
}
#endif

/**
 * @since 2015-02-17
 * @see none
 * @objective Test 'CAGenerateToken' negatively with out of lower bundary value in Token Length parameter [LOBV]
 * @target CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @test_data out of lower bundary value as token length argument
 * @pre_condition none
 * @procedure Call CAGenerateToken with out of lower boundary value in token length parameter
 * @post_condition none
 * @expected It will return CA_STATUS_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)
TEST_F(CATest_btc , CAGenerateToken_LOBV_N)
{
    CAToken_t token = NULL;
    CAResult_t result = CAGenerateToken(&token, TOKEN_LOBV_LENGTH);

    if (result != CA_STATUS_INVALID_PARAM)
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        IOTIVITYTEST_LOG(ERROR, "CAGenerateToken, Returned:  %s, Expected: %s", m_caHelper.getResultName(result), m_caHelper.getResultName(CA_STATUS_INVALID_PARAM));
    }
}
#endif

/**
 * @since 2015-02-17
 * @see none
 * @objective Test 'CAGenerateToken' negatively with out of upper boundary value in Token Length parameter [UOBV]
 * @target CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @test_data out of upper boundary value as token length argument
 * @pre_condition none
 * @procedure Call CAGenerateToken with out of upper boundary value in token length parameter
 * @post_condition none
 * @expected It will return CA_STATUS_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)
TEST_F(CATest_btc , CAGenerateToken_UOBV_N)
{
    CAToken_t m_token = NULL;
    CAResult_t m_result = CAGenerateToken(&m_token, TOKEN_UOBV_LENGTH);

    if (m_result != CA_STATUS_INVALID_PARAM)
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        IOTIVITYTEST_LOG(ERROR, "CAGenerateToken, Returned:  %s, Expected: %s", m_caHelper.getResultName(m_result), m_caHelper.getResultName(CA_STATUS_INVALID_PARAM));
    }
}
#endif

/**
 * @since 2016-02-18
 * @see none
 * @objective Test 'CAGenerateToken' negatively with passing invalid refrerence in token parameter [UFRV]
 * @target CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @test_data Invalid reference as token argument
 * @pre_condition none
 * @procedure Call CAGenerateToken with unformatted reference value in token type parameter
 * @post_condition none
 * @expected It will not generate token & will return CA_STATUS_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)
TEST_F(CATest_btc , CAGenerateToken_UFRV_N)
{
    CAToken_t *m_token = NULL;
    CAResult_t m_result = CAGenerateToken(m_token, TOKEN_MAX_LENGTH);

    if (m_result != CA_STATUS_INVALID_PARAM)
    {
        SET_FAILURE(m_caHelper.getFailureMessage("CAGenerateToken", m_result, CA_STATUS_INVALID_PARAM));
    }
}
#endif

/**
 * @since 2014-12-02
 * @see none
 * @objective Test 'CAGenerateToken' negatively with passing NULL value as token argument
 * @target CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @test_data NULL as token argument
 * @pre_condition none
 * @procedure Call CAGenerateToken with NULL value as token argument
 * @post_condition none
 * @expected It will return CA_STATUS_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)
TEST_F(CATest_btc , CAGenerateToken_N)
{
    if (!m_caHelper.generateToken(CA_STATUS_INVALID_PARAM))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }
}
#endif

/**
 * @since 2016-02-22
 * @see CAResult_t CAInitialize()
 * @see void CATerminate()
 * @objective Test 'CASelectNetwork' positively with lower boundary value of selected network [LBV]
 * @target CAResult_t CASelectNetwork(CATransportAdapter_t interestedNetwork)
 * @test_data lower boundary value as network argument
 * @pre_condition Initialize CA using CAInitialize
 * @procedure  1. Call CASelectNetwork using lower boundary value as network argument
 *             2. Check it's return value
 * @post_condition Terminate CA using CATerminate function
 * @expected It will return CA_NOT_SUPPORTED
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)
TEST_F(CATest_btc, CASelectNetwork_LBV_P)
{
    if (!m_caHelper.initialize())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if (!m_caHelper.selectNetwork(CA_DEFAULT_ADAPTER, CA_NOT_SUPPORTED))
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
 * @objective Test 'CASelectNetwork' positively with upper boundary value of selected network [UBV]
 * @target CAResult_t CASelectNetwork(CATransportAdapter_t interestedNetwork)
 * @test_data upper boundary value as network argument
 * @pre_condition Initialize CA using CAInitialize
 * @procedure  1. Call CASelectNetwork using upper boundary value as network argument
 *             2. Check it's return value
 * @post_condition Terminate CA using CATerminate function
 * @expected It will return CA_STATUS_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)
TEST_F(CATest_btc, CASelectNetwork_UBV_P)
{
    if (!m_caHelper.initialize())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if (!m_caHelper.selectNetwork(CA_ALL_ADAPTERS, CA_STATUS_OK))
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
 * @objective Test 'CASelectNetwork' negatively with out of lower boundary value of selected network [LOBV]
 * @target CAResult_t CASelectNetwork(CATransportAdapter_t interestedNetwork)
 * @test_data out of lower boundary value as Network argument
 * @pre_condition Initialize CA using CAInitialize
 * @procedure  1. Call CASelectNetwork using out of lower boundary value as network argument
 *             2. Check it's return value
 * @post_condition Terminate CA using CATerminate function
 * @expected It will return CA_NOT_SUPPORTED
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)
TEST_F(CATest_btc, CASelectNetwork_LOBV_N)
{
    if (!m_caHelper.initialize())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    IOTIVITYTEST_LOG(DEBUG, "[CASelectNetwork] IN ");

    m_caHelper.m_result = CASelectNetwork(CA_INVALID_ADAPTER);

    if (m_caHelper.m_result != CA_NOT_SUPPORTED)
    {
        SET_FAILURE(m_caHelper.getFailureMessage("CASelectNetwork", m_caHelper.m_result, CA_NOT_SUPPORTED));
    }

    IOTIVITYTEST_LOG(DEBUG, "[CASelectNetwork] OUT ");

    CATerminate();
}
#endif

/**
 * @since 2016-02-23
 * @see CAResult_t CAInitialize()
 * @see void CATerminate()
 * @objective Test 'CASelectNetwork' negatively with out of upper boundary value of selected network [UOBV]
 * @target CAResult_t CASelectNetwork(CATransportAdapter_t interestedNetwork)
 * @test_data upper boundary value as network argument
 * @pre_condition Initialize CA using CAInitialize
 * @procedure  1. Call CASelectNetwork using out of upper boundary value as network argument
 *             2. Check it's return value
 * @post_condition Terminate CA using CATerminate function
 * @expected It will return CA_NOT_SUPPORTED
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)
TEST_F(CATest_btc, CASelectNetwork_UOBV_N)
{
    if (!m_caHelper.initialize())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if (!m_caHelper.selectNetwork(CA_INVALID_UOBV_ADAPTER, CA_NOT_SUPPORTED))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    CATerminate();
}
#endif

/**
 * @since 2014-12-04
 * @see CAResult_t CAInitialize()
 * @see void CATerminate()
 * @objective Test 'CASelectNetwork' negatively to select invalid network
 * @target CAResult_t CASelectNetwork(CATransportAdapter_t interestedNetwork)
 * @test_data invalid value for which network interface is not defined
 * @pre_condition Initialize CA using CAInitialize
 * @procedure  1. Call CASelectNetwork using invalid network value
 *             2. Check it's return value
 * @post_condition Terminate CA using CATerminate function
 * @expected It will fail to select a network and will return CA_NOT_SUPPORTED
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)
TEST_F(CATest_btc, CASelectNetwork_N)
{
    if (!m_caHelper.initialize())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if (!m_caHelper.selectNetwork(NETWORK_OUT_OF_BOUNDARY_VALUE, CA_NOT_SUPPORTED))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    CATerminate();
}
#endif

/**
 * @since 2016-02-23
 * @see CAResult_t CAInitialize()
 * @see CAResult_t CASelectNetwork(const uint32_t interestedNetwork)
 * @see void CATerminate()
 * @objective Test 'CAUnSelectNetwork' positively with upper boundary value of selected network [UBV]
 * @target CAResult_t CAUnSelectNetwork(CATransportAdapter_t nonInterestedNetwork)
 * @test_data upper boundary value as Network argument
 * @pre_condition Initialize CA using CAInitialize
 * @procedure  1. Call CASelectNetwork using upper boundary value as network argument
 *             2. Check it's return value
 *             3. Call CAUnselectNetwork using upper boundary value value as network argument
 *             4. Check it's return value
 * @post_condition Terminate CA using CATerminate function
 * @expected It will unselect the network and will return CA_STATUS_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)
TEST_F(CATest_btc, CAUnSelectNetwork_UBV_P)
{
    if (!m_caHelper.initialize())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if (!m_caHelper.selectNetwork(CA_ALL_ADAPTERS, CA_STATUS_OK))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        CATerminate();
        return;
    }

    if (!m_caHelper.unselectNetwork(CA_ALL_ADAPTERS, CA_STATUS_OK))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    CATerminate();
}
#endif

/**
 * @since 2014-11-28
 * @see CAResult_t CAInitialize()
 * @see void CATerminate()
 * @objective Test 'CAUnSelectNetwork' negatively to un-select invalid network
 * @target CAResult_t CAUnSelectNetwork(CATransportAdapter_t nonInterestedNetwork)
 * @test_data invalid value for which adapter is not defined
 * @pre_condition Initialize CA using CAInitialize
 * @procedure  1. Call CAUnSelectNetwork with invalid adapter
 *             2. Check it's return value
 * @post_condition Terminate CA using CATerminate
 * @expected It will fail to un-select the network and will return CA_STATUS_FAILED
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)
TEST_F(CATest_btc, CAUnSelectNetwork_N)
{
    if (!m_caHelper.initialize())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if (!m_caHelper.unselectNetwork(NETWORK_OUT_OF_BOUNDARY_VALUE, CA_STATUS_FAILED))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    CATerminate();
}
#endif

/**
 * @since 2014-12-02
 * @see CAResult_t CAInitialize()
 * @see void CATerminate()
 * @objective Test CAHandleRequestResponse() API positively
 * @target CAResult_t CAHandleRequestResponse()
 * @test_data none
 * @pre_condition Initialize CA using CAInitialize()
 * @procedure  1. Call CAHandleRequestResponse() API
 *             2. Check it's return value
 * @post_condition Terminate CA using CATerminate()
 * @expected CAHandleRequestResponse() will return CA_STATUS_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)
TEST_F(CATest_btc , CAHandleRequestResponse_P)
{
    if (!m_caHelper.initialize())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    CAResult_t result = CAHandleRequestResponse();
    if (result != CA_STATUS_OK)
    {
        SET_FAILURE(m_caHelper.getFailureMessage("CAHandleRequestResponse", result, CA_STATUS_OK));
    }

    CATerminate();
}
#endif

/**
 * @since 2016-02-23
 * @see CAResult_t CAInitialize()
 * @see void CATerminate()
 * @objective Test 'CAStartListeningServer' without selecting a network [ECRC]
 * @target CAResult_t CAStartListeningServer()
 * @test_data none
 * @pre_condition  1. Initialize CA using CAInitialize
 *                 2. Do not select network
 * @procedure  1. Call CAStartListeningServer API
 *             2. Check it's return value
 * @post_condition Terminate CA using CATerminate
 * @expected It will return CA_STATUS_FAILED
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)
TEST_F(CATest_btc, CAStartListeningServerWithoutSelectingNetwork_ECRC_N)
{

    CAResult_t m_result=NULL;

    if (!m_caHelper.initialize())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if (!m_caHelper.startListeningServer(CA_STATUS_FAILED))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    CATerminate();
}
#endif

/**
 * @since 2015-02-16
 * @see CAResult_t CAInitialize()
 * @see void CATerminate()
 * @objective Test 'CAStopListeningServer' negatively without SelectingNetwork [ECRC]
 * @target CAResult_t CAStopListeningServer()
 * @test_data none
 * @pre_condition Initialize CA using CAInitialize
 * @procedure 1. Call the CAStopListeningServer API
 *            2. Check it's return value
 * @post_condition Terminate CA using CATerminate
 * @expected It will return CA_STATUS_FAILED
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)
TEST_F(CATest_btc, CAStopListeningServerWithoutSelectingNetwork_ECRC_N)
{
    if (!m_caHelper.initialize())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    CAResult_t m_result=NULL;

    if (!m_caHelper.stopListeningServer(CA_STATUS_FAILED))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    CATerminate();
}
#endif

/**
 * @since 2016-02-24
 * @see CAResult_t CAInitialize()
 * @see void CATerminate()
 * @objective Test 'CAStartDiscoveryServer' without selecting a network [ECRC]
 * @target CAResult_t CAStartDiscoveryServer()
 * @test_data none
 * @pre_condition  1. Initialize CA using CAInitialize
 *                 2. Do not select network
 * @procedure  1. Call the CAStartDiscoveryServer API
 *             2. Check it's return value
 * @post_condition Terminate CA using CATerminate
 * @expected It will return CA_STATUS_FAILED
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)
TEST_F(CATest_btc, CAStartDiscoveryServerWithoutSelectingNetwork_ECRC_N)
{
    CAResult_t m_result=NULL;

    if (!m_caHelper.initialize())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if (!m_caHelper.startDiscoveryServer(CA_STATUS_FAILED))
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    CATerminate();
}
#endif

/**
 * @since 2016-02-24
 * @see none
 * @objective Test 'CACreateEndpoint' to check against negative value in Adapter [adapter-ECRC]
 * @target CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @test_data  Invalid adapter
 * @pre_condition none
 * @procedure  1. call the CACreateEndpoint API and pass the invalid value as adapter arguments
 *             2. Check it's return value
 * @post_condition none
 * @expected It will fail to create end point and return CA_STATUS_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)
TEST_F(CATest_btc, CACreateEndpoint_Adapter_ECRC_N)
{
    CAEndpoint_t* m_endpoint = NULL;

    CAResult_t m_result = CACreateEndpoint(CA_DEFAULT_FLAGS, CA_INVALID_ADAPTER, ENDPOINT_IP, ENDPOINT_PORT, &m_endpoint);
    if (m_result != CA_STATUS_OK)
    {
        SET_FAILURE(m_caHelper.getFailureMessage("CACreateEndpoint", m_result, CA_STATUS_OK));
    }
}
#endif

/**
 * @since 2016-02-25
 * @see none
 * @objective Test 'CADestroyEndpoint' negatively to check with NULL value [NV]
 * @target void CADestroyEndpoint(CAEndpoint_t *object)
 * @test_data  NULL value in endpoint
 * @pre_condition none
 * @procedure  1. call the CADestroyEndpoint API and pass the NULL value
 *             2. Check it's return value
 * @post_condition none
 * @expected It will invoke the API without any error/exception
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)
TEST_F(CATest_btc, CADestroyEndpoint_NV_N)
{
    CADestroyEndpoint(NULL);
}
#endif

#ifdef __WITH_DTLS__

/**
 * @since 2015-02-02
 * @see CAResult_t CAInitialize()
 * @see void CATerminate()
 * @objective Test CAregisterPskCredentialsHandler return value when a valid handler is registered
 * @target   CAResult_t CAregisterPskCredentialsHandler(CAGetDTLSPskCredentialsHandler GetDTLSCredentials)
 * @test_data OCDtlsPskCredsBlob object
 * @pre_condition Initialize CA using CAInitialize
 * @procedure call 1. Call CAregisterPskCredentialsHandler
 *                 2. Check it's return value
 * @post_condition Terminate CA using CATerminate
 * @expected CAregisterPskCredentialsHandler return value will be CA_STATUS_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)
TEST_F(CATest_btc, RegisterDtls_P)
{
    if (!m_caHelper.initialize())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    if(!m_caHelper.setDtls())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    CATerminate();

}
#endif

/**
 * @since 2015-02-04
 * @see CAResult_t CAInitialize()
 * @see void CATerminate()
 * @objective Test CAregisterPskCredentialsHandler return value when a invalid handler is registered
 * @target   CAResult_t CAregisterPskCredentialsHandler(CAGetDTLSPskCredentialsHandler GetDTLSCredentials)
 * @test_data NULL as GetDTLSCredentials
 * @pre_condition Initialize CA using CAInitialize
 * @procedure call 1. Call CAregisterPskCredentialsHandler with NULL handler
 *                 2. Check it's return value
 * @post_condition Terminate CA using CATerminate
 * @expected CAregisterPskCredentialsHandler return value won't be CA_STATUS_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)
TEST_F(CATest_btc, RegisterDtlsCredentialsWithNullHandler_N)
{
    if (!m_caHelper.initialize())
    {
        SET_FAILURE(m_caHelper.getFailureMessage());
        return;
    }

    CAResult_t result = CAregisterPskCredentialsHandler(NULL);
    if (result != CA_STATUS_OK)
    {
        SET_FAILURE("CAregisterPskCredentialsHandler not returned CA_STATUS_OK with NULL handler");
    }

    CATerminate();

}
#endif

#endif
