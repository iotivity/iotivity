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

#ifndef USAGES_H_
#define USAGES_H_

#include "ISerializer.h"
#include "IDeserializer.h"
#include "list"
#include "string"
#include "Resource.h"
#include "PeriodType.h"
#include "PowerUnitType.h"
#include "String50.h"
#include "Usage.h"
#include "UuidType.h"
#include "DateTimeType.h"



using namespace std;

/**
 * @class   Usages
 * @brief   This class defines methods to serialize and/or deserialize contents of XSD complex/list type TEMPLATE_SERIALIZABLE_NAME
 */

class Usages: public ::Resource
{
    public:

        /**
         * Default constructor of Usages class.
         */
        Usages();

        /**
         * Default virtual destructor of Usages class.
         */
        virtual ~Usages();

        /**
         * Serializes the contents of Usages object.
         *
         * @param[out] serializer   Serializer object which provide methods to serialize
         *
         * @return  @c True if serialization is successful @n
         *          @c False if serialization is failed
         */
        virtual bool serializeContents(OC::Cm::Serialization::ISerializer &serializer);

        /**
         * Deserializes data and prepares the Usages object contents.
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
         * Creates a new Usages object of the same class as this object. It then initializes each of the new object's fields by assigning it the same value as the corresponding field in this object.
         *
         * @param[out] pSrcObject   Source Usages object to be cloned.
         *
         * @return  @c True if cloning is successful @n
         *          @c False if cloning is failed
         */
        bool clone(::Usages *pSrcObject);
        OC::Cm::Serialization::Xsd::DateTimeType   *mpStartTime;
        OC::Cm::Serialization::Xsd::DateTimeType   *mpEndTime;
        ::PeriodType   *mpPeriodType;
        ::PowerUnitType    *mpUnit;
        OC::Cm::Serialization::Xsd::UuidType   *mpDeviceID;
        ::String50     *mpGroupID;
        std::list< ::Usage * >   mpUsage;
};

#endif /* USAGES_H_ */
