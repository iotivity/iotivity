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

#include <Device.h>
#include <DeviceType.h>
#include "ApplicationType.h"
#include "CMSerialization.h"

#include "string"
#include "list"
#include "xsd/Capability.h"
#include "xsd/Devices.h"
#include "xsd/Device.h"

/**
*  @addtogroup ControlManager
*  @{
*/

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
         * Returns a singleton instance of MyDevice.
         *
         * @return Singleton instance of MyDevice.
         */
        static MyDevice *getInstance();

        static void removeInstance();


        /**
         * Return all the resources supported by the self device.
         *
         * @return List of string that contains all the resources supported by the device.
         */
        std::list< ::ResourceType > &getSupportedResourceTypes();

        /**
         * To set a supported resource information, user should repeatedly call this method as many times as the number of resources that are supported by the device.
         *
         * @param[in] resource  Supported resource to be added in the m_resourceTypes private variable.
         *
         * @return  @c True if given device resource is successfully added @n
         *          @c False if given device resource is not added
         */
        bool setSupportedResourceType(std::string resource);

        /**
         * Returns a subscription link of the self device.
         *
         * @return Subscription link of the self device.
         */
        virtual std::string getSubscriptionsLink();

        /**
         * METHOD CURRENTLY NOT USED. PLEASE DO NOT USE.
         */
        virtual bool loadDeviceDescription(OC::Cm::Serialization::ISerializable &serializable);

        /**
         * Sets IP address of the self device.
         *
         * @param[in] address IP address of the device
         */
        virtual void setAddress(const std::string &address);

        /**
         * Returns a capability information written in the given Capability object.
         *
         * @param[in] caps  Capability object to be written
         *
         * @return  @c True if capability information is successfully written to the Capability object @n
         *          @c False if given device resource is not added
         */
        bool getCapability(::Capability *caps);

        /**
         * VARIABLE CURRENTLY NOT USED. PLEASE DO NOT USE.
         */
        ::Capability m_capabilities ;

        /**
         * This is a sample implementation and can be used as a response to a GET request on /devices
         *
         * @param[in]   devices         Devices object to be written
         *
         * @return  @c True if device information is successfully written to the Devices object  @n
         *          @c False if given device resource is not added
         */
        bool getDevices(::Devices *devices) ;

        /**
         * This is a sample implementation and can be used as a response to a GET request on /devices
         *
         * Caution and Action required before using it as a response to GET request on /devices:
         *  This implementation needs to be modified with acutal values of the device, for example:
         *  actual link to 'information' resource, actual supported resources list, type of the device and etc.
         *
         * @param[in]   m_device        Device object to be written
         *
         * @return  @c True if device information is successfully written to the Device object  @n
         *          @c False if given device resource is not added
         */
        bool getDevice(::Device *m_device) ;

    private:

        /**
         * List string variable that contains all the resources supported by the self device.
         */
        std::list< ::ResourceType > m_resourceTypes;

        /**
         * String variable that contains SubscriptionLink information.
         */
        std::string m_subscriptionLink;

        /**
         * Default constructor of MyDevice class.
         */
        MyDevice();

        /**
         * Default virtual destructor of MyDevice class.
         */
        virtual ~MyDevice();

        /**
         * MyDevice class singleton instance variable.
         */
        static MyDevice *s_instance ;
};
/** @} */
#endif /* MyDevice_H_ */
