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
 * This resource has a set of items associated with the level control.<br>
 * Also, this class defines methods that serialize and/or deserialize contents
 * of XSD complex/list type Level.
 * 
 * @since 1.0.0
 */
public class Level extends gen.xsd.Resource {
    /**
     * [Mandatory] Indicate the current level value.
     */
    public Integer current;

    /**
     * [Mandatory] Indicate the desired level value.
     */
    public Integer desired;

    /**
     * [Optional] If this attribute is exists, desired level needed to be set
     * according to this common difference attribute. Difference between two
     * successive values based on the maximum value should have the
     * characteristics of constant value. For example, assuming that the maximum
     * level value is 18 and the value of this attribute is 2. Valid value that
     * can be set is 16, 14, 12, 10.. and should be defined in the form of the
     * arithmetic sequence. Note that attribute value is converted into a
     * negative internally. But for the convenience of the user, attribute value
     * will be displayed as a positive. (e.g., â€œ2â€�)
     */
    public Integer increment;

    /**
     * [Optional] Maximum level that can be set on the device. If the set level
     * value is higher than the maximum set value, device will ignore the level
     * setting request and attribute will not be updated. (e.g., â€œ90â€�)
     */
    public Integer maximum;

    /**
     * [Optional] Minimum level that can be set on the device. If the set level
     * value is lower than the minimum set value, device will ignore the level
     * setting request and attribute will not be updated. (e.g., â€œ64â€�)
     */
    public Integer minimum;

    /**
     * Default constructor of Level class.
     * 
     * @since 1.0.0
     */
    public Level() {
    }

    /**
     * Destroy method for Level class.
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
        return Elements.EN_LEVEL;
    }

    /**
     * Serializes the contents of Level object.
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
                    bRet = serializer.setUnsignedByte("current", current);
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
                    bRet = serializer.setUnsignedByte("desired", desired);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " desired");
                    }
                } else {
                    if (serializer.isInFilteringMode("desired") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " desired");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != increment) {
                    bRet = serializer.setUnsignedByte("increment", increment);
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
                    bRet = serializer.setUnsignedByte("maximum", maximum);
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
                    bRet = serializer.setUnsignedByte("minimum", minimum);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " minimum");
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
     * Deserializes data and prepares the Level object contents.
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
                        current = deserializer.getUnsignedByte("current");
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
                        desired = deserializer.getUnsignedByte("desired");
                    }
                } else {
                    if (deserializer.isInFilteringMode("desired") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " desired");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("increment")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        increment = deserializer.getUnsignedByte("increment");
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
                        maximum = deserializer.getUnsignedByte("maximum");
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
                        minimum = deserializer.getUnsignedByte("minimum");
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
