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

/*Fake bundle resource class for testing*/
class TestBundleResource: public BundleResource
{
public:
    string getAttribute(string attributeName)
    {
        return "test";
    }
    ;
    void setAttribute(string attributeName, string value)
    {
    }
    ;
    void initAttributes()
    {
        BundleResource::setAttribute("attri", "test");
    }
    ;
};

class ResourceContainerTest: public Test
{
public:
    ResourceContainer *m_pResourceContainer;

protected:
    void SetUp() override
    {
        m_pResourceContainer = ResourceContainer::getInstance();
    }
};

TEST_F(ResourceContainerTest, BundleRegisteredWhenContainerStartedWithValidConfigFile)
{
    m_pResourceContainer->startContainer(CONFIG_FILE);

    EXPECT_GT(m_pResourceContainer->listBundles().size(), 0);
    EXPECT_STREQ("oic.bundle.test",
            (*m_pResourceContainer->listBundles().begin())->getID().c_str());
    EXPECT_STREQ("libTestBundle.so",
            (*m_pResourceContainer->listBundles().begin())->getPath().c_str());
    EXPECT_STREQ("1.0.0", (*m_pResourceContainer->listBundles().begin())->getVersion().c_str());

    m_pResourceContainer->stopContainer();
}

TEST_F(ResourceContainerTest, BundleLoadedWhenContainerStartedWithValidConfigFile)
{
    m_pResourceContainer->startContainer(CONFIG_FILE);

    EXPECT_GT(m_pResourceContainer->listBundles().size(), 0);
    EXPECT_TRUE(((BundleInfoInternal *)(*m_pResourceContainer->listBundles().begin()))->isLoaded());
    EXPECT_NE(nullptr,
            ((BundleInfoInternal *)( *m_pResourceContainer->listBundles().begin()))->getBundleHandle());

    m_pResourceContainer->stopContainer();
}

TEST_F(ResourceContainerTest, BundleActivatedWhenContainerStartedWithValidConfigFile)
{
    m_pResourceContainer->startContainer(CONFIG_FILE);

    EXPECT_GT(m_pResourceContainer->listBundles().size(), 0);
    EXPECT_TRUE(
            ((BundleInfoInternal *)(*m_pResourceContainer->listBundles().begin()))->isActivated());
    EXPECT_NE(nullptr,
            ((BundleInfoInternal *)( *m_pResourceContainer->listBundles().begin()))->getBundleActivator());

    m_pResourceContainer->stopContainer();
}

TEST_F(ResourceContainerTest, BundleNotRegisteredWhenContainerStartedWithInvalidConfigFile)
{
    m_pResourceContainer->startContainer("invalideConfig");

    EXPECT_EQ(0, m_pResourceContainer->listBundles().size());
}

TEST_F(ResourceContainerTest, BundleUnregisteredWhenContainerStopped)
{
    m_pResourceContainer->startContainer(CONFIG_FILE);
    m_pResourceContainer->stopContainer();

    EXPECT_EQ(0, m_pResourceContainer->listBundles().size());
}

TEST_F(ResourceContainerTest, BundleStoppedWithStartBundleAPI)
{
    m_pResourceContainer->startContainer(CONFIG_FILE);
    m_pResourceContainer->stopBundle("oic.bundle.test");

    EXPECT_FALSE(
            ((BundleInfoInternal *)(*m_pResourceContainer->listBundles().begin()))->isActivated());
}

TEST_F(ResourceContainerTest, BundleStartedWithStartBundleAPI)
{
    m_pResourceContainer->startContainer(CONFIG_FILE);
    m_pResourceContainer->stopBundle("oic.bundle.test");
    m_pResourceContainer->startBundle("oic.bundle.test");

    EXPECT_TRUE(
            ((BundleInfoInternal *)(*m_pResourceContainer->listBundles().begin()))->isActivated());
}

class ResourceContainerBundleAPITest: public Test
{
public:
    MockRepository mocks;
    ResourceObject *m_pResourceObject;
    ResourceContainerBundleAPI *m_pResourceContainer;
    TestBundleResource *m_pBundleResource;

protected:
    void SetUp() override
    {
        m_pResourceObject = mocks.Mock<ResourceObject>();
        m_pResourceContainer = ResourceContainerBundleAPI::getInstance();

        m_pBundleResource = new TestBundleResource();
        m_pBundleResource->m_bundleId = "oic.bundle.test";
        m_pBundleResource->m_uri = "/test_resource";
        m_pBundleResource->m_resourceType = "oic.test";
    }
};

