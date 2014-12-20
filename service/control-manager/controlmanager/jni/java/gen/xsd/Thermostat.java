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
 * This is a set of attributes associated with the theremostat.<br>
 * Also, this class defines methods that serialize and/or deserialize contents
 * of XSD complex/list type Thermostat.
 * 
 * @since 1.0.0
 */
public class Thermostat extends gen.xsd.Resource {
    /**
     * [Mandatory] Contains a set of available modes of the thermostat.
     * Available in both the singular and plural forms expression. In case of
     * plural forms, each option can be expressed in array of string. (e.g.,
     * â€œHeatingâ€�, â€œCoolingâ€�, â€œAutoâ€�, â€œFan Onlyâ€�, â€œDry Airâ€�,
     * â€œEnergy_Save_Heatâ€�)
     */
    public gen.xsd.ArrayOfString SupportedThermostatModes;

    /**
     * [Mandatory] Indicate currently selected thermostat mode. If there exists
     * â€œsupportedThermostatModesâ€�, the value of â€œthermostatModeâ€� should
     * be one of â€œsupportedThermostatModesâ€� list.
     */
    public gen.xsd.String20      ThermostatMode;

    /**
     * [Mandatory] Contains a set of available fan modes. Available in both the
     * singular and plural forms expression. In case of plural forms, each
     * option can be expressed in array of string. (e.g., â€œLowâ€�,
     * â€œMediumâ€�, â€œHighâ€�, â€œCirculationâ€�)
     */
    public gen.xsd.ArrayOfString SupportedFanModes;

    /**
     * [Mandatory] Indicate currently selected mode of the fan. If there exists
     * â€œsupportedFanModesâ€�, the value of â€œfanModeâ€� should be one of
     * â€œsupportedFanModesâ€� list.
     */
    public gen.xsd.String20      FanMode;

    /**
     * [Mandatory] Contains a set of available modes. Available in both the
     * singular and plural forms expression. In case of plural forms, each
     * option can be expressed in array of string. (e.g., â€œHeatingâ€�,
     * â€œCoolingâ€�, â€œFan Onlyâ€�, â€œPending_Heatâ€�, â€œPending_Coolâ€�)
     */
    public gen.xsd.ArrayOfString SupportedOperatingStates;

    /**
     * [Mandatory] Indicate currently selected operating states. If there exists
     * â€œsupportedOperatingStatesâ€�, the value of â€œoperatingStateâ€� should
     * be one of â€œsupportedOperatingStatesâ€� list.
     */
    public gen.xsd.String20      OperatingState;

    /**
     * Default constructor of Thermostat class.
     * 
     * @since 1.0.0
     */
    public Thermostat() {
    }

    /**
     * Destroy method for Thermostat class.
     * 
     * @since 1.0.0
     */
    @Override
    public void destroy() {
        super.destroy();
        if (null != SupportedThermostatModes) {
            SupportedThermostatModes.destroy();
        }
        if (null != SupportedFanModes) {
            SupportedFanModes.destroy();
        }
        if (null != SupportedOperatingStates) {
            SupportedOperatingStates.destroy();
        }
    }

    /**
     * Returns name of the serializable.
     * 
     * @since 1.0.0
     */
    @Override
    public String getElementName() {
        return Elements.EN_THERMOSTAT;
    }

