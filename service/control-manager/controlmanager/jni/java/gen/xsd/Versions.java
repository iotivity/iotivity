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
 * This class manages list of Version object and defines methods that serialize
 * and/or deserialize contents of XSD complex/list type Versions.
 * 
 * @since 1.0.0
 */
public class Versions extends gen.xsd.List {
    /**
     * Variable that contains Version objects in the List.
     */
    public java.util.List<gen.xsd.Version> version = new java.util.ArrayList<gen.xsd.Version>();

    /**
     * Default constructor of Versions class.
     * 
     * @since 1.0.0
     */
    public Versions() {
        setList();
    }

    /**
     * Destroy method for Versions class.
     * 
     * @since 1.0.0
     */
    @Override
    public void destroy() {
        super.destroy();
        for (gen.xsd.Version itr : version) {
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
        return Elements.EN_VERSIONS;
    }

    /**
     * Serializes the contents of Versions object.
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

                if (null != version) {
                    if (version.size() >= 0) {
                        if (serializer.validateMandatory() == false)
                            serializer.setIDExcemption(true);
                        if (bRet == true) {
                            serializer.startListProperty("Versions");
                            for (gen.xsd.Version itr : version) {
                                if (null == itr) {
                                    continue;
                                }
                                (itr).setInstanceName("Versions");
                                if (false == (itr).serialize(serializer)) {
                                    bRet = false;
                                    break;
                                }
                            }
                            serializer.endListProperty("Versions");
                        }
                        if (false == bRet)
                            serializer
                                    .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                            + " Versions");
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
     * Deserializes data and prepares the Versions object contents.
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
                boolean isArray = deserializer.getIsArray("Versions");
                int sizeOfArray = 0;

                if (deserializer.validateMandatory() == false) {
                    deserializer.setIDExcemption(true);
                }

                if (isArray) {
                    sizeOfArray = deserializer.getSize("Versions");

                    if (sizeOfArray > 0) {
                        for (int i = 0; i < sizeOfArray; i++) {
                            gen.xsd.Version m = new gen.xsd.Version();

                            deserializer.setCurrentProperty("Versions", i);

                            m.setInstanceName("Versions");

                            boolean bRet = m.deSerialize(deserializer);
                            if (bRet == false) {
                                retValue = bRet;
                                break;
                            }

                            version.add(m);
                        }
                    } else if (sizeOfArray == 0) {
                        deserializer.removeParameter("Versions");
                    }
                } else {
                    if (true == deserializer.hasProperty("Versions")) {
                        gen.xsd.Version m = new gen.xsd.Version();
                        boolean bRet = m.deSerialize(deserializer);
                        if (bRet == false) {
                            retValue = bRet;
                        }
                        version.add(m);
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
