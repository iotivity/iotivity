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
 * This is a set of attributes associated with the temperature.<br>
 * Also, this class defines methods that serialize and/or deserialize contents
 * of XSD complex/list type Temperature.
 * 
 * @since 1.0.0
 */
public class Temperature extends gen.xsd.IdentifiedObject {
    /**
     * [Optional] This attribute represents the name of the door resource. Each
     * resource must have a name field value for the purpose of distinction.
     * (e.g., â€œFreezerâ€�, â€œFridgeâ€�)
     */
    public gen.xsd.String10            Name;

    /**
     * [Optional] Indicates current temperature. (e.g., â€œ88.5â€�)
     */
    public Float                       current;

    /**
     * [Optional] Indicates desired temperature. (e.g., â€œ75.2â€�)
     */
    public Float                       desired;

    /**
     * [Optional] If this attribute is exists, desired temperature needed to be
     * set according to this common difference attribute. Difference between two
     * successive values based on the maximum value should have the
     * characteristics of constant value. For example, assuming that the maximum
     * temperature value is 88 and the value of this attribute is 3. Valid value
     * that can be set is 88, 86, 84, 82.. and should be defined in the form of
     * the arithmetic sequence. Note that attribute value is converted into a
     * negative internally. But for the convenience of the user, attribute value
     * will be displayed as a positive. (e.g., â€œ2.0â€�)
     */
    public Float                       increment;

    /**
     * [Optional] Maximum temperature that can be set on the device. If the set
     * temperature is higher than the maximum set temperature, device will
     * ignore the temperature setting request and attribute will not be updated.
     * (e.g., â€œ90.0â€�)
     */
    public Float                       maximum;

    /**
     * [Optional] Minimum temperature that can be set on the device. If the set
     * temperature is lower than the minimum set temperature, device will ignore
     * the temperature setting request and attribute will not be updated. (e.g.,
     * â€œ64.0â€�)
     */
    public Float                       minimum;

    /**
     * [Mandatory] This indicates the unit type defined for this temperature
     * resource. Note that declaration of the temperature unit enumeration type
     * can be found in the "Attribute Type" declaration. (e.g.,
     * â€œFahrenheitâ€�, â€œCelsiusâ€�)
     */
    public gen.xsd.TemperatureUnitType Unit;

    /**
     * Default constructor of Temperature class.
     * 
     * @since 1.0.0
     */
    public Temperature() {
    }

    /**
     * Destroy method for Temperature class.
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
        return Elements.EN_TEMPERATURE;
    }

    /**
     * Serializes the contents of Temperature object.
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
                if (null != Name) {
                    if (Name.value.length() > 0) {
                        if (true == Name.validateContent()) {
                            bRet = serializer.setStringPropertyValue("name",
                                    Name.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " name");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != current) {
                    bRet = serializer.setfloatPropertyValue("current", current);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " current");
                    }
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
                if (null != increment) {
                    bRet = serializer.setfloatPropertyValue("increment",
                            increment);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " increment");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != maximum) {
                    bRet = serializer.setfloatPropertyValue("maximum", maximum);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " maximum");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != minimum) {
                    bRet = serializer.setfloatPropertyValue("minimum", minimum);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " minimum");
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
     * Deserializes data and prepares the Temperature object contents.
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
                if (true == deserializer.hasProperty("name")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        Name = new gen.xsd.String10();
                        Name.value = deserializer
                                .getStringPropertyValue("name");
                        if (false == Name.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " name");
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
                if (true == deserializer.hasProperty("current")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        current = deserializer.getfloatPropertyValue("current");
                    }
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
                if (true == deserializer.hasProperty("increment")) {
                    if (bRet == true) {
                        increment = deserializer
                                .getfloatPropertyValue("increment");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("maximum")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        maximum = deserializer.getfloatPropertyValue("maximum");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("minimum")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        minimum = deserializer.getfloatPropertyValue("minimum");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("unit")) {
                    if (bRet == true) {
                        Unit = new gen.xsd.TemperatureUnitType();
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
