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
 * This resource provides an information about access points located near the
 * device.<br>
 * Also, this class defines methods that serialize and/or deserialize contents
 * of XSD complex/list type AccessPoint.
 * 
 * @since 1.0.0
 */
public class AccessPoint extends gen.xsd.IdentifiedObject {
    /**
     * [Mandatory] The channel that the access point is currently using for
     * communications. (e.g., â€œ1â€�, â€œ5â€�, â€œ13â€�)
     */
    public Integer          channel;

    /**
     * [Mandatory] Service set identifier(SSID) indicates the name of an
     * available wireless network. (e.g., â€œlinksysâ€�, â€œNETGEARâ€�,
     * â€œdefaultâ€�)
     */
    public gen.xsd.String50 Ssid;

    /**
     * [Mandatory] The access point's MAC address. (e.g.,
     * â€œ00:0B:FC:FF:B2:58â€�)
     */
    public gen.xsd.String20 MacAddress;

    /**
     * [Mandatory] The maximum rate at which the client adapter can transfer
     * data with an access point. The supported rates of both the client adapter
     * and the access point are examined, and the highest rate that they have in
     * common is the one that is used. (e.g., â€œ11â€�, â€œ54â€�, â€œ150â€�,
     * â€œ300â€�)
     */
    public Integer          maxRate;

    /**
     * [Mandatory] The received signal strength indicator(RSSI) is a measure of
     * signal strength in decibels with respect to milliwatts(dBm). (e.g.,
     * â€œ-54â€�, â€œ-74â€�, â€œ-85â€�)
     */
    public Integer          rssi;

    /**
     * [Optional] Indicate the security type that the Wi-Fi adapter uses. (e.g.,
     * â€œNoneâ€�, â€œWEPâ€�, â€œWPA-PSKâ€�, â€œWPA2-PSKâ€�)
     */
    public gen.xsd.String20 SecurityType;

    /**
     * [Optional] Indicate the security type that the Wi-Fi adapter uses. (e.g.,
     * â€œNoneâ€�, â€œWEPâ€�, â€œWPA-PSKâ€�, â€œWPA2-PSKâ€�)
     */
    public gen.xsd.String20 EncryptionType;

    /**
     * Default constructor of AccessPoint class.
     * 
     * @since 1.0.0
     */
    public AccessPoint() {
    }

    /**
     * Destroy method for AccessPoint class.
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
        return Elements.EN_ACCESSPOINT;
    }

    /**
     * Serializes the contents of AccessPoint object.
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
                if (null != channel) {
                    bRet = serializer.setUnsignedByte("channel", channel);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " channel");
                    }
                } else {
                    if (serializer.isInFilteringMode("channel") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " channel");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != Ssid) {
                    if (Ssid.value.length() > 0) {
                        if (true == Ssid.validateContent()) {
                            bRet = serializer.setStringPropertyValue("ssid",
                                    Ssid.value);
                        } else
                            bRet = false;
                    } else
                        bRet = false;
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " ssid");
                } else {
                    if (serializer.isInFilteringMode("ssid") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " ssid");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != MacAddress) {
                    if (MacAddress.value.length() > 0) {
                        if (true == MacAddress.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "macAddress", MacAddress.value);
                        } else
                            bRet = false;
                    } else
                        bRet = false;
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " macAddress");
                } else {
                    if (serializer.isInFilteringMode("macAddress") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " macAddress");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != maxRate) {
                    bRet = serializer.setUnsignedByte("maxRate", maxRate);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " maxRate");
                    }
                } else {
                    if (serializer.isInFilteringMode("maxRate") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " maxRate");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != rssi) {
                    bRet = serializer.setBytePropertyValue("rssi", rssi);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " rssi");
                    }
                } else {
                    if (serializer.isInFilteringMode("rssi") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " rssi");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != SecurityType) {
                    if (SecurityType.value.length() > 0) {
                        if (true == SecurityType.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "securityType", SecurityType.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " securityType");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != EncryptionType) {
                    if (EncryptionType.value.length() > 0) {
                        if (true == EncryptionType.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "encryptionType", EncryptionType.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " encryptionType");
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
     * Deserializes data and prepares the AccessPoint object contents.
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
                if (true == deserializer.hasProperty("channel")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        channel = deserializer.getUnsignedByte("channel");
                    }
                } else {
                    if (deserializer.isInFilteringMode("channel") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " channel");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("ssid")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        Ssid = new gen.xsd.String50();
                        Ssid.value = deserializer
                                .getStringPropertyValue("ssid");
                        if (false == Ssid.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " ssid");
                            bRet = false;
                        } else
                            bRet = true;
                    }
                } else {
                    if (deserializer.isInFilteringMode("ssid") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " ssid");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("macAddress")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        MacAddress = new gen.xsd.String20();
                        MacAddress.value = deserializer
                                .getStringPropertyValue("macAddress");
                        if (false == MacAddress.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " macAddress");
                            bRet = false;
                        } else
                            bRet = true;
                    }
                } else {
                    if (deserializer.isInFilteringMode("macAddress") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " macAddress");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("maxRate")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        maxRate = deserializer.getUnsignedByte("maxRate");
                    }
                } else {
                    if (deserializer.isInFilteringMode("maxRate") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " maxRate");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("rssi")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        rssi = deserializer.getBytePropertyValue("rssi");
                    }
                } else {
                    if (deserializer.isInFilteringMode("rssi") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " rssi");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("securityType")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        SecurityType = new gen.xsd.String20();
                        SecurityType.value = deserializer
                                .getStringPropertyValue("securityType");
                        if (false == SecurityType.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " securityType");
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
                if (true == deserializer.hasProperty("encryptionType")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        EncryptionType = new gen.xsd.String20();
                        EncryptionType.value = deserializer
                                .getStringPropertyValue("encryptionType");
                        if (false == EncryptionType.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " encryptionType");
                            bRet = false;
                        } else
                            bRet = true;
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
