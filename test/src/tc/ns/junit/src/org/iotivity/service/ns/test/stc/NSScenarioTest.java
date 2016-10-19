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

package org.iotivity.service.ns.test.stc;

import static org.iotivity.service.ns.test.helper.NSTestUtilily.*;

import java.util.Vector;

import org.iotivity.service.ns.common.Message;
import org.iotivity.service.ns.common.NSException;
import org.iotivity.service.ns.common.SyncInfo;
import org.iotivity.service.ns.common.SyncInfo.SyncType;
import org.iotivity.service.ns.common.Topic;
import org.iotivity.service.ns.common.Topic.TopicState;
import org.iotivity.service.ns.consumer.Provider.ProviderState;
import org.iotivity.service.ns.provider.Consumer;
import org.iotivity.service.ns.test.helper.NSHelper;

import android.test.InstrumentationTestCase;

public class NSScenarioTest extends InstrumentationTestCase {
  private NSHelper mHelper = null;

  protected void setUp() throws Exception {
    super.setUp();

    mHelper = NSHelper.getInstance(getInstrumentation().getTargetContext());
  }

  protected void tearDown() throws Exception {
    super.tearDown();

    mHelper.stopServices();
  }

  /**
   * @since 2016-10-17
   * @see int start(OnConsumerSubscribedListener subscribedListener,
   *      OnMessageSynchronizedListener messageSynchronized, boolean
   *      subControllability, String userInfo, boolean resourceSecurity)
   * @see int stop()
   * @see Message createMessage()
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
   *                6. Get Consumer object from callback
   *                7. Accept Subscription
   * @procedure 1. Stop Provider Service
   *            2. Check the callback for state provider change
   * @post_condition Stop Consumer Service
   * @expected Should get the STOPPED state.
   */
  public void testProviderStateChanged_SRC_P() {
    Consumer consumer = mHelper.getConsumer(true, false);

    assertNotNull("Consumer should not be null", consumer);

    try {
      consumer.acceptSubscription(true);
    } catch (NSException e) {
      assertTrue("Can't accept subscription", false);
    }

    mHelper.waitInSecond(WAIT_DEFAULT_TIME);

    assertEquals("Should stop provider service successfully", true,
        mHelper.stopProviderService());

    mHelper.waitInSecond(WAIT_MIN_TIME);

    ProviderState state = mHelper.getProviderState();
    assertNotNull("Provider State should not null", state);

    assertEquals(ProviderState.STOPPED, state);
  }

  /**
   * @since 2016-10-17
   * @see int start(OnConsumerSubscribedListener subscribedListener,
   *      OnMessageSynchronizedListener messageSynchronized, boolean
   *      subControllability, String userInfo, boolean resourceSecurity)
   * @see int stop()
   * @see Message createMessage()
   * @see int registerTopic(String topicName)
   * @see int setTopic(String topicName)
   * @see void start(OnProviderDiscoveredListener onProviderDiscoveredListener)
   * @objective Test scenario based 'sendMessage' API functionality
   * @target int sendMessage(Message message)
   * @test_data Message
   * @pre_condition 1. Get Provider Service Instance
   *                2. Start Provider with Provider acceptor
   *                3. Get Consumer Service Instance
   *                4. Start Consumer Service
   *                5. Wait for OnConsumerSubscribedListener
   *                6. Get Consumer object from callback
   *                7. Accept Subscription
   * @procedure 1. Register Topic
   *            2. Set Same topic which is registered
   *            3. Get Message instance with createMessage()
   *            4. Set Message Title, Body, SourceName and the TopicName
   *               and also the mediaContents
   *            5. Perform sendMessage() API
   *            6. Wait for onMessageReceived Listener
   * @post_condition 1. Stop Provider Service
   *                 2. Stop Consumer Service
   * @expected 1. No exception occurs
   *           2. Should get the Notification in consumer side
   */
  public void testSendMessage_SRC_P() {
    Consumer consumer = mHelper.getConsumer(true, false);

    assertNotNull("Consumer should not be null", consumer);

    try {
      consumer.acceptSubscription(true);
    } catch (NSException e) {
      assertTrue("Can't accept subscription", false);
    }

    mHelper.waitInSecond(WAIT_MIN_TIME);

    assertEquals("Can't send notification", true, mHelper.sendNotification());

    mHelper.waitInSecond(WAIT_DEFAULT_TIME);

    assertNotNull("Doesn't get any notification", mHelper.getNotification());
  }

