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

#define DELAY_TIME SLEEP_TIME * 1000
#define UINT16_T_PORT 6298
#define SPEED 115200
#define TESTLOG_START "Previous API Calls: ";
#define IP_ADDRESS_LOCAL "107.109.214.78";

CAHelper caHelper;
CAResult_t result;
String testLog;
int COUNT = 3;

bool isCATerminated = false;

void terminateCA()
{
    CATerminate();
    isCATerminated = true;
}

void setupTestCase()
{
    if(!isCATerminated)
    {
        terminateCA();
    }
}

void setup()
{
    Serial.begin(SPEED);

    Serial.println("================================");
    Serial.println("TestSuite: CACommonTest");
    Serial.println("================================");

    CAHelper::addFilter();

    caHelper.Connect(false);

    Serial.println("------------------------------------BEGIN------------------------------------");
}

void loop()
{
    Test::run();
}

/**
 * @since 2015-04-07
 * @see CAResult_t CAInitialize()
 * @objective Without making any prior call to CAInitialize API, check the return value of other CA APIs
 * @target  CAResult_t CASelectNetwork(const CATransportAdapter_t interestedNetwork)
 * @target  CAResult_t CAUnSelectNetwork(const CATransportAdapter_t nonInterestedNetwork)
 * @target  CAResult_t CAGetNetworkInformation(CALocalConnectivity_t **info, uint32_t *size)
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
test(CallOtherCAApiWithoutCallingCAInitialize_N)
{
    testLog = TESTLOG_START;
    CAEndpoint_t* endpoint = NULL;
    uint32_t size = 0;
    CARequestInfo_t requestInfo = {0};
    CAResponseInfo_t responseInfo = {0};
    
    result = CAStartListeningServer();
    assertNotEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CAStartListeningServer";
   
    result = CAStopListeningServer();
    assertNotEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CAStopListeningServer";

    result = CAStartDiscoveryServer();
    assertNotEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CAStartDiscoveryServer";

    result = CASendRequest(endpoint, &requestInfo);
    assertNotEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CASendRequest";

    result = CASendResponse(endpoint, &responseInfo);
    assertNotEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CASendResponse";

    result = CASelectNetwork(caHelper.m_availableNetwork);
    assertNotEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CASelectNetwork";

    result = CAUnSelectNetwork(caHelper.m_availableNetwork);
    assertNotEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CAUnSelectNetwork";
    
    result = CAGetNetworkInformation(&endpoint, &size);
    assertNotEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CAGetNetworkInformation";

    if(endpoint != NULL)
    {
        free(endpoint);
    }
    
    result = CAHandleRequestResponse();
    assertNotEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CAHandleRequestResponse";


}

/**
 * @since 2015-04-07
 * @see none
 * @objective Test 'CAInitialize' and 'CATerminate' in loops
 * @target  CAResult_t CAInitialize()
 * @target  void CATerminate()
 * @test_data none
 * @pre_condition none
 * @procedure 1. Call CAInitialize() function
 *            2. Check it's return value
 *            3. Call sleep to halt the program.
 *            4. Call CATerminate() functions
 *            5. Follow step 1-4 multiple times
 * @post_condition none
 * @expected each time CAInitialization will return CA_STATUS_OK
 */
test(CAInitializeAndCATerminateLoop_P)
{
    testLog = TESTLOG_START;
    for (int count = 0; count < COUNT; count++)
    {
        result = CAInitialize();
        assertEqual(result, CA_STATUS_OK, testLog);
        testLog += ", CAInitialize";

        delay(DELAY_TIME);

        CATerminate();
    }
}

/**
 * @since 2015-04-07
 * @see none
 * @objective Test 'CAInitialize', 'CATerminate' and other CA API in loops
 * @target  CAResult_t CAInitialize()
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
 *            8. Repeat step 1-6 multiple times
 * @post_condition none
 * @expected Initialization will succeed and return CA_STATUS_OK
 */
test(CAInitializeAndCATerminateLoopWithOtherFn_P)
{
    testLog = TESTLOG_START;

    for (int count = 0; count < COUNT; count++)
    {
        result = CAInitialize();
        assertEqual(result, CA_STATUS_OK, testLog);
        testLog += ", CAInitialize";

        result = CASelectNetwork(caHelper.m_availableNetwork);
        assertEqual(result, CA_STATUS_OK, testLog);
        testLog += ", CASelectNetwork";

        result = CAStartListeningServer();
        assertEqual(result, CA_STATUS_OK, testLog);
        testLog += ", CAStartListeningServer";

        CATerminate();
    }
}

