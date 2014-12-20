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

package OC.Cm.Serialization;

public class IDeserializer {

    private long nativeDeserializer;

    private native boolean parse(long deserializer, String buffer, int length)
            throws Exception;

    private native String getStringPropertyValue(long deserializer,
            String propertyName) throws Exception;

    private native boolean getBoolPropertyValue(long deserializer,
            String propertyName) throws Exception;

    private native boolean getGroupProperty(long deserializer, String nodeName)
            throws Exception;

    private native int getIntPropertyValue(long deserializer,
            String propertyValue) throws Exception;

    private native double getDoublePropertyValue(long deserializer,
            String propertyValue) throws Exception;

    private native boolean getIsArray(long deserializer, String propertyName)
            throws Exception;

    private native int getSize(long deserializer, String propertyName)
            throws Exception;

    private native boolean setCurrentProperty(long deserializer,
            String propertyName, int index) throws Exception;

    private native String getSchemaInstanceType(long deserializer)
            throws Exception;

    private native boolean hasGroupProperty(long deserializer, String property)
            throws Exception;

    private native boolean hasProperty(long deserializer, String property)
            throws Exception;

    private native boolean setCurrentArrayIndex(long nativeDeserializer,
            String string, int i);

    private native boolean isInFilteringMode(long nativeDeserializer,
            String string);

    private native boolean isRecursive(long nativeDeserializer);

    private native boolean validateMandatory(long nativeDeserializer);

    private native void setErrorMessage(long nativeDeserializer, String message);

    private native long getLongPropertyValue(long deserializer,
            String propertyValue) throws Exception;

    private native void setIDExcemption(long nativeDeserializer,
            boolean excemption);

    private native boolean getIDExcemption(long nativeDeserializer);

    private native boolean removeParameter(long nativeDeserializer,
            String paramName);

    public IDeserializer(long deserializer) {
        nativeDeserializer = deserializer;
    }

    public long getNativeHandle() {
        return nativeDeserializer;
    }

    public boolean parse(String buffer, int length) throws Exception {
        return this.parse(nativeDeserializer, buffer, length);
    }

    public String getStringPropertyValue(String propertyName) throws Exception {
        return this.getStringPropertyValue(nativeDeserializer, propertyName);
    }

    public boolean getBoolPropertyValue(String propertyName) throws Exception {
        return this.getBoolPropertyValue(nativeDeserializer, propertyName);
    }

    public boolean getGroupProperty(String nodeName) throws Exception {
        return this.getGroupProperty(nativeDeserializer, nodeName);
    }

    public int getIntPropertyValue(String propertyValue) throws Exception {
        return this.getIntPropertyValue(nativeDeserializer, propertyValue);
    }

    public double getDoublePropertyValue(String propertyValue) throws Exception {
        return this.getDoublePropertyValue(nativeDeserializer, propertyValue);
    }

    public boolean getIsArray(String propertyName) throws Exception {
        return this.getIsArray(nativeDeserializer, propertyName);
    }

    public int getSize(String propertyName) throws Exception {
        return this.getSize(nativeDeserializer, propertyName);
    }

    public boolean setCurrentProperty(String propertyName, int index)
            throws Exception {
        return this.setCurrentProperty(nativeDeserializer, propertyName, index);
    }

    public String getSchemaInstanceType() throws Exception {
        return this.getSchemaInstanceType(nativeDeserializer);
    }

    public boolean hasGroupProperty(String property) throws Exception {
        return this.hasGroupProperty(nativeDeserializer, property);
    }

    public boolean setCurrentArrayIndex(String string, int i) throws Exception {
        return this.setCurrentArrayIndex(nativeDeserializer, string, i);
    }

    public boolean hasProperty(String string) throws Exception {
        return this.hasProperty(nativeDeserializer, string);
    }

    public boolean isInFilteringMode(String string) {
        return this.isInFilteringMode(nativeDeserializer, string);
    }

    public boolean isRecursive() {
        return this.isRecursive(nativeDeserializer);
    }

    public boolean validateMandatory() {
        return this.validateMandatory(nativeDeserializer);
    }

    public float getfloatPropertyValue(String propertyName) throws Exception {
        return (float) this.getDoublePropertyValue(propertyName);
    }

    public long getLongPropertyValue(String propertyName) throws Exception {
        return this.getLongPropertyValue(nativeDeserializer, propertyName);
    }

    public int getShortPropertyValue(String propertyName) throws Exception {
        int value = this.getIntPropertyValue(nativeDeserializer, propertyName);

        if ((value < -32768) || (value > 32767)) {
            throw new Exception("value is out of range : " + propertyName);
        }

        return value;
    }

    public int getBytePropertyValue(String propertyName) throws Exception {
        int value = this.getIntPropertyValue(nativeDeserializer, propertyName);

        if ((value < -128) || (value > 127)) {
            throw new Exception("value is out of range : " + propertyName);
        }

        return value;
    }

    public long getNonNegativeInteger(String propertyName) throws Exception {
        long value = this
                .getLongPropertyValue(nativeDeserializer, propertyName);

        if (value < 0) {
            throw new Exception("value is out of range : " + propertyName);
        }

        return value;
    }

    public long getNonPositiveInteger(String propertyName) throws Exception {
        long value = this
                .getLongPropertyValue(nativeDeserializer, propertyName);

        if (value > 0) {
            throw new Exception("value is out of range : " + propertyName);
        }

        return value;
    }

    public long getNegativeInteger(String propertyName) throws Exception {
        long value = this
                .getLongPropertyValue(nativeDeserializer, propertyName);

        if (value < -1) {
            throw new Exception("value is out of range : " + propertyName);
        }

        return value;
    }

    public long getPositiveInteger(String propertyName) throws Exception {
        long value = this
                .getLongPropertyValue(nativeDeserializer, propertyName);

        if (value > 1) {
            throw new Exception("value is out of range : " + propertyName);
        }

        return value;
    }

    public long getUnsignedLong(String propertyName) throws Exception {
        long value = this
                .getLongPropertyValue(nativeDeserializer, propertyName);

        if (value < 0) {
            throw new Exception("value is out of range : " + propertyName);
        }

        return value;
    }

    public long getUnsignedInt(String propertyName) throws Exception {
        long value = this
                .getLongPropertyValue(nativeDeserializer, propertyName);

        if ((value < 0) || (value > 4294967295L)) {
            throw new Exception("value is out of range : " + propertyName);
        }

        return value;
    }

    public int getUnsignedShort(String propertyName) throws Exception {
        int value = this.getIntPropertyValue(nativeDeserializer, propertyName);

        if ((value < 0) || (value > 65535)) {
            throw new Exception("value is out of range : " + propertyName);
        }

        return value;
    }

    public int getUnsignedByte(String propertyName) throws Exception {
        int value = this.getIntPropertyValue(nativeDeserializer, propertyName);

        if ((value < 0) || (value > 255)) {
            throw new Exception("value is out of range : " + propertyName);
        }

        return value;
    }

    public void setErrorMessage(String message) {
        this.setErrorMessage(nativeDeserializer, message);
    }

    public void setIDExcemption(boolean excemption) {
        this.setIDExcemption(nativeDeserializer, excemption);
    }

    public boolean getIDExcemption() {
        return this.getIDExcemption(nativeDeserializer);
    }

    public boolean removeParameter(String paramName) {
        return this.removeParameter(nativeDeserializer, paramName);
    }
}
