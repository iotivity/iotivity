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

import org.iotivity.service.ns.common.MediaContents;
import org.iotivity.service.ns.common.Message;
import org.iotivity.service.ns.common.Message.MessageType;
import org.iotivity.service.ns.common.SyncInfo;
import org.iotivity.service.ns.common.SyncInfo.SyncType;
import org.iotivity.service.ns.common.Topic;
import org.iotivity.service.ns.common.Topic.TopicState;
import org.iotivity.service.ns.common.TopicsList;

import static org.iotivity.service.ns.test.helper.NSTestUtilily.*;
import android.test.InstrumentationTestCase;
import android.util.Log;

public class NSUtilityTest extends InstrumentationTestCase {
  private Message             mMessage = null;
  private static final String ICON_IMG = "media/img.jpg";

  protected void setUp() throws Exception {
    super.setUp();

    mMessage = new Message(MSG_TITLE, MSG_BODY, MSG_SOURCE);
  }

  protected void tearDown() throws Exception {
    super.tearDown();

    mMessage = null;
  }

  /**
   * @since 2016-10-03
   * @see none
   * @objective Test 'Message()' function with positive basic way
   * @target Message(String title, String contentText, String sourceName)
   * @test_data title = "Test Title", contentTest = "Test Message Body",
   *            sourceName = "OCF"
   * @pre_condition None
   * @procedure Perform Message() API
   * @post_condition None
   * @expected No exception occurs
   */
  public void testMessageInstance_SRC_P() {
    try {
      new Message(MSG_TITLE, MSG_BODY, MSG_SOURCE);
    } catch (Exception e) {
      fail("Fail to construct Message Class");
    }
  }

  /**
   * @since 2016-10-03
   * @see none
   * @objective Test 'getMessageId()' function with positive basic way
   * @target long getMessageId()
   * @test_data None
   * @pre_condition Construct Message class with title = "Test Title",
   *                contentTest = "Test Message Body", sourceName = "OCF"
   * @procedure Perform getMessageId() API
   * @post_condition None
   * @expected Should get messageId = 0
   */
  public void testGetMessageId_SRC_P() {
    assertEquals("Message ID should be 0 at initial stage", 0,
        mMessage.getMessageId());
  }

  /**
   * @since 2016-10-03
   * @see none
   * @objective Test 'getProviderId()' function with positive basic way
   * @target String getProviderId()
   * @test_data None
   * @pre_condition Construct Message class with title = "Test Title",
   *                contentTest = "Test Message Body", sourceName = "OCF"
   * @procedure Perform getProviderId() API
   * @post_condition None
   * @expected Should get providerId = null
   */
  public void testGetProviderId_SRC_P() {
    assertNull("Provider ID should be null at initial stage",
        mMessage.getProviderId());
  }

  /**
   * @since 2016-10-03
   * @see none
   * @objective Test 'getSourceName()' function with positive basic way
   * @target String getSourceName()
   * @test_data None
   * @pre_condition Construct Message class with title = "Test Title",
   *                contentTest = "Test Message Body", sourceName = "OCF"
   * @procedure Perform getSourceName() API
   * @post_condition None
   * @expected Should get sourceName = OCF
   */
  public void testGetSourceName_SRC_P() {
    assertEquals("Source should be OCF", MSG_SOURCE, mMessage.getSourceName());
  }

  /**
   * @since 2016-10-03
   * @see none
   * @objective Test 'getType()' function with positive basic way
   * @target MessageType getType()
   * @test_data None
   * @pre_condition Construct Message class with title = "Test Title",
   *                contentTest = "Test Message Body", sourceName = "OCF"
   * @procedure Perform getType() API
   * @post_condition None
   * @expected Should get Message type should be ALERT, EVENT, INFO or NOTICE
   */
  public void testGetType_SRC_P() {
    MessageType type = mMessage.getType();

    if (type == MessageType.ALERT || type == MessageType.EVENT
        || type == MessageType.INFO || type == MessageType.NOTICE) {
      Log.i(TAG, "Got Message type");
    } else {
      fail("Message type should be ALERT, EVENT, INFO or NOTICE");
    }
  }

