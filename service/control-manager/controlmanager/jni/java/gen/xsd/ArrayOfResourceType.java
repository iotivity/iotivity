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

public class ArrayOfResourceType extends OC.Cm.Serialization.ISerializable {

    /**
     * Variable that contains ResourceType objects in the List.
     */
    public java.util.List<gen.xsd.ResourceType> members = new java.util.ArrayList<gen.xsd.ResourceType>();

    @Override
    public String getElementName() {
        return "XS:LIST";
    }

    @Override
    public boolean serializeContents(ISerializer serializer) {
        boolean bRet = true;

        try {
            serializer.startListProperty(this.getInstanceName());

            for (gen.xsd.ResourceType itr : members) {

                bRet = (itr).validateContent();
                if (bRet)
                    bRet = serializer.setStringPropertyValue(
                            this.getInstanceName(), itr.value);

                if (bRet == false) {
                    serializer
                            .setErrorMessage(" Parameter Contains Invalid Value : "
                                    + this.getInstanceName());
                    break;
                }
            }
            serializer.endListProperty(this.getInstanceName());
        } catch (Exception e) {
            return false;
        }

        return bRet;
    }

    @Override
    public boolean deSerializeContents(IDeserializer deserializer) {
        boolean bRet = true;
        try {
            boolean isArray = deserializer.getIsArray(this.getInstanceName());
            int sizeOfArray = 0;

            if (isArray) {
                sizeOfArray = deserializer.getSize(this.getInstanceName());
                for (int i = 0; i < sizeOfArray; i++) {
                    deserializer.setCurrentProperty(this.getInstanceName(), i);

                    gen.xsd.ResourceType el;

                    el = new gen.xsd.ResourceType();
                    el.value = deserializer.getStringPropertyValue(this
                            .getInstanceName());

                    if (el.value != null && !el.value.isEmpty()) {
                        if (el.validateContent()) {
                            members.add(el);
                        }
                    }
                    if (!bRet) {
                        deserializer
                                .setErrorMessage(" Parameter Contains Invalid Value : "
                                        + this.getInstanceName());
                        break;
                    }
                }
            } else {
                if (true == deserializer.hasProperty(this.getInstanceName())) {
                    gen.xsd.ResourceType el;

                    el = new gen.xsd.ResourceType();
                    el.value = deserializer.getStringPropertyValue(this
                            .getInstanceName());
                    if (el.value != null && !el.value.isEmpty()) {
                        if (el.validateContent()) {
                            members.add(el);
                        }
                    }
                    if (!bRet) {
                        deserializer
                                .setErrorMessage(" Parameter Contains Invalid Value : "
                                        + this.getInstanceName());
                        return false;
                    }
                }
            }
        } catch (Exception e) {
            return false;
        }

        return bRet;
    }
};
