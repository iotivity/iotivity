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
 * This resource provides a configuration functions to configure the network
 * interfaces of device and set system time properties.<br>
 * Also, this class defines methods that serialize and/or deserialize contents
 * of XSD complex/list type Configuration.
 * 
 * @since 1.0.0
 */
public class Configuration extends gen.xsd.Resource {
    /**
     * [Optional] Indicate installed location of the device. Note that
     * declaration of the internal location enumeration type can be found in the
     * "Attribute Type" declaration. (e.g., â€œLiving_Roomâ€�, â€œBed_Roomâ€�,
     * â€œDining_Roomâ€�, â€œKitchenâ€�, â€œLobbyâ€�)
     */
    public gen.xsd.InternalLocationType InstalledLocation;

    /**
     * [Optional] Indicate the current region where the device is located. This
     * attribute is used to retrieve the weather information from weather
     * information provider.
     */
    public gen.xsd.String20             Region;

    /**
     * [Optional] The currency type indicating the currency. Normally gateway
     * device exposes this attribute. Note that declaration of a currency
     * enumeration type can be found in the "Attribute Type" declaration. (e.g.,
     * â€œUS_Dollarâ€�, â€œKorean_Wonâ€�)
     */
    // public gen.xsd.CurrencyType Currency;

    /**
     * [Optional] Indicate whether the device can be controlled remotely. (e.g.,
     * â€œtrueâ€�, â€œfalseâ€�)
     */
    public Boolean                      remoteControlEnabled;

    /**
     * [Optional] Indicate whether the pairing mode of bridge device is started
     * or stopped (e.g., â€œOnâ€�, â€œOffâ€�). This attribute will be used by
     * ZigBee/Z-Wave Bridge device to start pairing with Non-IP ZigBee/Z-Wave
     * devices.
     */
    public gen.xsd.OnType               PairingStart;

    /**
     * [Optional] Indicate whether the unpairing mode of bridge device is
     * started or stopped. (e.g., â€œOnâ€�, â€œOffâ€�). This attribute will be
     * used by Z-Wave Bridge device to start unpairing with Non-IP Z-Wave
     * devices.
     */
    public gen.xsd.OnType               UnpairingStart;

    /**
     * [Optional] Indicate the timeout of pairing mode ZigBee/Z-Wave Bridge
     * device. After timeout, the bridge device will stop the pairing mode.
     */
    public Integer                      pairingTimeout;

    /**
     * [Optional] Relationships between 'Device' objects are described by
     * linking 'device id's together using this attribute that used to provide
     * logical association information. For example, if two devices are being
     * linked by using this attribute, depending on the perspective of the user
     * or implementor, two devices that have a different 'deviceID's are
     * possible to treat as a single device. Note that prioritize between
     * devices is entirely left to the user or implementor's choice. Also, it's
     * allowed to create multiple relations for a single 'Device' object.
     * However, to prevent excessive increase in complexity, nested relations
     * should be ignored. Available in both the singular and plural forms
     * expression. In case of plural forms, each option can be expressed in
     * array of string.
     */
    public gen.xsd.ArrayOfString        SubDeviceIDs;

    /**
     * [Optional] Indicate whether this device is sub-device or not.
     */
    public Boolean                      isSubDevice;

    /**
     * [Optional] Indicate device type which legacy device is expressed in user
     * interface. For example, when smart plug is connected to legacy TV device
     * without connectivity, connectedDeviceType attribute of smart plug device
     * is set to â€œTVâ€�.
     */
    public gen.xsd.DeviceType           ConnectedDeviceType;

    /**
     * [Optional] Variable of Networks object.
     */
    public gen.xsd.Networks             networks;

    /**
     * [Optional] Variable of Time object.
     */
    public gen.xsd.Time                 time;

    /**
     * [Optional] Variable of Remote object.
     */
    public gen.xsd.Remote               remote;

    /**
     * Default constructor of Configuration class.
     * 
     * @since 1.0.0
     */
    public Configuration() {
    }

