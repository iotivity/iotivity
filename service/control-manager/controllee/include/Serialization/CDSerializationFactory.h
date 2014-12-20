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

#if !defined(_CDSERIALIZATIONFACTORY_H)
#define _CDSERIALIZATIONFACTORY_H

#include "string"
#include "SerializableDataFactory.h"

/**
 * @class   CDSerializationFactory
 * @brief   Defines methods required to instantiate a serializable
 *
 */
namespace OC
{
    class CDSerializationFactory : public OC::Cm::Serialization::SerializableDataFactory
    {

        public:

            /**
             * Default constructor of CDSerializationFactory class
             */
            CDSerializationFactory();

            /**
             * Default virtual destructor of CDSerializationFactory class
             */
            ~CDSerializationFactory();

            /**
             * Instantiates request and response object that are identified by specific resource and http method.
             *
             * @param[in]   resourceType    Type of resource (Device, Configuration, Operation, etc.)
             * @param[in]   methodId        Http method type which needs to be executed (GET, PUT, POST, DELETE)
             * @param[out]  pp_request      Instantiated serializable request object
             * @param[out]  pp_response     Instantiated serializable response object
             *
             * @return  @c True If the object is successfully instantiated @n
             *          @c False In case of any error occurred
             */
            virtual bool createMethodSerializables(int resourceType, std::string &methodId,
                                                   OC::Cm::Serialization::ISerializable **pp_request,
                                                   OC::Cm::Serialization::ISerializable **pp_response);

            virtual bool deleteMethodSerializables(int resourceType, OC::Cm::Serialization::ISerializable *pp_request,
                                                   OC::Cm::Serialization::ISerializable *pp_response);

            /**
             * Instantiates any of the serializable object which is related to the specific element.
             *
             * @param[in] elementType Type of the element (Device, Configuration, Operation, etc.)
             * @param[out] pp_output Instantiated serializable object
             *
             * @return  @c True If the object is successfully instantiated @n
             *          @c False In case of any error occurred
             */
            virtual bool createSerializable(std::string &elementType, OC::Cm::Serialization::ISerializable **pp_output);

    };
}

#endif  //_CDSERIALIZATIONFACTORY_H
