//******************************************************************
//
// Copyright 2014 MediaTek All Rights Reserved.
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

package org.iotivity.base;

public class OCResource extends NativeInstance {
    final private static String TAG = "OCResource";
    static {
        System.loadLibrary("ocstack-jni");
    }

    public native OCStackResult get(AbstractGetCallback attributeHandler);
//    public native OCStackResult get(AbstractGetCallback attributeHandler, PlatformConfig.QualityOfService QoS);

    public native OCStackResult get(String resourceType, String resourceInterface, AbstractGetCallback attributeHandler);
//    public native OCStackResult get(String resourceType, String resourceInterface, AbstractGetCallback attributeHandler, PlatformConfig.QualityOfService QoS);

    public native OCStackResult put(OCRepresentation rep, AbstractPutCallback attributeHandler);
//    public native OCStackResult put(OCRepresentation rep, AbstractPutCallback attributeHandler, PlatformConfig.QualityOfService QoS);
//    public native OCStackResult put(String resourceType, String resourceInterface, OCRepresentation rep, AbstractPutCallback attributeHandler);
//    public native OCStackResult put(String resourceType, String resourceInterface, OCRepresentation rep, AbstractPutCallback attributeHandler, PlatformConfig.QualityOfService QoS);

    public native OCStackResult post(OCRepresentation rep, AbstractPostCallback attributeHandler);
//    public native OCStackResult post(OCRepresentation rep, AbstractPostCallback attributeHandler, PlatformConfig.QualityOfService QoS);
//    public native OCStackResult post(String resourceType, String resourceInterface, OCRepresentation rep, AbstractPostCallback attributeHandler);
//    public native OCStackResult post(String resourceType, String resourceInterface, OCRepresentation rep, AbstractPostCallback attributeHandler, PlatformConfig.QualityOfService QoS);

    public native OCStackResult observe(int observeType, AbstractObserveCallback observerHandler);
//    public native OCStackResult observe(int observeType, AbstractObserveCallback observerHandler, PlatformConfig.QualityOfService QoS);

    public native OCStackResult cancelObserve();
//    public native OCStackResult cancelObserve(PlatformConfig.QualityOfService QoS);

    public native String uri();
    public native String host();

    public native String[] getResourceInterfaces();
    public native String[] getResourceTypes();

    public OCResource(long instPtr) {
        super.nativeHandle = instPtr;
    }
}
