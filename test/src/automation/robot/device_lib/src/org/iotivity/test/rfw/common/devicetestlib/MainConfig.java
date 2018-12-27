/******************************************************************
 *
 * Copyright 2018 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 ******************************************************************/

package org.iotivity.test.rfw.common.devicetestlib;

import java.util.HashMap;

public class MainConfig {
    public class DeviceType {
        public static final String Linux   = "Linux";
        public static final String Android = "Android";
        public static final String Tizen   = "Tizen";
    }

    public static HashMap<String, String> deviceBridgeMap     = new HashMap<String, String>();
    public static HashMap<String, String> deviceControllerMap = new HashMap<String, String>();
    static {
        deviceControllerMap.put(DeviceType.Linux,
                org.iotivity.test.rfw.common.devicecontroller.linux.LinuxDeviceController.class
                        .getName());
        deviceControllerMap.put(DeviceType.Android,
                org.iotivity.test.rfw.common.devicecontroller.android.AndroidDeviceController.class
                        .getName());
        deviceBridgeMap.put(DeviceType.Android, "adb");
        deviceControllerMap.put(DeviceType.Tizen,
                org.iotivity.test.rfw.common.devicecontroller.tizen.TizenDeviceController.class
                        .getName());
        deviceBridgeMap.put(DeviceType.Tizen, "sdb");
    }

}
