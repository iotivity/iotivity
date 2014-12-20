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


#if !defined (__OC_DEVICE_FINDER__H)
#define __OC_DEVICE_FINDER__H

#include "../Service/DeviceFinderServiceDevice.h"
#include "../Service/IDeviceFinderServiceListener.h"
#include "OCResource.h"
#include "OCPlatform.h"
#include "../Service/IDeviceFinderService.h"
#include "IOCDeviceNotifier.h"
#include "OCPlatform.h"

namespace OC
{
    namespace Cm
    {
        namespace Core
        {
            namespace DeviceFinder
            {
                namespace Impl
                {

                    class OCDeviceFinder : public OC::Cm::Core::DeviceFinder::Service::IDeviceFinderService,
                        IOCDeviceNotifier
                    {

                        public:
                            OCDeviceFinder();
                            virtual ~OCDeviceFinder(); // Destructor

                            // IDeviceFinderServiceDevice Methods
                            bool start(int, std::string &type, std::string protocol, std::string &address, std::string &uuid,
                                       std::string &, std::string &, uint32_t maxAge);
                            void setDeviceListener(OC::Cm::Core::DeviceFinder::Service::IDeviceFinderServiceListener
                                                   &aListener);
                            bool stop();
                            bool bounce();
                            std::list<OC::Cm::Core::DeviceFinder::Service::DeviceFinderServiceDevice *> getDevices();
                            void searchDevice(std::string &deviceType);

                            // IOCDeviceNotifier Methods
                            void notifyAllListeners(OC_DEVICE_NOTIFICATION_TYPE , OCDevice &);


                        private:
                            OCStackResult cmFindResource(const std::string &host, const std::string &resourceName);
                            bool findRootResource();
                            static void myResourceHandler(std::shared_ptr<OCResource> resource);
                            void buildDiscoveryTargetURI(std::string protocol);

                            OC::Cm::Core::DeviceFinder::Service::IDeviceFinderServiceListener *myListener;


                    };

                } // Impl
            } // DeviceFinder
        } // Core
    } // Cm
} // OC

#endif // __OC_DEVICE_FINDER__H
