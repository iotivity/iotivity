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

public class ArrayOfAnyURI extends OC.Cm.Serialization.ISerializable {

    /**
     * Variable that contains String objects in the List.
     */
    public java.util.List<String> members = new java.util.ArrayList<String>();

    @Override
    public String getElementName() {
        return "XS:LIST";
    }

    @Override
    public boolean serializeContents(ISerializer serializer) {
        boolean bRet = true;

        try {
            serializer.startListProperty(this.getInstanceName());

            for (String itr : members) {

                bRet = serializer.setStringPropertyValue(
                        this.getInstanceName(), itr);

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

                    String el;

                    el = deserializer.getStringPropertyValue(this
                            .getInstanceName());

                    members.add(el);
                }
            } else {
                if (true == deserializer.hasProperty(this.getInstanceName())) {
                    String el;

                    el = deserializer.getStringPropertyValue(this
                            .getInstanceName());

                    members.add(el);
                }
            }
        } catch (Exception e) {
            return false;
        }

        return bRet;
    }
};
