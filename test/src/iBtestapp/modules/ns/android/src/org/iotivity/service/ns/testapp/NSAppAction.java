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

import org.iotivity.service.ns.testapp.NSAppHelper.LogLevel;
import org.iotivity.service.testapp.framework.Base;

public class NSAppAction extends Base {
    private NSAppHelper mHelper = null;

    public NSAppAction() {
        mHelper = NSAppHelper.getInstance();
    }

    // Provider Methods
    public void startProvider() {
        mHelper.showLog(LogLevel.INFO,
                "Starting Provider Service as Provider Acceptor...", true);
        mHelper.startProvider(true);
    }

    public void startProviderForConsumer() {
        mHelper.showLog(LogLevel.INFO,
                "Starting Provider Service as Consumer Acceptor...", true);
        mHelper.startProvider(false);
    }

    public void stopProvider() {
        mHelper.showLog(LogLevel.INFO, "Stopping Provider Service...", true);
        mHelper.stopProvider();
    }

    public void acceptSubscription() {
        mHelper.showLog(LogLevel.INFO, "Accepting Subscription...", true);
        mHelper.pendingSubscripton(true);
    }

    public void rejectSubscription() {
        mHelper.showLog(LogLevel.INFO, "Rejecting Subscription...", true);
        mHelper.pendingSubscripton(false);
    }

    public void sendNotification() {
        mHelper.showLog(LogLevel.INFO, "Sending Notification...", true);
        mHelper.sendMessage();
    }

    public void sendProviderSyncInfo() {
        mHelper.showLog(LogLevel.INFO, "Sending SyncInfo...", true);
        mHelper.sendProviderSyncInfo();
    }

    public void addTopic() {
        mHelper.showLog(LogLevel.INFO, "Registering Topic...", true);
        mHelper.registerTopic();
    }

    public void deleteTopic() {
        mHelper.showLog(LogLevel.INFO, "Unregistering Topic...", true);
        mHelper.unRegisterTopic();
    }

    public void selectTopic() {
        mHelper.showLog(LogLevel.INFO, "Setting topic...", true);
        mHelper.setTopic();
    }

    public void unselectTopic() {
        mHelper.showLog(LogLevel.INFO, "Unsetting topic...", true);
        mHelper.unSetTopic();
    }

    public void consumerTopics() {
        mHelper.showLog(LogLevel.INFO, "Getting Consumer Topic...", true);
        mHelper.getConsumerTopics();
    }

    public void showTopics() {
        mHelper.showLog(LogLevel.INFO, "Getting Registered Topic...", true);
        mHelper.getProviderTopics();
    }

    // Consumer Methods
    public void startConsumer() {
        mHelper.showLog(LogLevel.INFO, "Starting Consumer...", true);
        mHelper.startConsumer();
    }

    public void stopConsumer() {
        mHelper.showLog(LogLevel.INFO, "Stopping Consumer...", true);
        mHelper.stopConsumer();
    }

    public void subscribeConsumer() {
        mHelper.showLog(LogLevel.INFO, "Subscribing Consumer...", true);
        mHelper.subscribeConsumer();
    }

    public void rescanProvider() {
        mHelper.showLog(LogLevel.INFO, "Rescanning Provider...", true);
        mHelper.manualScan();
    }

    public void sendConsumerSyncInfo() {
        mHelper.showLog(LogLevel.INFO, "Sending SyncInfo to Provider...", true);
        mHelper.sendConsumerSyncInfo();
    }

    public void getTopics() {
        mHelper.showLog(LogLevel.INFO, "Getting Provider Topics...", true);
        mHelper.getTopics();
    }

    public void updateTopics() {
        mHelper.showLog(LogLevel.INFO, "Updating Topics...", true);
        mHelper.updateTopics();
    }

    public void dummyFunction() {
    }

    public void exitApplication() {
        mHelper.showLog(LogLevel.INFO, "Quit from NSTestApp...", true);

        android.os.Process.killProcess(android.os.Process.myPid());
    }
}
