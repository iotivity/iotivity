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

#ifndef MyDevice_H_
#define MyDevice_H_

#include "ApplicationType.h"
#include <string>
#include <list>
#include "Capability.h"
#include "Devices.h"
#include "Device.h"
#include "ResourceType.h"

using namespace std;

/**
 * @class   MyDevice
 * @brief   Create a self device for the device discovery.
 * @n
 * This class provide methods to contain self device information that only used for a device discovery purpose.
 */
class MyDevice: public OC::Cm::Device
{
    public:
        /**
         * getInstance - Returns a singleton instance of MyDevice.
         *@param -none
         * @return -type:MyDevice* - Singleton instance of MyDevice.
         */
        static MyDevice *getInstance();

        /**
         * getSupportedResourceTypes - Return all the resources supported by the self device.
         *@param -none
         * @return -type:list< ::ResourceType >&- List of string that contains all the resources supported by the device.
         */
        std::list< ::ResourceType>& getSupportedResourceTypes();

        /**
         * setSupportedResourceType- To set a supported resource information, user should repeatedly call this method as many times as the number of resources that are supported by the device.
         *
         * @param -resource Supported resource to be added in the m_resourceTypes private variable.
         *
         * @return  type:bool - True if given device resource is successfully added else False if given device resource is not added
         */
        bool setSupportedResourceType(std::string resource);

        /**
         * getSubscriptionsLink- Returns a subscription link of the self device.
         *@param - none
         * @return type:string Subscription link of the self device.
         */
        virtual std::string getSubscriptionsLink();

        /**
         * METHOD CURRENTLY NOT USED. PLEASE DO NOT USE.
         */
        virtual bool loadDeviceDescription(
            OC::Cm::Serialization::ISerializable &serializable);

        /**
         * METHOD CURRENTLY NOT USED. PLEASE DO NOT USE.
         */
        virtual bool getDeviceDescription(
            OC::Cm::Serialization::ISerializable **serializable);

        /**
         * setAddress - Sets IP address of the self device.
         *
         * @param - address IP address of the device
         * @return - none
         */
        virtual void setAddress(const std::string &address);

        /**
         * getCapability - Returns a capability information written in the given Capability object.
         *
         * @param caps  Capability object to be written
         *
         * @return type:bool    True if capability information is successfully written to the Capability object, False if given device resource is not added
         */
        bool getCapability(Capability *caps);

        /**
         * VARIABLE CURRENTLY NOT USED. PLEASE DO NOT USE.
         */
        Capability m_capabilities;

        /**
         * getDevices - This is a sample implementation and can be used as a response to a GET request on /devices
         *
         * @param   devices         Devices object to be written
         *
         * @return  type:bool True if device information is successfully written to the Devices object, False if given device resource is not added
         */
        bool getDevices(::Devices *devices);

        /**
         * getDevice - This is a sample implementation and can be used as a response to a GET request on /devices
         *
         * Caution and Action required before using it as a response to GET request on /devices:
         *  This implementation needs to be modified with acutal values of the device, for example:
         *  actual link to 'information' resource, actual supported resources list, type of the device and etc.
         *
         * @param   m_device        Device object to be written
         *
         * @return  type:bool - True if device information is successfully written to the Device object, False if given device resource is not added
         */
        bool getDevice(::Device *m_device);

    private:
        std::list<ResourceType> m_resourceTypes; /**< List string variable that contains all the resources supported by the self device*/
        std::string m_subscriptionLink; /**< String variable that contains SubscriptionLink information.*/
        /**
         * MyDevice - Default constructor of MyDevice class.
         * @param -none
         * @return -none
         */
        MyDevice();

        /**
         *~MyDevice  Default virtual destructor of MyDevice class.
         * @param -none
         * @return -none
         */
        virtual ~MyDevice();

        /**
         * MyDevice class singleton instance variable.
         */
        static MyDevice *s_instance;
};

#endif /* MyDevice_H_ */
