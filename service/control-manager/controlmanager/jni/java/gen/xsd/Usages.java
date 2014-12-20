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
 * This function set provides interfaces to retrieve usage information. Usage
 * information can be retrieved according to the time period defined in query
 * parameter of the request.<br>
 * Also, this class defines methods that serialize and/or deserialize contents
 * of XSD complex/list type Usages.
 * 
 * @since 1.0.0
 */
public class Usages extends gen.xsd.Resource {
    /**
     * [Mandatory] Indicate the start time for this energy usage information .
     */
    public OC.Cm.Serialization.Xsd.DateTimeType StartTime;

    /**
     * [Mandatory] Variable of DateTimeType object.
     */
    public OC.Cm.Serialization.Xsd.DateTimeType EndTime;

    /**
     * [Mandatory] The type field specifies a periodic operation cycle. Note
     * that declaration of the period enumeration type can be found in the
     * section 2.2.3. (e.g., â€œHourlyâ€�, â€œDailyâ€�, â€œWeeklyâ€�,
     * â€œMonthlyâ€�, â€œYearlyâ€�)
     */
    public gen.xsd.PeriodType                   PeriodType;

    /**
     * [Optional] Indicate the power unit type (e.g., â€œWhâ€�, â€œkWhâ€�)
     */
    public gen.xsd.PowerUnitType                Unit;

    /**
     * [Optional] Indicate the device ID.
     */
    public OC.Cm.Serialization.Xsd.UuidType     DeviceID;

    /**
     * [Optional] Indicate the group ID.
     */
    public gen.xsd.String50                     GroupID;

    /**
     * Variable that contains Usage objects in the List.
     */
    public java.util.List<gen.xsd.Usage>        usage = new java.util.ArrayList<gen.xsd.Usage>();

    /**
     * Default constructor of Usages class.
     * 
     * @since 1.0.0
     */
    public Usages() {
    }

    /**
     * Destroy method for Usages class.
     * 
     * @since 1.0.0
     */
    @Override
    public void destroy() {
        super.destroy();
        for (gen.xsd.Usage itr : usage) {
            if (itr != null) {
                itr.destroy();
            }
        }
    }

    /**
     * Returns name of the serializable.
     * 
     * @since 1.0.0
     */
    @Override
    public String getElementName() {
        return Elements.EN_USAGES;
    }

    /**
     * Serializes the contents of Usages object.
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
            {
                boolean bRet = true;
                if (null != StartTime) {
                    if (StartTime.value.length() > 0) {
                        if (true == StartTime.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "startTime", StartTime.value);
                        } else
                            bRet = false;
                    } else
                        bRet = false;
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " startTime");
                } else {
                    if (serializer.isInFilteringMode("startTime") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " startTime");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (null != EndTime) {
                    if (EndTime.value.length() > 0) {
                        if (true == EndTime.validateContent()) {
                            bRet = serializer.setStringPropertyValue("endTime",
                                    EndTime.value);
                        } else
                            bRet = false;
                    } else
                        bRet = false;
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " endTime");
                } else {
                    if (serializer.isInFilteringMode("endTime") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " endTime");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (null != PeriodType) {
                    if (PeriodType.value.length() > 0) {
                        if (true == PeriodType.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "periodType", PeriodType.value);
                        } else
                            bRet = false;
                    } else
                        bRet = false;
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " periodType");
                } else {
                    if (serializer.isInFilteringMode("periodType") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " periodType");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (null != Unit) {
                    if (Unit.value.length() > 0) {
                        if (true == Unit.validateContent()) {
                            bRet = serializer.setStringPropertyValue("unit",
                                    Unit.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " unit");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (null != DeviceID) {
                    if (DeviceID.value.length() > 0) {
                        if (true == DeviceID.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "deviceID", DeviceID.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " deviceID");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (null != GroupID) {
                    if (GroupID.value.length() > 0) {
                        if (true == GroupID.validateContent()) {
                            bRet = serializer.setStringPropertyValue("groupID",
                                    GroupID.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " groupID");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (null != usage) {
                    if (usage.size() > 0) {
                        if (serializer.validateMandatory() == false)
                            serializer.setIDExcemption(true);
                        if (bRet == true) {
                            serializer.startListProperty("Usage");
                            for (gen.xsd.Usage itr : usage) {
                                if (false == (itr).serialize(serializer)) {
                                    bRet = false;
                                    break;
                                }
                            }
                            serializer.endListProperty("Usage");
                        }
                        if (false == bRet) {
                            serializer
                                    .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                            + " Usage");
                        }
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
     * Deserializes data and prepares the Usages object contents.
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
                if (true == deserializer.hasProperty("startTime")) {
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
                } else {
                    if (deserializer.isInFilteringMode("startTime") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " startTime");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("endTime")) {
                    if (bRet == true) {
                        EndTime = new OC.Cm.Serialization.Xsd.DateTimeType();
                        EndTime.value = deserializer
                                .getStringPropertyValue("endTime");
                        if (false == EndTime.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " endTime");
                            bRet = false;
                        } else
                            bRet = true;
                    }
                } else {
                    if (deserializer.isInFilteringMode("endTime") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " endTime");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("periodType")) {
                    if (bRet == true) {
                        PeriodType = new gen.xsd.PeriodType();
                        PeriodType.value = deserializer
                                .getStringPropertyValue("periodType");
                        if (false == PeriodType.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " periodType");
                            bRet = false;
                        } else
                            bRet = true;
                    }
                } else {
                    if (deserializer.isInFilteringMode("periodType") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " periodType");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("unit")) {
                    if (bRet == true) {
                        Unit = new gen.xsd.PowerUnitType();
                        Unit.value = deserializer
                                .getStringPropertyValue("unit");
                        if (false == Unit.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " unit");
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
                if (true == deserializer.hasProperty("deviceID")) {
                    if (bRet == true) {
                        DeviceID = new OC.Cm.Serialization.Xsd.UuidType();
                        DeviceID.value = deserializer
                                .getStringPropertyValue("deviceID");
                        if (false == DeviceID.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " deviceID");
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
                if (true == deserializer.hasProperty("groupID")) {
                    if (bRet == true) {
                        GroupID = new gen.xsd.String50();
                        GroupID.value = deserializer
                                .getStringPropertyValue("groupID");
                        if (false == GroupID.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " groupID");
                            bRet = false;
                        } else
                            bRet = true;
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean isArray = deserializer.getIsArray("Usage");
                int sizeOfArray = 0;

                if (deserializer.validateMandatory() == false) {
                    deserializer.setIDExcemption(true);
                }

                if (isArray) {
                    sizeOfArray = deserializer.getSize("Usage");

                    if (sizeOfArray > 0) {
                        for (int i = 0; i < sizeOfArray; i++) {
                            gen.xsd.Usage m = new gen.xsd.Usage();

                            deserializer.setCurrentProperty("Usage", i);

                            m.setInstanceName("Usage");

                            boolean bRet = m.deSerialize(deserializer);
                            if (bRet == false) {
                                retValue = bRet;
                                break;
                            }

                            usage.add(m);
                        }
                    } else if (sizeOfArray == 0) {
                        deserializer.removeParameter("Usage");
                    }
                } else {
                    if (true == deserializer.hasProperty("Usage")) {
                        gen.xsd.Usage m = new gen.xsd.Usage();
                        boolean bRet = m.deSerialize(deserializer);
                        if (bRet == false) {
                            retValue = bRet;
                        }
                        usage.add(m);
                    } else {

                    }
                }
            }

        } catch (Exception e) {
            return false;
        }
        return retValue;

    }
}
