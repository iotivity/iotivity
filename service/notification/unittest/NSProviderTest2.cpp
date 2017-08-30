//******************************************************************
//
// Copyright 2017 Samsung Electronics All Rights Reserved.
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
#include <iostream>
#include <condition_variable>
#include <mutex>
#include <chrono>

#include "OCPlatform.h"
#include "ocpayload.h"

#include "NSCommon.h"
#include "NSConstants.h"
#include "NSProviderInterface.h"

#include "NSProviderListener.h"
#include "NSUnittestUtil.h"

namespace
{
    std::chrono::milliseconds g_waitForResponse(1000);

    std::condition_variable responseProviderSub;
    std::mutex responseProviderSubLock;

    std::condition_variable responseProviderSync;
    std::mutex responseProviderSyncLock;

    char * g_consumerID = NULL;
    int expectedMsgId;
    int expectedSyncType = NS_SYNC_READ;

    const std::string testConsumerId = "123456789012345678901234567890123457";

    static FILE* server_open(const char * path, const char * mode)
    {
        if (0 == strcmp(path, OC_SECURITY_DB_DAT_FILE_NAME))
        {
            std::string file_name = "./oic_svr_db_ns.dat";
#ifndef LOCAL_RUNNING
            file_name = "./service/notification/unittest/oic_svr_db_ns.dat";
#endif
            return fopen(file_name.c_str(), mode);
        }
        else
        {
            return fopen(path, mode);
        }
    }

    static void NSRequestedSubscribeCallback(NSConsumer * consumer)
    {
        if (g_consumerID)
        {
            free(g_consumerID);
        }
        g_consumerID = strdup(consumer->consumerId);

        responseProviderSub.notify_all();
    }

    static void NSSyncCallback(NSSyncInfo * sync)
    {
        expectedSyncType = sync->state;
        expectedMsgId = sync->messageId;
        free(sync);
        responseProviderSync.notify_all();
    }

    OCEntityHandlerRequest * getEntityRequest(OCMethod method, OCObserveAction action)
    {
        static OCObservationId id = 10;
        OCEntityHandlerRequest * request =
                (OCEntityHandlerRequest *)malloc(sizeof(OCEntityHandlerRequest));
        EXPECT_NE((void *)NULL, request);
        request->requestHandle = NULL;
        request->resource = NULL;

        if (OC_REST_OBSERVE == method)
        {
            request->obsInfo.action = action;
            request->obsInfo.obsId = id++;

            const std::string query = std::string(NS_QUERY_CONSUMER_ID)
                    + "=" + testConsumerId;
            request->query = strdup(query.c_str());
            EXPECT_NE((void *)NULL, request->query);
        }
        request->method = method;
        request->numRcvdVendorSpecificHeaderOptions = 0;
        request->payload = NULL;

        return request;
    }

    OCEntityHandlerRequest * getPostSyncEntityRequest(uint64_t id)
    {
        OCEntityHandlerRequest * request =
                (OCEntityHandlerRequest *)malloc(sizeof(OCEntityHandlerRequest));
        EXPECT_NE((void *)NULL, request);
        request->requestHandle = NULL;
        request->resource = NULL;

        request->method = OC_REST_POST;
        request->numRcvdVendorSpecificHeaderOptions = 0;
        request->query = NULL;

        OCRepPayload * payload = OCRepPayloadCreate();
        EXPECT_NE((void *)NULL, payload);

        bool ret = OCRepPayloadSetPropInt(payload, NS_ATTRIBUTE_MESSAGE_ID, id);
        OCUUIdentity provider;
        OC::OCPlatform::getDeviceId(&provider);

        char providerId[UUID_IDENTITY_SIZE] = {0,};
        OICStrcpy(providerId, UUID_IDENTITY_SIZE, (const char *)provider.id);

        ret &= OCRepPayloadSetPropString(payload,
                     NS_ATTRIBUTE_PROVIDER_ID, (const char*)providerId);
        ret &= OCRepPayloadSetPropInt(payload, NS_ATTRIBUTE_STATE, NS_SYNC_READ);
        EXPECT_EQ(true, ret);

        request->payload = (OCPayload *) payload;

        return request;
    }

    void stackInit()
    {
        static OCPersistentStorage gps {server_open, fread, fwrite, fclose, unlink };

        OC::PlatformConfig cfg
        {
            OC::ServiceType::InProc,
            OC::ModeType::Both,
            "0.0.0.0",
            0,
            OC::QualityOfService::HighQos,
            &gps
        };
        OC::OCPlatform::Configure(cfg);

        try
        {
            OC::OCPlatform::stopPresence();
        }
        catch (...)
        {

        }
    }
}

