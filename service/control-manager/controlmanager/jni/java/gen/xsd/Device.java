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
 * This resource provides an interface to manage installed device in Smart Home.<br>
 * Also, this class defines methods that serialize and/or deserialize contents
 * of XSD complex/list type Device.
 * 
 * @since 1.0.0
 */
public class Device extends gen.xsd.IdentifiedObject {
    /**
     * [Mandatory] A universally unique identifier (UUID) is an unique
     * identifier.
     */
    public OC.Cm.Serialization.Xsd.UuidType Uuid;

    /**
     * [Optional] This attribute represents the name of the device.
     */
    public gen.xsd.String30                 Name;

    /**
     * [Mandatory] Indicates type of the device. Note that declaration of the
     * device enumeration type can be found in the "Attribute Type" declaration.
     * (e.g., â€œWasherâ€�)
     */
    public gen.xsd.DeviceType               Type;

    /**
     * [Mandatory] Names of all resources directly connected with the
     * â€œDeviceâ€� resource as a parent of a resource. Available in both the
     * singular and plural forms expression. In case of plural forms, each
     * option can be expressed in array of string. Note that declaration of the
     * available resource enumeration type can be found in the section 3.2.3.
     * (e.g., â€œInformationâ€�, â€œAlarmsâ€�, â€œCameraâ€�, â€œDetectionâ€�)
     */
    public gen.xsd.ArrayOfResourceType      Resources;

    /**
     * [Optional] Indicate whether the device is connected or not. (e.g.,
     * â€œTrueâ€�, â€œFalseâ€�)
     */
    public Boolean                          connected;

    /**
     * [Optional] Variable of Wind object.
     */
    // public gen.xsd.Wind wind;

    /**
     * [Optional] Variable of Mode object.
     */
    public gen.xsd.Mode                     mode;

    /**
     * [Optional] Variable of InformationLink object.
     */
    public gen.xsd.InformationLink          informationLink;

    /**
     * [Optional] Variable of MessagesLink object.
     */
    public gen.xsd.MessagesLink             messagesLink;

    /**
     * [Optional] Variable of Movement object.
     */
    public gen.xsd.Movement                 movement;

    /**
     * [Optional] Variable of Light object.
     */
    public gen.xsd.Light                    light;

    /**
     * [Optional] Variable of Operation object.
     */
    public gen.xsd.Operation                operation;

    /**
     * [Optional] Variable of Humidity object.
     */
    public gen.xsd.Humidity                 humidity;

    /**
     * [Optional] Variable of Temperatures object.
     */
    public gen.xsd.Temperatures             temperatures;

    /**
     * [Optional] Variable of ConfigurationLink object.
     */
    public gen.xsd.ConfigurationLink        configurationLink;

    /**
     * [Optional] Variable of Audio object.
     */
    // public gen.xsd.Audio audio;

    /**
     * [Optional] Variable of AVSourcesLink object.
     */
    // public gen.xsd.AVSourcesLink aVSourcesLink;

    /**
     * [Optional] Variable of Camera object.
     */
    // public gen.xsd.Camera camera;

    /**
     * [Optional] Variable of Detection object.
     */
    public gen.xsd.Detection                detection;

    /**
     * [Optional] Variable of Level object.
     */
    public gen.xsd.Level                    level;

    /**
     * [Optional] Variable of Alarms object.
     */
    public gen.xsd.Alarms                   alarms;

    /**
     * [Optional] Variable of EnergyConsumption object.
     */
    public gen.xsd.EnergyConsumption        energyConsumption;

    /**
     * [Optional] Variable of Fridge object.
     */
    // public gen.xsd.Fridge fridge;

    /**
     * [Optional] Variable of DRLC object.
     */
    public gen.xsd.DRLC                     dRLC;

    /**
     * [Optional] Variable of Thermostat object.
     */
    public gen.xsd.Thermostat               thermostat;

    /**
     * [Optional] Variable of Doors object.
     */
    public gen.xsd.Doors                    doors;

    /**
     * [Optional] Variable of Consumables object.
     */
    // public gen.xsd.Consumables consumables;

    /**
     * [Optional] Variable of Sensors object.
     */
    // public gen.xsd.Sensors sensors;

    /**
     * Default constructor of Device class.
     * 
     * @since 1.0.0
     */
    public Device() {
    }

