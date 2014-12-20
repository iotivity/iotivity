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



#include "Device.h"
#include "CMContext.h"
#include "Utils.h"
#include "Log.h"

#include "Client/Resources.h"

namespace OC
{
    namespace Cm
    {
        Device::Device(OC::Cm::DeviceType type, OC::Cm::DeviceDomain domain)
        {
            this->m_supportedResources.clear();

            this->m_deviceType = type;

            this->m_device_description_loaded = false;

            this->m_domain = domain ;

            this->p_context = NULL ;

            this->m_protocol = "https";

            this->m_isOnline = false;

            this->m_name.clear();

        }

        std::string
        Device::getDeviceName()
        {
            return m_name ;
        }

        bool
        Device::setDeviceName(const char *name)
        {
            if ((NULL == name) || (0 >= strlen(name))) { return false; }
            this->m_name = name;
            return true;
        }

        std::string Device::getAddress()
        {
            return m_address;
        }

        std::string
        Device::getProtocol()
        {
            return m_protocol ;
        }

        bool
        Device::setProtocol(const char *protocol)
        {
            if ((NULL == protocol) || (0 >= strlen(protocol))) { return false; }
            this->m_protocol = protocol ;
            return true;
        }

        bool Device::setAddress(const char *address)
        {
            if ((NULL == address) || (0 >= strlen(address))) { return false; }
            if (isValidAddress(m_domain, address) == false) { return false;}

            m_address = address;
            return true;
        }

        std::string Device::getDeviceDescriptionUri() const
        {
            return m_deviceResourceURI ;
        }

        bool Device::setDeviceDescriptionUri(const char *deviceResourceUri)
        {
            if ((NULL == deviceResourceUri) || (0 >= strlen(deviceResourceUri))) { return false; }
            m_deviceResourceURI = deviceResourceUri;
            return true;
        }

        OC::Cm::DeviceType Device::getDeviceType() const
        {
            return m_deviceType;
        }

        bool Device::setDeviceType(OC::Cm::DeviceType deviceType)
        {
            if (deviceType < DeviceType_Unknown) { return false; }
            m_deviceType = deviceType;
            return true;
        }

        OC::Cm::DeviceDomain Device::getDomain() const
        {
            return m_domain;
        }

        bool Device::setDomain(OC::Cm::DeviceDomain domain)
        {
            if ((domain != DeviceDomain_Local) && (domain != DeviceDomain_Remote)
                && (domain != DeviceDomain_All)) { return false; }
            m_domain = domain;
            return true;
        }

        std::string Device::getUUID() const
        {
            return m_uuid;
        }

        bool Device::setUUID(const char *uuid)
        {
            if ((NULL == uuid) || (0 >= strlen(uuid))) { return false; }
            m_uuid = uuid;
            return true;
        }

        void
        Device::setDeviceDescriptionLoaded(bool isLoaded)
        {
            m_device_description_loaded = isLoaded;
        }

        bool
        Device::isDeviceDescriptionLoaded()
        {
            return m_device_description_loaded ;
        }

        bool
        Device::setContext(OC::Cm::IContext *context)
        {
            if (context == NULL) { return false; }
            this->p_context = context ;
            return true;
        }

        OC::Cm::IContext *
        Device::getContext()
        {
            return this->p_context;
        }

        std::string Device::getDescription() const
        {
            return m_description;
        }
        bool Device::setDescription(const char *description)
        {
            if ((NULL == description) || (0 >= strlen(description))) { return false; }
            m_description = description;
            return true;
        }

        std::string Device::getManufacturer() const
        {
            return m_manufacturer;
        }

        bool Device::setManufacturer(const char *manufacturer)
        {
            if ((NULL == manufacturer) || (0 >= strlen(manufacturer))) { return false; }
            m_manufacturer = manufacturer;
            return true;
        }

        std::string Device::getDeviceSubType() const
        {
            return m_deviceSubType;
        }

        bool Device::setDeviceSubType(const char *deviceSubType)
        {
            if ((NULL == deviceSubType) || (0 >= strlen(deviceSubType))) { return false; }
            m_deviceSubType = deviceSubType;
            return true;
        }

        std::string Device::getModelID() const
        {
            return m_modelID;
        }

        bool Device::setModelID(const char *modelID)
        {
            if ((NULL == modelID) || (0 >= strlen(modelID))) { return false; }
            m_modelID = modelID;
            return true;
        }

        std::string Device::getSerialNumber() const
        {
            return m_serialNumber;
        }

        bool Device::setSerialNumber(const char *serialNumber)
        {
            if ((NULL == serialNumber) || (0 >= strlen(serialNumber))) { return false; }
            m_serialNumber = serialNumber;
            return true;
        }

