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

class REResourceWrapperAPITest_stc: public ::testing::Test {
protected:

    REHelper* m_pREHelper;
    static std::mutex s_mutex;
    std::string m_errorMsg;
    shared_ptr<RCSRemoteResourceObject> m_resource;
    vector<shared_ptr<RCSRemoteResourceObject> > m_foundResourceList;
    unique_ptr<RCSDiscoveryManager::DiscoveryTask> discoveryTask;
    bool callbackcheck;

    virtual void SetUp() {
        CommonUtil::runCommonTCSetUpPart();

#ifdef __LINUX__
        CommonUtil::launchApp(RE_SERVER_APP);
        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);
#endif
        m_pREHelper = REHelper::getInstance();

        callbackcheck = false;
        bool isResourceAvailable = m_pREHelper->findPrimitiveResources(
                RESOURCE_TYPE_LIGHT);
        if (isResourceAvailable) {
            m_resource = m_pREHelper->getFoundResourceList().at(0);
            IOTIVITYTEST_LOG(DEBUG, "Uri of 1st resource on list is = %s",
                    m_pREHelper->getFoundResourceList().at(0)->getUri().c_str());
        } else {
            SET_FAILURE("Precondition Failed, No Resource Found!!");
        }

        discoveryTask = nullptr;
        m_foundResourceList.clear();
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
        s_mutex.lock();
        callbackcheck = true;

        IOTIVITYTEST_LOG(DEBUG, "Inside OnResourceDiscovered!!");

        if (pResource) {
            m_foundResourceList.push_back(pResource);
            IOTIVITYTEST_LOG(DEBUG,
                    "Resource added to local found resource list, host = %s , uri = %s",
                    pResource->getAddress().c_str(),
                    pResource->getUri().c_str());
        } else {
            IOTIVITYTEST_LOG(WARNING, "Found resource is null");
        }
        s_mutex.unlock();
    }
};

std::mutex REResourceWrapperAPITest_stc::s_mutex;

