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

public class SimulatorResourceAttribute {

    private long nativeHandle;

    public SimulatorResourceAttribute() {
    }

    public SimulatorResourceAttribute(String attrName) {
        create(attrName);
    }

    private SimulatorResourceAttribute(long handle) {
        nativeHandle = handle;
    }

    @Override
    protected void finalize() throws Throwable {
        dispose();
    }

    public <T> T getAttributeValue() {
        Object obj = getValue();
        @SuppressWarnings("unchecked")
        T t = (T) obj;
        return t;
    }

    private native void dispose();

    public native void create(String attrName);

    public native String getName();

    public native Object getValue();

    public native int allowedValuesSize();

    public native String valueToString();

    public native String allowedValuesToString();
}