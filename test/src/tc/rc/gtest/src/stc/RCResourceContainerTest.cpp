/******************************************************************
*
* Copyright 2016 Samsung Electronics All Rights Reserved.
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

#include <string>
#include "RCHelper.h"
#include "gtest/gtest.h"

#if defined(__LINUX__)
#define BUNDLE_PATH "libBMISensorBundle.so"
#define CONFIG_FILE "ResourceContainerConfig.xml"
#define CONFIG_EMPTY_FILE "ResourceContainerEmptyConfig.xml"
#endif

#if defined(__TIZEN__)
#define BUNDLE_PATH "/usr/bin/rc/libBMISensorBundle.so"
#define CONFIG_FILE "/usr/bin/rc/ResourceContainerConfig.xml"
#define CONFIG_EMPTY_FILE "/usr/bin/rc/ResourceContainerEmptyConfig.xml"
#endif

#define emptyString ""
#define nullValue "NULL"
#define zeroValue 0
#define loopInitVal   0

#define BUNDLE_BMI_SENSOR_STRING "oic.bundle.BMISensor"
#define BUNDLE_INSERT_VERSION "Version"
#define INVALID_PARAM "Invalid"
#define INVALID_PATH "invalidBundle.so"
#define INVALID_BUNDLE_NAME "InvalidBundleName"


const std::string BUNDLE_NAME                       = "Bundle";
const std::string BUNDLE_ID                         = "oic.bundle.BMISensor";
const std::string BUNDLE_URL                        = "/BMISensor";
const std::string BUNDLE_VERSION                    = "1.0.0";
const std::string BUNDLE_ACTIVATOR                  = "bmisensor";
const std::string BUNDLE_RESOURCE_URL               = "/softsensor/BMIsensor/1";
const std::string BUNDLE_RESOURCE_TYPE              = "oic.r.sensor";
const std::string BUNDLE_RESOURCE_ADDRESS           = "";

std::unique_ptr<RCSBundleInfo> pBundleInfo;
std::map< string, string > params;
std::list<std::unique_ptr<RCSBundleInfo>> bundleList;
std::string bundleID;
std::string bundlePath;
std::string bundleVer;


class RCResourceContainerAPITest_stc: public ::testing::Test
{
protected:

    RCSResourceContainer *m_pContainer;
    std::string m_errorMsg;
    int m_count;

protected:

    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();

        m_pContainer = RCSResourceContainer::getInstance();
        m_count = 1;

        m_errorMsg = "";
        bundleID="";
        bundlePath="";
        bundleVer="";

        pBundleInfo=nullptr;
    }

    virtual void TearDown()
    {
        CommonUtil::runCommonTCTearDownPart();
    }
};

/**
 * @since 2015-09-08
 * @see GetInstance
 * @see StartContainer
 * @objective Test 'StopContainer' function multiple times with positive way
 * @target void StopContainer(string configFile)
 * @test_data ResourceContainerConfig
 * @pre_condition Get container instance
 * @procedure 1. Perform StopContainer() API
 *            2. Perform StopContainer() API
 *            3. Repeat step 1~2 10 times
 * @post_condition None
 * @expected Should not crashed
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerAPITest_stc, StartStopContainerMultipleTimes_SRC_P)
{
    for (int i = loopInitVal; i < m_count; i++)
    {
        try
        {
            m_pContainer->startContainer(CONFIG_FILE);
            m_pContainer->stopContainer();
        }
        catch (exception& e)
        {
            SET_FAILURE("Exception occurred when stopping container multiple times" + std::string(e.what()));
        }
    }
}
#endif

/**
 * @since 2015-09-07
 * @see get container instance
 * @see startContainer
 * @objective Test 'listBundles' function that provides the bundle list
 * @target list<BundleInfo*> listBundles(void)
 * @test_data ResourceContainerConfig
 * @pre_condition 1. Get container instance
 *                2. Perform StartContainer() API
 * @procedure Perform listBundles() API
 * @post_condition stopContainer
 * @expected list bundles provides bundle details
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerAPITest_stc, ListBundlesForResourceContainerConfig_SRC_P)
{
    m_pContainer->startContainer(CONFIG_FILE);

    pBundleInfo = std::move(*m_pContainer->listBundles().begin());
    ASSERT_EQ(zeroValue, std::string(pBundleInfo->getID()).compare(BUNDLE_BMI_SENSOR_STRING));

    m_pContainer->stopContainer();
}
#endif

/**
 * @since 2015-09-08
 * @see ListBundle
 * @objective Test 'addBundle' function added bundle with ResourceContainerConfig
 * @target void addBundle(string bundleId, string bundleUri, string bundlePath, std::map<string, string> params)
 * @test_data Bundle information
 * @pre_condition Get container instance
 * @procedure Perform addBundle() API
 * @post_condition None
 * @expected Container can added a new bundle
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerAPITest_stc, AddBundleWithCustomBundleInfo_SRC_P)
{
    try
    {
        std::map< std::string, std::string > resourceParams;
        resourceParams.insert(std::make_pair("resourceType", BUNDLE_RESOURCE_TYPE));
        resourceParams.insert(std::make_pair("address", BUNDLE_RESOURCE_ADDRESS));


        m_pContainer->startContainer(CONFIG_EMPTY_FILE);

        bundleList = m_pContainer->listBundles();
        cout << "Initial Bundle list : " << bundleList.size() << endl;

        m_pContainer->addBundle(BUNDLE_ID, BUNDLE_URL, BUNDLE_PATH, BUNDLE_ACTIVATOR, resourceParams);

        cout << "After added, bundle list : " << m_pContainer->listBundles().size()<<endl;

        EXPECT_EQ(bundleList.size() + 1, m_pContainer->listBundles().size());

        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside AddBundle_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-09-09
 * @see get Instance
 * @see startContainer
 * @objective Test 'startBundle' and 'stopBundle' function multiple times
 * @target void startBundle(string bundleId)
 * @test_data Bundle information
 * @pre_condition None
 * @procedure 1. Get container instance
              2. Perform startBundle() API
              3. Perform stopBundle() API
              4. Repeat 1~3 10 times
 * @post_condition stopContainer
 * @expected Container can start and stop bundle successfully
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerAPITest_stc, StartAndStopBundleMultipleTimes_SRC_P)
{

    for (int i = loopInitVal; i < m_count; i++)
    {
        m_pContainer->startContainer(CONFIG_FILE);
        pBundleInfo = std::move(*m_pContainer->listBundles().begin());

        try
        {
            m_pContainer->startBundle(pBundleInfo->getID());
            IOTIVITYTEST_LOG(DEBUG, "Loop:  %d", i);

        }
        catch (exception& e)
        {
            SET_FAILURE("Exception occurred at startBundle() when adding multiple times. " + std::string(e.what()));
            return;
        }

        try
        {
            m_pContainer->stopBundle(pBundleInfo->getID());
            IOTIVITYTEST_LOG(DEBUG, "Loop:  %d", i);
        }
        catch (exception& e)
        {
            SET_FAILURE("Exception occurred at stopBundle() when adding multiple times. " + std::string(e.what()));
            return;
        }

        m_pContainer->stopContainer();
    }
}
#endif

/**
 * @since 2015-09-09
 * @see void addBundle(string bundleId, string bundleUri, string bundlePath, std::map<string, string> params)
 * @objective Test 'startBundle' function started bundle successfully
 * @target void startBundle(string bundleId)
 * @test_data Bundle information
 * @pre_condition Get container instance
 * @procedure 1. Add custom bundle
 *            2. Perform startBundle() API
 * @post_condition None
 * @expected startBundle should not crashed
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerAPITest_stc, StartBundleWithoutResourceContainerConfig_SRC_P)
{
    try
    {
        params.insert(std::make_pair(BUNDLE_INSERT_VERSION, BUNDLE_VERSION));

        m_pContainer->addBundle(BUNDLE_ID, BUNDLE_URL, BUNDLE_PATH, BUNDLE_ACTIVATOR, params);

        m_pContainer->startBundle(BUNDLE_ID);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside StartBundle_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-09-09
 * @see None
 * @objective Test 'removeBundle' function with negative way
 * @target void removeBundle(string bundleId)
 * @test_data None
 * @pre_condition 1. Get container instance
 *                2. Add bundle
 *                3. Remove bundle
 * @procedure Perform removeBundle() API
 * @post_condition None
 * @expected Container can remove bundle
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerAPITest_stc, RemoveBundleWhichAlreadyRemoved_USTC_N)
{
    try
    {
        params.insert(std::make_pair(BUNDLE_INSERT_VERSION, BUNDLE_VERSION));

        m_pContainer->addBundle(BUNDLE_ID, BUNDLE_URL, BUNDLE_PATH, BUNDLE_ACTIVATOR, params);

        m_pContainer->removeBundle(BUNDLE_ID);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred at precontion. " + std::string(e.what()));
    }

    try {
        m_pContainer->removeBundle(BUNDLE_ID);
    } catch (exception& e) {
        SET_FAILURE("Exception occurred when removeBundle. " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-09-09
 * @see None
 * @objective Test 'removeBundle' function with invalid bundleID
 * @target void removeBundle(string bundleId)
 * @test_data Invalid bundleId
 * @pre_condition Get container instance
 * @procedure Perform removeBundle() with Invalid bundleId
 * @post_condition None
 * @expected Container can not remove bundle
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerAPITest_stc, RemoveBundleWithInvalidBundleId_USV_N)
{
    try
    {
        m_pContainer->removeBundle(INVALID_PARAM);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred when removing invalid Bundle ID. " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-09-10
 * @see None
 * @objective Test 'addBundle' with invalid bundle path and 'startBundle' that started the bundle
 * @target void addBundle(string bundleId, string bundleUri, string bundlePath, std::map<string, string> params)
 * @target void startBundle(string bundleId)
 * @test_data Bundle data
 * @pre_condition Get container instance
 * @procedure 1. AddBundle with invalid bundle path
 *            2. Perform startBundle() API
 * @post_condition None
 * @expected addBundle and startBundle should not crash occurs
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerAPITest_stc, StartBundleWithInvalidBundlePath_USV_N)
{
    try
    {
        params.insert(std::make_pair(BUNDLE_INSERT_VERSION, BUNDLE_VERSION));

        m_pContainer->addBundle(BUNDLE_ID, BUNDLE_URL, INVALID_PATH, BUNDLE_ACTIVATOR, params);

        m_pContainer->startBundle(BUNDLE_ID);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside adding invalid bundle path and start bundle." + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-09-11
 * @see startContainer
 * @objective Test 'getID' function provides bundleID
 * @target string getID()
 * @test_data ResourceContainerConfig
 * @pre_condition Perform build() API
 * @procedure Perform getID() API
 * @post_condition stopContainer
 * @expected bundleID should not empty
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerAPITest_stc, GetBundleId_SRC_P)
{
    try
    {
        m_pContainer->startContainer(CONFIG_FILE);

        bundleID = (*m_pContainer->listBundles().begin())->getID();

        ASSERT_FALSE(bundleID.empty());

        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside GetBundleIdWithoutSet_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-09-11
 * @see startContainer
 * @objective Test 'getPath' function provides bundlePath
 * @target string getPath()
 * @test_data ResourceContainerConfig
 * @pre_condition Perform build() API
 * @procedure Perform getPath() API
 * @post_condition stopContainer
 * @expected bundlePath should not empty
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerAPITest_stc, GetBundlePath_SRC_P)
{
    try
    {
        m_pContainer->startContainer(CONFIG_FILE);

        bundlePath = (*m_pContainer->listBundles().begin())->getPath();

        ASSERT_FALSE(bundlePath.empty());

        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside GetBundleIdWithoutSet_P" + std::string(e.what()));
    }
}
#endif

/**
* @since 2015-09-11
* @see StartContainer
* @objective Test 'getVersion' function provides bundle version
* @target string getVersion()
* @test_data ResourceContainerConfig
* @pre_condition Perform build() API
* @procedure Perform getVersion() API
* @post_condition stopContainer
* @expected bundleID should not empty
**/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerAPITest_stc, GetBundleVersion_SRC_P)
{
   try
   {
        m_pContainer->startContainer(CONFIG_FILE);

        bundleVer = (*m_pContainer->listBundles().begin())->getVersion();

        ASSERT_FALSE(bundleVer.empty());

        m_pContainer->stopContainer();
   }
   catch (exception& e)
   {
       SET_FAILURE("Exception occurred inside GetBundleIdWithoutSet_P" + std::string(e.what()));
   }
}
#endif

