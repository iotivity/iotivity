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
#include <string>
#include <map>

namespace OCResourceTest
{
    using namespace OC;
    // Callbacks
    void onObserve(const HeaderOptions, const OCRepresentation&, const int&, const int&)
    {
    }

    void onGetPut(const HeaderOptions&, const OCRepresentation& , const int eCode)
    {
        EXPECT_EQ(eCode, OC_STACK_OK);
    }

    void foundResource(std::shared_ptr<OCResource> )
    {
    }

    void deleteResponse(const HeaderOptions&, const int)
    {
    }

    //Helper method
    OCResource::Ptr ConstructResourceObject(std::string host, std::string uri)
    {
        OCConnectivityType connectivityType = OC_WIFI;
        std::vector<std::string> types = {"intel.rpost"};
        std::vector<std::string> ifaces = {DEFAULT_INTERFACE};

        return OCPlatform::constructResourceObject(host, uri,
                connectivityType, false, types, ifaces);
    }

     //Get Test
    TEST(ConstructResourceTest, ConstructResourceObject)
    {
        EXPECT_ANY_THROW(ConstructResourceObject(std::string(""), std::string("")));
    }

    TEST(ResourceGetTest, ResourceGetForValidUri)
    {
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
        EXPECT_EQ(OC_STACK_OK, resource->get(OC::QueryParamsMap(), &onGetPut));

    }

    TEST(ResourceGetTest, ResourceGetForBadUri)
    {
        OCResource::Ptr resource = ConstructResourceObject("", "coap://192.168.1.2:5000");
        EXPECT_TRUE(resource != NULL);
        EXPECT_THROW(resource->get(OC::QueryParamsMap(), &onGetPut), OC::OCException);
    }

    TEST(ResourceGetTest, ResourceGetWithHighQos)
    {
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
        EXPECT_EQ(OC_STACK_OK,
                resource->get(OC::QueryParamsMap(), &onGetPut, QualityOfService::HighQos));
    }

    TEST(ResourceGetTest, ResourceGetWithLowQos)
    {
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
        EXPECT_EQ(OC_STACK_OK,
                resource->get(OC::QueryParamsMap(), &onGetPut, QualityOfService::LowQos));
    }

    TEST(ResourceGetTest, ResourceGetWithMidQos)
    {
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
        EXPECT_EQ(OC_STACK_OK,
                resource->get(OC::QueryParamsMap(), &onGetPut, QualityOfService::MidQos));
    }

    TEST(ResourceGetTest, ResourceGetWithNaQos)
    {
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
        EXPECT_EQ(OC_STACK_OK,
                resource->get(OC::QueryParamsMap(), &onGetPut, QualityOfService::NaQos));
    }

    TEST(ResourceGetTest, ResourceGetWithRTIFNaQos)
    {
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
        EXPECT_EQ(OC_STACK_OK,
                resource->get("", DEFAULT_INTERFACE, QueryParamsMap(), &onGetPut,
                        QualityOfService::NaQos));
    }

    TEST(ResourceGetTest, ResourceGetWithNullResourceType)
    {
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
        EXPECT_ANY_THROW(
                resource->get(nullptr, DEFAULT_INTERFACE, QueryParamsMap(), &onGetPut,
                        QualityOfService::NaQos));
    }


    TEST(ResourceGetTest, ResourceGetWithNullResourceInterface)
    {
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
        EXPECT_ANY_THROW(
                resource->get("", nullptr, QueryParamsMap(), &onGetPut, QualityOfService::NaQos));
    }

    TEST(ResourceGetTest, ResourceGetWithTypeAndInterface)
    {
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
        EXPECT_EQ(OC_STACK_OK, resource->get("", DEFAULT_INTERFACE, QueryParamsMap(), &onGetPut));
    }

    //Post Test
    TEST(ResourcePostTest, ResourcePostValidConfiguration)
    {
        OCRepresentation rep;
        QueryParamsMap query = {};
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
        EXPECT_EQ(OC_STACK_OK, resource->post(rep, query, &onGetPut));
    }

    TEST(ResourcePostTest, ResourcePostWithNaQos)
    {
        OCRepresentation rep;
        QueryParamsMap query = {};
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
        EXPECT_EQ(OC_STACK_OK, resource->post(rep, query, &onGetPut, QualityOfService::NaQos));
    }

    TEST(ResourcePostTest, ResourcePostWithMidQos)
    {
        OCRepresentation rep;
        QueryParamsMap query = {};
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
        EXPECT_EQ(OC_STACK_OK, resource->post(rep, query, &onGetPut, QualityOfService::MidQos));
    }

    TEST(ResourcePostTest, ResourcePostWithLowQos)
    {
        OCRepresentation rep;
        QueryParamsMap query = {};
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
        EXPECT_EQ(OC_STACK_OK, resource->post(rep, query, &onGetPut, QualityOfService::LowQos));
    }

