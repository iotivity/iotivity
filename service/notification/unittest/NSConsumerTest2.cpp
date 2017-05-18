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

#include <list>
#include <condition_variable>
#include <mutex>
#include <chrono>

#include "OCPlatform.h"
#include "octypes.h"
#include "ocstack.h"
#include "ocpayload.h"
#include "cainterface.h"

#include "NSCommon.h"
#include "NSConsumerInterface.h"
#include "NSConsumerDiscovery.h"
#include "NSConsumerCommunication.h"
#include "NSConsumerInternalTaskController.h"

#define NS_UUID_STRING_SIZE 37

namespace
{
    NSProvider * g_provider = NULL;
    NSProvider_internal * g_provider_internal = NULL;

    OCClientResponse * g_testResponse = NULL;
    OCDevAddr * testAddr = NULL;

    uint64_t revId = 0;
    NSSyncType type = NS_SYNC_DELETED;
    NSProviderState revState = NS_STOPPED;
    NSProviderState expectedState = NS_STOPPED;

    NSConsumerConfig cfg;

    std::chrono::milliseconds g_waitForResponse(1000);

    std::condition_variable messageReceive;
    std::mutex messageReceiveLock;

    std::condition_variable syncReceive;
    std::mutex syncReceiveLock;

    std::condition_variable providerChanged;
    std::mutex providerChangedLock;

    const std::string testProviderID = "123456789012345678901234567890123456";

    static void NSNotificationReceivedCallback(NSMessage * message)
    {
        if (0 != testProviderID.compare(message->providerId))
        {
            NSRemoveMessage(message);
            return;
        }

        std::cout << __func__ << std::endl;
        std::cout << "Income Notification : " << message->messageId << std::endl;
        revId = message->messageId;

        NSRemoveMessage(message);
        messageReceive.notify_all();
    }

    static void NSSyncCallback(NSSyncInfo * sync)
    {
        if (0 != testProviderID.compare(sync->providerId))
        {
            free(sync);
            return;
        }

        std::cout << __func__ << std::endl;
        std::cout << "Income SyncInfo : " << sync->messageId
                << ", State : " << sync->state << std::endl;
        type = sync->state;

        free(sync);
        syncReceive.notify_all();
    }

    static void NSProviderChangedCallback(NSProvider * provider,  NSProviderState state)
    {
        if (0 != testProviderID.compare(provider->providerId))
        {
            NSRemoveProvider(provider);
            return;
        }

        std::cout << __func__ << " " << state << std::endl;
        std::cout << provider->providerId << std::endl;
        if (g_provider)
        {
            NSRemoveProvider(g_provider);
            g_provider = NULL;
        }
        g_provider = provider;

        if (expectedState == state)
        {
            revState = state;
            providerChanged.notify_all();
        }
    }
    static FILE* client_open(const char * path, const char * mode)
    {
        if (0 == strcmp(path, OC_SECURITY_DB_DAT_FILE_NAME))
        {
            std::string file_name = "./oic_svr_db_ns.dat";
            return fopen(file_name.c_str(), mode);
        }
        else
        {
            return fopen(path, mode);
        }
    }

    OCDevAddr * getDevAddr()
    {
        CAEndpoint_t * ninfo = NULL;
        size_t size = 0;
        CAResult_t ret = CAGetNetworkInformation(&ninfo, &size);
        if (ret != CA_STATUS_OK || size == 0)
        {
            return NULL;
        }

        OCDevAddr * addr =
                (OCDevAddr *)malloc(sizeof(OCDevAddr));
        if (addr == NULL)
        {
            free(ninfo);
            return NULL;
        }
        addr->adapter = (ninfo[0].adapter == CA_ALL_ADAPTERS) ? OC_ADAPTER_IP :
                (OCTransportAdapter) ninfo[0].adapter;
        strncpy(addr->addr, ninfo[0].addr, sizeof(ninfo[0].addr));
        addr->flags = (OCTransportFlags)ninfo[0].flags;
        addr->ifindex = ninfo[0].ifindex;
        addr->port = ninfo[0].port;
        strncpy(addr->remoteId, ninfo[0].remoteId, 37);

        free(ninfo);
        ninfo = NULL;
        return addr;
    }

