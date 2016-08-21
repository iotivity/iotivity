//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
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
#include <atomic>
#include <functional>
#include <condition_variable>
#include <mutex>
#include <chrono>

#include "NSProviderService.h"
#include "NSConsumerServiceSimulator.h"
#include "NSUtils.h"
#include "NSSyncInfo.h"
#include "NSMessage.h"
#include "NSMediaContents.h"

namespace
{
    std::atomic_bool g_isStartedStack(false);

    std::chrono::milliseconds g_waitForResponse(3000);

    std::condition_variable responseCon;
    std::mutex mutexForCondition;

    NSConsumerSimulator g_consumerSimul;
     OIC::Service::NSConsumer * g_consumer;
}

class TestWithMock: public testing::Test
{
public:
    MockRepository mocks;

protected:
    virtual ~TestWithMock() noexcept(noexcept(std::declval<Test>().~Test())) {}

    virtual void TearDown() {
        try
        {
            mocks.VerifyAll();
        }
        catch (...)
        {
            mocks.reset();
            throw;
        }
    }
};

class NotificationProviderServiceTest : public TestWithMock
{
public:
    NotificationProviderServiceTest() = default;
    ~NotificationProviderServiceTest() = default;

    static void ConsumerSubscribedCallbackEmpty(OIC::Service::NSConsumer *)
    {
        std::cout << __func__ << std::endl;
    }

    static void MessageSynchronizedCallbackEmpty(OIC::Service::NSSyncInfo *)
    {
        std::cout << __func__ << std::endl;
    }

    static void MessageCallbackFromConsumerEmpty(
            const int &, const std::string &, const std::string &, const std::string &)
    {
        std::cout << __func__ << std::endl;
    }

    static void SyncCallbackFromConsumerEmpty(int, int)
    {
        std::cout << __func__ << std::endl;
    }

protected:

    void SetUp()
    {
        TestWithMock::SetUp();

        if (g_isStartedStack == false)
        {
            OC::PlatformConfig cfg
            {
                OC::ServiceType::InProc,
                OC::ModeType::Both,
                "0.0.0.0",
                0,
                OC::QualityOfService::HighQos
            };
            OC::OCPlatform::Configure(cfg);

            try
            {
                OC::OCPlatform::stopPresence();
            }
            catch (...)
            {

            }

            g_isStartedStack = true;
        }

    }

    void TearDown()
    {
        TestWithMock::TearDown();
    }

};

TEST_F(NotificationProviderServiceTest, StartProviderPositiveWithPolicyTrue)
{
    OIC::Service::NSProviderService::ProviderConfig config;
    config.m_subscribeRequestCb = ConsumerSubscribedCallbackEmpty;
    config.m_syncInfoCb = MessageSynchronizedCallbackEmpty;
    config.policy = true;

    OIC::Service::NSResult ret =  OIC::Service::NSProviderService::getInstance()->Start(config);

    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);

    EXPECT_EQ(ret,  OIC::Service::NSResult::OK);
}

TEST_F(NotificationProviderServiceTest, StopProviderPositive)
{
     OIC::Service::NSResult ret =  OIC::Service::NSProviderService::getInstance()->Stop();

    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);

    EXPECT_EQ(ret,  OIC::Service::NSResult::OK);
}

TEST_F(NotificationProviderServiceTest, StartProviderPositiveWithPolicyFalse)
{
    OIC::Service::NSProviderService::ProviderConfig  config;
    config.m_subscribeRequestCb = ConsumerSubscribedCallbackEmpty;
    config.m_syncInfoCb = MessageSynchronizedCallbackEmpty;
    config.policy = false;

     OIC::Service::NSResult ret =  OIC::Service::NSProviderService::getInstance()->Start(config);

    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, std::chrono::milliseconds(3000));
    g_consumerSimul.findProvider();

    responseCon.wait_for(lock, std::chrono::milliseconds(3000));
     OIC::Service::NSProviderService::getInstance()->Stop();
    EXPECT_EQ(ret,  OIC::Service::NSResult::OK);
}

TEST_F(NotificationProviderServiceTest, ExpectCallbackWhenReceiveSubscribeRequestWithAccepterProvider)
{
    mocks.ExpectCallFunc(ConsumerSubscribedCallbackEmpty).Do(
            []( OIC::Service::NSConsumer * consumer)
            {
                std::cout << "ConsumerSubscribedCallbackEmpty" << std::endl;
                g_consumer = new  OIC::Service::NSConsumer(consumer->getConsumerId());
                responseCon.notify_all();
            });

    OIC::Service::NSProviderService::ProviderConfig  config;
    config.m_subscribeRequestCb = ConsumerSubscribedCallbackEmpty;
    config.m_syncInfoCb = MessageSynchronizedCallbackEmpty;
    config.policy = true;

     OIC::Service::NSProviderService::getInstance()->Start(config);

    {
        std::unique_lock< std::mutex > lock{ mutexForCondition };
        responseCon.wait_for(lock, g_waitForResponse);
    }

    g_consumerSimul.setCallback(MessageCallbackFromConsumerEmpty,
            SyncCallbackFromConsumerEmpty);
    g_consumerSimul.findProvider();

    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, std::chrono::milliseconds(1000));
}

