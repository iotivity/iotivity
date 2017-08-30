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

/**
 * @file NSConsumerServiceTest2.cpp
 *
 * This file contains Notification consumer service test cases that do not involve network related methods.
 * The test cases are executed during the build by enabling the option 'TEST' in scons build.
 * Eg., scons [options] TEST=1
 */

#include <gtest/gtest.h>
#include <atomic>
#include <functional>
#include <condition_variable>
#include <mutex>
#include <chrono>

#include "ocstack.h"

#include "NSUtils.h"
#include "NSSyncInfo.h"
#include "NSMessage.h"
#include "NSMediaContents.h"
#include "NSConsumerService.h"
#include "NSAcceptedProviders.h"
#include "NSCommon.h"

#include "NSProviderServiceSimulator.h"

namespace
{

    NSProviderSimulator g_providerSimul;
    std::shared_ptr<OIC::Service::NSProvider> g_provider;

    std::atomic_bool g_isStartedStack(false);

    std::chrono::milliseconds g_waitForResponse(10);

    std::condition_variable responseCon;
    std::mutex mutexForCondition;

    enum class NSSelector
    {
        NS_SELECTION_CONSUMER = 0,
        NS_SELECTION_PROVIDER = 1
    };

    OIC::Service::NSProviderState g_expectedState = OIC::Service::NSProviderState::DENY;
    OIC::Service::NSProviderState g_revState = OIC::Service::NSProviderState::DENY;

    uint64_t g_messageId = 0;

    OIC::Service::NSSyncInfo::NSSyncType g_expectedSyncType =
        OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_DELETED;
    OIC::Service::NSSyncInfo::NSSyncType g_syncType =
        OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_DELETED;

}

class TestWithMock: public testing::Test
{
    protected:
        virtual ~TestWithMock() noexcept(noexcept(std::declval<Test>().~Test()))
        {

        }

        virtual void TearDown()
        {

        }
};

class NotificationServiceConsumerTest : public TestWithMock
{
    public:
        NotificationServiceConsumerTest() = default;
        ~NotificationServiceConsumerTest() = default;

        static void ProviderDiscoveredCallback( std::shared_ptr<OIC::Service::NSProvider> provider)
        {
            std::cout << __func__ << std::endl;
            g_provider = provider;
            responseCon.notify_all();
        }

        static void NotificationReceivedCallback( OIC::Service::NSMessage message)
        {
            std::cout << "Income Notification : " << message.getMessageId() << std::endl;
            g_messageId = message.getMessageId();
            responseCon.notify_all();
        }

        static void SyncCallback(OIC::Service::NSSyncInfo sync)
        {
            std::cout << __func__ << std::endl;
            g_syncType = sync.getState();
            if (g_expectedSyncType == sync.getState())
            {
                std::cout << "[Expected]" << __func__ << std::endl;
                responseCon.notify_all();
            }
        }

        static void FoundResource(std::shared_ptr< OC::OCResource >)
        {
            std::cout << __func__ << std::endl;
        }

        static void ProviderChangedCallback( OIC::Service::NSProviderState state)
        {
            std::cout << __func__ << std::endl;

            if (g_expectedState == state)
            {
                g_revState = state;
                std::cout << "[Expected]" << __func__ << std::endl;
                responseCon.notify_all();
            }
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

TEST_F(NotificationServiceConsumerTest, StartConsumerPositive)
{

    OIC::Service::NSResult res = OIC::Service::NSConsumerService::getInstance()->start(
                                     ProviderDiscoveredCallback);
    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);
    EXPECT_EQ(OIC::Service::NSResult::OK, res);
}

TEST_F(NotificationServiceConsumerTest, StopConsumerPositive)
{
    OIC::Service::NSResult res = OIC::Service::NSConsumerService::getInstance()->stop();
    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);
    EXPECT_EQ(OIC::Service::NSResult::OK, res);
}

TEST_F(NotificationServiceConsumerTest, ExpectSuccessGetValidDiscoverProviderCb)
{
    OIC::Service::NSConsumerService::getInstance()->start(ProviderDiscoveredCallback);

    auto discCb = OIC::Service::NSConsumerService::getInstance()->getProviderDiscoveredCb();
    ASSERT_NE(nullptr, discCb) << "error: discovery cb register failure";
}

