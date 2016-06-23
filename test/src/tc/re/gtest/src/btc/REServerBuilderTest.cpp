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

class REServerBuilderTest_btc: public ::testing::Test
{
protected:

    REHelper* m_pREHelper;
    std::string m_errorMsg;
    RCSResourceObject::Builder* m_pBuilder;
    shared_ptr< RCSResourceObject > m_resourceObject;

    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();
#ifdef __LINUX__ 
        CommonUtil::launchApp(RE_SERVER_APP);
        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);
#endif
        m_pREHelper = REHelper::getInstance();
        m_pBuilder = new RCSResourceObject::Builder(LIGHT_1_URI, RESOURCE_TYPE_LIGHT,
                DEFAULT_INTERFACE);
        m_pBuilder->setDiscoverable(true);
        m_pBuilder->setObservable(true);
        m_resourceObject = m_pBuilder->build();
        m_resourceObject->setAttribute(DEFAULT_POWER_KEY, DEFAULT_POWER_VALUE);
        m_resourceObject->setAttribute(DEFAULT_INTENSITY_KEY, DEFAULT_INTENSITY_VALUE);
        m_errorMsg = "";
    }

    virtual void TearDown()
    {
        delete m_pBuilder;
        m_pBuilder = NULL;

#ifdef __LINUX__ 
        CommonUtil::killApp(RE_SERVER_APP);
        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);
