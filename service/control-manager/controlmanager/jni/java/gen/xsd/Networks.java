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
 * This class manages list of Network object and defines methods that serialize
 * and/or deserialize contents of XSD complex/list type Networks.
 * 
 * @since 1.0.0
 */
public class Networks extends gen.xsd.List {
    /**
     * Variable that contains Network objects in the List.
     */
    public java.util.List<gen.xsd.Network> network = new java.util.ArrayList<gen.xsd.Network>();

    /**
     * Default constructor of Networks class.
     * 
     * @since 1.0.0
     */
    public Networks() {
        setList();
    }

    /**
     * Destroy method for Networks class.
     * 
     * @since 1.0.0
     */
    @Override
    public void destroy() {
        super.destroy();
        for (gen.xsd.Network itr : network) {
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
        return Elements.EN_NETWORKS;
    }

    /**
     * Serializes the contents of Networks object.
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

                if (null != network) {
                    if (network.size() >= 0) {
                        if (serializer.validateMandatory() == false)
                            serializer.setIDExcemption(true);
                        if (bRet == true) {
                            serializer.startListProperty("Networks");
                            for (gen.xsd.Network itr : network) {
                                if (null == itr) {
                                    continue;
                                }
                                (itr).setInstanceName("Networks");
                                if (false == (itr).serialize(serializer)) {
                                    bRet = false;
                                    break;
                                }
                            }
                            serializer.endListProperty("Networks");
                        }
                        if (false == bRet)
                            serializer
                                    .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                            + " Networks");
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
     * Deserializes data and prepares the Networks object contents.
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
                boolean isArray = deserializer.getIsArray("Networks");
                int sizeOfArray = 0;

                if (deserializer.validateMandatory() == false) {
                    deserializer.setIDExcemption(true);
                }

                if (isArray) {
                    sizeOfArray = deserializer.getSize("Networks");

                    if (sizeOfArray > 0) {
                        for (int i = 0; i < sizeOfArray; i++) {
                            gen.xsd.Network m = new gen.xsd.Network();

                            deserializer.setCurrentProperty("Networks", i);

                            m.setInstanceName("Networks");

                            boolean bRet = m.deSerialize(deserializer);
                            if (bRet == false) {
                                retValue = bRet;
                                break;
                            }

                            network.add(m);
                        }
                    } else if (sizeOfArray == 0) {
                        deserializer.removeParameter("Networks");
                    }
                } else {
                    if (true == deserializer.hasProperty("Networks")) {
                        gen.xsd.Network m = new gen.xsd.Network();
                        boolean bRet = m.deSerialize(deserializer);
                        if (bRet == false) {
                            retValue = bRet;
                        }
                        network.add(m);
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
