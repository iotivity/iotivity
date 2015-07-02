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

#include <gtest/gtest.h>
#include <HippoMocks/hippomocks.h>

#include <ResourceObject.h>

#include <OCPlatform.h>

using namespace std;
using namespace std::placeholders;

using namespace testing;

using namespace OIC::Service;
using namespace OC;

typedef OCStackResult (*registerResourceSig)(OCResourceHandle&,
                       string&,
                       const string&,
                       const string&,
                       EntityHandler,
                       uint8_t );

static constexpr char RESOURCE_URI[]{ "a/test" };
static constexpr char RESOURCE_TYPE[]{ "resourceType" };
static constexpr char KEY[]{ "key" };

TEST(ResourceObjectBuilderCreateTest, ThrowIfUriIsInvalid)
{
    ASSERT_THROW(ResourceObject::Builder("", "", "").build(), PlatformException);
}

class ResourceObjectBuilderTest: public Test
{
public:
    MockRepository mocks;

protected:
    void SetUp() override
    {
        mocks.OnCallFuncOverload(static_cast<registerResourceSig>(OCPlatform::registerResource))
                .Return(OC_STACK_OK);
    }
};

TEST_F(ResourceObjectBuilderTest, RegisterResourceWhenCallCreate)
{
    mocks.ExpectCallFuncOverload(static_cast<registerResourceSig>(OCPlatform::registerResource))
            .Return(OC_STACK_OK);
    ResourceObject::Builder(RESOURCE_URI, RESOURCE_TYPE, "").build();
}

TEST_F(ResourceObjectBuilderTest, ResourceServerHasPropertiesSetByBuilder)
{
    auto serverResource = ResourceObject::Builder(RESOURCE_URI, RESOURCE_TYPE, "").
            setDiscoverable(false).setObservable(true).build();

    EXPECT_FALSE(serverResource->isDiscoverable());
    EXPECT_TRUE(serverResource->isObservable());
}

TEST_F(ResourceObjectBuilderTest, ResourceServerHasAttrsSetByBuilder)
{
    ResourceAttributes attrs;
    attrs[KEY] = 100;

    auto serverResource = ResourceObject::Builder(RESOURCE_URI, RESOURCE_TYPE, "").
            setAttributes(attrs).build();

    ResourceObject::LockGuard lock{ serverResource };
    EXPECT_EQ(attrs, serverResource->getAttributes());
}


class ResourceObjectTest: public Test
{
public:
    MockRepository mocks;
    ResourceObject::Ptr server;

protected:
    void SetUp() override
    {
        initMocks();
        server = ResourceObject::Builder(RESOURCE_URI, RESOURCE_TYPE, "").build();
    }

    virtual void initMocks()
    {
        mocks.OnCallFuncOverload(static_cast< registerResourceSig >(OCPlatform::registerResource)).
                Return(OC_STACK_OK);

        mocks.OnCallFunc(OCPlatform::unregisterResource).Return(OC_STACK_OK);
    }
};

TEST_F(ResourceObjectTest, AccessAttributesWithLock)
{
    constexpr int value{ 100 };

    {
        ResourceObject::LockGuard lock{ server };
        auto& attr = server->getAttributes();
        attr[KEY] = value;
    }

    ASSERT_EQ(value, server->getAttribute<int>(KEY));
}

TEST_F(ResourceObjectTest, ThrowIfTryToAccessAttributesWithoutLock)
{
    ASSERT_THROW(server->getAttributes(), NoLockException);
}

TEST_F(ResourceObjectTest, ThrowIfLockRecursively)
{
    ResourceObject::LockGuard lock{ server };

    ASSERT_THROW(ResourceObject::LockGuard again{ server }, DeadLockException);
}

TEST_F(ResourceObjectTest, AccessingAttributesWithMethodsWithinLockDoesntCauseDeadLock)
{
    constexpr int value{ 100 };

    {
        ResourceObject::LockGuard lock{ server };
        server->setAttribute(KEY, value);
    }

    ASSERT_EQ(value, server->getAttribute<int>(KEY));
}



class ResourceObjectHandlingRequestTest: public ResourceObjectTest
{
public:
    EntityHandler handler;

    static constexpr OCRequestHandle fakeRequestHandle =
            reinterpret_cast<OCRequestHandle>(0x1234);
    static constexpr OCResourceHandle fakeResourceHandle =
            reinterpret_cast<OCResourceHandle>(0x4321);

public:
    OCResourceRequest::Ptr createRequest(OCMethod method = OC_REST_GET)
    {
        auto request = make_shared<OCResourceRequest>();

        OCEntityHandlerRequest ocEntityHandlerRequest { 0 };
        OC::MessageContainer mc;
        OCRepresentation ocRep;

        mc.addRepresentation(ocRep);

        string json = mc.getJSONRepresentation(OCInfoFormat::ExcludeOC);

        ocEntityHandlerRequest.requestHandle = fakeRequestHandle;
        ocEntityHandlerRequest.resource = fakeResourceHandle;
        ocEntityHandlerRequest.method = method;
        ocEntityHandlerRequest.reqJSONPayload = &json[0];

        formResourceRequest(OC_REQUEST_FLAG, &ocEntityHandlerRequest, request);

        return request;
    }

protected:
    OCStackResult registerResourceFake(OCResourceHandle&, string&, const string&,
            const string&, EntityHandler handler, uint8_t)
    {
        this->handler = handler;
        return OC_STACK_OK;
    }

