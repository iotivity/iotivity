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
 * This resource provides an interface for a history record.<br>
 * Also, this class defines methods that serialize and/or deserialize contents
 * of XSD complex/list type Records.
 * 
 * @since 1.0.0
 */
public class Records extends gen.xsd.List {
    /**
     * Variable that contains Record objects in the List.
     */
    public java.util.List<gen.xsd.Record> record = new java.util.ArrayList<gen.xsd.Record>();

    /**
     * Default constructor of Records class.
     * 
     * @since 1.0.0
     */
    public Records() {
        setList();
    }

    /**
     * Destroy method for Records class.
     * 
     * @since 1.0.0
     */
    @Override
    public void destroy() {
        super.destroy();
        for (gen.xsd.Record itr : record) {
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
        return Elements.EN_RECORDS;
    }

    /**
     * Serializes the contents of Records object.
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

                if (null != record) {
                    if (record.size() >= 0) {
                        if (serializer.validateMandatory() == false)
                            serializer.setIDExcemption(true);
                        if (bRet == true) {
                            serializer.startListProperty("Records");
                            for (gen.xsd.Record itr : record) {
                                if (null == itr) {
                                    continue;
                                }
                                (itr).setInstanceName("Records");
                                if (false == (itr).serialize(serializer)) {
                                    bRet = false;
                                    break;
                                }
                            }
                            serializer.endListProperty("Records");
                        }
                        if (false == bRet)
                            serializer
                                    .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                            + " Records");
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
     * Deserializes data and prepares the Records object contents.
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
                boolean isArray = deserializer.getIsArray("Records");
                int sizeOfArray = 0;

                if (deserializer.validateMandatory() == false) {
                    deserializer.setIDExcemption(true);
                }

                if (isArray) {
                    sizeOfArray = deserializer.getSize("Records");

                    if (sizeOfArray > 0) {
                        for (int i = 0; i < sizeOfArray; i++) {
                            gen.xsd.Record m = new gen.xsd.Record();

                            deserializer.setCurrentProperty("Records", i);

                            m.setInstanceName("Records");

                            boolean bRet = m.deSerialize(deserializer);
                            if (bRet == false) {
                                retValue = bRet;
                                break;
                            }

                            record.add(m);
                        }
                    } else if (sizeOfArray == 0) {
                        deserializer.removeParameter("Records");
                    }
                } else {
                    if (true == deserializer.hasProperty("Records")) {
                        gen.xsd.Record m = new gen.xsd.Record();
                        boolean bRet = m.deSerialize(deserializer);
                        if (bRet == false) {
                            retValue = bRet;
                        }
                        record.add(m);
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
