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



#include "OCDevice.h"
#include "OCDeviceCommon.h"
#include "OCResource.h"
#include "IOCDeviceNotifier.h"
#include "OCDeviceManager.h"

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

                    OCDeviceManager::OCDeviceManager(IOCDeviceNotifier *notifier)
                    {
                        m_notifier = notifier;

                    }

                    OCDeviceManager::~OCDeviceManager()
                    {

                    }

                    void
                    OCDeviceManager::handleNewResource(OCResource resource)
                    {
                        OCDevice tmpOCDevice = addOCDevice(resource);
                        m_notifier->notifyAllListeners(OC_DEVICE_ADDED, tmpOCDevice);
                    }

                    void
                    OCDeviceManager::clearAllDevices()
                    {

                        for (std::list<OCDevice *>::iterator itr = m_deviceList.begin(); itr != m_deviceList.end(); itr++ )
                        {
                            OCDevice *device = (*itr) ;
                            if ( device != NULL )
                            {
                                delete device ;
                            }
                        }

                        m_deviceList.clear();
                    }

                    std::list<OCDevice *>
                    OCDeviceManager::getAllOCDevices()
                    {

                        return m_deviceList;
                    }

                    OCDevice
                    OCDeviceManager::addOCDevice(OCResource resource)
                    {
                        OCDevice *m_ocDevice = new OCDevice();

                        m_ocDevice->setLocation(resource.host() + resource.uri());
                        //m_ocDevice->setIPAddress(resource.host());
                        m_ocDevice->setDeviceType(OC::Cm::DeviceType::DeviceType_Unknown);
                        m_ocDevice->setDomain(OC::Cm::DeviceDomain::DeviceDomain_Local);
                        m_deviceList.push_back(m_ocDevice);

                        return *m_ocDevice;

                    }
                } // Impl
            } // DeviceFinder
        } // Core
    } // Cm
} // OC

