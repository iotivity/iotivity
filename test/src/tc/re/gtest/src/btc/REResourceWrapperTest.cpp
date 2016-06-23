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

class REResourceWrapperTest_btc: public ::testing::Test {
protected:

    REHelper* m_pREHelper;
    std::string m_errorMsg;
    shared_ptr<RCSRemoteResourceObject> m_resource;
    ResourceState m_resourceState;
    RCSResourceAttributes m_cacheUpdateAttributes;
    RCSResourceAttributes m_remoteResourceAttributes;

    virtual void SetUp() {
        CommonUtil::runCommonTCSetUpPart();

#ifdef __LINUX__   		
        CommonUtil::launchApp(RE_SERVER_APP);
        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);
#endif
        m_pREHelper = REHelper::getInstance();
        bool isResourceAvailable = m_pREHelper->findPrimitiveResources(
                RESOURCE_TYPE_LIGHT);
        if (isResourceAvailable) {
            IOTIVITYTEST_LOG(DEBUG, "Resource found!!");
            m_resource = m_pREHelper->getFoundResourceList().at(0);
        } else {
            SET_FAILURE("Precondition Failed, No Resource Found!!");
        }
        m_errorMsg = "";
    }

    virtual void TearDown() {
#ifdef __LINUX__  		
        CommonUtil::killApp(RE_SERVER_APP);
        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);
#endif		
        CommonUtil::runCommonTCTearDownPart();
    }

public:
    void OnResourceDiscovered(
            shared_ptr<OIC::Service::RCSRemoteResourceObject> pResource) {
    }

    void onResourceStateChanged(ResourceState resourceState) {
        m_resourceState = resourceState;
    }

    void onRemoteAttributesReceived(
            RCSResourceAttributes & resourceAttributes) {
        m_remoteResourceAttributes = resourceAttributes;
    }
};

