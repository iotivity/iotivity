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

#define MAX_BUF_LEN 100
#define MAX_OPT_LEN 16
#define UINT16_T_PORT 6298
#define UINT16_T_PORT_ZERO 0
#define LBV 0
#define LOBV -1
#define UOBV 4294967296
#define IP_ADDRESS_LOCAL "107.109.214.78";
#define IP_ADDRESS "192.168.0.10";
#define TESTLOG_START "Previous API Calls: ";

#define WRITE_LOG Serial.print
#define WRITE_LOG_LINE Serial.println
#define NETWORK_OUT_OF_BOUNDARY_VALUE 0
#define SPEED 115200

bool isCATerminated = false;

String testLog;

CAHelper caHelper;

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

    WRITE_LOG_LINE("==========================");
    WRITE_LOG_LINE("TestSuite: CATest");
    WRITE_LOG_LINE("==========================");

    CAHelper::addFilter();

    caHelper.Connect(false);

    Serial.println("------------------------------------BEGIN------------------------------------");
}

void loop()
{
    Test::run();
}

/**
 * @since 2015-02-13
 * @see void CATerminate()
 * @objective Test 'CAInitialize' positively to initialize CA module
 * @target CAResult_t CAInitialize()
 * @test_data none
 * @pre_condition none
 * @procedure Call CAInitialize()
 * @post_condition Terminate CA using CATerminate
 * @expected Initialization will succeed and return CA_STATUS_OK
 */
test(Initialize_P)
{
    setupTestCase();

    assertEqual(CAInitialize(), CA_STATUS_OK, "Initialization");
    isCATerminated = false;

    terminateCA();
}

/**
 * @since 2015-02-17
 * @see CAResult_t CAInitialize()
 * @see CAResult_t CASelectNetwork(const CATransportAdapter_t interestedNetwork)
 * @see void CATerminate()
 * @objective Test 'CAStartListeningServer' positively to start the CA server module
 * @target CAResult_t CAStartListeningServer()
 * @test_data none
 * @pre_condition  1. Initialize CA using CAInitialize
 *                 2. Select a valid network using CASelectNetwork
 * @procedure Call CAStartListeningServer
 * @post_condition Terminate CA using CATerminate
 * @expected It will start the CA server module and return CA_STATUS_OK
 */
test(StartListeningServer_P)
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

    terminateCA();
}

/**
 * @since 2015-02-17
 * @see CAResult_t CAInitialize()
 * @see CAResult_t CASelectNetwork(const CATransportAdapter_t interestedNetwork)
 * @see void CATerminate()
 * @objective Test 'CAStartDiscoveryServer' positively to start CA client module
 * @target CAResult_t CAStartDiscoveryServer()
 * @test_data none
 * @pre_condition  1. Initialize CA using CAInitialize
 *                 2. Select a valid network using CASelectNetwork function
 * @procedure Call CAStartDiscoveryServer function
 * @post_condition Terminate CA using CATerminate
 * @expected It will start the CA client module and return CA_STATUS_OK
 */
test(StartDiscoveryServer_P)
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

    result = CAStartDiscoveryServer();
    assertEqual(result, CA_STATUS_OK, testLog);

    terminateCA();
}

/**
 * @since 2016-03-10
 * @see none
 * @objective Test 'CACreateEndpoint' negatively to check against NULL value in flags [flags-NV]
 * @target CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object);
 * @test_data  NULL flags
 * @pre_condition none
 * @procedure  1. call the CACreateEndpoint API and pass the NULL as flags arguments
 *             2. Check it's return value
 * @post_condition none
 * @expected It will fail to create end point and not return CA_STATUS_OK
 */
test(CACreateEndpoint_Flag_NV_N)
{
    int port = UINT16_T_PORT;
    CAResult_t result = CA_STATUS_OK;
    char* address = (char *) IP_ADDRESS_LOCAL;
    CAEndpoint_t* endpoint = NULL;

    result = CACreateEndpoint(NULL, caHelper.m_availableNetwork, address, port, &endpoint);
    assertEqual(result, CA_STATUS_OK, testLog);
}

/**
 * @since 2016-03-10
 * @see none
 * @objective Test 'CACreateEndpoint' negatively to check against NULL value in Adapter [adapter-NV]
 * @target CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object);
 * @test_data  NULL adapter
 * @pre_condition none
 * @procedure  1. call the CACreateEndpoint API and pass the NULL as adapter arguments
 *             2. Check it's return value
 * @post_condition none
 * @expected It will fail to create end point and not return CA_STATUS_OK
 */
test(CACreateEndpoint_Adapter_NV_N)
{
    int port = UINT16_T_PORT;
    CAResult_t result = CA_STATUS_OK;
    char* address = (char *) IP_ADDRESS_LOCAL;
    CAEndpoint_t* endpoint = NULL;

    result = CACreateEndpoint(CA_DEFAULT_FLAGS, NULL, address, port, &endpoint);
    assertEqual(result, CA_STATUS_OK, testLog);
}

/**
 * @since 2016-03-10
 * @see none
 * @objective Test 'CACreateEndpoint' negatively to check against NULL value in port [port-NV]
 * @target CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object);
 * @test_data  NULL port
 * @pre_condition none
 * @procedure  1. call the CACreateEndpoint API and pass the NULL as port arguments
 *             2. Check it's return value
 * @post_condition none
 * @expected It will fail to create end point and not return CA_STATUS_OK
 */
test(CACreateEndpoint_Port_NV_N)
{
    CAResult_t result = CA_STATUS_OK;
    CAEndpoint_t* endpoint = NULL;
    char* address = (char *) IP_ADDRESS_LOCAL;

    result = CACreateEndpoint(CA_DEFAULT_FLAGS, caHelper.m_availableNetwork , address, NULL, &endpoint);
    assertEqual(result, CA_STATUS_OK, testLog);
}

/**
 * @since 2016-03-10
 * @see none
 * @objective Test 'CACreateEndpoint' negatively to check against NULL value in endpoint object [endpoint-NV]
 * @target CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object);
 * @test_data  NULL endpoint reference
 * @pre_condition none
 * @procedure  1. call the CACreateEndpoint API and pass the NULL as endpoint arguments
 *             2. Check it's return value
 * @post_condition none
 * @expected It will fail to create end point and not return CA_STATUS_OK
 */
test(CACreateEndpoint_Endpoint_NV_N)
{
    CAResult_t result = CA_STATUS_OK;
    int port = UINT16_T_PORT;
    char* address = (char *) IP_ADDRESS_LOCAL;

    result = CACreateEndpoint(CA_DEFAULT_FLAGS, caHelper.m_availableNetwork , address, port, NULL);
    assertNotEqual(result, CA_STATUS_OK, testLog);
}

/**
 * @since 2016-03-10
 * @see none
 * @objective Test 'CACreateEndpoint' positively to check endpointresentative value against flags argument [RV]
 * @target CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object);
 * @test_data  endpointresentative value of flags
 * @pre_condition none
 * @procedure  1. call the CACreateEndpoint API and pass the endpointresentative value as flags arguments
 *             2. Check it's return value
 * @post_condition none
 * @expected It will create end point and return CA_STATUS_OK
 */
