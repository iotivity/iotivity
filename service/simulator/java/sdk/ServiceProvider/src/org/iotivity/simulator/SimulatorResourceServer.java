/*
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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
 */

package org.iotivity.simulator;

import java.util.Vector;

public class SimulatorResourceServer {

    private String resourceName;
    private String resourceURI;
    private String resourceType;
    private String interfaceType;

    private long   nativeHandle;

    private SimulatorResourceServer(long nativeHandle) {
        this.nativeHandle = nativeHandle;
    }

    public void setName(String resourceName) {
        this.resourceName = resourceName;
    }

    private void setURI(String resourceURI) {
        this.resourceURI = resourceURI;
    }

    private void setResourceType(String resourceType) {
        this.resourceType = resourceType;
    }

    private void setInterfaceType(String interfaceType) {
        this.interfaceType = interfaceType;
    }

    public String getName() {
        return resourceName;
    }

    public String getURI() {
        return resourceURI;
    }

    public String getResourceType() {
        return resourceType;
    }

    public String getInterfaceType() {
        return interfaceType;
    }

    @Override
    protected void finalize() throws Throwable {
        dispose();
    }

    public native SimulatorResourceModel getModel();

    public native void updateAttributeFromAllowedValues(String attrName,
            int index);

    public native void setRange(String attrName, int min, int max);

    public native void addAttributeInteger(String key, int value);

    public native void addAttributeDouble(String key, double value);

    public native void addAttributeBoolean(String key, Boolean value);

    public native void addAttributeStringN(String key, String value);

    public native void updateAttributeInteger(String key, int value);

    public native void updateAttributeDouble(String key, double value);

    public native void updateAttributeBoolean(String key, Boolean value);

    public native void updateAttributeStringN(String key, String value);

    public native void setAllowedValuesInteger(String key, int[] allowedValues);

    public native void setAllowedValuesDouble(String key, double[] allowedValues);

    public native void setAllowedValuesStringN(String key,
            String[] allowedValues);

    public native int startResourceAutomation();

    public native int startAttributeAutomation(String attrName);

    public native void startAutomation(int automationId);

    public native void removeAttribute(String key);

    private native void dispose();
}
