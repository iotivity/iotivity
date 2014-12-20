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

#if !defined(_DEVICE_H)
#define _DEVICE_H

#include "DeviceDomain.h"
#include "DeviceType.h"
#include "IContext.h"
#include "ISerializable.h"
#include <map>
#include <list>

#include <string>

namespace OC
{
    namespace Cm
    {

        /**
         * @class       Device
         * @brief       This class holds the details of a SHP device
         *              This class is used to set and get properties of a SHP device
         */
        class Device
        {
            public:

                /**
                 * Device - Constructor of this class.
                 *
                 * @param  -type        Represents the type of the SHP device.
                 * @param  -domain  Represents the domain to which this device belongs.
                 *
                 */
                Device(OC::Cm::DeviceType type, OC::Cm::DeviceDomain domain);

                /**
                 * ~Device -Default destructor of this class
                 */
                virtual ~Device()
                {
                }
                ;

                /**
                 * getAddress - Returns address of the device
                 *
                 * @return -string- Address of the device
                 */
                virtual std::string getAddress();

                /**
                 * setAddress sets the address of the device
                 *
                 * @param  addressPointer to the address of the device
                 *
                 * @return  true in case of success
                 *          false in case input parameter is either NULL, empty string or invalid IP pattern.
                 *
                 * @note    If input is domain name then framework will not validate that, just it checks NULLness and EMPTYness.
                 */
                virtual bool setAddress(const char *address);

                /**
                 * getDeviceDescriptionUri returns Device resource Uri
                 *
                 * @return -string- Device resource uri
                 */
                virtual std::string getDeviceDescriptionUri() const;

                /**
                 * getDeviceDescriptionUri sets device resource uri
                 *
                 * @param deviceResourceUri     Pointer to the device resource uri
                 *
                 * @return -bool- true in case of success
                 *                false in case input parameter is either NULL or empty string
                 */
                virtual bool setDeviceDescriptionUri(const char *deviceResourceUri);

                /**
                 * getProtocolreturns the communication protocol supported by device.
                 * Application developer must use this protocol while constructing OC::Cm::Client::Resource::Resource class.
                 * @param -none
                 * @return -string-Communication protocol
                 */
                virtual std::string getProtocol();

                /**
                 * setProtocol sets the communication protocol supported by the device
                 *
                 * @param protocol  Pointer to the Communication protocol
                 *
                 * @return -bool- true in case of success,false in case input parameter is either NULL or empty string
                 */
                virtual bool setProtocol(const char *protocol);

                /**
                 * getDomain returns domain of the device
                 *
                 * @return -DeviceDomain-   Domain of the device
                 */
                virtual OC::Cm::DeviceDomain getDomain() const;

                /**
                 * setDomain sets domain of the device
                 *
                 * @param domain    Domain of the device
                 *
                 * @return -bool- true in case of success @n@c false in case of any failure
                 */
                virtual bool setDomain(OC::Cm::DeviceDomain domain);

                /**
                 * getUUID returns Device UUID
                 *
                 * @return -string- Device UUID
                 */
                virtual std::string getUUID() const;

                /**
                 * This API sets device UUID
                 *
                 * @param[in] uuid  Pointer to the device UUID
                 *
                 * @return Return @c true in case of success @n
                 *                @c false in case input parameter is either NULL or empty string
                 */
                virtual bool setUUID(const char *uuid);

                /**
                 * This API returns the type of the device
                 *
                 * @return Type of device
                 */
                virtual OC::Cm::DeviceType getDeviceType() const;

                /**
                 * This API sets type of device
                 *
                 * @param[in] deviceType Type of device
                 *
                 * @return Return @c true in case of success @n@c false in case of any failure
                 */
                virtual bool setDeviceType(OC::Cm::DeviceType deviceType);

                /**
                 * This API returns the name of the device.
                 *
                 * @return Name of the device
                 */
                virtual std::string getDeviceName();

                /**
                 * This API sets the Device Name
                 *
                 * @param[in] name Pointer to the Device Name
                 *
                 * @return Return @c true in case of success @n
                 *                @c false in case input parameter is either NULL or empty string
                 */
                virtual bool setDeviceName(const char *name);

                /**
                 * This API returns Device description
                 *
                 * @return  Device description
                 */
                virtual std::string getDescription() const;

                /**
                 * This API sets Device description
                 *
                 * @param[in] description Pointer to the device description
                 *
                 * @return Return @c true in case of success @n
                 *                @c false in case input parameter is either NULL or empty string
                 */
                virtual bool setDescription(const char *description);

