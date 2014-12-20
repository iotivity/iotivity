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
 * complex/list type Energy.
 * 
 * @since 1.0.0
 */
public class Energy extends OC.Cm.Serialization.ISerializable {
    /**
     * [Optional] Variable of Usages object.
     */
    public gen.xsd.Usages      usages;

    /**
     * [Optional] Variable of UsageLimits object.
     */
    public gen.xsd.UsageLimits usageLimits;

    /**
     * Default constructor of Energy class.
     * 
     * @since 1.0.0
     */
    public Energy() {
    }

    /**
     * Destroy method for Energy class.
     * 
     * @since 1.0.0
     */
    @Override
    public void destroy() {
        super.destroy();
        if (null != usages) {
            usages.destroy();
        }
        if (null != usageLimits) {
            usageLimits.destroy();
        }
    }

    /**
     * Returns name of the serializable.
     * 
     * @since 1.0.0
     */
    @Override
    public String getElementName() {
        return Elements.EN_ENERGY;
    }

    /**
     * Serializes the contents of Energy object.
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
            if (serializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (null != usages) {
                        bRet = usages.serialize(serializer);
                        if (false == bRet) {
                            serializer
                                    .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                            + " Usages");
                        }
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            }
            if (serializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (null != usageLimits) {
                        bRet = usageLimits.serialize(serializer);
                        if (false == bRet) {
                            serializer
                                    .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                            + " UsageLimits");
                        }
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            }
        } catch (Exception e) {
            return false;
        }
        return retValue;

    }

    /**
     * Deserializes data and prepares the Energy object contents.
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
            if (deserializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (true == deserializer.hasGroupProperty("Usages")) {
                        if (bRet == true) {
                            usages = new gen.xsd.Usages();
                            bRet = usages.deSerialize(deserializer);
                        }
                        if (false == bRet)
                            deserializer
                                    .setErrorMessage("Failed To deserialize : "
                                            + " Usages");
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            } else {
                if (true == deserializer.hasProperty("Usages")) {
                    retValue = false;
                    deserializer
                            .setErrorMessage("Additional data to deserialize: Usages");
                }
            }
            if (deserializer.isRecursive()) {
                {
                    boolean bRet = true;
                    if (true == deserializer.hasGroupProperty("UsageLimits")) {
                        if (bRet == true) {
                            usageLimits = new gen.xsd.UsageLimits();
                            bRet = usageLimits.deSerialize(deserializer);
                        }
                        if (false == bRet)
                            deserializer
                                    .setErrorMessage("Failed To deserialize : "
                                            + " UsageLimits");
                    }
                    if (retValue == true)
                        retValue = bRet;
                }

            } else {
                if (true == deserializer.hasProperty("UsageLimits")) {
                    retValue = false;
                    deserializer
                            .setErrorMessage("Additional data to deserialize: UsageLimits");
                }
            }
        } catch (Exception e) {
            return false;
        }
        return retValue;

    }
}
