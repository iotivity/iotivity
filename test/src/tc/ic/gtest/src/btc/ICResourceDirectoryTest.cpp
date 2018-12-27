/*******************************************************************
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

class ICResourceDirectoryTest_btc: public ::testing::Test
{
public:
    ResourceHandles m_resourceHandles;
    OCPresenceHandle m_ocPresenceHandle = nullptr;
    QueryParamsList m_queryParams =
    { };
    const std::vector< std::string > m_vecDeviceIdentifier;bool m_isCallbackInvoked;
    OCStackResult m_actualResult;

protected:
    ICHelper* m_ICHelper;

    ICResourceDirectoryTest_btc()
    {
        m_isCallbackInvoked = false;
        m_ICHelper = nullptr;
        m_actualResult = OC_STACK_ERROR;
    }

    virtual void SetUp()
    {
        CommonTestUtil::runCommonTCSetUpPart();
        m_ICHelper = ICHelper::getInstance();
        m_actualResult = OC_STACK_ERROR;
        m_isCallbackInvoked = false;
    }

    virtual void TearDown()
    {
        CommonTestUtil::runCommonTCTearDownPart();
    }

public:
    void onPublish(const OCRepresentation& rep, const int& eCode)
    {
        m_isCallbackInvoked = true;
        IOTIVITYTEST_LOG(INFO, "[ICResourceDirectoryTest_btc] Response from onPublish\n");
    }

    void onDelete(const int& eCode)
    {
        m_isCallbackInvoked = true;
        IOTIVITYTEST_LOG(INFO, "[ICResourceDirectoryTest_btc] Response from onDelete\n");
    }

    void onObserve(const HeaderOptions headerOption, const OCRepresentation&, const int& val1,
            const int& val2)
    {
        m_isCallbackInvoked = true;
        IOTIVITYTEST_LOG(INFO, "[ICResourceDirectoryTest_btc] Response from onObserve\n");
    }
};

/**
 * @since           2016-08-25
 * @see             none
 * @objective       Test 'subscribeDevicePresence' positively to subscribes to a server's device presence change events
 * @target          subscribeDevicePresence(OCPresenceHandle& presenceHandle,const std::string& host,const std::vector<std::string>& di,
 *                                          OCConnectivityType connectivityType,ObserveCallback callback)
 * @test_data       1. presenceHandle       OCPresenceHandle
 *                  2. host                 host address
 *                  3. di                   device Identity
 *                  4. connectivityType     CT_DEFAULT
 *                  5. callback             onObserve as callback
 * @pre_condition   none
 * @procedure       1. Call subscribeDevicePresence API
 *                  2. Check it's return value
 * @post_condition  none
 * @expected        It will subscribes to a server's device presence change events & return OC_STACK_OK
 */

#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICResourceDirectoryTest_btc, SubscribeDevicePresenceWithValidParameters_SRC_FSV_P)
{
    try
    {
        m_actualResult = OCPlatform::subscribeDevicePresence(m_ocPresenceHandle,IC_HOST_ADDRESS, m_vecDeviceIdentifier,IC_CONNECTIVITY_TYPE,
                std::bind(&ICResourceDirectoryTest_btc::onObserve, this,placeholders::_1, placeholders::_2, placeholders::_3, placeholders::_4));
        ASSERT_EQ(OC_STACK_OK,m_actualResult) << "Actual result string : " << CommonUtil::s_OCStackResultString.at(m_actualResult);
    }
    catch(OCException ex)
    {
        FAIL() << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
    }
    SUCCEED();
}
#endif


/**
 * @since           2016-08-25
 * @see             none
 * @objective       Test 'subscribeDevicePresence' negatively with Invalid Host address
 * @target          subscribeDevicePresence(OCPresenceHandle& presenceHandle,const std::string& host,const std::vector<std::string>& di,
 *                                          OCConnectivityType connectivityType,ObserveCallback callback)
 * @test_data       1. presenceHandle       OCPresenceHandle
 *                  2. host                 invalid host address
 *                  3. di                   device Identity
 *                  4. connectivityType     CT_DEFAULT
 *                  5. callback             onObserve as callback
 * @pre_condition   none
 * @procedure       1. Call subscribeDevicePresence API
 *                  2. Check it's return value
 * @post_condition  none
 * @expected        It will not subscribes & will return OC_STACK_INVALID_URI
 */

#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICResourceDirectoryTest_btc, SubscribeDevicePresenceWithInvalidHostAddress_USV_N)
{
    try
    {
        m_actualResult = OCPlatform::subscribeDevicePresence(m_ocPresenceHandle,IC_HOST_ADDRESS_INVALID, m_vecDeviceIdentifier,IC_CONNECTIVITY_TYPE,
                std::bind(&ICResourceDirectoryTest_btc::onObserve, this,placeholders::_1, placeholders::_2, placeholders::_3, placeholders::_4));
        FAIL() << "Actual result string : " << CommonUtil::s_OCStackResultString.at(m_actualResult);
    }
    catch(OCException ex)
    {
        EXPECT_EQ(OC_STACK_INVALID_URI, ex.code()) << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
    }
    SUCCEED();
}
#endif

