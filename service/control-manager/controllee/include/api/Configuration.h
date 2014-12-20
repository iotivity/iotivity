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

#if !defined(_CONFIGURATION_H)
#define _CONFIGURATION_H

namespace OC
{
    namespace Cm
    {
        namespace DeviceFinder
        {
            class DeviceFactory;
        }

        namespace Server
        {
            namespace ResourceHandler
            {
                class ResourceHandlerFactory;
            }
        }

        namespace Serialization
        {
            class ISerializable;
            class ISerializer;
            class IDeserializer;
            class SerializableDataFactory;
        }
    }
}

#include "string"
#include "map"
#include "list"
#include "ApplicationType.h"

namespace OC
{
    namespace Cm
    {
        /**
         * @class       Configuration
         * @brief       This class holds the details required to configure SHP. SHP should be configured properly before starting it on a device.
         */
        class Configuration
        {

            public:

                /**
                 * Configuration - Default constructor of this class. It initializes all Configuration class variables.
                 * @param - none
                 * @return - none
                 */
                Configuration();

                /**
                 * ~Configuration - Default destructor of this class. It de-initializes all Configuration class variables.
                 * @param - none
                 * @return - none
                 */
                virtual ~Configuration();

                /**
                 * reset - This API resets the current instance of configuration to default values
                 * @param - none
                 * @return - none
                 */
                void reset();

                /**
                 * setDeviceFactory - This API configures a instance of DeviceFactory. This interface defines methods required to instantiate a Device Object.
                 *
                 * @param deviceFactory Represents the instance of DeviceFactory
                 * @return- bool- true in case of success @n@c false in case of any failure
                 */
                bool setDeviceFactory(OC::Cm::DeviceFinder::DeviceFactory *deviceFactory);

                /**
                 * setSerializableDataFactory - This API configures a instance of SerializableDataFactory. This interface defines methods required to instantiate a serializable.
                 *
                 * @param serializableDataFactory Represents the instance of SerializableDataFactory
                 * @return -bool- true in case of success @n@c false in case of any failure
                 */
                bool setSerializableDataFactory(
                    OC::Cm::Serialization::SerializableDataFactory *serializableDataFactory);

                /**
                 * setResourceHandlerFactory - Configures a instance of ResourceHandlerFactory. This interface defines methods required to instantiate ResourceHandler objects
                 *
                 * @param handlerFactory Represents the instance of ResourceHandlerFactory
                 * @return -bool- true in case of success @n@c false in case of any failure
                 */
                bool setResourceHandlerFactory(
                    OC::Cm::Server::ResourceHandler::ResourceHandlerFactory *handlerFactory);

                /**
                 * addDataFormat - This API adds serializer and deserializer for particular content type
                 *
                 * @param contentType   Represents content type which handles by the Serializer and Deserializer
                 * @param serializer    Represents the instance of ISerializer
                 * @param deserializer  Represents the instance of IDeserializer
                 * @return - none
                 */
                void addDataFormat(std::string contentType,
                                   OC::Cm::Serialization::ISerializer &serializer,
                                   OC::Cm::Serialization::IDeserializer &deserializer);

                /**
                 * setAppType - Configures the type of application.(Controllee).
                 *
                 * @param appType Represents the type of SHP application.
                 * @return -bool- true in case of success @n@c false in case of any failure
                 */
                bool setAppType(OC::Cm::ApplicationType appType);

                /**
                 * setSubscriptionDbPath - Configures database file path. Framework will create a database to store subscription details.
                 * Developer can use this API to set the path where they want to store the subscription database.
                 *
                 * @param dbPath Represents the pointer to the subscription database file path
                 *
                 * @return -bool- true in case of success
                 *                false in case input parameter is either NULL or empty string
                 */
                bool setSubscriptionDbPath(const char *dbPath);

                /**
                 * setOSName - Configures Operating System's Name. Framework will use this string in SSDP packets.
                 * SHP supports various like Windows, Linux, Android, Mac, iOS, Tizen etc. for both 32 bit and 64 bit architecture.
                 * Developer can use this API to set the Operating System where they want to use SHP.
                 *
                 * @param osname Represents the name of the operating system
                 *
                 * @return -bool- true in case of success
                 *                false in case input parameter is either NULL or empty string
                 */
                bool setOSName(const char *osname);

                /**
                 * setApplicationName -Configures Application Name. Framework will use this string in ssdp packets
                 *
                 * @param applicationName Represents the pointer to the name of the application
                 *
                 * @return -bool- true in case of success
                 *                false in case input parameter is either NULL or empty string
                 */
                bool setApplicationName(const char *applicationName);

