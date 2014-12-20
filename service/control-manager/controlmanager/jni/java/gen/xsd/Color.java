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
 * complex/list type Color.
 * 
 * @since 1.0.0
 */
public class Color extends OC.Cm.Serialization.ISerializable {
    /**
     * [Mandatory] Indicate the currently set hue level.
     */
    public Integer          hue;

    /**
     * [Mandatory] Indicate the relative hue level based on current hue level.
     */
    public Integer          relativeHue;

    /**
     * [Mandatory] Indicate the currently set saturation level.
     */
    public Integer          saturation;

    /**
     * [Mandatory] Indicate the relative saturation level based on current
     * saturation level.
     */
    public Integer          relativeSaturation;

    /**
     * [Mandatory] Indicate the currently set chromaticity X level.
     */
    public Integer          colorX;

    /**
     * [Mandatory] Indicate the currently set chromaticity Y level.
     */
    public Integer          colorY;

    /**
     * [Mandatory] Indicate the currently set color temperature.
     */
    public Integer          colorTemperature;

    /**
     * [Mandatory] Variable of String20 object.
     */
    public gen.xsd.String20 ColorMode;

    /**
     * Default constructor of Color class.
     * 
     * @since 1.0.0
     */
    public Color() {
    }

    /**
     * Destroy method for Color class.
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
        return Elements.EN_COLOR;
    }

    /**
     * Serializes the contents of Color object.
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
            {
                boolean bRet = true;
                if (null != hue) {
                    bRet = serializer.setUnsignedByte("hue", hue);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " hue");
                    }
                } else {
                    if (serializer.isInFilteringMode("hue") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " hue");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (null != relativeHue) {
                    bRet = serializer.setShortPropertyValue("relativeHue",
                            relativeHue);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " relativeHue");
                    }
                } else {
                    if (serializer.isInFilteringMode("relativeHue") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " relativeHue");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (null != saturation) {
                    bRet = serializer.setUnsignedByte("saturation", saturation);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " saturation");
                    }
                } else {
                    if (serializer.isInFilteringMode("saturation") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " saturation");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (null != relativeSaturation) {
                    bRet = serializer.setShortPropertyValue(
                            "relativeSaturation", relativeSaturation);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " relativeSaturation");
                    }
                } else {
                    if (serializer.isInFilteringMode("relativeSaturation") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " relativeSaturation");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (null != colorX) {
                    bRet = serializer.setUnsignedShort("colorX", colorX);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " colorX");
                    }
                } else {
                    if (serializer.isInFilteringMode("colorX") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " colorX");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (null != colorY) {
                    bRet = serializer.setUnsignedShort("colorY", colorY);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " colorY");
                    }
                } else {
                    if (serializer.isInFilteringMode("colorY") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " colorY");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (null != colorTemperature) {
                    bRet = serializer.setUnsignedShort("colorTemperature",
                            colorTemperature);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " colorTemperature");
                    }
                } else {
                    if (serializer.isInFilteringMode("colorTemperature") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " colorTemperature");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != ColorMode) {
                    if (ColorMode.value.length() > 0) {
                        if (true == ColorMode.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "colorMode", ColorMode.value);
                        } else
                            bRet = false;
                    } else
                        bRet = false;
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " colorMode");
                } else {
                    if (serializer.isInFilteringMode("colorMode") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " colorMode");
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
     * Deserializes data and prepares the Color object contents.
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
                if (true == deserializer.hasProperty("hue")) {
                    if (bRet == true) {
                        hue = deserializer.getUnsignedByte("hue");
                    }
                } else {
                    if (deserializer.isInFilteringMode("hue") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " hue");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("relativeHue")) {
                    if (bRet == true) {
                        relativeHue = deserializer
                                .getShortPropertyValue("relativeHue");
                    }
                } else {
                    if (deserializer.isInFilteringMode("relativeHue") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " relativeHue");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("saturation")) {
                    if (bRet == true) {
                        saturation = deserializer.getUnsignedByte("saturation");
                    }
                } else {
                    if (deserializer.isInFilteringMode("saturation") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " saturation");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("relativeSaturation")) {
                    if (bRet == true) {
                        relativeSaturation = deserializer
                                .getShortPropertyValue("relativeSaturation");
                    }
                } else {
                    if (deserializer.isInFilteringMode("relativeSaturation") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " relativeSaturation");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("chromaticityX")) {
                    if (bRet == true) {
                        colorX = deserializer.getUnsignedShort("chromaticityX");
                    }
                } else {
                    if (deserializer.isInFilteringMode("chromaticityX") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " chromaticityX");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("chromaticityY")) {
                    if (bRet == true) {
                        colorY = deserializer.getUnsignedShort("chromaticityY");
                    }
                } else {
                    if (deserializer.isInFilteringMode("chromaticityY") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " chromaticityY");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("colorTemperature")) {
                    if (bRet == true) {
                        colorTemperature = deserializer
                                .getUnsignedShort("colorTemperature");
                    }
                } else {
                    if (deserializer.isInFilteringMode("colorTemperature") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " colorTemperature");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            /*
             * { boolean bRet = true; if (true ==
             * deserializer.hasProperty("colorMode")) {
             * if(deserializer.validateMandatory() == false) bRet = false;
             * if(bRet == true) { ColorMode = new gen.xsd.String20();
             * ColorMode.value =
             * deserializer.getStringPropertyValue("colorMode"); if (false ==
             * ColorMode.validateContent()) {
             * deserializer.setErrorMessage("Parameter Contains Invalid Value : "
             * + " colorMode"); bRet = false ; } else bRet = true; } } else { if
             * (deserializer.isInFilteringMode("colorMode") == false) {
             * deserializer.setErrorMessage("Mandatory Parameter not found : " +
             * " colorMode"); bRet = false; } else bRet = true; } if (retValue
             * == true) retValue = bRet; }
             */

        } catch (Exception e) {
            return false;
        }
        return retValue;

    }
}
