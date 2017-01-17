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

import org.iotivity.service.ns.common.NSException;
import org.iotivity.service.ns.common.SyncInfo.SyncType;
import org.iotivity.service.ns.common.TopicsList;
import org.iotivity.service.ns.consumer.ConsumerService;
import org.iotivity.service.ns.consumer.Provider;
import org.iotivity.service.ns.consumer.Provider.ProviderState;
import org.iotivity.service.ns.test.helper.NSHelper;

import static org.iotivity.service.ns.test.helper.NSTestUtilily.*;
import android.test.InstrumentationTestCase;
import android.util.Log;

public class NSConsumerTest extends InstrumentationTestCase {
    private NSHelper        mHelper          = null;
    private ConsumerService mConsumerService = null;
    private final long      mMessageId       = 10;

    protected void setUp() throws Exception {
        super.setUp();

        mHelper = NSHelper.getInstance(getInstrumentation().getTargetContext());
        mConsumerService = mHelper.getConsumerService();
    }

    protected void tearDown() throws Exception {
        super.tearDown();

        mHelper.stopServices();
        mConsumerService = null;

    }

    /**
     * @since 2016-09-24
     * @see none
     * @objective Test 'getInstance' function with positive basic way
     * @target public static ConsumerService getInstance()
     * @test_data None
     * @pre_condition None
     * @procedure Perform getInstance() API
     * @post_condition None
     * @expected No exception occurs
     */
    public void testGetConsumerInstance_SRC_P() {
        try {
            ConsumerService.getInstance();
        } catch (Exception e) {
            fail("Exception occurs when try to Get Consumer Instance. Exception: "
                    + e.getLocalizedMessage());
        }
    }

    /**
     * @since 2016-09-24
     * @see none
     * @objective Test 'start' function with positive basic way
     * @target void start(OnProviderDiscoveredListener
     *         onProviderDiscoveredListener) throws NSException
     * @test_data ProviderDiscoveredListener
     * @pre_condition Get Consumer Instance
     * @procedure Perform start() API with test data
     * @post_condition None
     * @expected No exception occurs
     */
    public void testStartConsumer_SRC_P() {
        try {
            mConsumerService.start(mHelper);
            mHelper.setConsumerStartStatus(true);
        } catch (NSException e) {
            fail("Exception occurs when calling start() API. Exception: "
                    + e.getLocalizedMessage());
        }
    }

    /**
     * @since 2016-09-24
     * @see none
     * @objective Test 'start' function with positive basic way
     * @target void start(OnProviderDiscoveredListener
     *         onProviderDiscoveredListener) throws NSException
     * @test_data null
     * @pre_condition Get Consumer Instance
     * @procedure Perform start() API with test data
     * @post_condition None
     * @expected NSException should occurs
     */
    public void testStartConsumer_NV_N() {
        try {
            mConsumerService.start(null);

            fail("Consumer Starts successfully while the listener is null");
        } catch (NSException e) {
        }
    }

    /**
     * @since 2016-09-24
     * @see none
     * @objective Test 'stop' function with positive basic way
     * @target void stop() throws NSException
     * @test_data None
     * @pre_condition Get Consumer Instance
     * @procedure Perform stop() API
     * @post_condition None
     * @expected No exception occurs
     */
    public void testStopConsumer_SRC_P() {
        try {
            mConsumerService.stop();
            mHelper.setConsumerStartStatus(false);
        } catch (NSException e) {
            fail("Failed to stop consumer");
        }
    }

    /**
     * @since 2016-09-24
     * @see none
     * @objective Test 'rescanProvider' function with positive basic way
     * @target void rescanProvider() throws NSException
     * @test_data None
     * @pre_condition Get Consumer Instance
     * @procedure Perform rescanProvider() API
     * @post_condition None
     * @expected No exception occurs
     */
    public void testRescanProvider_SRC_P() {
        try {
            mConsumerService.rescanProvider();
        } catch (NSException e) {
            fail("Successfully called start() API with null Subscribed CB");
        }
    }

