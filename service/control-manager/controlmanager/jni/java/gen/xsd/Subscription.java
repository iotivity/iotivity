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
 * A subscription resource is realized as a resource for an individual client,
 * providing interfaces to all subscriptions for the given client. The
 * notification resource is used to receive notifications that a resource to
 * which a host is subscribed has changed. The location of the notification
 * resource is passed to the subscription server in the body of the
 * subscription. As such, a given client (notification resource server) may have
 * one notification resource for multiple different notifications or may have a
 * different notification resource for different notifications.<br>
 * Also, this class defines methods that serialize and/or deserialize contents
 * of XSD complex/list type Subscription.
 * 
 * @since 1.0.0
 */
public class Subscription extends gen.xsd.IdentifiedObject {
    /**
     * [Mandatory] Indicate the UUID of device (REST client) which subscribes
     * event notification.
     */
    public OC.Cm.Serialization.Xsd.UuidType Uuid;

    /**
     * [Mandatory] The resource for which the subscription applies. Query string
     * parameters SHALL NOT be specified when subscribing to list resources.
     * Should a query string parameter be specified, servers SHALL ignore them.
     * Client devices can subscribe to the â€œ/devicesâ€� resource. Available in
     * both the singular and plural forms expression. In case of plural forms,
     * each option can be expressed in array of string. Note that declaration of
     * the available resource enumeration type can be found in the section
     * 2.2.3. (e.g., â€œdevices/{deviceID}/operationâ€�,
     * â€œdevices/{deviceID}/lightâ€�)
     */
    public gen.xsd.ArrayOfAnyURI            ResourceURIs;

    /**
     * [Mandatory] The resource to which to post the notifications about the
     * requested subscribed resource. (e.g., â€œ/notificationâ€�)
     */
    public String                           notificationURI;

    /**
     * [Optional] This attribute is only used in long polling notification
     * mechanism. In subscription/notification mechanism, this attribute will
     * not be used.
     */
    public Integer                          timeout;

    /**
     * [Optional] This attribute is only used in long polling notification
     * mechanism. In subscription/notification mechanism, this attribute will
     * not be used.
     */
    public Integer                          duration;

    /**
     * Default constructor of Subscription class.
     * 
     * @since 1.0.0
     */
    public Subscription() {
    }

    /**
     * Destroy method for Subscription class.
     * 
     * @since 1.0.0
     */
    @Override
    public void destroy() {
        super.destroy();
        if (null != ResourceURIs) {
            ResourceURIs.destroy();
        }
    }

    /**
     * Returns name of the serializable.
     * 
     * @since 1.0.0
     */
    @Override
    public String getElementName() {
        return Elements.EN_SUBSCRIPTION;
    }

    /**
     * Serializes the contents of Subscription object.
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
                if (null != Uuid) {
                    if (Uuid.value.length() > 0) {
                        if (true == Uuid.validateContent()) {
                            bRet = serializer.setStringPropertyValue("uuid",
                                    Uuid.value);
                        } else
                            bRet = false;
                    } else
                        bRet = false;
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " uuid");
                } else {
                    if (serializer.isInFilteringMode("uuid") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " uuid");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (null != ResourceURIs) {
                    ResourceURIs.setInstanceName("resourceURIs");
                    bRet = ResourceURIs.serializeContents(serializer);
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " resourceURIs");
                } else {
                    if (serializer.isInFilteringMode("resourceURIs") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " resourceURIs");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (null != notificationURI) {
                    if (notificationURI.length() > 0) {
                        bRet = serializer.setStringPropertyValue(
                                "notificationURI", notificationURI);
                    } else
                        bRet = false;
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " notificationURI");
                    }
                } else {
                    if (serializer.isInFilteringMode("notificationURI") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " notificationURI");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (null != timeout) {
                    bRet = serializer.setUnsignedByte("timeout", timeout);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " timeout");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (null != duration) {
                    bRet = serializer.setUnsignedByte("duration", duration);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " duration");
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
     * Deserializes data and prepares the Subscription object contents.
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
                if (true == deserializer.hasProperty("uuid")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        Uuid = new OC.Cm.Serialization.Xsd.UuidType();
                        Uuid.value = deserializer
                                .getStringPropertyValue("uuid");
                        if (false == Uuid.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " uuid");
                            bRet = false;
                        } else
                            bRet = true;
                    }
                } else {
                    if (deserializer.isInFilteringMode("uuid") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " uuid");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("resourceURIs")) {
                    if (bRet == true) {
                        ResourceURIs = new gen.xsd.ArrayOfAnyURI();
                        ResourceURIs.setInstanceName("resourceURIs");
                        if (false == ResourceURIs
                                .deSerializeContents(deserializer)) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " resourceURIs");
                            bRet = false;
                        } else
                            bRet = true;
                    }
                } else {
                    if (deserializer.isInFilteringMode("resourceURIs") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " resourceURIs");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("notificationURI")) {
                    if (bRet == true) {
                        notificationURI = deserializer
                                .getStringPropertyValue("notificationURI");
                    }
                } else {
                    if (deserializer.isInFilteringMode("notificationURI") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " notificationURI");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("timeout")) {
                    if (bRet == true) {
                        timeout = deserializer.getUnsignedByte("timeout");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("duration")) {
                    if (bRet == true) {
                        duration = deserializer.getUnsignedByte("duration");
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
