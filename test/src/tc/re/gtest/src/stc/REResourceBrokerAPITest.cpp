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

class REResourceBrokerAPITest_stc: public ::testing::Test
{
protected:

    REHelper* m_pREHelper;
    std::string m_errorMsg;
    bool m_isCallbackCalled;
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
        m_isCallbackCalled = false;
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
        m_isCallbackCalled = true;
    }
};

/**
 * @since 2015-11-12
 * @see None
 * @objective Test 'startMonitoring' function for twice
 * @target void startMonitoring(StateChangedCallback cb);
 * @test_data callback function for receiving changed state
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform startMonitoring() API for twice
 * @post_condition None
 * @expected No crash should occur
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceBrokerAPITest_stc, StartMonitoringTwice_VLCC_P)
{
    try
    {
        m_resource->startMonitoring(
                std::bind(&REResourceBrokerAPITest_stc::onStateChangeReceived, this, PH::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);
        m_resource->startMonitoring(
                std::bind(&REResourceBrokerAPITest_stc::onStateChangeReceived, this, PH::_1));

        SET_FAILURE("Successfully startMonitoring in second time.");
    }
    catch (exception& e)
    {
        if (string(e.what()).compare("Monitoring already started.") != 0)
        {
            SET_FAILURE(
                    "Expected exception message is not found. Exception is: " + string(e.what()));
        }

    }
}
#endif

/**
 * @since 2015-11-12
 * @see None
 * @objective Test 'StartMonitoring' function with sequential validation
 * @target void startMonitoring(StateChangedCallback cb);
 * @test_data callback function for receiving changed state
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Perform startMonitoring() API
 *            2. Check the value of isMonitoring()
 * @post_condition None
 * @expected 1. isMonitoring() should return True
 *           2. No crash occurs
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceBrokerAPITest_stc, StartMonitoringWithIsMonitoring_SQV_P)
{
    try
    {
        m_resource->startMonitoring(
                std::bind(&REResourceBrokerAPITest_stc::onStateChangeReceived, this, PH::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);

        if(m_resource->isMonitoring() == false)
        {
            SET_FAILURE("isMonitoring() is false after startMonitoring");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside StartMonitoringWithIsMonitoring_SQV_P: " + string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-13
 * @see void startMonitoring(ResourceStateChangedCallback callback)
 * @objective Test 'stopMonitoring' function with sequential validation
 * @target void stopMonitoring()
 * @test_data callback function for receiving changed state
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Perform startMonitoring() API
 *            2. Perform stopMonitoring() API
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceBrokerAPITest_stc, StopMonitoringAfterStartMonitoring_SQV_P)
{
    try
    {
        m_resource->startMonitoring(
                std::bind(&REResourceBrokerAPITest_stc::onStateChangeReceived, this, PH::_1));

        CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);
        m_resource->stopMonitoring();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside StopMonitoringAfterStartMonitoring_SQV_P: " + string(e.what()));
    }
}
#endif

/**
 * @since 2015-11-12
 * @see void startMonitoring(ResourceStateChangedCallback callback)
 * @objective Test 'stopMonitoring' function with get/set return value
 * @target void stopMonitoring()
 * @test_data callback function for receiving changed state
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Perform startMonitoring() API
 *            2. Perform stopMonitoring() API
 *            3. Check the monitoring status with isMonitoring()
 * @post_condition None
 * @expected 1. isMonitoring() should return false after stopMonitoring()
 *           2. No crash occurs
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceBrokerAPITest_stc, StopMonitoringWithIsMonitoring_GSRV_P)
{
    try
    {
        m_resource->startMonitoring(
                std::bind(&REResourceBrokerAPITest_stc::onStateChangeReceived, this, PH::_1));

        CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);
        m_resource->stopMonitoring();

        if (m_resource->isMonitoring() == true)
        {
            SET_FAILURE("isMonitoring() returns true after stopMonitoring.");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside StopMonitoringWithIsMonitoring_P: " + string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-13
 * @see void startMonitoring(ResourceStateChangedCallback callback)
 * @objective Test 'stopMonitoring' function with Initial/Terminate Loop check
 * @target void stopMonitoring()
 * @test_data callback function for receiving changed state
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Perform startMonitoring() API
 *            2. Perform stopMonitoring() API for twice
 * @post_condition None
 * @expected No crash occurs
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceBrokerAPITest_stc, StopMonitoringTwice_ITLC_P)
{
    try
    {
        m_resource->startMonitoring(
                std::bind(&REResourceBrokerAPITest_stc::onStateChangeReceived, this, PH::_1));

        CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);
        m_resource->stopMonitoring();

        m_resource->stopMonitoring();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside StoptMonitoringTwice_P : " + string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-14
 * @see None
 * @objective Test 'getState' function with sequential validation
 * @target ResourceState getState()
 * @test_data None
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform getState() API
 * @post_condition None
 * @expected 1. No crash occurs
 *           2. Should get ResourceState = NONE
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceBrokerAPITest_stc, GetState_SQV_P)
{
    ResourceState resourceState = m_resource->getState();

    if (resourceState != OIC::Service::ResourceState::NONE)
    {
        SET_FAILURE("Expected ResourceState: NONE, actual: " + string(m_pREHelper->getResourceState(resourceState)));
    }
}
#endif

/**
 * @since 2015-11-11
 * @see None
 * @objective Test 'getState' function with sequential validation
 * @target ResourceState getState()
 * @test_data None
 * @pre_condition 1. Remote Resource Object should be instantialized
 *                2. Perform startMonitoring() API
 * @procedure Perform getState() API
 * @post_condition None
 * @expected 1. No crash occurs
 *           2. Should get ResourceState = REQUESTED
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceBrokerAPITest_stc, GetStateAfterStartMonitoring_SQV_P)
{
    try
    {
        m_resource->startMonitoring(
                std::bind(&REResourceBrokerAPITest_stc::onStateChangeReceived, this, PH::_1));

        ResourceState resourceState = m_resource->getState();

        if (resourceState != OIC::Service::ResourceState::REQUESTED)
        {
            SET_FAILURE("Expected ResourceState: REQUESTED, actual: " + string(m_pREHelper->getResourceState(resourceState)));
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside GetStateAfterStartMonitoring_GSRV_P : " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-02
 * @see None
 * @objective Test 'getState' function with state transition condition check
 * @target ResourceState getState()
 * @test_data None
 * @pre_condition 1. Remote Resource Object should be instantialized
 *                2. Perform startMonitoring() API             
 * @procedure 1. Perform getState() API 
 *            2. wait
 *            3. Again perform getState() API
 * @post_condition Perform stopMonitoring() API
 * @expected 1. No crash occurs
 *           2. Should get ResourceState = ALIVE
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceBrokerAPITest_stc, GetState_STCC_P)
{
    ResourceState resourceState;

    try
    {

        m_resource->startMonitoring(
                std::bind(&REResourceBrokerAPITest_stc::onStateChangeReceived, this, PH::_1));

        resourceState = m_resource->getState();

        if (resourceState != OIC::Service::ResourceState::REQUESTED)
        {
            SET_FAILURE("Expected ResourceState: REQUESTED, actual: " + string(m_pREHelper->getResourceState(resourceState)));
        }

        CommonUtil::waitInSecond(5);

        resourceState = m_resource->getState();

        if (resourceState != OIC::Service::ResourceState::ALIVE)
        {
            SET_FAILURE("Expected ResourceState: ALIVE, actual: " + string(m_pREHelper->getResourceState(resourceState)));
        }
        m_resource->stopMonitoring();

    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside GetStateAfterStartMonitoring_P : " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-02
 * @see None
 * @objective Test 'isMonitoring' function with positive basic way
 * @target bool isMonitoring()
 * @test_data None
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Perform startMonitoring() API
 *            2. Check the value of isMonitoring()
 *            3. Perform stopMonitoring() API
 *            4. Again Check the value of isMonitoring()
 * @post_condition None
 * @expected 1. isMonitoring() should return True and false one by one
 *           2. No crash occurs
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceBrokerAPITest_stc, IsMonitoring_SCV_P)
{
    bool isMtoring;

    try
    {
        m_resource->startMonitoring(
                std::bind(&REResourceBrokerAPITest_stc::onStateChangeReceived, this, PH::_1));

        isMtoring = m_resource->isMonitoring();

        if(isMtoring == false)
        {
            SET_FAILURE("isMonitoring() is false after startMonitoring");
        }

        m_resource->stopMonitoring();

        isMtoring = m_resource->isMonitoring();

        if(isMtoring == true)
        {
            SET_FAILURE("isMonitoring() is false after startMonitoring");
        }

    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside StartMonitoring_P: " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-02
 * @see None
 * @objective Test 'isMonitoring' function with negative basic way
 * @target bool isMonitoring()
 * @test_data None
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Check the value of isMonitoring()
 *            2. Perform startMonitoring() API
 * 		      3. Check the value of isMonitoring()
 *            4. Perform stopMonitoring() API
 * @post_condition None
 * @expected No crash should occur
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceBrokerAPITest_stc, IsMonitoring_USTC_N)
{
    bool isMtoring;

    try
    {
        isMtoring = m_resource->isMonitoring();

        if(isMtoring == true)
        {
            SET_FAILURE("isMonitoring() is false after startMonitoring");
        }

        m_resource->startMonitoring(
                std::bind(&REResourceBrokerAPITest_stc::onStateChangeReceived, this, PH::_1));

        isMtoring = m_resource->isMonitoring();

        if(isMtoring == false)
        {
            SET_FAILURE("isMonitoring() is false after startMonitoring");
        }
        m_resource->stopMonitoring();

    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside StartMonitoring_P: " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-02
 * @see None
 * @objective Test 'isObservable' function with positive basic way
 * @target bool isObservable()
 * @test_data None
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Perform startMonitoring() API
 *            2. Check the value of isObservable()
 * @post_condition Perform stopMonitoring()
 * @expected 1. isObservable() should return True
 *           2. No crash occurs
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceBrokerAPITest_stc, IsObservable_SQV_P)
{
    try
    {
        m_resource->startMonitoring(
                std::bind(&REResourceBrokerAPITest_stc::onStateChangeReceived, this, PH::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);

        bool isObable = m_resource->isObservable();

        if(isObable == false)
        {
            SET_FAILURE("isObservable() is false after startMonitoring");
        }
        m_resource->stopMonitoring();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside isObservable: " + string(e.what()));
    }

}
#endif

/**
 * @since 2016-03-02
 * @see None
 * @objective Test 'isObservable' function with positive basic way
 * @target bool isObservable()
 * @test_data None
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Perform stopMonitoring() API
 *            2. Perform startMonitoring() API
 *            3. Check the value of isObservable()
 * @post_condition None
 * @expected No crash should occur
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceBrokerAPITest_stc, IsObservable_SCV_P)
{
    bool isObable;

    try
    {
        m_resource->stopMonitoring();

        m_resource->startMonitoring(
                std::bind(&REResourceBrokerAPITest_stc::onStateChangeReceived, this, PH::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);

        isObable = m_resource->isObservable();

        if(isObable == false)
        {
            SET_FAILURE("isObservable() is false after startMonitoring");
        }

    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside isObservable: " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-02
 * @see None
 * @objective Test 'isObservable' function with negative basic way
 * @target isObservable API
 * @test_data None
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Check the value of isMonitoring()
 *            2. Perform startMonitoring()
 *            2. Check the value of isObservable()
 * @post_condition Perform stopMonitoring() API
 * @expected 1. isObservable() should return True
 *           2. No crash occurs
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceBrokerAPITest_stc, IsObservable_USTC_N)
{
    bool isObable;

    try
    {
        isObable = m_resource->isMonitoring();

        if(isObable == true)
        {
            SET_FAILURE("isMonitoring() is false after startMonitoring");
        }

        m_resource->startMonitoring(
                std::bind(&REResourceBrokerAPITest_stc::onStateChangeReceived, this, PH::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);

        isObable = m_resource->isObservable();

        if(isObable == false)
        {
            SET_FAILURE("isObservable() is false !");
        }

        m_resource->stopMonitoring();

    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside isObservable: " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-02
 * @see None
 * @objective Test 'stopMonitoring' function with positive basic way
 * @target void stopMonitoring()
 * @test_data None
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Perform startMonitoring() API
 *            2. Perform stopMonitoring() multiple times
 *            3. Perform startMonitoring() API
 * @post_condition None
 * @expected The API should not crash
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceBrokerAPITest_stc, StopMonitoring_SLCC_P)
{
    try
    {
        m_resource->startMonitoring(
                std::bind(&REResourceBrokerAPITest_stc::onStateChangeReceived, this, PH::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);

        for (int i = 0; i < 10; i++)
        {
            m_resource->stopMonitoring();
        }

    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside StopMonitoring_SLCC_P: " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-02
 * @see None
 * @objective Test 'stopMonitoring' function with negative basic way
 * @target void stopMonitoring()
 * @test_data None
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Perform stopMonitoring() API
 *            2. Perform startMonitoring() API
 * @post_condition None
 * @expected The API should not crash
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceBrokerAPITest_stc, StopMonitoring_USTC_N)
{
    try
    {
        m_resource->stopMonitoring();

        m_resource->startMonitoring(
                std::bind(&REResourceBrokerAPITest_stc::onStateChangeReceived, this, PH::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);

        m_resource->stopMonitoring();

    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside stopMonitoring: " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-02
 * @see None
 * @objective Test 'stopMonitoring' function 
 * @target void stopMonitoring()
 * @test_data None
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Perform stopMonitoring()
 *            2. Perform startMonitoring() API
 * 			  3. Again Perform stopMonitoring()
 * 			  4. Repeat 1~3 multiple times
 * @post_condition None
 * @expected The API should not crash
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceBrokerAPITest_stc, StopMonitoring_STLSEC_N)
{
    try
    {
        for(int i = 0; i < 10; i++)
        {
            m_resource->stopMonitoring();

            m_resource->startMonitoring(
                    std::bind(&REResourceBrokerAPITest_stc::onStateChangeReceived, this, PH::_1));
            CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);

            m_resource->stopMonitoring();
        }

    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside StopMonitoring_STLSEC_N: " + string(e.what()));
    }
}
#endif
