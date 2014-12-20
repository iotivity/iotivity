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



#ifndef EVENTS_H_
#define EVENTS_H_

#include "CMSerialization.h"
#include "list"
#include "string"
#include "NotificationEventType.h"


/**
*  @addtogroup xsd
*  @{
*/

/**
 * @class   Events
 *
 * This class defines methods that serialize and/or deserialize contents of XSD complex/list type Events.
 * @since   1.0.0
 */

class Events: public OC::Cm::Serialization::ISerializable
{

    protected:

        /**
         * Serializes the contents of Events object using the specified serializer.
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
         * Deserializes data and prepares the Events object contents.
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
         * Default constructor of Events class.
         * @since   1.0.0
         */
        Events();

        /**
         * Default virtual destructor of Events class.
         * @since   1.0.0
         */
        virtual ~Events();

        /**
         * This method gets the name of the serializable.
         *
         * @return Returns name of the serializable.
         * @since   1.0.0
         */
        virtual std::string getElementName();

        /**
         * Creates a new Events object of the same class as this object. It then initializes each of the new object's fields by assigning it with the same value as the corresponding field in this object.
         *
         * @param[out]  pSrcObject  Source Events object to be cloned.
         *
         * @return  @c  True    if cloning is successful@n
         *          @c  False   if cloning failed
         * @since   1.0.0
         */
        bool clone(::Events *pSrcObject);

        /**
         * [Mandatory] Indicate the UUID of device (REST server) where event occurs.
         */
        OC::Cm::Serialization::Xsd::UuidType *mpUuid;

        /**
         * [Mandatory] The resource for which the subscription applies. Query string parameters SHALL NOT be specified when subscribing to list resources.  Should a query string parameter be specified, servers SHALL ignore them. Client devices can subscribe to the �/devices� resource. But to prevent overwhelming network resource, notification associated with the connection status will only be sent to a given client. (e.g., �/detection�)
         */
        std::string *mpResourceURI;

        /**
         * [Optional] This field contains the acknowledgement or event status. Note that declaration of the event enumeration type can be found in the "Attribute Type" declaration. (e.g., �Created�, �Notified�, �Deleted�)
         */
        ::NotificationEventType *mpEvent;

        /**
         * [Mandatory] Indicate time when event occurs, in the format defined by DateTimeType.
         */
        OC::Cm::Serialization::Xsd::DateTimeType *mpEventTime;

        OC::Cm::Serialization::ISerializable *mpChangedResource;

        std::list<std::string> mpChoiceItems;
};
/** @} */
#endif /* EVENTS_H_ */