test(CACreateEndpoint_RV_P)
{
    CAResult_t result = CA_STATUS_OK;
    int port = UINT16_T_PORT;
    char* address = (char *) IP_ADDRESS_LOCAL;
    CAEndpoint_t* endpoint = NULL;

    result = CACreateEndpoint(0, caHelper.m_availableNetwork , address, port, &endpoint);
    assertEqual(result, CA_STATUS_OK, testLog);
}

/**
 * @since 2016-03-10
 * @see none
 * @objective Test 'CACreateEndpoint' positively to check reference sample value in ip address [RSV]
 * @target CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object)
 * @test_data  Reference sample value in IP address
 * @pre_condition none
 * @procedure  1. call the CACreateEndpoint API and pass the reference sample value in IP address
 *             2. Check it's return value
 * @post_condition none
 * @expected It will create end point and return CA_STATUS_OK
 */
test(CACreateEndpoint_RSV_P)
{
    CAResult_t result = CA_STATUS_OK;
    int port = UINT16_T_PORT;
    char* address = (char *) IP_ADDRESS_LOCAL;
    CAEndpoint_t* endpoint = NULL;

    result = CACreateEndpoint(CA_DEFAULT_FLAGS, caHelper.m_availableNetwork , address , port, &endpoint);
    assertEqual(result, CA_STATUS_OK, testLog);
}

/**
 * @since 2016-03-10
 * @see none
 * @objective Test 'CACreateEndpoint' positively to check addreess value with empty string [ESV]
 * @target CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object);
 * @test_data  Adress value with empty string
 * @pre_condition none
 * @procedure  1. call the CACreateEndpoint API and pass the empty string as address arguments
 *             2. Check it's return value
 * @post_condition none
 * @expected It will create end point and return CA_STATUS_OK
 */
test(CACreateEndpoint_ESV_P)
{
    CAResult_t result = CA_STATUS_OK;
    int port = UINT16_T_PORT;
    CAEndpoint_t* endpoint = NULL;

    result = CACreateEndpoint(CA_DEFAULT_FLAGS, caHelper.m_availableNetwork , "" , port, &endpoint);
    assertEqual(result, CA_STATUS_OK, testLog);
}

/**
 * @since 2016-03-10
 * @see none
 * @objective Test 'CACreateEndpoint' negatively to check unformated reference sample value in endpoint [URSV]
 * @target CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr,
 *         uint16_t port, CAEndpoint_t **object);
 * @test_data  Unformated Reference sample value in endpoint
 * @pre_condition none
 * @procedure  1. call the CACreateEndpoint API and pass the unformated reference sample value in endpoint
 *             2. Check it's return value
 * @post_condition none
 * @expected It will not create end point and not return CA_STATUS_OK
 */
test(CACreateEndpoint_URSV_N)
{
    CAResult_t result = CA_STATUS_OK;
    char* address = (char *) IP_ADDRESS_LOCAL;
    int port = UINT16_T_PORT;
    CAEndpoint_t** endpoint = NULL;

    result = CACreateEndpoint(CA_DEFAULT_FLAGS, caHelper.m_availableNetwork , address , port, endpoint);
    assertNotEqual(result, CA_STATUS_OK, testLog);
}

/**
 * @since 2015-02-17
 * @see void CADestroyEndpoint(CAEndpoint_t *object)
 * @objective Test 'CACreateEndpoint' positively to generate CAEndpoint_t data from a given URI
 * @target CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr, uint16_t port, CAEndpoint_t **object)
 * @test_data  1. Absolute resource URI which will be used to generate the remote endpoint
 *             2. Valid network interface identifier
 *             3. A pointer to an object of type CAEndpoint_t
 * @pre_condition  1. Initialize CA using CAInitialize
 *                 2. Select a valid network using CASelectNetwork function
 * @procedure Call the CACreateEndpoint API and pass the URI, network identifier and CAEndpoint_t pointer as arguments
 * @post_condition Destroy the endpoint object by calling CADestroyEndpoint.
 * @expected  1. It will parse the URI to construct the endpoint object and return CA_STATUS_OK.
 *            2. The endpoint object will not be NULL.
 */
test(CreateEndpoint_P)
{
    setupTestCase();
    CAResult_t result = CA_STATUS_OK;
    
    char* address = (char *) IP_ADDRESS;
    CAEndpoint_t* endpoint = NULL;
    int port = UINT16_T_PORT_ZERO;
    
    result = CACreateEndpoint(CA_DEFAULT_FLAGS, caHelper.m_availableNetwork, address, port, &endpoint);
    assertEqual(result, CA_STATUS_OK, testLog);

    if(endpoint == NULL)
    {
        fail();
        return;
    }

    CADestroyEndpoint(endpoint);
}

/**
 * @since 2015-02-17
 * @see  None
 * @objective Test 'CACreateEndpoint' negatively with a blank address as argument
 * @target CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr, uint16_t port, CAEndpoint_t **object)
 * @test_data  1. A blank URI string
 *             2. Valid network interface identifier
 *             3. A pointer to an object of type CAEndpoint_t
 * @pre_condition  1. Initialize CA using CAInitialize
 *                 2. Select a valid network using CASelectNetwork function
 * @procedure Call the CACreateEndpoint API and pass the blank URI, valid network identifier and CAEndpoint_t pointer as arguments
 * @post_condition none
 * @expected  It will fail to create an endpoint object and return CA_STATUS_FAILED
 */
test(CreateRemoteEndpointWithEmptyAddress_N)
{
    setupTestCase();
    CAResult_t result = CA_STATUS_OK;

    char* address = (char *) "";
    CAEndpoint_t* endpoint = NULL;
    int port = UINT16_T_PORT_ZERO;

    result = CACreateEndpoint(CA_DEFAULT_FLAGS, caHelper.m_availableNetwork, address, port, &endpoint);
    assertEqual(result, CA_STATUS_OK, testLog);
}

/**
 * @since 2015-02-17
 * @see void CADestroyToken(CAToken_t token)
 * @objective Test 'CAGenerateToken' positively to generate a token
 * @target CAResult_t CAGenerateToken(CAToken_t* token)
 * @test_data A pointer of the type CAToken_t
 * @pre_condition select a valid network using CASelectNetwork function                 
 * @procedure Call the CAGenerateToken API and pass a token pointer as argument
 * @post_condition Destroy token using CADestroyToken                
 * @expected  1. It will generate a token and return CA_STATUS_OK
 *            2. Token variable won't be NULL
 */
test(GenerateToken_P)
{
    setupTestCase();
    CAResult_t result = CA_STATUS_OK;
    testLog = TESTLOG_START;

    CAToken_t token = NULL;
    uint8_t tokenLength = CA_MAX_TOKEN_LEN;
    result = CAGenerateToken(&token, tokenLength);
    assertEqual(result, CA_STATUS_OK, testLog);

    if(token == NULL)
    {
        fail();
    }

    CADestroyToken (token);
}

/**
 * @since 2015-02-17
 * @see none
 * @objective Test 'CAGenerateToken' negatively by passing a null pointer as argument
 * @target CAResult_t CAGenerateToken(CAToken_t* token)
 * @test_data Null pointer
 * @pre_condition none
 * @procedure Call the CAGenerateToken API and use NULL as argument
 * @post_condition Destroy token using CADestroyToken
 * @expected It will fail to generate a token and return CA_STATUS_FAILED
 */
