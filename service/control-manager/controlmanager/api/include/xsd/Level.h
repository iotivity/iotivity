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



#ifndef LEVEL_H_
#define LEVEL_H_

#include "CMSerialization.h"
#include "list"
#include "string"
#include "Resource.h"


/**
*  @addtogroup xsd
*  @{
*/

/**
 * @class   Level
 *
 * This resource has a set of items associated with the level control.
 * Also, this class defines methods that serialize and/or deserialize contents of XSD complex/list type Level.
 * @since   1.0.0
 */

class Level: public ::Resource
{

    protected:

        /**
         * Serializes the contents of Level object using the specified serializer.
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
         * Deserializes data and prepares the Level object contents.
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
         * Default constructor of Level class.
         * @since   1.0.0
         */
        Level();

        /**
         * Default virtual destructor of Level class.
         * @since   1.0.0
         */
        virtual ~Level();

        /**
         * This method gets the name of the serializable.
         *
         * @return Returns name of the serializable.
         * @since   1.0.0
         */
        virtual std::string getElementName();

        /**
         * Creates a new Level object of the same class as this object. It then initializes each of the new object's fields by assigning it with the same value as the corresponding field in this object.
         *
         * @param[out]  pSrcObject  Source Level object to be cloned.
         *
         * @return  @c  True    if cloning is successful@n
         *          @c  False   if cloning failed
         * @since   1.0.0
         */
        bool clone(::Level *pSrcObject);

        /**
         * [Mandatory] Indicate the current level value.
         */
        uint8_t *mpCurrent;

        /**
         * [Mandatory] Indicate the desired level value.
         */
        uint8_t *mpDesired;

        /**
         * [Optional] If this attribute is exists, desired level needed to be set according to this common difference attribute. Difference between two successive values based on the maximum value should have the characteristics of constant value. For example, assuming that the maximum level value is 18 and the value of this attribute is 2. Valid value that can be set is 16, 14, 12, 10.. and should be defined in the form of the arithmetic sequence. Note that attribute value is converted into a negative internally. But for the convenience of the user, attribute value will be displayed as a positive. (e.g., �2�)
         */
        uint8_t *mpIncrement;

        /**
         * [Optional] Maximum level that can be set on the device. If the set level value is higher than the maximum set value, device will ignore the level setting request and attribute will not be updated. (e.g., �90�)
         */
        uint8_t *mpMaximum;

        /**
         * [Optional] Minimum level that can be set on the device. If the set level value is lower than the minimum set value, device will ignore the level setting request and attribute will not be updated. (e.g., �64�)
         */
        uint8_t *mpMinimum;
};
/** @} */
#endif /* LEVEL_H_ */