                /**
                 * getOSName -  returns name of the OS configured by the user
                 *
                 * @return -string- name of the operating system
                 */
                std::string getOSName();

                /**
                 * getApplicationName returns name of the application configured by the user
                 *
                 * @return -string- Name of the SHP application
                 */
                std::string getApplicationName();

                /**
                 * getDeSerializers returns list of deserializers configured by the application user
                 *
                 * @return std::map<std::string, OC::Cm::Serialization::IDeserializer*>& List of deserializers
                 */
                const std::map<std::string, OC::Cm::Serialization::IDeserializer *>& getDeSerializers() const;

                /**
                 * getDeviceFactory returns the instance of device factory class configured by application user
                 *
                 * @return- OC::Cm::DeviceFinder::DeviceFactory*  -Instance of device factory class
                 */
                OC::Cm::DeviceFinder::DeviceFactory *getDeviceFactory() const;

                /**
                 * getResourceHandlerFactory returns instance of resource handler factory class configured by application user
                 *
                 * @return -ResourceHandlerFactory*- Instance of resource handler factory class
                 */
                OC::Cm::Server::ResourceHandler::ResourceHandlerFactory *getResourceHandlerFactory() const;

                /**
                 * getSerializableDataFactory returns instance of serializable data factory class configured by application user
                 *
                 * @return -SerializableDataFactory*- Instance of serializable data factory class
                 */
                OC::Cm::Serialization::SerializableDataFactory *getSerializableDataFactory() const;

                /**
                 * getSerializers returns map of serializers configured by the application user
                 *
                 * @return -map<std::string, OC::Cm::Serialization::ISerializer*>& -Map of serializers
                 */
                std::map<std::string, OC::Cm::Serialization::ISerializer *>& getSerializers();

                /**
                 * getAppType returns type of SHP application
                 *
                 * @return Type of SHP application
                 */
                OC::Cm::ApplicationType getAppType() const;

                /**
                 * getSubscriptionDbPath returns subscription database file path
                 *
                 * @return - string& - Subscription database file path
                 */
                std::string &getSubscriptionDbPath();

                /**
                 * getDeviceIpAddress returns IP Address of the device running SHP.
                 *
                 * @return -string& -IP Address of the device running SHP.
                 */
                std::string &getDeviceIpAddress();

                /**
                 * setDeviceIpAddress sets IP Address of the device running SHP.
                 *
                 * @param ipAddress Pointer to the IP Address of the device running SHP
                 *
                 * @return -bool- true in case of success
                 *                false in case input parameter is either NULL, empty string or invalid IP pattern.
                 *
                 * @note    If input is domain name then framework will not validate that, just it checks NULLness and EMPTYness.
                 */
                bool setDeviceIpAddress(const char *ipAddress);

            private:

                OC::Cm::DeviceFinder::DeviceFactory *m_deviceFactory; /** Represents DeviceFactory class instance which has set by the application. */
                OC::Cm::Server::ResourceHandler::ResourceHandlerFactory *m_resourceHandlerFactory; /** Represents ResourceHandlerFactory class instance which has set by the application. */
                OC::Cm::Serialization::SerializableDataFactory *m_serializableDataFactory; /** Represents SerializableDataFactory class instance which has set by the application */
                std::map<std::string, OC::Cm::Serialization::ISerializer *> m_serializers; /** Represents map of ISerializer class instances which are set by the application */
                std::map<std::string, OC::Cm::Serialization::IDeserializer *> m_deSerializers; /** Represents map of IDeserializer class instances which are set by the application */
                OC::Cm::ApplicationType m_app_type; /** Represents SHP Application Type */
                std::string m_subscriptionDbPath; /** Represents the subscription DB path */
                std::string m_osName; /** Represents OS Name which is used by SSDP */
                std::string m_applicationName; /** Represents Application Name which is used by SSDP */
                std::string m_ipAddress; /** Represents device IP address */
                bool m_isRemoteAccessEnabled; /** Represents whether framework supports remote access or not, this will be set by the application */
                std::string m_raConfigPath; /** Represents the remote access configuration file path which is set the application */
                std::string m_raPrivateKey; /** Represents the private key (UUID) for encrypting/decrypting the remote access configuration file */
                OC::Cm::Serialization::ISerializable *m_pWifiData; /** Represents the WIFI resource representation which is set by the Controller application */
        };
    }
}

#endif  //_CONFIGURATION_H