    OCRepPayload * getDiscoverPayload()
    {
        OCRepPayload * payload = OCRepPayloadCreate();
        EXPECT_NE((void *)NULL, payload);

        std::string msgUri = "/notification/message";
        std::string syncUri = "/notification/sync";

        bool getResult = OCRepPayloadSetPropBool(payload, NS_ATTRIBUTE_POLICY, false);
        getResult &= OCRepPayloadSetPropString(payload, NS_ATTRIBUTE_PROVIDER_ID, testProviderID.c_str());
        getResult &= OCRepPayloadSetPropString(payload, NS_ATTRIBUTE_MESSAGE, msgUri.c_str());
        getResult &= OCRepPayloadSetPropString(payload, NS_ATTRIBUTE_SYNC, syncUri.c_str());
        if (getResult == false)
        {
            OCRepPayloadDestroy(payload);
        }
        EXPECT_EQ(true, getResult);

        return payload;
    }

    OCRepPayload * getMsgPayload(uint64_t msgId)
    {
        OCRepPayload * payload = OCRepPayloadCreate();
        EXPECT_NE((void *)NULL, payload);
        bool getResult = OCRepPayloadSetPropInt(payload, NS_ATTRIBUTE_MESSAGE_ID, msgId);
        getResult &= OCRepPayloadSetPropString(payload, NS_ATTRIBUTE_PROVIDER_ID, testProviderID.c_str());
        if (getResult == false)
        {
            OCRepPayloadDestroy(payload);
        }
        EXPECT_EQ(true, getResult);

        return payload;
    }

    OCRepPayload * getSyncPayload(uint64_t msgId, int64_t state)
    {
        OCRepPayload * payload = OCRepPayloadCreate();
        EXPECT_NE((void *)NULL, payload);
        bool getResult = OCRepPayloadSetPropInt(payload, NS_ATTRIBUTE_MESSAGE_ID, msgId);
        getResult &= OCRepPayloadSetPropString(payload, NS_ATTRIBUTE_PROVIDER_ID, testProviderID.c_str());
        getResult &= OCRepPayloadSetPropInt(payload, NS_ATTRIBUTE_STATE, state);
        if (getResult == false)
        {
            OCRepPayloadDestroy(payload);
        }
        EXPECT_EQ(true, getResult);

        return payload;
    }

    OCRepPayload * getTopicPayload()
    {
        size_t dimensions[MAX_REP_ARRAY_DEPTH] = {3, 0, 0};
        OCRepPayload ** topicList = (OCRepPayload **)malloc(sizeof(OCRepPayload *)*3);
        EXPECT_NE((void *)NULL, topicList);
        for (int i = 0; i < 3; ++i)
        {
            topicList[i] = OCRepPayloadCreate();
            EXPECT_NE((void *)NULL, topicList[i]);

            bool getResult = OCRepPayloadSetPropInt(topicList[i],
                            NS_ATTRIBUTE_TOPIC_SELECTION, 1);
            getResult &= OCRepPayloadSetPropString(topicList[i],
                            NS_ATTRIBUTE_TOPIC_NAME, std::to_string(i+1).c_str());
            EXPECT_EQ(true, getResult);
        }

        OCRepPayload * payload = OCRepPayloadCreate();
        EXPECT_NE((void *)NULL, payload);
        bool getResult = OCRepPayloadSetPropString(payload,
                        NS_ATTRIBUTE_CONSUMER_ID, testProviderID.c_str());
        getResult &= OCRepPayloadSetPropObjectArrayAsOwner(payload,
                        NS_ATTRIBUTE_TOPIC_LIST, topicList, dimensions);
        if (getResult == false)
        {
            OCRepPayloadDestroy(payload);
        }
        EXPECT_EQ(true, getResult);

        return payload;
    }

    OCClientResponse * createResponse()
    {
        OCClientResponse * testResponse =
                (OCClientResponse *)malloc(sizeof(OCClientResponse));
        EXPECT_NE((void *)NULL, testResponse);

        std::string notiUri = "/notifiationTest";

        testResponse->addr = testAddr;
        testResponse->devAddr = *testAddr;
        testResponse->connType = CT_ADAPTER_IP;
        testResponse->identity.id_length = 37;
        strncpy((char *)(testResponse->identity.id), testProviderID.c_str(), 37);
        testResponse->numRcvdVendorSpecificHeaderOptions = 0;
        testResponse->resourceUri = (char*)malloc(sizeof(char)*notiUri.size() + 1);
        strncpy((char*)testResponse->resourceUri, notiUri.c_str(), notiUri.size()+1);
        testResponse->result = OC_STACK_OK;
        testResponse->sequenceNumber = 1;
        testResponse->payload = NULL;

        return testResponse;
    }

