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

#include "REHelper.h"

static bool m_GetCallbackCheck;
static bool m_SetCallbackCheck;

class REResourceCacheAPITest_stc: public ::testing::Test
{
protected:

    REHelper* m_pREHelper;
    shared_ptr< RCSRemoteResourceObject > m_resource;
    std::string m_errorMsg;
    ResourceState m_resourceState;
    RCSResourceAttributes m_cacheUpdateAttributes;
    RCSResourceAttributes m_remoteResourceAttributes;
    bool m_isResourceStateChanged;
    bool m_isResourceStateUpdated;
    bool m_isResourceStateReceived;

    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();
#ifdef __LINUX__        
        CommonUtil::launchApp(RE_SERVER_APP);
        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);
#endif
        m_isResourceStateChanged = false;
        m_isResourceStateUpdated = false;
        m_isResourceStateReceived = false;
        m_GetCallbackCheck = false;
        m_SetCallbackCheck = false;
        m_cacheUpdateAttributes.clear();
        m_remoteResourceAttributes.clear();

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
    void onResourceStateChanged(ResourceState resourceState)
    {
        m_resourceState = resourceState;
        m_isResourceStateChanged = true;
    }
    void onCacheUpdated(const RCSResourceAttributes resourceAttributes)
    {
        m_cacheUpdateAttributes = resourceAttributes;
        m_isResourceStateUpdated = true;
    }
    void onRemoteAttributesReceived(const RCSResourceAttributes resourceAttributes)
    {
        m_remoteResourceAttributes = resourceAttributes;
        m_isResourceStateReceived = true;
    }

    static void onRROSetCallback(const HeaderOpts& headerOp, const RCSRepresentation& rep,
            int eCode)
    {
        m_SetCallbackCheck = true;
        std::cout << "Set callback is Successfully Invoked!" << endl;
    }

    static void onRROGetCallback(const HeaderOpts& headerOp, const RCSRepresentation& rep,
            int eCode)
    {
        m_GetCallbackCheck = true;
        std::cout << "Get callback is Successfully Invoked!" << endl;
    }
};

