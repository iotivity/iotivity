/* ****************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#include <OCPlatform.h>
#include <OCApi.h>
#include <gtest/gtest.h>

namespace OCAccountManagerTest
{
    using namespace OC;

    // Callbacks
    void accountHandler(const HeaderOptions& /*headerOptions*/, const OCRepresentation& /*rep*/,
            const int /*eCode*/)
    {
    }

    // Helper method
    OCAccountManager::Ptr ConstructAccountManagerObject(std::string host)
    {
        auto ret = OCPlatform::constructAccountManagerObject(host, CT_DEFAULT);

        if (!ret)
        {
            ADD_FAILURE() << "ConstructAccountManagerObject result was null";
            throw std::runtime_error("ConstructAccountManagerObject result was null");
        }

        return ret;
    }

    // Host Test
    TEST(HostTest, Host)
    {
        std::string host("coap://192.168.1.2:5000");
        OCAccountManager::Ptr accountManager = ConstructAccountManagerObject(host);
        EXPECT_TRUE(NULL != accountManager);
        EXPECT_TRUE(accountManager->host() == host);
    }

    // ConnectivityType Test
    TEST(ConnectivityTypeTest, ConnectivityType)
    {
        std::string host("coap://192.168.1.2:5000");
        OCAccountManager::Ptr accountManager = ConstructAccountManagerObject(host);
        EXPECT_TRUE(NULL != accountManager);
        EXPECT_TRUE(accountManager->connectivityType() == CT_DEFAULT);
    }

    // SignUp Test
    TEST(SignUpTest, DISABLED_SignUpWithoutOptionForValid)
    {
        std::string host("coap://192.168.1.2:5000");
        std::string authProvider("AnyAuthProvider");
        std::string authCode("AnyAuthCode");
        OCAccountManager::Ptr accountManager = ConstructAccountManagerObject(host);
        EXPECT_TRUE(NULL != accountManager);
        EXPECT_EQ(OC_STACK_OK, accountManager->signUp(authProvider, authCode, &accountHandler));
    }

    TEST(SignUpTest, DISABLED_SignUpForValid)
    {
        std::string host("coap://192.168.1.2:5000");
        std::string authProvider("AnyAuthProvider");
        std::string authCode("AnyAuthCode");
        QueryParamsMap options = {};
        options.insert(std::pair<std::string, std::string>("AnyOptionKey", "AnyOptionValue"));
        OCAccountManager::Ptr accountManager = ConstructAccountManagerObject(host);
        EXPECT_TRUE(NULL != accountManager);
        EXPECT_EQ(OC_STACK_OK, accountManager->signUp(authProvider, authCode, options,
                                                      &accountHandler));
    }

    TEST(SignUpTest, SignUpWithNullCallback)
    {
        std::string host("coap://192.168.1.2:5000");
        std::string authProvider("AnyAuthProvider");
        std::string authCode("AnyAuthCode");
        OCAccountManager::Ptr accountManager = ConstructAccountManagerObject(host);
        EXPECT_TRUE(NULL != accountManager);
        EXPECT_ANY_THROW(accountManager->signUp(authProvider, authCode, nullptr));
    }

    // SignIn Test
    TEST(SignInTest, DISABLED_SignInForValid)
    {
        std::string host("coap://192.168.1.2:5000");
        std::string userId("AnyUserId");
        std::string accessToken("AnyAccessToken");
        OCAccountManager::Ptr accountManager = ConstructAccountManagerObject(host);
        EXPECT_TRUE(NULL != accountManager);
        EXPECT_EQ(OC_STACK_OK, accountManager->signIn(userId, accessToken, &accountHandler));
    }

    TEST(SignInTest, SignInWithNullCallback)
    {
        std::string host("coap://192.168.1.2:5000");
        std::string userId("AnyUserId");
        std::string accessToken("AnyAccessToken");
        OCAccountManager::Ptr accountManager = ConstructAccountManagerObject(host);
        EXPECT_TRUE(NULL != accountManager);
        EXPECT_ANY_THROW(accountManager->signIn(userId, accessToken, nullptr));
    }

    // SignOut Test
    TEST(SignOutTest, DISABLED_SignOutForValid)
    {
        std::string host("coap://192.168.1.2:5000");
        std::string userId("AnyUserId");
        std::string accessToken("AnyAccessToken");
        OCAccountManager::Ptr accountManager = ConstructAccountManagerObject(host);
        EXPECT_TRUE(NULL != accountManager);
        EXPECT_EQ(OC_STACK_OK, accountManager->signOut(userId, accessToken, &accountHandler));
    }

    TEST(SignOutTest, SignOutWithNullCallback)
    {
        std::string host("coap://192.168.1.2:5000");
        std::string userId("AnyUserId");
        std::string accessToken("AnyAccessToken");
        OCAccountManager::Ptr accountManager = ConstructAccountManagerObject(host);
        EXPECT_TRUE(NULL != accountManager);
        EXPECT_ANY_THROW(accountManager->signOut(userId, accessToken, nullptr));
    }

    // RefreshAccessToken Test
    TEST(RefreshAccessTokenTest, DISABLED_RefreshAccessTokenForValid)
    {
        std::string host("coap://192.168.1.2:5000");
        std::string userId("AnyUserId");
        std::string refreshToken("AnyRefreshToken");
        OCAccountManager::Ptr accountManager = ConstructAccountManagerObject(host);
        EXPECT_TRUE(NULL != accountManager);
        EXPECT_EQ(OC_STACK_OK, accountManager->refreshAccessToken(userId, refreshToken,
                                                                  &accountHandler));
    }

    TEST(RefreshAccessTokenTest, RefreshAccessTokenWithNullCallback)
    {
        std::string host("coap://192.168.1.2:5000");
        std::string userId("AnyUserId");
        std::string refreshToken("AnyRefreshToken");
        OCAccountManager::Ptr accountManager = ConstructAccountManagerObject(host);
        EXPECT_TRUE(NULL != accountManager);
        EXPECT_ANY_THROW(accountManager->refreshAccessToken(userId, refreshToken, nullptr));
    }
}
