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



#ifndef USAGE_H_
#define USAGE_H_

#include "CMSerialization.h"
#include "list"
#include "string"


/**
*  @addtogroup xsd
*  @{
*/


/**
 * @class   Usage
 *
 * This class defines methods that serialize and/or deserialize contents of XSD complex/list type Usage.
 * @since   1.0.0
 */

class Usage: public OC::Cm::Serialization::ISerializable
{

    protected:

        /**
         * Serializes the contents of Usage object using the specified serializer.
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
         * Deserializes data and prepares the Usage object contents.
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
         * Default constructor of Usage class.
         * @since   1.0.0
         */
        Usage();

        /**
         * Default virtual destructor of Usage class.
         * @since   1.0.0
         */
        virtual ~Usage();

        /**
         * This method gets the name of the serializable.
         *
         * @return Returns name of the serializable.
         * @since   1.0.0
         */
        virtual std::string getElementName();

        /**
         * Creates a new Usage object of the same class as this object. It then initializes each of the new object's fields by assigning it with the same value as the corresponding field in this object.
         *
         * @param[out]  pSrcObject  Source Usage object to be cloned.
         *
         * @return  @c  True    if cloning is successful@n
         *          @c  False   if cloning failed
         * @since   1.0.0
         */
        bool clone(::Usage *pSrcObject);

        /**
         * [Optional] The amount of energy used for a specified period of time. Note that unit is specified in the Usages resource. (e.g., �58�)
         */
        uint16_t *mpEnergy;

        /**
         * [Optional] Indicate the usage time that the user consumes devices. Time unit is a second.
         */
        uint32_t *mpUsageTime;

        /**
         * [Mandatory] Indicate the start time for this energy usage information.
         */
        OC::Cm::Serialization::Xsd::DateTimeType *mpStartTime;

        /**
         * [Mandatory] Indicate the end time for this energy usage information.
         */
        OC::Cm::Serialization::Xsd::DateTimeType *mpEndTime;
};
/** @} */
#endif /* USAGE_H_ */