/**
 * @since 2015-07-14
 * @see None
 * @objective Test 'discoverResource' function with sequential validation
 * @target DiscoveryTask::Ptr discoverResource(const RCSAddress& address,
 *                  const std::string& relativeUri, ResourceDiscoveredCallback cb);
 * @test_data 1. address
 *            2. uri
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResource() API
 * @post_condition None
 * @expected returned discovery manager should not be null
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperAPITest_stc, DiscoverResource_SQV_P)
{
    RCSAddress host = RCSAddress::multicast();
    string uri = "";
    uri = OC_RSRVD_WELL_KNOWN_URI + uri + "?rt=" + RESOURCE_TYPE_LIGHT;
    try
    {
        IOTIVITYTEST_LOG(DEBUG, "Discovering Resource!!");

        bool isSuccess = m_pREHelper->findPrimitiveResources(RESOURCE_TYPE_LIGHT);

        if (isSuccess == false)
        {
            SET_FAILURE("No Resource Found!!");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while discovering resource");
    }
}
#endif

/**
 * @since 2015-07-14
 * @see None
 * @objective Test 'discoverResource' function with empty uri
 * @target DiscoveryTask::Ptr discoverResource(const RCSAddress& address,
 *                  const std::string& relativeUri, ResourceDiscoveredCallback cb);
 * @test_data 1. host
 *            2. empty uri
 *            3. Callback
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResource() API
 * @post_condition None
 * @expected The API should not crash
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperAPITest_stc, DiscoverResourceWithEmptyUri_ESV_N)
{
    RCSDiscoveryManager* discoveryManager = RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    string uri = "";
    try
    {
        discoveryTask = discoveryManager->discoverResource(host, uri,
                bind(&REResourceWrapperAPITest_stc::OnResourceDiscovered, this, PH::_1));
        discoveryTask->cancel();
    }
    catch (exception& e)
    {
        string exceptionMsg = string(e.what());

        if (exceptionMsg.compare("Failed : Invalid URI") != 0)
        {
            SET_FAILURE(
                    "Exception occurred while discovering resource, exception is : "
                    + exceptionMsg);
        }
    }
}
#endif

/**
 * @since 2015-07-14
 * @see None
 * @objective Test 'discoverResource' function with Null Callback
 * @target DiscoveryTask::Ptr discoverResource(const RCSAddress& address,
 *                  const std::string& relativeUri, ResourceDiscoveredCallback cb);
 * @test_data None
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResource() API
 * @post_condition 1. host
 *            	   2. uri
 *                 3. Null Callback
 * @expected The API should not crash
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperAPITest_stc, DiscoverResourceWithNullCallback_ETC_N)
{
    RCSDiscoveryManager* discoveryManager = RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    string uri = "";
    uri = OC_RSRVD_WELL_KNOWN_URI + uri + "?rt=" + RESOURCE_TYPE_LIGHT;
    try
    {
        discoveryTask = discoveryManager->discoverResource(host, uri, NULL);
        discoveryTask->cancel();
    }
    catch (exception& e)
    {
        string exceptionMsg = string(e.what());
        if (exceptionMsg.compare("Callback is empty") != 0)
        {
            SET_FAILURE(
                    "Exception occurred while discovering resource, exception is : "
                    + exceptionMsg);
        }
    }
}
#endif

/**
 * @since 2015-07-14
 * @see None
 * @objective Test 'discoverResource' function multiple times
 * @target DiscoveryTask::Ptr discoverResource(const RCSAddress& address,
 *                  const std::string& relativeUri, ResourceDiscoveredCallback cb);
 * @test_data None
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Perform discoverResource() API multiple times
 * @post_condition None
 * @expected returned discovery manager should not be null
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperAPITest_stc, DiscoverResourceMultipleTimes_VLCC_P)
{
    for (int i = 1; i <= 10; i++)
    {
        m_pREHelper->findPrimitiveResources(RESOURCE_TYPE_LIGHT);

        if (m_pREHelper->getFoundResourceList().size() == 0)
        {
            SET_FAILURE(
                    "DiscoverResourceMultipleTimes_P failed for iteration no. " + to_string(i)
                    + ", No resource found!!");
        }
    }
}
#endif

/**
 * @since 2015-07-14
 * @see None
 * @objective Test 'discoverResource' function with state transition loop check
 * @target DiscoveryTask::Ptr discoverResource(const RCSAddress& address,
 *                  const std::string& relativeUri, ResourceDiscoveredCallback cb);
 * @test_data 1. host
 *            2. uri
 *            3. Callback
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure 1. Perform discoverResource() API multiple times Without Waiting
 * @post_condition Cancel Discovery Task
 * @expected The API should not crash
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperAPITest_stc, DiscoverResourceMultipleTimesWithoutWaiting_STLC_N)
{
    RCSDiscoveryManager* discoveryManager = RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    string uri = "";
    uri = OC_RSRVD_WELL_KNOWN_URI;
    int iter = 0;

    try
    {
        for (iter = 1; iter <10; iter++)
        {
            discoveryTask = discoveryManager->discoverResource(host, uri,
                    bind(&REResourceWrapperAPITest_stc::OnResourceDiscovered, this, PH::_1));
            discoveryTask->cancel();
        }

    }
    catch (exception& e)
    {
        SET_FAILURE(
                "Exception occurred while discovering resource, from iteration: "
                + to_string(iter));

        if (m_foundResourceList.size() == 0)
        {
            SET_FAILURE("No Resource Found!!");
        }
    }

}
#endif

/**
 * @since 2015-07-14
 * @see None
 * @objective Test 'discoverResource' function with sequential validation
 * @target DiscoveryTask::Ptr discoverResource(const RCSAddress& address,
 *                  const std::string& relativeUri, ResourceDiscoveredCallback cb);
 * @test_data 1. host
 *            2. uri
 *            3. Callback
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResource() API
 * @post_condition None
 * @expected returned discovery manager should not be null
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperAPITest_stc, DiscoverAllResources_SQV_P)
{
    RCSDiscoveryManager* discoveryManager = RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    string uri = "";
    uri = OC_RSRVD_WELL_KNOWN_URI;
    try
    {
        discoveryTask = discoveryManager->discoverResource(host, uri,
                bind(&REResourceWrapperAPITest_stc::OnResourceDiscovered, this, PH::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);
        discoveryTask->cancel();

        if (m_foundResourceList.size() < 2)
        {
            SET_FAILURE("All the Resources not Found!!");
        }

    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while discovering resource");
    }
}
#endif

/**
 * @since 2016-02-29
 * @see None
 * @objective Test 'discoverResourceByType' function with callback condition checking
 * @target DiscoveryTask::Ptr discoverResourceByType(const RCSAddress& address,
 *                   const std::string& resourceType, ResourceDiscoveredCallback cb);
 * @test_data 1. host
 *            2. resource Type
 *            3. Callback
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResourceByType() API
 * @post_condition None
 * @expected returned discovery manager should not be null
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperAPITest_stc, DiscoverResourcesByTypeWithoutUri_CCC_P)
{
    RCSDiscoveryManager* discoveryManager = RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();

    try
    {
        discoveryTask = discoveryManager->discoverResourceByType(host, RESOURCE_TYPE_LIGHT,
                bind(&REResourceWrapperAPITest_stc::OnResourceDiscovered, this, PH::_1));

        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);
        discoveryTask->cancel();

        if (m_foundResourceList.size() < 1)
        {
            SET_FAILURE("All the Resources not Found!!");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while discovering resource");
    }
}
#endif

/**
 * @since 2016-02-29
 * @see None
 * @objective Test 'discoverResourceByType' function twice
 * @target DiscoveryTask::Ptr discoverResourceByType(const RCSAddress& address,
 *                   const std::string& resourceType, ResourceDiscoveredCallback cb);
 * @test_data 1. host
 *            2. resource Type
 *            3. Callback
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResourceByType() API twice
 * @post_condition None
 * @expected returned discovery manager should not be null
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperAPITest_stc, DiscoverResourcesByTypeWithoutUri_CACC_P)
{
    RCSDiscoveryManager* discoveryManager = RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();

    try
    {
        discoveryTask = discoveryManager->discoverResourceByType(host, RESOURCE_TYPE_LIGHT,
                bind(&REResourceWrapperAPITest_stc::OnResourceDiscovered, this, PH::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);

        discoveryTask->cancel();

        discoveryTask = discoveryManager->discoverResourceByType(host, RESOURCE_TYPE_LIGHT,
                bind(&REResourceWrapperAPITest_stc::OnResourceDiscovered, this, PH::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);

        discoveryTask->cancel();

        if (m_foundResourceList.size() < 1)
        {
            SET_FAILURE("All the Resources not Found!!");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while discovering resource");
    }
}
#endif


/**
 * @since 2016-02-29
 * @see None
 * @objective Test 'discoverResourceByType' function with invalid resource type
 * @target DiscoveryTask::Ptr discoverResourceByType(const RCSAddress& address,
 *                   const std::string& resourceType, ResourceDiscoveredCallback cb);
 * @test_data 1. host
 *            2. invalid resource Type
 *            3. Callback
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResourceByType() API with invalid resource type
 * @post_condition None
 * @expected The API should not crash
 **/

