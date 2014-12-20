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



#ifndef WIFI_H_
#define WIFI_H_

#include "CMSerialization.h"
#include "list"
#include "string"
#include "Resource.h"
#include "WifiOperationType.h"
#include "String50.h"
#include "String20.h"
#include "String100.h"
#include "WifiEventType.h"
#include "AccessPointsLink.h"


/**
*  @addtogroup xsd
*  @{
*/


/**
 * @class   WiFi
 *
 * This resource provides an information of the WiFi connectivity.
 * Also, this class defines methods that serialize and/or deserialize contents of XSD complex/list type WiFi.
 * @since   1.0.0
 */

class WiFi: public ::Resource
{

    protected:

        /**
         * Serializes the contents of WiFi object using the specified serializer.
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
         * Deserializes data and prepares the WiFi object contents.
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
         * Default constructor of WiFi class.
         * @since   1.0.0
         */
        WiFi();

        /**
         * Default virtual destructor of WiFi class.
         * @since   1.0.0
         */
        virtual ~WiFi();

        /**
         * This method gets the name of the serializable.
         *
         * @return Returns name of the serializable.
         * @since   1.0.0
         */
        virtual std::string getElementName();

        /**
         * Creates a new WiFi object of the same class as this object. It then initializes each of the new object's fields by assigning it with the same value as the corresponding field in this object.
         *
         * @param[out]  pSrcObject  Source WiFi object to be cloned.
         *
         * @return  @c  True    if cloning is successful@n
         *          @c  False   if cloning failed
         * @since   1.0.0
         */
        bool clone(::WiFi *pSrcObject);

        /**
         * [Optional] Defines the role of the Wi-Fi adapter. Note that declaration of the Wi-Fi operation enumeration type can be found in the "Attribute Type" declaration. (e.g., �Infrastructure�, �Soft AP�, �WiFi Direct�)
         */
        ::WifiOperationType *mpOperationType;

        /**
         * [Optional] The channel that the Wi-Fi adapter is currently using for communications. (e.g., �1�, �5�, �13�)
         */
        uint8_t *mpChannel;

        /**
         * [Optional] The SSID to connect the Wi-Fi network and it can be obtained through the ApList resources. The SSID length may be up to 32 characters long. If it is longer, all exceeding characters will be cut off. (e.g., �linksys�, �NETGEAR�, �default�)
         */
        ::String50 *mpSsid;

        /**
         * [Optional] The received signal strength indicator(RSSI) is a measure of signal strength in decibels with respect to milliwatts(dBm). (e.g., �-54�, �-74�, �-85�)
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

        /**
         * [Optional] The security key to connect to the access point. If securityType attribute has a �None� as its value, this attribute does not affect security settings for communication with access points. The security key length may be up to 64 characters long. If it is longer, all exceeding characters will be cut off. Note that this attribute is write-only. (e.g., �passphrase�)
         */
        ::String100 *mpSecurityKey;

        /**
         * [Optional] If the selected WifiOperationType is Wi-Fi Direct and adapter supports Wi-Fi Direct, this attribute will be shown. Attribute indicates the Wi-Fi direct connection status and device will accept the connection request from the other Wi-Fi direct device without any confirmation. Note that declaration of the event enumeration type can be found in the "Attribute Type" declaration. (e.g., �Started�, �Connected�, �Disconnected�)
         */
        ::WifiEventType *mpWifiDirectStatus;

        /**
         * [Optional] If the selected WifiOperationType is Infrastructure and adapter supports WPS, this attribute will be shown. Attribute indicates whether the WPS is enabled or not on this network adapter. If this option is enabled, device will try to connect to every WPS enable AP available in environment and will take precedence over all other network preferences. (e.g., �True�, �False�)
         */
        bool *mpWpsEnabled;

        /**
         * [Optional] If the selected WifiOperationType is Infrastructure and adapter supports WPS, this attribute will be shown. Attribute indicates the WPS connection. Note that declaration of the event enumeration type can be found in the "Attribute Type" declaration. (e.g., �Started�, �Connected�, �Disconnected�)
         */
        ::WifiEventType *mpWpsStatus;

        /**
         * [Optional] Variable of AccessPointsLink object.
         */
        ::AccessPointsLink *mpAccessPointsLink;
};
/** @} */
#endif /* WIFI_H_ */