    NSProvider_internal * getProvider(OCDevAddr * addr)
    {
        if (addr == NULL)
        {
            return NULL;
        }

        NSProviderConnectionInfo * info =
                (NSProviderConnectionInfo *)malloc(sizeof(NSProviderConnectionInfo));
        if (info == NULL)
        {
            free(addr);
            return NULL;
        }
        info->isCloudConnection = false;
        info->isSubscribing = false;
        info->messageHandle = NULL;
        info->syncHandle = NULL;
        info->next = NULL;
        info->addr = addr;

        NSProvider_internal * provider =
                (NSProvider_internal *)malloc(sizeof(NSProvider_internal));
        if (provider == NULL)
        {
            free(addr);
            free(info);
            return NULL;
        }
        provider->accessPolicy = NSSelector::NS_SELECTION_CONSUMER;
        provider->state = NS_DISCOVERED;
        strncpy(provider->providerId, testProviderID.c_str(), NS_UUID_STRING_SIZE);
        provider->messageUri = strdup("/notificationTest/message");
        provider->syncUri = strdup("/notificationTest/sync");
        provider->topicUri = strdup("/notificationTest/topic");
        provider->topicLL = NULL;
        provider->connection = info;

        return provider;
    }

    void stackInit()
    {
        static OCPersistentStorage gps {client_open, fread, fwrite, fclose, unlink };
        OC::PlatformConfig occfg
        {
            OC::ServiceType::InProc,
            OC::ModeType::Both,
            "0.0.0.0",
            0,
            OC::QualityOfService::LowQos,
            &gps
        };
        OC::OCPlatform::Configure(occfg);

        try
        {
            OC::OCPlatform::stopPresence();
        }
        catch (...)
        {

        }

        testAddr = getDevAddr();
        if (testAddr == NULL)
        {
            throw std::exception();
        }
    }
    void stackTearDown()
    {
        if (testAddr)
        {
            free(testAddr);
            testAddr = NULL;
        }
        if (g_provider)
        {
            NSRemoveProvider(g_provider);
            g_provider = NULL;
        }
    }

    void removeGlobalResponse()
    {
        if (g_testResponse)
        {
            free((void*)(g_testResponse->resourceUri));
            free(g_testResponse);
            g_testResponse = NULL;
        }
    }
}

TEST(NotificationConsumerTest, StartConsumerNegativeNonSetChangedCB)
{
    cfg.changedCb = NULL;
    cfg.messageCb = NSNotificationReceivedCallback;
    cfg.syncInfoCb = NSSyncCallback;

    EXPECT_EQ(NS_ERROR, NSStartConsumer(cfg));
}

TEST(NotificationConsumerTest, StartConsumerNegativeNonSetNotiReceiveCB)
{
    cfg.changedCb = NSProviderChangedCallback;
    cfg.messageCb = NULL;
    cfg.syncInfoCb = NSSyncCallback;

    EXPECT_EQ(NS_ERROR, NSStartConsumer(cfg));
}

TEST(NotificationConsumerTest, StartConsumerNegativeNonSetSyncCB)
{
    cfg.changedCb = NSProviderChangedCallback;
    cfg.messageCb = NSNotificationReceivedCallback;
    cfg.syncInfoCb = NULL;

    EXPECT_EQ(NS_ERROR, NSStartConsumer(cfg));
}

TEST(NotificationConsumerTest, StartConsumerPositive)
{

    EXPECT_NO_THROW(stackInit());

    cfg.changedCb = NSProviderChangedCallback;
    cfg.messageCb = NSNotificationReceivedCallback;
    cfg.syncInfoCb = NSSyncCallback;

    EXPECT_EQ(NS_OK, NSStartConsumer(cfg));
}

