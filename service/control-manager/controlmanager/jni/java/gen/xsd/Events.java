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
 * This class defines methods that serialize and/or deserialize contents of XSD
 * complex/list type Events.
 * 
 * @since 1.0.0
 */
public class Events extends OC.Cm.Serialization.ISerializable {
    /**
     * [Mandatory] Indicate the UUID of device (REST server) where event occurs.
     */
    public OC.Cm.Serialization.Xsd.UuidType     Uuid;

    /**
     * [Mandatory] The resource for which the subscription applies. Query string
     * parameters SHALL NOT be specified when subscribing to list resources.
     * Should a query string parameter be specified, servers SHALL ignore them.
     * Client devices can subscribe to the â€œ/devicesâ€� resource. But to
     * prevent overwhelming network resource, notification associated with the
     * connection status will only be sent to a given client. (e.g.,
     * â€œ/detectionâ€�)
     */
    public String                               resourceURI;

    /**
     * [Optional] This field contains the acknowledgement or event status. Note
     * that declaration of the event enumeration type can be found in the
     * "Attribute Type" declaration. (e.g., â€œCreatedâ€�, â€œNotifiedâ€�,
     * â€œDeletedâ€�)
     */
    public gen.xsd.NotificationEventType        Event;

    /**
     * [Mandatory] Indicate time when event occurs, in the format defined by
     * DateTimeType.
     */
    public OC.Cm.Serialization.Xsd.DateTimeType EventTime;

    /**
     * [Optional] Variable of Capability object.
     */
    public gen.xsd.Capability                   capability;

    /**
     * [Optional] Variable of Devices object.
     */
    public gen.xsd.Devices                      devices;

    /**
     * [Optional] Variable of Device object.
     */
    public gen.xsd.Device                       device;

    /**
     * [Optional] Variable of Operation object.
     */
    public gen.xsd.Operation                    operation;

    /**
     * [Optional] Variable of Mode object.
     */
    public gen.xsd.Mode                         mode;

    /**
     * [Optional] Variable of Movement object.
     */
    public gen.xsd.Movement                     movement;

    /**
     * [Optional] Variable of Temperatures object.
     */
    public gen.xsd.Temperatures                 temperatures;

    /**
     * [Optional] Variable of Temperature object.
     */
    public gen.xsd.Temperature                  temperature;

    /**
     * [Optional] Variable of Doors object.
     */
    public gen.xsd.Doors                        doors;

    /**
     * [Optional] Variable of Door object.
     */
    public gen.xsd.Door                         door;

    /**
     * [Optional] Variable of Humidity object.
     */
    public gen.xsd.Humidity                     humidity;

    /**
     * [Optional] Variable of Wind object.
     */
    // public gen.xsd.Wind wind;

    /**
     * [Optional] Variable of Light object.
     */
    public gen.xsd.Light                        light;

    /**
     * [Optional] Variable of Washer object.
     */
    // public gen.xsd.Washer washer;

    /**
     * [Optional] Variable of Fridge object.
     */
    // public gen.xsd.Fridge fridge;

    /**
     * [Optional] Variable of Oven object.
     */
    // public gen.xsd.Oven oven;

    /**
     * [Optional] Variable of CustomRecipes object.
     */
    // public gen.xsd.CustomRecipes customRecipes;

    /**
     * [Optional] Variable of CustomRecipe object.
     */
    // public gen.xsd.CustomRecipe customRecipe;

    /**
     * [Optional] Variable of Audio object.
     */
    // public gen.xsd.Audio audio;

    /**
     * [Optional] Variable of AVSources object.
     */
    // public gen.xsd.AVSources aVSources;

    /**
     * [Optional] Variable of AVSource object.
     */
    // gen.xsd.AVSource aVSource;

    /**
     * [Optional] Variable of Subscriptions object.
     */
    public gen.xsd.Subscriptions                subscriptions;

    /**
     * [Optional] Variable of Subscription object.
     */
    public gen.xsd.Subscription                 subscription;

    /**
     * [Optional] Variable of Information object.
     */
    public gen.xsd.Information                  information;

    /**
     * [Optional] Variable of Versions object.
     */
    public gen.xsd.Versions                     versions;

    /**
     * [Optional] Variable of Version object.
     */
    public gen.xsd.Version                      version;

    /**
     * [Optional] Variable of Configuration object.
     */
    public gen.xsd.Configuration                configuration;

    /**
     * [Optional] Variable of Time object.
     */
    public gen.xsd.Time                         time;

    /**
     * [Optional] Variable of Remote object.
     */
    public gen.xsd.Remote                       remote;

    /**
     * [Optional] Variable of Networks object.
     */
    public gen.xsd.Networks                     networks;

    /**
     * [Optional] Variable of Network object.
     */
    public gen.xsd.Network                      network;

    /**
     * [Optional] Variable of WiFi object.
     */
    public gen.xsd.WiFi                         wiFi;

    /**
     * [Optional] Variable of AccessPoints object.
     */
    public gen.xsd.AccessPoints                 accessPoints;

    /**
     * [Optional] Variable of AccessPoint object.
     */
    public gen.xsd.AccessPoint                  accessPoint;

    /**
     * [Optional] Variable of EnergyConsumption object.
     */
    public gen.xsd.EnergyConsumption            energyConsumption;

    /**
     * [Optional] Variable of Alarms object.
     */
    public gen.xsd.Alarms                       alarms;

    /**
     * [Optional] Variable of Alarm object.
     */
    public gen.xsd.Alarm                        alarm;

    /**
     * [Optional] Variable of Level object.
     */
    public gen.xsd.Level                        level;

    /**
     * [Optional] Variable of Messages object.
     */
    public gen.xsd.Messages                     messages;

    /**
     * [Optional] Variable of Message object.
     */
    public gen.xsd.Message                      message;

    /**
     * [Optional] Variable of Detection object.
     */
    public gen.xsd.Detection                    detection;

    /**
     * [Optional] Variable of Camera object.
     */
    // public gen.xsd.Camera camera;

    /**
     * [Optional] Variable of Captures object.
     */
    // public gen.xsd.Captures captures;

    /**
     * [Optional] Variable of Camera object.
     */
    // public gen.xsd.Camera capture;

    /**
     * [Optional] Variable of Groups object.
     */
    // public gen.xsd.Groups groups;

    /**
     * [Optional] Variable of Group object.
     */
    // public gen.xsd.Group group;

    /**
     * [Optional] Variable of Actions object.
     */
    // public gen.xsd.Actions actions;

    /**
     * [Optional] Variable of Action object.
     */
    // public gen.xsd.Action action;

    /**
     * [Optional] Variable of Patterns object.
     */
    // public gen.xsd.Patterns patterns;

    /**
     * [Optional] Variable of Pattern object.
     */
    // public gen.xsd.Pattern pattern;

    /**
     * [Optional] Variable of Records object.
     */
    public gen.xsd.Records                      records;

    /**
     * [Optional] Variable of Record object.
     */
    public gen.xsd.Record                       record;

    /**
     * [Optional] Variable of Energy object.
     */
    public gen.xsd.Energy                       energy;

    /**
     * [Optional] Variable of UsageLimits object.
     */
    public gen.xsd.UsageLimits                  usageLimits;

    /**
     * [Optional] Variable of UsageLimit object.
     */
    public gen.xsd.UsageLimit                   usageLimit;

    /**
     * [Optional] Variable of FoodManager object.
     */
    // public gen.xsd.FoodManager foodManager;

    /**
     * [Optional] Variable of FoodList object.
     */
    // public gen.xsd.FoodList foodList;

    /**
     * [Optional] Variable of Food object.
     */
    // public gen.xsd.Food food;

    /**
     * [Optional] Variable of ShoppingList object.
     */
    // public gen.xsd.ShoppingList shoppingList;

    /**
     * [Optional] Variable of Shopping object.
     */
    // public gen.xsd.Shopping shopping;

    /**
     * [Optional] Variable of FoodImages object.
     */
    // public gen.xsd.FoodImages foodImages;

    /**
     * [Optional] Variable of FoodImage object.
     */
    // public gen.xsd.FoodImage foodImage;

    /**
     * [Optional] Variable of Sensors object.
     */
    // public gen.xsd.Sensors sensors;

    /**
     * [Optional] Variable of Sensor object.
     */
    // public gen.xsd.Sensor sensor;

