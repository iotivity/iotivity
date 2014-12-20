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
 * This function set provides an interface for a demand response load control
 * (DRLC) service.<br>
 * Also, this class defines methods that serialize and/or deserialize contents
 * of XSD complex/list type DRLC.
 * 
 * @since 1.0.0
 */
public class DRLC extends gen.xsd.Resource {
    /**
     * [Mandatory] Indicate the DRLC Level.
     */
    public Integer                          drlcLevel;

    /**
     * [Mandatory] The amount of time executed in DRLC.
     */
    public OC.Cm.Serialization.Xsd.TimeType Duration;

    /**
     * [Mandatory] Indicate the time when the DRLC is started.
     */
    public OC.Cm.Serialization.Xsd.TimeType DrlcStartTime;

    /**
     * [Mandatory] Indicate the override On/Off status.
     */
    public gen.xsd.OnType                   Override;

    /**
     * Default constructor of DRLC class.
     * 
     * @since 1.0.0
     */
    public DRLC() {
    }

    /**
     * Destroy method for DRLC class.
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
        return Elements.EN_DRLC;
    }

    /**
     * Serializes the contents of DRLC object.
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
                if (null != drlcLevel) {
                    bRet = serializer.setUnsignedByte("drlcLevel", drlcLevel);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " drlcLevel");
                    }
                } else {
                    if (serializer.isInFilteringMode("drlcLevel") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " drlcLevel");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (null != Duration) {
                    if (Duration.value.length() > 0) {
                        if (true == Duration.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "duration", Duration.value);
                        } else
                            bRet = false;
                    } else
                        bRet = false;
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " duration");
                } else {
                    if (serializer.isInFilteringMode("duration") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " duration");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (null != DrlcStartTime) {
                    if (DrlcStartTime.value.length() > 0) {
                        if (true == DrlcStartTime.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "drlcStartTime", DrlcStartTime.value);
                        } else
                            bRet = false;
                    } else
                        bRet = false;
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " drlcStartTime");
                } else {
                    if (serializer.isInFilteringMode("drlcStartTime") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " drlcStartTime");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (null != Override) {
                    if (Override.value.length() > 0) {
                        if (true == Override.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "override", Override.value);
                        } else
                            bRet = false;
                    } else
                        bRet = false;
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " override");
                } else {
                    if (serializer.isInFilteringMode("override") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " override");
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
     * Deserializes data and prepares the DRLC object contents.
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
                if (true == deserializer.hasProperty("drlcLevel")) {
                    if (bRet == true) {
                        drlcLevel = deserializer.getUnsignedByte("drlcLevel");
                    }
                } else {
                    if (deserializer.isInFilteringMode("drlcLevel") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " drlcLevel");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("duration")) {
                    if (bRet == true) {
                        Duration = new OC.Cm.Serialization.Xsd.TimeType();
                        Duration.value = deserializer
                                .getStringPropertyValue("duration");
                        if (false == Duration.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " duration");
                            bRet = false;
                        } else
                            bRet = true;
                    }
                } else {
                    if (deserializer.isInFilteringMode("duration") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " duration");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("drlcStartTime")) {
                    if (bRet == true) {
                        DrlcStartTime = new OC.Cm.Serialization.Xsd.TimeType();
                        DrlcStartTime.value = deserializer
                                .getStringPropertyValue("drlcStartTime");
                        if (false == DrlcStartTime.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " drlcStartTime");
                            bRet = false;
                        } else
                            bRet = true;
                    }
                } else {
                    if (deserializer.isInFilteringMode("drlcStartTime") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " drlcStartTime");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("override")) {
                    if (bRet == true) {
                        Override = new gen.xsd.OnType();
                        Override.value = deserializer
                                .getStringPropertyValue("override");
                        if (false == Override.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " override");
                            bRet = false;
                        } else
                            bRet = true;
                    }
                } else {
                    if (deserializer.isInFilteringMode("override") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " override");
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