#endif
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
 * @since 2015-07-16
 * @see None
 * @objective Test 'Builder' constructor with positive basic way
 * @target Builder(std::string uri, std::string type, std::string interface)
 * @test_data uri = LIGHT_1_URI
 *            type = RESOURCE_TYPE_LIGHT
 *            interface = DEFAULT_INTERFACE
 * @pre_condition None
 * @procedure Make Builder class with valid values
 * @post_condition None
 * @expected Should not throw exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REServerBuilderTest_btc, Builder_SRC_P)
{
    try
    {
        RCSResourceObject::Builder* builder;
        builder = new RCSResourceObject::Builder(LIGHT_1_URI, RESOURCE_TYPE_LIGHT, DEFAULT_INTERFACE);
    }
    catch (exception& e)
    {
        SET_FAILURE("Unable to make Builder object, exception occurred: " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-07
 * @see None
 * @objective Test 'setDiscoverable' function with discoverable as true
 * @target Builder& setDiscoverable(bool discoverable)
 * @test_data discoverable = true
 * @pre_condition Builder class should be initialized
 * @procedure Perform setDiscoverable() API with discoverable as true
 * @post_condition None
 * @expected The API should not throw any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REServerBuilderTest_btc, SetDiscoverableAsTrue_SRC_P)
{
    bool discoverable = true;

    try
    {
        m_pBuilder->setDiscoverable(discoverable);
    }catch(exception& e) {
        SET_FAILURE("Unable to set discoverable to true, exception is " + string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-07
 * @see None
 * @objective Test 'setDiscoverable' function with discoverable as false
 * @target Builder& setDiscoverable(bool discoverable)
 * @test_data discoverable = false
 * @pre_condition Builder class should be initialized
 * @procedure Perform setDiscoverable() API with discoverable as false
 * @post_condition None
 * @expected The API should not throw any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REServerBuilderTest_btc, SetDiscoverableAsFalse_SRC_P)
{
    bool discoverable = false;

    try
    {
        m_pBuilder->setDiscoverable(discoverable);
    }catch(exception& e) {
        SET_FAILURE("Unable to set discoverable to false, exception is " + string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-07
 * @see None
 * @objective Test 'setObservable' function with observable as true
 * @target Builder& setObservable(bool observable)
 * @test_data observable = true
 * @pre_condition Builder class should be initialized
 * @procedure Perform setObservable() API with observable as true
 * @post_condition None
 * @expected The API should not throw any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REServerBuilderTest_btc, SetObservableAsTrue_SRC_P)
{
    bool observable = true;

    try
    {
        m_pBuilder->setObservable(observable);
    }catch(exception& e) {
        SET_FAILURE("Unable to set observable to true, exception is " + string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-07
 * @see None
 * @objective Test 'setObservable' function with observable as false
 * @target Builder& setObservable(bool observable)
 * @test_data observable = false
 * @pre_condition Builder class should be initialized
 * @procedure Perform setObservable() API with observable as false
 * @post_condition None
 * @expected The API should not throw any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REServerBuilderTest_btc, SetObservableAsFalse_SRC_P)
{
    bool observable = false;

    try
    {
        m_pBuilder->setObservable(observable);
    }catch(exception& e) {
        SET_FAILURE("Unable to set observable to false, exception is " + string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-07
 * @see None
 * @objective Test 'setAttributes' function with positive basic way
 * @target Builder& setAttributes(const ResourceAttributes&)
 * @test_data Resource Attributes
 * @pre_condition Builder class should be initialized
 * @procedure Perform setAttributes() API
 * @post_condition None
 * @expected The API should not throw exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REServerBuilderTest_btc, SetAttributes_SRC_P)
{
    RCSResourceAttributes resAttributes;
    string key = "power";
    string value = "on";
    resAttributes[key] = value;

    try
    {
        m_pBuilder->setAttributes(resAttributes);
    }catch (exception& e){
        SET_FAILURE("Unable to set Resource Attributes, exception is : " + string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-07
 * @see None
 * @objective Test 'setAttributes' function without key and value
 * @target Builder& setAttributes(const ResourceAttributes&)
 * @test_data None
 * @pre_condition Builder class should be initialized
 * @procedure Perform setAttributes() API without key and value
 * @post_condition None
 * @expected the API should not crash
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REServerBuilderTest_btc, SetAttributes_ECRC_N)
{
    RCSResourceAttributes resAttributes;

    try
    {
        m_pBuilder->setAttributes(resAttributes);
    }catch (exception& e){
        SET_FAILURE("Unable to set Resource Attributes, exception is : " + string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-07
 * @see None
 * @objective Test 'setAttribute' function with positive basic way
 * @target void setAttribute(const std::string& key, RCSResourceAttributes::Value&& value)
 * @test_data   1. key = "power"
 *              2. value = "on"
 * @pre_condition   1. Builder class should be initialized
 *                  2. Resource object should be built
 * @procedure Perform setAttribute() API with key and value
 * @post_condition None
 * @expected The API should not generate exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REServerBuilderTest_btc, SetAttribute_SRC_P)
{
    RCSResourceAttributes resAttributes;

    string key = "power";
    string value = "on";

    try
    {
        m_resourceObject->setAttribute(key, value);
    }
    catch (exception& e)
    {
        SET_FAILURE("Unable to set attribute, exception occurred: " + string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-07
 * @see None
 * @objective Test 'setAttribute' function with with empty key and value
 * @target void setAttribute(const std::string& key, RCSResourceAttributes::Value&& value)
 * @test_data   1. key = ""
 *              2. value = ""
 * @pre_condition   1. Builder class should be initialized
 *                  2. Resource object should be built
 * @procedure Perform setAttribute() API with empty key and value
 * @post_condition None
 * @expected The API should not create exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REServerBuilderTest_btc, SetAttribute_ESV_P)
{
    RCSResourceAttributes resAttributes;

    string key = "";
    string value = "";

    try
    {
        m_resourceObject->setAttribute(key, value);
    }
    catch (exception& e)
    {
        SET_FAILURE("Unable to set attribute, exception occurred: " + string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-07
 * @see None
 * @objective Test 'getAttribute' function with positive basic way
 * @target T& getAttribute(const std::string& key)
 * @test_data   key = "power"
 * @pre_condition   1. Builder class should be initialized
 *                  2. Resource object should be built
 * @procedure Perform getAttribute() API using key
 * @post_condition None
 * @expected The returned value should not be empty
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REServerBuilderTest_btc, GetAttribute_SRC_P)
{
    RCSResourceAttributes resAttributes;

    string key = "power";
    string value = m_resourceObject->getAttributeValue(key).toString();
    ;

    if (value.compare("") == 0)
    {
        SET_FAILURE("Attribute Value is empty!!");
    }
}
#endif

/**
 * @since 2015-07-07
 * @see None
 * @objective Test 'getAttribute' function using empty key
 * @target T& getAttribute(const std::string& key)
 * @test_data   key = ""
 *              value = ""
 * @pre_condition   1. Builder class should be initialized
 *                  2. Resource object should be built
 * @procedure Perform getAttribute() API using empty key
 * @post_condition None
 * @expected The returned value should be empty
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REServerBuilderTest_btc, GetAttribute_ESV_N)
{
    RCSResourceAttributes resAttributes;

    const string key = "";
    string value = "";

    try{
        value = m_resourceObject->getAttributeValue(key).toString();

        if (value.compare("") != 0)
        {
            SET_FAILURE("Attribute Value is not empty!!");
        }
    }catch(exception& e){
        string exceptionMsg = string(e.what());
        if(exceptionMsg.compare("No attribute named ''") != 0){
            SET_FAILURE("Exception Occurred : " + exceptionMsg);
        }
    }
}
#endif

/**
 * @since 2015-07-07
 * @see None
 * @objective Test 'getAttributes' function with positive basic way
 * @target ResourceAttributes getAttributes()
 * @test_data   None
 * @pre_condition   None
 * @procedure Perform getAttributes() API
 * @post_condition None
 * @expected The returned resource Attributes should not be empty
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REServerBuilderTest_btc, GetAttributes_SRC_P)
{
    RCSResourceObject::LockGuard lock(*m_resourceObject);
    RCSResourceAttributes resAttributes = m_resourceObject->getAttributes();

    if (resAttributes.size() <= 0)
    {
        SET_FAILURE("Unable to get Resource Attributes");
    }
}
#endif

/**
 * @since 2015-07-07
 * @see None
 * @objective Test 'containsAttribute' function with positive basic way
 * @target bool containsAttribute(const std::string& key)
 * @test_data   key = "power"
 * @pre_condition   1. Builder class should be initialized
 *                  2. Resource object should be built
 * @procedure Perform hasAttribute() API using key
 * @post_condition None
 * @expected The returned value should be true
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REServerBuilderTest_btc, ContainsAttribute_SRC_P)
{
    string key = "power";
    bool hasPowerAttribute = m_resourceObject->containsAttribute(key);

    if (hasPowerAttribute == false)
    {
        SET_FAILURE("Power attribute is absent!!");
    }
}
#endif

/**
 * @since 2015-07-07
 * @see None
 * @objective Test 'containsAttribute' function using empty key
 * @target bool hasAttribute(const std::string& key)
 * @test_data   key = ""
 * @pre_condition   1. Builder class should be initialized
 *                  2. Resource object should be built
 * @procedure Perform hasAttribute() API using empty key
 * @post_condition None
 * @expected The returned value should be true
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REServerBuilderTest_btc, ContainsAttribute_ESV_N)
{
    const string key = "";
    bool hasEmptyAttribute = m_resourceObject->containsAttribute(key);

    if (hasEmptyAttribute == true)
    {
        SET_FAILURE("Empty attribute is Present!!");
    }
}
#endif

/**
 * @since 2015-07-07
 * @see None
 * @objective Test 'notify' function with positive basic way
 * @target bool notify()
 * @test_data   None
 * @pre_condition   1. Builder class should be initialized
 *                  2. Resource object should be built
 * @procedure Perform notify() API
 * @post_condition None
 * @expected The returned value should be true
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REServerBuilderTest_btc, Notify_SRC_P)
{
    RCSResourceAttributes resAttributes;

    try
    {
        m_resourceObject->notify();
    }
    catch (exception& e)
    {
        SET_FAILURE("Unable to notify, exception occurred: " + string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-07
 * @see None
 * @objective Test 'setGetRequestHandler' function with positive basic way
 * @target void setGetRequestHandler(GetRequestHandler handler)
 * @test_data   GetRequestHandler
 * @pre_condition   1. Builder class should be initialized
 *                  2. Resource object should be built
 * @procedure Perform setGetRequestHandler() API using GetRequestHandler
 * @post_condition None
 * @expected No exception should be generated
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REServerBuilderTest_btc, SetGetAttributeRequestHandler_SRC_P)
{
    try
    {
        m_resourceObject->setGetRequestHandler(bind(&REServerBuilderTest_btc::OnGetRequestReceived, this, PH::_1, PH::_2));
    }
    catch (exception& e)
    {
        SET_FAILURE("Unable to set GetRequestHandler, exception occurred: " + string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-07
 * @see None
 * @objective Test 'setGetRequestHandler' function using NULL handler
 * @target virtual void setGetRequestHandler(GetRequestHandler handler)
 * @test_data   None
 * @pre_condition   1. Builder class should be initialized
 *                  2. Resource object should be built
 * @procedure Perform setGetRequestHandler() API using NULL handler
 * @post_condition None
 * @expected  No exception should be generated
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REServerBuilderTest_btc, SetGetAttributeRequestHandler_NV_N)
{
    try
    {
        m_resourceObject->setGetRequestHandler(NULL);
    }
    catch (exception& e)
    {
        SET_FAILURE("exception occurred with NULL handler: " + string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-07
 * @see None
 * @objective Test 'setGetRequestHandler' function with positive basic way
 * @target void setSetRequestHandler(SetRequestHandler handler)
 * @test_data   SetRequestHandler
 * @pre_condition   1. Builder class should be initialized
 *                  2. Resource object should be built
 * @procedure Perform setSetRequestHandler() API using SetRequestHandler
 * @post_condition None
 * @expected    No exception should be generated
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REServerBuilderTest_btc, SetSetAttributesRequestHandler_SRC_P)
{
    try
    {
        m_resourceObject->setSetRequestHandler(bind(&REServerBuilderTest_btc::OnSetRequestReceived, this, PH::_1, PH::_2));
    }
    catch (exception& e)
    {
        SET_FAILURE("Unable to set SetRequestHandler, exception occurred: " + string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-07
 * @see None
 * @objective Test 'setSetRequestHandler' function using NULL handler
 * @target void setSetRequestHandler(SetRequestHandler handler)
 * @test_data   None
 * @pre_condition   1. Builder class should be initialized
 *                  2. Resource object should be built
 * @procedure Perform setSetRequestHandler() API using NULL handler
 * @post_condition None
 * @expected    No exception should be generated
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REServerBuilderTest_btc, SetSetAttributesRequestHandler_NV_N)
{
    try
    {
        m_resourceObject->setSetRequestHandler(NULL);
    }
    catch (exception& e)
    {
        SET_FAILURE("exception occurred with NULL handler: " + string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-07
 * @see None
 * @objective Test 'setSetRequestHandlerPolicy()' function with positive basic way
 * @target void setSetRequestHandlerPolicy(SetRequestHandlerPolicy policy)
 * @test_data   SetRequestHandlerPolicy
 * @pre_condition   1. Builder class should be initialized
 *                  2. Resource object should be built
 * @procedure   Perform setSetRequestHandlerPolicy() API using SetRequestHandlerPolicy
 * @post_condition None
 * @expected    No exception should be generated
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REServerBuilderTest_btc, SetSetRequestHandlerPolicy_SRC_P)
{
    try
    {
        m_resourceObject->setSetRequestHandlerPolicy(RCSResourceObject::SetRequestHandlerPolicy::ACCEPTANCE);
    }
    catch (exception& e)
    {
        SET_FAILURE("Unable to set SetRequestHandlerPolicy, exception occurred: " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-02-23
 * @see None
 * @objective Test 'removeAttribute' function with positive basic way [SRC]
 * @target bool removeAttribute(const std::string& key)
 * @test_data Resource Attributes
 * @pre_condition Builder class should be initialized
 * @procedure 1. Perform setAttribute() API
              2. Perform removeAttribute() API
 * @post_condition None
 * @expected API should not generate exception
 **/

#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REServerBuilderTest_btc, RemoveAttribute_SRC_P)
{
    RCSResourceAttributes resAttributes;

    string key = "power";
    string value = "on";

    bool m_result = NULL;

    try
    {
        m_resourceObject->setAttribute(key, value);
        m_result = m_resourceObject->removeAttribute(key);
        if(!m_result)
        {
            SET_FAILURE("Unable to remove attribute");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Unable to remove attribute, exception occurred: " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-02-23
 * @see None
 * @objective Test 'removeAttribute' function with a custom key to check success return
 * @target bool removeAttribute(const std::string& key)
 * @test_data Resource Attributes
 * @pre_condition Builder class should be initialized
 * @procedure 1. Perform setAttribute() API
              2. Perform removeAttribute() API with a custom key to check success return
 * @post_condition None
 * @expected The API should not crash
 **/

#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REServerBuilderTest_btc, RemoveAttribute_ECRC_N)
{
    RCSResourceAttributes resAttributes;

    bool m_result = NULL;

    try
    {
        m_resourceObject->setAttribute(DEFAULT_POWER_KEY, DEFAULT_POWER_VALUE_ON);
        m_result = m_resourceObject->removeAttribute("energy");
        if(m_result)
        {
            SET_FAILURE("Unable to remove attribute");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Unable to remove attribute, exception occurred: " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-02-23
 * @see None
 * @objective Test 'removeAttribute' function with empty string value
 * @target bool removeAttribute(const std::string& key)
 * @test_data key = ""
 * @pre_condition Builder class should be initialized
 * @procedure 1. Perform removeAttribute() API with empty string value
 * @post_condition None
 * @expected API should not generate exception
 **/

#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REServerBuilderTest_btc, RemoveAttribute_ESV_P)
{
    RCSResourceAttributes resAttributes;

    try
    {
        m_resourceObject->removeAttribute("");
    }
    catch (exception& e)
    {
        SET_FAILURE("Unable to remove attribute, exception occurred: " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-02-23
 * @see None
 * @objective Test 'containsAttribute' function with a custom key to check success return
 * @target bool containsAttribute(const std::string& key)
 * @test_data key = "energy"
 * @pre_condition Builder class should be initialized
 * @procedure 1. Perform setAttribute() API
              2. Perform containsAttribute() API with a custom key to check success return
 * @post_condition None
 * @expected API should not generate exception
 **/

#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REServerBuilderTest_btc, ContainsAttribute_ECRC_N)
{
    RCSResourceAttributes resAttributes;
    bool m_result = NULL;

    try
    {
        m_resourceObject->setAttribute(DEFAULT_POWER_KEY, DEFAULT_POWER_VALUE_ON);
        m_result = m_resourceObject->containsAttribute("energy");
        if(m_result)
        {
            SET_FAILURE("No such attribute present!");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("No such attribute present!, exception occurred: " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-02-23
 * @see None
 * @objective Test 'isDiscoverable' function with positive basic way [SRC]
 * @target virtual bool isDiscoverable() const
 * @test_data None
 * @pre_condition Builder class should be initialized
 * @procedure Perform isDiscoverable() API
 * @post_condition None
 * @expected API should not generate exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REServerBuilderTest_btc, IsDiscoverable_SRC_P)
{
    try
    {
        bool m_result = NULL;
        m_result = m_resourceObject->isDiscoverable();
        if (!m_result)
        {
            SET_FAILURE("Resource isn't Discoverable");
        }
    }
    catch(exception& e) {
        SET_FAILURE("exception occured inside IsDiscoverable_SRC_P: " + string(e.what()));
    }

}
#endif

/**
 * @since 2016-02-24
 * @see None
 * @objective Test 'isDiscoverable' function with success return checking
 * @target virtual bool isDiscoverable() const
 * @test_data discoverable = false
 * @pre_condition Builder class should be initialized & perform setDiscoverable() API
 * @procedure Perform isDiscoverable() API
 * @post_condition None
 * @expected the resource isn't discoverable by checking
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REServerBuilderTest_btc, IsDiscoverable_SRCC_P)
{
    bool discoverable = false;

    try
    {
        bool m_result = NULL;
        m_pBuilder->setDiscoverable(discoverable);
        m_result = m_resourceObject->isDiscoverable();
        if (m_result == true)
        {
            SET_FAILURE("Resource is Discoverable although discoverable is set as false!" );
            IOTIVITYTEST_LOG(DEBUG, "Return value is:  %d", m_result);
        }
    }
    catch(exception& e) {
        SET_FAILURE("exception occured inside IsDiscoverable_SRCC_P: " + string(e.what()));
    }

}
#endif

/**
 * @since 2016-02-24
 * @see None
 * @objective Test 'isObservable' function with positive basic way [SRC]
 * @target virtual bool isObservable() const
 * @test_data None
 * @pre_condition Builder class should be initialized
 * @procedure Perform isObservable() API
 * @post_condition None
 * @expected the resource is observable by checking
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REServerBuilderTest_btc, IsObservable_SRC_P)
{
    try
    {
        bool m_result = NULL;

        m_result = m_resourceObject->isObservable();
        if (!m_result)
        {
            SET_FAILURE("Resource isn't Observable");
            IOTIVITYTEST_LOG(DEBUG, "Return value is:  %d", m_result);
        }
    }
    catch(exception& e) {
        SET_FAILURE("exception occured inside IsObservable_SRC_P: " + string(e.what()));
    }

}
#endif

/**
 * @since 2016-02-24
 * @see None
 * @objective Test 'isObservable' function with success return checking
 * @target virtual bool isObservable() const
 * @test_data observable = false
 * @pre_condition Builder class should be initialized
 * @procedure Perform isObservable() API
 * @post_condition None
 * @expected the resource isn't observable by checking
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REServerBuilderTest_btc, IsObservable_SRCC_P)
{
    bool observable = false;
    try
    {
        m_pBuilder->setObservable(observable);

        bool m_result = NULL;
        m_result = m_resourceObject->isObservable();
        if (m_result == true)
        {
            SET_FAILURE("Resource is Discoverable although discoverable is set as false!");
        }
    }
    catch(exception& e) {
        SET_FAILURE("exception occured inside IsObservable_SRCC_P: " + string(e.what()));
    }

}
#endif

/**
 * @since 2016-02-24
 * @see None
 * @objective Test 'setAutoNotifyPolicy()' function with positive basic way [SRC]
 * @target void setAutoNotifyPolicy(AutoNotifyPolicy policy)
 * @test_data   AutoNotifyPolicy
 * @pre_condition   1. Builder class should be initialized
 *                  2. Resource object should be built
 * @procedure   Perform setAutoNotifyPolicy() API using AutoNotifyPolicy
 * @post_condition None
 * @expected    No exception should be generated
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REServerBuilderTest_btc, SetAutoNotifyPolicy_SRC_P)
{
    try
    {
        m_resourceObject->setAutoNotifyPolicy(RCSResourceObject::AutoNotifyPolicy::ALWAYS);
    }

    catch (exception& e)
    {
        SET_FAILURE("Unable to set SetAutoNotifyPolicy_FSV_P, exception occurred: " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-02-24
 * @see None
 * @objective Test 'setAutoNotifyPolicy()' function using AutoNotifyPolicy with enum value 'NEVER'
 * @target void setAutoNotifyPolicy(AutoNotifyPolicy policy)
 * @test_data   AutoNotifyPolicy
 * @pre_condition   1. Builder class should be initialized
 *                  2. Resource object should be built
 * @procedure   Perform setAutoNotifyPolicy() API using AutoNotifyPolicy with enum value 'NEVER'
 * @post_condition None
 * @expected    No exception should be generated
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REServerBuilderTest_btc, SetAutoNotifyPolicyNever_CLU_P)
{
    try
    {
        m_resourceObject->setAutoNotifyPolicy(RCSResourceObject::AutoNotifyPolicy::NEVER);
        IOTIVITYTEST_LOG(DEBUG, "Requests are ignored");

    }
    catch (exception& e)
    {
        SET_FAILURE("Requests aren't ignored, exception occurred: " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-02-24
 * @see None
 * @objective Test 'setAutoNotifyPolicy()' function using AutoNotifyPolicy with enum value "UPDATED"
 * @target void setAutoNotifyPolicy(AutoNotifyPolicy policy)
 * @test_data   AutoNotifyPolicy
 * @pre_condition   1. Builder class should be initialized
 *                  2. Resource object should be built
 * @procedure   Perform setAutoNotifyPolicy() API using AutoNotifyPolicy with enum value "UPDATED"
 * @post_condition None
 * @expected    No exception should be generated
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REServerBuilderTest_btc, SetAutoNotifyPolicyUPDATED_CLU_P)
{
    try
    {
        m_resourceObject->setAutoNotifyPolicy(RCSResourceObject::AutoNotifyPolicy::UPDATED);
        IOTIVITYTEST_LOG(DEBUG, "attributes are changed");

    }
    catch (exception& e)
    {
        SET_FAILURE("attributes aren't changed, exception occurred: " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-02-25
 * @see None
 * @objective Test 'getAutoNotifyPolicy()' function with positive basic way [SRC]
 * @target AutoNotifyPolicy getAutoNotifyPolicy() const
 * @test_data   AutoNotifyPolicy
 * @pre_condition   1. Builder class should be initialized
 *                  2. Resource object should be built
 * @procedure   1. Perform setAutoNotifyPolicy() API using AutoNotifyPolicy
                2. Perform getAutoNotifyPolicy() API
 * @post_condition None
 * @expected    Current auto notify policy should be returned
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REServerBuilderTest_btc, GetAutoNotifyPolicy_SRC_P)
{
    try
    {
        RCSResourceObject::AutoNotifyPolicy autoNotifyPolicy;
        m_resourceObject->setAutoNotifyPolicy(RCSResourceObject::AutoNotifyPolicy::ALWAYS);
        m_resourceObject->getAutoNotifyPolicy();
        if(autoNotifyPolicy != RCSResourceObject::AutoNotifyPolicy::ALWAYS)
            SET_FAILURE("Current auto notify policy not found!");
    }

    catch (exception& e)
    {
        SET_FAILURE("Unable to Get AutoNotifyPolicy, exception occurred: " + string(e.what()));
    }
}
#endif


/**
 * @since 2016-02-25
 * @see None
 * @objective Test 'getSetRequestHandlerPolicy()' function with positive basic way [SRC]
 * @target SetRequestHandlerPolicy getSetRequestHandlerPolicy() const
 * @test_data   SetRequestHandlerPolicy
 * @pre_condition   1. Builder class should be initialized
 *                  2. Resource object should be built
 * @procedure   1. Perform setSetRequestHandlerPolicy() API using SetRequestHandlerPolicy
                2. Perform getSetRequestHandlerPolicy()
 * @post_condition None
 * @expected    Current set request handling policy should be returned
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REServerBuilderTest_btc, GetSetRequestHandlerPolicy_SRC_P)
{
    try
    {
        RCSResourceObject::SetRequestHandlerPolicy setRequestHandlerPolicy;
        m_resourceObject->setSetRequestHandlerPolicy(RCSResourceObject::SetRequestHandlerPolicy::ACCEPTANCE);
        m_resourceObject->getSetRequestHandlerPolicy();
        if(setRequestHandlerPolicy != RCSResourceObject::SetRequestHandlerPolicy::ACCEPTANCE)
            SET_FAILURE("Current set request handling policy not found!");
    }
    catch (exception& e)
    {
        SET_FAILURE("Unable to get SetRequestHandlerPolicy, exception occurred: " + string(e.what()));
    }
}
#endif


/**
 * @since 2016-02-25
 * @see None
 * @objective Test 'setSetRequestHandlerPolicy()' function using SetRequestHandlerPolicy with enum value "NEVER"
 * @target  void setSetRequestHandlerPolicy(SetRequestHandlerPolicy policy)
 * @test_data   SetRequestHandlerPolicy
 * @pre_condition   1. Builder class should be initialized
 *                  2. Resource object should be built
 * @procedure   Perform setSetRequestHandlerPolicy() API using SetRequestHandlerPolicy with enum value "NEVER"
 * @post_condition None
 * @expected   The API should not generate exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REServerBuilderTest_btc, setSetRequestHandlerPolicy_CLU_P)
{
    try
    {
        m_resourceObject->setSetRequestHandlerPolicy(RCSResourceObject::SetRequestHandlerPolicy::NEVER);
    }
    catch (exception& e)
    {
        SET_FAILURE("Unable to set SetRequestHandlerPolicy, exception occurred: " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-02-25
 * @see None
 * @objective Test 'build()' function with positive basic way [SRC]
 * @target  RCSResourceObject::Ptr build()
 * @test_data None
 * @pre_condition 1. Builder class should be initialized
 *                2. Resource object should be built
 * @procedure Perform build() API
 * @post_condition None
 * @expected  The API should not generate exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REServerBuilderTest_btc, Build_SRC_P)
{
    try
    {
        RCSResourceObject::Builder* builder;
        builder = new RCSResourceObject::Builder("/device/ac-1", "core.ac", DEFAULT_INTERFACE);
        builder->build();
    }
    catch (exception& e)
    {
        SET_FAILURE("BUILD FAILED: " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-02-26
 * @see None
 * @objective Test 'getAttributeValue()' function with set & get attribute value
 * @target RCSResourceAttributes::Value getAttributeValue(const std::string& key) const
 * @test_data   1. key = "power"
 *              2. value = "on"
 * @pre_condition   1. Builder class should be initialized
 *                  2. Resource object should be built
 * @procedure 1. Perform setAttribute() API
              2. Perform getAttributeValue() API
 * @post_condition None
 * @expected The API should not generate exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REServerBuilderTest_btc, GetAttributeValue_SRC_P)
{
    RCSResourceAttributes resAttributes;
    try
    {
        string value;
        m_resourceObject->setAttribute(DEFAULT_POWER_KEY, DEFAULT_POWER_VALUE_ON);
        value = m_resourceObject->getAttributeValue(DEFAULT_POWER_KEY).toString();

        if (value.compare(DEFAULT_POWER_VALUE_ON) == 0)
        {
            SET_FAILURE("Attribute Value is empty!!");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Unable to get attribute, exception occurred inside GetAttributeValue_SRC_P: " + string(e.what()));
    }
}
#endif
