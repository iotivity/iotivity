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
#include "gtest/gtest.h"
#include "RCHelper.h"

#if defined(__LINUX__)
#define HUE_PATH "libHueBundle.so"
#define DI_PATH "libDISensorBundle.so"
#define BMI_PATH "libBMISensorBundle.so"
#define CONFIG_FILE "ResourceContainerConfig.xml"
#define CONFIG_EMPTY_FILE "ResourceContainerEmptyConfig.xml"
#endif

#if defined(__TIZEN__)
#define HUE_PATH "/usr/bin/rc/libHueBundle.so"
#define DI_PATH "/usr/bin/rc/libDISensorBundle.so"
#define BMI_PATH "/usr/bin/rc/libBMISensorBundle.so"
#define CONFIG_FILE "/usr/bin/rc/ResourceContainerConfig.xml"
#define CONFIG_EMPTY_FILE "/usr/bin/rc/ResourceContainerEmptyConfig.xml"
#endif

#define HUE_BUNDLE_ID "oic.bundle.hueSample"
#define HUE_NAME "huesample"

#define DI_BUNDLE_ID "oic.bundle.discomfortIndexSensor"
#define DI_NAME "disensor"

#define BMI_BUNDLE_ID "oic.bundle.BMISensor"
#define BMI_NAME "bmisensor"

#define emptyString ""
#define nullValue "NULL"
#define zeroValue 0

class RCResourceBundleTest_stc: public ::testing::Test
{
protected:
    RCHelper* m_pRCHelper;
    RCSResourceContainer *m_pContainer;
    std::map< string, string > m_params;

protected:
    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();

        m_pRCHelper = RCHelper::getInstance();
        m_pContainer = RCSResourceContainer::getInstance();
    }

    virtual void TearDown()
    {
        CommonUtil::runCommonTCTearDownPart();
    }
};