  /**
   * @since 2016-10-03
   * @see none
   * @objective Test 'getTime()' function with positive basic way
   * @target String getTime()
   * @test_data None
   * @pre_condition Construct Message class with title = "Test Title",
   *                contentTest = "Test Message Body", sourceName = "OCF"
   * @procedure Perform getTime() API
   * @post_condition None
   * @expected Should get null as Time
   */
  public void testGetTime_SRC_P() {
    assertNull("Time should be null at initial stage", mMessage.getTime());
  }

  /**
   * @since 2016-10-03
   * @see none
   * @objective Test 'getTTL()' function with positive basic way
   * @target long getTTL()
   * @test_data None
   * @pre_condition Construct Message class with title = "Test Title",
   *                contentTest = "Test Message Body", sourceName = "OCF"
   * @procedure Perform getTTL() API
   * @post_condition None
   * @expected Should get 0 as TTL
   */
  public void testGetTTL_SRC_P() {
    assertEquals("TTL should be 0 at initial stage", 0, mMessage.getTTL());
  }

  /**
   * @since 2016-10-03
   * @see none
   * @objective Test 'getTitle()' function with positive basic way
   * @target String getTitle()
   * @test_data None
   * @pre_condition Construct Message class with title = "Test Title",
   *                contentTest = "Test Message Body", sourceName = "OCF"
   * @procedure Perform getTitle() API
   * @post_condition None
   * @expected Should get title = Test Title
   */
  public void testGetTitle_SRC_P() {
    assertEquals("Source should be OCF", MSG_TITLE, mMessage.getTitle());
  }

  /**
   * @since 2016-10-03
   * @see none
   * @objective Test 'getContentText()' function with positive basic way
   * @target String getContentText()
   * @test_data None
   * @pre_condition Construct Message class with title = "Test Title",
   *                contentTest = "Test Message Body", sourceName = "OCF"
   * @procedure Perform getContentText() API
   * @post_condition None
   * @expected Should get contentText = Test Message Body
   */
  public void testGetContentText_SRC_P() {
    assertEquals("Source should be OCF", MSG_BODY, mMessage.getContentText());
  }

  /**
   * @since 2016-10-03
   * @see none
   * @objective Test 'getMediaContents()' function with positive basic way
   * @target MediaContents getMediaContents()
   * @test_data None
   * @pre_condition Construct Message class with title = "Test Title",
   *                contentTest = "Test Message Body", sourceName = "OCF"
   * @procedure Perform getMediaContents() API
   * @post_condition None
   * @expected Should get MediaContents = null
   */
  public void testGetMediaContents_SRC_P() {
    assertNull("MediaContents should be null at initial stage",
        mMessage.getMediaContents());
  }

  /**
   * @since 2016-10-03
   * @see none
   * @objective Test 'getTopic()' function with positive basic way
   * @target String getTopic()
   * @test_data None
   * @pre_condition Construct Message class with title = "Test Title",
   *                contentTest = "Test Message Body", sourceName = "OCF"
   * @procedure Perform getTopic() API
   * @post_condition None
   * @expected Should get Topic = null
   */
  public void testGetTopic_SRC_P() {
    assertNull("Topic should be null at initial stage", mMessage.getTopic());
  }

  /**
   * @since 2016-10-03
   * @see none
   * @objective Test 'getSourceName()' function with positive basic way
   * @target String getSourceName()
   * @test_data sourceName = "TEST"
   * @pre_condition Construct Message class with title = "Test Title",
   *                contentTest = "Test Message Body", sourceName = "OCF"
   * @procedure 1. Perform setSourceName() with "TEST" 2. Perform
   *            getSourceName()
   * @post_condition None
   * @expected Should get sourceName = TEST
   */
  public void testSetSourceName_SRC_P() {
    String source = "TEST";

    mMessage.setSourceName(source);

    assertEquals("Source should be TEST", source, mMessage.getSourceName());
  }

  /**
   * @since 2016-10-03
   * @see MessageType getType()
   * @objective Test 'setType()' function with positive basic way
   * @target void setType()
   * @test_data MessageType = ALERT
   * @pre_condition Construct Message class with title = "Test Title",
   *                contentTest = "Test Message Body", sourceName = "OCF"
   * @procedure 1. Perform setType() API with ALERT 2. Perform getType() API
   * @post_condition None
   * @expected Should get Message type should be ALERT
   */
  public void testSetTypeAsAlert_SRC_P() {
    mMessage.setType(MessageType.ALERT);

    assertEquals("Message type should be ALERT", MessageType.ALERT,
        mMessage.getType());
  }

