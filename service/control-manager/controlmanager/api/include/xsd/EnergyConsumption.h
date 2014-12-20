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



#ifndef ENERGYCONSUMPTION_H_
#define ENERGYCONSUMPTION_H_

#include "CMSerialization.h"
#include "list"
#include "string"
#include "Resource.h"
#include "Float32.h"


/**
*  @addtogroup xsd
*  @{
*/

/**
 * @class   EnergyConsumption
 *
 * This resource provides an interface for an energy usage information of the device.
 * Also, this class defines methods that serialize and/or deserialize contents of XSD complex/list type EnergyConsumption.
 * @since   1.0.0
 */

class EnergyConsumption: public ::Resource
{

    protected:

        /**
         * Serializes the contents of EnergyConsumption object using the specified serializer.
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
         * Deserializes data and prepares the EnergyConsumption object contents.
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
         * Default constructor of EnergyConsumption class.
         * @since   1.0.0
         */
        EnergyConsumption();

        /**
         * Default virtual destructor of EnergyConsumption class.
         * @since   1.0.0
         */
        virtual ~EnergyConsumption();

        /**
         * This method gets the name of the serializable.
         *
         * @return Returns name of the serializable.
         * @since   1.0.0
         */
        virtual std::string getElementName();

        /**
         * Creates a new EnergyConsumption object of the same class as this object. It then initializes each of the new object's fields by assigning it with the same value as the corresponding field in this object.
         *
         * @param[out]  pSrcObject  Source EnergyConsumption object to be cloned.
         *
         * @return  @c  True    if cloning is successful@n
         *          @c  False   if cloning failed
         * @since   1.0.0
         */
        bool clone(::EnergyConsumption *pSrcObject);

        /**
         * [Optional] Estimate of remaining battery charge as a percent of full charge. (e.g., �37�)
         */
        uint8_t *mpBatteryCharge;

        /**
         * [Optional] Provides instantaneous power usage data of the device. Energy unit is a Watt. (e.g., �65.47�)
         */
        ::Float32  *mpInstantaneousPower;

        /**
         * [Optional] Provides cumulative power usage consumed since the device turn on. Energy unit is a Wh.
         */
        ::Float32  *mpCumulativeConsumption;

        /**
         * [Optional] Indicate the start time for calculating cumulative consumption.
         */
        OC::Cm::Serialization::Xsd::DateTimeType *mpStartTime;

        /**
         * [Optional] Provides cumulative power usage during the hour. Energy unit is a Wh.
         */
        ::Float32  *mpHourlyConsumption;

        /**
         * [Optional] Provides cumulative power usage during the day. Energy unit is a Wh.
         */
        ::Float32  *mpDailyConsumption;

        /**
         * [Optional] Provides cumulative power usage during the week. Energy unit is a Wh.
         */
        ::Float32  *mpWeeklyConsumption;

        /**
         * [Optional] Provides cumulative power usage for the month. Energy unit is a Wh.
         */
        ::Float32  *mpMonthlyConsumption;

        /**
         * [Optional] Contains the number of seconds which expresses data refresh cycle. Attribute value zero means disabled. (e.g., �1�)
         */
        uint32_t *mpReportInterval;

        /**
         * [Optional] Indicate the energy usage threshold defined by device.
         */
        ::Float32  *mpUsageThreshold;

        /**
         * [Optional] This attribute contains the target URL of the file saving energy consumption information.
         */
        std::string *mpSaveLocation;
};
/** @} */
#endif /* ENERGYCONSUMPTION_H_ */
