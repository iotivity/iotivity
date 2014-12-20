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



#ifndef NETWORK_H_
#define NETWORK_H_

#include "CMSerialization.h"
#include "list"
#include "string"
#include "IdentifiedObject.h"
#include "String50.h"
#include "String20.h"
#include "WiFi.h"


/**
*  @addtogroup xsd
*  @{
*/

/**
 * @class   Network
 *
 * This resource provides a network adapter management functions.
 * Also, this class defines methods that serialize and/or deserialize contents of XSD complex/list type Network.
 * @since   1.0.0
 */

class Network: public ::IdentifiedObject
{

    protected:

        /**
         * Serializes the contents of Network object using the specified serializer.
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
         * Deserializes data and prepares the Network object contents.
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
         * Default constructor of Network class.
         * @since   1.0.0
         */
        Network();

        /**
         * Default virtual destructor of Network class.
         * @since   1.0.0
         */
        virtual ~Network();

        /**
         * This method gets the name of the serializable.
         *
         * @return Returns name of the serializable.
         * @since   1.0.0
         */
        virtual std::string getElementName();

        /**
         * Creates a new Network object of the same class as this object. It then initializes each of the new object's fields by assigning it with the same value as the corresponding field in this object.
         *
         * @param[out]  pSrcObject  Source Network object to be cloned.
         *
         * @return  @c  True    if cloning is successful@n
         *          @c  False   if cloning failed
         * @since   1.0.0
         */
        bool clone(::Network *pSrcObject);

        /**
         * [Mandatory] Indicate whether the network adapter is enabled or not. (e.g., �True�, �False�)
         */
        bool *mpEnabled;

        /**
         * [Mandatory] Indicate whether the adapter is connected to the network or not. (e.g., �True�, �False�)
         */
        bool *mpConnected;

        /**
         * [Optional] Set the priority value of the preferred network adapter to be lower than the priority values of the other adapters. Setting the priority value lower means that the network adapter has a higher priority. The priority value takes precedence in the network adapter order. If the priority value about to assign is already exists, priority of network adapter will be internally determined, and the results cannot be predicted. Therefore, should be careful not to assign the same priority value. (e.g., �0�, �1�, �2�)
         */
        uint8_t *mpPriority;

        /**
         * [Mandatory] This attribute indicates whether the DHCP is enabled or not on this network adapter. (e.g., �True�, �False�)
         */
        bool *mpDhcpEnabled;

        /**
         * [Mandatory] The network adapter�s IP address. (e.g., �192.168.10.2�, �fe80::7464:36e5:41a5:821/64�)
         */
        ::String50 *mpIpAddress;

        /**
         * [Mandatory] The subnet mask for this network adapter. (e.g., �255.255.255.0�, �64�)
         */
        ::String20 *mpSubnetMask;

        /**
         * [Mandatory] The default gateway for this network adapter. (e.g., �192.168.10.1�)
         */
        ::String50 *mpGateway;

        /**
         * [Mandatory] The primary DNS server address for DNS lookups. (e.g., �8.8.4.4�)
         */
        ::String50 *mpDnsPrimary;

        /**
         * [Optional] The secondary DNS server address for DNS lookups when the primary DNS server is unavailable. (e.g., �8.8.8.8�)
         */
        ::String50 *mpDnsSecondary;

        /**
         * [Mandatory] The network adapter�s MAC address. (e.g., �00:23:32:95:40:CB�)
         */
        ::String20 *mpMacAddress;

        /**
         * [Optional] Variable of WiFi object.
         */
        ::WiFi *mpWiFi;
};
/** @} */
#endif /* NETWORK_H_ */