/**
 * @since 2015-11-12
 * @see None
 * @objective Test 'startCaching' function with Sequential Validation
 * @target void startCaching(ReportPolicy , CacheUpdatedCallback)
 * @test_data 1. Report Policy
 *            2. callback function for receiving cached attribute
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform startWatching() API
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheAPITest_stc, StartCachingWithIsCaching_SQV_P)
{
    try
    {
        RCSRemoteResourceObject::CacheUpdatedCallback cucb = std::bind(
                &REResourceCacheAPITest_stc::onCacheUpdated, this, PH::_1);
        m_resource->startCaching(cucb);
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        if (m_resource->isCaching() == false)
        {
            SET_FAILURE("isCaching() returns false after startCaching.");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside StartCachingWithIsCaching_SQV_P" + string(e.what()));
    }
}
#endif

/**
 * @since 2015-11-12
 * @see None
 * @objective Test 'startCaching' function with IsCachedAvailable function
 * @target void startCaching(ReportPolicy , CacheUpdatedCallback)
 * @test_data 1. Report Policy
 *            2. callback function for receiving cached attribute
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Perform startCaching() API
 *            2. Check whether cache is available or not
 * @post_condition None
 * @expected 1. isCachedAvailable() should return true.
 *           2. No crash occurs
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheAPITest_stc, StartCachingWithIsCachedAvailable_SQV_P)
{
    try
    {
        RCSRemoteResourceObject::CacheUpdatedCallback cucb = std::bind(
                &REResourceCacheAPITest_stc::onCacheUpdated, this, PH::_1);
        m_resource->startCaching(cucb);
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

        if (m_resource->isCachedAvailable() == false)
        {
            SET_FAILURE("isCachedAvailable() returns false after startCaching.");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE(
                "Exception occurred inside StartCachingWithIsCachedAvailable_SQV_P"
                + string(e.what()));
    }
}
#endif

/**
 * @since 2015-11-12
 * @see None
 * @objective Test 'startCaching' function for twice
 * @target void startCaching(ReportPolicy , CacheUpdatedCallback)
 * @test_data 1. Report Policy
 *            2. callback function for receiving cached attribute
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform startCaching() API for twice
 * @post_condition None
 * @expected No crash occurs
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheAPITest_stc, StartCachingTwice_ECRC_N)
{
    try
    {
        RCSRemoteResourceObject::CacheUpdatedCallback cucb = std::bind(
                &REResourceCacheAPITest_stc::onCacheUpdated, this, PH::_1);
        m_resource->startCaching(cucb);
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

        m_resource->startCaching(cucb);

        SET_FAILURE("Successfully startCaching in second time.");
    }
    catch (exception& e)
    {
        if (string(e.what()).compare("Caching already started.") != 0)
        {
            SET_FAILURE(
                    "Expected exception message is not found. Exception is: " + string(e.what()));
        }
    }
}
#endif

/**
 * @since 2015-07-13
 * @see None
 * @objective Test 'startCaching' function with NULL Callback for error throw checking
 * @target void startCaching(ReportPolicy , CacheUpdatedCallback)
 * @test_data 1. Report Policy
 *            2. Null callback
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform startCaching() API
 * @post_condition None
 * @expected The API should throw exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheAPITest_stc, StartCaching_ETC_N)
{
    try
    {
        m_resource->startCaching(NULL);
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        if (m_isResourceStateChanged == true)
        {
            SET_FAILURE("Callback come");
        }
    }
    catch (exception& e)
    {
        string exceptionMsg = string(e.what());
        if (exceptionMsg.compare("startCaching : Callback is NULL") != 0)
        {
            SET_FAILURE("Exception occurred inside StartCaching_ETC_N: " + exceptionMsg);
        }
    }
}
#endif

/**
 * @since 2015-07-13
 * @see None
 * @objective Test 'stopCaching' function with Sequential Validation
 * @target void stopCaching()
 * @test_data None
 * @pre_condition 1. Remote Resource Object should be instantialized
 *                2. Perform startCaching() API
 * @procedure Perform stopCaching() API
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheAPITest_stc, StopCaching_SQV_P)
{
    try
    {
        RCSRemoteResourceObject::CacheUpdatedCallback cucb = std::bind(
                &REResourceCacheAPITest_stc::onCacheUpdated, this, PH::_1);
        m_resource->startCaching(cucb);
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

        m_resource->stopCaching();
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

        if (m_isResourceStateUpdated == false)
        {
            SET_FAILURE("Callback did not come");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside StopCaching_SQV_P" + string(e.what()));
    }
}
#endif

/**
 * @since 2015-11-12
 * @see None
 * @objective Test 'startCaching' function with State Check Validation
 * @target void stopCaching()
 * @test_data 1. Report Policy
 *            2. callback function for receiving cached attribute
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Perform startCaching() API
 *            2. Perform stopCaching() API
 *            3. Perform isCaching() to check cache state
 * @post_condition None
 * @expected No crash occurs
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheAPITest_stc, StopCachingWithIsCaching_SCV_P)
{
    try
    {
        RCSRemoteResourceObject::CacheUpdatedCallback cucb = std::bind(
                &REResourceCacheAPITest_stc::onCacheUpdated, this, PH::_1);
        m_resource->startCaching(cucb);
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        m_resource->stopCaching();

        if (m_resource->isCaching() == true)
        {
            SET_FAILURE("isCaching() returns true after stopCaching.");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside StopCachingWithIsCaching_P. " + string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-13
 * @see void startCaching()
 * @objective Test 'getCachedAttributes' function with Sequential Validation
 * @target void getCachedAttributes()
 * @test_data None
 * @pre_condition 1. Remote Resource Object should be instantialized
 *                2. Perform startCaching() API
 * @procedure Perform getCachedAttributes() API
 * @post_condition None
 * @expected Returned Resource Attributes is not null
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheAPITest_stc, GetCachedAttributes_SQV_P)
{
    try
    {
        RCSRemoteResourceObject::CacheUpdatedCallback cucb = std::bind(
                &REResourceCacheAPITest_stc::onCacheUpdated, this, PH::_1);
        m_resource->startCaching(cucb);
        CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);

        RCSResourceAttributes resourceAttributes = m_resource->getCachedAttributes();
        if (resourceAttributes.size() == 0)
        {
            SET_FAILURE("getCachedAttributes() API returned NULL resource attributes");
        }
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        if (m_isResourceStateUpdated == false)
        {
            SET_FAILURE("Callback did not come");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside GetCachedAttributes_SQV_P" + string(e.what()));
    }
}
#endif

/**
 * @since 2015-11-12
 * @see None
 * @objective Test 'getCachedAttributes' function without Performing startCaching() API
 * @target void getCachedAttributes()
 * @test_data None
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform getCachedAttributes() API without Performing startCaching() API
 * @post_condition None
 * @expected Should return correct exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheAPITest_stc, GetCachedAttributes_DSCC_N)
{
    try
    {
        RCSResourceAttributes resourceAttributes = m_resource->getCachedAttributes();
        if (resourceAttributes.size() == 0)
        {
            SET_FAILURE("getCachedAttributes() API returned NULL resource attributes");
        }
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        if (m_isResourceStateUpdated == true)
        {
            SET_FAILURE("Callback come when calling getCachedAttributes() without startCaching");
        }
    }
    catch (exception& e)
    {
        if (string(e.what()).compare("Caching not started.") != 0)
        {
            SET_FAILURE(
                    "Expected exception: \"Caching not started.\" But Actual: " + string(e.what()));
        }
    }
}
#endif

/**
 * @since 2015-07-14
 * @see None
 * @objective Test 'getCachedAttribute' function with Sequential Validation
 * @target void getCachedAttribute(string&)
 * @test_data key = "power"
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Perform startCaching() API
 *            2. Perform getCachedAttribute(string&) API
 * @post_condition None
 * @expected returned Attribute is not empty
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheAPITest_stc, GetCachedAttribute_SQV_P)
{
    try
    {
        RCSRemoteResourceObject::CacheUpdatedCallback cucb = std::bind(
                &REResourceCacheAPITest_stc::onCacheUpdated, this, PH::_1);
        m_resource->startCaching(cucb);
        CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);

        string key = "power";
        string value = m_resource->getCachedAttribute(key).toString();
        if (value.compare("") == 0)
        {
            SET_FAILURE("getCachedAttribute(string&) API returned NULL resource attributes");
        }
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        if (m_isResourceStateUpdated == false)
        {
            SET_FAILURE("Callback did not come");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside GetCachedAttribute_SQV_P" + string(e.what()));
    }
}
#endif

/**
 * @since 2015-11-12
 * @see None
 * @objective Test 'getCachedAttribute' function with invalid key
 * @target void getCachedAttribute(string&)
 * @test_data key = "invalid_key"
 * @pre_condition 1. Remote Resource Object should be instantialized
 *                2. Perform startCaching() API
 * @procedure Perform getCachedAttribute(string&) API with invalid key
 * @post_condition None
 * @expected Should return exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheAPITest_stc, GetCachedAttributeWithInvalidKey_EG_N)
{
    try
    {
        RCSRemoteResourceObject::CacheUpdatedCallback cucb = std::bind(
                &REResourceCacheAPITest_stc::onCacheUpdated, this, PH::_1);
        m_resource->startCaching(cucb);
        CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);

        string key = "invalid_key";
        m_resource->getCachedAttribute(key);

        SET_FAILURE("Successfully called getCachedAttribute() with invalid key.");
    }
    catch (exception& e)
    {
        if (string(e.what()).size() == 0)
        {
            SET_FAILURE(
                    "Should got an exception when calling getCachedAttribute() with invalid key.");
        }
    }
}
#endif

/**
 * @since 2015-07-14
 * @see None
 * @objective Test 'getCachedAttribute' function without performing startCaching() API
 * @target void getCachedAttribute(string&)
 * @test_data key = "power"
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform getCachedAttribute(string&) API without performing startCaching() API
 * @post_condition None
 * @expected returned Attribute is empty
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheAPITest_stc, GetCachedAttribute_DSCC_N)
{
    try
    {
        string value = m_resource->getCachedAttribute(DEFAULT_POWER_KEY).toString();
        if (value.compare("") != 0)
        {
            SET_FAILURE("getCachedAttribute(string&) API returned NULL resource attributes");
        }
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        if (m_isResourceStateChanged == true)
        {
            SET_FAILURE("Callback came unexpectedly!!");
        }
    }
    catch (exception& e)
    {
        if (string(e.what()).compare("Caching not started.") != 0)
        {
            SET_FAILURE(
                    "Exception occurred inside GetCachedAttribute_DSCC_N : " + string(e.what()));
        }
    }
}
#endif

/**
 * @since 2016-01-06
 * @see None
 * @objective Test 'getCacheState' function without startCaching() API
 * @target CacheState getCacheState()
 * @test_data None
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform getCacheState() API
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheAPITest_stc, GetCacheState_SCV_P)
{
    try
    {
        CacheState cacheState = m_resource->getCacheState();
        if (cacheState != OIC::Service::CacheState::NONE)
        {
            SET_FAILURE(
                    "Expected CacheState: NONE, actual: "
                    + string(m_pREHelper->getCacheState(cacheState)));
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside GetCacheState_SQV_P" + string(e.what()));
    }
}
#endif

/**
 * @since 2016-01-06
 * @see None
 * @objective Test 'getCacheState' function with Sequential Validation
 * @target CacheState getCacheState()
 * @test_data None
 * @pre_condition 1. Remote Resource Object should be instantialized
 *                2. Perform startCaching() API
 * @procedure Perform getCacheState() API
 * @post_condition None
 * @expected Should return CacheState = UNREADY
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheAPITest_stc, GetCacheStateAfterStartCaching_SQV_P)
{
    try
    {
        m_resource->startCaching();

        CacheState cacheState = m_resource->getCacheState();
        if (cacheState != OIC::Service::CacheState::UNREADY)
        {
            SET_FAILURE(
                    "Expected CacheState: UNREADY, actual: "
                    + string(m_pREHelper->getCacheState(cacheState)));
        }
    }
    catch (exception& e)
    {
        SET_FAILURE(
                "Exception occurred inside GetCacheStateAfterStartCaching_SQV_P"
                + string(e.what()));
    }
}
#endif

/**
 * @since 2016-01-06
 * @see None
 * @objective Test 'getCacheState' function with state check validation
 * @target CacheState getCacheState()
 * @test_data None
 * @pre_condition 1. Remote Resource Object should be instantialized
 *                2. Perform startCaching() API
 *                3. Waited for 5 seconds
 * @procedure Perform getCacheState() API
 * @post_condition None
 * @expected Should return CacheState = READY
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheAPITest_stc, GetCacheStateAfterStartCachingAndWait_SCV_P)
{
    try
    {
        m_resource->startCaching();
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

        CacheState cacheState = m_resource->getCacheState();
        if (cacheState != OIC::Service::CacheState::READY)
        {
            SET_FAILURE(
                    "Expected CacheState: READY, actual: "
                    + string(m_pREHelper->getCacheState(cacheState)));
        }
    }
    catch (exception& e)
    {
        SET_FAILURE(
                "Exception occurred inside GetCacheStateAfterStartCachingAndWait_SCV_P"
                + string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-01
 * @see None
 * @objective Test 'getCacheState' function with State Transition Condition Check
 * @target CacheState getCacheState()
 * @test_data None
 * @pre_condition 1. Remote Resource Object should be instantialized
 *                2. Perform startCaching() API
 *                3. Waited for 5 seconds
 * @procedure Perform getCacheState() API
 * @post_condition None
 * @expected Should return CacheState = READY
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheAPITest_stc, GetCacheState_STCC_P)
{
    CacheState cacheState;
    try
    {
        m_resource->startCaching();

        cacheState = m_resource->getCacheState();
        if (cacheState != OIC::Service::CacheState::UNREADY)
        {
            SET_FAILURE(
                    "Expected CacheState: UNREADY, actual: "
                    + string(m_pREHelper->getCacheState(cacheState)));
        }

        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

        cacheState = m_resource->getCacheState();
        if (cacheState != OIC::Service::CacheState::READY)
        {
            SET_FAILURE(
                    "Expected CacheState: READY, actual: "
                    + string(m_pREHelper->getCacheState(cacheState)));
        }

        m_resource->stopCaching();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside GetCacheState_STCC_P" + string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-01
 * @see None
 * @objective Test 'getCacheState' function with State Loop Transition Check
 * @target CacheState getCacheState()
 * @test_data None
 * @pre_condition Remote Resource Object should be instantialized *
 * @procedure 1. Perform startCaching() API
 *            2. Perform getCacheState() API
 *            3. Waited for 5 seconds
 *            4. Perform getCacheState() API
 *            5. Perform stopCaching() API
 *            6. Repeat 1~5 multiple times
 * @post_condition None
 * @expected No crash occurs
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheAPITest_stc, GetCacheState_SLCC_P)
{
    CacheState cacheState;
    try
    {
        for (int i = 0; i < 10; i++)
        {
            m_resource->startCaching();

            cacheState = m_resource->getCacheState();
            if (cacheState != OIC::Service::CacheState::UNREADY)
            {
                SET_FAILURE(
                        "Expected CacheState: UNREADY, actual: "
                        + string(m_pREHelper->getCacheState(cacheState)));
            }

            CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

            cacheState = m_resource->getCacheState();
            if (cacheState != OIC::Service::CacheState::READY)
            {
                SET_FAILURE(
                        "Expected CacheState: READY, actual: "
                        + string(m_pREHelper->getCacheState(cacheState)));
            }

            m_resource->stopCaching();
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside GetCacheState_SLCC_P" + string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-14
 * @see None
 * @objective Test 'getRemoteAttributes' function with sequential validation
 * @target void getRemoteAttributes(AttributeRecievedCallback)
 * @test_data AttributeRecievedCallback
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform getRemoteAttributes() API
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheAPITest_stc, GetRemoteAttributes_SQV_P)
{
    try
    {
        m_resource->getRemoteAttributes(
                bind(&REResourceCacheAPITest_stc::onRemoteAttributesReceived, this, PH::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        if (m_isResourceStateReceived == false)
        {
            SET_FAILURE("Callback did not come");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside SetAttributeValue_P" + string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-14
 * @see None
 * @objective Test 'getRemoteAttributes' function with null callback
 * @target void getRemoteAttributes(AttributeRecievedCallback)
 * @test_data AttributeRecievedCallback is NULL
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform getRemoteAttributes() API
 * @post_condition None
 * @expected The API should not crash
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheAPITest_stc, GetRemoteAttributes_ETC_N)
{
    try
    {
        m_resource->getRemoteAttributes(NULL);
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        if (m_isResourceStateChanged == true)
        {
            SET_FAILURE("Callback come");
        }
    }
    catch (exception& e)
    {
        string exceptionMsg = string(e.what());
        if (exceptionMsg.compare("getRemoteAttributes : Callback is empty") != 0)
        {
            SET_FAILURE(
                    "Exception occurred inside GetRemoteAttribute_N, exception is : "
                    + exceptionMsg);
        }
    }
}
#endif

/**
 * @since 2015-07-13
 * @see None
 * @objective Test 'setRemoteAttributes' function with sequential validation
 * @target void setRemoteAttributes(ResourceAttributes &)
 * @test_data Resource Attributes
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Create resource Attributes
 *            2. Perform setRemoteAttributes() API
 * @post_condition None
 * @expected Returned CacheState should not be NULL
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheAPITest_stc, SetRemoteAttributes_SQV_P)
{
    try
    {
        RCSResourceAttributes resAttributes;
        string key = DEFAULT_POWER_KEY;
        string value = DEFAULT_POWER_VALUE_ON;
        resAttributes[key] = value;

        m_resource->setRemoteAttributes(resAttributes,
                bind(&REResourceCacheAPITest_stc::onRemoteAttributesReceived, this, PH::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        if (m_isResourceStateReceived == false)
        {
            SET_FAILURE("Callback did not come");
        }

        string receivedValue = m_remoteResourceAttributes[key].toString();
        if (receivedValue.find(value) == string::npos)
        {
            SET_FAILURE("Received Value: " + receivedValue + " ;Actual value: " + value + " .");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside SetAttributeValue_P" + string(e.what()));
    }
}
#endif

/**
 * @since 2015-11-12
 * @see None
 * @objective Test 'setRemoteAttributes' and 'getRemoteAttributes' function with get/set return value checking
 * @target void setRemoteAttributes(ResourceAttributes &)
 * @target void getRemoteAttributes(RemoteAttributesGetCallback)
 * @test_data Resource Attributes
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Create resource Attributes
 *            2. Perform setRemoteAttributes() API
 *            3. Perform getRemoteAttributes() API
 *            4. Check received value
 * @post_condition None
 * @expected Received value should same as set value
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheAPITest_stc, SetAndGetRemoteAttributes_GSRV_P)
{
    try
    {
        RCSResourceAttributes resAttributes;
        string key = "power";
        string value = "on";
        resAttributes[key] = value;
        m_resource->setRemoteAttributes(resAttributes,
                bind(&REResourceCacheAPITest_stc::onRemoteAttributesReceived, this, PH::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        if (m_isResourceStateReceived == false)
        {
            SET_FAILURE("Callback did not come");
        }

        m_resource->getRemoteAttributes(
                bind(&REResourceCacheAPITest_stc::onRemoteAttributesReceived, this, PH::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

        string receivedValue = m_remoteResourceAttributes[key].toString();
        if (receivedValue.find(value) == string::npos)
        {
            SET_FAILURE("Received Value: " + receivedValue + " Actual value: " + value);
        }
    }
    catch (exception& e)
    {
        SET_FAILURE(
                "Exception occurred inside SetRemoteAttributesWithCheckingGetValue_P. "
                + string(e.what()));
    }
}
#endif

/**
 * @since 2015-11-12
 * @see None
 * @objective Test 'setRemoteAttributes' and 'getCachedAttribute' function with sequential validation
 * @target void setRemoteAttributes(ResourceAttributes &)
 * @target void getRemoteAttributes(RemoteAttributesGetCallback)
 * @test_data Resource Attributes
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Perform startCaching() API
 *            2. Create resource Attributes
 *            3. Perform setRemoteAttributes() API
 *            4. Perform getCachedAttribute() API
 *            5. Check received value
 * @post_condition None
 * @expected Received value should same as set value
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheAPITest_stc, SetRemoteAttributesAndGetCashedAttribute_SQV_P)
{
    try
    {
        RCSRemoteResourceObject::CacheUpdatedCallback cucb = std::bind(
                &REResourceCacheAPITest_stc::onCacheUpdated, this, PH::_1);
        m_resource->startCaching(cucb);
        CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);
        RCSResourceAttributes resAttributes;
        string key = "power";
        string value = "off";
        resAttributes[key] = value;
        m_resource->setRemoteAttributes(resAttributes,
                bind(&REResourceCacheAPITest_stc::onRemoteAttributesReceived, this, PH::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        if (m_isResourceStateReceived == false)
        {
            SET_FAILURE("Callback did not come");
        }

        string receivedValue = m_resource->getCachedAttribute(key).toString();

        if (receivedValue.find(value) == string::npos)
        {
            SET_FAILURE("Received Value: " + receivedValue + " Actual value: " + value);
        }
    }
    catch (exception& e)
    {
        SET_FAILURE(
                "Exception occurred inside SetRemoteAttributesWithCheckingGetValue_P. "
                + string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-13
 * @see None
 * @objective Test 'setRemoteAttributes' function with Resource Attributes
 * @target void setRemoteAttributes(ResourceAttributes &)
 * @test_data Null Resource Attributes
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Create resource Attributes
 *            2. Perform setRemoteAttributes() API
 * @post_condition None
 * @expected The API should not crash
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheAPITest_stc, SetRemoteAttributes_NV_N)
{
    try
    {
        RCSResourceAttributes resAttributes;
        m_resource->setRemoteAttributes(resAttributes,
                bind(&REResourceCacheAPITest_stc::onRemoteAttributesReceived, this, PH::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        if (m_isResourceStateChanged == true)
        {
            SET_FAILURE("Callback come");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside SetAttributeValue_P" + string(e.what()));
    }
}
#endif

/**
 * @since 2015-11-12
 * @see None
 * @objective Test 'setRemoteAttributes' function with null callback
 * @target void setRemoteAttributes(ResourceAttributes &)
 * @test_data Null Callback function
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Create resource Attributes
 *            2. Perform setRemoteAttributes() API
 * @post_condition None
 * @expected Should return correct exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheAPITest_stc, SetRemoteAttributesWithNullCallback_N)
{
    try
    {
        RCSResourceAttributes resAttributes;
        m_resource->setRemoteAttributes(resAttributes, NULL);
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        if (m_isResourceStateChanged == true)
        {
            SET_FAILURE("Callback come for null callback for setRemoteAttributes.");
        }
    }
    catch (exception& e)
    {
        if (string(e.what()).compare("setRemoteAttributes : Callback is empty") != 0)
        {
            SET_FAILURE(
                    "Expected exception is \"setRemoteAttributes : Callback is empty\" But actual: "
                    + string(e.what()));
        }
    }
}
#endif

/**
 * @since 2016-02-26
 * @see None
 * @objective Test 'isCaching' function with sequential validation
 * @target isCaching API
 * @test_data None
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Perform startCaching() API
 *            2. Check the value of isCaching()
 * @post_condition None
 * @expected 1. isCaching() should return True
 *           2. No crash occurs
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheAPITest_stc, IsCaching_SQV_P)
{
    bool isCach;
    try
    {
        RCSRemoteResourceObject::CacheUpdatedCallback cucb = std::bind(
                &REResourceCacheAPITest_stc::onCacheUpdated, this, PH::_1);

        m_resource->startCaching(cucb);
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

        isCach = m_resource->isCaching();

        if (isCach == false)
        {
            SET_FAILURE("isCaching() is false after startMonitoring");
        }

        m_resource->stopCaching();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside isCaching: " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-02-26
 * @see None
 * @objective Test 'isCaching' function with state check validation 
 * @target isCaching API
 * @test_data None
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Perform startCaching() API
 *            2. Check the value of isCaching()
 *            3. Perform stopCachingg() API
 *            4. Again Check the value of isCaching()
 * @post_condition None
 * @expected 1. isCaching() should return True and false one by one
 *           2. No crash occurs
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheAPITest_stc, IsCaching_SCV_P)
{
    bool isCaching;

    try
    {
        RCSRemoteResourceObject::CacheUpdatedCallback cucb = std::bind(
                &REResourceCacheAPITest_stc::onCacheUpdated, this, PH::_1);
        m_resource->startCaching(cucb);
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

        isCaching = m_resource->isCaching();

        if (isCaching == false)
        {
            SET_FAILURE("isMonitoring() is false after startMonitoring");
        }

        m_resource->stopCaching();

        isCaching = m_resource->isCaching();

        if (isCaching == true)
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
 * @since 2016-02-26
 * @see None
 * @objective Test 'isCaching' function with state loop condition checking
 * @target isCaching API
 * @test_data None
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Perform startCaching() API
 *            2. Check the value of isCaching() for 10 times
 *            3. Perform stopCaching() API
 *            4. Check the value of isCaching() for 10 times
 * @post_condition None
 * @expected 1. isCaching() should return True and false one by one
 *           2. No crash occurs
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheAPITest_stc, IsCaching_SLCC_P)
{
    bool isCaching;

    try
    {

        RCSRemoteResourceObject::CacheUpdatedCallback cucb = std::bind(
                &REResourceCacheAPITest_stc::onCacheUpdated, this, PH::_1);
        m_resource->startCaching(cucb);
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

        for (int i = 0; i < 10; i++)
        {
            isCaching = m_resource->isCaching();

            if (isCaching == false)
            {
                SET_FAILURE("isMonitoring() is false after startMonitoring");
            }
        }

        m_resource->stopCaching();

        for (int i = 0; i < 10; i++)
        {
            isCaching = m_resource->isCaching();

            if (isCaching == true)
            {
                SET_FAILURE("isMonitoring() is false after startMonitoring");
            }
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside StartMonitoring_P: " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-02-26
 * @see None
 * @objective Test 'isCaching' function with Undefined State Transition Check
 * @target isCaching() API
 * @test_data None
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Check the value of isCaching()
 *            2. Perform startCaching() API
 *            3. Check the value of isCaching()
 *            4. Perform stopCaching() API
 * @post_condition None
 * @expected 1. isCaching() should return True
 *           2. No crash occurs
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheAPITest_stc, IsCaching_USTC_N)
{
    bool isCaching;

    try
    {
        isCaching = m_resource->isCaching();

        if (isCaching == true)
        {
            SET_FAILURE("isMonitoring() is false after startMonitoring");
        }

        RCSRemoteResourceObject::CacheUpdatedCallback cucb = std::bind(
                &REResourceCacheAPITest_stc::onCacheUpdated, this, PH::_1);
        m_resource->startCaching(cucb);
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

        isCaching = m_resource->isCaching();

        if (isCaching == false)
        {
            SET_FAILURE("isMonitoring() is false after startMonitoring");
        }

        m_resource->stopCaching();

    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside IsCaching_USTC_N: " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-02-26
 * @see None
 * @objective Test 'isCaching' function with undefined order multiple times
 * @target isCaching API
 * @test_data None
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Check the value of isCaching()
 *             2. Perform startCaching() API
 *             3. Check the value of isCaching()
 *             4. Perform stopCaching() API
 *             5. Repeat 1~4 multiple times
 * @post_condition None
 * @expected 1. isCaching() should return True
 *           2. No crash occurs
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheAPITest_stc, IsCaching_STLSEC_N)
{
    bool isCaching;

    try
    {
        for (int i = 0; i < 1; i++)
        {
            isCaching = m_resource->isCaching();

            if (isCaching == true)
            {
                SET_FAILURE("isMonitoring() is false after startMonitoring");
            }

            RCSRemoteResourceObject::CacheUpdatedCallback cucb = std::bind(
                    &REResourceCacheAPITest_stc::onCacheUpdated, this, PH::_1);
            m_resource->startCaching(cucb);
            CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

            isCaching = m_resource->isCaching();

            if (isCaching == false)
            {
                SET_FAILURE("isMonitoring() is false after startMonitoring");
            }

            m_resource->stopCaching();
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside IsCaching_STLSEC_N: " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-02-26
 * @see None
 * @objective Test 'isCachedAvailable' function with Sequential Validation
 * @target isCachedAvailable API
 * @test_data None
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Perform startCaching() API
 *            2. Check the value of isCachedAvailable()
 * @post_condition Perform stopCaching() API
 * @expected 1. isCachedAvailable() should return True
 *           2. No crash occurs
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheAPITest_stc, IsCachedAvailable_SQV_P)
{
    bool isCach;
    try
    {
        RCSRemoteResourceObject::CacheUpdatedCallback cucb = std::bind(
                &REResourceCacheAPITest_stc::onCacheUpdated, this, PH::_1);

        m_resource->startCaching(cucb);
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

        isCach = m_resource->isCachedAvailable();

        if (isCach == false)
        {
            SET_FAILURE("isCachedAvailable() is false");
        }

        m_resource->stopCaching();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside isCachedAvailable: " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-02-26
 * @see None
 * @objective Test 'isCachedAvailable' function with State Check Validation
 * @target isCachedAvailable API
 * @test_data None
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Perform isCaching() API
 *            2. Check the value of isCachedAvailable()
 *            3. Perform stopCaching() API
 *            4. Again Check the value of isCachedAvailable()
 * @post_condition None
 * @expected 1. isCachedAvailable() should return True and false one by one
 *           2. No crash occurs
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheAPITest_stc, IsCachedAvailable_SCV_P)
{
    bool isCaching;

    try
    {
        RCSRemoteResourceObject::CacheUpdatedCallback cucb = std::bind(
                &REResourceCacheAPITest_stc::onCacheUpdated, this, PH::_1);
        m_resource->startCaching(cucb);
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

        isCaching = m_resource->isCachedAvailable();

        if (isCaching == false)
        {
            SET_FAILURE("isCachedAvailable() is false");
        }

        m_resource->stopCaching();

        isCaching = m_resource->isCachedAvailable();

        if (isCaching == true)
        {
            SET_FAILURE("isCachedAvailable() is false ");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside isCachedAvailable: " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-01
 * @see None
 * @objective Test 'isCachedAvailable' function with State Loop Condition Checking
 * @target isCachedAvailable API
 * @test_data None
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Perform startCaching() API
 *            2. Check the value of isCachedAvailable() for 10 times
 *            3. Perform stopCaching() API
 *            4. Again Check the value of isCachedAvailable() for 10 times
 * @post_condition None
 * @expected 1. isCachedAvailable() should return True and false one by one
 *           2. No crash occurs
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheAPITest_stc, IsCachedAvailable_SLCC_P)
{
    bool isCaching;

    try
    {

        RCSRemoteResourceObject::CacheUpdatedCallback cucb = std::bind(
                &REResourceCacheAPITest_stc::onCacheUpdated, this, PH::_1);
        m_resource->startCaching(cucb);
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

        for (int i = 0; i < 10; i++)
        {
            isCaching = m_resource->isCachedAvailable();

            if (isCaching == false)
            {
                SET_FAILURE("isCachedAvailable() is false after startMonitoring");
            }
        }

        m_resource->stopCaching();

        for (int i = 0; i < 10; i++)
        {
            isCaching = m_resource->isCachedAvailable();

            if (isCaching == true)
            {
                SET_FAILURE("isCachedAvailable() is false after startMonitoring");
            }
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside isCachedAvailable: " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-11
 * @see None
 * @objective Test 'getResourceInterface' function with Sequential Validation
 * @target std::string getResourceInterface() const
 * @test_data interface = DEFAULT_INTERFACE
 * @pre_condition Perform setResourceInterface(std::string interface) API
 * @procedure Perform getResourceInterface() API
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheAPITest_stc, GetResourceInterface_SQV_P)
{
    try
    {
        RCSQueryParams m_get;
        string interface = DEFAULT_INTERFACE;
        m_get.setResourceInterface(interface);
        string val = m_get.getResourceInterface();
        if (val.compare(DEFAULT_INTERFACE) != 0)
        {
            SET_FAILURE("get and set interface isn't the same!");
        }

    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside GetResourceInterface_SQV_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-07-25
 * @see None
 * @objective Test 'getResourceInterface' function by performing setResourceInterface() API with empty interface
 * @target std::string getResourceInterface() const
 * @test_data interface = ""
 * @pre_condition Perform setResourceInterface(std::string interface) API with empty interface
 * @procedure Perform getResourceInterface() API
 * @post_condition None
 * @expected 1. The API should not generate throw exception
 *           2. Should get empty string for interface
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheAPITest_stc, SetGetResourceInterface_ESV_P)
{
    try
    {
        RCSQueryParams m_get;
        string interface = "";
        m_get.setResourceInterface(interface);
        string value = m_get.getResourceInterface();

        if (value.compare(interface) != 0)
        {
            SET_FAILURE("ResourceInterface is not empty!!");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE(
                "Exception occurred inside SetGetResourceInterface_ESV_P " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-11
 * @see None
 * @objective Test 'getResourceType' function with Sequential Validation
 * @target std::string getResourceType() const
 * @test_data type = RESOURCE_TYPE_LIGHT
 * @pre_condition Perform setResourceType(std::string type) API
 * @procedure Perform getResourceType() API
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheAPITest_stc, GetResourceType_SQV_P)
{
    try
    {
        RCSQueryParams m_get;
        string type = RESOURCE_TYPE_LIGHT;
        m_get.setResourceType(type);
        string val = m_get.getResourceType();
        if (val.compare(RESOURCE_TYPE_LIGHT) != 0)
        {
            SET_FAILURE("get and set resource type isn't the same!");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside GetResourceType_SQV_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-07-25
 * @see None
 * @objective Test 'setResourceType' API with empty resource type and then perform getResourceType() API
 * @target 1. setResourceType(std::string type)
 *         2. std::string getResourceType() const
 * @test_data type = ""
 * @pre_condition None
 * @procedure 1. Perform setResourceType(std::string type) API with empty resource type
 *            2. Perform getResourceType() API
 * @post_condition None
 * @expected The API should throw exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheAPITest_stc, SetGetResourceType_ESV_P)
{
    try
    {
        RCSQueryParams m_get;
        string type = "";
        m_get.setResourceType(type);
        string value = m_get.getResourceType();

        if (value.compare("") != 0)
        {
            SET_FAILURE("ResourceType is not empty!");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside SetGetResourceType_ESV_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-11
 * @see None
 * @objective Test 'get' function with Sequential Validation
 * @target std::string get(const std::string& key) const
 * @test_data &key = "power"
 *            value = "on"
 * @pre_condition Perform put(std::string key, std::string value) API
 * @procedure Perform get(const std::string& key) API
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheAPITest_stc, Get_SQV_P)
{
    try
    {

        RCSQueryParams m_get;
        const string &key = DEFAULT_POWER_KEY;
        string value = DEFAULT_POWER_VALUE_ON;

        m_get.put(key, value);
        string val = m_get.get(key);

        if (val != value)
        {
            SET_FAILURE("put value and get value are not equal");
        }

    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside Get_SQV_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-07-25
 * @see None
 * @objective Test 'get' function with empty value
 * @target std::string get(const std::string& key) const
 * @test_data &key = "power"
 *            value = ""
 * @pre_condition Perform put(std::string key, std::string value) API
 * @procedure Perform get(const std::string& key) API
 * @post_condition None
 * @expected 1. The API should not throw exception
 *           2. Should get proper value
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheAPITest_stc, PutGet_ESV_P)
{
    try
    {
        RCSQueryParams m_get;
        const string &key = "";
        string value = "";

        m_get.put(key, value);
        string val = m_get.get(key);

        if (value.compare("") != 0)
        {
            SET_FAILURE("Get value isn't empty!");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside PutGet_ESV_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-11
 * @see None
 * @objective Test 'getAll' function with Sequential Validation
 * @target const Map& getAll() const
 * @test_data key = DEFAULT_POWER_KEY
 *            value = DEFAULT_POWER_VALUE_ON
 * @pre_condition Perform put(std::string key, std::string value) API
 * @procedure Perform getAll() API
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheAPITest_stc, GetAll_SQV_P)
{
    try
    {

        RCSQueryParams m_get;
        string key = DEFAULT_POWER_KEY;
        string value = DEFAULT_POWER_VALUE_ON;
        std::unordered_map < std::string, std::string > m_map;

        m_get.put(key, value);

        m_map = m_get.getAll();
        if (m_map.empty())
        {
            SET_FAILURE("Failed to return all params");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside GetAll_SQV_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-08-01
 * @see None
 * @objective Test 'put' and 'getAll' function by setting empty key and empty value
 * @target  1. put(std::string key, std::string value) API
 *          2. const Map& getAll() const
 * @test_data 1. Empty key
 *            2. Empty value
 * @pre_condition None
 * @procedure 1. Perform put() API with empty key and empty value
 *            2. Perform getAll() API
 * @post_condition None
 * @expected The API should not throw any exception with empty key and empty value
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheAPITest_stc, PutAndGetAll_ESV_P)
{
    try
    {
        RCSQueryParams m_get;
        std::string key;
        std::string value;

        std::unordered_map < std::string, std::string > m_map;

        m_get.put(key, value);
        m_map = m_get.getAll();

        if (m_map.size() != 1)
        {
            SET_FAILURE("Failed to put empty key & empty value in map ");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside PutAndGetAll_ESV_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-12
 * @see None
 * @objective Test 'void get(GetCallback cb)' function with Callback Validation
 * @target void get(GetCallback cb)
 * @test_data GetCallbackCallback
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform  get(GetCallback cb)API
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheAPITest_stc, RemoteResourceObjectGet_CV_P)
{
    try
    {
        m_resource->get(&REResourceCacheAPITest_stc::onRROGetCallback);
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

        if (m_GetCallbackCheck == false)
        {
            SET_FAILURE("Callback is not Invoked!");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE(
                "Exception occurred inside RemoteResourceObjectGet_CV_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-11
 * @see None
 * @objective Test 'void get(RCSQueryParams queryParams, GetCallback cb)' function with Callback Validation
 * @target void get(queryParams, GetCallback cb)
 * @test_data GetCallbackCallback
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform get(queryParams, GetCallback cb) API
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheAPITest_stc, RemoteResourceObjectGetWithQueryParams_CV_P)
{
    RCSQueryParams queryParams;
    try
    {
        m_resource->get(queryParams, &REResourceCacheAPITest_stc::onRROGetCallback);
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

        if (m_GetCallbackCheck == false)
        {
            SET_FAILURE("Callback is not Invoked!");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE(
                "Exception occurred inside RemoteResourceObjectGetWithQueryParams_CV_P"
                + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-11
 * @see None
 * @objective Test 'set' function with Callback Validation
 * @target void set(queryParams, SetCallback cb);
 * @test_data Resource Attributes
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Create resource Attributes
 *            2. Perform set() API
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheAPITest_stc, RemoteResourceObjectSet_CV_P)
{
    try
    {
        RCSResourceAttributes resAttributes;
        string key = DEFAULT_POWER_KEY;
        string value = DEFAULT_POWER_VALUE_ON;
        resAttributes[key] = value;

        m_resource->set(resAttributes, &REResourceCacheAPITest_stc::onRROSetCallback);
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

        if (m_SetCallbackCheck == false)
        {
            SET_FAILURE("Callback is not Invoked!");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE(
                "Exception occurred inside RemoteResourceObjectSet_CV_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-11
 * @see None
 * @objective Test 'set' function with Sequential Validation
 * @target void set(queryParams, SetCallback cb);
 * @test_data Resource Attributes
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Create resource Attributes
 *            2. Perform get() API
 *            3. Perform set() API
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheAPITest_stc, RemoteResourceObjectSet_SQV_P)
{
    try
    {
        RCSResourceAttributes resAttributes;
        string key = DEFAULT_POWER_KEY;
        string value = DEFAULT_POWER_VALUE_ON;
        resAttributes[key] = value;

        m_resource->get(&REResourceCacheAPITest_stc::onRROGetCallback);
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

        if (m_GetCallbackCheck == false)
        {
            SET_FAILURE("Callback is not Invoked!");
        }

        m_resource->set(resAttributes, &REResourceCacheAPITest_stc::onRROSetCallback);
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

        if (m_SetCallbackCheck == false)
        {
            SET_FAILURE("Callback is not Invoked!");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE(
                "Exception occurred inside RemoteResourceObjectSet_SQV_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-11
 * @see None
 * @objective Test 'set' function with Callback Validation
 * @target void set(queryParams, const RCSResourceAttributes& attributes, SetCallback cb);
 * @test_data Resource Attributes
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Create resource Attributes
 *            2. Perform set() API
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheAPITest_stc, RemoteResourceObjectSetWithQueryParams_CV_P)
{
    try
    {
        RCSResourceAttributes resAttributes;
        string key = DEFAULT_POWER_KEY;
        string value = DEFAULT_POWER_VALUE_ON;
        resAttributes[key] = value;
        RCSQueryParams queryParams;

        m_resource->set(queryParams, resAttributes, &REResourceCacheAPITest_stc::onRROSetCallback);
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

        if (m_SetCallbackCheck == false)
        {
            SET_FAILURE("Callback is not Invoked!");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE(
                "Exception occurred inside RemoteResourceObjectSetWithQueryParams_CV_P"
                + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-11
 * @see None
 * @objective Test 'set' function with Sequential Validation
 * @target void set(queryParams, const RCSResourceAttributes& attributes, SetCallback cb);
 * @test_data Resource Attributes
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Create resource Attributes
 *            2. Perform get()API
 *            3. Perform set() API
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheAPITest_stc, RemoteResourceObjectSetWithQueryParams_SQV_P)
{
    try
    {
        RCSResourceAttributes resAttributes;
        string key = DEFAULT_POWER_KEY;
        string value = DEFAULT_POWER_VALUE_ON;
        resAttributes[key] = value;
        RCSQueryParams queryParams;

        m_resource->get(queryParams, &REResourceCacheAPITest_stc::onRROGetCallback);
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

        if (m_GetCallbackCheck == false)
        {
            SET_FAILURE("Callback is not Invoked!");
        }

        m_resource->set(queryParams, resAttributes, &REResourceCacheAPITest_stc::onRROSetCallback);
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

        if (m_SetCallbackCheck == false)
        {
            SET_FAILURE("Callback is not Invoked!");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE(
                "Exception occurred inside RemoteResourceObjectSetWithQueryParams_SQV_P"
                + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-09-08
 * @see   RCSResourceObject, RCSResourceObject::SetRequestHandlerPolicy
 * @see   RCSRepresentation(const std::string& uri, const std::vector< std::string >& interfaces,const std::vector< std::string >& resourceTypes,
            const RCSResourceAttributes& attrs);
 * @objective Test set function with sequential validation 
 * @target void set(const RCSQueryParams& queryParams, const RCSRepresentation &rep,
                    SetCallback cb);
 * @test_data Query parameters, Representation and SetCallback
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Perform RCSRepresentation() API
 *            2. Perform set() API  
 * @post_condition None
 * @expected 1. set request should be sucessful with resource representation 2. should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheAPITest_stc, RemoteResourceObjectSetRCSQRCSREP_SQV_P)
{
    try
    {
        RCSQueryParams queryParams;
        RCSRepresentation rcsRep;

        std::string uri = "uri";
        std::vector < std::string > interfaces;
        std::vector < std::string > resourceTypes;
        RCSResourceAttributes resattributes;
        string key = DEFAULT_POWER_KEY;
        string value = DEFAULT_POWER_VALUE_ON;
        resattributes[key] = value;


        rcsRep = RCSRepresentation(uri, interfaces, resourceTypes, resattributes);

        m_resource->set(queryParams, rcsRep, REResourceCacheAPITest_stc::onRROSetCallback);
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        if (m_SetCallbackCheck == false)
        {
            SET_FAILURE("Set Callback is not Invoked!");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE(
                "Exception occurred inside RemoteResourceObjectSetRCSQRCSREP_SQV_P: " + string(e.what()));
    }
}
#endif