        std::string Device::getSalesLocation() const
        {
            return m_salesLocation;
        }

        bool Device::setSalesLocation(const char *salesLocation)
        {
            if ((NULL == salesLocation) || (0 >= strlen(salesLocation))) { return false; }
            m_salesLocation = salesLocation;
            return true;
        }

        std::list<std::string> &Device::getSupportedResource()
        {
            return m_supportedResources;
        }

        bool Device::setSupportedResource(const char *supportedResource)
        {
            if ((NULL == supportedResource) || (0 >= strlen(supportedResource))) { return false; }
            m_supportedResources.push_back(supportedResource);
            return true;
        }

        bool Device::getDeviceOnlineStatus()
        {
            return m_isOnline;
        }

        void Device::setDeviceOnlineStatus(bool status)
        {
            m_isOnline = status;
        }

        OC::Cm::Client::Resource::Resource *
        Device::getResource( std::string resourceType )
        {
            for ( std::list<std::string>::const_iterator itr = m_supportedResources.begin();
                  itr != m_supportedResources.end(); itr++ )
            {
                if ( resourceType.compare(*itr) == 0)
                {
                    return createResource(resourceType);
                }
            }

            return NULL ;
        }

        std::string
        Device::getSubscriptionsLink()
        {
            return m_subscriptionsLink ;
        }

        bool
        Device::loadDeviceDescription(OC::Cm::Serialization::ISerializable &serializable)
        {
            std::string elementName = serializable.getElementName();

            CM_LOG_INFO_A(CM_API_CLIENT, "loadDeviceDescription elementName [%s]", elementName.c_str());

            if (0 == elementName.compare(EN_CAPABILITY))
            {
                //m_capabilities = (Capability&) serializable ;
                m_capabilities.clone((Capability *)&serializable);

                if ((m_capabilities.mpDevicesLink == NULL) || (m_capabilities.mpDevicesLink->mpHref == NULL))
                {
                    return false ;
                }

                setDeviceDescriptionUri(m_capabilities.mpDevicesLink->mpHref->c_str());

                if ((m_capabilities.mpSubscriptionsLink == NULL)
                    || (m_capabilities.mpSubscriptionsLink->mpHref == NULL))
                {
                    return false ;
                }

                if (m_capabilities.mpSubscriptionsLink->mpHref->empty() == false)
                {
                    m_subscriptionsLink = *(m_capabilities.mpSubscriptionsLink->mpHref) ;
                }

                m_device_description_loaded = false ;

                return true ;
            }
            else if (0 == elementName.compare(EN_DEVICES))
            {
                //m_devices = (::Devices&) serializable ;
                m_devices.clone((::Devices *)&serializable);

                // load  Device[0]....
                if ((m_devices.mpDevice.size() > 0))
                {
                    ::Device *device = m_devices.mpDevice.front();
                    if (NULL != device)
                    {
                        if (/*( NULL != device ) && ( */NULL != device->mpResources/*)*/)
                        {
                            for ( std::list< ::ResourceType >::const_iterator itr = device->mpResources->members.begin();
                                  itr != device->mpResources->members.end(); itr++ )
                            {
                                m_supportedResources.push_back( (*itr).value );
                            }
                        }

                        if (device->mpName != NULL)
                        {
                            this->setDeviceName(device->mpName->value.c_str());
                        }
                        if (device->mpDescription != NULL)
                        {
                            this->setDescription(device->mpDescription->value.c_str());
                        }

                        if (device->mpUuid != NULL)
                        {
                            this->setUUID(device->mpUuid->value.c_str());
                        }

                        if (device->mpConnected != NULL)
                        {
                            this->setDeviceOnlineStatus(device->mpConnected);
                        }


                        if (device->mpType != NULL)
                        {
                            for (int i = 0 ; i < (int)OC::Cm::DeviceType::DeviceType_All ;  i++ )
                            {
                                if (device->mpType->value.compare(OC::Cm::DeviceType_Strings[i]) == 0 )
                                {
                                    this->setDeviceType((OC::Cm::DeviceType)i);
                                    break ;
                                }
                            }
                        }

                        m_device_description_loaded = true ;
                        return true ;
                    }
                }
            }

            return false;
        }

