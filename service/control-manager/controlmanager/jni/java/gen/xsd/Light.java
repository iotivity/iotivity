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
 * This resource has a set of items associated with the light.<br>
 * Also, this class defines methods that serialize and/or deserialize contents
 * of XSD complex/list type Light.
 * 
 * @since 1.0.0
 */
public class Light extends gen.xsd.Resource {
    /**
     * [Optional] Indicate the power status of light device
     */
    public gen.xsd.OnType   LightPower;

    /**
     * [Mandatory] Indicate currently set dimming level. If the set dimming
     * level is higher than the number of dimming levels, device will ignore the
     * dimming level setting request and attribute will not be updated. (e.g.,
     * â€œ10â€�)
     */
    public Integer          dimmingLevel;

    /**
     * [Mandatory] Indicate maximum number of dimming levels. (e.g., â€œ20â€�)
     */
    public Integer          maxDimmingLevel;

    /**
     * [Optional] Indicate the relative dimming level based on current dimming
     * level.
     */
    public Integer          relativeDimmingLevel;

    /**
     * [Optional] Indicate the time interval to reach the newly set dimming
     * level. Time unit is second.
     */
    public Integer          transitionTime;

    /**
     * [Optional] Indicate the currently selected pattern ID.
     */
    public gen.xsd.String50 ActivatePatternID;

    /**
     * [Optional] Indicate the pattern ID executed when light bulb turns on.
     */
    public gen.xsd.String50 OnPatternID;

    /**
     * [Optional] Variable of Color object.
     */
    public gen.xsd.Color    color;

    /**
     * Default constructor of Light class.
     * 
     * @since 1.0.0
     */
    public Light() {
    }

    /**
     * Destroy method for Light class.
     * 
     * @since 1.0.0
     */
    @Override
    public void destroy() {
        super.destroy();
        if (null != color) {
            color.destroy();
        }
    }

    /**
     * Returns name of the serializable.
     * 
     * @since 1.0.0
     */
    @Override
    public String getElementName() {
        return Elements.EN_LIGHT;
    }

    /**
     * Serializes the contents of Light object.
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
                if (null != LightPower) {
                    if (LightPower.value.length() > 0) {
                        if (true == LightPower.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "lightPower", LightPower.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " lightPower");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (null != dimmingLevel) {
                    bRet = serializer.setUnsignedByte("dimmingLevel",
                            dimmingLevel);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " dimmingLevel");
                    }
                } else {
                    if (serializer.isInFilteringMode("dimmingLevel") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " dimmingLevel");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != maxDimmingLevel) {
                    bRet = serializer.setUnsignedByte("maxDimmingLevel",
                            maxDimmingLevel);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " maxDimmingLevel");
                    }
                } else {
                    if (serializer.isInFilteringMode("maxDimmingLevel") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " maxDimmingLevel");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (null != relativeDimmingLevel) {
                    bRet = serializer.setShortPropertyValue(
                            "relativeDimmingLevel", relativeDimmingLevel);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " relativeDimmingLevel");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (null != transitionTime) {
                    bRet = serializer.setUnsignedShort("transitionTime",
                            transitionTime);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " transitionTime");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (null != ActivatePatternID) {
                    if (ActivatePatternID.value.length() > 0) {
                        if (true == ActivatePatternID.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "activatePatternID",
                                    ActivatePatternID.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " activatePatternID");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (null != OnPatternID) {
                    if (OnPatternID.value.length() > 0) {
                        if (true == OnPatternID.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "onPatternID", OnPatternID.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " onPatternID");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (null != color) {
                    bRet = color.serialize(serializer);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " Color");
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
     * Deserializes data and prepares the Light object contents.
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
                if (true == deserializer.hasProperty("lightPower")) {
                    if (bRet == true) {
                        LightPower = new gen.xsd.OnType();
                        LightPower.value = deserializer
                                .getStringPropertyValue("lightPower");
                        if (false == LightPower.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " lightPower");
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
                if (true == deserializer.hasProperty("dimmingLevel")) {
                    if (bRet == true) {
                        dimmingLevel = deserializer
                                .getUnsignedByte("dimmingLevel");
                    }
                } else {
                    if (deserializer.isInFilteringMode("dimmingLevel") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " dimmingLevel");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("maxDimmingLevel")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        maxDimmingLevel = deserializer
                                .getUnsignedByte("maxDimmingLevel");
                    }
                } else {
                    if (deserializer.isInFilteringMode("maxDimmingLevel") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " maxDimmingLevel");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("relativeDimmingLevel")) {
                    if (bRet == true) {
                        relativeDimmingLevel = deserializer
                                .getShortPropertyValue("relativeDimmingLevel");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("transitionTime")) {
                    if (bRet == true) {
                        transitionTime = deserializer
                                .getUnsignedShort("transitionTime");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("activatePatternID")) {
                    if (bRet == true) {
                        ActivatePatternID = new gen.xsd.String50();
                        ActivatePatternID.value = deserializer
                                .getStringPropertyValue("activatePatternID");
                        if (false == ActivatePatternID.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " activatePatternID");
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
                if (true == deserializer.hasProperty("onPatternID")) {
                    if (bRet == true) {
                        OnPatternID = new gen.xsd.String50();
                        OnPatternID.value = deserializer
                                .getStringPropertyValue("onPatternID");
                        if (false == OnPatternID.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " onPatternID");
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
                if (true == deserializer.hasGroupProperty("Color")) {
                    if (bRet == true) {
                        color = new gen.xsd.Color();
                        bRet = color.deSerialize(deserializer);
                    }
                    if (false == bRet)
                        deserializer.setErrorMessage("Failed To deserialize : "
                                + " Color");
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
