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



#ifndef USAGELIMIT_H_
#define USAGELIMIT_H_

#include "CMSerialization.h"
#include "list"
#include "string"
#include "IdentifiedObject.h"
#include "String200.h"
#include "String50.h"


/**
*  @addtogroup xsd
*  @{
*/


/**
 * @class   UsageLimit
 *
 * This resource provides the ability to set the threshold of energy usage consumed in devices/group.
 * Also, this class defines methods that serialize and/or deserialize contents of XSD complex/list type UsageLimit.
 * @since   1.0.0
 */

class UsageLimit: public ::IdentifiedObject
{

    protected:

        /**
         * Serializes the contents of UsageLimit object using the specified serializer.
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
         * Deserializes data and prepares the UsageLimit object contents.
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
         * Default constructor of UsageLimit class.
         * @since   1.0.0
         */
        UsageLimit();

        /**
         * Default virtual destructor of UsageLimit class.
         * @since   1.0.0
         */
        virtual ~UsageLimit();

        /**
         * This method gets the name of the serializable.
         *
         * @return Returns name of the serializable.
         * @since   1.0.0
         */
        virtual std::string getElementName();

        /**
         * Creates a new UsageLimit object of the same class as this object. It then initializes each of the new object's fields by assigning it with the same value as the corresponding field in this object.
         *
         * @param[out]  pSrcObject  Source UsageLimit object to be cloned.
         *
         * @return  @c  True    if cloning is successful@n
         *          @c  False   if cloning failed
         * @since   1.0.0
         */
        bool clone(::UsageLimit *pSrcObject);

        /**
         * [Mandatory] Budget goal within a certain time. This value could be projected by the server or possibly chosen by the user. Note that currency specified in the Information resource. (e.g., �150�)
         */
        uint16_t *mpThreshold;

        /**
         * [Mandatory] Indicate the start time for measuring the threshold of energy usage.
         */
        OC::Cm::Serialization::Xsd::DateTimeType *mpStartTime;

        /**
         * [Mandatory] Indicate the end time for measuring the threshold of energy usage.
         */
        OC::Cm::Serialization::Xsd::DateTimeType *mpEndTime;

        /**
         * [Optional] This attribute describes the state of the budget savings. (e.g., �Energy cost now seems to exceed the budget goals!�)
         */
        ::String200 *mpEfficiency;

        /**
         * [Optional] Indicate the device ID.
         */
        OC::Cm::Serialization::Xsd::UuidType *mpDeviceID;

        /**
         * [Optional] Indicate the group ID.
         */
        ::String50 *mpGroupID;
};
/** @} */
#endif /* USAGELIMIT_H_ */
