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



#ifndef SUBSCRIPTION_H_
#define SUBSCRIPTION_H_

#include "CMSerialization.h"
#include "list"
#include "string"
#include "IdentifiedObject.h"
#include "ArrayOfAnyURI.h"


/**
*  @addtogroup xsd
*  @{
*/


/**
 * @class   Subscription
 *
 * A subscription resource is realized as a resource for an individual client, providing interfaces to all subscriptions for the given client. The notification resource is used to receive notifications that a resource to which a host is subscribed has changed. The location of the notification resource is passed to the subscription server in the body of the subscription.  As such, a given client (notification resource server) may have one notification resource for multiple different notifications or may have a different notification resource for different notifications.
 * Also, this class defines methods that serialize and/or deserialize contents of XSD complex/list type Subscription.
 * @since   1.0.0
 */

class Subscription: public ::IdentifiedObject
{

    protected:

        /**
         * Serializes the contents of Subscription object using the specified serializer.
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
         * Deserializes data and prepares the Subscription object contents.
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
         * Default constructor of Subscription class.
         * @since   1.0.0
         */
        Subscription();

        /**
         * Default virtual destructor of Subscription class.
         * @since   1.0.0
         */
        virtual ~Subscription();

        /**
         * This method gets the name of the serializable.
         *
         * @return Returns name of the serializable.
         * @since   1.0.0
         */
        virtual std::string getElementName();

        /**
         * Creates a new Subscription object of the same class as this object. It then initializes each of the new object's fields by assigning it with the same value as the corresponding field in this object.
         *
         * @param[out]  pSrcObject  Source Subscription object to be cloned.
         *
         * @return  @c  True    if cloning is successful@n
         *          @c  False   if cloning failed
         * @since   1.0.0
         */
        bool clone(::Subscription *pSrcObject);

        /**
         * [Mandatory] Indicate the UUID of device (REST client) which subscribes event notification.
         */
        OC::Cm::Serialization::Xsd::UuidType *mpUuid;

        /**
         * [Mandatory] The resource for which the subscription applies. Query string parameters SHALL NOT be specified when subscribing to list resources.  Should a query string parameter be specified, servers SHALL ignore them. Client devices can subscribe to the “/devices” resource. Available in both the singular and plural forms expression. In case of plural forms, each option can be expressed in array of string. Note that declaration of the available resource enumeration type can be found in the section 2.2.3. (e.g., “devices/{deviceID}/operation”, “devices/{deviceID}/light”)
         */
        ::ArrayOfAnyURI *mpResourceURIs;

        /**
         * [Mandatory] The resource to which to post the notifications about the requested subscribed resource. (e.g., “/notification”)
         */
        std::string *mpNotificationURI;

        /**
         * [Optional] This attribute is only used in long polling notification mechanism. In subscription/notification mechanism, this attribute will not be used.
         */
        uint8_t *mpTimeout;

        /**
         * [Optional] This attribute is only used in long polling notification mechanism. In subscription/notification mechanism, this attribute will not be used.
         */
        uint8_t *mpDuration;
};
/** @} */
#endif /* SUBSCRIPTION_H_ */
