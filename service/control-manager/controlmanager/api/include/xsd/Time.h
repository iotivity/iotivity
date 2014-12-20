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



#ifndef TIME_H_
#define TIME_H_

#include "CMSerialization.h"
#include "list"
#include "string"
#include "Resource.h"
#include "TimeZoneType.h"


/**
*  @addtogroup xsd
*  @{
*/


/**
 * @class   Time
 *
 * This resource contains the representation of time.
 * Also, this class defines methods that serialize and/or deserialize contents of XSD complex/list type Time.
 * @since   1.0.0
 */
class Time: public ::Resource
{

    protected:

        /**
         * Serializes the contents of Time object using the specified serializer.
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
         * Deserializes data and prepares the Time object contents.
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
         * Default constructor of Time class.
         * @since   1.0.0
         */
        Time();

        /**
         * Default virtual destructor of Time class.
         * @since   1.0.0
         */
        virtual ~Time();

        /**
         * This method gets the name of the serializable.
         *
         * @return Returns name of the serializable.
         * @since   1.0.0
         */
        virtual std::string getElementName();

        /**
         * Creates a new Time object of the same class as this object. It then initializes each of the new object's fields by assigning it with the same value as the corresponding field in this object.
         *
         * @param[out]  pSrcObject  Source Time object to be cloned.
         *
         * @return  @c  True    if cloning is successful@n
         *          @c  False   if cloning failed
         * @since   1.0.0
         */
        bool clone(::Time *pSrcObject);

        /**
         * [Mandatory] The current time, in the format defined by DateTimeType (e.g., �2013-07-15T18:44:31�) . This attribute may be set by client or may be calculated by following expression: currentTime = UTC+ tzOffset (+ dstOffset when in effect).
         */
        OC::Cm::Serialization::Xsd::DateTimeType *mpCurrentTime;

        /**
         * [Optional] Time at which daylight savings begins (apply dstOffset). Result of dstStartRule calculation.
         */
        OC::Cm::Serialization::Xsd::DateTimeType *mpDstStartTime;

        /**
         * [Optional] Daylight savings time offset from local standard time. A typical practice is advancing clocks one hour when daylight savings time is in effect, which would result in a positive dstOffset.
         */
        OC::Cm::Serialization::Xsd::TimeOffsetType *mpDstOffset;

        /**
         * [Optional] Time at which daylight savings ends (dstOffset no longer applied). Result of dstEndRule calculation.
         */
        OC::Cm::Serialization::Xsd::DateTimeType *mpDstEndTime;

        /**
         * [Optional] Time zones are expressed as positive or negative offsets form UTC. Offset from UTC are written in the format +-[hh]:[mm]. So if the time being described is one hour ahead of UTC, the zone designator would be "+01:00".
         */
        OC::Cm::Serialization::Xsd::TimeOffsetType *mpTzOffset;

        /**
         * [Optional] Indicate the Time Zone ID. (e.g., "Asia/Seoul")
         */
        ::TimeZoneType *mpTzID;

        /**
         * [Optional] UTC (Coordinated Universal Time) is the primary time standard. UTC is used in many Internet and World Wide Web standards. The Network Time Protocol, designed to synchronise the clocks of computers over the Internet, encodes times using the UTC system.
         */
        OC::Cm::Serialization::Xsd::DateTimeType *mpUtcTime;
};
/** @} */
#endif /* TIME_H_ */
