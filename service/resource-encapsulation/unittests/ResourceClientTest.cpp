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

#include "UnitTestHelper.h"

#include "OCResource.h"

#include "RCSRemoteResourceObject.h"
#include "RCSDiscoveryManager.h"
#include "RCSResourceObject.h"
#include "RCSAddress.h"
#include "RCSRequest.h"
#include "cainterface.h"

#include <condition_variable>
#include <mutex>

using namespace OIC::Service;
using namespace OC;

constexpr char RESOURCEURI[]{ "/a/TemperatureSensor" };
constexpr char RESOURCETYPE[]{ "resource.type" };
constexpr char RESOURCEINTERFACE[]{ "oic.if.baseline" };

constexpr char ATTR_KEY[]{ "Temperature" };
constexpr int ATTR_VALUE{ 0 };

constexpr int DEFAULT_WAITING_TIME_IN_MILLIS = 3000;

void getRemoteAttributesCallback(const RCSResourceAttributes&, int) {}
void setRemoteAttributesCallback(const RCSResourceAttributes&, int) {}
void setRemoteRepresentationCallback(const HeaderOpts&, const RCSRepresentation&, int) {}
void resourceStateChanged(ResourceState) { }
void cacheUpdatedCallback(const RCSResourceAttributes&, int) {}

class RemoteResourceObjectTest: public TestWithMock
{
public:
    RCSResourceObject::Ptr server;
    RCSRemoteResourceObject::Ptr object;

public:
    void Proceed()
    {
        cond.notify_all();
    }

    void Wait(int waitingTime = DEFAULT_WAITING_TIME_IN_MILLIS)
    {
        std::unique_lock< std::mutex > lock{ mutex };
        cond.wait_for(lock, std::chrono::milliseconds{ waitingTime });
    }

protected:
    void SetUp()
    {
        TestWithMock::SetUp();

        CreateResource();

        WaitUntilDiscovered();

        ASSERT_NE(object, nullptr);
    }

    void TearDown()
    {
        TestWithMock::TearDown();

        // This method is to make sure objects disposed.
        WaitForPtrBeingUnique();
    }

private:
    void CreateResource()
    {
        server = RCSResourceObject::Builder(RESOURCEURI, RESOURCETYPE, RESOURCEINTERFACE)
        .setDefaultInterface(RESOURCEINTERFACE).build();
        server->setAttribute(ATTR_KEY, ATTR_VALUE);
    }

    void WaitUntilDiscovered()
    {
        for (int i=0; i<10 && !object; ++i)
        {
            auto discoveryTask = RCSDiscoveryManager::getInstance()->discoverResourceByType(
                    RCSAddress::multicast(), RESOURCETYPE,
                    std::bind(&RemoteResourceObjectTest::resourceDiscovered, this,
                            std::placeholders::_1));
            Wait(1000);
            discoveryTask->cancel();
        }
    }

    void WaitForPtrBeingUnique()
    {
        while((object && !object.unique()) || (server && !server.unique()))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds{ 100 });
        }
    }

    void resourceDiscovered(RCSRemoteResourceObject::Ptr resourceObject)
    {
        object = resourceObject;

        Proceed();
    }
private:
    std::condition_variable cond;
    std::mutex mutex;
};

TEST_F(RemoteResourceObjectTest, GetRemoteAttributesDoesNotAllowEmptyFunction)
{
    ASSERT_THROW(object->getRemoteAttributes({ }), RCSInvalidParameterException);
}

class TestRemoteAttributesCallback
{
public:
    virtual void getRemoteAttributesCallback(const RCSResourceAttributes&, int) = 0;
    virtual void setRemoteAttributesCallback(const RCSResourceAttributes&, int) = 0;
    virtual ~TestRemoteAttributesCallback() { }
};

TEST_F(RemoteResourceObjectTest, GetRemoteAttributesGetsAttributesOfServer)
{
    auto mockCallback = mocks.Mock< TestRemoteAttributesCallback >();
    mocks.ExpectCall(mockCallback, TestRemoteAttributesCallback::getRemoteAttributesCallback).
            Match([this](const RCSResourceAttributes& attrs, int)
            {
                RCSResourceObject::LockGuard lock{ server };
                return attrs == server->getAttributes();
            }
    ).Do([this](const RCSResourceAttributes&, int){ Proceed(); });

    object->getRemoteAttributes(std::bind(
            &TestRemoteAttributesCallback::getRemoteAttributesCallback, mockCallback,
            std::placeholders::_1, std::placeholders::_2));

    Wait();
}