test(GenerateTokenWithNullPointer_N)
{
    setupTestCase();
    CAResult_t result = CA_STATUS_OK;
    testLog = TESTLOG_START;
	
	CAToken_t token = NULL;
    uint8_t tokenLength = CA_MAX_TOKEN_LEN;
    result = CAGenerateToken(NULL, tokenLength);
    assertNotEqual(result, CA_STATUS_OK, testLog);
    
    CADestroyToken (token);
}

/**
 * @since 2015-02-18
 * @see CAResult_t CAInitialize()
 * @see CAResult_t CASelectNetwork(const CATransportAdapter_t interestedNetwork)
 * @see CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr, uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CAGenerateToken(CAToken_t* token)
 * @see void CADestroyToken(CAToken_t token)
 * @see void CADestroyEndpoint(CAEndpoint_t *object)
 * @see void CATerminate()
 * @objective Test CASendRequest API positively
 * @target CAResult_t CASendRequest(const CAEndpoint_t* object, CARequestInfo_t* requestInfo)
 * @test_data  1. A valid IP, PORT and Resource_URI (A string with the format, "IP:Port/IP, PORT and Resource_URI")
 *             2. A token
 *             3. A valid payload
 * @pre_condition  1. Initialize CA using CAInitialize
 *                 2. Select a valid network using CASelectNetwork
 *                 3. Generate token using CAGenerateToken
 *                 4. Generate remote endpoint from a valid URI using CACreateEndpoint
 *                 5. Form an object of type CARequestInfo_t with all the necessary parameters, a valid message type, a valid method, the token & the payload
 * @procedure  Call CASendRequest with the object of type CAEndpoint_t & pointer to CARequestInfo_t
 * @post_condition  1. Destroy token
 *                  2. Destroy remote endpoint
 *                  3. Terminate CA using CATerminate
 * @expected CASendRequest will return CA_STATUS_OK
 */
test(SendRequest_P)
{
    setupTestCase();
    CAResult_t result = CA_STATUS_OK;
    testLog = TESTLOG_START;

    result = CAInitialize();
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += "CAInitialize";
    isCATerminated = false;

    result = CASelectNetwork(caHelper.m_availableNetwork);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CASelectNetwork";

	int port = UINT16_T_PORT_ZERO;
    char* address = (char *) IP_ADDRESS;
    CARequestInfo_t requestInfo;
    memset(&requestInfo, 0, sizeof(CARequestInfo_t));  

    CAEndpoint_t* endpoint = NULL;
    result = CACreateEndpoint(CA_DEFAULT_FLAGS, caHelper.m_availableNetwork, address, port, &endpoint);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CACreateEndpoint";

    CAToken_t token = NULL;
    uint8_t tokenLength = CA_MAX_TOKEN_LEN;
    result = CAGenerateToken(&token, tokenLength);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CAGenerateToken";

    requestInfo.method = CA_GET;
    requestInfo.info.type = CA_MSG_NONCONFIRM;
    requestInfo.info.token = token;
    requestInfo.info.tokenLength = tokenLength;
    requestInfo.info.payload = (char *) "request payload";
    requestInfo.info.resourceUri = (char *) "/a/light";

    result = CASendRequest(endpoint, &requestInfo);
    assertEqual(result, CA_STATUS_OK, testLog);

    CADestroyToken(token);
    CADestroyEndpoint(endpoint);
    terminateCA();
}

/**
 * @since 2015-02-18
 * @see CAResult_t CAInitialize()
 * @see CAResult_t CASelectNetwork(const CATransportAdapter_t interestedNetwork)
 * @see CAResult_t CAGenerateToken(CAToken_t* token)
 * @see void CADestroyToken(CAToken_t token)
 * @see void CATerminate()
 * @objective Test CASendRequest API negatively with null remote endpoint
 * @target CAResult_t CASendRequest(const CAEndpoint_t* object, CARequestInfo_t* requestInfo)
 * @test_data  1. A token
 *             2. A valid payload
 *             3. Null as CAEndpoint_t
 * @pre_condition  1. Initialize CA using CAInitialize
 *                 2. Select a valid network using CASelectNetwork
 *                 3. Generate token using CAGenerateToken
 *                 4. Form an object of type CARequestInfo_t with all the necessary parameters, a valid message type, a valid method, the token & the payload
 * @procedure  Call CASendRequest with NULL as remote endpoint & a valid pointer to CARequestInfo_t object
 * @post_condition  1. Destroy token
 *                  2. Terminate CA using CATerminate
 * @expected CASendRequest will not return CA_STATUS_OK
 */
test(SendRequestUsingNullEndpoint_N)
{
    setupTestCase();
    CAResult_t result = CA_STATUS_OK;
    testLog = TESTLOG_START;

    result = CAInitialize();
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += "CAInitialize";
    isCATerminated = false;

    result = CASelectNetwork(caHelper.m_availableNetwork);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CASelectNetwork";

    CARequestInfo_t requestInfo;
    memset(&requestInfo, 0, sizeof(CARequestInfo_t));

    CAToken_t token = NULL;
    uint8_t tokenLength = CA_MAX_TOKEN_LEN;
    result = CAGenerateToken(&token, tokenLength);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CAGenerateToken";

    requestInfo.method = CA_GET;
    requestInfo.info.type = CA_MSG_NONCONFIRM;
    requestInfo.info.token = token;
    requestInfo.info.tokenLength = tokenLength;
    requestInfo.info.payload = (char *) "request payload";

    result = CASendRequest(NULL, &requestInfo);
    assertNotEqual(result, CA_STATUS_OK, testLog);

    CADestroyToken(token);
    terminateCA();
}

/**
 * @since 2015-02-18
 * @see CAResult_t CAInitialize()
 * @see CAResult_t CASelectNetwork(const CATransportAdapter_t interestedNetwork)
 * @see CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr, uint16_t port, CAEndpoint_t **object)
 * @see void CADestroyEndpoint(CAEndpoint_t *object)
 * @see void CATerminate()
 * @objective Test CASendRequest API negatively with null CARequestInfo_t
 * @target CAResult_t CASendRequest(const CAEndpoint_t* object, CARequestInfo_t* requestInfo)
 * @test_data  1. A valid URI (A string with the format, "IP:Port/URI")
 *             2. Null as CARequestInfo_t
 * @pre_condition  1. Initialize CA using CAInitialize
 *                 2. Select a valid network using CASelectNetwork
 *                 3. Generate remote endpoint from a valid URI using CACreateEndpoint
 * @procedure  Call CASendRequest with the object of type CAEndpoint_t & NULL in place of CARequestInfo_t
 * @post_condition  1. Destroy remote endpoint
 *                  2. Terminate CA using CATerminate
 * @expected CASendRequest will not return CA_STATUS_OK
 */
test(SendRequestUsingNullRequestInfo_N)
{
    setupTestCase();
    CAResult_t result = CA_STATUS_OK;
    testLog = TESTLOG_START;

    result = CAInitialize();
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += "CAInitialize";
    isCATerminated = false;

    result = CASelectNetwork(caHelper.m_availableNetwork);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CASelectNetwork";

    char* address = (char *) IP_ADDRESS;
    CAEndpoint_t* endpoint = NULL;
    int port = UINT16_T_PORT_ZERO;
    result = CACreateEndpoint(CA_DEFAULT_FLAGS, caHelper.m_availableNetwork, address, port, &endpoint);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CACreateEndpoint";

    result = CASendRequest(endpoint, NULL);
    assertNotEqual(result, CA_STATUS_OK, testLog);

    CADestroyEndpoint(endpoint);
    terminateCA();
}

