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

package org.iotivity.service.ns.test.helper;

import org.iotivity.base.ModeType;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;
import org.iotivity.service.ns.provider.*;
import org.iotivity.service.ns.common.*;
import org.iotivity.service.ns.common.SyncInfo.SyncType;
import org.iotivity.service.ns.consumer.*;
import org.iotivity.service.ns.consumer.Provider.ProviderState;

import static org.iotivity.service.ns.test.helper.NSTestUtilily.*;
import android.content.Context;
import android.util.Log;

public class NSHelper implements ProviderService.OnConsumerSubscribedListener,
    ProviderService.OnMessageSynchronizedListener,
    ConsumerService.OnProviderDiscoveredListener,
    Provider.OnMessageReceivedListner, Provider.OnProviderStateListener,
    Provider.OnSyncInfoReceivedListner {
  private static NSHelper nsHelperInstance   = null;
  private Context         mContext           = null;
  private Consumer        mConsumer          = null;
  private Provider        mProvider          = null;
  private ProviderService mProviderService   = null;
  private ConsumerService mConsumerService   = null;
  private Message         mMessage           = null;
  private ProviderState   mProviderState     = null;
  private SyncInfo        mProviderSyncInfo  = null;
  private SyncInfo        mConsumerSyncInfo  = null;
  private boolean         mIsProviderStarted = false;
  private boolean         mIsConsumerStarted = false;

  private NSHelper(Context context) {
    mContext = context;
    configPlatform();
  }

  public static NSHelper getInstance(Context context) {
    if (nsHelperInstance == null) {
      nsHelperInstance = new NSHelper(context);
    }

    return nsHelperInstance;
  }

  private void configPlatform() {
    PlatformConfig platformConfig = new PlatformConfig(mContext,
        ServiceType.IN_PROC, ModeType.CLIENT_SERVER, "0.0.0.0", 0,
        QualityOfService.LOW);

    Log.i(TAG, "Configuring platform.");

    OcPlatform.Configure(platformConfig);
    try {
      OcPlatform.stopPresence();
    } catch (Exception e) {
      Log.e(TAG, "Exception: stopping presence when configuration step: " + e);
    }
    Log.i(TAG, "Configuration done Successfully");
  }

  public void stopServices() {
    if (mIsProviderStarted) {
      try {
        mProviderService.stop();
        mIsProviderStarted = false;
        mProviderService = null;

        Log.i(TAG, "Provider Service Stopped Successfully");
      } catch (NSException e) {
        e.printStackTrace();
      }
    }

    if (mIsConsumerStarted) {
      try {
        mConsumerService.stop();
        mIsConsumerStarted = false;
        mConsumerService = null;

        Log.i(TAG, "Consumer Service Stopped Successfully");
      } catch (NSException e) {
        e.printStackTrace();
      }
    }
  }

  public boolean stopProviderService() {
    if (mProviderService == null) {
      return false;
    }

    try {
      int result = mProviderService.stop();

      if (result != NS_OK) {
        Log.e(TAG, "Can't stop provider service");
        return false;
      }
    } catch (NSException e) {
      e.printStackTrace();
      return false;
    }

    mIsProviderStarted = false;

    return true;
  }

  public Consumer getConsumer(boolean subControllability,
      boolean isTopicRegister) {
    mProvider = null;
    mConsumer = null;

    mProviderService = new ProviderService();
    mConsumerService = new ConsumerService();

    try {
      mProviderService.start(this, this, subControllability, USER_INFO,
          IS_SECURED);
      mIsProviderStarted = true;

      if (isTopicRegister)
        mProviderService.registerTopic(TOPIC_NAME);

      waitInSecond(WAIT_MIN_TIME);

      mConsumerService.start(this);
      mIsConsumerStarted = true;

      waitInSecond(WAIT_MIN_TIME);

      mConsumerService.rescanProvider();
      waitInSecond(WAIT_MIN_TIME);
    } catch (NSException e) {
      Log.e(TAG, "Can't Start Consumer Service");
      return null;
    }

    if (subControllability) {
      boolean isFound = waitForService(WAIT_MAX_TIME, false);

      if (!isFound) {
        Log.w(TAG, "Can't find provider");
      }
    } else {
      boolean isFound = waitForService(WAIT_MAX_TIME, true);

      if (isFound) {
        try {
          mProvider.subscribe();
        } catch (NSException e) {
          Log.e(TAG, "Can't Subscribe to provider");
        }
      } else {
        Log.e(TAG, "Can't find provider");
      }

      isFound = waitForService(WAIT_MAX_TIME, false);

      if (isFound) {
        Log.i(TAG, "Consumer Found Successfully");
      } else {
        Log.w(TAG, "Can't find any Consumer");
      }
    }

    return mConsumer;
  }

  public Provider getProvider(boolean subControllability) {
    mProvider = null;
    mConsumer = null;

    mProviderService = ProviderService.getInstance();
    mConsumerService = ConsumerService.getInstance();

    try {
      mProviderService.start(this, this, subControllability, USER_INFO,
          IS_SECURED);
      mProviderService.registerTopic(TOPIC_NAME);
      mIsProviderStarted = true;
      waitInSecond(WAIT_MIN_TIME);

      mConsumerService.start(this);
      mIsConsumerStarted = true;
      mConsumerService.rescanProvider();
    } catch (NSException e) {
      Log.e(TAG, "Can't Start. Exception is " + e.getLocalizedMessage());
    }

    waitForService(WAIT_MAX_TIME, true);

    return mProvider;
  }

  public boolean sendNotification() {
    if (mProviderService == null || mConsumer == null) {
      Log.e(TAG, "ProviderService or Consumer is null");
      return false;
    }

    try {
      mProviderService.registerTopic(TOPIC_NAME);
      mConsumer.setTopic(TOPIC_NAME);
    } catch (NSException e) {
      e.printStackTrace();
      return false;
    }

    Message msg = null;

    try {
      msg = mProviderService.createMessage();
      msg.mTitle = MSG_TITLE;
      msg.mContentText = MSG_BODY;
      msg.mSourceName = MSG_SOURCE;
      msg.mTopic = TOPIC_NAME;

      MediaContents mediaContents = new MediaContents("Image");

      msg.mMediaContents = mediaContents;
    } catch (NSException e) {
      e.printStackTrace();
      return false;
    }

    try {
      mMessage = null;
      int result = mProviderService.sendMessage(msg);

      if (result != NS_OK) {
        Log.e(TAG, "SendMessage is failed.");
        return false;
      }
    } catch (NSException e) {
      e.printStackTrace();
      return false;
    }

    Log.i(TAG, "Notification Sent");

    return true;
  }

  public Message getNotification() {
    return mMessage;
  }

  private boolean waitForService(int timeOut, boolean isProvider) {
    int progessTime = 0;

    while (true) {
      if (isProvider)
        Log.d(TAG, "Waiting for Provider " + progessTime + "...");
      else
        Log.d(TAG, "Waiting for Consumer " + progessTime + "...");

      if (isProvider) {
        if (mProvider != null) {
          Log.i(TAG, "Provider found with ID: " + mProvider.getProviderId());
          return true;
        }
      } else {
        if (mConsumer != null) {
          Log.i(TAG, "Consumer found with ID: " + mConsumer.getConsumerId());
          return true;
        }
      }

      waitInSecond(WAIT_MIN_TIME);

      if (++progessTime > timeOut) {
        break;
      }
    }

    return false;
  }

  public void waitInSecond(int time) {
    int timeInSec = time * 1000;
    try {
      Thread.sleep(timeInSec);
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
    }
  }

  public boolean sendSyncInfo(boolean isProvider, SyncType type) {
    if (mMessage == null) {
      Log.e(TAG, "Notification is null");
      return false;
    }

    if (isProvider) {
      if (mProviderService == null) {
        Log.e(TAG, "Provider service is null");
        return false;
      }
    } else {
      if (mProvider == null) {
        Log.e(TAG, "Provider is null");
        return false;
      }
    }

    try {
      if (isProvider)
        mProviderService.sendSyncInfo(mMessage.mMessageId, type);
      else
        mProvider.sendSyncInfo(mMessage.mMessageId, type);
    } catch (NSException e) {
      e.printStackTrace();
      return false;
    }

    return true;
  }

  public SyncInfo getSyncInfo(boolean isProvider) {
    if (isProvider)
      return mProviderSyncInfo;
    else
      return mConsumerSyncInfo;
  }

  public ProviderState getProviderState() {
    return mProviderState;
  }

  // Provider Callback
  @Override
  public void onMessageSynchronized(SyncInfo syncInfo) {
    Log.i(TAG, "onMessageSynchronized Listener Called...");

    mConsumerSyncInfo = syncInfo;

    printSyncInfo(syncInfo);
  }

  @Override
  public void onConsumerSubscribed(Consumer consumer) {
    Log.i(TAG, "onConsumerSubscribed Listener Called with ConsumerID: "
        + consumer.getConsumerId() + "...");
    mConsumer = consumer;
  }

  // Consumer Callback
  @Override
  public void onSyncInfoReceived(SyncInfo sync) {
    Log.i(TAG, "onSyncInfoReceived Listener Called ...");

    mProviderSyncInfo = sync;

    printSyncInfo(sync);
  }

  @Override
  public void onProviderStateReceived(ProviderState state) {
    Log.i(TAG, "onProviderStateReceived Listener Called...");

    mProviderState = state;
  }

  @Override
  public void onMessageReceived(Message message) {
    Log.i(TAG, "onMessageReceived Listener Called...");

    mMessage = message;

    printMessage(mMessage);
  }

  @Override
  public void onProviderDiscovered(Provider provider) {
    Log.i(TAG, "onProviderDiscovered Listener Called with ProviderID: "
        + provider.getProviderId() + "...");

    mProvider = provider;

    try {
      mProvider.setListener(this, this, this);
    } catch (NSException e) {
      Log.e(TAG, "Can't set listener");
    }
  }

  void printMessage(Message message) {
    String log = "MessageID: " + message.getMessageId();
    log += " Title = " + message.getTitle();
    log += " Body = " + message.getContentText();
    log += " Topic = " + message.getTopic();
    log += " ProviderID = " + message.getProviderId();

    Log.i(TAG, log);
  }

  void printSyncInfo(SyncInfo info) {
    try {
      Log.i(TAG, "SyncInfo --> ProviderID: " + info.getProviderId()
          + " MessageID: " + info.getMessageId() + " State: "
          + getSyncType(info.getState()));
    } catch (Exception e) {
      Log.w(TAG, "Can't print syncInfo. Exception: " + e.getLocalizedMessage());
    }
  }

  String getSyncType(SyncType type) {
    switch (type) {
      case UNREAD:
        return "UNREAD";

      case READ:
        return "READ";

      case DELETED:
        return "DELETED";

      default:
        return "UNKNOWN";
    }
  }
}