        OC::Cm::Client::Resource::Resource *
        Device::createResource(std::string &resourceType)
        {
            std::string address     = getAddress() ;
            std::string protocol    = getProtocol() ;

            if (resourceType.compare("AccessPoint") == 0)
            {
                return new ::AccessPointResource(getContext() , protocol, address) ;
            }
            else if (resourceType.compare("AccessPoints") == 0)
            {
                return new ::AccessPointsResource(getContext() , protocol, address) ;
            }



            else if (resourceType.compare("Alarm") == 0)
            {
                return new ::AlarmResource(getContext() , protocol, address) ;
            }
            else if (resourceType.compare("Alarms") == 0)
            {
                return new ::AlarmsResource(getContext() , protocol, address) ;
            }
            else if (resourceType.compare("Capability") == 0)
            {
                return new ::CapabilityResource(getContext() , protocol, address) ;
            }
            else if (resourceType.compare("Configuration") == 0)
            {
                return new ::ConfigurationResource(getContext() , protocol, address) ;
            }
            else if (resourceType.compare("Detection") == 0)
            {
                return new ::DetectionResource(getContext() , protocol, address) ;
            }
            else if (resourceType.compare("Device") == 0)
            {
                return new ::DeviceResource(getContext() , protocol, address) ;
            }
            else if (resourceType.compare("Devices") == 0)
            {
                return new ::DevicesResource(getContext() , protocol, address) ;
            }
            else if (resourceType.compare("Door") == 0)
            {
                return new ::DoorResource(getContext() , protocol, address) ;
            }
            else if (resourceType.compare("Doors") == 0)
            {
                return new ::DoorsResource(getContext() , protocol, address) ;
            }
            else if (resourceType.compare("Energy") == 0)
            {
                return new ::EnergyResource(getContext() , protocol, address) ;
            }
            else if (resourceType.compare("EnergyConsumption") == 0)
            {
                return new ::EnergyConsumptionResource(getContext() , protocol, address) ;
            }

            else if (resourceType.compare("Humidity") == 0)
            {
                return new ::HumidityResource(getContext() , protocol, address) ;
            }
            else if (resourceType.compare("Information") == 0)
            {
                return new ::InformationResource(getContext() , protocol, address) ;
            }
            else if (resourceType.compare("Level") == 0)
            {
                return new ::LevelResource(getContext() , protocol, address) ;
            }
            else if (resourceType.compare("Light") == 0)
            {
                return new ::LightResource(getContext() , protocol, address) ;
            }
            else if (resourceType.compare("LongPollingNotification") == 0)
            {
                return new ::LongPollingNotificationResource(getContext() , protocol, address) ;
            }
            else if (resourceType.compare("Mode") == 0)
            {
                return new ::ModeResource(getContext() , protocol, address) ;
            }
            else if (resourceType.compare("Movement") == 0)
            {
                return new ::MovementResource(getContext() , protocol, address) ;
            }
            else if (resourceType.compare("Network") == 0)
            {
                return new ::NetworkResource(getContext() , protocol, address) ;
            }
            else if (resourceType.compare("Networks") == 0)
            {
                return new ::NetworksResource(getContext() , protocol, address) ;
            }
            else if (resourceType.compare("Notification") == 0)
            {
                return new ::NotificationResource(getContext() , protocol, address) ;
            }
            else if (resourceType.compare("Notifications") == 0)
            {
                return new ::NotificationsResource(getContext() , protocol, address) ;
            }
            else if (resourceType.compare("Operation") == 0)
            {
                return new ::OperationResource(getContext() , protocol, address) ;
            }
            else if (resourceType.compare("Subscription") == 0)
            {
                return new ::SubscriptionResource(getContext() , protocol, address) ;
            }
            else if (resourceType.compare("Subscriptions") == 0)
            {
                return new ::SubscriptionsResource(getContext() , protocol, address) ;
            }
            else if (resourceType.compare("Temperature") == 0)
            {
                return new ::TemperatureResource(getContext() , protocol, address) ;
            }
            else if (resourceType.compare("Temperatures") == 0)
            {
                return new ::TemperaturesResource(getContext() , protocol, address) ;
            }
            else if (resourceType.compare("Thermostat") == 0)
            {
                return new ::ThermostatResource(getContext() , protocol, address) ;
            }
            else if (resourceType.compare("Time") == 0)
            {
                return new ::TimeResource(getContext() , protocol, address) ;
            }
            else if (resourceType.compare("UsageLimit") == 0)
            {
                return new ::UsageLimitResource(getContext() , protocol, address) ;
            }
            else if (resourceType.compare("UsageLimits") == 0)
            {
                return new ::UsageLimitsResource(getContext() , protocol, address) ;
            }
            else if (resourceType.compare("Usages") == 0)
            {
                return new ::UsagesResource(getContext() , protocol, address) ;
            }
            else if (resourceType.compare("Version") == 0)
            {
                return new ::VersionResource(getContext() , protocol, address) ;
            }
            else if (resourceType.compare("Versions") == 0)
            {
                return new ::VersionsResource(getContext() , protocol, address) ;
            }

            else if (resourceType.compare("WiFi") == 0)
            {
                return new ::WiFiResource(getContext() , protocol, address) ;
            }



            return NULL ;
        }



    }
}