/**
 * @since 2015-02-18
 * @see CAResult_t CAInitialize()
 * @see CAResult_t CASelectNetwork(const CATransportAdapter_t interestedNetwork)
 * @see CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr, uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CAGenerateToken(CAToken_t* token)
 * @see void CADestroyToken(CAToken_t token)
 * @see void CADestroyEndpoint(CAEndpoint_t *object)
 * @see void CATerminate()
 * @objective Test CASendRequest API with empty CARequestInfo_t
 * @target CAResult_t CASendRequest(const CAEndpoint_t* object, CARequestInfo_t* requestInfo)
 * @test_data  1. A valid URI (A string with the format, "IP:Port/URI")
 *             2. Empty CARequestInfo_t object
 * @pre_condition  1. Initialize CA using CAInitialize
 *                 2. Select a valid network using CASelectNetwork
 *                 3. Generate remote endpoint from a valid URI using CACreateEndpoint
 * @procedure  Call CASendRequest with the object of type CAEndpoint_t & CARequestInfo_t (which is empty)
 * @post_condition  1. Destroy remote endpoint
 *                  2. Terminate CA using CATerminate
 * @expected CASendRequest will return CA_STATUS_OK
 */
test(SendRequestUsingEmptyRequestInfo_P)
{
    setupTestCase();

    CAResult_t result = CA_STATUS_OK;
    testLog = TESTLOG_START;

    result = CAInitialize();
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += "CAInitialize";
    isCATerminated = false;

    result = CASelectNetwork(caHelper.m_availableNetwork);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CASelectNetwork";

    char* address = (char *) IP_ADDRESS;
    CAEndpoint_t* endpoint = NULL;
    int port = UINT16_T_PORT_ZERO;
    result = CACreateEndpoint(CA_DEFAULT_FLAGS, caHelper.m_availableNetwork, address, port, &endpoint);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CACreateEndpoint";

    CARequestInfo_t requestInfo;
    memset(&requestInfo, 0, sizeof(CARequestInfo_t));

    result = CASendRequest(endpoint, &requestInfo);
    assertEqual(result, CA_MEMORY_ALLOC_FAILED, testLog);

    CADestroyEndpoint(endpoint);
    terminateCA();
}

/**
 * @since 2015-02-19
 * @see CAResult_t CAInitialize()
 * @see CAResult_t CASelectNetwork(const CATransportAdapter_t interestedNetwork)
 * @see CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr, uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CAGenerateToken(CAToken_t* token)
 * @see void CADestroyToken(CAToken_t token)
 * @see void CADestroyEndpoint(CAEndpoint_t *object)
 * @see void CATerminate()
 * @objective Test CASendResponse API positively
 * @target CAResult_t CASendResponse(const CAEndpoint_t* object, CAResponseInfo_t* responseInfo)
 * @test_data  1. A valid URI (A string with the format, "IP:Port/URI")
 *             2. A token
 *             3. A valid payload
 * @pre_condition  1. Initialize CA using CAInitialize
 *                 2. Select a valid network using CASelectNetwork
 *                 3. Generate token using CAGenerateToken
 *                 4. Generate remote endpoint from a valid URI using CACreateEndpoint
 *                 5. Form an object of type CAResponseInfo_t with all the necessary parameters, a valid message type, a valid result, the token & the payload
 * @procedure  Call CASendResponse with the object of type CAEndpoint_t & pointer to CAResponseInfo_t
 * @post_condition  1. Destroy token
 *                  2. Destroy remote endpoint
 *                  3. Terminate CA using CATerminate
 * @expected CASendResponse will return CA_STATUS_OK
 */
test(SendResponse_P)
{
    setupTestCase();
    CAResult_t result = CA_STATUS_OK;
    testLog = TESTLOG_START;

    result = CAInitialize();
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += "CAInitialize";
    isCATerminated = false;

    result = CASelectNetwork(caHelper.m_availableNetwork);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CASelectNetwork";
	
	int port = UINT16_T_PORT_ZERO;
    char* address = (char *) IP_ADDRESS;
    CAResponseInfo_t responseInfo;
    memset(&responseInfo, 0, sizeof(CAResponseInfo_t));

    CAEndpoint_t* endpoint = NULL;
    result = CACreateEndpoint(CA_DEFAULT_FLAGS, caHelper.m_availableNetwork, address, port, &endpoint);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CACreateEndpoint";

    CAToken_t token = NULL;
    uint8_t tokenLength = CA_MAX_TOKEN_LEN;
    result = CAGenerateToken(&token, tokenLength);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CAGenerateToken";

    responseInfo.result = CA_VALID;
    responseInfo.info.type = CA_MSG_NONCONFIRM;
    responseInfo.info.token = token;
    responseInfo.info.payload = (char *) "response payload";

    result = CASendResponse(endpoint, &responseInfo);
    assertEqual(result, CA_STATUS_OK, testLog);

    CADestroyToken(token);
    CADestroyEndpoint(endpoint);
    terminateCA();
}

/**
 * @since 2015-02-19
 * @see  1. CAResult_t CAInitialize()
 *       2. CAResult_t CASelectNetwork(const CATransportAdapter_t interestedNetwork)
 *       3. CAResult_t CAGenerateToken(CAToken_t* token)
 *       4. void CADestroyToken(CAToken_t token)
 *       5. void CATerminate()
 * @objective Test CASendResponse API negatively with null remote endpoint
 * @target CAResult_t CASendResponse(const CAEndpoint_t* object, CAResponseInfo_t* responseInfo)
 * @test_data  1. A token
 *             2. A valid payload
 *             3. Null as CAEndpoint_t
 * @pre_condition  1. Initialize CA using CAInitialize
 *                 2. Select a valid network using CASelectNetwork
 *                 3. Generate token using CAGenerateToken
 *                 4. Form an object of type CAResponseInfo_t with all the necessary parameters, a valid message type, a valid result, the token & the payload
 * @procedure  Call CASendResponse with NULL as remote endpoint & a valid pointer to CAResponseInfo_t object
 * @post_condition  1. Destroy token
 *                  2. Terminate CA using CATerminate
 * @expected CASendResponse will not return CA_STATUS_OK
 */
test(SendResponseUsingNullEndpoint_N)
{
    setupTestCase();
    CAResult_t result = CA_STATUS_OK;
    testLog = TESTLOG_START;

    result = CAInitialize();
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += "CAInitialize";
    isCATerminated = false;

    result = CASelectNetwork(caHelper.m_availableNetwork);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CASelectNetwork";

    CAResponseInfo_t responseInfo;
    memset(&responseInfo, 0, sizeof(CAResponseInfo_t));

    CAToken_t token = NULL;
    uint8_t tokenLength = CA_MAX_TOKEN_LEN;
    result = CAGenerateToken(&token, tokenLength);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CAGenerateToken";

    responseInfo.result = CA_VALID;
    responseInfo.info.type = CA_MSG_NONCONFIRM;
    responseInfo.info.token = token;
    responseInfo.info.payload = (char *) "response payload";

    result = CASendResponse(NULL, &responseInfo);
    assertNotEqual(result, CA_STATUS_OK, testLog);
    
    CADestroyToken(token);    
    terminateCA();
}

