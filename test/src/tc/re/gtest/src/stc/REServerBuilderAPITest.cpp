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

class REServerBuilderAPITest_stc: public ::testing::Test
{
protected:

    REHelper* m_pREHelper;
    RCSResourceObject::Builder* m_pBuilder;
    shared_ptr< RCSResourceObject > m_resourceObject;

    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();

        m_pREHelper = REHelper::getInstance();
        try
        {
            m_pBuilder = new RCSResourceObject::Builder(LIGHT_1_URI, RESOURCE_TYPE_LIGHT,
                    DEFAULT_INTERFACE);
            m_pBuilder->setDiscoverable(true);
            m_pBuilder->setObservable(true);
            m_resourceObject = m_pBuilder->build();
            m_resourceObject->setAttribute(DEFAULT_POWER_KEY, DEFAULT_POWER_VALUE);
            m_resourceObject->setAttribute(DEFAULT_INTENSITY_KEY, DEFAULT_INTENSITY_VALUE);
        }
        catch (exception& e)
        {
            SET_FAILURE("Exception occurred in setup, reason : " + string(e.what()));
        }

        IOTIVITYTEST_LOG(INFO, "Testcase setUp finished!!");
    }

    virtual void TearDown()
    {
        delete m_pBuilder;
        m_pBuilder = NULL;

        CommonUtil::runCommonTCTearDownPart();

    }

public:
    RCSGetResponse OnGetRequestReceived(const RCSRequest& primitiveRequest,
            RCSResourceAttributes& resourceAttributes)
    {
        RCSGetResponse response = RCSGetResponse::create(resourceAttributes);

        return response;

    }

    RCSSetResponse OnSetRequestReceived(const RCSRequest& primitiveRequest,
            RCSResourceAttributes& resourceAttributes)
    {
        RCSSetResponse response = RCSSetResponse::create(resourceAttributes);

        return response;
    }
};

