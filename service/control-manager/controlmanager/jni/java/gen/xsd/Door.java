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
 * This resource has a set of items associated with the door.<br>
 * Also, this class defines methods that serialize and/or deserialize contents
 * of XSD complex/list type Door.
 * 
 * @since 1.0.0
 */
public class Door extends gen.xsd.IdentifiedObject {
    /**
     * [Optional] This attribute represents the name of the door resource. Each
     * resource must have a name field value for the purpose of distinction.
     * (e.g., â€œFreezerâ€�, â€œFridgeâ€�)
     */
    public gen.xsd.String10                     Name;

    /**
     * [Optional] Indicate whether the door is opened or closed. Note that
     * declaration of the open enumeration type can be found in the
     * "Attribute Type" declaration. (e.g., â€œOpenâ€�, â€œCloseâ€�)
     */
    public gen.xsd.OpenType                     OpenState;

    /**
     * [Optional] Total elapsed time since the door is opened.
     */
    public OC.Cm.Serialization.Xsd.TimeType     OpenElapsedTime;

    /**
     * [Optional] Indicate when the door is opened, in the format defined by
     * DateTimeType.
     */
    public OC.Cm.Serialization.Xsd.DateTimeType OpenTime;

    /**
     * [Optional] Indicate current door lock status. If client want to lock the
     * door, before send the PUT request fill this attribute with â€œLockâ€�.
     * Note that declaration of the lock enumeration type can be found in the
     * "Attribute Type" declaration. (e.g., â€œLockâ€�, â€œUnlockâ€�)
     */
    public gen.xsd.LockType                     Lock;

    /**
     * [Optional] Indicate the password of the door lock.
     */
    public gen.xsd.String20                     Password;

    /**
     * [Optional] Indicate the one time password (OTP) of the door lock.
     */
    public gen.xsd.String20                     OneTimePassword;

    /**
     * [Optional] Indicate the security mode of the door lock.
     */
    public gen.xsd.DoorLockModeType             Mode;

    /**
     * [Optional] Indicate whether the emergency situation occurs. (e.g.,
     * â€œtrueâ€�, â€œfalseâ€�)
     */
    public Boolean                              emergencyAlarm;

    /**
     * Default constructor of Door class.
     * 
     * @since 1.0.0
     */
    public Door() {
    }

    /**
     * Destroy method for Door class.
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
        return Elements.EN_DOOR;
    }

    /**
     * Serializes the contents of Door object.
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
                if (null != OpenState) {
                    if (OpenState.value.length() > 0) {
                        if (true == OpenState.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "openState", OpenState.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " openState");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != OpenElapsedTime) {
                    if (OpenElapsedTime.value.length() > 0) {
                        if (true == OpenElapsedTime.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "openElapsedTime", OpenElapsedTime.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " openElapsedTime");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != OpenTime) {
                    if (OpenTime.value.length() > 0) {
                        if (true == OpenTime.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "openTime", OpenTime.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " openTime");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (null != Lock) {
                    if (Lock.value.length() > 0) {
                        if (true == Lock.validateContent()) {
                            bRet = serializer.setStringPropertyValue("lock",
                                    Lock.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " lock");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (null != Password) {
                    if (Password.value.length() > 0) {
                        if (true == Password.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "password", Password.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " password");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (null != OneTimePassword) {
                    if (OneTimePassword.value.length() > 0) {
                        if (true == OneTimePassword.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "oneTimePassword", OneTimePassword.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " oneTimePassword");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (null != Mode) {
                    if (Mode.value.length() > 0) {
                        if (true == Mode.validateContent()) {
                            bRet = serializer.setStringPropertyValue("mode",
                                    Mode.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " mode");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (null != emergencyAlarm) {
                    bRet = serializer.setBoolPropertyValue("emergencyAlarm",
                            emergencyAlarm);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " emergencyAlarm");
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
     * Deserializes data and prepares the Door object contents.
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
                if (true == deserializer.hasProperty("name")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        Name = new gen.xsd.String10();
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
                if (true == deserializer.hasProperty("openState")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        OpenState = new gen.xsd.OpenType();
                        OpenState.value = deserializer
                                .getStringPropertyValue("openState");
                        if (false == OpenState.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " openState");
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
                if (true == deserializer.hasProperty("openElapsedTime")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        OpenElapsedTime = new OC.Cm.Serialization.Xsd.TimeType();
                        OpenElapsedTime.value = deserializer
                                .getStringPropertyValue("openElapsedTime");
                        if (false == OpenElapsedTime.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " openElapsedTime");
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
                if (true == deserializer.hasProperty("openTime")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        OpenTime = new OC.Cm.Serialization.Xsd.DateTimeType();
                        OpenTime.value = deserializer
                                .getStringPropertyValue("openTime");
                        if (false == OpenTime.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " openTime");
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
                if (true == deserializer.hasProperty("lock")) {
                    if (bRet == true) {
                        Lock = new gen.xsd.LockType();
                        Lock.value = deserializer
                                .getStringPropertyValue("lock");
                        if (false == Lock.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " lock");
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
                if (true == deserializer.hasProperty("password")) {
                    if (bRet == true) {
                        Password = new gen.xsd.String20();
                        Password.value = deserializer
                                .getStringPropertyValue("password");
                        if (false == Password.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " password");
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
                if (true == deserializer.hasProperty("oneTimePassword")) {
                    if (bRet == true) {
                        OneTimePassword = new gen.xsd.String20();
                        OneTimePassword.value = deserializer
                                .getStringPropertyValue("oneTimePassword");
                        if (false == OneTimePassword.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " oneTimePassword");
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
                if (true == deserializer.hasProperty("mode")) {
                    if (bRet == true) {
                        Mode = new gen.xsd.DoorLockModeType();
                        Mode.value = deserializer
                                .getStringPropertyValue("mode");
                        if (false == Mode.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " mode");
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
                if (true == deserializer.hasProperty("emergencyAlarm")) {
                    if (bRet == true) {
                        emergencyAlarm = deserializer
                                .getBoolPropertyValue("emergencyAlarm");
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
