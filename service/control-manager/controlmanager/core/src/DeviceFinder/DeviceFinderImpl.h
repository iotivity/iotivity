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



#if !defined(_DEVICEFINDERIMPL_H)
#define _DEVICEFINDERIMPL_H

#include "IContext.h"
#include "CMDeviceFinder.h"
#include "DeviceFactory.h"
#include "CMSerialization.h"

#include "Service/IDeviceFinderServiceListener.h"
#include "Service/IDeviceFinderService.h"
#include "CMClientResource.h"

#include "ApplicationType.h"
#include "DeviceType.h"
#include "DeviceDomain.h"
#include "Device.h"

#include "map"

#include "Thread/Mutex.h"

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
                    class DeviceFinderImpl : public OC::Cm::DeviceFinder::IDeviceFinder,
                        public OC::Cm::Core::DeviceFinder::Service::IDeviceFinderServiceListener,
                        public OC::Cm::Client::Resource::IResourceResponseListener
                    {
                        public:

                            DeviceFinderImpl(OC::Cm::IContext *context);

                            virtual ~DeviceFinderImpl();

                            bool init(OC::Cm::DeviceFinder::DeviceFactory *factory);

                            bool deinit();

                            virtual bool refresh();

                            virtual void removeDeviceFinderListener(OC::Cm::DeviceFinder::IDeviceFinderListener  &listener) ;

                            virtual bool setDeviceFinderListener(OC::Cm::DeviceFinder::IDeviceFinderListener  &listener);

                            virtual bool setDeviceFinderListener(OC::Cm::DeviceType deviceType,
                                                                 OC::Cm::DeviceFinder::IDeviceFinderListener  &listener);

                            virtual void removeDeviceFinderListener( OC::Cm::DeviceType deviceType,
                                    OC::Cm::DeviceFinder::IDeviceFinderListener  &listener) ;

                            virtual bool setDeviceFinderListener( OC::Cm::DeviceDomain deviceDomain,
                                                                  OC::Cm::DeviceType deviceType,
                                                                  OC::Cm::DeviceFinder::IDeviceFinderListener  &listener);

                            virtual void removeDeviceFinderListener( OC::Cm::DeviceDomain deviceDomain,
                                    OC::Cm::DeviceType deviceType,
                                    OC::Cm::DeviceFinder::IDeviceFinderListener  &listener) ;

                            virtual std::list<Device *>  getDevices();

                            virtual std::list<Device *>  getDevices(OC::Cm::DeviceType deviceType);

                            virtual std::list<Device *>  getDevices(OC::Cm::DeviceDomain domain, OC::Cm::DeviceType deviceType);


                            bool start(OC::Cm::ApplicationType appType, OC::Cm::DeviceType deviceType, std::string protocol,
                                       std::string &address, std::string &uuid, std::string appName, std::string osTag);

                            bool stop();

                            virtual void OnDeviceAdded(OC::Cm::Core::DeviceFinder::Service::DeviceFinderServiceDevice &device);

                            virtual void OnDeviceRemoved(OC::Cm::Core::DeviceFinder::Service::DeviceFinderServiceDevice
                                                         &device);

                            virtual void OnDeviceUpdated(OC::Cm::Core::DeviceFinder::Service::DeviceFinderServiceDevice
                                                         &device);

                            virtual void OnServiceError();


                            void OnGetResponseReceived(int clientSessionId, int status, OC::Cm::Connector::HeadersList &headers,
                                                       OC::Cm::Serialization::ISerializable *response) ;
                            void OnPutResponseReceived(int clientSessionId, int status, OC::Cm::Connector::HeadersList &headers,
                                                       OC::Cm::Serialization::ISerializable *response) ;
                            void OnPostResponseReceived(int clientSessionId, int status,
                                                        OC::Cm::Connector::HeadersList &headers, OC::Cm::Serialization::ISerializable *response) ;
                            void onDeleteResponseReceived(int clientSessionId, int status,
                                                          OC::Cm::Connector::HeadersList &headers,  OC::Cm::Serialization::ISerializable *response);
                            void onSubscribeResponseReceived(int clientSessionId, int status, std::string &subscriptionUri);
                            void onUnSubscribeResponseReceived(int clientSessionId, int status);
                            void onNotifyResponseReceived(int clientSessionId, int status) ;
                            void onErrorReceived( int requestId, int status, OC::Cm::CMError error,
                                                  OC::Cm::Serialization::ISerializable *errorMessage );
                            void onRequestComplete(int requestId, std::string requestPayload,  std::string responsePayload);

                            OC::Cm::Device *getDeviceByAddress(std::string &deviceAddress);

                            OC::Cm::Device  *getDeviceByUUID( std::string &uuid ) ;

                            virtual bool addDevice(OC::Cm::DeviceType type, std::string uuid, std::string address,
                                                   OC::Cm::Client::Resource::IResourceResponseListener &listener) ;

                            void setIsEasySetupThroughDeviceDiscovery(bool flag);

                            uint32_t setMaxAge();       // Setter (based on Control Manager Mode) of Device Finder Services' Max-AGE value - Specific to SSDP


                        private:

                            OC::Cm::Core::DeviceFinder::Service::IDeviceFinderService *m_pDeviceFinderService;

                            std::map< std::string, OC::Cm::Device *> m_devicesMap;

//                      std::map< std::string, OC::Cm::Device *> m_errorDevicesMap;

                            std::map<int, OC::Cm::Device *> m_pPendingDevicesMap;

                            std::map<int, OC::Cm::Client::Resource::IResourceResponseListener *> m_pAddDeviceListener;

                            std::map<std::pair < OC::Cm::DeviceDomain, OC::Cm::DeviceType >, OC::Cm::DeviceFinder::IDeviceFinderListener *>
                            m_ListenerMap;

                            OC::Cm::DeviceFinder::DeviceFactory *m_pDeviceFactory ;

                            OC::Cm::Platform::Thread::Mutex     m_deviceListMutex ;

                            bool    isStarted ;

                            bool m_isEasySetupThroughDeviceDiscovery;  /**< Represents whether easy setup process is happening through device discovery or not; default is true */

                            Device *createDevice(OC::Cm::DeviceType type, OC::Cm::DeviceDomain domain);

                            OC::Cm::IContext *p_context ;

                            void createDevicefromServiceDevice(OC::Cm::Core::DeviceFinder::Service::DeviceFinderServiceDevice
                                                               &device);
                            void updateDevicefromServiceDevice(OC::Cm::Core::DeviceFinder::Service::DeviceFinderServiceDevice
                                                               &device, OC::Cm::Device *ControlDevice);

                            /**
                             * type 0 -> Added
                             * type 1 -> Removed
                             */
                            void notifyDevice(int type, OC::Cm::Device *pDevice) ;

                            void notifyDevice( int type, OC::Cm::Device *pDevice,
                                               OC::Cm::DeviceFinder::IDeviceFinderListener *pListener) ;

                            OC::Cm::Device *getPendingDevice( int requestId ) ;

                            void addPendingDevice( int requestId, OC::Cm::Device *pDevice ) ;

                            void removePendingDevice( int requestId ) ;

                            OC::Cm::Device *getPendingDevice( std::string &uuid ) ;

                            void addDevice( OC::Cm::Device *pDevice ) ;

                            void removeDevice( OC::Cm::Device *pDevice ) ;

                            void clearDevices();

                            void addAddDeviceListener( int requestId,
                                                       OC::Cm::Client::Resource::IResourceResponseListener *pListsener ) ;

                            void removeAddDeviceListener( int requestId ) ;

                            OC::Cm::Client::Resource::IResourceResponseListener *getAddDeviceListener( int requestId ) ;

                    };
                }
            }
        }
    }
}

#endif  //_DEVICEFINDERIMPL_H
