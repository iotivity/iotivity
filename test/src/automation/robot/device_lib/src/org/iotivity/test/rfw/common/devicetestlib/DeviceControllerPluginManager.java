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

import java.lang.reflect.Constructor;

import org.iotivity.test.rfw.common.devicecontroller.interfaces.IDeviceController;

import org.iotivity.test.rfw.common.devicetestlib.MainConfig;

public class DeviceControllerPluginManager {

    static IDeviceController createDeviceController(String deviceType,
            String deviceID) {
        try {
            @SuppressWarnings("unchecked")
            Class<IDeviceController> deviceControlelr = (Class<IDeviceController>) Class
                    .forName(MainConfig.deviceControllerMap.get(deviceType));

            @SuppressWarnings("rawtypes")
            Class parameterType[] = new Class[1];
            parameterType[0] = String.class;

            @SuppressWarnings("rawtypes")
            Constructor constructor = deviceControlelr
                    .getConstructor(parameterType);
            Object parameter[] = new Object[1];
            parameter[0] = deviceID;

            return (IDeviceController) constructor.newInstance(parameter);

        } catch (Exception e) {
            e.printStackTrace();
        }

        return null;
    }
}
