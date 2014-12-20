//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
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

package OC.Cm.Notification;

import java.util.HashMap;
import java.util.Map;

public class ISubscriptionManager {

    private long nativeSubscriptionHandle;

    private long listener;

    private long provider;

    private native long addSubscriptionProvider(long subsctionHandle,
            OC.Cm.Notification.ISubscriptionProvider notificationListener)
            throws Exception;

    private native void removeSubscriptionProvider(long subsctionHandle,
            long listeber) throws Exception;

    private native long addNotificationListener(long subsctionHandle,
            OC.Cm.Notification.INotificationListener notificationListener)
            throws Exception;

    private native void removeNotificationListener(long subsctionHandle,
            long listeber) throws Exception;

    private native int addSubscription(long subsctionHandle, String protocol,
            String duuid, String deviceAddress, String suuid,
            String[] resourcePaths,
            OC.Cm.Client.Resource.IResourceResponseListener listener,
            String description) throws Exception;

    private native HashMap<Object, Object> removeSubscription(
            long subsctionHandle, String uuid, String deviceAddress,
            String protocol, String[] resourcePaths,
            OC.Cm.Client.Resource.IResourceResponseListener listener)
            throws Exception;

    private native boolean notify(long subsctionHandle, long notificationData)
            throws Exception;

    private native boolean notifymultiple(long subsctionHandle,
            long[] notificationList) throws Exception;

    private native boolean notifyAll(long subsctionHandle, String resourcePath,
            String notificationType,
            OC.Cm.Serialization.ISerializable notificationData)
            throws Exception;

    private native String[] getMySubscription(long subsctionHandle, String uuid);

    private native int removeSubscription(long subsctionHandle, String uuid,
            String deviceAddress, String subscriptionUri, String protocol,
            OC.Cm.Client.Resource.IResourceResponseListener listener)
            throws Exception;

    public ISubscriptionManager(long handle) {

        nativeSubscriptionHandle = handle;

        listener = 0;

        provider = 0;
    }

    public String[] getMySubscription(String uuid) {
        return getMySubscription(nativeSubscriptionHandle, uuid);
    }

    public void addNotificationListener(
            OC.Cm.Notification.INotificationListener notificationListener)
            throws Exception {
        listener = this.addNotificationListener(nativeSubscriptionHandle,
                notificationListener);
    }

    public void removeNotificationListener(
            OC.Cm.Notification.INotificationListener notificationListener)
            throws Exception {
        if (listener != 0) {
            this.removeNotificationListener(nativeSubscriptionHandle,
                    (long) listener);
            listener = 0;
        }
    }

    public void addSubscriptionProvider(
            OC.Cm.Notification.ISubscriptionProvider subscriptionProvider)
            throws Exception {
        provider = this.addSubscriptionProvider(nativeSubscriptionHandle,
                subscriptionProvider);
    }

    public void removeSubscriptionProvider(
            OC.Cm.Notification.ISubscriptionProvider subscriptionProvider)
            throws Exception {
        if (provider != 0) {
            this.removeSubscriptionProvider(nativeSubscriptionHandle,
                    (long) provider);
            provider = 0;
        }
    }

    public int addSubscription(String protocol, String duuid,
            String deviceAddress, String suuid, String[] resourcePaths,
            OC.Cm.Client.Resource.IResourceResponseListener listener,
            String description) throws Exception {
        return this.addSubscription(nativeSubscriptionHandle, protocol, duuid,
                deviceAddress, suuid, resourcePaths, listener, description);
    }