  /**
   * @since 2016-10-17
   * @see int start(OnConsumerSubscribedListener subscribedListener,
   *      OnMessageSynchronizedListener messageSynchronized, boolean
   *      subControllability, String userInfo, boolean resourceSecurity)
   * @see int stop()
   * @see Message createMessage()
   * @see int registerTopic(String topicName)
   * @see int setTopic(String topicName)
   * @see void start(OnProviderDiscoveredListener onProviderDiscoveredListener)
   * @objective Test scenario based 'sendMessage' API functionality
   * @target int sendMessage(Message message)
   * @test_data Message
   * @pre_condition 1. Get Provider Service Instance
   *                2. Start Provider with Provider acceptor
   *                3. Get Consumer Service Instance
   *                4. Start Consumer Service
   *                5. Wait for OnConsumerSubscribedListener
   *                6. Get Consumer object from callback
   *                7. Accept Subscription
   * @procedure 1. Register Topic
   *            2. Set Same topic which is registered
   *            3. Get Message instance with createMessage()
   *            4. Set Message Title, Body, SourceName and the TopicName and also the mediaContents
   *            5. Perform sendMessage() API
   *            6. Wait for onMessageReceived Listener
   * @post_condition 1. Stop Provider Service
   *                 2. Stop Consumer Service
   * @expected 1. No exception occurs
   *           2. Should get the Notification in consumer side
   *           3. Title, ContentText, SourceName and Topic should be same
   */
  public void testSendMessageAndVerifyMessage_SRC_P() {
    Consumer consumer = mHelper.getConsumer(true, false);

    assertNotNull("Consumer should not be null", consumer);

    try {
      consumer.acceptSubscription(true);
    } catch (NSException e) {
      assertTrue("Can't accept subscription", false);
    }

    mHelper.waitInSecond(WAIT_MIN_TIME);

    assertEquals("Can't send notification", true, mHelper.sendNotification());

    mHelper.waitInSecond(WAIT_DEFAULT_TIME);

    Message msg = mHelper.getNotification();

    assertNotNull("Doesn't get any notification", msg);

    assertEquals(MSG_TITLE, msg.getTitle());
    assertEquals(MSG_BODY, msg.getContentText());
    assertEquals(MSG_SOURCE, msg.getSourceName());
    assertEquals(TOPIC_NAME, msg.getTopic());
  }

  /**
   * @since 2016-10-17
   * @see int start(OnConsumerSubscribedListener subscribedListener,
   *      OnMessageSynchronizedListener messageSynchronized, boolean
   *      subControllability, String userInfo, boolean resourceSecurity)
   * @see int stop()
   * @see Message createMessage()
   * @see int registerTopic(String topicName)
   * @see int setTopic(String topicName)
   * @see void start(OnProviderDiscoveredListener onProviderDiscoveredListener)
   * @objective Test scenario based 'sendMessage' API functionality
   * @target int sendMessage(Message message)
   * @test_data SyncType = READ
   * @pre_condition 1. Get Provider Service Instance
   *                2. Start Provider with Provider acceptor
   *                3. Get Consumer Service Instance
   *                4. Start Consumer Service
   *                5. Wait for OnConsumerSubscribedListener
   *                6. Get Consumer object from callback
   *                7. Accept Subscription
   * @procedure 1. Register Topic
   *            2. Set Same topic which is registered
   *            3. Get Message instance with createMessage()
   *            4. Set Message Title, Body, SourceName and the TopicName and also the mediaContents
   *            5. Perform sendMessage() API
   *            6. Wait for onMessageReceived Listener
   *            7. Send SyncInfo to Provider Service
   *            8. Check the callback data
   * @post_condition 1. Stop Provider Service
   *                 2. Stop Consumer Service
   * @expected 1. No exception occurs
   *           2. Should get the Notification in consumer side
   *           3. Should get syncInfo in Provider side
   */
  public void testConsumerServiceSendSyncInfoAsRead_SRC_P() {
    Consumer consumer = mHelper.getConsumer(true, false);

    assertNotNull("Consumer should not be null", consumer);

    try {
      consumer.acceptSubscription(true);
    } catch (NSException e) {
      assertTrue("Can't accept subscription", false);
    }

    mHelper.waitInSecond(WAIT_MIN_TIME);

    assertEquals("Can't send notification", true, mHelper.sendNotification());

    mHelper.waitInSecond(WAIT_DEFAULT_TIME);

    Message msg = mHelper.getNotification();

    assertNotNull("Doesn't get any notification", msg);

    assertEquals("Send SyncInfo failed",true, mHelper.sendSyncInfo(false, SyncType.READ));

    mHelper.waitInSecond(WAIT_DEFAULT_TIME);

    SyncInfo syncInfo = mHelper.getSyncInfo(false);

    assertNotNull("SyncInfo should not be null", syncInfo);
    assertEquals(SyncInfo.SyncType.READ, syncInfo.getState());
  }

