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

public abstract class DeviceFactory {

    private long nativeDevFinder;

    private native long constructNative(DeviceFactory factory);

    private native void deleteNative(long serDatafactory);

    public DeviceFactory() {
        nativeDevFinder = constructNative(this);
    }

    @Override
    protected void finalize() throws Throwable {
        this.destroy();
        super.finalize();
    }

    public void destroy() {
        if (nativeDevFinder != 0) {
            this.deleteNative(nativeDevFinder);
            nativeDevFinder = 0;
        }
    }

    public void onNativeDelete() {
        nativeDevFinder = 0;
    }

    public abstract long createDevice(OC.Cm.DeviceType type,
            OC.Cm.DeviceDomain domain);

    public long getNativeHandle() {
        return nativeDevFinder;
    }
}
