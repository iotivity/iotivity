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



#if !defined(_IDEVICEFINDERSERVICE_H)
#define _IDEVICEFINDERSERVICE_H

#include "CMConfig.h"

#include "string"
#include "list"

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
                    class IDeviceFinderServiceListener;
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
                    class CMAPI IDeviceFinderService
                    {

                        public:

                            virtual ~IDeviceFinderService() {}

                            virtual void searchDevice(std::string &deviceType) = 0;

                            virtual void setDeviceListener(OC::Cm::Core::DeviceFinder::Service::IDeviceFinderServiceListener
                                                           &listener) = 0;

                            virtual bool start(int, std::string &type, std::string protocol, std::string &address,
                                               std::string &uuid,
                                               std::string &, std::string &, uint32_t maxAge) = 0;

                            virtual bool stop() = 0;

                            virtual bool bounce() = 0;              // Device Finder Service bouncer

                            virtual std::list<OC::Cm::Core::DeviceFinder::Service::DeviceFinderServiceDevice *> getDevices() =
                                0;
                    };
                }
            }
        }
    }
}

#endif  //_IDEVICEFINDERSERVICE_H
