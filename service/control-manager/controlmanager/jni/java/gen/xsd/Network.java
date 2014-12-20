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
 * This resource provides a network adapter management functions.<br>
 * Also, this class defines methods that serialize and/or deserialize contents
 * of XSD complex/list type Network.
 * 
 * @since 1.0.0
 */
public class Network extends gen.xsd.IdentifiedObject {
    /**
     * [Mandatory] Indicate whether the network adapter is enabled or not.
     * (e.g., â€œTrueâ€�, â€œFalseâ€�)
     */
    public Boolean          enabled;

    /**
     * [Mandatory] Indicate whether the adapter is connected to the network or
     * not. (e.g., â€œTrueâ€�, â€œFalseâ€�)
     */
    public Boolean          connected;

    /**
     * [Optional] Set the priority value of the preferred network adapter to be
     * lower than the priority values of the other adapters. Setting the
     * priority value lower means that the network adapter has a higher
     * priority. The priority value takes precedence in the network adapter
     * order. If the priority value about to assign is already exists, priority
     * of network adapter will be internally determined, and the results cannot
     * be predicted. Therefore, should be careful not to assign the same
     * priority value. (e.g., â€œ0â€�, â€œ1â€�, â€œ2â€�)
     */
    public Integer          priority;

    /**
     * [Mandatory] This attribute indicates whether the DHCP is enabled or not
     * on this network adapter. (e.g., â€œTrueâ€�, â€œFalseâ€�)
     */
    public Boolean          dhcpEnabled;

    /**
     * [Mandatory] The network adapterâ€™s IP address. (e.g.,
     * â€œ192.168.10.2â€�, â€œfe80::7464:36e5:41a5:821/64â€�)
     */
    public gen.xsd.String50 IpAddress;

    /**
     * [Mandatory] The subnet mask for this network adapter. (e.g.,
     * â€œ255.255.255.0â€�, â€œ64â€�)
     */
    public gen.xsd.String20 SubnetMask;

    /**
     * [Mandatory] The default gateway for this network adapter. (e.g.,
     * â€œ192.168.10.1â€�)
     */
    public gen.xsd.String50 Gateway;

    /**
     * [Mandatory] The primary DNS server address for DNS lookups. (e.g.,
     * â€œ8.8.4.4â€�)
     */
    public gen.xsd.String50 DnsPrimary;

    /**
     * [Optional] The secondary DNS server address for DNS lookups when the
     * primary DNS server is unavailable. (e.g., â€œ8.8.8.8â€�)
     */
    public gen.xsd.String50 DnsSecondary;

    /**
     * [Mandatory] The network adapterâ€™s MAC address. (e.g.,
     * â€œ00:23:32:95:40:CBâ€�)
     */
    public gen.xsd.String20 MacAddress;

    /**
     * [Optional] Variable of WiFi object.
     */
    public gen.xsd.WiFi     wiFi;

    /**
     * Default constructor of Network class.
     * 
     * @since 1.0.0
     */
    public Network() {
    }

    /**
     * Destroy method for Network class.
     * 
     * @since 1.0.0
     */
    @Override
    public void destroy() {
        super.destroy();
        if (null != wiFi) {
            wiFi.destroy();
        }
    }

    /**
     * Returns name of the serializable.
     * 
     * @since 1.0.0
     */
    @Override
    public String getElementName() {
        return Elements.EN_NETWORK;
    }