#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperAPITest_stc, DiscoverResourcesByTypeWithoutUri_EG_N)
{


    try
    {
        RCSDiscoveryManager* discoveryManager = RCSDiscoveryManager::getInstance();

        RCSAddress host = RCSAddress::multicast();
        string resType = "Hello";

        discoveryTask = discoveryManager->discoverResourceByType(host, resType, bind(&REResourceWrapperAPITest_stc::OnResourceDiscovered, this, PH::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);
        discoveryTask->cancel();

        if(callbackcheck == true)
        {
            SET_FAILURE("Callback Invoked !");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while discovering resource: " + string(e.what()));
    }
}
#endif


/**
 * @since 2016-02-29
 * @see None
 * @objective Test 'discoverResourceByType' function with null callback
 * @target DiscoveryTask::Ptr discoverResourceByType(const RCSAddress& address,
 *                   const std::string& resourceType, ResourceDiscoveredCallback cb);
 * @test_data 1. host
 *            2. invalid resource Type
 *            3. null Callback
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResourceByType() API with null callback
 * @post_condition None
 * @expected The API should not crash
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperAPITest_stc, DiscoverResourcesByTypeWithoutUri_ICCC_N)
{
    RCSDiscoveryManager* discoveryManager = RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();

    try
    {
        discoveryTask = discoveryManager->discoverResourceByType(host, RESOURCE_TYPE_LIGHT, NULL);
        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);

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
 * @since 2016-02-29
 * @see None
 * @objective Test 'discoverResourceByType' function multiple times
 * @target DiscoveryTask::Ptr discoverResourceByType(const RCSAddress& address,
 *                   const std::string& resourceType, ResourceDiscoveredCallback cb);
 * @test_data 1. host
 *            2. invalid resource Type
 *            3. null Callback
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResourceByType() API multiple times
 * @post_condition None
 * @expected returned discovery manager should not be null
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperAPITest_stc, DiscoverResourcesByTypeWithoutUri_ALVC_N)
{
    RCSDiscoveryManager* discoveryManager = RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();

    try
    {
        for(int i = 0; i < 10; i++)
        {
            discoveryTask = discoveryManager->discoverResourceByType(host, RESOURCE_TYPE_LIGHT, bind(&REResourceWrapperAPITest_stc::OnResourceDiscovered, this, PH::_1));

            discoveryTask->cancel();
        }

    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while discovering resource");
    }
}
#endif

/**
 * @since 2016-02-29
 * @see None
 * @objective Test 'discoverResourceByType' function with callback condition checking
 * @target DiscoveryTask::Ptr discoverResourceByType(const RCSAddress& address,
 *                  const std::string& relativeUri, const std::string& resourceType,
 *                  ResourceDiscoveredCallback cb);
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
TEST_F(REResourceWrapperAPITest_stc, DiscoverResourcesByType_CCC_P)
{
    RCSDiscoveryManager* discoveryManager = RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    string URI = OC_RSRVD_WELL_KNOWN_URI;

    try
    {
        discoveryTask = discoveryManager->discoverResourceByType(host, URI, RESOURCE_TYPE_LIGHT,
                bind(&REResourceWrapperAPITest_stc::OnResourceDiscovered, this, PH::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);
        discoveryTask->cancel();

        if (m_foundResourceList.size() < 1)
        {
            SET_FAILURE("All the Resources not Found!!");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while discovering resource");
    }
}
#endif

/**
 * @since 2016-02-29
 * @see None
 * @objective Test 'discoverResourceByType' function twice
 * @target DiscoveryTask::Ptr discoverResourceByType(const RCSAddress& address,
 *                  const std::string& relativeUri, const std::string& resourceType,
 *                  ResourceDiscoveredCallback cb);
 * @test_data 1. host
 *            2. uri
 *            3. Resource Type
 *            4. Callback
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResourceByType() API twice
 * @post_condition None
 * @expected returned discovery manager should not be null
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperAPITest_stc, DiscoverResourcesByType_CACC_P)
{
    RCSDiscoveryManager* discoveryManager = RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    string URI = OC_RSRVD_WELL_KNOWN_URI;

    try
    {
        discoveryTask = discoveryManager->discoverResourceByType(host, URI, RESOURCE_TYPE_LIGHT,
                bind(&REResourceWrapperAPITest_stc::OnResourceDiscovered, this, PH::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);
        discoveryTask->cancel();

        discoveryTask = discoveryManager->discoverResourceByType(host, RESOURCE_TYPE_LIGHT,
                bind(&REResourceWrapperAPITest_stc::OnResourceDiscovered, this, PH::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);
        discoveryTask->cancel();

        if (m_foundResourceList.size() < 1)
        {
            SET_FAILURE("All the Resources not Found!!");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while discovering resource");
    }
}
#endif

/**
 * @since 2016-02-29
 * @see None
 * @objective Test 'discoverResourceByType' function with Invalid Resource Type
 * @target DiscoveryTask::Ptr discoverResourceByType(const RCSAddress& address,
 *                  const std::string& relativeUri, const std::string& resourceType,
 *                  ResourceDiscoveredCallback cb);
 * @test_data 1. host
 *            2. uri
 *            3. Invalid Resource Type
 *            4. Callback
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResourceByType() API with Invalid Resource Type
 * @post_condition None
 * @expected The API should not crash
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperAPITest_stc, DiscoverResourcesByTypeWithInvalidResType_EG_N)
{
    RCSDiscoveryManager* discoveryManager = RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    string URI = OC_RSRVD_WELL_KNOWN_URI;

    string resType = "Hello";

    try
    {
        discoveryTask = discoveryManager->discoverResourceByType(host, URI, resType, bind(&REResourceWrapperAPITest_stc::OnResourceDiscovered, this, PH::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);
        discoveryTask->cancel();

        if(callbackcheck == true)
            SET_FAILURE("Callback Invoked !");
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while discovering resource");
    }
}
#endif

/**
 * @since 2016-02-29
 * @see None
 * @objective Test 'discoverResourceByType' function with With Invalid Uri
 * @target DiscoveryTask::Ptr discoverResourceByType(const RCSAddress& address,
 *                  const std::string& relativeUri, const std::string& resourceType,
 *                  ResourceDiscoveredCallback cb);
 * @test_data 1. host
 *            2. Invalid uri
 *            3. Resource Type
 *            4. Callback
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResourceByType() API with With Invalid Uri
 * @post_condition None
 * @expected The API should not crash
 **/

#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperAPITest_stc, DiscoverResourcesByTypeWithInvalidUri_EG_N)
{
    RCSDiscoveryManager* discoveryManager = RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    string URI = "hello";

    string resType = RESOURCE_TYPE_LIGHT;

    try
    {
        discoveryTask = discoveryManager->discoverResourceByType(host, URI, resType, bind(&REResourceWrapperAPITest_stc::OnResourceDiscovered, this, PH::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);

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
 * @since 2016-02-29
 * @see None
 * @objective Test 'discoverResourceByType' function with null callback
 * @target DiscoveryTask::Ptr discoverResourceByType(const RCSAddress& address,
 *                  const std::string& relativeUri, const std::string& resourceType,
 *                  ResourceDiscoveredCallback cb);
 * @test_data 1. host
 *            2. uri
 *            3. Resource Type
 *            4. Null Callback
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResourceByType() API with null callback
 * @post_condition None
 * @expected The API should not crash
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperAPITest_stc, DiscoverResourcesByType_ICCC_N)
{
    RCSDiscoveryManager* discoveryManager = RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    string URI = OC_RSRVD_WELL_KNOWN_URI;

    try
    {
        discoveryTask = discoveryManager->discoverResourceByType(host, URI, RESOURCE_TYPE_LIGHT, NULL);
        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);

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
 * @since 2016-02-29
 * @see None
 * @objective Test 'discoverResourceByType' function multiple times
 * @target DiscoveryTask::Ptr discoverResourceByType(const RCSAddress& address,
 *                  const std::string& relativeUri, const std::string& resourceType,
 *                  ResourceDiscoveredCallback cb);
 * @test_data 1. host
 *            2. uri
 *            3. Resource Type
 *            4. Callback
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResourceByType() API multiple times
 * @post_condition None
 * @expected returned discovery manager should not be null
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperAPITest_stc, DiscoverResourcesByType_ALVC_N)
{
    RCSDiscoveryManager* discoveryManager = RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    string URI = OC_RSRVD_WELL_KNOWN_URI;

    try
    {
        for(int i = 0; i < 10; i++)
        {
            discoveryTask = discoveryManager->discoverResourceByType(host, URI, RESOURCE_TYPE_LIGHT, bind(&REResourceWrapperAPITest_stc::OnResourceDiscovered, this, PH::_1));
            discoveryTask->cancel();
        }

    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while discovering resource");
    }
}
#endif

/**
 * @since 2016-03-01
 * @see None
 * @objective Test 'discoverResource' function with callback condition checking
 * @target DiscoveryTask::Ptr discoverResource(const RCSAddress& address,
 *                  const std::string& relativeUri, ResourceDiscoveredCallback cb);
 * @test_data 1. host
 *            2. uri
 *            3. Callback
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResource() API
 * @post_condition None
 * @expected returned discovery manager should not be null
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperAPITest_stc, DiscoverResources_CCC_P)
{
    RCSDiscoveryManager* discoveryManager = RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    string URI = OC_RSRVD_WELL_KNOWN_URI;

    try
    {
        discoveryTask = discoveryManager->discoverResource(host, URI,
                bind(&REResourceWrapperAPITest_stc::OnResourceDiscovered, this, PH::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);
        discoveryTask->cancel();

        if (m_foundResourceList.size() < 1)
        {
            SET_FAILURE("All the Resources not Found!!");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while discovering resource");
    }
}
#endif

/**
 * @since 2016-03-01
 * @see None
 * @objective Test 'discoverResource' function twice
 * @target DiscoveryTask::Ptr discoverResource(const RCSAddress& address,
 *                  const std::string& relativeUri, ResourceDiscoveredCallback cb);
 * @test_data 1. host
 *            2. uri
 *            3. Callback
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResource() API twice
 * @post_condition None
 * @expected returned discovery manager should not be null
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperAPITest_stc, DiscoverResources_CACC_P)
{
    RCSDiscoveryManager* discoveryManager = RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    string URI = OC_RSRVD_WELL_KNOWN_URI;

    try
    {
        discoveryTask = discoveryManager->discoverResource(host, URI,
                bind(&REResourceWrapperAPITest_stc::OnResourceDiscovered, this, PH::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);
        discoveryTask->cancel();

        discoveryTask = discoveryManager->discoverResource(host, URI,
                bind(&REResourceWrapperAPITest_stc::OnResourceDiscovered, this, PH::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);
        discoveryTask->cancel();

        if (m_foundResourceList.size() < 1)
        {
            SET_FAILURE("All the Resources not Found!!");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while discovering resource");
    }
}
#endif

/**
 * @since 2016-03-01
 * @see None
 * @objective Test 'discoverResource' function with invalid uri
 * @target DiscoveryTask::Ptr discoverResource(const RCSAddress& address,
 *                  const std::string& relativeUri, ResourceDiscoveredCallback cb);
 * @test_data 1. host
 *            2. invalid uri
 *            3. Callback
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResource() API with invalid uri
 * @post_condition None
 * @expected The API should not crash
 **/

#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperAPITest_stc, DiscoverResourcesWithInvalidUri_EG_N)
{
    RCSDiscoveryManager* discoveryManager = RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    string URI = "hello";

    try
    {
        discoveryTask = discoveryManager->discoverResource(host, URI, bind(&REResourceWrapperAPITest_stc::OnResourceDiscovered, this, PH::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);

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
 * @since 2016-03-01
 * @see None
 * @objective Test 'discoverResource' function with null callback
 * @target DiscoveryTask::Ptr discoverResource(const RCSAddress& address,
 *                  const std::string& relativeUri, ResourceDiscoveredCallback cb);
 * @test_data 1. host
 *            2. uri
 *            3. null Callback
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResource() API with null callback
 * @post_condition None
 * @expected The API should not crash
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperAPITest_stc, DiscoverResources_ICCC_N)
{
    RCSDiscoveryManager* discoveryManager = RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    string URI = OC_RSRVD_WELL_KNOWN_URI;

    try
    {
        discoveryTask = discoveryManager->discoverResource(host, URI, NULL);
        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);

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
 * @since 2016-03-01
 * @see None
 * @objective Test 'discoverResource' function multiple times
 * @target DiscoveryTask::Ptr discoverResource(const RCSAddress& address,
 *                  const std::string& relativeUri, ResourceDiscoveredCallback cb);
 * @test_data 1. host
 *            2. uri
 *            3. Callback
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResource() API multiple times
 * @post_condition None
 * @expected The API should not crash
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperAPITest_stc, DiscoverResources_ALVC_N)
{
    RCSDiscoveryManager* discoveryManager = RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();
    string URI = OC_RSRVD_WELL_KNOWN_URI;

    try
    {
        for(int i = 0; i < 10; i++)
        {
            discoveryTask = discoveryManager->discoverResource(host, URI, bind(&REResourceWrapperAPITest_stc::OnResourceDiscovered, this, PH::_1));
            discoveryTask->cancel();
        }

    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while discovering resource");
    }
}
#endif

/**
 * @since 2016-03-01
 * @see None
 * @objective Test 'discoverResource' function with callback condition checking
 * @target DiscoveryTask::Ptr discoverResource(const RCSAddress& address,
 *                   ResourceDiscoveredCallback cb);
 * @test_data 1. host
 *            2. Callback
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResource() API
 * @post_condition None
 * @expected returned discovery manager should not be null
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperAPITest_stc, DiscoverResourcesWithoutResType_CCC_P)
{
    RCSDiscoveryManager* discoveryManager = RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();

    try
    {
        discoveryTask = discoveryManager->discoverResource(host, bind(&REResourceWrapperAPITest_stc::OnResourceDiscovered, this, PH::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);
        discoveryTask->cancel();

        if (m_foundResourceList.size() < 1)
        {
            SET_FAILURE("All the Resources not Found!!");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while discovering resource");
    }
}
#endif

/**
 * @since 2016-03-01
 * @see None
 * @objective Test 'discoverResource' function for twice
 * @target DiscoveryTask::Ptr discoverResource(const RCSAddress& address,
 *                   ResourceDiscoveredCallback cb);
 * @test_data 1. host
 *            2. Callback
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResource() API for twice
 * @post_condition None
 * @expected The API should not crash
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperAPITest_stc, DiscoverResourcesWithoutResType_CACC_P)
{
    RCSDiscoveryManager* discoveryManager = RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();

    try
    {
        discoveryTask = discoveryManager->discoverResource(host, bind(&REResourceWrapperAPITest_stc::OnResourceDiscovered, this, PH::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);
        discoveryTask->cancel();

        discoveryTask = discoveryManager->discoverResource(host, bind(&REResourceWrapperAPITest_stc::OnResourceDiscovered, this, PH::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);
        discoveryTask->cancel();

        if (m_foundResourceList.size() < 1)
        {
            SET_FAILURE("All the Resources not Found!!");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while discovering resource");
    }
}
#endif

/**
 * @since 2016-03-01
 * @see None
 * @objective Test 'discoverResource' function with null callback
 * @target DiscoveryTask::Ptr discoverResource(const RCSAddress& address,
 *                   ResourceDiscoveredCallback cb);
 * @test_data 1. host
 *            2. null Callback
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResource() API
 * @post_condition None
 * @expected The API should not crash
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperAPITest_stc, DiscoverResourcesWithoutResType_ICCC_N)
{
    RCSDiscoveryManager* discoveryManager = RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();

    try
    {
        discoveryTask = discoveryManager->discoverResource(host, NULL);
        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);

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
 * @since 2016-03-01
 * @see None
 * @objective Test 'discoverResource' function multiple times
 * @target DiscoveryTask::Ptr discoverResource(const RCSAddress& address,
 *                   ResourceDiscoveredCallback cb);
 * @test_data 1. host
 *            2. Callback
 * @pre_condition Remote Resource Object should be instantialized
 * @procedure Perform discoverResource() API multiple times
 * @post_condition None
 * @expected returned discovery manager should not be null
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperAPITest_stc, DiscoverResourcesWithoutResType_ALVC_N)
{
    RCSDiscoveryManager* discoveryManager = RCSDiscoveryManager::getInstance();

    RCSAddress host = RCSAddress::multicast();

    try
    {
        for(int i = 0; i < 10; i++)
        {
            discoveryTask = discoveryManager->discoverResource(host, bind(&REResourceWrapperAPITest_stc::OnResourceDiscovered, this, PH::_1));
            discoveryTask->cancel();
        }

    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while discovering resource");
    }
}
#endif

/**
 * @since 2016-03-11
 * @see RCSDiscoveryManager getInstance()
 * @objective Test 'discoverResourceByTypes' function with callback validation
 * @target DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                 const std::vector< std::string >& resourceTypes, ResourceDiscoveredCallback cb)
 * @test_data 1. host
 *            2. Resource Types
 *            3. Callback
 * @pre_condition None
 * @procedure Perform discoverResourceByTypes() API with callback validation
 * @post_condition None
 * @expected returned discovery manager should not be null
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperAPITest_stc, DiscoverResourceByTypes_CV_P)
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
                        std::bind(&REResourceWrapperAPITest_stc::OnResourceDiscovered, this, PH::_1));

        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        discoveryTask->cancel();

        if(callbackcheck != true) {
            SET_FAILURE("Callback Condition Check Fail !");
        }

        if (m_foundResourceList.size() < 1)
        {
            SET_FAILURE("All the Resources not Found!!");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while discovering resource");
    }
}
#endif

/**
 * @since 2016-03-11
 * @see RCSDiscoveryManager getInstance()
 * @objective Test 'discoverResourceByTypes' function with callback validation
 * @target DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address, const std::string& relativeUri,
 *                  const std::vector< std::string >& resourceTypes, ResourceDiscoveredCallback cb)
 * @test_data 1. host
 *            2. uri
 *            3. Resource Types
 *            4. Callback
 * @pre_condition None
 * @procedure Perform discoverResourceByTypes() API with callback validation
 * @post_condition None
 * @expected returned discovery manager should not be null
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(REResourceWrapperAPITest_stc, DiscoverResourceByTypesWithUri_CV_P)
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
                        std::bind(&REResourceWrapperAPITest_stc::OnResourceDiscovered, this, PH::_1));

        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        discoveryTask->cancel();

        if(callbackcheck != true) {
            SET_FAILURE("Callback Condition Check Fail !");
        }

        if (m_foundResourceList.size() < 1)
        {
            SET_FAILURE("All the Resources not Found!!");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while discovering resource");
    }
}
#endif
