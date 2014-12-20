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



#include "DeviceFinderImpl.h"
#include "DeviceType.h"
#include "DeviceDomain.h"
#include "Device.h"
#include "string"
#include "Utils.h"

#include "CMDeviceFinder.h"
#include "OC/OCDeviceFinder.h"

#include "Log.h"
#include "CMContext.h"
#include "CMCommon.h"

#include "DeviceFinder/Service/IDeviceFinderService.h"
#include "DeviceFinder/Service/DeviceFinderServiceDevice.h"
#include "DeviceFinder/Service/IDeviceFinderServiceListener.h"

#define MODULE_ID CM_CORE_DEVICE_FINDER

#define DEVICE_ADDED_NORMAL 0
#define DEVICE_REMOVED      1
#define DEVICE_UPDATED      2
#define DEVICE_ADDED_ERROR  3
#define SERVICE_ERROR       4

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

                    DeviceFinderImpl::DeviceFinderImpl(OC::Cm::IContext *context)
                    {
                        m_pDeviceFactory = NULL ;

                        m_pDeviceFinderService = NULL ;

                        p_context  = context ;

                        isStarted = false ;

                        m_isEasySetupThroughDeviceDiscovery = true;
                    }

                    DeviceFinderImpl::~DeviceFinderImpl()
                    {
                        this->deinit();
                    }

                    bool
                    DeviceFinderImpl::init(OC::Cm::DeviceFinder::DeviceFactory *pDeviceFactory)
                    {
                        m_pDeviceFactory = pDeviceFactory;

                        // Get handle to cmEngine and ocPlatform that can be passed to the Device Finder Service
                        //
                        OC::Cm::Core::CMContext *p_cm_context = (OC::Cm::Core::CMContext *)p_context;
                        m_pDeviceFinderService = new OC::Cm::Core::DeviceFinder::Impl::OCDeviceFinder();
                        TryReturn( m_pDeviceFinderService != NULL, false, "Failed to Create OC DeviceFinder Service");

                        m_pDeviceFinderService->setDeviceListener(*this) ;

                        return true ;
                    }

                    bool
                    DeviceFinderImpl::deinit()
                    {
                        m_pDeviceFactory = NULL ;

                        m_ListenerMap.clear();

                        clearDevices();

                        if ( m_pDeviceFinderService != NULL )
                        {
                            delete m_pDeviceFinderService ;
                            m_pDeviceFinderService = NULL ;
                        }

                        return true;
                    }

                    bool
                    DeviceFinderImpl::refresh()
                    {
                        OC::Cm::Core::CMContext *pContext = (OC::Cm::Core::CMContext *)p_context ;
                        TryReturn(pContext != NULL, false, "Failed to get Control Manager Context Class Instance");

                        if (pContext->m_engine.getCMEngineState() != CM_STARTED)
                        {
                            CM_LOG_DEBUG_A(CM_REMOTE_DEVICE_FINDER, "%s", "Control Manager is not started yet.");
                            return false;
                        }

                        TryReturn( m_pDeviceFinderService != NULL, false, "DeviceFinder Service is NULL");
                        TryReturn( isStarted == true, false, "DeviceFinder Not yet started");

                        clearDevices();

                        std::string deviceType ;
                        m_pDeviceFinderService->searchDevice( deviceType ) ;

                        return true ;
                    }

                    void
                    DeviceFinderImpl::removeDeviceFinderListener(OC::Cm::DeviceFinder::IDeviceFinderListener  &listener)
                    {
                        //m_pListener = NULL ;
                        m_ListenerMap.erase(std::make_pair(DeviceDomain_All, DeviceType_All)) ;
                    }

                    bool
                    DeviceFinderImpl::setDeviceFinderListener(OC::Cm::DeviceFinder::IDeviceFinderListener  &listener)
                    {
                        if ((&listener) == NULL) { return false; }
                        m_ListenerMap[ std::make_pair(DeviceDomain_All, DeviceType_All) ] = &listener ;
                        return true;
                    }

                    bool
                    DeviceFinderImpl::setDeviceFinderListener( OC::Cm::DeviceType deviceType,
                            OC::Cm::DeviceFinder::IDeviceFinderListener  &listener)
                    {
                        if ((&listener) == NULL) { return false; }
                        m_ListenerMap[ std::make_pair(DeviceDomain_All, deviceType) ] = &listener ;
                        return true;
                    }

                    void
                    DeviceFinderImpl::removeDeviceFinderListener( OC::Cm::DeviceType deviceType,
                            OC::Cm::DeviceFinder::IDeviceFinderListener  &listener)
                    {
                        m_ListenerMap.erase(std::make_pair(DeviceDomain_All, deviceType)) ;
                    }


                    bool
                    DeviceFinderImpl::setDeviceFinderListener( OC::Cm::DeviceDomain deviceDomain,
                            OC::Cm::DeviceType deviceType,
                            OC::Cm::DeviceFinder::IDeviceFinderListener  &listener)
                    {
                        if ((&listener) == NULL) { return false; }
                        m_ListenerMap[ std::make_pair(deviceDomain, deviceType) ] = &listener ;
                        return true;
                    }


                    void
                    DeviceFinderImpl::removeDeviceFinderListener( OC::Cm::DeviceDomain deviceDomain,
                            OC::Cm::DeviceType deviceType,
                            OC::Cm::DeviceFinder::IDeviceFinderListener  &listener)
                    {
                        m_ListenerMap.erase(std::make_pair(deviceDomain, deviceType)) ;
                    }


                    std::list<Device *>
                    DeviceFinderImpl::getDevices()
                    {
                        std::list<Device *> retList ;

                        m_deviceListMutex.lock() ;

                        for ( std::map< std::string, OC::Cm::Device * >::const_iterator itr = m_devicesMap.begin();
                              itr != m_devicesMap.end(); itr++ )
                        {
                            retList.push_back(itr->second) ;
                        }

                        m_deviceListMutex.unlock() ;

                        return retList ;
                    }

                    std::list<Device *>
                    DeviceFinderImpl::getDevices(OC::Cm::DeviceType deviceType)
                    {
                        if (DeviceType_All == deviceType)
                        {
                            return getDevices();
                        }

                        std::list<Device *> retList ;

                        m_deviceListMutex.lock() ;

                        for ( std::map< std::string, OC::Cm::Device * >::const_iterator itr = m_devicesMap.begin();
                              itr != m_devicesMap.end(); itr++ )
                        {
                            if (NULL != itr->second)
                            {
                                if ( itr->second->getDeviceType() == deviceType )
                                {
                                    retList.push_back((itr->second));
                                }
                            }
                        }

                        m_deviceListMutex.unlock() ;

                        return retList ;
                    }

                    std::list<Device *>
                    DeviceFinderImpl::getDevices(OC::Cm::DeviceDomain domain, OC::Cm::DeviceType deviceType)
                    {
                        std::list<Device *> retList ;

                        m_deviceListMutex.lock() ;

                        for ( std::map< std::string, OC::Cm::Device * >::const_iterator itr = m_devicesMap.begin();
                              itr != m_devicesMap.end(); itr++ )
                        {
                            if (NULL != itr->second)
                            {
                                if (( itr->second->getDeviceType() == deviceType ) && ( itr->second->getDomain() == domain ))
                                {
                                    retList.push_back((itr->second));
                                }
                            }
                        }

                        m_deviceListMutex.unlock() ;

                        return retList ;
                    }

                    bool
                    DeviceFinderImpl::start(OC::Cm::ApplicationType appType, OC::Cm::DeviceType deviceType,
                                            std::string protocol, std::string &address, std::string &uuid, std::string appName,
                                            std::string osTag)
                    {
                        TryReturn( m_pDeviceFinderService != NULL, false, "DeviceFinder Service in NULL: Not initialized");

                        std::string deviecTypeStr = "Unknown" ;

                        if ((deviceType >= DeviceType_Unknown) || (deviceType < DeviceType_All))
                        {
                            deviecTypeStr = DeviceType_Strings[deviceType];
                        }

                        if (appName.empty())
                        {
                            appName = "TestAppName/1.5";
                        }

                        CM_LOG_INFO_A(CM_CORE_DEVICE_FINDER, "Self Device Address [%s]", address.c_str());

                        bool bRet;
                        bRet = m_pDeviceFinderService->start( (int)appType, deviecTypeStr, protocol, address, uuid, appName,
                                                              osTag, setMaxAge()) ;
                        if (bRet == false)
                        {
                            CM_LOG_ERROR_A(CM_CORE_DEVICE_FINDER, "ERROR: Failed to start DEVICE FINDER SERVICE - [%s]",
                                           "SSDP");
                            return false ;
                        }
                        else
                        {
                            CM_LOG_DEBUG_A(CM_CORE_DEVICE_FINDER, "SUCCESS: Successfully started DEVICE FINDER SERVICE - [%s]",
                                           "SSDP");
                        }

                        isStarted = true ;

                        return bRet ;
                    }

                    uint32_t
                    DeviceFinderImpl::setMaxAge()
                    {
                        std::string fName = "DeviceFinderImpl::setMaxAge()";
                        CM_LOG_DEBUG_A(CM_CORE_DEVICE_FINDER, "\n%s - Entered", fName.c_str());

                        uint32_t retVal = 0;
                        CM_LOG_DEBUG_A(CM_CORE_DEVICE_FINDER, "\n%s - Leaving with retVal>>%u<<",
                                       fName.c_str(), retVal);

                        return retVal;
                    } // end of setMaxAge(

                    bool
                    DeviceFinderImpl::stop()
                    {
                        TryReturn( m_pDeviceFinderService != NULL, false, "DeviceFinder Service in NULL: Not initialized");

                        bool bRet = m_pDeviceFinderService->stop() ;
                        TryLog( bRet == true, "Failed to stop SSDP devicefinder... continue others");

                        isStarted = false ;

                        return bRet ;
                    }

                    void
                    DeviceFinderImpl::onRequestComplete(int requestId, std::string requestPayload,
                                                        std::string responsePayload)
                    {
                        OC::Cm::Client::Resource::IResourceResponseListener *pLister = getAddDeviceListener(requestId);
                        if (pLister != NULL)
                        {
                            pLister->onRequestComplete(requestId, requestPayload, responsePayload) ;
                            removeAddDeviceListener(requestId);
                        }
                    }

                    void
                    DeviceFinderImpl::onErrorReceived( int clientSessionId, int status, OC::Cm::CMError error,
                                                       OC::Cm::Serialization::ISerializable *errorMessage )
                    {
                        CM_LOG_ERROR_A( CM_CORE_DEVICE_FINDER, "%s", "DeviceFinder::onErrorReceived():: Received" );

                        OC::Cm::Device *p_current_device = getPendingDevice(clientSessionId);
                        if ( p_current_device == NULL )
                        {
                            CM_LOG_ERROR_A( CM_CORE_DEVICE_FINDER, "%s",
                                            "DeviceFinder::onErrorReceived():: Pending device is NULL" );
                            return ;
                        }

                        removePendingDevice(clientSessionId) ;
                        this->notifyDevice(DEVICE_ADDED_ERROR,
                                           p_current_device);  // Needs to notify to Application in case of Error

                        OC::Cm::Client::Resource::IResourceResponseListener *pLister = getAddDeviceListener(
                                    clientSessionId);
                        if (pLister != NULL)
                        {
                            pLister->onErrorReceived(clientSessionId, status, error, errorMessage) ;
                        }
                    }


                    void
                    DeviceFinderImpl::OnGetResponseReceived( int clientSessionId, int status,
                            OC::Cm::Connector::HeadersList &headers, OC::Cm::Serialization::ISerializable *response )
                    {
                        CM_LOG_ERROR_A( CM_CORE_DEVICE_FINDER, "Called with %d", status);
                        OC::Cm::Device *p_current_device = getPendingDevice(clientSessionId);
                        if ( p_current_device == NULL )
                        {
                            CM_LOG_ERROR_A( CM_CORE_DEVICE_FINDER, "%s",
                                            "DeviceFinder::OnGetResponseReceived():: Pending device is NULL" );
                            return ;
                        }

                        if ( status == 200 )
                        {
                            if ( false == p_current_device->isDeviceDescriptionLoaded() )
                            {
                                if ( false == p_current_device->loadDeviceDescription( *response ) )
                                {
                                    CM_LOG_ERROR_A( CM_CORE_DEVICE_FINDER, "%s", "loadDeviceDescription failed" );
                                    //this->notifyDevice(DEVICE_ADDED_NORMAL, p_current_device);
                                }
                                else
                                {
                                    if ( true == p_current_device->isDeviceDescriptionLoaded() )
                                    {
                                        this->addDevice(p_current_device);
                                        this->notifyDevice(DEVICE_ADDED_NORMAL, p_current_device);
                                    }
                                    else
                                    {
                                        std::string deviceDescUri = p_current_device->getDeviceDescriptionUri() ;
                                        std::string host ;
                                        std::string port;
                                        std::string path;
                                        std::string hostheader;
                                        std::string method = "GET";
                                        std::string protocol ;

                                        int client_session_id ;

                                        if (isValidUri(deviceDescUri))
                                        {
                                            if ( false == parseUrl( deviceDescUri, protocol, host, port, path, hostheader ) )
                                            {
                                                CM_LOG_ERROR_A( CM_CORE_DEVICE_FINDER, "%s", "failed to parse uri" );
                                                return;
                                            }
                                        }
                                        else
                                        {
                                            protocol    = p_current_device->getProtocol();
                                            hostheader  = p_current_device->getAddress();
                                            path        = deviceDescUri ;
                                        }

                                        OC::Cm::Core::CMContext *pContext = (OC::Cm::Core::CMContext *)p_context ;
                                        TryReturnVoid(pContext != NULL, "Failed to get Control Manager Context Class Instance");

                                        OC::Cm::CMError error = pContext->m_engine.sendClientRequest(protocol, hostheader, path, method,
                                                                NULL, NULL, client_session_id, *this, false);
                                        if ( error.getErrorCode() != CM_SUCCESS)
                                        {
                                            CM_LOG_ERROR_A(CM_CORE_DEVICE_FINDER, "%s", "failed to sendClientRequest");
                                            return;
                                        }

                                        CM_LOG_ERROR_A(CM_CORE_DEVICE_FINDER, "Adding Device into Pending List with Session ID [%d]",
                                                       client_session_id);
                                        addPendingDevice( client_session_id, p_current_device ) ;
                                        addAddDeviceListener( client_session_id, getAddDeviceListener(clientSessionId));
                                        return ;
                                    }
                                }
                            }
                            else
                            {
                                this->notifyDevice(DEVICE_ADDED_NORMAL, p_current_device);
                            }
                        }
                        else
                        {
                            CM_LOG_ERROR_A(CM_CORE_DEVICE_FINDER, "%s",
                                           "DeviceFinder::OnGetResponseReceived():: Error response received for device");
                            //this->notifyDevice(DEVICE_ADDED_NORMAL, p_current_device);
                        }

                        removePendingDevice(clientSessionId) ;
                    }

                    void
                    DeviceFinderImpl::OnPutResponseReceived(int clientSessionId,  int status,
                                                            OC::Cm::Connector::HeadersList &headers,  OC::Cm::Serialization::ISerializable *response)
                    {
                        CM_LOG_INFO_A( CM_CORE_DEVICE_FINDER, "Called with Session ID [%d], Status [%d]", clientSessionId,
                                       status );
                    }

                    void
                    DeviceFinderImpl::OnPostResponseReceived(int clientSessionId, int status,
                            OC::Cm::Connector::HeadersList &headers, OC::Cm::Serialization::ISerializable *response)
                    {
                    }

                    void
                    DeviceFinderImpl::onDeleteResponseReceived(int clientSessionId, int status,
                            OC::Cm::Connector::HeadersList &headers, OC::Cm::Serialization::ISerializable *response)
                    {
                    }

                    void
                    DeviceFinderImpl::onSubscribeResponseReceived(int clientSessionId, int status,
                            std::string &subscriptionUri)
                    {
                    }

                    void
                    DeviceFinderImpl::onUnSubscribeResponseReceived(int clientSessionId, int status )
                    {
                    }

                    void
                    DeviceFinderImpl::onNotifyResponseReceived(int clientSessionId, int status)
                    {
                    }

                    void
                    DeviceFinderImpl::createDevicefromServiceDevice(
                        OC::Cm::Core::DeviceFinder::Service::DeviceFinderServiceDevice &device)
                    {
                        CM_LOG_INFO_A(CM_CORE_DEVICE_FINDER, "%s", "Loading Device Description");
                        DeviceDomain deviceDomain = device.getDomain();
                        DeviceType deviceType =  device.getDeviceType();

                        CM_LOG_DEBUG_A(CM_CORE_DEVICE_FINDER, "DeviceType of found device: %d", (int)deviceType);

                        CM_LOG_INFO_A(CM_CORE_DEVICE_FINDER, "createDevice with Address [%s] DeviceType [%d], Domain [%d]",
                                      device.getIPAddress().c_str(), deviceType, deviceDomain);
                        OC::Cm::Device *p_controlDevice = createDevice(deviceType, deviceDomain);
                        TryReturnVoid(p_controlDevice != NULL, "failed to create Device Instance");
                        p_controlDevice->setDeviceDescriptionLoaded(false);

                        p_controlDevice->setContext(p_context) ;

                        std::string deviceResourceUri = device.getLocation();
                        std::string host ;
                        std::string port;
                        std::string path;
                        std::string hostheader;
                        std::string protocol;

                        CM_LOG_INFO_A(CM_CORE_DEVICE_FINDER, "Before ParseUrl [%s]", deviceResourceUri.c_str());

                        if (false == parseUrl(deviceResourceUri, protocol, host, port, path, hostheader))
                        {
                            CM_LOG_ERROR_A(CM_CORE_DEVICE_FINDER, "%s", "failed to parse uri");
                            return;
                        }

                        p_controlDevice->setAddress(hostheader.c_str());
                        p_controlDevice->setProtocol(protocol.c_str());
                        p_controlDevice->setUUID(device.getDeviceUUID().c_str());
                        p_controlDevice->setDeviceName(device.getDeviceName().c_str());
                        p_controlDevice->setDescription(device.getDeviceDescription().c_str());
                        p_controlDevice->setDeviceDescriptionUri(device.getLocation().c_str());
                        p_controlDevice->setManufacturer(device.getDeviceManufacturer().c_str());
                        p_controlDevice->setDeviceSubType(device.getDeviceDeviceSubType().c_str());
                        p_controlDevice->setModelID(device.getDeviceModelID().c_str());
                        p_controlDevice->setSerialNumber(device.getDeviceSerialNumber().c_str());
                        p_controlDevice->setSalesLocation(device.getDeviceSalesLocation().c_str());
                        p_controlDevice->setDeviceOnlineStatus(device.getDeviceOnlineStatus());

                        OC::Cm::Core::CMContext *pContext = (OC::Cm::Core::CMContext *)p_context ;
                        TryReturnVoid(pContext != NULL, "Failed to get Control Manager Context Class Instance");
                        TryReturnVoid((pContext->m_engine.getCMConfiguration()) != NULL,
                                      "Failed to get Control Manager Configuration Class Instance");

                        CM_LOG_INFO_A(CM_CORE_DEVICE_FINDER, "%s", "Before sendClientRequest");

                        std::string method = "GET";
                        int client_session_id ;
                        OC::Cm::CMError error = pContext->m_engine.sendClientRequest( protocol, hostheader, path, method,
                                                NULL, NULL, client_session_id, *this, false );
                        if ( error.getErrorCode() != CM_SUCCESS)
                        {
                            CM_LOG_ERROR_A(CM_CORE_DEVICE_FINDER, "%s", "failed to sendClientRequest");

                            //CM_LOG_INFO_A(CM_CORE_DEVICE_FINDER, "%s", "Adding Device as without populating resources");
                            //this->notifyDevice(DEVICE_ADDED_ERROR, p_controlDevice); /* OIC TODO : Need to handle loading the device description in this scenario */
                            delete p_controlDevice;

                            return;
                        }

                        CM_LOG_INFO_A(CM_CORE_DEVICE_FINDER, "Adding Device into Pending List with Session ID [%d]",
                                      client_session_id);
                        addPendingDevice(client_session_id, p_controlDevice) ;

                        CM_LOG_INFO_A(CM_CORE_DEVICE_FINDER, "%s", "Leave");
                    }

                    void
                    DeviceFinderImpl::updateDevicefromServiceDevice(
                        OC::Cm::Core::DeviceFinder::Service::DeviceFinderServiceDevice &device,
                        OC::Cm::Device *ControlDevice)
                    {
                        CM_LOG_INFO_A(CM_CORE_DEVICE_FINDER, "%s", "Loading Device Description");
                        DeviceDomain deviceDomain = device.getDomain();
                        DeviceType deviceType =  device.getDeviceType();

                        CM_LOG_DEBUG_A(CM_CORE_DEVICE_FINDER, "DeviceType of found device: %d", (int)deviceType);

                        CM_LOG_INFO_A(CM_CORE_DEVICE_FINDER, "update Device with DeviceType [%d], Domain [%d]", deviceType,
                                      deviceDomain);
                        ControlDevice->setDeviceType(deviceType);
                        ControlDevice->setDomain(deviceDomain);
                        ControlDevice->setDeviceDescriptionLoaded(false);

                        ControlDevice->setContext(p_context) ;

                        std::string deviceResourceUri = device.getLocation();
                        std::string host ;
                        std::string port;
                        std::string path;
                        std::string hostheader;
                        std::string protocol;

                        CM_LOG_INFO_A(CM_CORE_DEVICE_FINDER, "Before ParseUrl [%s]", deviceResourceUri.c_str());

                        if (false == parseUrl(deviceResourceUri, protocol, host, port, path, hostheader))
                        {
                            CM_LOG_ERROR_A(CM_CORE_DEVICE_FINDER, "%s", "failed to parse uri");
                            return;
                        }

                        ControlDevice->setAddress(hostheader.c_str());
                        ControlDevice->setProtocol(protocol.c_str());
                        ControlDevice->setDeviceName(device.getDeviceName().c_str());
                        ControlDevice->setDescription(device.getDeviceDescription().c_str());
                        ControlDevice->setDeviceDescriptionUri(device.getLocation().c_str());
                        ControlDevice->setManufacturer(device.getDeviceManufacturer().c_str());
                        ControlDevice->setDeviceSubType(device.getDeviceDeviceSubType().c_str());
                        ControlDevice->setModelID(device.getDeviceModelID().c_str());
                        ControlDevice->setSerialNumber(device.getDeviceSerialNumber().c_str());
                        ControlDevice->setSalesLocation(device.getDeviceSalesLocation().c_str());
                        ControlDevice->setDeviceOnlineStatus(device.getDeviceOnlineStatus());

                        OC::Cm::Core::CMContext *pContext = (OC::Cm::Core::CMContext *)p_context ;
                        TryReturnVoid(pContext != NULL, "Failed to get Control Manager Context Class Instance");
                        TryReturnVoid((pContext->m_engine.getCMConfiguration()) != NULL,
                                      "Failed to get Control Manager Configuration Class Instance");

                        this->notifyDevice(DEVICE_UPDATED, ControlDevice) ;

                        CM_LOG_INFO_A(CM_CORE_DEVICE_FINDER, "%s", "Leave");
                    }

                    void
                    DeviceFinderImpl::OnDeviceAdded(OC::Cm::Core::DeviceFinder::Service::DeviceFinderServiceDevice
                                                    &sdevice)
                    {
                        CM_LOG_INFO_A(CM_CORE_DEVICE_FINDER, "%s", "Start");

                        sdevice.setDeviceOnlineStatus(true);
                        createDevicefromServiceDevice(sdevice);

                        CM_LOG_INFO_A(CM_CORE_DEVICE_FINDER, "%s", "Leave");
                    }

                    void
                    DeviceFinderImpl::OnDeviceRemoved(OC::Cm::Core::DeviceFinder::Service::DeviceFinderServiceDevice
                                                      &sdevice)
                    {
                        CM_LOG_INFO_A(CM_CORE_DEVICE_FINDER, "%s", "Start");

                        OC::Cm::Device *device = NULL ;

                        {
                            std::string uuid = sdevice.getDeviceUUID() ;
                            device = getDeviceByUUID(uuid) ;
                        }

                        if (NULL != device)
                        {
                            this->notifyDevice(DEVICE_REMOVED, device);
                            removeDevice(device);
                            delete device; device = NULL;
                        }

                        CM_LOG_INFO_A(CM_CORE_DEVICE_FINDER, "%s", "Leave");
                    }

                    void
                    DeviceFinderImpl::OnDeviceUpdated(OC::Cm::Core::DeviceFinder::Service::DeviceFinderServiceDevice
                                                      &sdevice)
                    {
////    CM_LOG_INFO_A(CM_CORE_DEVICE_FINDER, "%s", "Start");
//
//
//  OC::Cm::Device* device = NULL ;
//
//  std::string uuid = sdevice.getDeviceUUID() ;
//  device = getDeviceByUUID(uuid) ;
//
//  if (NULL != device)
//  {
//      this->notifyDevice(DEVICE_UPDATED, device);
//  }
//
////    CM_LOG_INFO_A(CM_CORE_DEVICE_FINDER, "%s", "Leave");
                    }

                    void
                    DeviceFinderImpl::OnServiceError()
                    {
                        std::string fName = "DeviceFinderImpl::OnServiceError()";
                        CM_LOG_DEBUG_A(CM_CORE_DEVICE_FINDER, "\n%s - Entered", fName.c_str());

                        if ( NULL == m_pDeviceFinderService)
                        {
                            CM_LOG_ERROR_A(CM_CORE_DEVICE_FINDER, "\n%s - ERROR: DeviceFinder Service is NULL: "
                                           "Hence not bouncing SSDP Device Finder Service", fName.c_str());
                            return;
                        }

                        CM_LOG_INFO_A(CM_CORE_DEVICE_FINDER, "\n%s - Clearing all devices discovered till now, "
                                      "as SSDP Device Finder will be re-started", fName.c_str());

                        clearDevices();

                        bool bRet = m_pDeviceFinderService->bounce();

                        if (false == bRet)
                        {
                            CM_LOG_ERROR_A(CM_CORE_DEVICE_FINDER,
                                           "\n%s - ERROR: Failed to BOUNCE SSDP Device Finder Service!", fName.c_str());
                        }

                        CM_LOG_DEBUG_A(CM_CORE_DEVICE_FINDER, "\n%s - Leaving with retVal:>>%s<<",
                                       fName.c_str(), (bRet) ? ("True") : ("False") );
                    }

                    Device *
                    DeviceFinderImpl::createDevice( OC::Cm::DeviceType type, OC::Cm::DeviceDomain domain)
                    {
                        CM_LOG_INFO_A(CM_CORE_DEVICE_FINDER, "%s", "Start");

                        TryReturn( m_pDeviceFactory != NULL, NULL, "DeviceFactory is NULL" );

                        OC::Cm::Device *pDevice = m_pDeviceFactory->createDevice(type, domain);

                        CM_LOG_INFO_A(CM_CORE_DEVICE_FINDER, "%s", "Leave");

                        return pDevice;
                    }

                    OC::Cm::Device *
                    DeviceFinderImpl::getDeviceByAddress(std::string &deviceAddress)
                    {
                        Device *device = NULL;

                        m_deviceListMutex.lock() ;

                        CM_LOG_INFO_A(CM_CORE_DEVICE_FINDER, "getDeviceByAddress: %s", deviceAddress.c_str());
                        for ( std::map< std::string, OC::Cm::Device * >::const_iterator itr = m_devicesMap.begin();
                              itr != m_devicesMap.end(); itr++ )
                        {
                            if (itr->second != NULL)
                            {
                                if (itr->second->getAddress().compare(deviceAddress) == 0)
                                {
                                    device = (itr)->second ;
                                    break ;
                                }
                            }
                        }

                        m_deviceListMutex.unlock() ;

                        return device ;
                    }

                    OC::Cm::Device *
                    DeviceFinderImpl::getDeviceByUUID(std::string &uuid)
                    {
                        Device *device = NULL;

                        m_deviceListMutex.lock() ;

                        std::map< std::string, OC::Cm::Device * >::iterator itr = m_devicesMap.find( uuid ) ;
                        if ( itr == m_devicesMap.end() )
                        {
                            CM_LOG_DEBUG_A( CM_CORE_DEVICE_FINDER, "%s", "DeviceFinder::getDeviceByUUID():: device not found" );
                        }
                        else
                        {
                            device = (itr)->second ;
                        }

                        m_deviceListMutex.unlock() ;

                        return device ;
                    }

                    void
                    DeviceFinderImpl::notifyDevice( int type, OC::Cm::Device *pDevice,
                                                    OC::Cm::DeviceFinder::IDeviceFinderListener *pListener)
                    {
                        if (NULL == pListener)
                        {
                            return ;
                        }

                        CM_LOG_DEBUG_A(CM_CORE_DEVICE_FINDER, "Notifying to Application by Type [%d]", type);

                        if (DEVICE_ADDED_NORMAL == type)
                        {
                            pListener->OnDeviceAdded( *pDevice ) ;
                        }
                        else if (DEVICE_REMOVED == type)
                        {
                            pListener->OnDeviceRemoved( *pDevice ) ;
                        }
                        else if (DEVICE_UPDATED == type)
                        {
                            pListener->OnDeviceUpdated( *pDevice );
                        }
                        else if (DEVICE_ADDED_ERROR == type)
                        {
                            pListener->OnDeviceError( *pDevice );
                        }
                    }

                    void
                    DeviceFinderImpl::notifyDevice( int type, OC::Cm::Device *pDevice)
                    {
                        if (NULL == pDevice)
                        {
                            return ;
                        }

                        OC::Cm::DeviceFinder::IDeviceFinderListener *pListener = m_ListenerMap[  std::make_pair(
                                    DeviceDomain_All, DeviceType_All) ] ;
                        if ( pListener != NULL )
                        {
                            this->notifyDevice( type, pDevice, pListener) ;
                        }

                        pListener = m_ListenerMap[  std::make_pair(pDevice->getDomain(), pDevice->getDeviceType()) ] ;
                        if ( pListener != NULL )
                        {
                            this->notifyDevice( type, pDevice, pListener) ;
                        }

                        pListener = m_ListenerMap[  std::make_pair(DeviceDomain_All, pDevice->getDeviceType()) ] ;
                        if ( pListener != NULL )
                        {
                            this->notifyDevice( type, pDevice, pListener) ;
                        }

                        pListener = m_ListenerMap[  std::make_pair(pDevice->getDomain(), DeviceType_All) ] ;
                        if ( pListener != NULL )
                        {
                            this->notifyDevice( type, pDevice, pListener) ;
                        }
                    }

                    void
                    DeviceFinderImpl::clearDevices()
                    {
                        Device *device = NULL;

                        m_deviceListMutex.lock();

                        for ( std::map< std::string, OC::Cm::Device * >::const_iterator itr = m_devicesMap.begin();
                              itr != m_devicesMap.end(); itr++ )
                        {
                            device = itr->second ;
                            if (NULL != device)
                            {
                                delete device; device = NULL;
                            }
                        }

                        for (std::map< int, OC::Cm::Device *>::iterator itr = m_pPendingDevicesMap.begin();
                             itr != m_pPendingDevicesMap.end(); itr++)
                        {
                            device = itr->second ;
                            if (NULL != device)
                            {
                                delete device; device = NULL;
                            }
                        }

                        m_devicesMap.clear();
                        m_pPendingDevicesMap.clear();

                        m_deviceListMutex.unlock();
                    }

                    void
                    DeviceFinderImpl::addPendingDevice( int requestId, OC::Cm::Device *pDevice )
                    {
                        m_deviceListMutex.lock();

                        m_pPendingDevicesMap.insert( std::pair< int, OC::Cm::Device * >( requestId, pDevice ) );

                        m_deviceListMutex.unlock();
                    }

                    OC::Cm::Device *
                    DeviceFinderImpl::getPendingDevice(std::string &uuid)
                    {
                        Device *device = NULL;
                        int requestId = -1 ;

                        m_deviceListMutex.lock() ;

                        for (std::map< int, OC::Cm::Device *>::iterator itr = m_pPendingDevicesMap.begin();
                             itr != m_pPendingDevicesMap.end(); itr++)
                        {
                            if ((itr->second != NULL) && (itr->second->getUUID().compare(uuid) == 0))
                            {
                                device = itr->second ;
                                requestId = itr->first ;
                                break ;
                            }
                        }

                        if (requestId >= 0)
                        {
                            m_pPendingDevicesMap.erase(requestId);
                        }

                        m_deviceListMutex.unlock() ;

                        return device ;
                    }

                    void
                    DeviceFinderImpl::removePendingDevice( int requestId )
                    {
                        m_deviceListMutex.lock();

                        m_pPendingDevicesMap.erase(requestId);

                        m_deviceListMutex.unlock();
                    }

                    OC::Cm::Device *
                    DeviceFinderImpl::getPendingDevice( int requestId )
                    {
                        Device *device = NULL;

                        m_deviceListMutex.lock();

                        std::map< int, OC::Cm::Device * >::iterator itr = m_pPendingDevicesMap.find( requestId ) ;
                        if ( itr == m_pPendingDevicesMap.end() )
                        {
                            CM_LOG_ERROR_A( CM_CORE_DEVICE_FINDER, "%s",
                                            "DeviceFinder::onErrorReceived():: Session id not found in pending session map" );
                        }
                        else
                        {
                            device = (itr)->second ;
                        }

                        m_pPendingDevicesMap.erase(requestId);

                        m_deviceListMutex.unlock();

                        return device ;
                    }

                    void
                    DeviceFinderImpl::addAddDeviceListener( int requestId,
                                                            OC::Cm::Client::Resource::IResourceResponseListener *pListsener )
                    {
                        if (NULL == pListsener)
                        {
                            return ;
                        }

                        m_deviceListMutex.lock();

                        m_pAddDeviceListener.insert(
                            std::pair< int, OC::Cm::Client::Resource::IResourceResponseListener * >( requestId, pListsener ) );

                        m_deviceListMutex.unlock();
                    }

                    void
                    DeviceFinderImpl::removeAddDeviceListener( int requestId )
                    {
                        m_deviceListMutex.lock();

                        m_pAddDeviceListener.erase(requestId);

                        m_deviceListMutex.unlock();
                    }

                    OC::Cm::Client::Resource::IResourceResponseListener *
                    DeviceFinderImpl::getAddDeviceListener( int requestId )
                    {
                        OC::Cm::Client::Resource::IResourceResponseListener *device = NULL;

                        m_deviceListMutex.lock();

                        std::map< int, OC::Cm::Client::Resource::IResourceResponseListener * >::iterator itr =
                            m_pAddDeviceListener.find( requestId ) ;
                        if ( itr == m_pAddDeviceListener.end() )
                        {
                            CM_LOG_ERROR_A( CM_CORE_DEVICE_FINDER, "%s",
                                            "DeviceFinder::getAddDeviceListener():: Session id not found in pending session map" );
                        }
                        else
                        {
                            device = (itr)->second ;
                        }

                        m_deviceListMutex.unlock();

                        return device ;
                    }

                    void
                    DeviceFinderImpl::addDevice( OC::Cm::Device *pDevice )
                    {
                        m_deviceListMutex.lock();

                        if (pDevice != NULL)
                        {
                            std::string uuid = pDevice->getUUID() ;

                            std::map< std::string, OC::Cm::Device * >::iterator itr = m_devicesMap.find( uuid ) ;
                            if ( itr != m_devicesMap.end() )
                            {
                                delete (itr->second) ;
                                m_devicesMap.erase( uuid );
                            }

                            m_devicesMap.insert( std::pair< std::string, OC::Cm::Device * >(uuid, pDevice)) ;
                        }

                        m_deviceListMutex.unlock();
                    }

                    void
                    DeviceFinderImpl::removeDevice( OC::Cm::Device *pDevice  )
                    {
                        m_deviceListMutex.lock();

                        if (pDevice != NULL)
                        {
                            m_devicesMap.erase(pDevice->getUUID());
                        }

                        m_deviceListMutex.unlock();
                    }

                    bool
                    DeviceFinderImpl::addDevice(OC::Cm::DeviceType type, std::string uuid, std::string address,
                                                OC::Cm::Client::Resource::IResourceResponseListener &listener)
                    {
                        CM_LOG_INFO_A(CM_CORE_DEVICE_FINDER, "%s", "Start");
                        std::string deviceResourceUri = address + "/capability" ;
                        std::string host ;
                        std::string port;
                        std::string path;
                        std::string hostheader;
                        std::string protocol;

                        CM_LOG_INFO_A(CM_CORE_DEVICE_FINDER, "%s", "Before createDevice");
                        OC::Cm::Device *ControlDevice = createDevice(type, DeviceDomain_Local);
                        if (ControlDevice == NULL)
                        {
                            CM_LOG_ERROR_A(CM_CORE_DEVICE_FINDER, "%s", "failed to create Device Instance");
                            return false;
                        }

                        ControlDevice->setContext(p_context) ;

                        ControlDevice->setUUID(uuid.c_str());

                        CM_LOG_INFO_A(CM_CORE_DEVICE_FINDER, "%s", "Before ParseUrl");

                        if (false == parseUrl(deviceResourceUri, protocol, host, port, path, hostheader))
                        {
                            CM_LOG_ERROR_A(CM_CORE_DEVICE_FINDER, "%s", "failed to parse uri");
                            return false;
                        }

                        ControlDevice->setAddress(hostheader.c_str());
                        ControlDevice->setProtocol(protocol.c_str()) ;

                        addDevice(ControlDevice) ;
                        this->notifyDevice(DEVICE_ADDED_NORMAL, ControlDevice) ;

                        CM_LOG_INFO_A(CM_CORE_DEVICE_FINDER, "%s", "Leave");
                        return true ;
                    }

                    void
                    DeviceFinderImpl::setIsEasySetupThroughDeviceDiscovery(bool flag)
                    {
                        m_isEasySetupThroughDeviceDiscovery = flag;
                    }


                }
            }
        }
    }
}