    /**
     * @since 2016-09-28
     * @see none
     * @objective Test 'getProviderId()' function with positive basic way
     * @target String getProviderId()
     * @test_data None
     * @pre_condition 1. Get Provider Service Instance 2. Start Provider with
     *                consumer acceptor 3. Get Consumer Service Instance 4.
     *                Start Consumer Service 5. Wait for Provider Discovered
     *                Callback 6. Get Provider from callback
     * @procedure Perform getProviderId() API
     * @post_condition None
     * @expected 1. No exception occurs 2. Should get the consumer id which is
     *           not empty
     */
    public void testGetProviderID_SRC_P() {
        Provider provider = mHelper.getProvider(false);

        assertNotNull("Can't get any provider", provider);

        try {
            assertNotSame("ProviderID should not be empty", 0,
                    provider.getProviderId().length());
        } catch (Exception e) {
            fail("Exception occurs when calling getProviderId() API. Exception is : "
                    + e.getLocalizedMessage());
        }
    }

    /**
     * @since 2016-09-30
     * @see none
     * @objective Test 'getTopicList()' function with positive basic way
     * @target TopicsList getTopicList() throws NSException
     * @test_data None
     * @pre_condition 1. Get Provider Service Instance 2. Start Provider with
     *                consumer acceptor 3. Get Consumer Service Instance 4.
     *                Start Consumer Service 5. Wait for Provider Discovered
     *                Callback 6. Get Provider from callback 7. Register Topic
     *                ("TestTopic")
     * @procedure Perform getTopicList() API
     * @post_condition None
     * @expected 1. No exception occurs 2. Should get the topic list which is
     *           not empty
     */
    public void testGetTopicList_SRC_P() {
        Provider provider = mHelper.getProvider(false);

        assertNotNull("Can't get any provider", provider);

        try {
            TopicsList topicList = provider.getTopicList();

            assertNotNull("TopicList should not be null", topicList);
            assertNotSame("TopicList should not empty.", 0,
                    topicList.mTopicsList.size());
        } catch (Exception e) {
            fail("Exception occurs when calling getProviderId() API. Exception is : "
                    + e.getLocalizedMessage());
        }
    }

    /**
     * @since 2016-09-30
     * @see none
     * @objective Test 'getProviderState()' function with positive basic way
     * @target ProviderState getProviderState() throws NSException
     * @test_data None
     * @pre_condition 1. Get Provider Service Instance 2. Start Provider with
     *                consumer acceptor 3. Get Consumer Service Instance 4.
     *                Start Consumer Service 5. Wait for Provider Discovered
     *                Callback 6. Get Provider from callback 7. Register Topic
     *                ("TestTopic")
     * @procedure Perform getProviderState() API
     * @post_condition None
     * @expected 1. No exception occurs 2. Should get ALLOW state
     */
    public void testGetProviderState_SRC_P() {
        Provider provider = mHelper.getProvider(false);

        assertNotNull("Can't get any provider", provider);

        try {
            ProviderState providerState = provider.getProviderState();

            if (providerState == ProviderState.ALLOW
                    || providerState == ProviderState.DENY
                    || providerState == ProviderState.TOPIC) {
                Log.i(TAG, "Provider State may be ALLOW, DENY or TOPIC");
            } else {
                fail("ProviderState should be ALLOW, DENY or TOPIC state for subControllability = false");
            }
        } catch (Exception e) {
            fail("Exception occurs when calling getProviderState() API. Exception is : "
                    + e.getLocalizedMessage());
        }
    }

    /**
     * @since 2016-10-03
     * @see none
     * @objective Test 'subscribe()' function with positive basic way
     * @target void subscribe() throws NSException
     * @test_data None
     * @pre_condition 1. Get Provider Service Instance 2. Start Provider with
     *                consumer acceptor 3. Get Consumer Service Instance 4.
     *                Start Consumer Service 5. Wait for Provider Discovered
     *                Callback 6. Get Provider from callback
     * @procedure Perform subscribe() API
     * @post_condition None
     * @expected 1. No exception occurs 2. Should get ALLOW state
     */
    public void testSubscribe_SRC_P() {
        Provider provider = mHelper.getProvider(false);

        assertNotNull("Can't get any provider", provider);

        try {
            provider.subscribe();
        } catch (Exception e) {
            fail("Exception occurs when calling subscribe() API. Exception is : "
                    + e.getLocalizedMessage());
        }
    }