    TEST(ResourcePostTest, ResourcePostWithHighQos)
    {
        OCRepresentation rep;
        QueryParamsMap query = {};
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
        EXPECT_EQ(OC_STACK_OK, resource->post(rep, query, &onGetPut, QualityOfService::HighQos));
    }

    //Put Test
    TEST(ResourcePutTest, ResourcePutForValid)
    {
        OCRepresentation rep;
        QueryParamsMap query = {};
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
        EXPECT_EQ(OC_STACK_OK, resource->put(rep, query, &onGetPut));
    }

    TEST(ResourcePutTest, ResourcePutWithNaQos)
    {
        OCRepresentation rep;
        QueryParamsMap query = {};
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
        EXPECT_EQ(OC_STACK_OK, resource->put(rep, query, &onGetPut, QualityOfService::NaQos));
    }

    TEST(ResourcePutTest, ResourcePutWithLowQos)
    {
        OCRepresentation rep;
        QueryParamsMap query = {};
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
        EXPECT_EQ(OC_STACK_OK, resource->put(rep, query, &onGetPut, QualityOfService::LowQos));
    }

    TEST(ResourcePutTest, ResourcePutWithMidQos)
    {
        OCRepresentation rep;
        QueryParamsMap query = {};
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
        EXPECT_EQ(OC_STACK_OK, resource->put(rep, query, &onGetPut, QualityOfService::MidQos));
    }

    TEST(ResourcePutTest, ResourcePutWithHighQos)
    {
        OCRepresentation rep;
        QueryParamsMap query = {};
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
        EXPECT_EQ(OC_STACK_OK, resource->put(rep, query, &onGetPut, QualityOfService::HighQos));
    }

    TEST(ResourcePutTest, ResourcePutWithTypeAndInterface)
    {
        OCRepresentation rep;
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
        EXPECT_EQ(OC_STACK_OK,
                resource->put("", DEFAULT_INTERFACE, rep, QueryParamsMap(), &onGetPut));
    }

    TEST(ResourcePutTest, ResourcePutWithNullType)
    {
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
        OCRepresentation rep;
        EXPECT_ANY_THROW(
                resource->put(nullptr, DEFAULT_INTERFACE, rep, QueryParamsMap(), &onGetPut));
        HeaderOptions headerOptions = {};
        onGetPut(headerOptions, rep, OC_STACK_OK);
    }

    TEST(ResourcePutTest, ResourcePutWithNullInterface)
    {
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
        OCRepresentation rep;
        EXPECT_ANY_THROW(resource->put("", nullptr, rep, QueryParamsMap(), &onGetPut));
    }

    // Observer Test
    TEST(ResourceObserveTest, ResourceObserveValidUri)
    {
        OCResource::Ptr resource =
                ConstructResourceObject("coap://192.168.1.2:5000", "/Observe");
        EXPECT_TRUE(resource != NULL);
        QueryParamsMap query = {};
        EXPECT_EQ(OC_STACK_OK, resource->observe(ObserveType::ObserveAll, query, &onObserve));
    }

    TEST(ResourceObserveTest, ResourceObserveLoQos)
    {
        QueryParamsMap query = {};
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/Observe");
        EXPECT_TRUE(resource != NULL);
        EXPECT_EQ(OC_STACK_OK,
                resource->observe(ObserveType::ObserveAll, query, &onObserve,
                        QualityOfService::LowQos));
    }

    TEST(ResourceObserveTest, ResourceObserveNaQos)
    {
        QueryParamsMap query = {};
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/Observe");
        EXPECT_TRUE(resource != NULL);
        EXPECT_EQ(OC_STACK_OK,
                resource->observe(ObserveType::ObserveAll, query, &onObserve,
                        QualityOfService::NaQos));
    }

    TEST(ResourceObserveTest, ResourceObserveMidQos)
    {
        QueryParamsMap query = {};
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/Observe");
        EXPECT_TRUE(resource != NULL);
        EXPECT_EQ(OC_STACK_OK,
                resource->observe(ObserveType::ObserveAll, query, &onObserve,
                        QualityOfService::MidQos));
    }

    TEST(ResourceObserveTest, ResourceObserveHighQos)
    {
        QueryParamsMap query = {};
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/Observe");
        EXPECT_TRUE(resource != NULL);
        EXPECT_EQ(OC_STACK_OK,
                resource->observe(ObserveType::ObserveAll, query, &onObserve,
                        QualityOfService::HighQos));
    }

    TEST(ResourceObserveTest, ResourceCancelObserveValidUri)
    {
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
        QueryParamsMap query = {};
        EXPECT_EQ(OC_STACK_OK, resource->observe(ObserveType::ObserveAll, query, &onObserve));
        EXPECT_EQ(OC_STACK_OK, resource->cancelObserve());
    }

    TEST(ResourceObserveTest, ResourceCancelObserveWithNaQos)
    {
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
        QueryParamsMap query = {};
        EXPECT_EQ(OC_STACK_OK,
                resource->observe(ObserveType::ObserveAll, query, &onObserve,
                        QualityOfService::NaQos));
        EXPECT_EQ(OC_STACK_OK, resource->cancelObserve(QualityOfService::NaQos));
    }