TEST(NotificationConsumerTest, StopConsumerPositive)
{
    EXPECT_EQ(NSStopConsumer(), NS_OK);
}

TEST(NotificationConsumerTest, StopConsumerNegative)
{
    EXPECT_EQ(NSStopConsumer(), NS_ERROR);
}

TEST(NotificationConsumerTest, ExpectCallbackDiscovered)
{
    cfg.changedCb = NSProviderChangedCallback;
    cfg.messageCb = NSNotificationReceivedCallback;
    cfg.syncInfoCb = NSSyncCallback;

    NSStartConsumer(cfg);

    revState = NS_STOPPED;
    expectedState = NS_DISCOVERED;

    g_testResponse = createResponse();
    g_testResponse->payload = (OCPayload *)getDiscoverPayload();
    NSIntrospectProvider(NULL, NULL, g_testResponse);

    std::unique_lock< std::mutex > lock{ providerChangedLock };
    providerChanged.wait_for(lock, g_waitForResponse);

    OCRepPayloadDestroy((OCRepPayload *)g_testResponse->payload);
    g_testResponse->payload = NULL;
    removeGlobalResponse();

    EXPECT_EQ(NS_DISCOVERED, revState);
}

TEST(NotificationConsumerTest, ExpectCallbackAllow)
{
    revState = NS_STOPPED;
    expectedState = NS_ALLOW;

    g_testResponse = createResponse();
    g_testResponse->payload = (OCPayload *)getMsgPayload(NS_ALLOW);
    NSConsumerMessageListener(NULL,NULL, g_testResponse);

    std::unique_lock< std::mutex > lock{ providerChangedLock };
    providerChanged.wait_for(lock, g_waitForResponse);

    OCRepPayloadDestroy((OCRepPayload *)g_testResponse->payload);
    g_testResponse->payload = NULL;
    removeGlobalResponse();

    EXPECT_EQ(NS_ALLOW, revState);
}

TEST(NotificationConsumerTest, ExpectReceiveNotification)
{
    uint64_t id = 100;

    g_testResponse = createResponse();
    g_testResponse->payload = (OCPayload *)getMsgPayload(id);
    NSConsumerMessageListener(NULL,NULL, g_testResponse);

    std::unique_lock< std::mutex > lock{ messageReceiveLock };
    messageReceive.wait_for(lock, g_waitForResponse);

    OCRepPayloadDestroy((OCRepPayload *)g_testResponse->payload);
    g_testResponse->payload = NULL;
    removeGlobalResponse();

    EXPECT_EQ(id, revId);
}

TEST(NotificationConsumerTest, ExpectReceiveSyncInfo)
{
    uint64_t id = 100;
    type = NS_SYNC_DELETED;

    g_testResponse = createResponse();
    g_testResponse->payload = (OCPayload *)getSyncPayload(id, NS_SYNC_READ);
    NSConsumerSyncInfoListener(NULL,NULL, g_testResponse);

    std::unique_lock< std::mutex > lock{ syncReceiveLock };
    syncReceive.wait_for(lock, g_waitForResponse);

    OCRepPayloadDestroy((OCRepPayload *)g_testResponse->payload);
    g_testResponse->payload = NULL;
    removeGlobalResponse();

    EXPECT_EQ(NS_SYNC_READ, type);
}

TEST(NotificationConsumerTest, ExpectSuccessSendSyncInfo)
{
    uint64_t id = 100;
    type = NS_SYNC_READ;

    auto ret = NSConsumerSendSyncInfo((g_provider->providerId)+1, id, NS_SYNC_DELETED);

    EXPECT_EQ(NS_OK, ret);
}

TEST(NotificationConsumerTest, GetValidProvider)
{
    NSProvider * provider = NSConsumerGetProvider(g_provider->providerId);
    EXPECT_NE((void *)NULL, provider);
    NSRemoveProvider(provider);
}

TEST(NotificationConsumerTest, GetInvalidProvider)
{
    NSProvider * provider = NSConsumerGetProvider(NULL);
    EXPECT_EQ((void *)NULL, provider);
}

TEST(NotificationConsumerTest, ExpectGetTopicListIsNULL)
{
    NSTopicLL * currentTopics = NSConsumerGetTopicList(g_provider->providerId);
    EXPECT_EQ(NULL, currentTopics);

    if (currentTopics)
    {
        NSRemoveTopicLL(currentTopics);
    }
}

