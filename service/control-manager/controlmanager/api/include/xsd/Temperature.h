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



#ifndef TEMPERATURE_H_
#define TEMPERATURE_H_

#include "CMSerialization.h"
#include "list"
#include "string"
#include "IdentifiedObject.h"
#include "String10.h"
#include "Float32.h"
#include "TemperatureUnitType.h"


/**
*  @addtogroup xsd
*  @{
*/


/**
 * @class   Temperature
 *
 * This is a set of attributes associated with the temperature.
 * Also, this class defines methods that serialize and/or deserialize contents of XSD complex/list type Temperature.
 * @since   1.0.0
 */

class Temperature: public ::IdentifiedObject
{

    protected:

        /**
         * Serializes the contents of Temperature object using the specified serializer.
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
         * Deserializes data and prepares the Temperature object contents.
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
         * Default constructor of Temperature class.
         * @since   1.0.0
         */
        Temperature();

        /**
         * Default virtual destructor of Temperature class.
         * @since   1.0.0
         */
        virtual ~Temperature();

        /**
         * This method gets the name of the serializable.
         *
         * @return Returns name of the serializable.
         * @since   1.0.0
         */
        virtual std::string getElementName();

        /**
         * Creates a new Temperature object of the same class as this object. It then initializes each of the new object's fields by assigning it with the same value as the corresponding field in this object.
         *
         * @param[out]  pSrcObject  Source Temperature object to be cloned.
         *
         * @return  @c  True    if cloning is successful@n
         *          @c  False   if cloning failed
         * @since   1.0.0
         */
        bool clone(::Temperature *pSrcObject);

        /**
         * [Optional] This attribute represents the name of the door resource. Each resource must have a name field value for the purpose of distinction. (e.g., �Freezer�, �Fridge�)
         */
        ::String10 *mpName;

        /**
         * [Optional] Indicates current temperature. (e.g., �88.5�)
         */
        ::Float32 *mpCurrent;

        /**
         * [Optional] Indicates desired temperature. (e.g., �75.2�)
         */
        ::Float32 *mpDesired;

        /**
         * [Optional] If this attribute is exists, desired temperature needed to be set according to this common difference attribute. Difference between two successive values based on the maximum value should have the characteristics of constant value. For example, assuming that the maximum temperature value is 88 and the value of this attribute is 3. Valid value that can be set is 88, 86, 84, 82.. and should be defined in the form of the arithmetic sequence. Note that attribute value is converted into a negative internally. But for the convenience of the user, attribute value will be displayed as a positive. (e.g., �2.0�)
         */
        ::Float32 *mpIncrement;

        /**
         * [Optional] Maximum temperature that can be set on the device. If the set temperature is higher than the maximum set temperature, device will ignore the temperature setting request and attribute will not be updated. (e.g., �90.0�)
         */
        ::Float32 *mpMaximum;

        /**
         * [Optional] Minimum temperature that can be set on the device. If the set temperature is lower than the minimum set temperature, device will ignore the temperature setting request and attribute will not be updated. (e.g., �64.0�)
         */
        ::Float32 *mpMinimum;

        /**
         * [Mandatory] This indicates the unit type defined for this temperature resource. Note that declaration of the temperature unit enumeration type can be found in the "Attribute Type" declaration. (e.g., �Fahrenheit�, �Celsius�)
         */
        ::TemperatureUnitType *mpUnit;
};
/** @} */
#endif /* TEMPERATURE_H_ */
