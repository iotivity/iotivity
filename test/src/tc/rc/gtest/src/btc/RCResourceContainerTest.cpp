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

#define emptyString ""
#define nullValue "NULL"
#define zeroValue 0

string btcBundleID;
const string &config_file_address = CONFIG_FILE;

std::map<string, string> btcParams;
std::list<std::unique_ptr<RCSBundleInfo>> bundles;
std::unique_ptr<RCSBundleInfo> btcBundleInfo;

class RCResourceContainerTest_btc: public ::testing::Test {
protected:
	std::string m_errorMsg;
	RCSResourceContainer *m_pContainer;

private:

protected:

	virtual void SetUp() {
		CommonUtil::runCommonTCSetUpPart();

		m_errorMsg = emptyString;
		m_pContainer = RCSResourceContainer::getInstance();
	}

	virtual void TearDown() {

		CommonUtil::runCommonTCTearDownPart();
	}

public:
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
	ASSERT_NE(nullValue, RCSResourceContainer::getInstance());
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
		m_pContainer->startContainer(emptyString);
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
 2. Perform StartContainer() API
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
		m_pContainer->stopContainer();
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
 2. Perform StartContainer() API
 * @procedure Perform listBundles() API
 * @post_condition Stop container
 * @expected Container provides list of bundles.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, ListBundles_SRC_P)
{
	m_pContainer->startContainer(CONFIG_FILE);

	bundles = m_pContainer->listBundles();

	ASSERT_NE(zeroValue, bundles.size());

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
 * 				  2. start container
 * @procedure Perform listBundles() API
 * @post_condition Stop container
 * @expected empty list of bundles.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, ListBundles_ESV_N)
{
	m_pContainer->startContainer(CONFIG_EMPTY_FILE);

	bundles = m_pContainer->listBundles();

	ASSERT_EQ(zeroValue, bundles.size());

	m_pContainer->stopContainer();
}
#endif

