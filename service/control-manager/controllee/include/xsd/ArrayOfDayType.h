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

#if !defined(_ARRAYOFDAYTYPE_H)
#define _ARRAYOFDAYTYPE_H

#include "ISerializer.h"
#include "IDeserializer.h"
#include "ISerializable.h"
#include "list"
#include "string"
#include "sstream"

#include "DayType.h"


/**
 * @class   ArrayOfDayType
 * @brief   ArrayOfDayType class implements OC::Cm::Serialization::ISerializable, defines methods to serialize and deserialize content of XSD Simple Type ArrayOfDayType List.
 */
class ArrayOfDayType : public OC::Cm::Serialization::ISerializable
{

    public:

        /**
         * Default constructor of ArrayOfDayType class.
         */
        ArrayOfDayType();

        /**
         * Default virtual destructor of ArrayOfDayType class.
         */
        virtual ~ArrayOfDayType();

        /**
         * Serializes the contents of ArrayOfDayType object.
         *
         * @param[in] serializer    Serializer object which provide methods to serialize
         *
         * @return  @c True if serialization is successful @n
         *          @c False if serialization is failed
         */
        virtual bool serializeContents(OC::Cm::Serialization::ISerializer &serializer);

        /**
         * Deserializes data and prepares the ArrayOfDayType object contents.
         *
         * @param[in] deserializer  Deserializer object which provide methods to deserialize
         *
         * @return  @c True if deserialization is successful @n
         *          @c False if deserialization is failed
         */
        virtual bool deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer);

        /**
         * Returns name of the serializable.
         *
         * @return Name of the serializable
         */
        virtual std::string getElementName();

        std::list< ::DayType > members;
};

#endif
