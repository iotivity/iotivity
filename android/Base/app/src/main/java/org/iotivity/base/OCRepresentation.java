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

public class OCRepresentation extends NativeInstance {
    final private static String TAG = "OCRepresentation";
    static {
        System.loadLibrary("ocstack-jni");
    }

    public OCRepresentation(long instPtr) {
        super.nativeHandle = instPtr;
    }
    public OCRepresentation() {
        super.nativeHandle = OCRepresentationConstructor();
    }

    public native long OCRepresentationConstructor();

    public native int getValueInt(String str);
    public native boolean getValueBool(String str);
    public native String getValueString(String str);

    public native void setValueInt (String str, int val);
    public native void setValueBool (String str, boolean val);
    public native void setValueString (String str, String val);

    public native String getUri();

    public native boolean hasAttribute(String str);
}
