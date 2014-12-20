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

public abstract class SerializableDataFactory {

    private long nativeSerDataFac;

    private native long constructNative(SerializableDataFactory factory);

    private native void deleteNative(long serDatafactory);

    public SerializableDataFactory() {
        nativeSerDataFac = constructNative(this);
    }

    @Override
    protected void finalize() throws Throwable {
        this.destroy();
        super.finalize();
    }

    public void destroy() {
        if (nativeSerDataFac != 0) {
            this.deleteNative(nativeSerDataFac);
            nativeSerDataFac = 0;
        }
    }

    public void onNativeDelete() {
        nativeSerDataFac = 0;
    }

    public long getNativeHandle() {
        return nativeSerDataFac;
    }

    public long getRequestSerializable(int resourceType, String methodId) {
        OC.Cm.Serialization.ISerializable reqData = createRequestSerializable(
                resourceType, methodId);
        return (reqData != null) ? reqData.getNativeHandle() : 0;
    }

    public long getResponseSerializable(int resourceType, String methodId) {
        OC.Cm.Serialization.ISerializable reqData = createResponseSerializable(
                resourceType, methodId);
        return (reqData != null) ? reqData.getNativeHandle() : 0;
    }

    public long getSerializable(String elementType) {
        OC.Cm.Serialization.ISerializable reqData = createSerializable(elementType);
        return (reqData != null) ? reqData.getNativeHandle() : 0;
    }

    public abstract OC.Cm.Serialization.ISerializable createRequestSerializable(
            int resourceType, String methodId);

    public abstract OC.Cm.Serialization.ISerializable createResponseSerializable(
            int resourceType, String methodId);

    public abstract OC.Cm.Serialization.ISerializable createSerializable(
            String elementType);
}
