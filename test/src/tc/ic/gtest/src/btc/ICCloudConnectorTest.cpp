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
#include "ICHelper.h"

using namespace OC;
using namespace OCPlatform;
using namespace std;

#define HOST            "coap://192.168.1.2:5000"
#define AUTH_PROVIDER   "AnyAuthProvider"
#define AUTH_CODE       "AnyAuthCode"
#define ACCESS_TOKEN    "AnyAccessToken"
#define REFRESH_TOKEN   "AnyRefreshToken"
#define USER_ID         "AnyUserId"
#define DEVICE_ID       "AnyDeviceId"
#define GROUP_ID        "AnyGroupId"
#define KEY             "AnyKey"
#define VALUE           "AnyValue"

#define INVALID_PARAMETER ""
#define EMPTY_VALUE       ""
#define ZERO_VALUE        0

class ICCloudConnectorTest_btc: public ::testing::Test {

public:
	std::vector<std::string> m_deviceId = { "AnyDevideId" };

	OCAccountManager::Ptr m_accountManager;

	QueryParamsMap m_queryParams;

	static void cloudConnectHandler(const HeaderOptions&,
			const OCRepresentation&, const int) {

	}

	static void cloudConnectHandlerGetCallback(const HeaderOptions&,
			const OCRepresentation&, const int) {

	}

	static void cloudConnectHandlerPostCallback(const HeaderOptions&,
			const OCRepresentation&, const int) {

	}

	static void cloudConnectHandlerPutCallback(const HeaderOptions&,
			const OCRepresentation&, const int) {

	}

	static void cloudConnectHandlerDeleteCallback(const HeaderOptions&,
			const int) {

	}

	static void cloudConnectHandlerDelete(const HeaderOptions&, const int) {

	}

	static void cloudConnectHandlerObserveCallback(const HeaderOptions&,
			const OCRepresentation&, const int, const int) {

	}

protected:
	std::string m_errorMsg;

	virtual void SetUp() {
		m_accountManager = OCPlatform::constructAccountManagerObject(HOST,
				CT_DEFAULT);

		m_queryParams = {{KEY,VALUE}};
	}

	virtual void TearDown() {
	}

};

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'host' function positively
 * @target string host()
 * @test_data None
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform string host() API
 * @post_condition None
 * @expected host will provides host address of account server.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, Host_SRC_P)
{
	EXPECT_EQ(HOST, m_accountManager->host())<<"Host is not to getting the host address of account server!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'connectivityType' function positively
 * @target OCConnectivityType connectivityType()
 * @test_data None
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform OCConnectivityType connectivityType() API
 * @post_condition None
 * @expected connectivityType will provides connectivity type for account server.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, ConnectivityType_SRC_P)
{
	EXPECT_EQ(CT_DEFAULT, m_accountManager->connectivityType())<<"Host is not to getting the connectivity type for account server!";
}
#endif

