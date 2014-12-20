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

#include "ISerializer.h"
#include "IDeserializer.h"
#include "list"
#include "string"
#include "IdentifiedObject.h"
#include "String200.h"
#include "String50.h"
#include "DateTimeType.h"
#include "UuidType.h"


using namespace std;

/**
 * @class   UsageLimit
 * @brief   This class defines methods to serialize and/or deserialize contents of XSD complex/list type TEMPLATE_SERIALIZABLE_NAME
 */

class UsageLimit: public ::IdentifiedObject
{
    public:

        /**
         * Default constructor of UsageLimit class.
         */
        UsageLimit();

        /**
         * Default virtual destructor of UsageLimit class.
         */
        virtual ~UsageLimit();

        /**
         * Serializes the contents of UsageLimit object.
         *
         * @param[out] serializer   Serializer object which provide methods to serialize
         *
         * @return  @c True if serialization is successful @n
         *          @c False if serialization is failed
         */
        virtual bool serializeContents(OC::Cm::Serialization::ISerializer &serializer);

        /**
         * Deserializes data and prepares the UsageLimit object contents.
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
         * Creates a new UsageLimit object of the same class as this object. It then initializes each of the new object's fields by assigning it the same value as the corresponding field in this object.
         *
         * @param[out] pSrcObject   Source UsageLimit object to be cloned.
         *
         * @return  @c True if cloning is successful @n
         *          @c False if cloning is failed
         */
        bool clone(::UsageLimit *pSrcObject);
        uint16_t   *mpThreshold;
        OC::Cm::Serialization::Xsd::DateTimeType   *mpStartTime;
        OC::Cm::Serialization::Xsd::DateTimeType   *mpEndTime;
        ::String200    *mpEfficiency;
        OC::Cm::Serialization::Xsd::UuidType   *mpDeviceID;
        ::String50     *mpGroupID;
};

#endif /* USAGELIMIT_H_ */
