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

package gen.devicefinder;

import OC.Cm.DeviceDomain;
import OC.Cm.DeviceType;
import OC.Cm.DeviceFinder.DeviceFactory;

/**
 * This class defines a method required to instantiate a Device Object.<br>
 * <br>
 * 
 * SHP framework uses SHPDeviceFactory to instantiate this generated Device
 * class.<br>
 * <br>
 * 
 * SHP-SDK also generates a DeviceFactory class which can instantiate the
 * generated Device class object. Developers must configure an instance of this
 * generated DeviceFactory with framework using
 * OC.Cm.Configuration.setDeviceFactory()
 * 
 * @since 1.0.0
 */
public class CMDeviceFactory extends DeviceFactory {
    /**
     * Instantiates the Device object
     * 
     * @param type
     *            Type of device (Air Conditioner, Washer, Dryer, etc.)
     * @param domain
     *            Device domain to which this device belongs (Local, Remote,
     *            All)
     * 
     * @return Handle of the OC.Cm.Device class
     * @since 1.0.0
     */
    @Override
    public long createDevice(DeviceType type, DeviceDomain domain) {
        return (new gen.cm.CMDevice(type, domain)).getNativeHandle();
    }
}