TEST_F(RemoteResourceObjectTest, SetRemoteAttributesDoesNotAllowEmptyFunction)
{
    ASSERT_THROW(object->setRemoteAttributes({ }, { }), RCSInvalidParameterException);
}

TEST_F(RemoteResourceObjectTest, SetRemoteAttributesSetsAttributesOfServer)
{
    constexpr int newValue = ATTR_VALUE + 1;
    RCSResourceAttributes newAttrs;
    newAttrs[ATTR_KEY] = newValue;

    auto mockCallback = mocks.Mock< TestRemoteAttributesCallback >();
    mocks.ExpectCall(mockCallback, TestRemoteAttributesCallback::setRemoteAttributesCallback).
            Do([this](const RCSResourceAttributes&, int){ Proceed(); });

    object->setRemoteAttributes(newAttrs, std::bind(
            &TestRemoteAttributesCallback::setRemoteAttributesCallback, mockCallback,
            std::placeholders::_1, std::placeholders::_2));
    Wait();

    ASSERT_EQ(newValue, server->getAttributeValue(ATTR_KEY).get<int>());
}

TEST_F(RemoteResourceObjectTest, SetRemoteRepresentationDoesNotAllowEmptyFunction)
{
    RCSQueryParams queryParams;
    RCSRepresentation rcsRep;
    ASSERT_THROW(object->set(queryParams, rcsRep, {}), RCSInvalidParameterException);
}