TEST(NotificationProviderTest, StartProviderPositiveWithNSPolicyTrue)
{
    stackInit();

    NSProviderConfig config;
    config.subRequestCallback = NSRequestedSubscribeCallback;
    config.syncInfoCallback = NSSyncCallback;
    config.subControllability = true;
    config.userInfo = strdup("user1");
    config.resourceSecurity = false;

    NSResult ret = NSStartProvider(config);

    EXPECT_EQ(ret, NS_OK);
    free(config.userInfo);
    config.userInfo = NULL;
}

TEST(NotificationProviderTest, StopProviderPositive)
{
    NSResult ret = NSStopProvider();

    EXPECT_EQ(ret, NS_OK);
}

TEST(NotificationProviderTest, ExpectCallbackSubscribeRequestWithAccepterProvider)
{
    NSProviderConfig config;
    config.subRequestCallback = NSRequestedSubscribeCallback;
    config.syncInfoCallback = NSSyncCallback;
    config.subControllability = true;
    config.userInfo = NULL;
    config.resourceSecurity = false;

    NSStartProvider(config);

    OCEntityHandlerFlag flag = OC_OBSERVE_FLAG;
    OCEntityHandlerRequest * msgRequest = getEntityRequest(OC_REST_OBSERVE, OC_OBSERVE_REGISTER);
    NSEntityHandlerMessageCb(flag, msgRequest, NULL);
    {
        std::unique_lock< std::mutex > lock{ responseProviderSubLock };
        responseProviderSub.wait_for(lock, g_waitForResponse);
    }
    free(msgRequest->query);
    free(msgRequest);

    OCEntityHandlerRequest * syncRequest = getEntityRequest(OC_REST_OBSERVE, OC_OBSERVE_REGISTER);
    NSEntityHandlerSyncCb(flag, syncRequest, NULL);
    {
        std::unique_lock< std::mutex > lock{ responseProviderSubLock };
        responseProviderSub.wait_for(lock, g_waitForResponse);
    }

    free(syncRequest->query);
    free(syncRequest);

    EXPECT_NE((void *)NULL, g_consumerID);
}

TEST(NotificationProviderTest, ExpectSuccessSendAllow)
{
    auto ret = NSAcceptSubscription(g_consumerID, true);
    EXPECT_EQ(NS_OK, ret);
}

TEST(NotificationProviderTest, ExpectFailGetRequestForNotificationWithInvalidInterface)
{
    OCEntityHandlerFlag flag = OC_REQUEST_FLAG;
    OCEntityHandlerRequest * getRequest = getEntityRequest(OC_REST_GET, OC_OBSERVE_NO_OPTION);
    const std::string query = std::string("if=") + "test.invalid";
    getRequest->query = strdup(query.c_str());
    auto ret = NSEntityHandlerNotificationCb(flag, getRequest, NULL);

    EXPECT_NE(ret, OC_EH_OK);

    free(getRequest->query);
    free(getRequest);
}

TEST(NotificationProviderTest, ExpectFailGetRequestForMsgWithInvalidInterface)
{
    OCEntityHandlerFlag flag = OC_REQUEST_FLAG;
    OCEntityHandlerRequest * getRequest = getEntityRequest(OC_REST_GET, OC_OBSERVE_NO_OPTION);
    const std::string query = std::string("if=") + "test.invalid";
    getRequest->query = strdup(query.c_str());
    auto ret = NSEntityHandlerMessageCb(flag, getRequest, NULL);

    EXPECT_NE(ret, OC_EH_OK);

    free(getRequest->query);
    free(getRequest);
}

TEST(NotificationProviderTest, ExpectFailGetRequestForSyncWithInvalidInterface)
{
    OCEntityHandlerFlag flag = OC_REQUEST_FLAG;
    OCEntityHandlerRequest * getRequest = getEntityRequest(OC_REST_GET, OC_OBSERVE_NO_OPTION);
    const std::string query = std::string("if=") + "test.invalid";
    getRequest->query = strdup(query.c_str());
    auto ret = NSEntityHandlerSyncCb(flag, getRequest, NULL);

    EXPECT_NE(ret, OC_EH_OK);

    free(getRequest->query);
    free(getRequest);
}

TEST(NotificationProviderTest, ExpectFailGetRequestForTopicWithInvalidInterface)
{
    OCEntityHandlerFlag flag = OC_REQUEST_FLAG;
    OCEntityHandlerRequest * getRequest = getEntityRequest(OC_REST_GET, OC_OBSERVE_NO_OPTION);
    const std::string query = std::string("if=") + "test.invalid";
    getRequest->query = strdup(query.c_str());
    auto ret = NSEntityHandlerTopicCb(flag, getRequest, NULL);

    EXPECT_NE(ret, OC_EH_OK);

    free(getRequest->query);
    free(getRequest);
}

