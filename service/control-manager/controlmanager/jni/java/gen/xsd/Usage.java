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
 * This class defines methods that serialize and/or deserialize contents of XSD
 * complex/list type Usage.
 * 
 * @since 1.0.0
 */
public class Usage extends OC.Cm.Serialization.ISerializable {
    /**
     * [Optional] The amount of energy used for a specified period of time. Note
     * that unit is specified in the Usages resource. (e.g., â€œ58â€�)
     */
    public Integer                              energy;

    /**
     * [Optional] Indicate the usage time that the user consumes devices. Time
     * unit is a second.
     */
    public Long                                 usageTime;

    /**
     * [Mandatory] Indicate the start time for this energy usage information.
     */
    public OC.Cm.Serialization.Xsd.DateTimeType StartTime;

    /**
     * [Mandatory] Indicate the end time for this energy usage information.
     */
    public OC.Cm.Serialization.Xsd.DateTimeType EndTime;

    /**
     * Default constructor of Usage class.
     * 
     * @since 1.0.0
     */
    public Usage() {
    }

    /**
     * Destroy method for Usage class.
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
        return Elements.EN_USAGE;
    }

    /**
     * Serializes the contents of Usage object.
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
        try {
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != energy) {
                    bRet = serializer.setUnsignedShort("energy", energy);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " energy");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != usageTime) {
                    bRet = serializer.setUnsignedInt("usageTime", usageTime);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " usageTime");
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

            if (serializer.validateMandatory() == true) {
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

        } catch (Exception e) {
            return false;
        }
        return retValue;

    }

    /**
     * Deserializes data and prepares the Usage object contents.
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
        try {
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("energy")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        energy = deserializer.getUnsignedShort("energy");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("usageTime")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        usageTime = deserializer.getUnsignedInt("usageTime");
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
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
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

        } catch (Exception e) {
            return false;
        }
        return retValue;

    }
}