  /**
   * @since 2016-10-03
   * @see MessageType getType()
   * @objective Test 'setType()' function with positive basic way
   * @target void setType()
   * @test_data MessageType = EVENT
   * @pre_condition Construct Message class with title = "Test Title",
   *                contentTest = "Test Message Body", sourceName = "OCF"
   * @procedure 1. Perform setType() API with ALERT 2. Perform getType() API
   * @post_condition None
   * @expected Should get Message type should be EVENT
   */
  public void testSetTypeAsEvent_SRC_P() {
    mMessage.setType(MessageType.EVENT);

    assertEquals("Message type should be EVENT", MessageType.EVENT,
        mMessage.getType());
  }

  /**
   * @since 2016-10-03
   * @see MessageType getType()
   * @objective Test 'setType()' function with positive basic way
   * @target void setType()
   * @test_data MessageType = INFO
   * @pre_condition Construct Message class with title = "Test Title",
   *                contentTest = "Test Message Body", sourceName = "OCF"
   * @procedure 1. Perform setType() API with INFO 2. Perform getType() API
   * @post_condition None
   * @expected Should get Message type should be INFO
   */
  public void testSetTypeAsInfo_SRC_P() {
    mMessage.setType(MessageType.INFO);

    assertEquals("Message type should be INFO", MessageType.INFO,
        mMessage.getType());
  }

  /**
   * @since 2016-10-03
   * @see MessageType getType()
   * @objective Test 'setType()' function with positive basic way
   * @target void setType()
   * @test_data MessageType = NOTICE
   * @pre_condition Construct Message class with title = "Test Title",
   *                contentTest = "Test Message Body", sourceName = "OCF"
   * @procedure 1. Perform setType() API with NOTICE 2. Perform getType() API
   * @post_condition None
   * @expected Should get Message type should be NOTICE
   */
  public void testSetTypeAsNotice_SRC_P() {
    mMessage.setType(MessageType.NOTICE);

    assertEquals("Message type should be NOTICE", MessageType.NOTICE,
        mMessage.getType());
  }

  /**
   * @since 2016-10-03
   * @see String getTime()
   * @objective Test 'setTime()' function with positive basic way
   * @target void setTime()
   * @test_data time = "12:00"
   * @pre_condition Construct Message class with title = "Test Title",
   *                contentTest = "Test Message Body", sourceName = "OCF"
   * @procedure 1. Perform setTime() API with "12:00" 2. Perform getTime() API
   * @post_condition None
   * @expected Should get 12:00 as Time
   */
  public void testSetTime_SRC_P() {
    String time = "12:00";

    mMessage.setTime(time);
    assertEquals("Time should be 12:00 after set", time, mMessage.getTime());
  }

  /**
   * @since 2016-10-03
   * @see long getTTL()
   * @objective Test 'setTTL()' function with positive basic way
   * @target void setTTL()
   * @test_data ttl = 20
   * @pre_condition Construct Message class with title = "Test Title",
   *                contentTest = "Test Message Body", sourceName = "OCF"
   * @procedure 1. Set TTL with 20; 2. Perform getTTL() API
   * @post_condition None
   * @expected Should get 20 as TTL
   */
  public void testSetTTL_SRC_P() {
    long ttl = 20;

    mMessage.setTTL(ttl);

    assertEquals("TTL should be " + ttl, ttl, mMessage.getTTL());
  }

  /**
   * @since 2016-10-03
   * @see String getTitle()
   * @objective Test 'setTitle()' function with positive basic way
   * @target void setTitle()
   * @test_data title = "TEST_TITLE"
   * @pre_condition Construct Message class with title = "Test Title",
   *                contentTest = "Test Message Body", sourceName = "OCF"
   * @procedure 1. setTitle as "TEST_TITLE" 2. Perform getTitle() API
   * @post_condition None
   * @expected Should get title = TEST_TITLE
   */
  public void testSetTitle_SRC_P() {
    String title = "TEST_TITLE";

    mMessage.setTitle(title);
    assertEquals("Source should be same as setted", title, mMessage.getTitle());
  }

