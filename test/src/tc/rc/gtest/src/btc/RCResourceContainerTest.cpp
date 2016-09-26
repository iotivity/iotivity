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
#define BUNDLE_PATH "libHueBundle.so"
#define CONFIG_FILE "ResourceContainerConfig.xml"
#define CONFIG_EMPTY_FILE "ResourceContainerEmptyConfig.xml"
#endif

#if defined(__TIZEN__)
#define BUNDLE_PATH "/usr/bin/rc/libHueBundle.so"
#define CONFIG_FILE "/usr/bin/rc/ResourceContainerConfig.xml"
#define CONFIG_EMPTY_FILE "/usr/bin/rc/ResourceContainerEmptyConfig.xml"
#endif

#define BUNDLE_NAME "Bundle"
#define BUNDLE_URL "bundle/url"
#define BUNDLE_VERSION "version"
#define BUNDLE_ACTIVATOR "activator"
#define BUNDLE_INSERT_VERSION "Version"
#define BMI_BUNDLE_ID "oic.bundle.BMISensor"
#define BMI_BUNDLE_PATH "libBMISensorBundle.so"
#define BMI_BUNDLE_VERSION "1.0.0"
#define EMPTY_STRING ""
#define INVALID_INPUT "invalidConfig"
#define NULL_VALUE NULL
#define ZERO_VALUE 0

const std::string HUE_BUNDLE_ID = "oic.bundle.hueSample";
const std::string HUE_BUNDLE_URI = "/hueSample";
const std::string HUE_BUNDLE_PATH = "libHueBundle.so";
const std::string HUE_BUNDLE_ACTIVATOR = "huesample";
const std::string HUE_RESOURCE_URI = "/hue/light/sample";
const std::string HUE_RESOURCE_TYPE = "oic.r.light";
const std::string HUE_ADDRESS = "http://192.168.0.2/api/newdeveloper/lights/1";
const std::string BMI_BUNDLE_ID_CONST = "oic.bundle.BMISensor";
const std::string BMI_BUNDLE_PATH_CONST = "libBMISensorBundle.so";
const std::string BUNDLE_VERSION_CONST = "1.0.0";

static string s_bundleID;
const string &g_config_file_address = CONFIG_FILE;
std::map< string, string > g_params;
std::list< std::unique_ptr< RCSBundleInfo > > g_bundles;
std::list< std::unique_ptr< RCSBundleInfo > > g_newBundles;
std::unique_ptr< RCSBundleInfo > g_bundleInfo;
int nValue;

class RCResourceContainerTest_btc: public ::testing::Test
{

public:
    int initialBundleSize;
    int loadedBundleSize;
    int initialBundleResourceSize;
    int loadedBundleResourceSize;

protected:
    std::string m_errorMsg;
    RCSResourceContainer *m_pContainer;

private:

protected:

    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();

        m_errorMsg = EMPTY_STRING;
        m_pContainer = RCSResourceContainer::getInstance();

        initialBundleSize = 0;
        loadedBundleSize = 0;
        initialBundleResourceSize = 0;
        loadedBundleResourceSize = 0;
    }

    virtual void TearDown()
    {
        CommonUtil::runCommonTCTearDownPart();
    }
};

