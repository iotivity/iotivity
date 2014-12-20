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



#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include "CMSerialization.h"
#include "list"
#include "string"
#include "Resource.h"
#include "InternalLocationType.h"
#include "String20.h"
#include "CurrencyType.h"
#include "OnType.h"
#include "ArrayOfString.h"
#include "DeviceType.h"
#include "Networks.h"
#include "Time.h"
#include "Remote.h"


/**
*  @addtogroup xsd
*  @{
*/

/**
 * @class   Configuration
 *
 * This resource provides a configuration functions to configure the network interfaces of device and set system time properties.
 * Also, this class defines methods that serialize and/or deserialize contents of XSD complex/list type Configuration.
 * @since   1.0.0
 */

class Configuration: public ::Resource
{

    protected:

        /**
         * Serializes the contents of Configuration object using the specified serializer.
         * This function validates the values of each member variable and returns false if any of the member contains invalid value.
         * This function returns false, if one of the mandatory parameter is not set or one of the attribute is invalid.
         *
         * @param[out]  serializer  Serializer object that provides methods to serialize.
         *
         * @return  @c  True    if serialization is successful@n
         *          @c  False   if serialization failed
         * @since   1.0.0
         */
        virtual bool serializeContents(OC::Cm::Serialization::ISerializer &serializer);

        /**
         * Deserializes data and prepares the Configuration object contents.
         * After parsing the values, this function validates the values.
         * This function returns false, if one of the mandatory parameter is not present or one of the attribute is invalid.
         *
         * @param[out]  deserializer    Deserializer object that provides methods to deserialize.
         *
         * @return  @c  True    if deserialization is successful@n
         *          @c  False   if deserialization failed
         * @since   1.0.0
         */
        virtual bool deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer);

    public:

        /**
         * Default constructor of Configuration class.
         * @since   1.0.0
         */
        Configuration();

        /**
         * Default virtual destructor of Configuration class.
         * @since   1.0.0
         */
        virtual ~Configuration();

        /**
         * This method gets the name of the serializable.
         *
         * @return Returns name of the serializable.
         * @since   1.0.0
         */
        virtual std::string getElementName();

        /**
         * Creates a new Configuration object of the same class as this object. It then initializes each of the new object's fields by assigning it with the same value as the corresponding field in this object.
         *
         * @param[out]  pSrcObject  Source Configuration object to be cloned.
         *
         * @return  @c  True    if cloning is successful@n
         *          @c  False   if cloning failed
         * @since   1.0.0
         */
        bool clone(::Configuration *pSrcObject);

        /**
         * [Optional] Indicate installed location of the device. Note that declaration of the internal location enumeration type can be found in the "Attribute Type" declaration. (e.g., �Living_Room�, �Bed_Room�, �Dining_Room�, �Kitchen�, �Lobby�)
         */
        ::InternalLocationType *mpInstalledLocation;

        /**
         * [Optional] Indicate the current region where the device is located. This attribute is used to retrieve the weather information from weather information provider.
         */
        ::String20 *mpRegion;

        /**
         * [Optional] The currency type indicating the currency. Normally gateway device exposes this attribute. Note that declaration of a currency enumeration type can be found in the "Attribute Type" declaration. (e.g., �US_Dollar�, �Korean_Won�)
         */
        ::CurrencyType *mpCurrency;

        /**
         * [Optional] Indicate whether the device can be controlled remotely. (e.g., �true�, �false�)
         */
        bool *mpRemoteControlEnabled;

        /**
         * [Optional] Indicate whether the pairing mode of bridge device is started or stopped (e.g., �On�, �Off�). This attribute will be used by ZigBee/Z-Wave Bridge device to start pairing with Non-IP ZigBee/Z-Wave devices.
         */
        ::OnType *mpPairingStart;

        /**
         * [Optional] Indicate whether the unpairing mode of bridge device is started or stopped. (e.g., �On�, �Off�). This attribute will be used by Z-Wave Bridge device to start unpairing with Non-IP Z-Wave devices.
         */
        ::OnType *mpUnpairingStart;

        /**
         * [Optional] Indicate the timeout of pairing mode ZigBee/Z-Wave Bridge device. After timeout, the bridge device will stop the pairing mode.
         */
        uint16_t *mpPairingTimeout;

        /**
         * [Optional] Relationships between 'Device' objects are described by linking 'device id's together using this attribute that used to provide logical association information. For example, if two devices are being linked by using this attribute, depending on the perspective of the user or implementor, two devices that have a different 'deviceID's are possible to treat as a single device. Note that prioritize between devices is entirely left to the user or implementor's choice. Also, it's allowed to create multiple relations for a single 'Device' object. However, to prevent excessive increase in complexity, nested relations should be ignored. Available in both the singular and plural forms expression. In case of plural forms, each option can be expressed in array of string.
         */
        ::ArrayOfString *mpSubDeviceIDs;

        /**
         * [Optional] Indicate whether this device is sub-device or not.
         */
        bool *mpIsSubDevice;

        /**
         * [Optional] Indicate device type which legacy device is expressed in user interface. For example, when smart plug is connected to legacy TV device without connectivity, connectedDeviceType attribute of smart plug device is set to �TV�.
         */
        ::DeviceType *mpConnectedDeviceType;

        /**
         * [Optional] Variable of Networks object.
         */
        ::Networks *mpNetworks;

        /**
         * [Optional] Variable of Time object.
         */
        ::Time *mpTime;

        /**
         * [Optional] Variable of Remote object.
         */
        ::Remote *mpRemote;
};
/** @} */
#endif /* CONFIGURATION_H_ */
