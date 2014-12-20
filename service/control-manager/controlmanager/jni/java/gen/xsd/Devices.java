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
 * Devices resource provides an interface to manage installed devices in Smart
 * Home. Devices resource has multiple Device resources. If a Smart Home Device
 * represent only itself, the device SHALL has a Device resource and /device
 * URI. In case of a Smart Home Gateway, it can represent other devices
 * simultaneously and have a Devices resource and /devices URI. <br>
 * Also, this class defines methods that serialize and/or deserialize contents
 * of XSD complex/list type Devices.
 * 
 * @since 1.0.0
 */
public class Devices extends gen.xsd.List {
    /**
     * Variable that contains Device objects in the List.
     */
    public java.util.List<gen.xsd.Device> device = new java.util.ArrayList<gen.xsd.Device>();

    /**
     * Default constructor of Devices class.
     * 
     * @since 1.0.0
     */
    public Devices() {
        setList();
    }

    /**
     * Destroy method for Devices class.
     * 
     * @since 1.0.0
     */
    @Override
    public void destroy() {
        super.destroy();
        for (gen.xsd.Device itr : device) {
            if (itr != null) {
                itr.destroy();
            }
        }
    }

    /**
     * Returns name of the serializable.
     * 
     * @since 1.0.0
     */
    @Override
    public String getElementName() {
        return Elements.EN_DEVICES;
    }

    /**
     * Serializes the contents of Devices object.
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

                if (null != device) {
                    if (serializer.validateMandatory() == false)
                        serializer.setIDExcemption(true);
                    if (device.size() < 1) {
                        if (serializer.isInFilteringMode("Devices") == false) {
                            serializer
                                    .setErrorMessage(" parameter item count is less than minOccures value specified in xsd : Devices");
                            bRet = false;
                        }
                    }
                    if (bRet == true) {
                        serializer.startListProperty("Devices");
                        for (gen.xsd.Device itr : device) {
                            if (null == itr) {
                                continue;
                            }
                            (itr).setInstanceName("Devices");
                            if (false == (itr).serialize(serializer)) {
                                bRet = false;
                                break;
                            }
                        }
                        serializer.endListProperty("Devices");
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " Devices");
                } else {
                    if (serializer.isInFilteringMode("Devices") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " Device");
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
     * Deserializes data and prepares the Devices object contents.
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
                boolean isArray = deserializer.getIsArray("Devices");
                int sizeOfArray = 0;

                if (deserializer.validateMandatory() == false) {
                    deserializer.setIDExcemption(true);
                }

                if (isArray) {
                    sizeOfArray = deserializer.getSize("Devices");

                    if (sizeOfArray < 1) {
                        if ((deserializer.isInFilteringMode("Devices") == false)) {
                            deserializer
                                    .setErrorMessage(" parameter item count is less than minOccures value specified in xsd : Devices");
                            retValue = false;
                        }
                    }

                    if (sizeOfArray > 0) {
                        for (int i = 0; i < sizeOfArray; i++) {
                            gen.xsd.Device m = new gen.xsd.Device();

                            deserializer.setCurrentProperty("Devices", i);

                            m.setInstanceName("Devices");

                            boolean bRet = m.deSerialize(deserializer);
                            if (bRet == false) {
                                retValue = bRet;
                                break;
                            }

                            device.add(m);
                        }
                    } else if (sizeOfArray == 0) {
                        deserializer.removeParameter("Devices");
                    }
                } else {
                    if (true == deserializer.hasProperty("Devices")) {
                        gen.xsd.Device m = new gen.xsd.Device();
                        boolean bRet = m.deSerialize(deserializer);
                        if (bRet == false) {
                            retValue = bRet;
                        }
                        device.add(m);
                    } else {
                        if ((deserializer.isInFilteringMode("Devices") == false)) {
                            deserializer
                                    .setErrorMessage("parameter not found : "
                                            + " Devices");
                            retValue = false;
                        }
                    }
                }
            }
        } catch (Exception e) {
            return false;
        }
        return retValue;

    }
}