  /**
   * @since 2016-10-04
   * @see none
   * @objective Test 'getContentText()' function with positive basic way
   * @target String getContentText()
   * @test_data contentText = "TEST_CONTENT"
   * @pre_condition Construct Message class with title = "Test Title",
   *                contentTest = "Test Message Body", sourceName = "OCF"
   * @procedure 1. setContentText as "TEST_TITLE" 2. Perform getContentText()
   *            API
   * @post_condition None
   * @expected Should get contentText = "TEST_CONTENT"
   */
  public void testSetContentText_SRC_P() {
    String content = "TEST_CONTENT";

    mMessage.setContentText(content);

    assertEquals("Source should be OCF", content, mMessage.getContentText());
  }

  /**
   * @since 2016-10-04
   * @see none
   * @objective Test 'getMediaContents()' function with positive basic way
   * @target MediaContents getMediaContents()
   * @test_data None
   * @pre_condition 1. Construct Message class with title = "Test Title",
   *                contentTest = "Test Message Body", sourceName = "OCF" 2.
   * @procedure Perform getMediaContents() API
   * @post_condition None
   * @expected Should not get null MediaContents
   */
  public void testSetMediaContents_SRC_P() {
    MediaContents mediaContents = new MediaContents(ICON_IMG);

    mMessage.setMediaContents(mediaContents);
    assertNotNull("MediaContents should not be", mMessage.getMediaContents());
  }

  /**
   * @since 2016-10-04
   * @see String getTopic()
   * @objective Test 'setTopic()' function with positive basic way
   * @target void setTopic()
   * @test_data topic = "TEST_TOPIC"
   * @pre_condition Construct Message class with title = "Test Title",
   *                contentTest = "Test Message Body", sourceName = "OCF"
   * @procedure 1. Set topic with "TEST_TOPIC" 2. Perform getTopic() API
   * @post_condition None
   * @expected Should get Topic = "TEST_TOPIC"
   */
  public void testSetTopic_SRC_P() {
    String topic = "TEST_TOPIC";

    mMessage.setTopic(topic);

    assertEquals("Topic should be " + topic, topic, mMessage.getTopic());
  }

  /**
   * @since 2016-10-04
   * @see none
   * @objective Test 'MediaContents()' function with positive basic way
   * @target MediaContents()
   * @test_data iconImage = "media/img.jpg"
   * @pre_condition None
   * @procedure Perform MediaContents() API
   * @post_condition None
   * @expected Should not throw any exception
   */
  public void testMediaContentsInstance_SRC_P() {
    try {
      new MediaContents(ICON_IMG);
    } catch (Exception e) {
      fail("Should not throw any exception");
    }
  }

  /**
   * @since 2016-10-04
   * @see none
   * @objective Test 'getIconImage()' function with positive basic way
   * @target String getIconImage()
   * @test_data None
   * @pre_condition Create MediaContents instance with "media/img.jpg"
   * @procedure Perform getIconImage() API
   * @post_condition None
   * @expected Should get "media/img.jpg"
   */
  public void testGetIconImage_SRC_P() {
    MediaContents mediaContents = new MediaContents(ICON_IMG);

    assertEquals("Should get the iconImage", ICON_IMG,
        mediaContents.getIconImage());
  }

  /**
   * @since 2016-10-04
   * @see String getIconImage()
   * @objective Test 'setIconImage()' function with positive basic way
   * @target void setIconImage()
   * @test_data iconImage = "test.png"
   * @pre_condition Create MediaContents instance with "media/img.jpg"
   * @procedure 1. Set Icon Image with test_data 2. Perform getIconImage() API
   * @post_condition None
   * @expected Should get same iconImage as set
   */
  public void testSetIconImage_SRC_P() {
    MediaContents mediaContents = new MediaContents(ICON_IMG);

    String iconImg = "test.png";
    mediaContents.setIconImage(iconImg);

    assertEquals("Should get the iconImage", iconImg,
        mediaContents.getIconImage());
  }

