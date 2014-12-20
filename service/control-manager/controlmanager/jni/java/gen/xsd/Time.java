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
 * This resource contains the representation of time.<br>
 * Also, this class defines methods that serialize and/or deserialize contents
 * of XSD complex/list type Time.
 * 
 * @since 1.0.0
 */
public class Time extends gen.xsd.Resource {
    /**
     * [Mandatory] The current time, in the format defined by DateTimeType
     * (e.g., â€œ2013-07-15T18:44:31â€�) . This attribute may be set by client
     * or may be calculated by following expression: currentTime = UTC+ tzOffset
     * (+ dstOffset when in effect).
     */
    public OC.Cm.Serialization.Xsd.DateTimeType   CurrentTime;

    /**
     * [Optional] Time at which daylight savings begins (apply dstOffset).
     * Result of dstStartRule calculation.
     */
    public OC.Cm.Serialization.Xsd.DateTimeType   DstStartTime;

    /**
     * [Optional] Daylight savings time offset from local standard time. A
     * typical practice is advancing clocks one hour when daylight savings time
     * is in effect, which would result in a positive dstOffset.
     */
    public OC.Cm.Serialization.Xsd.TimeOffsetType DstOffset;

    /**
     * [Optional] Time at which daylight savings ends (dstOffset no longer
     * applied). Result of dstEndRule calculation.
     */
    public OC.Cm.Serialization.Xsd.DateTimeType   DstEndTime;

    /**
     * [Optional] Time zones are expressed as positive or negative offsets form
     * UTC. Offset from UTC are written in the format +-[hh]:[mm]. So if the
     * time being described is one hour ahead of UTC, the zone designator would
     * be "+01:00".
     */
    public OC.Cm.Serialization.Xsd.TimeOffsetType TzOffset;

    /**
     * [Optional] Indicate the Time Zone ID. (e.g., "Asia/Seoul")
     */
    public gen.xsd.TimeZoneType                   TzID;

    /**
     * [Optional] UTC (Coordinated Universal Time) is the primary time standard.
     * UTC is used in many Internet and World Wide Web standards. The Network
     * Time Protocol, designed to synchronise the clocks of computers over the
     * Internet, encodes times using the UTC system.
     */
    public OC.Cm.Serialization.Xsd.DateTimeType   UtcTime;

    /**
     * Default constructor of Time class.
     * 
     * @since 1.0.0
     */
    public Time() {
    }

    /**
     * Destroy method for Time class.
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
        return Elements.EN_TIME;
    }

    /**
     * Serializes the contents of Time object.
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
                if (null != CurrentTime) {
                    if (CurrentTime.value.length() > 0) {
                        if (true == CurrentTime.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "currentTime", CurrentTime.value);
                        } else
                            bRet = false;
                    } else
                        bRet = false;
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " currentTime");
                } else {
                    if (serializer.isInFilteringMode("currentTime") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " currentTime");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (null != DstStartTime) {
                    if (DstStartTime.value.length() > 0) {
                        if (true == DstStartTime.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "dstStartTime", DstStartTime.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " dstStartTime");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (null != DstOffset) {
                    if (DstOffset.value.length() > 0) {
                        if (true == DstOffset.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "dstOffset", DstOffset.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " dstOffset");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (null != DstEndTime) {
                    if (DstEndTime.value.length() > 0) {
                        if (true == DstEndTime.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "dstEndTime", DstEndTime.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " dstEndTime");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (null != TzOffset) {
                    if (TzOffset.value.length() > 0) {
                        if (true == TzOffset.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "tzOffset", TzOffset.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " tzOffset");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (null != TzID) {
                    if (TzID.value.length() > 0) {
                        if (true == TzID.validateContent()) {
                            bRet = serializer.setStringPropertyValue("tzID",
                                    TzID.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " tzID");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (null != UtcTime) {
                    if (UtcTime.value.length() > 0) {
                        if (true == UtcTime.validateContent()) {
                            bRet = serializer.setStringPropertyValue("utcTime",
                                    UtcTime.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " utcTime");
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
     * Deserializes data and prepares the Time object contents.
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
                if (true == deserializer.hasProperty("currentTime")) {
                    if (bRet == true) {
                        CurrentTime = new OC.Cm.Serialization.Xsd.DateTimeType();
                        CurrentTime.value = deserializer
                                .getStringPropertyValue("currentTime");
                        if (false == CurrentTime.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " currentTime");
                            bRet = false;
                        } else
                            bRet = true;
                    }
                } else {
                    if (deserializer.isInFilteringMode("currentTime") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " currentTime");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("dstStartTime")) {
                    if (bRet == true) {
                        DstStartTime = new OC.Cm.Serialization.Xsd.DateTimeType();
                        DstStartTime.value = deserializer
                                .getStringPropertyValue("dstStartTime");
                        if (false == DstStartTime.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " dstStartTime");
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
                if (true == deserializer.hasProperty("dstOffset")) {
                    if (bRet == true) {
                        DstOffset = new OC.Cm.Serialization.Xsd.TimeOffsetType();
                        DstOffset.value = deserializer
                                .getStringPropertyValue("dstOffset");
                        if (false == DstOffset.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " dstOffset");
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
                if (true == deserializer.hasProperty("dstEndTime")) {
                    if (bRet == true) {
                        DstEndTime = new OC.Cm.Serialization.Xsd.DateTimeType();
                        DstEndTime.value = deserializer
                                .getStringPropertyValue("dstEndTime");
                        if (false == DstEndTime.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " dstEndTime");
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
                if (true == deserializer.hasProperty("tzOffset")) {
                    if (bRet == true) {
                        TzOffset = new OC.Cm.Serialization.Xsd.TimeOffsetType();
                        TzOffset.value = deserializer
                                .getStringPropertyValue("tzOffset");
                        if (false == TzOffset.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " tzOffset");
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
                if (true == deserializer.hasProperty("tzID")) {
                    if (bRet == true) {
                        TzID = new gen.xsd.TimeZoneType();
                        TzID.value = deserializer
                                .getStringPropertyValue("tzID");
                        if (false == TzID.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " tzID");
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
                if (true == deserializer.hasProperty("utcTime")) {
                    if (bRet == true) {
                        UtcTime = new OC.Cm.Serialization.Xsd.DateTimeType();
                        UtcTime.value = deserializer
                                .getStringPropertyValue("utcTime");
                        if (false == UtcTime.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " utcTime");
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
