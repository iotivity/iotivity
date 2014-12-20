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



#ifndef ALARM_H_
#define ALARM_H_

#include "CMSerialization.h"
#include "list"
#include "string"
#include "IdentifiedObject.h"
#include "AlarmCodeType.h"
#include "String20.h"


/**
*  @addtogroup xsd
*  @{
*/

/**
 * @class   Alarm
 *
 * This function set provides an interface for an alarm service. If device need to inform the user such as warning messages when exceeding or dropping below the specific thresholds, this feature can be useful.
 * Also, this class defines methods that serialize and/or deserialize contents of XSD complex/list type Alarm.
 * @since   1.0.0
 */

class Alarm: public ::IdentifiedObject
{

    protected:

        /**
         * Serializes the contents of Alarm object using the specified serializer.
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
         * Deserializes data and prepares the Alarm object contents.
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
         * Default constructor of Alarm class.
         * @since   1.0.0
         */
        Alarm();

        /**
         * Default virtual destructor of Alarm class.
         * @since   1.0.0
         */
        virtual ~Alarm();

        /**
         * This method gets the name of the serializable.
         *
         * @return Returns name of the serializable.
         * @since   1.0.0
         */
        virtual std::string getElementName();

        /**
         * Creates a new Alarm object of the same class as this object. It then initializes each of the new object's fields by assigning it with the same value as the corresponding field in this object.
         *
         * @param[out]  pSrcObject  Source Alarm object to be cloned.
         *
         * @return  @c  True    if cloning is successful@n
         *          @c  False   if cloning failed
         * @since   1.0.0
         */
        bool clone(::Alarm *pSrcObject);

        /**
         * [Mandatory] Indicate the alarm triggered time, in the format defined by DateTimeType. (e.g., �2013-07-15T18:44:31�)
         */
        OC::Cm::Serialization::Xsd::DateTimeType *mpTriggeredTime;

        /**
         * [Mandatory] Represents the reason of alarm activation. If there is no alarm history, attribute value will be left �Unknown� as its value. Note that declaration of the alarm code enumeration type can be found in the section 2.2.3. (e.g., �Unknown�, �Freezer_Abnormal_Temperature� , �Refrigerator_Abnormal_Temperature�)
         */
        ::AlarmCodeType *mpAlarmType;

        /**
         * [Optional] Represents the error code. (e.g., �0x4E�, �0x5E�) Since these codes are not standardized in this specification, it is RECOMMENDED that alarm codes be described in the user manual (or a development manual)
         */
        ::String20 *mpCode;

        /**
         * [Optional] Indicate whether the alarm message is transported by multicasting or subscription/notification method. (e.g., �true�, �false�)
         */
        bool *mpMulticastEnabled;
};
/** @} */
#endif /* ALARM_H_ */
