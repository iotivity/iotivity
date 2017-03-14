/******************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
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

static OCAccountManager::Ptr g_accountMgrControlee = nullptr;

class ICOCAccountManagerTest_stc: public ::testing::Test
{
protected:
    std::string m_errorMsg;
    ICHelper m_ICHelper;
    string m_hostAddress = COAP_HOST_ADDRESS;
    QueryParamsMap m_queryParams;
    string m_key = "gname";
    string m_value = "SRBD";
    string m_update_value = "SRBD_IoTivity";
    vector< string > m_values;
    string temp;
    string temp2;
    OCRepresentation m_propertyValue;
    string m_email = "email";
    string m_phone = "phone";

    virtual void SetUp()
    {
        CommonTestUtil::runCommonTCSetUpPart();
        m_ICHelper.initCloudClient();

        g_accountMgrControlee = OCPlatform::constructAccountManagerObject(m_hostAddress,
                CT_ADAPTER_TCP);
    }

    virtual void TearDown()
    {
    }
};

/**
 * @since 2017-01-27
 * @see OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @objective Test 'Signup' API With valid scenario
 * @target OCStackResult signUp() API
 * @test_data None
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform OCStackResult signUp() API
 * @post_condition None
 * @expected 'Signup' API will provides OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICOCAccountManagerTest_stc, SignUp_SRC_P)
{
    EXPECT_EQ(true, CloudCommonUtil::signUp(g_accountMgrControlee))<<"signUp filed";
}
#endif

/**
 * @since 2017-01-27
 * @see OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @objective Test 'SignIn' and 'SignOut' API With valid scenario
 * @target OCStackResult OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @target OCStackResult signOut(const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @test_data None
 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
 * @procedure Perform OCStackResult signIn() and SignOut() API
 * @post_condition None
 * @expected 'SignIn' and 'SignOut' API will provides OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICOCAccountManagerTest_stc, SignInSignOut_SRC_P)
{
    EXPECT_EQ(true, CloudCommonUtil::signIn(g_accountMgrControlee))<<"signIn filed";
    EXPECT_EQ(true, CloudCommonUtil::signOut(g_accountMgrControlee))<<"SingOut failed.";
}
#endif

/**
 * @since 2017-01-27
 * @see OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler) API
 * @see OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @see OCStackResult signOut(const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @objective Test 'searchUser' API With valid scenario
 * @target OCStackResult searchUser(const QueryParamsMap& queryParams, GetCallback cloudConnectHandler) API
 * @test_data QueryParam
 * @pre_condition constructAccountManagerObject(host, connectivity_type), SignUp(), SignIn() API
 * @procedure Call searchUser with valid name queryParam
 * @post_condition SignOut()
 * @expected 'SearchUser' API will provides OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICOCAccountManagerTest_stc, SearchUserWithQueryParamsMapEmail_SRC_P)
{
    m_queryParams[m_email] = EMAIL;
    ICHelper::s_IsGetSuccessful = false;

    EXPECT_EQ(true, CloudCommonUtil::signIn(g_accountMgrControlee))<<"signIn filed";
    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->searchUser(m_queryParams, ICHelper::cloudConnectGetHandler)) <<"searchUser API is not getting information of the user from account server!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(true, ICHelper::s_IsGetSuccessful);
    EXPECT_EQ(true, CloudCommonUtil::signOut(g_accountMgrControlee))<<"SingOut failed.";
    SUCCEED();

}
#endif

/**
 * @since 2017-01-27
 * @see OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)() API
 * @see OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @see OCStackResult signOut(const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @objective Test 'searchUser' API With valid scenario
 * @target OCStackResult searchUser(const QueryParamsMap& queryParams, GetCallback cloudConnectHandler) API
 * @test_data QueryParam
 * @pre_condition constructAccountManagerObject(host, connectivity_type), SignUp(), SignIn() API
 * @procedure Perform searchUser() with valid name queryParam
 * @post_condition SignOut()
 * @expected 'SearchUser' API will provides OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICOCAccountManagerTest_stc, SearchUserWithQueryParamsMapName_SRC_P)
{
    m_queryParams["name"] = NAME;
    ICHelper::s_IsGetSuccessful = false;

    EXPECT_EQ(true, CloudCommonUtil::signIn(g_accountMgrControlee))<<"signIn filed";
    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->searchUser(m_queryParams, ICHelper::cloudConnectGetHandler)) <<"searchUser API is not getting information of the user from account server!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(true, ICHelper::s_IsGetSuccessful);
    EXPECT_EQ(true, CloudCommonUtil::signOut(g_accountMgrControlee))<<"SingOut failed.";
    SUCCEED();

}
#endif

/**
 * @since 2017-01-27
 * @see OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler) API
 * @see OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @see OCStackResult signOut(const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @objective Test 'searchUser' API With valid scenario
 * @target OCStackResult searchUser(const QueryParamsMap& queryParams, GetCallback cloudConnectHandler) API
 * @test_data QueryParam
 * @pre_condition constructAccountManagerObject(host, connectivity_type), SignUp(), SignIn() API
 * @procedure Perform searchUser() with valid phone queryParam
 * @post_condition SignOut()
 * @expected 'SearchUser' API will provides OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICOCAccountManagerTest_stc, SearchUserWithQueryParamsMapPhone_SRC_P)
{
    m_queryParams[m_phone] = PHONE;
    ICHelper::s_IsGetSuccessful = false;

    EXPECT_EQ(true, CloudCommonUtil::signIn(g_accountMgrControlee))<<"signIn filed";
    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->searchUser(m_queryParams, ICHelper::cloudConnectGetHandler)) <<"searchUser API is not getting information of the user from account server!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(true, ICHelper::s_IsGetSuccessful);
    EXPECT_EQ(true, CloudCommonUtil::signOut(g_accountMgrControlee))<<"SingOut failed.";
    SUCCEED();

}
#endif

/**
 * @since 2017-01-27
 * @see OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler) API
 * @see OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @see OCStackResult signOut(const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @objective Test 'searchUser' API With valid scenario
 * @target OCStackResult searchUser(const QueryParamsMap& queryParams, GetCallback cloudConnectHandler) API
 * @test_data None
 * @pre_condition constructAccountManagerObject(host, connectivity_type), SignUp(), SignIn() API
 * @procedure Perform searchUser() with valid email queryParam
 * @post_condition SignOut()
 * @expected 'SearchUser' API will provides OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICOCAccountManagerTest_stc, SearchUserWithQueryParamsMapEmail_ESV_N)
{
    m_queryParams[m_email] = EMPTY_STRING;
    ICHelper::s_IsGetSuccessful = true;

    EXPECT_EQ(true, CloudCommonUtil::signIn(g_accountMgrControlee))<<"signIn filed";
    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->searchUser(m_queryParams, ICHelper::cloudConnectGetHandler)) <<"searchUser API is not getting information of the user from account server!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(false, ICHelper::s_IsGetSuccessful);
    EXPECT_EQ(true, CloudCommonUtil::signOut(g_accountMgrControlee))<<"SingOut failed.";
    SUCCEED();
}
#endif

/**
 * @since 2017-01-27
 * @see OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler) API
 * @see OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @see OCStackResult signOut(const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @objective Test 'searchUser' API With valid scenario
 * @target OCStackResult searchUser(const QueryParamsMap& queryParams, GetCallback cloudConnectHandler) API
 * @test_data None
 * @pre_condition constructAccountManagerObject(host, connectivity_type), SignUp(), SignIn() API
 * @procedure Perform searchUser() with invalid email queryParam
 * @post_condition SignOut()
 * @expected 'SearchUser' API will provides OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICOCAccountManagerTest_stc, SearchUserWithQueryParamsMapEmail_USV_N)
{
    m_queryParams[m_email] = INVALID;
    ICHelper::s_IsGetSuccessful = true;

    EXPECT_EQ(true, CloudCommonUtil::signIn(g_accountMgrControlee))<<"signIn filed";
    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->searchUser(m_queryParams, ICHelper::cloudConnectGetHandler)) <<"searchUser API is not getting information of the user from account server!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(false, ICHelper::s_IsGetSuccessful);
    EXPECT_EQ(true, CloudCommonUtil::signOut(g_accountMgrControlee))<<"SingOut failed.";
    SUCCEED();
}
#endif

/**
 * @since 2017-01-31
 * @see OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler) API
 * @see OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @see OCStackResult signOut(const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @objective Test 'createGroup' API With valid scenario
 * @target OCStackResult createGroup(PostCallback cloudConnectHandler); API
 * @test_data cloudConnectGetHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type), SignUp(), SignIn() API
 * @procedure Perform OCStackResult createGroup(PostCallback cloudConnectHandler) API
 * @post_condition SignOut()
 * @expected 'createGroup' API will provides OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICOCAccountManagerTest_stc, CreateAndGetGroup_SRC_P)
{
    EXPECT_EQ(true, CloudCommonUtil::signIn(g_accountMgrControlee))<<"signIn filed";

    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->createGroup(ICHelper::cloudConnectGetHandler)) <<"createGroup API not working!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(true, ICHelper::s_IsGetSuccessful)<<"CreateGroup API is not creating a group on account server!! ";
    ICHelper::s_IsGetSuccessful = false;
    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->getGroupInfoAll(ICHelper::cloudConnectGetHandler))<<"GetGroupList API is getting a list of groups joined from account server!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(true, ICHelper::s_IsGetSuccessful)<<"getGroupList API is not getting a list of groups joined from account server! ";
    EXPECT_EQ(true, CloudCommonUtil::signOut(g_accountMgrControlee))<<"SingOut failed.";

    SUCCEED();

}
#endif

/**
 * @since 2017-01-31
 * @see OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler) API
 * @see OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @see OCStackResult signOut(const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @objective Test 'createGroup' API With valid scenario and QueryParams
 * @target OCStackResult createGroup(PostCallback cloudConnectHandler); API
 * @test_data cloudConnectGetHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type), SignUp(), SignIn() API
 * @procedure Perform OCStackResult createGroup(PostCallback cloudConnectHandler) API with QueryParams
 * @post_condition SignOut()
 * @expected 'createGroup' API will provides OC_STACK_OK and return exactly same value which set before.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICOCAccountManagerTest_stc, CreateAndGetGroupWithQueryParams_SRC_P)
{
    m_queryParams.insert(pair< string, string >(m_key, m_value));
    EXPECT_EQ(true, CloudCommonUtil::signIn(g_accountMgrControlee))<<"signIn filed";
    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->createGroup(m_queryParams, ICHelper::cloudConnectGetHandler)) <<"createGroup API not working!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(true, ICHelper::s_IsGetSuccessful)<<"CreateGroup API is not creating a group on account server!! ";
    ICHelper::s_IsGetSuccessful = false;
    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->getGroupInfoAll(ICHelper::cloudConnectGetHandler))<<"GetGroupList API is getting a list of groups joined from account server!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(true, ICHelper::s_IsGetSuccessful)<<"getGroupList API is not getting a list of groups joined from account server! ";

    ICHelper::s_IsGetSuccessful = false;
    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->getGroupInfo(ICHelper::s_GroupID, ICHelper::cloudConnectGetHandler))<<"getGroupInfo API is not to get information of the group from account server!";
    ICHelper::waitForServerResponse();

    EXPECT_EQ(m_value, ICHelper::s_GName);
    EXPECT_EQ(true, ICHelper::s_IsGetSuccessful)<<"getGroupList API is not getting a list of groups joined from account server! ";

    EXPECT_EQ(true, CloudCommonUtil::signOut(g_accountMgrControlee))<<"SingOut failed.";

    SUCCEED();

}
#endif

/**
 * @since 2017-01-31
 * @see OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler) API
 * @see OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @objective Test 'createGroup' and 'getGroupInfo' API With valid scenario
 * @target OCStackResult createGroup(PostCallback cloudConnectHandler); and getGroupInfo() API
 * @test_data group ID
 * @pre_condition constructAccountManagerObject(host, connectivity_type), SignUp(), SignIn() API
 * @procedure Perform createGroup and getGroupInfo
 * @post_condition SignOut()
 * @expected 'createGroup' and getGroupInfo API will provides OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICOCAccountManagerTest_stc, CreateAndGetGroupInfo_SRC_P)
{
    EXPECT_EQ(true, CloudCommonUtil::signIn(g_accountMgrControlee))<<"signIn filed";

    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->createGroup(ICHelper::cloudConnectGetHandler)) <<"createGroup API not working!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(true, ICHelper::s_IsGetSuccessful)<<"CreateGroup API is not creating a group on account server!! ";
    ICHelper::s_IsGetSuccessful = false;

    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->getGroupInfo(ICHelper::s_GroupID, ICHelper::cloudConnectGetHandler))<<"getGroupInfo API is not to get information of the group from account server!";
    ICHelper::waitForServerResponse();

    EXPECT_NE(EMPTY_STRING, ICHelper::s_GroupID);

    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->getGroupInfoAll(ICHelper::cloudConnectGetHandler))<<"GetGroupList API is getting a list of groups joined from account server!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(true, CloudCommonUtil::signOut(g_accountMgrControlee))<<"SingOut failed.";

    SUCCEED();

}
#endif

/**
 * @since 2017-01-31
 * @see OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler) API
 * @see OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @objective Test 'observeGroup' API With valid scenario
 * @target OCStackResult observeGroup(ObserveCallback cloudConnectHandler) API
 * @test_data None
 * @pre_condition constructAccountManagerObject(host, connectivity_type), SignUp(), SignIn(), createGroup API
 * @procedure Perform observeGroup() API
 * @post_condition SignOut()
 * @expected 'observeGroup' API will provides OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICOCAccountManagerTest_stc, ObserveAndGetGroupList_SRC_P)
{
    EXPECT_EQ(true, CloudCommonUtil::signIn(g_accountMgrControlee))<<"signIn filed";

    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->createGroup(ICHelper::cloudConnectGetHandler)) <<"createGroup API not working!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(true, ICHelper::s_IsGetSuccessful)<<"CreateGroup API is not creating a group on account server!! ";
    ICHelper::s_IsGetSuccessful = false;

    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->observeGroup(ICHelper::cloudConnectObserveHandler))<<"ObserveGroup API is not register observe to the group on account server!";
    ICHelper::waitForServerResponse();

    EXPECT_EQ(true, ICHelper::s_IsObserveSuccessful)<<"ObserveGroup API is not register observe to the group on account server! ";

    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->getGroupInfoAll(ICHelper::cloudConnectGetHandler))<<"GetGroupList API is getting a list of groups joined from account server!";
    ICHelper::waitForServerResponse();

    EXPECT_EQ(true, ICHelper::s_IsGetSuccessful)<<"createGroup API is not creating a group on account server! ";
    EXPECT_NE(EMPTY_STRING, ICHelper::s_GroupID);

    EXPECT_EQ(true, CloudCommonUtil::signOut(g_accountMgrControlee))<<"SingOut failed.";
    SUCCEED();

}
#endif

/**
 * @since 2017-01-31
 * @see OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler) API
 * @see OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @objective Test 'cancelObserveGroup' API With valid scenario
 * @target OCStackResult cancelObserveGroup() API
 * @test_data None
 * @pre_condition constructAccountManagerObject(host, connectivity_type), SignUp(), SignIn(), createGroup API
 * @procedure 1. Perform observeGroup() API
 *            2. Perform cancelObserveGroup() API
 *            3. Check cancelObserveGroup works properly
 * @post_condition SignOut()
 * @expected 'cancelObserveGroup' API will provides OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICOCAccountManagerTest_stc, CancelobserveAndGetGroupList_SRC_P)
{
    EXPECT_EQ(true, CloudCommonUtil::signIn(g_accountMgrControlee))<<"signIn filed";

    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->createGroup(ICHelper::cloudConnectGetHandler)) <<"createGroup API not working!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(true, ICHelper::s_IsGetSuccessful)<<"CreateGroup API is not creating a group on account server!! ";
    ICHelper::s_IsGetSuccessful = false;

    ///observe group
    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->observeGroup(ICHelper::cloudConnectObserveHandler))<<"ObserveGroup API is not register observe to the group on account server!";
    ICHelper::waitForServerResponse();

    EXPECT_EQ(true, ICHelper::s_IsObserveSuccessful)<<"ObserveGroup API is not register observe to the group on account server! ";

    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->getGroupInfoAll(ICHelper::cloudConnectGetHandler))<<"GetGroupList API is getting a list of groups joined from account server!";
    ICHelper::waitForServerResponse();

    EXPECT_EQ(true, ICHelper::s_IsGetSuccessful)<<"createGroup API is not creating a group on account server! ";
    EXPECT_NE(EMPTY_STRING, ICHelper::s_GroupID);

    ////cancel observe group
    ASSERT_EQ(OC_STACK_OK, g_accountMgrControlee->cancelObserveGroup())<<"cancelObserveGroup API is not register observe to the group on account server!";
    ICHelper::waitForServerResponse();

    EXPECT_EQ(true, CloudCommonUtil::signOut(g_accountMgrControlee))<<"SingOut failed.";
    SUCCEED();

}
#endif

/**
 * @since 2017-01-31
 * @see OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler) API
 * @see OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @see OCStackResult signOut(const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @objective Test 'deleteGroup' API With valid scenario
 * @target OCStackResult deleteGroup(const std::string& groupId, DeleteCallback cloudConnectHandler) API
 * @test_data groupID
 * @pre_condition constructAccountManagerObject(host, connectivity_type), SignUp(), SignIn() API
 * @procedure   1.CreateGroup with valid  queryParams
 *              2. Check group is created with given group id
 *              3. Perform deleteGroup() API
 *              4. Check group deleted successfully
 * @post_condition SignOut()
 * @expected 'deleteGroup' API will provides OC_STACK_OK and delete the group
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICOCAccountManagerTest_stc, DeleteGroupWithGroupID_SRC_P)
{
    m_queryParams.insert(pair< string, string >(m_key, m_value));
    EXPECT_EQ(true, CloudCommonUtil::signIn(g_accountMgrControlee))<<"signIn filed";
    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->createGroup(m_queryParams, ICHelper::cloudConnectGetHandler)) <<"createGroup API not working!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(true, ICHelper::s_IsGetSuccessful)<<"CreateGroup API is not creating a group on account server!! ";
    ICHelper::s_IsGetSuccessful = false;

    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->getGroupInfo(ICHelper::s_GroupID, ICHelper::cloudConnectGetHandler))<<"getGroupInfo API is not to get information of the group from account server!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(m_value, ICHelper::s_GName);
    EXPECT_EQ(true, ICHelper::s_IsGetSuccessful)<<"getGroupList API is not getting a list of groups joined from account server! ";

    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->deleteGroup(ICHelper::s_GroupID, ICHelper::onDeleteHandler))<<"deleteGroup API is not to get information of the group from account server!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(true, ICHelper::s_IsDeleteSuccessful)<<"deleteGroup API is not called callback! ";

    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->getGroupInfo(ICHelper::s_GroupID, ICHelper::cloudConnectGetHandler))<<"getGroupInfo API is not to get information of the group from account server!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(false, ICHelper::s_IsGetSuccessful)<<"getGroupInfo API is worked with invalid G_ID! ";

    EXPECT_EQ(true, CloudCommonUtil::signOut(g_accountMgrControlee))<<"SingOut failed.";

    SUCCEED();

}
#endif

/**
 * @since 2017-01-31
 * @see OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler) API
 * @see OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @see OCStackResult signOut(const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @objective Test 'addPropertyValueToGroup' API With valid scenario and QueryParams
 * @target OCStackResult addPropertyValueToGroup(const std::string& groupId,const OCRepresentation propertyValue, PostCallback cloudConnectHandler) API
 * @test_data queryParam
 * @pre_condition constructAccountManagerObject(host, connectivity_type), SignUp(), SignIn(), createGroup() API
 * @procedure   1. CreateGroup with valid  queryParams
 *              2. Check group is created with given group id
 *              3. Call addPropertyValueToGroup API
 *              4. Check property add successfully
 * @post_condition SignOut()
 * @expected 'addPropertyValueToGroup' API will provides OC_STACK_OK and return exactly same value which set before.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICOCAccountManagerTest_stc, AddPropertyValueToGroup_SRC_P)
{
    m_queryParams.insert(pair< string, string >(m_key, m_value));
    m_values.push_back(m_value);
    m_propertyValue.setValue < vector < string >> (m_key, m_values);

    EXPECT_EQ(true, CloudCommonUtil::signIn(g_accountMgrControlee))<<"signIn filed";

    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->createGroup(m_queryParams, ICHelper::cloudConnectGetHandler)) <<"createGroup API not working!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(true, ICHelper::s_IsGetSuccessful)<<"CreateGroup API is not creating a group on account server!! ";
    ICHelper::s_IsGetSuccessful = false;

    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->addPropertyValueToGroup(ICHelper::s_GroupID, m_propertyValue, ICHelper::cloudConnectGetHandler)) <<"createGroup API not working!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(true, ICHelper::s_IsGetSuccessful)<<"CreateGroup API is not creating a group on account server!! ";
    ICHelper::s_IsGetSuccessful = false;

    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->getGroupInfo(ICHelper::s_GroupID, ICHelper::cloudConnectGetHandler))<<"getGroupInfo API is not to get information of the group from account server!";
    ICHelper::waitForServerResponse();

    EXPECT_EQ(m_value, ICHelper::s_GName);
    EXPECT_EQ(true, ICHelper::s_IsGetSuccessful)<<"getGroupList API is not getting a list of groups joined from account server! ";

    EXPECT_EQ(true, CloudCommonUtil::signOut(g_accountMgrControlee))<<"SingOut failed.";

    SUCCEED();

}
#endif

/**
 * @since 2017-01-31
 * @see OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler) API
 * @see OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @see OCStackResult signOut(const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @objective Test 'updatePropertyValueOnGroup' API With valid scenario and QueryParams
 * @target OCStackResult updatePropertyValueOnGroup(const std::string& groupId,const OCRepresentation propertyValue, PostCallback cloudConnectHandler) API
 * @test_data cloudConnectGetHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type), SignUp(), SignIn() API
 * @procedure   1.CreateGroup with valid  queryParams
 *              2. Check group is created with given group id
 *              3. Call addPropertyValueToGroup API
 *              4. Check property add successfully
 *              5. Update property
 *              6. Check property updated
 * @post_condition SignOut()
 * @expected 'updatePropertyValueOnGroup' API will provides OC_STACK_OK and return exactly same value which set for update.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICOCAccountManagerTest_stc, UpdatePropertyValueOnGroup_SRC_P)
{
    m_values.push_back(m_value);
    m_propertyValue.setValue < vector < string >> (m_key, m_values);
    m_queryParams.insert(pair< string, string >(m_key, m_value));

    EXPECT_EQ(true, CloudCommonUtil::signIn(g_accountMgrControlee))<<"signIn filed";

    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->createGroup(m_queryParams, ICHelper::cloudConnectGetHandler)) <<"createGroup API not working!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(true, ICHelper::s_IsGetSuccessful)<<"CreateGroup API is not creating a group on account server!! ";
    ICHelper::s_IsGetSuccessful = false;

    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->addPropertyValueToGroup(ICHelper::s_GroupID, m_propertyValue, ICHelper::cloudConnectGetHandler)) <<"createGroup API not working!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(true, ICHelper::s_IsGetSuccessful)<<"CreateGroup API is not creating a group on account server!! ";
    ICHelper::s_IsGetSuccessful = false;

    m_values.push_back(m_update_value);
    m_propertyValue.setValue < vector < string >> (m_key, m_values);

    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->updatePropertyValueOnGroup(ICHelper::s_GroupID, m_propertyValue, ICHelper::cloudConnectGetHandler)) <<"createGroup API not working!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(true, ICHelper::s_IsGetSuccessful)<<"CreateGroup API is not creating a group on account server!! ";
    ICHelper::s_IsGetSuccessful = false;

    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->getGroupInfo(ICHelper::s_GroupID, ICHelper::cloudConnectGetHandler))<<"getGroupInfo API is not to get information of the group from account server!";
    ICHelper::waitForServerResponse();

    EXPECT_EQ(m_update_value, ICHelper::s_GName);
    EXPECT_EQ(true, ICHelper::s_IsGetSuccessful)<<"getGroupList API is not getting a list of groups joined from account server! ";

    EXPECT_EQ(true, CloudCommonUtil::signOut(g_accountMgrControlee))<<"SingOut failed.";

    SUCCEED();

}
#endif

/**
 * @since 2017-01-31
 * @see OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler) API
 * @see OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @see OCStackResult signOut(const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @objective Test 'deletePropertyValueFromGroup' API With valid scenario and QueryParams
 * @target OCStackResult deletePropertyValueFromGroup(const std::string& groupId,const OCRepresentation propertyValue, PostCallback cloudConnectHandler) API
 * @test_data cloudConnectGetHandler
 * @pre_condition constructAccountManagerObject(host, connectivity_type), SignUp(), SignIn() API
 * @procedure   1. CreateGroup with valid  queryParams
 *              2. Check group is created with given group id
 *              3. Call addPropertyValueToGroup API
 *              4. Check property add successfully
 *              5. delete property
 *              6. Check deletePropertyValueFromGroup API work
 * @post_condition SignOut()
 * @expected 'deletePropertyValueFromGroup' API will provides OC_STACK_OK and delete group successfully
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICOCAccountManagerTest_stc, DeletePropertyValueFromGroup_SRC_P)
{
    m_values.push_back(m_value);
    m_propertyValue.setValue < vector < string >> (m_key, m_values);
    m_queryParams.insert(pair< string, string >(m_key, m_value));

    EXPECT_EQ(true, CloudCommonUtil::signIn(g_accountMgrControlee))<<"signIn filed";

    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->createGroup(m_queryParams, ICHelper::cloudConnectGetHandler)) <<"createGroup API not working!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(true, ICHelper::s_IsGetSuccessful)<<"CreateGroup API is not creating a group on account server!! ";
    ICHelper::s_IsGetSuccessful = false;

    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->addPropertyValueToGroup(ICHelper::s_GroupID, m_propertyValue, ICHelper::cloudConnectGetHandler)) <<"createGroup API not working!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(true, ICHelper::s_IsGetSuccessful)<<"CreateGroup API is not creating a group on account server!! ";
    ICHelper::s_IsGetSuccessful = false;

    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->deletePropertyValueFromGroup(ICHelper::s_GroupID, m_propertyValue, ICHelper::cloudConnectGetHandler)) <<"createGroup API not working!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(true, ICHelper::s_IsGetSuccessful)<<"CreateGroup API is not creating a group on account server!! ";
    ICHelper::s_IsGetSuccessful = false;

    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->getGroupInfo(ICHelper::s_GroupID, ICHelper::cloudConnectGetHandler))<<"getGroupInfo API is not to get information of the group from account server!";
    ICHelper::waitForServerResponse();

    EXPECT_NE(m_value, ICHelper::s_GName);
    EXPECT_EQ(true, ICHelper::s_IsGetSuccessful)<<"getGroupList API is not getting a list of groups joined from account server! ";

    EXPECT_EQ(true, CloudCommonUtil::signOut(g_accountMgrControlee))<<"SingOut failed.";

    SUCCEED();

}
#endif

/**
 * @since 2017-02-01
 * @see OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler) API
 * @see OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @objective Test 'observeGroup and sendInvitation' API With valid scenario
 * @target OCStackResult observeGroup(ObserveCallback cloudConnectHandler)
 * @target OCStackResult sendInvitation(const std::string& groupId, const std::string& userUuid, PostCallback cloudConnectHandler) API
 * @test_data GroupID
 * @pre_condition constructAccountManagerObject(host, connectivity_type), SignUp(), SignIn() API
 * @procedure 1. Perform CreateGroup
 *            2. Perform observeInvitation
 *            3. Perform sendInvitation
 * @post_condition replyToInvitation which help to delete new invitation and SignOut()
 * @expected 'observeGroup and sendInvitation' API will provides OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICOCAccountManagerTest_stc, SendAndObserveInvitation_SRC_P)
{
    EXPECT_EQ(true, CloudCommonUtil::signIn(g_accountMgrControlee))<<"signIn filed";

    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->createGroup(ICHelper::cloudConnectGetHandler)) <<"createGroup API not working!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(true, ICHelper::s_IsGetSuccessful)<<"CreateGroup API is not creating a group on account server!! ";
    ICHelper::s_IsGetSuccessful = false;

    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->observeInvitation(ICHelper::cloudConnectObserveHandler))<<"ObserveGroup API is not register observe to the group on account server!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(true, ICHelper::s_IsObserveSuccessful)<<"observeInvitation API is not register observe to the group on account server! ";
    ICHelper::s_IsObserveSuccessful = false;
    temp= ICHelper::s_GroupID;

    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->sendInvitation(ICHelper::s_GroupID, ICHelper::s_GroupUUID, ICHelper::cloudConnectGetHandler))<<"sendInvitation API is not register observe to the group on account server!";
    ICHelper::waitForServerResponse();

    EXPECT_EQ(true, ICHelper::s_IsGetSuccessful)<<"sendInvitation API is not register observe to the group on account server! ";

    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->replyToInvitation(temp, false, ICHelper::onDeleteHandler))<<"replyToInvitation API is not register observe to the group on account server!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(true, ICHelper::s_IsDeleteSuccessful)<<"sendInvitation API is not register observe to the group on account server! ";
    ICHelper::s_IsDeleteSuccessful = false;

    EXPECT_EQ(true, CloudCommonUtil::signOut(g_accountMgrControlee))<<"SingOut failed.";
    SUCCEED();

}
#endif

/**
 * @since 2017-02-01
 * @see OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler) API
 * @see OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @objective Test 'observeGroup and cancelObserveInvitation' API With valid scenario
 * @target OCStackResult observeInvitation(ObserveCallback cloudConnectHandler)
 * @target OCStackResult cancelObserveInvitation() API
 * @test_data None
 * @pre_condition constructAccountManagerObject(host, connectivity_type), SignUp(), SignIn() API
 * @procedure 1. Perform CreateGroup
 *            2. Perform observeInvitation
 *            3. Perform cancelObserveInvitation
 * @post_condition replyToInvitation which help to delete new invitation and SignOut()
 * @expected 'observeGroup and cancelObserveInvitation' API will provides OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICOCAccountManagerTest_stc, ObserveandCancalobserveInvitation_SRC_P)
{
    EXPECT_EQ(true, CloudCommonUtil::signIn(g_accountMgrControlee))<<"signIn filed";

    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->createGroup(ICHelper::cloudConnectGetHandler)) <<"createGroup API not working!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(true, ICHelper::s_IsGetSuccessful)<<"CreateGroup API is not creating a group on account server!! ";
    ICHelper::s_IsGetSuccessful = false;

    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->observeInvitation(ICHelper::cloudConnectObserveHandler))<<"ObserveGroup API is not register observe to the group on account server!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(true, ICHelper::s_IsObserveSuccessful)<<"observeInvitation API is not register observe to the group on account server! ";
    ICHelper::s_IsObserveSuccessful = false;
    temp= ICHelper::s_GroupID;

    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->cancelObserveInvitation())<<"cancelObserveInvitation API is not register observe to the group on account server!";
    ICHelper::waitForServerResponse();

    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->replyToInvitation(temp, false, ICHelper::onDeleteHandler))<<"replyToInvitation API is not register observe to the group on account server!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(true, ICHelper::s_IsDeleteSuccessful)<<"sendInvitation API is not register observe to the group on account server! ";
    ICHelper::s_IsDeleteSuccessful = false;

    EXPECT_EQ(true, CloudCommonUtil::signOut(g_accountMgrControlee))<<"SingOut failed.";
    SUCCEED();

}
#endif

/**
 * @since 2017-02-01
 * @see OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler) API
 * @see OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @objective Test 'sendInvitation and cancelInvitation' API With valid scenario
 * @target OCStackResult sendInvitation(const std::string& groupId, const std::string& userUuid, PostCallback cloudConnectHandler)
 * @target OCStackResult cancelInvitation(const std::string& groupId, const std::string& userUuid, DeleteCallback cloudConnectHandler) API
 * @test_data None
 * @pre_condition constructAccountManagerObject(host, connectivity_type), SignUp(), SignIn() API
 * @procedure 1. Perform CreateGroup
 *            2. Perform observeInvitation
 *            3. Perform sendInvitation
 *            4. Perform  cancelInvitation() API
 * @post_condition replyToInvitation which help to delete new invitation and SignOut()
 * @expected 'sendInvitation and cancelInvitation' API will provides OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICOCAccountManagerTest_stc, SendAndCancelInvitation_SRC_P)
{
    EXPECT_EQ(true, CloudCommonUtil::signIn(g_accountMgrControlee))<<"signIn filed";

    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->createGroup(ICHelper::cloudConnectGetHandler)) <<"createGroup API not working!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(true, ICHelper::s_IsGetSuccessful)<<"CreateGroup API is not creating a group on account server!! ";
    ICHelper::s_IsGetSuccessful = false;

    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->observeInvitation(ICHelper::cloudConnectObserveHandler))<<"ObserveGroup API is not register observe to the group on account server!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(true, ICHelper::s_IsObserveSuccessful)<<"observeInvitation API is not register observe to the group on account server! ";
    ICHelper::s_IsObserveSuccessful = false;

    temp= ICHelper::s_GroupID;
    temp2 = ICHelper::s_GroupUUID;

    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->sendInvitation(ICHelper::s_GroupID, ICHelper::s_GroupUUID, ICHelper::cloudConnectGetHandler))<<"sendInvitation API is not register observe to the group on account server!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(true, ICHelper::s_IsGetSuccessful)<<"sendInvitation API is not register observe to the group on account server! ";
    ICHelper::s_IsGetSuccessful = false;

    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->cancelInvitation(temp, temp2, ICHelper::onDeleteHandler))<<"onDeleteHandler API is not register observe to the group on account server!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(true, ICHelper::s_IsDeleteSuccessful)<<"sendInvitation API is not register observe to the group on account server! ";
    ICHelper::s_IsDeleteSuccessful = false;

    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->replyToInvitation(temp, false, ICHelper::onDeleteHandler))<<"replyToInvitation API is not register observe to the group on account server!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(true, ICHelper::s_IsDeleteSuccessful)<<"sendInvitation API is not register observe to the group on account server! ";
    ICHelper::s_IsDeleteSuccessful = false;

    EXPECT_EQ(true, CloudCommonUtil::signOut(g_accountMgrControlee))<<"SingOut failed.";
    SUCCEED();

}
#endif

/**
 * @since 2017-02-01
 * @see OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler) API
 * @see OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @objective Test 'replyToInvitation' API With valid scenario
 * @target OCStackResult replyToInvitation(const std::string& groupId, const bool accept, DeleteCallback cloudConnectHandler) API
 * @test_data None
 * @pre_condition constructAccountManagerObject(host, connectivity_type), SignUp(), SignIn() API
 * @procedure 1. Perform CreateGroup
 *            2. Perform observeInvitation
 *            3. Perform sendInvitation
 *            4. Perform replyToInvitation() API
 * @post_condition SignOut()
 * @expected 'replyToInvitation' API will provides OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICOCAccountManagerTest_stc, ReplyToInvitation_SRC_P)
{
    EXPECT_EQ(true, CloudCommonUtil::signIn(g_accountMgrControlee))<<"signIn filed";

    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->createGroup(ICHelper::cloudConnectGetHandler)) <<"createGroup API not working!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(true, ICHelper::s_IsGetSuccessful)<<"CreateGroup API is not creating a group on account server!! ";
    ICHelper::s_IsGetSuccessful = false;

    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->observeInvitation(ICHelper::cloudConnectObserveHandler))<<"ObserveGroup API is not register observe to the group on account server!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(true, ICHelper::s_IsObserveSuccessful)<<"observeInvitation API is not register observe to the group on account server! ";
    ICHelper::s_IsObserveSuccessful = false;

    temp= ICHelper::s_GroupID;
    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->sendInvitation(ICHelper::s_GroupID, ICHelper::s_GroupUUID, ICHelper::cloudConnectGetHandler))<<"sendInvitation API is not register observe to the group on account server!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(true, ICHelper::s_IsGetSuccessful)<<"sendInvitation API is not register observe to the group on account server! ";
    ICHelper::s_IsGetSuccessful = false;

    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->replyToInvitation(temp, true, ICHelper::onDeleteHandler))<<"replyToInvitation API is not register observe to the group on account server!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(true, ICHelper::s_IsDeleteSuccessful)<<"sendInvitation API is not register observe to the group on account server! ";
    ICHelper::s_IsDeleteSuccessful = false;

    EXPECT_EQ(true, CloudCommonUtil::signOut(g_accountMgrControlee))<<"SingOut failed.";
    SUCCEED();

}
#endif

/**
 * @since 2017-02-01
 * @see OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler) API
 * @see OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @objective Test 'replyToInvitation' API With valid scenario
 * @target OCStackResult replyToInvitation(const std::string& groupId, const bool accept, DeleteCallback cloudConnectHandler) API
 * @test_data None
 * @pre_condition constructAccountManagerObject(host, connectivity_type), SignUp(), SignIn() API
 * @procedure 1. Perform CreateGroup
 *            2. Perform observeInvitation
 *            3. Perform sendInvitation
 *            4. Perform replyToInvitation() API
 * @post_condition SignOut()
 * @expected 'replyToInvitation' API will provides OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICOCAccountManagerTest_stc, ReplyToInvitationAndRejecttoJoin_SRC_P)
{
    EXPECT_EQ(true, CloudCommonUtil::signIn(g_accountMgrControlee))<<"signIn filed";

    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->createGroup(ICHelper::cloudConnectGetHandler)) <<"createGroup API not working!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(true, ICHelper::s_IsGetSuccessful)<<"CreateGroup API is not creating a group on account server!! ";
    ICHelper::s_IsGetSuccessful = false;

    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->observeInvitation(ICHelper::cloudConnectObserveHandler))<<"ObserveGroup API is not register observe to the group on account server!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(true, ICHelper::s_IsObserveSuccessful)<<"observeInvitation API is not register observe to the group on account server! ";
    ICHelper::s_IsObserveSuccessful = false;

    temp= ICHelper::s_GroupID;
    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->sendInvitation(ICHelper::s_GroupID, ICHelper::s_GroupUUID, ICHelper::cloudConnectGetHandler))<<"sendInvitation API is not register observe to the group on account server!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(true, ICHelper::s_IsGetSuccessful)<<"sendInvitation API is not register observe to the group on account server! ";
    ICHelper::s_IsGetSuccessful = false;

    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->replyToInvitation(temp, false, ICHelper::onDeleteHandler))<<"replyToInvitation API is not register observe to the group on account server!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(true, ICHelper::s_IsDeleteSuccessful)<<"sendInvitation API is not register observe to the group on account server! ";
    ICHelper::s_IsDeleteSuccessful = false;

    EXPECT_EQ(true, CloudCommonUtil::signOut(g_accountMgrControlee))<<"SingOut failed.";
    SUCCEED();

}
#endif

/**
 * @since 2017-02-01
 * @see OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler) API
 * @see OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @objective Test 'deleteDevice' API With valid scenario
 * @target OCStackResult deleteDevice(const std::string& accessToken, const std::string& deviceId, DeleteCallback cloudConnectHandler) API
 * @test_data Uid, access Token
 * @pre_condition constructAccountManagerObject(host, connectivity_type), SignUp(), SignIn() API
 * @procedure 1. Perform CreateGroup
 *            2. Get GroupInfo
 *            3. Perform deleteDevice() API
 *            4. Check device delete successful
 * @post_condition SignOut()
 * @expected 'deleteDevice' API will provides OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICOCAccountManagerTest_stc, DeleteDeviceWithDeviceID_SRC_P)
{
    EXPECT_EQ(true, CloudCommonUtil::signIn(g_accountMgrControlee))<<"signIn filed";

    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->createGroup(ICHelper::cloudConnectGetHandler)) <<"createGroup API not working!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(true, ICHelper::s_IsGetSuccessful)<<"CreateGroup API is not creating a group on account server!! ";

    ICHelper::s_IsGetSuccessful = false;
    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->getGroupInfoAll(ICHelper::cloudConnectGetHandler))<<"GetGroupList API is getting a list of groups joined from account server!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(true, ICHelper::s_IsGetSuccessful)<<"getGroupList API is not getting a list of groups joined from account server! ";

    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->deleteDevice(CloudCommonUtil::g_accessToken, CloudCommonUtil::g_uid, ICHelper::onDeleteHandler))<<"replyToInvitation API is not register observe to the group on account server!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(true, ICHelper::s_IsDeleteSuccessful)<<"sendInvitation API is not register observe to the group on account server! ";
    ICHelper::s_IsDeleteSuccessful = false;

    EXPECT_EQ(OC_STACK_OK, g_accountMgrControlee->getGroupInfoAll(ICHelper::cloudConnectGetHandler))<<"GetGroupList API is getting a list of groups joined from account server!";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(false, ICHelper::s_IsGetSuccessful)<<"getGroupList API is not getting a list of groups joined from account server! ";

    EXPECT_EQ(true, CloudCommonUtil::signOut(g_accountMgrControlee))<<"SingOut failed.";
    SUCCEED();

}
#endif
