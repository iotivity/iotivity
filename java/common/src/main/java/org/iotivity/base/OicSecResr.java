/*
 *******************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

package org.iotivity.base;

import java.util.List;
public class OicSecResr {

    private String href;
    private String rel;
    private List<String> types;
    private int typeLen;
    private List<String> interfaces;
    private int interfaceLen;

    public OicSecResr(String href, String rel, List<String> types, int typeLen,
            List<String> interfaces, int interfaceLen) {
        super();
        this.href = href;
        this.rel = rel;
        this.types = types;
        this.typeLen = typeLen;
        this.interfaces = interfaces;
        this.interfaceLen = interfaceLen;
    }

    public String getHref() {
        return href;
    }

    public void setHref(String href) {
        this.href = href;
    }

    public String getRel() {
        return rel;
    }

    public void setRel(String rel) {
        this.rel = rel;
    }

    public List<String> getTypesList() {
        return types;
    }

    public void setTypes(List<String> types) {
        this.types = types;
    }

    public int getTypeLen() {
        return typeLen;
    }

    public void setTypeLen(int typeLen) {
        this.typeLen = typeLen;
    }

    public List<String> getInterfacesList() {
        return interfaces;
    }

    public String getInterfaces(int i) {
        return this.interfaces.get(i);
    }

    public String getTypes(int i) {
        return this.types.get(i);
    }

    public void setInterfaces(List<String> interfaces) {
        this.interfaces = interfaces;
    }

    public int getInterfaceLen() {
        return interfaceLen;
    }

    public void setInterfaceLen(int interfaceLen) {
        this.interfaceLen = interfaceLen;
    }

}
