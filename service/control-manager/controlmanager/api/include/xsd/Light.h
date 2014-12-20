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


#ifndef LIGHT_H_
#define LIGHT_H_

#include "CMSerialization.h"
#include "list"
#include "string"
#include "Resource.h"
#include "OnType.h"
#include "String50.h"
#include "Color.h"


/**
*  @addtogroup xsd
*  @{
*/

/**
 * @class   Light
 *
 * This resource has a set of items associated with the light.
 * Also, this class defines methods that serialize and/or deserialize contents of XSD complex/list type Light.
 * @since   1.0.0
 */

class Light: public ::Resource
{

    protected:

        /**
         * Serializes the contents of Light object using the specified serializer.
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
         * Deserializes data and prepares the Light object contents.
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
         * Default constructor of Light class.
         * @since   1.0.0
         */
        Light();

        /**
         * Default virtual destructor of Light class.
         * @since   1.0.0
         */
        virtual ~Light();

        /**
         * This method gets the name of the serializable.
         *
         * @return Returns name of the serializable.
         * @since   1.0.0
         */
        virtual std::string getElementName();

        /**
         * Creates a new Light object of the same class as this object. It then initializes each of the new object's fields by assigning it with the same value as the corresponding field in this object.
         *
         * @param[out]  pSrcObject  Source Light object to be cloned.
         *
         * @return  @c  True    if cloning is successful@n
         *          @c  False   if cloning failed
         * @since   1.0.0
         */
        bool clone(::Light *pSrcObject);

        /**
         * [Optional] Indicate the power status of light device
         */
        ::OnType *mpLightPower;

        /**
         * [Mandatory] Indicate currently set dimming level. If the set dimming level is higher than the number of dimming levels, device will ignore the dimming level setting request and attribute will not be updated. (e.g., �10�)
         */
        uint8_t *mpDimmingLevel;

        /**
         * [Mandatory] Indicate maximum number of dimming levels. (e.g., �20�)
         */
        uint8_t *mpMaxDimmingLevel;

        /**
         * [Optional] Indicate the relative dimming level based on current dimming level.
         */
        int16_t *mpRelativeDimmingLevel;

        /**
         * [Optional] Indicate the time interval to reach the newly set dimming level. Time unit is second.
         */
        uint16_t *mpTransitionTime;

        /**
         * [Optional] Indicate the currently selected pattern ID.
         */
        ::String50 *mpActivatePatternID;

        /**
         * [Optional] Indicate the pattern ID executed when light bulb turns on.
         */
        ::String50 *mpOnPatternID;

        /**
         * [Optional] Variable of Color object.
         */
        ::Color *mpColor;
};
/** @} */
#endif /* LIGHT_H_ */