    /**
     * [Optional] Variable of Consumables object.
     */
    // public gen.xsd.Consumables consumables;

    /**
     * [Optional] Variable of Consumable object.
     */
    // public gen.xsd.Consumable consumable;

    /**
     * Default constructor of Events class.
     * 
     * @since 1.0.0
     */
    public Events() {
    }

    /**
     * Destroy method for Events class.
     * 
     * @since 1.0.0
     */
    @Override
    public void destroy() {
        super.destroy();
        if (null != capability) {
            capability.destroy();
        }
        if (null != devices) {
            devices.destroy();
        }
        if (null != device) {
            device.destroy();
        }
        if (null != operation) {
            operation.destroy();
        }
        if (null != mode) {
            mode.destroy();
        }
        if (null != movement) {
            movement.destroy();
        }
        if (null != temperatures) {
            temperatures.destroy();
        }
        if (null != temperature) {
            temperature.destroy();
        }
        if (null != doors) {
            doors.destroy();
        }
        if (null != door) {
            door.destroy();
        }
        if (null != humidity) {
            humidity.destroy();
        }
        // if (null != wind) { wind.destroy(); }
        if (null != light) {
            light.destroy();
        }
        // if (null != washer) { washer.destroy(); }
        // if (null != fridge) { fridge.destroy(); }
        // if (null != oven) { oven.destroy(); }
        // if (null != customRecipes) { customRecipes.destroy(); }
        // if (null != customRecipe) { customRecipe.destroy(); }
        // if (null != audio) { audio.destroy(); }
        // if (null != aVSources) { aVSources.destroy(); }
        // if (null != aVSource) { aVSource.destroy(); }
        if (null != subscriptions) {
            subscriptions.destroy();
        }
        if (null != subscription) {
            subscription.destroy();
        }
        if (null != information) {
            information.destroy();
        }
        if (null != versions) {
            versions.destroy();
        }
        if (null != version) {
            version.destroy();
        }
        if (null != configuration) {
            configuration.destroy();
        }
        if (null != time) {
            time.destroy();
        }
        if (null != remote) {
            remote.destroy();
        }
        if (null != networks) {
            networks.destroy();
        }
        if (null != network) {
            network.destroy();
        }
        if (null != wiFi) {
            wiFi.destroy();
        }
        if (null != accessPoints) {
            accessPoints.destroy();
        }
        if (null != accessPoint) {
            accessPoint.destroy();
        }
        if (null != energyConsumption) {
            energyConsumption.destroy();
        }
        if (null != alarms) {
            alarms.destroy();
        }
        if (null != alarm) {
            alarm.destroy();
        }
        if (null != level) {
            level.destroy();
        }
        if (null != messages) {
            messages.destroy();
        }
        if (null != message) {
            message.destroy();
        }
        if (null != detection) {
            detection.destroy();
        }
        // if (null != camera) { camera.destroy(); }
        // if (null != captures) { captures.destroy(); }
        // if (null != capture) { capture.destroy(); }
        // if (null != groups) { groups.destroy(); }
        // if (null != group) { group.destroy(); }
        // if (null != actions) { actions.destroy(); }
        // if (null != action) { action.destroy(); }
        // if (null != patterns) { patterns.destroy(); }
        // if (null != pattern) { pattern.destroy(); }
        if (null != records) {
            records.destroy();
        }
        if (null != record) {
            record.destroy();
        }
        if (null != energy) {
            energy.destroy();
        }
        if (null != usageLimits) {
            usageLimits.destroy();
        }
        if (null != usageLimit) {
            usageLimit.destroy();
        }
        // if (null != foodManager) { foodManager.destroy(); }
        // if (null != foodList) { foodList.destroy(); }
        // if (null != food) { food.destroy(); }
        // if (null != shoppingList) { shoppingList.destroy(); }
        // if (null != shopping) { shopping.destroy(); }
        // if (null != foodImages) { foodImages.destroy(); }
        // if (null != foodImage) { foodImage.destroy(); }
        // if (null != sensors) { sensors.destroy(); }
        // if (null != sensor) { sensor.destroy(); }
        // if (null != consumables) { consumables.destroy(); }
        // if (null != consumable) { consumable.destroy(); }
    }

    /**
     * Returns name of the serializable.
     * 
     * @since 1.0.0
     */
    @Override
    public String getElementName() {
        return Elements.EN_EVENTS;
    }