  /**
   * @since 2016-10-17
   * @see int start(OnConsumerSubscribedListener subscribedListener,
   *      OnMessageSynchronizedListener messageSynchronized, boolean
   *      subControllability, String userInfo, boolean resourceSecurity)
   * @see int stop()
   * @see Message createMessage()
   * @see int registerTopic(String topicName)
   * @see int setTopic(String topicName)
   * @see void start(OnProviderDiscoveredListener onProviderDiscoveredListener)
   * @objective Test scenario based 'sendMessage' API functionality
   * @target int sendMessage(Message message)
   * @test_data SyncType = DELETED
   * @pre_condition 1. Get Provider Service Instance
   *                2. Start Provider with Provider acceptor
   *                3. Get Consumer Service Instance
   *                4. Start Consumer Service
   *                5. Wait for OnConsumerSubscribedListener
   *                6. Get Consumer object from callback
   *                7. Accept Subscription
   * @procedure 1. Register Topic
   *            2. Set Same topic which is registered
   *            3. Get Message instance with createMessage()
   *            4. Set Message Title, Body, SourceName and the TopicName and also the mediaContents
   *            5. Perform sendMessage() API
   *            6. Wait for onMessageReceived Listener
   *            7. Send SyncInfo to Provider Service
   *            8. Check the callback data
   * @post_condition 1. Stop Provider Service
   *                 2. Stop Consumer Service
   * @expected 1. No exception occurs
   *           2. Should get the Notification in consumer side
   *           3. Should get syncInfo in Provider side
   */
  public void testConsumerServiceSendSyncInfoAsDeleted_SRC_P() {
    Consumer consumer = mHelper.getConsumer(true, false);

    assertNotNull("Consumer should not be null", consumer);

    try {
      consumer.acceptSubscription(true);
    } catch (NSException e) {
      assertTrue("Can't accept subscription", false);
    }

    mHelper.waitInSecond(WAIT_MIN_TIME);

    assertEquals("Can't send notification", true, mHelper.sendNotification());

    mHelper.waitInSecond(WAIT_DEFAULT_TIME);

    Message msg = mHelper.getNotification();

    assertNotNull("Doesn't get any notification", msg);

    assertEquals("Send SyncInfo failed",true, mHelper.sendSyncInfo(false, SyncType.DELETED));

    mHelper.waitInSecond(WAIT_DEFAULT_TIME);

    SyncInfo syncInfo = mHelper.getSyncInfo(false);

    assertNotNull("SyncInfo should not be null", syncInfo);
    assertEquals(SyncInfo.SyncType.DELETED, syncInfo.getState());
  }

  /**
   * @since 2016-10-17
   * @see int start(OnConsumerSubscribedListener subscribedListener,
   *      OnMessageSynchronizedListener messageSynchronized, boolean
   *      subControllability, String userInfo, boolean resourceSecurity)
   * @see int stop()
   * @see Message createMessage()
   * @see int registerTopic(String topicName)
   * @see int setTopic(String topicName)
   * @see void start(OnProviderDiscoveredListener onProviderDiscoveredListener)
   * @objective Test scenario based 'sendMessage' API functionality
   * @target int sendMessage(Message message)
   * @test_data SyncType = UNREAD
   * @pre_condition 1. Get Provider Service Instance
   *                2. Start Provider with Provider acceptor
   *                3. Get Consumer Service Instance
   *                4. Start Consumer Service
   *                5. Wait for OnConsumerSubscribedListener
   *                6. Get Consumer object from callback
   *                7. Accept Subscription
   * @procedure 1. Register Topic
   *            2. Set Same topic which is registered
   *            3. Get Message instance with createMessage()
   *            4. Set Message Title, Body, SourceName and the TopicName and also the mediaContents
   *            5. Perform sendMessage() API
   *            6. Wait for onMessageReceived Listener
   *            7. Send SyncInfo to Provider Service
   *            8. Check the callback data
   * @post_condition 1. Stop Provider Service
   *                 2. Stop Consumer Service
   * @expected 1. No exception occurs
   *           2. Should get the Notification in consumer side
   *           3. Should get syncInfo in Provider side
   */
  public void testConsumerServiceSendSyncInfoAsUnread_SRC_P() {
    Consumer consumer = mHelper.getConsumer(true, false);

    assertNotNull("Consumer should not be null", consumer);

    try {
      consumer.acceptSubscription(true);
    } catch (NSException e) {
      assertTrue("Can't accept subscription", false);
    }

    mHelper.waitInSecond(WAIT_MIN_TIME);

    assertEquals("Can't send notification", true, mHelper.sendNotification());

    mHelper.waitInSecond(WAIT_DEFAULT_TIME);

    Message msg = mHelper.getNotification();

    assertNotNull("Doesn't get any notification", msg);

    assertEquals("Send SyncInfo failed",true, mHelper.sendSyncInfo(false, SyncType.UNREAD));

    mHelper.waitInSecond(WAIT_DEFAULT_TIME);

    SyncInfo syncInfo = mHelper.getSyncInfo(false);

    assertNotNull("SyncInfo should not be null", syncInfo);
    assertEquals(SyncInfo.SyncType.UNREAD, syncInfo.getState());
  }