/**
 * @since 2016-02-18
 * @see startContainer
 * @see startBundle
 * @objective Test 'stopBundle' function that stop bundle but the bundle was not started
 * @target void stopBundle(string bundleId)
 * @test_data ResourceContainerConfig
 * @pre_condition 1. Get container instance
 *                2. Perform StartContainer() API
 * @procedure 1. Perform stopBundle() API
 *            2. Perform startBundle() API
 * @post_condition None
 * @expected stopBundle should not crashed
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerAPITest_stc, StopBundle_USTC_N)
{
        try
        {
            m_pContainer->startContainer(CONFIG_FILE);

            pBundleInfo = std::move(*m_pContainer->listBundles().begin());
            m_pContainer->stopBundle(pBundleInfo->getID());
            m_pContainer->startBundle(pBundleInfo->getID());

            m_pContainer->stopContainer();
        }
        catch (exception& e)
        {
            SET_FAILURE("Exception occurred at StopBundleUSTC_N " + std::string(e.what()));
        }

}
#endif

/**
 * @since 2016-02-18
 * @see None
 * @objective Test 'removeBundle' that's Remove bundle
 * @target void removeBundle(string bundleId)
 * @test_data None
 * @pre_condition 1. Get container instance
 * @procedure 1. Perform removeBundle() API
 *            2. Perform startBundle() API
 * @post_condition None
 * @expected Should not crashed
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerAPITest_stc, RemoveBundle_USTC_N)
{
    try
    {
        params.insert(std::make_pair(BUNDLE_INSERT_VERSION, BUNDLE_VERSION));
        m_pContainer->removeBundle(BUNDLE_ID);
        m_pContainer->addBundle(BUNDLE_ID, BUNDLE_URL, BUNDLE_PATH, BUNDLE_ACTIVATOR, params);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred at RemoveBundleUSTC_N " + std::string(e.what()));
    }

}
#endif

/**
 * @since 2016-02-18
 * @see None
 * @objective Test 'removeResourceConfig' that Remove resourceConfig
 * @target void removeResourceConfig(string bundleId)
 * @test_data None
 * @pre_condition 1. Get container instance
 *                2. Add resource
 * @procedure 1. Perform removeResourceConfig() API
 *            2. Perform addResourceConfig() API
 * @post_condition None
 * @expected Should not crashed
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerAPITest_stc, RemoveResourceConfig_USTC_N)
{
    try
    {
        params.insert(std::make_pair(BUNDLE_INSERT_VERSION, BUNDLE_VERSION));
        m_pContainer->removeResourceConfig(BUNDLE_NAME, BUNDLE_URL);
        m_pContainer->addResourceConfig(BUNDLE_NAME, BUNDLE_URL, params);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside RemoveResourceConfig_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-02-18
 * @see Add Resource Config
 * @objective Test 'removeResourceConfig' that Remove resourceConfig in multiple times
 * @target void removeResourceConfig(string bundleId)
 * @test_data None
 * @pre_condition 1. Get container instance
 *                2. Add resource
 * @procedure Perform addResourceConfig() & removeResourceConfig() API multiple times
 * @post_condition None
 * @expected Container can remove resource in resourceConfig
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerAPITest_stc, RemoveResourceConfig_SLCC_P)
{
    params.insert(std::make_pair(BUNDLE_INSERT_VERSION, BUNDLE_VERSION));
    for (int i = loopInitVal; i < m_count; i++)
    {
        try
        {
            m_pContainer->addResourceConfig(BUNDLE_NAME, BUNDLE_URL, params);
            m_pContainer->removeResourceConfig(BUNDLE_NAME, BUNDLE_URL);
        }
        catch (exception& e)
        {
            SET_FAILURE("Exception occurred at RemoveResourceConfigSLCC_P " + std::string(e.what()));
        }
    }
}
#endif

/**
 * @since 2016-02-22
 * @see Add bundle
 * @objective Test 'removeBundle' that Remove bundle in multiple times
 * @target void removeBundle(string bundleId)
 * @test_data None
 * @pre_condition Get container instance
 * @procedure Perform addBundle() & removeBundle() API multiple times
 * @post_condition None
 * @expected Container can remove bundle
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerAPITest_stc, RemoveBundle_SLCC_P)
{
    params.insert(std::make_pair(BUNDLE_INSERT_VERSION, BUNDLE_VERSION));

    m_pContainer->startContainer(CONFIG_FILE);

    for (int i = loopInitVal; i < m_count; i++)
    {
        try
        {
            m_pContainer->addBundle(BUNDLE_ID, BUNDLE_URL, BUNDLE_PATH, BUNDLE_ACTIVATOR, params);
            m_pContainer->removeBundle(BUNDLE_ID);
            sleep(2);
        }
        catch (exception& e)
        {
            SET_FAILURE("Exception occurred at RemoveBundleSLCC_P " + std::string(e.what()));
        }
    }

    m_pContainer->stopContainer();
}
#endif

/**
 * @since 2016-02-22
 * @see None
 * @objective Test 'removeBundle' that remove bundle in multiple times
 * @target void removeBundle(string bundleId)
 * @test_data None
 * @pre_condition Get container instance
 * @procedure Perform removeBundle() API without performing addBundle() API
 * @post_condition None
 * @expected removeBundle should not crashed
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerAPITest_stc, RemoveBundle_DSCC_N)
{
    for (int i = loopInitVal; i < m_count; i++)
    {
        try
        {
            params.insert(std::make_pair(BUNDLE_INSERT_VERSION, BUNDLE_VERSION));
            m_pContainer->removeBundle(BUNDLE_ID);
        }
        catch (exception& e)
        {
            SET_FAILURE("Exception occurred at RemoveBundleDSCC_N " + std::string(e.what()));
        }
    }
}
#endif

/**
 * @since 2015-09-22
 * @see None
 * @objective Test 'addResourceConfig' added resources in multiple times
 * @target void addResourceConfig(string bundleId, string resourceUri, std::map<string, string> params)
 * @test_data Bundle information
 * @pre_condition None
 * @procedure 1. Get container instance
 *            2. Perform addResourceConfig() API
 *            3. Repeat steps 1~2 multiple times
 * @post_condition None
 * @expected Container can add resource in resourceConfig
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerAPITest_stc, AddResourceConfig_VLCC_P)
{
    for (int i = loopInitVal; i < m_count; i++)
    {
        try
        {
            params.insert(std::make_pair(BUNDLE_INSERT_VERSION, BUNDLE_VERSION));

            m_pContainer->addResourceConfig(BUNDLE_NAME, BUNDLE_URL, params);
        }
        catch (exception& e)
        {
            SET_FAILURE("Exception occurred inside AddResourceConfigVLCC_P" + std::string(e.what()));
        }
    }
}
#endif
