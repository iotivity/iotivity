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
 * This is a collection of common modes of the device. List of available modes,
 * currently selected mode are queryable.<br>
 * Also, this class defines methods that serialize and/or deserialize contents
 * of XSD complex/list type Mode.
 * 
 * @since 1.0.0
 */
public class Mode extends gen.xsd.Resource {
    /**
     * [Mandatory] Contains a set of available modes. Available in both the
     * singular and plural forms expression. In case of plural forms, each
     * option can be expressed in array of string. If there exists multiple
     * group of mode functions, the name of modes is comprised of two part as
     * follows: â€œModeName_Valueâ€� (e.g., â€œOpmode_Coolâ€�, â€œOpmode_Dryâ€�,
     * â€œOpmode_Windâ€�, â€œComode_Speedâ€�, â€œComode_Sleepâ€�,
     * â€œComode_Quietâ€�)
     */
    public gen.xsd.ArrayOfString SupportedModes;

    /**
     * [Mandatory] Indicate currently selected modes. If there exists
     * â€œsupportedModesâ€�, the value of â€œmodesâ€� should be one of
     * â€œsupportedModesâ€� list. Available in both the singular and plural
     * forms expression. In case of plural forms, each option can be expressed
     * in array of string. (e.g., â€œOpmode_Coolâ€�, â€œComode_Quietâ€�)
     */
    public gen.xsd.ArrayOfString Modes;

    /**
     * Default constructor of Mode class.
     * 
     * @since 1.0.0
     */
    public Mode() {
    }

    /**
     * Destroy method for Mode class.
     * 
     * @since 1.0.0
     */
    @Override
    public void destroy() {
        super.destroy();
        if (null != SupportedModes) {
            SupportedModes.destroy();
        }
        if (null != Modes) {
            Modes.destroy();
        }
    }

    /**
     * Returns name of the serializable.
     * 
     * @since 1.0.0
     */
    @Override
    public String getElementName() {
        return Elements.EN_MODE;
    }

    /**
     * Serializes the contents of Mode object.
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
                if (null != SupportedModes) {
                    SupportedModes.setInstanceName("supportedModes");
                    bRet = SupportedModes.serializeContents(serializer);
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " supportedModes");
                } else {
                    if (serializer.isInFilteringMode("supportedModes") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " supportedModes");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (null != Modes) {
                    Modes.setInstanceName("modes");
                    bRet = Modes.serializeContents(serializer);
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " modes");
                } else {
                    if (serializer.isInFilteringMode("modes") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " modes");
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
     * Deserializes data and prepares the Mode object contents.
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
                if (true == deserializer.hasProperty("supportedModes")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        SupportedModes = new gen.xsd.ArrayOfString();
                        SupportedModes.setInstanceName("supportedModes");
                        if (false == SupportedModes
                                .deSerializeContents(deserializer)) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " supportedModes");
                            bRet = false;
                        } else
                            bRet = true;
                    }
                } else {
                    if (deserializer.isInFilteringMode("supportedModes") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " supportedModes");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("modes")) {
                    if (bRet == true) {
                        Modes = new gen.xsd.ArrayOfString();
                        Modes.setInstanceName("modes");
                        if (false == Modes.deSerializeContents(deserializer)) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " modes");
                            bRet = false;
                        } else
                            bRet = true;
                    }
                } else {
                    if (deserializer.isInFilteringMode("modes") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " modes");
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