/**
 * @since           2016-08-25
 * @see             none
 * @objective       Test 'subscribeDevicePresence' negatively with Empty IC_HOST_ADDRESS
 * @target          subscribeDevicePresence(OCPresenceHandle& presenceHandle,const std::string& host,const std::vector<std::string>& di,
 *                                          OCConnectivityType connectivityType,ObserveCallback callback)
 * @test_data       1. presenceHandle       OCPresenceHandle
 *                  2. host                 empty host address
 *                  3. di                   device Identity
 *                  4. connectivityType     CT_DEFAULT
 *                  5. callback             onObserve as callback
 * @pre_condition   none
 * @procedure       1. Call subscribeDevicePresence API
 *                  2. Check it's return value
 * @post_condition  none
 * @expected        It will not subscribes & should return OC_STACK_INVALID_PARAM
 */

#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICResourceDirectoryTest_btc, SubscribeDevicePresenceWithEmptyHost_ESV_N)
{
    try
    {
        m_actualResult = OCPlatform::subscribeDevicePresence(m_ocPresenceHandle,IC_HOST_ADDRESS_EMPTY, m_vecDeviceIdentifier,IC_CONNECTIVITY_TYPE,
                std::bind(&ICResourceDirectoryTest_btc::onObserve, this,placeholders::_1, placeholders::_2, placeholders::_3, placeholders::_4));
        FAIL() << "Actual result string : " << CommonUtil::s_OCStackResultString.at(m_actualResult);
    }
    catch(OCException ex)
    {
        EXPECT_EQ(OC_STACK_INVALID_PARAM, ex.code()) << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
    }
    SUCCEED();
}
#endif

/**
 * @since           2016-08-25
 * @see             none
 * @objective       Test 'subscribeDevicePresence' negatively with null OnObserve
 * @target          subscribeDevicePresence(OCPresenceHandle& presenceHandle,const std::string& host,const std::vector<std::string>& di,
 *                                          OCConnectivityType connectivityType,ObserveCallback callback)
 * @test_data       1. presenceHandle       OCPresenceHandle
 *                  2. host                 host address
 *                  3. di                   device Identity
 *                  4. connectivityType     CT_DEFAULT
 *                  5. callback             null as callback
 * @pre_condition   none
 * @procedure       1. Call subscribeDevicePresence API
 *                  2. Check it's return value
 * @post_condition  none
 * @expected        It will not subscribes & should not return OC_STACK_OK
 */

#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICResourceDirectoryTest_btc, SubscribeDevicePresenceWithNullCallback_NV_N)
{
    try
    {
        m_actualResult = OCPlatform::subscribeDevicePresence(m_ocPresenceHandle,IC_HOST_ADDRESS_EMPTY, m_vecDeviceIdentifier,
                IC_CONNECTIVITY_TYPE,IC_CALLBACK_HANDLER_NULL);
        FAIL() << "Actual result string : " << CommonUtil::s_OCStackResultString.at(m_actualResult);
    }
    catch(OCException ex)
    {
        ASSERT_EQ(OC_STACK_INVALID_PARAM, ex.code()) << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
    }
    SUCCEED();
}
#endif

/**
 * @since           2016-08-25
 * @see             subscribeDevicePresence(OCPresenceHandle& presenceHandle,const std::string& host,const std::vector<std::string>& di,
 *                                          OCConnectivityType connectivityType,ObserveCallback callback)
 * @objective       Test 'unsubscribePresence' with positive value
 * @target          unsubscribePresence(OCPresenceHandle presenceHandle)
 * @test_data       1. presenceHandle       OCPresenceHandle
 * @pre_condition   none
 * @procedure       1. Call subscribeDevicePresence API
 *                  2. Check it's return value
 *                  3. Call unsubscribePresence API
 *                  4. Check it's return value
 * @post_condition  none
 * @expected        It will unsubscribes & should return OC_STACK_OK
 */

#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICResourceDirectoryTest_btc, UnsubscribePresenceWithValidParameters_SRC_FSV_P)
{
    try
    {
        m_actualResult = OCPlatform::subscribeDevicePresence(m_ocPresenceHandle,IC_HOST_ADDRESS, m_vecDeviceIdentifier,IC_CONNECTIVITY_TYPE,
                std::bind(&ICResourceDirectoryTest_btc::onObserve, this,placeholders::_1, placeholders::_2, placeholders::_3, placeholders::_4));
        EXPECT_EQ(OC_STACK_OK,m_actualResult) << "Actual result string : " << CommonUtil::s_OCStackResultString.at(m_actualResult);

        m_actualResult = OC_STACK_ERROR;
        m_actualResult = OCPlatform::unsubscribePresence(m_ocPresenceHandle);
        ASSERT_EQ(OC_STACK_OK,m_actualResult) << "Actual result string : " << CommonUtil::s_OCStackResultString.at(m_actualResult);
    }
    catch(OCException ex)
    {
        FAIL() << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
    }
    SUCCEED();
}
#endif

