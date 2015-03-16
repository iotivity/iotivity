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

namespace OCResourceTest
{
    using namespace OC;
    // Callbacks

    void onObserve(const HeaderOptions headerOptions, const OCRepresentation& rep,
                        const int& eCode, const int& sequenceNumber)
    {
    }

    void onGetPut(const HeaderOptions& headerOptions, const OCRepresentation& rep, const int eCode)
    {
    }

    void foundResource(std::shared_ptr<OCResource> resource)
    {

    }

    //Helper method
    OCResource::Ptr ConstructResourceObject(std::string uri)
    {
        OCConnectivityType connectivityType = OC_WIFI;
        std::vector<std::string> types = {"intel.rpost"};
        std::vector<std::string> ifaces = {DEFAULT_INTERFACE};

        return OCPlatform::constructResourceObject(std::string(""), uri,
                connectivityType, false, types, ifaces);
    }

    //ConstructResourceTest
    TEST(ConstructResourceTest, ConstructResourceObject)
    {
        EXPECT_ANY_THROW(ConstructResourceObject(std::string("")));
    }

    TEST(ResourceGetTest, ResourceGetForValidUri)
    {
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000");
        if(resource)
        {
            QueryParamsMap test;
            EXPECT_EQ(OC_STACK_OK, resource->get(OC::QueryParamsMap(), &onGetPut));
        }
    }

    TEST(ResourcePutTest, ResourcePutForValid)
    {
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000");
        if(resource)
        {
            QueryParamsMap test;
            OCRepresentation rep;
            EXPECT_EQ(OC_STACK_OK, resource->put(rep, test, &onGetPut));
        }
    }


    TEST(ResourcePostTest, ResourcePostValidConfiguration)
    {
        PlatformConfig cfg;
        OCPlatform::Configure(cfg);

        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000");
        if(resource)
        {
            OCRepresentation rep;
            QueryParamsMap test;
            EXPECT_EQ(OC_STACK_OK, resource->post(rep, test, &onGetPut));
        }
    }

    TEST(ResourceObserveTest, ResourceObserveValidUri)
    {
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000");
        if(resource)
        {
            QueryParamsMap test;
            OCRepresentation rep;
            EXPECT_EQ(OC_STACK_OK,resource->observe(ObserveType::ObserveAll, test, &onObserve));
        }
    }

}