  /**
   * @since 2016-10-17
   * @see int start(OnConsumerSubscribedListener subscribedListener,
   *      OnMessageSynchronizedListener messageSynchronized, boolean
   *      subControllability, String userInfo, boolean resourceSecurity)
   * @see int stop()
   * @see Message createMessage()
   * @see int registerTopic(String topicName)
   * @see int setTopic(String topicName)
   * @see void start(OnProviderDiscoveredListener onProviderDiscoveredListener)
   * @objective Test scenario based 'sendMessage' API functionality
   * @target int sendMessage(Message message)
   * @test_data SyncType = READ
   * @pre_condition 1. Get Provider Service Instance
   *                2. Start Provider with Provider acceptor
   *                3. Get Consumer Service Instance
   *                4. Start Consumer Service
   *                5. Wait for OnConsumerSubscribedListener
   *                6. Get Consumer object from callback
   *                7. Accept Subscription
   * @procedure 1. Register Topic
   *            2. Set Same topic which is registered
   *            3. Get Message instance with createMessage()
   *            4. Set Message Title, Body, SourceName and the TopicName and also the mediaContents
   *            5. Perform sendMessage() API
   *            6. Wait for onMessageReceived Listener
   *            7. Send SyncInfo to Consumer Service
   *            8. Check the callback data
   * @post_condition 1. Stop Provider Service
   *                 2. Stop Consumer Service
   * @expected 1. No exception occurs
   *           2. Should get the Notification in consumer side
   *           3. Should get syncInfo in Consumer side
   */
  public void testProviderServiceSendSyncInfoAsREAD_SRC_P() {
    Consumer consumer = mHelper.getConsumer(true, false);

    assertNotNull("Consumer should not be null", consumer);

    try {
      consumer.acceptSubscription(true);
    } catch (NSException e) {
      assertTrue("Can't accept subscription", false);
    }

    mHelper.waitInSecond(WAIT_MIN_TIME);

    assertEquals("Can't send notification", true, mHelper.sendNotification());

    mHelper.waitInSecond(WAIT_DEFAULT_TIME);

    Message msg = mHelper.getNotification();

    assertNotNull("Doesn't get any notification", msg);

    assertEquals("Send SyncInfo failed",true, mHelper.sendSyncInfo(true, SyncType.READ));

    mHelper.waitInSecond(WAIT_DEFAULT_TIME);

    SyncInfo syncInfo = mHelper.getSyncInfo(true);

    assertNotNull("SyncInfo should not be null", syncInfo);
    assertEquals(SyncType.READ, syncInfo.getState());
  }

