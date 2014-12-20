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

public class ISerializer {

    private long nativeSerializer;

    private native boolean setStringPropertyValue(long serializer,
            String propertyName, String value) throws Exception;

    private native boolean setBoolPropertyValue(long serializer,
            String propertyName, boolean value) throws Exception;

    private native boolean setIntPropertyValue(long serializer,
            String propertyName, int value) throws Exception;

    private native boolean setDoublePropertyValue(long serializer,
            String propertyName, double value) throws Exception;

    private native boolean startDocument(long serializer) throws Exception;

    private native boolean endDocument(long serializer) throws Exception;

    private native boolean startGroupProperty(long serializer, String nodeName)
            throws Exception;

    private native boolean endGroupProperty(long serializer, String nodeName)
            throws Exception;

    private native String getContentType(long serializer) throws Exception;

    private native boolean setSchemaInstanceType(long serializer, String type)
            throws Exception;

    private native boolean setLongPropertyValue(long serializer,
            String propertyName, long value) throws Exception;

    private native boolean isInFilteringMode(long serializer, String string);

    private native boolean isRecursive(long serializer);

    private native boolean validateMandatory(long serializer);

    private native void setErrorMessage(long nativeDeserializer, String message);

    private native boolean startListProperty(long nativeDeserializer,
            String nodeName);

    private native boolean endListProperty(long nativeDeserializer,
            String nodeName);

    private native void setIDExcemption(long nativeSeserializer,
            boolean excemption);

    private native boolean getIDExcemption(long nativeSeserializer);

    public ISerializer(long serializer) {
        nativeSerializer = serializer;
    }

    public long getNativeHandle() {
        return nativeSerializer;
    }

    public boolean setStringPropertyValue(String propertyName, String value)
            throws Exception {
        return this.setStringPropertyValue(nativeSerializer, propertyName,
                value);
    }

    public boolean setBoolPropertyValue(String propertyName, boolean value)
            throws Exception {
        return this.setBoolPropertyValue(nativeSerializer, propertyName, value);
    }

    public boolean setIntPropertyValue(String propertyName, int value)
            throws Exception {
        return this.setIntPropertyValue(nativeSerializer, propertyName, value);
    }

    public boolean setDoublePropertyValue(String propertyName, double value)
            throws Exception {
        return this.setDoublePropertyValue(nativeSerializer, propertyName,
                value);
    }

    public boolean setfloatPropertyValue(String propertyName, float value)
            throws Exception {
        return this.setDoublePropertyValue(nativeSerializer, propertyName,
                value);
    }

    public boolean setLongPropertyValue(String propertyName, long value)
            throws Exception {
        return this.setLongPropertyValue(nativeSerializer, propertyName, value);
    }

    public boolean setShortPropertyValue(String propertyName, int value)
            throws Exception {
        if ((value < -32768) || (value > 32767)) {
            return false;
        }
        return this.setIntPropertyValue(nativeSerializer, propertyName, value);
    }

    public boolean setBytePropertyValue(String propertyName, int value)
            throws Exception {
        if ((value < -128) || (value > 127)) {
            return false;
        }
        return this.setIntPropertyValue(nativeSerializer, propertyName, value);
    }

    public boolean setNonNegativeInteger(String propertyName, long value)
            throws Exception {
        if (value < 0) {
            return false;
        }
        return this.setLongPropertyValue(nativeSerializer, propertyName, value);
    }

    public boolean setNonPositiveInteger(String propertyName, long value)
            throws Exception {
        if (value > 0) {
            return false;
        }
        return this.setLongPropertyValue(nativeSerializer, propertyName, value);
    }

    public boolean setNegativeInteger(String propertyName, long value)
            throws Exception {
        if (value > -1) {
            return false;
        }
        return this.setLongPropertyValue(nativeSerializer, propertyName, value);
    }

    public boolean setPositiveInteger(String propertyName, long value)
            throws Exception {
        if (value < 1) {
            return false;
        }
        return this.setLongPropertyValue(nativeSerializer, propertyName, value);
    }

    public boolean setUnsignedLong(String propertyName, long value)
            throws Exception {
        if (value < 0) {
            return false;
        }
        return this.setLongPropertyValue(nativeSerializer, propertyName, value);
    }

    public boolean setUnsignedInt(String propertyName, long value)
            throws Exception {
        if ((value < 0) || (value > 4294967295L)) {
            return false;
        }
        return this.setLongPropertyValue(nativeSerializer, propertyName, value);
    }

    public boolean setUnsignedShort(String propertyName, int value)
            throws Exception {
        if ((value < 0) || (value > 65535)) {
            return false;
        }
        return this.setIntPropertyValue(nativeSerializer, propertyName, value);
    }

    public boolean setUnsignedByte(String propertyName, int value)
            throws Exception {
        if ((value < 0) || (value > 255)) {
            return false;
        }
        return this.setIntPropertyValue(nativeSerializer, propertyName, value);
    }

    public boolean isInFilteringMode(String string) {
        return this.isInFilteringMode(nativeSerializer, string);
    }

    public boolean isRecursive() {
        return this.isRecursive(nativeSerializer);
    }

    public boolean validateMandatory() {
        return this.validateMandatory(nativeSerializer);
    }

    public boolean startDocument() throws Exception {
        return this.startDocument(nativeSerializer);

    }

    public boolean endDocument() throws Exception {
        return this.endDocument(nativeSerializer);

    }

    public boolean startGroupProperty(String nodeName) throws Exception {
        return this.startGroupProperty(nativeSerializer, nodeName);

    }

    public boolean endGroupProperty(String nodeName) throws Exception {
        return this.endGroupProperty(nativeSerializer, nodeName);

    }

    String getContentType() throws Exception {
        return this.getContentType(nativeSerializer);

    }

    boolean setSchemaInstanceType(String type) throws Exception {
        return setSchemaInstanceType(nativeSerializer, type);

    }

    public void setErrorMessage(String message) {
        this.setErrorMessage(nativeSerializer, message);
    }

    public boolean startListProperty(String nodeName) {
        return this.startListProperty(nativeSerializer, nodeName);
    }

    public boolean endListProperty(String nodeName) {
        return this.endListProperty(nativeSerializer, nodeName);
    }

    public void setIDExcemption(boolean excemption) {
        this.setIDExcemption(nativeSerializer, excemption);
    }

    public boolean getIDExcemption() {
        return this.getIDExcemption(nativeSerializer);
    }
}
