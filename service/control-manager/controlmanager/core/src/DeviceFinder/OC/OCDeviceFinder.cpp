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



#include "../Service/DeviceFinderServiceDevice.h"
#include "OCDeviceFinder.h"
#include "OCDeviceManager.h"
#include "DeviceDomain.h"
#include "OCPlatform.h"
#include "OCApi.h"
#include "OCResource.h"
#include "Log.h"

#define MODULE_ID CM_CORE_DEVICE_FINDER_OC

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

                    OCDeviceManager *m_ocDeviceManager = NULL;
                    std::string m_protocol, m_targetURI;

                    const int SUCCESS_RESPONSE = 0;
                    std::string COAP_MULTICAST_ADDRESS = "//224.0.1.187";
                    std::string OC_BASE_REOURCE_LOCATION = "/oc/core";
                    std::string ROOT_RESOURCE = "capability";


                    OCDeviceFinder::OCDeviceFinder()
                    {
                        //myOCPlatform = platform;
                    }

                    OCDeviceFinder::~OCDeviceFinder()
                    {

                    }

                    void
                    OCDeviceFinder::myResourceHandler(std::shared_ptr<OCResource> resource)
                    {

                        // TO DO
                        //

                        CM_LOG_DEBUG_A(MODULE_ID, "%s", "MyResourceHandler : Discovery Response");

                        if (resource)
                        {
                            // Take the resource object; build a serviceDevice and push to the pending stack
                            //
                            //
                            m_ocDeviceManager->handleNewResource(*resource);

                        }
                        else
                        {
                            CM_LOG_ERROR_A(MODULE_ID, "%s", "OCResource is null in OCDeviceFinder::myResourceHandler");
                        }

                    }

                    bool
                    OCDeviceFinder::findRootResource()
                    {
                        OCStackResult result = cmFindResource("", ROOT_RESOURCE);

                        if (OC_STACK_OK == result)
                        {
                            CM_LOG_DEBUG_A(MODULE_ID, "Resource Discovery Initiated for [%s]", ROOT_RESOURCE.c_str());
                            return true;
                        }

                        CM_LOG_ERROR_A(MODULE_ID, "Resource Discovery failed [%s]", ROOT_RESOURCE.c_str());
                        return false;
                    }

                    OCStackResult
                    OCDeviceFinder::cmFindResource(const std::string &host, const std::string &resourceName)
                    {
                        std::string resourceToFind = m_targetURI + resourceName;
                        CM_LOG_DEBUG_A(MODULE_ID, "findResource host[%s] resourceToFind[%s]", host.c_str(),
                                       resourceToFind.c_str());
                        return OC::OCPlatform::findResource(host, resourceToFind, &myResourceHandler);
                    }

                    void
                    OCDeviceFinder::notifyAllListeners(OC_DEVICE_NOTIFICATION_TYPE type, OCDevice &device)
                    {

                        switch (type)
                        {
                            case OC_DEVICE_ADDED:
                                myListener->OnDeviceAdded(device);
                                break;

                            case OC_DEVICE_DELETED:
                            case OC_DEVICE_ERROR:
                                break;
                        }
                    }

                    void
                    OCDeviceFinder::setDeviceListener(
                        OC::Cm::Core::DeviceFinder::Service::IDeviceFinderServiceListener &aListener)
                    {

                        myListener = &aListener;
                    }

                    std::list<OC::Cm::Core::DeviceFinder::Service::DeviceFinderServiceDevice *>
                    OCDeviceFinder::getDevices()
                    {

                        std::list<OC::Cm::Core::DeviceFinder::Service::DeviceFinderServiceDevice *> shpDevList;
                        std::list<OCDevice *> ocDevList = m_ocDeviceManager->getAllOCDevices();

                        for (std::list<OCDevice *>::const_iterator ocDevItr = ocDevList.begin();
                             ocDevItr != ocDevList.end(); ++ocDevItr)
                        {

                            shpDevList.push_back(*ocDevItr);
                        }

                        return shpDevList;
                    }

                    void
                    OCDeviceFinder::buildDiscoveryTargetURI(std::string protocol)
                    {
                        m_protocol = protocol;
                        m_targetURI = protocol + std::string(":") + COAP_MULTICAST_ADDRESS + OC_BASE_REOURCE_LOCATION +
                                      std::string("?rt=");
                    }

                    bool
                    OCDeviceFinder::start(int, std::string &type, std::string protocol, std::string &address,
                                          std::string &uuid,
                                          std::string &, std::string &, uint32_t maxAge)
                    {

                        // Persist our protocol string and build our resource name string for discovery; for now we know the protocol is "coap" but other discovery protocols may be supported in
                        // future releases
                        buildDiscoveryTargetURI(protocol);

                        m_ocDeviceManager = new OCDeviceManager(this);

                        // Invoke local method to find specified resource based on passed in values
                        //
                        return findRootResource();

                        // TODO
                        // Subscribe to Presence indications for active discovery; need to be able to add a /ResURI to this as we only
                        // care about presence for specified resources
                        // We also need a presence handler
                        //
                        // e.g. myOCPlatform->subscribePresence();
                    }

                    bool
                    OCDeviceFinder::stop()
                    {
                        return true;
                    }

                    bool
                    OCDeviceFinder::bounce()
                    {
                        return true;
                    }

                    void
                    OCDeviceFinder::searchDevice(std::string &deviceType)
                    {

                        // No ability to as yet search for specific device types; awaiting DI level search from Intel
                        // Complete refresh of the device list
                        //
                        if (deviceType.empty())
                        {
                            m_ocDeviceManager->clearAllDevices();
                        }
                        else
                        {
                            // TODO
                            // Handle DI based search
                        }

                        // Reinitialize search for our root resource; this invokes a COAP MCAST by the core
                        //
                        if (findRootResource())
                        {
                            CM_LOG_DEBUG_A(MODULE_ID, "%s", "Resource Discovery Re-Initiated");
                        }
                        else
                        {
                            CM_LOG_ERROR_A(MODULE_ID, "%s", "Resource Discovery Re-initiation Failed");
                        }
                    }


                } // Impl
            } // DeviceFinder
        } // Core
    } // Cm
} // OC
