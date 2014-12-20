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
 * This Subscription and Notification resource set support a generic,
 * lightweight subscription/notification mechanism for use throughout the
 * specification. This mechanism may be useful when a client wishes to quickly
 * learn of changes to a resource on a server.<br>
 * Also, this class defines methods that serialize and/or deserialize contents
 * of XSD complex/list type Subscriptions.
 * 
 * @since 1.0.0
 */
public class Subscriptions extends gen.xsd.List {
    /**
     * Variable that contains Subscription objects in the List.
     */
    public java.util.List<gen.xsd.Subscription> subscription = new java.util.ArrayList<gen.xsd.Subscription>();

    /**
     * Default constructor of Subscriptions class.
     * 
     * @since 1.0.0
     */
    public Subscriptions() {
        setList();
    }

    /**
     * Destroy method for Subscriptions class.
     * 
     * @since 1.0.0
     */
    @Override
    public void destroy() {
        super.destroy();
        for (gen.xsd.Subscription itr : subscription) {
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
        return Elements.EN_SUBSCRIPTIONS;
    }

    /**
     * Serializes the contents of Subscriptions object.
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

                if (null != subscription) {
                    if (subscription.size() >= 0) {
                        if (serializer.validateMandatory() == false)
                            serializer.setIDExcemption(true);
                        if (bRet == true) {
                            serializer.startListProperty("Subscriptions");
                            for (gen.xsd.Subscription itr : subscription) {
                                if (null == itr) {
                                    continue;
                                }
                                (itr).setInstanceName("Subscriptions");
                                if (false == (itr).serialize(serializer)) {
                                    bRet = false;
                                    break;
                                }
                            }
                            serializer.endListProperty("Subscriptions");
                        }
                        if (false == bRet)
                            serializer
                                    .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                            + " Subscriptions");
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
     * Deserializes data and prepares the Subscriptions object contents.
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
                boolean isArray = deserializer.getIsArray("Subscriptions");
                int sizeOfArray = 0;

                if (deserializer.validateMandatory() == false) {
                    deserializer.setIDExcemption(true);
                }

                if (isArray) {
                    sizeOfArray = deserializer.getSize("Subscriptions");

                    if (sizeOfArray > 0) {
                        for (int i = 0; i < sizeOfArray; i++) {
                            gen.xsd.Subscription m = new gen.xsd.Subscription();

                            deserializer.setCurrentProperty("Subscriptions", i);

                            m.setInstanceName("Subscriptions");

                            boolean bRet = m.deSerialize(deserializer);
                            if (bRet == false) {
                                retValue = bRet;
                                break;
                            }

                            subscription.add(m);
                        }
                    } else if (sizeOfArray == 0) {
                        deserializer.removeParameter("Subscriptions");
                    }
                } else {
                    if (true == deserializer.hasProperty("Subscriptions")) {
                        gen.xsd.Subscription m = new gen.xsd.Subscription();
                        boolean bRet = m.deSerialize(deserializer);
                        if (bRet == false) {
                            retValue = bRet;
                        }
                        subscription.add(m);
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
