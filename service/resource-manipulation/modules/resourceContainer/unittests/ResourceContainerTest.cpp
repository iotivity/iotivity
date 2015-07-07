//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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
#include <map>
#include <vector>

#include <gtest/gtest.h>
#include <HippoMocks/hippomocks.h>

#include "Configuration.h"
#include "BundleResource.h"
#include "ResourceContainer.h"
#include "ResourceContainerBundleAPI.h"
#include "ResourceContainerImpl.h"

#include "ResourceObject.h"

using namespace std;
using namespace testing;
using namespace OIC::Service;


string CONFIG_FILE = "ResourceContainerTestConfig.xml";

typedef ResourceObject::Ptr(*resourceObjectBuild)();

class TestBundleResource : public BundleResource
{
    public:
        string getAttribute(string attributeName)
        {
            return "test";
        };
        void setAttribute(string attributeName, string value)
        {};
        void initAttributes()
        {};
};

class ResourceContainerTest : public Test
{
    public:
        ResourceContainer *m_pResourceContainer;

    protected:
        void SetUp() override
        {
            m_pResourceContainer = ResourceContainer::getInstance();
        }
};


class ResourceContainerBundleAPITest : public Test
{
    public:
        MockRepository mocks;
        ResourceContainerBundleAPI *m_pResourceContainer;

    protected:
        void SetUp() override
        {
            m_pResourceContainer = ResourceContainerBundleAPI::getInstance();
        }
};

// TC for registering resource should be implemented
//TEST_F(ResourceContainerBundleAPITest, registerResourceTest)
//{
//    ResourceObject::Builder *builder = new ResourceObject::Builder("/test_resource", "oic.test", "");
//    TestBundleResource *resource = new TestBundleResource();
//
//    mocks.ExpectCall(builder, ResourceObject::Builder::build).Return(nullptr);
//
//    m_pResourceContainer->registerResource(resource);
//}

// TC for unregistering resource should be implemented
//TEST_F(ResourceContainerBundleAPITest, unregisterResourceTest)
//{
//}


class ResourceContainerImplTest : public Test
{
    public:
        ResourceContainerImpl *m_pResourceContainer;
        BundleInfo *m_pBundleInfo;

    protected:
        void SetUp() override
        {
            m_pResourceContainer = ResourceContainerImpl::getImplInstance();
            m_pBundleInfo = BundleInfo::createBundleInfo();
        }
};

TEST_F(ResourceContainerImplTest, RegisterBundleTest)
{
    m_pBundleInfo->setPath("libTestBundle.so");
    m_pBundleInfo->setVersion("1.0");
    m_pBundleInfo->setID("oic.bundle.test");

    m_pResourceContainer->registerBundle(m_pBundleInfo);

    EXPECT_NE(((BundleInfoInternal *)m_pBundleInfo)->getBundleHandle(), nullptr);
}

TEST_F(ResourceContainerImplTest, RegisterBundleTestWithInvalidPath)
{
    m_pBundleInfo->setPath("");
    m_pBundleInfo->setVersion("1.0");
    m_pBundleInfo->setID("oic.bundle.test");

    m_pResourceContainer->registerBundle(m_pBundleInfo);

    EXPECT_EQ(((BundleInfoInternal *)m_pBundleInfo)->getBundleActivator(), nullptr);
}

TEST_F(ResourceContainerImplTest, ActivateBundle)
{
    m_pBundleInfo->setPath("libTestBundle.so");
    m_pBundleInfo->setVersion("1.0");
    m_pBundleInfo->setID("oic.bundle.test");

    m_pResourceContainer->registerBundle(m_pBundleInfo);
    m_pResourceContainer->activateBundle(m_pBundleInfo);

    EXPECT_NE(((BundleInfoInternal *)m_pBundleInfo)->getBundleActivator(), nullptr);
}

TEST_F(ResourceContainerImplTest, ActivateUnregisteredBundle)
{
    m_pBundleInfo->setPath("libTestBundle.so");
    m_pBundleInfo->setVersion("1.0");
    m_pBundleInfo->setID("oic.bundle.test");

    m_pResourceContainer->activateBundle(m_pBundleInfo);

    EXPECT_EQ(((BundleInfoInternal *)m_pBundleInfo)->getBundleActivator(), nullptr);
}

// TC for deactivating bundle should be implemented

//TEST_F(ResourceContainerImplTest, DeactivateBundle)
//{
//
//}


/* Test for Configuration */
TEST(ConfigurationTest, LoadConfigFileWithValidPath)
{
    Configuration *config = new Configuration(CONFIG_FILE);

    EXPECT_EQ(config->isLoaded(), true);
}

TEST(ConfigurationTest, LoadConfigFileWithInvalidPath)
{
    Configuration *config = new Configuration("test");

    EXPECT_EQ(config->isLoaded(), false);
}

TEST(ConfigurationTest, getConfiguredBundlesTest)
{
    Configuration *config = new Configuration(CONFIG_FILE);

    configInfo bundles;
    map<string, string> results;

    config->getConfiguredBundles(&bundles);

    results = *bundles.begin();

    EXPECT_STREQ(results["id"].c_str(), "oic.bundle.test");
    EXPECT_STREQ(results["path"].c_str(), "libTestBundle.so");
    EXPECT_STREQ(results["version"].c_str(), "1.0.0");
}

TEST(ConfigurationTest, getBundleConfigurationTest)
{
    Configuration *config = new Configuration(CONFIG_FILE);

    configInfo bundle;
    map<string, string> results;

    config->getBundleConfiguration("oic.bundle.test", &bundle);

    results = *bundle.begin();

    EXPECT_STREQ(results["id"].c_str(), "oic.bundle.test");
    EXPECT_STREQ(results["path"].c_str(), "libTestBundle.so");
    EXPECT_STREQ(results["version"].c_str(), "1.0.0");
}

TEST(ConfigurationTest, getBundleConfigurationTestWithInvalidBundleId)
{
    Configuration *config = new Configuration(CONFIG_FILE);

    configInfo bundles;
    config->getBundleConfiguration("test", &bundles);

    EXPECT_EQ(bundles.empty(), true);
}

TEST(ConfigurationTest, getResourceConfigurationTest)
{
    Configuration *config = new Configuration(CONFIG_FILE);

    vector<resourceInfo> resourceConfig;
    resourceInfo result;

    config->getResourceConfiguration("oic.bundle.test", &resourceConfig);

    result = *resourceConfig.begin();

    EXPECT_STREQ(result.name.c_str(), "test_resource");
    EXPECT_STREQ(result.resourceType.c_str(), "oic.test");
}

TEST(ConfigurationTest, getResourceConfigurationTestWithInvalideBundleId)
{
    Configuration *config = new Configuration(CONFIG_FILE);

    configInfo bundles;
    vector<resourceInfo> resourceConfig;
    config->getResourceConfiguration("test", &resourceConfig);

    EXPECT_EQ(bundles.empty(), true);
}