TEST_F(NotificationServiceConsumerTest, ExpectSuccessRescanProvider)
{
    OIC::Service::NSResult res = OIC::Service::NSConsumerService::getInstance()->rescanProvider();
    EXPECT_EQ(OIC::Service::NSResult::OK, res);
}

TEST_F(NotificationServiceConsumerTest, ExpectGetProviderSuccessWithInvalidProviderId)
{
    std::shared_ptr<OIC::Service::NSProvider> provider =
        OIC::Service::NSConsumerService::getInstance()->getProvider("123456");
    bool res = (provider == nullptr);
    EXPECT_EQ(res, 1);
}

TEST_F(NotificationServiceConsumerTest, ExpectGetProviderSuccessWithValidProviderId)
{
    ::NSProvider *provider = (::NSProvider *)malloc(sizeof(::NSProvider));
    strncpy(provider->providerId, "098765432109876543210987654321098765", NS_UTILS_UUID_STRING_SIZE);
    std::string provId;
    provId.assign(provider->providerId, NS_UTILS_UUID_STRING_SIZE - 1);

    std::shared_ptr<OIC::Service::NSProvider> providerTemp = std::make_shared<OIC::Service::NSProvider>
            (provider);

    OIC::Service::NSAcceptedProviders *acceptedProviders =
        OIC::Service::NSConsumerService::getInstance()->getAcceptedProviders();
    acceptedProviders->addProvider(providerTemp);

    std::shared_ptr<OIC::Service::NSProvider> resProvider =
        OIC::Service::NSConsumerService::getInstance()->getProvider(provId);
    bool res = (resProvider != nullptr);
    EXPECT_EQ(res, 1);
    free(provider);
}


TEST_F(NotificationServiceConsumerTest, ExpectSuccessSendSyncInfo)
{
    uint64_t msgId = 10;
    OIC::Service::NSResult res = OIC::Service::NSResult::FAIL;
    std::string provId;

    ::NSProvider *provider = (::NSProvider *)malloc(sizeof(::NSProvider));
    strncpy(provider->providerId, "098765432109876543210987654321098765", NS_UTILS_UUID_STRING_SIZE);
    provId.assign(provider->providerId, NS_UTILS_UUID_STRING_SIZE - 1);

    std::shared_ptr<OIC::Service::NSProvider> providerTemp = std::make_shared<OIC::Service::NSProvider>
            (provider);

    OIC::Service::NSAcceptedProviders *acceptedProviders =
        OIC::Service::NSConsumerService::getInstance()->getAcceptedProviders();
    acceptedProviders->addProvider(providerTemp);

    std::shared_ptr<OIC::Service::NSProvider> resProvider =
        OIC::Service::NSConsumerService::getInstance()->getProvider(provId);

    if (resProvider != nullptr)
    {
        res = resProvider->sendSyncInfo(msgId, OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_READ);
    }
    EXPECT_EQ(OIC::Service::NSResult::OK, res);

    OIC::Service::NSConsumerService::getInstance()
        ->getAcceptedProviders()->removeProvider(provider->providerId);
    free(provider);
}

TEST_F(NotificationServiceConsumerTest, ExpectSuccessGetTopicsList)
{
    std::string provId;

    ::NSProvider *provider = (::NSProvider *)malloc(sizeof(::NSProvider));
    strncpy(provider->providerId, "098765432109876543210987654321098765", NS_UTILS_UUID_STRING_SIZE);
    provId.assign(provider->providerId, NS_UTILS_UUID_STRING_SIZE - 1);

    std::shared_ptr<OIC::Service::NSProvider> providerTemp = std::make_shared<OIC::Service::NSProvider>
            (provider);

    OIC::Service::NSAcceptedProviders *acceptedProviders =
        OIC::Service::NSConsumerService::getInstance()->getAcceptedProviders();
    acceptedProviders->addProvider(providerTemp);

    std::shared_ptr<OIC::Service::NSProvider> resProvider =
        OIC::Service::NSConsumerService::getInstance()->getProvider(provId);

    auto topicList = resProvider->getTopicList();

    ASSERT_NE(nullptr, topicList) << "Get topics list failure";

    OIC::Service::NSConsumerService::getInstance()
        ->getAcceptedProviders()->removeProvider(provider->providerId);
    free(provider);

    OIC::Service::NSConsumerService::getInstance()->stop();
    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);
}

