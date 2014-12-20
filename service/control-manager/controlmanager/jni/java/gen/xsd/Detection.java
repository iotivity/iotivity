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
 * This function set provides an interface for a detection service. Sound and
 * motion detect threshold value can be set through this interface. In case
 * event occurs by detecting devices, notification will send to the given SNS
 * account. Detailed configuration in notification can be achieved by the
 * SNSNotification instance.<br>
 * Also, this class defines methods that serialize and/or deserialize contents
 * of XSD complex/list type Detection.
 * 
 * @since 1.0.0
 */
public class Detection extends gen.xsd.Resource {
    /**
     * [Optional] Boolean property value that specifies whether the motion
     * detection is being used. (e.g., â€œTrueâ€�, â€œFalseâ€�)
     */
    public Boolean motionDetectionInUse;

    /**
     * [Optional] Boolean property value that specifies whether the motion is
     * detected. (e.g., â€œtrueâ€�, â€œfalseâ€�)
     */
    public Boolean motionDetected;

    /**
     * [Optional] This attribute indicates the threshold of motion detection. If
     * this value is zero, it represents motion detection function is currently
     * disabled. (e.g., â€œ200â€�)
     */
    public Integer motionSensitivity;

    /**
     * [Optional] Indicate maximum number of the motion detection sensitivity.
     * (e.g., â€œ255â€�)
     */
    public Integer maxMotionSensitivity;

    /**
     * [Optional] Boolean property value that specifies whether the sound
     * detection is being used. (e.g., â€œTrueâ€�, â€œFalseâ€�)
     */
    public Boolean soundDetectionInUse;

    /**
     * [Optional] Boolean property value that specifies whether the sound is
     * detected. (e.g., â€œtrueâ€�, â€œfalseâ€�)
     */
    public Boolean soundDetected;

    /**
     * [Optional] This attribute indicates the threshold of sound detection. If
     * this value is zero, it represents sound detection function is currently
     * disabled. (e.g., â€œ129â€�)
     */
    public Integer soundSensitivity;

    /**
     * [Optional] Indicate maximum number of the sound detection sensitivity.
     * (e.g., â€œ255â€�)
     */
    public Integer maxSoundSensitivity;

    /**
     * [Optional] Boolean property value that specifies whether the barrier
     * detection function is being used. (e.g., â€œtrueâ€�, â€œfalseâ€�)
     */
    public Boolean barrierDetectionInUse;

    /**
     * [Optional] Boolean property value that specifies whether the barrier is
     * detected. (e.g., â€œtrueâ€�, â€œfalseâ€�)
     */
    public Boolean barrierDetected;

    /**
     * Default constructor of Detection class.
     * 
     * @since 1.0.0
     */
    public Detection() {
    }

    /**
     * Destroy method for Detection class.
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
        return Elements.EN_DETECTION;
    }

    /**
     * Serializes the contents of Detection object.
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
                if (null != motionDetectionInUse) {
                    bRet = serializer.setBoolPropertyValue(
                            "motionDetectionInUse", motionDetectionInUse);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " motionDetectionInUse");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != motionDetected) {
                    bRet = serializer.setBoolPropertyValue("motionDetected",
                            motionDetected);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " motionDetected");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (null != motionSensitivity) {
                    bRet = serializer.setUnsignedByte("motionSensitivity",
                            motionSensitivity);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " motionSensitivity");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != maxMotionSensitivity) {
                    bRet = serializer.setUnsignedByte("maxMotionSensitivity",
                            maxMotionSensitivity);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " maxMotionSensitivity");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (null != soundDetectionInUse) {
                    bRet = serializer.setBoolPropertyValue(
                            "soundDetectionInUse", soundDetectionInUse);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " soundDetectionInUse");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != soundDetected) {
                    bRet = serializer.setBoolPropertyValue("soundDetected",
                            soundDetected);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " soundDetected");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (null != soundSensitivity) {
                    bRet = serializer.setUnsignedByte("soundSensitivity",
                            soundSensitivity);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " soundSensitivity");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != maxSoundSensitivity) {
                    bRet = serializer.setUnsignedByte("maxSoundSensitivity",
                            maxSoundSensitivity);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " maxSoundSensitivity");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (null != barrierDetectionInUse) {
                    bRet = serializer.setBoolPropertyValue(
                            "barrierDetectionInUse", barrierDetectionInUse);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " barrierDetectionInUse");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != barrierDetected) {
                    bRet = serializer.setBoolPropertyValue("barrierDetected",
                            barrierDetected);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " barrierDetected");
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
     * Deserializes data and prepares the Detection object contents.
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
                if (true == deserializer.hasProperty("motionDetectionInUse")) {
                    if (bRet == true) {
                        motionDetectionInUse = deserializer
                                .getBoolPropertyValue("motionDetectionInUse");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("motionDetected")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        motionDetected = deserializer
                                .getBoolPropertyValue("motionDetected");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("motionSensitivity")) {
                    if (bRet == true) {
                        motionSensitivity = deserializer
                                .getUnsignedByte("motionSensitivity");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("maxMotionSensitivity")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        maxMotionSensitivity = deserializer
                                .getUnsignedByte("maxMotionSensitivity");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("soundDetectionInUse")) {
                    if (bRet == true) {
                        soundDetectionInUse = deserializer
                                .getBoolPropertyValue("soundDetectionInUse");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("soundDetected")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        soundDetected = deserializer
                                .getBoolPropertyValue("soundDetected");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("soundSensitivity")) {
                    if (bRet == true) {
                        soundSensitivity = deserializer
                                .getUnsignedByte("soundSensitivity");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("maxSoundSensitivity")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        maxSoundSensitivity = deserializer
                                .getUnsignedByte("maxSoundSensitivity");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("barrierDetectionInUse")) {
                    if (bRet == true) {
                        barrierDetectionInUse = deserializer
                                .getBoolPropertyValue("barrierDetectionInUse");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("barrierDetected")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        barrierDetected = deserializer
                                .getBoolPropertyValue("barrierDetected");
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