/**
 * @since 2015-07-14
 * @see None
 * @objective Test 'build' function with Sequential validation
 * @target shared_ptr<ResourceObject> build(void)
 * @test_data None
 * @pre_condition Builder class should be initialized
 * @procedure 1. Perform setDiscoverable() API
 *            2. Perform setObservable() API
 *            3. Perform build() API
 * @post_condition None
 * @expected ResourceObject shared pointer should not be null
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REServerBuilderAPITest_stc, BuildResourcePointer_SQV_P)
{
    shared_ptr< RCSResourceObject > resourceObject = NULL;

    RCSResourceObject::Builder* pBuilder = new RCSResourceObject::Builder(DUMMY_RESOURCE_URI, DUMMY_RESOURCE_TYPE,
            DEFAULT_INTERFACE);
    pBuilder->setDiscoverable(true);
    pBuilder->setObservable(false);
    resourceObject = pBuilder->build();

    if (resourceObject == NULL)
    {
        SET_FAILURE("Unable to build Resource Object");
    }

    delete pBuilder;
    pBuilder = NULL;
}
#endif

/**
 * @since 2015-07-14
 * @see None
 * @objective Test 'removeAttribute' function with Sequential Validation
 * @target bool removeAttribute(const std::string& key)
 * @test_data   key = "power"
 * @pre_condition   1. Builder class should be initialized
 *                  2. Resource object should be built
 * @procedure Perform removeAttribute() API using key
 * @post_condition None
 * @expected The returned value should be true
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REServerBuilderAPITest_stc, RemoveAttribute_SQV_P)
{
    bool isRemoved = m_resourceObject->removeAttribute(DEFAULT_POWER_KEY);

    if (isRemoved == false)
    {
        SET_FAILURE("Attribute Value is not removed!!");
    }
}
#endif

/**
 * @since 2015-07-14
 * @see None
 * @objective Test 'removeAttribute' function with empty key
 * @target bool removeAttribute(const std::string& key)
 * @test_data   key = ""
 * @pre_condition   1. Builder class should be initialized
 *                  2. Resource object should be built
 * @procedure Perform removeAttribute() API using empty key
 * @post_condition None
 * @expected The returned value should be false
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REServerBuilderAPITest_stc, RemoveAttribute_ESV_N)
{
    RCSResourceAttributes resAttributes;

    bool isRemoved = m_resourceObject->removeAttribute(DEFAULT_EMPTY_KEY);

    if (isRemoved == true)
    {
        SET_FAILURE("Attribute Value is removed!!");
    }
}
#endif

/**
 * @since 2015-11-12
 * @see None
 * @objective Test 'removeAttribute' function with Sequential Validation
 * @target bool containsAttribute(const std::string& key)
 * @target bool removeAttribute(const std::string& key)
 * @test_data key = "power"
 * @pre_condition   1. Builder class should be initialized
 *                  2. Resource object should be built
 * @procedure 1. Perform removeAttribute() API using key
 *            2. Perform containsAttribute() API using same key
 * @post_condition None
 * @expected TC should not crashed
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REServerBuilderAPITest_stc, RemoveAttributeAndContainsAttribute_SQV_P)
{
    try {
        bool isRemoved = m_resourceObject->removeAttribute(DEFAULT_POWER_KEY);

        if (isRemoved == false) {
            SET_FAILURE("Attribute with key=power is not removed");
        }

        bool hasPowerAttribute = m_resourceObject->containsAttribute(DEFAULT_POWER_KEY);

        if (hasPowerAttribute == true)
        {
            SET_FAILURE("Power attribute should already removed.");
        }
    } catch (exception &e) {
        SET_FAILURE("Exception occurs at RemoveAttributeAndContainsAttribute_P " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-02-25
 * @see None
 * @objective Test 'getSetRequestHandlerPolicy()' function without performing setSetRequestHandlerPolicy() API
 * @target SetRequestHandlerPolicy getSetRequestHandlerPolicy() const
 * @test_data   SetRequestHandlerPolicy
 * @pre_condition   1. Builder class should be initialized
 *                  2. Resource object should be built
 * @procedure  Perform getSetRequestHandlerPolicy() API without performing setSetRequestHandlerPolicy() API
 * @post_condition None
 * @expected No exception should occur
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REServerBuilderAPITest_stc, GetSetRequestHandlerPolicy_DSCC_N)
{
    try
    {
        m_resourceObject->getSetRequestHandlerPolicy();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred in GetSetRequestHandlerPolicy_DSCC_N:: " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-02-25
 * @see None
 * @objective Test 'getAutoNotifyPolicy()' function without performing setAutoNotifyPolicy() API
 * @target AutoNotifyPolicy getAutoNotifyPolicy() const
 * @test_data   None
 * @pre_condition   1. Builder class should be initialized
 *                  2. Resource object should be built
 * @procedure Perform getAutoNotifyPolicy() API without performing setAutoNotifyPolicy() API
 * @post_condition None
 * @expected No exception should occur
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REServerBuilderAPITest_stc, GetAutoNotifyPolicy_DSCC_N)
{
    try
    {
        m_resourceObject->getAutoNotifyPolicy();
        IOTIVITYTEST_LOG(DEBUG, "No policy found");
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred in GetAutoNotifyPolicy_DSCC_N: " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-02-24
 * @see None
 * @objective Test 'setAttributes' function multiple times
 * @target Builder& setAttributes(const ResourceAttributes&)
 * @test_data Resource Attributes
 * @pre_condition None
 * @procedure 1. Builder class should be initialized
              2. Perform setAttributes() API
              3. Repeat steps 1~2 multiple times
 * @post_condition None
 * @expected Attributes for the resource should be set
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REServerBuilderAPITest_stc, SetAttributes_VLCC_P)
{

    for (int i = 0; i<10; i++)
    {
        RCSResourceAttributes resAttributes;
        resAttributes[DEFAULT_POWER_KEY] = DEFAULT_POWER_VALUE_ON;

        try
        {
            m_pBuilder->setAttributes(resAttributes);
        }
        catch (exception& e){
            SET_FAILURE("Unable to set Resource Attributes, exception is : " + string(e.what()));
        }
    }
}
#endif