/**
 * @since 2015-02-19
 * @see CAResult_t CAInitialize()
 * @see CAResult_t CASelectNetwork(const CATransportAdapter_t interestedNetwork)
 * @see CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr, uint16_t port, CAEndpoint_t **object)
 * @see void CADestroyEndpoint(CAEndpoint_t *object)
 * @see svoid CATerminate()
 * @objective Test CASendResponse API negatively with null CAResponseInfo_t
 * @target CAResult_t CASendResponse(const CAEndpoint_t* object, CAResponseInfo_t* responseInfo)
 * @test_data  1. A valid URI (A string with the format, "IP:Port/URI")
 *             2. Null as CAResponseInfo_t
 * @pre_condition  1. Initialize CA using CAInitialize
 *                 2. Select a valid network using CASelectNetwork
 *                 3. Generate remote endpoint from a valid URI using CACreateEndpoint
 * @procedure  Call CASendResponse with the object of type CAEndpoint_t & NULL in place of CAResponseInfo_t
 * @post_condition  1. Destroy remote endpoint
 *                  2. Terminate CA using CATerminate
 * @expected CASendResponse will not return CA_STATUS_OK
 */
test(SendResponseUsingNullResponseInfo_N)
{
    setupTestCase();
    CAResult_t result = CA_STATUS_OK;
    testLog = TESTLOG_START;

    result = CAInitialize();
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += "CAInitialize";
    isCATerminated = false;

    result = CASelectNetwork(caHelper.m_availableNetwork);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CASelectNetwork";

    char* address = (char *) IP_ADDRESS;
    CAEndpoint_t* endpoint = NULL;
    int port = UINT16_T_PORT_ZERO;
    result = CACreateEndpoint(CA_DEFAULT_FLAGS, caHelper.m_availableNetwork, address, port, &endpoint);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CACreateEndpoint";

    result = CASendResponse(endpoint, NULL);
    assertNotEqual(result, CA_STATUS_OK, testLog);

    CADestroyEndpoint(endpoint);
    terminateCA();
}

/**
 * @since 2015-02-18
 * @see CAResult_t CAInitialize()
 * @see CAResult_t CASelectNetwork(const CATransportAdapter_t interestedNetwork)
 * @see CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr, uint16_t port, CAEndpoint_t **object)
 * @see void CADestroyEndpoint(CAEndpoint_t *object)
 * @see void CATerminate()
 * @objective Test CASendResponse API with empty CAResponseInfo_t
 * @target CAResult_t CASendResponse(const CAEndpoint_t* object, CAResponseInfo_t* responseInfo)
 * @test_data  1. A valid URI (A string with the format, "IP:Port/URI")
 *             2. Empty CAResponseInfo_t object
 * @pre_condition  1. Initialize CA using CAInitialize
 *                 2. Select a valid network using CASelectNetwork
 *                 3. Generate remote endpoint from a valid URI using CACreateEndpoint
 * @procedure  Call CASendResponse with the object of type CAEndpoint_t & CAResponseInfo_t (which is empty)
 * @post_condition  1.Destroy remote endpoint
 *                  2.Terminate CA using CATerminate
 * @expected CASendResponse will return CA_STATUS_OK
 */
test(SendResponseUsingEmptyResponseInfo_P)
{
    setupTestCase();
    CAResult_t result = CA_STATUS_OK;
    testLog = TESTLOG_START;

    result = CAInitialize();
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += "CAInitialize";
    isCATerminated = false;

    result = CASelectNetwork(caHelper.m_availableNetwork);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CASelectNetwork";

    char* address = (char *) IP_ADDRESS;
    CAEndpoint_t* endpoint = NULL;
    int port = UINT16_T_PORT_ZERO;
    result = CACreateEndpoint(CA_DEFAULT_FLAGS, caHelper.m_availableNetwork, address, port, &endpoint);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CACreateEndpoint";

    CAResponseInfo_t responseInfo;
    memset(&responseInfo, 0, sizeof(CAResponseInfo_t));

    result = CASendResponse(endpoint, &responseInfo);
    assertEqual(result, CA_STATUS_OK, testLog);

    CADestroyEndpoint(endpoint);
    terminateCA();
}

/**
 * @since 2015-03-27
 * @see AResult_t CAInitialize()
 * @see CAResult_t CASelectNetwork(const CATransportAdapter_t interestedNetwork)
 * @see CAResult_t CACreateEndpoint(CATransportFlags_t flags, CATransportAdapter_t adapter, const char *addr, uint16_t port, CAEndpoint_t **object)
 * @see CAResult_t CAGenerateToken(CAToken_t* token)
 * @see void CADestroyToken(CAToken_t token)
 * @see void CADestroyEndpoint(CAEndpoint_t *object)
 * @see void CATerminate()
 * @objective Test CASendRequestToAll API positively
 * @target CAResult_t CASendRequestToAll(const CAGroupEndpoint_t *object, const CARequestInfo_t *requestInfo);
 * @test_data  1. A valid URI (A string with the format, "IP:Port/URI")
 *             2. A token
 *             3. A payload
 * @pre_condition  1. Initialize CA using CAInitialize
 *                 2. Select a valid network using CASelectNetwork
 *                 3. Generate token using CAGenerateToken
 *                 4. Generate remote endpoint from a valid URI using CACreateEndpoint
 *                 5. Form an object of type CAGroupEndpoint_t with information from the remote endpoint generated earlier
 *                 6. Form an object of type CARequestInfo_t with all the necessary parameters, a valid message type, a valid method, the token & the payload
 * @procedure  Call CASendRequestToAll with the object pointer of types CAGroupEndpoint_t & CARequestInfo_t
 * @post_condition  1. Destroy token
 *                  2. Destroy remote endpoint
 *                  3. Terminate CA using CATerminate
 * @expected CASendRequestToAll will return CA_STATUS_OK
 */
test(SendRequestToAll_P)
{
    setupTestCase();
    CAResult_t result = CA_STATUS_OK;
    testLog = TESTLOG_START;

    result = CAInitialize();
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += "CAInitialize";
    isCATerminated = false;

    result = CASelectNetwork(caHelper.m_availableNetwork);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CASelectNetwork";
	
	int port = UINT16_T_PORT_ZERO;
    char* address = (char *) IP_ADDRESS;
    char* resourceUri = (char *) "/a/light";
    CARequestInfo_t requestInfo;
    memset(&requestInfo, 0, sizeof(CARequestInfo_t));

    CAEndpoint_t* endpoint = NULL;
    result = CACreateEndpoint(CA_DEFAULT_FLAGS, caHelper.m_availableNetwork, address, port, &endpoint);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CACreateEndpoint";

    CAToken_t token = NULL;
    uint8_t tokenLength = CA_MAX_TOKEN_LEN;
    result = CAGenerateToken(&token, tokenLength);
    assertEqual(result, CA_STATUS_OK, testLog);
    testLog += ", CAGenerateToken";

    requestInfo.method = CA_GET;
    requestInfo.isMulticast = true;
    requestInfo.info.type = CA_MSG_NONCONFIRM;
    requestInfo.info.token = token;
    requestInfo.info.tokenLength = tokenLength;
    requestInfo.info.payload = (char *) "request payload";

    result = CASendRequest(endpoint, &requestInfo);
    assertEqual(result, CA_STATUS_OK, testLog);

    CADestroyToken(token);
    CADestroyEndpoint(endpoint);
    terminateCA();
}