    /**
     * Destroy method for Device class.
     * 
     * @since 1.0.0
     */
    @Override
    public void destroy() {
        super.destroy();
        if (null != Resources) {
            Resources.destroy();
        }
        // if (null != wind) { wind.destroy(); }
        if (null != mode) {
            mode.destroy();
        }
        // if (null != washer) { washer.destroy(); }
        if (null != informationLink) {
            informationLink.destroy();
        }
        if (null != messagesLink) {
            messagesLink.destroy();
        }
        if (null != movement) {
            movement.destroy();
        }
        if (null != light) {
            light.destroy();
        }
        if (null != operation) {
            operation.destroy();
        }
        if (null != humidity) {
            humidity.destroy();
        }
        // if (null != oven) { oven.destroy(); }
        if (null != temperatures) {
            temperatures.destroy();
        }
        if (null != configurationLink) {
            configurationLink.destroy();
        }
        // if (null != audio) { audio.destroy(); }
        // if (null != aVSourcesLink) { aVSourcesLink.destroy(); }
        // if (null != camera) { camera.destroy(); }
        if (null != detection) {
            detection.destroy();
        }
        if (null != level) {
            level.destroy();
        }
        if (null != alarms) {
            alarms.destroy();
        }
        if (null != energyConsumption) {
            energyConsumption.destroy();
        }
        // if (null != fridge) { fridge.destroy(); }
        if (null != dRLC) {
            dRLC.destroy();
        }
        if (null != thermostat) {
            thermostat.destroy();
        }
        if (null != doors) {
            doors.destroy();
        }
        // if (null != consumables) { consumables.destroy(); }
        // if (null != sensors) { sensors.destroy(); }
    }

    /**
     * Returns name of the serializable.
     * 
     * @since 1.0.0
     */
    @Override
    public String getElementName() {
        return Elements.EN_DEVICE;
    }

