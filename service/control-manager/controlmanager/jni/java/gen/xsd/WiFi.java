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
 * This resource provides an information of the WiFi connectivity. <br>
 * Also, this class defines methods that serialize and/or deserialize contents
 * of XSD complex/list type WiFi.
 * 
 * @since 1.0.0
 */
public class WiFi extends gen.xsd.Resource {
    /**
     * [Optional] Defines the role of the Wi-Fi adapter. Note that declaration
     * of the Wi-Fi operation enumeration type can be found in the
     * "Attribute Type" declaration. (e.g., â€œInfrastructureâ€�, â€œSoft APâ€�,
     * â€œWiFi Directâ€�)
     */
    public gen.xsd.WifiOperationType OperationType;

    /**
     * [Optional] The channel that the Wi-Fi adapter is currently using for
     * communications. (e.g., â€œ1â€�, â€œ5â€�, â€œ13â€�)
     */
    public Integer                   channel;

    /**
     * [Optional] The SSID to connect the Wi-Fi network and it can be obtained
     * through the ApList resources. The SSID length may be up to 32 characters
     * long. If it is longer, all exceeding characters will be cut off. (e.g.,
     * â€œlinksysâ€�, â€œNETGEARâ€�, â€œdefaultâ€�)
     */
    public gen.xsd.String50          Ssid;

    /**
     * [Optional] The received signal strength indicator(RSSI) is a measure of
     * signal strength in decibels with respect to milliwatts(dBm). (e.g.,
     * â€œ-54â€�, â€œ-74â€�, â€œ-85â€�)
     */
    public Integer                   rssi;

    /**
     * [Optional] Indicate the security type that the Wi-Fi adapter uses. (e.g.,
     * â€œNoneâ€�, â€œWEPâ€�, â€œWPA-PSKâ€�, â€œWPA2-PSKâ€�)
     */
    public gen.xsd.String20          SecurityType;

    /**
     * [Optional] Indicate the security type that the Wi-Fi adapter uses. (e.g.,
     * â€œNoneâ€�, â€œWEPâ€�, â€œWPA-PSKâ€�, â€œWPA2-PSKâ€�)
     */
    public gen.xsd.String20          EncryptionType;

    /**
     * [Optional] The security key to connect to the access point. If
     * securityType attribute has a â€œNoneâ€� as its value, this attribute does
     * not affect security settings for communication with access points. The
     * security key length may be up to 64 characters long. If it is longer, all
     * exceeding characters will be cut off. Note that this attribute is
     * write-only. (e.g., â€œpassphraseâ€�)
     */
    public gen.xsd.String100         SecurityKey;

    /**
     * [Optional] If the selected WifiOperationType is Wi-Fi Direct and adapter
     * supports Wi-Fi Direct, this attribute will be shown. Attribute indicates
     * the Wi-Fi direct connection status and device will accept the connection
     * request from the other Wi-Fi direct device without any confirmation. Note
     * that declaration of the event enumeration type can be found in the
     * "Attribute Type" declaration. (e.g., â€œStartedâ€�, â€œConnectedâ€�,
     * â€œDisconnectedâ€�)
     */
    public gen.xsd.WifiEventType     WifiDirectStatus;

    /**
     * [Optional] If the selected WifiOperationType is Infrastructure and
     * adapter supports WPS, this attribute will be shown. Attribute indicates
     * whether the WPS is enabled or not on this network adapter. If this option
     * is enabled, device will try to connect to every WPS enable AP available
     * in environment and will take precedence over all other network
     * preferences. (e.g., â€œTrueâ€�, â€œFalseâ€�)
     */
    public Boolean                   wpsEnabled;

    /**
     * [Optional] If the selected WifiOperationType is Infrastructure and
     * adapter supports WPS, this attribute will be shown. Attribute indicates
     * the WPS connection. Note that declaration of the event enumeration type
     * can be found in the "Attribute Type" declaration. (e.g., â€œStartedâ€�,
     * â€œConnectedâ€�, â€œDisconnectedâ€�)
     */
    public gen.xsd.WifiEventType     WpsStatus;

