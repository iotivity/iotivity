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
 * This class manages list of Door object and defines methods that serialize
 * and/or deserialize contents of XSD complex/list type Doors.
 * 
 * @since 1.0.0
 */
public class Doors extends gen.xsd.List {
    /**
     * Variable that contains Door objects in the List.
     */
    public java.util.List<gen.xsd.Door> door = new java.util.ArrayList<gen.xsd.Door>();

    /**
     * Default constructor of Doors class.
     * 
     * @since 1.0.0
     */
    public Doors() {
        setList();
    }

    /**
     * Destroy method for Doors class.
     * 
     * @since 1.0.0
     */
    @Override
    public void destroy() {
        super.destroy();
        for (gen.xsd.Door itr : door) {
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
        return Elements.EN_DOORS;
    }

    /**
     * Serializes the contents of Doors object.
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

                if (null != door) {
                    if (door.size() >= 0) {
                        if (serializer.validateMandatory() == false)
                            serializer.setIDExcemption(true);
                        if (bRet == true) {
                            serializer.startListProperty("Doors");
                            for (gen.xsd.Door itr : door) {
                                if (null == itr) {
                                    continue;
                                }
                                (itr).setInstanceName("Doors");
                                if (false == (itr).serialize(serializer)) {
                                    bRet = false;
                                    break;
                                }
                            }
                            serializer.endListProperty("Doors");
                        }
                        if (false == bRet)
                            serializer
                                    .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                            + " Doors");
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
     * Deserializes data and prepares the Doors object contents.
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
                boolean isArray = deserializer.getIsArray("Doors");
                int sizeOfArray = 0;

                if (deserializer.validateMandatory() == false) {
                    deserializer.setIDExcemption(true);
                }

                if (isArray) {
                    sizeOfArray = deserializer.getSize("Doors");

                    if (sizeOfArray > 0) {
                        for (int i = 0; i < sizeOfArray; i++) {
                            gen.xsd.Door m = new gen.xsd.Door();

                            deserializer.setCurrentProperty("Doors", i);

                            m.setInstanceName("Doors");

                            boolean bRet = m.deSerialize(deserializer);
                            if (bRet == false) {
                                retValue = bRet;
                                break;
                            }

                            door.add(m);
                        }
                    } else if (sizeOfArray == 0) {
                        deserializer.removeParameter("Doors");
                    }
                } else {
                    if (true == deserializer.hasProperty("Doors")) {
                        gen.xsd.Door m = new gen.xsd.Door();
                        boolean bRet = m.deSerialize(deserializer);
                        if (bRet == false) {
                            retValue = bRet;
                        }
                        door.add(m);
                    } else {

                    }
                }
            }
        } catch (Exception e) {
            return false;
        }
        return retValue;

    }
}
