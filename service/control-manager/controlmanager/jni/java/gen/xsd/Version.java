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
 * This resource provides a version information of the device.<br>
 * Also, this class defines methods that serialize and/or deserialize contents
 * of XSD complex/list type Version.
 * 
 * @since 1.0.0
 */
public class Version extends gen.xsd.IdentifiedObject {
    /**
     * [Mandatory] : Indicate the version type. (e.g., â€œHardwareâ€�,
     * â€œSoftwareâ€�, â€œFirmwareâ€�, â€œProtocolâ€�,
     * â€œManufacturer_Specificâ€�)
     */
    public gen.xsd.VersionType Type;

    /**
     * [Optional] Indicate the manufacturer-specific version type if above
     * â€œtypeâ€� value is â€œManufacturer_Specificâ€�.
     */
    public gen.xsd.String50    Name;

    /**
     * [Mandatory] Indicate the version number. (e.g., â€œ1.1.5â€�)
     */
    public gen.xsd.String20    Number;

    /**
     * [Optional] Indicate whether the new version of software is available.
     */
    public Boolean             newVersionAvailable;

    /**
     * [Optional] Indicate the user agreement about software update. If this
     * value is true, the device will start software update over the network.
     */
    public Boolean             userAgreement;

    /**
     * Default constructor of Version class.
     * 
     * @since 1.0.0
     */
    public Version() {
    }

    /**
     * Destroy method for Version class.
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
        return Elements.EN_VERSION;
    }

    /**
     * Serializes the contents of Version object.
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
                if (null != Type) {
                    if (Type.value.length() > 0) {
                        if (true == Type.validateContent()) {
                            bRet = serializer.setStringPropertyValue("type",
                                    Type.value);
                        } else
                            bRet = false;
                    } else
                        bRet = false;
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " type");
                } else {
                    if (serializer.isInFilteringMode("type") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " type");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

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
                if (null != Number) {
                    if (Number.value.length() > 0) {
                        if (true == Number.validateContent()) {
                            bRet = serializer.setStringPropertyValue("number",
                                    Number.value);
                        } else
                            bRet = false;
                    } else
                        bRet = false;
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " number");
                } else {
                    if (serializer.isInFilteringMode("number") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " number");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != newVersionAvailable) {
                    bRet = serializer.setBoolPropertyValue(
                            "newVersionAvailable", newVersionAvailable);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " newVersionAvailable");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (null != userAgreement) {
                    bRet = serializer.setBoolPropertyValue("userAgreement",
                            userAgreement);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " userAgreement");
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
     * Deserializes data and prepares the Version object contents.
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
                if (true == deserializer.hasProperty("type")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        Type = new gen.xsd.VersionType();
                        Type.value = deserializer
                                .getStringPropertyValue("type");
                        if (false == Type.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " type");
                            bRet = false;
                        } else
                            bRet = true;
                    }
                } else {
                    if (deserializer.isInFilteringMode("type") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " type");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("name")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        Name = new gen.xsd.String50();
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
                if (true == deserializer.hasProperty("number")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        Number = new gen.xsd.String20();
                        Number.value = deserializer
                                .getStringPropertyValue("number");
                        if (false == Number.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " number");
                            bRet = false;
                        } else
                            bRet = true;
                    }
                } else {
                    if (deserializer.isInFilteringMode("number") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " number");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("newVersionAvailable")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        newVersionAvailable = deserializer
                                .getBoolPropertyValue("newVersionAvailable");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("userAgreement")) {
                    if (bRet == true) {
                        userAgreement = deserializer
                                .getBoolPropertyValue("userAgreement");
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
