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

#include "CMSerialization.h"
#include "list"
#include "string"
#include "Resource.h"
#include "OnType.h"
#include "StateType.h"
#include "ArrayOfString.h"
#include "String20.h"


/**
*  @addtogroup xsd
*  @{
*/

/**
 * @class   Operation
 *
 * This is a collection of common operational features of the device.
 * Also, this class defines methods that serialize and/or deserialize contents of XSD complex/list type Operation.
 * @since   1.0.0
 */

class Operation: public ::Resource
{

    protected:

        /**
         * Serializes the contents of Operation object using the specified serializer.
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
         * Deserializes data and prepares the Operation object contents.
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
         * Default constructor of Operation class.
         * @since   1.0.0
         */
        Operation();

        /**
         * Default virtual destructor of Operation class.
         * @since   1.0.0
         */
        virtual ~Operation();

        /**
         * This method gets the name of the serializable.
         *
         * @return Returns name of the serializable.
         * @since   1.0.0
         */
        virtual std::string getElementName();

        /**
         * Creates a new Operation object of the same class as this object. It then initializes each of the new object's fields by assigning it with the same value as the corresponding field in this object.
         *
         * @param[out]  pSrcObject  Source Operation object to be cloned.
         *
         * @return  @c  True    if cloning is successful@n
         *          @c  False   if cloning failed
         * @since   1.0.0
         */
        bool clone(::Operation *pSrcObject);

        /**
         * [Mandatory] Indicate the power On/Off status. Note that declaration of the on enumeration type can be found in the "Attribute Type" declaration. (e.g., �On�, �Off�)
         */
        ::OnType *mpPower;

        /**
         * [Optional] Indicate current state of the device. Note that declaration of the state enumeration type can be found in the "Attribute Type" declaration. (e.g., �Unknown� , �Start�, �Stop�, �Pause�)
         */
        ::StateType *mpState;

        /**
         * [Optional] Contains a set of supported progress. Available in both the singular and plural forms expression. In case of plural forms, each progress can be coupled with �|� sign. (e.g., �Wash�, �Wash|Dry|Preheat|Cooling|Spin|Weight Sense�)
         */
        ::ArrayOfString *mpSupportedProgress;

        /**
         * [Optional] Indicate current progress of the device. (e.g., �Wash�, �Dry�, �Preheat�, �Cook�)
        Washer/Dryer : Reserved, Pre Wash, Wash, Rinse, Spin, Dry, Air Wash, Cooling, Wrinkle Prevent, Finish
        Dish Washer : Wash, Dry, Clean, Finish
        Oven : Schedule, Weight Sense, Preheat, Cook
         */
        ::String20 *mpProgress;

        /**
         * [Optional] Indicate an operation time.
         */
        OC::Cm::Serialization::Xsd::TimeType *mpOperationTime;

        /**
         * [Optional] Indicate the minimum operation time that can be set on the device.
         */
        OC::Cm::Serialization::Xsd::TimeType *mpOperationTimeMin;

        /**
         * [Optional] Indicate the maximum operation time that can be set on the device.
         */
        OC::Cm::Serialization::Xsd::TimeType *mpOperationTimeMax;

        /**
         * [Optional] Indicate a remaining amount of time.
         */
        OC::Cm::Serialization::Xsd::TimeType *mpRemainingTime;

        /**
         * [Optional] Indicate a running time.
         */
        OC::Cm::Serialization::Xsd::TimeType *mpRunningTime;

        /**
         * [Optional] Unless the specified time has elapsed, device will not be started. Time unit is a minute. (e.g., �50�)
         */
        OC::Cm::Serialization::Xsd::TimeType *mpDelayStartTime;

        /**
         * [Optional] Device operation will be finished in accordance with the specified time. If the value is set to 300, then device operation requested will finish after five hours. However, if the time value is shorter than the minimum time required to complete the requested operation, device will ignore the delayEndTime setting request and attribute will not be updated. Time unit is a minute. (e.g., �300�)
         */
        OC::Cm::Serialization::Xsd::TimeType *mpDelayEndTime;

        /**
         * [Optional] This defines the accomplished progress of the device as a percentage. (e.g., �45�)
         */
        uint8_t *mpProgressPercentage;

        /**
         * [Optional] Indicate whether the Kids Lock function is enabled or not. Note that declaration of the on enumeration type can be found in the "Attribute Type" declaration. (e.g., �On�, �Off�)
         */
        ::StateType *mpKidsLock;

        /**
         * [Optional] Indicate the reset operation (e.g., �On�, �Off�)
         */
        ::OnType *mpReset;
};
/** @} */
#endif /* OPERATION_H_ */
