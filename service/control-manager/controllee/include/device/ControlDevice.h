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

#if !defined(_ControlDevice_H)
#define _ControlDevice_H

#include "Capability.h"
#include "Device.h"
#include "DeviceType.h"
#include "DeviceDomain.h"
#include "ISerializable.h"

#include "list"
#include "string"
#include "Elements.h"

/**
 * @class   ControlDevice
 * @brief   ControlDevice class implements OC::Cm::Device and provides methods to maintain device information of the devices which are discovered.
 */

class ControlDevice: public OC::Cm::Device
{

    public:

        /**
         * ControlDevice - Constructor of ControlDevice class.
         *
         * @param type      Type of device (Air Conditioner, Washer, Dryer, etc.)
         * @param domain    Device domain to which this device belongs (Local, Remote, All)
         */
        ControlDevice(OC::Cm::DeviceType type, OC::Cm::DeviceDomain domain);

        /**
         * getSupportedResourceTypes - Return all the resources supported by the device.
         *
         * @return type:std::list<std::string> List of string that contains all the resources supported by the device.
         */
        std::list<std::string> getSupportedResourceTypes();

        /**
         * METHOD CURRENTLY NOT USED. PLEASE DO NOT USE.
         */
        //OC::Cm::Client::Resource::Resource* getResource(std::string resourceType);
        /**
         * Configures the current instance of device object with specified device description.
         *
         * @param[ serializable Device description to be set
         *
         * @return  True if device description is successfully set False if device description is failed to set
         */
        virtual bool loadDeviceDescription(
            OC::Cm::Serialization::ISerializable &serializable);

        /**
         * METHOD CURRENTLY NOT USED. PLEASE DO NOT USE.
         */
        virtual bool getDeviceDescription(
            OC::Cm::Serialization::ISerializable **serializable);

        /**
         * getSubscriptionsLink - Returns subscriptions link.
         *
         * @return Subscriptions link string
         */
        virtual std::string getSubscriptionsLink();

        /**
         * Capability class variable
         */
        Capability m_capabilities;

    private:
        std::list<std::string> m_resourceTypes;/**< List string variable that contains all the resources supported by the device.*/
        std::string m_subscriptionsLink;/**< Represents the OCPlatform class instance which is under the OIC base. This can be used for OC layer operations.*/
};

#endif  //_ControlDevice_H
