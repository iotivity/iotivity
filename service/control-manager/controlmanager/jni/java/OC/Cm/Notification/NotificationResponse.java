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

import OC.Cm.Serialization.ISerializable;

public class NotificationResponse {

    private long nativeHandle;

    private native long constructNative(String resourcePath,
            String notificationType, String uuid, long notificationData);

    private native void deleteNative(long devToken);

    private native void setResourcePath(long nativeHandle, String resourcePath)
            throws Exception;

    private native void setNotificationType(long nativeHandle,
            String notificationType) throws Exception;

    private native void setNotificationData(long nativeHandle,
            long iSerializable) throws Exception;

    private native void setUUID(long nativeHandle, String UUID)
            throws Exception;

    private native String getUUID(long nativeHandle) throws Exception;

    private native String getResourcePath(long nativeHandle) throws Exception;

    private native String getNotificationType(long nativeHandle)
            throws Exception;

    private native ISerializable getNotificationData(long nativeHandle)
            throws Exception;

    @Override
    protected void finalize() throws Throwable {
        this.destroy();
        super.finalize();
    }

    public void destroy() {
        if (nativeHandle != 0) {
            this.deleteNative(nativeHandle);
            nativeHandle = 0;
        }
    }

    public void onNativeDelete() {
        nativeHandle = 0;
    }

    public NotificationResponse(String resourcePath, String notificationType,
            String uuid, ISerializable notificationData) {
        nativeHandle = constructNative(resourcePath, notificationType, uuid,
                notificationData.getNativeHandle());
    }

    public NotificationResponse() {
        nativeHandle = constructNative("", "", "", 0);
    }

    public NotificationResponse(long handle) {
        nativeHandle = handle;
    }

    public long getNativeHandle() {
        return nativeHandle;
    }

    public void setResourcePath(String resourcePath) throws Exception {
        setResourcePath(nativeHandle, resourcePath);
    }

    public void setNotificationType(String notificationType) throws Exception {
        setNotificationType(nativeHandle, notificationType);
    }

    public void setNotificationData(ISerializable notificationData)
            throws Exception {
        setNotificationData(nativeHandle, notificationData.getNativeHandle());
    }

    public void setUUID(String uuid) throws Exception {
        setUUID(nativeHandle, uuid);
    }

    public String getUUID() throws Exception {
        return getUUID(nativeHandle);
    }

    public String getResourcePath() throws Exception {
        return getResourcePath(nativeHandle);
    }

    public String getNotificationType() throws Exception {
        return getNotificationType(nativeHandle);
    }

    public ISerializable getNotificationData() throws Exception {
        return getNotificationData(nativeHandle);
    }
}