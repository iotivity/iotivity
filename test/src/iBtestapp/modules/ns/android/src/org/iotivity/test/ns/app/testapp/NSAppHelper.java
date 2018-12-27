/******************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 ******************************************************************/

package org.iotivity.test.ns.app.testapp;

import java.util.Vector;

import org.iotivity.service.ns.provider.*;
import org.iotivity.base.OcException;
import org.iotivity.base.OcPlatform;
import org.iotivity.service.ns.common.*;
import org.iotivity.service.ns.common.SyncInfo.SyncType;
import org.iotivity.service.ns.common.Topic.TopicState;
import org.iotivity.service.ns.consumer.*;
import org.iotivity.service.ns.consumer.Provider.OnMessageReceivedListener;
import org.iotivity.service.ns.consumer.Provider.OnSyncInfoReceivedListener;
import org.iotivity.service.ns.consumer.Provider.ProviderState;

import static org.iotivity.test.ns.app.testapp.NSAppUtility.*;

import org.iotivity.service.testapp.framework.Base;

import android.util.Log;

public class NSAppHelper extends Base implements
        ProviderService.OnConsumerSubscribedListener,
        ProviderService.OnMessageSynchronizedListener,
        ConsumerService.OnProviderDiscoveredListener,
        OnSyncInfoReceivedListener, OnMessageReceivedListener,
        Provider.OnProviderStateListener {

    private static NSAppHelper instance           = null;
    private Boolean            mIsProvider        = true;
    private ProviderService    mProviderService   = null;
    private ConsumerService    mConsumerService   = null;
    private Boolean            mIsProviderStarted = false;
    private Boolean            mIsConsumerStarted = false;
    private Consumer           mConsumer          = null;
    private Provider           mProvider          = null;
    private SyncInfo           mProviderSyncInfo  = null;
    private Message            mNotification      = null;
    private static int         id                 = 0;

    private NSAppHelper() {
    }

    public static NSAppHelper getInstance() {
        if (instance == null) {
            instance = new NSAppHelper();
        }

        return instance;
    }

    public void startProvider(Boolean subcontrollability) {
        mIsProvider = subcontrollability;
        try {
            OcPlatform.stopPresence();
            showLog(LogLevel.INFO, "stopPresence Calleded Successfully", false);
        } catch (OcException e1) {
            e1.printStackTrace();
            showLog(LogLevel.INFO, "stopPresence generated exception", false);
        }
        mProviderService = ProviderService.getInstance();

        try {
            mProviderService.start(this, this, subcontrollability,
                    USER_INFO, IS_SECURED);
            showLog(LogLevel.INFO, "Provider Started Successfully", false);
        } catch (NSException e) {
            showLog(LogLevel.ERROR,
                    "Failed to Start Provider. Exception is "
                            + e.getLocalizedMessage(), false);
            return;
        }

    }

    public void stopProvider() {
        if (!mIsProviderStarted) {
            showLog(LogLevel.ERROR, "Provider Service is not started yet",
                    false);
            return;
        }

        try {
            mProviderService.stop();
            showLog(LogLevel.INFO, "Provider is stopped successfully", false);
        } catch (NSException e) {
            showLog(LogLevel.ERROR,
                    "Fail to stop provider. Exception is "
                            + e.getLocalizedMessage(), false);
            return;
        }
    }

    public void pendingSubscripton(Boolean isAccept) {
        if (!mIsProviderStarted) {
            showLog(LogLevel.ERROR, "Provider Service is not started yet",
                    false);
            return;
        }

        if (mConsumer != null) {
            try {
                mConsumer.acceptSubscription(isAccept);
                showLog(LogLevel.INFO,
                        "Accept/Reject sucessfully. ConsumerID: "
                                + mConsumer.getConsumerId(), false);
            } catch (NSException e) {
                showLog(LogLevel.ERROR,
                        "Failed to Accept/Reject Consumer",
                        false);
            }
        } else {
            showLog(LogLevel.ERROR, "No consumer found...", false);
        }
    }

    public void sendMessage() {
        if (!mIsProviderStarted) {
            showLog(LogLevel.ERROR, "Provider Service is not started yet",
                    false);
            return;
        }

        Message message = null;

        try {
            message = mProviderService.createMessage();
            message.setTitle(MSG_TITLE + id);
            message.setContentText(MSG_BODY + id);
            message.setSourceName(MSG_SOURCE);
        } catch (NSException e1) {
            showLog(LogLevel.ERROR,
                    "Can't create message. Exception: "
                            + e1.getLocalizedMessage(), false);
            return;
        }

        message.setTTL(10);
        // message.setTime(DateFormat.getDateTimeInstance().format(new Date()));
        message.setTopic(TOPIC_NAME + id);
        MediaContents mediaContents = new MediaContents("Image");
        message.setMediaContents(mediaContents);

        try {
            mProviderService.sendMessage(message);
            showLog(LogLevel.INFO, "Notification Sent Successfully..", false);
        } catch (NSException e) {
            showLog(LogLevel.ERROR,
                    "Can't sent notification. Exception is "
                            + e.getLocalizedMessage(), false);
            return;
        }
    }

    public void registerTopic() {
        if (!mIsProviderStarted) {
            showLog(LogLevel.ERROR, "Provider Service is not started yet",
                    false);
            return;
        }

        ++id;

        try {
            mProviderService.registerTopic(TOPIC_NAME + id);
            showLog(LogLevel.INFO, "Topic registered Successfully...", false);
        } catch (NSException e) {
            showLog(LogLevel.ERROR, "Topic register failed...",
                    false);
            return;
        }
    }

    public void unRegisterTopic() {
        if (!mIsProviderStarted) {
            showLog(LogLevel.ERROR, "Provider Service is not started yet",
                    false);
            return;
        }

        try {
            mProviderService.unregisterTopic(TOPIC_NAME + id);
            showLog(LogLevel.INFO, "Topic unregistered Successfully...", false);
        } catch (NSException e) {
            showLog(LogLevel.ERROR, "Topic unregister failed...", false);
            return;
        }

        --id;
    }

    public void setTopic() {
        if (!mIsProviderStarted) {
            showLog(LogLevel.ERROR, "Provider Service is not started yet",
                    false);
            return;
        }

        if (!mIsProvider) {
            showLog(LogLevel.WARNING, "Operation is not permitted", false);
            return;
        }

        for (int i = 1; i <= id; i += 2) {
            try {
                mConsumer.setTopic(TOPIC_NAME + i);
                showLog(LogLevel.INFO, "Set the topic " + TOPIC_NAME + i
                        + " Successfully...", false);
            } catch (NSException e) {
                showLog(LogLevel.ERROR,
                        "Failed to set topic: = "
                                + TOPIC_NAME + i + ". Exception is "
                                + e.getLocalizedMessage(), false);
                return;
            }
        }
    }

    public void unSetTopic() {
        if (!mIsProviderStarted) {
            showLog(LogLevel.ERROR, "Provider Service is not started yet",
                    false);
            return;
        }

        if (!mIsProvider) {
            showLog(LogLevel.WARNING, "Operation is not permitted", false);
            return;
        }

        for (int i = 1; i <= id; i += 2) {
            try {
                mConsumer.unsetTopic(TOPIC_NAME + i);
                showLog(LogLevel.INFO, "UnSet the topic " + TOPIC_NAME + i
                        + " Successfully...", false);
            } catch (NSException e) {
                showLog(LogLevel.ERROR,
                        "Failed to unset topic: = "
                                + TOPIC_NAME + i + ". Exception is "
                                + e.getLocalizedMessage(), false);
                return;
            }
        }
    }

    public void getProviderTopics() {
        if (!mIsProviderStarted) {
            showLog(LogLevel.ERROR, "Provider Service is not started yet",
                    false);
            return;
        }

        try {
            printTopicList(mProviderService.getRegisteredTopicList());
        } catch (NSException e) {
            showLog(LogLevel.ERROR,
                    "Exception occurs when calling getRegisteredTopicList(). Exception: "
                            + e.getLocalizedMessage(), false);
            return;
        }
    }

    public void getConsumerTopics() {
        if (!mIsProviderStarted) {
            showLog(LogLevel.ERROR, "Provider Service is not started yet",
                    false);
            return;
        }

        try {
            printTopicList(mConsumer.getConsumerTopicList());
        } catch (NSException e) {
            showLog(LogLevel.ERROR,
                    "Exception occurs when calling getConsumerTopicList(). Exception: "
                            + e.getLocalizedMessage(), false);
            return;
        }
    }

    public void sendProviderSyncInfo() {
        if (!mIsProviderStarted) {
            showLog(LogLevel.ERROR, "Provider Service is not started yet",
                    false);
            return;
        }

        if (mProviderSyncInfo == null) {
            showLog(LogLevel.ERROR, "Didn't get any sync from consumer", false);
            return;
        }

        try {
            mProviderService.sendSyncInfo(mProviderSyncInfo.getMessageId(),
                    SyncType.READ);
        } catch (NSException e) {
            showLog(LogLevel.ERROR,
                    "Exception occurs when calling sendSyncInfo(). Exception: "
                            + e.getLocalizedMessage(), false);
        }
    }

    // Consumer Service Methods
    public void startConsumer() {
        mProvider = null;
        try {
            OcPlatform.stopPresence();
            showLog(LogLevel.INFO, "stopPresence Calleded Successfully", false);
        } catch (OcException e1) {
            e1.printStackTrace();
            showLog(LogLevel.INFO, "stopPresence generated exception", false);
        }
        mConsumerService = ConsumerService.getInstance();

        try {
            mConsumerService.start(this);
        } catch (NSException e) {
            showLog(LogLevel.ERROR,
                    "Can't start consumer service. Occurs exception: "
                            + e.getLocalizedMessage(), false);
            return;
        }

        mIsConsumerStarted = true;

        showLog(LogLevel.INFO, "Consumer Service Started Successfully", false);
    }

    public void stopConsumer() {
        if (!mIsConsumerStarted) {
            showLog(LogLevel.ERROR, "Consumer Service is not started yet",
                    false);
            return;
        }

        try {
            mConsumerService.stop();
        } catch (NSException e) {
            showLog(LogLevel.ERROR,
                    "Can't stop consumer service. Occurs exception: "
                            + e.getLocalizedMessage(), false);
        }

        mIsConsumerStarted = false;
        showLog(LogLevel.INFO, "Consumer Service Stopped Successfully", false);

        mProvider = null;
        mConsumerService = null;
    }

    public void manualScan() {
        if (!mIsConsumerStarted) {
            showLog(LogLevel.ERROR, "Consumer Service is not started yet",
                    false);
            return;
        }

        try {
            mConsumerService.rescanProvider();
        } catch (NSException e) {
            showLog(LogLevel.ERROR,
                    "Can't rescan because of " + e.getLocalizedMessage(), false);
        }
    }

    public void subscribeConsumer() {
        if (!mIsConsumerStarted) {
            showLog(LogLevel.ERROR, "Consumer Service is not started yet",
                    false);
            return;
        }

        if (mProvider == null) {
            showLog(LogLevel.ERROR, "Can't find any provider...", false);
            return;
        }

        try {
            mProvider.subscribe();
        } catch (NSException e) {
            showLog(LogLevel.ERROR,
                    "Can't subscribe because of " + e.getLocalizedMessage(),
                    false);
        }

        showLog(LogLevel.INFO,
                "Subscribe sucessfully to the provider with ID: "
                        + mProvider.getProviderId(), false);
    }

    public void sendConsumerSyncInfo() {
        if (!mIsConsumerStarted) {
            showLog(LogLevel.ERROR, "Consumer Service is not started yet",
                    false);
            return;
        }

        if (mProvider == null) {
            showLog(LogLevel.ERROR, "Can't find any provider...", false);
            return;
        }

        if (mNotification == null) {
            showLog(LogLevel.ERROR, "Can't find any notification...", false);
            return;
        }

        try {
            mProvider.sendSyncInfo(mNotification.getMessageId(), SyncType.READ);
        } catch (NSException e) {
            showLog(LogLevel.ERROR,
                    "Can't send syncinfo because of " + e.getLocalizedMessage(),
                    false);
        }

        showLog(LogLevel.INFO,
                "Send syncinfo successfully to "
                        + mNotification.getProviderId(), false);
    }

    public void getTopics() {
        TopicsList topicList = getTopicList();

        if (topicList != null) {
            printTopicList(topicList);
        }
    }

    private TopicsList getTopicList() {
        if (!mIsConsumerStarted) {
            showLog(LogLevel.ERROR, "Consumer Service is not started yet",
                    false);
            return null;
        }

        if (mProvider == null) {
            showLog(LogLevel.ERROR, "Can't find any provider...", false);
            return null;
        }

        try {
            return mProvider.getTopicList();
        } catch (NSException e) {
            showLog(LogLevel.ERROR,
                    "Can't get topicList because of " + e.getLocalizedMessage(),
                    false);
            return null;
        }
    }

    public void updateTopics() {
        if (mIsProvider) {
            showLog(LogLevel.WARNING, "Operation is not permitted", false);
            return;
        }

        TopicsList topics = getTopicList();

        if (topics == null) {
            showLog(LogLevel.ERROR, "Can't find any topics", false);
            return;
        }

        try {
            mProvider.updateTopicList(topics);
        } catch (NSException e) {
            showLog(LogLevel.ERROR,
                    "Can't update because of " + e.getLocalizedMessage(), false);
        }
    }

    public void showLog(LogLevel level, String log, Boolean isLogClear) {
        switch (level) {
            case INFO:
                Log.i(TAG, log);
                break;

            case DEBUG:
                Log.d(TAG, log);
                break;

            case WARNING:
                Log.w(TAG, log);
                break;

            case ERROR:
                Log.e(TAG, log);
                break;

            default:
                Log.v(TAG, log);
                break;
        }

        // if (isLogClear) {
        // clearOutPut();
        // }
        //
         showOutPut(log);
    }

    enum LogLevel {
        INFO, DEBUG, WARNING, VERBOSE, ERROR
    }

    // Provider Callback
    @Override
    public void onMessageSynchronized(SyncInfo syncInfo) {
        showLog("syncInfoCallback Called", false);
        showLog(LogLevel.INFO,
                "onMessageSynchronized Listener Called for Message ID: "
                        + syncInfo.getMessageId(), false);

        mProviderSyncInfo = syncInfo;
    }

    @Override
    public void onConsumerSubscribed(Consumer consumer) {
        showLog("subRequestCallback Called", false);
        showLog(LogLevel.INFO,
                "onConsumerSubscribed Listener Called with ConsumerID: "
                        + consumer.getConsumerId(), false);

        mConsumer = consumer;
    }

    // Consumer Callback
    @Override
    public void onSyncInfoReceived(SyncInfo sync) {
        showLog("NSSyncInfoReceivedCallback called", false);
        showLog(LogLevel.INFO,
                "onSyncInfoReceived Listener Called from Provider "
                        + sync.getMessageId() + " Message ID: "
                        + sync.getMessageId(), false);
    }

    @Override
    public void onProviderStateReceived(ProviderState state) {
        showLog("NSProviderStateCallback called", false);
        showLog(LogLevel.INFO, "onProviderStateReceived Listener Called with "
                + getProviderState(state) + " ...", false);
    }

    @Override
    public void onMessageReceived(Message message) {
        showLog("NSMessageReceivedCallback called", false);
        showLog(LogLevel.INFO, "onMessageReceived Listener Called...", false);

        mNotification = message;

        printMessage(message);
    }

    @Override
    public void onProviderDiscovered(Provider provider) {
        showLog("Notification Resource Discovered", false);
        showLog(LogLevel.INFO,
                "onProviderDiscovered Listener Called with ProviderID: "
                        + provider.getProviderId(), false);

        mProvider = provider;

        try {
            mProvider.setListener(this, this, this);
        } catch (NSException e) {
            showLog(LogLevel.ERROR, "Can't set listener", false);
        }
    }

    void printMessage(Message message) {
        String log = "MessageID: " + message.getMessageId();
        log += " Title = " + message.getTitle();
        log += " Body = " + message.getContentText();
        log += " Topic Name: " + message.getTopic();
        log += " ProviderID = " + message.getProviderId();

        showLog(LogLevel.INFO, log, false);
    }

    String getProviderState(ProviderState state) {
        switch (state) {
            case ALLOW:
                showLog("Provider changed to NS_ALLOW State. ", false);
                return "ALLOW";

            case DENY:
                showLog("Provider changed to NS_DENY State. ", false);
                return "DENY";

            case TOPIC:
                showLog("Provider changed to NS_TOPIC State. ", false);
                return "TOPIC";

            case STOPPED:
                showLog("Provider changed to NS_STOPPED State. ", false);
                return "STOPPED";

            default:
                return "UNKNOWN";
        }
    }

    void printTopicList(TopicsList topics) {
        Vector<Topic> topicList = topics.getTopicsList();

        String log = "Registered topics: ";

        for (Topic topic : topicList) {
            log += "Topic: " + topic;

            if (topic.getState() == TopicState.SUBSCRIBED) {
                log += " State: SUBSCRIBED";
            } else {
                log += " State: UNSUBSCRIBED";
            }
        }

        showLog(LogLevel.INFO, log, false);
    }
}
