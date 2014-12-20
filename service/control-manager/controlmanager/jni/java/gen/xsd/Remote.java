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
 * This resource provides an interface for accessing cloud server. Attributes
 * defined in this resource This function set provides an interface for an alarm
 * service.<br>
 * Also, this class defines methods that serialize and/or deserialize contents
 * of XSD complex/list type Remote.
 * 
 * @since 1.0.0
 */
public class Remote extends gen.xsd.Resource {
    /**
     * [Mandatory] Indicate the authentication code.
     */
    public gen.xsd.String30 AuthCode;

    /**
     * [Mandatory] Indicate the account ID of user.
     */
    public gen.xsd.String30 AccountID;

    /**
     * [Mandatory] Indicate the peer ID.
     */
    public gen.xsd.String50 PeerID;

    /**
     * [Mandatory] Indicate the peer group ID.
     */
    public gen.xsd.String30 PeerGroupID;

    /**
     * [Mandatory] Indicate the country code of device. Country code is
     * registered when user signs up (3 chars). (e.g., KOR)
     */
    public gen.xsd.String10 CountryCode;

    /**
     * Default constructor of Remote class.
     * 
     * @since 1.0.0
     */
    public Remote() {
    }

    /**
     * Destroy method for Remote class.
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
        return Elements.EN_REMOTE;
    }

    /**
     * Serializes the contents of Remote object.
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
                if (null != AuthCode) {
                    if (AuthCode.value.length() > 0) {
                        if (true == AuthCode.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "authCode", AuthCode.value);
                        } else
                            bRet = false;
                    } else
                        bRet = false;
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " authCode");
                } else {
                    if (serializer.isInFilteringMode("authCode") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " authCode");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (null != AccountID) {
                    if (AccountID.value.length() > 0) {
                        if (true == AccountID.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "accountID", AccountID.value);
                        } else
                            bRet = false;
                    } else
                        bRet = false;
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " accountID");
                } else {
                    if (serializer.isInFilteringMode("accountID") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " accountID");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (null != PeerID) {
                    if (PeerID.value.length() > 0) {
                        if (true == PeerID.validateContent()) {
                            bRet = serializer.setStringPropertyValue("peerID",
                                    PeerID.value);
                        } else
                            bRet = false;
                    } else
                        bRet = false;
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " peerID");
                } else {
                    if (serializer.isInFilteringMode("peerID") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " peerID");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (null != PeerGroupID) {
                    if (PeerGroupID.value.length() > 0) {
                        if (true == PeerGroupID.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "peerGroupID", PeerGroupID.value);
                        } else
                            bRet = false;
                    } else
                        bRet = false;
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " peerGroupID");
                } else {
                    if (serializer.isInFilteringMode("peerGroupID") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " peerGroupID");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (null != CountryCode) {
                    if (CountryCode.value.length() > 0) {
                        if (true == CountryCode.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "countryCode", CountryCode.value);
                        } else
                            bRet = false;
                    } else
                        bRet = false;
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " countryCode");
                } else {
                    if (serializer.isInFilteringMode("countryCode") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " countryCode");
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
     * Deserializes data and prepares the Remote object contents.
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
                if (true == deserializer.hasProperty("authCode")) {
                    if (bRet == true) {
                        AuthCode = new gen.xsd.String30();
                        AuthCode.value = deserializer
                                .getStringPropertyValue("authCode");
                        if (false == AuthCode.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " authCode");
                            bRet = false;
                        } else
                            bRet = true;
                    }
                } else {
                    if (deserializer.isInFilteringMode("authCode") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " authCode");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("accountID")) {
                    if (bRet == true) {
                        AccountID = new gen.xsd.String30();
                        AccountID.value = deserializer
                                .getStringPropertyValue("accountID");
                        if (false == AccountID.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " accountID");
                            bRet = false;
                        } else
                            bRet = true;
                    }
                } else {
                    if (deserializer.isInFilteringMode("accountID") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " accountID");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("peerID")) {
                    if (bRet == true) {
                        PeerID = new gen.xsd.String50();
                        PeerID.value = deserializer
                                .getStringPropertyValue("peerID");
                        if (false == PeerID.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " peerID");
                            bRet = false;
                        } else
                            bRet = true;
                    }
                } else {
                    if (deserializer.isInFilteringMode("peerID") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " peerID");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("peerGroupID")) {
                    if (bRet == true) {
                        PeerGroupID = new gen.xsd.String30();
                        PeerGroupID.value = deserializer
                                .getStringPropertyValue("peerGroupID");
                        if (false == PeerGroupID.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " peerGroupID");
                            bRet = false;
                        } else
                            bRet = true;
                    }
                } else {
                    if (deserializer.isInFilteringMode("peerGroupID") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " peerGroupID");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("countryCode")) {
                    if (bRet == true) {
                        CountryCode = new gen.xsd.String10();
                        CountryCode.value = deserializer
                                .getStringPropertyValue("countryCode");
                        if (false == CountryCode.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " countryCode");
                            bRet = false;
                        } else
                            bRet = true;
                    }
                } else {
                    if (deserializer.isInFilteringMode("countryCode") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " countryCode");
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
