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

public class NotificationService {



    private native void Discover();

    private native void StartNotificationProducer(String DeviceName);

    private native void StartSubscribeNotifications(int resourceIndex);

    private native void StopSubscribeNotifications();

    private native void SendNotification(String URL, String Message, String Sender, int type);

    private static NotificationService notificationServiceobj = null;
    private String Tag = "NotificationService";

    static {
        // load the static libraries

        System.loadLibrary("gnustl_shared");
        System.loadLibrary("connectivity_abstraction");
        System.loadLibrary("octbstack");
        System.loadLibrary("oc_logger");
        System.loadLibrary("oc");
        System.loadLibrary("rcs_client");
        System.loadLibrary("rcs_server");
        System.loadLibrary("rcs_common");
        System.loadLibrary("notification_service_jni");
    }

    private NotificationService() {

    }

    public static synchronized NotificationService getInstance() {
        if (null == notificationServiceobj) {
            notificationServiceobj = new NotificationService();
        }
        return notificationServiceobj;
    }

    public void discover() {
        Log.d(Tag, "Inside discover");
        Discover();
    }

    public void startnotificationproducer(String DeviceName) {
        Log.d(Tag, "StartNotificationProducer");
        StartNotificationProducer(DeviceName);
    }

    public void subscribesnotifications(int resourceIndex) {
        StartSubscribeNotifications(resourceIndex);
    }

    public void stopSubscribeNotifications() {
        StopSubscribeNotifications();
    }

    public void sendnotification(NotificationObject notificationObject) {

        if (notificationObject.getNotificationType() == NotificationType.TYPE_TEXT) {
            TextNotification textNotification = (TextNotification) notificationObject;
            SendNotification(null, textNotification.getNotificationMessage(), textNotification.getNotifcationSender(), textNotification.getNotificationType().getValue());
        } else if (notificationObject.getNotificationType() == NotificationType.TYPE_IMAGE) {
            ImageNotification imageNotification = (ImageNotification) notificationObject;
            SendNotification(imageNotification.getNotificationImageUrl(), imageNotification.getNotificationMessage(), imageNotification.getNotifcationSender(), imageNotification.getNotificationType().getValue());
        } else if (notificationObject.getNotificationType() == NotificationType.TYPE_VIDEO) {
            VideoNotification videoNotification = (VideoNotification) notificationObject;
            SendNotification(videoNotification.getNotificationVideoUrl(), null, videoNotification.getNotifcationSender(), videoNotification.getNotificationType().getValue());
        }

    }
}