    /**
     * Serializes the contents of Network object.
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
                if (null != enabled) {
                    bRet = serializer.setBoolPropertyValue("enabled", enabled);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " enabled");
                    }
                } else {
                    if (serializer.isInFilteringMode("enabled") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " enabled");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (null != connected) {
                    bRet = serializer.setBoolPropertyValue("connected",
                            connected);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " connected");
                    }
                } else {
                    if (serializer.isInFilteringMode("connected") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " connected");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (null != priority) {
                    bRet = serializer.setUnsignedByte("priority", priority);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " priority");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (null != dhcpEnabled) {
                    bRet = serializer.setBoolPropertyValue("dhcpEnabled",
                            dhcpEnabled);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " dhcpEnabled");
                    }
                } else {
                    if (serializer.isInFilteringMode("dhcpEnabled") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " dhcpEnabled");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (null != IpAddress) {
                    if (IpAddress.value.length() > 0) {
                        if (true == IpAddress.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "ipAddress", IpAddress.value);
                        } else
                            bRet = false;
                    } else
                        bRet = false;
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " ipAddress");
                } else {
                    if (serializer.isInFilteringMode("ipAddress") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " ipAddress");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (null != SubnetMask) {
                    if (SubnetMask.value.length() > 0) {
                        if (true == SubnetMask.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "subnetMask", SubnetMask.value);
                        } else
                            bRet = false;
                    } else
                        bRet = false;
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " subnetMask");
                } else {
                    if (serializer.isInFilteringMode("subnetMask") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " subnetMask");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (null != Gateway) {
                    if (Gateway.value.length() > 0) {
                        if (true == Gateway.validateContent()) {
                            bRet = serializer.setStringPropertyValue("gateway",
                                    Gateway.value);
                        } else
                            bRet = false;
                    } else
                        bRet = false;
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " gateway");
                } else {
                    if (serializer.isInFilteringMode("gateway") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " gateway");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (null != DnsPrimary) {
                    if (DnsPrimary.value.length() > 0) {
                        if (true == DnsPrimary.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "dnsPrimary", DnsPrimary.value);
                        } else
                            bRet = false;
                    } else
                        bRet = false;
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " dnsPrimary");
                } else {
                    if (serializer.isInFilteringMode("dnsPrimary") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " dnsPrimary");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (null != DnsSecondary) {
                    if (DnsSecondary.value.length() > 0) {
                        if (true == DnsSecondary.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "dnsSecondary", DnsSecondary.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " dnsSecondary");
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

            if (serializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (null != wiFi) {
                        bRet = wiFi.serialize(serializer);
                        if (false == bRet) {
                            serializer
                                    .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                            + " WiFi");
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
     * Deserializes data and prepares the Network object contents.
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
                if (true == deserializer.hasProperty("enabled")) {
                    if (bRet == true) {
                        enabled = deserializer.getBoolPropertyValue("enabled");
                    }
                } else {
                    if (deserializer.isInFilteringMode("enabled") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " enabled");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("connected")) {
                    if (bRet == true) {
                        connected = deserializer
                                .getBoolPropertyValue("connected");
                    }
                } else {
                    if (deserializer.isInFilteringMode("connected") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " connected");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("priority")) {
                    if (bRet == true) {
                        priority = deserializer.getUnsignedByte("priority");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("dhcpEnabled")) {
                    if (bRet == true) {
                        dhcpEnabled = deserializer
                                .getBoolPropertyValue("dhcpEnabled");
                    }
                } else {
                    if (deserializer.isInFilteringMode("dhcpEnabled") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " dhcpEnabled");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("ipAddress")) {
                    if (bRet == true) {
                        IpAddress = new gen.xsd.String50();
                        IpAddress.value = deserializer
                                .getStringPropertyValue("ipAddress");
                        if (false == IpAddress.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " ipAddress");
                            bRet = false;
                        } else
                            bRet = true;
                    }
                } else {
                    if (deserializer.isInFilteringMode("ipAddress") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " ipAddress");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("subnetMask")) {
                    if (bRet == true) {
                        SubnetMask = new gen.xsd.String20();
                        SubnetMask.value = deserializer
                                .getStringPropertyValue("subnetMask");
                        if (false == SubnetMask.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " subnetMask");
                            bRet = false;
                        } else
                            bRet = true;
                    }
                } else {
                    if (deserializer.isInFilteringMode("subnetMask") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " subnetMask");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("gateway")) {
                    if (bRet == true) {
                        Gateway = new gen.xsd.String50();
                        Gateway.value = deserializer
                                .getStringPropertyValue("gateway");
                        if (false == Gateway.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " gateway");
                            bRet = false;
                        } else
                            bRet = true;
                    }
                } else {
                    if (deserializer.isInFilteringMode("gateway") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " gateway");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("dnsPrimary")) {
                    if (bRet == true) {
                        DnsPrimary = new gen.xsd.String50();
                        DnsPrimary.value = deserializer
                                .getStringPropertyValue("dnsPrimary");
                        if (false == DnsPrimary.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " dnsPrimary");
                            bRet = false;
                        } else
                            bRet = true;
                    }
                } else {
                    if (deserializer.isInFilteringMode("dnsPrimary") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " dnsPrimary");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("dnsSecondary")) {
                    if (bRet == true) {
                        DnsSecondary = new gen.xsd.String50();
                        DnsSecondary.value = deserializer
                                .getStringPropertyValue("dnsSecondary");
                        if (false == DnsSecondary.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " dnsSecondary");
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

            if (deserializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (true == deserializer.hasGroupProperty("WiFi")) {
                        if (bRet == true) {
                            wiFi = new gen.xsd.WiFi();
                            bRet = wiFi.deSerialize(deserializer);
                        }
                        if (false == bRet)
                            deserializer
                                    .setErrorMessage("Failed To deserialize : "
                                            + " WiFi");
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            } else {
                if (true == deserializer.hasProperty("WiFi")) {
                    retValue = false;
                    deserializer
                            .setErrorMessage("Additional data to deserialize: WiFi");
                }
            }
        } catch (Exception e) {
            return false;
        }
        return retValue;

    }
}
