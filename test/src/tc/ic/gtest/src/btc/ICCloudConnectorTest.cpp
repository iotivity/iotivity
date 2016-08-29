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

#define HOST "coap://192.168.1.2:5000"
#define AUTH_PROVIDER "AnyAuthProvider"
#define AUTH_CODE "AnyAuthCode"
#define ACCESS_TOKEN "AnyAccessToken"
#define REFRESH_TOKEN "AnyRefreshToken"
#define USER_ID "AnyUserId"
#define DEVICE_ID "AnyDeviceId"

#define INVALID_PARAMETER "Hello"
#define EMPTY_VALUE ""
#define ZERO_VALUE 0

class ICCloudConnectorTest_btc: public ::testing::Test
{

public:
    QueryParamsMap m_queryParams =
    { };

    OCAccountManager::Ptr m_accountManager;

    static void cloudConnectHandler(const HeaderOptions&, const OCRepresentation&, const int)
    {

    }

    static void cloudConnectHandlerDelete(const HeaderOptions&, const int)
    {

    }

protected:
    std::string m_errorMsg;

    virtual void SetUp()
    {
        m_accountManager = OCPlatform::constructAccountManagerObject(HOST, CT_DEFAULT);
    }

    virtual void TearDown()
    {
    }

};

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
    EXPECT_ANY_THROW(m_accountManager->refreshAccessToken(HOST, REFRESH_TOKEN, nullptr))<<"RefreshAccessToken API refresh access token to account server with NULL callback!";
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
