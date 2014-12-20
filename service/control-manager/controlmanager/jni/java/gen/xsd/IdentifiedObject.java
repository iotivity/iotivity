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
 * This is a root class to provide common naming attributes for all classes
 * needing naming attributes.<br>
 * Also, this class defines methods that serialize and/or deserialize contents
 * of XSD complex/list type IdentifiedObject.
 * 
 * @since 1.0.0
 */
public class IdentifiedObject extends gen.xsd.Resource {
    /**
     * [Mandatory] The device id should has sufficient entropy to uniquely
     * identify the device in the context of its usage and is used to identify a
     * device within a premise or cloud service. When the value of the attribute
     * is zero, it represents device itself. (e.g., â€œ0â€�) In case of the
     * Group ID, value should start from zero and be increased by one per new
     * group creation request. (e.g., â€œ3â€�) Note that if the representation
     * of the requested resource contains this attribute for PUT and POST
     * methods, attribute should be ignored by the device.
     */
    public gen.xsd.String50 Id;

    /**
     * [Optional] The description is a human readable text describing or naming
     * the object. (e.g., â€œManage installed devicesâ€�)
     */
    public gen.xsd.String50 Description;

    /**
     * Default constructor of IdentifiedObject class.
     * 
     * @since 1.0.0
     */
    public IdentifiedObject() {
    }

    /**
     * Destroy method for IdentifiedObject class.
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
        return Elements.EN_IDENTIFIEDOBJECT;
    }

    /**
     * Serializes the contents of IdentifiedObject object.
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
            if ((serializer.validateMandatory() == true)
                    || ((serializer.validateMandatory() == false) && (serializer
                            .getIDExcemption()))) {
                boolean bRet = true;
                if (null != Id) {
                    if (Id.value.length() > 0) {
                        if (true == Id.validateContent()) {
                            bRet = serializer.setStringPropertyValue("id",
                                    Id.value);
                        } else
                            bRet = false;
                    } else
                        bRet = false;
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " id");
                } else {
                    if (serializer.isInFilteringMode("id") == false) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " id");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (null != Description) {
                    if (Description.value.length() > 0) {
                        if (true == Description.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "description", Description.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " description");
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
     * Deserializes data and prepares the IdentifiedObject object contents.
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
                if (true == deserializer.hasProperty("id")) {
                    if ((deserializer.validateMandatory() == false)
                            && (deserializer.getIDExcemption() == false))
                        bRet = false;
                    if (bRet == true) {
                        Id = new gen.xsd.String50();
                        Id.value = deserializer.getStringPropertyValue("id");
                        if (false == Id.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " id");
                            bRet = false;
                        } else
                            bRet = true;
                    }
                } else {
                    if (deserializer.isInFilteringMode("id") == false) {
                        deserializer
                                .setErrorMessage("Mandatory Parameter not found : "
                                        + " id");
                        bRet = false;
                    } else
                        bRet = true;
                }
                if (retValue == true)
                    retValue = bRet;
            }

            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("description")) {
                    if (bRet == true) {
                        Description = new gen.xsd.String50();
                        Description.value = deserializer
                                .getStringPropertyValue("description");
                        if (false == Description.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " description");
                            bRet = false;
                        } else
                            bRet = true;
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
}
