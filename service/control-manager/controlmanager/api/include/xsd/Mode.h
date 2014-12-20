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



#ifndef MODE_H_
#define MODE_H_

#include "CMSerialization.h"
#include "list"
#include "string"
#include "Resource.h"
#include "ArrayOfString.h"


/**
*  @addtogroup xsd
*  @{
*/

/**
 * @class   Mode
 *
 * This is a collection of common modes of the device. List of available modes, currently selected mode are queryable.
 * Also, this class defines methods that serialize and/or deserialize contents of XSD complex/list type Mode.
 * @since   1.0.0
 */

class Mode: public ::Resource
{

    protected:

        /**
         * Serializes the contents of Mode object using the specified serializer.
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
         * Deserializes data and prepares the Mode object contents.
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
         * Default constructor of Mode class.
         * @since   1.0.0
         */
        Mode();

        /**
         * Default virtual destructor of Mode class.
         * @since   1.0.0
         */
        virtual ~Mode();

        /**
         * This method gets the name of the serializable.
         *
         * @return Returns name of the serializable.
         * @since   1.0.0
         */
        virtual std::string getElementName();

        /**
         * Creates a new Mode object of the same class as this object. It then initializes each of the new object's fields by assigning it with the same value as the corresponding field in this object.
         *
         * @param[out]  pSrcObject  Source Mode object to be cloned.
         *
         * @return  @c  True    if cloning is successful@n
         *          @c  False   if cloning failed
         * @since   1.0.0
         */
        bool clone(::Mode *pSrcObject);

        /**
         * [Mandatory] Contains a set of available modes. Available in both the singular and plural forms expression. In case of plural forms, each option can be expressed in array of string. If there exists multiple group of mode functions, the name of modes is comprised of two part as follows: �ModeName_Value� (e.g., �Opmode_Cool�, �Opmode_Dry�, �Opmode_Wind�, �Comode_Speed�, �Comode_Sleep�, �Comode_Quiet�)
         */
        ::ArrayOfString *mpSupportedModes;

        /**
         * [Mandatory] Indicate currently selected modes. If there exists �supportedModes�, the value of �modes� should be one of �supportedModes� list. Available in both the singular and plural forms expression. In case of plural forms, each option can be expressed in array of string. (e.g., �Opmode_Cool�, �Comode_Quiet�)
         */
        ::ArrayOfString *mpModes;
};
/** @} */
#endif /* MODE_H_ */
