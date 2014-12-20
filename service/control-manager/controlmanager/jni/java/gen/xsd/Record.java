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
 * complex/list type Record.
 * 
 * @since 1.0.0
 */
public class Record extends gen.xsd.IdentifiedObject {
    /**
     * [Optional] Indicate the list of device UUID which the action is recorded.
     */
    public gen.xsd.ArrayOfUuid                  DeviceIDs;

    /**
     * [Optional] Indicate the list of groups which the action is recorded.
     */
    public gen.xsd.ArrayOfString                GroupIDs;

    /**
     * [Mandatory] Indicate time when event occurs, in the format defined by
     * DateTimeType.
     */
    public OC.Cm.Serialization.Xsd.DateTimeType EventTime;

    /**
     * [Mandatory] Indicate the event description. It defines the behavior of
     * the device during the certain period of the time. The regular expression
     * in this field is
     * "ResourceName:AttributeName^AttributeValue-AttributeName^AttributeValue|ResourceName:AttributeName^AttributeValueâ€�.
     * (e.g., â€œTemperature:current^6-set^4|Fridge:rapidFreezing^trueâ€�)
     */
    public gen.xsd.String200                    EventDescription;

    /**
     * Default constructor of Record class.
     * 
     * @since 1.0.0
     */
    public Record() {
    }

    /**
     * Destroy method for Record class.
     * 
     * @since 1.0.0
     */
    @Override
    public void destroy() {
        super.destroy();
        if (null != DeviceIDs) {
            DeviceIDs.destroy();
        }
        if (null != GroupIDs) {
            GroupIDs.destroy();
        }
    }

    /**
     * Returns name of the serializable.
     * 
     * @since 1.0.0
     */
    @Override
    public String getElementName() {
        return Elements.EN_RECORD;
    }

    /**
     * Serializes the contents of Record object.
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
            if (serializer.validateMandatory() == true) {
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

            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != EventDescription) {
                    if (EventDescription.value.length() > 0) {
                        if (true == EventDescription.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "eventDescription", EventDescription.value);
                        } else
                            bRet = false;
                    } else
                        bRet = false;
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " eventDescription");
                } else {
                    if (serializer.isInFilteringMode("eventDescription") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " eventDescription");
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
     * Deserializes data and prepares the Record object contents.
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
                if (true == deserializer.hasProperty("deviceIDs")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
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
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
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

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("eventDescription")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        EventDescription = new gen.xsd.String200();
                        EventDescription.value = deserializer
                                .getStringPropertyValue("eventDescription");
                        if (false == EventDescription.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " eventDescription");
                            bRet = false;
                        } else
                            bRet = true;
                    }
                } else {
                    if (deserializer.isInFilteringMode("eventDescription") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " eventDescription");
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
