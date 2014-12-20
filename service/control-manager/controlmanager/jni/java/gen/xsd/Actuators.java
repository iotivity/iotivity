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
 * complex/list type Actuators.
 * 
 * @since 1.0.0
 */
public class Actuators extends OC.Cm.Serialization.ISerializable {
    /**
     * [Optional] Indicate the list of device UUID which the action is
     * performed.
     */
    public gen.xsd.ArrayOfUuid   DeviceIDs;

    /**
     * [Optional] Indicate the list of groups which the action is performed.
     */
    public gen.xsd.ArrayOfString GroupIDs;

    /**
     * [Optional] Variable of Operation object.
     */
    public gen.xsd.Operation     operation;

    /**
     * [Optional] Variable of Mode object.
     */
    public gen.xsd.Mode          mode;

    /**
     * [Optional] Variable of Movement object.
     */
    public gen.xsd.Movement      movement;

    /**
     * [Optional] Variable of Temperatures object.
     */
    public gen.xsd.Temperatures  temperatures;

    /**
     * [Optional] Variable of Temperature object.
     */
    public gen.xsd.Temperature   temperature;

    /**
     * [Optional] Variable of Humidity object.
     */
    public gen.xsd.Humidity      humidity;

    /**
     * [Optional] Variable of Doors object.
     */
    public gen.xsd.Doors         doors;

    /**
     * [Optional] Variable of Door object.
     */
    public gen.xsd.Door          door;

    /**
     * [Optional] Variable of Light object.
     */
    public gen.xsd.Light         light;

    /**
     * [Optional] Variable of Level object.
     */
    public gen.xsd.Level         level;

    /**
     * [Optional] Variable of Messages object.
     */
    public gen.xsd.Messages      messages;

    /**
     * [Optional] Variable of Message object.
     */
    public gen.xsd.Message       message;

    /**
     * [Optional] Variable of Detection object.
     */
    public gen.xsd.Detection     detection;

    /**
     * Default constructor of Actuators class.
     * 
     * @since 1.0.0
     */
    public Actuators() {
    }

