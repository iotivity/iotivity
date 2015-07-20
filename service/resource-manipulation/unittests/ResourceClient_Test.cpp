#define private public
#include <gtest/gtest.h>
#include <iostream>
#include "ResourceClient.h"
#include "ResourceObject.h"
#include "OCPlatform.h"

#define RESOURCEURI "/a/TemperatureSensor"
#define RESOURCETYPE "Resource.Hosting"
#define RESOURCEINTERFACE "oic.if.baseline"

using namespace OIC::Service;
using namespace OC;

bool cbresult = false;
std::string uri = "/oic/res?rt=Resource.Hosting";
std::shared_ptr<RemoteResourceObject> object;
DiscoveryManager *manager = DiscoveryManager::getInstance();
ResourceState receivedResourceState;
ResourceAttributes receivedResourceAttributes;
ResourceObject::Ptr server;

void createResource()
{
    server = ResourceObject::Builder(RESOURCEURI, RESOURCETYPE,
                                     RESOURCEINTERFACE).setDiscoverable(true).setObservable(true).build();
    std::cout << "Resource created successfully " << std::endl;
    server->setAttribute("Temperature", 0);
    PlatformConfig config
    {
        OC::ServiceType::InProc, ModeType::Client, "0.0.0.0", 0, OC::QualityOfService::LowQos
    };
    OCPlatform::Configure(config);
}

void destroyResource()
{
    server = NULL;
}

//Callbacks
void onRemoteAttrReceived(const ResourceAttributes &attributes)
{
    cbresult = true;
    receivedResourceAttributes = attributes;
}

void onResourceDiscoveredCallback(std::shared_ptr<RemoteResourceObject> receivedObject)
{
    std::cout << "Resource discovered" << std::endl;
    object = receivedObject;
    std::cout << object->getUri() << std::endl;
    std::cout << "Resource discovered exited" << std::endl;
    EXPECT_TRUE(object != NULL);
}

void onResourceStateChanged(ResourceState state)
{
    receivedResourceState = state;
}

void onCacheUpdated(const ResourceAttributes attribute)
{
    receivedResourceAttributes = attribute;
}

void OnRemoteAttributesSetCallback(const ResourceAttributes &attributes)
{
    cbresult = true;
}

//Send valid call to discover resource
TEST(ResourceClientTest, testDiscoverResourcePass)
{
    createResource();
    DiscoveryManager *instance = DiscoveryManager::getInstance();
    cbresult = false;

    instance->discoverResource("", uri, CT_DEFAULT, &onResourceDiscoveredCallback);
    sleep(2);
    EXPECT_TRUE(object != NULL);
    destroyResource();
}

//Get remote attributes when callback is not NULL
TEST(ResourceClientTest, testGetRemoteAttributesPass)
{
    createResource();
    sleep(2);
    cbresult = false;
    object->getRemoteAttributes(&onRemoteAttrReceived);
    sleep(2);

    EXPECT_TRUE(cbresult);
    destroyResource();
}

//Set remote attributes when callback is not NULL
TEST(ResourceClientTest, testSetRemoteAttributesPass)
{
    createResource();
    sleep(2);

    cbresult = false;
    object->getRemoteAttributes(&onRemoteAttrReceived);
    sleep(2);
    ResourceAttributes::const_iterator iter = receivedResourceAttributes.begin();
    for (unsigned int i = 0; i < receivedResourceAttributes.size(); ++i)
    {
        if ( iter->key() == "Temperature")
        {
            std::cout << "It is there" << std::endl;
            receivedResourceAttributes["Temperature"]  = 100;
            object->setRemoteAttributes(receivedResourceAttributes, &OnRemoteAttributesSetCallback);
        }
        ++iter;
    }
    sleep(2);
    EXPECT_TRUE(cbresult);
    destroyResource();
}

//Check value of isWatching
TEST(ResourceClientTest, testIsWatching)
{
    createResource();
    manager->DiscoveryManager::discoverResource("", uri, CT_DEFAULT, &onResourceDiscoveredCallback);
    sleep(1);
    destroyResource();
    EXPECT_FALSE(object->isWatching());
}

//Check value of isCaching
TEST(ResourceClientTest, testIsCaching)
{
    createResource();
    destroyResource();
    EXPECT_FALSE(object->isCaching());
}

TEST(ResourceClientTest, testIsObservable)
{
    createResource();
    destroyResource();
    EXPECT_TRUE(object->isObservable());
}

//Check value of startWatching for NULL callback
TEST(ResourceClientTest, testStartWatchingFail)
{
    createResource();
    object->m_watchingFlag = false;
    object->startMonitoring(NULL);
    EXPECT_TRUE(object->m_watchingFlag);
    object->stopMonitoring();
    destroyResource();
}

//Check value of startWatching for non NULL callback
TEST(ResourceClientTest, testStartWatchingPass)
{
    createResource();
    object->startMonitoring(&onResourceStateChanged);
    EXPECT_TRUE(object->m_watchingFlag);
    object->stopMonitoring();
    destroyResource();
}

//Stop watching when watching is true
TEST(ResourceClientTest, testStopWatching)
{
    createResource();
    object->startMonitoring(&onResourceStateChanged);
    object->stopMonitoring();
    EXPECT_FALSE(object->m_watchingFlag);
    object->startMonitoring(&onResourceStateChanged);
    destroyResource();
}