TEST_F(ResourceContainerBundleAPITest, ResourceServerCreatedWhenRegisterResourceCalled)
{
    mocks.ExpectCallFunc(ResourceContainerImpl::buildResourceObject).With(m_pBundleResource->m_uri,
            m_pBundleResource->m_resourceType).Return(nullptr);

    m_pResourceContainer->registerResource(m_pBundleResource);
}

TEST_F(ResourceContainerBundleAPITest, RequestHandlerForResourceServerSetWhenRegisterResourceCalled)
{
    mocks.OnCallFunc(ResourceContainerImpl::buildResourceObject).Return(
            ResourceObject::Ptr(m_pResourceObject, [](ResourceObject *)
            {}));

    mocks.ExpectCall(m_pResourceObject, ResourceObject::setGetRequestHandler);
    mocks.ExpectCall(m_pResourceObject, ResourceObject::setSetRequestHandler);

    m_pResourceContainer->registerResource(m_pBundleResource);
}

TEST_F(ResourceContainerBundleAPITest, BundleResourceUnregisteredWhenUnregisterResourceCalled)
{
    mocks.OnCallFunc(ResourceContainerImpl::buildResourceObject).Return(
            ResourceObject::Ptr(m_pResourceObject, [](ResourceObject *)
            {}));

    mocks.ExpectCall(m_pResourceObject, ResourceObject::setGetRequestHandler);
    mocks.ExpectCall(m_pResourceObject, ResourceObject::setSetRequestHandler);

    m_pResourceContainer->registerResource(m_pBundleResource);
    m_pResourceContainer->unregisterResource(m_pBundleResource);

    EXPECT_EQ(0,
            ((ResourceContainerImpl *)m_pResourceContainer)->listBundleResources( m_pBundleResource->m_bundleId).size());
}

TEST_F(ResourceContainerBundleAPITest,
        ServerNotifiesToObserversWhenNotificationReceivedFromResource)
{
    mocks.OnCallFunc(ResourceContainerImpl::buildResourceObject).Return(
            ResourceObject::Ptr(m_pResourceObject, [](ResourceObject *)
            {}));

    mocks.ExpectCall(m_pResourceObject, ResourceObject::setGetRequestHandler);
    mocks.ExpectCall(m_pResourceObject, ResourceObject::setSetRequestHandler);

    m_pResourceContainer->registerResource(m_pBundleResource);

    mocks.ExpectCall(m_pResourceObject, ResourceObject::notify);

    m_pResourceContainer->onNotificationReceived(m_pBundleResource->m_uri);
}

TEST_F(ResourceContainerBundleAPITest, BundleConfigurationParsedWithValidBundleId)
{
    configInfo bundle;
    map< string, string > results;

    m_pResourceContainer->getBundleConfiguration("oic.bundle.test", &bundle);

    results = *bundle.begin();

    EXPECT_STREQ("oic.bundle.test", results["id"].c_str());
    EXPECT_STREQ("libTestBundle.so", results["path"].c_str());
    EXPECT_STREQ("1.0.0", results["version"].c_str());
}

TEST_F(ResourceContainerBundleAPITest, BundleResourceConfigurationListParsed)
{
    vector< resourceInfo > resourceConfig;
    resourceInfo result;

    m_pResourceContainer->getResourceConfiguration("oic.bundle.test", &resourceConfig);

    result = *resourceConfig.begin();

    EXPECT_STREQ("test_resource", result.name.c_str());
    EXPECT_STREQ("oic.test", result.resourceType.c_str());
}

class ResourceContainerImplTest: public Test
{
public:
    MockRepository mocks;
    ResourceContainerImpl *m_pResourceContainer;
    BundleInfo *m_pBundleInfo;

protected:
    void SetUp() override
    {
        m_pResourceContainer = ResourceContainerImpl::getImplInstance();
        m_pBundleInfo = BundleInfo::build();
    }
};

TEST_F(ResourceContainerImplTest, SoBundleLoadedWhenRegisteredWithRegisterBundleAPI)
{
    m_pBundleInfo->setPath("libTestBundle.so");
    m_pBundleInfo->setVersion("1.0");
    m_pBundleInfo->setLibraryPath(".");
    m_pBundleInfo->setID("oic.bundle.test");

    m_pResourceContainer->registerBundle(m_pBundleInfo);

    EXPECT_NE(nullptr, ((BundleInfoInternal *)m_pBundleInfo)->getBundleHandle());
}

TEST_F(ResourceContainerImplTest, JavaBundleLoadedWhenRegisteredWithRegisterBundleAPIWrongPath)
{
    m_pBundleInfo->setPath("wrong_path.jar");
    m_pBundleInfo->setActivatorName("org/iotivity/bundle/hue/HueBundleActivator");
    m_pBundleInfo->setLibraryPath("../.");
    m_pBundleInfo->setVersion("1.0");
    m_pBundleInfo->setID("oic.bundle.java.test");

    m_pResourceContainer->registerBundle(m_pBundleInfo);
    EXPECT_FALSE(((BundleInfoInternal *)m_pBundleInfo)->isLoaded());
}

