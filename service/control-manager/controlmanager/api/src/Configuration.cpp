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



#include "Configuration.h"
#include "Log.h"

namespace OC
{
    namespace Cm
    {


        Configuration::Configuration()
        {
            m_serializableDataFactory   = NULL ;

            m_resourceHandlerFactory    = NULL ;

            m_deviceFactory             = NULL ;

            m_app_type                  = ApplicationType_Controller ;

            m_raConfigPath              = "";

            m_raPrivateKey              = "";

            m_isRemoteAccessEnabled     = false;

            m_pWifiData                 = NULL;
        }

        Configuration::~Configuration()
        {
            this->reset() ;
        }

        void
        Configuration::reset()
        {
            m_serializableDataFactory   = NULL ;

            m_resourceHandlerFactory    = NULL ;

            m_deviceFactory             = NULL ;

            m_app_type                  = ApplicationType_Controller ;

            m_serializers.clear() ;

            m_deSerializers.clear() ;
        }

        bool
        Configuration::setDeviceFactory(OC::Cm::DeviceFinder::DeviceFactory *deviceFactory)
        {
            if (deviceFactory == NULL) { return false; }
            m_deviceFactory = deviceFactory ;
            return true;
        }

        bool
        Configuration::setSerializableDataFactory(OC::Cm::Serialization::SerializableDataFactory
                *serializableDataFactory)
        {
            if (serializableDataFactory == NULL) { return false; }
            m_serializableDataFactory = serializableDataFactory ;
            return true;
        }

        bool
        Configuration::setResourceHandlerFactory(OC::Cm::Server::ResourceHandler::ResourceHandlerFactory
                *handlerFactory)
        {
            if (handlerFactory == NULL) { return false; }
            m_resourceHandlerFactory = handlerFactory ;
            return true;
        }


        const std::map<std::string, OC::Cm::Serialization::IDeserializer *> &
        Configuration::getDeSerializers() const
        {
            return m_deSerializers;
        }

        OC::Cm::DeviceFinder::DeviceFactory *
        Configuration::getDeviceFactory() const
        {
            return m_deviceFactory;
        }

        OC::Cm::Server::ResourceHandler::ResourceHandlerFactory *
        Configuration::getResourceHandlerFactory() const
        {
            return m_resourceHandlerFactory;
        }

        OC::Cm::Serialization::SerializableDataFactory *
        Configuration::getSerializableDataFactory() const
        {
            return m_serializableDataFactory;
        }

        std::map<std::string, OC::Cm::Serialization::ISerializer *> &
        Configuration::getSerializers()
        {
            return m_serializers;
        }

        OC::Cm::ApplicationType
        Configuration::getAppType() const
        {
            return m_app_type;
        }

        bool
        Configuration::setAppType(OC::Cm::ApplicationType appType)
        {
            if ((appType != ApplicationType_Controller) && (appType != ApplicationType_Controllable)
                && (appType != ApplicationType_Both))
            {
                return false;
            }
            m_app_type = appType;
            return true;
        }

        std::string &
        Configuration::getSubscriptionDbPath()
        {
            return m_subscriptionDbPath ;
        }

        bool
        Configuration::setSubscriptionDbPath(const char *dbPath)
        {
            if ((NULL == dbPath) || (0 >= strlen(dbPath))) { return false; }
            m_subscriptionDbPath = dbPath ;
            return true;
        }

        bool
        Configuration::setOSName(const char *osName)
        {
            if ((NULL == osName) || (0 >= strlen(osName))) { return false; }
            this->m_osName = osName;
            return true;
        }

        bool
        Configuration::setApplicationName(const char *applicationName)
        {
            if ((NULL == applicationName) || (0 >= strlen(applicationName))) { return false; }
            this->m_applicationName = applicationName;
            return true;
        }

        std::string Configuration::getOSName()
        {
            return m_osName;
        }

        std::string Configuration::getApplicationName()
        {
            return m_applicationName;
        }


        std::string &Configuration::getDeviceIpAddress()
        {
            return m_ipAddress;
        }

        bool Configuration::setDeviceIpAddress(const char *ipAddress)
        {
            if ((NULL == ipAddress) || (0 >= strlen(ipAddress))) { return false; }
            m_ipAddress = ipAddress;
            return true;
        }

    }
}