//Get state of resource
TEST(ResourceClientTest, testGetStatePass)
{
    createResource();
    ResourceState result = object->getState();
    EXPECT_TRUE(result >= ResourceState::NOT_WATCHING && result <= ResourceState::DESTROYED);
    destroyResource();
}

//Cache ID is not zero
TEST(ResourceClientTest, testStartCachingPass)
{
    createResource();
    object->m_cachingFlag = false;
    object->startCaching();
    EXPECT_TRUE(object->m_cachingFlag);
    destroyResource();
}

TEST(ResourceClientTest, testStopCaching)
{
    createResource();
    object->m_cachingFlag = false;
    object->startCaching();
    object->stopCaching();
    EXPECT_FALSE(object->m_cachingFlag);
    destroyResource();
}

TEST(ResourceClientTest, testRefreshCache)
{
    createResource();
    object->m_cachingFlag = false;
    object->startCaching();
    object->refreshCache();
    EXPECT_TRUE(object->getResourceCacheState() == CacheState::UPDATING);
    destroyResource();
}

//Callback for start caching is not NULL
TEST(ResourceClientTest, testStartCachingCbPass)
{
    createResource();
    object->m_cachingFlag = false;
    object->startCaching(&onCacheUpdated);
    EXPECT_TRUE(object->m_cachingFlag);
    destroyResource();
}

//Callback for start caching is NULL
TEST(ResourceClientTest, testStartCachingCbFail)
{
    createResource();
    object->m_cachingFlag = false;
    object->startCaching(NULL);
    EXPECT_TRUE(object->m_cachingFlag);
    destroyResource();
}

//Get resource state
TEST(ResourceClientTest, testGetResourceCacheState)
{
    createResource();
    CacheState result = object->getResourceCacheState();
    EXPECT_TRUE(result >= CacheState::READY && result <= CacheState::NONE);
    destroyResource();
}

//Get cached attributes
TEST(ResourceClientTest, testGetCachedAttributesWithoutCallback)
{
    createResource();
    ResourceAttributes result = object->getCachedAttributes();
    EXPECT_TRUE(result != NULL);
    destroyResource();
}

//Check with invalid attribute value
TEST(ResourceClientTest, testGetCachedAttributeWithInvalidAttribute)
{
    createResource();
    ResourceAttributes::Value result = object->getCachedAttribute("");
    std::nullptr_t null;
    EXPECT_TRUE(result == null);
    destroyResource();
}

//Get remote attributes when callback is NULL
TEST(ResourceClientTest, testGetRemoteAttributesFail)
{
    createResource();
    cbresult = false;
    object->getRemoteAttributes(NULL);
    EXPECT_FALSE(cbresult);
    destroyResource();
}

TEST(ResourceClientTest, testGetUri)
{
    createResource();
    std::string result = object->getUri();
    EXPECT_TRUE(result != NULL);
    destroyResource();
}

TEST(ResourceClientTest, testGetAddress)
{
    createResource();
    std::string result = object->getAddress();
    EXPECT_TRUE(result != NULL);
    destroyResource();
}

TEST(ResourceClientTest, testGetTypes)
{
    createResource();
    std::vector < std::string > result = object->getTypes();
    EXPECT_TRUE(result.size() != 0);
    destroyResource();
}

TEST(ResourceClientTest, testGetInterfaces)
{
    createResource();
    std::vector < std::string > result = object->getInterfaces();
    EXPECT_TRUE(result.size() != 0);
    destroyResource();
}

//Check with valid attribute value
TEST(ResourceClientTest, testGetCachedAttribute)
{
    createResource();
    ResourceAttributes::Value result = object->getCachedAttribute("Temperature");
    EXPECT_TRUE(result != NULL);
    destroyResource();
}

//Test getting instance of DiscoveryManager
TEST(ResourceClientTest, testGetInstance)
{
    createResource();
    DiscoveryManager *instance = DiscoveryManager::getInstance();
    EXPECT_TRUE(instance != NULL);
    destroyResource();
}

//Send empty resource URI
TEST(ResourceClientTest, testDiscoverResourceEmptyResource)
{
    createResource();
    DiscoveryManager *instance = DiscoveryManager::getInstance();
    EXPECT_THROW(instance->discoverResource("", "", CT_DEFAULT, &onResourceDiscoveredCallback),
                 InvalidParameterException);
    destroyResource();
}

//Send NULL callback
TEST(ResourceClientTest, testDiscoverResourceEmptyCallback)
{
    createResource();
    DiscoveryManager *instance = DiscoveryManager::getInstance();
    EXPECT_THROW(instance->discoverResource("", uri, CT_DEFAULT, NULL), InvalidParameterException);
    destroyResource();
    object->stopMonitoring();
}

//Send invalid ResourceAttributes object to function
TEST(ResourceClientTest, testSetRemoteAttributesInvalidAttributes)
{
    createResource();
    cbresult = false;
    ResourceAttributes attr;
    //object->getRemoteAttributes(&onRemoteAttrReceived);
    object->setRemoteAttributes(attr, &OnRemoteAttributesSetCallback);
    EXPECT_FALSE(cbresult);
    destroyResource();
}
