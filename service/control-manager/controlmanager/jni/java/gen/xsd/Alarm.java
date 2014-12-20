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
 * This function set provides an interface for an alarm service. If device need
 * to inform the user such as warning messages when exceeding or dropping below
 * the specific thresholds, this feature can be useful.<br>
 * Also, this class defines methods that serialize and/or deserialize contents
 * of XSD complex/list type Alarm.
 * 
 * @since 1.0.0
 */
public class Alarm extends gen.xsd.IdentifiedObject {
    /**
     * [Mandatory] Indicate the alarm triggered time, in the format defined by
     * DateTimeType. (e.g., â€œ2013-07-15T18:44:31â€�)
     */
    public OC.Cm.Serialization.Xsd.DateTimeType TriggeredTime;

    /**
     * [Mandatory] Represents the reason of alarm activation. If there is no
     * alarm history, attribute value will be left â€œUnknownâ€� as its value.
     * Note that declaration of the alarm code enumeration type can be found in
     * the section 2.2.3. (e.g., â€œUnknownâ€�,
     * â€œFreezer_Abnormal_Temperatureâ€� ,
     * â€œRefrigerator_Abnormal_Temperatureâ€�)
     */
    public gen.xsd.AlarmCodeType                AlarmType;

    /**
     * [Optional] Represents the error code. (e.g., â€œ0x4Eâ€�, â€œ0x5Eâ€�)
     * Since these codes are not standardized in this specification, it is
     * RECOMMENDED that alarm codes be described in the user manual (or a
     * development manual)
     */
    public gen.xsd.String20                     code;

    /**
     * [Optional] Indicate whether the alarm message is transported by
     * multicasting or subscription/notification method. (e.g., â€œtrueâ€�,
     * â€œfalseâ€�)
     */
    public Boolean                              multicastEnabled;

    /**
     * Default constructor of Alarm class.
     * 
     * @since 1.0.0
     */
    public Alarm() {
    }

    /**
     * Destroy method for Alarm class.
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
        return Elements.EN_ALARM;
    }

    /**
     * Serializes the contents of Alarm object.
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
                if (null != TriggeredTime) {
                    if (TriggeredTime.value.length() > 0) {
                        if (true == TriggeredTime.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "triggeredTime", TriggeredTime.value);
                        } else
                            bRet = false;
                    } else
                        bRet = false;
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " triggeredTime");
                } else {
                    if (serializer.isInFilteringMode("triggeredTime") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " triggeredTime");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != AlarmType) {
                    if (AlarmType.value.length() > 0) {
                        if (true == AlarmType.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "alarmType", AlarmType.value);
                        } else
                            bRet = false;
                    } else
                        bRet = false;
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " alarmType");
                } else {
                    if (serializer.isInFilteringMode("alarmType") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " alarmType");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != code) {
                    if (code.value.length() > 0) {
                        if (true == code.validateContent()) {
                            bRet = serializer.setStringPropertyValue("code",
                                    code.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " code");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != multicastEnabled) {
                    bRet = serializer.setBoolPropertyValue("multicastEnabled",
                            multicastEnabled);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " multicastEnabled");
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
     * Deserializes data and prepares the Alarm object contents.
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
                if (true == deserializer.hasProperty("triggeredTime")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        TriggeredTime = new OC.Cm.Serialization.Xsd.DateTimeType();
                        TriggeredTime.value = deserializer
                                .getStringPropertyValue("triggeredTime");
                        if (false == TriggeredTime.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " triggeredTime");
                            bRet = false;
                        } else
                            bRet = true;
                    }
                } else {
                    if (deserializer.isInFilteringMode("triggeredTime") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " triggeredTime");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("alarmType")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        AlarmType = new gen.xsd.AlarmCodeType();
                        AlarmType.value = deserializer
                                .getStringPropertyValue("alarmType");
                        if (false == AlarmType.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " alarmType");
                            bRet = false;
                        } else
                            bRet = true;
                    }
                } else {
                    if (deserializer.isInFilteringMode("alarmType") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " alarmType");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("code")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        code = new gen.xsd.String20();
                        code.value = deserializer
                                .getStringPropertyValue("code");
                        if (false == code.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " code");
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
                if (true == deserializer.hasProperty("multicastEnabled")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        multicastEnabled = deserializer
                                .getBoolPropertyValue("multicastEnabled");
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
