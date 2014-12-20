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

#ifndef SUBSCRIPTION_H_
#define SUBSCRIPTION_H_

#include "ISerializer.h"
#include "IDeserializer.h"
#include "list"
#include "string"
#include "IdentifiedObject.h"
#include "ArrayOfAnyURI.h"
#include "UuidType.h"


using namespace std;

/**
 * @class   Subscription
 * @brief   This class defines methods to serialize and/or deserialize contents of XSD complex/list type TEMPLATE_SERIALIZABLE_NAME
 */

class Subscription: public ::IdentifiedObject
{
    public:

        /**
         * Default constructor of Subscription class.
         */
        Subscription();

        /**
         * Default virtual destructor of Subscription class.
         */
        virtual ~Subscription();

        /**
         * Serializes the contents of Subscription object.
         *
         * @param[out] serializer   Serializer object which provide methods to serialize
         *
         * @return  @c True if serialization is successful @n
         *          @c False if serialization is failed
         */
        virtual bool serializeContents(OC::Cm::Serialization::ISerializer &serializer);

        /**
         * Deserializes data and prepares the Subscription object contents.
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
         * Creates a new Subscription object of the same class as this object. It then initializes each of the new object's fields by assigning it the same value as the corresponding field in this object.
         *
         * @param[out] pSrcObject   Source Subscription object to be cloned.
         *
         * @return  @c True if cloning is successful @n
         *          @c False if cloning is failed
         */
        bool clone(::Subscription *pSrcObject);
        OC::Cm::Serialization::Xsd::UuidType   *mpUuid;
        ::ArrayOfAnyURI    *mpResourceURIs;
        std::string    *mpNotificationURI;
        uint8_t    *mpTimeout;
        uint8_t    *mpDuration;
};

#endif /* SUBSCRIPTION_H_ */