    /**
     * Serializes the contents of Events object.
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

            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != resourceURI) {
                    if (resourceURI.length() > 0) {
                        bRet = serializer.setStringPropertyValue("resourceURI",
                                resourceURI);
                    } else
                        bRet = false;
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " resourceURI");
                    }
                } else {
                    if (serializer.isInFilteringMode("resourceURI") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " resourceURI");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != Event) {
                    if (Event.value.length() > 0) {
                        if (true == Event.validateContent()) {
                            bRet = serializer.setStringPropertyValue("event",
                                    Event.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " event");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != EventTime) {
                    if (EventTime.value.length() > 0) {
                        if (true == EventTime.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "eventTime", EventTime.value);
                        } else
                            bRet = false;
                    } else
                        bRet = false;
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " eventTime");
                } else {
                    if (serializer.isInFilteringMode("eventTime") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " eventTime");
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
                    if (null != capability) {
                        bRet = capability.serialize(serializer);
                        if (false == bRet) {
                            serializer
                                    .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                            + " Capability");
                        }
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            }
            if (serializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (null != devices) {
                        bRet = devices.serialize(serializer);
                        if (false == bRet) {
                            serializer
                                    .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                            + " Devices");
                        }
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            }
            if (serializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (null != device) {
                        bRet = device.serialize(serializer);
                        if (false == bRet) {
                            serializer
                                    .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                            + " Device");
                        }
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            }
            if (serializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (null != operation) {
                        bRet = operation.serialize(serializer);
                        if (false == bRet) {
                            serializer
                                    .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                            + " Operation");
                        }
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            }
            if (serializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (null != mode) {
                        bRet = mode.serialize(serializer);
                        if (false == bRet) {
                            serializer
                                    .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                            + " Mode");
                        }
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            }
            if (serializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (null != movement) {
                        bRet = movement.serialize(serializer);
                        if (false == bRet) {
                            serializer
                                    .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                            + " Movement");
                        }
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            }
            if (serializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (null != temperatures) {
                        bRet = temperatures.serialize(serializer);
                        if (false == bRet) {
                            serializer
                                    .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                            + " Temperatures");
                        }
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            }
            if (serializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (null != temperature) {
                        bRet = temperature.serialize(serializer);
                        if (false == bRet) {
                            serializer
                                    .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                            + " Temperature");
                        }
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            }
            if (serializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (null != doors) {
                        bRet = doors.serialize(serializer);
                        if (false == bRet) {
                            serializer
                                    .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                            + " Doors");
                        }
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            }
            if (serializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (null != door) {
                        bRet = door.serialize(serializer);
                        if (false == bRet) {
                            serializer
                                    .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                            + " Door");
                        }
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            }
            if (serializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (null != humidity) {
                        bRet = humidity.serialize(serializer);
                        if (false == bRet) {
                            serializer
                                    .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                            + " Humidity");
                        }
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            }
            /*
             * if(serializer.isRecursive()) { { boolean bRet = true; if (null !=
             * wind) { bRet = wind.serialize(serializer); if (false == bRet) {
             * serializer
             * .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
             * + " Wind"); } } if (retValue == true) retValue = bRet; }
             * 
             * }
             */
            if (serializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (null != light) {
                        bRet = light.serialize(serializer);
                        if (false == bRet) {
                            serializer
                                    .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                            + " Light");
                        }
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            }
            /*
             * if(serializer.isRecursive()) { { boolean bRet = true; if (null !=
             * washer) { bRet = washer.serialize(serializer); if (false == bRet)
             * { serializer.setErrorMessage(
             * "Failed To Serialize parameter (Invalid content) : " +
             * " Washer"); } } if (retValue == true) retValue = bRet; }
             * 
             * }
             */
            /*
             * if(serializer.isRecursive()) { { boolean bRet = true; if (null !=
             * fridge) { bRet = fridge.serialize(serializer); if (false == bRet)
             * { serializer.setErrorMessage(
             * "Failed To Serialize parameter (Invalid content) : " +
             * " Fridge"); } } if (retValue == true) retValue = bRet; }
             * 
             * }
             */
            /*
             * if(serializer.isRecursive()) { { boolean bRet = true; if (null !=
             * oven) { bRet = oven.serialize(serializer); if (false == bRet) {
             * serializer
             * .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
             * + " Oven"); } } if (retValue == true) retValue = bRet; }
             * 
             * }
             */
            /*
             * if(serializer.isRecursive()) { { boolean bRet = true; if (null !=
             * customRecipes) { bRet = customRecipes.serialize(serializer); if
             * (false == bRet) { serializer.setErrorMessage(
             * "Failed To Serialize parameter (Invalid content) : " +
             * " CustomRecipes"); } } if (retValue == true) retValue = bRet; }
             * 
             * }
             */
            /*
             * if(serializer.isRecursive()) { { boolean bRet = true; if (null !=
             * customRecipe) { bRet = customRecipe.serialize(serializer); if
             * (false == bRet) { serializer.setErrorMessage(
             * "Failed To Serialize parameter (Invalid content) : " +
             * " CustomRecipe"); } } if (retValue == true) retValue = bRet; }
             * 
             * }
             */
            /*
             * if(serializer.isRecursive()) { { boolean bRet = true; if (null !=
             * audio) { bRet = audio.serialize(serializer); if (false == bRet) {
             * serializer
             * .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
             * + " Audio"); } } if (retValue == true) retValue = bRet; }
             * 
             * }
             */
            /*
             * if(serializer.isRecursive()) { { boolean bRet = true; if (null !=
             * aVSources) { bRet = aVSources.serialize(serializer); if (false ==
             * bRet) { serializer.setErrorMessage(
             * "Failed To Serialize parameter (Invalid content) : " +
             * " AVSources"); } } if (retValue == true) retValue = bRet; }
             * 
             * }
             */
            /*
             * if(serializer.isRecursive()) { { boolean bRet = true; if (null !=
             * aVSource) { bRet = aVSource.serialize(serializer); if (false ==
             * bRet) { serializer.setErrorMessage(
             * "Failed To Serialize parameter (Invalid content) : " +
             * " AVSource"); } } if (retValue == true) retValue = bRet; }
             * 
             * }
             */
            if (serializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (null != subscriptions) {
                        bRet = subscriptions.serialize(serializer);
                        if (false == bRet) {
                            serializer
                                    .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                            + " Subscriptions");
                        }
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            }
            if (serializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (null != subscription) {
                        bRet = subscription.serialize(serializer);
                        if (false == bRet) {
                            serializer
                                    .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                            + " Subscription");
                        }
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            }
            if (serializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (null != information) {
                        bRet = information.serialize(serializer);
                        if (false == bRet) {
                            serializer
                                    .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                            + " Information");
                        }
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

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
            if (serializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (null != version) {
                        bRet = version.serialize(serializer);
                        if (false == bRet) {
                            serializer
                                    .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                            + " Version");
                        }
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            }
            if (serializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (null != configuration) {
                        bRet = configuration.serialize(serializer);
                        if (false == bRet) {
                            serializer
                                    .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                            + " Configuration");
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
                    if (null != network) {
                        bRet = network.serialize(serializer);
                        if (false == bRet) {
                            serializer
                                    .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                            + " Network");
                        }
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

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
            if (serializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (null != accessPoints) {
                        bRet = accessPoints.serialize(serializer);
                        if (false == bRet) {
                            serializer
                                    .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                            + " AccessPoints");
                        }
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            }
            if (serializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (null != accessPoint) {
                        bRet = accessPoint.serialize(serializer);
                        if (false == bRet) {
                            serializer
                                    .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                            + " AccessPoint");
                        }
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            }
            if (serializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (null != energyConsumption) {
                        bRet = energyConsumption.serialize(serializer);
                        if (false == bRet) {
                            serializer
                                    .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                            + " EnergyConsumption");
                        }
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            }
            if (serializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (null != alarms) {
                        bRet = alarms.serialize(serializer);
                        if (false == bRet) {
                            serializer
                                    .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                            + " Alarms");
                        }
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            }
            if (serializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (null != alarm) {
                        bRet = alarm.serialize(serializer);
                        if (false == bRet) {
                            serializer
                                    .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                            + " Alarm");
                        }
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            }
            if (serializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (null != level) {
                        bRet = level.serialize(serializer);
                        if (false == bRet) {
                            serializer
                                    .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                            + " Level");
                        }
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            }
            if (serializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (null != messages) {
                        bRet = messages.serialize(serializer);
                        if (false == bRet) {
                            serializer
                                    .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                            + " Messages");
                        }
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            }
            if (serializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (null != message) {
                        bRet = message.serialize(serializer);
                        if (false == bRet) {
                            serializer
                                    .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                            + " Message");
                        }
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            }
            if (serializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (null != detection) {
                        bRet = detection.serialize(serializer);
                        if (false == bRet) {
                            serializer
                                    .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                            + " Detection");
                        }
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            }
            /*
             * if(serializer.isRecursive()) { { boolean bRet = true; if (null !=
             * camera) { bRet = camera.serialize(serializer); if (false == bRet)
             * { serializer.setErrorMessage(
             * "Failed To Serialize parameter (Invalid content) : " +
             * " Camera"); } } if (retValue == true) retValue = bRet; }
             * 
             * }
             */
            /*
             * if(serializer.isRecursive()) { { boolean bRet = true; if (null !=
             * captures) { bRet = captures.serialize(serializer); if (false ==
             * bRet) { serializer.setErrorMessage(
             * "Failed To Serialize parameter (Invalid content) : " +
             * " Captures"); } } if (retValue == true) retValue = bRet; }
             * 
             * }
             */
            /*
             * if(serializer.isRecursive()) { { boolean bRet = true; if (null !=
             * capture) { bRet = capture.serialize(serializer); if (false ==
             * bRet) { serializer.setErrorMessage(
             * "Failed To Serialize parameter (Invalid content) : " +
             * " Capture"); } } if (retValue == true) retValue = bRet; }
             * 
             * }
             */
            /*
             * if(serializer.isRecursive()) { { boolean bRet = true; if (null !=
             * groups) { bRet = groups.serialize(serializer); if (false == bRet)
             * { serializer.setErrorMessage(
             * "Failed To Serialize parameter (Invalid content) : " +
             * " Groups"); } } if (retValue == true) retValue = bRet; }
             * 
             * }
             */
            /*
             * if(serializer.isRecursive()) { { boolean bRet = true; if (null !=
             * group) { bRet = group.serialize(serializer); if (false == bRet) {
             * serializer
             * .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
             * + " Group"); } } if (retValue == true) retValue = bRet; }
             * 
             * }
             */
            if (serializer.isRecursive())
                /*
                 * { { boolean bRet = true; if (null != actions) { bRet =
                 * actions.serialize(serializer); if (false == bRet) {
                 * serializer.setErrorMessage(
                 * "Failed To Serialize parameter (Invalid content) : " +
                 * " Actions"); } } if (retValue == true) retValue = bRet; }
                 * 
                 * }
                 */
                /*
                 * if(serializer.isRecursive()) { { boolean bRet = true; if
                 * (null != action) { bRet = action.serialize(serializer); if
                 * (false == bRet) { serializer.setErrorMessage(
                 * "Failed To Serialize parameter (Invalid content) : " +
                 * " Action"); } } if (retValue == true) retValue = bRet; }
                 * 
                 * }
                 */
                /*
                 * if(serializer.isRecursive()) { { boolean bRet = true; if
                 * (null != patterns) { bRet = patterns.serialize(serializer);
                 * if (false == bRet) { serializer.setErrorMessage(
                 * "Failed To Serialize parameter (Invalid content) : " +
                 * " Patterns"); } } if (retValue == true) retValue = bRet; }
                 * 
                 * }
                 */
                /*
                 * if(serializer.isRecursive()) { { boolean bRet = true; if
                 * (null != pattern) { bRet = pattern.serialize(serializer); if
                 * (false == bRet) { serializer.setErrorMessage(
                 * "Failed To Serialize parameter (Invalid content) : " +
                 * " Pattern"); } } if (retValue == true) retValue = bRet; }
                 * 
                 * }
                 */
                if (serializer.isRecursive()) {
                    {
                        boolean bRet = true;
                        if (null != records) {
                            bRet = records.serialize(serializer);
                            if (false == bRet) {
                                serializer
                                        .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                                + " Records");
                            }
                        }
                        if (retValue == true)
                            retValue = bRet;
                    }

                }
            if (serializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (null != record) {
                        bRet = record.serialize(serializer);
                        if (false == bRet) {
                            serializer
                                    .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                            + " Record");
                        }
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            }
            {
                boolean bRet = true;
                if (null != energy) {
                    bRet = energy.serialize(serializer);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " Energy");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (null != usageLimits) {
                        bRet = usageLimits.serialize(serializer);
                        if (false == bRet) {
                            serializer
                                    .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                            + " UsageLimits");
                        }
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            }
            if (serializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (null != usageLimit) {
                        bRet = usageLimit.serialize(serializer);
                        if (false == bRet) {
                            serializer
                                    .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                            + " UsageLimit");
                        }
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            }
            /*
             * if(serializer.isRecursive()) { { boolean bRet = true; if (null !=
             * foodManager) { bRet = foodManager.serialize(serializer); if
             * (false == bRet) { serializer.setErrorMessage(
             * "Failed To Serialize parameter (Invalid content) : " +
             * " FoodManager"); } } if (retValue == true) retValue = bRet; }
             * 
             * }
             */
            /*
             * if(serializer.isRecursive()) { { boolean bRet = true; if (null !=
             * foodList) { bRet = foodList.serialize(serializer); if (false ==
             * bRet) { serializer.setErrorMessage(
             * "Failed To Serialize parameter (Invalid content) : " +
             * " FoodList"); } } if (retValue == true) retValue = bRet; }
             * 
             * }
             */
            /*
             * if(serializer.isRecursive()) { { boolean bRet = true; if (null !=
             * food) { bRet = food.serialize(serializer); if (false == bRet) {
             * serializer
             * .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
             * + " Food"); } } if (retValue == true) retValue = bRet; }
             * 
             * }
             */
            /*
             * if(serializer.isRecursive()) { { boolean bRet = true; if (null !=
             * shoppingList) { bRet = shoppingList.serialize(serializer); if
             * (false == bRet) { serializer.setErrorMessage(
             * "Failed To Serialize parameter (Invalid content) : " +
             * " ShoppingList"); } } if (retValue == true) retValue = bRet; }
             * 
             * }
             */
            /*
             * if(serializer.isRecursive()) { { boolean bRet = true; if (null !=
             * shopping) { bRet = shopping.serialize(serializer); if (false ==
             * bRet) { serializer.setErrorMessage(
             * "Failed To Serialize parameter (Invalid content) : " +
             * " Shopping"); } } if (retValue == true) retValue = bRet; }
             * 
             * }
             */
            /*
             * if(serializer.isRecursive()) { { boolean bRet = true; if (null !=
             * foodImages) { bRet = foodImages.serialize(serializer); if (false
             * == bRet) { serializer.setErrorMessage(
             * "Failed To Serialize parameter (Invalid content) : " +
             * " FoodImages"); } } if (retValue == true) retValue = bRet; }
             * 
             * }
             */
            /*
             * if(serializer.isRecursive()) { { boolean bRet = true; if (null !=
             * foodImage) { bRet = foodImage.serialize(serializer); if (false ==
             * bRet) { serializer.setErrorMessage(
             * "Failed To Serialize parameter (Invalid content) : " +
             * " FoodImage"); } } if (retValue == true) retValue = bRet; }
             * 
             * }
             */
            /*
             * if(serializer.isRecursive()) { { boolean bRet = true; if (null !=
             * sensors) { bRet = sensors.serialize(serializer); if (false ==
             * bRet) { serializer.setErrorMessage(
             * "Failed To Serialize parameter (Invalid content) : " +
             * " Sensors"); } } if (retValue == true) retValue = bRet; }
             * 
             * }
             */
            /*
             * if(serializer.isRecursive()) { { boolean bRet = true; if (null !=
             * sensor) { bRet = sensor.serialize(serializer); if (false == bRet)
             * { serializer.setErrorMessage(
             * "Failed To Serialize parameter (Invalid content) : " +
             * " Sensor"); } } if (retValue == true) retValue = bRet; }
             * 
             * }
             */
            /*
             * if(serializer.isRecursive()) { { boolean bRet = true; if (null !=
             * consumables) { bRet = consumables.serialize(serializer); if
             * (false == bRet) { serializer.setErrorMessage(
             * "Failed To Serialize parameter (Invalid content) : " +
             * " Consumables"); } } if (retValue == true) retValue = bRet; }
             * 
             * }
             */
            /*
             * if(serializer.isRecursive()) { { boolean bRet = true; if (null !=
             * consumable) { bRet = consumable.serialize(serializer); if (false
             * == bRet) { serializer.setErrorMessage(
             * "Failed To Serialize parameter (Invalid content) : " +
             * " Consumable"); } } if (retValue == true) retValue = bRet; }
             * 
             * }
             */
        } catch (Exception e) {
            return false;
        }
        return retValue;

    }