/**
 * @since 2015-04-07
 * @see none
 * @objective Test multiple times 'CAInitialize' and multiple times 'CATerminate' calling effect
 * @target  CAResult_t CAInitialize()
 * @target  void CATerminate()
 * @test_data none
 * @pre_condition none
 * @procedure 1. Call CAInitialize API
 *            2. Check it's return value
 *            3. Call sleep to halt the program.
 *            4. Follow step 1-3 multiple(n) times
 *            5. Call CATerminate API
 *            6. Call sleep to halt the program.
 *            7. Repeat step 5-6 n times
 * @post_condition none
 * @expected APIs will return CA_STATUS_OK
 */
test(CAInitializeAndCATerminateLoop_N)
{
    testLog = TESTLOG_START;
    for (int count = 0; count < COUNT; count++)
    {
        result = CAInitialize();
        assertEqual(result, CA_STATUS_OK, testLog);
        testLog += ", CAInitialize";

        delay(DELAY_TIME);
    }

    for (int count = 0; count < COUNT; count++)
    {
        CATerminate();
        testLog += ", CATerminate";
        delay(DELAY_TIME);
    }
}

/**
 * @since 2016-03-11
 * @see CAResult_t CAInitialize()
 * @see void CATerminate()
 * @objective Test 'CAStartListeningServer' without selecting a network [DSCC]
 * @target CAResult_t CAStartListeningServer()
 * @test_data none
 * @pre_condition  1. Initialize CA using CAInitialize
 *                 2. Do not select network
 * @procedure  1. Call CAStartListeningServer API
 *             2. Check it's return value
 * @post_condition Terminate CA using CATerminate
 * @expected It will return not CA_STATUS_OK
 */
test(CAStartListeningServer_DSCC_N)
{
    CAResult_t result=NULL;

    setupTestCase();
    result = CA_STATUS_OK;
    testLog = TESTLOG_START;

    assertEqual(CAInitialize(), CA_STATUS_OK, "Initialization");
    testLog += "CAInitialize";
    isCATerminated = false;

    result = CAStartListeningServer();
    assertNotEqual(result, CA_STATUS_OK, testLog);
    testLog += "CAStartListeningServer";

    CATerminate();
}

/**
 * @since 2016-03-11
 * @see CAResult_t CAInitialize()
 * @see void CATerminate()
 * @objective Test 'CAStopListeningServer' without selecting a network [DSCC]
 * @target CAResult_t CAStopListeningServer()
 * @test_data none
 * @pre_condition  1. Initialize CA using CAInitialize
 *                 2. Do not select network
 * @procedure  1. Call the CAStopListeningServer API
 *             2. Check it's return value
 * @post_condition Terminate CA using CATerminate
 * @expected It will not return CA_STATUS_OK
 */
test(CAStopListeningServer_DSCC_N)
{
    CAResult_t result=NULL;

    setupTestCase();
    result = CA_STATUS_OK;
    testLog = TESTLOG_START;

    assertEqual(CAInitialize(), CA_STATUS_OK, "Initialization");
    testLog += "CAInitialize";
    isCATerminated = false;

    result = CAStopListeningServer();
    assertNotEqual(result, CA_STATUS_OK, testLog);
    testLog += "CAStopListeningServer";

    terminateCA();
}

/**
 * @since 2016-03-11
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
test(CAStartListeningServerStopListeningServer_VLCC_P)
{
    CAResult_t result=NULL;

    for (int count = 0; count < COUNT; count++)
    {
        setupTestCase();
        result = CA_STATUS_OK;
        testLog = TESTLOG_START;

        assertEqual(CAInitialize(), CA_STATUS_OK, "Initialization");
        testLog += "CAInitialize";
        isCATerminated = false;

        result = CASelectNetwork(caHelper.m_availableNetwork);
		assertEqual(result, CA_STATUS_OK, testLog);
        testLog += ", CASelectNetwork";
       
        result = CAStartListeningServer();
        assertEqual(result, CA_STATUS_OK, testLog);
        testLog += ", CAStartListeningServer";
            
        result = CAStopListeningServer();
        assertEqual(result, CA_STATUS_OK, testLog);
        testLog += ", CAStopListeningServer";      
    }

    CATerminate();
}

/**
 * @since 2016-03-11
 * @see CAResult_t CAInitialize()
 * @see void CATerminate()
 * @objective Test 'CAStartDiscoveryServer' without selecting a network [DSCC]
 * @target CAResult_t CAStartDiscoveryServer()
 * @test_data none
 * @pre_condition  1. Initialize CA using CAInitialize
 *                 2. Do not select network
 * @procedure  1. Call the CAStartDiscoveryServer API
 *             2. Check it's return value
 * @post_condition Terminate CA using CATerminate
 * @expected It will not return CA_STATUS_OK
 */
