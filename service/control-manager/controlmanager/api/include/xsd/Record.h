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



#ifndef RECORD_H_
#define RECORD_H_

#include "CMSerialization.h"
#include "list"
#include "string"
#include "IdentifiedObject.h"
#include "ArrayOfUuid.h"
#include "ArrayOfString.h"
#include "String200.h"


/**
*  @addtogroup xsd
*  @{
*/

/**
 * @class   Record
 *
 * This class defines methods that serialize and/or deserialize contents of XSD complex/list type Record.
 * @since   1.0.0
 */

class Record: public ::IdentifiedObject
{

    protected:

        /**
         * Serializes the contents of Record object using the specified serializer.
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
         * Deserializes data and prepares the Record object contents.
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
         * Default constructor of Record class.
         * @since   1.0.0
         */
        Record();

        /**
         * Default virtual destructor of Record class.
         * @since   1.0.0
         */
        virtual ~Record();

        /**
         * This method gets the name of the serializable.
         *
         * @return Returns name of the serializable.
         * @since   1.0.0
         */
        virtual std::string getElementName();

        /**
         * Creates a new Record object of the same class as this object. It then initializes each of the new object's fields by assigning it with the same value as the corresponding field in this object.
         *
         * @param[out]  pSrcObject  Source Record object to be cloned.
         *
         * @return  @c  True    if cloning is successful@n
         *          @c  False   if cloning failed
         * @since   1.0.0
         */
        bool clone(::Record *pSrcObject);

        /**
         * [Optional] Indicate the list of device UUID which the action is recorded.
         */
        ::ArrayOfUuid *mpDeviceIDs;

        /**
         * [Optional] Indicate the list of groups which the action is recorded.
         */
        ::ArrayOfString *mpGroupIDs;

        /**
         * [Mandatory] Indicate time when event occurs, in the format defined by DateTimeType.
         */
        OC::Cm::Serialization::Xsd::DateTimeType *mpEventTime;

        /**
         * [Mandatory] Indicate the event description. It defines the behavior of the device during the certain period of the time. The regular expression in this field is "ResourceName:AttributeName^AttributeValue-AttributeName^AttributeValue|ResourceName:AttributeName^AttributeValue�. (e.g., �Temperature:current^6-set^4|Fridge:rapidFreezing^true�)
         */
        ::String200 *mpEventDescription;
};
/** @} */
#endif /* RECORD_H_ */
