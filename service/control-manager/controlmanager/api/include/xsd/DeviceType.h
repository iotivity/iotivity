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



#ifndef DEVICETYPE_H_
#define DEVICETYPE_H_


/**
*  @addtogroup xsd
*  @{
*/
/**
 * @class DeviceType
 *
 * The enumeration set of device type which SHP supports.
 */
class DeviceType
{
    public:

        /**
         * Variable that contains DeviceType type value.
         */
        std::string value;

        /**
         * Check whether the value is following the rules or not.
         *
         * @return @c True if the value is valid@n
         *         @c False if the value is not valid
         */
        bool validateContent()
        {
            if (value == "Unknown") {}
            else if (value == "Gateway") {}
            else if (value == "Smartphone") {}
            else if (value == "Tablet") {}
            else if (value == "TV") {}
            else if (value == "AV") {}
            else if (value == "Refrigerator") {}
            else if (value == "Washer") {}
            else if (value == "Dryer") {}
            else if (value == "Air_Conditioner") {}
            else if (value == "Air_Purifier") {}
            else if (value == "Dishwasher") {}
            else if (value == "Oven") {}
            else if (value == "Microwave_Oven") {}
            else if (value == "Cooktop") {}
            else if (value == "Robot_Cleaner") {}
            else if (value == "Vacuum_Cleaner") {}
            else if (value == "Light") {}
            else if (value == "IP_Camera") {}
            else if (value == "Door_Lock") {}
            else if (value == "Door_Phone") {}
            else if (value == "Thermostat") {}
            else if (value == "Water_Heater") {}
            else if (value == "Smart_Plug") {}
            else if (value == "Generic_Sensor") {}
            else if (value == "Motion_Detector") {}
            else if (value == "Zigbee_Bridge") {}
            else if (value == "Zwave_Bridge") {}
            else if (value == "Coffee_Pot") {}
            else if (value == "Iron") {}
            else if (value == "Heater") {}
            else if (value == "Ventilator") {}
            else if (value == "Fan") {}
            else if (value == "Blind") {}
            else if (value == "Switch") {}
            else if (value == "Motor") {}
            else if (value == "Shutter") {}
            else
                return false;
            return true;
        }
};
/** @} */
#endif
