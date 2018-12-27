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

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.logging.Logger;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.iotivity.test.rfw.common.devicecontroller.interfaces.IDeviceController;
import org.iotivity.test.rfw.common.util.IoTivityLogger;

import org.iotivity.test.rfw.common.devicetestlib.DeviceControllerPluginManager;
import org.iotivity.test.rfw.common.devicetestlib.MainConfig;

public class MultiDeviceManager {

    class DeviceInfo {
        public IDeviceController controller;
        String                   deviceID;
    }

    private static MultiDeviceManager instance = new MultiDeviceManager();

    public static MultiDeviceManager getInstance() {
        return instance;
    }

    private ArrayList<String>                  deviceSerialList;
    private HashMap<String, IDeviceController> deviceMap;
    private Logger                             logger = IoTivityLogger
            .GetLogger();

    private MultiDeviceManager() {
        deviceSerialList = new ArrayList<String>();
        deviceMap = new HashMap<String, IDeviceController>();
    }

    public boolean addDevice(String deviceType, String deviceID) {
        try {
            for (int i = 0; i < 5; i++) {
                Thread.sleep(1000);
                if (getDeviceSerials(deviceType).size() > 0)
                    break;
            }
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        logger.info("DeviceSerials List :" + deviceSerialList.toString());

        for (String deviceSerial : deviceSerialList) {
            boolean IsAdded = false;
            for (IDeviceController tempDevice : deviceMap.values()) {
                if (deviceSerial.equals(tempDevice.getDeviceID())) {
                    logger.info(" already added deviceSerialList.get(i): "
                            + deviceSerial + "equals tempDevice.getDeviceID()"
                            + tempDevice.getDeviceID());
                    IsAdded = true;
                    break;
                }
            }

            if (IsAdded == false) {
                logger.info("Add device serial: " + deviceSerial);
                return addDevice(deviceType, deviceID, deviceSerial);
            }

        }

        return true;
    }

    public boolean addDevice(String deviceType, String deviceID, String spec) {
        logger.info("deviceType : " + deviceType + "  deviceID : " + deviceID
                + "  spec:" + spec);

        if (deviceMap.containsKey(deviceID)) {
            return true;
        }

        deviceMap.put(deviceID, DeviceControllerPluginManager
                .createDeviceController(deviceType, spec));
        logger.info("device Added");

        Runtime.getRuntime().gc();

        return deviceMap.containsKey(deviceID);
    }

    public IDeviceController getDevice(String deviceId) {

        //logger.info("getDevice in");

        IDeviceController deviceController = deviceMap.get(deviceId);

        //logger.info("getDevice out");

        return deviceController;
    }

    public void removeDevice(String deviceId) {
        getDevice(deviceId).close();
        deviceMap.remove(deviceId);
    }

    public void removeAllDevices() {
        try {
            for (String tempDeviceID : deviceMap.keySet()) {
                getDevice(tempDeviceID).close();
            }

            deviceMap.clear();
            deviceSerialList.clear();
        } catch (Exception ex) {
            logger.info("Exception occured when removing the devices");
        }
    }

    private ArrayList<String> getDeviceSerials(String deviceType) {
        try {
            deviceSerialList.clear();

            String cmd = MainConfig.deviceBridgeMap.get(deviceType)
                    + " devices";
            Process process = Runtime.getRuntime().exec(cmd);
            BufferedReader in = new BufferedReader(
                    new InputStreamReader(process.getInputStream()));
            String line = null;
            Pattern pattern = Pattern.compile(
                    "^([^ \\t\\r\\n\\v\\f]+)(\\s+)(device)(\\s*)([^ \\t\\r\\n\\v\\f]*)");
            Matcher matcher;
            while ((line = in.readLine()) != null) {
                if (line.matches(pattern.pattern())) {
                    matcher = pattern.matcher(line);
                    if (matcher.find())
                        deviceSerialList.add(matcher.group(1));
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        return deviceSerialList;
    }
}