    /**
     * 
     * Sends a unsubscribe request to subscription uri
     * 
     * @param uuid
     *            uuid of the device to which request needs to be sent
     * @param deviceAddress
     *            address of the device to which request needs to be sent
     * @param subscriptionUri
     *            subscription uri
     * @param protocol
     *            protocol to be used to send the request (http or https)
     * @param listener
     *            listener to receive subscription response
     * @param requestId
     *            request id.
     * 
     * 
     * @return requestId request id.
     * 
     * @see Sec::Cm::Client::Resource::IResourceResponseListener::
     *      onUnSubscribeResponseReceived()
     * 
     */
    public int removeSubscription(String uuid, String deviceAddress,
            String subscriptionUri, String protocol,
            OC.Cm.Client.Resource.IResourceResponseListener listener)
            throws Exception {
        return this.removeSubscription(nativeSubscriptionHandle, uuid,
                deviceAddress, subscriptionUri, protocol, listener);
    }

    /**
     * 
     * Sends a unsubscribe request to a list of resource paths. this request
     * will send more than one requests
     * 
     * 
     * @param uuid
     *            uuid of the device to which request needs to be sent
     * @param deviceAddress
     *            address of the device to which request needs to be sent
     * @param protocol
     *            protocol to be used to send the request (http or https)
     * @param resourcePaths
     *            list of resource paths to unsubscribe
     * @param listener
     *            listener to receive subscription response
     * 
     * 
     * @return requestIDMap map containing ids of the requests sent by this api.
     * 
     * @see Sec::Cm::Client::Resource::IResourceResponseListener::
     *      onUnSubscribeResponseReceived()
     * 
     */
    public Map<Integer, String[]> removeSubscription(String uuid,
            String deviceAddress, String protocol, String[] resourcePaths,
            OC.Cm.Client.Resource.IResourceResponseListener listener)
            throws Exception {
        HashMap<Object, Object> result = this.removeSubscription(
                nativeSubscriptionHandle, uuid, deviceAddress, protocol,
                resourcePaths, listener);
        HashMap<Integer, String[]> retMap = new HashMap<Integer, String[]>();

        for (Object key : result.keySet()) {
            Object value = result.get(key);

            // if ((key instanceof Integer) && (value instanceof String[]))
            {
                retMap.put((Integer) key, (String[]) value);
            }
        }

        return retMap;
    }

    /**
     * 
     * This method is used to send notifications to subscribed devices.
     * 
     * @param resourcePath
     *            resource path
     * @param notificationType
     *            notification type to which notification data belongs. (Allowed
     *            values are Created, Deleted, Notified)
     * @param notificationData
     *            notification data
     * 
     * @return returns @c true if request is sent successfully @n
     * @c false in case of any error
     * 
     */
    public boolean notify(
            OC.Cm.Notification.NotificationResponse notificationData)
            throws Exception {
        if (notificationData != null) {
            return this.notify(nativeSubscriptionHandle,
                    notificationData.getNativeHandle());
        }
        return false;
    }

    /**
     * 
     * This method is used to send notifications of multiple resources to
     * subscribed devices.
     * 
     * @param notificationDataList
     *            List of notification data objects
     * 
     * @return returns @c true if request is sent successfully @n
     * @c false in case of any error
     * 
     */
    public boolean notify(
            OC.Cm.Notification.NotificationResponse[] notificationList)
            throws Exception {
        if (notificationList.length > 0) {
            long[] notiflist = new long[notificationList.length];
            int count = 0;
            for (OC.Cm.Notification.NotificationResponse devtoken : notificationList) {
                notiflist[count] = devtoken.getNativeHandle();
                count++;
            }
            return this.notifymultiple(nativeSubscriptionHandle, notiflist);
        }
        return false;
    }

    /**
     * 
     * This method is used to broadcast notifications to all Cm devices.
     * 
     * Currently Not Implemented
     * 
     * @param resourcePath
     *            resource path
     * @param notificationData
     *            notification data
     * @param notificationType
     *            element type to which notification data belongs.
     * 
     * @return returns @c true if request is sent successfully @n
     * @c false in case of any error
     * 
     */
    private boolean notifyAll(String resourcePath, String notificationType,
            OC.Cm.Serialization.ISerializable notificationData)
            throws Exception {
        return this.notifyAll(nativeSubscriptionHandle, resourcePath,
                notificationType, notificationData);
    }
}
