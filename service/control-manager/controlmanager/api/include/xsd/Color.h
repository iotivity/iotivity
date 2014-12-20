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



#ifndef COLOR_H_
#define COLOR_H_

#include "CMSerialization.h"
#include "list"
#include "string"
#include "String20.h"


/**
*  @addtogroup xsd
*  @{
*/

/**
 * @class   Color
 *
 * This class defines methods that serialize and/or deserialize contents of XSD complex/list type Color.
 * @since   1.0.0
 */

class Color: public OC::Cm::Serialization::ISerializable
{

    protected:

        /**
         * Serializes the contents of Color object using the specified serializer.
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
         * Deserializes data and prepares the Color object contents.
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
         * Default constructor of Color class.
         * @since   1.0.0
         */
        Color();

        /**
         * Default virtual destructor of Color class.
         * @since   1.0.0
         */
        virtual ~Color();

        /**
         * This method gets the name of the serializable.
         *
         * @return Returns name of the serializable.
         * @since   1.0.0
         */
        virtual std::string getElementName();

        /**
         * Creates a new Color object of the same class as this object. It then initializes each of the new object's fields by assigning it with the same value as the corresponding field in this object.
         *
         * @param[out]  pSrcObject  Source Color object to be cloned.
         *
         * @return  @c  True    if cloning is successful@n
         *          @c  False   if cloning failed
         * @since   1.0.0
         */
        bool clone(::Color *pSrcObject);

        /**
         * [Mandatory] Indicate the currently set hue level.
         */
        uint8_t *mpHue;

        /**
         * [Mandatory] Indicate the relative hue level based on current hue level.
         */
        int16_t *mpRelativeHue;

        /**
         * [Mandatory] Indicate the currently set saturation level.
         */
        uint8_t *mpSaturation;

        /**
         * [Mandatory] Indicate the relative saturation level based on current saturation level.
         */
        int16_t *mpRelativeSaturation;

        /**
         * [Mandatory] Indicate the currently set chromaticity X level.
         */
        uint16_t *mpChromaticityX;

        /**
         * [Mandatory] Indicate the currently set chromaticity Y level.
         */
        uint16_t *mpChromaticityY;

        /**
         * [Mandatory] Indicate the currently set color temperature.
         */
        uint16_t *mpColorTemperature;

        /**
         * [Mandatory] Variable of String20 object.
         */
        ::String20 *mpColorMode;
};
/** @} */
#endif /* COLOR_H_ */
