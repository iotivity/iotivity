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

#if !defined(_ControlDeviceFACTORY_H)
#define _ControlDeviceFACTORY_H

#include "Device.h"
#include "DeviceType.h"
#include "DeviceDomain.h"
#include "DeviceFactory.h"

/**
 *
 * @class   ControlDeviceFactory
 * @brief   Defines methods required to instantiate a Device object
 *
 * This class defines methods required to instantiate a Device Object.@n
 * @n
 * SHP-SDK generates a Device class which extends OC::Cm::Device class and@n
 * provides methods to access resources supported by the device.@n
 * SHP framework uses ControlDeviceFactory to instantiate this generated Device class@n
 * @n
 * SHP-SDK also generates a DeviceFactory class which can instantiate the generated Device class@n
 * object. Developers must configure an instance of this generated DeviceFactory with framework@n
 * using OC::Cm::Configuration::setDeviceFactory()
 */
class ControlDeviceFactory: public OC::Cm::DeviceFinder::DeviceFactory
{

    public:

        /**
         *ControlDeviceFactory -  Default constructor of ControlDeviceFactory class.
         */
        ControlDeviceFactory();

        /**
         *~ControlDeviceFactory -  Default virtual destructor of ControlDeviceFactory class.
         */
        ~ControlDeviceFactory();

        /**
         * createDevice - Instantiates the Device object
         *
         * @param type  Type of device (Air Conditioner, Washer, Dryer, etc.)
         * @param domain    Device domain to which this device belongs (Local, Remote, All)
         *
         * @return Instance of OC::Cm::Device class
         */
        OC::Cm::Device *createDevice(OC::Cm::DeviceType type,
                                     OC::Cm::DeviceDomain domain);

};

#endif //_ControlDeviceFACTORY_H
