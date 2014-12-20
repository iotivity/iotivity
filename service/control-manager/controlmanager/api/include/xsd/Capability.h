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



#ifndef CAPABILITY_H_
#define CAPABILITY_H_

#include "CMSerialization.h"
#include "list"
#include "string"
#include "Resource.h"
#include "SubscriptionsLink.h"
#include "GroupsLink.h"
#include "RecordsLink.h"
#include "PatternsLink.h"
#include "EnergyLink.h"
#include "DevicesLink.h"
#include "ActionsLink.h"
#include "FoodManagerLink.h"
#include "NotificationsLink.h"
#include "LongPollingNotificationLink.h"


/**
*  @addtogroup xsd
*  @{
*/

/**
 * @class   Capability
 *
 * This resource enumerates the function sets supported by a device and can be used by clients. Also, this could be returned by the URI provided by the discovery service to allow clients to find the URIs to the functions in which they are interested.
 * Also, this class defines methods that serialize and/or deserialize contents of XSD complex/list type Capability.
 * @since   1.0.0
 */

class Capability: public ::Resource
{

    protected:

        /**
         * Serializes the contents of Capability object using the specified serializer.
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
         * Deserializes data and prepares the Capability object contents.
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
         * Default constructor of Capability class.
         * @since   1.0.0
         */
        Capability();

        /**
         * Default virtual destructor of Capability class.
         * @since   1.0.0
         */
        virtual ~Capability();

        /**
         * This method gets the name of the serializable.
         *
         * @return Returns name of the serializable.
         * @since   1.0.0
         */
        virtual std::string getElementName();

        /**
         * Creates a new Capability object of the same class as this object. It then initializes each of the new object's fields by assigning it with the same value as the corresponding field in this object.
         *
         * @param[out]  pSrcObject  Source Capability object to be cloned.
         *
         * @return  @c  True    if cloning is successful@n
         *          @c  False   if cloning failed
         * @since   1.0.0
         */
        bool clone(::Capability *pSrcObject);

        /**
         * [Mandatory] Variable of SubscriptionsLink object.
         */
        ::SubscriptionsLink *mpSubscriptionsLink;

        /**
         * [Optional] Variable of GroupsLink object.
         */
        ::GroupsLink *mpGroupsLink;

        /**
         * [Optional] Variable of RecordsLink object.
         */
        ::RecordsLink *mpRecordsLink;

        /**
         * [Optional] Variable of PatternsLink object.
         */
        ::PatternsLink *mpPatternsLink;

        /**
         * [Optional] Variable of EnergyLink object.
         */
        ::EnergyLink *mpEnergyLink;

        /**
         * [Mandatory] Variable of DevicesLink object.
         */
        ::DevicesLink *mpDevicesLink;

        /**
         * [Optional] Variable of ActionsLink object.
         */
        ::ActionsLink *mpActionsLink;

        /**
         * [Optional] Variable of FoodManagerLink object.
         */
        ::FoodManagerLink *mpFoodManagerLink;

        /**
         * [Optional] Variable of NotificationsLink object.
         */
        ::NotificationsLink *mpNotificationsLink;

        /**
         * [Optional] Variable of LongPollingNotificationLink object.
         */
        ::LongPollingNotificationLink *mpLongPollingNotificationLink;
};
/** @} */
#endif /* CAPABILITY_H_ */
