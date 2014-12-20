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

public abstract class ISerializable {
    private long   nativeSerializable;
    private String instanceName;

    private native long constructNative();

    private native void deleteNative(long serializable);

    private native boolean serialize(long serializable, long serializer)
            throws Exception;

    private native boolean deSerialize(long serializable, long deserializer)
            throws Exception;

    private native boolean validateContent(long serializable);

    private native boolean setInstanceName(long serializable, String elementName);

    public abstract String getElementName();

    public abstract boolean serializeContents(
            OC.Cm.Serialization.ISerializer serializer);

    public abstract boolean deSerializeContents(
            OC.Cm.Serialization.IDeserializer deserializer);

    public native void setList(long serializable);

    @Override
    protected void finalize() throws Throwable {
        this.destroy();
        super.finalize();
    }

    public void destroy() {
        if (nativeSerializable != 0) {
            this.deleteNative(nativeSerializable);
            nativeSerializable = 0;
        }
    }

    public void onNativeDelete() {
        nativeSerializable = 0;
    }

    public long getNativeHandle() {
        return nativeSerializable;
    }

    public ISerializable() {
        nativeSerializable = constructNative();
    }

    public ISerializable(long serializable) {
        nativeSerializable = serializable;
    }

    public boolean serialize(OC.Cm.Serialization.ISerializer serializer)
            throws Exception {
        return this.serialize(nativeSerializable, serializer.getNativeHandle());
    }

    public boolean deSerialize(OC.Cm.Serialization.IDeserializer deserializer)
            throws Exception {
        return this.deSerialize(nativeSerializable,
                deserializer.getNativeHandle());
    }

    public boolean validateContent() throws Exception {
        return validateContent(nativeSerializable);
    }

    public boolean setInstanceName(String elementName) {
        instanceName = elementName;
        return setInstanceName(nativeSerializable, elementName);
    }

    public String getInstanceName() {
        return instanceName;
    }

    public void setList() {
        this.setList(nativeSerializable);
    }
}
