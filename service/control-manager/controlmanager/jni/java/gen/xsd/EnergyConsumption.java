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

package gen.xsd;

import OC.Cm.Serialization.IDeserializer;
import OC.Cm.Serialization.ISerializer;

/**
 * This resource provides an interface for an energy usage information of the
 * device.<br>
 * Also, this class defines methods that serialize and/or deserialize contents
 * of XSD complex/list type EnergyConsumption.
 * 
 * @since 1.0.0
 */
public class EnergyConsumption extends gen.xsd.Resource {
    /**
     * [Optional] Estimate of remaining battery charge as a percent of full
     * charge. (e.g., â€œ37â€�)
     */
    public Integer                              batteryCharge;

    /**
     * [Optional] Provides instantaneous power usage data of the device. Energy
     * unit is a Watt. (e.g., â€œ65.47â€�)
     */
    public Float                                instantaneousPower;

    /**
     * [Optional] Provides cumulative power usage consumed since the device turn
     * on. Energy unit is a Wh.
     */
    public Float                                cumulativeConsumption;

    /**
     * [Optional] Indicate the start time for calculating cumulative
     * consumption.
     */
    public OC.Cm.Serialization.Xsd.DateTimeType StartTime;

    /**
     * [Optional] Provides cumulative power usage during the hour. Energy unit
     * is a Wh.
     */
    public Float                                hourlyConsumption;

    /**
     * [Optional] Provides cumulative power usage during the day. Energy unit is
     * a Wh.
     */
    public Float                                dailyConsumption;

    /**
     * [Optional] Provides cumulative power usage during the week. Energy unit
     * is a Wh.
     */
    public Float                                weeklyConsumption;

    /**
     * [Optional] Provides cumulative power usage for the month. Energy unit is
     * a Wh.
     */
    public Float                                monthlyConsumption;

    /**
     * [Optional] Contains the number of seconds which expresses data refresh
     * cycle. Attribute value zero means disabled. (e.g., â€œ1â€�)
     */
    public Long                                 reportInterval;

    /**
     * [Optional] Indicate the energy usage threshold defined by device.
     */
    public Float                                usageThreshold;

    /**
     * [Optional] This attribute contains the target URL of the file saving
     * energy consumption information.
     */
    public String                               saveLocation;

    /**
     * Default constructor of EnergyConsumption class.
     * 
     * @since 1.0.0
     */
    public EnergyConsumption() {
    }

    /**
     * Destroy method for EnergyConsumption class.
     * 
     * @since 1.0.0
     */
    @Override
    public void destroy() {
        super.destroy();
    }

    /**
     * Returns name of the serializable.
     * 
     * @since 1.0.0
     */
    @Override
    public String getElementName() {
        return Elements.EN_ENERGYCONSUMPTION;
    }

    /**
     * Serializes the contents of EnergyConsumption object.
     * 
     * @param serializer
     *            Serializer object that provides methods to serialize.
     * 
     * @return True if serialization is successful / False if serialization
     *         failed
     * @since 1.0.0
     */
    @Override
    public boolean serializeContents(ISerializer serializer) {
        boolean retValue = true;
        if (false == super.serializeContents(serializer))
            retValue = false;

        try {
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != batteryCharge) {
                    bRet = serializer.setUnsignedByte("batteryCharge",
                            batteryCharge);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " batteryCharge");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != instantaneousPower) {
                    bRet = serializer.setfloatPropertyValue(
                            "instantaneousPower", instantaneousPower);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " instantaneousPower");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != cumulativeConsumption) {
                    bRet = serializer.setfloatPropertyValue(
                            "cumulativeConsumption", cumulativeConsumption);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " cumulativeConsumption");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != StartTime) {
                    if (StartTime.value.length() > 0) {
                        if (true == StartTime.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "startTime", StartTime.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " startTime");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != hourlyConsumption) {
                    bRet = serializer.setfloatPropertyValue(
                            "hourlyConsumption", hourlyConsumption);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " hourlyConsumption");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != dailyConsumption) {
                    bRet = serializer.setfloatPropertyValue("dailyConsumption",
                            dailyConsumption);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " dailyConsumption");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != weeklyConsumption) {
                    bRet = serializer.setfloatPropertyValue(
                            "weeklyConsumption", weeklyConsumption);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " weeklyConsumption");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != monthlyConsumption) {
                    bRet = serializer.setfloatPropertyValue(
                            "monthlyConsumption", monthlyConsumption);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " monthlyConsumption");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (null != reportInterval) {
                    bRet = serializer.setUnsignedInt("reportInterval",
                            reportInterval);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " reportInterval");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (null != usageThreshold) {
                    bRet = serializer.setfloatPropertyValue("usageThreshold",
                            usageThreshold);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " usageThreshold");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != saveLocation) {
                    if (saveLocation.length() > 0) {
                        bRet = serializer.setStringPropertyValue(
                                "saveLocation", saveLocation);
                    }
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " saveLocation");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
        } catch (Exception e) {
            return false;
        }
        return retValue;

    }

    /**
     * Deserializes data and prepares the EnergyConsumption object contents.
     * 
     * @param deserializer
     *            Deserializer object that provides methods to deserialize.
     * 
     * @return True if deserialization is successful / False if deserialization
     *         failed
     * @since 1.0.0
     */
    @Override
    public boolean deSerializeContents(IDeserializer deserializer) {
        boolean retValue = true;
        if (false == super.deSerializeContents(deserializer))
            retValue = false;

        try {
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("batteryCharge")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        batteryCharge = deserializer
                                .getUnsignedByte("batteryCharge");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("instantaneousPower")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        instantaneousPower = deserializer
                                .getfloatPropertyValue("instantaneousPower");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("cumulativeConsumption")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        cumulativeConsumption = deserializer
                                .getfloatPropertyValue("cumulativeConsumption");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("startTime")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        StartTime = new OC.Cm.Serialization.Xsd.DateTimeType();
                        StartTime.value = deserializer
                                .getStringPropertyValue("startTime");
                        if (false == StartTime.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " startTime");
                            bRet = false;
                        } else
                            bRet = true;
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("hourlyConsumption")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        hourlyConsumption = deserializer
                                .getfloatPropertyValue("hourlyConsumption");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("dailyConsumption")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        dailyConsumption = deserializer
                                .getfloatPropertyValue("dailyConsumption");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("weeklyConsumption")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        weeklyConsumption = deserializer
                                .getfloatPropertyValue("weeklyConsumption");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("monthlyConsumption")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        monthlyConsumption = deserializer
                                .getfloatPropertyValue("monthlyConsumption");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("reportInterval")) {
                    if (bRet == true) {
                        reportInterval = deserializer
                                .getUnsignedInt("reportInterval");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("usageThreshold")) {
                    if (bRet == true) {
                        usageThreshold = deserializer
                                .getfloatPropertyValue("usageThreshold");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("saveLocation")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        saveLocation = deserializer
                                .getStringPropertyValue("saveLocation");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
        } catch (Exception e) {
            return false;
        }
        return retValue;

    }
}
