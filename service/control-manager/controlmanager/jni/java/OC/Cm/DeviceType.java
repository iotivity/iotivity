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

public enum DeviceType {
    DeviceType_Unknown(0), DeviceType_Gateway(1), DeviceType_Smartphone(2), DeviceType_Tablet(
            3), DeviceType_TV(4), DeviceType_Refrigerator(5),
    // DeviceType_Kimchi_Refrigerator(6),
    DeviceType_Washer(6), DeviceType_Dryer(7), DeviceType_Air_Conditioner(8),
    // DeviceType_System_Air_Conditioner(10)
    DeviceType_Dishwasher(9), DeviceType_Oven(10), DeviceType_Microwave_Oven(11), DeviceType_Cooktop(
            12), DeviceType_Robot_Cleaner(13), DeviceType_Light(14), DeviceType_IP_Camera(
            15), DeviceType_Door_Lock(16), DeviceType_Door_Phone(17), DeviceType_Thermostat(
            18), DeviceType_Water_Heater(19), DeviceType_Smart_Plug(20), DeviceType_Generic_Sensor(
            21), DeviceType_Motion_Detector(22), DeviceType_Heater(23), DeviceType_Coffee_Pot(
            24), DeviceType_Iron(25), DeviceType_Vacuum_Cleaner(26), DeviceType_Zigbee_Bridge(
            27), DeviceType_Zwave_Bridge(28);

    private int value;

    private DeviceType(int value) {
        this.value = value;
    }

    private DeviceType() {
    }

    public int getValue() {
        return value;
    }

    public static DeviceType getDeviceType(int value) {
        for (DeviceType error : DeviceType.values()) {
            if (error.getValue() == value) {
                return error;
            }
        }

        return DeviceType_Unknown;
    }
}