/**
 * @since 2015-07-16
 * @see None
 * @objective Test 'getInstance' function that provides instance for RC ResourceContainer
 * @target void getInstance(void)
 * @test_data None
 * @pre_condition None
 * @procedure Perform getInstance() API
 * @post_condition None
 * @expected Container can get ResourceContainer instance.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, GetInstance_SRC_P)
{
    ASSERT_NE(NULL_VALUE, RCSResourceContainer::getInstance())<<"Get Instance Not Found!";
}
#endif

/**
 * @since 2015-07-16
 * @see getInstance
 * @objective Test 'StartContainer' function that's start resource container with resource config information
 * @target void StartContainer(string configFile)
 * @test_data ResourceContainerConfig
 * @pre_condition Get container instance
 * @procedure Perform StartContainer() API
 * @post_condition None
 * @expected Container can gather information from ResourceContainerConfig.xml file.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, StartContainer_SRC_P)
{
    try
    {
        m_pContainer->startContainer(CONFIG_FILE);
        EXPECT_GT(m_pContainer->listBundles().size(), ZERO_VALUE) << "Container is Not started !";
        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside StartContainer_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-16
 * @see getInstance
 * @objective Test 'StartContainer' function that's does not start resource container with empty resource config file
 * @target void StartContainer(string configFile)
 * @test_data None
 * @pre_condition Get container instance
 * @procedure Perform StartContainer() API
 * @post_condition None
 * @expected start container should No crash occurs
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, StartContainer_ESV_N)
{
    try
    {
        m_pContainer->startContainer(CONFIG_EMPTY_FILE);
        EXPECT_EQ(ZERO_VALUE, m_pContainer->listBundles().size()) << "Container Started with empty string!";
        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside StartContainer_N" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-07-26
 * @see getInstance
 * @objective Test 'StartContainer' function that's does not start resource container with invalid resource config file
 * @target void StartContainer(string configFile)
 * @test_data None
 * @pre_condition Get container instance
 * @procedure Perform StartContainer() API
 * @post_condition None
 * @expected start container should Not started and verify with the list bundle size
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, StartContainer_USV_N)
{
    try
    {
        m_pContainer->startContainer(INVALID_INPUT);
        EXPECT_EQ(ZERO_VALUE, m_pContainer->listBundles().size()) << "Container Started with invalid parameter!";
        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside StartContainer_N" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-16
 * @see getInstance
 * @see startContainer
 * @objective Test 'StopContainer' function that's stop resource container
 * @target void StopContainer(void)
 * @test_data ResourceContainerConfig
 * @pre_condition 1. Get container instance
 *                2. Perform StartContainer() API
 * @procedure Perform StopContainer() API
 * @post_condition None
 * @expected Container can stop container.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, StopContainer_SRC_P)
{
    try
    {
        m_pContainer->startContainer(CONFIG_FILE);
        ASSERT_GT(m_pContainer->listBundles().size(), ZERO_VALUE) << "Container is Not started !";

        m_pContainer->stopContainer();
        EXPECT_EQ(m_pContainer->listBundles().size(), ZERO_VALUE) << "Container can't stopped!";
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside StopContainer_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-07-27
 * @see getInstance
 * @see startContainer
 * @objective Test 'StopContainer' function without start startContainer
 * @target void StopContainer(void)
 * @test_data ResourceContainerConfig
 * @pre_condition Get container instance
 * @procedure Perform StopContainer() API
 * @post_condition None
 * @expected Container can not stop container because container not started before.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, StopContainer_SQV_P)
{
    try
    {
        m_pContainer->startContainer(INVALID_INPUT);
        ASSERT_EQ(ZERO_VALUE, m_pContainer->listBundles().size()) << "Container Started with invalid parameter!";

        m_pContainer->stopContainer();
        EXPECT_EQ(ZERO_VALUE, m_pContainer->listBundles().size()) << "Container stopped without started before!";
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside StopContainer_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-07-27
 * @see getInstance
 * @see startContainer
 * @objective Test 'StopContainer' function that start startContainer with invalid parameter first
 * @target void StopContainer(void)
 * @test_data ResourceContainerConfig
 * @pre_condition 1. Get container instance
 *                2. Perform StartContainer() API
 * @procedure Perform StopContainer() API
 * @post_condition None
 * @expected Container can not stop container.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, StopContainer_USV_N)
{
    try
    {
        m_pContainer->startContainer(INVALID_INPUT);
        m_pContainer->stopContainer();
        EXPECT_EQ(ZERO_VALUE, m_pContainer->listBundles().size())
        << "Container Stopped successfully but container started with invalid parameter!";
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside StopContainer_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-16
 * @see getInstance
 * @see startContainer
 * @objective Test 'listBundles' function gives bundle list in positive way
 * @target list<BundleInfo*> listBundles(void)
 * @test_data ResourceContainerConfig
 * @pre_condition 1. Get container instance
 *                2. Perform StartContainer() API
 * @procedure Perform listBundles() API
 * @post_condition Stop container
 * @expected Container provides list of bundles.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, ListBundles_SRC_P)
{
    m_pContainer->startContainer(CONFIG_FILE);

    g_bundles = m_pContainer->listBundles();

    EXPECT_GT(g_bundles.size(), ZERO_VALUE) << "Bundle list found!";

    m_pContainer->stopContainer();
}
#endif

/**
 * @since 2015-07-16
 * @see getInstance
 * @see startContainer
 * @objective Test 'listBundles' function gives bundle list in positive way
 * @target list<BundleInfo*> listBundles(void)
 * @test_data ResourceContainerConfig
 * @pre_condition 1. Get container instance
 *                2. Perform StartContainer() API
 * @procedure Perform listBundles() API
 * @post_condition Stop container
 * @expected Container provides list of bundles and verify with bundle ID, Path and version info.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, ListBMIBundles_SQV_P)
{
    m_pContainer->startContainer(CONFIG_FILE);

    EXPECT_STREQ(BMI_BUNDLE_ID, (*m_pContainer->listBundles().begin())->getID().c_str())
    << "BMI bundle id found!";
    EXPECT_STREQ(BMI_BUNDLE_PATH, (*m_pContainer->listBundles().begin())->getPath().c_str())
    << "BMI bundle path found!";
    EXPECT_STREQ(BMI_BUNDLE_VERSION, (*m_pContainer->listBundles().begin())->getVersion().c_str())
    << "BMI bundle version found!";

    m_pContainer->stopContainer();
}
#endif

/**
 * @since 2015-07-16
 * @see None
 * @objective Test 'listBundles' function does not give bundle list in negative way
 * @target list<BundleInfo*> listBundles(void)
 * @test_data None
 * @pre_condition 1. Get container instance
 *                2. start container
 * @procedure Perform listBundles() API
 * @post_condition Stop container
 * @expected empty list of bundles.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, ListBundles_ESV_N)
{
    m_pContainer->startContainer(CONFIG_EMPTY_FILE);

    g_bundles = m_pContainer->listBundles();

    EXPECT_EQ(ZERO_VALUE, g_bundles.size())<<"Bundle list id greater than zero!";

    m_pContainer->stopContainer();
}
#endif

/**
 * @since 2016-07-26
 * @see None
 * @objective Test 'listBundles' function does not give bundle list in negative way
 * @target list<BundleInfo*> listBundles(void)
 * @test_data None
 * @pre_condition 1. Get container instance
 *                2. start container
 * @procedure Perform listBundles() API
 * @post_condition Stop container
 * @expected empty list of bundles.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, ListBundles_USV_N)
{
    m_pContainer->startContainer(INVALID_INPUT);

    g_bundles = m_pContainer->listBundles();

    EXPECT_EQ(ZERO_VALUE, g_bundles.size())<<"Bundle size is greater than zero through container start with invalid parameter!";

    m_pContainer->stopContainer();
}
#endif

/**
 * @since 2015-07-16
 * @see startContainer
 * @objective Test 'startBundle' function start bundle in positive way
 * @target void startBundle(string bundleID)
 * @test_data ResourceContainerConfig
 * @pre_condition 1. Get container instance
 *                2. Perform StartContainer() API
 * @procedure Perform startBundle() API
 * @post_condition Stop container
 * @expected Container can start bundle.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, StartBundle_SRC_P)
{
    try
    {
        m_pContainer->startContainer(CONFIG_FILE);

        g_bundleInfo = std::move(*m_pContainer->listBundles().begin());

        m_pContainer->startBundle(g_bundleInfo->getID());

        EXPECT_GT(m_pContainer->listBundles().size(), ZERO_VALUE)<<"Bundle size is NOT greater than zero!";

        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside StartBundle_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-16
 * @see get instance
 * @see startContainer
 * @objective Test 'startBundle' function start bundle without start container
 * @target void startBundle(string bundleID)
 * @test_data None
 * @pre_condition Get container instance
 * @procedure Perform startBundle() API
 * @post_condition None
 * @expected No crash occurs
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, StartBundle_ESV_N)
{
    try
    {
        m_pContainer->startContainer(CONFIG_EMPTY_FILE);
        m_pContainer->startBundle(EMPTY_STRING);
        EXPECT_EQ(m_pContainer->listBundles().size(), ZERO_VALUE)<<"Bundle size is NOT zero!";
        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside StartBundle_N" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-16
 * @see get instance
 * @see startContainer
 * @objective Test 'startBundle' with invalid input
 * @target void startBundle(string bundleID)
 * @test_data None
 * @pre_condition Get container instance
 * @procedure Perform startBundle() API
 * @post_condition None
 * @expected No crash occurs
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, StartBundle_USV_N)
{
    try
    {
        m_pContainer->startContainer(CONFIG_EMPTY_FILE);
        m_pContainer->startBundle(INVALID_INPUT);
        EXPECT_EQ(m_pContainer->listBundles().size(), ZERO_VALUE)<<"Bundle size is NOT zero!";
        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside StartBundle_N" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-16
 * @see startContainer
 * @see startBundle
 * @objective Test 'stopBundle' function stop bundle in positive way
 * @target void stopBundle(string bundleID)
 * @test_data ResourceContainerConfig
 * @pre_condition 1. Get container instance
 *                2. Perform StartContainer() API
 *                3. Get bundle list
 * @procedure Perform stopBundle() API
 * @post_condition StopContainer
 * @expected Container can stop bundle
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, StopBundle_SRC_P)
{
    try
    {
        m_pContainer->startContainer(CONFIG_FILE);

        g_bundleInfo = std::move(*m_pContainer->listBundles().begin());

        m_pContainer->startBundle(g_bundleInfo->getID());
        m_pContainer->stopBundle(g_bundleInfo->getID());

        m_pContainer->stopContainer();

        EXPECT_EQ(m_pContainer->listBundles().size(), ZERO_VALUE)<<"Bundle size is NOT zero!";
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside StartBundle_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-16
 * @see None
 * @objective Test 'stopBundle' function does not stop bundle without start bundle
 * @target void stopBundle(string bundleID)
 * @test_data None
 * @pre_condition Get container instance
 * @procedure Perform stopBundle() API
 * @post_condition None
 * @expected stop bundle can't stop bundle
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, StopBundle_ESV_N)
{
    try
    {
        m_pContainer->startContainer(CONFIG_EMPTY_FILE);
        m_pContainer->stopBundle(EMPTY_STRING);
        EXPECT_EQ(m_pContainer->listBundles().size(), ZERO_VALUE)<<"Bundle size is NOT zero!";
        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside StartBundle_N" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-16
 * @see None
 * @objective Test 'stopBundle' function with invalid input
 * @target void stopBundle(string bundleID)
 * @test_data None
 * @pre_condition Get container instance
 * @procedure Perform stopBundle() API
 * @post_condition None
 * @expected stop bundle can't stop bundle
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, StopBundle_USV_N)
{
    try
    {
        m_pContainer->startContainer(CONFIG_EMPTY_FILE);
        m_pContainer->stopBundle(INVALID_INPUT);
        EXPECT_EQ(m_pContainer->listBundles().size(), ZERO_VALUE)<<"Bundle size is NOT zero!";
        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside StartBundle_N" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-16
 * @see Get container instance
 * @objective Test 'addBundle' function add bundle data to RCResourceConfig.xml file in positive way
 * @target void addBundle(string bundleID, string bundleUri, string bundlePath, std::map<string, string> btcParams)
 * @test_data Bundle information
 * @pre_condition Get container instance
 * @procedure Perform addBundle() API
 * @post_condition None
 * @expected Container can add bundle
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, AddBundle_SRC_P)
{
    try
    {
        m_pContainer->startContainer(CONFIG_EMPTY_FILE);

        g_bundles = m_pContainer->listBundles();

        g_params.insert(std::make_pair(BUNDLE_INSERT_VERSION, BUNDLE_VERSION));

        m_pContainer->addBundle(BUNDLE_NAME, BUNDLE_URL, BUNDLE_PATH, BUNDLE_ACTIVATOR, g_params);

        EXPECT_GT(m_pContainer->listBundles().size(), g_bundles.size())<<"New bundle is not added!";

        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside AddBundle_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-16
 * @see None
 * @objective Test 'addBundle' function does not add bundle without data but not crash
 * @target void addBundle(string bundleID, string bundleUri, string bundlePath, std::map<string, string> params)
 * @test_data None
 * @pre_condition Get container instance
 * @procedure Perform addBundle() API
 * @post_condition None
 * @expected No crash occurs
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, AddBundle_ESV_N)
{
    try
    {
        m_pContainer->startContainer(CONFIG_EMPTY_FILE);
        m_pContainer->addBundle( EMPTY_STRING, EMPTY_STRING, EMPTY_STRING, EMPTY_STRING, g_params);
        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside AddBundle_N" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-16
 * @see None
 * @objective Test 'addBundle' function does not add bundle without data but not crash
 * @target void addBundle(string bundleID, string bundleUri, string bundlePath, std::map<string, string> params)
 * @test_data None
 * @pre_condition Get container instance
 * @procedure Perform addBundle() API
 * @post_condition None
 * @expected No crash occurs
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, AddBundle_USV_N)
{
    try
    {
        m_pContainer->startContainer(CONFIG_EMPTY_FILE);
        m_pContainer->addBundle( INVALID_INPUT, INVALID_INPUT, INVALID_INPUT, INVALID_INPUT, g_params);
        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside AddBundle_N" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-16
 * @see addBundle
 * @objective Test 'removeBundle' function remove bundle in positive way
 * @target void removeBundle(string bundleID)
 * @test_data None
 * @pre_condition 1. Get container instance
 *                2. Add bundle
 * @procedure Perform removeBundle() API
 * @post_condition None
 * @expected Container can remove bundle
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, RemoveBundle_SRC_P)
{
    try
    {
        m_pContainer->startContainer(CONFIG_EMPTY_FILE);

        g_bundles = m_pContainer->listBundles();

        g_params.insert(std::make_pair(BUNDLE_INSERT_VERSION, BUNDLE_VERSION));

        m_pContainer->addBundle(BUNDLE_NAME, BUNDLE_URL, BUNDLE_PATH, BUNDLE_ACTIVATOR, g_params);

        EXPECT_GT(m_pContainer->listBundles().size(), g_bundles.size())<<"New bundle is not added!";

        m_pContainer->removeBundle(BUNDLE_NAME);

        EXPECT_EQ(m_pContainer->listBundles().size(), g_bundles.size())<<"Bundle is not removed!";

        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside RemoveBundle_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-16
 * @see None
 * @objective Test 'removeBundle' function does not remove bundle without add bundle
 * @target void removeBundle(string bundleID)
 * @test_data None
 * @pre_condition Get container instance
 * @procedure Perform removeBundle() API
 * @post_condition None
 * @expected remove bundle should not crash occurs
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, RemoveBundle_ESV_N)
{
    try
    {
        m_pContainer->startContainer(CONFIG_FILE);
        initialBundleSize = m_pContainer->listBundles().size();

        m_pContainer->removeBundle(EMPTY_STRING);
        loadedBundleSize = m_pContainer->listBundles().size();

        EXPECT_EQ(loadedBundleSize, initialBundleSize)<< "Bundle is removed without bundleID!";

        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside RemoveBundle_N" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-16
 * @see None
 * @objective Test 'removeBundle' function with invalid input
 * @target void removeBundle(string bundleID)
 * @test_data None
 * @pre_condition Get container instance
 * @procedure Perform removeBundle() API
 * @post_condition None
 * @expected remove bundle should not crash occurs
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, RemoveBundle_USV_N)
{
    try
    {
        m_pContainer->startContainer(CONFIG_FILE);
        initialBundleSize = m_pContainer->listBundles().size();

        m_pContainer->removeBundle(INVALID_INPUT);
        loadedBundleSize = m_pContainer->listBundles().size();

        EXPECT_EQ(loadedBundleSize, initialBundleSize) << "Bundle is removed with invalid bundleID!";

        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside RemoveBundle_N" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-16
 * @see None
 * @objective Test 'addResourceConfig' function add resource in positive way
 * @target void addResourceConfig(string bundleID, string resourceUri, std::map<string, string> params)
 * @test_data Bundle information
 * @pre_condition Get container instance
 * @procedure Perform addResourceConfig() API
 * @post_condition None
 * @expected Container can add resource in resourceConfig
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, AddResourceConfig_SRC_P)
{
    try
    {
        m_pContainer->startContainer(CONFIG_FILE);
        initialBundleResourceSize = m_pContainer->listBundleResources(HUE_BUNDLE_ID).size();

        g_params.insert(std::make_pair("resourceType", HUE_RESOURCE_TYPE));
        g_params.insert(std::make_pair("address", HUE_ADDRESS));

        m_pContainer->addResourceConfig(HUE_BUNDLE_ID, HUE_BUNDLE_URI, g_params);
        loadedBundleResourceSize = m_pContainer->listBundleResources(HUE_BUNDLE_ID).size();

        EXPECT_GT(loadedBundleResourceSize, initialBundleResourceSize) << "Resource Config is not added!";

        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside AddResourceConfig_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-16
 * @see None
 * @objective Test 'addResourceConfig' function does not add resource in a empty string
 * @target void addResourceConfig(string bundleID, string resourceUri, std::map<string, string> params)
 * @test_data None
 * @pre_condition Get container instance
 * @procedure Perform addResourceConfig() API
 * @post_condition None
 * @expected No crash occurs
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, AddResourceConfig_ESV_N)
{
    try
    {
        m_pContainer->startContainer(CONFIG_FILE);

        nValue = m_pContainer->listBundleResources((*m_pContainer->listBundles().begin())->getID()).size();

        m_pContainer->addResourceConfig(EMPTY_STRING, EMPTY_STRING, g_params);

        EXPECT_EQ(m_pContainer->listBundleResources((*m_pContainer->listBundles().begin())->getID()).size(), nValue)
                << "Resource Config is added with empty string!";

        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside AddResourceConfig_N" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-16
 * @see None
 * @objective Test 'addResourceConfig' function does not add resource in a invalid input
 * @target void addResourceConfig(string bundleId, string resourceUri, std::map<string, string> params)
 * @test_data None
 * @pre_condition Get container instance
 * @procedure Perform addResourceConfig() API
 * @post_condition None
 * @expected No crash occurs
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, AddResourceConfig_USV_N)
{
    try
    {
        m_pContainer->startContainer(CONFIG_FILE);

        nValue = m_pContainer->listBundleResources((*m_pContainer->listBundles().begin())->getID()).size();

        m_pContainer->addResourceConfig(INVALID_INPUT, INVALID_INPUT, g_params);

        EXPECT_EQ(m_pContainer->listBundleResources((*m_pContainer->listBundles().begin())->getID()).size(), nValue)
                << "Resource Config is added with invalid input!";
        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside AddResourceConfig_N" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-16
 * @see addResourceConfig
 * @objective Test 'removeResourceConfig' function remove resource config in positive way
 * @target void removeResourceConfig(string bundleId)
 * @test_data None
 * @pre_condition 1. Get container instance
 *                2. Add resource
 * @procedure Perform removeResourceConfig() API
 * @post_condition None
 * @expected Container can remove resource in resourceConfig
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, RemoveResourceConfig_SRC_P)
{
    try
    {
        m_pContainer->startContainer(CONFIG_FILE);

        nValue = m_pContainer->listBundleResources((*m_pContainer->listBundles().begin())->getID()).size();

        g_params.insert(std::make_pair(BUNDLE_INSERT_VERSION, BUNDLE_VERSION));

        m_pContainer->addResourceConfig(BUNDLE_NAME, BUNDLE_URL, g_params);

        m_pContainer->removeResourceConfig(BUNDLE_NAME, BUNDLE_URL);

        EXPECT_EQ(m_pContainer->listBundleResources((*m_pContainer->listBundles().begin())->getID()).size(), nValue)
                << "Remove resourceConfig does not work that's why before addResourceConfig list bundle size and after addResourceConfig then removeResourceConfig list bundle size is Not equal!";
        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside RemoveResourceConfig_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-16
 * @see None
 * @objective Test 'removeResourceConfig' function does not remove resource with empty string
 * @target void removeResourceConfig(string bundleId)
 * @test_data None
 * @pre_condition Get container instance
 * @procedure Perform removeResourceConfig() API
 * @post_condition None
 * @expected No crash occurs
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, RemoveResourceConfig_ESV_N)
{
    try
    {
        m_pContainer->startContainer(CONFIG_FILE);

        nValue = m_pContainer->listBundleResources((*m_pContainer->listBundles().begin())->getID()).size();

        m_pContainer->removeResourceConfig(EMPTY_STRING, EMPTY_STRING);

        EXPECT_EQ(m_pContainer->listBundleResources((*m_pContainer->listBundles().begin())->getID()).size(), nValue)
                        << "Remove resourceConfig with empty string!";
        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside RemoveResourceConfig_N" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-07-26
 * @see None
 * @objective Test 'removeResourceConfig' function does not remove resource with invalid input
 * @target void removeResourceConfig(string bundleId)
 * @test_data None
 * @pre_condition Get container instance
 * @procedure Perform removeResourceConfig() API
 * @post_condition None
 * @expected No crash occurs
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, RemoveResourceConfig_USV_N)
{
    try
    {
        m_pContainer->startContainer(CONFIG_FILE);

        nValue = m_pContainer->listBundleResources((*m_pContainer->listBundles().begin())->getID()).size();

        m_pContainer->removeResourceConfig(INVALID_INPUT, INVALID_INPUT);

        EXPECT_EQ(m_pContainer->listBundleResources((*m_pContainer->listBundles().begin())->getID()).size(), nValue)
                                << "Remove resourceConfig with Invalid parameter!";
        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside RemoveResourceConfig_N" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-16
 * @see startContainer
 * @objective Test 'listBundleResources' function provides bundle list in positive way
 * @target list<string> listBundleResources(void)
 * @test_data None
 * @pre_condition 1. Get container instance
 *                2. Start container
 * @procedure Perform listBundleResources() API
 * @post_condition StopContainer
 * @expected Container can remove resource in resourceConfig
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, ListBundleResources_SRC_P)
{
    m_pContainer->startContainer(CONFIG_FILE);

    initialBundleResourceSize = m_pContainer->listBundleResources(HUE_BUNDLE_ID).size();

    EXPECT_GT(initialBundleResourceSize, ZERO_VALUE) << "list bundle resource not found!";

    m_pContainer->stopContainer();
}
#endif

/**
 * @since 2015-07-16
 * @see startContainer
 * @objective Test 'listBundleResources' function does not provide bundle list without start container
 * @target list<string> listBundleResources(void)
 * @test_data None
 * @pre_condition Get container instance
 * @procedure Perform listBundleResources() API zero value;
 * @post_condition StopContainer
 * @expected No crash occurs
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, ListBundleResourcesWithEmptyConfigFile_ESV_N)
{
    m_pContainer->startContainer(CONFIG_EMPTY_FILE);

    EXPECT_EQ(ZERO_VALUE, m_pContainer->listBundleResources(s_bundleID).size())<<"list bundle resource found without start container!";

    m_pContainer->stopContainer();
}
#endif

/**
 * @since 2015-07-16
 * @see startContainer
 * @see start container
 * @see listBundles
 * @objective Test 'getID' function provides bundleID in positive way
 * @target string getID(void)
 * @test_data 1. ResourceContainerConfig
 *            2. Bundle name
 * @pre_condition set valid bundle name using setID API.
 * @procedure Perform getID() API
 * @post_condition StopContainer
 * @expected getID value and bundle name is same.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, GetBundleID_SRC_P)
{
    try
    {
        m_pContainer->startContainer(CONFIG_FILE);
        std::list < std::unique_ptr< RCSBundleInfo > > bundleList;
        bundleList = m_pContainer->listBundles();
        string strGetBundleID = "";

        for (auto &bundleinfo : bundleList)
        {
            if (bundleinfo->getID().compare(HUE_BUNDLE_ID) == 0)
                strGetBundleID = bundleinfo->getID();
        }

        ASSERT_STRCASEEQ(strGetBundleID.c_str(), HUE_BUNDLE_ID.c_str()) << "bundle ID not found!";

        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside GetBundleID_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-16
 * @see startContainer
 * @see listBundles
 * @objective Test 'getID' function provide BMI bundle ID
 * @target string getID(void)
 * @test_data None
 * @pre_condition None
 * @procedure Perform getID() API
 * @post_condition stop container
 * @expected No crash occurs
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, GetBMIBundleID_SRC_P)
{
    try
    {
        m_pContainer->startContainer(CONFIG_FILE);
        std::list < std::unique_ptr< RCSBundleInfo > > bundleList;
        bundleList = m_pContainer->listBundles();
        string strGetBundleID = "";

        for (auto &bundleinfo : bundleList)
        {
            if (bundleinfo->getID().compare(BMI_BUNDLE_ID_CONST) == 0)
                strGetBundleID = bundleinfo->getID();
        }

        ASSERT_STRCASEEQ(strGetBundleID.c_str(), BMI_BUNDLE_ID_CONST.c_str()) << "BMI bundle Id not found.";

        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside GetBundleID_N" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-16
 * @see startContainer
 * @see listBundles
 * @objective Test 'getPath' function provides bundle path in positive way
 * @target string getPath(void)
 * @test_data Bundle path
 * @pre_condition set valid bundle path using setPath API.
 * @procedure Perform getPath() API
 * @post_condition stop container
 * @expected getPath value and bundle path is same.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, GetBundlePath_SRC_P)
{
    try
    {
        m_pContainer->startContainer(CONFIG_FILE);

        std::list < std::unique_ptr< RCSBundleInfo > > bundleList;
        bundleList = m_pContainer->listBundles();
        string strGetBundlePath = "";

        for (auto &bundleinfo : bundleList)
        {
            if (bundleinfo->getPath().compare(HUE_BUNDLE_PATH) == 0)
                strGetBundlePath = bundleinfo->getPath();
        }

        ASSERT_STRCASEEQ(strGetBundlePath.c_str(), HUE_BUNDLE_PATH.c_str()) << "HUE bundle path not found.";

        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside GetBundlePath_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-16
 * @see startContainer
 * @see listBundles
 * @objective Test 'getPath' function provide BMI bundle path
 * @target string getPath(void)
 * @test_data None
 * @pre_condition None
 * @procedure Perform getPath() API
 * @post_condition StopContainer
 * @expected No crash occurs
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, GetBMIBundlePath_SRC_P)
{
    try
    {
        m_pContainer->startContainer(CONFIG_FILE);

        std::list < std::unique_ptr< RCSBundleInfo > > bundleList;
        bundleList = m_pContainer->listBundles();
        string strGetBundlePath = "";

        for (auto &bundleinfo : bundleList)
        {
            if (bundleinfo->getPath().compare(BMI_BUNDLE_PATH_CONST) == 0)
                strGetBundlePath = bundleinfo->getPath();
        }

        ASSERT_STRCASEEQ(strGetBundlePath.c_str(), BMI_BUNDLE_PATH_CONST.c_str()) << "BMI bundle path not found.";

        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside GetBundlePath_N" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-16
 * @see startContainer
 * @see listBundles
 * @objective Test 'getVersion' function provides bundle version in positive way
 * @target string getVersion()
 * @test_data Bundle version
 * @pre_condition set valid bundle version using setVersion API.
 * @procedure Perform getVersion() API
 * @post_condition None
 * @expected getVersion value and bundle version is same.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, GetBundleVersion_SRC_P)
{
    try
    {
        m_pContainer->startContainer(CONFIG_FILE);

        std::list < std::unique_ptr< RCSBundleInfo > > bundleList;
        bundleList = m_pContainer->listBundles();
        string strGetBundleVersion = "";

        for (auto &bundleinfo : bundleList)
        {
            if (bundleinfo->getVersion().compare(BUNDLE_VERSION_CONST) == 0)
            {
                strGetBundleVersion = bundleinfo->getVersion();
                break;
            }
        }

        ASSERT_STRCASEEQ(strGetBundleVersion.c_str(), BUNDLE_VERSION_CONST.c_str()) << "Bundle version mismatched.";

        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside GetBundleVersion_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-07-16
 * @see startContainer
 * @see listBundles
 * @objective Test 'getVersion' function provide BMI bundle versio
 * @target string getVersion()
 * @test_data None
 * @pre_condition None
 * @procedure Perform getVersion() API
 * @post_condition None
 * @expected No crash occurs
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, GetBMIBundleVersion_SRC_P)
{
    try
    {
        m_pContainer->startContainer(CONFIG_FILE);

        std::list < std::unique_ptr< RCSBundleInfo > > bundleList;
        bundleList = m_pContainer->listBundles();
        string strGetBundleVersion = "";

        for (auto &bundleinfo : bundleList)
        {
            if (bundleinfo->getID().compare(BMI_BUNDLE_ID_CONST) == 0)
            {
                strGetBundleVersion = bundleinfo->getVersion();
                break;
            }
        }

        ASSERT_STRCASEEQ(strGetBundleVersion.c_str(), BUNDLE_VERSION_CONST.c_str()) << "Bundle version mismatched.";

        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside GetBundleVersion_N" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-02-17
 * @see Get Instance
 * @objective Test 'StartContainer' function with reference sample value in Positive way
 * @target void StartContainer(string configFile)
 * @test_data ResourceContainerConfig
 * @pre_condition Get container instance
 * @procedure Perform StartContainer() API
 * @post_condition None
 * @expected Container can gather information from ResourceContainerConfig.xml file.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, StartContainer_RSV_P)
{
    try
    {
        m_pContainer->startContainer(g_config_file_address);
        EXPECT_GT(m_pContainer->listBundles().size(), ZERO_VALUE)
                << "Container is Not started !";
        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside StartContainer_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-02-18
 * @see None
 * @objective Test 'ListBundleResources' function with empty string value in negative way
 * @target void removeResourceConfig(string bundleId)
 * @test_data None
 * @pre_condition Get container instance
 * @procedure Perform removeResourceConfig() API
 * @post_condition None
 * @expected No crash occurs
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, ListBundleResources_ESV_N)
{
    try
    {
        m_pContainer->startContainer(CONFIG_EMPTY_FILE);
        m_pContainer->listBundleResources(EMPTY_STRING);
        EXPECT_EQ(ZERO_VALUE, m_pContainer->listBundleResources(s_bundleID).size())
                << "list bundle resource found without start container!";
        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside ListBundleResources_ESV_N" + std::string(e.what()));
    }
}
#endif
