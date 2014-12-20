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
 * This resource provides the ability to control the movement of the device.<br>
 * Also, this class defines methods that serialize and/or deserialize contents
 * of XSD complex/list type Movement.
 * 
 * @since 1.0.0
 */
public class Movement extends gen.xsd.Resource {
    /**
     * [Mandatory] This attribute defines the movement of the device. Note that
     * declaration of the movement enumeration type can be found in the
     * "Attribute Type" declaration. (e.g., â€œStopâ€�, â€œForwardâ€�,
     * â€œBackwardâ€�, â€œLeftâ€�, â€œRightâ€�)
     */
    public gen.xsd.MovementType Behavior;

    /**
     * Default constructor of Movement class.
     * 
     * @since 1.0.0
     */
    public Movement() {
    }

    /**
     * Destroy method for Movement class.
     * 
     * @since 1.0.0
     */
    @Override
    public void destroy() {
        super.destroy();
    }

    /**
     * Returns name of the serializable.
     * 
     * @since 1.0.0
     */
    @Override
    public String getElementName() {
        return Elements.EN_MOVEMENT;
    }

    /**
     * Serializes the contents of Movement object.
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
                if (null != Behavior) {
                    if (Behavior.value.length() > 0) {
                        if (true == Behavior.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "behavior", Behavior.value);
                        } else
                            bRet = false;
                    } else
                        bRet = false;
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " behavior");
                } else {
                    if (serializer.isInFilteringMode("behavior") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " behavior");
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
     * Deserializes data and prepares the Movement object contents.
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
                if (true == deserializer.hasProperty("behavior")) {
                    if (bRet == true) {
                        Behavior = new gen.xsd.MovementType();
                        Behavior.value = deserializer
                                .getStringPropertyValue("behavior");
                        if (false == Behavior.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " behavior");
                            bRet = false;
                        } else
                            bRet = true;
                    }
                } else {
                    if (deserializer.isInFilteringMode("behavior") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " behavior");
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
