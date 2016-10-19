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

package org.iotivity.service.ns.testapp;

import java.util.Vector;

import org.iotivity.service.ns.provider.*;
import org.iotivity.service.ns.common.*;
import org.iotivity.service.ns.common.SyncInfo.SyncType;
import org.iotivity.service.ns.common.Topic.TopicState;
import org.iotivity.service.ns.consumer.*;
import org.iotivity.service.ns.consumer.Provider.ProviderState;

import static org.iotivity.service.ns.testapp.NSAppUtility.*;

import org.iotivity.service.testapp.framework.Base;

import android.util.Log;

public class NSAppHelper extends Base implements
        ProviderService.OnConsumerSubscribedListener,
        ProviderService.OnMessageSynchronizedListener,
        ConsumerService.OnProviderDiscoveredListener,
        Provider.OnSyncInfoReceivedListner, Provider.OnMessageReceivedListner,
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
    private int                mResult            = 0;
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

        mProviderService = ProviderService.getInstance();

        try {
            mResult = mProviderService.start(this, this, subcontrollability,
                    USER_INFO, IS_SECURED);
        } catch (NSException e) {
            showLog(LogLevel.ERROR,
                    "Can't start provider service. Exception is "
                            + e.getLocalizedMessage(), false);
            return;
        }

        if (mResult == NS_OK) {
            mIsProviderStarted = true;
            showLog(LogLevel.INFO, "Provider Started Successfully", false);
        } else {
            showLog(LogLevel.ERROR, "Failed to Start Provider", false);
        }
    }

    public void stopProvider() {
        if (!mIsProviderStarted) {
            showLog(LogLevel.ERROR, "Provider Service is not started yet",
                    false);
            return;
        }

        try {
            mResult = mProviderService.stop();
        } catch (NSException e) {
            showLog(LogLevel.ERROR,
                    "Fail to stop provider. Exception is "
                            + e.getLocalizedMessage(), false);
            return;
        }

        if (mResult == NS_OK) {
            mIsProviderStarted = false;
            showLog(LogLevel.INFO, "Provider is stopped successfully", false);
        } else {
            showLog(LogLevel.ERROR, "Fail to stop provider.", false);
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
                mResult = mConsumer.acceptSubscription(isAccept);
            } catch (NSException e) {
                showLog(LogLevel.ERROR,
                        "Exception occurs when calling acceptSubscription()",
                        false);
            }

            if (mResult == NS_OK) {
                showLog(LogLevel.INFO,
                        "Accept/Reject sucessfully. ConsumerID: "
                                + mConsumer.getConsumerId(), false);
            } else {
                showLog(LogLevel.ERROR, "Failed to Accept/Reject Consumer.",
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
            mResult = mProviderService.sendMessage(message);
        } catch (NSException e) {
            showLog(LogLevel.ERROR,
                    "Exception occurs when sending notification. Exception is "
                            + e.getLocalizedMessage(), false);
            return;
        }

        if (mResult == NS_OK) {
            showLog(LogLevel.INFO, "Notification Sent Successfully..", false);
        } else {
            showLog(LogLevel.ERROR, "Can't sent notification", false);
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
            mResult = mProviderService.registerTopic(TOPIC_NAME + id);
        } catch (NSException e) {
            showLog(LogLevel.ERROR, "Exception occurs when registering topic",
                    false);
            return;
        }

        if (mResult == NS_OK) {
            showLog(LogLevel.INFO, "Topic registered Successfully...", false);
        } else {
            showLog(LogLevel.ERROR, "Topic register failed...", false);
        }
    }

    public void unRegisterTopic() {
        if (!mIsProviderStarted) {
            showLog(LogLevel.ERROR, "Provider Service is not started yet",
                    false);
            return;
        }

        try {
            mResult = mProviderService.unregisterTopic(TOPIC_NAME + id);
        } catch (NSException e) {
            showLog(LogLevel.ERROR,
                    "Exception occurs when unregistering topic", false);
            return;
        }

        --id;

        if (mResult == NS_OK) {
            showLog(LogLevel.INFO, "Topic unregistered Successfully...", false);
        } else {
            showLog(LogLevel.ERROR, "Topic unregister failed...", false);
        }
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
                mResult = mConsumer.setTopic(TOPIC_NAME + i);
            } catch (NSException e) {
                showLog(LogLevel.ERROR,
                        "Exception occurs when setting the topic = "
                                + TOPIC_NAME + i + ". Exception is "
                                + e.getLocalizedMessage(), false);
                return;
            }

            if (mResult == NS_OK) {
                showLog(LogLevel.INFO, "Set the topic " + TOPIC_NAME + i
                        + " Successfully...", false);
            } else {
                showLog(LogLevel.WARNING, "Failed to set topic: " + TOPIC_NAME
                        + i, false);
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
                mResult = mConsumer.unsetTopic(TOPIC_NAME + i);
            } catch (NSException e) {
                showLog(LogLevel.ERROR,
                        "Exception occurs when unsetting the topic = "
                                + TOPIC_NAME + i + ". Exception is "
                                + e.getLocalizedMessage(), false);
                return;
            }

            if (mResult == NS_OK) {
                showLog(LogLevel.INFO, "UnSet the topic " + TOPIC_NAME + i
                        + " Successfully...", false);
            } else {
                showLog(LogLevel.WARNING, "Failed to unset topic: "
                        + TOPIC_NAME + i, false);
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
            mProviderService.sendSyncInfo(mProviderSyncInfo.mMessageId,
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
        // showOutPut(log);
    }

    enum LogLevel {
        INFO, DEBUG, WARNING, VERBOSE, ERROR
    }

    // Provider Callback
    @Override
    public void onMessageSynchronized(SyncInfo syncInfo) {
        showLog(LogLevel.INFO,
                "onMessageSynchronized Listener Called for Message ID: "
                        + syncInfo.getMessageId(), false);

        mProviderSyncInfo = syncInfo;
    }

    @Override
    public void onConsumerSubscribed(Consumer consumer) {
        showLog(LogLevel.INFO,
                "onConsumerSubscribed Listener Called with ConsumerID: "
                        + consumer.getConsumerId(), false);

        mConsumer = consumer;
    }

    // Consumer Callback
    @Override
    public void onSyncInfoReceived(SyncInfo sync) {
        showLog(LogLevel.INFO,
                "onSyncInfoReceived Listener Called from Provider "
                        + sync.getMessageId() + " Message ID: "
                        + sync.getMessageId(), false);
    }

    @Override
    public void onProviderStateReceived(ProviderState state) {
        showLog(LogLevel.INFO, "onProviderStateReceived Listener Called with "
                + getProviderState(state) + " ...", false);
    }

    @Override
    public void onMessageReceived(Message message) {
        showLog(LogLevel.INFO, "onMessageReceived Listener Called...", false);

        mNotification = message;

        printMessage(message);
    }

    @Override
    public void onProviderDiscovered(Provider provider) {
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
        log += " Topic = " + message.getTopic();
        log += " ProviderID = " + message.getProviderId();

        showLog(LogLevel.INFO, log, false);
    }

    String getProviderState(ProviderState state) {
        switch (state) {
            case ALLOW:
                return "ALLOW";

            case DENY:
                return "DENY";

            case TOPIC:
                return "TOPIC";

            case STOPPED:
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
