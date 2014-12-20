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
#include "IContext.h"
#include "Utils.h"
#include <cstring>

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

            this->m_isRefreshTokenExpired = false;

            this->m_name.clear();

            this->m_tokenGrantStatus = -1;

            this->uuidAndTokenMap.clear();

            this->m_myDeviceToken.clear();
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
            if ((domain != DeviceDomain_Local) && (domain != DeviceDomain_Remote) && (domain != DeviceDomain_All)) { return false; }
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

        std::list<std::string>& Device::getSupportedResource()
        {
            return m_supportedResources;
        }

        bool Device::setSupportedResource(const char *supportedResource)
        {
            if ((NULL == supportedResource) || (0 >= strlen(supportedResource))) { return false; }
            m_supportedResources.push_back(supportedResource);
            return true;
        }

        std::string Device::getMyDeviceToken() const
        {
            return m_myDeviceToken;
        }

        bool Device::setMyDeviceToken(const char *token)
        {
            if ((NULL == token) || (0 >= strlen(token))) { return false; }
            m_myDeviceToken = token;
            return true;
        }

        bool Device::setPeerTokenIntoUUIDMap(const char *uuid, const char *token)
        {
            if ((NULL == uuid) || (NULL == token))
            {
                return false;
            }

            if ((0 >= strlen(uuid)) || (0 >= strlen(token)))
            {
                return false;
            }

            if (m_uuid.compare(uuid) == 0)
            {
                m_myDeviceToken = token;
            }
            else
            {
                uuidAndTokenMap[uuid] = token;
            }

            return true;
        }

        std::string Device::getPeerTokenFromUUIDMap(const std::string &uuid)
        {
            std::string peerDeviceToken = "";
            if (m_uuid.compare(uuid) == 0)
            {
                return m_myDeviceToken;
            }
            else
            {
                std::map<std::string, std::string>::iterator itr = uuidAndTokenMap.find(uuid);
                if (itr != uuidAndTokenMap.end())
                {
                    peerDeviceToken = itr->second;
                }
            }

            return peerDeviceToken;
        }

        std::string Device::getMyPeerUUID() const
        {
            return m_peerUUID;
        }

        void Device::setMyPeerIP(const std::string &IP)
        {
            m_myPeerIP = IP;
        }

        std::string Device::getMyPeerIP() const
        {
            return m_myPeerIP;
        }

        void Device::setMyPeerUUID(const std::string &uuid)
        {
            m_peerUUID = uuid;
        }

        bool Device::getDeviceOnlineStatus()
        {
            return m_isOnline;
        }

        void Device::setDeviceOnlineStatus(bool status)
        {
            m_isOnline = status;
        }
    }
}
