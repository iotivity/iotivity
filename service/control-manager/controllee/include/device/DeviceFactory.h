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

#if !defined(_DEVICEFACTORY_H)
#define _DEVICEFACTORY_H

#include "DeviceType.h"
#include "DeviceDomain.h"

//Forward declarations
namespace OC
{
    namespace Cm
    {
        class Device;

        namespace Serialization
        {
            class ISerializable ;
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
             * @interface   DeviceFactory
             * @brief       Defines methods required to instantiate a Device object
             *
             * This interface defines methods required to instantiate a Device Object.
             *
             * SHP-SDK generates a Device class which extends OC::Cm::Device class and
             * provides methods to access resources supported by the device.
             * SHP framework uses DeviceFactory to instantiate this generated Device class
             *
             * SHP-SDK also generates a DeviceFactory class which can instantiate the generated Device class
             * object. Developers must configure an instance of this generated DeviceFactory with framework
             * using OC::Cm::Configuration::setDeviceFactory()
             *
             *
             */
            class DeviceFactory
            {

                public:

                    /**
                     * ~DeviceFactory - Default virtual destructor of this class
                     */
                    virtual ~DeviceFactory() {};

                    /**
                     * createDevice - instantiates the Device object
                     *
                     * @param type      type of the device
                     * @param domain    domain to which this device belongs
                     * @return -type:Device - returns instance of OC::Cm::Device class
                     */
                    virtual Device *createDevice( OC::Cm::DeviceType type,
                                                  OC::Cm::DeviceDomain domain) = 0;
            };
        }
    }
}

#endif  //_DEVICEFACTORY_H
