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
 * This resource enumerates the function sets supported by a device and can be
 * used by clients. Also, this could be returned by the URI provided by the
 * discovery service to allow clients to find the URIs to the functions in which
 * they are interested.<br>
 * Also, this class defines methods that serialize and/or deserialize contents
 * of XSD complex/list type Capability.
 * 
 * @since 1.0.0
 */
public class Capability extends gen.xsd.Resource {
    /**
     * [Mandatory] Variable of SubscriptionsLink object.
     */
    public gen.xsd.SubscriptionsLink           subscriptionsLink;

    /**
     * [Optional] Variable of GroupsLink object.
     */
    public gen.xsd.GroupsLink                  groupsLink;

    /**
     * [Optional] Variable of RecordsLink object.
     */
    public gen.xsd.RecordsLink                 recordsLink;

    /**
     * [Optional] Variable of PatternsLink object.
     */
    public gen.xsd.PatternsLink                patternsLink;

    /**
     * [Optional] Variable of EnergyLink object.
     */
    public gen.xsd.EnergyLink                  energyLink;

    /**
     * [Mandatory] Variable of DevicesLink object.
     */
    public gen.xsd.DevicesLink                 devicesLink;

    /**
     * [Optional] Variable of ActionsLink object.
     */
    public gen.xsd.ActionsLink                 actionsLink;

    /**
     * [Optional] Variable of FoodManagerLink object.
     */
    public gen.xsd.FoodManagerLink             foodManagerLink;

    /**
     * [Optional] Variable of NotificationsLink object.
     */
    public gen.xsd.NotificationsLink           notificationsLink;

    /**
     * [Optional] Variable of LongPollingNotificationLink object.
     */
    public gen.xsd.LongPollingNotificationLink longPollingNotificationLink;

    /**
     * Default constructor of Capability class.
     * 
     * @since 1.0.0
     */
    public Capability() {
    }

    /**
     * Destroy method for Capability class.
     * 
     * @since 1.0.0
     */
    @Override
    public void destroy() {
        super.destroy();
        if (null != subscriptionsLink) {
            subscriptionsLink.destroy();
        }
        if (null != groupsLink) {
            groupsLink.destroy();
        }
        if (null != recordsLink) {
            recordsLink.destroy();
        }
        if (null != patternsLink) {
            patternsLink.destroy();
        }
        if (null != energyLink) {
            energyLink.destroy();
        }
        if (null != devicesLink) {
            devicesLink.destroy();
        }
        if (null != actionsLink) {
            actionsLink.destroy();
        }
        if (null != foodManagerLink) {
            foodManagerLink.destroy();
        }
        if (null != notificationsLink) {
            notificationsLink.destroy();
        }
        if (null != longPollingNotificationLink) {
            longPollingNotificationLink.destroy();
        }
    }

    /**
     * Returns name of the serializable.
     * 
     * @since 1.0.0
     */
    @Override
    public String getElementName() {
        return Elements.EN_CAPABILITY;
    }

    /**
     * Serializes the contents of Capability object.
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
                if (null != subscriptionsLink) {
                    bRet = subscriptionsLink.serialize(serializer);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " SubscriptionsLink");
                    }
                } else {
                    if (serializer.isInFilteringMode("SubscriptionsLink") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " SubscriptionsLink");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != groupsLink) {
                    bRet = groupsLink.serialize(serializer);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " GroupsLink");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != recordsLink) {
                    bRet = recordsLink.serialize(serializer);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " RecordsLink");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != patternsLink) {
                    bRet = patternsLink.serialize(serializer);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " PatternsLink");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != energyLink) {
                    bRet = energyLink.serialize(serializer);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " EnergyLink");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != devicesLink) {
                    bRet = devicesLink.serialize(serializer);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " DevicesLink");
                    }
                } else {
                    if (serializer.isInFilteringMode("DevicesLink") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " DevicesLink");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != actionsLink) {
                    bRet = actionsLink.serialize(serializer);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " ActionsLink");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != foodManagerLink) {
                    bRet = foodManagerLink.serialize(serializer);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " FoodManagerLink");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != notificationsLink) {
                    bRet = notificationsLink.serialize(serializer);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " NotificationsLink");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != longPollingNotificationLink) {
                    bRet = longPollingNotificationLink.serialize(serializer);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " LongPollingNotificationLink");
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
     * Deserializes data and prepares the Capability object contents.
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
                if (true == deserializer.hasGroupProperty("SubscriptionsLink")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        subscriptionsLink = new gen.xsd.SubscriptionsLink();
                        bRet = subscriptionsLink.deSerialize(deserializer);
                    }
                    if (false == bRet)
                        deserializer.setErrorMessage("Failed To deserialize : "
                                + " SubscriptionsLink");
                } else {
                    if (deserializer.isInFilteringMode("SubscriptionsLink") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " SubscriptionsLink");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasGroupProperty("GroupsLink")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        groupsLink = new gen.xsd.GroupsLink();
                        bRet = groupsLink.deSerialize(deserializer);
                    }
                    if (false == bRet)
                        deserializer.setErrorMessage("Failed To deserialize : "
                                + " GroupsLink");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasGroupProperty("RecordsLink")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        recordsLink = new gen.xsd.RecordsLink();
                        bRet = recordsLink.deSerialize(deserializer);
                    }
                    if (false == bRet)
                        deserializer.setErrorMessage("Failed To deserialize : "
                                + " RecordsLink");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasGroupProperty("PatternsLink")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        patternsLink = new gen.xsd.PatternsLink();
                        bRet = patternsLink.deSerialize(deserializer);
                    }
                    if (false == bRet)
                        deserializer.setErrorMessage("Failed To deserialize : "
                                + " PatternsLink");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasGroupProperty("EnergyLink")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        energyLink = new gen.xsd.EnergyLink();
                        bRet = energyLink.deSerialize(deserializer);
                    }
                    if (false == bRet)
                        deserializer.setErrorMessage("Failed To deserialize : "
                                + " EnergyLink");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasGroupProperty("DevicesLink")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        devicesLink = new gen.xsd.DevicesLink();
                        bRet = devicesLink.deSerialize(deserializer);
                    }
                    if (false == bRet)
                        deserializer.setErrorMessage("Failed To deserialize : "
                                + " DevicesLink");
                } else {
                    if (deserializer.isInFilteringMode("DevicesLink") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " DevicesLink");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasGroupProperty("ActionsLink")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        actionsLink = new gen.xsd.ActionsLink();
                        bRet = actionsLink.deSerialize(deserializer);
                    }
                    if (false == bRet)
                        deserializer.setErrorMessage("Failed To deserialize : "
                                + " ActionsLink");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasGroupProperty("FoodManagerLink")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        foodManagerLink = new gen.xsd.FoodManagerLink();
                        bRet = foodManagerLink.deSerialize(deserializer);
                    }
                    if (false == bRet)
                        deserializer.setErrorMessage("Failed To deserialize : "
                                + " FoodManagerLink");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasGroupProperty("NotificationsLink")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        notificationsLink = new gen.xsd.NotificationsLink();
                        bRet = notificationsLink.deSerialize(deserializer);
                    }
                    if (false == bRet)
                        deserializer.setErrorMessage("Failed To deserialize : "
                                + " NotificationsLink");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer
                        .hasGroupProperty("LongPollingNotificationLink")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        longPollingNotificationLink = new gen.xsd.LongPollingNotificationLink();
                        bRet = longPollingNotificationLink
                                .deSerialize(deserializer);
                    }
                    if (false == bRet)
                        deserializer.setErrorMessage("Failed To deserialize : "
                                + " LongPollingNotificationLink");
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