    /**
     * [Optional] Variable of AccessPointsLink object.
     */
    public gen.xsd.AccessPointsLink  accessPointsLink;

    /**
     * Default constructor of WiFi class.
     * 
     * @since 1.0.0
     */
    public WiFi() {
    }

    /**
     * Destroy method for WiFi class.
     * 
     * @since 1.0.0
     */
    @Override
    public void destroy() {
        super.destroy();
        if (null != accessPointsLink) {
            accessPointsLink.destroy();
        }
    }

    /**
     * Returns name of the serializable.
     * 
     * @since 1.0.0
     */
    @Override
    public String getElementName() {
        return Elements.EN_WIFI;
    }

    /**
     * Serializes the contents of WiFi object.
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
                if (null != OperationType) {
                    if (OperationType.value.length() > 0) {
                        if (true == OperationType.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "operationType", OperationType.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " operationType");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (null != channel) {
                    bRet = serializer.setUnsignedByte("channel", channel);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " channel");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (null != Ssid) {
                    if (Ssid.value.length() > 0) {
                        if (true == Ssid.validateContent()) {
                            bRet = serializer.setStringPropertyValue("ssid",
                                    Ssid.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " ssid");
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
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
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
            {
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
            {
                boolean bRet = true;
                if (null != SecurityKey) {
                    if (SecurityKey.value.length() > 0) {
                        if (true == SecurityKey.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "securityKey", SecurityKey.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " securityKey");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != WifiDirectStatus) {
                    if (WifiDirectStatus.value.length() > 0) {
                        if (true == WifiDirectStatus.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "wifiDirectStatus", WifiDirectStatus.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " wifiDirectStatus");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (null != wpsEnabled) {
                    bRet = serializer.setBoolPropertyValue("wpsEnabled",
                            wpsEnabled);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " wpsEnabled");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != WpsStatus) {
                    if (WpsStatus.value.length() > 0) {
                        if (true == WpsStatus.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "wpsStatus", WpsStatus.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " wpsStatus");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != accessPointsLink) {
                    bRet = accessPointsLink.serialize(serializer);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " AccessPointsLink");
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
     * Deserializes data and prepares the WiFi object contents.
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
                if (true == deserializer.hasProperty("operationType")) {
                    if (bRet == true) {
                        OperationType = new gen.xsd.WifiOperationType();
                        OperationType.value = deserializer
                                .getStringPropertyValue("operationType");
                        if (false == OperationType.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " operationType");
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
                if (true == deserializer.hasProperty("channel")) {
                    if (bRet == true) {
                        channel = deserializer.getUnsignedByte("channel");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("ssid")) {
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
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("securityType")) {
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
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("securityKey")) {
                    if (bRet == true) {
                        SecurityKey = new gen.xsd.String100();
                        SecurityKey.value = deserializer
                                .getStringPropertyValue("securityKey");
                        if (false == SecurityKey.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " securityKey");
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
                if (true == deserializer.hasProperty("wifiDirectStatus")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        WifiDirectStatus = new gen.xsd.WifiEventType();
                        WifiDirectStatus.value = deserializer
                                .getStringPropertyValue("wifiDirectStatus");
                        if (false == WifiDirectStatus.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " wifiDirectStatus");
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
                if (true == deserializer.hasProperty("wpsEnabled")) {
                    if (bRet == true) {
                        wpsEnabled = deserializer
                                .getBoolPropertyValue("wpsEnabled");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("wpsStatus")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        WpsStatus = new gen.xsd.WifiEventType();
                        WpsStatus.value = deserializer
                                .getStringPropertyValue("wpsStatus");
                        if (false == WpsStatus.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " wpsStatus");
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
                if (true == deserializer.hasGroupProperty("AccessPointsLink")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        accessPointsLink = new gen.xsd.AccessPointsLink();
                        bRet = accessPointsLink.deSerialize(deserializer);
                    }
                    if (false == bRet)
                        deserializer.setErrorMessage("Failed To deserialize : "
                                + " AccessPointsLink");
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