    /**
     * Deserializes data and prepares the Events object contents.
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
                if (true == deserializer.hasProperty("resourceURI")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        resourceURI = deserializer
                                .getStringPropertyValue("resourceURI");
                    }
                } else {
                    if (deserializer.isInFilteringMode("resourceURI") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " resourceURI");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("event")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        Event = new gen.xsd.NotificationEventType();
                        Event.value = deserializer
                                .getStringPropertyValue("event");
                        if (false == Event.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " event");
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
                if (true == deserializer.hasProperty("eventTime")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        EventTime = new OC.Cm.Serialization.Xsd.DateTimeType();
                        EventTime.value = deserializer
                                .getStringPropertyValue("eventTime");
                        if (false == EventTime.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " eventTime");
                            bRet = false;
                        } else
                            bRet = true;
                    }
                } else {
                    if (deserializer.isInFilteringMode("eventTime") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " eventTime");
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
                    if (true == deserializer.hasGroupProperty("Capability")) {
                        if (bRet == true) {
                            capability = new gen.xsd.Capability();
                            bRet = capability.deSerialize(deserializer);
                        }
                        if (false == bRet)
                            deserializer
                                    .setErrorMessage("Failed To deserialize : "
                                            + " Capability");
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            } else {
                if (true == deserializer.hasProperty("Capability")) {
                    retValue = false;
                    deserializer
                            .setErrorMessage("Additional data to deserialize: Capability");
                }
            }
            if (deserializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (true == deserializer.hasGroupProperty("Devices")) {
                        if (bRet == true) {
                            devices = new gen.xsd.Devices();
                            bRet = devices.deSerialize(deserializer);
                        }
                        if (false == bRet)
                            deserializer
                                    .setErrorMessage("Failed To deserialize : "
                                            + " Devices");
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            } else {
                if (true == deserializer.hasProperty("Devices")) {
                    retValue = false;
                    deserializer
                            .setErrorMessage("Additional data to deserialize: Devices");
                }
            }
            if (deserializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (true == deserializer.hasGroupProperty("Device")) {
                        if (bRet == true) {
                            device = new gen.xsd.Device();
                            bRet = device.deSerialize(deserializer);
                        }
                        if (false == bRet)
                            deserializer
                                    .setErrorMessage("Failed To deserialize : "
                                            + " Device");
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            } else {
                if (true == deserializer.hasProperty("Device")) {
                    retValue = false;
                    deserializer
                            .setErrorMessage("Additional data to deserialize: Device");
                }
            }
            if (deserializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (true == deserializer.hasGroupProperty("Operation")) {
                        if (bRet == true) {
                            operation = new gen.xsd.Operation();
                            bRet = operation.deSerialize(deserializer);
                        }
                        if (false == bRet)
                            deserializer
                                    .setErrorMessage("Failed To deserialize : "
                                            + " Operation");
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            } else {
                if (true == deserializer.hasProperty("Operation")) {
                    retValue = false;
                    deserializer
                            .setErrorMessage("Additional data to deserialize: Operation");
                }
            }
            if (deserializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (true == deserializer.hasGroupProperty("Mode")) {
                        if (bRet == true) {
                            mode = new gen.xsd.Mode();
                            bRet = mode.deSerialize(deserializer);
                        }
                        if (false == bRet)
                            deserializer
                                    .setErrorMessage("Failed To deserialize : "
                                            + " Mode");
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            } else {
                if (true == deserializer.hasProperty("Mode")) {
                    retValue = false;
                    deserializer
                            .setErrorMessage("Additional data to deserialize: Mode");
                }
            }
            if (deserializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (true == deserializer.hasGroupProperty("Movement")) {
                        if (bRet == true) {
                            movement = new gen.xsd.Movement();
                            bRet = movement.deSerialize(deserializer);
                        }
                        if (false == bRet)
                            deserializer
                                    .setErrorMessage("Failed To deserialize : "
                                            + " Movement");
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            } else {
                if (true == deserializer.hasProperty("Movement")) {
                    retValue = false;
                    deserializer
                            .setErrorMessage("Additional data to deserialize: Movement");
                }
            }
            if (deserializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (true == deserializer.hasGroupProperty("Temperatures")) {
                        if (bRet == true) {
                            temperatures = new gen.xsd.Temperatures();
                            bRet = temperatures.deSerialize(deserializer);
                        }
                        if (false == bRet)
                            deserializer
                                    .setErrorMessage("Failed To deserialize : "
                                            + " Temperatures");
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            } else {
                if (true == deserializer.hasProperty("Temperatures")) {
                    retValue = false;
                    deserializer
                            .setErrorMessage("Additional data to deserialize: Temperatures");
                }
            }
            if (deserializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (true == deserializer.hasGroupProperty("Temperature")) {
                        if (bRet == true) {
                            temperature = new gen.xsd.Temperature();
                            bRet = temperature.deSerialize(deserializer);
                        }
                        if (false == bRet)
                            deserializer
                                    .setErrorMessage("Failed To deserialize : "
                                            + " Temperature");
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            } else {
                if (true == deserializer.hasProperty("Temperature")) {
                    retValue = false;
                    deserializer
                            .setErrorMessage("Additional data to deserialize: Temperature");
                }
            }
            if (deserializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (true == deserializer.hasGroupProperty("Doors")) {
                        if (bRet == true) {
                            doors = new gen.xsd.Doors();
                            bRet = doors.deSerialize(deserializer);
                        }
                        if (false == bRet)
                            deserializer
                                    .setErrorMessage("Failed To deserialize : "
                                            + " Doors");
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            } else {
                if (true == deserializer.hasProperty("Doors")) {
                    retValue = false;
                    deserializer
                            .setErrorMessage("Additional data to deserialize: Doors");
                }
            }
            if (deserializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (true == deserializer.hasGroupProperty("Door")) {
                        if (bRet == true) {
                            door = new gen.xsd.Door();
                            bRet = door.deSerialize(deserializer);
                        }
                        if (false == bRet)
                            deserializer
                                    .setErrorMessage("Failed To deserialize : "
                                            + " Door");
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            } else {
                if (true == deserializer.hasProperty("Door")) {
                    retValue = false;
                    deserializer
                            .setErrorMessage("Additional data to deserialize: Door");
                }
            }
            if (deserializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (true == deserializer.hasGroupProperty("Humidity")) {
                        if (bRet == true) {
                            humidity = new gen.xsd.Humidity();
                            bRet = humidity.deSerialize(deserializer);
                        }
                        if (false == bRet)
                            deserializer
                                    .setErrorMessage("Failed To deserialize : "
                                            + " Humidity");
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            } else {
                if (true == deserializer.hasProperty("Humidity")) {
                    retValue = false;
                    deserializer
                            .setErrorMessage("Additional data to deserialize: Humidity");
                }
            }
            /*
             * if(deserializer.isRecursive()) { { boolean bRet = true; if (true
             * == deserializer.hasGroupProperty("Wind")) { if(bRet == true) {
             * wind = new gen.xsd.Wind(); bRet = wind.deSerialize(deserializer);
             * } if (false == bRet)
             * deserializer.setErrorMessage("Failed To deserialize : " +
             * " Wind"); } if (retValue == true) retValue=bRet; }
             * 
             * } else { if (true == deserializer.hasProperty("Wind")) { retValue
             * = false;
             * deserializer.setErrorMessage("Additional data to deserialize: Wind"
             * ); } }
             */
            if (deserializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (true == deserializer.hasGroupProperty("Light")) {
                        if (bRet == true) {
                            light = new gen.xsd.Light();
                            bRet = light.deSerialize(deserializer);
                        }
                        if (false == bRet)
                            deserializer
                                    .setErrorMessage("Failed To deserialize : "
                                            + " Light");
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            } else {
                if (true == deserializer.hasProperty("Light")) {
                    retValue = false;
                    deserializer
                            .setErrorMessage("Additional data to deserialize: Light");
                }
            }
            /*
             * if(deserializer.isRecursive()) { { boolean bRet = true; if (true
             * == deserializer.hasGroupProperty("Washer")) { if(bRet == true) {
             * washer = new gen.xsd.Washer(); bRet =
             * washer.deSerialize(deserializer); } if (false == bRet)
             * deserializer.setErrorMessage("Failed To deserialize : " +
             * " Washer"); } if (retValue == true) retValue=bRet; }
             * 
             * } else { if (true == deserializer.hasProperty("Washer")) {
             * retValue = false;
             * deserializer.setErrorMessage("Additional data to deserialize: Washer"
             * ); } }
             */
            /*
             * if(deserializer.isRecursive()) { { boolean bRet = true; if (true
             * == deserializer.hasGroupProperty("Fridge")) { if(bRet == true) {
             * fridge = new gen.xsd.Fridge(); bRet =
             * fridge.deSerialize(deserializer); } if (false == bRet)
             * deserializer.setErrorMessage("Failed To deserialize : " +
             * " Fridge"); } if (retValue == true) retValue=bRet; }
             * 
             * } else { if (true == deserializer.hasProperty("Fridge")) {
             * retValue = false;
             * deserializer.setErrorMessage("Additional data to deserialize: Fridge"
             * ); } }
             */
            /*
             * if(deserializer.isRecursive()) { { boolean bRet = true; if (true
             * == deserializer.hasGroupProperty("Oven")) { if(bRet == true) {
             * oven = new gen.xsd.Oven(); bRet = oven.deSerialize(deserializer);
             * } if (false == bRet)
             * deserializer.setErrorMessage("Failed To deserialize : " +
             * " Oven"); } if (retValue == true) retValue=bRet; }
             * 
             * } else { if (true == deserializer.hasProperty("Oven")) { retValue
             * = false;
             * deserializer.setErrorMessage("Additional data to deserialize: Oven"
             * ); } }
             */
            /*
             * if(deserializer.isRecursive()) { { boolean bRet = true; if (true
             * == deserializer.hasGroupProperty("CustomRecipes")) { if(bRet ==
             * true) { customRecipes = new gen.xsd.CustomRecipes(); bRet =
             * customRecipes.deSerialize(deserializer); } if (false == bRet)
             * deserializer.setErrorMessage("Failed To deserialize : " +
             * " CustomRecipes"); } if (retValue == true) retValue=bRet; }
             * 
             * } else { if (true == deserializer.hasProperty("CustomRecipes")) {
             * retValue = false; deserializer.setErrorMessage(
             * "Additional data to deserialize: CustomRecipes"); } }
             */
            /*
             * if(deserializer.isRecursive()) { { boolean bRet = true; if (true
             * == deserializer.hasGroupProperty("CustomRecipe")) { if(bRet ==
             * true) { customRecipe = new gen.xsd.CustomRecipe(); bRet =
             * customRecipe.deSerialize(deserializer); } if (false == bRet)
             * deserializer.setErrorMessage("Failed To deserialize : " +
             * " CustomRecipe"); } if (retValue == true) retValue=bRet; }
             * 
             * } else { if (true == deserializer.hasProperty("CustomRecipe")) {
             * retValue = false; deserializer.setErrorMessage(
             * "Additional data to deserialize: CustomRecipe"); } }
             */
            /*
             * if(deserializer.isRecursive()) { { boolean bRet = true; if (true
             * == deserializer.hasGroupProperty("Audio")) { if(bRet == true) {
             * audio = new gen.xsd.Audio(); bRet =
             * audio.deSerialize(deserializer); } if (false == bRet)
             * deserializer.setErrorMessage("Failed To deserialize : " +
             * " Audio"); } if (retValue == true) retValue=bRet; }
             * 
             * } else { if (true == deserializer.hasProperty("Audio")) {
             * retValue = false;
             * deserializer.setErrorMessage("Additional data to deserialize: Audio"
             * ); } }
             */
            /*
             * if(deserializer.isRecursive()) { { boolean bRet = true; if (true
             * == deserializer.hasGroupProperty("AVSources")) { if(bRet == true)
             * { aVSources = new gen.xsd.AVSources(); bRet =
             * aVSources.deSerialize(deserializer); } if (false == bRet)
             * deserializer.setErrorMessage("Failed To deserialize : " +
             * " AVSources"); } if (retValue == true) retValue=bRet; }
             * 
             * } else { if (true == deserializer.hasProperty("AVSources")) {
             * retValue = false; deserializer.setErrorMessage(
             * "Additional data to deserialize: AVSources"); } }
             */
            /*
             * if(deserializer.isRecursive()) { { boolean bRet = true; if (true
             * == deserializer.hasGroupProperty("AVSource")) { if(bRet == true)
             * { aVSource = new gen.xsd.AVSource(); bRet =
             * aVSource.deSerialize(deserializer); } if (false == bRet)
             * deserializer.setErrorMessage("Failed To deserialize : " +
             * " AVSource"); } if (retValue == true) retValue=bRet; }
             * 
             * } else { if (true == deserializer.hasProperty("AVSource")) {
             * retValue = false; deserializer.setErrorMessage(
             * "Additional data to deserialize: AVSource"); } }
             */
            if (deserializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (true == deserializer.hasGroupProperty("Subscriptions")) {
                        if (bRet == true) {
                            subscriptions = new gen.xsd.Subscriptions();
                            bRet = subscriptions.deSerialize(deserializer);
                        }
                        if (false == bRet)
                            deserializer
                                    .setErrorMessage("Failed To deserialize : "
                                            + " Subscriptions");
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            } else {
                if (true == deserializer.hasProperty("Subscriptions")) {
                    retValue = false;
                    deserializer
                            .setErrorMessage("Additional data to deserialize: Subscriptions");
                }
            }
            if (deserializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (true == deserializer.hasGroupProperty("Subscription")) {
                        if (bRet == true) {
                            subscription = new gen.xsd.Subscription();
                            bRet = subscription.deSerialize(deserializer);
                        }
                        if (false == bRet)
                            deserializer
                                    .setErrorMessage("Failed To deserialize : "
                                            + " Subscription");
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            } else {
                if (true == deserializer.hasProperty("Subscription")) {
                    retValue = false;
                    deserializer
                            .setErrorMessage("Additional data to deserialize: Subscription");
                }
            }
            if (deserializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (true == deserializer.hasGroupProperty("Information")) {
                        if (bRet == true) {
                            information = new gen.xsd.Information();
                            bRet = information.deSerialize(deserializer);
                        }
                        if (false == bRet)
                            deserializer
                                    .setErrorMessage("Failed To deserialize : "
                                            + " Information");
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            } else {
                if (true == deserializer.hasProperty("Information")) {
                    retValue = false;
                    deserializer
                            .setErrorMessage("Additional data to deserialize: Information");
                }
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
            if (deserializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (true == deserializer.hasGroupProperty("Version")) {
                        if (bRet == true) {
                            version = new gen.xsd.Version();
                            bRet = version.deSerialize(deserializer);
                        }
                        if (false == bRet)
                            deserializer
                                    .setErrorMessage("Failed To deserialize : "
                                            + " Version");
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            } else {
                if (true == deserializer.hasProperty("Version")) {
                    retValue = false;
                    deserializer
                            .setErrorMessage("Additional data to deserialize: Version");
                }
            }
            if (deserializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (true == deserializer.hasGroupProperty("Configuration")) {
                        if (bRet == true) {
                            configuration = new gen.xsd.Configuration();
                            bRet = configuration.deSerialize(deserializer);
                        }
                        if (false == bRet)
                            deserializer
                                    .setErrorMessage("Failed To deserialize : "
                                            + " Configuration");
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            } else {
                if (true == deserializer.hasProperty("Configuration")) {
                    retValue = false;
                    deserializer
                            .setErrorMessage("Additional data to deserialize: Configuration");
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
                    if (true == deserializer.hasGroupProperty("Network")) {
                        if (bRet == true) {
                            network = new gen.xsd.Network();
                            bRet = network.deSerialize(deserializer);
                        }
                        if (false == bRet)
                            deserializer
                                    .setErrorMessage("Failed To deserialize : "
                                            + " Network");
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            } else {
                if (true == deserializer.hasProperty("Network")) {
                    retValue = false;
                    deserializer
                            .setErrorMessage("Additional data to deserialize: Network");
                }
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
            if (deserializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (true == deserializer.hasGroupProperty("AccessPoints")) {
                        if (bRet == true) {
                            accessPoints = new gen.xsd.AccessPoints();
                            bRet = accessPoints.deSerialize(deserializer);
                        }
                        if (false == bRet)
                            deserializer
                                    .setErrorMessage("Failed To deserialize : "
                                            + " AccessPoints");
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            } else {
                if (true == deserializer.hasProperty("AccessPoints")) {
                    retValue = false;
                    deserializer
                            .setErrorMessage("Additional data to deserialize: AccessPoints");
                }
            }
            if (deserializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (true == deserializer.hasGroupProperty("AccessPoint")) {
                        if (bRet == true) {
                            accessPoint = new gen.xsd.AccessPoint();
                            bRet = accessPoint.deSerialize(deserializer);
                        }
                        if (false == bRet)
                            deserializer
                                    .setErrorMessage("Failed To deserialize : "
                                            + " AccessPoint");
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            } else {
                if (true == deserializer.hasProperty("AccessPoint")) {
                    retValue = false;
                    deserializer
                            .setErrorMessage("Additional data to deserialize: AccessPoint");
                }
            }
            if (deserializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (true == deserializer
                            .hasGroupProperty("EnergyConsumption")) {
                        if (bRet == true) {
                            energyConsumption = new gen.xsd.EnergyConsumption();
                            bRet = energyConsumption.deSerialize(deserializer);
                        }
                        if (false == bRet)
                            deserializer
                                    .setErrorMessage("Failed To deserialize : "
                                            + " EnergyConsumption");
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            } else {
                if (true == deserializer.hasProperty("EnergyConsumption")) {
                    retValue = false;
                    deserializer
                            .setErrorMessage("Additional data to deserialize: EnergyConsumption");
                }
            }
            if (deserializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (true == deserializer.hasGroupProperty("Alarms")) {
                        if (bRet == true) {
                            alarms = new gen.xsd.Alarms();
                            bRet = alarms.deSerialize(deserializer);
                        }
                        if (false == bRet)
                            deserializer
                                    .setErrorMessage("Failed To deserialize : "
                                            + " Alarms");
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            } else {
                if (true == deserializer.hasProperty("Alarms")) {
                    retValue = false;
                    deserializer
                            .setErrorMessage("Additional data to deserialize: Alarms");
                }
            }
            if (deserializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (true == deserializer.hasGroupProperty("Alarm")) {
                        if (bRet == true) {
                            alarm = new gen.xsd.Alarm();
                            bRet = alarm.deSerialize(deserializer);
                        }
                        if (false == bRet)
                            deserializer
                                    .setErrorMessage("Failed To deserialize : "
                                            + " Alarm");
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            } else {
                if (true == deserializer.hasProperty("Alarm")) {
                    retValue = false;
                    deserializer
                            .setErrorMessage("Additional data to deserialize: Alarm");
                }
            }
            if (deserializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (true == deserializer.hasGroupProperty("Level")) {
                        if (bRet == true) {
                            level = new gen.xsd.Level();
                            bRet = level.deSerialize(deserializer);
                        }
                        if (false == bRet)
                            deserializer
                                    .setErrorMessage("Failed To deserialize : "
                                            + " Level");
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            } else {
                if (true == deserializer.hasProperty("Level")) {
                    retValue = false;
                    deserializer
                            .setErrorMessage("Additional data to deserialize: Level");
                }
            }
            if (deserializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (true == deserializer.hasGroupProperty("Messages")) {
                        if (bRet == true) {
                            messages = new gen.xsd.Messages();
                            bRet = messages.deSerialize(deserializer);
                        }
                        if (false == bRet)
                            deserializer
                                    .setErrorMessage("Failed To deserialize : "
                                            + " Messages");
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            } else {
                if (true == deserializer.hasProperty("Messages")) {
                    retValue = false;
                    deserializer
                            .setErrorMessage("Additional data to deserialize: Messages");
                }
            }
            if (deserializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (true == deserializer.hasGroupProperty("Message")) {
                        if (bRet == true) {
                            message = new gen.xsd.Message();
                            bRet = message.deSerialize(deserializer);
                        }
                        if (false == bRet)
                            deserializer
                                    .setErrorMessage("Failed To deserialize : "
                                            + " Message");
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            } else {
                if (true == deserializer.hasProperty("Message")) {
                    retValue = false;
                    deserializer
                            .setErrorMessage("Additional data to deserialize: Message");
                }
            }
            if (deserializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (true == deserializer.hasGroupProperty("Detection")) {
                        if (bRet == true) {
                            detection = new gen.xsd.Detection();
                            bRet = detection.deSerialize(deserializer);
                        }
                        if (false == bRet)
                            deserializer
                                    .setErrorMessage("Failed To deserialize : "
                                            + " Detection");
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            } else {
                if (true == deserializer.hasProperty("Detection")) {
                    retValue = false;
                    deserializer
                            .setErrorMessage("Additional data to deserialize: Detection");
                }
            }
            /*
             * if(deserializer.isRecursive()) { { boolean bRet = true; if (true
             * == deserializer.hasGroupProperty("Camera")) { if(bRet == true) {
             * camera = new gen.xsd.Camera(); bRet =
             * camera.deSerialize(deserializer); } if (false == bRet)
             * deserializer.setErrorMessage("Failed To deserialize : " +
             * " Camera"); } if (retValue == true) retValue=bRet; }
             * 
             * } else { if (true == deserializer.hasProperty("Camera")) {
             * retValue = false;
             * deserializer.setErrorMessage("Additional data to deserialize: Camera"
             * ); } }
             */
            /*
             * if(deserializer.isRecursive()) { { boolean bRet = true; if (true
             * == deserializer.hasGroupProperty("Captures")) { if(bRet == true)
             * { captures = new gen.xsd.Captures(); bRet =
             * captures.deSerialize(deserializer); } if (false == bRet)
             * deserializer.setErrorMessage("Failed To deserialize : " +
             * " Captures"); } if (retValue == true) retValue=bRet; }
             * 
             * } else { if (true == deserializer.hasProperty("Captures")) {
             * retValue = false; deserializer.setErrorMessage(
             * "Additional data to deserialize: Captures"); } }
             */
            /*
             * if(deserializer.isRecursive()) { { boolean bRet = true; if (true
             * == deserializer.hasGroupProperty("Capture")) { if(bRet == true) {
             * capture = new gen.xsd.Camera(); bRet =
             * capture.deSerialize(deserializer); } if (false == bRet)
             * deserializer.setErrorMessage("Failed To deserialize : " +
             * " Capture"); } if (retValue == true) retValue=bRet; }
             * 
             * } else { if (true == deserializer.hasProperty("Capture")) {
             * retValue = false; deserializer.setErrorMessage(
             * "Additional data to deserialize: Capture"); } }
             */
            /*
             * if(deserializer.isRecursive()) { { boolean bRet = true; if (true
             * == deserializer.hasGroupProperty("Groups")) { if(bRet == true) {
             * groups = new gen.xsd.Groups(); bRet =
             * groups.deSerialize(deserializer); } if (false == bRet)
             * deserializer.setErrorMessage("Failed To deserialize : " +
             * " Groups"); } if (retValue == true) retValue=bRet; }
             * 
             * } else { if (true == deserializer.hasProperty("Groups")) {
             * retValue = false;
             * deserializer.setErrorMessage("Additional data to deserialize: Groups"
             * ); } }
             */
            /*
             * if(deserializer.isRecursive()) { { boolean bRet = true; if (true
             * == deserializer.hasGroupProperty("Group")) { if(bRet == true) {
             * group = new gen.xsd.Group(); bRet =
             * group.deSerialize(deserializer); } if (false == bRet)
             * deserializer.setErrorMessage("Failed To deserialize : " +
             * " Group"); } if (retValue == true) retValue=bRet; }
             * 
             * } else { if (true == deserializer.hasProperty("Group")) {
             * retValue = false;
             * deserializer.setErrorMessage("Additional data to deserialize: Group"
             * ); } }
             */
            /*
             * if(deserializer.isRecursive()) { { boolean bRet = true; if (true
             * == deserializer.hasGroupProperty("Actions")) { if(bRet == true) {
             * actions = new gen.xsd.Actions(); bRet =
             * actions.deSerialize(deserializer); } if (false == bRet)
             * deserializer.setErrorMessage("Failed To deserialize : " +
             * " Actions"); } if (retValue == true) retValue=bRet; }
             * 
             * } else { if (true == deserializer.hasProperty("Actions")) {
             * retValue = false; deserializer.setErrorMessage(
             * "Additional data to deserialize: Actions"); } }
             */
            /*
             * if(deserializer.isRecursive()) { { boolean bRet = true; if (true
             * == deserializer.hasGroupProperty("Action")) { if(bRet == true) {
             * action = new gen.xsd.Action(); bRet =
             * action.deSerialize(deserializer); } if (false == bRet)
             * deserializer.setErrorMessage("Failed To deserialize : " +
             * " Action"); } if (retValue == true) retValue=bRet; }
             * 
             * } else { if (true == deserializer.hasProperty("Action")) {
             * retValue = false;
             * deserializer.setErrorMessage("Additional data to deserialize: Action"
             * ); } }
             */
            /*
             * if(deserializer.isRecursive()) { { boolean bRet = true; if (true
             * == deserializer.hasGroupProperty("Patterns")) { if(bRet == true)
             * { patterns = new gen.xsd.Patterns(); bRet =
             * patterns.deSerialize(deserializer); } if (false == bRet)
             * deserializer.setErrorMessage("Failed To deserialize : " +
             * " Patterns"); } if (retValue == true) retValue=bRet; }
             * 
             * } else { if (true == deserializer.hasProperty("Patterns")) {
             * retValue = false; deserializer.setErrorMessage(
             * "Additional data to deserialize: Patterns"); } }
             */
            /*
             * if(deserializer.isRecursive()) { { boolean bRet = true; if (true
             * == deserializer.hasGroupProperty("Pattern")) { if(bRet == true) {
             * pattern = new gen.xsd.Pattern(); bRet =
             * pattern.deSerialize(deserializer); } if (false == bRet)
             * deserializer.setErrorMessage("Failed To deserialize : " +
             * " Pattern"); } if (retValue == true) retValue=bRet; }
             * 
             * } else { if (true == deserializer.hasProperty("Pattern")) {
             * retValue = false; deserializer.setErrorMessage(
             * "Additional data to deserialize: Pattern"); } }
             */
            if (deserializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (true == deserializer.hasGroupProperty("Records")) {
                        if (bRet == true) {
                            records = new gen.xsd.Records();
                            bRet = records.deSerialize(deserializer);
                        }
                        if (false == bRet)
                            deserializer
                                    .setErrorMessage("Failed To deserialize : "
                                            + " Records");
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            } else {
                if (true == deserializer.hasProperty("Records")) {
                    retValue = false;
                    deserializer
                            .setErrorMessage("Additional data to deserialize: Records");
                }
            }
            if (deserializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (true == deserializer.hasGroupProperty("Record")) {
                        if (bRet == true) {
                            record = new gen.xsd.Record();
                            bRet = record.deSerialize(deserializer);
                        }
                        if (false == bRet)
                            deserializer
                                    .setErrorMessage("Failed To deserialize : "
                                            + " Record");
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            } else {
                if (true == deserializer.hasProperty("Record")) {
                    retValue = false;
                    deserializer
                            .setErrorMessage("Additional data to deserialize: Record");
                }
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasGroupProperty("Energy")) {
                    if (bRet == true) {
                        energy = new gen.xsd.Energy();
                        bRet = energy.deSerialize(deserializer);
                    }
                    if (false == bRet)
                        deserializer.setErrorMessage("Failed To deserialize : "
                                + " Energy");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (deserializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (true == deserializer.hasGroupProperty("UsageLimits")) {
                        if (bRet == true) {
                            usageLimits = new gen.xsd.UsageLimits();
                            bRet = usageLimits.deSerialize(deserializer);
                        }
                        if (false == bRet)
                            deserializer
                                    .setErrorMessage("Failed To deserialize : "
                                            + " UsageLimits");
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            } else {
                if (true == deserializer.hasProperty("UsageLimits")) {
                    retValue = false;
                    deserializer
                            .setErrorMessage("Additional data to deserialize: UsageLimits");
                }
            }
            if (deserializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (true == deserializer.hasGroupProperty("UsageLimit")) {
                        if (bRet == true) {
                            usageLimit = new gen.xsd.UsageLimit();
                            bRet = usageLimit.deSerialize(deserializer);
                        }
                        if (false == bRet)
                            deserializer
                                    .setErrorMessage("Failed To deserialize : "
                                            + " UsageLimit");
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            } else {
                if (true == deserializer.hasProperty("UsageLimit")) {
                    retValue = false;
                    deserializer
                            .setErrorMessage("Additional data to deserialize: UsageLimit");
                }
            }
            /*
             * if(deserializer.isRecursive()) { { boolean bRet = true; if (true
             * == deserializer.hasGroupProperty("FoodManager")) { if(bRet ==
             * true) { foodManager = new gen.xsd.FoodManager(); bRet =
             * foodManager.deSerialize(deserializer); } if (false == bRet)
             * deserializer.setErrorMessage("Failed To deserialize : " +
             * " FoodManager"); } if (retValue == true) retValue=bRet; }
             * 
             * } else { if (true == deserializer.hasProperty("FoodManager")) {
             * retValue = false; deserializer.setErrorMessage(
             * "Additional data to deserialize: FoodManager"); } }
             */
            /*
             * if(deserializer.isRecursive()) { { boolean bRet = true; if (true
             * == deserializer.hasGroupProperty("FoodList")) { if(bRet == true)
             * { foodList = new gen.xsd.FoodList(); bRet =
             * foodList.deSerialize(deserializer); } if (false == bRet)
             * deserializer.setErrorMessage("Failed To deserialize : " +
             * " FoodList"); } if (retValue == true) retValue=bRet; }
             * 
             * } else { if (true == deserializer.hasProperty("FoodList")) {
             * retValue = false; deserializer.setErrorMessage(
             * "Additional data to deserialize: FoodList"); } }
             */
            /*
             * if(deserializer.isRecursive()) { { boolean bRet = true; if (true
             * == deserializer.hasGroupProperty("Food")) { if(bRet == true) {
             * food = new gen.xsd.Food(); bRet = food.deSerialize(deserializer);
             * } if (false == bRet)
             * deserializer.setErrorMessage("Failed To deserialize : " +
             * " Food"); } if (retValue == true) retValue=bRet; }
             * 
             * } else { if (true == deserializer.hasProperty("Food")) { retValue
             * = false;
             * deserializer.setErrorMessage("Additional data to deserialize: Food"
             * ); } }
             */
            /*
             * if(deserializer.isRecursive()) { { boolean bRet = true; if (true
             * == deserializer.hasGroupProperty("ShoppingList")) { if(bRet ==
             * true) { shoppingList = new gen.xsd.ShoppingList(); bRet =
             * shoppingList.deSerialize(deserializer); } if (false == bRet)
             * deserializer.setErrorMessage("Failed To deserialize : " +
             * " ShoppingList"); } if (retValue == true) retValue=bRet; }
             * 
             * } else { if (true == deserializer.hasProperty("ShoppingList")) {
             * retValue = false; deserializer.setErrorMessage(
             * "Additional data to deserialize: ShoppingList"); } }
             */
            /*
             * if(deserializer.isRecursive()) { { boolean bRet = true; if (true
             * == deserializer.hasGroupProperty("Shopping")) { if(bRet == true)
             * { shopping = new gen.xsd.Shopping(); bRet =
             * shopping.deSerialize(deserializer); } if (false == bRet)
             * deserializer.setErrorMessage("Failed To deserialize : " +
             * " Shopping"); } if (retValue == true) retValue=bRet; }
             * 
             * } else { if (true == deserializer.hasProperty("Shopping")) {
             * retValue = false; deserializer.setErrorMessage(
             * "Additional data to deserialize: Shopping"); } }
             */
            /*
             * if(deserializer.isRecursive()) { { boolean bRet = true; if (true
             * == deserializer.hasGroupProperty("FoodImages")) { if(bRet ==
             * true) { foodImages = new gen.xsd.FoodImages(); bRet =
             * foodImages.deSerialize(deserializer); } if (false == bRet)
             * deserializer.setErrorMessage("Failed To deserialize : " +
             * " FoodImages"); } if (retValue == true) retValue=bRet; }
             * 
             * } else { if (true == deserializer.hasProperty("FoodImages")) {
             * retValue = false; deserializer.setErrorMessage(
             * "Additional data to deserialize: FoodImages"); } }
             */
            /*
             * if(deserializer.isRecursive()) { { boolean bRet = true; if (true
             * == deserializer.hasGroupProperty("FoodImage")) { if(bRet == true)
             * { foodImage = new gen.xsd.FoodImage(); bRet =
             * foodImage.deSerialize(deserializer); } if (false == bRet)
             * deserializer.setErrorMessage("Failed To deserialize : " +
             * " FoodImage"); } if (retValue == true) retValue=bRet; }
             * 
             * } else { if (true == deserializer.hasProperty("FoodImage")) {
             * retValue = false; deserializer.setErrorMessage(
             * "Additional data to deserialize: FoodImage"); } }
             */
            /*
             * if(deserializer.isRecursive()) { { boolean bRet = true; if (true
             * == deserializer.hasGroupProperty("Sensors")) { if(bRet == true) {
             * sensors = new gen.xsd.Sensors(); bRet =
             * sensors.deSerialize(deserializer); } if (false == bRet)
             * deserializer.setErrorMessage("Failed To deserialize : " +
             * " Sensors"); } if (retValue == true) retValue=bRet; }
             * 
             * } else { if (true == deserializer.hasProperty("Sensors")) {
             * retValue = false; deserializer.setErrorMessage(
             * "Additional data to deserialize: Sensors"); } }
             */
            /*
             * if(deserializer.isRecursive()) { { boolean bRet = true; if (true
             * == deserializer.hasGroupProperty("Sensor")) { if(bRet == true) {
             * sensor = new gen.xsd.Sensor(); bRet =
             * sensor.deSerialize(deserializer); } if (false == bRet)
             * deserializer.setErrorMessage("Failed To deserialize : " +
             * " Sensor"); } if (retValue == true) retValue=bRet; }
             * 
             * } else { if (true == deserializer.hasProperty("Sensor")) {
             * retValue = false;
             * deserializer.setErrorMessage("Additional data to deserialize: Sensor"
             * ); } }
             */
            /*
             * if(deserializer.isRecursive()) { { boolean bRet = true; if (true
             * == deserializer.hasGroupProperty("Consumables")) { if(bRet ==
             * true) { consumables = new gen.xsd.Consumables(); bRet =
             * consumables.deSerialize(deserializer); } if (false == bRet)
             * deserializer.setErrorMessage("Failed To deserialize : " +
             * " Consumables"); } if (retValue == true) retValue=bRet; }
             * 
             * } else { if (true == deserializer.hasProperty("Consumables")) {
             * retValue = false; deserializer.setErrorMessage(
             * "Additional data to deserialize: Consumables"); } }
             */
            /*
             * if(deserializer.isRecursive()) { { boolean bRet = true; if (true
             * == deserializer.hasGroupProperty("Consumable")) { if(bRet ==
             * true) { consumable = new gen.xsd.Consumable(); bRet =
             * consumable.deSerialize(deserializer); } if (false == bRet)
             * deserializer.setErrorMessage("Failed To deserialize : " +
             * " Consumable"); } if (retValue == true) retValue=bRet; }
             * 
             * } else { if (true == deserializer.hasProperty("Consumable")) {
             * retValue = false; deserializer.setErrorMessage(
             * "Additional data to deserialize: Consumable"); } }
             */
        } catch (Exception e) {
            return false;
        }
        return retValue;

    }
}
