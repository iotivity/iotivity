//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include <string>
#include "gtest/gtest.h"
#include "REHelper.h"

class REResourceBrokerTest_btc: public ::testing::Test
{
protected:

    REHelper* m_pREHelper;
    std::string m_errorMsg;
    shared_ptr< OIC::Service::RCSRemoteResourceObject > m_resource;

    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();

#ifdef __LINUX__
        CommonUtil::launchApp(RE_SERVER_APP);
        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);
#endif
        m_pREHelper = REHelper::getInstance();

        bool isResourceAvailable = m_pREHelper->findPrimitiveResources(RESOURCE_TYPE_LIGHT);
        if (isResourceAvailable)
        {
            m_resource = m_pREHelper->getFoundResourceList().at(0);
        }
        else
        {
            SET_FAILURE("Precondition Failed, No Resource Found!!");
        }
        m_errorMsg = "";
    }

    virtual void TearDown()
    {
#ifdef __LINUX__
        CommonUtil::killApp(RE_SERVER_APP);
        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);
#endif
        CommonUtil::runCommonTCTearDownPart();
    }

public:
    void onStateChangeReceived(ResourceState resourceState)
    {

    }

};

/**
 * @since 2015-06-30
 * @see None
 * @objective Test 'startMonitoring' function with positive basic way
 * @target void startMonitoring(StateChangedCallback cb);
 * @test_data callback function for receiving changed state
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform startMonitoring() API
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceBrokerTest_btc, startMonitoring_SRC_P)
{
    try
    {
        m_resource->startMonitoring(
                std::bind(&REResourceBrokerTest_btc::onStateChangeReceived, this, PH::_1));
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside StartMonitoring_SRC_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-02-22
 * @see None
 * @objective Test 'startMonitoring' function with negative basic way
 * @target void startMonitoring(ResourceStateChangedCallback callback)
 * @test_data NULL
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform startMonitoring() API with NULL Cb
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceBrokerTest_btc, startMonitoring_NV_N)
{
    try
    {
        m_resource->startMonitoring(NULL);
    }
    catch (exception& e)
    {
        string exceptionMsg = string(e.what());

        if (exceptionMsg != "startMonitoring : Callback is NULL")
        {
            SET_FAILURE(
                    "Exception occurred while startMonitoring, exception is: " + exceptionMsg);
        }

    }
}
#endif

/**
 * @since 2015-06-30
 * @see void startMonitoring(StateChangedCallback cb);
 * @objective Test 'StopMonitoring' function with positive basic way
 * @target void stopMonitoring();
 * @test_data callback function for receiving changed state
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Perform startMonitoring() API
 *            2. Perform stopMonitoring() API
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceBrokerTest_btc, stopMonitoring_SRC_P)
{
    try
    {
        m_resource->startMonitoring(
                std::bind(&REResourceBrokerTest_btc::onStateChangeReceived, this, PH::_1));

        CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);
        m_resource->stopMonitoring();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside stopMonitoring_SRC_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-06-30
 * @see None
 * @objective Test 'stopMonitoring' function with negative basic way
 * @target void stopMonitoring()
 * @test_data callback function for receiving changed state
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform stopMonitoring() API without startMonitoring() API
 * @post_condition None
 * @expected No crash should occur
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceBrokerTest_btc, stopMonitoring_ECRC_N)
{
    try
    {
        m_resource->stopMonitoring();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside StopWatching_ECRC_N" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-02-22
 * @see None
 * @objective Test 'startCaching' function with positive basic way
 * @target void startCaching()
 * @test_data None
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform startCaching() API
 * @post_condition None
 * @expected No crash should occur
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceBrokerTest_btc, StartCaching_SRC_P)
{
    try
    {
        m_resource->startCaching();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside StartCatching_SRC_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-02-22
 * @see None
 * @objective Test 'stopCaching' function with positive basic way
 * @target void stopCaching()
 * @test_data None
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform stopCaching() API
 * @post_condition None
 * @expected No crash should occur
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceBrokerTest_btc, StopCaching_SRC_P)
{
    try
    {
        m_resource->stopCaching();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside StopCatching_SRC_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-02-22
 * @see startCaching()
 * @objective Test 'isCaching' function with positive basic way
 * @target bool isCaching() const;
 * @test_data None
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform isCaching() API
 * @post_condition None
 * @expected No crash occurs
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceBrokerTest_btc, IsCaching_SRC_P)
{
    m_resource->startCaching();

    bool isC = m_resource->isCaching();

    if(isC == false)
    {
        SET_FAILURE("Fail to get cache state !");
    }
}
#endif 

/**
 * @since 2016-02-22
 * @see startCaching()
 * @objective Test 'isCaching' function with positive basic way
 * @target bool isCaching() const;
 * @test_data None
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform isCaching() API
 * @post_condition None
 * @expected No crash occurs
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceBrokerTest_btc, IsCaching_SRCC_P)
{

    bool isC = m_resource->isCaching();

    if(isC == true)
    {
        SET_FAILURE("Fail to get cache state!");
    }
}
#endif 

/**
 * @since 2015-07-01
 * @see startMonitoring
 * @objective Test 'GetState' function with positive basic way
 * @target ResourceState getState() const;
 * @test_data None
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform GetState() API
 * @post_condition None
 * @expected No crash should occur
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceBrokerTest_btc, GetState_SRC_P)
{
    ResourceState resourceState = m_resource->getState();

    if ((int) resourceState < 0 || (int) resourceState > 3)
    {
        SET_FAILURE("Failed to get state");
    }

}
#endif

/**
 * @see startMonitoring
 * @objective Test 'GetState' function with positive basic way
 * @target ResourceState getState() const;
 * @test_data None
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform GetState() API
 * @post_condition None
 * @expected No crash should occur
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceBrokerTest_btc, GetState_SRCC_P)
{
    ResourceState resourceState = m_resource->getState();

    if(int(resourceState) != 0)
    {
        SET_FAILURE("Failed to get state");
    }

}
#endif

/**
 * @since 2016-02-19
 * @see startMonitoring()
 * @objective Test 'isMonitoring' function with positive basic way
 * @target bool isMonitoring()
 * @test_data callback function for receiving changed state
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Perform startMonitoring() API
 * 			  2. Perform isMonitoring() API
 * @post_condition Perform stopMonitoring() API
 * @expected The API should not throw any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceBrokerTest_btc, IsMonitoring_SRC_P)
{
    m_resource->startMonitoring(
            std::bind(&REResourceBrokerTest_btc::onStateChangeReceived, this, PH::_1));

    CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);

    bool monitorState = m_resource->isMonitoring();

    if (!monitorState)
    {
        SET_FAILURE("Failed to start monitoring");
    }

    m_resource->stopMonitoring();
}
#endif

/**
 * @since 2016-02-19
 * @see startMonitoring()
 * @objective Test 'isMonitoring' function without calling startMonitoring()
 * @target bool isMonitoring()
 * @test_data None
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform isMonitoring() API
 * @post_condition Perform stopMonitoring() API
 * @expected The API should not throw any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceBrokerTest_btc, IsMonitoring_SRCC_P)
{

    bool monitorState = m_resource->isMonitoring();

    if (monitorState)
    {
        SET_FAILURE("Start monitoring successful, but isMonitoring failed!");
    }

    m_resource->stopMonitoring();
}
#endif

/**
 * @since 2016-02-22
 * @see startObservable()
 * @objective Test 'isObservable' function with positive basic way
 * @target bool isObservable_P()
 * @test_data None
 * @pre_condition 1. Remote Resource Object should be instantialized
 *                2. Perform startMonitoring() API
 * @procedure Perform isObservable() API
 * @post_condition Perform stopMonitoring() API
 * @expected The API should not throw any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceBrokerTest_btc, IsObservable_SRC_P)
{
    m_resource->startMonitoring(
            std::bind(&REResourceBrokerTest_btc::onStateChangeReceived, this, PH::_1));

    CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);

    bool Observable = m_resource->isObservable();

    if (!Observable)
    {
        SET_FAILURE("Failed to start Observable");
    }

    m_resource->stopMonitoring();
}
#endif

/**
 * @since 2016-02-22
 * @see startObservable()
 * @objective Test 'isObservable' function without calling startObservable()
 * @target bool isObservable()
 * @test_data None
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform isObservable() API
 * @post_condition Perform stopMonitoring() API
 * @expected The API should not throw any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceBrokerTest_btc, IsObservable_SRCC_P)
{

    bool Observable = m_resource->isObservable();
    if (Observable == 0)
    {
        SET_FAILURE("Failed to start Observable");
    }

    m_resource->stopMonitoring();
}
#endif
