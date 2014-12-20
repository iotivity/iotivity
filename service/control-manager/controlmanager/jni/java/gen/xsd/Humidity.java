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
 * This is a set of attributes associated with the humidity.<br>
 * Also, this class defines methods that serialize and/or deserialize contents
 * of XSD complex/list type Humidity.
 * 
 * @since 1.0.0
 */
public class Humidity extends gen.xsd.Resource {
    /**
     * [Mandatory] Indicates current atmospheric humidity, in percent. (e.g.,
     * â€œ74.7â€�)
     */
    public Float                    current;

    /**
     * [Optional] Indicates desired atmospheric humidity, in percent. (e.g.,
     * â€œ40.6â€�)
     */
    public Float                    desired;

    /**
     * [Mandatory] This indicate the unit type defined for this humidity
     * resource.
     */
    public gen.xsd.HumidityUnitType Unit;

    /**
     * Default constructor of Humidity class.
     * 
     * @since 1.0.0
     */
    public Humidity() {
    }

    /**
     * Destroy method for Humidity class.
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
        return Elements.EN_HUMIDITY;
    }

    /**
     * Serializes the contents of Humidity object.
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
                if (null != current) {
                    bRet = serializer.setfloatPropertyValue("current", current);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " current");
                    }
                } else {
                    if (serializer.isInFilteringMode("current") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " current");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (null != desired) {
                    bRet = serializer.setfloatPropertyValue("desired", desired);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " desired");
                    }
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
                    } else
                        bRet = false;
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " unit");
                } else {
                    if (serializer.isInFilteringMode("unit") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " unit");
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
     * Deserializes data and prepares the Humidity object contents.
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
                if (true == deserializer.hasProperty("current")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        current = deserializer.getfloatPropertyValue("current");
                    }
                } else {
                    if (deserializer.isInFilteringMode("current") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " current");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("desired")) {
                    if (bRet == true) {
                        desired = deserializer.getfloatPropertyValue("desired");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("unit")) {
                    if (bRet == true) {
                        Unit = new gen.xsd.HumidityUnitType();
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
                } else {
                    if (deserializer.isInFilteringMode("unit") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " unit");
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