/**
 * @since 2015-09-22
 * @see Get Instance
 * @see Start Container
 * @objective Test addBundle function added hue bundle in ResourceConfig
 * @target void addBundle(string bundleId, string bundleUri, string bundlePath, std::map<string, string> params)
 * @test_data ResourceContainerConfig
 * @pre_condition 1. Get container instance
                  2. start container
 * @procedure 1. Call addBundle with hue bundle
              2. Check hue bundle is created by addBundle.
 * @post_condition Stop container
 * @expected Container can add hue bundle.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceBundleTest_stc, AddHueBundleFromEmptyContainer_ESV_N)
{
    try
    {
        m_pContainer->startContainer(CONFIG_EMPTY_FILE);
        m_pContainer->addBundle(HUE_BUNDLE_ID, emptyString, HUE_PATH, HUE_NAME, m_params);

        ASSERT_TRUE(m_pRCHelper->isAddedBundle(m_pContainer, HUE_BUNDLE_ID));

        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while add hue bundle : " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-09-22
 * @see Get container instance
 * @see start container
 * @objective Test addBundle function add di bundle with empty container positive basic way
 * @target void addBundle(string bundleId, string bundleUri, string bundlePath, std::map<string, string> params)
 * @test_data ResourceContainerConfig
 * @pre_condition 1. Get container instance
                  2. start container
 * @procedure 1. Call addBundle with di bundle
              2. Check di bundle is created by addBundle.
 * @post_condition Stop container
 * @expected Container can add di bundle.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceBundleTest_stc, AddDIBundleFromEmptyContainer_ESV_N)
{
    try
    {
        m_pContainer->startContainer(CONFIG_EMPTY_FILE);
        m_pContainer->addBundle(DI_BUNDLE_ID, emptyString, DI_PATH, DI_NAME, m_params);

        ASSERT_TRUE(m_pRCHelper->isAddedBundle(m_pContainer, DI_BUNDLE_ID));

        m_pContainer->stopContainer();

    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while add di bundle : " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-09-22
 * @see Get container instance
 * @see start container
 * @objective Test addBundle function add bmi bundle with empty container positive basic way
 * @target void addBundle(string bundleId, string bundleUri, string bundlePath, std::map<string, string> params)
 * @test_data ResourceContainerConfig
 * @pre_condition 	1. Get container instance
					2. start container
 * @procedure 		1. Call addBundle with bmi bundle
					2. Check bmi bundle is created by addBundle.
 * @post_condition Stop container
 * @expected Container can add bmi bundle.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceBundleTest_stc, AddBMIBundleFromEmptyContainer_ESV_N)
{
    try
    {
        m_pContainer->startContainer(CONFIG_EMPTY_FILE);
        m_pContainer->addBundle(BMI_BUNDLE_ID, emptyString, BMI_PATH, BMI_NAME, m_params);

        ASSERT_TRUE(m_pRCHelper->isAddedBundle(m_pContainer, BMI_BUNDLE_ID));

        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while add bmi bundle : " + std::string(e.what()));
    }
}
#endif


/**
 * @since 2015-09-22 
 * @see Get container instance
 * @see start container
 * @see add bundle
 * @objective Test startBundle function start hue bundle with empty container positive basic way
 * @target void startBundle(string bundleId)
 * @test_data ResourceContainerConfig
 * @pre_condition 1. Get container instance
                  2. start container
                  3. add hue bundle
 * @procedure Call startBundle API
 * @post_condition Stop container
 * @expected Container can start hue bundle.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceBundleTest_stc, StartHueBundleFromEmptyContainer_ESV_N)
{
    try
    {
        m_pContainer->startContainer(CONFIG_EMPTY_FILE);
        m_pContainer->addBundle(HUE_BUNDLE_ID, emptyString, HUE_PATH, HUE_NAME, m_params);

        ASSERT_TRUE(m_pRCHelper->isAddedBundle(m_pContainer, HUE_BUNDLE_ID));

        ASSERT_TRUE(m_pRCHelper->startBundle(m_pContainer, HUE_BUNDLE_ID));

        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while add hue bundle : " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-09-22
 * @see Get container instance
 * @see start container
 * @see add bundle
 * @objective Test startBundle function with di bundle positive basic way
 * @target void startBundle(string bundleId)
 * @test_data ResourceContainerConfig
 * @pre_condition 1. Get container instance
                  2. start container
                  3. add di bundle
 * @procedure Call startBundle API
 * @post_condition Stop container
 * @expected Container can start di bundle.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceBundleTest_stc, StartDIBundleFromEmptyContainer_ESV_N)
{
    try
    {
        m_pContainer->startContainer(CONFIG_EMPTY_FILE);
        m_pContainer->addBundle(DI_BUNDLE_ID, emptyString, DI_PATH, DI_NAME, m_params);

        ASSERT_TRUE(m_pRCHelper->isAddedBundle(m_pContainer, DI_BUNDLE_ID));

        ASSERT_TRUE(m_pRCHelper->startBundle(m_pContainer, DI_BUNDLE_ID));

        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while add di bundle : " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-09-22
 * @see Get container instance
 * @see start container
 * @see add bundle
 * @objective Test startBundle function with bmi bundle positive basic way
 * @target void startBundle(string bundleId)
 * @test_data ResourceContainerConfig
 * @pre_condition 1. Get container instance
                  2. start container
                  3. add bmi bundle
 * @procedure Call startBundle API
 * @post_condition Stop container
 * @expected Container can start bmi bundle.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceBundleTest_stc, StartBMIBundleFromEmptyContainer_ESV_N)
{
    try
    {
        m_pContainer->startContainer(CONFIG_EMPTY_FILE);
        m_pContainer->addBundle(BMI_BUNDLE_ID, emptyString, BMI_PATH, BMI_NAME, m_params);

        ASSERT_TRUE(m_pRCHelper->isAddedBundle(m_pContainer, BMI_BUNDLE_ID));

        ASSERT_TRUE(m_pRCHelper->startBundle(m_pContainer, BMI_BUNDLE_ID));

        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while add bmi bundle : " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-09-22
 * @see Get container instance
 * @see start container
 * @objective Test addBundle function with hue bundle positive basic way
 * @target void addBundle(string bundleId, string bundleUri, string bundlePath, std::map<string, string> params)
 * @test_data ResourceContainerConfig
 * @pre_condition 1. Get container instance
                  2. start container
 * @procedure Check hue bundle is created by XML infomation.
 * @post_condition Stop container
 * @expected Container can add hue bundle from XML.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceBundleTest_stc, AddHueBundleFromXML_SRC_P)
{
    try
    {
        m_pContainer->startContainer(CONFIG_FILE);

        ASSERT_TRUE(m_pRCHelper->isAddedBundle(m_pContainer, HUE_BUNDLE_ID));

        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while add hue bundle : " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-09-22
 * @see Get container instance
 * @see start container
 * @objective Test addBundle function with di bundle positive basic way
 * @target void addBundle(string bundleId, string bundleUri, string bundlePath, std::map<string, string> params)
 * @test_data ResourceContainerConfig
 * @pre_condition 1. Get container instance
                  2. start container
 * @procedure Check di bundle is created by XML infomation.
 * @post_condition Stop container
 * @expected Container can add di bundle from XML.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceBundleTest_stc, AddDIBundleFromXML_SRC_P)
{
    try
    {
        m_pContainer->startContainer(CONFIG_FILE);

        ASSERT_TRUE(m_pRCHelper->isAddedBundle(m_pContainer, DI_BUNDLE_ID));

        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while add di bundle : " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-09-22
 * @see Get container instance
 * @see start container
 * @objective Test addBundle function with bmi bundle positive basic way
 * @target void addBundle(string bundleId, string bundleUri, string bundlePath, std::map<string, string> params)
 * @test_data ResourceContainerConfig
 * @pre_condition 1. Get container instance
                  2. start container
 * @procedure Check bmi bundle is created by XML infomation.
 * @post_condition Stop container
 * @expected Container can add bmi bundle from XML.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceBundleTest_stc, AddBMIBundleFromXML_SRC_P)
{
    try
    {
        m_pContainer->startContainer(CONFIG_FILE);

        ASSERT_TRUE(m_pRCHelper->isAddedBundle(m_pContainer, BMI_BUNDLE_ID));

        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while add bmi bundle : " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-09-22
 * @see Get container instance
 * @see start container
 * @see add bundle
 * @objective Test startBundle function with hue bundle positive basic way
 * @target void startBundle(string bundleId)
 * @test_data ResourceContainerConfig
 * @pre_condition 1. Get container instance
                  2. start container
                  3. add hue bundle
 * @procedure Call startBundle API
 * @post_condition Stop container
 * @expected Container can start hue bundle using xml information.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceBundleTest_stc, StartHueBundleFromXML_SRC_P)
{
    try
    {
        m_pContainer->startContainer(CONFIG_FILE);

        ASSERT_TRUE(m_pRCHelper->isAddedBundle(m_pContainer, HUE_BUNDLE_ID));

        ASSERT_TRUE(m_pRCHelper->startBundle(m_pContainer, HUE_BUNDLE_ID));

        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while add hue bundle : " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-09-22
 * @see Get container instance
 * @see start container
 * @see add bundle
 * @objective Test startBundle function start di bundle from xml positive basic way
 * @target void startBundle(string bundleId)
 * @test_data ResourceContainerConfig
 * @pre_condition 1. Get container instance
                  2. start container
                  3. add di bundle
 * @procedure Call startBundle API
 * @post_condition Stop container
 * @expected Container can start di bundle using xml information.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceBundleTest_stc, StartDIBundleFromXML_SRC_P)
{
    try
    {
        m_pContainer->startContainer(CONFIG_FILE);

        ASSERT_TRUE(m_pRCHelper->isAddedBundle(m_pContainer, DI_BUNDLE_ID));

        ASSERT_TRUE(m_pRCHelper->startBundle(m_pContainer, DI_BUNDLE_ID));

        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while add di bundle : " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-09-22
 * @see Get container instance
 * @see start container
 * @see add bundle
 * @objective Test startBundle function start bmi bundle from xml positive basic way
 * @target void startBundle(string bundleId)
 * @test_data ResourceContainerConfig
 * @pre_condition 1. Get container instance
                  2. start container
                  3. add bmi bundle
 * @procedure Call startBundle API
 * @post_condition Stop container
 * @expected Container can start bmi bundle using xml information.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceBundleTest_stc, StartBMIBundleFromXML_SRC_P)
{
    try
    {
        m_pContainer->startContainer(CONFIG_FILE);

        ASSERT_TRUE(m_pRCHelper->isAddedBundle(m_pContainer, BMI_BUNDLE_ID));

        ASSERT_TRUE(m_pRCHelper->startBundle(m_pContainer, BMI_BUNDLE_ID));

        m_pContainer->stopContainer();
        SUCCEED();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while add bmi bundle : " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-09-22
 * @see Get container instance
 * @see start container
 * @see add bundle
 * @see start bundle
 * @objective Test stopBundle function stop hue bundle positive basic way
 * @target void stopBundle(string bundleId)
 * @test_data ResourceContainerConfig
 * @pre_condition 1. Get container instance
                  2. start container
                  3. start hue bundle
 * @procedure Call stopBundle API
 * @post_condition stop container
 * @expected Container can stop hue bundle.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceBundleTest_stc, StopHueBundle_SRC_P)
{
    try
    {
        m_pContainer->startContainer(CONFIG_FILE);

        ASSERT_TRUE(m_pRCHelper->isAddedBundle(m_pContainer, HUE_BUNDLE_ID));

        ASSERT_TRUE(m_pRCHelper->startBundle(m_pContainer, HUE_BUNDLE_ID));

        ASSERT_TRUE(m_pRCHelper->stopBundle(m_pContainer, HUE_BUNDLE_ID));

        m_pContainer->stopContainer();
        SUCCEED();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while add hue bundle : " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-09-22
 * @see Get container instance
 * @see start container
 * @see add bundle
 * @see start bundle
 * @objective Test stopBundle function stop di bundle positive basic way
 * @target void stopBundle(string bundleId)
 * @test_data ResourceContainerConfig
 * @pre_condition 1. Get container instance
                  2. start container
                  3. add di bundle
                  4. start di bundle
 * @procedure Call stopBundle API
 * @post_condition stop container
 * @expected Container can stom di bundle.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceBundleTest_stc, StopDIBundle_SRC_P)
{
    try
    {
        m_pContainer->startContainer(CONFIG_FILE);

        ASSERT_TRUE(m_pRCHelper->isAddedBundle(m_pContainer, DI_BUNDLE_ID));

        ASSERT_TRUE(m_pRCHelper->startBundle(m_pContainer, DI_BUNDLE_ID));

        ASSERT_TRUE(m_pRCHelper->stopBundle(m_pContainer, DI_BUNDLE_ID));

        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while add di bundle : " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-09-22
 * @see Get container instance
 * @see start container
 * @see add bundle
 * @see start bundle
 * @objective Test stopBundle function with stop bmi bundle positive basic way
 * @target void stopBundle(string bundleId)
 * @test_data ResourceContainerConfig
 * @pre_condition 1. Get container instance
                  2. start container
                  3. add bmi bundle
                  4. start bmi bundle
 * @procedure Call stopBundle API
 * @post_condition stop container
 * @expected Container can stop bmi bundle.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceBundleTest_stc, StopBMIBundle_SRC_P)
{
    try
    {
        m_pContainer->startContainer(CONFIG_FILE);

        ASSERT_TRUE(m_pRCHelper->isAddedBundle(m_pContainer, BMI_BUNDLE_ID));

        ASSERT_TRUE(m_pRCHelper->startBundle(m_pContainer, BMI_BUNDLE_ID));

        ASSERT_TRUE(m_pRCHelper->stopBundle(m_pContainer, BMI_BUNDLE_ID));

        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred while add bmi bundle : " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-09-22
 * @see Get container instance
 * @see start container
 * @see add bundle
 * @objective Test removeBundle function with remove hue bundle positive way
 * @target void removeBundle(string bundleId)
 * @test_data None
 * @pre_condition 1. Get container instance
                  2. Add bundle
 * @procedure Call removeBundle() API
 * @post_condition stop container
 * @expected Container can remove hue bundle
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceBundleTest_stc, RemoveHueBundle_SRC_P)
{
    try
    {
        m_pContainer->startContainer(CONFIG_FILE);

        ASSERT_TRUE(m_pRCHelper->isAddedBundle(m_pContainer, HUE_BUNDLE_ID));

        ASSERT_TRUE(m_pRCHelper->removeBundle(m_pContainer, HUE_BUNDLE_ID));

        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside RemoveBundle_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-09-22
 * @see Get container instance
 * @see start container
 * @see add bundle
 * @objective Test removeBundle function with remove di bundle positive way
 * @target void removeBundle(string bundleId)
 * @test_data None
 * @pre_condition 1. Get container instance
                  2. Add bundle
 * @procedure Call removeBundle() API
 * @post_condition stop container
 * @expected Container can remove di bundle
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceBundleTest_stc, RemoveDIBundle_SRC_P)
{
    try
    {
        m_pContainer->startContainer(CONFIG_FILE);

        ASSERT_TRUE(m_pRCHelper->isAddedBundle(m_pContainer, DI_BUNDLE_ID));

        ASSERT_TRUE(m_pRCHelper->removeBundle(m_pContainer, DI_BUNDLE_ID));

        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside RemoveBundle_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-09-22
 * @see Get container instance
 * @see start container
 * @see add bundle
 * @objective Test removeBundle function with bmi bundle positive way
 * @target void removeBundle(string bundleId)
 * @test_data None
 * @pre_condition 1. Get container instance
                  2. Add bundle
 * @procedure Call removeBundle() API
 * @post_condition stop container
 * @expected Container can remove bmi bundle
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceBundleTest_stc, RemoveBMIBundle_SRC_P)
{
    try
    {
        m_pContainer->startContainer(CONFIG_FILE);

        ASSERT_TRUE(m_pRCHelper->isAddedBundle(m_pContainer, BMI_BUNDLE_ID));

        ASSERT_TRUE(m_pRCHelper->removeBundle(m_pContainer, BMI_BUNDLE_ID));

        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside RemoveBundle_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-09-22
 * @see Get container instance
 * @see start container
 * @see add bundle
 * @see start bundle
 * @objective Test removeBundle function with remove hue bundle
 * @target void removeBundle(string bundleId)
 * @test_data None
 * @pre_condition 1. Get container instance
                  2. Add bundle
                  3. start hue bundle
 * @procedure Call removeBundle() API
 * @post_condition stop container
 * @expected Container can remove hue bundle after stopped bundle.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceBundleTest_stc, RemoveHueBundleWhileRunning_STLSEC_N)
{
    try
    {
        m_pContainer->startContainer(CONFIG_FILE);

        ASSERT_TRUE(m_pRCHelper->isAddedBundle(m_pContainer, HUE_BUNDLE_ID));

        ASSERT_TRUE(m_pRCHelper->startBundle(m_pContainer, HUE_BUNDLE_ID));

        ASSERT_TRUE(m_pRCHelper->removeBundle(m_pContainer, HUE_BUNDLE_ID));

        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside RemoveBundle_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-09-22
 * @see Get container instance
 * @see start container
 * @see add bundle
 * @see start bundle
 * @objective Test removeBundle function with remove di bundle
 * @target void removeBundle(string bundleId)
 * @test_data None
 * @pre_condition 1. Get container instance
                  2. Add bundle
                  3. start di bundle
 * @procedure Call removeBundle() API
 * @post_condition stop container
 * @expected Container can remove di bundle after stopped bundle.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceBundleTest_stc, RemoveDIBundleWhileRunning_STLSEC_N)
{
    try
    {
        m_pContainer->startContainer(CONFIG_FILE);

        ASSERT_TRUE(m_pRCHelper->isAddedBundle(m_pContainer, DI_BUNDLE_ID));

        ASSERT_TRUE(m_pRCHelper->startBundle(m_pContainer, DI_BUNDLE_ID));

        ASSERT_TRUE(m_pRCHelper->removeBundle(m_pContainer, DI_BUNDLE_ID));

        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside RemoveBundle_P" + std::string(e.what()));
    }
}
#endif

/**
 * @since 2015-09-22
 * @see Get container instance
 * @see start container
 * @see add bundle
 * @see start bundle
 * @objective Test removeBundle function with remove bmi bundle
 * @target void removeBundle(string bundleId)
 * @test_data None
 * @pre_condition 1. Get container instance
                  2. Add bundle
                  3. start bmi bundle
 * @procedure Call removeBundle() API
 * @post_condition stop container
 * @expected Container can remove bmi bundle after stopped bundle.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RCResourceBundleTest_stc, RemoveBMIBundleWhileRunning_STLSEC_N)
{
    try
    {
        m_pContainer->startContainer(CONFIG_FILE);

        ASSERT_TRUE(m_pRCHelper->isAddedBundle(m_pContainer, BMI_BUNDLE_ID));

        ASSERT_TRUE(m_pRCHelper->startBundle(m_pContainer, BMI_BUNDLE_ID));

        ASSERT_TRUE(m_pRCHelper->removeBundle(m_pContainer, BMI_BUNDLE_ID));

        m_pContainer->stopContainer();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside RemoveBundle_P" + std::string(e.what()));
    }
}
#endif
