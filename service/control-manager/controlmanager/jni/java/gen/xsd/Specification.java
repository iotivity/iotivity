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
 * complex/list type Specification.
 * 
 * @since 1.0.0
 */
public class Specification extends OC.Cm.Serialization.ISerializable {
    /**
     * [Optional] Indicate the input power source of the device. (e.g.,
     * â€œ105V-230V 60Hzâ€�, â€œ220V 60Hzâ€�, â€œ230V 50Hzâ€�, â€œ110V 60Hzâ€�,
     * â€œUnknownâ€�)
     */
    public gen.xsd.String20                Power;

    /**
     * [Optional] Indicate the supported display type of the device. Note that
     * declaration of the display enumeration type can be found in the
     * "Attribute Type" declaration. (e.g., â€œLEDâ€�, â€œSegmentâ€�, â€œSTNâ€�,
     * â€œLCDâ€�, â€œNot_Supportedâ€�)
     */
    public gen.xsd.DisplayType             Display;

    /**
     * [Optional] Indicate the supported sound information of the device. Note
     * that declaration of the sound enumeration type can be found in the
     * "Attribute Type" declaration. (e.g., â€œBeepâ€�, â€œAudio_32Bitâ€�,
     * â€œNot_Supportedâ€�)
     */
    public gen.xsd.SoundType               Sound;

    /**
     * [Optional] Indicate whether the over the air upgrade is supported or not
     * supported. (e.g., â€œTrueâ€�, â€œFalseâ€�)
     */
    public Boolean                         otaSupported;

    /**
     * [Optional] Indicate whether the device reset function is supported or not
     * supported. (e.g., â€œTrueâ€�, â€œFalseâ€�)
     */
    public Boolean                         resetDeviceSupported;

    /**
     * [Optional] Indicate whether the factory reset function is supported or
     * not supported. (e.g., â€œTrueâ€�, â€œFalseâ€�)
     */
    public Boolean                         resetFactoryDefaultSupported;

    /**
     * [Optional] Indicate the list of connectivity option supported by device.
     * (e.g., â€œWiFiâ€�, â€œZigbeeâ€�, â€œZ-waveâ€�, â€œBluetoothâ€�, â€œIRâ€�,
     * â€œCellularâ€�)
     */
    public gen.xsd.ArrayOfConnectivityType SupportedConnectivity;

    /**
     * Default constructor of Specification class.
     * 
     * @since 1.0.0
     */
    public Specification() {
    }

    /**
     * Destroy method for Specification class.
     * 
     * @since 1.0.0
     */
    @Override
    public void destroy() {
        super.destroy();
        if (null != SupportedConnectivity) {
            SupportedConnectivity.destroy();
        }
    }

    /**
     * Returns name of the serializable.
     * 
     * @since 1.0.0
     */
    @Override
    public String getElementName() {
        return Elements.EN_SPECIFICATION;
    }

    /**
     * Serializes the contents of Specification object.
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
                if (null != Power) {
                    if (Power.value.length() > 0) {
                        if (true == Power.validateContent()) {
                            bRet = serializer.setStringPropertyValue("power",
                                    Power.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " power");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != Display) {
                    if (Display.value.length() > 0) {
                        if (true == Display.validateContent()) {
                            bRet = serializer.setStringPropertyValue("display",
                                    Display.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " display");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != Sound) {
                    if (Sound.value.length() > 0) {
                        if (true == Sound.validateContent()) {
                            bRet = serializer.setStringPropertyValue("sound",
                                    Sound.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " sound");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != otaSupported) {
                    bRet = serializer.setBoolPropertyValue("otaSupported",
                            otaSupported);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " otaSupported");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != resetDeviceSupported) {
                    bRet = serializer.setBoolPropertyValue(
                            "resetDeviceSupported", resetDeviceSupported);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " resetDeviceSupported");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != resetFactoryDefaultSupported) {
                    bRet = serializer.setBoolPropertyValue(
                            "resetFactoryDefaultSupported",
                            resetFactoryDefaultSupported);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " resetFactoryDefaultSupported");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != SupportedConnectivity) {
                    SupportedConnectivity
                            .setInstanceName("supportedConnectivity");
                    bRet = SupportedConnectivity.serializeContents(serializer);
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " supportedConnectivity");
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
     * Deserializes data and prepares the Specification object contents.
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
                if (true == deserializer.hasProperty("power")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        Power = new gen.xsd.String20();
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
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("display")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        Display = new gen.xsd.DisplayType();
                        Display.value = deserializer
                                .getStringPropertyValue("display");
                        if (false == Display.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " display");
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
                if (true == deserializer.hasProperty("sound")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        Sound = new gen.xsd.SoundType();
                        Sound.value = deserializer
                                .getStringPropertyValue("sound");
                        if (false == Sound.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " sound");
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
                if (true == deserializer.hasProperty("otaSupported")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        otaSupported = deserializer
                                .getBoolPropertyValue("otaSupported");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("resetDeviceSupported")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        resetDeviceSupported = deserializer
                                .getBoolPropertyValue("resetDeviceSupported");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer
                        .hasProperty("resetFactoryDefaultSupported")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        resetFactoryDefaultSupported = deserializer
                                .getBoolPropertyValue("resetFactoryDefaultSupported");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("supportedConnectivity")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        SupportedConnectivity = new gen.xsd.ArrayOfConnectivityType();
                        SupportedConnectivity
                                .setInstanceName("supportedConnectivity");
                        if (false == SupportedConnectivity
                                .deSerializeContents(deserializer)) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " supportedConnectivity");
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
