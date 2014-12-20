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


#include "CMConfig.h"
#include "DeviceDomain.h"
#include "DeviceType.h"
#include "CMSerialization.h"
#include "CMClientResource.h"
#include "xsd/Elements.h"

#include "string"
#include <map>
#include <list>

namespace OC
{
    namespace Cm
    {

        /**
        *  @addtogroup ControlManager
        *  @{
        */

        /**
         * @class       Device
         * @brief       This class holds the details of a Control Manager device
         *              This class is used to set and get properties of a Control Manager device
         */
        class CMAPI Device
        {
            public:

                /**
                 * Constructor of this class.
                 *
                 * @param[in] type      Represents the type of the Control Manager device.
                 * @param[in] domain    Represents the domain to which this device belongs.
                 *
                 */
                Device(OC::Cm::DeviceType type, OC::Cm::DeviceDomain domain);

                /**
                 * Default destructor of this class
                 */
                virtual ~Device() {};

                /**
                 * This API returns address of the device
                 *
                 * @return Address of the device
                 */
                virtual std::string getAddress() ;

                /**
                 * This API sets the address of the device
                 *
                 * @param[in] address   Pointer to the address of the device
                 *
                 * @return Return @c true in case of success @n
                 *                @c false in case input parameter is either NULL, empty string or invalid IP pattern.
                 *
                 * @note    If input is domain name then framework will not validate that, just it checks NULLness and EMPTYness.
                 */
                virtual bool setAddress(const char *address);

                /**
                 * This API returns Device resource Uri
                 *
                 * @return Device resource uri
                 */
                virtual std::string getDeviceDescriptionUri() const;

                /**
                 * This API sets device resource uri
                 *
                 * @param[in] deviceResourceUri     Pointer to the device resource uri
                 *
                 * @return Return @c true in case of success @n
                 *                @c false in case input parameter is either NULL or empty string
                 */
                virtual bool setDeviceDescriptionUri(const char *deviceResourceUri);

                /**
                 * This API returns the communication protocol supported by device.
                 * Application developer must use this protocol while constructing OC::Cm::Client::Resource::Resource class.
                 *
                 * @return Communication protocol
                 */
                virtual std::string getProtocol() ;

                /**
                 * This API sets the communication protocol supported by the device
                 *
                 * @param[in] protocol  Pointer to the Communication protocol
                 *
                 * @return Return @c true in case of success @n
                 *                @c false in case input parameter is either NULL or empty string
                 */
                virtual bool setProtocol(const char *protocol);

                /**
                 * This API returns domain of the device
                 *
                 * @return  Domain of the device
                 */
                virtual OC::Cm::DeviceDomain getDomain() const;

                /**
                 * This API sets domain of the device
                 *
                 * @param[in] domain    Domain of the device
                 *
                 * @return Return @c true in case of success @n@c false in case of any failure
                 */
                virtual bool setDomain(OC::Cm::DeviceDomain domain);

                /**
                 * This API returns Device UUID
                 *
                 * @return Device UUID
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
                virtual std::string getDeviceName() ;

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
                 * This API sets Device Sales Location
                 *
                 * @param[in] salesLocation Pointer to the device Sales Location
                 *
                 * @return Return @c true in case of success @n
                 *                @c false in case input parameter is either NULL or empty string
                 */
                virtual bool setSalesLocation(const char *salesLocation);

                /**
                 * gets Device Supported Resources
                 *
                 * @return  Device Supported Resources List
                 */
                virtual std::list<std::string> &getSupportedResource();

                /**
                 * This API sets Device Supported Resource
                 *
                 * @param[in] supportedResource Pointer to the device Supported Resource
                 *
                 * @return Return @c true in case of success @n
                 *                @c false in case input parameter is either NULL or empty string
                 */
                virtual bool setSupportedResource(const char *supportedResource);

                /**
                 * This API returns subscriptions link. This is pure virtual function, so that any Control Manager device class
                 * which inherits by this class has to implement this method.
                 *
                 * @return Subscriptions link
                 */
                virtual std::string getSubscriptionsLink() ;

                /**
                 * This API sets Control Manager context to which this device belongs.
                 *
                 * @param[in] context Instance Control Manager context
                 *
                 * @return Return @c true in case of success @n@c false in case of any failure
                 */
                bool setContext(OC::Cm::IContext *context) ;

                /**
                 * This API returns Control Manager context to which this device belongs.
                 *
                 * @return instance of Control Manager context
                 */
                OC::Cm::IContext *getContext() ;

                /**
                 * This API sets device description loaded flag. This flag will tells about whether device description is loaded or not.
                 *
                  * @param[in] isLoaded Device description loaded flag
                 */
                void setDeviceDescriptionLoaded(bool isLoaded);

                /**
                 * This API checks whether the current instance of device is already obtained Device description or not.
                 *
                 * @return  returns @c true if description is already loaded @n
                 *                  @c false if description is not loaded
                 */
                virtual bool isDeviceDescriptionLoaded();

                /**
                 * This API configures the current instance of device object with specified device description.
                 *
                 * @param[in] serializable  Device description serializable object
                 *
                 * @return  returns @c true in case of success @n
                 *                  @c false in case of failure
                 */
                virtual bool loadDeviceDescription( OC::Cm::Serialization::ISerializable &serializable ) ;

                /**
                 * This API returns Device Online Status
                 *
                 * @return  @c true if Device Status is Online @n
                 *          @c false if Device Status is Offline
                 */
                virtual bool getDeviceOnlineStatus();

                /**
                 * This API sets Device Online Status
                 *
                 * @param[in] status    Device Online Status
                 */
                virtual void setDeviceOnlineStatus(bool status);

                /**
                 * Capability class variable
                 */
                ::Capability m_capabilities ;

                ::Devices m_devices ;

                /**
                 * METHOD CURRENTLY NOT USED. PLEASE DO NOT USE.
                 */
                OC::Cm::Client::Resource::Resource *getResource(std::string resourceType);


            protected:

                bool    m_device_description_loaded;                /**< Represents whether device description is loaded or not */

            private:
                IContext               *p_context
                ;                 /**< Represents the instance of Control Manager Context */

                std::list <std::string>
                m_supportedResources;       /**< Represents the list of supported resources */

                std::string             m_address;                  /**< Represents the device IP address */

                std::string             m_deviceResourceURI;        /**< Represents the device resource URI */

                std::string             m_protocol;                 /**< Represents the device protocol */

                OC::Cm::DeviceDomain    m_domain;                   /**< Represents the device domain */

                std::string
                m_uuid;                     /**< Represents the device domain which it belogs */

                OC::Cm::DeviceType  m_deviceType;               /**< Represents the device type */

                std::string             m_name;                     /**< Represents the device name */

                std::string             m_description;              /**< Represents the device description */

                std::string             m_manufacturer;             /**< Represents the device manufacturer name */

                std::string             m_deviceSubType;            /**< Represents the device sub type */

                std::string             m_modelID;                  /**< Represents the device model ID */

                std::string             m_serialNumber;             /**< Represents the device serial number */

                std::string             m_salesLocation;            /**< Represents the device sales location */

                bool                    m_isOnline;                 /**< Represents the device online status */

                OC::Cm::Client::Resource::Resource *createResource(std::string &resourceType);

                /**
                 * String variable that contains SubscriptionLink information.
                 */
                std::string m_subscriptionsLink ;

        };
        /** @} */
    }
}

#endif  //_DEVICE_H
