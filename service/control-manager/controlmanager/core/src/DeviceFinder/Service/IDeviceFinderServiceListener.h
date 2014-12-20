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



#if !defined(_IDEVICEFINDERSERVICELISTENER_H)
#define _IDEVICEFINDERSERVICELISTENER_H

#include "CMConfig.h"

namespace OC
{
    namespace Cm
    {
        namespace Core
        {
            namespace DeviceFinder
            {
                namespace Service
                {
                    class DeviceFinderServiceDevice;
                }
            }
        }
    }
}

namespace OC
{
    namespace Cm
    {
        namespace Core
        {
            namespace DeviceFinder
            {
                namespace Service
                {
                    class CMAPI IDeviceFinderServiceListener
                    {

                        public:

                            virtual ~IDeviceFinderServiceListener() {}

                            virtual void OnDeviceAdded(OC::Cm::Core::DeviceFinder::Service::DeviceFinderServiceDevice &device) =
                                0;

                            virtual void OnDeviceRemoved(OC::Cm::Core::DeviceFinder::Service::DeviceFinderServiceDevice &device)
                                = 0;

                            virtual void OnDeviceUpdated(OC::Cm::Core::DeviceFinder::Service::DeviceFinderServiceDevice &device)
                                = 0;

                            virtual void OnServiceError() = 0;

                    };
                }
            }
        }
    }
}

#endif  //_IDEVICEFINDERSERVICELISTENER_H
