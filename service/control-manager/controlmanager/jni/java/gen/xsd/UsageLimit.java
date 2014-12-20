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
 * This resource provides the ability to set the threshold of energy usage
 * consumed in devices/group.<br>
 * Also, this class defines methods that serialize and/or deserialize contents
 * of XSD complex/list type UsageLimit.
 * 
 * @since 1.0.0
 */
public class UsageLimit extends gen.xsd.IdentifiedObject {
    /**
     * [Mandatory] Budget goal within a certain time. This value could be
     * projected by the server or possibly chosen by the user. Note that
     * currency specified in the Information resource. (e.g., â€œ150â€�)
     */
    public Integer                              threshold;

    /**
     * [Mandatory] Indicate the start time for measuring the threshold of energy
     * usage.
     */
    public OC.Cm.Serialization.Xsd.DateTimeType StartTime;

    /**
     * [Mandatory] Indicate the end time for measuring the threshold of energy
     * usage.
     */
    public OC.Cm.Serialization.Xsd.DateTimeType EndTime;

    /**
     * [Optional] This attribute describes the state of the budget savings.
     * (e.g., â€œEnergy cost now seems to exceed the budget goals!â€�)
     */
    public gen.xsd.String200                    Efficiency;

    /**
     * [Optional] Indicate the device ID.
     */
    public OC.Cm.Serialization.Xsd.UuidType     DeviceID;

    /**
     * [Optional] Indicate the group ID.
     */
    public gen.xsd.String50                     GroupID;

    /**
     * Default constructor of UsageLimit class.
     * 
     * @since 1.0.0
     */
    public UsageLimit() {
    }

    /**
     * Destroy method for UsageLimit class.
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
        return Elements.EN_USAGELIMIT;
    }

    /**
     * Serializes the contents of UsageLimit object.
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
                if (null != threshold) {
                    bRet = serializer.setUnsignedShort("threshold", threshold);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " threshold");
                    }
                } else {
                    if (serializer.isInFilteringMode("threshold") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " threshold");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

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

            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != Efficiency) {
                    if (Efficiency.value.length() > 0) {
                        if (true == Efficiency.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "efficiency", Efficiency.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " efficiency");
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
        } catch (Exception e) {
            return false;
        }
        return retValue;

    }

    /**
     * Deserializes data and prepares the UsageLimit object contents.
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
                if (true == deserializer.hasProperty("threshold")) {
                    if (bRet == true) {
                        threshold = deserializer.getUnsignedShort("threshold");
                    }
                } else {
                    if (deserializer.isInFilteringMode("threshold") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " threshold");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

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
                if (true == deserializer.hasProperty("efficiency")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        Efficiency = new gen.xsd.String200();
                        Efficiency.value = deserializer
                                .getStringPropertyValue("efficiency");
                        if (false == Efficiency.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " efficiency");
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
        } catch (Exception e) {
            return false;
        }
        return retValue;

    }
}