TEST(NotificationProviderTest, ExpectSuccessGetRequestForTopicWithInvalidInterface)
{
    OCEntityHandlerFlag flag = OC_REQUEST_FLAG;
    OCEntityHandlerRequest * getRequest = getEntityRequest(OC_REST_GET, OC_OBSERVE_NO_OPTION);
    const std::string query = std::string("if=") + NS_INTERFACE_BASELINE;
    getRequest->query = strdup(query.c_str());
    auto ret = NSEntityHandlerTopicCb(flag, getRequest, NULL);

    EXPECT_EQ(ret, OC_EH_OK);

    free(getRequest->query);
    free(getRequest);
}

TEST(NotificationProviderTest, ExpectCallbackReceiveSync)
{
    int id = 100;
    int type = NS_SYNC_READ;

    OCEntityHandlerFlag flag = OC_REQUEST_FLAG;
    auto request = getPostSyncEntityRequest(id);
    NSEntityHandlerSyncCb(flag, request, NULL);

    std::unique_lock< std::mutex > lock{ responseProviderSyncLock };
    responseProviderSync.wait_for(lock, g_waitForResponse);

    EXPECT_EQ(expectedMsgId, id);
    EXPECT_EQ(expectedSyncType, type);

    OCRepPayloadDestroy((OCRepPayload *)request->payload);
    free(request);
}

TEST(NotificationProviderTest, ExpectSuccessSetTopics)
{
    std::string str("TEST1");
    std::string str2("TEST2");
    auto ret = NSProviderRegisterTopic(str.c_str());
    EXPECT_EQ(NS_OK, ret);
    ret = NSProviderRegisterTopic(str2.c_str());
    EXPECT_EQ(NS_OK, ret);
}

TEST(NotificationProviderTest, ExpectSuccessUnsetTopics)
{
    std::string str("TEST1");
    std::string str2("TEST2");
    auto ret = NSProviderUnregisterTopic(str.c_str());
    EXPECT_EQ(NS_OK, ret);
    ret = NSProviderUnregisterTopic(str2.c_str());
    EXPECT_EQ(NS_OK, ret);
}

TEST(NotificationProviderTest, ExpectSuccessTopicAllowConsumer)
{
    std::string str("TEST1");
    std::string str2("TEST2");
    auto ret = NSProviderRegisterTopic(str.c_str());
    EXPECT_EQ(NS_OK, ret);
    ret = NSProviderRegisterTopic(str2.c_str());
    EXPECT_EQ(NS_OK, ret);

    ret = NSProviderSetConsumerTopic(g_consumerID, str.c_str());
    EXPECT_EQ(NS_OK, ret);
    ret = NSProviderSetConsumerTopic(g_consumerID, str2.c_str());
    EXPECT_EQ(NS_OK, ret);
}

TEST(NotificationProviderTest, ExpectSuccessTopicDenyConsumer)
{
    std::string str("TEST1");
    std::string str2("TEST2");

    auto ret = NSProviderUnsetConsumerTopic(g_consumerID, str.c_str());
    EXPECT_EQ(NS_OK, ret);
    ret = NSProviderUnsetConsumerTopic(g_consumerID, str2.c_str());
    EXPECT_EQ(NS_OK, ret);
}

TEST(NotificationProviderTest, ExpectEqualUnSetConsumerTopicsAndGetConsumerTopics)
{
    std::string str("TEST1");
    std::string str2("TEST2");
    NSProviderSetConsumerTopic(g_consumerID, str2.c_str());

    bool isSame = false;

    NSTopicLL * topics = NSProviderGetConsumerTopics(g_consumerID);

    if(!topics)
    {
        isSame = false;
    }
    else
    {
        NSTopicLL * firstData = topics;
        NSTopicLL * secondData = firstData->next;

        if(str.compare(firstData->topicName) == 0 && str2.compare(secondData->topicName) == 0
                && ((int)firstData->state) == 0 && ((int)secondData->state) == 1)
        {
            isSame = true;
        }
    }

    removeTopics(topics);

    NSProviderUnregisterTopic(str.c_str());
    NSProviderUnregisterTopic(str2.c_str());
    EXPECT_EQ(isSame, true);
}

TEST(NotificationProviderTest, ExpectSuccessCreateMessage)
{
    auto msg = NSCreateMessage();
    EXPECT_NE((void *)NULL, msg);
    NSFreeMessage(msg);
}

TEST(NotificationProviderTest, ExpectCopyMessage)
{
    auto msg = NSCreateMessage();
    auto copied = NSDuplicateMessage(msg);
    EXPECT_EQ(msg->messageId, copied->messageId);
    NSFreeMessage(msg);
    NSFreeMessage(copied);
}