  /**
   * @since 2016-10-04
   * @see None
   * @objective Test 'SyncInfo()' constructor with positive basic way
   * @target SyncInfo()
   * @test_data message_id = 1000, provider_id = "TEST_001", SyncType = READ
   * @pre_condition None
   * @procedure Create instance of SyncInfo
   * @post_condition None
   * @expected Should not throw any exception
   */
  public void testSyncInfoInstance_SRC_P() {
    try {
      new SyncInfo(MESSAGE_ID, PROVIDER_ID, SyncType.READ);
    } catch (Exception e) {
      fail("Exception occurs when calling SyncInfo() constructor");
    }
  }

  /**
   * @since 2016-10-04
   * @see None
   * @objective Test 'getMessageId()' function with positive basic way
   * @target long getMessageId()
   * @test_data None
   * @pre_condition Create instance of SyncInfo with message_id = 1000,
   *                provider_id = "TEST_001", SyncType = READ
   * @procedure Perform getMessageId() API
   * @post_condition None
   * @expected Should return same value as construct method
   */
  public void testSyncInfoGetMessageId_SRC_P() {
    SyncInfo syncInfo = new SyncInfo(MESSAGE_ID, PROVIDER_ID, SyncType.READ);

    assertEquals("SyncInfo MessageID doesn't match", MESSAGE_ID,
        syncInfo.getMessageId());
  }

  /**
   * @since 2016-10-04
   * @see None
   * @objective Test 'getProviderId()' function with positive basic way
   * @target String getProviderId()
   * @test_data None
   * @pre_condition Create instance of SyncInfo with message_id = 1000,
   *                provider_id = "TEST_001", SyncType = READ
   * @procedure Perform getProviderId() API
   * @post_condition None
   * @expected Should return same value as construct method
   */
  public void testSyncInfoGetProviderId_SRC_P() {
    SyncInfo syncInfo = new SyncInfo(MESSAGE_ID, PROVIDER_ID, SyncType.READ);

    assertEquals("SyncInfo ProviderID doesn't match", PROVIDER_ID,
        syncInfo.getProviderId());
  }

  /**
   * @since 2016-10-04
   * @see None
   * @objective Test 'getState()' function with positive basic way
   * @target SyncType getState()
   * @test_data None
   * @pre_condition Create instance of SyncInfo with message_id = 1000,
   *                provider_id = "TEST_001", SyncType = READ
   * @procedure Perform getState() API
   * @post_condition None
   * @expected Should return same value as construct method
   */
  public void testSyncInfoGetSyncState_SRC_P() {
    SyncInfo syncInfo = new SyncInfo(MESSAGE_ID, PROVIDER_ID, SyncType.READ);

    assertEquals("SyncInfo Sync State doesn't match", SyncType.READ,
        syncInfo.getState());
  }

  /**
   * @since 2016-10-04
   * @see None
   * @objective Test 'getTopicName()' function with positive basic way
   * @target String getTopicName()
   * @test_data None
   * @pre_condition Create instance of Topic with topicName = "TestTopic", TopicState = UNSUBSCRIBED
   * @procedure Perform getTopicName() API
   * @post_condition None
   * @expected Should return same value as constructed method
   */
  public void testGetTopicName_SRC_P() {
    Topic topic = new Topic(TOPIC_NAME, TopicState.UNSUBSCRIBED);

    assertEquals("Topic Name is not same in getTopicName api", TOPIC_NAME, topic.getTopicName());
  }

  /**
   * @since 2016-10-04
   * @see String getTopicName()
   * @objective Test 'setTopicName()' function with positive basic way
   * @target void setTopicName(String topicName)
   * @test_data topicName = "TEST_TOPIC"
   * @pre_condition Create instance of Topic with topicName = "TestTopic", TopicState = UNSUBSCRIBED
   * @procedure 1. Perform setTopicName() API
   *            2. Perform getTopicName() API
   * @post_condition None
   * @expected Should return same value as set value
   */
  public void testSetTopicName_SRC_P() {
    Topic topic = new Topic(TOPIC_NAME, TopicState.UNSUBSCRIBED);
    String topicName = "TEST_TOPIC";
    topic.setTopicName(topicName);

    assertEquals("Topic Name is not same as setTopicName api", topicName, topic.getTopicName());
  }

