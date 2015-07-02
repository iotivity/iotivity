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

#include <internal/RequestHandler.h>

#include <OCPlatform.h>

using namespace std;

using namespace testing;
using namespace OIC::Service;

constexpr char EXISTING[]{ "ext" };
constexpr int ORIGIN_VALUE{ 100 };

using RegisterResource = OCStackResult (*)(OCResourceHandle&, std::string&,
        const std::string&, const std::string&, OC::EntityHandler, uint8_t);

class SimpleRequestHandlerTest: public Test
{
public:
    ResourceObject::Ptr server;

    ResourceAttributes requestAttrs;

    MockRepository mocks;

protected:
    void SetUp() override
    {
        mocks.OnCallFuncOverload(static_cast<RegisterResource>(OC::OCPlatform::registerResource))
                .Return(OC_STACK_OK);

        server = ResourceObject::Builder("a/test", "resourceType", "").build();

        server->setAttribute(EXISTING, ORIGIN_VALUE);
    }
};

TEST_F(SimpleRequestHandlerTest, ResponseHasSameValuesPassedToHandlerConstructor)
{
    SimpleRequestHandler handler{ OC_EH_ERROR, -1000 };

    auto response = handler.buildResponse(*server, requestAttrs);

    ASSERT_EQ(OC_EH_ERROR, response->getResponseResult());
    ASSERT_EQ(-1000, response->getErrorCode());
}

TEST_F(SimpleRequestHandlerTest, ResponseHasSameAttrsWithServerAttrs)
{
    SimpleRequestHandler handler{};

    auto response = handler.buildResponse(*server, requestAttrs);

    ASSERT_EQ(ORIGIN_VALUE, response->getResourceRepresentation()[EXISTING].getValue<int>());
}

TEST_F(SimpleRequestHandlerTest, ResponseHasAttrsSetByCustomAttrRequestHandler)
{
    constexpr char key[] { "key" };
    constexpr int newValue{ 100 };

    ResourceAttributes attrs;
    attrs[key] = newValue;
    CustomAttrRequestHandler handler{ attrs };

    auto response = handler.buildResponse(*server, requestAttrs);

    ASSERT_EQ(ORIGIN_VALUE, response->getResourceRepresentation()[key].getValue<int>());
}



class SetRequestProxyHandlerTest: public Test
{
public:
    ResourceObject::Ptr server;
    ResourceAttributes requestAttrs;
    RequestHandler::Ptr setRequestProxyHandler;

    MockRepository mocks;

protected:
    void SetUp() override
    {
        mocks.OnCallFuncOverload(static_cast<RegisterResource>(OC::OCPlatform::registerResource))
                .Return(OC_STACK_OK);

        setRequestProxyHandler = make_shared<SetRequestProxyHandler>(
                make_shared<SimpleRequestHandler>());

        server = ResourceObject::Builder("a/test", "resourceType", "").build();

        server->setAttribute(EXISTING, ORIGIN_VALUE);
    }
};

TEST_F(SetRequestProxyHandlerTest, NothingHappenedWithEmptyAttrs)
{

    setRequestProxyHandler->buildResponse(*server, requestAttrs);

    ASSERT_EQ(ORIGIN_VALUE, server->getAttribute<int>(EXISTING));
}


TEST_F(SetRequestProxyHandlerTest, ServerAttributesChangedIfOnlySameKeyExists)
{
    constexpr int newValue{ 100 };

    requestAttrs[EXISTING] = newValue;

    setRequestProxyHandler->buildResponse(*server, requestAttrs);

    ASSERT_EQ(newValue, server->getAttribute<int>(EXISTING));
}

TEST_F(SetRequestProxyHandlerTest, ThrowIfTypeMismatch)
{
    requestAttrs[EXISTING] = "";

    ASSERT_THROW(setRequestProxyHandler->buildResponse(*server, requestAttrs), PrimitiveException);
}

TEST_F(SetRequestProxyHandlerTest, ThrowIfRequestAttrsHasUnknownKey)
{
    constexpr char unknownKey[]{ "???" };

    requestAttrs[EXISTING] = ORIGIN_VALUE;
    requestAttrs[unknownKey] = ORIGIN_VALUE;

    ASSERT_THROW(setRequestProxyHandler->buildResponse(*server, requestAttrs), PrimitiveException);
}
