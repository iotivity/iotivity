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
 * This function set provides a mechanism to allow the server to push down
 * targets or challenges to help consumers manage their budgets within a certain
 * time frame.<br>
 * Also, this class defines methods that serialize and/or deserialize contents
 * of XSD complex/list type UsageLimits.
 * 
 * @since 1.0.0
 */
public class UsageLimits extends gen.xsd.List {
    /**
     * Variable that contains UsageLimit objects in the List.
     */
    public java.util.List<gen.xsd.UsageLimit> usageLimit = new java.util.ArrayList<gen.xsd.UsageLimit>();

    /**
     * Default constructor of UsageLimits class.
     * 
     * @since 1.0.0
     */
    public UsageLimits() {
        setList();
    }

    /**
     * Destroy method for UsageLimits class.
     * 
     * @since 1.0.0
     */
    @Override
    public void destroy() {
        super.destroy();
        for (gen.xsd.UsageLimit itr : usageLimit) {
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
        return Elements.EN_USAGELIMITS;
    }

    /**
     * Serializes the contents of UsageLimits object.
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

                if (null != usageLimit) {
                    if (usageLimit.size() >= 0) {
                        if (serializer.validateMandatory() == false)
                            serializer.setIDExcemption(true);
                        if (bRet == true) {
                            serializer.startListProperty("UsageLimits");
                            for (gen.xsd.UsageLimit itr : usageLimit) {
                                if (null == itr) {
                                    continue;
                                }
                                (itr).setInstanceName("UsageLimits");
                                if (false == (itr).serialize(serializer)) {
                                    bRet = false;
                                    break;
                                }
                            }
                            serializer.endListProperty("UsageLimits");
                        }
                        if (false == bRet)
                            serializer
                                    .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                            + " UsageLimits");
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
     * Deserializes data and prepares the UsageLimits object contents.
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
                boolean isArray = deserializer.getIsArray("UsageLimits");
                int sizeOfArray = 0;

                if (deserializer.validateMandatory() == false) {
                    deserializer.setIDExcemption(true);
                }

                if (isArray) {
                    sizeOfArray = deserializer.getSize("UsageLimits");

                    if (sizeOfArray > 0) {
                        for (int i = 0; i < sizeOfArray; i++) {
                            gen.xsd.UsageLimit m = new gen.xsd.UsageLimit();

                            deserializer.setCurrentProperty("UsageLimits", i);

                            m.setInstanceName("UsageLimits");

                            boolean bRet = m.deSerialize(deserializer);
                            if (bRet == false) {
                                retValue = bRet;
                                break;
                            }

                            usageLimit.add(m);
                        }
                    } else if (sizeOfArray == 0) {
                        deserializer.removeParameter("UsageLimits");
                    }
                } else {
                    if (true == deserializer.hasProperty("UsageLimits")) {
                        gen.xsd.UsageLimit m = new gen.xsd.UsageLimit();
                        boolean bRet = m.deSerialize(deserializer);
                        if (bRet == false) {
                            retValue = bRet;
                        }
                        usageLimit.add(m);
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
