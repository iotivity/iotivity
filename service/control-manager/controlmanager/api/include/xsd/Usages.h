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



#ifndef USAGES_H_
#define USAGES_H_

#include "CMSerialization.h"
#include "list"
#include "string"
#include "Resource.h"
#include "PeriodType.h"
#include "PowerUnitType.h"
#include "String50.h"
#include "Usage.h"


/**
*  @addtogroup xsd
*  @{
*/


/**
 * @class   Usages
 *
 * This function set provides interfaces to retrieve usage information. Usage information can be retrieved according to the time period defined in query parameter of the request.
 * Also, this class defines methods that serialize and/or deserialize contents of XSD complex/list type Usages.
 * @since   1.0.0
 */

class Usages: public ::Resource
{

    protected:

        /**
         * Serializes the contents of Usages object using the specified serializer.
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
         * Deserializes data and prepares the Usages object contents.
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
         * Default constructor of Usages class.
         * @since   1.0.0
         */
        Usages();

        /**
         * Default virtual destructor of Usages class.
         * @since   1.0.0
         */
        virtual ~Usages();

        /**
         * This method gets the name of the serializable.
         *
         * @return Returns name of the serializable.
         * @since   1.0.0
         */
        virtual std::string getElementName();

        /**
         * Creates a new Usages object of the same class as this object. It then initializes each of the new object's fields by assigning it with the same value as the corresponding field in this object.
         *
         * @param[out]  pSrcObject  Source Usages object to be cloned.
         *
         * @return  @c  True    if cloning is successful@n
         *          @c  False   if cloning failed
         * @since   1.0.0
         */
        bool clone(::Usages *pSrcObject);

        /**
         * [Mandatory] Indicate the start time for this energy usage information .
         */
        OC::Cm::Serialization::Xsd::DateTimeType *mpStartTime;

        /**
         * [Mandatory] Variable of DateTimeType object.
         */
        OC::Cm::Serialization::Xsd::DateTimeType *mpEndTime;

        /**
         * [Mandatory] The type field specifies a periodic operation cycle. Note that declaration of the period enumeration type can be found in the section 2.2.3. (e.g., �Hourly�, �Daily�, �Weekly�, �Monthly�, �Yearly�)
         */
        ::PeriodType *mpPeriodType;

        /**
         * [Optional] Indicate the power unit type (e.g., �Wh�, �kWh�)
         */
        ::PowerUnitType *mpUnit;

        /**
         * [Optional] Indicate the device ID.
         */
        OC::Cm::Serialization::Xsd::UuidType *mpDeviceID;

        /**
         * [Optional] Indicate the group ID.
         */
        ::String50 *mpGroupID;

        /**
         * [Optional] Variable that contains Usage objects in the List.
         */
        std::list< ::Usage *> mpUsage;
};
/** @} */
#endif /* USAGES_H_ */