                /**
                 * This API returns Device manufacturer
                 *
                 * @return Device manufacturer
                 */
                virtual std::string getManufacturer() const;

                /**
                 * This API sets Device manufacturer
                 *
                 * @param[in] manufacturer  Pointer to the device manufacturer
                 *
                 * @return Return @c true in case of success @n
                 *                @c false in case input parameter is either NULL or empty string
                 */
                virtual bool setManufacturer(const char *manufacturer);

                /**
                 * This API returns Device deviceSubType
                 *
                 * @return Device deviceSubType
                 */
                virtual std::string getDeviceSubType() const;

                /**
                 * This API sets Device deviceSubType
                 *
                 * @param[in] deviceSubType Pointer to the device SubType
                 *
                 * @return Return @c true in case of success @n
                 *                @c false in case input parameter is either NULL or empty string
                 */
                virtual bool setDeviceSubType(const char *deviceSubType);

                /**
                 * This API returns Device modelID
                 *
                 * @return  Device modelID
                 */
                virtual std::string getModelID() const;

                /**
                 * This API sets Device modelID
                 *
                 * @param[in] modelID   Pointer to the device modelID
                 *
                 * @return Return @c true in case of success @n
                 *                @c false in case input parameter is either NULL or empty string
                 */
                virtual bool setModelID(const char *modelID);

                /**
                 * This API returns Device serialNumber
                 *
                 * @return  Device serialNumber
                 */
                virtual std::string getSerialNumber() const;
                /**
                 * This API sets Device serialNumber
                 *
                 * @param[in] serialNumber  Pointer to the device serialNumber
                 *
                 * @return Return @c true in case of success @n
                 *                @c false in case input parameter is either NULL or empty string
                 */
                virtual bool setSerialNumber(const char *serialNumber);

                /**
                 * This API returns Device Sales Location
                 *
                 * @return  Device Sales Location
                 */
                virtual std::string getSalesLocation() const;

                /**
                 * setSalesLocation sets Device Sales Location
                 *
                 * @param salesLocation Pointer to the device Sales Location
                 *
                 * @return -bool- true in case of success false in case input parameter is either NULL or empty string
                 */
                virtual bool setSalesLocation(const char *salesLocation);

                /**
                 * getSupportedResource - gets Device Supported Resources
                 * @params -none
                 *
                 * @return  Device Supported Resources List
                 */
                virtual std::list<std::string>& getSupportedResource();

                /**
                 * setSupportedResource sets Device Supported Resource
                 *
                 * @param supportedResource Pointer to the device Supported Resource
                 *
                 * @return -bool- true in case of success, false in case input parameter is either NULL or empty string
                 */
                virtual bool setSupportedResource(const char *supportedResource);

                /**
                 * getSubscriptionsLink returns subscriptions link. This is pure virtual function.
                 *
                 * @param -none
                 *
                 * @return type:string- Subscriptions link
                 */
                virtual std::string getSubscriptionsLink() = 0;

                /**
                 * setContext sets SHP context to which this device belongs.
                 *
                 * @param context Instance SHP context
                 *
                 * @return type:bool  true in case of success @n@c false in case of any failure
                 */
                bool setContext(OC::Cm::IContext *context);

                /**
                 * getContext returns SHP context to which this device belongs.
                 * @param -none
                 *
                 * @return type:IContext* instance of SHP context
                 */
                OC::Cm::IContext *getContext();

                /**
                 * setDeviceDescriptionLoaded sets device description loaded flag. This flag will tells about whether device description is loaded or not.
                 *
                 * @param isLoaded  Device description loaded flag
                 * @return -none
                 */
                void setDeviceDescriptionLoaded(bool isLoaded);

                /**
                 * isDeviceDescriptionLoaded checks whether the current instance of device is already obtained Device description or not.
                 *
                 * @return  type:bool  true if description is already loaded, false if description is not loaded
                 */
                virtual bool isDeviceDescriptionLoaded();

                /**
                 * loadDeviceDescriptionconfigures the current instance of device object with specified device description.
                 *
                 * @param serializable  Device description serializable object
                 *
                 * @return  type:bool true in case of success , false in case of failure
                 */
                virtual bool loadDeviceDescription(
                    OC::Cm::Serialization::ISerializable &serializable) = 0;

