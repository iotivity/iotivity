/**
 * ***************************************************************
 * <p>
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 * <p>
 * <p>
 * <p>
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * <p>
 * http://www.apache.org/licenses/LICENSE-2.0
 * <p>
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * <p>
 * ****************************************************************
 */

package org.iotivity.service.notificationservice;

import android.util.Log;

import org.iotivity.service.notificationservice.impl.ImageNotification;
import org.iotivity.service.notificationservice.impl.TextNotification;
import org.iotivity.service.notificationservice.impl.VideoNotification;

public class NotificationServiceCallbackHandler {

    private static NotificationServiceCallbackHandler notificationServiceCallbackHandlerobj;
    private NotificationCallback mNotificationCallback;
    private NotificationProducerCallback mNotificationProducerCallback;
    private String TAG="NSCallbackHandler";

    public static synchronized NotificationServiceCallbackHandler getInstance() {
        if (notificationServiceCallbackHandlerobj == null)
            notificationServiceCallbackHandlerobj = new NotificationServiceCallbackHandler();
        return notificationServiceCallbackHandlerobj;
    }

    public void setNotificationCallback(NotificationCallback notificationCallback) {
        mNotificationCallback = notificationCallback;
    }


    public void setNotificationCallback(NotificationProducerCallback callback) {
        mNotificationProducerCallback = callback;

    }

    public void ReceivedNotificationcallback(String url, String message, String sender, String time, int type, int TTL, int id) {

        Log.d(TAG, "Received notifcation" + TTL + ":" + id);

        if (type == NotificationType.TYPE_TEXT.getValue()) {

            TextNotification textNotification = new TextNotification();
            textNotification.setNotificationMessage(message);
            textNotification.setNotifcationSender(sender);
            textNotification.setNotifcationTime(time);
            textNotification.setNotifcationTTL(TTL);
            textNotification.setNotifcationId(id);
            mNotificationCallback.onNotificationReceived(textNotification);

        } else if (type == NotificationType.TYPE_IMAGE.getValue()) {
            ImageNotification imageNotification = new ImageNotification();
            imageNotification.setNotificationImageUrl(url);
            imageNotification.setNotificationMessage(message);
            imageNotification.setNotifcationSender(sender);
            imageNotification.setNotifcationTime(time);
            imageNotification.setNotifcationId(id);
            imageNotification.setNotifcationTTL(TTL);
            mNotificationCallback.onNotificationReceived(imageNotification);
        } else if (type == NotificationType.TYPE_VIDEO.getValue()) {
            VideoNotification videoNotification = new VideoNotification();

            videoNotification.setNotificationVideoUrl(url);
            videoNotification.setNotifcationSender(sender);
            videoNotification.setNotifcationTime(time);
            videoNotification.setNotifcationId(id);
            videoNotification.setNotifcationTTL(TTL);
            mNotificationCallback.onNotificationReceived(videoNotification);

        }

    }

    public void onResourceDiscoveredCallback(String resourceName, int resourceIndex) {
        Log.d(TAG, "Resource discovered" + resourceIndex);
        mNotificationCallback.onResourceDiscoveredCallback(resourceName, resourceIndex);
    }

        public void onNotificationAcknowledgementReceievedCallback(int notificationID, int readStatus)
    {
        // TODO: 10/22/2015 add sdk code
        Log.d(TAG,"onNotificationAcknowledgementReceievedCallback:"+notificationID+":"+readStatus);
        mNotificationProducerCallback.onNotificationAcknowledgementReceieved(notificationID,readStatus);


    }


}


