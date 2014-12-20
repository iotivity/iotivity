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

package OC.Cm;

public class DeviceToken {

    private long nativeHandle;

    private native long constructNative(String type, String domain);

    private native void deleteNative(long devToken);

    private native void setUUID(long nativeHandle, String UUID)
            throws Exception;

    private native void setDeviceToken(long nativeHandle, String deviceToken)
            throws Exception;

    private native String getUUID(long nativeHandle) throws Exception;

    private native String getDeviceToken(long nativeHandle) throws Exception;

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

    public DeviceToken(String UUID, String deviceToken) {
        nativeHandle = constructNative(UUID, deviceToken);
    }

    public DeviceToken() {
        nativeHandle = constructNative("default_uuid", "default_token");
    }

    public DeviceToken(long handle) {
        nativeHandle = handle;
    }

    public long getNativeHandle() {
        return nativeHandle;
    }

    public void setUUID(String UUID) throws Exception {
        setUUID(nativeHandle, UUID);
    }

    public void setDeviceToken(String deviceToken) throws Exception {
        setDeviceToken(nativeHandle, deviceToken);
    }

    public String getUUID() throws Exception {
        return getUUID(nativeHandle);
    }

    public String getDeviceToken() throws Exception {
        return getDeviceToken(nativeHandle);
    }

}