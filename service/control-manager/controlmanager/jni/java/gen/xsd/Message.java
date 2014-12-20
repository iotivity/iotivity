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
 * This function set provides an interface for a text messaging service. Each
 * messaging instance will contain information that a device can use to display
 * the message appropriately.<br>
 * Also, this class defines methods that serialize and/or deserialize contents
 * of XSD complex/list type Message.
 * 
 * @since 1.0.0
 */
public class Message extends gen.xsd.IdentifiedObject {
    /**
     * [Mandatory] The date/time when the message is to take expose.
     */
    public OC.Cm.Serialization.Xsd.DateTimeType StartTime;

    /**
     * [Optional] Duration of time message is to be exposed.
     */
    public OC.Cm.Serialization.Xsd.TimeType     Duration;

    /**
     * [Optional] The priority is used to inform the client of the priority of
     * the particular message and the higher value has a higher priority. Note
     * that there is no limit to the number of relative priority levels, but for
     * practicality, message sender should strive for simplicity and hesitate to
     * use the priority value greater than three. (e.g., â€œ0â€�, â€œ1â€�,
     * â€œ2â€�, â€œ3â€�)
     */
    public Integer                              priority;

    /**
     * [Optional] Indicate the messageâ€™s writer.
     */
    public gen.xsd.String20                     Writer;

    /**
     * [Optional] Indicate the message title.
     */
    public gen.xsd.String20                     Title;

    /**
     * [Mandatory] Indicate the type of message (e.g., â€œImageâ€�, â€œTextâ€�)
     */
    public gen.xsd.MessageType                  MessageType;

    /**
     * [Mandatory] This attribute contains the actual UTF-8 encoded text to be
     * exposed. (e.g., â€œThis is a test messageâ€�)
     */
    public gen.xsd.String500                    Text;

    /**
     * [Optional] Variable of Image object.
     */
    public gen.xsd.Image                        image;

    /**
     * Default constructor of Message class.
     * 
     * @since 1.0.0
     */
    public Message() {
    }

    /**
     * Destroy method for Message class.
     * 
     * @since 1.0.0
     */
    @Override
    public void destroy() {
        super.destroy();
        if (null != image) {
            image.destroy();
        }
    }

    /**
     * Returns name of the serializable.
     * 
     * @since 1.0.0
     */
    @Override
    public String getElementName() {
        return Elements.EN_MESSAGE;
    }

    /**
     * Serializes the contents of Message object.
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
                if (null != StartTime) {
                    if (StartTime.value.length() > 0) {
                        if (true == StartTime.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "startTime", StartTime.value);
                        } else
                            bRet = false;
                    } else
                        bRet = false;
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " startTime");
                } else {
                    if (serializer.isInFilteringMode("startTime") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " startTime");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (null != Duration) {
                    if (Duration.value.length() > 0) {
                        if (true == Duration.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "duration", Duration.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " duration");
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
                if (null != Writer) {
                    if (Writer.value.length() > 0) {
                        if (true == Writer.validateContent()) {
                            bRet = serializer.setStringPropertyValue("writer",
                                    Writer.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " writer");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (null != Title) {
                    if (Title.value.length() > 0) {
                        if (true == Title.validateContent()) {
                            bRet = serializer.setStringPropertyValue("title",
                                    Title.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " title");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (null != MessageType) {
                    if (MessageType.value.length() > 0) {
                        if (true == MessageType.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "messageType", MessageType.value);
                        } else
                            bRet = false;
                    } else
                        bRet = false;
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " messageType");
                } else {
                    if (serializer.isInFilteringMode("messageType") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " messageType");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (null != Text) {
                    if (Text.value.length() > 0) {
                        if (true == Text.validateContent()) {
                            bRet = serializer.setStringPropertyValue("text",
                                    Text.value);
                        } else
                            bRet = false;
                    } else
                        bRet = false;
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " text");
                } else {
                    if (serializer.isInFilteringMode("text") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " text");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (null != image) {
                    bRet = image.serialize(serializer);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " Image");
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
     * Deserializes data and prepares the Message object contents.
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
                if (true == deserializer.hasProperty("startTime")) {
                    if (bRet == true) {
                        StartTime = new OC.Cm.Serialization.Xsd.DateTimeType();
                        StartTime.value = deserializer
                                .getStringPropertyValue("startTime");
                        if (false == StartTime.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " startTime");
                            bRet = false;
                        } else
                            bRet = true;
                    }
                } else {
                    if (deserializer.isInFilteringMode("startTime") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " startTime");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("duration")) {
                    if (bRet == true) {
                        Duration = new OC.Cm.Serialization.Xsd.TimeType();
                        Duration.value = deserializer
                                .getStringPropertyValue("duration");
                        if (false == Duration.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " duration");
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
                if (true == deserializer.hasProperty("writer")) {
                    if (bRet == true) {
                        Writer = new gen.xsd.String20();
                        Writer.value = deserializer
                                .getStringPropertyValue("writer");
                        if (false == Writer.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " writer");
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
                if (true == deserializer.hasProperty("title")) {
                    if (bRet == true) {
                        Title = new gen.xsd.String20();
                        Title.value = deserializer
                                .getStringPropertyValue("title");
                        if (false == Title.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " title");
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
                if (true == deserializer.hasProperty("messageType")) {
                    if (bRet == true) {
                        MessageType = new gen.xsd.MessageType();
                        MessageType.value = deserializer
                                .getStringPropertyValue("messageType");
                        if (false == MessageType.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " messageType");
                            bRet = false;
                        } else
                            bRet = true;
                    }
                } else {
                    if (deserializer.isInFilteringMode("messageType") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " messageType");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("text")) {
                    if (bRet == true) {
                        Text = new gen.xsd.String500();
                        Text.value = deserializer
                                .getStringPropertyValue("text");
                        if (false == Text.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " text");
                            bRet = false;
                        } else
                            bRet = true;
                    }
                } else {
                    if (deserializer.isInFilteringMode("text") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " text");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasGroupProperty("Image")) {
                    if (bRet == true) {
                        image = new gen.xsd.Image();
                        bRet = image.deSerialize(deserializer);
                    }
                    if (false == bRet)
                        deserializer.setErrorMessage("Failed To deserialize : "
                                + " Image");
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