/**
 * @since 2015-02-13
 * @see CAResult_t CAInitialize()
 * @see void CATerminate()
 * @objective Test 'CASelectNetwork' positively to select valid network adapter
 * @target CAResult_t CASelectNetwork(const CATransportAdapter_t interestedNetwork)
 * @test_data A valid network interface specifier (CA_ETHERNET, CA_WIFI etc.)
 * @pre_condition Initialize CA using CAInitialize
 * @procedure Call CASelectNetwork using the same interface specifier which was used to build the code
 * @post_condition Terminate CA by calling CATerminate
 * @expected It will initialize/enable the given network inteface and return CA_STATUS_OK
 */
test(SelectAvailableNetwork_P)
{
    setupTestCase();
    CAResult_t result = CA_STATUS_OK;
    testLog = TESTLOG_START;

    assertEqual(CAInitialize(), CA_STATUS_OK, "Initialization");
    testLog += "CAInitialize";
    isCATerminated = false;

    result = CASelectNetwork(caHelper.m_availableNetwork);
    assertEqual(result, CA_STATUS_OK, testLog);

    terminateCA();
}

/**
 * @since 2015-02-16
 * @see CAResult_t CAInitialize()
 * @see void CATerminate()
 * @objective Test 'CASelectNetwork' negatively by using a network interface which is not available
 * @target CAResult_t CASelectNetwork(const CATransportAdapter_t interestedNetwork)
 * @test_data valid but unavailable network interface specifier (any specifier other than the one was used to build the code)
 * @pre_condition Initialize CA using CAInitialize
 * @procedure Call CASelectNetwork with unavailable network interface specifier
 * @post_condition Terminate CA by calling CATerminate
 * @expected It will fail to select the given network inteface and won't return CA_STATUS_OK
 */
test(SelectUnavailableNetwork_N)
{
    setupTestCase();
    CAResult_t result = CA_STATUS_OK;
    testLog = TESTLOG_START;

    assertEqual(CAInitialize(), CA_STATUS_OK, "Initialization");
    testLog += "CAInitialize";
    isCATerminated = false;

    result = CASelectNetwork(caHelper.m_unAvailableNetwork);
    assertNotEqual(result, CA_STATUS_OK, testLog);

    terminateCA();
}

/**
 * @since 2015-02-16
 * @see CAResult_t CAInitialize()
 * @see void CATerminate()
 * @objective Test 'CASelectNetwork' negatively by using a invalid value for network specifier
 * @target CAResult_t CASelectNetwork(const CATransportAdapter_t interestedNetwork)
 * @test_data Invalid network interface specifier
 * @pre_condition Initialize CA calling CAInitialize
 * @procedure Call CASelectNetwork invalid network interface specifier
 * @post_condition Terminate CA by calling CATerminate
 * @expected It will fail to select the given network inteface and won't return CA_STATUS_OK
 */
test(SelectInvalidNetwork_N)
{
    setupTestCase();
    CAResult_t result = CA_STATUS_OK;
    testLog = TESTLOG_START;

    assertEqual(CAInitialize(), CA_STATUS_OK, "Initialization");
    testLog += "CAInitialize";
    isCATerminated = false;

    result = CASelectNetwork(NETWORK_OUT_OF_BOUNDARY_VALUE);
    assertNotEqual(result, CA_STATUS_OK, testLog);

    terminateCA();
}

/**
 * @since 2015-02-16
 * @see CAResult_t CAInitialize()
 * @see CAResult_t CASelectNetwork(const CATransportAdapter_t interestedNetwork)
 * @see void CATerminate()
 * @objective Test 'CAUnSelectNetwork' positively to unselect available network interface
 * @target CAResult_t CAUnSelectNetwork(const CATransportAdapter_t nonInterestedNetwork)
 * @test_data valid network interface identifiers
 * @pre_condition  1. Initialize CA using CAInitialize
 *                 2. Call CASelectNetwork with an available network specifier.
 * @procedure Call CAUnSelectNetwork with an available network specifier.
 * @post_condition Terminate CA using CATerminate
 * @expected It will deselect the given network and return CA_STATUS_OK
 */
test(UnSelectAvailableNetwork_P)
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

    result = CAUnSelectNetwork(caHelper.m_availableNetwork);
    assertEqual(result, CA_STATUS_OK, testLog);

    terminateCA();
}

/**
 * @since 2015-02-16
 * @see CAResult_t CAInitialize()
 * @see CAResult_t CASelectNetwork(const CATransportAdapter_t interestedNetwork)
 * @see void CATerminate()
 * @objective Test 'CAUnSelectNetwork' negatively to unselect unavailable network interface
 * @target CAResult_t CAUnSelectNetwork(const CATransportAdapter_t nonInterestedNetwork)
 * @test_data valid but unavailable network interface specifier
 * @pre_condition   1. Initialize CA using CAInitialize
 *                  2. Call CASelectNetwork with an available network interface specifier.
 * @procedure Call CAUnSelectNetwork with an unavailable network interface specifier.
 * @post_condition Terminate CA using CATerminate
 * @expected It will fail to deselect the given network and won't return CA_STATUS_OK
 */
test(UnSelectUnAvailableNetwork_N)
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

    result = CAUnSelectNetwork(caHelper.m_unAvailableNetwork);
    assertNotEqual(result, CA_STATUS_OK, testLog);

    terminateCA();
}

/**
 * @since 2015-02-16
 * @see CAResult_t CAInitialize()
 * @see CAResult_t CASelectNetwork(const CATransportAdapter_t interestedNetwork)
 * @see void CATerminate()
 * @objective Test 'CAUnSelectNetwork' negatively using invalid network interface specifier
 * @target CAResult_t CAUnSelectNetwork(const CATransportAdapter_t nonInterestedNetwork)
 * @test_data invalid network interface specifier
 * @pre_condition   1. Initialize CA using CAInitialize
 *                  2. Call CASelectNetwork with an available network interface specifier.
 * @procedure Call CAUnSelectNetwork with an invalid network interface specifier.
 * @post_condition Terminate CA using CATerminate
 * @expected It will fail to deselect and won't return CA_STATUS_OK
 */
test(UnSelectInvalidNetwork_N)
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

    result = CAUnSelectNetwork(NETWORK_OUT_OF_BOUNDARY_VALUE);
    assertNotEqual(result, CA_STATUS_OK, testLog);

    terminateCA();
}

/**
 * @since 2016-03-08
 * @see CAResult_t CAInitialize()
 * @see CAResult_t CASelectNetwork(CATransportAdapter_t interestedNetwork)
 * @see CAResult_t CAStartListeningServer()
 * @see void CATerminate()
 * @objective Test 'CAStopListeningServer' positively to stop the listening service [SRC]
 * @target CAResult_t CAStopListeningServer();
 * @test_data none
 * @pre_condition  1. Initialize CA using CAInitialize
 *                 2. Select one of the available networks
                   3. Start the listening server
 * @procedure 1. Call the CAStopListeningServer API
 *            2. Check it's return value
 * @post_condition Terminate CA using CATerminate
 * @expected It will stop the listening server and will return CA_STATUS_OK
 */
test(CAStopListeningServer_SRC_P)
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

    result = CAStopListeningServer();
    assertEqual(result, CA_STATUS_OK, testLog);

    terminateCA();
}