    /**
     * Destroy method for Actuators class.
     * 
     * @since 1.0.0
     */
    @Override
    public void destroy() {

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
        if (null != humidity) {
            humidity.destroy();
        }
        if (null != doors) {
            doors.destroy();
        }
        if (null != door) {
            door.destroy();
        }
        if (null != light) {
            light.destroy();
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

    }

    /**
     * Returns name of the serializable.
     * 
     * @since 1.0.0
     */
    @Override
    public String getElementName() {
        return Elements.EN_ACTUATORS;
    }

    /**
     * Serializes the contents of Actuators object.
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
            {
                boolean bRet = true;
                if (null != DeviceIDs) {
                    DeviceIDs.setInstanceName("deviceIDs");
                    bRet = DeviceIDs.serializeContents(serializer);
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " deviceIDs");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (null != GroupIDs) {
                    GroupIDs.setInstanceName("groupIDs");
                    bRet = GroupIDs.serializeContents(serializer);
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " groupIDs");
                }
                if (retValue == true)
                    retValue = bRet;
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
            /*
             * if(serializer.isRecursive()) { { boolean bRet = true; if (null !=
             * camera) { bRet = camera.serialize(serializer); if (false == bRet)
             * { serializer.setErrorMessage(
             * "Failed To Serialize parameter (Invalid content) : " +
             * " Camera"); } } if (retValue == true) retValue = bRet; }
             * 
             * } if(serializer.isRecursive()) { { boolean bRet = true; if (null
             * != captures) { bRet = captures.serialize(serializer); if (false
             * == bRet) { serializer.setErrorMessage(
             * "Failed To Serialize parameter (Invalid content) : " +
             * " Captures"); } } if (retValue == true) retValue = bRet; }
             * 
             * } if(serializer.isRecursive()) { { boolean bRet = true; if (null
             * != capture) { bRet = capture.serialize(serializer); if (false ==
             * bRet) { serializer.setErrorMessage(
             * "Failed To Serialize parameter (Invalid content) : " +
             * " Capture"); } } if (retValue == true) retValue = bRet; }
             * 
             * }
             */
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
             * sensors) { bRet = sensors.serialize(serializer); if (false ==
             * bRet) { serializer.setErrorMessage(
             * "Failed To Serialize parameter (Invalid content) : " +
             * " Sensors"); } } if (retValue == true) retValue = bRet; }
             * 
             * } if(serializer.isRecursive()) { { boolean bRet = true; if (null
             * != sensor) { bRet = sensor.serialize(serializer); if (false ==
             * bRet) { serializer.setErrorMessage(
             * "Failed To Serialize parameter (Invalid content) : " +
             * " Sensor"); } } if (retValue == true) retValue = bRet; }
             * 
             * } if(serializer.isRecursive()) { { boolean bRet = true; if (null
             * != consumables) { bRet = consumables.serialize(serializer); if
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
     * Deserializes data and prepares the Actuators object contents.
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
                if (true == deserializer.hasProperty("deviceIDs")) {
                    if (bRet == true) {
                        DeviceIDs = new gen.xsd.ArrayOfUuid();
                        DeviceIDs.setInstanceName("deviceIDs");
                        if (false == DeviceIDs
                                .deSerializeContents(deserializer)) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " deviceIDs");
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
                if (true == deserializer.hasProperty("groupIDs")) {
                    if (bRet == true) {
                        GroupIDs = new gen.xsd.ArrayOfString();
                        GroupIDs.setInstanceName("groupIDs");
                        if (false == GroupIDs.deSerializeContents(deserializer)) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " groupIDs");
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
             * ); } } if(deserializer.isRecursive()) { { boolean bRet = true; if
             * (true == deserializer.hasGroupProperty("Oven")) { if(bRet ==
             * true) { oven = new gen.xsd.Oven(); bRet =
             * oven.deSerialize(deserializer); } if (false == bRet)
             * deserializer.setErrorMessage("Failed To deserialize : " +
             * " Oven"); } if (retValue == true) retValue=bRet; }
             * 
             * } else { if (true == deserializer.hasProperty("Oven")) { retValue
             * = false;
             * deserializer.setErrorMessage("Additional data to deserialize: Oven"
             * ); } } if(deserializer.isRecursive()) { { boolean bRet = true; if
             * (true == deserializer.hasGroupProperty("Audio")) { if(bRet ==
             * true) { audio = new gen.xsd.Audio(); bRet =
             * audio.deSerialize(deserializer); } if (false == bRet)
             * deserializer.setErrorMessage("Failed To deserialize : " +
             * " Audio"); } if (retValue == true) retValue=bRet; }
             * 
             * } else { if (true == deserializer.hasProperty("Audio")) {
             * retValue = false;
             * deserializer.setErrorMessage("Additional data to deserialize: Audio"
             * ); } } if(deserializer.isRecursive()) { { boolean bRet = true; if
             * (true == deserializer.hasGroupProperty("AVSources")) { if(bRet ==
             * true) { aVSources = new gen.xsd.AVSources(); bRet =
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
             * ); } } if(deserializer.isRecursive()) { { boolean bRet = true; if
             * (true == deserializer.hasGroupProperty("Captures")) { if(bRet ==
             * true) { captures = new gen.xsd.Captures(); bRet =
             * captures.deSerialize(deserializer); } if (false == bRet)
             * deserializer.setErrorMessage("Failed To deserialize : " +
             * " Captures"); } if (retValue == true) retValue=bRet; }
             * 
             * } else { if (true == deserializer.hasProperty("Captures")) {
             * retValue = false; deserializer.setErrorMessage(
             * "Additional data to deserialize: Captures"); } }
             * if(deserializer.isRecursive()) { { boolean bRet = true; if (true
             * == deserializer.hasGroupProperty("Capture")) { if(bRet == true) {
             * capture = new gen.xsd.Capture(); bRet =
             * capture.deSerialize(deserializer); } if (false == bRet)
             * deserializer.setErrorMessage("Failed To deserialize : " +
             * " Capture"); } if (retValue == true) retValue=bRet; }
             * 
             * } else { if (true == deserializer.hasProperty("Capture")) {
             * retValue = false; deserializer.setErrorMessage(
             * "Additional data to deserialize: Capture"); } }
             */
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
             * == deserializer.hasGroupProperty("Sensors")) { if(bRet == true) {
             * sensors = new gen.xsd.Sensors(); bRet =
             * sensors.deSerialize(deserializer); } if (false == bRet)
             * deserializer.setErrorMessage("Failed To deserialize : " +
             * " Sensors"); } if (retValue == true) retValue=bRet; }
             * 
             * } else { if (true == deserializer.hasProperty("Sensors")) {
             * retValue = false; deserializer.setErrorMessage(
             * "Additional data to deserialize: Sensors"); } }
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
             * ); } } if(deserializer.isRecursive()) { { boolean bRet = true; if
             * (true == deserializer.hasGroupProperty("Consumables")) { if(bRet
             * == true) { consumables = new gen.xsd.Consumables(); bRet =
             * consumables.deSerialize(deserializer); } if (false == bRet)
             * deserializer.setErrorMessage("Failed To deserialize : " +
             * " Consumables"); } if (retValue == true) retValue=bRet; }
             * 
             * } else { if (true == deserializer.hasProperty("Consumables")) {
             * retValue = false; deserializer.setErrorMessage(
             * "Additional data to deserialize: Consumables"); } }
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
