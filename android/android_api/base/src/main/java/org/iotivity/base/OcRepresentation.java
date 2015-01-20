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

/*
* //******************************************************************
* //
* // Portions Copyright 2015 Intel Corporation.
* //
* //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
* //
* // Licensed under the Apache License, Version 2.0 (the "License");
* // you may not use this file except in compliance with the License.
* // You may obtain a copy of the License at
* //
* //      http://www.apache.org/licenses/LICENSE-2.0
* //
* // Unless required by applicable law or agreed to in writing, software
* // distributed under the License is distributed on an "AS IS" BASIS,
* // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* // See the License for the specific language governing permissions and
* // limitations under the License.
* //
* //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/

package org.iotivity.base;

/**
 * TODO: OcRepresentation currently provides a set of very simplistic data access methods and needs
 * to be re-designed to allow more flexibility in data types
 */
public class OcRepresentation {

    public OcRepresentation() {
        create();
        //Native OCRepresentation object was created using "new" and needs to be deleted
        this.mNativeNeedsDelete = true;
    }

    private OcRepresentation(long nativeHandle) {
        this.mNativeHandle = nativeHandle;
        this.mNativeNeedsDelete = false;
    }

    private OcRepresentation(long nativeHandle, boolean nativeNeedsDelete ) {
        this.mNativeHandle = nativeHandle;
        this.mNativeNeedsDelete = nativeNeedsDelete;
    }

    public native int getValueInt(String str);

    public native boolean getValueBool(String str);

    public native String getValueString(String str);

    public native void setValueInt(String str, int val);

    public native void setValueBool(String str, boolean val);

    public native void setValueString(String str, String val);

    public native String getUri();

    public native void setUri(String uri);

    public native boolean hasAttribute(String str);

    @Override
    protected void finalize() throws Throwable {
        super.finalize();

        dispose(this.mNativeNeedsDelete);
    }

    private native void create();

    private native void dispose(boolean needsDelete);

    private long mNativeHandle;
    private boolean mNativeNeedsDelete;
}
