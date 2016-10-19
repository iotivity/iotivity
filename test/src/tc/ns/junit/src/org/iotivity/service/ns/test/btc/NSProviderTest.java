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

package org.iotivity.service.ns.test.btc;

import org.iotivity.service.ns.common.*;
import org.iotivity.service.ns.common.SyncInfo.SyncType;
import org.iotivity.service.ns.provider.Consumer;
import org.iotivity.service.ns.provider.ProviderService;
import org.iotivity.service.ns.test.helper.NSHelper;

import static org.iotivity.service.ns.test.helper.NSTestUtilily.*;
import android.test.InstrumentationTestCase;

public class NSProviderTest extends InstrumentationTestCase {
  private NSHelper        mHelper          = null;
  private ProviderService mProviderService = null;

  protected void setUp() throws Exception {
    super.setUp();

    mHelper = NSHelper.getInstance(getInstrumentation().getTargetContext());
    mProviderService = ProviderService.getInstance();
  }

  protected void tearDown() throws Exception {
    super.tearDown();

    mHelper.stopServices();

    mProviderService = null;
  }

  /**
   * @since 2016-09-24
   * @see none
   * @objective Test 'getInstance' function with positive basic way
   * @target public static ProviderService getInstance()
   * @test_data None
   * @pre_condition None
   * @procedure Perform getInstance() API
   * @post_condition None
   * @expected No exception occurs
   */
  public void testGetProviderInstance_SRC_P() {
    try {
      ProviderService.getInstance();
    } catch (Exception e) {
      fail("Exception occurs when try to Get Provider Instance. Exception: "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2016-09-24
   * @see none
   * @objective Test 'start' function with positive basic way
   * @target int start(OnConsumerSubscribedListener subscribedListener,
   *         OnMessageSynchronizedListener messageSynchronized, boolean
   *         subControllability, String userInfo) throws NSException
   * @test_data subscribedListener, messageSynchronizedListener,
   *            subControllability = true, userInfo = "Info"
   * @pre_condition Get Provider Instance
   * @procedure Perform start() API with test data
   * @post_condition None
   * @expected 1. No exception occurs 2. Result should be NS_OK (100)
   */
  public void testStartProviderWithProviderAcceptor_SRC_P() {
    try {
      int result = mProviderService.start(mHelper, mHelper, true, USER_INFO, IS_SECURED);

      assertEquals("Should return NS_OK (100)", NS_OK, result);
    } catch (NSException e) {
      fail("Exception occurs when calling start() API. Exception: "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2016-09-24
   * @see none
   * @objective Test 'start' function with positive basic way
   * @target int start(OnConsumerSubscribedListener subscribedListener,
   *         OnMessageSynchronizedListener messageSynchronized, boolean
   *         subControllability, String userInfo) throws NSException
   * @test_data subscribedListener, messageSynchronizedListener,
   *            subControllability = true, userInfo = "Info"
   * @pre_condition Get Provider Instance
   * @procedure Perform start() API with test data
   * @post_condition None
   * @expected 1. No exception occurs 2. Result should be NS_OK (100)
   */
  public void testStartProviderWithConsumerAcceptor_SRC_P() {
    try {
      int result = mProviderService.start(mHelper, mHelper, false, USER_INFO, IS_SECURED);

      assertEquals(NS_OK, result);
    } catch (NSException e) {
      fail("Exception occurs when calling start() API. Exception: "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2016-09-24
   * @see none
   * @objective Test 'start' function with positive basic way
   * @target int start(OnConsumerSubscribedListener subscribedListener,
   *         OnMessageSynchronizedListener messageSynchronized, boolean
   *         subControllability, String userInfo) throws NSException
   * @test_data subscribedListener, messageSynchronizedListener,
   *            subControllability = true, userInfo = "Info"
   * @pre_condition Get Provider Instance
   * @procedure Perform start() API with test data
   * @post_condition None
   * @expected Should throw exception
   */
  public void testStartProviderSubscribedCB_NV_N() {
    try {
      mProviderService.start(null, mHelper, true, USER_INFO, IS_SECURED);

      fail("Successfully called start() API with null Subscribed CB");
    } catch (NSException e) {
    }
  }

  /**
   * @since 2016-09-24
   * @see none
   * @objective Test 'start' function with positive basic way
   * @target int start(OnConsumerSubscribedListener subscribedListener,
   *         OnMessageSynchronizedListener messageSynchronized, boolean
   *         subControllability, String userInfo) throws NSException
   * @test_data subscribedListener, messageSynchronizedListener = null,
   *            subControllability = true, userInfo = "Info"
   * @pre_condition Get Provider Instance
   * @procedure Perform start() API with test data
   * @post_condition None
   * @expected Exception should occurs
   */
  public void testStartProviderSynchronizedCB_NV_N() {
    try {
      mProviderService.start(mHelper, null, true, USER_INFO, IS_SECURED);

      fail("Successfully called start() API with null Subscribed CB");
    } catch (NSException e) {
    }
  }

  /**
   * @since 2016-09-24
   * @see none
   * @objective Test 'stop' function with positive basic way
   * @target int stop() throws NSException
   * @test_data None
   * @pre_condition Get Provider Instance
   * @procedure Perform stop() API
   * @post_condition None
   * @expected 1. No exception occurs 2. Result should be NS_OK (100)
   */
  public void testStopProvider_SRC_P() {
    try {
      int result = mProviderService.stop();

      assertEquals("Should return NS_OK (100)", NS_OK, result);
    } catch (NSException e) {
      fail("Successfully called stop() API with null Subscribed CB");
    }
  }

  /**
   * @since 2016-09-24
   * @see none
   * @objective Test 'createMessage' function with positive basic way
   * @target Message createMessage() throws NSException
   * @test_data None
   * @pre_condition Get Provider Instance
   * @procedure Perform createMessage() API
   * @post_condition None
   * @expected 1. No exception occurs 2. Should not return null Message
   */
  public void testCreateMessage_SRC_P() {
    try {
      assertNotNull("Message should not be null",
          mProviderService.createMessage());
    } catch (NSException e) {
      fail("Fail to Create Message. Exception: " + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2016-09-24
   * @see none
   * @objective Test 'sendMessage' function with positive basic way
   * @target int sendMessage(Message message) throws NSException
   * @test_data Message
   * @pre_condition 1. Get Provider Instance 2. Create Message
   * @procedure Perform sendMessage() API
   * @post_condition None
   * @expected 1. No exception occurs 2. Should return NS_OK (100)
   */
  public void testSendMessage_SRC_P() {
    Message msg = null;

    try {
      msg = mProviderService.createMessage();
      msg.mTitle = MSG_TITLE;
      msg.mContentText = MSG_BODY;
      msg.mSourceName = MSG_SOURCE;
      msg.mTopic = TOPIC_NAME;

      MediaContents mediaContents = new MediaContents("");

      msg.mMediaContents = mediaContents;
    } catch (NSException e) {
      assertTrue("Message Creation Failed", false);
    }

    try {
      mProviderService.start(mHelper, mHelper, true, USER_INFO, IS_SECURED);
      int result = mProviderService.sendMessage(msg);

      assertEquals("Should return NS_OK (100)", NS_OK, result);
    } catch (NSException e) {
      fail("Exception occurs when calling sendMessage() API");
    }
  }

  /**
   * @since 2016-09-24
   * @see none
   * @objective Test 'sendMessage' function with positive basic way
   * @target int sendMessage(Message message) throws NSException
   * @test_data null
   * @pre_condition Get Provider Instance
   * @procedure Perform sendMessage() API
   * @post_condition None
   * @expected NSException should occur
   */
  public void testSendMessage_NV_N() {
    try {
      mProviderService.sendMessage(null);

      fail("Successfully called sendMessage() with null message");
    } catch (NSException e) {
    }
  }

  /**
   * @since 2016-09-24
   * @see none
   * @objective Test 'sendMessage' function with positive basic way
   * @target int sendMessage(Message message) throws NSException
   * @test_data messageId, SyncType
   * @pre_condition 1. Get Provider Instance 2. Create Message and get messageId
   * @procedure Perform sendSyncInfo() API
   * @post_condition None
   * @expected No exception occurs
   */
  public void testSendSyncInfo_SRC_P() {
    Message msg = null;

    try {
      msg = mProviderService.createMessage();
    } catch (NSException e) {
      assertTrue("Message Creation Failed", false);
    }

    try {
      mProviderService.sendSyncInfo(msg.mMessageId, SyncType.READ);
    } catch (NSException e) {
      fail("Exception occurs when calling sendSyncInfo() API");
    }
  }

  /**
   * @since 2016-09-24
   * @see none
   * @objective Test 'registerTopic' function with positive basic way
   * @target int registerTopic(String topicName) throws NSException
   * @test_data topicName = "TestTopic"
   * @pre_condition Get Provider Instance
   * @procedure 1. Perform Start Provider Service
   *            2. Perform registerTopic() API
   * @post_condition None
   * @expected 1. No exception occurs 2. Should return NS_OK (100)
   */
  public void testRegisterTopic_SRC_P() {
    try {
      mProviderService.start(mHelper, mHelper, true, USER_INFO, IS_SECURED);
      assertEquals(100, mProviderService.registerTopic(TOPIC_NAME));
    } catch (NSException e) {
      fail("Exception occurs when calling registerTopic() API");
    }
  }

  /**
   * @since 2016-09-24
   * @see none
   * @objective Test 'registerTopic' function with positive basic way
   * @target int registerTopic(String topicName) throws NSException
   * @test_data topicName = null
   * @pre_condition Get Provider Instance
   * @procedure Perform registerTopic() API
   * @post_condition None
   * @expected NSException should occur
   */
  public void testRegisterTopic_NV_N() {
    try {
      mProviderService.registerTopic(null);

      fail("Successfully called registerTopic() with null");
    } catch (NSException e) {
    }
  }

  /**
   * @since 2016-09-24
   * @see none
   * @objective Test 'registerTopic' function with positive basic way
   * @target int registerTopic(String topicName) throws NSException
   * @test_data topicName = "TestTopic"
   * @pre_condition 1. Get Provider Instance
   *                2. Perform Start Provider
   *                3. Call registerTopic() API with test data
   * @procedure Perform unregisterTopic() API
   * @post_condition None
   * @expected 1. No exception occurs 2. Should return NS_OK (100)
   */
  public void testUnregisterTopic_SRC_P() {
    try {
      mProviderService.start(mHelper, mHelper, true, USER_INFO, IS_SECURED);
      mProviderService.registerTopic(TOPIC_NAME);

      assertEquals("Should return NS_OK (100)", NS_OK,
          mProviderService.unregisterTopic(TOPIC_NAME));
    } catch (NSException e) {
      fail("Exception occurs when calling unregisterTopic() API");
    }
  }

  /**
   * @since 2016-09-24
   * @see none
   * @objective Test 'registerTopic' function with positive basic way
   * @target int registerTopic(String topicName) throws NSException
   * @test_data topicName = "TestTopic"
   * @pre_condition 1. Get Provider Instance
   *                2. Start Provider Service
   * @procedure Perform unregisterTopic() API
   * @post_condition None
   * @expected 1. No exception occurs 2. Should not return NS_OK (100)
   */
  public void testUnregisterTopic_USV_N() {
    try {
      mProviderService.start(mHelper, mHelper, true, USER_INFO, IS_SECURED);
      assertNotSame(
          "Should not return NS_OK when the topic is not registered.", NS_OK,
          mProviderService.unregisterTopic(TOPIC_NAME));
    } catch (NSException e) {
      fail("Exception occurs when calling unregisterTopic() API");
    }
  }

  /**
   * @since 2016-09-24
   * @see none
   * @objective Test 'registerTopic' function with positive basic way
   * @target int registerTopic(String topicName) throws NSException
   * @test_data topicName = null
   * @pre_condition Get Provider Instance
   * @procedure Perform registerTopic() API
   * @post_condition None
   * @expected 1. No exception occurs
   */
  public void testUnregisterTopic_NV_N() {
    try {
      mProviderService.unregisterTopic(null);

      fail("Successfully called unregisterTopic() with null");
    } catch (NSException e) {
    }
  }

  /**
   * @since 2016-09-24
   * @see none
   * @objective Test 'getRegisteredTopicList' function with positive basic way
   * @target TopicsList getRegisteredTopicList() throws NSException
   * @test_data None
   * @pre_condition 1. Get Provider Instance 2. Start Provider 3. Register a
   *                topic
   * @procedure Perform getRegisteredTopicList() API
   * @post_condition None
   * @expected 1. No exception occurs 2. Topic list should not be empty
   */
  public void testGetRegisteredTopicList_SRC_P() {
    try {
      mProviderService.start(mHelper, mHelper, true, USER_INFO, IS_SECURED);
      mProviderService.registerTopic(TOPIC_NAME);

      TopicsList topicList = mProviderService.getRegisteredTopicList();

      assertNotSame("TopicList should not empty", 0,
          topicList.mTopicsList.size());
    } catch (NSException e) {
      fail("Exception occurs when calling getRegisteredTopicList() API");
    }
  }

  /**
   * @since 2016-09-28
   * @see none
   * @objective Test 'getConsumerId()' function with positive basic way
   * @target String getConsumerId()
   * @test_data None
   * @pre_condition 1. Get Provider Service Instance 2. Start Provider with
   *                provider acceptor 3. Get Consumer Service Instance 4. Start
   *                Consumer Service 5. Wait for Provider Discovered Callback 6.
   *                Get Provider from callback 7. Subscribe the Consumer Service
   *                with that provider object 8. Wait for Consumer Subscribed
   *                Callback 9. Get the Consumer from callback
   * @procedure Perform getConsumerId() API
   * @post_condition None
   * @expected 1. No exception occurs 2. Should get the consumer id which is not
   *           empty
   */
  public void testGetConsumerID_SRC_P() {
    Consumer consumer = mHelper.getConsumer(true, false);

    assertNotNull("Can't get any consumer", consumer);

    try {
      assertNotSame("ConsumerID should not be empty", 0, consumer
          .getConsumerId().length());
    } catch (Exception e) {
      fail("Exception occurs when calling getConsumerId() API. Exception is : "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2016-09-28
   * @see none
   * @objective Test 'acceptSubscription()' function with positive basic way
   * @target int acceptSubscription(boolean accepted) throws NSException
   * @test_data accepted = true
   * @pre_condition 1. Get Provider Service Instance
   *                2. Start Provider with provider acceptor
   *                3. Get Consumer Service Instance
   *                4. Start Consumer Service
   *                5. Wait for Provider Discovered Callback
   *                6. Get Provider from callback
   *                7. Subscribe the Consumer Service with that provider object
   *                8. Wait for Consumer Subscribed Callback
   *                9. Get the Consumer from callback
   * @procedure Perform acceptSubscription() API with test_data
   * @post_condition None
   * @expected 1. No exception occurs 2. Should return NS_OK (100)
   */
  public void testAcceptSubscription_SRC_P() {
    Consumer consumer = mHelper.getConsumer(true, false);

    assertNotNull("Can't get any consumer", consumer);

    try {
      assertEquals("Should throw NS_OK", NS_OK, consumer.acceptSubscription(true));
    } catch (Exception e) {
      fail("Exception occurs when calling acceptSubscription() API. Exception is : "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2016-09-28
   * @see none
   * @objective Test 'acceptSubscription()' function with positive basic way
   * @target int acceptSubscription(boolean accepted) throws NSException
   * @test_data accepted = false
   * @pre_condition 1. Get Provider Service Instance
   *                2. Start Provider with provider acceptor
   *                3. Get Consumer Service Instance
   *                4. Start Consumer Service
   *                5. Wait for Provider Discovered Callback
   *                6. Get Provider from callback
   *                7. Subscribe the Consumer Service with that provider object
   *                8. Wait for Consumer Subscribed Callback
   *                9. Get the Consumer from callback
   * @procedure Perform acceptSubscription() API
   * @post_condition None
   * @expected 1. No exception occurs 2. Should return NS_OK (100)
   */
  public void testRejectSubscription_SRC_P() {
    Consumer consumer = mHelper.getConsumer(true, false);

    assertNotNull("Can't get any consumer", consumer);

    try {
      assertEquals("Should throw NS_OK", NS_OK, consumer.acceptSubscription(false));
    } catch (Exception e) {
      fail("Exception occurs when calling acceptSubscription() API. Exception is : "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2016-09-28
   * @see none
   * @objective Test 'setTopic()' function with positive basic way
   * @target int setTopic(String topicName) throws NSException
   * @test_data topicName = "TestTopic"
   * @pre_condition 1. Get Provider Service Instance
   *                2. Start Provider with provider acceptor
   *                3. Register Topic with topicName = "TestTopic"
   *                4. Get Consumer Service Instance
   *                5. Start Consumer Service
   *                6. Wait for Provider Discovered Callback
   *                7. Get Provider from callback
   *                8. Subscribe the Consumer Service with that provider object
   *                9. Wait for Consumer Subscribed Callback
   *                10. Get the Consumer from callback
   * @procedure Perform setTopic() API
   * @post_condition None
   * @expected 1. No exception occurs 2. Should return NS_OK (100)
   */
  public void testSetTopic_SRC_P() {
    Consumer consumer = mHelper.getConsumer(true, true);

    assertNotNull("Can't get any consumer", consumer);

    try {
      assertEquals("Should return NS_OK", NS_OK, consumer.setTopic(TOPIC_NAME));
    } catch (Exception e) {
      fail("Exception occurs when calling setTopic() API. Exception is : "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2016-10-06
   * @see none
   * @objective Test 'setTopic()' function with positive basic way
   * @target int setTopic(String topicName) throws NSException
   * @test_data topicName = "TestTopic"
   * @pre_condition 1. Get Provider Service Instance
   *                2. Start Provider with provider acceptor
   *                3. Get Consumer Service Instance
   *                4. Start Consumer Service
   *                5. Wait for Provider Discovered Callback
   *                6. Get Provider from callback
   *                7. Subscribe the Consumer Service with that provider object
   *                8. Wait for Consumer Subscribed Callback
   *                9. Get the Consumer from callback
   * @procedure Perform setTopic() API
   * @post_condition None
   * @expected 1. No exception occurs 2. Should not return NS_OK (100)
   */
  public void testSetTopic_USV_N() {
    Consumer consumer = mHelper.getConsumer(true, false);

    assertNotNull("Can't get any consumer", consumer);

    try {
      assertNotSame("Should not return NS_OK", NS_OK, consumer.setTopic(TOPIC_NAME));
    } catch (Exception e) {
      fail("Exception occurs when calling setTopic() API. Exception is : "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2016-09-28
   * @see none
   * @objective Test 'unsetTopic()' function with positive basic way
   * @target int unsetTopic(String topicName) throws NSException
   * @test_data topicName = "TestTopic"
   * @pre_condition 1. Get Provider Service Instance
   *                2. Start Provider with provider acceptor
   *                3. Get Consumer Service Instance
   *                4. Start Consumer Service
   *                5. Wait for Provider Discovered Callback
   *                6. Get Provider from callback
   *                7. Subscribe the Consumer Service with that provider object
   *                8. Wait for Consumer Subscribed Callback
   *                9. Get the Consumer from callback
   * @procedure 1. Perform setTopic() API with test_data
   *            2. Perform unsetTopic() API with same topicName
   * @post_condition None
   * @expected 1. No exception occurs 2. Should return NS_OK (100)
   */
  public void testUnsetTopic_SRC_P() {
    Consumer consumer = mHelper.getConsumer(true, true);

    assertNotNull("Can't get any consumer", consumer);

    try {
      consumer.setTopic(TOPIC_NAME);
      assertEquals("Should throw NS_OK", NS_OK, consumer.unsetTopic(TOPIC_NAME));
    } catch (Exception e) {
      fail("Exception occurs when calling unsetTopic() API. Exception is : "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2016-09-30
   * @see none
   * @objective Test 'unsetTopic()' function with positive basic way
   * @target int unsetTopic(String topicName) throws NSException
   * @test_data topicName = "TestTopic"
   * @pre_condition 1. Get Provider Service Instance
   *                2. Start Provider with provider acceptor
   *                3. Get Consumer Service Instance
   *                4. Start Consumer Service
   *                5. Wait for Provider Discovered Callback
   *                6. Get Provider from callback
   *                7. Subscribe the Consumer Service with that provider object
   *                8. Wait for Consumer Subscribed Callback
   *                9. Get the Consumer from callback
   * @procedure 1. Perform setTopic() API with test_data
   *            2. Perform unsetTopic() API with same topicName
   * @post_condition None
   * @expected Should not return NS_OK (100)
   */
  public void testUnsetTopic_USV_N() {
    Consumer consumer = mHelper.getConsumer(true, true);

    assertNotNull("Can't get any consumer", consumer);

    try {
      assertNotSame("Should not return NS_OK", NS_OK, consumer.unsetTopic(TOPIC_NAME));
    } catch (Exception e) {
    }
  }

  /**
   * @since 2016-09-29
   * @see none
   * @objective Test 'getConsumerTopicList()' function with positive basic way
   * @target TopicsList getConsumerTopicList() throws NSException
   * @test_data None
   * @pre_condition 1. Get Provider Service Instance
   *                2. Start Provider with consumer acceptor
   *                3. Get Consumer Service Instance
   *                4. Start Consumer Service
   *                5. Wait for Provider Discovered Callback
   *                6. Get Provider from callback
   *                7. Subscribe the Consumer Service with that provider object
   *                8. Wait for Consumer Subscribed Callback
   *                9. Get the Consumer from callback
   *                10. Register Topic with topicName = "TestTopic"
   * @procedure Perform getConsumerTopicList() API
   * @post_condition None
   * @expected 1. No exception occurs 2. Should return TopicList
   */
  public void testGetConsumerTopicList_SRC_P() {
    Consumer consumer = mHelper.getConsumer(false, true);

    assertNotNull("Can't get any consumer", consumer);

    try {
      TopicsList topicList = consumer.getConsumerTopicList();
      assertNotNull("TopicList should not be null", topicList);
      assertNotSame("TopicList should not be empty", 0, topicList.mTopicsList.size());
    } catch (NSException e) {
      fail("Exception occurs when calling getConsumerTopicList() API. Exception is : "
          + e.getLocalizedMessage());
    }
  }
}