    /**
     * Destroy method for Configuration class.
     * 
     * @since 1.0.0
     */
    @Override
    public void destroy() {
        super.destroy();
        if (null != SubDeviceIDs) {
            SubDeviceIDs.destroy();
        }
        if (null != networks) {
            networks.destroy();
        }
        if (null != time) {
            time.destroy();
        }
        if (null != remote) {
            remote.destroy();
        }
    }

    /**
     * Returns name of the serializable.
     * 
     * @since 1.0.0
     */
    @Override
    public String getElementName() {
        return Elements.EN_CONFIGURATION;
    }

    /**
     * Serializes the contents of Configuration object.
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
                if (null != InstalledLocation) {
                    if (InstalledLocation.value.length() > 0) {
                        if (true == InstalledLocation.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "installedLocation",
                                    InstalledLocation.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " installedLocation");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (null != Region) {
                    if (Region.value.length() > 0) {
                        if (true == Region.validateContent()) {
                            bRet = serializer.setStringPropertyValue("region",
                                    Region.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " region");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            /*
             * { boolean bRet = true; if ( null != Currency ) {
             * if(Currency.value.length() > 0) { if (true ==
             * Currency.validateContent()) { bRet =
             * serializer.setStringPropertyValue("currency", Currency.value); }
             * else bRet = false; } if (false == bRet)
             * serializer.setErrorMessage
             * ("Failed To Serialize parameter (Invalid content) : " +
             * " currency"); } if(retValue == true) retValue = bRet; }
             */
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != remoteControlEnabled) {
                    bRet = serializer.setBoolPropertyValue(
                            "remoteControlEnabled", remoteControlEnabled);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " remoteControlEnabled");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (null != PairingStart) {
                    if (PairingStart.value.length() > 0) {
                        if (true == PairingStart.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "pairingStart", PairingStart.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " pairingStart");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (null != UnpairingStart) {
                    if (UnpairingStart.value.length() > 0) {
                        if (true == UnpairingStart.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "unpairingStart", UnpairingStart.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " unpairingStart");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != pairingTimeout) {
                    bRet = serializer.setUnsignedShort("pairingTimeout",
                            pairingTimeout);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " pairingTimeout");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (null != SubDeviceIDs) {
                    SubDeviceIDs.setInstanceName("subDeviceIDs");
                    bRet = SubDeviceIDs.serializeContents(serializer);
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " subDeviceIDs");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (null != isSubDevice) {
                    bRet = serializer.setBoolPropertyValue("isSubDevice",
                            isSubDevice);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " isSubDevice");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (null != ConnectedDeviceType) {
                    if (ConnectedDeviceType.value.length() > 0) {
                        if (true == ConnectedDeviceType.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "connectedDeviceType",
                                    ConnectedDeviceType.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " connectedDeviceType");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (null != networks) {
                        bRet = networks.serialize(serializer);
                        if (false == bRet) {
                            serializer
                                    .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                            + " Networks");
                        }
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            }
            if (serializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (null != time) {
                        bRet = time.serialize(serializer);
                        if (false == bRet) {
                            serializer
                                    .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                            + " Time");
                        }
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            }
            if (serializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (null != remote) {
                        bRet = remote.serialize(serializer);
                        if (false == bRet) {
                            serializer
                                    .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                            + " Remote");
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
     * Deserializes data and prepares the Configuration object contents.
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
                if (true == deserializer.hasProperty("installedLocation")) {
                    if (bRet == true) {
                        InstalledLocation = new gen.xsd.InternalLocationType();
                        InstalledLocation.value = deserializer
                                .getStringPropertyValue("installedLocation");
                        if (false == InstalledLocation.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " installedLocation");
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
                if (true == deserializer.hasProperty("region")) {
                    if (bRet == true) {
                        Region = new gen.xsd.String20();
                        Region.value = deserializer
                                .getStringPropertyValue("region");
                        if (false == Region.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " region");
                            bRet = false;
                        } else
                            bRet = true;
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            /*
             * { boolean bRet = true; if (true ==
             * deserializer.hasProperty("currency")) { if(bRet == true) {
             * Currency = new gen.xsd.CurrencyType(); Currency.value =
             * deserializer.getStringPropertyValue("currency"); if (false ==
             * Currency.validateContent()) {
             * deserializer.setErrorMessage("Parameter Contains Invalid Value : "
             * + " currency"); bRet = false ; } else bRet = true; } } if
             * (retValue == true) retValue = bRet; }
             */
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("remoteControlEnabled")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        remoteControlEnabled = deserializer
                                .getBoolPropertyValue("remoteControlEnabled");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("pairingStart")) {
                    if (bRet == true) {
                        PairingStart = new gen.xsd.OnType();
                        PairingStart.value = deserializer
                                .getStringPropertyValue("pairingStart");
                        if (false == PairingStart.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " pairingStart");
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
                if (true == deserializer.hasProperty("unpairingStart")) {
                    if (bRet == true) {
                        UnpairingStart = new gen.xsd.OnType();
                        UnpairingStart.value = deserializer
                                .getStringPropertyValue("unpairingStart");
                        if (false == UnpairingStart.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " unpairingStart");
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
                if (true == deserializer.hasProperty("pairingTimeout")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        pairingTimeout = deserializer
                                .getUnsignedShort("pairingTimeout");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("subDeviceIDs")) {
                    if (bRet == true) {
                        SubDeviceIDs = new gen.xsd.ArrayOfString();
                        SubDeviceIDs.setInstanceName("subDeviceIDs");
                        if (false == SubDeviceIDs
                                .deSerializeContents(deserializer)) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " subDeviceIDs");
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
                if (true == deserializer.hasProperty("isSubDevice")) {
                    if (bRet == true) {
                        isSubDevice = deserializer
                                .getBoolPropertyValue("isSubDevice");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("connectedDeviceType")) {
                    if (bRet == true) {
                        ConnectedDeviceType = new gen.xsd.DeviceType();
                        ConnectedDeviceType.value = deserializer
                                .getStringPropertyValue("connectedDeviceType");
                        if (false == ConnectedDeviceType.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " connectedDeviceType");
                            bRet = false;
                        } else
                            bRet = true;
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (deserializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (true == deserializer.hasGroupProperty("Networks")) {
                        if (bRet == true) {
                            networks = new gen.xsd.Networks();
                            bRet = networks.deSerialize(deserializer);
                        }
                        if (false == bRet)
                            deserializer
                                    .setErrorMessage("Failed To deserialize : "
                                            + " Networks");
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            } else {
                if (true == deserializer.hasProperty("Networks")) {
                    retValue = false;
                    deserializer
                            .setErrorMessage("Additional data to deserialize: Networks");
                }
            }
            if (deserializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (true == deserializer.hasGroupProperty("Time")) {
                        if (bRet == true) {
                            time = new gen.xsd.Time();
                            bRet = time.deSerialize(deserializer);
                        }
                        if (false == bRet)
                            deserializer
                                    .setErrorMessage("Failed To deserialize : "
                                            + " Time");
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            } else {
                if (true == deserializer.hasProperty("Time")) {
                    retValue = false;
                    deserializer
                            .setErrorMessage("Additional data to deserialize: Time");
                }
            }
            if (deserializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (true == deserializer.hasGroupProperty("Remote")) {
                        if (bRet == true) {
                            remote = new gen.xsd.Remote();
                            bRet = remote.deSerialize(deserializer);
                        }
                        if (false == bRet)
                            deserializer
                                    .setErrorMessage("Failed To deserialize : "
                                            + " Remote");
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            } else {
                if (true == deserializer.hasProperty("Remote")) {
                    retValue = false;
                    deserializer
                            .setErrorMessage("Additional data to deserialize: Remote");
                }
            }
        } catch (Exception e) {
            return false;
        }
        return retValue;

    }
}