    TEST(ResourceObserveTest, ResourceCancelObserveWithLowQos)
    {
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
        QueryParamsMap query = {};
        EXPECT_EQ(OC_STACK_OK,
                resource->observe(ObserveType::ObserveAll, query, &onObserve,
                        QualityOfService::LowQos));
        EXPECT_EQ(OC_STACK_OK, resource->cancelObserve(QualityOfService::LowQos));
    }

    TEST(ResourceObserveTest, ResourceCancelObserveWithMidQos)
    {
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
            QueryParamsMap query = {};
        EXPECT_EQ(OC_STACK_OK,
                resource->observe(ObserveType::ObserveAll, query, &onObserve,
                        QualityOfService::MidQos));
        EXPECT_EQ(OC_STACK_OK, resource->cancelObserve(QualityOfService::MidQos));
    }

    TEST(ResourceObserveTest, ResourceCancelObserveWithHighQos)
    {
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
            QueryParamsMap query = {};
        EXPECT_EQ(OC_STACK_OK,
                resource->observe(ObserveType::ObserveAll, query, &onObserve,
                        QualityOfService::HighQos));
        EXPECT_EQ(OC_STACK_OK, resource->cancelObserve(QualityOfService::HighQos));
    }

    //DeleteResource
    TEST(DeleteResourceTest, DeleteResourceValidUri)
    {
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
        EXPECT_EQ(OC_STACK_OK, resource->deleteResource(&deleteResponse));
    }

    TEST(DeleteResourceTest, DeleteResourceWithLowQos)
    {
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
        EXPECT_EQ(OC_STACK_OK,
                resource->deleteResource(&deleteResponse, QualityOfService::LowQos));
    }

    TEST(DeleteResourceTest, DeleteResourceWithMidQos)
    {
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
        EXPECT_EQ(OC_STACK_OK,
                resource->deleteResource(&deleteResponse, QualityOfService::MidQos));
    }

    TEST(DeleteResourceTest, DeleteResourceWithHighQos)
    {
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
        EXPECT_EQ(OC_STACK_OK,
                resource->deleteResource(&deleteResponse, QualityOfService::HighQos));
    }
    TEST(DeleteResourceTest, DeleteResourceWithNaQos)
    {
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
        EXPECT_EQ(OC_STACK_OK, resource->deleteResource(&deleteResponse, QualityOfService::NaQos));
    }

    //GetResourceInterfaces Test
    TEST(GetResourceInterfacesTest, GetResourceInterfaces)
    {
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
        EXPECT_TRUE(resource->getResourceInterfaces().front() == DEFAULT_INTERFACE);
    }

    //GetResourceTypes Test
    TEST(GetResourceTypesTest, GetResourceTypes)
    {
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
        EXPECT_TRUE(resource->getResourceTypes().front() == "intel.rpost");
    }

    // Host Test
    TEST(HostTest, Host)
    {
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
        EXPECT_TRUE(resource->host() == "coap://192.168.1.2:5000");
    }

    //Uri Test
    TEST(UriTest, Uri)
    {
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
        EXPECT_TRUE(resource->uri() == "/resource");
    }

    //ConnectivityType Test
    TEST(ConnectivityTypeTest, ConnectivityType)
    {
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
        EXPECT_TRUE(resource->connectivityType() == OC_WIFI);
    }

    //IsObservable Test
    TEST(IsObservableTest, isObservable)
    {
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
        EXPECT_FALSE(resource->isObservable());
    }

    //SID Test
    TEST(SidTest, sid)
    {
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
        EXPECT_TRUE(resource->sid() == "0");
    }

    //UniqueIdentifier Test
    TEST(UniqueIdentifierTest, uniqueIdentifier)
    {
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
        std::ostringstream ss;
        ss << resource->uniqueIdentifier();
        EXPECT_TRUE(ss.str() == "0/resource");
    }

    // HeaderOptions Test
    TEST(HeaderOptionsTest, SetHeaderOptionsValidInput)
    {
        const uint16_t API_VERSION = 2048;
        const uint16_t TOKEN = 3000;
        HeaderOptions headerOptions;
        HeaderOption::OCHeaderOption apiVersion(API_VERSION, "v.1.0");
        HeaderOption::OCHeaderOption clientToken(TOKEN, "21ae43gf");
        headerOptions.push_back(apiVersion);
        headerOptions.push_back(clientToken);

        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
        EXPECT_NO_THROW(resource->setHeaderOptions(headerOptions));
        EXPECT_NO_THROW(resource->unsetHeaderOptions());
    }

    TEST(HeaderOptionsTest, SetHeaderOptionsEmptyInput)
    {
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
        HeaderOptions headerOptions = {};
        EXPECT_NO_THROW(resource->setHeaderOptions(headerOptions));
        EXPECT_NO_THROW(resource->unsetHeaderOptions());
    }
}