/**
 * @since           2016-08-25
 * @see             none
 * @objective       Test 'subscribeDevicePresence' negatively with Invalid Host address
 * @target          unsubscribePresence(OCPresenceHandle presenceHandle)
 * @test_data       1. presenceHandle       nullptr as OCPresenceHandle
 * @pre_condition   none
 * @procedure       1. Call subscribeDevicePresence API
 *                  2. Check it's return value
 * @post_condition  none
 * @expected        It will not subscribes & will return OC_STACK_INVALID_PARAM
 */

#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICResourceDirectoryTest_btc, UnSubscribeDevicePresenceWithInvalidOCPresentHandler_USV_N)
{
    try
    {
        m_actualResult = OCPlatform::unsubscribePresence(nullptr);
        FAIL() << "Actual result string : " << CommonUtil::s_OCStackResultString.at(m_actualResult);
    }
    catch(OCException ex)
    {
        ASSERT_EQ(OC_STACK_INVALID_PARAM, ex.code()) << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
    }
    SUCCEED();
}
#endif

/**
 * @since           2017-01-19
 * @see             OCAccountManager::Ptr constructAccountManagerObject(const std::string& host,
                                                            OCConnectivityType connectivityType)
 * @objective       Test 'constructAccountManagerObject' with positive value
 * @target          constructAccountManagerObject(OCPresenceHandle presenceHandle)
 * @test_data       1. presenceHandle       OCPresenceHandle
 * @pre_condition   none
 * @procedure       1. Call constructAccountManagerObject API
 *                  2. Check it's return value
 * @post_condition  none
 * @expected        It will return an object
 */

#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICResourceDirectoryTest_btc, ConstructAccountManagerObjectWithValidParameters_SRC_FSV_P)
{
    try
    {
        ASSERT_NE(nullptr, OCPlatform::constructAccountManagerObject(IC_HOST_ADDRESS,IC_CONNECTIVITY_TYPE));
    }
    catch(OCException ex)
    {
        FAIL() << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
    }
    SUCCEED();
}
#endif

/**
 * @since           2016-08-25
 * @see             none
 * @objective       Test 'constructAccountManagerObject' negatively with Invalid Host address
 * @target          OCAccountManager::Ptr constructAccountManagerObject(const std::string& host,
                                                            OCConnectivityType connectivityType)
 * @test_data       1. presenceHandle       EMPTY_VALUE as host and connectivityType
 * @pre_condition   none
 * @procedure       1. Call constructAccountManagerObject API
 *                  2. Check it's return value
 * @post_condition  none
 * @expected        It will not return OC_STACK_INVALID_PARAM
 */

#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICResourceDirectoryTest_btc, ConstructAccountManagerObjectWithEmptyHost_ESV_N)
{
    try
    {
        OCPlatform::constructAccountManagerObject(EMPTY_VALUE, IC_CONNECTIVITY_TYPE);
        FAIL() << "Actual result string : " << CommonUtil::s_OCStackResultString.at(m_actualResult);
    }
    catch(OCException ex)
    {
        ASSERT_EQ( OC_STACK_ERROR, ex.code()) << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
    }
    SUCCEED();
}
#endif

/**
 * @since           2016-08-25
 * @see             none
 * @objective       Test 'constructAccountManagerObject' negatively with Invalid Host address
 * @target          OCAccountManager::Ptr constructAccountManagerObject(const std::string& host,
                                                            OCConnectivityType connectivityType)
 * @test_data       1. presenceHandle       INVALID_PARAMETER as host and connectivityType
 * @pre_condition   none
 * @procedure       1. Call constructAccountManagerObject API
 *                  2. Check it's return value
 * @post_condition  none
 * @expected        It will not return OC_STACK_INVALID_PARAM
 */

#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICResourceDirectoryTest_btc, ConstructAccountManagerObjectWithInvalidHost_ESV_N)
{
    try
    {
        ICHelper::waitForServerResponse();
        OCPlatform::constructAccountManagerObject(INVALID_PARAMETER, IC_CONNECTIVITY_TYPE);
        FAIL() << "Actual result string : " << CommonUtil::s_OCStackResultString.at(m_actualResult);
    }
    catch(OCException ex)
    {
        ASSERT_EQ(OC_STACK_ERROR, ex.code()) << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
    }
    SUCCEED();
}
#endif