/**
 * @since 2016-08-24
 * @see None
 * @objective Test 'signUp' function with valid authProvide, authCode and PostCallback
 * @target OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @test_data authProvider authCode and cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler) API
 * @post_condition None
 * @expected signUp will provides OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, SignUp_SRC_P)
{
	EXPECT_EQ(OC_STACK_OK, m_accountManager->signUp(AUTH_PROVIDER, AUTH_CODE, &cloudConnectHandler))<<"SignUp API does not account registration to account server!";
}
#endif

/**
 * @since 2016-08-24
 * @see None
 * @objective Test 'signUp' function with valid authProvide, authCode and NULL PostCallback
 * @target OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @test_data authProvider and authCode
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler) API
 * @post_condition None
 * @expected signUp will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, SignUpNULLCallback_NV_N)
{
	EXPECT_ANY_THROW(m_accountManager->signUp(AUTH_PROVIDER, AUTH_CODE, nullptr))<<"SignUp API create account registration to account server with NULL callback!";
}
#endif

/**
 * @since 2016-08-24
 * @see None
 * @objective Test 'signUp' function with empty authProvide, authCode and valid PostCallback
 * @target OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @test_data empty string and cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler) API
 * @post_condition None
 * @expected signUp will will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, SignUp_ESV_N)
{
	EXPECT_ANY_THROW(m_accountManager->signUp(EMPTY_VALUE, EMPTY_VALUE, &cloudConnectHandler))<<"SignUp API create account registration to account server with empty string!";
}
#endif

/**
 * @since 2016-08-24
 * @see None
 * @objective Test 'signUp' function with invalid authProvide, authCode and valid PostCallback
 * @target OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @test_data invalid parameter and cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler) API
 * @post_condition None
 * @expected signUp will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, SignUp_USV_N)
{
	EXPECT_ANY_THROW(m_accountManager->signUp(INVALID_PARAMETER, INVALID_PARAMETER, &cloudConnectHandler))<<"SignUp API create account registration to account server with Invalid Parameter!";
}
#endif

/**
 * @since 2016-08-24
 * @see None
 * @objective Test 'signUp' function with valid authProvide, authCode, QueryParamMap and PostCallback
 * @target OCStackResult signUp(const std::string& authProvider, const std::string& authCode, const QueryParamsMap& options, PostCallback cloudConnectHandler)
 * @test_data authProvider authCode, queryParams and cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform OCStackResult signUp(const std::string& authProvider, const std::string& authCode, const QueryParamsMap& options, PostCallback cloudConnectHandler) API
 * @post_condition None
 * @expected signUp will provides OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, SignUpQueryParams_SRC_P)
{
	EXPECT_EQ(OC_STACK_OK, m_accountManager->signUp(AUTH_PROVIDER, AUTH_CODE, m_queryParams, &cloudConnectHandler))<<"SignUp API does not account registration to account server!";
}
#endif

/**
 * @since 2016-08-24
 * @see None
 * @objective Test 'signUp' function with valid authProvide, authCode, QueryParamMap and NULL PostCallback
 * @target OCStackResult signUp(const std::string& authProvider, const std::string& authCode, const QueryParamsMap& options, PostCallback cloudConnectHandler)
 * @test_data authProvider, authCode, queryParams and NULL cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform OCStackResult signUp(const std::string& authProvider, const std::string& authCode, const QueryParamsMap& options, PostCallback cloudConnectHandler) API
 * @post_condition None
 * @expected signUp will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, SignUpQueryParams_NV_N)
{
	EXPECT_ANY_THROW(m_accountManager->signUp(AUTH_PROVIDER, AUTH_CODE, m_queryParams, nullptr))<<"SignUp API create account registration to account server with NULL callback!";
}
#endif

/**
 * @since 2016-08-24
 * @see None
 * @objective Test 'signUp' function with invalid authProvide, authCode, QueryParamMap and valid PostCallback
 * @target OCStackResult signUp(const std::string& authProvider, const std::string& authCode, const QueryParamsMap& options, PostCallback cloudConnectHandler)
 * @test_data invalid parameter, queryParams and cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform OCStackResult signUp(const std::string& authProvider, const std::string& authCode, const QueryParamsMap& options, PostCallback cloudConnectHandler) API
 * @post_condition None
 * @expected signUp will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, SignUpQueryParams_USV_N)
{
	EXPECT_ANY_THROW(m_accountManager->signUp(INVALID_PARAMETER, INVALID_PARAMETER, m_queryParams, &cloudConnectHandler))<<"SignUp API create account registration to account server with Invalid Parameter!";
}
#endif

/**
 * @since 2016-08-24
 * @see None
 * @objective Test 'signUp' function with empty authProvide, authCode, QueryParamMap and valid PostCallback
 * @target OCStackResult signUp(const std::string& authProvider, const std::string& authCode, const QueryParamsMap& options, PostCallback cloudConnectHandler)
 * @test_data empty string, queryParams and cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform OCStackResult signUp(const std::string& authProvider, const std::string& authCode, const QueryParamsMap& options, PostCallback cloudConnectHandler) API
 * @post_condition None
 * @expected signUp will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, SignUpQueryParams_ESV_N)
{
	EXPECT_ANY_THROW(m_accountManager->signUp(EMPTY_VALUE, EMPTY_VALUE, m_queryParams, &cloudConnectHandler))<<"SignUp API create account registration to account server with empty parameter!";
}
#endif

/**
 * @since 2016-08-24
 * @see None
 * @objective Test 'signIn' function with valid userId, accessToken and PostCallback
 * @target OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @test_data userId, access token and cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @post_condition None
 * @expected signIn will provides OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, SignIn_SRC_P)
{
	EXPECT_EQ(OC_STACK_OK, m_accountManager->signIn(USER_ID, ACCESS_TOKEN, &cloudConnectHandler))<<"SignIn API does not sign-in to account server!";
}
#endif

/**
 * @since 2016-08-24
 * @see None
 * @objective Test 'signIn' function with valid userId, accessToken and NULL PostCallback
 * @target OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @test_data userId, access token and NULL cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @post_condition None
 * @expected signIn will not provides OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, SignInNULLCallback_NV_N)
{
	EXPECT_ANY_THROW(m_accountManager->signIn(USER_ID, ACCESS_TOKEN, nullptr))<<"SignIn API sign-in to account server with NULL callback!";
}
#endif

/**
 * @since 2016-08-24
 * @see None
 * @objective Test 'signIn' function with invalid userId, accessToken and valid PostCallback
 * @target OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @test_data invalid parameter and cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @post_condition None
 * @expected signIn will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, SignIn_USV_N)
{
	EXPECT_ANY_THROW(m_accountManager->signIn(INVALID_PARAMETER, INVALID_PARAMETER, &cloudConnectHandler))<<"SignIn API sign-in to account server with invalid parameter!";
}
#endif

/**
 * @since 2016-08-24
 * @see None
 * @objective Test 'signIn' function with empty userId, accessToken and valid PostCallback
 * @target OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @test_data empty string and cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @post_condition None
 * @expected signIn will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, SignIn_ESV_N)
{
	EXPECT_ANY_THROW(m_accountManager->signIn(EMPTY_VALUE, EMPTY_VALUE, &cloudConnectHandler))<<"SignIn API sign-in to account server with empty parameter!";
}
#endif

/**
 * @since 2016-08-24
 * @see None
 * @objective Test 'signOut' function with valid PostCallback
 * @target OCStackResult signOut(PostCallback cloudConnectHandler)
 * @test_data cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform OCStackResult signOut(PostCallback cloudConnectHandler) API
 * @post_condition None
 * @expected signOut will provides OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, SignOut_SRC_P)
{
	EXPECT_EQ(OC_STACK_OK, m_accountManager->signOut(&cloudConnectHandler))<<"SignOut API does not sign-out to account server!";
}
#endif

/**
 * @since 2016-08-24
 * @see None
 * @objective Test 'signOut' function with NULL PostCallback
 * @target OCStackResult signOut(PostCallback cloudConnectHandler)
 * @test_data NULL cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform OCStackResult signOut(PostCallback cloudConnectHandler) API
 * @post_condition None
 * @expected signOut will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, SignOutNULLcallback_NV_N)
{
	EXPECT_ANY_THROW(m_accountManager->signOut(nullptr))<<"SignOut API sign-out to account server with NULL callback!";
}
#endif

/**
 * @since 2016-08-24
 * @see None
 * @objective Test 'refreshAccessToken' function with valid userUuid, refreshToken and PostCallback
 * @target OCStackResult refreshAccessToken(const std::string& userUuid, const std::string& refreshToken, PostCallback cloudConnectHandler)
 * @test_data userId, refresh token and cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform OCStackResult refreshAccessToken(const std::string& userUuid, const std::string& refreshToken, PostCallback cloudConnectHandler) API
 * @post_condition None
 * @expected refreshAccessToken will provides OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, RefreshAccessToken_SRC_P)
{
	EXPECT_EQ(OC_STACK_OK, m_accountManager->refreshAccessToken(USER_ID, REFRESH_TOKEN, &cloudConnectHandler))<<"RefreshAccessToken API does not refresh access token to account server!";
}
#endif

/**
 * @since 2016-08-24
 * @see None
 * @objective Test 'refreshAccessToken' function with valid userUuid, refreshToken and NULL PostCallback
 * @target OCStackResult refreshAccessToken(const std::string& userUuid, const std::string& refreshToken, PostCallback cloudConnectHandler)
 * @test_data userId, refresh token and NULL cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform OCStackResult refreshAccessToken(const std::string& userUuid, const std::string& refreshToken, PostCallback cloudConnectHandler) API
 * @post_condition None
 * @expected refreshAccessToken will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, RefreshAccessTokenNULLCallback_NV_N)
{
	EXPECT_ANY_THROW(m_accountManager->refreshAccessToken(USER_ID, REFRESH_TOKEN, nullptr))<<"RefreshAccessToken API refresh access token to account server with NULL callback!";
}
#endif

/**
 * @since 2016-08-24
 * @see None
 * @objective Test 'refreshAccessToken' function with empty userUuid, refreshToken and valid PostCallback
 * @target OCStackResult refreshAccessToken(const std::string& userUuid, const std::string& refreshToken, PostCallback cloudConnectHandler)
 * @test_data empty string and cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform OCStackResult refreshAccessToken(const std::string& userUuid, const std::string& refreshToken, PostCallback cloudConnectHandler) API
 * @post_condition None
 * @expected refreshAccessToken will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, RefreshAccessToken_ESV_N)
{
	EXPECT_ANY_THROW(m_accountManager->refreshAccessToken(EMPTY_VALUE, EMPTY_VALUE, &cloudConnectHandler))<<"RefreshAccessToken API refresh access token to account server with empty parameter!";
}
#endif

/**
 * @since 2016-08-24
 * @see None
 * @objective Test 'refreshAccessToken' function with invalid userUuid, refreshToken and valid PostCallback
 * @target OCStackResult refreshAccessToken(const std::string& userUuid, const std::string& refreshToken, PostCallback cloudConnectHandler)
 * @test_data invalid parameter and cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform OCStackResult refreshAccessToken(const std::string& userUuid, const std::string& refreshToken, PostCallback cloudConnectHandler) API
 * @post_condition None
 * @expected refreshAccessToken will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, RefreshAccessToken_USV_N)
{
	EXPECT_ANY_THROW(m_accountManager->refreshAccessToken(INVALID_PARAMETER, INVALID_PARAMETER, &cloudConnectHandler))<<"RefreshAccessToken API refresh access token to account server with invalid parameter!";
}
#endif

/**
 * @since 2016-08-25
 * @see None
 * @objective Test 'searchUser' function with user ID and getCallback
 * @target OCStackResult searchUser(const std::string& userUuid,GetCallback cloudConnectHandler);
 * @test_data userId and cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform searchUser(const std::string& userUuid,GetCallback cloudConnectHandler) API
 * @post_condition None
 * @expected searchUser provide OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, SearchUserWithUserID_SRC_P)
{
	EXPECT_EQ(OC_STACK_OK, m_accountManager->searchUser(USER_ID, &cloudConnectHandler))<<"SearchUser API does not get information of the user to account server!";
}
#endif

/**
 * @since 2016-08-25
 * @see None
 * @objective Test 'searchUser' function with user ID and NULL getCallback
 * @target OCStackResult searchUser(const std::string& userUuid,GetCallback cloudConnectHandler);
 * @test_data userId and NULL cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform searchUser(const std::string& userUuid,GetCallback cloudConnectHandler) API
 * @post_condition None
 * @expected searchUser will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, SearchUserWithUserID_NV_N)
{
	EXPECT_ANY_THROW(m_accountManager->searchUser(USER_ID, nullptr))<<"SearchUser API is getting information of the user to account server with NULL callback!";
}
#endif

/**
 * @since 2016-08-25
 * @see None
 * @objective Test 'searchUser' function with empty user ID and valid getCallback
 * @target OCStackResult searchUser(const std::string& userUuid,GetCallback cloudConnectHandler);
 * @test_data empty string and cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform searchUser(const std::string& userUuid,GetCallback cloudConnectHandler) API
 * @post_condition None
 * @expected searchUser will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, SearchUserWithUserID_ESV_N)
{
	EXPECT_ANY_THROW(m_accountManager->searchUser(EMPTY_VALUE, &cloudConnectHandler))<<"SearchUser API is getting information of the user to account server with empty parameter!";
}
#endif

/**
 * @since 2016-08-25
 * @see None
 * @objective Test 'searchUser' function with invalid user ID and valid getCallback
 * @target OCStackResult searchUser(const std::string& userUuid,GetCallback cloudConnectHandler);
 * @test_data invalid parameter and cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform searchUser(const std::string& userUuid,GetCallback cloudConnectHandler) API
 * @post_condition None
 * @expected searchUser will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, SearchUserWithUserID_USV_N)
{
	EXPECT_ANY_THROW(m_accountManager->searchUser(INVALID_PARAMETER, &cloudConnectHandler))<<"SearchUser API is getting information of the user to account server with invalid parameter!";
}
#endif

/**
 * @since 2016-08-25
 * @see None
 * @objective Test 'searchUser' function with query param and getCallback
 * @target OCStackResult searchUser(const std::string& userUuid,GetCallback cloudConnectHandler);
 * @test_data queryParams and cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform searchUser API
 * @post_condition None
 * @expected searchUser will provide OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, SearchUserWithQueryParam_SRC_P)
{
	EXPECT_EQ(OC_STACK_OK, m_accountManager->searchUser(m_queryParams, &cloudConnectHandler))<<"SearchUser API is not getting information of the user to account server!";
}
#endif

/**
 * @since 2016-08-25
 * @see None
 * @objective Test 'searchUser' function with query param and NULL getCallback
 * @target OCStackResult searchUser(const std::string& userUuid,GetCallback cloudConnectHandler);
 * @test_data queryParams and NULL cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform searchUser API
 * @post_condition None
 * @expected searchUser will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, SearchUserWithQueryParam_NV_N)
{
	EXPECT_ANY_THROW(m_accountManager->searchUser(m_queryParams, nullptr))<<"SearchUser API is getting information of the user to account server with NULL callback!";
}
#endif

/**
 * @since 2016-08-25
 * @see None
 * @objective Test 'searchUser' function with empty query param and valid getCallback
 * @target OCStackResult searchUser(const std::string& userUuid,GetCallback cloudConnectHandler);
 * @test_data empty string and cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform searchUser API
 * @post_condition None
 * @expected searchUser will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, SearchUserWithQueryParam_ESV_N)
{
	EXPECT_ANY_THROW(m_accountManager->searchUser(EMPTY_VALUE, &cloudConnectHandler))<<"SearchUser API is getting information of the user to account server with empty string!";
}
#endif

/**
 * @since 2016-08-25
 * @see None
 * @objective Test 'searchUser' function with invalid query param and valid getCallback
 * @target OCStackResult searchUser(const std::string& userUuid,GetCallback cloudConnectHandler);
 * @test_data invalid parameter and cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform searchUser API
 * @post_condition None
 * @expected searchUser will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, SearchUserWithQueryParam_USV_N)
{
	EXPECT_ANY_THROW(m_accountManager->searchUser(INVALID_PARAMETER, &cloudConnectHandler))<<"SearchUser API is getting information of the user to account server with invalid parameter!";
}
#endif

/**
 * @since 2016-08-25
 * @see None
 * @objective Test 'deleteDevice' function with deviceID and deleteCallback
 * @target OCStackResult deleteDevice(const std::string& deviceId, DeleteCallback cloudConnectHandler);
 * @test_data deviceId and cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform deleteDevice API
 * @post_condition None
 * @expected deleteDevice provide OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, DeleteDevice_SRC_P)
{
	EXPECT_EQ(OC_STACK_OK, m_accountManager->deleteDevice(DEVICE_ID, &cloudConnectHandlerDelete))<<"DeleteDevice API is not deleteting the device registered on the account signed-in!";
}
#endif

/**
 * @since 2016-08-25
 * @see None
 * @objective Test 'deleteDevice' function with deviceID and NULL deleteCallback
 * @target OCStackResult deleteDevice(const std::string& deviceId, DeleteCallback cloudConnectHandler);
 * @test_data deviceId and NULL cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform deleteDevice API
 * @post_condition None
 * @expected deleteDevice will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, DeleteDevice_NV_N)
{
	EXPECT_ANY_THROW(m_accountManager->deleteDevice(DEVICE_ID, nullptr))<<"DeleteDevice API is deleteting the device registered on the account signed-in with NULL callback!";
}
#endif

/**
 * @since 2016-08-26
 * @see None
 * @objective Test 'deleteDevice' function with empty deviceID and valid deleteCallback
 * @target OCStackResult deleteDevice(const std::string& deviceId, DeleteCallback cloudConnectHandler);
 * @test_data empty string and cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform deleteDevice API
 * @post_condition None
 * @expected deleteDevice will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, DeleteDevice_ESV_N)
{
	EXPECT_ANY_THROW(m_accountManager->deleteDevice(EMPTY_VALUE, &cloudConnectHandlerDelete))<<"DeleteDevice API is deleteting the device registered on the account signed-in with empty parameter!";
}
#endif

/**
 * @since 2016-08-26
 * @see None
 * @objective Test 'deleteDevice' function with invalid deviceID and valid deleteCallback
 * @target OCStackResult deleteDevice(const std::string& deviceId, DeleteCallback cloudConnectHandler);
 * @test_data invalid parameter and cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform deleteDevice API
 * @post_condition None
 * @expected deleteDevice will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, DeleteDevice_USV_N)
{
	EXPECT_ANY_THROW(m_accountManager->deleteDevice(INVALID_PARAMETER, &cloudConnectHandlerDelete))<<"DeleteDevice API is deleteting the device registered on the account signed-in with invalid parameter!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'createGroup' function with AclGroupType groupType and PostCallback cloudConnectHandler
 * @target OCStackResult createGroup(AclGroupType groupType, PostCallback cloudConnectHandler);
 * @test_data groupType and cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform createGroup API
 * @post_condition None
 * @expected createGroup provide OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, CreateGroup_SRC_P)
{
	EXPECT_EQ(OC_STACK_OK, m_accountManager->createGroup(AclGroupType::PUBLIC, &cloudConnectHandlerPostCallback))<<"CreateGroup API is not getting a list of groups joined from account server!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'createGroup' function with AclGroupType groupType and PostCallback cloudConnectHandler
 * @target OCStackResult createGroup(AclGroupType groupType, PostCallback cloudConnectHandler);
 * @test_data private groupType and cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform createGroup API
 * @post_condition None
 * @expected createGroup provide OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, CreateGroupPrivate_SRC_P)
{
	EXPECT_EQ(OC_STACK_OK, m_accountManager->createGroup(AclGroupType::PRIVATE, &cloudConnectHandlerPostCallback))<<"CreateGroup API is not getting a list of groups joined from account server!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'createGroup' function with AclGroupType groupType and NULL PostCallback cloudConnectHandler
 * @target OCStackResult createGroup(AclGroupType groupType, PostCallback cloudConnectHandler);
 * @test_data groupType and cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform createGroup API
 * @post_condition None
 * @expected createGroup will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, CreateGroup_NV_N)
{
	EXPECT_ANY_THROW(m_accountManager->createGroup(AclGroupType::PUBLIC, nullptr))<<"CreateGroup API is getting a list of groups joined from account server!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'createGroup' function with AclGroupType groupType and NULL PostCallback cloudConnectHandler
 * @target OCStackResult createGroup(AclGroupType groupType, PostCallback cloudConnectHandler);
 * @test_data private groupType and cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform createGroup API
 * @post_condition None
 * @expected createGroup will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, CreateGroupPrivate_NV_N)
{
	EXPECT_ANY_THROW(m_accountManager->createGroup(AclGroupType::PRIVATE, nullptr))<<"CreateGroup API is getting a list of groups joined from account server!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'getGroupList' function with GetCallback cloudConnectHandler
 * @target OCStackResult getGroupList(GetCallback cloudConnectHandler);
 * @test_data cloudConnectHandlerGetCallback
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform getGroupList API
 * @post_condition None
 * @expected getGroupList provide OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, GetGroupList_SRC_P)
{
	EXPECT_EQ(OC_STACK_OK, m_accountManager->getGroupList(&cloudConnectHandlerGetCallback))<<"GetGroupList API is getting a list of groups joined from account server!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'getGroupList' function with NULL GetCallback cloudConnectHandler
 * @target OCStackResult getGroupList(GetCallback cloudConnectHandler);
 * @test_data cloudConnectHandlerGetCallback
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform getGroupList API
 * @post_condition None
 * @expected getGroupList will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, GetGroupList_NV_N)
{
	EXPECT_ANY_THROW(m_accountManager->getGroupList(nullptr))<<"GetGroupList API is getting a list of groups joined from account server!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'deleteGroup' function with const std::string& groupId and DeleteCallback cloudConnectHandler
 * @target OCStackResult deleteGroup(const std::string& groupId and DeleteCallback cloudConnectHandler);
 * @test_data groupId and DeleteCallback cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform deleteGroup API
 * @post_condition None
 * @expected deleteGroup provide OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, DeleteGroup_SRC_P)
{
	EXPECT_EQ(OC_STACK_OK, m_accountManager->deleteGroup(GROUP_ID, &cloudConnectHandlerDeleteCallback))<<"deleteGroup API is NOT to delete the group from account server!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'deleteGroup' function with const std::string& groupId and NULL DeleteCallback cloudConnectHandler
 * @target OCStackResult deleteGroup(const std::string& groupId and DeleteCallback cloudConnectHandler);
 * @test_data groupId and DeleteCallback cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform deleteGroup API
 * @post_condition None
 * @expected deleteGroup will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, DeleteGroup_NV_N)
{
	EXPECT_ANY_THROW(m_accountManager->deleteGroup(GROUP_ID, nullptr))<<"deleteGroup API is to delete the group from account server!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'deleteGroup' function with const invalid groupId and valid DeleteCallback cloudConnectHandler
 * @target OCStackResult deleteGroup(const std::string& groupId and DeleteCallback cloudConnectHandler);
 * @test_data groupId and DeleteCallback cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform deleteGroup API
 * @post_condition None
 * @expected deleteGroup will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, DeleteGroup_USV_N)
{
	EXPECT_ANY_THROW(m_accountManager->deleteGroup(INVALID_PARAMETER, &cloudConnectHandlerDeleteCallback))<<"deleteGroup API is to delete the group from account server!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'deleteGroup' function with const invalid groupId and valid DeleteCallback cloudConnectHandler
 * @target OCStackResult deleteGroup(const std::string& groupId and DeleteCallback cloudConnectHandler);
 * @test_data groupId and DeleteCallback cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform deleteGroup API
 * @post_condition None
 * @expected deleteGroup will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, DeleteGroup_ESV_N)
{
	EXPECT_ANY_THROW(m_accountManager->deleteGroup(EMPTY_VALUE, &cloudConnectHandlerDeleteCallback))<<"deleteGroup API is to delete the group from account server!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'joinGroup' function with const std::string& groupId and PostCallback cloudConnectHandler
 * @target OCStackResult joinGroup(const std::string& groupId and PostCallback cloudConnectHandler);
 * @test_data groupId and PostCallback cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform joinGroup API
 * @post_condition None
 * @expected joinGroup provide OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, JoinGroup_SRC_P)
{
	EXPECT_EQ(OC_STACK_OK, m_accountManager->joinGroup(GROUP_ID, &cloudConnectHandlerPostCallback))<<"joinGroup API is not to join the group on account server!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'joinGroup' function with const std::string& groupId and NULL PostCallback cloudConnectHandler
 * @target OCStackResult joinGroup(const std::string& groupId and PostCallback cloudConnectHandler);
 * @test_data groupId and PostCallback cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform joinGroup API
 * @post_condition None
 * @expected joinGroup will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, JoinGroup_NV_N)
{
	EXPECT_ANY_THROW(m_accountManager->joinGroup(GROUP_ID, nullptr))<<"joinGroup API is to join the group on account server!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'joinGroup' function with invalid groupId and valid PostCallback cloudConnectHandler
 * @target OCStackResult joinGroup(const std::string& groupId and PostCallback cloudConnectHandler);
 * @test_data groupId and PostCallback cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform joinGroup API
 * @post_condition None
 * @expected joinGroup will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, JoinGroup_USV_N)
{
	EXPECT_ANY_THROW(m_accountManager->joinGroup(INVALID_PARAMETER, &cloudConnectHandlerPostCallback))<<"joinGroup API is to join the group on account server!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'joinGroup' function with empty groupId and valid PostCallback cloudConnectHandler
 * @target OCStackResult joinGroup(const std::string& groupId and PostCallback cloudConnectHandler);
 * @test_data groupId and PostCallback cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform joinGroup API
 * @post_condition None
 * @expected joinGroup will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, JoinGroup_ESV_N)
{
	EXPECT_ANY_THROW(m_accountManager->joinGroup(EMPTY_VALUE, &cloudConnectHandlerPostCallback))<<"joinGroup API is to join the group on account server!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'addDeviceToGroup' function with const std::string& groupId,const std::vector<std::string>& deviceId and PostCallback cloudConnectHandler
 * @target OCStackResult addDeviceToGroup(const std::string& groupId, const std::vector<std::string>& deviceId and PostCallback cloudConnectHandler);
 * @test_data groupId, deviceId and PostCallback cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform addDeviceToGroup API
 * @post_condition None
 * @expected addDeviceToGroup provide OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, AddDeviceToGroup_SRC_P)
{
	EXPECT_EQ(OC_STACK_OK, m_accountManager->addDeviceToGroup(GROUP_ID, m_deviceId, &cloudConnectHandlerPostCallback))<<"addDeviceToGroup API is not to add devices to the group on account server!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'addDeviceToGroup' function with const std::string& groupId,const std::vector<std::string>& deviceId and NULL PostCallback cloudConnectHandler
 * @target OCStackResult addDeviceToGroup(const std::string& groupId, const std::vector<std::string>& deviceId and PostCallback cloudConnectHandler);
 * @test_data groupId, deviceId and PostCallback cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform addDeviceToGroup API
 * @post_condition None
 * @expected addDeviceToGroup will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, AddDeviceToGroup_NV_N)
{
	EXPECT_ANY_THROW(m_accountManager->addDeviceToGroup(GROUP_ID, m_deviceId, nullptr))<<"addDeviceToGroup API is to add devices to the group on account server!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'addDeviceToGroup' function with invalid groupId,valid deviceId and NULL PostCallback cloudConnectHandler
 * @target OCStackResult addDeviceToGroup(const std::string& groupId, const std::vector<std::string>& deviceId and PostCallback cloudConnectHandler);
 * @test_data groupId, deviceId and PostCallback cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform addDeviceToGroup API
 * @post_condition None
 * @expected addDeviceToGroup will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, AddDeviceToGroup_USV_N)
{
	EXPECT_ANY_THROW(m_accountManager->addDeviceToGroup(INVALID_PARAMETER, m_deviceId, &cloudConnectHandlerPostCallback))<<"addDeviceToGroup API is to add devices to the group on account server!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'addDeviceToGroup' function with empty groupId,valid deviceId and NULL PostCallback cloudConnectHandler
 * @target OCStackResult addDeviceToGroup(const std::string& groupId, const std::vector<std::string>& deviceId and PostCallback cloudConnectHandler);
 * @test_data groupId, deviceId and PostCallback cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform addDeviceToGroup API
 * @post_condition None
 * @expected addDeviceToGroup will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, AddDeviceToGroup_ESV_N)
{
	EXPECT_ANY_THROW(m_accountManager->addDeviceToGroup(EMPTY_VALUE, m_deviceId, &cloudConnectHandlerPostCallback))<<"addDeviceToGroup API is to add devices to the group on account server!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'getGroupInfo' function with const std::string& groupId and GetCallback cloudConnectHandler
 * @target OCStackResult getGroupInfo(const std::string& groupId and GetCallback cloudConnectHandler);
 * @test_data groupId and GetCallback cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform getGroupInfo API
 * @post_condition None
 * @expected getGroupInfo provide OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, GetGroupInfo_SRC_P)
{
	EXPECT_EQ(OC_STACK_OK, m_accountManager->getGroupInfo(GROUP_ID, &cloudConnectHandlerGetCallback))<<"getGroupInfo API is not to get information of the group from account server!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'getGroupInfo' function with const std::string& groupId and NULL GetCallback cloudConnectHandler
 * @target OCStackResult getGroupInfo(const std::string& groupId and GetCallback cloudConnectHandler);
 * @test_data groupId and GetCallback cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform getGroupInfo API
 * @post_condition None
 * @expected getGroupInfo will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, GetGroupInfo_NV_N)
{
	EXPECT_ANY_THROW(m_accountManager->getGroupInfo(GROUP_ID, nullptr))<<"getGroupInfo API is to get information of the group from account server!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'getGroupInfo' function with invalid groupId and valid GetCallback cloudConnectHandler
 * @target OCStackResult getGroupInfo(const std::string& groupId and GetCallback cloudConnectHandler);
 * @test_data groupId and GetCallback cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform getGroupInfo API
 * @post_condition None
 * @expected getGroupInfo will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, GetGroupInfo_USV_N)
{
	EXPECT_ANY_THROW(m_accountManager->getGroupInfo(INVALID_PARAMETER, &cloudConnectHandlerGetCallback))<<"getGroupInfo API is to get information of the group from account server!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'getGroupInfo' function with invalid groupId and valid GetCallback cloudConnectHandler
 * @target OCStackResult getGroupInfo(const std::string& groupId and GetCallback cloudConnectHandler);
 * @test_data groupId and GetCallback cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform getGroupInfo API
 * @post_condition None
 * @expected getGroupInfo will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, GetGroupInfo_ESV_N)
{
	EXPECT_ANY_THROW(m_accountManager->getGroupInfo(EMPTY_VALUE, &cloudConnectHandlerGetCallback))<<"getGroupInfo API is to get information of the group from account server!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'leaveGroup' function with const std::string& groupId and DeleteCallback cloudConnectHandler
 * @target OCStackResult leaveGroup(const std::string& groupId and DeleteCallback cloudConnectHandler);
 * @test_data groupId and DeleteCallback cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform leaveGroup API
 * @post_condition None
 * @expected leaveGroup provide OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, LeaveGroup_SRC_P)
{
	EXPECT_EQ(OC_STACK_OK, m_accountManager->leaveGroup(GROUP_ID, &cloudConnectHandlerDeleteCallback))<<"leaveGroup API is not to leave the group joined on account server!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'leaveGroup' function with const std::string& groupId and NULL DeleteCallback cloudConnectHandler
 * @target OCStackResult leaveGroup(const std::string& groupId and DeleteCallback cloudConnectHandler);
 * @test_data groupId and DeleteCallback cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform leaveGroup API
 * @post_condition None
 * @expected leaveGroup will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, LeaveGroup_NV_N)
{
	EXPECT_ANY_THROW(m_accountManager->leaveGroup(GROUP_ID, nullptr))<<"leaveGroup API is to leave the group joined on account server!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'leaveGroup' function with invalid groupId and valid DeleteCallback cloudConnectHandler
 * @target OCStackResult leaveGroup(const std::string& groupId and DeleteCallback cloudConnectHandler);
 * @test_data groupId and DeleteCallback cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform leaveGroup API
 * @post_condition None
 * @expected leaveGroup will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, LeaveGroup_USV_N)
{
	EXPECT_ANY_THROW(m_accountManager->leaveGroup(INVALID_PARAMETER, &cloudConnectHandlerDeleteCallback))<<"leaveGroup API is to leave the group joined on account server!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'leaveGroup' function with empty groupId and valid DeleteCallback cloudConnectHandler
 * @target OCStackResult leaveGroup(const std::string& groupId and DeleteCallback cloudConnectHandler);
 * @test_data groupId and DeleteCallback cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform leaveGroup API
 * @post_condition None
 * @expected leaveGroup will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, LeaveGroup_ESV_N)
{
	EXPECT_ANY_THROW(m_accountManager->leaveGroup(EMPTY_VALUE, &cloudConnectHandlerDeleteCallback))<<"leaveGroup API is to leave the group joined on account server!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'deleteDeviceFromGroup' function with const std::string& groupId,const std::vector<std::string>& deviceId and DeleteCallback cloudConnectHandler
 * @target OCStackResult deleteDeviceFromGroup(const std::string& groupId, const std::vector<std::string>& deviceId and DeleteCallback cloudConnectHandler);
 * @test_data groupId, deviceId and DeleteCallback cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform deleteDeviceFromGroup API
 * @post_condition None
 * @expected deleteDeviceFromGroup provide OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, DeleteDeviceFromGroup_SRC_P)
{
	EXPECT_EQ(OC_STACK_OK, m_accountManager->deleteDeviceFromGroup(GROUP_ID, m_deviceId, &cloudConnectHandlerDeleteCallback))<<"deleteDeviceFromGroup API is not to delete devices from the group on account server!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'deleteDeviceFromGroup' function with valid groupId, deviceId and NULL DeleteCallback cloudConnectHandler
 * @target OCStackResult deleteDeviceFromGroup(const std::string& groupId, const std::vector<std::string>& deviceId and DeleteCallback cloudConnectHandler);
 * @test_data groupId, deviceId and DeleteCallback cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform deleteDeviceFromGroup API
 * @post_condition None
 * @expected deleteDeviceFromGroup will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, DeleteDeviceFromGroup_NV_N)
{
	EXPECT_ANY_THROW(m_accountManager->deleteDeviceFromGroup(GROUP_ID, m_deviceId, nullptr))<<"deleteDeviceFromGroup API is to delete devices from the group on account server!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'deleteDeviceFromGroup' function with invalid groupId, deviceId and valid DeleteCallback cloudConnectHandler
 * @target OCStackResult deleteDeviceFromGroup(const std::string& groupId, const std::vector<std::string>& deviceId and DeleteCallback cloudConnectHandler);
 * @test_data groupId, deviceId and DeleteCallback cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform deleteDeviceFromGroup API
 * @post_condition None
 * @expected deleteDeviceFromGroup will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, DeleteDeviceFromGroup_USV_N)
{
	EXPECT_ANY_THROW(m_accountManager->deleteDeviceFromGroup(INVALID_PARAMETER, m_deviceId, &cloudConnectHandlerDeleteCallback))<<"deleteDeviceFromGroup API is to delete devices from the group on account server!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'deleteDeviceFromGroup' function with empty groupId, deviceId and valid DeleteCallback cloudConnectHandler
 * @target OCStackResult deleteDeviceFromGroup(const std::string& groupId, const std::vector<std::string>& deviceId and DeleteCallback cloudConnectHandler);
 * @test_data groupId, deviceId and DeleteCallback cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform deleteDeviceFromGroup API
 * @post_condition None
 * @expected deleteDeviceFromGroup will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, DeleteDeviceFromGroup_ESV_N)
{
	EXPECT_ANY_THROW(m_accountManager->deleteDeviceFromGroup(EMPTY_VALUE, m_deviceId, &cloudConnectHandlerDeleteCallback))<<"deleteDeviceFromGroup API is to delete devices from the group on account server!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'observeGroup' function with const std::string& groupId and ObserveCallback cloudConnectHandler
 * @target OCStackResult observeGroup(const std::string& groupId and ObserveCallback cloudConnectHandler);
 * @test_data groupId and ObserveCallback cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform observeGroup API
 * @post_condition None
 * @expected observeGroup provide OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, ObserveGroup_SRC_P)
{
	EXPECT_EQ(OC_STACK_OK, m_accountManager->observeGroup(GROUP_ID, &cloudConnectHandlerObserveCallback))<<"observeGroup API is not to register observe to the group on account server!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'observeGroup' function with valid groupId and NULL ObserveCallback cloudConnectHandler
 * @target OCStackResult observeGroup(const std::string& groupId and ObserveCallback cloudConnectHandler);
 * @test_data groupId and ObserveCallback cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform observeGroup API
 * @post_condition None
 * @expected observeGroup will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, ObserveGroup_NV_N)
{
	EXPECT_ANY_THROW(m_accountManager->observeGroup(GROUP_ID, nullptr))<<"observeGroup API is to register observe to the group on account server!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'observeGroup' function with invalid groupId and valid ObserveCallback cloudConnectHandler
 * @target OCStackResult observeGroup(const std::string& groupId and ObserveCallback cloudConnectHandler);
 * @test_data groupId and ObserveCallback cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform observeGroup API
 * @post_condition None
 * @expected observeGroup will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, ObserveGroup_USV_N)
{
	EXPECT_ANY_THROW(m_accountManager->observeGroup(INVALID_PARAMETER, &cloudConnectHandlerObserveCallback))<<"observeGroup API is to register observe to the group on account server!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'observeGroup' function with empty groupId and valid ObserveCallback cloudConnectHandler
 * @target OCStackResult observeGroup(const std::string& groupId and ObserveCallback cloudConnectHandler);
 * @test_data groupId and ObserveCallback cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform observeGroup API
 * @post_condition None
 * @expected observeGroup will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, ObserveGroup_ESV_N)
{
	EXPECT_ANY_THROW(m_accountManager->observeGroup(EMPTY_VALUE, &cloudConnectHandlerObserveCallback))<<"observeGroup API is to register observe to the group on account server!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'cancelObserveGroup' function with const std::string& groupId
 * @target OCStackResult cancelObserveGroup(const std::string& groupId);
 * @test_data groupId
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform cancelObserveGroup API
 * @post_condition None
 * @expected cancelObserveGroup provide OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, CancelObserveGroup_SRC_P)
{
	EXPECT_EQ(OC_STACK_OK, m_accountManager->observeGroup(GROUP_ID, &cloudConnectHandlerObserveCallback))<<"observeGroup API is not to register observe to the group on account server!";
	EXPECT_EQ(OC_STACK_OK, m_accountManager->cancelObserveGroup(GROUP_ID))<<"cancelObserveGroup API is not to cancel observe to the group on account server!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'cancelObserveGroup' function with invalid groupId
 * @target OCStackResult cancelObserveGroup(const std::string& groupId);
 * @test_data groupId
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform cancelObserveGroup API
 * @post_condition None
 * @expected cancelObserveGroup will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, CancelObserveGroup_USV_N)
{
	EXPECT_ANY_THROW(m_accountManager->cancelObserveGroup(INVALID_PARAMETER))<<"cancelObserveGroup API is to cancel observe to the group on account server!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'cancelObserveGroup' function with empty groupId
 * @target OCStackResult cancelObserveGroup(const std::string& groupId);
 * @test_data groupId
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform cancelObserveGroup API
 * @post_condition None
 * @expected cancelObserveGroup will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, CancelObserveGroup_ESV_N)
{
	EXPECT_ANY_THROW(m_accountManager->cancelObserveGroup(EMPTY_VALUE))<<"cancelObserveGroup API is to cancel observe to the group on account server!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'observeInvitation' function with ObserveCallback cloudConnectHandler
 * @target OCStackResult observeInvitation(ObserveCallback cloudConnectHandler);
 * @test_data ObserveCallback cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform observeInvitation API
 * @post_condition None
 * @expected observeInvitation provide OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, ObserveInvitation_SRC_P)
{
	EXPECT_EQ(OC_STACK_OK, m_accountManager->observeInvitation(&cloudConnectHandlerObserveCallback))<<"observeInvitation API is not to register observe to invitation resource on account server!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'observeInvitation' function with NULL ObserveCallback cloudConnectHandler
 * @target OCStackResult observeInvitation(ObserveCallback cloudConnectHandler);
 * @test_data ObserveCallback cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform observeInvitation API
 * @post_condition None
 * @expected observeInvitation will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, ObserveInvitation_NV_N)
{
	EXPECT_ANY_THROW(m_accountManager->observeInvitation(nullptr))<<"observeInvitation API is to register observe to invitation resource on account server!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'cancelObserveInvitation' function positively
 * @target OCStackResult cancelObserveInvitation();
 * @test_data None
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform cancelObserveInvitation API
 * @post_condition None
 * @expected observeInvitation provide OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, CancelObserveInvitation_SRC_P)
{
	EXPECT_EQ(OC_STACK_OK, m_accountManager->observeInvitation(&cloudConnectHandlerObserveCallback))<<"observeInvitation API is not to register observe to invitation resource on account server!";
	EXPECT_EQ(OC_STACK_OK, m_accountManager->cancelObserveInvitation())<<"cancelObserveInvitation API is not to cancel observe to invitation resource on account server!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'sendInvitation' function with const std::string& groupId, const std::string& userUuid and PostCallback cloudConnectHandler
 * @target OCStackResult sendInvitation(const std::string& groupId, const std::string& userUuid, and PostCallback cloudConnectHandler);
 * @test_data groupId, userUuId and PostCallback cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform sendInvitation API
 * @post_condition None
 * @expected sendInvitation provide OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, SendInvitation_SRC_P)
{
	EXPECT_EQ(OC_STACK_OK, m_accountManager->sendInvitation(GROUP_ID, USER_ID, &cloudConnectHandlerPostCallback))<<"sendInvitation API is not to send a invitation to invite a user into a group!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'sendInvitation' function with const std::string& groupId, const std::string& userUuid and NULL PostCallback cloudConnectHandler
 * @target OCStackResult sendInvitation(const std::string& groupId, const std::string& userUuid, and PostCallback cloudConnectHandler);
 * @test_data groupId, userUuId and PostCallback cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform sendInvitation API
 * @post_condition None
 * @expected sendInvitation will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, SendInvitation_NV_N)
{
	EXPECT_ANY_THROW(m_accountManager->sendInvitation(GROUP_ID, USER_ID, nullptr))<<"sendInvitation API is to send a invitation to invite a user into a group!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'sendInvitation' function with invalid groupId, userUuid and valid PostCallback cloudConnectHandler
 * @target OCStackResult sendInvitation(const std::string& groupId, const std::string& userUuid, and PostCallback cloudConnectHandler);
 * @test_data groupId, userUuId and PostCallback cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform sendInvitation API
 * @post_condition None
 * @expected sendInvitation will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, SendInvitation_USV_N)
{
	EXPECT_ANY_THROW(m_accountManager->sendInvitation(INVALID_PARAMETER, INVALID_PARAMETER, &cloudConnectHandlerPostCallback))<<"sendInvitation API is to send a invitation to invite a user into a group!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'sendInvitation' function with empty groupId, userUuid and valid PostCallback cloudConnectHandler
 * @target OCStackResult sendInvitation(const std::string& groupId, const std::string& userUuid, and PostCallback cloudConnectHandler);
 * @test_data groupId, userUuId and PostCallback cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform sendInvitation API
 * @post_condition None
 * @expected sendInvitation will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, SendInvitation_ESV_N)
{
	EXPECT_ANY_THROW(m_accountManager->sendInvitation(EMPTY_VALUE, EMPTY_VALUE, &cloudConnectHandlerPostCallback))<<"sendInvitation API is to send a invitation to invite a user into a group!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'cancelInvitation' function with const std::string& groupId, const std::string& userUuid and DeleteCallback cloudConnectHandler
 * @target OCStackResult cancelInvitation(const std::string& groupId, const std::string& userUuid, and DeleteCallback cloudConnectHandler);
 * @test_data groupId, userUuId and PostCallback cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform cancelInvitation API
 * @post_condition None
 * @expected cancelInvitation provide OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, CancelInvitation_SRC_P)
{
	EXPECT_EQ(OC_STACK_OK, m_accountManager->cancelInvitation(GROUP_ID, USER_ID, &cloudConnectHandlerDeleteCallback))<<"cancelInvitation API is not to delete a invitation on account server that user has received!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'cancelInvitation' function with valid groupId, userUuid and NULL DeleteCallback cloudConnectHandler
 * @target OCStackResult cancelInvitation(const std::string& groupId, const std::string& userUuid, and DeleteCallback cloudConnectHandler);
 * @test_data groupId, userUuId and PostCallback cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform cancelInvitation API
 * @post_condition None
 * @expected cancelInvitation will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, CancelInvitation_NV_N)
{
	EXPECT_ANY_THROW(m_accountManager->cancelInvitation(GROUP_ID, USER_ID, nullptr))<<"cancelInvitation API is to delete a invitation on account server that user has received!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'cancelInvitation' function with invalid groupId, userUuid and valid DeleteCallback cloudConnectHandler
 * @target OCStackResult cancelInvitation(const std::string& groupId, const std::string& userUuid, and DeleteCallback cloudConnectHandler);
 * @test_data groupId, userUuId and PostCallback cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform cancelInvitation API
 * @post_condition None
 * @expected cancelInvitation will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, CancelInvitation_USV_N)
{
	EXPECT_ANY_THROW(m_accountManager->cancelInvitation(INVALID_PARAMETER, INVALID_PARAMETER, &cloudConnectHandlerDeleteCallback))<<"cancelInvitation API is to delete a invitation on account server that user has received!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'cancelInvitation' function with empty groupId, userUuid and valid DeleteCallback cloudConnectHandler
 * @target OCStackResult cancelInvitation(const std::string& groupId, const std::string& userUuid, and DeleteCallback cloudConnectHandler);
 * @test_data groupId, userUuId and PostCallback cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform cancelInvitation API
 * @post_condition None
 * @expected cancelInvitation will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, CancelInvitation_ESV_N)
{
	EXPECT_ANY_THROW(m_accountManager->cancelInvitation(EMPTY_VALUE, EMPTY_VALUE, &cloudConnectHandlerDeleteCallback))<<"cancelInvitation API is to delete a invitation on account server that user has received!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'deleteInvitation' function with const std::string& groupId and DeleteCallback cloudConnectHandler
 * @target OCStackResult deleteInvitation(const std::string& groupId, DeleteCallback cloudConnectHandler);
 * @test_data groupId and PostCallback cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform deleteInvitation API
 * @post_condition None
 * @expected deleteInvitation provide OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, DeleteInvitation_SRC_P)
{
	EXPECT_EQ(OC_STACK_OK, m_accountManager->deleteInvitation(GROUP_ID, &cloudConnectHandlerDeleteCallback))<<"deleteInvitation API is not to delete a invitation on account server that user has received!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'deleteInvitation' function with valid groupId and NULL DeleteCallback cloudConnectHandler
 * @target OCStackResult deleteInvitation(const std::string& groupId, DeleteCallback cloudConnectHandler);
 * @test_data groupId and PostCallback cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform deleteInvitation API
 * @post_condition None
 * @expected deleteInvitation will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, DeleteInvitation_NV_N)
{
	EXPECT_ANY_THROW(m_accountManager->deleteInvitation(GROUP_ID, nullptr))<<"deleteInvitation API is to delete a invitation on account server that user has received!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'deleteInvitation' function with invalid groupId and valid DeleteCallback cloudConnectHandler
 * @target OCStackResult deleteInvitation(const std::string& groupId, DeleteCallback cloudConnectHandler);
 * @test_data groupId and PostCallback cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform deleteInvitation API
 * @post_condition None
 * @expected deleteInvitation will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, DeleteInvitation_USV_N)
{
	EXPECT_ANY_THROW(m_accountManager->deleteInvitation(INVALID_PARAMETER, &cloudConnectHandlerDeleteCallback))<<"deleteInvitation API is to delete a invitation on account server that user has received!";
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'deleteInvitation' function with empty groupId and valid DeleteCallback cloudConnectHandler
 * @target OCStackResult deleteInvitation(const std::string& groupId, DeleteCallback cloudConnectHandler);
 * @test_data groupId and PostCallback cloudConnectHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform deleteInvitation API
 * @post_condition None
 * @expected deleteInvitation will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICCloudConnectorTest_btc, DeleteInvitation_ESV_N)
{
	EXPECT_ANY_THROW(m_accountManager->deleteInvitation(EMPTY_VALUE, &cloudConnectHandlerDeleteCallback))<<"deleteInvitation API is to delete a invitation on account server that user has received!";
}
#endif