    /**
     * Serializes the contents of Thermostat object.
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
                if (null != SupportedThermostatModes) {
                    SupportedThermostatModes
                            .setInstanceName("supportedThermostatModes");
                    bRet = SupportedThermostatModes
                            .serializeContents(serializer);
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " supportedThermostatModes");
                } else {
                    if (serializer
                            .isInFilteringMode("supportedThermostatModes") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " supportedThermostatModes");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (null != ThermostatMode) {
                    if (ThermostatMode.value.length() > 0) {
                        if (true == ThermostatMode.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "thermostatMode", ThermostatMode.value);
                        } else
                            bRet = false;
                    } else
                        bRet = false;
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " thermostatMode");
                } else {
                    if (serializer.isInFilteringMode("thermostatMode") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " thermostatMode");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != SupportedFanModes) {
                    SupportedFanModes.setInstanceName("supportedFanModes");
                    bRet = SupportedFanModes.serializeContents(serializer);
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " supportedFanModes");
                } else {
                    if (serializer.isInFilteringMode("supportedFanModes") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " supportedFanModes");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (null != FanMode) {
                    if (FanMode.value.length() > 0) {
                        if (true == FanMode.validateContent()) {
                            bRet = serializer.setStringPropertyValue("fanMode",
                                    FanMode.value);
                        } else
                            bRet = false;
                    } else
                        bRet = false;
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " fanMode");
                } else {
                    if (serializer.isInFilteringMode("fanMode") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " fanMode");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != SupportedOperatingStates) {
                    SupportedOperatingStates
                            .setInstanceName("supportedOperatingStates");
                    bRet = SupportedOperatingStates
                            .serializeContents(serializer);
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " supportedOperatingStates");
                } else {
                    if (serializer
                            .isInFilteringMode("supportedOperatingStates") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " supportedOperatingStates");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (null != OperatingState) {
                    if (OperatingState.value.length() > 0) {
                        if (true == OperatingState.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "operatingState", OperatingState.value);
                        } else
                            bRet = false;
                    } else
                        bRet = false;
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " operatingState");
                } else {
                    if (serializer.isInFilteringMode("operatingState") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " operatingState");
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
     * Deserializes data and prepares the Thermostat object contents.
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
                if (true == deserializer
                        .hasProperty("supportedThermostatModes")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        SupportedThermostatModes = new gen.xsd.ArrayOfString();
                        SupportedThermostatModes
                                .setInstanceName("supportedThermostatModes");
                        if (false == SupportedThermostatModes
                                .deSerializeContents(deserializer)) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " supportedThermostatModes");
                            bRet = false;
                        } else
                            bRet = true;
                    }
                } else {
                    if (deserializer
                            .isInFilteringMode("supportedThermostatModes") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " supportedThermostatModes");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("thermostatMode")) {
                    if (bRet == true) {
                        ThermostatMode = new gen.xsd.String20();
                        ThermostatMode.value = deserializer
                                .getStringPropertyValue("thermostatMode");
                        if (false == ThermostatMode.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " thermostatMode");
                            bRet = false;
                        } else
                            bRet = true;
                    }
                } else {
                    if (deserializer.isInFilteringMode("thermostatMode") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " thermostatMode");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("supportedFanModes")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        SupportedFanModes = new gen.xsd.ArrayOfString();
                        SupportedFanModes.setInstanceName("supportedFanModes");
                        if (false == SupportedFanModes
                                .deSerializeContents(deserializer)) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " supportedFanModes");
                            bRet = false;
                        } else
                            bRet = true;
                    }
                } else {
                    if (deserializer.isInFilteringMode("supportedFanModes") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " supportedFanModes");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("fanMode")) {
                    if (bRet == true) {
                        FanMode = new gen.xsd.String20();
                        FanMode.value = deserializer
                                .getStringPropertyValue("fanMode");
                        if (false == FanMode.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " fanMode");
                            bRet = false;
                        } else
                            bRet = true;
                    }
                } else {
                    if (deserializer.isInFilteringMode("fanMode") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " fanMode");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer
                        .hasProperty("supportedOperatingStates")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        SupportedOperatingStates = new gen.xsd.ArrayOfString();
                        SupportedOperatingStates
                                .setInstanceName("supportedOperatingStates");
                        if (false == SupportedOperatingStates
                                .deSerializeContents(deserializer)) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " supportedOperatingStates");
                            bRet = false;
                        } else
                            bRet = true;
                    }
                } else {
                    if (deserializer
                            .isInFilteringMode("supportedOperatingStates") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " supportedOperatingStates");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("operatingState")) {
                    if (bRet == true) {
                        OperatingState = new gen.xsd.String20();
                        OperatingState.value = deserializer
                                .getStringPropertyValue("operatingState");
                        if (false == OperatingState.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " operatingState");
                            bRet = false;
                        } else
                            bRet = true;
                    }
                } else {
                    if (deserializer.isInFilteringMode("operatingState") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " operatingState");
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