TEST_F(ResourceContainerImplTest, JavaBundleTest)
{
    m_pBundleInfo->setPath("TestBundleJava/hue-0.1-jar-with-dependencies.jar");
    m_pBundleInfo->setActivatorName("org/iotivity/bundle/hue/HueBundleActivator");
    m_pBundleInfo->setLibraryPath("../.");
    m_pBundleInfo->setVersion("1.0");
    m_pBundleInfo->setID("oic.bundle.java.test");

    m_pResourceContainer->registerBundle(m_pBundleInfo);
    EXPECT_TRUE(((BundleInfoInternal *)m_pBundleInfo)->isLoaded());

    m_pResourceContainer->activateBundle(m_pBundleInfo);
    EXPECT_TRUE(((BundleInfoInternal *) m_pBundleInfo)->isActivated());

    m_pResourceContainer->deactivateBundle(m_pBundleInfo);
    EXPECT_FALSE(((BundleInfoInternal *) m_pBundleInfo)->isActivated());
}

TEST_F(ResourceContainerImplTest, BundleNotRegisteredIfBundlePathIsInvalid)
{
    m_pBundleInfo->setPath("");
    m_pBundleInfo->setVersion("1.0");
    m_pBundleInfo->setLibraryPath("../.");
    m_pBundleInfo->setID("oic.bundle.test");

    m_pResourceContainer->registerBundle(m_pBundleInfo);

    EXPECT_EQ(nullptr, ((BundleInfoInternal *)m_pBundleInfo)->getBundleHandle());

}

TEST_F(ResourceContainerImplTest, SoBundleActivatedWithValidBundleInfo)
{
    m_pBundleInfo->setPath("libTestBundle.so");
    m_pBundleInfo->setVersion("1.0");
    m_pBundleInfo->setLibraryPath("../.");
    m_pBundleInfo->setID("oic.bundle.test");

    m_pResourceContainer->registerBundle(m_pBundleInfo);
    m_pResourceContainer->activateBundle(m_pBundleInfo);

    EXPECT_NE(nullptr, ((BundleInfoInternal *)m_pBundleInfo)->getBundleActivator());
}

/*TEST_F(ResourceContainerImplTest, JavaBundleActivatedWithValidBundleInfo)
{
    m_pBundleInfo->setPath("TestBundleJava/hue-0.1-jar-with-dependencies.jar");
    m_pBundleInfo->setActivatorName("org/iotivity/bundle/hue/HueBundleActivator");
    m_pBundleInfo->setLibraryPath("../.");
    m_pBundleInfo->setVersion("1.0");
    m_pBundleInfo->setID("oic.bundle.java.test2");

    m_pResourceContainer->registerBundle(m_pBundleInfo);
    m_pResourceContainer->activateBundle(m_pBundleInfo);
    EXPECT_TRUE(((BundleInfoInternal *) m_pBundleInfo)->isActivated());

}*/

TEST_F(ResourceContainerImplTest, BundleNotActivatedWhenNotRegistered)
{
    m_pBundleInfo->setPath("libTestBundle.so");
    m_pBundleInfo->setVersion("1.0");
    m_pBundleInfo->setLibraryPath("../.");
    m_pBundleInfo->setID("oic.bundle.test");

    m_pResourceContainer->activateBundle(m_pBundleInfo);

    EXPECT_EQ(nullptr, ((BundleInfoInternal *)m_pBundleInfo)->getBundleActivator());
}

TEST_F(ResourceContainerImplTest, SoBundleActivatedWithBundleID)
{
    m_pBundleInfo->setPath("libTestBundle.so");
    m_pBundleInfo->setVersion("1.0");
    m_pBundleInfo->setLibraryPath("../.");
    m_pBundleInfo->setID("oic.bundle.test");

    m_pResourceContainer->registerBundle(m_pBundleInfo);
    m_pResourceContainer->activateBundle(m_pBundleInfo->getID());

    EXPECT_NE(nullptr, ((BundleInfoInternal *)m_pBundleInfo)->getBundleActivator());
    EXPECT_TRUE(((BundleInfoInternal *)m_pBundleInfo)->isActivated());
}