test(CAStartDiscoveryServer_DSCC_N)
{
    CAResult_t result=NULL;

    setupTestCase();
    result = CA_STATUS_OK;
    testLog = TESTLOG_START;

    assertEqual(CAInitialize(), CA_STATUS_OK, "Initialization");
    testLog += "CAInitialize";
    isCATerminated = false;

    result = CAStartDiscoveryServer();
    assertNotEqual(result, CA_STATUS_OK, testLog);
    testLog += "CAStartDiscoveryServer";

    terminateCA();
}

/**
 * @since 2016-03-11
 * @see CAResult_t CAInitialize()
 * @see CAResult_t CASelectNetwork(CATransportAdapter_t interestedNetwork)
 * @see void CATerminate()
 * @objective Test 'CAStartDiscoveryServer' positively to start the discovery service [VLCC]
 * @target CAResult_t CAStartDiscoveryServer()
 * @test_data none
 * @pre_condition none
 * @procedure  1. Initialize CA using CAInitialize
 *             2. Select one of the available network
 *             3. Call the CAStartDiscoveryServer API
 *             4. Repeat Steps 2~3 for multiple times
 * @post_condition Terminate CA using CATerminate function
 * @expected It will start the discovery server and will return CA_STATUS_OK
 */
test(CAStartDiscoveryServer_VLCC_P)
{
    setupTestCase();
    CAResult_t result = CA_STATUS_OK;
    testLog = TESTLOG_START;

    assertEqual(CAInitialize(), CA_STATUS_OK, " Initialization");
    testLog += "CAInitialize";
    isCATerminated = false;

    for (int count = 0; count < COUNT; count++)
    {
        result = CASelectNetwork(CA_ADAPTER_IP);
        assertEqual(result, CA_STATUS_OK, testLog);
        testLog += ", CASelectNetwork";

        result = CAStartDiscoveryServer();
        assertEqual(result, CA_STATUS_OK, testLog);
        testLog += "CAStartDiscoveryServer";

        delay(DELAY_TIME);
    }

    CATerminate();
}

/**
 * @since 2016-03-11
 * @see CAResult_t CAInitialize()
 * @see void CATerminate()
 * @objective Test functionality of 'CACreateEndpoint' and 'CADestroyEndpoint' in a loop [VLCC]
 * @target CAResult_t CASelectNetwork(CATransportAdapter_t interestedNetwork)
 * @target CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @target void CADestroyToken(CAToken_t token)
 * @test_data none
 * @pre_condition Initialize CA using CAInitialize
 * @procedure  1. Select one of the available networks
 *             2. Call CACreateEndpoint API
 *             3. Check it's return value
 *             4. Call CADestroyEndpoint API
 *             5. Repeat Steps 1~4 for multiple times
 * @post_condition Terminate CA using CATerminate
 * @expected It will create and destroy end point and will return CA_STATUS_OK
 */
test(CACreateEndpointDestroyEndpoint_VLCC_P)
{
    setupTestCase();
    CAResult_t result = CA_STATUS_OK;
    testLog = TESTLOG_START;

    assertEqual(CAInitialize(), CA_STATUS_OK, "Initialization");
    testLog += " CAInitialize";
    isCATerminated = false;

    for (int count = 0; count < COUNT; count++)
    {
        result = CASelectNetwork(CA_ADAPTER_IP);
        assertEqual(result, CA_STATUS_OK, testLog);
        testLog += ", CASelectNetwork";

        int port = UINT16_T_PORT;
        result = CA_STATUS_OK;
        char* address = (char *) IP_ADDRESS_LOCAL;
        CAEndpoint_t* endpoint = NULL;

        result = CACreateEndpoint(CA_DEFAULT_FLAGS, caHelper.m_availableNetwork , address , port, &endpoint);
        assertEqual(result, CA_STATUS_OK, testLog);
        testLog += " CACreateEndpoint";

        CADestroyEndpoint(endpoint);

        delay(DELAY_TIME);
    }

    CATerminate();
}

/**
 * @since 2016-03-11
 * @see None
 * @objective Test 'CASelectNetwork' negatively without initializing CA at loop [ITLC]
 * @target CAResult_t CASelectNetwork(const CATransportAdapter_t interestedNetwork)
 * @test_data try to select network without initializing CA
 * @pre_condition Do not call CAInitialize
 * @procedure  1. Call CASelectNetwork
 *             2. Check it's return value
 *             3. Repeat steps 1-2 for multiple times
 * @post_condition none
 * @expected It will fail to select a network and will not return CA_STATUS_OK
 */