  /**
   * @since 2016-10-17
   * @see int start(OnConsumerSubscribedListener subscribedListener,
   *      OnMessageSynchronizedListener messageSynchronized, boolean
   *      subControllability, String userInfo, boolean resourceSecurity)
   * @see int stop()
   * @see Message createMessage()
   * @see int registerTopic(String topicName)
   * @see int setTopic(String topicName)
   * @see void start(OnProviderDiscoveredListener onProviderDiscoveredListener)
   * @objective Test scenario based 'sendMessage' API functionality
   * @target int sendMessage(Message message)
   * @test_data SyncType = UNREAD
   * @pre_condition 1. Get Provider Service Instance
   *                2. Start Provider with Provider acceptor
   *                3. Get Consumer Service Instance
   *                4. Start Consumer Service
   *                5. Wait for OnConsumerSubscribedListener
   *                6. Get Consumer object from callback
   *                7. Accept Subscription
   * @procedure 1. Register Topic
   *            2. Set Same topic which is registered
   *            3. Get Message instance with createMessage()
   *            4. Set Message Title, Body, SourceName and the TopicName and also the mediaContents
   *            5. Perform sendMessage() API
   *            6. Wait for onMessageReceived Listener
   *            7. Send SyncInfo to Consumer Service
   *            8. Check the callback data
   * @post_condition 1. Stop Provider Service
   *                 2. Stop Consumer Service
   * @expected 1. No exception occurs
   *           2. Should get the Notification in consumer side
   *           3. Should not get syncInfo in Consumer side
   */
  public void testProviderServiceSendSyncInfoAsUNREAD_SRC_P() {
    Consumer consumer = mHelper.getConsumer(true, false);

    assertNotNull("Consumer should not be null", consumer);

    try {
      consumer.acceptSubscription(true);
    } catch (NSException e) {
      assertTrue("Can't accept subscription", false);
    }

    mHelper.waitInSecond(WAIT_MIN_TIME);

    assertEquals("Can't send notification", true, mHelper.sendNotification());

    mHelper.waitInSecond(WAIT_DEFAULT_TIME);

    Message msg = mHelper.getNotification();

    assertNotNull("Doesn't get any notification", msg);

    assertEquals("Send SyncInfo failed",true, mHelper.sendSyncInfo(true, SyncType.UNREAD));

    mHelper.waitInSecond(WAIT_DEFAULT_TIME);

    SyncInfo syncInfo = mHelper.getSyncInfo(true);

    assertNull("SyncInfo should be null", syncInfo);
  }

  /**
   * @since 2016-10-17
   * @see int start(OnConsumerSubscribedListener subscribedListener,
   *      OnMessageSynchronizedListener messageSynchronized, boolean
   *      subControllability, String userInfo, boolean resourceSecurity)
   * @see int stop()
   * @see Message createMessage()
   * @see int registerTopic(String topicName)
   * @see int setTopic(String topicName)
   * @see void start(OnProviderDiscoveredListener onProviderDiscoveredListener)
   * @objective Test scenario based 'sendMessage' API functionality
   * @target int sendMessage(Message message)
   * @test_data SyncType = DELETED
   * @pre_condition 1. Get Provider Service Instance
   *                2. Start Provider with Provider acceptor
   *                3. Get Consumer Service Instance
   *                4. Start Consumer Service
   *                5. Wait for OnConsumerSubscribedListener
   *                6. Get Consumer object from callback
   *                7. Accept Subscription
   * @procedure 1. Register Topic
   *            2. Set Same topic which is registered
   *            3. Get Message instance with createMessage()
   *            4. Set Message Title, Body, SourceName and the TopicName and also the mediaContents
   *            5. Perform sendMessage() API
   *            6. Wait for onMessageReceived Listener
   *            7. Send SyncInfo to Consumer Service
   *            8. Check the callback data
   * @post_condition 1. Stop Provider Service
   *                 2. Stop Consumer Service
   * @expected 1. No exception occurs
   *           2. Should get the Notification in consumer side
   *           3. Should get syncInfo in Consumer side
   */
  public void testProviderServiceSendSyncInfoAsDELETED_SRC_P() {
    Consumer consumer = mHelper.getConsumer(true, false);

    assertNotNull("Consumer should not be null", consumer);

    try {
      consumer.acceptSubscription(true);
    } catch (NSException e) {
      assertTrue("Can't accept subscription", false);
    }

    mHelper.waitInSecond(WAIT_MIN_TIME);

    assertEquals("Can't send notification", true, mHelper.sendNotification());

    mHelper.waitInSecond(WAIT_DEFAULT_TIME);

    Message msg = mHelper.getNotification();

    assertNotNull("Doesn't get any notification", msg);

    assertEquals("Send SyncInfo failed",true, mHelper.sendSyncInfo(true, SyncType.DELETED));

    mHelper.waitInSecond(WAIT_DEFAULT_TIME);

    SyncInfo syncInfo = mHelper.getSyncInfo(true);

    assertNotNull("SyncInfo should not be null", syncInfo);
    assertEquals(SyncType.DELETED, syncInfo.getState());
  }

