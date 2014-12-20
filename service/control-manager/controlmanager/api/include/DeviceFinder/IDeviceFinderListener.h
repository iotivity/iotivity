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



#if !defined(_IDEVICEFINDERLISTENER_H)
#define _IDEVICEFINDERLISTENER_H

#include "CMConfig.h"
#include "Device.h"

//Forward declarations
namespace OC
{
    namespace Cm
    {
        class Device;
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
             *
             * @interface   IDeviceFinderListener
             * @brief       listener to receive device discovery related events
             *
             * @since       1.0
             *
             * This interface defines methods to receive device discovery related events
             *
             * To receive device discovery related events, Application must register a listener
             * using OC::Cm::DeviceFinder::IDeviceFinder::setDeviceFinderListener()
             *
             */
            class CMAPI IDeviceFinderListener
            {
                public:

                    /**
                     * Default virtual destructor of this class
                     */
                    virtual ~IDeviceFinderListener() {}

                    /**
                     * This method will be invoked when a new device is discovered
                     *
                     * @param[in] device device object
                     */
                    virtual void OnDeviceAdded( OC::Cm::Device &device ) = 0;

                    /**
                     * This method will be invoked when a device is leaves the network
                     *
                     * @param[in] device device object
                     */
                    virtual void OnDeviceRemoved( OC::Cm::Device &device) = 0;

                    /**
                     * This method will be invoked when a device updates its capabilities or services
                     *
                     * @param[in] device device object
                     */
                    virtual void OnDeviceUpdated( OC::Cm::Device &device) = 0;

                    /**
                     * This method will be invoked when a shp device is discovered and its validation
                     * is failed (Error in capability resource processing).
                     *
                     * @param[in] device device object
                     */
                    virtual void OnDeviceError( OC::Cm::Device &device ) = 0;
            };
            /** @} */
        }
    }
}

#endif  //_IDEVICEFINDERLISTENER_H