test(CASelectNetwork_ITLC_N)
{

    for (int count = 0; count < COUNT; count++)
    {
        result = CASelectNetwork(caHelper.m_availableNetwork);
        assertNotEqual(result, CA_STATUS_OK, testLog);
        testLog += ", CASelectNetwork";
    }
}

/**
 * @since 2016-03-11
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
 * @expected It will not return CA_STATUS_OK
 */
test(CAUnSelectNetworkLoop_ITLC_N)
{
    setupTestCase();
    CAResult_t result = CA_STATUS_OK;
    testLog = TESTLOG_START;

    assertEqual(CAInitialize(), CA_STATUS_OK, "Initialization");
    testLog += "CAInitialize";
    isCATerminated = false;

    for (int count = 0; count < COUNT; count++)
    {
        caHelper.m_availableNetwork = 0;
        result = CASelectNetwork(caHelper.m_availableNetwork);
        assertNotEqual(result, CA_STATUS_OK, testLog);
        testLog += ", CASelectNetwork";
    }

    terminateCA();
}

/**
 * @since 2016-03-11
 * @see CAResult_t CAInitialize()
 * @see void CATerminate()
 * @objective Test 'CASelectNetwork' and 'CAUnSelectNetwork' with state transition condition check [STCC]
 * @target CAResult_t CASelectNetwork(const CATransportAdapter_t interestedNetwork)
 * @target CAResult_t CAUnSelectNetwork(CATransportAdapter_t nonInterestedNetwork)
 * @test_data try to unselect-select-unselect network
 * @pre_condition call CAinitialize
 * @procedure  1. Call CAUnSelectNetwork
 *             2. Check it's return value as not CA_STATUS_OK
 *             3. Call CASelectNetwork
 *             4. Check it's return value
 *             5. Call CAUnSelectNetwork
 *             6. Check it's return value
 * @post_condition Terminate CA using CATerminate function
 * @expected 1. It will fail to unselect network initially
 *           2. It will select network
 *           3. It will unselect network
 */
test(CASelectNetworkUnSelectNetwor_STCC_P)
{
    setupTestCase();
    CAResult_t result = CA_STATUS_OK;
    testLog = TESTLOG_START;

    assertEqual(CAInitialize(), CA_STATUS_OK, "Initialization");
    testLog += "CAInitialize";
    isCATerminated = false;

    result = CAUnSelectNetwork(caHelper.m_availableNetwork);
    assertNotEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CAUnSelectNetwork";

    result = CASelectNetwork(caHelper.m_availableNetwork);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CASelectNetwork";

    result = CAUnSelectNetwork(caHelper.m_availableNetwork);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CAUnSelectNetwork";

    terminateCA();
}

/**
 * @since 2016-03-11
 * @see CAResult_t CAInitialize()
 * @see void CATerminate()
 * @objective Test 'CAUnSelectNetwork' with network type which is not selected in 'CASelectNetwork' [EG]
 * @target CAResult_t CASelectNetwork(const CATransportAdapter_t interestedNetwork)
 * @target CAResult_t CAUnSelectNetwork(CATransportAdapter_t nonInterestedNetwork)
 * @test_data try to unselect network
 * @pre_condition call CAinitialize
 * @procedure  1. Call CASelectNetwork
 *             2. Check it's return value
 *             3. Call CAUnSelectNetwork with another network type that is not selected in step 1
 *             4. Check it's return value as CA_STATUS_FAILED
 *             5. Call CAUnselectNetwork with network type as selected in step 1
 * @post_condition Terminate CA using CATerminate function
 * @expected It will return not CA_STATUS_OK
 */
test(CASelectNetworkUnSelectNetwork_EG_N)
{
    setupTestCase();
    CAResult_t result = CA_STATUS_OK;
    testLog = TESTLOG_START;

    assertEqual(CAInitialize(), CA_STATUS_OK, "Initialization");
    testLog += "CAInitialize";
    isCATerminated = false;

    result = CASelectNetwork(CA_ADAPTER_IP);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CASelectNetwork";

    result = CAUnSelectNetwork(CA_DEFAULT_ADAPTER);
    assertNotEqual(result, CA_STATUS_OK, testLog);
    testLog += "CAUnSelectNetwork";

    result = CAUnSelectNetwork(CA_ADAPTER_IP);
    assertNotEqual(result, CA_STATUS_OK, testLog);
    testLog += "CAUnSelectNetwork";

    terminateCA();
}