                /**
                 *getDeviceDescription returns pointer instance to the serializable version of device description. Pure virtual
                 *
                 * @param serializable  device description (outputs)
                 *
                 * @return  type:bool true in case of success ,false in case of failure
                 */
                virtual bool getDeviceDescription(
                    OC::Cm::Serialization::ISerializable **serializable) = 0;

                /**
                 * getMyDeviceToken returns device token of its own.
                 * @param -none
                 *
                 * @return  type:string Device Token
                 */
                virtual std::string getMyDeviceToken() const;

                /**
                 * setMyDeviceToken sets device token of its own.
                 *
                 * @param token     Pointer to the device Token
                 *
                 * @return type:bool true in case of success , false in case input parameter is either NULL or empty string
                 */
                virtual bool setMyDeviceToken(const char *token);

                /**
                 * getPeerTokenFromUUIDMap returns Peer Device Token From the UUID Map by passing Peer Device UUID
                 * @param -none
                 * @return type:string Peer Device Token
                 */
                virtual std::string getPeerTokenFromUUIDMap(const std::string &uuid);

                /**
                 * setPeerTokenIntoUUIDMap sets Peer Device Token into UUID Map by passing UUID and Token
                 *
                 * @param uuid      Represents Pointer to the Peer Device UUID
                 *
                 * @return type:bool  true in case of success, false in case input parameters is either NULL or empty string
                 */
                virtual bool setPeerTokenIntoUUIDMap(const char *uuid, const char *token);

                /**
                 * setMyPeerIP sets Device IP Address
                 *
                 * @param IP        Device IP Address
                 * @return -none
                 */
                virtual void setMyPeerIP(const std::string &IP);

                /**
                 * getMyPeerIP returns device IP Address.
                 * @param -none
                 *
                 * @return type:string Device IP Address
                 */
                virtual std::string getMyPeerIP() const;

                /**
                 * setMyPeerUUID sets Device UUID.
                 *
                 * @param[in] uuid      Device UUID
                 * @return -none
                 */
                virtual void setMyPeerUUID(const std::string &uuid);

                /**
                 * getMyPeerUUID returns device UUID.
                 *@param -none
                 * @return type:string Device UUID
                 */
                virtual std::string getMyPeerUUID() const;

                /**
                 * getDeviceOnlineStatus returns Device Online Status
                 *
                 * @return type:bool true if Device Status is Online ,false if Device Status is Offline
                 */
                virtual bool getDeviceOnlineStatus();

                /**
                 * setDeviceOnlineStatus sets device to Online
                 *
                 * @param status    Device Online Status
                 * @return -none
                 */
                virtual void setDeviceOnlineStatus(bool status);

            protected:

                bool m_device_description_loaded; /**< Represents whether device description is loaded or not */

            private:
                IContext *p_context; /**< Represents the instance of SHP Context */
                std::list<std::string> m_supportedResources; /**< Represents the list of supported resources */
                std::string m_address; /**< Represents the device IP address */
                std::string m_deviceResourceURI; /**< Represents the device resource URI */
                std::string m_protocol; /**< Represents the device protocol */
                OC::Cm::DeviceDomain m_domain; /**< Represents the device domain */
                std::string m_uuid; /**< Represents the device domain which it belogs */
                OC::Cm::DeviceType m_deviceType; /**< Represents the device type */
                std::string m_name; /**< Represents the device name */
                std::string m_description; /**< Represents the device description */
                std::string m_manufacturer; /**< Represents the device manufacturer name */
                std::string m_deviceSubType; /**< Represents the device sub type */
                std::string m_modelID; /**< Represents the device model ID */
                std::string m_serialNumber; /**< Represents the device serial number */
                std::string m_salesLocation; /**< Represents the device sales location */
                bool m_isRefreshTokenExpired; /**< Represents the refresh token expire status */
                bool m_isOnline; /**< Represents the device online status */
                /**
                 * Represents the grant status of the token which user choose@n
                 * @c  0    In case user denied token request@n
                 * @c  1    In case user granted token request
                 */
                int m_tokenGrantStatus;/**< Represents if a token is granted or not */
                std::string m_myDeviceToken; /**< Represents the device token of its own */
                std::string m_peerUUID; /**< Represents the peer device UUID */
                std::string m_myPeerIP; /**< Represents the peer device IP address */
                std::map<std::string, std::string> uuidAndTokenMap; /**< Represents the token-UUID map */
        };
    }
}

#endif  //_DEVICE_H
