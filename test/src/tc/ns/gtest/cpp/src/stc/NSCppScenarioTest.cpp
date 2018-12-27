//******************************************************************
//
// Copyright 2018 Samsung Electronics All Rights Reserved.
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

#include "NSCppHelper.h"
#include "NSCppUtility.h"
using namespace std;

class NSCppTest_stc: public ::testing::Test
{
protected:

    NSCppHelper* m_pNSHelper;
    string m_testAppLog;

    virtual void SetUp()
    {
        CommonTestUtil::runCommonTCSetUpPart();

        m_pNSHelper = NSCppHelper::getInstance();
        m_pNSHelper->preConfigure();

        m_testAppLog = "";
    }

    virtual void TearDown()
    {
        m_pNSHelper->stopServices();
    }
};

/**
 * @since 2017-03-16
 * @see int start(OnConsumerSubscribedListener subscribedListener,
 *      OnMessageSynchronizedListener messageSynchronized, boolean
 *      subControllability, String userInfo, boolean resourceSecurity)
 * @see int stop()
 * @see OIC::Service::NSMessage createMessage()
 * @see int registerTopic(String topicName)
 * @see int setTopic(String topicName)
 * @see void start(OnProviderDiscoveredListener onProviderDiscoveredListener)
 * @objective Test Provider State Changed Listener callback
 * @target OnProviderDiscoveredListener onProviderDiscoveredListener
 * @test_data None
 * @pre_condition 1. Get Provider Service Instance
 *                2. Start Provider with Provider acceptor
 *                3. Get Consumer Service Instance
 *                4. Start Consumer Service
 *                5. Wait for OnConsumerSubscribedListener
 *                6. Get shared_ptr<OIC::Service::NSConsumer> object from callback
 *                7. Accept Subscription
 * @procedure 1. Stop Provider Service
 *            2. Check the callback for state provider change
 * @post_condition Stop shared_ptr<OIC::Service::NSConsumer> Service
 * @expected Should get the STOPPED state.
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(NSCppTest_stc, ProviderStateChanged_SRC_P)
{
    shared_ptr< OIC::Service::NSConsumer > consumer = m_pNSHelper->getLocalConsumer(true, false);

    ASSERT_NE(nullptr, consumer.get())<< "Consumer should not be null";

    try
    {
        consumer->acceptSubscription(true);
    }
    catch (OIC::Service::NSException &e)
    {
        FAIL()<< "Can't accept subscription";
    }

    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    ASSERT_EQ(true, m_pNSHelper->stopProviderService())<< "Should stop provider service successfully";

    CommonUtil::waitInSecond(WAIT_TIME_MIN);

    OIC::Service::NSProviderState state = m_pNSHelper->getProviderState();
    ASSERT_EQ(OIC::Service::NSProviderState::STOPPED, state)<< "Provider State should be stopped";

}
#endif

/**
 * @since 2017-03-21
 * @see int start(OnConsumerSubscribedListener subscribedListener,
 *      OnMessageSynchronizedListener messageSynchronized, boolean
 *      subControllability, String userInfo, boolean resourceSecurity)
 * @see int stop()
 * @see OIC::Service::NSMessage createMessage()
 * @see int registerTopic(String topicName)
 * @see int setTopic(String topicName)
 * @see void start(OnProviderDiscoveredListener onProviderDiscoveredListener)
 * @objective Test scenario based 'sendMessage' API functionality
 * @target int sendMessage(OIC::Service::NSMessage message)
 * @test_data OIC::Service::NSMessage
 * @pre_condition 1. Get Provider Service Instance
 *                2. Start Provider with Provider acceptor
 *                3. Get shared_ptr<OIC::Service::NSConsumer> Service Instance
 *                4. Start shared_ptr<OIC::Service::NSConsumer> Service
 *                5. Wait for OnConsumerSubscribedListener
 *                6. Get shared_ptr<OIC::Service::NSConsumer> object from callback
 *                7. Accept Subscription
 * @procedure 1. Register Topic
 *            2. Set Same topic which is registered
 *            3. Get OIC::Service::NSMessage instance with createMessage()
 *            4. Set OIC::Service::NSMessage Title, Body, SourceName and the TopicName
 *               and also the mediaContents
 *            5. Perform sendMessage() API
 *            6. Wait for onMessageReceived Listener
 * @post_condition 1. Stop Provider Service
 *                 2. Stop shared_ptr<OIC::Service::NSConsumer> Service
 * @expected 1. No exception occurs
 *           2. Should get the Notification in consumer side
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(NSCppTest_stc, SendMessage_SRC_P)
{
    shared_ptr< OIC::Service::NSConsumer > consumer = m_pNSHelper->getLocalConsumer(true, false);

    ASSERT_NE(nullptr, consumer.get())<< "Consumer should not be null";

    try
    {
        consumer->acceptSubscription(true);
    }
    catch (OIC::Service::NSException &e)
    {
        FAIL()<< "Can't accept subscription";
    }

    CommonUtil::waitInSecond(WAIT_TIME_MIN);

    ASSERT_EQ(true, m_pNSHelper->sendNotification())<< "Can't send notification";

    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    ASSERT_NE("", m_pNSHelper->getNotification().getContentText())<< "Doesn't get any notification";
}
#endif

/**
 * @since 2017-03-21
 * @see int start(OnConsumerSubscribedListener subscribedListener,
 *      OnMessageSynchronizedListener messageSynchronized, boolean
 *      subControllability, String userInfo, boolean resourceSecurity)
 * @see int stop()
 * @see OIC::Service::NSMessage createMessage()
 * @see int registerTopic(String topicName)
 * @see int setTopic(String topicName)
 * @see void start(OnProviderDiscoveredListener onProviderDiscoveredListener)
 * @objective Test scenario based 'sendMessage' API functionality
 * @target int sendMessage(OIC::Service::NSMessage message)
 * @test_data OIC::Service::NSMessage
 * @pre_condition 1. Get Provider Service Instance
 *                2. Start Provider with Provider acceptor
 *                3. Get shared_ptr<OIC::Service::NSConsumer> Service Instance
 *                4. Start shared_ptr<OIC::Service::NSConsumer> Service
 *                5. Wait for OnConsumerSubscribedListener
 *                6. Get shared_ptr<OIC::Service::NSConsumer> object from callback
 *                7. Accept Subscription
 * @procedure 1. Register Topic
 *            2. Set Same topic which is registered
 *            3. Get OIC::Service::NSMessage instance with createMessage()
 *            4. Set OIC::Service::NSMessage Title, Body, SourceName and the TopicName and also the mediaContents
 *            5. Perform sendMessage() API
 *            6. Wait for onMessageReceived Listener
 * @post_condition 1. Stop Provider Service
 *                 2. Stop shared_ptr<OIC::Service::NSConsumer> Service
 * @expected 1. No exception occurs
 *           2. Should get the Notification in consumer side
 *           3. Title, ContentText, SourceName and Topic should be same
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(NSCppTest_stc, SendMessageAndVerifyMessage_SRC_P)
{
    shared_ptr< OIC::Service::NSConsumer > consumer = m_pNSHelper->getLocalConsumer(true, false);

    ASSERT_NE(nullptr, consumer.get())<< "Consumer should not be null";

    try
    {
        consumer->acceptSubscription(true);
    }
    catch (OIC::Service::NSException &e)
    {
        FAIL()<< "Can't accept subscription";
    }

    CommonUtil::waitInSecond(WAIT_TIME_MIN);

    ASSERT_EQ(true, m_pNSHelper->sendNotification())<< "Can't send notification";

    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    OIC::Service::NSMessage msg = m_pNSHelper->getNotification();

    ASSERT_NE("", msg.getContentText())<< "Doesn't get any notification";

    ASSERT_EQ(TITLE_TO_SET, msg.getTitle());
    ASSERT_EQ(CONTENTTEXT_TO_SET, msg.getContentText());
    ASSERT_EQ(SOURCE_NAME, msg.getSourceName());
    ASSERT_EQ(TEST_TOPIC_1, msg.getTopic());
}
#endif

/**
 * @since 2017-03-21
 * @see int start(OnConsumerSubscribedListener subscribedListener,
 *      OnMessageSynchronizedListener messageSynchronized, boolean
 *      subControllability, String userInfo, boolean resourceSecurity)
 * @see int stop()
 * @see OIC::Service::NSMessage createMessage()
 * @see int registerTopic(String topicName)
 * @see int setTopic(String topicName)
 * @see void start(OnProviderDiscoveredListener onProviderDiscoveredListener)
 * @objective Test scenario based 'sendMessage' API functionality
 * @target int sendMessage(OIC::Service::NSMessage message)
 * @test_data OIC::Service::NSSyncInfo::NSSyncType = READ
 * @pre_condition 1. Get Provider Service Instance
 *                2. Start Provider with Provider acceptor
 *                3. Get shared_ptr<OIC::Service::NSConsumer> Service Instance
 *                4. Start shared_ptr<OIC::Service::NSConsumer> Service
 *                5. Wait for OnConsumerSubscribedListener
 *                6. Get shared_ptr<OIC::Service::NSConsumer> object from callback
 *                7. Accept Subscription
 * @procedure 1. Register Topic
 *            2. Set Same topic which is registered
 *            3. Get OIC::Service::NSMessage instance with createMessage()
 *            4. Set OIC::Service::NSMessage Title, Body, SourceName and the TopicName and also the mediaContents
 *            5. Perform sendMessage() API
 *            6. Wait for onMessageReceived Listener
 *            7. Send OIC::Service::NSSyncInfo to Provider Service
 *            8. Check the callback data
 * @post_condition 1. Stop Provider Service
 *                 2. Stop shared_ptr<OIC::Service::NSConsumer> Service
 * @expected 1. No exception occurs
 *           2. Should get the Notification in consumer side
 *           3. Should get syncInfo in Provider side
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(NSCppTest_stc, ConsumerServiceSendSyncInfoAsRead_SRC_P)
{
    shared_ptr< OIC::Service::NSConsumer > consumer = m_pNSHelper->getLocalConsumer(true, false);

    ASSERT_NE(nullptr, consumer.get())<< "Consumer should not be null";

    try
    {
        consumer->acceptSubscription(true);
    }
    catch (OIC::Service::NSException &e)
    {
        FAIL()<< "Can't accept subscription";
    }

    CommonUtil::waitInSecond(WAIT_TIME_MIN);

    ASSERT_EQ(true, m_pNSHelper->sendNotification())<< "Can't send notification";

    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    OIC::Service::NSMessage msg = m_pNSHelper->getNotification();

    ASSERT_NE("", msg.getContentText())<< "Doesn't get any notification";

    ASSERT_EQ(true, m_pNSHelper->sendSyncInfo(false,
            OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_READ))<< "Send OIC::Service::NSSyncInfo failed";

    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    OIC::Service::NSSyncInfo syncInfo = m_pNSHelper->getSyncInfo(false);

    ASSERT_NE("" , syncInfo.getProviderId())<< "SyncInfo should not be null";
    ASSERT_EQ(OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_READ,
            syncInfo.getState())<< "Syc state must be READ";
}
#endif

/**
 * @since 2017-03-21
 * @see int start(OnConsumerSubscribedListener subscribedListener,
 *      OnMessageSynchronizedListener messageSynchronized, boolean
 *      subControllability, String userInfo, boolean resourceSecurity)
 * @see int stop()
 * @see OIC::Service::NSMessage createMessage()
 * @see int registerTopic(String topicName)
 * @see int setTopic(String topicName)
 * @see void start(OnProviderDiscoveredListener onProviderDiscoveredListener)
 * @objective Test scenario based 'sendMessage' API functionality
 * @target int sendMessage(OIC::Service::NSMessage message)
 * @test_data OIC::Service::NSSyncInfo::NSSyncType = DELETED
 * @pre_condition 1. Get Provider Service Instance
 *                2. Start Provider with Provider acceptor
 *                3. Get shared_ptr<OIC::Service::NSConsumer> Service Instance
 *                4. Start shared_ptr<OIC::Service::NSConsumer> Service
 *                5. Wait for OnConsumerSubscribedListener
 *                6. Get shared_ptr<OIC::Service::NSConsumer> object from callback
 *                7. Accept Subscription
 * @procedure 1. Register Topic
 *            2. Set Same topic which is registered
 *            3. Get OIC::Service::NSMessage instance with createMessage()
 *            4. Set OIC::Service::NSMessage Title, Body, SourceName and the TopicName and also the mediaContents
 *            5. Perform sendMessage() API
 *            6. Wait for onMessageReceived Listener
 *            7. Send OIC::Service::NSSyncInfo to Provider Service
 *            8. Check the callback data
 * @post_condition 1. Stop Provider Service
 *                 2. Stop shared_ptr<OIC::Service::NSConsumer> Service
 * @expected 1. No exception occurs
 *           2. Should get the Notification in consumer side
 *           3. Should get syncInfo in Provider side
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(NSCppTest_stc, ConsumerServiceSendSyncInfoAsDeleted_SRC_P)
{
    shared_ptr< OIC::Service::NSConsumer > consumer = m_pNSHelper->getLocalConsumer(true, false);

    ASSERT_NE(nullptr, consumer.get())<< "Consumer should not be null";

    try
    {
        consumer->acceptSubscription(true);
    }
    catch (OIC::Service::NSException &e)
    {
        FAIL()<< "Can't accept subscription";
    }

    CommonUtil::waitInSecond(WAIT_TIME_MIN);

    ASSERT_EQ(true, m_pNSHelper->sendNotification())<< "Can't send notification";

    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    OIC::Service::NSMessage msg = m_pNSHelper->getNotification();

    ASSERT_NE("", msg.getContentText())<< "Doesn't get any notification";

    ASSERT_EQ(true, m_pNSHelper->sendSyncInfo(false,
                    OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_DELETED))<< "Send OIC::Service::NSSyncInfo failed";

    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    OIC::Service::NSSyncInfo syncInfo = m_pNSHelper->getSyncInfo(false);

    ASSERT_NE("" , syncInfo.getProviderId())<< "SyncInfo should not be null";
    ASSERT_EQ(OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_DELETED,
            syncInfo.getState())<< "Sync state must be DELETED";
}
#endif

/**
 * @since 2017-03-21
 * @see int start(OnConsumerSubscribedListener subscribedListener,
 *      OnMessageSynchronizedListener messageSynchronized, boolean
 *      subControllability, String userInfo, boolean resourceSecurity)
 * @see int stop()
 * @see OIC::Service::NSMessage createMessage()
 * @see int registerTopic(String topicName)
 * @see int setTopic(String topicName)
 * @see void start(OnProviderDiscoveredListener onProviderDiscoveredListener)
 * @objective Test scenario based 'sendMessage' API functionality
 * @target int sendMessage(OIC::Service::NSMessage message)
 * @test_data OIC::Service::NSSyncInfo::NSSyncType = UNREAD
 * @pre_condition 1. Get Provider Service Instance
 *                2. Start Provider with Provider acceptor
 *                3. Get shared_ptr<OIC::Service::NSConsumer> Service Instance
 *                4. Start shared_ptr<OIC::Service::NSConsumer> Service
 *                5. Wait for OnConsumerSubscribedListener
 *                6. Get shared_ptr<OIC::Service::NSConsumer> object from callback
 *                7. Accept Subscription
 * @procedure 1. Register Topic
 *            2. Set Same topic which is registered
 *            3. Get OIC::Service::NSMessage instance with createMessage()
 *            4. Set OIC::Service::NSMessage Title, Body, SourceName and the TopicName and also the mediaContents
 *            5. Perform sendMessage() API
 *            6. Wait for onMessageReceived Listener
 *            7. Send OIC::Service::NSSyncInfo to Provider Service
 *            8. Check the callback data
 * @post_condition 1. Stop Provider Service
 *                 2. Stop shared_ptr<OIC::Service::NSConsumer> Service
 * @expected 1. No exception occurs
 *           2. Should get the Notification in consumer side
 *           3. Should get syncInfo in Provider side
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(NSCppTest_stc, ConsumerServiceSendSyncInfoAsUnread_SRC_P)
{
    shared_ptr< OIC::Service::NSConsumer > consumer = m_pNSHelper->getLocalConsumer(true, false);

    ASSERT_NE(nullptr, consumer.get())<< "Consumer should not be null";

    try
    {
        consumer->acceptSubscription(true);
    }
    catch (OIC::Service::NSException &e)
    {
        FAIL()<< "Can't accept subscription";
    }

    CommonUtil::waitInSecond(WAIT_TIME_MIN);

    ASSERT_EQ(true, m_pNSHelper->sendNotification())<< "Can't send notification";

    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    OIC::Service::NSMessage msg = m_pNSHelper->getNotification();

    ASSERT_NE("", msg.getTopic())<< "Doesn't get any notification";

    ASSERT_EQ(true, m_pNSHelper->sendSyncInfo(true,
            OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_UNREAD))<< "Send OIC::Service::NSSyncInfo failed";

    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    OIC::Service::NSSyncInfo syncInfo = m_pNSHelper->getSyncInfo(true);

    ASSERT_EQ(OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_UNREAD,
            syncInfo.getState())<< "Syanc state must be UNREAD";
}
#endif

/**
 * @since 2017-03-21
 * @see int start(OnConsumerSubscribedListener subscribedListener,
 *      OnMessageSynchronizedListener messageSynchronized, boolean
 *      subControllability, String userInfo, boolean resourceSecurity)
 * @see int stop()
 * @see OIC::Service::NSMessage createMessage()
 * @see int registerTopic(String topicName)
 * @see int setTopic(String topicName)
 * @see void start(OnProviderDiscoveredListener onProviderDiscoveredListener)
 * @objective Test scenario based 'sendMessage' API functionality
 * @target int sendMessage(OIC::Service::NSMessage message)
 * @test_data OIC::Service::NSSyncInfo::NSSyncType = READ
 * @pre_condition 1. Get Provider Service Instance
 *                2. Start Provider with Provider acceptor
 *                3. Get shared_ptr<OIC::Service::NSConsumer> Service Instance
 *                4. Start shared_ptr<OIC::Service::NSConsumer> Service
 *                5. Wait for OnConsumerSubscribedListener
 *                6. Get shared_ptr<OIC::Service::NSConsumer> object from callback
 *                7. Accept Subscription
 * @procedure 1. Register Topic
 *            2. Set Same topic which is registered
 *            3. Get OIC::Service::NSMessage instance with createMessage()
 *            4. Set OIC::Service::NSMessage Title, Body, SourceName and the TopicName and also the mediaContents
 *            5. Perform sendMessage() API
 *            6. Wait for onMessageReceived Listener
 *            7. Send OIC::Service::NSSyncInfo to shared_ptr<OIC::Service::NSConsumer> Service
 *            8. Check the callback data
 * @post_condition 1. Stop Provider Service
 *                 2. Stop shared_ptr<OIC::Service::NSConsumer> Service
 * @expected 1. No exception occurs
 *           2. Should get the Notification in consumer side
 *           3. Should get syncInfo in shared_ptr<OIC::Service::NSConsumer> side
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(NSCppTest_stc, ProviderServiceSendSyncInfoAsREAD_SRC_P)
{
    shared_ptr< OIC::Service::NSConsumer > consumer = m_pNSHelper->getLocalConsumer(true, false);

    ASSERT_NE(nullptr, consumer.get())<< "Consumer should not be null";

    try
    {
        consumer->acceptSubscription(true);
    }
    catch (OIC::Service::NSException &e)
    {
        FAIL()<< "Can't accept subscription";
    }

    CommonUtil::waitInSecond(WAIT_TIME_MIN);

    ASSERT_EQ(true, m_pNSHelper->sendNotification())<< "Can't send notification";

    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    OIC::Service::NSMessage msg = m_pNSHelper->getNotification();

    ASSERT_NE("", msg.getContentText())<< "Doesn't get any notification";

    ASSERT_EQ(true, m_pNSHelper->sendSyncInfo(false,
            OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_READ))<< "Send OIC::Service::NSSyncInfo failed";

    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    OIC::Service::NSSyncInfo syncInfo = m_pNSHelper->getSyncInfo(true);

    ASSERT_NE("" , syncInfo.getProviderId())<< "SyncInfo should not be null";
    ASSERT_EQ(OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_READ, syncInfo.getState());
}
#endif

/**
 * @since 2017-03-21
 * @see int start(OnConsumerSubscribedListener subscribedListener,
 *      OnMessageSynchronizedListener messageSynchronized, boolean
 *      subControllability, String userInfo, boolean resourceSecurity)
 * @see int stop()
 * @see OIC::Service::NSMessage createMessage()
 * @see int registerTopic(String topicName)
 * @see int setTopic(String topicName)
 * @see void start(OnProviderDiscoveredListener onProviderDiscoveredListener)
 * @objective Test scenario based 'sendMessage' API functionality
 * @target int sendMessage(OIC::Service::NSMessage message)
 * @test_data OIC::Service::NSSyncInfo::NSSyncType = UNREAD
 * @pre_condition 1. Get Provider Service Instance
 *                2. Start Provider with Provider acceptor
 *                3. Get shared_ptr<OIC::Service::NSConsumer> Service Instance
 *                4. Start shared_ptr<OIC::Service::NSConsumer> Service
 *                5. Wait for OnConsumerSubscribedListener
 *                6. Get shared_ptr<OIC::Service::NSConsumer> object from callback
 *                7. Accept Subscription
 * @procedure 1. Register Topic
 *            2. Set Same topic which is registered
 *            3. Get OIC::Service::NSMessage instance with createMessage()
 *            4. Set OIC::Service::NSMessage Title, Body, SourceName and the TopicName and also the mediaContents
 *            5. Perform sendMessage() API
 *            6. Wait for onMessageReceived Listener
 *            7. Send OIC::Service::NSSyncInfo to shared_ptr<OIC::Service::NSConsumer> Service
 *            8. Check the callback data
 * @post_condition 1. Stop Provider Service
 *                 2. Stop shared_ptr<OIC::Service::NSConsumer> Service
 * @expected 1. No exception occurs
 *           2. Should get the Notification in consumer side
 *           3. Should not get syncInfo in shared_ptr<OIC::Service::NSConsumer> side
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(NSCppTest_stc, ProviderServiceSendSyncInfoAsUNREAD_SRC_P)
{
    shared_ptr< OIC::Service::NSConsumer > consumer = m_pNSHelper->getLocalConsumer(true, false);

    ASSERT_NE(nullptr, consumer.get())<< "Consumer should not be null";

    try
    {
        consumer->acceptSubscription(true);
    }
    catch (OIC::Service::NSException &e)
    {
        FAIL()<< "Can't accept subscription";
    }

    CommonUtil::waitInSecond(WAIT_TIME_MIN);

    ASSERT_EQ(true, m_pNSHelper->sendNotification())<< "Can't send notification";

    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    OIC::Service::NSMessage msg = m_pNSHelper->getNotification();

    ASSERT_NE("", msg.getContentText())<< "Doesn't get any notification";

    ASSERT_EQ(true, m_pNSHelper->sendSyncInfo(false,
            OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_UNREAD))<< "Send SyncInfo failed";

    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    OIC::Service::NSSyncInfo syncInfo = m_pNSHelper->getSyncInfo(true);

    ASSERT_NE("" , syncInfo.getProviderId())<< "SyncInfo should not be null";
}
#endif

/**
 * @since 2017-03-21
 * @see int start(OnConsumerSubscribedListener subscribedListener,
 *      OnMessageSynchronizedListener messageSynchronized, boolean
 *      subControllability, String userInfo, boolean resourceSecurity)
 * @see int stop()
 * @see OIC::Service::NSMessage createMessage()
 * @see int registerTopic(String topicName)
 * @see int setTopic(String topicName)
 * @see void start(OnProviderDiscoveredListener onProviderDiscoveredListener)
 * @objective Test scenario based 'sendMessage' API functionality
 * @target int sendMessage(OIC::Service::NSMessage message)
 * @test_data OIC::Service::NSSyncInfo::NSSyncType = DELETED
 * @pre_condition 1. Get Provider Service Instance
 *                2. Start Provider with Provider acceptor
 *                3. Get shared_ptr<OIC::Service::NSConsumer> Service Instance
 *                4. Start shared_ptr<OIC::Service::NSConsumer> Service
 *                5. Wait for OnConsumerSubscribedListener
 *                6. Get shared_ptr<OIC::Service::NSConsumer> object from callback
 *                7. Accept Subscription
 * @procedure 1. Register Topic
 *            2. Set Same topic which is registered
 *            3. Get OIC::Service::NSMessage instance with createMessage()
 *            4. Set OIC::Service::NSMessage Title, Body, SourceName and the TopicName and also the mediaContents
 *            5. Perform sendMessage() API
 *            6. Wait for onMessageReceived Listener
 *            7. Send OIC::Service::NSSyncInfo to shared_ptr<OIC::Service::NSConsumer> Service
 *            8. Check the callback data
 * @post_condition 1. Stop Provider Service
 *                 2. Stop shared_ptr<OIC::Service::NSConsumer> Service
 * @expected 1. No exception occurs
 *           2. Should get the Notification in consumer side
 *           3. Should get syncInfo in shared_ptr<OIC::Service::NSConsumer> side
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(NSCppTest_stc, ProviderServiceSendSyncInfoAsDELETED_SRC_P)
{
    shared_ptr< OIC::Service::NSConsumer > consumer = m_pNSHelper->getLocalConsumer(true, false);

    ASSERT_NE(nullptr, consumer.get())<< "Consumer should not be null";

    try
    {
        consumer->acceptSubscription(true);
    }
    catch (OIC::Service::NSException &e)
    {
        FAIL()<< "Can't accept subscription";
    }

    CommonUtil::waitInSecond(WAIT_TIME_MIN);

    ASSERT_EQ(true, m_pNSHelper->sendNotification())<< "Can't send notification";

    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    OIC::Service::NSMessage msg = m_pNSHelper->getNotification();

    ASSERT_NE("", msg.getContentText())<< "Doesn't get any notification";

    ASSERT_EQ(true, m_pNSHelper->sendSyncInfo(true,
            OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_DELETED))<< "Send OIC::Service::NSSyncInfo failed";

    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    OIC::Service::NSSyncInfo syncInfo = m_pNSHelper->getSyncInfo(false);

    ASSERT_NE("" , syncInfo.getProviderId())<< "SyncInfo should not be null";
    ASSERT_EQ(OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_DELETED, syncInfo.getState());
}
#endif

/**
 * @since 2016-10-20
 * @see int start(OnConsumerSubscribedListener subscribedListener,
 *      OnMessageSynchronizedListener messageSynchronized, boolean
 *      subControllability, String userInfo, boolean resourceSecurity)
 * @see int stop()
 * @see OIC::Service::NSMessage createMessage()
 * @see int registerTopic(String topicName)
 * @see void start(OnProviderDiscoveredListener onProviderDiscoveredListener)
 * @objective Test scenario based 'sendMessage' API functionality
 * @target int sendMessage(OIC::Service::NSMessage message)
 * @test_data None
 * @pre_condition 1. Get Provider Service Instance
 *                2. Start Provider with Provider acceptor
 *                3. Register Topic with topicName = "OCF_TestTopic"
 *                4. Get shared_ptr<OIC::Service::NSConsumer> Service Instance
 *                5. Start shared_ptr<OIC::Service::NSConsumer> Service
 *                6. Wait for OnConsumerSubscribedListener
 *                7. Get shared_ptr<OIC::Service::NSConsumer> object from callback
 *                8. Accept Subscription
 * @procedure 1. Get the topic list
 *            2. Check the topic state
 * @post_condition 1. Stop Provider Service
 *                 2. Stop shared_ptr<OIC::Service::NSConsumer> Service
 * @expected 1. No exception occurs
 *           2. Topic state should be UNSUBSCRIBED at initial stage
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(NSCppTest_stc, TopicState_SRC_P)
{
    shared_ptr< OIC::Service::NSConsumer > consumer = m_pNSHelper->getLocalConsumer(true, true);

    ASSERT_NE(nullptr, consumer.get())<< "Consumer should not be null";

    try
    {
        consumer->acceptSubscription(true);
    }
    catch (OIC::Service::NSException &e)
    {
        FAIL()<< "Can't accept subscription";
    }

    CommonUtil::waitInSecond(WAIT_TIME_MIN);

    try
    {
        list< OIC::Service::NSTopic > topics = consumer->getConsumerTopicList()->getTopicsList();
        OIC::Service::NSTopic topic = topics.front();
        ASSERT_EQ(OIC::Service::NSTopic::NSTopicState::UNSUBSCRIBED,
                topic.getState())<< "Topic should be UNSUBSCRIBED state after unsetTopic()";
    }
    catch (OIC::Service::NSException &e)
    {
        FAIL() << "Can't get consumerTopicList. Exception: " << e.what();
    }
}
#endif

/**
 * @since 2016-10-20
 * @see int start(OnConsumerSubscribedListener subscribedListener,
 *      OnMessageSynchronizedListener messageSynchronized, boolean
 *      subControllability, String userInfo, boolean resourceSecurity)
 * @see int stop()
 * @see OIC::Service::NSMessage createMessage()
 * @see int registerTopic(String topicName)
 * @see void start(OnProviderDiscoveredListener onProviderDiscoveredListener)
 * @objective Test scenario based 'sendMessage' API functionality
 * @target int sendMessage(OIC::Service::NSMessage message)
 * @test_data None
 * @pre_condition 1. Get Provider Service Instance
 *                2. Start Provider with Provider acceptor
 *                3. Register Topic with topicName = "OCF_TestTopic"
 *                4. Get shared_ptr<OIC::Service::NSConsumer> Service Instance
 *                5. Start shared_ptr<OIC::Service::NSConsumer> Service
 *                6. Wait for OnConsumerSubscribedListener
 *                7. Get shared_ptr<OIC::Service::NSConsumer> object from callback
 *                8. Accept Subscription
 * @procedure 1. SetTopic for the topicName = "OCF_TestTopic"
 *            2. Get the topic list
 *            3. Check the topic state
 *            4. Unset the topic
 *            5. Get and check the topic state
 * @post_condition 1. Stop Provider Service
 *                 2. Stop shared_ptr<OIC::Service::NSConsumer> Service
 * @expected 1. No exception occurs
 *           2. Topic state should be SUBSCRIBED state after set
 *           3. Topic state should be UNSUBSCRIBED state after unset
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(NSCppTest_stc, SetUnsetTopic_SQV_P)
{
    shared_ptr< OIC::Service::NSConsumer > consumer = m_pNSHelper->getLocalConsumer(true, true);

    ASSERT_NE(nullptr, consumer.get())<< "Consumer should not be null";

    try
    {
        consumer->acceptSubscription(true);
    }
    catch (OIC::Service::NSException &e)
    {
        FAIL()<< "Exception occurred while accept subscription : " << e.what();
    }

    CommonUtil::waitInSecond(WAIT_TIME_MIN + WAIT_TIME_MIN );

    try
    {
        OIC::Service::NSTopic topic = NSCppUtility::getDesiredTopic(consumer->getConsumerTopicList(), TEST_TOPIC_1);
        ASSERT_EQ((int) OIC::Service::NSTopic::NSTopicState::UNSUBSCRIBED,
                (int) topic.getState())<< "Topic should be UNSUBSCRIBED state before setTopic()";

        consumer->setTopic(TEST_TOPIC_1);
        topic = NSCppUtility::getDesiredTopic(consumer->getConsumerTopicList(), TEST_TOPIC_1);
        ASSERT_EQ((int) OIC::Service::NSTopic::NSTopicState::SUBSCRIBED,
                (int) topic.getState())<< "Topic should be SUBSCRIBED state after setTopic()";

        consumer->unsetTopic(TEST_TOPIC_1);
        topic = NSCppUtility::getDesiredTopic(consumer->getConsumerTopicList(), TEST_TOPIC_1);
        ASSERT_EQ(OIC::Service::NSTopic::NSTopicState::UNSUBSCRIBED,
                topic.getState())<< "Topic should be UNSUBSCRIBED state after unsetTopic()";
    }
    catch (OIC::Service::NSException &e)
    {
        FAIL() << "Can't get consumerTopicList. Exception: " << e.what();
    }
}
#endif