    /**
     * Serializes the contents of Device object.
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
                if (null != Resources) {
                    Resources.setInstanceName("resources");
                    bRet = Resources.serializeContents(serializer);
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " resources");
                } else {
                    if (serializer.isInFilteringMode("resources") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " resources");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != connected) {
                    bRet = serializer.setBoolPropertyValue("connected",
                            connected);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " connected");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
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
            /*
             * if(serializer.isRecursive()) { { boolean bRet = true; if (null !=
             * washer) { bRet = washer.serialize(serializer); if (false == bRet)
             * { serializer.setErrorMessage(
             * "Failed To Serialize parameter (Invalid content) : " +
             * " Washer"); } } if (retValue == true) retValue = bRet; }
             * 
             * }
             */
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != informationLink) {
                    bRet = informationLink.serialize(serializer);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " InformationLink");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != messagesLink) {
                    bRet = messagesLink.serialize(serializer);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " MessagesLink");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
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
             * oven) { bRet = oven.serialize(serializer); if (false == bRet) {
             * serializer
             * .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
             * + " Oven"); } } if (retValue == true) retValue = bRet; }
             * 
             * }
             */
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
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != configurationLink) {
                    bRet = configurationLink.serialize(serializer);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " ConfigurationLink");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
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
             * if(serializer.validateMandatory() == true) { boolean bRet = true;
             * if (null != aVSourcesLink) { bRet =
             * aVSourcesLink.serialize(serializer); if (false == bRet) {
             * serializer
             * .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
             * + " AVSourcesLink"); } } if (retValue == true) retValue = bRet; }
             */
            /*
             * if(serializer.isRecursive()) { { boolean bRet = true; if (null !=
             * camera) { bRet = camera.serialize(serializer); if (false == bRet)
             * { serializer.setErrorMessage(
             * "Failed To Serialize parameter (Invalid content) : " +
             * " Camera"); } } if (retValue == true) retValue = bRet; }
             * 
             * }
             */
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
            /*
             * if(serializer.isRecursive()) { { boolean bRet = true; if (null !=
             * fridge) { bRet = fridge.serialize(serializer); if (false == bRet)
             * { serializer.setErrorMessage(
             * "Failed To Serialize parameter (Invalid content) : " +
             * " Fridge"); } } if (retValue == true) retValue = bRet; }
             * 
             * }
             */
            if (serializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (null != dRLC) {
                        bRet = dRLC.serialize(serializer);
                        if (false == bRet) {
                            serializer
                                    .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                            + " DRLC");
                        }
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            }
            if (serializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (null != thermostat) {
                        bRet = thermostat.serialize(serializer);
                        if (false == bRet) {
                            serializer
                                    .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                            + " Thermostat");
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
             * sensors) { bRet = sensors.serialize(serializer); if (false ==
             * bRet) { serializer.setErrorMessage(
             * "Failed To Serialize parameter (Invalid content) : " +
             * " Sensors"); } } if (retValue == true) retValue = bRet; }
             * 
             * }
             */
        } catch (Exception e) {
            return false;
        }
        return retValue;

    }

    /**
     * Deserializes data and prepares the Device object contents.
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
                if (true == deserializer.hasProperty("name")) {
                    if (bRet == true) {
                        Name = new gen.xsd.String30();
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
                if (true == deserializer.hasProperty("type")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        Type = new gen.xsd.DeviceType();
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
                if (true == deserializer.hasProperty("resources")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        Resources = new gen.xsd.ArrayOfResourceType();
                        Resources.setInstanceName("resources");
                        if (false == Resources
                                .deSerializeContents(deserializer)) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " resources");
                            bRet = false;
                        } else
                            bRet = true;
                    }
                } else {
                    if (deserializer.isInFilteringMode("resources") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " resources");
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
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        connected = deserializer
                                .getBoolPropertyValue("connected");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
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
            {
                boolean bRet = true;
                if (true == deserializer.hasGroupProperty("InformationLink")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        informationLink = new gen.xsd.InformationLink();
                        bRet = informationLink.deSerialize(deserializer);
                    }
                    if (false == bRet)
                        deserializer.setErrorMessage("Failed To deserialize : "
                                + " InformationLink");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasGroupProperty("MessagesLink")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        messagesLink = new gen.xsd.MessagesLink();
                        bRet = messagesLink.deSerialize(deserializer);
                    }
                    if (false == bRet)
                        deserializer.setErrorMessage("Failed To deserialize : "
                                + " MessagesLink");
                }
                if (retValue == true)
                    retValue = bRet;
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
            {
                boolean bRet = true;
                if (true == deserializer.hasGroupProperty("ConfigurationLink")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        configurationLink = new gen.xsd.ConfigurationLink();
                        bRet = configurationLink.deSerialize(deserializer);
                    }
                    if (false == bRet)
                        deserializer.setErrorMessage("Failed To deserialize : "
                                + " ConfigurationLink");
                }
                if (retValue == true)
                    retValue = bRet;
            }
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
             * { boolean bRet = true; if (true ==
             * deserializer.hasGroupProperty("AVSourcesLink")) {
             * if(deserializer.validateMandatory() == false) bRet = false;
             * if(bRet == true) { aVSourcesLink = new gen.xsd.AVSourcesLink();
             * bRet = aVSourcesLink.deSerialize(deserializer); } if (false ==
             * bRet) deserializer.setErrorMessage("Failed To deserialize : " +
             * " AVSourcesLink"); } if (retValue == true) retValue=bRet; }
             */
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
            if (deserializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (true == deserializer.hasGroupProperty("DRLC")) {
                        if (bRet == true) {
                            dRLC = new gen.xsd.DRLC();
                            bRet = dRLC.deSerialize(deserializer);
                        }
                        if (false == bRet)
                            deserializer
                                    .setErrorMessage("Failed To deserialize : "
                                            + " DRLC");
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            } else {
                if (true == deserializer.hasProperty("DRLC")) {
                    retValue = false;
                    deserializer
                            .setErrorMessage("Additional data to deserialize: DRLC");
                }
            }
            if (deserializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (true == deserializer.hasGroupProperty("Thermostat")) {
                        if (bRet == true) {
                            thermostat = new gen.xsd.Thermostat();
                            bRet = thermostat.deSerialize(deserializer);
                        }
                        if (false == bRet)
                            deserializer
                                    .setErrorMessage("Failed To deserialize : "
                                            + " Thermostat");
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            } else {
                if (true == deserializer.hasProperty("Thermostat")) {
                    retValue = false;
                    deserializer
                            .setErrorMessage("Additional data to deserialize: Thermostat");
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
        } catch (Exception e) {
            return false;
        }
        return retValue;

    }
}
