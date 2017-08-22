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

#include "UnitTestHelperWithFakeOCPlatform.h"

using namespace std;
using namespace std::placeholders;

extern FakeOCPlatform * mockFakePlatform;

namespace OC
{
    namespace OCPlatform
    {
        OCStackResult subscribePresence(OCPresenceHandle& a, const std::string& b,
                                        OCConnectivityType c, SubscribeCallback d)
        {
            return mockFakePlatform->subscribePresence(a, b, c, d);
        }

        OCStackResult subscribePresence(OCPresenceHandle& a, const std::string& b,
                                        const std::string& c, OCConnectivityType d,
                                        SubscribeCallback e)
        {
            return mockFakePlatform->subscribePresence2(a, b, c, d, e);
        }

        OCStackResult unsubscribePresence(OCPresenceHandle a)
        {
            return mockFakePlatform->unsubscribePresence(a);
        }

        OCStackResult findResource(const std::string& a, const std::string& b,
                                   OCConnectivityType c, FindCallback d)
        {
            return mockFakePlatform->findResource(a, b, c, d);
        }

        OCStackResult registerResource(
                OCResourceHandle &a, string &b, const string &c,
                const string &d, EntityHandler e, uint8_t f)
        {
            return mockFakePlatform->registerResource(a, b, c, d, e, f);
        }

        OCStackResult unregisterResource(const OCResourceHandle &a)
        {
            return mockFakePlatform->unregisterResource(a);
        }

        OCStackResult bindResource(const OCResourceHandle a, const OCResourceHandle b)
        {
            return mockFakePlatform->bindResource(a, b);
        }

        OCStackResult sendResponse(const std::shared_ptr<OCResourceResponse> a)
        {
            return mockFakePlatform->sendResponse(a);
        }

        OCStackResult bindTypeToResource(const OCResourceHandle &a,
                                const std::string &b)
        {
            return mockFakePlatform->bindTypeToResource(a, b);
        }

        OCStackResult bindInterfaceToResource(const OCResourceHandle &a,
                                              const std::string &b)
        {
            return mockFakePlatform->bindInterfaceToResource(a, b);
        }

        OCStackResult notifyAllObservers(OCResourceHandle a)
        {
            return mockFakePlatform->notifyAllObservers(a);
        }
    }
}