    /**
     * @since 2016-10-03
     * @see none
     * @objective Test 'isSubscribed()' function with positive basic way
     * @target boolean isSubscribed() throws NSException
     * @test_data None
     * @pre_condition 1. Get Provider Service Instance 2. Start Provider with
     *                consumer acceptor 3. Get Consumer Service Instance 4.
     *                Start Consumer Service 5. Wait for Provider Discovered
     *                Callback 6. Get Provider from callback
     * @procedure Perform isSubscribed() without calling subscribe()
     * @post_condition None
     * @expected 1. No exception occurs 2. Should get false as return value
     */
    public void testIsSubscribe_SRC_P() {
        Provider provider = mHelper.getProvider(false);

        assertNotNull("Can't get any provider", provider);

        try {
            assertEquals("Should not subscribed, before subscription", false,
                    provider.isSubscribed());
        } catch (Exception e) {
            fail("Exception occurs when calling subscribe() API. Exception is : "
                    + e.getLocalizedMessage());
        }
    }

    /**
     * @since 2016-10-03
     * @see none
     * @objective Test 'sendSyncInfo()' function with positive basic way
     * @target void sendSyncInfo(long messageId, SyncInfo.SyncType syncType)
     * @test_data None
     * @pre_condition 1. Get Provider Service Instance 2. Start Provider with
     *                consumer acceptor 3. Get Consumer Service Instance 4.
     *                Start Consumer Service 5. Wait for Provider Discovered
     *                Callback 6. Get Provider from callback
     * @procedure Perform sendSyncInfo() API
     * @post_condition None
     * @expected No exception occurs
     */
    public void testSendSyncInfo_SRC_P() {
        Provider provider = mHelper.getProvider(false);

        assertNotNull("Can't get any provider", provider);

        try {
            provider.sendSyncInfo(mMessageId, SyncType.READ);
        } catch (Exception e) {
            fail("Exception occurs when calling sendSyncInfo() API. Exception is : "
                    + e.getLocalizedMessage());
        }
    }

    /**
     * @since 2016-10-03
     * @see none
     * @objective Test 'setListener()' function with positive basic way
     * @target void setListener(OnProviderStateListener onProviderStateListener,
     *         OnMessageReceivedListner onMessageReceivedListner,
     *         OnSyncInfoReceivedListner onSyncInfoReceivedListner)
     * @test_data OnProviderStateListener, OnMessageReceivedListner,
     *            OnSyncInfoReceivedListner
     * @pre_condition 1. Get Provider Service Instance 2. Start Provider with
     *                consumer acceptor 3. Get Consumer Service Instance 4.
     *                Start Consumer Service 5. Wait for Provider Discovered
     *                Callback 6. Get Provider from callback
     * @procedure Perform setListener() API
     * @post_condition None
     * @expected No exception occurs
     */
    public void testSetListener_SRC_P() {
        Provider provider = mHelper.getProvider(false);

        assertNotNull("Can't get any provider", provider);

        try {
            provider.setListener(mHelper, mHelper, mHelper);
        } catch (Exception e) {
            fail("Exception occurs when calling setListener() API. Exception is : "
                    + e.getLocalizedMessage());
        }
    }

    /**
     * @since 2016-10-03
     * @see none
     * @objective Test 'setListener()' function with null
     *            OnProviderStateListener
     * @target void setListener(OnProviderStateListener onProviderStateListener,
     *         OnMessageReceivedListner onMessageReceivedListner,
     *         OnSyncInfoReceivedListner onSyncInfoReceivedListner)
     * @test_data OnProviderStateListener = null, OnMessageReceivedListner,
     *            OnSyncInfoReceivedListner
     * @pre_condition 1. Get Provider Service Instance 2. Start Provider with
     *                consumer acceptor 3. Get Consumer Service Instance 4.
     *                Start Consumer Service 5. Wait for Provider Discovered
     *                Callback 6. Get Provider from callback
     * @procedure Perform setListener() with null OnProviderStateListener
     * @post_condition None
     * @expected NSException should occur
     */
    public void testSetListenerWithStateListener_NV_N() {
        Provider provider = mHelper.getProvider(false);

        assertNotNull("Can't get any provider", provider);

        try {
            provider.setListener(null, mHelper, mHelper);
            fail("Successfully set null as OnProviderStateListener");
        } catch (NSException nsE) {
        } catch (Exception e) {
            fail("Should throw NSException");
        }
    }

