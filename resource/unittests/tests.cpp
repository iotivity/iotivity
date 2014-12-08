//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
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


#include <iostream>
#include <thread>
#include <functional>
#include <condition_variable>

#include <OCPlatform.h>
#include <OCApi.h>

#include <gtest/gtest.h>

namespace PH = std::placeholders;

using namespace OC;
using namespace std;

// Entity handler used for register and find test
OCEntityHandlerResult entityHandler_rf(std::shared_ptr<OCResourceRequest> request)
{
    return OC_EH_OK;
}

// Condition variables used for register and find
std::mutex mutex_rf;
std::condition_variable cv_rf;
std::shared_ptr <OCResource> res_rf;

void foundResource_rf(std::shared_ptr<OCResource> resource)
{
    if(resource)
    {
        res_rf = resource;
        cv_rf.notify_all();
    }
}

// Resource : Register and find test
TEST(Resource, rf) {
    // Create PlatformConfig object
    PlatformConfig cfg {
        OC::ServiceType::InProc,
        OC::ModeType::Both,
        "0.0.0.0", // By setting to "0.0.0.0", it binds to all available interfaces
        0,         // Uses randomly available port
        OC::QualityOfService::LowQos
    };
    OCPlatform::Configure(cfg);

    std::string resourceURI = "/a/res";
    std::string resourceTypeName = "core.res";
    std::string resourceInterface = DEFAULT_INTERFACE;

    uint8_t resourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE;

    OCResourceHandle resourceHandle;

    // This will internally create and register the resource.
    if(OC_STACK_OK == OCPlatform::registerResource(
                                    resourceHandle, resourceURI, resourceTypeName,
                                    resourceInterface, entityHandler_rf, resourceProperty))
    {
        OCPlatform::findResource("","coap://224.0.1.187/oc/core?rt=core.res", foundResource_rf);

        {
            std::unique_lock<std::mutex> lk(mutex_rf);
            cv_rf.wait(lk);
        }

        if(res_rf)
        {
            EXPECT_EQ(res_rf->uri(), "/a/res");
            vector<std::string> rts = res_rf->getResourceTypes();
            EXPECT_EQ(rts.size(), (unsigned) 1);
            EXPECT_EQ(rts[0], "core.res");
            vector<std::string> ifs = res_rf->getResourceInterfaces();
            EXPECT_EQ(ifs.size(), (unsigned) 1);
            EXPECT_EQ(ifs[0], DEFAULT_INTERFACE);
        }
    }
}

