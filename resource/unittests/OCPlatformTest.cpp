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

#include <OCPlatform.h>
#include <OCApi.h>
#include <gtest/gtest.h>

namespace OCPlatformTest
{
    using namespace OC;

    // Callbacks
    OCEntityHandlerResult entityHandler(std::shared_ptr<OCResourceRequest> request)
    {
        return OC_EH_OK;
    }

    const OCResourceHandle HANDLE_ZERO = 0;

    PlatformConfig cfg;

    void foundResource(std::shared_ptr<OCResource> resource)
    {
    }

    std::string resourceURI;
    std::string resourceTypeName = "core.res";
    std::string resourceInterface = DEFAULT_INTERFACE;
    uint8_t resourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE;
    OCResourceHandle resourceHandle;

    //Helper methods
    OCResourceHandle RegisterResource(std::string uri, std::string type, std::string iface)
    {
        EXPECT_EQ(OC_STACK_OK,OCPlatform::registerResource(
                                        resourceHandle, uri, type,
                                        iface, entityHandler, resourceProperty));
        return resourceHandle;
    }

    OCResourceHandle RegisterResource(std::string uri, std::string type)
    {
        EXPECT_EQ(OC_STACK_OK, OCPlatform::registerResource(
                                        resourceHandle, uri, type,
                                        resourceInterface, entityHandler, resourceProperty));
        return resourceHandle;
    }

    OCResourceHandle RegisterResource(std::string uri)
    {
        EXPECT_EQ(OC_STACK_OK, OCPlatform::registerResource(
                                        resourceHandle, uri, resourceTypeName,
                                        resourceInterface, entityHandler, resourceProperty));
        return resourceHandle;
    }

    //RegisterResourceTest
    TEST(RegisterResourceTest, RegisterSingleResource)
    {
        std::string uri = "/a/res2";
        EXPECT_NE(HANDLE_ZERO, RegisterResource(uri));
    }

    TEST(RegisterResourceTest, RegisterMultipleResources)
    {
        std::string uri = "/a/multi";
        //Good enough for 5 resources.
        for(int i=0; i< 5; i++)
        {
            uri +=std::to_string(i);
            EXPECT_NE(HANDLE_ZERO, RegisterResource(uri));
        }
    }

    TEST(RegisterResourceTest, ReregisterResource)
    {
        OCResourceHandle resourceHandle = RegisterResource(std::string("/a/light5"),
            std::string("core.light"));
        EXPECT_EQ(OC_STACK_OK, OC::OCPlatform::unregisterResource(resourceHandle));

        EXPECT_NE(HANDLE_ZERO, RegisterResource(std::string("/a/light5"),
            std::string("core.light")));

    }

    TEST(RegisterResourceTest, RegisterEmptyResource)
    {
        // We should not allow empty URI.
        std::string emptyStr = "";
        EXPECT_ANY_THROW(OCPlatform::registerResource(resourceHandle, emptyStr, emptyStr,
                                        emptyStr, entityHandler, resourceProperty));
    }

    TEST(RegisterResourceTest, RegisterZeroResourceProperty)
    {
        std::string uri = "/a/light6";
        std::string type = "core.light";
        uint8_t resourceProperty = 0;
        EXPECT_EQ(OC_STACK_OK, OCPlatform::registerResource(
                resourceHandle, uri, type,
                resourceInterface, entityHandler, resourceProperty));
    }

    //UnregisterTest
    TEST(UnregisterTest, UnregisterZeroHandleValue)
    {
        EXPECT_ANY_THROW(OC::OCPlatform::unregisterResource(HANDLE_ZERO));
    }

    //UnbindResourcesTest
    TEST(UnbindResourcesTest, UnbindResources)
    {
        OCResourceHandle resourceHome = RegisterResource(std::string("a/home"),
            std::string("core.home"));
        OCResourceHandle resourceKitchen = RegisterResource(std::string("a/kitchen"),
            std::string("core.kitchen"), LINK_INTERFACE);
        OCResourceHandle resourceRoom = RegisterResource(std::string("a/office"),
            std::string("core.office"), LINK_INTERFACE);

        std::vector<OCResourceHandle> rList;
        rList.push_back(resourceKitchen);
        rList.push_back(resourceRoom);
        EXPECT_EQ(OC_STACK_OK, OCPlatform::bindResources(resourceHome, rList));
        EXPECT_EQ(OC_STACK_OK, OCPlatform::unbindResources(resourceHome, rList));
    }