TEST(NotificationConsumerTest, ExpectCallbackTopicUpdated)
{
    revState = NS_STOPPED;

    g_testResponse = createResponse();
    g_testResponse->payload = (OCPayload *)getTopicPayload();
    g_provider_internal = getProvider(testAddr);
    NSIntrospectTopic((void *)g_provider_internal, NULL, g_testResponse);

    expectedState = NS_TOPIC;
    std::unique_lock< std::mutex > lock{ providerChangedLock };
    providerChanged.wait_for(lock, g_waitForResponse);

    EXPECT_EQ(NS_TOPIC, revState);

    OCRepPayloadDestroy((OCRepPayload *)g_testResponse->payload);
    g_testResponse->payload = NULL;
    removeGlobalResponse();

    expectedState = NS_STOPPED;
}

TEST(NotificationConsumerTest, ExpectEQTopicList)
{
    bool isSame = true;

    typedef std::list<std::string> TesttopicList;
    TesttopicList topics;
    topics.push_back("1");
    topics.push_back("2");
    topics.push_back("3");

    NSTopicLL * retTopic = NSConsumerGetTopicList(g_provider_internal->providerId);
    EXPECT_NE((void *)NULL, (void *)retTopic);

    NSTopicLL * iter = retTopic;
    std::for_each (topics.begin(), topics.end(),
            [this, & iter, & isSame](const std::string & str)
            {
                isSame &= (str == std::string(iter->topicName));
                iter = iter->next;
            });

    NSRemoveTopicLL(retTopic);

    EXPECT_EQ(true, isSame);
}

TEST(NotificationConsumerTest, ExpectSuccessUpdateTopicOnConsumer)
{
    NSTopicLL * retTopic = NSConsumerGetTopicList(g_provider_internal->providerId);

    NSTopicLL * iter = retTopic;
    for (; iter; iter = iter->next)
    {
        iter->state = NS_TOPIC_SUBSCRIBED;
    }
    NSResult ret = NSConsumerUpdateTopicList(g_provider_internal->providerId, retTopic);

    NSRemoveTopicLL(retTopic);

    EXPECT_EQ(NS_OK, ret);
}

TEST(NotificationConsumerTest, ExpectUnsubscribeSuccess)
{
    revState = NS_DENY;
    NSResult ret = NSUnsubscribe(g_provider_internal->providerId);

    expectedState = NS_STOPPED;
    std::unique_lock< std::mutex > lock{ providerChangedLock };
    providerChanged.wait_for(lock, g_waitForResponse);

    EXPECT_EQ(NS_STOPPED, revState);
    EXPECT_EQ(NS_OK, ret);
}

TEST(NotificationConsumerTest, ExpectUnsubscribeWithPresenceStart)
{
    g_testResponse = createResponse();
    OCPresencePayload * payload = OCPresencePayloadCreate(1, 2, OC_PRESENCE_TRIGGER_CREATE, NULL);
    EXPECT_NE((void *)NULL, payload);
    g_testResponse->payload = (OCPayload *)payload;
    g_testResponse->addr = NULL;

    auto ret = NSConsumerPresenceListener(NULL,NULL, g_testResponse);

    EXPECT_EQ(OC_STACK_KEEP_TRANSACTION, ret);
    OCPresencePayloadDestroy(payload);
    removeGlobalResponse();
}

TEST(NotificationConsumerTest, ExpectUnsubscribeWithPresenceStop)
{
    g_testResponse = createResponse();
    OCPresencePayload * payload = OCPresencePayloadCreate(2, 2, OC_PRESENCE_TRIGGER_DELETE, NULL);
    EXPECT_NE((void *)NULL, payload);
    g_testResponse->payload = (OCPayload *)payload;

    auto ret = NSConsumerPresenceListener(NULL,NULL, g_testResponse);

    std::unique_lock< std::mutex > lock{ providerChangedLock };
    providerChanged.wait_for(lock, g_waitForResponse);

    EXPECT_EQ(OC_STACK_KEEP_TRANSACTION, ret);
    OCPresencePayloadDestroy(payload);

    NSStopConsumer();

    stackTearDown();
}