/**
 * @since 2015-07-16
 * @see startContainer
 * @objective Test 'startBundle' function start bundle in positive way
 * @target void startBundle(string bundleId)
 * @test_data ResourceContainerConfig
 * @pre_condition 1. Get container instance
 2. Perform StartContainer() API
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

		btcBundleInfo = std::move(*m_pContainer->listBundles().begin());

		m_pContainer->startBundle(btcBundleInfo->getID());

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
 * @target void startBundle(string bundleId)
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
		m_pContainer->startBundle(emptyString);
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
 * @target void stopBundle(string bundleId)
 * @test_data ResourceContainerConfig
 * @pre_condition 1. Get container instance
 2. Perform StartContainer() API
 3. Get bundle list
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

		btcBundleInfo = std::move(*m_pContainer->listBundles().begin());

		m_pContainer->startBundle(btcBundleInfo->getID());
		m_pContainer->stopBundle(btcBundleInfo->getID());

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
 * @see None
 * @objective Test 'stopBundle' function does not stop bundle without start bundle
 * @target void stopBundle(string bundleId)
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
		m_pContainer->stopBundle(emptyString);
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
 * @target void addBundle(string bundleId, string bundleUri, string bundlePath, std::map<string, string> btcParams)
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
		btcParams.insert(std::make_pair(BUNDLE_INSERT_VERSION, BUNDLE_VERSION));

		m_pContainer->addBundle(BUNDLE_NAME, BUNDLE_URL, BUNDLE_PATH, BUNDLE_ACTIVATOR, btcParams);
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
 * @target void addBundle(string bundleId, string bundleUri, string bundlePath, std::map<string, string> params)
 * @test_data None
 * @pre_condition Get container instance
 * @procedure Perform addBundle() API
 * @post_condition None
 * @expected No crash occurs
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, AddBundle_SRC_N)
{
	try
	{
		m_pContainer->addBundle( emptyString, emptyString, emptyString, emptyString, btcParams);
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
 * @target void removeBundle(string bundleId)
 * @test_data None
 * @pre_condition 1. Get container instance
 2. Add bundle
 * @procedure Perform removeBundle() API
 * @post_condition None
 * @expected Container can remove bundle
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, RemoveBundle_SRC_P)
{
	try
	{
		btcParams.insert(std::make_pair(BUNDLE_INSERT_VERSION, BUNDLE_VERSION));

		m_pContainer->addBundle(BUNDLE_NAME, BUNDLE_URL, BUNDLE_PATH, BUNDLE_ACTIVATOR, btcParams);

		m_pContainer->removeBundle(BUNDLE_NAME);
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
 * @target void removeBundle(string bundleId)
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
		m_pContainer->removeBundle(emptyString);
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
 * @target void addResourceConfig(string bundleId, string resourceUri, std::map<string, string> params)
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
		btcParams.insert(std::make_pair(BUNDLE_INSERT_VERSION, BUNDLE_VERSION));

		m_pContainer->addResourceConfig(BUNDLE_NAME, BUNDLE_URL, btcParams);
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
 * @target void addResourceConfig(string bundleId, string resourceUri, std::map<string, string> params)
 * @test_data None
 * @pre_condition 1. Get container instance
 * @procedure Perform addResourceConfig() API
 * @post_condition None
 * @expected No crash occurs
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, AddResourceConfig_ESV_N)
{
	try
	{
		m_pContainer->addResourceConfig(emptyString, emptyString, btcParams);
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
 2. Add resource
 * @procedure Perform removeResourceConfig() API
 * @post_condition None
 * @expected Container can remove resource in resourceConfig
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, RemoveResourceConfig_SRC_P)
{
	try
	{
		btcParams.insert(std::make_pair(BUNDLE_INSERT_VERSION, BUNDLE_VERSION));

		m_pContainer->addResourceConfig(BUNDLE_NAME, BUNDLE_URL, btcParams);

		m_pContainer->removeResourceConfig(BUNDLE_NAME, BUNDLE_URL);
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
 * @pre_condition 1. Get container instance
 * @procedure Perform removeResourceConfig() API
 * @post_condition None
 * @expected No crash occurs
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, RemoveResourceConfig_ESV_N)
{
	try
	{
		m_pContainer->removeResourceConfig(emptyString, emptyString);
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
 2. Start container
 * @procedure Perform listBundleResources() API
 * @post_condition StopContainer
 * @expected Container can remove resource in resourceConfig
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, ListBundleResources_SRC_P)
{
	m_pContainer->startContainer(CONFIG_FILE);

	btcBundleInfo = std::move(*m_pContainer->listBundles().begin());
	btcBundleID = btcBundleInfo->getID();

	ASSERT_EQ(zeroValue, m_pContainer->listBundleResources(btcBundleID).size());

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
 * @procedure Perform listBundleResources() APIASSERT_NE(zeroValue, container->listBundleResources(bundleID).size());
 * @post_condition StopContainer
 * @expected No crash occurs
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, ListBundleResources_ESV_N01)
{
	m_pContainer->startContainer(CONFIG_EMPTY_FILE);

	ASSERT_EQ(zeroValue, m_pContainer->listBundleResources(btcBundleID).size());

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
 * 			  2. Bundle name
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

		btcBundleInfo = std::move(*m_pContainer->listBundles().begin());

		ASSERT_EQ(zeroValue, btcBundleInfo->getID().size());

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
 * @objective Test 'getID' function does not provide bundle ID with start startContainer with empty config file
 * @target string getID(void)
 * @test_data None
 * @pre_condition None
 * @procedure Perform getID() API
 * @post_condition stop container
 * @expected No crash occurs
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, GetBundleID_N)
{
	try
	{
		m_pContainer->startContainer(CONFIG_FILE);

		btcBundleInfo = std::move(*m_pContainer->listBundles().begin());

		btcBundleInfo->getID();

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
		btcBundleInfo = std::move(*m_pContainer->listBundles().begin());

		ASSERT_EQ(zeroValue, btcBundleInfo->getPath().size());

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
 * @objective Test 'getPath' function does not provide bundle path without start startContainer with empty config file 
 * @target string getPath(void)
 * @test_data None
 * @pre_condition None
 * @procedure Perform getPath() API
 * @post_condition StopContainer
 * @expected No crash occurs
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, GetBundlePath_N)
{
	try
	{
		m_pContainer->startContainer(CONFIG_FILE);

		btcBundleInfo = std::move(*m_pContainer->listBundles().begin());
		btcBundleInfo->getPath();

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
		btcBundleInfo = std::move(*m_pContainer->listBundles().begin());

		ASSERT_EQ(zeroValue, btcBundleInfo->getVersion().size());

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
 * @objective Test 'getVersion' function  does not provide bundle versio without start startContainer in a data config file
 * @target string getVersion()
 * @test_data None
 * @pre_condition None
 * @procedure Perform getVersion() API
 * @post_condition None
 * @expected No crash occurs
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceContainerTest_btc, GetBundleVersion_N)
{
	try
	{
		m_pContainer->startContainer(CONFIG_FILE);

		btcBundleInfo = std::move(*m_pContainer->listBundles().begin());

		btcBundleInfo->getVersion();

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
		m_pContainer->startContainer(config_file_address);
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
TEST_F(RCResourceContainerTest_btc, ListBundleResources_ESV_N02)
{
	try
	{
		m_pContainer->listBundleResources(emptyString);
	}
	catch (exception& e)
	{
		SET_FAILURE("Exception occurred inside ListBundleResources_ESV_N" + std::string(e.what()));
	}
}
#endif
