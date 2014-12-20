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

#if !defined(_SUBSERIALIZATIONFACTORY_H)
#define _SUBSERIALIZATIONFACTORY_H

#include "SerializableDataFactory.h"
#include "ISerializable.h"
#include <string>

namespace OC
{
    namespace Cm
    {
        namespace Notification
        {

            /**
             * @class       SubscriptionSerializationFactory
             * @brief       This is a subscription serialization factory which creates the correct serializable objects based on resource type.
             */
            class SubscriptionSerializationFactory: public OC::Cm::Serialization::SerializableDataFactory
            {

                public:

                    /**
                     * SubscriptionSerializationFactory - Default constructor
                     *
                     * @param -none
                     * @return -none
                     */
                    SubscriptionSerializationFactory();

                    /**
                     * ~SubscriptionSerializationFactory - Default destructor
                     *
                     * @param -none
                     * @return -none
                     */
                    ~SubscriptionSerializationFactory();

                    /**
                     * ~SubscriptionSerializationFactory - Default destructor
                     *
                     * @param -none
                     * @return -none
                     */
                    virtual bool createMethodSerializables(int resourceType,
                                                           std::string &methodId,
                                                           OC::Cm::Serialization::ISerializable **pp_request,
                                                           OC::Cm::Serialization::ISerializable **pp_response);

                    /**
                     * createSerializable - Create the ISerializable object based on the element type.
                     *
                     * @param elementType  The resourceType
                     * @param pp_output The ISerializable object as output(output)
                     *
                     * @return  type:bool returns true in case of success, false in case of any error
                     *
                     */
                    virtual bool createSerializable(std::string &elementType,
                                                    OC::Cm::Serialization::ISerializable **pp_output);

                    /**
                     * deleteMethodSerializables - Deletes request object which can hold request data of request identified by specified resource and method
                     *
                     * @param resourceType The resourceType
                     * @param pp_request    Represents ISerializable instance of the request(output)
                     * @param pp_response   Represents ISerializable instance of the response(output)
                     *
                     * @return  type:bool returns true in case of success, false in case of any error
                     *
                     */
                    virtual bool deleteMethodSerializables(int resourceType,
                                                           OC::Cm::Serialization::ISerializable *pp_request,
                                                           OC::Cm::Serialization::ISerializable *pp_response);

            };

        }
    }
}

#endif  //_SUBSERIALIZATIONFACTORY_H
