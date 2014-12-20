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

#ifndef CDSerializableDataFactory_H_
#define CDSerializableDataFactory_H_

#include "SerializableDataFactory.h"

using namespace std;
/**
 * @class   CDSerializableDataFactory
 * @brief   This class is the Core class factory which redirects to appropriate factory
 */
class CDSerializableDataFactory: public OC::Cm::Serialization::SerializableDataFactory
{

    public:
        /**
         * CDSerializableDataFactory - Constructor
         *
         * @param -none
         * @return -none
         */
        CDSerializableDataFactory();

        /**
         * ~CDSerializableDataFactory - Destructor
         *
         * @param -none
         * @return -none
         */
        virtual ~CDSerializableDataFactory();

        /**
         *createSerializable -Get the ISerializable** object based on the element type.
         *
         * @param - elementType -  The resource type
         * @param - pp_output - The variable after processing(output)
         * @return -type:bool true if the method the operation was successful, else false
         */
        virtual bool createSerializable(std::string &elementType,
                                        OC::Cm::Serialization::ISerializable **pp_output);
        /**
         *createMethodSerializables -Get the ISerializable** objects based on the element type.
         *
         * @param - resourceType -  The resource type
         * @param - pp_request - The variable after processing(output)
         * @param - pp_response - The variable after processing(output)
         * @return -type:bool true if the method the operation was successful, else false
         */
        virtual bool createMethodSerializables(int resourceType,
                                               std::string &methodId,
                                               OC::Cm::Serialization::ISerializable **pp_request,
                                               OC::Cm::Serialization::ISerializable **pp_response);

        /**
         *createMethodSerializables -Get the ISerializable** objects based on the element type.
         *
         * @param - resourceType -  The resource type
         * @param - pp_request - The request object from createMethodSerializables
         * @param - pp_response - The response object from createMethodSerializables
         * @return -type:bool true if the method the operation was successful, else false
         */
        virtual bool deleteMethodSerializables(int resourceType,
                                               OC::Cm::Serialization::ISerializable *pp_request,
                                               OC::Cm::Serialization::ISerializable *pp_response);

        OC::Cm::Serialization::SerializableDataFactory *pApplicationFactory;/**< Represents the Application Factory Class instance. */
        OC::Cm::Serialization::SerializableDataFactory *pSubscriptionFactory;/**< Represents the Subscription Factory Class instance. */
};

#endif /* CDSERIALIZABLEDATAFACTORY_H_ */
