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



#ifndef THERMOSTAT_H_
#define THERMOSTAT_H_

#include "CMSerialization.h"
#include "list"
#include "string"
#include "Resource.h"
#include "ArrayOfString.h"
#include "String20.h"


/**
*  @addtogroup xsd
*  @{
*/


/**
 * @class   Thermostat
 *
 * This is a set of attributes associated with the theremostat.
 * Also, this class defines methods that serialize and/or deserialize contents of XSD complex/list type Thermostat.
 * @since   1.0.0
 */

class Thermostat: public ::Resource
{

    protected:

        /**
         * Serializes the contents of Thermostat object using the specified serializer.
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
         * Deserializes data and prepares the Thermostat object contents.
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
         * Default constructor of Thermostat class.
         * @since   1.0.0
         */
        Thermostat();

        /**
         * Default virtual destructor of Thermostat class.
         * @since   1.0.0
         */
        virtual ~Thermostat();

        /**
         * This method gets the name of the serializable.
         *
         * @return Returns name of the serializable.
         * @since   1.0.0
         */
        virtual std::string getElementName();

        /**
         * Creates a new Thermostat object of the same class as this object. It then initializes each of the new object's fields by assigning it with the same value as the corresponding field in this object.
         *
         * @param[out]  pSrcObject  Source Thermostat object to be cloned.
         *
         * @return  @c  True    if cloning is successful@n
         *          @c  False   if cloning failed
         * @since   1.0.0
         */
        bool clone(::Thermostat *pSrcObject);

        /**
         * [Mandatory] Contains a set of available modes of the thermostat. Available in both the singular and plural forms expression. In case of plural forms, each option can be expressed in array of string. (e.g., �Heating�, �Cooling�, �Auto�, �Fan Only�, �Dry Air�, �Energy_Save_Heat�)
         */
        ::ArrayOfString *mpSupportedThermostatModes;

        /**
         * [Mandatory] Indicate currently selected thermostat mode. If there exists �supportedThermostatModes�, the value of �thermostatMode� should be one of �supportedThermostatModes� list.
         */
        ::String20 *mpThermostatMode;

        /**
         * [Mandatory] Contains a set of available fan modes. Available in both the singular and plural forms expression. In case of plural forms, each option can be expressed in array of string.  (e.g., �Low�, �Medium�, �High�, �Circulation�)
         */
        ::ArrayOfString *mpSupportedFanModes;

        /**
         * [Mandatory] Indicate currently selected mode of the fan. If there exists �supportedFanModes�, the value of �fanMode� should be one of �supportedFanModes� list.
         */
        ::String20 *mpFanMode;

        /**
         * [Mandatory] Contains a set of available modes. Available in both the singular and plural forms expression. In case of plural forms, each option can be expressed in array of string. (e.g., �Heating�, �Cooling�,  �Fan Only�, �Pending_Heat�, �Pending_Cool�)
         */
        ::ArrayOfString *mpSupportedOperatingStates;

        /**
         * [Mandatory] Indicate currently selected operating states. If there exists �supportedOperatingStates�, the value of �operatingState� should be one of �supportedOperatingStates� list.
         */
        ::String20 *mpOperatingState;
};
/** @} */
#endif /* THERMOSTAT_H_ */
