/* ****************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/


#include <OCPlatform.h>
#include <OCApi.h>
#include <gtest/gtest.h>
#include <string>
#include <map>

namespace OCMQResourceTest
{
    using namespace OC;

#ifdef MQ_SUBSCRIBER
    void onObserve(const HeaderOptions, const OCRepresentation&, const int&, const int&)
    {
    }
#endif

#ifdef MQ_PUBLISHER
    void onGetPut(const HeaderOptions&, const OCRepresentation& , const int eCode)
    {
    }
#endif

    void foundResource(std::shared_ptr<OCResource>)
    {
    }

    void createdTopic(const HeaderOptions &, const OCRepresentation &, const int,
                      std::shared_ptr<OCResource>)
    {
    }

    //Helper method
    OCResource::Ptr ConstructResourceObject(std::string host, std::string uri)
    {
        OCConnectivityType connectivityType = CT_DEFAULT;
        std::vector<std::string> types = {"tkss.wk"};
        std::vector<std::string> ifaces = {DEFAULT_INTERFACE};

        auto ret = OCPlatform::constructResourceObject(host, uri,
                connectivityType, false, types, ifaces);

        if (!ret)
        {
            ADD_FAILURE() << "ConstructResourceObject result was null";
            return nullptr;
        }

        return ret;
    }

    // Message Queue Test
    TEST(MessageQueueTest, DiscoveryMQTopicsValid)
    {
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
        QueryParamsMap query = {};
        EXPECT_EQ(OC_STACK_OK, resource->discoveryMQTopics(query, &foundResource));
    }

    TEST(MessageQueueTest, CreateMQTopicValid)
    {
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
        OCRepresentation rep;
        QueryParamsMap query = {};
        EXPECT_EQ(OC_STACK_OK, resource->createMQTopic(rep, "/ps/nweTopic", query, &createdTopic));
    }

#ifdef MQ_PUBLISHER
    TEST(MessageQueueTest, PublishMQTopicValid)
    {
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
        OCRepresentation rep;
        QueryParamsMap query = {};
        EXPECT_EQ(OC_STACK_OK, resource->publishMQTopic(rep, query, &onGetPut));
    }
#endif

#ifdef MQ_SUBSCRIBER
    TEST(MessageQueueTest, SubscribeMQTopicValid)
    {
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
        QueryParamsMap query = {};
        EXPECT_EQ(OC_STACK_OK, resource->subscribeMQTopic(ObserveType::Observe, query, &onObserve));
    }

    TEST(MessageQueueTest, RequestMQPublishValid)
    {
        OCResource::Ptr resource = ConstructResourceObject("coap://192.168.1.2:5000", "/resource");
        EXPECT_TRUE(resource != NULL);
        QueryParamsMap query = {};
        EXPECT_EQ(OC_STACK_OK, resource->requestMQPublish(query, &onGetPut));
    }
#endif
}

