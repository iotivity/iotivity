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



#ifndef OCDEVICEMANAGER_H_
#define OCDEVICEMANAGER_H_

#include "OCDevice.h"
#include "IOCDeviceNotifier.h"
#include "OCResource.h"
#include <list>

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

                    class OCDeviceManager
                    {

                        public:
                            OCDeviceManager(IOCDeviceNotifier *notifier);
                            virtual ~OCDeviceManager();
                            void handleNewResource(OCResource resource);
                            std::list<OCDevice *> getAllOCDevices();
                            void clearAllDevices();

                        protected:
                            IOCDeviceNotifier *m_notifier ;

                        private:
                            OCDevice addOCDevice(OCResource resource);
                            std::list<OCDevice *> m_deviceList;
                    };


                } // Impl
            } // DeviceFinder
        } // Core
    } // Cm
} // OC

#endif /* OCDEVICEMANAGER_H_ */
