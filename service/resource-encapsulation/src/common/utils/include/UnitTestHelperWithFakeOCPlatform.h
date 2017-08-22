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

#ifndef COMMON_UTIL_UNITTESTHELPERWITHFAKEOCPLATFORM_H
#define COMMON_UTIL_UNITTESTHELPERWITHFAKEOCPLATFORM_H

#include <gtest/gtest.h>
#include <HippoMocks/hippomocks.h>

#include "octypes.h"
#include "OCPlatform.h"

using namespace std;
using namespace std::placeholders;

class FakeOCPlatform
{
public:
    virtual OCStackResult subscribePresence(OC::OCPlatform::OCPresenceHandle& a,
                                            const std::string& b, OCConnectivityType c,
                                            OC::SubscribeCallback d);

    virtual OCStackResult subscribePresence2(OC::OCPlatform::OCPresenceHandle& a,
                                             const std::string& b, const std::string& c,
                                             OCConnectivityType d, OC::SubscribeCallback e);

    virtual OCStackResult unsubscribePresence(OC::OCPlatform::OCPresenceHandle a);

    virtual OCStackResult findResource(const std::string& a, const std::string& b,
                               OCConnectivityType c, OC::FindCallback d);
    virtual OCStackResult registerResource(OCResourceHandle&, string&, const string&, const string&,
               OC::EntityHandler, uint8_t ) = 0;
    virtual OCStackResult unregisterResource(const OCResourceHandle &) = 0;

    virtual OCStackResult sendResponse(const std::shared_ptr<OC::OCResourceResponse>) = 0;
    virtual OCStackResult bindInterfaceToResource(
            const OCResourceHandle &, const std::string &) = 0;
    virtual OCStackResult bindTypeToResource(const OCResourceHandle &, const std::string &) = 0;
    virtual OCStackResult bindResource(const OCResourceHandle, const OCResourceHandle) = 0;

    virtual OCStackResult notifyAllObservers(OCResourceHandle) = 0;

    virtual ~FakeOCPlatform() { }
};

extern MockRepository mocks;
extern FakeOCPlatform * mockFakePlatform;

namespace OC
{
    namespace OCPlatform
    {
        OCStackResult subscribePresence(OCPresenceHandle& a, const std::string& b,
                                        OCConnectivityType c, SubscribeCallback d);

        OCStackResult subscribePresence(OCPresenceHandle& a, const std::string& b,
                                        const std::string& c, OCConnectivityType d,
                                        SubscribeCallback e);

        OCStackResult unsubscribePresence(OCPresenceHandle a);

        OCStackResult findResource(const std::string& a, const std::string& b,
                                   OCConnectivityType c, FindCallback d);

        OCStackResult registerResource(
                OCResourceHandle &a, string &b, const string &c,
                const string &d, EntityHandler e, uint8_t f);

        OCStackResult unregisterResource(const OCResourceHandle &a);

        OCStackResult bindResource(const OCResourceHandle a, const OCResourceHandle b);

        OCStackResult sendResponse(const std::shared_ptr<OCResourceResponse> a);

        OCStackResult bindTypeToResource(const OCResourceHandle &a,
                                const std::string &b);

        OCStackResult bindInterfaceToResource(const OCResourceHandle &a,
                                              const std::string &b);

        OCStackResult notifyAllObservers(OCResourceHandle a);
    }
}

class TestWithExternMock: public testing::Test
{
protected:
    virtual ~TestWithExternMock() noexcept(noexcept(std::declval<Test>().~Test())) {}

    virtual void SetUp()
    {
        mockFakePlatform = mocks.Mock< FakeOCPlatform >();
    }

    virtual void TearDown()
    {
        mocks.reset();
    }
};

#endif // COMMON_UTIL_UNITTESTHELPERWITHFAKEOCPLATFORM_H