  /**
   * @since 2016-10-20
   * @see int start(OnConsumerSubscribedListener subscribedListener,
   *      OnMessageSynchronizedListener messageSynchronized, boolean
   *      subControllability, String userInfo, boolean resourceSecurity)
   * @see int stop()
   * @see Message createMessage()
   * @see int registerTopic(String topicName)
   * @see void start(OnProviderDiscoveredListener onProviderDiscoveredListener)
   * @objective Test scenario based 'sendMessage' API functionality
   * @target int sendMessage(Message message)
   * @test_data None
   * @pre_condition 1. Get Provider Service Instance
   *                2. Start Provider with Provider acceptor
   *                3. Register Topic with topicName = "OCF_TestTopic"
   *                4. Get Consumer Service Instance
   *                5. Start Consumer Service
   *                6. Wait for OnConsumerSubscribedListener
   *                7. Get Consumer object from callback
   *                8. Accept Subscription
   * @procedure 1. Get the topic list
   *            2. Check the topic state
   * @post_condition 1. Stop Provider Service
   *                 2. Stop Consumer Service
   * @expected 1. No exception occurs
   *           2. Topic state should be UNSUBSCRIBED at initial stage
   */
  public void testTopicState_SRC_P() {
    Consumer consumer = mHelper.getConsumer(true, true);

    assertNotNull("Consumer should not be null", consumer);

    try {
      consumer.acceptSubscription(true);
    } catch (NSException e) {
      assertTrue("Can't accept subscription", false);
    }

    mHelper.waitInSecond(WAIT_MIN_TIME);

    try {
      Vector<Topic> topics = consumer.getConsumerTopicList().getTopicsList();
      Topic topic = topics.get(0);
      assertEquals("Topic should be UNSUBSCRIBED state after unsetTopic().", TopicState.UNSUBSCRIBED, topic.getState());
    } catch (NSException e) {
      assertTrue("Can't get consumerTopicList. Exception: " + e.getLocalizedMessage(), false);
    }
  }

  /**
   * @since 2016-10-20
   * @see int start(OnConsumerSubscribedListener subscribedListener,
   *      OnMessageSynchronizedListener messageSynchronized, boolean
   *      subControllability, String userInfo, boolean resourceSecurity)
   * @see int stop()
   * @see Message createMessage()
   * @see int registerTopic(String topicName)
   * @see void start(OnProviderDiscoveredListener onProviderDiscoveredListener)
   * @objective Test scenario based 'sendMessage' API functionality
   * @target int sendMessage(Message message)
   * @test_data None
   * @pre_condition 1. Get Provider Service Instance
   *                2. Start Provider with Provider acceptor
   *                3. Register Topic with topicName = "OCF_TestTopic"
   *                4. Get Consumer Service Instance
   *                5. Start Consumer Service
   *                6. Wait for OnConsumerSubscribedListener
   *                7. Get Consumer object from callback
   *                8. Accept Subscription
   * @procedure 1. SetTopic for the topicName = "OCF_TestTopic"
   *            2. Get the topic list
   *            3. Check the topic state
   *            4. Unset the topic
   *            5. Get and check the topic state
   * @post_condition 1. Stop Provider Service
   *                 2. Stop Consumer Service
   * @expected 1. No exception occurs
   *           2. Topic state should be SUBSCRIBED state after set
   *           3. Topic state should be UNSUBSCRIBED state after unset
   */
  public void testSetUnsetTopic_SQV_P() {
    Consumer consumer = mHelper.getConsumer(true, true);

    assertNotNull("Consumer should not be null", consumer);

    try {
      consumer.acceptSubscription(true);
    } catch (NSException e) {
      assertTrue("Can't accept subscription", false);
    }

    mHelper.waitInSecond(WAIT_MIN_TIME);

    try {
      consumer.setTopic(TOPIC_NAME);
      Vector<Topic> topics = consumer.getConsumerTopicList().getTopicsList();

      Topic topic = topics.get(0);
      assertEquals("Topic should be SUBSCRIBED state after setTopic().", TopicState.SUBSCRIBED, topic.getState());

      consumer.unsetTopic(TOPIC_NAME);
      topics = consumer.getConsumerTopicList().getTopicsList();

      topic = topics.get(0);
      assertEquals("Topic should be UNSUBSCRIBED state after unsetTopic().", TopicState.UNSUBSCRIBED, topic.getState());
    } catch (NSException e) {
      assertTrue("Can't get consumerTopicList. Exception: " + e.getLocalizedMessage(), false);
    }
  }
}
