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



#ifndef IDENTIFIEDOBJECT_H_
#define IDENTIFIEDOBJECT_H_

#include "CMSerialization.h"
#include "list"
#include "string"
#include "Resource.h"
#include "String50.h"



/**
*  @addtogroup xsd
*  @{
*/

/**
 * @class   IdentifiedObject
 *
 * This is a root class to provide common naming attributes for all classes needing naming attributes.
 * Also, this class defines methods that serialize and/or deserialize contents of XSD complex/list type IdentifiedObject.
 * @since   1.0.0
 */

class IdentifiedObject: public ::Resource
{

    protected:

        /**
         * Serializes the contents of IdentifiedObject object using the specified serializer.
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
         * Deserializes data and prepares the IdentifiedObject object contents.
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
         * Default constructor of IdentifiedObject class.
         * @since   1.0.0
         */
        IdentifiedObject();

        /**
         * Default virtual destructor of IdentifiedObject class.
         * @since   1.0.0
         */
        virtual ~IdentifiedObject();

        /**
         * This method gets the name of the serializable.
         *
         * @return Returns name of the serializable.
         * @since   1.0.0
         */
        virtual std::string getElementName();

        /**
         * Creates a new IdentifiedObject object of the same class as this object. It then initializes each of the new object's fields by assigning it with the same value as the corresponding field in this object.
         *
         * @param[out]  pSrcObject  Source IdentifiedObject object to be cloned.
         *
         * @return  @c  True    if cloning is successful@n
         *          @c  False   if cloning failed
         * @since   1.0.0
         */
        bool clone(::IdentifiedObject *pSrcObject);

        /**
         * [Mandatory] The device id should has sufficient entropy to uniquely identify the device in the context of its usage and is used to identify a device within a premise or cloud service. When the value of the attribute is zero, it represents device itself. (e.g., �0�) In case of the Group ID, value should start from zero and be increased by one per new group creation request. (e.g., �3�) Note that if the representation of the requested resource contains this attribute for PUT and POST methods, attribute should be ignored by the device.
         */
        ::String50 *mpId;

        /**
         * [Optional] The description is a human readable text describing or naming the object. (e.g., �Manage installed devices�)
         */
        ::String50 *mpDescription;
};
/** @} */
#endif /* IDENTIFIEDOBJECT_H_ */
