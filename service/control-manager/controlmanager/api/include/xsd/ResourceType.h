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



#ifndef RESOURCETYPE_H_
#define RESOURCETYPE_H_

/**
*  @addtogroup xsd
*  @{
*/
/**
 * @class ResourceType
 *
 * The enumeration set of resource type which the device supports.
 */
class ResourceType
{
    public:

        /**
         * Variable that contains ResourceType type value.
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
            if (value == "AccessPoint" ) {}
            else if (value == "AccessPoints" ) {}
            else if (value == "Alarm" ) {}
            else if (value == "Alarms" ) {}
            else if (value == "Configuration" ) {}
            else if (value == "Detection" ) {}
            else if (value == "Doors" ) {}
            else if (value == "Capability" ) {}
            else if (value == "Humidity" ) {}
            else if (value == "Information" ) {}
            else if (value == "Level" ) {}
            else if (value == "Light" ) {}
            else if (value == "Mode" ) {}
            else if (value == "Movement" ) {}
            else if (value == "Operation" ) {}
            else if (value == "Devices" ) {}
            else if (value == "Temperatures" ) {}
            else if (value == "Device" ) {}
            else if (value == "Temperature" ) {}
            else if (value == "Door" ) {}
            else if (value == "Energy" ) {}
            else if (value == "EnergyConsumption" ) {}
            else if (value == "LongPollingNotification" ) {}
            else if (value == "Subscriptions" ) {}
            else if (value == "Subscription" ) {}
            else if (value == "Notifications" ) {}
            else if (value == "Notification" ) {}
            else if (value == "Network" ) {}
            else if (value == "Networks" ) {}
            else if (value == "WiFi" ) {}
            else if (value == "Versions" ) {}
            else if (value == "Version" ) {}
            else if (value == "Usages" ) {}
            else if (value == "Usage" ) {}
            else if (value == "Time" ) {}
            else if (value == "Thermostat" ) {}
            else { return false; }
            return true ;
        }
};
/** @} */
#endif
