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
#include <unordered_map>
#include "gtest/gtest.h"
#include "REHelper.h"

class REResourceCacheTest_btc: public ::testing::Test
{
protected:

    REHelper* m_pREHelper;
    shared_ptr< RCSRemoteResourceObject > m_resource;
    std::string m_errorMsg;

    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();

#ifdef __LINUX__
        CommonUtil::launchApp(RE_SERVER_APP);
        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);
#endif    

        m_pREHelper = REHelper::getInstance();

        bool isResourceAvailable = m_pREHelper->findPrimitiveResources(
                RESOURCE_TYPE_LIGHT);
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
    void onCacheUpdated(const RCSResourceAttributes & resourceAttributes)
    {

    }

    void onRemoteAttributesReceived(const RCSResourceAttributes & resourceAttributes)
    {

    }

    static void onRROSetCallback(const HeaderOpts& headerOp, const RCSRepresentation& rep,
            int eCode)
    {
        IOTIVITYTEST_LOG(DEBUG, "Set callback is Successfully Invoked!");
    }

    static void onRROGetCallback(const HeaderOpts& headerOp, const RCSRepresentation& rep,
            int eCode)
    {
        IOTIVITYTEST_LOG(DEBUG, "Get callback is Successfully Invoked!");
    }
};

/**
 * @since 2015-07-01
 * @see None
 * @objective Test 'startCaching' function with positive basic way
 * @target void startCaching(void)
 * @test_data None
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform startCaching() API
 * @post_condition None
 * @expected No crash should occur
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheTest_btc, StartCaching_SRC_P)
{
    try
    {
        m_resource->startCaching();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside StartCaching_P" + string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-01
 * @see None
 * @objective Test 'startCaching' function with positive basic way
 * @target void startCaching(ReportPolicy , CacheUpdatedCallback)
 * @test_data 1. Report Policy
 *            2. callback function for receiving cached attribute
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform startCaching() API
 * @post_condition None
 * @expected The API should not crash
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheTest_btc, StartCachingWithCallback_SRC_P)
{
    try
    {
        RCSRemoteResourceObject::CacheUpdatedCallback cucb = std::bind(
                &REResourceCacheTest_btc::onCacheUpdated, this, PH::_1);
        m_resource->startCaching(cucb);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside StartCachingWithCallback_SRC_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-01
 * @see None
 * @objective Test 'startCaching' function with null callback
 * @target void startCaching(ReportPolicy , CacheUpdatedCallback)
 * @test_data 1. Report Policy
 *            2. Null callback
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform startCaching() API
 * @post_condition None
 * @expected The API should not crash
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheTest_btc, StartCaching_NV_N)
{
    try
    {
        m_resource->startCaching(NULL);
    }

    catch (exception& e)
    {
        string exceptionMsg = std::string(e.what());
        if (exceptionMsg.compare("startCaching : Callback is NULL") != 0)
        {
            SET_FAILURE("Exception occurred inside StartCaching_NV_N" + exceptionMsg);
        }
    }
}
#endif

/**
 * @since 2015-07-01
 * @see None
 * @objective Test 'stopCaching' function with positive basic way
 * @target void stopCaching()
 * @test_data None
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform stopCaching() API
 * @post_condition None
 * @expected The API should not crash
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheTest_btc, StopCaching_SRC_P)
{
    try
    {
        m_resource->stopCaching();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside StopCaching_SRC_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-01
 * @see None
 * @objective Test 'getCachedAttributes' function with positive basic way
 * @target void getCachedAttributes()
 * @test_data None
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform getCachedAttributes() API
 * @post_condition None
 * @expected Returned Resource Attributes are not null
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheTest_btc, GetCachedAttributes_SRC_P)
{
    try
    {
        m_resource->startCaching();
        CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);
        RCSResourceAttributes resourceAttributes = m_resource->getCachedAttributes();
        if(resourceAttributes.size() <= 0)
        {
            SET_FAILURE("getCachedAttributes() API returned NULL resource attributes");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside GetChaedAttributes_SRC_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-07
 * @see None
 * @objective Test 'getCachedAttribute' function with positive basic way
 * @target void getCachedAttribute(string&)
 * @test_data key = "power"
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform getCachedAttributes(string&) API
 * @post_condition None
 * @expected returned Attribute is not empty
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheTest_btc, GetCachedAttribute_SRC_P)
{
    try
    {
        m_resource->startCaching();
        CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);

        string value = m_resource->getCachedAttribute(DEFAULT_POWER_KEY).toString();
        if(value.compare("") == 0)
        {
            SET_FAILURE("getCachedAttribute(string&) API returned NULL resource attributes");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside GetChaedAttributes_SRC_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-07
 * @see None
 * @objective Test 'getCachedAttribute' function with invalid key
 * @target void getCachedAttribute(string&)
 * @test_data key = "invalid_key"
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform getCachedAttributes(string&) API
 * @post_condition None
 * @expected returned Attribute is empty
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheTest_btc, GetCachedAttribute_ECRC_N)
{
    try
    {
        string key = "invalid_key";
        string value = m_resource->getCachedAttribute(key).toString();
        if(value.compare("") != 0)
        {
            SET_FAILURE("getCachedAttribute(string&) API returned NULL resource attributes");
        }
    }
    catch (exception& e)
    {
        if (string(e.what()).compare("Caching not started.") != 0)
        {
            SET_FAILURE("Exception occurred inside GetChaedAttributes_ECRC_N: " + string(e.what()));
        }
    }
}
#endif

/**
 * @since 2016-01-06
 * @see None
 * @objective Test 'getCacheState' function with positive basic way
 * @target CacheState getCacheState()
 * @test_data None
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform getCacheState() API
 * @post_condition None
 * @expected The API should not crash
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheTest_btc, GetCacheState_SRC_P)
{
    try
    {
        CacheState cacheState = m_resource->getCacheState();
        if((int) cacheState < 0 || (int) cacheState > 6)
        {
            SET_FAILURE("getCacheState() API returned NULL resource attributes");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside GetCacheState_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-07
 * @see None
 * @objective Test 'getRemoteAttributes' function with positive basic way
 * @target void getRemoteAttributes(AttributeRecievedCallback)
 * @test_data AttributeRecievedCallback
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform getRemoteAttributes() API
 * @post_condition None
 * @expected The API should not crash
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheTest_btc, GetRemoteAttributes_SRC_P)
{
    try
    {
        m_resource->getRemoteAttributes(bind(&REResourceCacheTest_btc::onRemoteAttributesReceived, this, PH::_1));

    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside SetAttributeValue_SRC_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-07
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
TEST_F(REResourceCacheTest_btc, GetRemoteAttributes_NV_N)
{
    try
    {
        m_resource->getRemoteAttributes(NULL);

    }
    catch (exception& e)
    {
        string exceptionMsg = std::string(e.what());
        if(exceptionMsg.compare("getRemoteAttributes : Callback is empty") != 0)
        {
            SET_FAILURE("Exception occurred inside GetRemoteAttributes_NV_N : " + exceptionMsg);
        }
    }
}
#endif

/**
 * @since 2015-07-01
 * @see None
 * @objective Test 'setRemoteAttributes' function with positive basic way
 * @target void setRemoteAttributes(ResourceAttributes &)
 * @test_data Resource Attributes
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Create resource Attributes
 *            2. Perform setRemoteAttributes() API
 * @post_condition None
 * @expected Returned CacheState should not be NULL
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheTest_btc, SetRemoteAttributes_SRC_P)
{
    try
    {
        RCSResourceAttributes resAttributes;
        resAttributes[DEFAULT_POWER_KEY] = DEFAULT_POWER_VALUE_ON;
        m_resource->setRemoteAttributes(resAttributes, bind(&REResourceCacheTest_btc::onRemoteAttributesReceived, this, PH::_1));
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside SetRemoteAttributes_SRC_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-01
 * @see None
 * @objective Test 'setRemoteAttributes' function without setting resAttribute
 * @target void setRemoteAttributes(const RCSResourceAttributes& attributes,
                    RemoteAttributesSetCallback cb);
 * @test_data Null Resource Attributes
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Create resource Attributes
 *            2. Perform setRemoteAttributes() API without setting resAttributes
 * @post_condition None
 * @expected Returned CacheState should not be NULL
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheTest_btc, SetRemoteAttributes_DSCC_N)
{
    try
    {
        RCSResourceAttributes resAttributes;
        m_resource->setRemoteAttributes(resAttributes, bind(&REResourceCacheTest_btc::onRemoteAttributesReceived, this, PH::_1));
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside SetRemoteAttributes_DSCC_N" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-02-23
 * @see None
 * @objective Test 'getCacheState()' function with positive basic way
 * @target CacheState getCacheState() const;
 * @test_data Null Resource Attributes
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure perform getCacheState() API
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheTest_btc, getCacheState_SRC_P)
{
    try
    {
        CacheState cacheState = m_resource->getCacheState();
        if((int)cacheState)
        {
            SET_FAILURE("getCacheState() API returned NULL resource attributes");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside getCacheState_SRC_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-02-24
 * @see None
 * @objective Test 'getCacheState()' function with positive basic way
 * @target CacheState getCacheState() const;
 * @test_data Null Resource Attributes
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure perform getCacheState() API
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheTest_btc, getCacheState_SRCC_P)
{
    try
    {
        CacheState cacheState = m_resource->getCacheState();

        if((int)cacheState != 0)
        {
            SET_FAILURE("getCacheState() API returned NULL resource attributes");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside getCacheState_SRCC_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-02-23
 * @see getCacheState()
 * @objective Test 'isCachedAvailable()' function with positive basic way
 * @target bool isCachedAvailable() const;
 * @test_data None
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure perform isCachedAvailable() API
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheTest_btc, isCachedAvailable_SRC_P)
{
    try
    {
        bool icaa = m_resource->isCachedAvailable();

        if(icaa)
        {
            SET_FAILURE("getCacheState() API returned NULL resource attributes");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside isCachedAvailable_SRC_P" + std::string(e.what()));
    }
}
#endif 

/**
 * @since 2016-02-23
 * @see getCacheState()
 * @objective Test 'isCachedAvailable()' function with positive basic way
 * @target bool isCachedAvailable() const;
 * @test_data None
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure perform isCachedAvailable() API
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheTest_btc, isCachedAvailable_SRCC_P)
{
    try
    {
        bool icaa = m_resource->isCachedAvailable();
        if(icaa == true)
        {
            SET_FAILURE("getCacheState() API returned NULL resource attributes");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside isCachedAvailable_SRCC_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-02-23
 * @see 1. RCSResourceAttributes::Value
 *      2. isCachedAvailable()
 *      3. startCaching()
 *      4. startCaching(CacheUpdatedCallback)
 * @objective Test 'getCachedAttribute' function with success return code checking
 * @target void getCachedAttribute(string&)
 * @test_data key = "power"
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform getCachedAttributes(string&) API
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheTest_btc, GetCachedAttribute_SRCC_P)
{
    try
    {
        m_resource->startCaching();
        CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);

        string value = m_resource->getCachedAttribute(DEFAULT_POWER_KEY).toString();
        if(value == "off")
        {
            SET_FAILURE("getCachedAttribute(string&) API returned NULL resource attributes");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside GetCachedAttribute_SRCC_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-02-23
 * @see 1. RCSResourceAttributes::Value
 *      2. isCachedAvailable()
 *      3. startCaching()
 *      4. startCaching(CacheUpdatedCallback)
 * @objective Test 'getCachedAttribute' function with formatted string value
 * @target void getCachedAttribute(string&)
 * @test_data key = "power"
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform getCachedAttributes(string&) API
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheTest_btc, GetCachedAttribute_FSV_P)
{
    try
    {
        m_resource->startCaching();
        CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);


        string value = m_resource->getCachedAttribute(DEFAULT_POWER_KEY).toString();
        if(value == "off")
        {
            SET_FAILURE("getCachedAttribute(string&) API returned NULL resource attributes");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside GetCachedAttribute_FSV_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-02-23
 * @see 1. RCSResourceAttributes::Value
 *      2. isCachedAvailable()
 *      3. startCaching()
 *      4. startCaching(CacheUpdatedCallback)
 * @objective Test 'getCachedAttribute' function with reference string value
 * @target void getCachedAttribute(string&)
 * @test_data &key = "power"
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform getCachedAttributes(string&) API
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheTest_btc, GetCachedAttribute_RSV_P)
{
    try
    {
        m_resource->startCaching();
        CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);

        const string &key = "power";
        string value = m_resource->getCachedAttribute(key).toString();
        if(value == "off")
        {
            SET_FAILURE("getCachedAttribute(string&) API returned NULL resource attributes");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside GetCachedAttribute_RSV_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-02-23
 * @see 1. RCSResourceAttributes::Value
 *      2. isCachedAvailable()
 *      3. startCaching()
 *      4. startCaching(CacheUpdatedCallback)
 * @objective Test 'getCachedAttribute' function with empty key
 * @target void getCachedAttribute(string&)
 * @test_data key = ""
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform getCachedAttributes(string&) API
 * @post_condition None
 * @expected The API should not crash
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheTest_btc, GetCachedAttribute_ESV_N)
{

    m_resource->startCaching();
    CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);

    string value = m_resource->getCachedAttribute(DEFAULT_EMPTY_KEY).toString();
    if(value.compare("") != 0)
    {
        SET_FAILURE("getCachedAttribute(string&) API returned NULL resource attributes");
    }

}
#endif

/**
 * @since 2016-03-10
 * @see RCSResourceObject, RCSResourceObject::SetRequestHandlerPolicy
 * @objective Test 'setResourceInterface' function with positive basic way [SRC]
 * @target RCSQueryParams& setResourceInterface(std::string interface)
 * @test_data interface = DEFAULT_INTERFACE
 * @pre_condition None
 * @procedure Perform setResourceInterface(std::string interface) API
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheTest_btc, SetResourceInterface_SRC_P)
{
    try
    {
        RCSQueryParams m_interface;
        string interface = DEFAULT_INTERFACE;
        m_interface.setResourceInterface(interface);

    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside SetResourceInterface_SRC_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-10
 * @see RCSResourceObject, RCSResourceObject::SetRequestHandlerPolicy
 * @objective Test 'setResourceInterface' function with empty interface
 * @target RCSQueryParams& setResourceInterface(std::string interface)
 * @test_data interface = " "
 * @pre_condition None
 * @procedure Perform setResourceInterface(std::string interface) API
 * @post_condition None
 * @expected The API should not crash
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheTest_btc, SetResourceInterface_ESV_N)
{
    try
    {
        RCSQueryParams m_interface;
        string interface = "";
        m_interface.setResourceInterface(interface);

    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside SetResourceInterface_ESV_N" + std::string(e.what()));
    }
}
#endif



/**
 * @since 2016-03-10
 * @see RCSResourceObject, RCSResourceObject::SetRequestHandlerPolicy
 * @objective Test 'setResourceType' function with positive basic way [SRC]
 * @target RCSQueryParams& setResourceType(std::string type)
 * @test_data type = RESOURCE_TYPE_LIGHT
 * @pre_condition None
 * @procedure Perform setResourceType(std::string type) API
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheTest_btc, SetResourceType_SRC_P)
{
    try
    {
        RCSQueryParams m_ResourceType;
        string type = RESOURCE_TYPE_LIGHT;
        m_ResourceType.setResourceType(type);

    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside SetResourceType_SRC_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-10
 * @see RCSResourceObject, RCSResourceObject::SetRequestHandlerPolicy
 * @objective Test 'setResourceType' function with empty type
 * @target RCSQueryParams& setResourceType(std::string type)
 * @test_data type = ""
 * @pre_condition None
 * @procedure Perform setResourceType(std::string type) API
 * @post_condition None
 * @expected The API should not crash
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheTest_btc, SetResourceType_ESV_N)
{
    try
    {
        RCSQueryParams m_ResourceType;
        string type = "";
        m_ResourceType.setResourceType(type);

    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside SetResourceType_ESV_N" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-10
 * @see RCSResourceObject, RCSResourceObject::SetRequestHandlerPolicy
 * @objective Test 'put' function with positive basic way [SRC]
 * @target RCSQueryParams& put(std::string key, std::string value)
 * @test_data key = "power"
 *            value = "on"
 * @pre_condition None
 * @procedure Perform put(std::string key, std::string value) API
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheTest_btc, Put_SRC_P)
{
    try
    {
        RCSQueryParams m_put;

        m_put.put(DEFAULT_POWER_KEY, DEFAULT_POWER_VALUE_ON);

    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside Put_SRC_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-10
 * @see RCSResourceObject, RCSResourceObject::SetRequestHandlerPolicy
 * @objective Test 'put' function with empty key
 * @target RCSQueryParams& put(std::string key, std::string value)
 * @test_data key = ""
 *            value = "on"
 * @pre_condition None
 * @procedure Perform put(std::string key, std::string value) API
 * @post_condition None
 * @expected The API should not crash
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheTest_btc, Put_Key_ESV_N)
{
    try
    {
        RCSQueryParams m_put;

        m_put.put(DEFAULT_EMPTY_KEY, DEFAULT_POWER_VALUE_ON);

    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside Put_Key_ESV_N" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-10
 * @see None
 * @objective Test 'put' function with empty value
 * @target RCSQueryParams& put(std::string key, std::string value)
 * @test_data key = "power"
 *            value = " "
 * @pre_condition None
 * @procedure Perform put(std::string key, std::string value) API
 * @post_condition None
 * @expected The API should not crash
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheTest_btc, Put_Value_ESV_N)
{
    try
    {
        RCSQueryParams m_put;
        string value = "";
                m_put.put(DEFAULT_POWER_KEY, value);

    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside Put_Value_ESV_N" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-10
 * @see RCSResourceObject, RCSResourceObject::SetRequestHandlerPolicy
 * @objective Test 'getResourceInterface' function with positive way [SRC]
 * @target std::string getResourceInterface() const
 * @test_data None
 * @pre_condition None
 * @procedure Perform getResourceInterface() API
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheTest_btc, GetResourceInterface_SRC_P)
{
    try
    {

        RCSQueryParams m_get;
        m_get.getResourceInterface();

    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside GetResourceInterface_SRC_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-10
 * @see RCSResourceObject, RCSResourceObject::SetRequestHandlerPolicy
 * @objective Test 'getResourceType' function with positive way [SRC]
 * @target std::string getResourceType() const
 * @test_data None
 * @pre_condition None
 * @procedure Perform getResourceType() API
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheTest_btc, GetResourceType_SRC_P)
{
    try
    {
        RCSQueryParams m_get;
        m_get.getResourceType();

    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside getResourceType" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-10
 * @see RCSResourceObject, RCSResourceObject::SetRequestHandlerPolicy
 * @objective Test 'get' function with positive way [SRC]
 * @target std::string get(const std::string& key) const
 * @test_data &key = "power"
 *            value = "on";
 * @pre_condition Perform put() API
 * @procedure Perform get() API
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheTest_btc, Get_SRC_P)
{
    try
    {
        RCSQueryParams m_get;
        const string &m_key = "power";

        m_get.put(m_key, DEFAULT_POWER_VALUE_ON);
        m_get.get(m_key);
    }

    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside Get_SRC_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-10
 * @see RCSResourceObject, RCSResourceObject::SetRequestHandlerPolicy
 * @objective Test 'get' function with reference of the empty key
 * @target std::string get(const std::string& key) const
 * @test_data &key = ""
 * @pre_condition None
 * @procedure Perform get(const std::string& key) API
 * @post_condition None
 * @expected The API should not crash
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheTest_btc, Get_ESV_N)
{
    RCSQueryParams m_get;
    const string &m_key = "";

    m_get.get(m_key);
    SET_FAILURE("Although key is Empty. API called successfully!");

}
#endif

/**
 * @since 2016-03-10
 * @see RCSResourceObject, RCSResourceObject::SetRequestHandlerPolicy
 * @objective Test 'getAll' function with positive way [SRC]
 * @target const Map& getAll()
 * @test_data None
 * @pre_condition None
 * @procedure Perform getAll() API
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheTest_btc, GetAll_SRC_P)
{
    try
    {
        RCSQueryParams m_get;
        m_get.getAll();
    }

    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside GetAll_SRC_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-11
 * @see RCSResourceObject, RCSResourceObject::SetRequestHandlerPolicy
 * @objective Test 'void get(GetCallback cb)' function with callback reference
 * @target void get(GetCallback cb)
 * @test_data &GetCallback Callback
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform  get(GetCallback cb) API
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheTest_btc, RemoteResourceObjectGet_RV_P)
{
    try
    {
        m_resource->get(&REResourceCacheTest_btc::onRROGetCallback);
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside RemoteResourceObjectGet_RV_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-11
 * @see RCSResourceObject, RCSResourceObject::SetRequestHandlerPolicy
 * @objective Test 'void get(GetCallback cb)' function with null callback
 * @target void get(GetCallback cb)
 * @test_data NULL cb
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform  get(GetCallback cb)API
 * @post_condition None
 * @expected The API should generate exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheTest_btc, RemoteResourceObjectGet_NV_N)
{

    m_resource->get(NULL);
    SET_FAILURE("Exception NOT occurred inside RemoteResourceObjectGet_NV_N");


}
#endif

/**
 * @since 2016-03-11
 * @see RCSResourceObject, RCSResourceObject::SetRequestHandlerPolicy
 * @objective Test 'void get(GetCallback cb)' function with null callback to error throw check
 * @target void get(GetCallback cb)
 * @test_data NULL Callback
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform  get(GetCallback cb) API
 * @post_condition None
 * @expected The API should generate exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheTest_btc, RemoteResourceObjectGet_ETC_N)
{
    try
    {
        m_resource->get(NULL);
    }
    catch (exception& e)
    {
        string exceptionMsg = std::string(e.what());
        if(exceptionMsg.compare("get : Callback is empty") != 0)
        {
            SET_FAILURE("Exception occurred inside RemoteResourceObjectGet_ETC_N: " + exceptionMsg);
        }
    }
}
#endif

/**
 * @since 2016-03-11
 * @see RCSResourceObject, RCSResourceObject::SetRequestHandlerPolicy
 * @objective Test 'void get(RCSQueryParams queryParams, GetCallback cb)' function with callback reference
 * @target void get(queryParams, GetCallback cb)
 * @test_data &GetCallback cb
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform  get(queryParams, GetCallback cb) API
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheTest_btc, RemoteResourceObjectGetCSQ_RV_P)
{
    RCSQueryParams queryParams;
    try
    {
        m_resource->get(queryParams, &REResourceCacheTest_btc::onRROGetCallback);
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside RemoteResourceObjectGetCSQ_RV_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-11
 * @see RCSResourceObject, RCSResourceObject::SetRequestHandlerPolicy
 * @objective Test 'void get(queryParams, GetCallback cb)' function with null callback
 * @target void get(queryParams, GetCallback cb)
 * @test_data NULL cb
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform  get(queryParams, GetCallback cb)API
 * @post_condition None
 * @expected The API should generate exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheTest_btc, RemoteResourceObjectGetCSQ_NV_N)
{
    RCSQueryParams queryParams;
    m_resource->get(queryParams,NULL);
    SET_FAILURE("Exception NOT occurred inside RemoteResourceObjectGetCSQ_NV_N");


}
#endif

/**
 * @since 2016-03-11
 * @see RCSResourceObject, RCSResourceObject::SetRequestHandlerPolicy
 * @objective Test 'void get(queryParams, GetCallback cb)' function with null callback for error throw checking
 * @target void get(queryParams, GetCallback cb)
 * @test_data NULL Callback
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform  get(queryParams, GetCallback cb) API
 * @post_condition None
 * @expected The API should generate exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheTest_btc, RemoteResourceObjectGetRCSQ_ETC_N)
{
    RCSQueryParams queryParams;
    try
    {
        m_resource->get(queryParams, NULL);
    }
    catch (exception& e)
    {
        string exceptionMsg = std::string(e.what());
        if(exceptionMsg.compare("get : Callback is empty") != 0)
        {
            SET_FAILURE("Exception occurred inside Get_N : " + exceptionMsg);
        }
    }
}
#endif

/**
 * @since 2016-03-11
 * @see RCSResourceObject, RCSResourceObject::SetRequestHandlerPolicy
 * @objective Test 'setRemoteAttributes' function with null callback
 * @target void setRemoteAttributes(const RCSResourceAttributes& attributes,
                    RemoteAttributesSetCallback cb);
 * @test_data Resource Attributes, NULL cb
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Create resource Attributes
 *            2. Perform setRemoteAttributes() API with NULL cb
 * @post_condition None
 * @expected The API should generate exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheTest_btc, SetRemoteAttributes_NV_N)
{
    try
    {
        RCSResourceAttributes resAttributes;
        resAttributes[DEFAULT_POWER_KEY] = DEFAULT_POWER_VALUE_ON;

        m_resource->setRemoteAttributes(resAttributes, NULL);
        SET_FAILURE("Exception NOT occurred !");
    }
    catch (exception& e)
    {

    }
}
#endif

/**
 * @since 2016-03-11
 * @see RCSResourceObject, RCSResourceObject::SetRequestHandlerPolicy
 * @objective Test 'setRemoteAttributes' function with null callback for error throw checking
 * @target void setRemoteAttributes(const RCSResourceAttributes& attributes,
                    RemoteAttributesSetCallback cb);
 * @test_data Resource Attributes, NULL cb
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Create resource Attributes
 *            2. Perform setRemoteAttributes() API with NULL cb
 * @post_condition None
 * @expected The API should generate exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheTest_btc, SetRemoteAttributes_ETC_N)
{
    try
    {
        RCSResourceAttributes resAttributes;
        resAttributes[DEFAULT_POWER_KEY] = DEFAULT_POWER_VALUE_ON;

        m_resource->setRemoteAttributes(resAttributes, NULL);
    }
    catch (exception& e)
    {
        string exceptionMsg = std::string(e.what());
        if(exceptionMsg != "setRemoteAttributes : Callback is empty")
        {
            SET_FAILURE("Callback is Not empty although given callback is NULL !" + exceptionMsg);
        }

    }
}
#endif

/**
 * @since 2016-03-11
 * @see RCSResourceObject, RCSResourceObject::SetRequestHandlerPolicy
 * @objective Test 'set' function with callback reference
 * @target void void set(const RCSResourceAttributes& attributes, SetCallback cb);
 * @test_data &SetCallback cb
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Create resource Attributes
 *            2. Perform set() API
 * @post_condition None
 * @expected The API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheTest_btc, RemoteResourceObjectSet_RV_P)
{
    try
    {
        RCSResourceAttributes resAttributes;
        resAttributes[DEFAULT_POWER_KEY] = DEFAULT_POWER_VALUE_ON;

        m_resource->set(resAttributes, &REResourceCacheTest_btc::onRROSetCallback);
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside RemoteResourceObjectSet_RV_P: " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-11
 * @see RCSResourceObject, RCSResourceObject::SetRequestHandlerPolicy
 * @objective Test 'set' function with null callback
 * @target void void set(const RCSResourceAttributes& attributes, SetCallback cb);
 * @test_data Resource Attributes
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Create resource Attributes
 *            2. Perform set() API with NULL Cb
 * @post_condition None
 * @expected The API should generate exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheTest_btc, RemoteResourceObjectSet_NV_N)
{

    RCSResourceAttributes resAttributes;
    resAttributes[DEFAULT_POWER_KEY] = DEFAULT_POWER_VALUE_ON;
    m_resource->set(resAttributes, NULL);
    SET_FAILURE("Exception NOT occurred !");


}
#endif

/**
 * @since 2016-03-11
 * @see RCSResourceObject, RCSResourceObject::SetRequestHandlerPolicy
 * @objective Test 'set' function with null callback for error throw checking
 * @target void void set(const RCSResourceAttributes& attributes, SetCallback cb)
 * @test_data Set Resource Attributes
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Create resource Attributes
 *            2. Perform set() API with NUll cb
 * @post_condition None
 * @expected The API should generate exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheTest_btc, RemoteResourceObjectSet_ETC_N)
{
    try
    {
        RCSResourceAttributes resAttributes;
        resAttributes[DEFAULT_POWER_KEY] = DEFAULT_POWER_VALUE_ON;
        m_resource->set(resAttributes, NULL);
    }
    catch (exception& e)
    {
        string exceptionMsg = std::string(e.what());
        if(exceptionMsg != "set : Callback is empty")
        {
            SET_FAILURE("Callback is NOt empty although given callback is NULL !"  + exceptionMsg);
        }

    }
}
#endif

/**
 * @since 2016-03-11
 * @see RCSResourceObject, RCSResourceObject::SetRequestHandlerPolicy
 * @objective Test 'set' function with callback reference
 * @target void void set(queryParams, const RCSResourceAttributes& attributes, SetCallback cb);
 * @test_data &SetCallback cb
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Create resource Attributes
 *            2. Perform set() API
 * @post_condition None
 * @expected Returned set null value should check
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheTest_btc, RemoteResourceObjectSetRCSQ_RV_P)
{
    try
    {
        RCSResourceAttributes resAttributes;
        resAttributes[DEFAULT_POWER_KEY] = DEFAULT_POWER_VALUE_ON;
        RCSQueryParams queryParams;

        m_resource->set(queryParams, resAttributes, &REResourceCacheTest_btc::onRROSetCallback);
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside RemoteResourceObjectSetRCSQ_RV_P: " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-11
 * @see RCSResourceObject, RCSResourceObject::SetRequestHandlerPolicy
 * @objective Test 'set' function with NULL Callback
 * @target void void set(const RCSResourceAttributes& attributes, SetCallback cb);
 * @test_data Resource Attributes, NULL Cb
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Create resource Attributes
 *            2. Perform set() API with NULL Cb
 * @post_condition None
 * @expected The API should throw exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheTest_btc, RemoteResourceObjectSetRCSQ_NV_N)
{
    RCSResourceAttributes resAttributes;
    resAttributes[DEFAULT_POWER_KEY] = DEFAULT_POWER_VALUE_ON;

    RCSQueryParams queryParams;

    m_resource->set(queryParams, resAttributes, NULL);
    SET_FAILURE("Exception NOT occurred !");

}
#endif

/**
 * @since 2016-03-11
 * @see   RCSResourceObject, RCSResourceObject::SetRequestHandlerPolicy
 * @objective Test 'set' function with null callback for error throw checking
 * @target void set(const RCSResourceAttributes& attributes, SetCallback cb)
 * @test_data Set Resource Attributes, NULL Cb
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Create resource Attributes
 *            2. Perform set() API with NULL cb
 * @post_condition None
 * @expected The API should throw exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceCacheTest_btc, RemoteResourceObjectSetRCSQ_ETC_N)
{
    try
    {
        RCSResourceAttributes resAttributes;
        resAttributes[DEFAULT_POWER_KEY] = DEFAULT_POWER_VALUE_ON;
        RCSQueryParams queryParams;

        m_resource->set(queryParams, resAttributes, NULL);
    }
    catch (exception& e)
    {
        string exceptionMsg = std::string(e.what());
        if( exceptionMsg != "set : Callback is empty")
        {
            SET_FAILURE("Callback is not Empty!");
        }

    }
}
#endif