TEST_F(NotificationProviderServiceTest, NeverCallNotifyOnConsumerByAcceptIsFalse)
{
    bool expectTrue = true;
    int msgID;

    mocks.OnCallFunc(MessageCallbackFromConsumerEmpty).Do(
            [& expectTrue, &msgID](const int &id, const std::string&, const std::string&, const std::string&)
            {
                if (id == msgID)
                {
                    std::cout << "This function never call" << std::endl;
                    expectTrue = false;
                }
            });

    g_consumer->acceptSubscription(g_consumer, false);

     OIC::Service::NSMessage * msg =  OIC::Service::NSProviderService::getInstance()->CreateMessage();
    msgID = (int)msg->getMessageId();
    msg->setTitle(std::string("Title"));
    msg->setContentText(std::string("ContentText"));
    msg->setSourceName(std::string("OCF"));

     OIC::Service::NSProviderService::getInstance()->SendMessage(msg);
    {
        std::unique_lock< std::mutex > lock{ mutexForCondition };
        responseCon.wait_for(lock, g_waitForResponse);
    }

    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, std::chrono::milliseconds(1000));

    EXPECT_EQ(expectTrue, true);
}

TEST_F(NotificationProviderServiceTest, ExpectCallNotifyOnConsumerByAcceptIsTrue)
{
    int msgID;

    mocks.ExpectCallFunc(MessageCallbackFromConsumerEmpty).Do(
            [&msgID](const int &id, const std::string&, const std::string&, const std::string&)
            {
                if (id == msgID)
                {
                    std::cout << "ExpectCallNotifyOnConsumerByAcceptIsTrue" << std::endl;
                    responseCon.notify_all();
                }
            });

    g_consumer->acceptSubscription(g_consumer, true);

     OIC::Service::NSMessage * msg =  OIC::Service::NSProviderService::getInstance()->CreateMessage();
    msgID = (int)msg->getMessageId();
    msg->setTitle(std::string("Title"));
    msg->setContentText(std::string("ContentText"));
    msg->setSourceName(std::string("OCF"));

      OIC::Service::NSProviderService::getInstance()->SendMessage(msg);
    {
        std::unique_lock< std::mutex > lock{ mutexForCondition };
        responseCon.wait_for(lock, g_waitForResponse);
    }

    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);
}

TEST_F(NotificationProviderServiceTest, ExpectCallbackSyncOnReadToConsumer)
{
    int id;

    mocks.ExpectCallFunc(SyncCallbackFromConsumerEmpty).Do(
            [& id](int & type, int &syncId)
            {
        std::cout << "MessageSynchronizedCallbackEmpty" << std::endl;
                if (syncId == id &&
                        type == (int)OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_READ)
                {
                    std::cout << "ExpectCallbackSyncOnReadFromConsumer" << std::endl;
                    responseCon.notify_all();
                }
            });

    OIC::Service::NSMessage * msg =  OIC::Service::NSProviderService::getInstance()->CreateMessage();
    id = (int)msg->getMessageId();
    msg->setTitle(std::string("Title"));
    msg->setContentText(std::string("ContentText"));
    msg->setSourceName(std::string("OCF"));

     OIC::Service::NSProviderService::getInstance()->SendSyncInfo(msg->getMessageId(), OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_READ);
    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, std::chrono::milliseconds(5000));
}

TEST_F(NotificationProviderServiceTest, ExpectCallbackSyncOnReadFromConsumer)
{
    int type = (int)OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_READ;
    int id;
    mocks.ExpectCallFunc(MessageSynchronizedCallbackEmpty).Do(
            [& id](OIC::Service::NSSyncInfo * sync)
            {
                std::cout << "MessageSynchronizedCallbackEmpty" << std::endl;
                if ((int)sync->getMessageId() == id && sync->getState() == OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_READ)
                {
                    std::cout << "ExpectCallbackSyncOnReadFromConsumer" << std::endl;
                    responseCon.notify_all();
                }
            });

     OIC::Service::NSMessage * msg =  OIC::Service::NSProviderService::getInstance()->CreateMessage();
    id = (int)msg->getMessageId();
    msg->setTitle(std::string("Title"));
    msg->setContentText(std::string("ContentText"));
    msg->setSourceName(std::string("OCF"));

    g_consumerSimul.syncToProvider(type, id, msg->getProviderId());
    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, std::chrono::milliseconds(5000));
}

TEST_F(NotificationProviderServiceTest, CancelObserves)
{
    bool ret = g_consumerSimul.cancelObserves();

    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, std::chrono::milliseconds(5000));

    EXPECT_EQ(ret, true);
}
