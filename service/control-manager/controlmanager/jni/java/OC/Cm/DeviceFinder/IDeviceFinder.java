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

package OC.Cm.DeviceFinder;

import java.util.HashMap;
import java.util.Map;

import OC.Cm.DeviceType;
import OC.Cm.Client.Resource.IResourceResponseListener;

public class IDeviceFinder {
    private long                                                nativeHandle;
    private Map<Long, OC.Cm.DeviceFinder.IDeviceFinderListener> nativeListenersMap = new HashMap<Long, OC.Cm.DeviceFinder.IDeviceFinderListener>();

    private native boolean addDevice(long devFinderHandler, int type,
            String uuid, String address, IResourceResponseListener respListener)
            throws Exception;

    private native boolean refresh(long devFinderHandler) throws Exception;

    private native long setDeviceFinderListener(long devFinderHandler,
            OC.Cm.DeviceFinder.IDeviceFinderListener listener) throws Exception;

    private native void setDeviceFinderListener(long devFinderHandler,
            int deviceType, OC.Cm.DeviceFinder.IDeviceFinderListener listener)
            throws Exception;

    private native void setDeviceFinderListener(long devFinderHandler,
            int domain, int deviceType,
            OC.Cm.DeviceFinder.IDeviceFinderListener listener) throws Exception;

    private native OC.Cm.Device[] getDevices(long devFinderHandler)
            throws Exception;

    private native OC.Cm.Device[] getDevices(long devFinderHandler,
            int deviceType) throws Exception;

    private native OC.Cm.Device[] getDevices(long devFinderHandler, int domain,
            int deviceType) throws Exception;

    private native void removeDeviceFinderListener(long devFinderHandler,
            long listener);

    public IDeviceFinder(long handle) {
        nativeHandle = handle;
    }

    public boolean refresh() throws Exception {
        return this.refresh(nativeHandle);
    }

    public void setDeviceFinderListener(
            OC.Cm.DeviceFinder.IDeviceFinderListener listener) throws Exception {
        long retListener = this.setDeviceFinderListener(nativeHandle, listener);
        nativeListenersMap.put(retListener, listener);
    }

    public void setDeviceFinderListener(int deviceType,
            OC.Cm.DeviceFinder.IDeviceFinderListener listener) throws Exception {
        this.setDeviceFinderListener(nativeHandle, deviceType, listener);
    }

    public void setDeviceFinderListener(int domain, int deviceType,
            OC.Cm.DeviceFinder.IDeviceFinderListener listener) throws Exception {
        this.setDeviceFinderListener(nativeHandle, domain, deviceType, listener);
    }

    public OC.Cm.Device[] getDevices() throws Exception {
        return this.getDevices(nativeHandle);
    }

    public OC.Cm.Device[] getDevices(int deviceType) throws Exception {
        return this.getDevices(nativeHandle, deviceType);
    }

    public OC.Cm.Device[] getDevices(int domain, int deviceType)
            throws Exception {
        return this.getDevices(nativeHandle, domain, deviceType);
    }

    public void removeDeviceFinderListener(
            OC.Cm.DeviceFinder.IDeviceFinderListener listener) {
        if (null != listener) {
            long keyFound = 0;

            for (Long key : nativeListenersMap.keySet()) {
                OC.Cm.DeviceFinder.IDeviceFinderListener liste = nativeListenersMap
                        .get(key);

                if (liste == listener) {
                    keyFound = key;
                    break;
                }
            }

            if (keyFound != 0) {
                this.removeDeviceFinderListener(nativeHandle, keyFound);
                nativeListenersMap.remove(keyFound);
            }
        }
    }

    public void addDevice(DeviceType type, String uuid, String address,
            IResourceResponseListener respListener) throws Exception {
        this.addDevice(nativeHandle, type.getValue(), uuid, address,
                respListener);
    }
}
