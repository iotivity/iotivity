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
 * This function set provides an information and specification of the device.<br>
 * Also, this class defines methods that serialize and/or deserialize contents
 * of XSD complex/list type Information.
 * 
 * @since 1.0.0
 */
public class Information extends gen.xsd.Resource {
    /**
     * [Optional] This is a textual description of the device. In general, the
     * model number will be displayed in this field. (e.g.,
     * â€œMichelangelo,Lenorard 13Kâ€�)
     */
    public gen.xsd.String100     Description;

    /**
     * [Optional] Indicate the manufacturer name of the device. (e.g.,
     * â€œSamsung Electronicsâ€�)
     */
    public gen.xsd.String20      Manufacturer;

    /**
     * [Optional] Variable of String30 object.
     */
    public gen.xsd.String30      DeviceSubType;

    /**
     * [Optional] Indicate the deviceâ€™s model ID. (e.g., â€œSEC-SHWM110Sâ€�)
     */
    public gen.xsd.String20      ModelID;

    /**
     * [Optional] Indicate the deviceâ€™s serial number. (e.g.,
     * â€œZS28401823NQ1763â€�)
     */
    public gen.xsd.String50      SerialNumber;

    /**
     * [Optional] Indicate sales location of the device. Note that declaration
     * of the location enumeration type can be found in the "Attribute Type"
     * declaration. (e.g., â€œWorld_Wideâ€�, â€œAsiaâ€�, â€œNorth_Americaâ€�,
     * â€œEuropeâ€�)
     */
    public gen.xsd.LocationType  SalesLocation;

    /**
     * [Optional] Variable of Specification object.
     */
    public gen.xsd.Specification specification;

    /**
     * [Optional] Variable of Versions object.
     */
    public gen.xsd.Versions      versions;

    /**
     * Default constructor of Information class.
     * 
     * @since 1.0.0
     */
    public Information() {
    }

    /**
     * Destroy method for Information class.
     * 
     * @since 1.0.0
     */
    @Override
    public void destroy() {
        super.destroy();
        if (null != specification) {
            specification.destroy();
        }
        if (null != versions) {
            versions.destroy();
        }
    }

    /**
     * Returns name of the serializable.
     * 
     * @since 1.0.0
     */
    @Override
    public String getElementName() {
        return Elements.EN_INFORMATION;
    }

    /**
     * Serializes the contents of Information object.
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
                if (null != Description) {
                    if (Description.value.length() > 0) {
                        if (true == Description.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "description", Description.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " description");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != Manufacturer) {
                    if (Manufacturer.value.length() > 0) {
                        if (true == Manufacturer.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "manufacturer", Manufacturer.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " manufacturer");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != DeviceSubType) {
                    if (DeviceSubType.value.length() > 0) {
                        if (true == DeviceSubType.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "deviceSubType", DeviceSubType.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " deviceSubType");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != ModelID) {
                    if (ModelID.value.length() > 0) {
                        if (true == ModelID.validateContent()) {
                            bRet = serializer.setStringPropertyValue("modelID",
                                    ModelID.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " modelID");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != SerialNumber) {
                    if (SerialNumber.value.length() > 0) {
                        if (true == SerialNumber.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "serialNumber", SerialNumber.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " serialNumber");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != SalesLocation) {
                    if (SalesLocation.value.length() > 0) {
                        if (true == SalesLocation.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "salesLocation", SalesLocation.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " salesLocation");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (null != specification) {
                    bRet = specification.serialize(serializer);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " Specification");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (null != versions) {
                        bRet = versions.serialize(serializer);
                        if (false == bRet) {
                            serializer
                                    .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                            + " Versions");
                        }
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            }
        } catch (Exception e) {
            return false;
        }
        return retValue;

    }

    /**
     * Deserializes data and prepares the Information object contents.
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
                if (true == deserializer.hasProperty("description")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        Description = new gen.xsd.String100();
                        Description.value = deserializer
                                .getStringPropertyValue("description");
                        if (false == Description.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " description");
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
                if (true == deserializer.hasProperty("manufacturer")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        Manufacturer = new gen.xsd.String20();
                        Manufacturer.value = deserializer
                                .getStringPropertyValue("manufacturer");
                        if (false == Manufacturer.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " manufacturer");
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
                if (true == deserializer.hasProperty("deviceSubType")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        DeviceSubType = new gen.xsd.String30();
                        DeviceSubType.value = deserializer
                                .getStringPropertyValue("deviceSubType");
                        if (false == DeviceSubType.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " deviceSubType");
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
                if (true == deserializer.hasProperty("modelID")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        ModelID = new gen.xsd.String20();
                        ModelID.value = deserializer
                                .getStringPropertyValue("modelID");
                        if (false == ModelID.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " modelID");
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
                if (true == deserializer.hasProperty("serialNumber")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        SerialNumber = new gen.xsd.String50();
                        SerialNumber.value = deserializer
                                .getStringPropertyValue("serialNumber");
                        if (false == SerialNumber.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " serialNumber");
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
                if (true == deserializer.hasProperty("salesLocation")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        SalesLocation = new gen.xsd.LocationType();
                        SalesLocation.value = deserializer
                                .getStringPropertyValue("salesLocation");
                        if (false == SalesLocation.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " salesLocation");
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
                if (true == deserializer.hasGroupProperty("Specification")) {
                    if (bRet == true) {
                        specification = new gen.xsd.Specification();
                        bRet = specification.deSerialize(deserializer);
                    }
                    if (false == bRet)
                        deserializer.setErrorMessage("Failed To deserialize : "
                                + " Specification");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (deserializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (true == deserializer.hasGroupProperty("Versions")) {
                        if (bRet == true) {
                            versions = new gen.xsd.Versions();
                            bRet = versions.deSerialize(deserializer);
                        }
                        if (false == bRet)
                            deserializer
                                    .setErrorMessage("Failed To deserialize : "
                                            + " Versions");
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            } else {
                if (true == deserializer.hasProperty("Versions")) {
                    retValue = false;
                    deserializer
                            .setErrorMessage("Additional data to deserialize: Versions");
                }
            }
        } catch (Exception e) {
            return false;
        }
        return retValue;

    }
}
