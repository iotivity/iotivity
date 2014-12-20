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



#ifndef INFORMATION_H_
#define INFORMATION_H_

#include "CMSerialization.h"
#include "list"
#include "string"
#include "Resource.h"
#include "String100.h"
#include "String20.h"
#include "String30.h"
#include "String50.h"
#include "LocationType.h"
#include "Specification.h"
#include "Versions.h"


/**
*  @addtogroup xsd
*  @{
*/

/**
 * @class   Information
 *
 * This function set provides an information and specification of the device.
 * Also, this class defines methods that serialize and/or deserialize contents of XSD complex/list type Information.
 * @since   1.0.0
 */
class Information: public ::Resource
{

    protected:

        /**
         * Serializes the contents of Information object using the specified serializer.
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
         * Deserializes data and prepares the Information object contents.
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
         * Default constructor of Information class.
         * @since   1.0.0
         */
        Information();

        /**
         * Default virtual destructor of Information class.
         * @since   1.0.0
         */
        virtual ~Information();

        /**
         * This method gets the name of the serializable.
         *
         * @return Returns name of the serializable.
         * @since   1.0.0
         */
        virtual std::string getElementName();

        /**
         * Creates a new Information object of the same class as this object. It then initializes each of the new object's fields by assigning it with the same value as the corresponding field in this object.
         *
         * @param[out]  pSrcObject  Source Information object to be cloned.
         *
         * @return  @c  True    if cloning is successful@n
         *          @c  False   if cloning failed
         * @since   1.0.0
         */
        bool clone(::Information *pSrcObject);

        /**
         * [Optional] This is a textual description of the device. In general, the model number will be displayed in this field. (e.g., �Michelangelo,Lenorard 13K�)
         */
        ::String100 *mpDescription;

        /**
         * [Optional] Indicate the manufacturer name of the device. (e.g., �Samsung Electronics�)
         */
        ::String20 *mpManufacturer;

        /**
         * [Optional] Variable of String30 object.
         */
        ::String30 *mpDeviceSubType;

        /**
         * [Optional] Indicate the device�s model ID. (e.g., �SEC-SHWM110S�)
         */
        ::String20 *mpModelID;

        /**
         * [Optional] Indicate the device�s serial number. (e.g., �ZS28401823NQ1763�)
         */
        ::String50 *mpSerialNumber;

        /**
         * [Optional] Indicate sales location of the device. Note that declaration of the location enumeration type can be found in the "Attribute Type" declaration. (e.g., �World_Wide�, �Asia�, �North_America�, �Europe�)
         */
        ::LocationType *mpSalesLocation;

        /**
         * [Optional] Variable of Specification object.
         */
        ::Specification *mpSpecification;

        /**
         * [Optional] Variable of Versions object.
         */
        ::Versions *mpVersions;
};
/** @} */
#endif /* INFORMATION_H_ */