/**
 * @since 2016-03-08
 * @see none
 * @objective Test 'CAStartListeningServer' negatively without initializing CA [ECRC]
 * @target CAResult_t CAStartListeningServer();
 * @test_data none
 * @pre_condition  Do not initialize CA
 * @procedure 1. Call the CAStartListeningServer API
 *            2. Check it's return value
 * @post_condition none
 * @expected It will return CA_STATUS_NOT_INITIALIZED
 */
test(CAStartListeningServer_ECRC_N)
{

    CAResult_t result=NULL;

    result = CAStartListeningServer();
    assertEqual(result, CA_STATUS_NOT_INITIALIZED, testLog);
    Serial.println(result);
}

/**
 * @since 2016-03-08
 * @see none
 * @objective Test 'CAStopListeningServer' negatively without initializing CA [ECRC]
 * @target CAResult_t CAStopListeningServer();
 * @test_data none
 * @pre_condition  Do not initialize CA
 * @procedure 1. Call the CAStopListeningServer API
 *            2. Check it's return value
 * @post_condition none
 * @expected It will return CA_STATUS_NOT_INITIALIZED
 */

test(CAStopListeningServer_ECRC_N)
{

    CAResult_t result=NULL;

    result = CAStopListeningServer();
    assertEqual(result, CA_STATUS_NOT_INITIALIZED, testLog);
}

/**
 * @since 2016-03-08
 * @see void CADestroyToken(CAToken_t token)
 * @objective Test 'CAGenerateToken' negatively with LOBV value in Token Length parameter [LOBV]
 * @target CAResult_t CAGenerateToken(CAToken_t* token)
 * @test_data NULL
 * @pre_condition Do not Initialize CA using CAInitialize
 * @procedure Call CAGenerateToken with NULL parameter.
 * @post_condition Destroy token using CADestroyToken function
 * @expected It will return CA_MEMORY_ALLOC_FAILED
 */
test(CAGenerateToken_LOBV_N)
{
	setupTestCase();
    CAResult_t result = CA_STATUS_OK;
    testLog = TESTLOG_START;

    CAToken_t token = NULL;

    result = CAGenerateToken(&token, LOBV);
    assertEqual(result, CA_STATUS_INVALID_PARAM, testLog);
    
	CADestroyToken(token);
}

/**
 * @since 2016-03-08
 * @see void CADestroyToken(CAToken_t token)
 * @objective Test 'CAGenerateToken' negatively with UOBV value in Token Length parameter [UOBV]
 * @target CAResult_t CAGenerateToken(CAToken_t* token)
 * @test_data NULL
 * @pre_condition Do not Initialize CA using CAInitialize
 * @procedure Call CAGenerateToken with NULL parameter.
 * @post_condition Destroy token using CADestroyToken function
 * @expected It will return CA_MEMORY_ALLOC_FAILED
 */
test(CAGenerateToken_UOBV_N)
{
    setupTestCase();
    CAResult_t result = CA_STATUS_OK;
    testLog = TESTLOG_START;

    CAToken_t token = NULL;

    uint8_t tokenLength = CA_MAX_TOKEN_LEN;
    result = CAGenerateToken(&token, tokenLength + 1);
    assertEqual(result,CA_STATUS_INVALID_PARAM, testLog);

    CADestroyToken(token);
}

/**
 * @since 2016-03-08
 * @see void CADestroyToken(CAToken_t token)
 * @objective Test 'CAGenerateToken' negatively with invalid refrerence in token parameter [UFRV]
 * @target CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @test_data Invalid reference
 * @pre_condition Do not Initialize CA using CAInitialize
 * @procedure Call CAGenerateToken with LBV parameter.
 * @post_condition Destroy token using CADestroyToken function
 * @expected It will not generate token & will return CA_STATUS_INVALID_PARAM
 */
test(CAGenerateToken_UFRV_N)
{
    setupTestCase();
    CAResult_t result = CA_STATUS_OK;
    testLog = TESTLOG_START;

    CAToken_t token = NULL;

    result = CAGenerateToken(&token, 0);
    assertEqual(result, CA_STATUS_INVALID_PARAM, testLog);

    CADestroyToken(token);
}

/**
 * @since 2016-03-09
 * @see void CADestroyToken(CAToken_t token)
 * @objective Test 'CAGenerateToken' positively with LBV value in Token Length parameter [LBV]
 * @target CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @test_data LBV value 1
 * @pre_condition Initialize CA using CAInitialize
 * @procedure Call CAGenerateToken with LBV parameter.
 * @post_condition Destroy token using CADestroyToken.
 * @expected 1. It will generate a token and return CA_STATUS_OK
 *           2. Token variable is not NULL
 */
test(CAGenerateToken_LBV_P)
{
    setupTestCase();
    CAResult_t result = CA_STATUS_OK;
    testLog = TESTLOG_START;

    CAToken_t token = NULL;

    uint8_t tokenLength = 1;
    result = CAGenerateToken(&token, tokenLength);
    assertEqual(result, CA_STATUS_OK, testLog);

    if(token == NULL)
    {
        fail();
    }
    
    CADestroyToken (token);
}

/**
 * @since 2016-03-09
 * @see CAResult_t CAInitialize()
 * @see void CATerminate()
 * @objective Test 'CASelectNetwork' negatively to select invalid network will NULL vale [NV]
 * @target CAResult_t CASelectNetwork(const CATransportAdapter_t interestedNetwork);
 * @test_data Network type
 * @pre_condition Initialize CA using CAInitialize
 * @procedure  1. Call CASelectNetwork using invalid network value (NULL)
 *             2. Check it's return value
 * @post_condition Terminate CA using CATerminate function
 * @expected It will fail to select a network and will return CA_NOT_SUPPORTED
 */
test(CASelectNetwork_NV_N)
{
    setupTestCase();
    CAResult_t result = CA_STATUS_OK;
    testLog = TESTLOG_START;

    assertEqual(CAInitialize(), CA_STATUS_OK, "Initialization");
    testLog += "CAInitialize";
    isCATerminated = false;

    result = CASelectNetwork(NULL);
    assertEqual(result, CA_NOT_SUPPORTED, testLog);

    terminateCA();
}

/**
 * @since 2016-03-09
 * @see CAResult_t CAInitialize()
 * @see void CATerminate()
 * @objective Test 'CASelectNetwork' positively with LBV of selected network [LBV]
 * @target CAResult_t CASelectNetwork(const CATransportAdapter_t interestedNetwork);
 * @test_data Network type
 * @pre_condition Initialize CA using CAInitialize
 * @procedure  1. Call CASelectNetwork using lower boundary value
 *             2. Check it's return value
 * @post_condition Terminate CA using CATerminate function
 * @expected It will return CA_NOT_SUPPORTED
 */
test(CASelectNetwork_LBV_P)
{
    setupTestCase();
    CAResult_t result = CA_STATUS_OK;
    testLog = TESTLOG_START;

    assertEqual(CAInitialize(), CA_STATUS_OK, "Initialization");
    testLog += "CAInitialize";
    isCATerminated = false;

    result = CASelectNetwork(CA_DEFAULT_ADAPTER);
    assertEqual(result, CA_NOT_SUPPORTED, testLog);

    terminateCA();

}