TEST_F(RemoteResourceObjectTest, SetRemoteRepresentationSetsRepresentationOfServer)
{
    class TestSetRemoteRepresentationCallback
    {
    public:
        virtual void
        setRemoteRepresentationCallback(const HeaderOpts&, const RCSRepresentation&, int) = 0;
        virtual ~TestSetRemoteRepresentationCallback() { }
    };
    RCSRepresentation rcsRep;
    RCSQueryParams queryParams;
    constexpr int newValue = ATTR_VALUE + 1;
    RCSResourceAttributes newAttrs;
    newAttrs[ATTR_KEY] = newValue;

    rcsRep.setAttributes(newAttrs);

    auto mockCallback = mocks.Mock< TestSetRemoteRepresentationCallback >();
    mocks.ExpectCall(mockCallback,
                     TestSetRemoteRepresentationCallback::setRemoteRepresentationCallback).
            Do([this](const HeaderOpts&, const RCSRepresentation&, int){ Proceed(); });

    object->set(queryParams, rcsRep, std::bind(
            &TestSetRemoteRepresentationCallback::setRemoteRepresentationCallback,
            mockCallback, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    Wait();

    ASSERT_EQ(newValue, server->getAttributeValue(ATTR_KEY).get<int>());
}

TEST_F(RemoteResourceObjectTest, QueryParamsForGetWillBePassedToBase)
{
    class CustomHandler
    {
    public:
        virtual RCSGetResponse handle(const RCSRequest&, RCSResourceAttributes&) = 0;
        virtual ~CustomHandler() {}
    };

    constexpr char PARAM_KEY[] { "aKey" };
    constexpr char VALUE[] { "value" };

    auto mockHandler = mocks.Mock< CustomHandler >();

#ifdef __clang__
#warning "TODO: please fix ResourceClientTest.cpp"
#else
    mocks.ExpectCall(mockHandler, CustomHandler::handle).
            Match([](const RCSRequest& request, RCSResourceAttributes&)
            {
                return request.getInterface() == RESOURCEINTERFACE &&
                        request.getQueryParams().at(PARAM_KEY) == VALUE;
            }
    ).
            Do([this](const RCSRequest&, RCSResourceAttributes&)
            {
                Proceed();
                return RCSGetResponse::defaultAction();
            }
    );

    server->setGetRequestHandler(std::bind(&CustomHandler::handle, mockHandler,
            std::placeholders::_1, std::placeholders::_2));

    object->get(RCSQueryParams().setResourceInterface(RESOURCEINTERFACE).setResourceType(RESOURCETYPE).
            put(PARAM_KEY, VALUE),
            [](const HeaderOpts&, const RCSRepresentation&, int){});
#endif

    Wait();
}

TEST_F(RemoteResourceObjectTest, MonitoringIsNotStartedByDefault)
{
    ASSERT_FALSE(object->isMonitoring());
}

TEST_F(RemoteResourceObjectTest, StartMonitoringThrowsIfFunctionIsEmpty)
{
    ASSERT_THROW(object->startMonitoring({ }), RCSInvalidParameterException);
}

TEST_F(RemoteResourceObjectTest, IsMonitoringReturnsTrueAfterStartMonitoring)
{
    object->startMonitoring(resourceStateChanged);

    ASSERT_TRUE(object->isMonitoring());
}

TEST_F(RemoteResourceObjectTest, StartMonitoringThrowsIfTryingToStartAgain)
{
    object->startMonitoring(resourceStateChanged);

    ASSERT_THROW(object->startMonitoring(resourceStateChanged), RCSBadRequestException);
}

TEST_F(RemoteResourceObjectTest, DefaultStateIsNone)
{
    ASSERT_EQ(ResourceState::NONE, object->getState());
}

TEST_F(RemoteResourceObjectTest, CachingIsNotStartedByDefault)
{
    ASSERT_FALSE(object->isCaching());
}

TEST_F(RemoteResourceObjectTest, IsCachingReturnsTrueAfterStartCaching)
{
    object->startCaching(cacheUpdatedCallback);

    ASSERT_TRUE(object->isCaching());
}

TEST_F(RemoteResourceObjectTest, StartCachingThrowsIfTryingToStartAgain)
{
    object->startCaching(cacheUpdatedCallback);

    ASSERT_THROW(object->startCaching(), RCSBadRequestException);
}

TEST_F(RemoteResourceObjectTest, DefaultCacheStateIsNone)
{
    ASSERT_EQ(CacheState::NONE, object->getCacheState());
}

TEST_F(RemoteResourceObjectTest, CacheStateIsUnreadyAfterStartCaching)
{
    object->startCaching();

    ASSERT_EQ(CacheState::UNREADY, object->getCacheState());
}

class TestCacheUpdatedCallback
{
public:
    virtual void cacheUpdatedCallback(const RCSResourceAttributes&, int) = 0;
    virtual ~TestCacheUpdatedCallback() { }
};

TEST_F(RemoteResourceObjectTest, CacheStateIsReadyAfterCacheUpdated)
{
    auto mockCallback = mocks.Mock< TestCacheUpdatedCallback >();
    mocks.ExpectCall(mockCallback, TestCacheUpdatedCallback::cacheUpdatedCallback).
            Do([this](const RCSResourceAttributes&, int){ Proceed(); });

    object->startCaching(std::bind(&TestCacheUpdatedCallback::cacheUpdatedCallback,
                           mockCallback, std::placeholders::_1, std::placeholders::_2));
    Wait();

    ASSERT_EQ(CacheState::READY, object->getCacheState());
}

TEST_F(RemoteResourceObjectTest, IsCachedAvailableReturnsTrueWhenCacheIsReady)
{
    auto mockCallback = mocks.Mock< TestCacheUpdatedCallback >();
    mocks.ExpectCall(mockCallback, TestCacheUpdatedCallback::cacheUpdatedCallback).
                Do([this](const RCSResourceAttributes&, int){ Proceed(); });

    object->startCaching(std::bind(&TestCacheUpdatedCallback::cacheUpdatedCallback,
                                   mockCallback, std::placeholders::_1, std::placeholders::_2));
    Wait();

    ASSERT_TRUE(object->isCachedAvailable());
}

TEST_F(RemoteResourceObjectTest, DISABLED_CacheUpdatedCallbackBeCalledWheneverCacheUpdated)
{
    auto mockCallback = mocks.Mock< TestCacheUpdatedCallback >();

    mocks.OnCall(mockCallback, TestCacheUpdatedCallback::cacheUpdatedCallback).
                Do([this](const RCSResourceAttributes&, int){ Proceed(); });
    object->startCaching(std::bind(&TestCacheUpdatedCallback::cacheUpdatedCallback,
                                   mockCallback, std::placeholders::_1, std::placeholders::_2));
    Wait();

    mocks.ExpectCall(mockCallback, TestCacheUpdatedCallback::cacheUpdatedCallback).
            Do([this](const RCSResourceAttributes&, int){ Proceed(); });

    server->setAttribute(ATTR_KEY, ATTR_VALUE + 1);

    Wait();
}

TEST_F(RemoteResourceObjectTest, DISABLED_CacheUpdatedCallbackBeCalledWithUpdatedAttributes)
{
    constexpr int newValue = ATTR_VALUE + 1;

    auto mockCallback = mocks.Mock< TestCacheUpdatedCallback >();
    mocks.OnCall(mockCallback, TestCacheUpdatedCallback::cacheUpdatedCallback).
            Do([this](const RCSResourceAttributes&, int){ Proceed(); });
    object->startCaching(std::bind(&TestCacheUpdatedCallback::cacheUpdatedCallback,
                   mockCallback, std::placeholders::_1, std::placeholders::_2));
    Wait();
    mocks.ExpectCall(mockCallback, TestCacheUpdatedCallback::cacheUpdatedCallback).
            Match([&](const RCSResourceAttributes& attrs, int){
                return attrs.at(ATTR_KEY) == newValue;
            }).
            Do([this](const RCSResourceAttributes&, int){ Proceed(); });

    server->setAttribute(ATTR_KEY, newValue);
    Wait();
}

TEST_F(RemoteResourceObjectTest, GetCachedAttributesThrowsIfCachingIsNotStarted)
{
    ASSERT_THROW(object->getCachedAttributes(), RCSBadRequestException);
}

TEST_F(RemoteResourceObjectTest, CachedAttributesHasSameAttributesWithServer)
{
    auto mockCallback = mocks.Mock< TestCacheUpdatedCallback >();
    mocks.OnCall(mockCallback, TestCacheUpdatedCallback::cacheUpdatedCallback).
            Do([this](const RCSResourceAttributes&, int){ Proceed(); });
    object->startCaching(std::bind(&TestCacheUpdatedCallback::cacheUpdatedCallback,
               mockCallback, std::placeholders::_1, std::placeholders::_2));
    Wait();

    RCSResourceObject::LockGuard lock{ server };

    ASSERT_EQ(object->getCachedAttributes(), server->getAttributes());
}

TEST_F(RemoteResourceObjectTest, GetCachedAttributeThrowsIfCachingIsNotStarted)
{
    ASSERT_THROW(object->getCachedAttribute(ATTR_KEY), RCSBadRequestException);
}

TEST_F(RemoteResourceObjectTest, GetCachedAttributeThrowsIfKeyIsInvalid)
{
    auto mockCallback = mocks.Mock< TestCacheUpdatedCallback >();
    mocks.OnCall(mockCallback, TestCacheUpdatedCallback::cacheUpdatedCallback).
            Do([this](const RCSResourceAttributes&, int){ Proceed(); });
    object->startCaching(std::bind(&TestCacheUpdatedCallback::cacheUpdatedCallback,
                   mockCallback, std::placeholders::_1, std::placeholders::_2));
    Wait();

    ASSERT_THROW(object->getCachedAttribute(""), RCSInvalidKeyException);
}

TEST_F(RemoteResourceObjectTest, HasSameUriWithServer)
{
    EXPECT_EQ(RESOURCEURI, object->getUri());
}

TEST_F(RemoteResourceObjectTest, HasSameTypeWithServer)
{
    EXPECT_EQ(RESOURCETYPE, object->getTypes()[0]);
}

TEST_F(RemoteResourceObjectTest, HasSameInterfaceWithServer)
{
    EXPECT_EQ(RESOURCEINTERFACE, object->getInterfaces()[0]);
}

TEST_F(RemoteResourceObjectTest, GetValidOCResourceTest)
{
    std::shared_ptr<OC::OCResource> ocRes = RCSRemoteResourceObject::toOCResource(object);

    EXPECT_NE(nullptr, ocRes);

    EXPECT_EQ(RESOURCEURI, ocRes->uri());
}

