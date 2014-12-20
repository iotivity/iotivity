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



#ifndef LONGPOLLINGNOTIFICATION_H_
#define LONGPOLLINGNOTIFICATION_H_

#include "CMSerialization.h"
#include "list"
#include "string"
#include "Resource.h"
#include "String50.h"
#include "Events.h"


/**
*  @addtogroup xsd
*  @{
*/

/**
 * @class   LongPollingNotification
 *
 * This Subscription and Notification resource set support a generic, lightweight subscription/notification mechanism for use throughout the specification. This mechanism may be useful when a client wishes to quickly learn of changes to a resource on a server.
 * Also, this class defines methods that serialize and/or deserialize contents of XSD complex/list type LongPollingNotification.
 * @since   1.0.0
 */

class LongPollingNotification: public ::Resource
{

    protected:

        /**
         * Serializes the contents of LongPollingNotification object using the specified serializer.
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
         * Deserializes data and prepares the LongPollingNotification object contents.
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
         * Default constructor of LongPollingNotification class.
         * @since   1.0.0
         */
        LongPollingNotification();

        /**
         * Default virtual destructor of LongPollingNotification class.
         * @since   1.0.0
         */
        virtual ~LongPollingNotification();

        /**
         * This method gets the name of the serializable.
         *
         * @return Returns name of the serializable.
         * @since   1.0.0
         */
        virtual std::string getElementName();

        /**
         * Creates a new LongPollingNotification object of the same class as this object. It then initializes each of the new object's fields by assigning it with the same value as the corresponding field in this object.
         *
         * @param[out]  pSrcObject  Source LongPollingNotification object to be cloned.
         *
         * @return  @c  True    if cloning is successful@n
         *          @c  False   if cloning failed
         * @since   1.0.0
         */
        bool clone(::LongPollingNotification *pSrcObject);

        /**
         * [Mandatory] The subscription from which this notification was triggered. (e.g., �/devices/167261211391/subscriptions/3�)
         */
        ::String50 *mpSubscriptionID;

        /**
         * [Mandatory] Variable that contains Events objects in the List.
         */
        std::list< ::Events *> mpEvents;
};
/** @} */
#endif /* LONGPOLLINGNOTIFICATION_H_ */