/**
 * @since 2015-07-01
 * @see None
 * @objective Test 'getUri' function with positive basic way
 * @target string getUri()
 * @test_data None
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform getUri() API
 * @post_condition None
 * @expected Returned uri should not be empty
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperTest_btc, GetUri_SRC_P)
{
    string uri = "";
    if (m_resource)
    {
        uri = m_resource->getUri();

        if (uri.compare("")== 0)
        {
            SET_FAILURE("Unable to get uri");
        }
    }

}
#endif

/**
 * @since 2016-02-23
 * @see None
 * @objective Test 'getUri' function with positive basic way
 * @target string getUri()
 * @test_data None
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform getUri() API
 * @post_condition None
 * @expected Returned uri should not be empty
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperTest_btc, GetUri_SRCC_P)
{
    string uri = "";
    if (m_resource)
    {
        uri = m_resource->getUri();

        if (uri != "/device/tube-light")
        {
            SET_FAILURE("Unable to get uri");
        }
    }

}
#endif

/**
 * @since 2015-07-01
 * @see None
 * @objective Test 'getAddress' function with positive basic way
 * @target string getAddress()
 * @test_data None
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform getAddress() API
 * @post_condition None
 * @expected Returned address should not be empty
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperTest_btc, GetAddress_SRC_P)
{
    string address = "";
    address = m_resource->getAddress();

    if (address.compare("") == 0)
    {
        SET_FAILURE("Unable to get address");
    }
}
#endif

/**
 * @since 2015-07-01
 * @see None
 * @objective Test 'getTypes' function with positive basic way
 * @target std::vector< std::string > getTypes()
 * @test_data None
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform getTypes() API
 * @post_condition None
 * @expected 1. Returned type list should not be empty
 *           2. At least one type should not be an empty string
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperTest_btc, GetTypes_SRC_P)
{
    vector< string > typeList;
    typeList.clear();
    typeList = m_resource->getTypes();

    if (typeList.size() > 0 && typeList.at(0).compare("") == 0)
    {
        SET_FAILURE("Unable to get types");
    }
}
#endif

/**
 * @since 2015-07-01
 * @see None
 * @objective Test 'getTypes' function with positive basic way
 * @target std::vector< std::string > getTypes()
 * @test_data None
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform getTypes() API
 * @post_condition None
 * @expected 1. Returned type list should not be empty
 *           2. At least one type should not be an empty string
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperTest_btc, GetTypes_SRCC_P)
{
    vector< string > typeList;
    typeList.clear();
    typeList = m_resource->getTypes();

    if (typeList[0] != "core.light")
    {
        SET_FAILURE("Unable to get types");
    }
}
#endif

/**
 * @since 2015-07-01
 * @see None
 * @objective Test 'getInterfaces' function with positive basic way
 * @target std::vector< std::string > getInterfaces()
 * @test_data None
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform getInterfaces() API
 * @post_condition None
 * @expected 1. Returned interface list should not be empty
 *           2. At least one interface should not be an empty string
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperTest_btc, GetInterfaces_SRC_P)
{
    vector< string > addressList;
    addressList.clear();
    addressList = m_resource->getInterfaces();

    if (addressList.size() > 0 && addressList.at(0).compare("") == 0)
    {
        SET_FAILURE("Unable to get interfaces");
    }
}
#endif

/**
 * @since 2015-07-01
 * @see None
 * @objective Test 'getInterfaces' function with positive basic way
 * @target std::vector< std::string > getInterfaces()
 * @test_data None
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform getInterfaces() API
 * @post_condition None
 * @expected 1. Returned interface list should not be empty
 *           2. At least one interface should not be an empty string
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperTest_btc, GetInterfaces_SRCC_P)
{
    vector< string > addressList;
    addressList.clear();
    addressList = m_resource->getInterfaces();

    if (addressList[0].empty())
    {
        SET_FAILURE("Unable to get interfaces");
    }
}
#endif

/**
 * @since 2015-07-07
 * @see None
 * @objective Test 'getInstance' function with positive basic way
 * @target RCSDiscoveryManager* getInstance()
 * @test_data None
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform getInstance() API
 * @post_condition None
 * @expected returned discovery manager should not be null
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperTest_btc, GetInstance_SRC_P)
{
    RCSDiscoveryManager* discoveryManager = RCSDiscoveryManager::getInstance();

    if (discoveryManager == NULL)
    {
        SET_FAILURE("Unable to get instance of discovery manager");
    }
}
#endif

/**
 * @since 2015-07-07
 * @see None
 * @objective Test 'discoverResourceByType' function with positive basic way
 * @target DiscoveryTask::Ptr discoverResourceByType(const RCSAddress& address,
 *         const std::string& relativeUri, const std::string& resourceType,
 *         ResourceDiscoveredCallback cb);
 * @test_data 1. host
 *            2. uri
 *            3. Resource Type
 *            4. Callback
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResourceByType() API
 * @post_condition None
 * @expected returned discovery manager should not be null
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperTest_btc, DiscoverResource_SRC_P)
{
    RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    string uri = OC_RSRVD_WELL_KNOWN_URI;

    try
    {
        std::unique_ptr<RCSDiscoveryManager::DiscoveryTask> discoveryTask;
        discoveryTask = RCSDiscoveryManager::getInstance()->discoverResourceByType(host,
                uri,RESOURCE_TYPE_LIGHT, std::bind(&REResourceWrapperTest_btc::OnResourceDiscovered, this, PH::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        discoveryTask->cancel();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while discovering resource");
    }
}
#endif

/**
 * @since 2015-07-07
 * @see None
 * @objective Test 'discoverResourceByType' function with empty uri
 * @target DiscoveryTask::Ptr discoverResourceByType(const RCSAddress& address,
 *          const std::string& relativeUri, const std::string& resourceType,
 *          ResourceDiscoveredCallback cb);
 * @test_data uri = "";
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResourceByType() API
 * @post_condition None
 * @expected The API should not crashed
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperTest_btc, DiscoverResourceUri_ESV_N)
{
    RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    string uri = "";
    try
    {
        std::unique_ptr<RCSDiscoveryManager::DiscoveryTask> discoveryTask;
        discoveryTask = RCSDiscoveryManager::getInstance()->discoverResourceByType(host,
                uri,RESOURCE_TYPE_LIGHT, std::bind(&REResourceWrapperTest_btc::OnResourceDiscovered, this, PH::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        discoveryTask->cancel();
    }
    catch (exception& e)
    {
        string exceptionMsg = string(e.what());
        if (exceptionMsg.compare("discoverResource NULL Callback'") != 0)
        {
            SET_FAILURE(
                    "Exception occurred while discovering resource, exception is: " + exceptionMsg);
        }
    }
}
#endif

/**
 * @since 2016-02-19
 * @see None
 * @objective Test 'discoverResourceByType' function with reference of the uri
 * @target DiscoveryTask::Ptr discoverResourceByType(const RCSAddress& address,
 *          const std::string& relativeUri, const std::string& resourceType,
 *          ResourceDiscoveredCallback cb);
 * @test_data 1. host
 *            2. uri
 *            3. resource Type
 *            4. Callback
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResourceByType() API with uri reference
 * @post_condition None
 * @expected returned discovery manager should not be null
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperTest_btc, DiscoverResource_RSV_P)
{
    RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    string uri = OC_RSRVD_WELL_KNOWN_URI;
    const string &adpass = uri;

    try
    {
        std::unique_ptr<RCSDiscoveryManager::DiscoveryTask> discoveryTask;
        discoveryTask = RCSDiscoveryManager::getInstance()->discoverResourceByType(host,
                adpass,RESOURCE_TYPE_LIGHT, std::bind(&REResourceWrapperTest_btc::OnResourceDiscovered, this, PH::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        discoveryTask->cancel();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while discovering resource");
    }
}
#endif

/**
 * @since 2016-02-17
 * @see None
 * @objective Test 'discoverResourceByType' function with null callback
 * @target DiscoveryTask::Ptr discoverResourceByType(const RCSAddress& address,
 *          const std::string& relativeUri, const std::string& resourceType,
 *          ResourceDiscoveredCallback cb);
 * @test_data cb = NULL
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResourceByType() API with Null Cb
 * @post_condition None
 * @expected API should not crashed
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperTest_btc, DiscoverResourceCB_NV_N)
{
    RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    string uri = OC_RSRVD_WELL_KNOWN_URI;
    try
    {
        std::unique_ptr<RCSDiscoveryManager::DiscoveryTask> discoveryTask;
        discoveryTask = RCSDiscoveryManager::getInstance()->discoverResourceByType(host,
                uri,RESOURCE_TYPE_LIGHT, NULL);
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        discoveryTask->cancel();
        SET_FAILURE("Callback NV Test Fail!");
    }
    catch (exception& e)
    {
        string exceptionMsg = string(e.what());
    }
}
#endif

/**
 * @since 2016-02-24
 * @see None
 * @objective Test 'discoverResourceByType' function with null callback for error throw checking
 * @target DiscoveryTask::Ptr discoverResourceByType(const RCSAddress& address,
 *          const std::string& relativeUri, const std::string& resourceType,
 *          ResourceDiscoveredCallback cb);
 * @test_data cb = NULL
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResourceByType() API
 * @post_condition None
 * @expected API should not crashed
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperTest_btc, DiscoverResourceCB_ECRC_N)
{
    RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    string uri = OC_RSRVD_WELL_KNOWN_URI;
    try
    {
        std::unique_ptr<RCSDiscoveryManager::DiscoveryTask> discoveryTask;
        discoveryTask = RCSDiscoveryManager::getInstance()->discoverResourceByType(host,
                uri,RESOURCE_TYPE_LIGHT, NULL);
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        discoveryTask->cancel();
    }
    catch (exception& e)
    {
        string exceptionMsg = string(e.what());
        if (exceptionMsg != "Callback is empty")
        {
            SET_FAILURE(
                    "Exception occurred while discovering resource, exception is: " + exceptionMsg);
        }
    }
}
#endif

/**
 * @since 2016-02-19
 * @see None
 * @objective Test 'discoverResourceByType' function with unformatted uri value
 * @target DiscoveryTask::Ptr discoverResourceByType(const RCSAddress& address,
 *          const std::string& relativeUri, const std::string& resourceType,
 *          ResourceDiscoveredCallback cb);
 * @test_data uri = "Hello"
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResourceByType() API with unformatted uri
 * @post_condition None
 * @expected API should not crashed 
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperTest_btc, DiscoverResource_USV_N)
{
    RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    string uri = "Hello";
    try
    {
        std::unique_ptr<RCSDiscoveryManager::DiscoveryTask> discoveryTask;
        discoveryTask = RCSDiscoveryManager::getInstance()->discoverResourceByType(host,
                uri,RESOURCE_TYPE_LIGHT, std::bind(&REResourceWrapperTest_btc::OnResourceDiscovered, this, PH::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        discoveryTask->cancel();
    }
    catch (exception& e)
    {
        string exceptionMsg = string(e.what());

        if (exceptionMsg != "Failed : Invalid URI")
        {
            SET_FAILURE(
                    "Exception occurred while discovering resource, exception is: " + exceptionMsg);
        }
    }
}
#endif

/**
 * @since 2016-02-19
 * @see None
 * @objective Test 'discoverResourceByType' function with unformatted uri reference value
 * @target DiscoveryTask::Ptr discoverResourceByType(const RCSAddress& address,
 *          const std::string& relativeUri, const std::string& resourceType,
 *          ResourceDiscoveredCallback cb);
 * @test_data &uri = "Hello"
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResourceByType() API with unformatted string reference
 * @post_condition None
 * @expected API should not crashed
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperTest_btc, DiscoverResourceHost_URSV_N)
{
    RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    string uri = "Hello";

    const string &adpass = uri;

    try
    {
        std::unique_ptr<RCSDiscoveryManager::DiscoveryTask> discoveryTask;
        discoveryTask = RCSDiscoveryManager::getInstance()->discoverResourceByType(host,
                adpass,RESOURCE_TYPE_LIGHT, std::bind(&REResourceWrapperTest_btc::OnResourceDiscovered, this, PH::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        discoveryTask->cancel();
    }
    catch (exception& e)
    {
        string exceptionMsg = string(e.what());
        if (exceptionMsg != "Failed : Invalid URI")
        {
            SET_FAILURE(
                    "Exception occurred while discovering resource, exception is: " + exceptionMsg);
        }
    }
}
#endif

/**
 * @since 2016-02-18
 * @see None
 * @objective Test 'discoverResourceByType' function with positive basic way
 * @target DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @test_data 1. host
 *            2. resource Type
 *            3. Callback
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResourceByType() API
 * @post_condition None
 * @expected returned discovery manager should not be null
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperTest_btc, DiscoverResourceWithoutUri_SRC_P)
{
    RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();

    try
    {
        std::unique_ptr<RCSDiscoveryManager::DiscoveryTask> discoveryTask;
        discoveryTask = RCSDiscoveryManager::getInstance()->discoverResourceByType(host,RESOURCE_TYPE_LIGHT, std::bind(&REResourceWrapperTest_btc::OnResourceDiscovered, this, PH::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        discoveryTask->cancel();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while discovering resource");
    }
}
#endif

/**
 * @since 2016-02-25
 * @see None
 * @objective Test 'discoverResourceByType' function with reference of the resource type
 * @target DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @test_data 1. host
 *            2. &resource Type
 *            3. Callback
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResourceByType() API with resource type's reference
 * @post_condition None
 * @expected returned discovery manager should not be null
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperTest_btc, DiscoverResourceWithoutUri_RSV_P)
{
    RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();

    const string &resType = RESOURCE_TYPE_LIGHT;

    try
    {
        std::unique_ptr<RCSDiscoveryManager::DiscoveryTask> discoveryTask;
        discoveryTask = RCSDiscoveryManager::getInstance()->discoverResourceByType(host,resType, std::bind(&REResourceWrapperTest_btc::OnResourceDiscovered, this, PH::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        discoveryTask->cancel();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while discovering resource");
    }
}
#endif

/**
 * @since 2016-02-25
 * @see None
 * @objective Test 'discoverResourceByType' function with unformatted resource type
 * @target DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @test_data 1. host
 *            2. resource Type  = "hello"
 *            3. Callback
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResourceByType() API with unformatted resource type
 * @post_condition None
 * @expected API should not crashed
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperTest_btc, DiscoverResourceWithoutUri_USV_N)
{
    RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();

    string resType = "hello";

    try
    {
        std::unique_ptr<RCSDiscoveryManager::DiscoveryTask> discoveryTask;
        discoveryTask = RCSDiscoveryManager::getInstance()->discoverResourceByType(host,resType, std::bind(&REResourceWrapperTest_btc::OnResourceDiscovered, this, PH::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        discoveryTask->cancel();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while discovering resource");
    }
}
#endif

/**
 * @since 2016-02-25
 * @see None
 * @objective Test 'discoverResourceByType' function with unformatted resource Type reference
 * @target DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @test_data 1. host
 *            2. &resource Type  = "hello"
 *            3. Callback
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResourceByType() API with unformatted resource type's reference
 * @post_condition None
 * @expected the API should not crash
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperTest_btc, DiscoverResourceWithoutUri_URSV_N)
{
    RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();

    const string &resType = "hello";

    try
    {
        std::unique_ptr<RCSDiscoveryManager::DiscoveryTask> discoveryTask;
        discoveryTask = RCSDiscoveryManager::getInstance()->discoverResourceByType(host,resType, std::bind(&REResourceWrapperTest_btc::OnResourceDiscovered, this, PH::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        discoveryTask->cancel();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while discovering resource");
    }
}
#endif

/**
 * @since 2016-02-18
 * @see None
 * @objective Test 'discoverResourceByType' function with null resource Type
 * @target DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @test_data 1. host
 *            2. resource Type  = NULL
 *            3. Callback
 * @test_data None
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResourceByType() API with NULL resource type
 * @post_condition None
 * @expected the api should not crash
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperTest_btc, DiscoverResourceWithoutUri_NV_N)
{
    RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    try
    {
        std::unique_ptr<RCSDiscoveryManager::DiscoveryTask> discoveryTask;
        discoveryTask = RCSDiscoveryManager::getInstance()->discoverResourceByType(host,NULL, std::bind(&REResourceWrapperTest_btc::OnResourceDiscovered, this, PH::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        discoveryTask->cancel();
        SET_FAILURE("NV Test Fail ! ");
    }
    catch (exception& e)
    {
        string exceptionMsg = string(e.what());
    }
}
#endif

/**
 * @since 2016-02-18
 * @see None
 * @objective Test 'discoverResourceByType' function with null callback
 * @target DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @test_data cb = NULL
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResourceByType() API with NULL cb
 * @post_condition None
 * @expected the api should not crash
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperTest_btc, DiscoverResourceWithoutUriCB_NV_N)
{
    RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    try
    {
        std::unique_ptr<RCSDiscoveryManager::DiscoveryTask> discoveryTask;
        discoveryTask = RCSDiscoveryManager::getInstance()->discoverResourceByType(host,RESOURCE_TYPE_LIGHT, NULL);
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        discoveryTask->cancel();
        SET_FAILURE("Callback NV Test Fail ! ");
    }
    catch (exception& e)
    {
        string exceptionMsg = string(e.what());
    }
}
#endif

/**
 * @since 2016-02-25
 * @see None
 * @objective Test 'discoverResourceByType' function with null callback for error throw checking
 * @target DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @test_data cb = NULL
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResourceByType() API with NUll cb
 * @post_condition None
 * @expected the API should generate exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperTest_btc, DiscoverResourceWithoutUriCB_ECRC_N)
{
    RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    try
    {
        std::unique_ptr<RCSDiscoveryManager::DiscoveryTask> discoveryTask;
        discoveryTask = RCSDiscoveryManager::getInstance()->discoverResourceByType(host,RESOURCE_TYPE_LIGHT, NULL);
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        discoveryTask->cancel();
    }
    catch (exception& e)
    {
        string exceptionMsg = string(e.what());
        if (exceptionMsg != "Callback is empty")
        {
            SET_FAILURE(
                    "Exception occurred while discovering resource, exception is: " + exceptionMsg);
        }
    }
}
#endif

/**
 * @since 2016-02-18
 * @see None
 * @objective Test 'discoverResource' function with positive basic way
 * @target DiscoveryTask::Ptr discoverResource(const RCSAddress& address,
 *                 const std::string& relativeUri, ResourceDiscoveredCallback cb);
 * @test_data 1. host
 *            2. uri
 *            3. Callback
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResource() API
 * @post_condition None
 * @expected returned discovery manager should not be null
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperTest_btc, DiscoverResourceWithoutResType_SRC_P)
{
    RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    string uri = OC_RSRVD_WELL_KNOWN_URI;

    try
    {
        std::unique_ptr<RCSDiscoveryManager::DiscoveryTask> discoveryTask;
        discoveryTask = RCSDiscoveryManager::getInstance()->discoverResource(host,uri, std::bind(&REResourceWrapperTest_btc::OnResourceDiscovered, this, PH::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        discoveryTask->cancel();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while discovering resource");
    }
}
#endif

/**
 * @since 2016-02-25
 * @see None
 * @objective Test 'discoverResource' function with uri reference value
 * @target vDiscoveryTask::Ptr discoverResource(const RCSAddress& address,
 *                 const std::string& relativeUri, ResourceDiscoveredCallback cb);
 * @test_data 1. host
 *            2. &uri
 *            3. Callback
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResource() API with uri reference
 * @post_condition None
 * @expected API should not generate exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperTest_btc, DiscoverResourceWithoutResType_RSV_P)
{
    RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    string uri = OC_RSRVD_WELL_KNOWN_URI;

    const string &adpass = uri;

    try
    {
        std::unique_ptr<RCSDiscoveryManager::DiscoveryTask> discoveryTask;
        discoveryTask = RCSDiscoveryManager::getInstance()->discoverResource(host,
                adpass, std::bind(&REResourceWrapperTest_btc::OnResourceDiscovered, this, PH::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        discoveryTask->cancel();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while discovering resource");
    }
}
#endif

/**
 * @since 2016-02-18
 * @see None
 * @objective Test 'discoverResource' function with empty uri
 * @target vDiscoveryTask::Ptr discoverResource(const RCSAddress& address,
 *                 const std::string& relativeUri, ResourceDiscoveredCallback cb);
 * @test_data 1. host
 *            2. uri = ""
 *            3. Callback
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResource() API
 * @post_condition None
 * @expected The API should not crash
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperTest_btc, DiscoverResourceWithoutResType_ESV_N)
{
    RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    string uri = "";

    try
    {
        std::unique_ptr<RCSDiscoveryManager::DiscoveryTask> discoveryTask;
        discoveryTask = RCSDiscoveryManager::getInstance()->discoverResource(host,uri, std::bind(&REResourceWrapperTest_btc::OnResourceDiscovered, this, PH::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        discoveryTask->cancel();
    }
    catch (exception& e)
    {
        string exceptionMsg = string(e.what());
        if (exceptionMsg.compare("discoverResource NULL Callback'") != 0)
        {
            SET_FAILURE(
                    "Exception occurred while discovering resource, exception is: " + exceptionMsg);
        }
    }
}
#endif

/**
 * @since 2016-02-18
 * @see None
 * @objective Test 'discoverResource' function with null callback
 * @target vDiscoveryTask::Ptr discoverResource(const RCSAddress& address,
 *                 const std::string& relativeUri, ResourceDiscoveredCallback cb);
 * @test_data 1. host
 *            2. uri
 *            3. Callback = NULL
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResource() API
 * @post_condition None
 * @expected The API should not crash
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperTest_btc, DiscoverResourceWithoutResTypeCB_NV_N)
{
    RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    string uri = OC_RSRVD_WELL_KNOWN_URI;
    try
    {
        std::unique_ptr<RCSDiscoveryManager::DiscoveryTask> discoveryTask;
        discoveryTask = RCSDiscoveryManager::getInstance()->discoverResource(host,uri, NULL);
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        discoveryTask->cancel();
        SET_FAILURE("CallBack ECRC Test Fail ");
    }
    catch (exception& e)
    {
        string exceptionMsg = string(e.what());
    }
}
#endif

/**
 * @since 2016-02-25
 * @see None
 * @objective Test 'discoverResource' function with null callback for error throw checking
 * @target vDiscoveryTask::Ptr discoverResource(const RCSAddress& address,
 *                 const std::string& relativeUri, ResourceDiscoveredCallback cb);
 * @test_data 1. host
 *            2. uri
 *            3. Callback = NULL
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResource() API
 * @post_condition None
 * @expected The API should generate exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperTest_btc, DiscoverResourceWithoutResTypeCB_ECRC_N)
{
    RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    string uri = OC_RSRVD_WELL_KNOWN_URI;
    try
    {
        std::unique_ptr<RCSDiscoveryManager::DiscoveryTask> discoveryTask;
        discoveryTask = RCSDiscoveryManager::getInstance()->discoverResource(host,uri, NULL);
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        discoveryTask->cancel();
    }
    catch (exception& e)
    {
        string exceptionMsg = string(e.what());
        if (exceptionMsg != "Callback is empty")
        {
            SET_FAILURE(
                    "Exception occurred while discovering resource, exception is: " + exceptionMsg);
        }
    }
}
#endif

/**
 * @since 2016-02-19
 * @see None
 * @objective Test 'discoverResource' function with unformatted uri
 * @target void DiscoveryTask::Ptr discoverResource(const RCSAddress& address,
 *                 const std::string& relativeUri, ResourceDiscoveredCallback cb);
 * @test_data 1. host
 *            2. uri = "Hello"
 *            3. Callback
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResource() API with unformatted uri
 * @post_condition None
 * @expected API should not crashed
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperTest_btc, DiscoverResourceWithoutResType_USV_N)
{
    RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    string uri = "Hello";
    try
    {
        std::unique_ptr<RCSDiscoveryManager::DiscoveryTask> discoveryTask;
        discoveryTask = RCSDiscoveryManager::getInstance()->discoverResource(host,
                uri, std::bind(&REResourceWrapperTest_btc::OnResourceDiscovered, this, PH::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        discoveryTask->cancel();
    }
    catch (exception& e)
    {
        string exceptionMsg = string(e.what());

        if (exceptionMsg.compare("Failed : Invalid URI") != 0)
        {
            SET_FAILURE(
                    "Exception occurred while discovering resource, exception is: " + exceptionMsg);
        }
    }
}
#endif

/**
 * @since 2016-02-19
 * @see None
 * @objective Test 'discoverResource' function with unformatted uri reference
 * @target void DiscoveryTask::Ptr discoverResource(const RCSAddress& address,
 *                 const std::string& relativeUri, ResourceDiscoveredCallback cb);
 * @test_data &uri = "Hello"
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResource() API with unformatted uri reference value
 * @post_condition None
 * @expected The API should not crashed
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperTest_btc, DiscoverResourceWithoutResType_URSV_N)
{
    RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    string uri = "Hello";

    const string &adpass = uri;

    try
    {
        std::unique_ptr<RCSDiscoveryManager::DiscoveryTask> discoveryTask;
        discoveryTask = RCSDiscoveryManager::getInstance()->discoverResource(host,
                adpass, std::bind(&REResourceWrapperTest_btc::OnResourceDiscovered, this, PH::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        discoveryTask->cancel();
        SET_FAILURE("URSV Test Fail ");
    }
    catch (exception& e)
    {
        string exceptionMsg = string(e.what());
    }
}
#endif

/**
 * @since 2016-02-25
 * @see None
 * @objective Test 'discoverResource' function with negative basic way
 * @target void DiscoveryTask::Ptr discoverResource(const RCSAddress& address,
 *                 const std::string& relativeUri, ResourceDiscoveredCallback cb);
 * @test_data None
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResource() API
 * @post_condition None
 * @expected The API should throw exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperTest_btc, DiscoverResourceHostWithoutResType_ECRC_N)
{
    RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    string uri = "Hello";

    const string &adpass = uri;

    try
    {
        std::unique_ptr<RCSDiscoveryManager::DiscoveryTask> discoveryTask;
        discoveryTask = RCSDiscoveryManager::getInstance()->discoverResource(host,
                adpass, std::bind(&REResourceWrapperTest_btc::OnResourceDiscovered, this, PH::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        discoveryTask->cancel();
    }
    catch (exception& e)
    {
        string exceptionMsg = string(e.what());

        if (exceptionMsg == "Invalid URI")
        {
            SET_FAILURE(
                    "Exception occurred while discovering resource, exception is: " + exceptionMsg);
        }
    }
}
#endif 

/**
 * @since 2016-02-18
 * @see None
 * @objective Test 'discoverResource' function with positive basic way
 * @target DiscoveryTask::Ptr discoverResource(const RCSAddress& address,
 *                  ResourceDiscoveredCallback cb);
 * @test_data 1. host
 *            2. Callback
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResource() API
 * @post_condition None
 * @expected returned discovery manager should not be null
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperTest_btc, DiscoverResourceWithoutUriAndResType_SRC_P)
{
    RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    try
    {
        std::unique_ptr<RCSDiscoveryManager::DiscoveryTask> discoveryTask;
        discoveryTask = RCSDiscoveryManager::getInstance()->discoverResource(host, std::bind(&REResourceWrapperTest_btc::OnResourceDiscovered, this, PH::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        discoveryTask->cancel();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while discovering resource");
    }
}
#endif

/**
 * @since 2016-02-18
 * @see None
 * @objective Test 'discoverResource' function with formatted string value
 * @target DiscoveryTask::Ptr discoverResource(const RCSAddress& address,
 *                 ResourceDiscoveredCallback cb);
 * @test_data address = "107.109.009.054"
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResource() API with unicast address
 * @post_condition None
 * @expected API should not generate any exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperTest_btc, DiscoverResourceUnicast_FSV_P)
{
    RCSDiscoveryManager::getInstance();

    string address = "107.109.009.054";

    RCSAddress host = RCSAddress::unicast(address);

    try
    {
        std::unique_ptr<RCSDiscoveryManager::DiscoveryTask> discoveryTask;
        discoveryTask = RCSDiscoveryManager::getInstance()->discoverResource(host, std::bind(&REResourceWrapperTest_btc::OnResourceDiscovered, this, PH::_1));

        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

        discoveryTask->cancel();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while discovering resource");
    }
}
#endif

/**
 * @since 2016-02-18
 * @see None
 * @objective Test 'discoverResource' function with null callback
 * @target DiscoveryTask::Ptr discoverResource(const RCSAddress& address,
 *                 ResourceDiscoveredCallback cb);
 * @test_data cb = NULL
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResource() API with NULL cb
 * @post_condition None
 * @expected The API should not crash
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperTest_btc, DiscoverResourceMulticastCB_NV_N)
{
    RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    try
    {
        std::unique_ptr<RCSDiscoveryManager::DiscoveryTask> discoveryTask;
        discoveryTask = RCSDiscoveryManager::getInstance()->discoverResource(host, NULL);
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        discoveryTask->cancel();
        SET_FAILURE("Callback NV Test Fail !");
    }
    catch (exception& e)
    {
        string exceptionMsg = string(e.what());

    }
}
#endif

/**
 * @since 2016-02-25
 * @see None
 * @objective Test 'discoverResource' function with null callback for error throw checking
 * @target DiscoveryTask::Ptr discoverResource(const RCSAddress& address,
 *                 ResourceDiscoveredCallback cb);
 * @test_data cb = NULL
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResource() API
 * @post_condition None
 * @expected The API should not crash
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperTest_btc, DiscoverResourceMulticastCB_ECRC_N)
{
    RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    try
    {
        std::unique_ptr<RCSDiscoveryManager::DiscoveryTask> discoveryTask;
        discoveryTask = RCSDiscoveryManager::getInstance()->discoverResource(host, NULL);
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        discoveryTask->cancel();
    }
    catch (exception& e)
    {
        string exceptionMsg = string(e.what());

        if (exceptionMsg != "Callback is empty")
        {
            SET_FAILURE(
                    "Exception occurred while discovering resource, exception is: " + exceptionMsg);
        }
    }
}
#endif

/**
 * @since 2016-03-08
 * @see RCSDiscoveryManager getInstance()
 * @objective Test 'discoverResourceByTypes' function with positive basic way
 * @target DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                 const std::vector< std::string >& resourceTypes, ResourceDiscoveredCallback cb)
 * @test_data 1. host
 *            2. Resource Types
 *            3. Callback
 * @pre_condition None
 * @procedure Perform discoverResourceByTypes() API
 * @post_condition None
 * @expected returned discovery manager should not be null
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperTest_btc, DiscoverResourceByTypes_SRC_P)
{
    RCSDiscoveryManager* discoveryManager = RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    vector< string > resourceTypes;

    resourceTypes.push_back(RESOURCE_TYPE_LIGHT);
    resourceTypes.push_back(RESOURCE_TYPE_FAN);

    try
    {
        std::unique_ptr< RCSDiscoveryManager::DiscoveryTask > discoveryTask =
                discoveryManager->discoverResourceByTypes(host, resourceTypes,
                        std::bind(&REResourceWrapperTest_btc::OnResourceDiscovered, this, PH::_1));

        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

        discoveryTask->cancel();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while discovering resource");
    }
}
#endif

/**
 * @since 2016-03-09
 * @see RCSDiscoveryManager getInstance()
 * @objective Test 'discoverResourceByTypes' function with empty Resource Types
 * @target DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                 const std::vector< std::string >& resourceTypes, ResourceDiscoveredCallback cb)
 * @test_data 1. host
 *            2. Resource Types = ""
 *            3. Callback
 * @pre_condition None
 * @procedure Perform discoverResourceByTypes() API
 * @post_condition None
 * @expected The API should not crash
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperTest_btc, DiscoverResourceByTypes_ESV_P)
{
    RCSDiscoveryManager* discoveryManager = RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    vector< string > resourceTypes;

    resourceTypes.push_back("");

    try
    {
        std::unique_ptr< RCSDiscoveryManager::DiscoveryTask > discoveryTask =
                discoveryManager->discoverResourceByTypes(host, resourceTypes,
                        std::bind(&REResourceWrapperTest_btc::OnResourceDiscovered, this, PH::_1));

        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

        discoveryTask->cancel();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while discovering resource");
    }
}
#endif

/**
 * @since 2016-03-09
 * @see RCSDiscoveryManager getInstance()
 * @objective Test 'discoverResourceByTypes' function with NULL Callback
 * @target DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                 const std::vector< std::string >& resourceTypes, ResourceDiscoveredCallback cb)
 * @test_data 1. host
 *            2. Resource Types
 *            3. NULL Callback
 * @pre_condition None
 * @procedure Perform discoverResourceByTypes() API
 * @post_condition None
 * @expected Should not be crashed
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperTest_btc, DiscoverResourceByTypes_Cb_NV_N)
{
    RCSDiscoveryManager* discoveryManager = RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    vector< string > resourceTypes;

    resourceTypes.push_back(RESOURCE_TYPE_LIGHT);
    resourceTypes.push_back(RESOURCE_TYPE_FAN);

    try
    {
        std::unique_ptr< RCSDiscoveryManager::DiscoveryTask > discoveryTask =
                discoveryManager->discoverResourceByTypes(host, resourceTypes,
                        NULL);

        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

        discoveryTask->cancel();
        SET_FAILURE("Although Callback is NULL. API called successfully!");
    }
    catch (exception& e)
    {

    }
}
#endif

/**
 * @since 2016-03-09
 * @see RCSDiscoveryManager getInstance()
 * @objective Test 'discoverResourceByTypes' function with NULL Callback for error throw checking
 * @target DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                 const std::vector< std::string >& resourceTypes, ResourceDiscoveredCallback cb)
 * @test_data 1. host
 *            2. Resource Types
 *            3. NULL Callback
 * @pre_condition None
 * @procedure Perform discoverResourceByTypes() API
 * @post_condition None
 * @expected The API should not crash
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperTest_btc, DiscoverResourceByTypes_ETC_N)
{
    RCSDiscoveryManager* discoveryManager = RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    vector< string > resourceTypes;

    resourceTypes.push_back(RESOURCE_TYPE_LIGHT);
    resourceTypes.push_back(RESOURCE_TYPE_FAN);

    try
    {
        std::unique_ptr< RCSDiscoveryManager::DiscoveryTask > discoveryTask =
                discoveryManager->discoverResourceByTypes(host, resourceTypes, NULL);

        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

        discoveryTask->cancel();
    }
    catch (exception& e)
    {
        string exceptionMsg = string(e.what());
        if (exceptionMsg.compare("Callback is empty") != 0)
        {
            SET_FAILURE(
                    "Callback is not empty! Error : " + exceptionMsg);
        }
    }
}
#endif

/**
 * @since 2016-03-09
 * @see RCSDiscoveryManager getInstance()
 * @objective Test 'discoverResourceByTypes' function with unformatted Resource Types
 * @target DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                 const std::vector< std::string >& resourceTypes, ResourceDiscoveredCallback cb)
 * @test_data 1. host
 *            2. Resource Types = "hello", "world"
 *            3. Callback
 * @pre_condition None
 * @procedure Perform discoverResourceByTypes() API
 * @post_condition None
 * @expected The API should not crash
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperTest_btc, DiscoverResourceByTypes_USV_N)
{
    RCSDiscoveryManager* discoveryManager = RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    vector< string > resourceTypes;

    resourceTypes.push_back("hello");
    resourceTypes.push_back("world");

    try
    {
        std::unique_ptr< RCSDiscoveryManager::DiscoveryTask > discoveryTask =
                discoveryManager->discoverResourceByTypes(host, resourceTypes,
                        std::bind(&REResourceWrapperTest_btc::OnResourceDiscovered, this, PH::_1));

        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

        discoveryTask->cancel();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while discovering resource");
    }
}
#endif

/**
 * @since 2016-03-08
 * @see RCSDiscoveryManager getInstance()
 * @objective Test 'discoverResourceByTypes' function with positive basic way
 * @target DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address, const std::string& relativeUri,
 *                  const std::vector< std::string >& resourceTypes, ResourceDiscoveredCallback cb)
 * @test_data 1. host
 *            2. uri
 *            3. Resource Types
 *            4. Callback
 * @pre_condition None
 * @procedure Perform discoverResourceByTypes() API
 * @post_condition None
 * @expected returned discovery manager should not be null
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperTest_btc, DiscoverResourceByTypesWithUri_SRC_P)
{
    RCSDiscoveryManager* discoveryManager = RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    string uri = OC_RSRVD_WELL_KNOWN_URI;
    vector< string > resourceTypes;

    resourceTypes.push_back(RESOURCE_TYPE_LIGHT);
    resourceTypes.push_back(RESOURCE_TYPE_FAN);

    try
    {
        std::unique_ptr< RCSDiscoveryManager::DiscoveryTask > discoveryTask =
                discoveryManager->discoverResourceByTypes(host, uri, resourceTypes,
                        std::bind(&REResourceWrapperTest_btc::OnResourceDiscovered, this, PH::_1));

        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

        discoveryTask->cancel();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while discovering resource");
    }
}
#endif

/**
 * @since 2016-03-09
 * @see RCSDiscoveryManager getInstance()
 * @objective Test 'discoverResourceByTypes' function with empty resource type
 * @target DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address, const std::string& relativeUri,
 *                  const std::vector< std::string >& resourceTypes, ResourceDiscoveredCallback cb)
 * @test_data 1. host
 *            2. uri 
 *            3. Resource Types = ""
 *            4. Callback
 * @pre_condition None
 * @procedure Perform discoverResourceByTypes() API
 * @post_condition None
 * @expected The API should not crash
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperTest_btc, DiscoverResourceByTypesWithUri_ResourceTypes_ESV_P)
{
    RCSDiscoveryManager* discoveryManager = RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    string uri = OC_RSRVD_WELL_KNOWN_URI;
    vector< string > resourceTypes;

    resourceTypes.push_back("");
    try
    {
        std::unique_ptr< RCSDiscoveryManager::DiscoveryTask > discoveryTask =
                discoveryManager->discoverResourceByTypes(host, uri, resourceTypes,
                        std::bind(&REResourceWrapperTest_btc::OnResourceDiscovered, this, PH::_1));

        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

        discoveryTask->cancel();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while discovering resource");
    }
}
#endif

/**
 * @since 2016-03-09
 * @see RCSDiscoveryManager getInstance()
 * @objective Test 'discoverResourceByTypes' function with positive way [ESV]
 * @target DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address, const std::string& relativeUri,
 *                  const std::vector< std::string >& resourceTypes, ResourceDiscoveredCallback cb)
 * @test_data 1. host
 *            2. uri = ""
 *            3. Resource Types
 *            4. Callback
 * @pre_condition None
 * @procedure Perform discoverResourceByTypes() API
 * @post_condition None
 * @expected The API should not crash
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperTest_btc, DiscoverResourceByTypesWithUri_Uri_ESV_P)
{
    RCSDiscoveryManager* discoveryManager = RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    string uri = "";
    vector< string > resourceTypes;

    resourceTypes.push_back(RESOURCE_TYPE_LIGHT);
    resourceTypes.push_back(RESOURCE_TYPE_FAN);
    try
    {
        std::unique_ptr< RCSDiscoveryManager::DiscoveryTask > discoveryTask =
                discoveryManager->discoverResourceByTypes(host, uri, resourceTypes,
                        std::bind(&REResourceWrapperTest_btc::OnResourceDiscovered, this, PH::_1));

        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

        discoveryTask->cancel();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while discovering resource");
    }
}
#endif

/**
 * @since 2016-03-09
 * @see RCSDiscoveryManager getInstance()
 * @objective Test 'discoverResourceByTypes' function with null callback
 * @target DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address, const std::string& relativeUri,
 *                  const std::vector< std::string >& resourceTypes, ResourceDiscoveredCallback cb)
 * @test_data 1. host
 *            2. uri
 *            3. Resource Types
 *            4. Callback
 * @pre_condition None
 * @procedure Perform discoverResourceByTypes() API
 * @post_condition None
 * @expected The API should not crash
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperTest_btc, DiscoverResourceByTypesWithUri_Cb_NV_N)
{
    RCSDiscoveryManager* discoveryManager = RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    vector< string > resourceTypes;
    string uri = OC_RSRVD_WELL_KNOWN_URI;

    resourceTypes.push_back(RESOURCE_TYPE_LIGHT);
    resourceTypes.push_back(RESOURCE_TYPE_FAN);

    try
    {
        std::unique_ptr< RCSDiscoveryManager::DiscoveryTask > discoveryTask =
                discoveryManager->discoverResourceByTypes(host, uri, resourceTypes, NULL);

        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

        discoveryTask->cancel();

        SET_FAILURE("Although Callback is NULL. API called successfully!");
    }
    catch (exception& e)
    {

    }
}
#endif

/**
 * @since 2016-03-09
 * @see RCSDiscoveryManager getInstance()
 * @objective Test 'discoverResourceByTypes' function with null callback for error throw checking
 * @target DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address, const std::string& relativeUri,
 *                  const std::vector< std::string >& resourceTypes, ResourceDiscoveredCallback cb)
 * @test_data 1. host
 *            2. uri
 *            3. Resource Types
 *            4. Callback = NULL
 * @pre_condition None
 * @procedure Perform discoverResourceByTypes() API
 * @post_condition None
 * @expected The API should throw exception
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperTest_btc, DiscoverResourceByTypesWithUri_Cb_ETC_N)
{
    RCSDiscoveryManager* discoveryManager = RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    vector< string > resourceTypes;
    string uri = OC_RSRVD_WELL_KNOWN_URI;

    resourceTypes.push_back(RESOURCE_TYPE_LIGHT);
    resourceTypes.push_back(RESOURCE_TYPE_FAN);

    try
    {
        std::unique_ptr< RCSDiscoveryManager::DiscoveryTask > discoveryTask =
                discoveryManager->discoverResourceByTypes(host, uri, resourceTypes, NULL);

        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

        discoveryTask->cancel();
    }
    catch (exception& e)
    {
        string exceptionMsg = string(e.what());
        if (exceptionMsg.compare("Callback is empty") != 0)
        {
            SET_FAILURE(
                    "Callback is not empty! Error : " + exceptionMsg);
        }
    }
}
#endif

/**
 * @since 2016-03-09
 * @see RCSDiscoveryManager getInstance()
 * @objective Test 'discoverResourceByTypes' function with unformatted resource type
 * @target DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address, const std::string& relativeUri,
 *                  const std::vector< std::string >& resourceTypes, ResourceDiscoveredCallback cb)
 * @test_data 1. host
 *            2. uri
 *            3. Resource Types = "Hello", "World"
 *            4. Callback
 * @pre_condition None
 * @procedure Perform discoverResourceByTypes() API
 * @post_condition None
 * @expected The API should not crash
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperTest_btc, DiscoverResourceByTypesWithUri_ResourceTypes_USV_N)
{
    RCSDiscoveryManager* discoveryManager = RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    vector< string > resourceTypes;
    string uri = OC_RSRVD_WELL_KNOWN_URI;

    resourceTypes.push_back("Hello");
    resourceTypes.push_back("World");

    try
    {
        std::unique_ptr< RCSDiscoveryManager::DiscoveryTask > discoveryTask =
                discoveryManager->discoverResourceByTypes(host, uri, resourceTypes,
                        std::bind(&REResourceWrapperTest_btc::OnResourceDiscovered, this, PH::_1));

        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

        discoveryTask->cancel();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while discovering resource");
    }
}
#endif

/**
 * @since 2016-03-09
 * @see RCSDiscoveryManager getInstance()
 * @objective Test 'discoverResourceByTypes' function with unformatted uri
 * @target DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address, const std::string& relativeUri,
 *                  const std::vector< std::string >& resourceTypes, ResourceDiscoveredCallback cb)
 * @test_data 1. host
 *            2. uri = "http://www.google.com";
 *            3. Resource Types
 *            4. Callback
 * @pre_condition None
 * @procedure Perform discoverResourceByTypes() API
 * @post_condition None
 * @expected The API should not crash
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperTest_btc, DiscoverResourceByTypesWithUri_Uri_USV_N)
{
    RCSDiscoveryManager* discoveryManager = RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    vector< string > resourceTypes;
    string uri = "http://www.google.com";

    resourceTypes.push_back(RESOURCE_TYPE_LIGHT);
    resourceTypes.push_back(RESOURCE_TYPE_FAN);

    try
    {
        std::unique_ptr< RCSDiscoveryManager::DiscoveryTask > discoveryTask =
                discoveryManager->discoverResourceByTypes(host, uri, resourceTypes,
                        std::bind(&REResourceWrapperTest_btc::OnResourceDiscovered, this, PH::_1));

        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

        discoveryTask->cancel();
        SET_FAILURE("Although uri is not valid. API called successfully!");
    }
    catch (exception& e)
    {

    }
}
#endif

