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



#ifndef ACCESSPOINT_H_
#define ACCESSPOINT_H_

#include "CMSerialization.h"
#include "list"
#include "string"
#include "IdentifiedObject.h"
#include "String50.h"
#include "String20.h"


/**
*  @addtogroup xsd
*  @{
*/

/**
 * @class   AccessPoint
 *
 * This resource provides an information about access points located near the device.
 * Also, this class defines methods that serialize and/or deserialize contents of XSD complex/list type AccessPoint.
 * @since   1.0.0
 */
class AccessPoint: public ::IdentifiedObject
{

    protected:

        /**
         * Serializes the contents of AccessPoint object using the specified serializer.
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
         * Deserializes data and prepares the AccessPoint object contents.
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
         * Default constructor of AccessPoint class.
         * @since   1.0.0
         */
        AccessPoint();

        /**
         * Default virtual destructor of AccessPoint class.
         * @since   1.0.0
         */
        virtual ~AccessPoint();

        /**
         * This method gets the name of the serializable.
         *
         * @return Returns name of the serializable.
         * @since   1.0.0
         */
        virtual std::string getElementName();

        /**
         * Creates a new AccessPoint object of the same class as this object. It then initializes each of the new object's fields by assigning it with the same value as the corresponding field in this object.
         *
         * @param[out]  pSrcObject  Source AccessPoint object to be cloned.
         *
         * @return  @c  True    if cloning is successful@n
         *          @c  False   if cloning failed
         * @since   1.0.0
         */
        bool clone(::AccessPoint *pSrcObject);

        /**
         * [Mandatory] The channel that the access point is currently using for communications. (e.g., �1�, �5�, �13�)
         */
        uint8_t *mpChannel;

        /**
         * [Mandatory] Service set identifier(SSID) indicates the name of an available wireless network. (e.g., �linksys�, �NETGEAR�, �default�)
         */
        ::String50 *mpSsid;

        /**
         * [Mandatory] The access point's MAC address. (e.g., �00:0B:FC:FF:B2:58�)
         */
        ::String20 *mpMacAddress;

        /**
         * [Mandatory] The maximum rate at which the client adapter can transfer data with an access point. The supported rates of both the client adapter and the access point are examined, and the highest rate that they have in common is the one that is used. (e.g., �11�, �54�, �150�, �300�)
         */
        uint8_t *mpMaxRate;

        /**
         * [Mandatory] The received signal strength indicator(RSSI) is a measure of signal strength in decibels with respect to milliwatts(dBm). (e.g., �-54�, �-74�, �-85�)
         */
        int8_t *mpRssi;

        /**
         * [Optional] Indicate the security type that the Wi-Fi adapter uses. (e.g., �None�, �WEP�, �WPA-PSK�, �WPA2-PSK�)
         */
        ::String20 *mpSecurityType;

        /**
         * [Optional] Indicate the security type that the Wi-Fi adapter uses. (e.g., �None�, �WEP�, �WPA-PSK�, �WPA2-PSK�)
         */
        ::String20 *mpEncryptionType;
};
/** @} */
#endif /* ACCESSPOINT_H_ */