    /**
     * @since 2016-10-03
     * @see none
     * @objective Test 'setListener()' function with null
     *            OnMessageReceivedListner
     * @target void setListener(OnProviderStateListener onProviderStateListener,
     *         OnMessageReceivedListner onMessageReceivedListner,
     *         OnSyncInfoReceivedListner onSyncInfoReceivedListner)
     * @test_data OnProviderStateListener, OnMessageReceivedListner = null,
     *            OnSyncInfoReceivedListner
     * @pre_condition 1. Get Provider Service Instance 2. Start Provider with
     *                consumer acceptor 3. Get Consumer Service Instance 4.
     *                Start Consumer Service 5. Wait for Provider Discovered
     *                Callback 6. Get Provider from callback
     * @procedure Perform setListener() with null OnMessageReceivedListner
     * @post_condition None
     * @expected NSException should occur
     */
    public void testSetListenerWithMsgReceivedListener_NV_N() {
        Provider provider = mHelper.getProvider(false);

        assertNotNull("Can't get any provider", provider);

        try {
            provider.setListener(mHelper, null, mHelper);
            fail("Successfully set null as OnMessageReceivedListner");
        } catch (NSException nsE) {
        } catch (Exception e) {
            fail("Should throw NSException");
        }
    }

    /**
     * @since 2016-10-03
     * @see none
     * @objective Test 'setListener()' function with null
     *            OnSyncInfoReceivedListner
     * @target void setListener(OnProviderStateListener onProviderStateListener,
     *         OnMessageReceivedListner onMessageReceivedListner,
     *         OnSyncInfoReceivedListner onSyncInfoReceivedListner)
     * @test_data OnProviderStateListener, OnMessageReceivedListner,
     *            OnSyncInfoReceivedListner = null
     * @pre_condition 1. Get Provider Service Instance 2. Start Provider with
     *                consumer acceptor 3. Get Consumer Service Instance 4.
     *                Start Consumer Service 5. Wait for Provider Discovered
     *                Callback 6. Get Provider from callback
     * @procedure Perform setListener() with null OnSyncInfoReceivedListner
     * @post_condition None
     * @expected NSException should occur
     */
    public void testSetListenerWithSyncInfoListener_NV_N() {
        Provider provider = mHelper.getProvider(false);

        assertNotNull("Can't get any provider", provider);

        try {
            provider.setListener(mHelper, mHelper, null);
            fail("Successfully set null as OnSyncInfoReceivedListner");
        } catch (NSException nsE) {
        } catch (Exception e) {
            fail("Should throw NSException");
        }
    }

    /**
     * @since 2016-10-03
     * @see none
     * @objective Test 'updateTopicList()' function with positive basic way
     * @target int updateTopicList() throws NSException
     * @test_data None
     * @pre_condition 1. Get Provider Service Instance 2. Start Provider with
     *                consumer acceptor 3. Get Consumer Service Instance 4.
     *                Start Consumer Service 5. Wait for Provider Discovered
     *                Callback 6. Get Provider from callback 7. Register Topic
     *                ("TestTopic") 8. Get TopicList
     * @procedure Perform updateTopicList() API
     * @post_condition None
     * @expected 1. No exception occurs 2. Should return NS_OK (100)
     */
    public void testUpdateTopicList_SRC_P() {
        Provider provider = mHelper.getProvider(false);

        assertNotNull("Can't get any provider", provider);

        try {
            TopicsList topicList = provider.getTopicList();

            assertNotNull("TopicList should not be null", topicList);

            assertEquals("Should return NS_OK(100)", NS_OK,
                    provider.updateTopicList(topicList));
        } catch (Exception e) {
            fail("Exception occurs when calling getProviderId() API. Exception is : "
                    + e.getLocalizedMessage());
        }
    }
}