TEST(NotificationProviderTest, ExpectSuccessSendMessage)
{
    auto msg = NSCreateMessage();
    auto ret = NSSendMessage(msg);
    EXPECT_EQ(NS_OK, ret);
    NSFreeMessage(msg);
}

TEST(NotificationProviderTest, ExpectCopyConsumer)
{
    auto consumer = (NSConsumer *)malloc(sizeof(NSConsumer));
    strncpy(consumer->consumerId, g_consumerID, (size_t)NS_UUID_STRING_SIZE);

    auto copied = NSDuplicateConsumer(consumer);
    EXPECT_EQ(0, strcmp(copied->consumerId, consumer->consumerId));

    NSFreeConsumer(consumer);
    NSFreeConsumer(copied);
}

TEST(NotificationProviderTest, ExpectFailSendMessageWithNULL)
{
    NSResult ret = NSSendMessage(NULL);
    EXPECT_NE(NS_OK, ret);
}

TEST(NotificationProviderTest, ExpectFailAcceptSubscription)
{
    NSResult result = NS_SUCCESS;
    result = NSAcceptSubscription(NULL, true);
    result = NSAcceptSubscription("\0", true);

    EXPECT_EQ(result, NS_FAIL);
}

TEST(NotificationProviderTest, ExpectFailRegisterTopic)
{
    NSResult result = NS_SUCCESS;
    result = NSProviderRegisterTopic(NULL);
    result = NSProviderRegisterTopic("\0");

    EXPECT_EQ(result, NS_FAIL);
}

TEST(NotificationProviderTest, ExpectFailUnregisterTopic)
{
    NSResult result = NS_SUCCESS;
    result = NSProviderUnregisterTopic(NULL);
    result = NSProviderUnregisterTopic("\0");

    EXPECT_EQ(result, NS_FAIL);
}

TEST(NotificationProviderTest, ExpectFailGetConsumerTopics)
{
    NSTopicLL topic = { NULL, NS_TOPIC_UNSUBSCRIBED, NULL };
    NSTopicLL * topicLL = &topic;

    topicLL = NSProviderGetConsumerTopics(NULL);
    topicLL = NSProviderGetConsumerTopics("\0");

    EXPECT_EQ(topicLL, (NSTopicLL *)NULL);
}

TEST(NotificationProviderTest, ExpectFailSetConsumerTopics)
{
    NSResult result = NS_SUCCESS;
    result = NSProviderSetConsumerTopic(NULL, NULL);
    result = NSProviderSetConsumerTopic(NULL, "\0");
    result = NSProviderSetConsumerTopic("\0", NULL);
    result = NSProviderSetConsumerTopic("\0", "\0");
    result = NSProviderSetConsumerTopic("abc", NULL);
    result = NSProviderSetConsumerTopic(NULL, "abc");
    result = NSProviderSetConsumerTopic("abc", "\0");
    result = NSProviderSetConsumerTopic("\0", "abc");

    EXPECT_EQ(result, NS_FAIL);
}

TEST(NotificationProviderTest, ExpectFailUnsetConsumerTopics)
{
    NSResult result = NS_SUCCESS;
    result = NSProviderUnsetConsumerTopic(NULL, NULL);
    result = NSProviderUnsetConsumerTopic(NULL, "\0");
    result = NSProviderUnsetConsumerTopic("\0", NULL);
    result = NSProviderUnsetConsumerTopic("\0", "\0");
    result = NSProviderUnsetConsumerTopic("abc", NULL);
    result = NSProviderUnsetConsumerTopic(NULL, "abc");
    result = NSProviderUnsetConsumerTopic("abc", "\0");
    result = NSProviderUnsetConsumerTopic("\0", "abc");

    EXPECT_EQ(result, NS_FAIL);
}

TEST(NotificationProviderTest, ExpectSuccessUnsub)
{
    OCEntityHandlerFlag flag = OC_OBSERVE_FLAG;
    OCEntityHandlerRequest * msgRequest =
            getEntityRequest(OC_REST_OBSERVE, OC_OBSERVE_DEREGISTER);
    NSEntityHandlerMessageCb(flag, msgRequest, NULL);
    {
        std::unique_lock< std::mutex > lock{ responseProviderSubLock };
        responseProviderSub.wait_for(lock, g_waitForResponse);
    }
    free(msgRequest->query);
    free(msgRequest);

    OCEntityHandlerRequest * syncRequest =
            getEntityRequest(OC_REST_OBSERVE, OC_OBSERVE_DEREGISTER);
    NSEntityHandlerSyncCb(flag, syncRequest, NULL);
    {
        std::unique_lock< std::mutex > lock{ responseProviderSubLock };
        responseProviderSub.wait_for(lock, g_waitForResponse);
    }
    free(syncRequest->query);
    free(syncRequest);

    NSStopProvider();
}