/**
 * @since 2016-03-09
 * @see CAResult_t CAInitialize()
 * @see void CATerminate()
 * @objective Test 'CASelectNetwork' positively with UBV of selected network [UBV]
 * @target CAResult_t CASelectNetwork(const CATransportAdapter_t interestedNetwork);
 * @test_data Network type
 * @pre_condition Initialize CA using CAInitialize
 * @procedure  1. Call CASelectNetwork using upper boundary value
 *             2. Check it's return value
 * @post_condition Terminate CA using CATerminate function
 * @expected It will return CA_STATUS_OK
 */
test(CASelectNetwork_UBV_P)
{
    setupTestCase();
    CAResult_t result = CA_STATUS_OK;
    testLog = TESTLOG_START;

    assertEqual(CAInitialize(), CA_STATUS_OK, "Initialization");
    testLog += "CAInitialize";
    isCATerminated = false;

    result = CASelectNetwork(CA_ALL_ADAPTERS);
    assertEqual(result, CA_STATUS_OK, testLog);

    terminateCA();
}

/**
 * @since 2016-03-09
 * @see none
 * @objective Test 'CAUnSelectNetwork' negatively without initializing CA [ECRC]
 * @target CAResult_t CAUnSelectNetwork(const CATransportAdapter_t nonInterestedNetwork);
 * @test_data Without initialize CA call to unselect network
 * @pre_condition none
 * @procedure  1. Call CAUnSelectNetwork
 *             2. Check it's return value
 * @post_condition none
 * @expected It will fail to un-select the network and will not return CA_STATUS_OK
 */
test(CAUnSelectNetwork_ECRC_N)
{
    CAResult_t result = CAUnSelectNetwork(caHelper.m_availableNetwork);
    assertNotEqual(result, CA_STATUS_OK, testLog);
}

/**
 * @since 2016-03-09
 * @see CAResult_t CAInitialize()
 * @see CAResult_t CASelectNetwork(const CATransportAdapter_t interestedNetwork)
 * @see void CATerminate()
 * @objective Test 'CAUnSelectNetwork' positively with LBV of selected network [LBV]
 * @target CAResult_t CAUnSelectNetwork(CATransportAdapter_t nonInterestedNetwork)
 * @test_data Network type
 * @pre_condition Initialize CA using CAInitialize
 * @procedure  1. Call CASelectNetwork using lower boundary value
 *             2. Check it's return value
 *             3. Call CAUnselectNetwork using lower boundary value
 *             4. Check it's return value
 * @post_condition Terminate CA using CATerminate function
 * @expected It will return not CA_STATUS_OK
 */
test(CAUnSelectNetwork_LBV_N)
{
    setupTestCase();
    CAResult_t result = CA_STATUS_OK;
    testLog = TESTLOG_START;

    assertEqual(CAInitialize(), CA_STATUS_OK, "Initialization");
    testLog += "CAInitialize";
    isCATerminated = false;

    result = CASelectNetwork(LBV);
    assertNotEqual(result, CA_STATUS_OK, testLog);

    result = CAUnSelectNetwork(LBV);
    assertNotEqual(result, CA_STATUS_OK, testLog);

    terminateCA();
}

/**
 * @since 2016-03-09
 * @see CAResult_t CAInitialize()
 * @see CAResult_t CASelectNetwork(const CATransportAdapter_t interestedNetwork)
 * @see void CATerminate()
 * @objective Test 'CAUnSelectNetwork' positively with UBV of selected network [UBV]
 * @target CAResult_t CAUnSelectNetwork(CATransportAdapter_t nonInterestedNetwork)
 * @test_data Network type
 * @pre_condition Initialize CA using CAInitialize
 * @procedure  1. Call CASelectNetwork using upper boundary value
 *             2. Check it's return value
 *             3. Call CAUnselectNetwork using upper boundary value
 *             4. Check it's return value
 * @post_condition Terminate CA using CATerminate function
 * @expected It will unselect the network and will return CA_STATUS_OK
 */
test(CAUnSelectNetwork_UBV_P)
{
    setupTestCase();
    CAResult_t result = CA_STATUS_OK;
    testLog = TESTLOG_START;

    assertEqual(CAInitialize(), CA_STATUS_OK, "Initialization");
    testLog += "CAInitialize";
    isCATerminated = false;

    result = CASelectNetwork(CA_ALL_ADAPTERS);
    assertEqual(result, CA_STATUS_OK, testLog);

    result = CAUnSelectNetwork(CA_ALL_ADAPTERS);
    assertEqual(result, CA_STATUS_OK, testLog);

    terminateCA();
}

/**
 * @since 2016-03-10
 * @see CAResult_t CAInitialize()
 * @see CAResult_t CASelectNetwork(const CATransportAdapter_t interestedNetwork)
 * @see void CATerminate()
 * @objective Test CAGetNetworkInformation negatively with invalid sample value in Info parameter [Info_USV]
 * @target CAResult_t CAGetNetworkInformation(CALocalConnectivity_t **info, uint32_t* size);
 * @test_data none
 * @pre_condition Initialize CA using CAInitialize
 * @procedure  1. Select available network
 *             2. Call CAGetNetworkInformation with inavlid info value
 *             3. Check it's return value
 * @post_condition Terminate CA using CATerminate
 * @expected It will not return CA_STATUS_OK
 */
test(CAGetNetworkInformation_Info_USV_N)
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

    result = NULL;
    CAEndpoint_t **tempInfo = NULL;
    uint32_t tempSize = 0;

    result = CAGetNetworkInformation(tempInfo, &tempSize);
    assertNotEqual(result, CA_STATUS_OK, testLog);

    CATerminate();
}

/**
 * @since 2016-02-10
 * @see CAResult_t CAInitialize()
 * @see CAResult_t CASelectNetwork(const CATransportAdapter_t interestedNetwork)
 * @see void CATerminate()
 * @objective Test CAGetNetworkInformation negatively with invalid sample value in size parameter [Size_USV]
 * @target CAResult_t CAGetNetworkInformation(CALocalConnectivity_t **info, uint32_t* size);
 * @test_data none
 * @pre_condition Initialize CA using CAInitialize
 * @procedure  1. Select available network
 *             2. Call CAGetNetworkInformation with inavlid size value
 *             3. Check it's return value
 * @post_condition Terminate CA using CATerminate
 * @expected It will not return CA_STATUS_OK
 */
test(CAGetNetworkInformation_temSize_USV_N)
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

    result = NULL;
    CAEndpoint_t *tempInfo = NULL;
    uint32_t *tempSize = 0;

    result = CAGetNetworkInformation(&tempInfo, tempSize);
    assertNotEqual(result, CA_STATUS_OK, testLog);

    terminateCA();
}

/**
 * @since 2016-03-10
 * @see CAResult_t CAInitialize()
 * @see void CATerminate()
 * @objective Test CAGetNetworkInformation negatively without initializing CA [ECRC]
 * @target CAResult_t CAGetNetworkInformation(CALocalConnectivity_t **info, uint32_t* size);
 * @test_data none
 * @pre_condition Do not initialize CA using CAInitialize
 * @procedure  1. Call CAGetNetworkInformation
 *             2. Check it's return value
 * @post_condition Terminate CA using CATerminate
 * @expected It will not return CA_STATUS_OK
 */
test(CAGetNetworkInformation_ECRC_N)
{
    CAResult_t result = NULL;
    CAEndpoint_t *tempInfo = NULL;
    uint32_t *tempSize = 0;

    result = CAGetNetworkInformation(&tempInfo, tempSize);
    assertNotEqual(result, CA_STATUS_OK, testLog);

    terminateCA();
}
