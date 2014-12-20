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

#ifndef OPERATION_H_
#define OPERATION_H_

#include "ISerializer.h"
#include "IDeserializer.h"
#include "list"
#include "string"
#include "Resource.h"
#include "OnType.h"
#include "StateType.h"
#include "TimeType.h"
#include "ArrayOfString.h"
#include "String20.h"


using namespace std;

/**
 * @class   Operation
 * @brief   This class defines methods to serialize and/or deserialize contents of XSD complex/list type TEMPLATE_SERIALIZABLE_NAME
 */

class Operation: public ::Resource
{
    public:

        /**
         * Default constructor of Operation class.
         */
        Operation();

        /**
         * Default virtual destructor of Operation class.
         */
        virtual ~Operation();

        /**
         * Serializes the contents of Operation object.
         *
         * @param[out] serializer   Serializer object which provide methods to serialize
         *
         * @return  @c True if serialization is successful @n
         *          @c False if serialization is failed
         */
        virtual bool serializeContents(OC::Cm::Serialization::ISerializer &serializer);

        /**
         * Deserializes data and prepares the Operation object contents.
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
         * Creates a new Operation object of the same class as this object. It then initializes each of the new object's fields by assigning it the same value as the corresponding field in this object.
         *
         * @param[out] pSrcObject   Source Operation object to be cloned.
         *
         * @return  @c True if cloning is successful @n
         *          @c False if cloning is failed
         */
        bool clone(::Operation *pSrcObject);
        ::OnType   *mpPower;
        ::StateType    *mpState;
        ::ArrayOfString    *mpSupportedProgress;
        ::String20     *mpProgress;
        OC::Cm::Serialization::Xsd::TimeType   *mpOperationTime;
        OC::Cm::Serialization::Xsd::TimeType   *mpOperationTimeMin;
        OC::Cm::Serialization::Xsd::TimeType   *mpOperationTimeMax;
        OC::Cm::Serialization::Xsd::TimeType   *mpRemainingTime;
        OC::Cm::Serialization::Xsd::TimeType   *mpRunningTime;
        OC::Cm::Serialization::Xsd::TimeType   *mpDelayStartTime;
        OC::Cm::Serialization::Xsd::TimeType   *mpDelayEndTime;
        uint8_t    *mpProgressPercentage;
        ::StateType    *mpKidsLock;
        ::OnType   *mpReset;
};

#endif /* OPERATION_H_ */