/**
 * @since 2016-03-11
 * @see CAResult_t CAInitialize()
 * @see void CATerminate()
 * @objective Test CASelectNetwork and CAUnselectNetwork with checking the network status [SCV]
 * @target CAResult_t CASelectNetwork(const CATransportAdapter_t interestedNetwork)
 * @target CAResult_t CAUnSelectNetwork(CATransportAdapter_t nonInterestedNetwork)
 * @test_data none
 * @pre_condition Initialize CA using CAInitialize
 * @procedure  1. Call CASelectNetwork
 *             2. Call CAStartListeningServer API
 *             3. Call CAUnselectNetwork
 *             4. Call CAStartListeningServer API
 * @post_condition Terminate CA using CATerminate
 * @expected Call It will not return CA_STATUS_OK
 */
test(CASelectNetworkUnSelectNetwork_SCV_N)
{
    setupTestCase();
    CAResult_t result = CA_STATUS_OK;
    testLog = TESTLOG_START;

    assertEqual(CAInitialize(), CA_STATUS_OK, "Initialization");
    testLog += "CAInitialize";
    isCATerminated = false;

    result = CASelectNetwork(caHelper.m_availableNetwork);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CASelectNetwork";

    result = CAStartListeningServer();
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CAStartListeningServer";

    result = CAUnSelectNetwork(caHelper.m_availableNetwork);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CAUnSelectNetwork";

    result = CAStartListeningServer();
    assertNotEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CAStartListeningServer";

    terminateCA();
}

/**
 * @since 2016-03-11
 * @see CAResult_t CAInitialize()
 * @see void CATerminate()
 * @objective Test CASelectNetwork and CAUnselectNetwork in a loop [SLCC]
 * @target CAResult_t CASelectNetwork(const CATransportAdapter_t interestedNetwork)
 * @target CAResult_t CAUnSelectNetwork(CATransportAdapter_t nonInterestedNetwork)
 * @test_data none
 * @pre_condition Initialize CA using CAInitialize
 * @procedure  1. Call CASelectNetwork
 *             2. Check it's return value
 *             3. Call CAUnselectNetwork
 *             4. Check it's return value
 *             5. Repeat Steps 1~4 for multiple times
 * @post_condition Terminate CA using CATerminate
 * @expected It will select and unselect network in each time and will return CA_STATUS_OK
 */
test(CASelectNetworkUnSelectNetworkLoop_SLCC_P)
{
    setupTestCase();
    CAResult_t result = CA_STATUS_OK;
    testLog = TESTLOG_START;

    assertEqual(CAInitialize(), CA_STATUS_OK, "Initialization");
    testLog += "CAInitialize";
    isCATerminated = false;

    for (int count = 0; count < COUNT; count++)
    {
        result = CASelectNetwork(caHelper.m_availableNetwork);
        assertEqual(result, CA_STATUS_OK, testLog);
        testLog += ", CASelectNetwork";
               
        result = CAUnSelectNetwork(caHelper.m_availableNetwork);   
        assertEqual(result, CA_STATUS_OK, testLog);
        testLog += ", CAUnSelectNetwork";      
    }
    
    terminateCA();
}

/**
 * @since 2016-03-11
 * @see none
 * @objective Test CAGenerateToken() and CADestroyToken() with checking token length [GSRV]
 * @target  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @target  void CADestroyToken(CAToken_t token)
 * @test_data none
 * @pre_condition none
 * @procedure 
 *            1. Call CAGenerateToken API
 *            2. Check if token is generated and API returns CA_STATUS_OK
 *            3. Check generated token length with the set value (TOKEN_LENGTH)
 *            4. Call CADestroyToken API
 *            5. Repeat steps 1~4 for multiple times
 * @post_condition none
 * @expected Token needs to be generated as the length is set and needs to destroy after calling CADestroyToken API
 */
test(CAGenerateTokenAndCADestroyTokenLoop_VLCC_P)
{
    setupTestCase();
    CAResult_t result = CA_STATUS_OK;
    testLog = TESTLOG_START;

    for (int count = 0; count < COUNT; count++)
    {
        CAToken_t token = NULL;
        uint8_t tokenLength = CA_MAX_TOKEN_LEN;
        result = CAGenerateToken(&token, tokenLength);
        assertEqual(result, CA_STATUS_OK, testLog);
        testLog += ", CAGenerateToken";

        CADestroyToken(token);
    }
}
