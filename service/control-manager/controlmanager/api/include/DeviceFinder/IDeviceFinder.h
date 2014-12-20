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



#if !defined(_IDEVICEFINDER_H)
#define _IDEVICEFINDER_H

#include "CMConfig.h"
#include "DeviceType.h"
#include "DeviceDomain.h"
#include "list"
#include "string"

//Forward declarations
namespace OC
{
    namespace Cm
    {
        class Device;

        namespace DeviceFinder
        {
            class IDeviceFinderListener ;
        }

        namespace Client
        {
            namespace Resource
            {
                class IResourceResponseListener ;
            }
        }
    }
}

namespace OC
{
    namespace Cm
    {
        namespace DeviceFinder
        {
            /**
            *  @addtogroup DeviceFinder
            *  @{
            */
            /**
             * @interface   IDeviceFinder
             * @brief       provides methods to discover devices in network
             *
             * @since       1.0
             *
             * This interface provides methods to discover devices.
             *
             * Instance of this interface can be obtained through OC::Cm::ControlManager::getDeviceFinder()
             *
             */
            class CMAPI IDeviceFinder
            {

                public:

                    /**
                     * virtual destructor of this class
                     *
                     */
                    virtual ~IDeviceFinder() {};

                    /**
                     * refreshes the list of devices. rescans the network to discover the devices
                     * @return  returns @c true in case of success @n
                     *                  @c false in case of any error
                     */
                    virtual bool refresh() = 0;

                    /**
                     * Registers a listener to receive device discovery related events
                     *
                     * @param[in] listener listener object
                     *
                     * @return Return @c true in case of success @n@c false in case of any failure
                     */
                    virtual bool setDeviceFinderListener(OC::Cm::DeviceFinder::IDeviceFinderListener  &listener) = 0;

                    /**
                     * removes previously configured listener
                     *
                     * @param[in] listener listener object
                     */
                    virtual void removeDeviceFinderListener(OC::Cm::DeviceFinder::IDeviceFinderListener  &listener) = 0;

                    /**
                     * Registers a listener to receive device discovery related events of certain device type
                     *
                     * @param[in] deviceType type of the device
                     * @param[in] listener listener object
                     *
                     * @return Return @c true in case of success @n@c false in case of any failure
                     */
                    virtual bool setDeviceFinderListener( OC::Cm::DeviceType deviceType,
                                                          OC::Cm::DeviceFinder::IDeviceFinderListener  &listener) = 0;

                    /**
                     * removes previously configured listener
                     *
                     * @param[in] deviceType type of the device
                     * @param[in] listener listener object
                     */
                    virtual void removeDeviceFinderListener( OC::Cm::DeviceType deviceType,
                            OC::Cm::DeviceFinder::IDeviceFinderListener  &listener) = 0;

                    /**
                     * Registers a listener to receive device discovery related events of certain device type and domain
                     *
                     * @param[in] deviceDomain device domain
                     * @param[in] deviceType type of the device
                     * @param[in] listener listener object
                     *
                     * @return Return @c true in case of success @n@c false in case of any failure
                     */
                    virtual bool setDeviceFinderListener( OC::Cm::DeviceDomain deviceDomain,
                                                          OC::Cm::DeviceType deviceType,
                                                          OC::Cm::DeviceFinder::IDeviceFinderListener  &listener) = 0;

                    /**
                     * removes previously configured listener
                     *
                     * @param[in] deviceDomain device domain
                     * @param[in] deviceType type of the device
                     * @param[in] listener listener object
                     */
                    virtual void removeDeviceFinderListener( OC::Cm::DeviceDomain deviceDomain,
                            OC::Cm::DeviceType deviceType,
                            OC::Cm::DeviceFinder::IDeviceFinderListener  &listener) = 0;

                    /**
                     * Returns a list of discovered devices
                     *
                     * @return list of devices
                     */
                    virtual std::list<Device *>  getDevices() = 0;

                    /**
                     * Returns list of discovered devices of specified type
                     *
                     * @param[in] deviceType type of device
                     *
                     * @return list of devices
                     */
                    virtual std::list<Device *>  getDevices(OC::Cm::DeviceType deviceType) = 0;

                    /**
                     *
                     * Returns list of discovered devices of specified type and within specified domain
                     *
                     * @param[in] domain    domain of the device
                     * @param[in] deviceType type of the device
                     * @return list of devices
                     */
                    virtual std::list<Device *>  getDevices(OC::Cm::DeviceDomain domain,
                                                            OC::Cm::DeviceType deviceType) = 0;


                    /**
                     *
                     * Returns device object which has specified address
                     *
                     * @param[in] address   address of the device
                     *
                     * @return instance of OC::Cm::Device class which has specified address
                     */
                    virtual Device *getDeviceByAddress( std::string &address ) = 0 ;

                    /**
                     *
                     * Returns device object which has specified uuid
                     *
                     * @param[in] uuid  uuid of the device
                     *
                     * @return instance of OC::Cm::Device class which has specified uuid
                     */
                    virtual Device *getDeviceByUUID( std::string &uuid ) = 0 ;

                    /**
                     *
                     * Adds a device to device finder.
                     *
                     * @param[in] type  type of the device
                     * @param[in] uuid  uuid of the device
                     * @param[in] address   address of the device
                     * @param[in] listener  response listener for requests sent by the framework to validate the device
                     *
                     * @return  returns @c true in case of success @n
                     *                  @c false in case of any error
                     */
                    virtual bool addDevice(OC::Cm::DeviceType type, std::string uuid, std::string address,
                                           OC::Cm::Client::Resource::IResourceResponseListener &listener) = 0 ;
            };
            /** @} */
        }
    }
}

#endif  //_IDEVICEFINDER_H
