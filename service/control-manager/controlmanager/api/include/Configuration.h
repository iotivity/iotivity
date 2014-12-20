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

#include "CMConfig.h"

namespace OC
{
    namespace Cm
    {
        namespace DeviceFinder
        {
            class DeviceFactory ;
        }

        namespace Server
        {
            namespace ResourceHandler
            {
                class ResourceHandlerFactory ;
            }
        }

        namespace Serialization
        {
            class ISerializable ;
            class ISerializer ;
            class IDeserializer ;
            class SerializableDataFactory;
        }

        namespace Connector
        {
            namespace Server
            {
                class IServerConnector ;
            }
            namespace Client
            {
                class IClientConnector ;
            }
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
        *  @addtogroup ControlManager
        *  @{
        */
        /**
         * @class       Configuration
         * @brief       This class holds the details required to configure Control Manager. Control Manager should be configured properly before starting it on a device.
         */
        class CMAPI Configuration
        {

            public:

                /**
                 * Default constructor of this class. It initializes all Configuration class variables.
                 */
                Configuration();

                /**
                 * Default destructor of this class. It de-initializes all Configuration class variables.
                 */
                virtual ~Configuration();

                /**
                 * This API resets the current instance of configuration to default values
                 */
                void reset() ;

                /**
                 * This API configures a instance of DeviceFactory. This interface defines methods required to instantiate a Device Object.
                 *
                 * @param[in] deviceFactory Represents the instance of DeviceFactory
                 *
                 * @return Return @c true in case of success @n@c false in case of any failure
                 */
                bool setDeviceFactory(OC::Cm::DeviceFinder::DeviceFactory *deviceFactory);


                /**
                 * This API configures database file path. Framework will create a database to store subscription details.
                 * Developer can use this API to set the path where they want to store the subscription database.
                 *
                 * @param[in] dbPath Represents the pointer to the subscription database file path
                 *
                 * @return Return @c true in case of success @n
                 *                @c false in case input parameter is either NULL or empty string
                 */
                bool setSubscriptionDbPath(const char *dbPath) ;

                /**
                 * This API configures Operating System's Name. Framework will use this string in SSDP packets.
                 * Control Manager supports various like Windows, Linux, Android, Mac, iOS, Tizen etc. for both 32 bit and 64 bit architecture.
                 * Developer can use this API to set the Operating System where they want to use Control Manager.
                 *
                 * @param[in] osname Represents the name of the operating system
                 *
                 * @return Return @c true in case of success @n
                 *                @c false in case input parameter is either NULL or empty string
                 */
                bool setOSName(const char *osname) ;

                /**
                 * This API configures Application Name. Framework will use this string in ssdp packets
                 *
                 * @param[in] applicationName Represents the pointer to the name of the application
                 *
                 * @return Return @c true in case of success @n
                 *                @c false in case input parameter is either NULL or empty string
                 */
                bool setApplicationName(const char *applicationName) ;

                /**
                 * This API returns name of the OS configured by the user
                 *
                 * @return name of the operating system
                 */
                std::string getOSName() ;

                /**
                 * This API returns name of the application configured by the user
                 *
                 * @return Name of the Control Manager application
                 */
                std::string getApplicationName() ;

                /**
                 * This API returns type of Control Manager application
                 *
                 * @return Type of Control Manager application
                 */
                OC::Cm::ApplicationType getAppType() const;

                /**
                 * This API returns list of deserializers configured by the application user
                 *
                 * @return List of deserializers
                 */
                const std::map<std::string, OC::Cm::Serialization::IDeserializer *> &getDeSerializers() const;

                /**
                 * This API returns the instance of device factory class configured by application user
                 *
                 * @return Instance of device factory class
                 */
                OC::Cm::DeviceFinder::DeviceFactory *getDeviceFactory() const;

                /**
                 * This API returns instance of resource handler factory class configured by application user
                 *
                 * @return Instance of resource handler factory class
                 */
                OC::Cm::Server::ResourceHandler::ResourceHandlerFactory *getResourceHandlerFactory() const;

                /**
                 * This API returns instance of serializable data factory class configured by application user
                 *
                 * @return Instance of serializable data factory class
                 */
                OC::Cm::Serialization::SerializableDataFactory *getSerializableDataFactory() const;

                /**
                 * This API returns map of serializers configured by the application user
                 *
                 * @return Map of serializers
                 */
                std::map<std::string, OC::Cm::Serialization::ISerializer *> &getSerializers();


                /**
                 * This API returns subscription database file path
                 *
                 * @return Subscription database file path
                 */
                std::string &getSubscriptionDbPath() ;

                /**
                 * This API returns IP Address of the device running Control Manager.
                 *
                 * @return IP Address of the device running Control Manager.
                 */
                std::string &getDeviceIpAddress();

                /**
                 * This API sets IP Address of the device running Control Manager.
                 *
                 * @param[in] ipAddress Pointer to the IP Address of the device running Control Manager
                 *
                 * @return Return @c true in case of success @n
                 *                @c false in case input parameter is either NULL, empty string or invalid IP pattern.
                 *
                 * @note    If input is domain name then framework will not validate that, just it checks NULLness and EMPTYness.
                 */
                bool setDeviceIpAddress(const char *ipAddress);



                bool setSerializableDataFactory(OC::Cm::Serialization::SerializableDataFactory
                                                *serializableDataFactory);


                bool setResourceHandlerFactory(OC::Cm::Server::ResourceHandler::ResourceHandlerFactory
                                               *handlerFactory);


            private:


                /**
                 * This API configures the type of application. Application can be either Controller, Controllable or having both functionalities.
                 *
                 * @param[in] appType Represents the type of Control Manager application.
                 *
                 * @return Return @c true in case of success @n@c false in case of any failure
                 */
                bool setAppType(OC::Cm::ApplicationType appType);



                OC::Cm::DeviceFinder::DeviceFactory
                *m_deviceFactory;                               /** Represents DeviceFactory class instance which has set by the application. */

                OC::Cm::Server::ResourceHandler::ResourceHandlerFactory
                *m_resourceHandlerFactory;  /** Represents ResourceHandlerFactory class instance which has set by the application. */

                OC::Cm::Serialization::SerializableDataFactory
                *m_serializableDataFactory;          /** Represents SerializableDataFactory class instance which has set by the application */

                std::map<std::string, OC::Cm::Serialization::ISerializer *>
                m_serializers;              /** Represents map of ISerializer class instances which are set by the application */

                std::map<std::string, OC::Cm::Serialization::IDeserializer *>
                m_deSerializers;          /** Represents map of IDeserializer class instances which are set by the application */

                OC::Cm::ApplicationType
                m_app_type;                                             /** Represents Control Manager Application Type */

                std::string                     m_subscriptionDbPath
                ;                                  /** Represents the subscription DB path */

                std::string
                m_osName;                                               /** Represents OS Name which is used by SSDP */

                std::string
                m_applicationName;                                      /** Represents Application Name which is used by SSDP */

                std::string
                m_ipAddress;                                            /** Represents device IP address */

                bool
                m_isRemoteAccessEnabled;                                /** Represents whether framework supports remote access or not, this will be set by the application */

                std::string                     m_raConfigPath
                ;                                        /** Represents the remote access configuration file path which is set the application */

                std::string                     m_raPrivateKey
                ;                                        /** Represents the private key (UUID) for encrypting/decrypting the remote access configuration file */

                OC::Cm::Serialization::ISerializable
                *m_pWifiData;                                  /** Represents the WIFI resource representation which is set by the Controller application */
        };
        /** @} */
    }
}

#endif  //_CONFIGURATION_H
