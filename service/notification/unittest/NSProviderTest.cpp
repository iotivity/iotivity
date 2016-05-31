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

#include "NSConsumerSimulator.h"

#include "NSCommon.h"
#include "NSProviderInterface.h"

namespace
{
    std::atomic_bool g_isStartedStack(false);

    std::chrono::milliseconds g_waitForResponse(500);

    std::condition_variable responseCon;
    std::mutex mutexForCondition;

    NSConsumerSimulator g_consumerSimul;
    NSConsumer * g_consumer;

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

class NotificationProviderTest : public TestWithMock
{
public:
    NotificationProviderTest() = default;
    ~NotificationProviderTest() = default;

    static void NSRequestedSubscribeCallbackEmpty(NSConsumer *)
    {
        std::cout << __func__ << std::endl;
    }

    static void NSSyncCallbackEmpty(NSSync *)
    {
        std::cout << __func__ << std::endl;
    }

    static void NSMessageCallbackFromConsumerEmpty(
            const std::string &, const std::string &, const std::string &)
    {
        std::cout << __func__ << std::endl;
    }

    static void NSSyncCallbackFromConsumerEmpty(int, const std::string &)
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
                OC::QualityOfService::LowQos
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

TEST_F(NotificationProviderTest, StartProviderPositive)
{
    NSResult ret = NSStartProvider(NS_ACCEPTER_PROVIDER,
            NSRequestedSubscribeCallbackEmpty,
            NSSyncCallbackEmpty);

    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);

    EXPECT_EQ(ret, NS_OK);
}

TEST_F(NotificationProviderTest, StopProviderPositive)
{
    NSResult ret = NSStopProvider();

    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);

    EXPECT_EQ(ret, NS_OK);
}

TEST_F(NotificationProviderTest, ExpectCallbackWhenReceiveSubscribeRequestWithAccepterProvider)
{
    mocks.ExpectCallFunc(NSRequestedSubscribeCallbackEmpty).Do(
            [](NSConsumer * consumer)
            {
                std::cout << "NSRequestedSubscribeCallback" << std::endl;
                g_consumer = consumer;
                responseCon.notify_all();
            });

    NSStartProvider(NS_ACCEPTER_PROVIDER,
            NSRequestedSubscribeCallbackEmpty, NSSyncCallbackEmpty);

    {
        std::unique_lock< std::mutex > lock{ mutexForCondition };
        responseCon.wait_for(lock, g_waitForResponse);
    }

    g_consumerSimul.setCallback(NSMessageCallbackFromConsumerEmpty,
            NSSyncCallbackFromConsumerEmpty);
    g_consumerSimul.findProvider();

    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, std::chrono::milliseconds(1000));
}

TEST_F(NotificationProviderTest, NeverCallNotifyOnConsumerByAcceptIsFalse)
{
    bool expectTrue = true;

    mocks.OnCallFunc(NSMessageCallbackFromConsumerEmpty).Do(
            [& expectTrue](const std::string &id, const std::string&, const std::string&)
            {
                if (id == "NeverCallNotifyOnConsumerByAcceptIsFalse")
                {
                    std::cout << "This function never call" << std::endl;
                    expectTrue = false;
                }
            });

    NSAccept(g_consumer, false);

    NSMessage * msg = new NSMessage();
    msg->mId = strdup(std::string("NeverCallNotifyOnConsumerByAcceptIsFalse").c_str());
    msg->mTitle = strdup(std::string("Title").c_str());
    msg->mContentText = strdup(std::string("ContentText").c_str());
    NSSendMessage(msg);
    {
        std::unique_lock< std::mutex > lock{ mutexForCondition };
        responseCon.wait_for(lock, g_waitForResponse);
    }

    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, std::chrono::milliseconds(1000));

    EXPECT_EQ(expectTrue, true);
}

TEST_F(NotificationProviderTest, ExpectCallNotifyOnConsumerByAcceptIsTrue)
{
    mocks.ExpectCallFunc(NSMessageCallbackFromConsumerEmpty).Do(
            [](const std::string &id, const std::string&, const std::string&)
            {
                if (id == "ExpectCallNotifyOnConsumerByAcceptIsTrue")
                {
                    std::cout << "ExpectCallNotifyOnConsumerByAcceptIsTrue" << std::endl;
                    responseCon.notify_all();
                }
            });

    NSAccept(g_consumer, true);

    NSMessage * msg = new NSMessage();
    msg->mId = strdup(std::string("ExpectCallNotifyOnConsumerByAcceptIsTrue").c_str());
    msg->mTitle = strdup(std::string("Title").c_str());
    msg->mContentText = strdup(std::string("ContentText").c_str());
    NSSendMessage(msg);
    {
        std::unique_lock< std::mutex > lock{ mutexForCondition };
        responseCon.wait_for(lock, g_waitForResponse);
    }

    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);
}

//TEST_F(NotificationProviderTest, ExpectCallbackSyncOnReadToConsumer)
//{
//    int type = 0;
//    std::string id = "ExpectCallNotifyOnConsumerByAcceptIsTrue";
//    mocks.ExpectCallFunc(NSSyncCallbackFromConsumerEmpty).Do(
//            [& id](int type, const std::string & syncId)
//            {
//        std::cout << "NSSyncCallbackEmpty" << std::endl;
//                if (syncId == id &&
//                        type == Notification_Read)
//                {
//                    std::cout << "ExpectCallbackSyncOnReadFromConsumer" << std::endl;
//                    responseCon.notify_all();
//                }
//            });
//
//    NSMessage * msg = new NSMessage();
//    msg->mId = strdup(std::string("ExpectCallNotifyOnConsumerByAcceptIsTrue").c_str());
//    msg->mTitle = strdup(std::string("Title").c_str());
//    msg->mContentText = strdup(std::string("ContentText").c_str());
//    NSProviderReadCheck(msg);
//    std::unique_lock< std::mutex > lock{ mutexForCondition };
//    responseCon.wait_for(lock, std::chrono::milliseconds(5000));
//}

//TEST_F(NotificationProviderTest, ExpectCallbackSyncOnReadFromConsumer)
//{
//    int type = 0;
//    std::string id("ExpectCallNotifyOnConsumerByAcceptIsTrue");
//    mocks.ExpectCallFunc(NSSyncCallbackEmpty).Do(
//            [& id](NSSync * sync)
//            {
//                std::cout << "NSSyncCallbackEmpty" << std::endl;
//                if (sync->mMessageId == id && sync->mState == Notification_Read)
//                {
//                    std::cout << "ExpectCallbackSyncOnReadFromConsumer" << std::endl;
//                    responseCon.notify_all();
//                }
//            });
//
//    g_consumerSimul.syncToProvider(type, std::string(id));
//    std::unique_lock< std::mutex > lock{ mutexForCondition };
//    responseCon.wait_for(lock, std::chrono::milliseconds(5000));
//}