    TEST(UnbindResourcesTest, UnbindResourcesWithZero)
    {
        OCResourceHandle resourceHandle1 = 0;
        OCResourceHandle resourceHandle2 = 0;
        OCResourceHandle resourceHandle3 = 0;

        std::vector<OCResourceHandle> rList;

        rList.push_back(resourceHandle2);
        rList.push_back(resourceHandle3);

        EXPECT_ANY_THROW(OCPlatform::unbindResources(resourceHandle1, rList));
    }

    //BindInterfaceToResourceTest
    TEST(BindInterfaceToResourceTest, BindResourceInterface)
    {
        OCResourceHandle resourceHandle = RegisterResource(std::string("/a/light"),
            std::string("core.light"));
        OCStackResult result = OC::OCPlatform::bindInterfaceToResource(resourceHandle,
            BATCH_INTERFACE);
        EXPECT_EQ(OC_STACK_OK, result);
    }

    TEST(BindInterfaceToResourceTest, BindZeroResourceInterface)
    {
        OCResourceHandle resourceHandle = RegisterResource(std::string("/a/light1"),
            std::string("core.light"));
        EXPECT_ANY_THROW(OC::OCPlatform::bindInterfaceToResource(resourceHandle, 0));
    }

    //BindTypeToResourceTest
    TEST(BindTypeToResourceTest, BindResourceType)
    {
        OCResourceHandle resourceHandle = RegisterResource(std::string("/a/light3"),
            std::string("core.light"));
        OCStackResult result = OC::OCPlatform::bindTypeToResource(resourceHandle,
            "core.brightlight");
        EXPECT_EQ(OC_STACK_OK, result);
    }

    TEST(BindTypeToResourceTest, BindZeroResourceType)
    {
        OCResourceHandle resourceHandle = RegisterResource(std::string("/a/light4"),
            std::string("core.light"));
        EXPECT_ANY_THROW(OC::OCPlatform::bindTypeToResource(resourceHandle, 0));
    }

    //UnbindResourceTest
    TEST(UnbindResourceTest, BindAndUnbindResource)
    {
        OCResourceHandle resourceHandle1 = RegisterResource(std::string("a/unres"),
            std::string("core.unres"));
        OCResourceHandle resourceHandle2 = RegisterResource(std::string("a/unres2"),
            std::string("core.unres"), LINK_INTERFACE);

        EXPECT_EQ(OC_STACK_OK, OCPlatform::bindResource(resourceHandle1, resourceHandle2));
        EXPECT_EQ(OC_STACK_OK, OCPlatform::unbindResource(resourceHandle1, resourceHandle2));
    }

    //PresenceTest
    TEST(PresenceTest, StartAndStopPresence)
    {
        EXPECT_EQ(OC_STACK_OK, OCPlatform::startPresence(30));
        EXPECT_NE(HANDLE_ZERO, RegisterResource( std::string("/a/Presence"),
            std::string("core.Presence")));
        EXPECT_EQ(OC_STACK_OK, OCPlatform::stopPresence());
    }

    TEST(OCPlatformTest, UnbindZeroRsourceHandleValue)
    {
        EXPECT_ANY_THROW(OCPlatform::unbindResource(HANDLE_ZERO, HANDLE_ZERO));
    }

    //NotifyAllObserverTest
    TEST(NotifyAllObserverTest, NotifyAllObservers)
    {
        OCResourceHandle resourceHome = RegisterResource(std::string("/a/obs"),
            std::string("core.obs"));
        EXPECT_EQ(OC_STACK_NO_OBSERVERS, OCPlatform::notifyAllObservers(resourceHome));
    }

    TEST(NotifyAllObserverTest, NotifyListOfObservers)
    {
        OCResourceHandle resourceHome = RegisterResource(std::string("/a/obs2"),
            std::string("core.obs"));

        std::shared_ptr<OCResourceResponse> resourceResponse(new OCResourceResponse());
        ObservationIds interestedObservers;
        EXPECT_ANY_THROW(OCPlatform::notifyListOfObservers(resourceHome,
            interestedObservers, resourceResponse));
    }

    //DeviceEntityHandlerTest
    TEST(DeviceEntityHandlerTest, SetDefaultDeviceEntityHandler)
    {
        EXPECT_EQ(OC_STACK_OK, OCPlatform::setDefaultDeviceEntityHandler(entityHandler));
        EXPECT_EQ(OC_STACK_OK, OCPlatform::setDefaultDeviceEntityHandler(NULL));
    }
}
