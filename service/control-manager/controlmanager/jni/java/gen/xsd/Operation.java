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
 * This is a collection of common operational features of the device.<br>
 * Also, this class defines methods that serialize and/or deserialize contents
 * of XSD complex/list type Operation.
 * 
 * @since 1.0.0
 */
public class Operation extends gen.xsd.Resource {
    /**
     * [Mandatory] Indicate the power On/Off status. Note that declaration of
     * the on enumeration type can be found in the "Attribute Type" declaration.
     * (e.g., â€œOnâ€�, â€œOffâ€�)
     */
    public gen.xsd.OnType                   Power;

    /**
     * [Optional] Indicate current state of the device. Note that declaration of
     * the state enumeration type can be found in the "Attribute Type"
     * declaration. (e.g., â€œUnknownâ€� , â€œStartâ€�, â€œStopâ€�, â€œPauseâ€�)
     */
    public gen.xsd.StateType                State;

    /**
     * [Optional] Contains a set of supported progress. Available in both the
     * singular and plural forms expression. In case of plural forms, each
     * progress can be coupled with â€œ|â€� sign. (e.g., â€œWashâ€�,
     * â€œWash|Dry|Preheat|Cooling|Spin|Weight Senseâ€�)
     */
    public gen.xsd.ArrayOfString            SupportedProgress;

    /**
     * [Optional] Indicate current progress of the device. (e.g., â€œWashâ€�,
     * â€œDryâ€�, â€œPreheatâ€�, â€œCookâ€�) Washer/Dryer : Reserved, Pre Wash,
     * Wash, Rinse, Spin, Dry, Air Wash, Cooling, Wrinkle Prevent, Finish Dish
     * Washer : Wash, Dry, Clean, Finish Oven : Schedule, Weight Sense, Preheat,
     * Cook
     */
    public gen.xsd.String20                 Progress;

    /**
     * [Optional] Indicate an operation time.
     */
    public OC.Cm.Serialization.Xsd.TimeType OperationTime;

    /**
     * [Optional] Indicate the minimum operation time that can be set on the
     * device.
     */
    public OC.Cm.Serialization.Xsd.TimeType OperationTimeMin;

    /**
     * [Optional] Indicate the maximum operation time that can be set on the
     * device.
     */
    public OC.Cm.Serialization.Xsd.TimeType OperationTimeMax;

    /**
     * [Optional] Indicate a remaining amount of time.
     */
    public OC.Cm.Serialization.Xsd.TimeType RemainingTime;

    /**
     * [Optional] Indicate a running time.
     */
    public OC.Cm.Serialization.Xsd.TimeType RunningTime;

    /**
     * [Optional] Unless the specified time has elapsed, device will not be
     * started. Time unit is a minute. (e.g., â€œ50â€�)
     */
    public OC.Cm.Serialization.Xsd.TimeType DelayStartTime;

    /**
     * [Optional] Device operation will be finished in accordance with the
     * specified time. If the value is set to 300, then device operation
     * requested will finish after five hours. However, if the time value is
     * shorter than the minimum time required to complete the requested
     * operation, device will ignore the delayEndTime setting request and
     * attribute will not be updated. Time unit is a minute. (e.g., â€œ300â€�)
     */
    public OC.Cm.Serialization.Xsd.TimeType DelayEndTime;

    /**
     * [Optional] This defines the accomplished progress of the device as a
     * percentage. (e.g., â€œ45â€�)
     */
    public Integer                          progressPercentage;

    /**
     * [Optional] Indicate whether the Kids Lock function is enabled or not.
     * Note that declaration of the on enumeration type can be found in the
     * "Attribute Type" declaration. (e.g., â€œOnâ€�, â€œOffâ€�)
     */
    public gen.xsd.StateType                KidsLock;

    /**
     * [Optional] Indicate the reset operation (e.g., â€œOnâ€�, â€œOffâ€�)
     */
    public gen.xsd.OnType                   Reset;

    /**
     * Default constructor of Operation class.
     * 
     * @since 1.0.0
     */
    public Operation() {
    }

    /**
     * Destroy method for Operation class.
     * 
     * @since 1.0.0
     */
    @Override
    public void destroy() {
        super.destroy();
        if (null != SupportedProgress) {
            SupportedProgress.destroy();
        }
    }

    /**
     * Returns name of the serializable.
     * 
     * @since 1.0.0
     */
    @Override
    public String getElementName() {
        return Elements.EN_OPERATION;
    }