TEST_F(ResourceContainerImplTest, BundleDeactivatedWithBundleInfo)
{
    m_pBundleInfo->setPath("libTestBundle.so");
    m_pBundleInfo->setVersion("1.0");
    m_pBundleInfo->setLibraryPath("../.");
    m_pBundleInfo->setID("oic.bundle.test");

    m_pResourceContainer->registerBundle(m_pBundleInfo);
    m_pResourceContainer->activateBundle(m_pBundleInfo);
    m_pResourceContainer->deactivateBundle(m_pBundleInfo);

    EXPECT_NE(nullptr, ((BundleInfoInternal *)m_pBundleInfo)->getBundleDeactivator());
    EXPECT_FALSE(((BundleInfoInternal *)m_pBundleInfo)->isActivated());
}

TEST_F(ResourceContainerImplTest, BundleDeactivatedWithBundleInfoJava)
{
    m_pBundleInfo->setPath("TestBundle/hue-0.1-jar-with-dependencies.jar");
    m_pBundleInfo->setActivatorName("org/iotivity/bundle/hue/HueBundleActivator");
    m_pBundleInfo->setLibraryPath("../.");
    m_pBundleInfo->setVersion("1.0");
    m_pBundleInfo->setID("oic.bundle.java.test");

    m_pResourceContainer->registerBundle(m_pBundleInfo);
    m_pResourceContainer->activateBundle(m_pBundleInfo);
    m_pResourceContainer->deactivateBundle(m_pBundleInfo);
    EXPECT_FALSE(((BundleInfoInternal *) m_pBundleInfo)->isActivated());
}

TEST_F(ResourceContainerImplTest, SoBundleDeactivatedWithBundleID)
{
    m_pBundleInfo->setPath("libTestBundle.so");
    m_pBundleInfo->setVersion("1.0");
    m_pBundleInfo->setLibraryPath("../.");
    m_pBundleInfo->setID("oic.bundle.test");

    m_pResourceContainer->registerBundle(m_pBundleInfo);
    m_pResourceContainer->activateBundle(m_pBundleInfo);

    m_pResourceContainer->deactivateBundle(m_pBundleInfo->getID());

    EXPECT_FALSE(((BundleInfoInternal *)m_pBundleInfo)->isActivated());
}

//TEST_F(ResourceContainerImplTest, JavaBundleDeactivatedWithBundleID)
//{
//}

/* Test for Configuration */TEST(ConfigurationTest, ConfigFileLoadedWithValidPath)
{
    Configuration *config = new Configuration(CONFIG_FILE);

    EXPECT_TRUE(config->isLoaded());
}

TEST(ConfigurationTest, ConfigFileNotLoadedWithInvalidPath)
{
    Configuration *config = new Configuration("InvalidPath");

    EXPECT_FALSE(config->isLoaded());
}

TEST(ConfigurationTest, BundleConfigurationListParsed)
{
    Configuration *config = new Configuration(CONFIG_FILE);

    configInfo bundles;
    map< string, string > results;

    config->getConfiguredBundles(&bundles);

    results = *bundles.begin();

    EXPECT_STREQ("oic.bundle.test", results["id"].c_str());
    EXPECT_STREQ("libTestBundle.so", results["path"].c_str());
    EXPECT_STREQ("1.0.0", results["version"].c_str());
}

TEST(ConfigurationTest, BundleConfigurationParsedWithValidBundleId)
{
    Configuration *config = new Configuration(CONFIG_FILE);

    configInfo bundle;
    map< string, string > results;

    config->getBundleConfiguration("oic.bundle.test", &bundle);

    results = *bundle.begin();

    EXPECT_STREQ("oic.bundle.test", results["id"].c_str());
    EXPECT_STREQ("libTestBundle.so", results["path"].c_str());
    EXPECT_STREQ("1.0.0", results["version"].c_str());
}

TEST(ConfigurationTest, BundleConfigurationNotParsedWithInvalidBundleId)
{
    Configuration *config = new Configuration(CONFIG_FILE);

    configInfo bundles;
    config->getBundleConfiguration("test", &bundles);

    EXPECT_TRUE(bundles.empty());
}

TEST(ConfigurationTest, BundleResourceConfigurationListParsed)
{
    Configuration *config = new Configuration(CONFIG_FILE);

    vector< resourceInfo > resourceConfig;
    resourceInfo result;

    config->getResourceConfiguration("oic.bundle.test", &resourceConfig);

    result = *resourceConfig.begin();

    EXPECT_STREQ("test_resource", result.name.c_str());
    EXPECT_STREQ("oic.test", result.resourceType.c_str());
}

TEST(ConfigurationTest, BundleResourceConfigurationNotParsedWithInvalidBundleId)
{
    Configuration *config = new Configuration(CONFIG_FILE);

    configInfo bundles;
    vector< resourceInfo > resourceConfig;
    config->getResourceConfiguration("test", &resourceConfig);

    EXPECT_TRUE(bundles.empty());
}