    void initMocks() override
    {
        mocks.OnCallFuncOverload(
            static_cast<registerResourceSig>(OCPlatform::registerResource)).Do(
                    bind(&ResourceObjectHandlingRequestTest::registerResourceFake,
                            this, _1, _2, _3, _4, _5, _6));

        mocks.OnCallFunc(OCPlatform::unregisterResource).Return(OC_STACK_OK);
    }
};

TEST_F(ResourceObjectHandlingRequestTest, CallSendResponseWhenReceiveRequest)
{
    mocks.ExpectCallFunc(OCPlatform::sendResponse).Return(OC_STACK_OK);

    ASSERT_EQ(OC_EH_OK, handler(createRequest()));
}

TEST_F(ResourceObjectHandlingRequestTest, ReturnErrorCodeWhenSendResponseFailed)
{
    mocks.ExpectCallFunc(OCPlatform::sendResponse).Return(OC_STACK_ERROR);

    ASSERT_EQ(OC_EH_ERROR, handler(createRequest()));
}

TEST_F(ResourceObjectHandlingRequestTest, SendResponseWithSameHandlesPassedByRequest)
{
    mocks.ExpectCallFunc(OCPlatform::sendResponse).Match(
            [](const shared_ptr<OCResourceResponse> response)
            {
                return response->getRequestHandle() == fakeRequestHandle &&
                        response->getResourceHandle() == fakeResourceHandle;
            }
    ).Return(OC_STACK_OK);

    ASSERT_EQ(OC_EH_OK, handler(createRequest()));
}

TEST_F(ResourceObjectHandlingRequestTest, SendResponseWithPrimitiveResponseResults)
{
    constexpr int errorCode{ 1999 };
    constexpr OCEntityHandlerResult result{ OC_EH_SLOW };

    server->setGetRequestHandler(
            [](const PrimitiveRequest&, ResourceAttributes&) -> PrimitiveGetResponse
            {
                return PrimitiveGetResponse::create(result, errorCode);
            }
    );

    mocks.ExpectCallFunc(OCPlatform::sendResponse).Match(
            [](const shared_ptr<OCResourceResponse> response)
            {
                return response->getErrorCode() == errorCode &&
                        response->getResponseResult() == result;
            }
    ).Return(OC_STACK_OK);

    ASSERT_EQ(OC_EH_OK, handler(createRequest()));
}

TEST_F(ResourceObjectHandlingRequestTest, SendSetResponseWithCustomAttrsAndResults)
{
    constexpr int errorCode{ 1999 };
    constexpr OCEntityHandlerResult result{ OC_EH_SLOW };
    constexpr char value[]{ "value" };

    server->setSetRequestHandler(
            [](const PrimitiveRequest&, ResourceAttributes&) -> PrimitiveSetResponse
            {
                ResourceAttributes attrs;
                attrs[KEY] = value;
                return PrimitiveSetResponse::create(attrs, result, errorCode);
            }
    );

    mocks.ExpectCallFunc(OCPlatform::sendResponse).Match(
            [](const shared_ptr<OCResourceResponse> response)
            {
                return value == response->getResourceRepresentation()[KEY].getValue<std::string>()
                        && response->getErrorCode() == errorCode
                        && response->getResponseResult() == result;
            }
    ).Return(OC_STACK_OK);

    ASSERT_EQ(OC_EH_OK, handler(createRequest(OC_REST_PUT)));
}



class ResourceObjectSynchronizationTest: public ResourceObjectHandlingRequestTest
{
public:

    static void withLock(ResourceObject::Ptr serverResource, int count)
    {
        for (int i=0; i<count; ++i)
        {
            ResourceObject::LockGuard lock{ serverResource };

            auto& attrs = serverResource->getAttributes();

            attrs[KEY] = attrs[KEY].get<int>() + 1;
        }
    }

    static void withSetter(ResourceObject::Ptr serverResource, int count)
    {
        for (int i=0; i<count; ++i)
        {
            ResourceObject::LockGuard lock{ serverResource };

            serverResource->setAttribute(KEY, serverResource->getAttribute<int>(KEY) + 1);
        }
    }
};

TEST_F(ResourceObjectSynchronizationTest, MultipleAccessToServerResource)
{
    int expected { 0 };
    vector<thread> threads;

    server->setAttribute(KEY, 0);

    for (int i = 20; i >= 0; --i) {
        int count = 5000 + i * 100;
        threads.push_back(thread { withLock, server, count });
        expected += count;
    }

    for (int i = 20; i >= 0; --i) {
        int count = 5000 + i * 100;
        threads.push_back(thread { withSetter, server, count });
        expected +=count;
    }

    for (auto& t : threads)
    {
        t.join();
    }

    ASSERT_EQ(expected, server->getAttribute<int>(KEY));
}

TEST_F(ResourceObjectSynchronizationTest, MultipleAccessToServerResourceWithRequests)
{
    int expected { 0 };
    vector<thread> threads;

    mocks.OnCallFunc(OCPlatform::sendResponse).Return(OC_STACK_OK);

    server->setAttribute(KEY, 0);

    for (int i = 20; i >= 0; --i) {
        int count = 5000 + i * 100;
        threads.push_back(thread{ withLock, server, count });
        expected += count;
    }

    for (int i = 20; i >= 0; --i) {
        int count = 5000 + i * 100;
        threads.push_back(thread{ withSetter, server, count });
        expected +=count;
    }

    threads.push_back(thread{
        [this]()
        {
            for (int i=0; i<10000; ++i)
            {
                if (i % 5 == 0) handler(createRequest(OC_REST_OBSERVE));
                handler(createRequest((i & 1) ? OC_REST_GET : OC_REST_PUT));
            }
        }
    });

    for (auto& t : threads)
    {
        t.join();
    }

    ASSERT_EQ(expected, server->getAttribute<int>(KEY));
}
