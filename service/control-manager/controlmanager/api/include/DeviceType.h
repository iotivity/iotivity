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



#if !defined(_DEVICETYPE_H)
#define _DEVICETYPE_H

#include "CMConfig.h"

namespace OC
{

    namespace Cm
    {

        extern CMAPI const char *DeviceType_Strings[]
        ; /**< Represents array of supported device type strings */

        /**
        *  @addtogroup ControlManager
        *  @{
        */
        /**
         * @enum        DeviceType
         * @brief       This enumeration defines types of devices. To add a new device to Control Manager, developer needs to add the device to DeviceType enumeration.
         *          This enumeration consists of all devices supported by Control Manager and can be a Controller or Controllable or Both.
         */
        enum DeviceType
        {
            DeviceType_Unknown = 0,             /**< Represents a device whose type is unknown */
            DeviceType_Gateway,                 /**< Represents a Gateway device */
            DeviceType_Smartphone,              /**< Represents a smart phone */
            DeviceType_Tablet,                  /**< Represents a Tablet */
            DeviceType_TV,                      /**< Represents a TV */
            DeviceType_Refrigerator,            /**< Represents a Refrigerator */
            //DeviceType_Kimchi_Refrigerator,       /**< Represents a Kimchi Refrigerator */
            DeviceType_Washer,                  /**< Represents a Washer */
            DeviceType_Dryer,                   /**< Represents a Dryer */
            DeviceType_Air_Conditioner,         /**< Represents a Air Conditioner */
            //  DeviceType_System_Air_Conditioner,  /**< Represents a System Air Conditioner */
            DeviceType_Dishwasher,              /**< Represents a Dish washer */
            DeviceType_Oven,                    /**< Represents a Oven */
            DeviceType_Microwave_Oven,          /**< Represents a Microwave oven */
            DeviceType_Cooktop,                 /**< Represents a Cooktop */
            DeviceType_Robot_Cleaner,           /**< Represents a Robot cleaner */
            DeviceType_Light,                   /**< Represents a Light */
            DeviceType_IP_Camera,               /**< Represents a IP Camera */
            DeviceType_Door_Lock,               /**< Represents a Door lock */
            DeviceType_Door_Phone,              /**< Represents a Door Phone */
            DeviceType_Thermostat,              /**< Represents a Thermostat */
            DeviceType_Water_Heater,            /**< Represents a Water Heater */
            DeviceType_Smart_Plug,              /**< Represents a Smart Socket */
            DeviceType_Generic_Sensor,          /**< Represents a Generic Sensor */
            DeviceType_Motion_Detector,         /**< Represents a Motion Detector */
            DeviceType_Heater,                  /**< Represents a Heater */
            DeviceType_Coffee_Pot,              /**< Represents a Coffee Pot */
            DeviceType_Iron,                    /**< Represents a Iron */
            DeviceType_Vacuum_Cleaner,          /**< Represents a Vacuum Cleaner */
            DeviceType_Zigbee_Bridge,           /**< Represents a Zigbee Bridge */
            DeviceType_Zwave_Bridge,            /**< Represents a Zwave Bridge */
            DeviceType_All,                     /**< Represents a device whose type can be any */
        };
        /** @} */
    } /* namespace Cm */

} /* namespace OC */

#endif  //_DEVICETYPE_H