    /**
     * Serializes the contents of Operation object.
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
                if (null != Power) {
                    if (Power.value.length() > 0) {
                        if (true == Power.validateContent()) {
                            bRet = serializer.setStringPropertyValue("power",
                                    Power.value);
                        } else
                            bRet = false;
                    } else
                        bRet = false;
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " power");
                } else {
                    if (serializer.isInFilteringMode("power") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " power");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (null != State) {
                    if (State.value.length() > 0) {
                        if (true == State.validateContent()) {
                            bRet = serializer.setStringPropertyValue("state",
                                    State.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " state");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != SupportedProgress) {
                    SupportedProgress.setInstanceName("supportedProgress");
                    bRet = SupportedProgress.serializeContents(serializer);
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " supportedProgress");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != Progress) {
                    if (Progress.value.length() > 0) {
                        if (true == Progress.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "progress", Progress.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " progress");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (null != OperationTime) {
                    if (OperationTime.value.length() > 0) {
                        if (true == OperationTime.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "operationTime", OperationTime.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " operationTime");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != OperationTimeMin) {
                    if (OperationTimeMin.value.length() > 0) {
                        if (true == OperationTimeMin.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "operationTimeMin", OperationTimeMin.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " operationTimeMin");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != OperationTimeMax) {
                    if (OperationTimeMax.value.length() > 0) {
                        if (true == OperationTimeMax.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "operationTimeMax", OperationTimeMax.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " operationTimeMax");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != RemainingTime) {
                    if (RemainingTime.value.length() > 0) {
                        if (true == RemainingTime.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "remainingTime", RemainingTime.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " remainingTime");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != RunningTime) {
                    if (RunningTime.value.length() > 0) {
                        if (true == RunningTime.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "runningTime", RunningTime.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " runningTime");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (null != DelayStartTime) {
                    if (DelayStartTime.value.length() > 0) {
                        if (true == DelayStartTime.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "delayStartTime", DelayStartTime.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " delayStartTime");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (null != DelayEndTime) {
                    if (DelayEndTime.value.length() > 0) {
                        if (true == DelayEndTime.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "delayEndTime", DelayEndTime.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " delayEndTime");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != progressPercentage) {
                    bRet = serializer.setUnsignedByte("progressPercentage",
                            progressPercentage);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " progressPercentage");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (null != KidsLock) {
                    if (KidsLock.value.length() > 0) {
                        if (true == KidsLock.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "kidsLock", KidsLock.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " kidsLock");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (null != Reset) {
                    if (Reset.value.length() > 0) {
                        if (true == Reset.validateContent()) {
                            bRet = serializer.setStringPropertyValue("reset",
                                    Reset.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " reset");
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
     * Deserializes data and prepares the Operation object contents.
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
                if (true == deserializer.hasProperty("power")) {
                    if (bRet == true) {
                        Power = new gen.xsd.OnType();
                        Power.value = deserializer
                                .getStringPropertyValue("power");
                        if (false == Power.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " power");
                            bRet = false;
                        } else
                            bRet = true;
                    }
                } else {
                    if (deserializer.isInFilteringMode("power") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " power");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("state")) {
                    if (bRet == true) {
                        State = new gen.xsd.StateType();
                        State.value = deserializer
                                .getStringPropertyValue("state");
                        if (false == State.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " state");
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
                if (true == deserializer.hasProperty("supportedProgress")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        SupportedProgress = new gen.xsd.ArrayOfString();
                        SupportedProgress.setInstanceName("supportedProgress");
                        if (false == SupportedProgress
                                .deSerializeContents(deserializer)) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " supportedProgress");
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
                if (true == deserializer.hasProperty("progress")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        Progress = new gen.xsd.String20();
                        Progress.value = deserializer
                                .getStringPropertyValue("progress");
                        if (false == Progress.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " progress");
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
                if (true == deserializer.hasProperty("operationTime")) {
                    if (bRet == true) {
                        OperationTime = new OC.Cm.Serialization.Xsd.TimeType();
                        OperationTime.value = deserializer
                                .getStringPropertyValue("operationTime");
                        if (false == OperationTime.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " operationTime");
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
                if (true == deserializer.hasProperty("operationTimeMin")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        OperationTimeMin = new OC.Cm.Serialization.Xsd.TimeType();
                        OperationTimeMin.value = deserializer
                                .getStringPropertyValue("operationTimeMin");
                        if (false == OperationTimeMin.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " operationTimeMin");
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
                if (true == deserializer.hasProperty("operationTimeMax")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        OperationTimeMax = new OC.Cm.Serialization.Xsd.TimeType();
                        OperationTimeMax.value = deserializer
                                .getStringPropertyValue("operationTimeMax");
                        if (false == OperationTimeMax.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " operationTimeMax");
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
                if (true == deserializer.hasProperty("remainingTime")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        RemainingTime = new OC.Cm.Serialization.Xsd.TimeType();
                        RemainingTime.value = deserializer
                                .getStringPropertyValue("remainingTime");
                        if (false == RemainingTime.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " remainingTime");
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
                if (true == deserializer.hasProperty("runningTime")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        RunningTime = new OC.Cm.Serialization.Xsd.TimeType();
                        RunningTime.value = deserializer
                                .getStringPropertyValue("runningTime");
                        if (false == RunningTime.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " runningTime");
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
                if (true == deserializer.hasProperty("delayStartTime")) {
                    if (bRet == true) {
                        DelayStartTime = new OC.Cm.Serialization.Xsd.TimeType();
                        DelayStartTime.value = deserializer
                                .getStringPropertyValue("delayStartTime");
                        if (false == DelayStartTime.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " delayStartTime");
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
                if (true == deserializer.hasProperty("delayEndTime")) {
                    if (bRet == true) {
                        DelayEndTime = new OC.Cm.Serialization.Xsd.TimeType();
                        DelayEndTime.value = deserializer
                                .getStringPropertyValue("delayEndTime");
                        if (false == DelayEndTime.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " delayEndTime");
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
                if (true == deserializer.hasProperty("progressPercentage")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        progressPercentage = deserializer
                                .getUnsignedByte("progressPercentage");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("kidsLock")) {
                    if (bRet == true) {
                        KidsLock = new gen.xsd.StateType();
                        KidsLock.value = deserializer
                                .getStringPropertyValue("kidsLock");
                        if (false == KidsLock.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " kidsLock");
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
                if (true == deserializer.hasProperty("reset")) {
                    if (bRet == true) {
                        Reset = new gen.xsd.OnType();
                        Reset.value = deserializer
                                .getStringPropertyValue("reset");
                        if (false == Reset.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " reset");
                            bRet = false;
                        } else
                            bRet = true;
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
