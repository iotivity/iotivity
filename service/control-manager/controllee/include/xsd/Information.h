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

#include "ISerializer.h"
#include "IDeserializer.h"
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


using namespace std;

/**
 * @class   Information
 * @brief   This class defines methods to serialize and/or deserialize contents of XSD complex/list type TEMPLATE_SERIALIZABLE_NAME
 */

class Information: public ::Resource
{
    public:

        /**
         * Default constructor of Information class.
         */
        Information();

        /**
         * Default virtual destructor of Information class.
         */
        virtual ~Information();

        /**
         * Serializes the contents of Information object.
         *
         * @param[out] serializer   Serializer object which provide methods to serialize
         *
         * @return  @c True if serialization is successful @n
         *          @c False if serialization is failed
         */
        virtual bool serializeContents(OC::Cm::Serialization::ISerializer &serializer);

        /**
         * Deserializes data and prepares the Information object contents.
         *
         * @param[out] deserializer Deserializer object which provide methods to deserialize
         *
         * @return  @c True if deserialization is successful @n
         *          @c False if deserialization is failed
         */
        virtual bool deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer);

        /**
         * This method gets name of the serializable.
         *
         * @return  Returns name of the serializable.
         */
        virtual std::string getElementName();

        /**
         * Creates a new Information object of the same class as this object. It then initializes each of the new object's fields by assigning it the same value as the corresponding field in this object.
         *
         * @param[out] pSrcObject   Source Information object to be cloned.
         *
         * @return  @c True if cloning is successful @n
         *          @c False if cloning is failed
         */
        bool clone(::Information *pSrcObject);
        ::String100    *mpDescription;
        ::String20     *mpManufacturer;
        ::String30     *mpDeviceSubType;
        ::String20     *mpModelID;
        ::String50     *mpSerialNumber;
        ::LocationType     *mpSalesLocation;
        ::Specification    *mpSpecification;
        ::Versions     *mpVersions;
};

#endif /* INFORMATION_H_ */