  /**
   * @since 2016-10-04
   * @see None
   * @objective Test 'getState()' function with positive basic way
   * @target String getState()
   * @test_data None
   * @pre_condition Create instance of Topic with topicName = "TestTopic", TopicState = UNSUBSCRIBED
   * @procedure Perform getState() API
   * @post_condition None
   * @expected Should return same value as constructed method
   */
  public void testGetTopicState_SRC_P() {
    Topic topic = new Topic(TOPIC_NAME, TopicState.UNSUBSCRIBED);

    assertEquals("Topic State is not same in getState() api", TopicState.UNSUBSCRIBED, topic.getState());
  }

  /**
   * @since 2016-10-04
   * @see TopicState getState()
   * @objective Test 'setState()' function with positive basic way
   * @target void setState(TopicState state)
   * @test_data TopicState = UNSUBSCRIBED
   * @pre_condition Create instance of Topic with topicName = "TestTopic", TopicState = UNSUBSCRIBED
   * @procedure 1. Perform setState() API
   *            2. Perform getState() API
   * @post_condition None
   * @expected Should return same value as set value
   */
  public void testSetTopicState_SRC_P() {
    Topic topic = new Topic(TOPIC_NAME, TopicState.UNSUBSCRIBED);
    topic.setState(TopicState.SUBSCRIBED);

    assertEquals("Topic State is not same in setState() api", TopicState.SUBSCRIBED, topic.getState());
  }

  /**
   * @since 2016-10-05
   * @see None
   * @objective Test 'getTopicsList()' function with positive basic way
   * @target Vector<Topic> getTopicsList()
   * @test_data None
   * @pre_condition Create instance of TopicList with topicName = "TestTopic", TopicState = UNSUBSCRIBED
   * @procedure 1. Perform getTopicsList() API
   *            2. Check the size of the list
   * @post_condition None
   * @expected Should be return empty topicList
   */
  public void testGetTopicList_SRC_P() {
    TopicsList topicList = new TopicsList();
    assertEquals("TopicList should be empty", 0, topicList.getTopicsList().size());
  }

  /**
   * @since 2016-10-05
   * @see Vector<Topic> getTopicsList()
   * @objective Test 'addTopic()' function with positive basic way
   * @target void addTopic(String topicname, Topic.TopicState state)
   * @test_data topicName = "TestTopic", TopicState = UNSUBSCRIBED
   * @pre_condition Create instance of TopicList with topicName = "TestTopic", TopicState = UNSUBSCRIBED
   * @procedure 1. Perform addTopic() API
   *            2. Perform getTopicsList() API
   *            3. Check the size of the list
   * @post_condition None
   * @expected Should not return empty topicList
   */
  public void testAddTopic_SRC_P() {
    TopicsList topicList = new TopicsList();

    topicList.addTopic(TOPIC_NAME, TopicState.UNSUBSCRIBED);

    assertNotSame("TopicList should not be empty", 0, topicList.getTopicsList().size());
  }

  /**
   * @since 2016-10-05
   * @see Vector<Topic> getTopicsList()
   * @see void addTopic(String topicname, Topic.TopicState state)
   * @objective Test 'addTopic()' function with positive basic way
   * @target void removeTopic(String topicName)
   * @test_data topicName = "TestTopic", TopicState = UNSUBSCRIBED
   * @pre_condition 1. Create instance of TopicList with topicName = "TestTopic", TopicState = UNSUBSCRIBED
   *                2. Perform addTopic() API
   *                3. Perform addTopic() API
   * @procedure 1. Perform removeTopic() API
   *            2. Check the size of the list
   * @post_condition None
   * @expected Should return topicList with 1 sized
   */
  public void testRemoveTopic_SRC_P() {
    TopicsList topicList = new TopicsList();
    String topicName = "TEST_TOPIC";
    topicList.addTopic(topicName, TopicState.UNSUBSCRIBED);
    topicList.addTopic(TOPIC_NAME, TopicState.UNSUBSCRIBED);

    topicList.removeTopic(topicName);

    assertSame("TopicList should be sized 1", 1, topicList.getTopicsList().size());
  }
}
