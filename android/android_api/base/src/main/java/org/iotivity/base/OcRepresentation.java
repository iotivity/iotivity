/*
 * //******************************************************************
 * //
 * // Copyright 2015 Intel Corporation.
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

import java.security.InvalidParameterException;
import java.util.Arrays;
import java.util.List;

/**
 * TODO: OcRepresentation currently provides a set of very simplistic data access methods and needs
 * to be re-designed to allow more flexibility in data types
 */
public class OcRepresentation {

    static {
        System.loadLibrary("oc");
        System.loadLibrary("ocstack-jni");
    }

    public OcRepresentation() {
        create();
        //Native OCRepresentation object was created using "new" and needs to be deleted
        this.mNativeNeedsDelete = true;
    }

    private OcRepresentation(long nativeHandle) {
        this.mNativeHandle = nativeHandle;
        this.mNativeNeedsDelete = false;
    }

    private OcRepresentation(long nativeHandle, boolean nativeNeedsDelete) {
        this.mNativeHandle = nativeHandle;
        this.mNativeNeedsDelete = nativeNeedsDelete;
    }

    public native int getValueInt(String attributeKey);

    public native boolean getValueBool(String attributeKey);

    public native String getValueString(String attributeKey);

    public native void setValueInt(String attributeKey, int value);

    public native void setValueBool(String attributeKey, boolean value);

    public native void setValueString(String attributeKey, String value);

    public native String getJSONRepresentation();

    public native void addChild(OcRepresentation representation);

    public native void clearChildren();

    public List<OcRepresentation> getChildren() {
        return Arrays.asList(
                getChildrenArray());
    }

    private native OcRepresentation[] getChildrenArray();

    public native String getUri();

    public native void setUri(String uri);

    /**
     * Method to get the list of resource types
     *
     * @return List of resource types
     */
    public native List<String> getResourceTypes();

    /**
     * Method to set the list of resource types
     *
     * @param resourceTypeList list of resources
     */
    public void setResourceTypes(List<String> resourceTypeList) {
        if(null == resourceTypeList) {
            throw new InvalidParameterException("resourceTypeList cannot be null");
        }

        this.setResourceTypeArray(
                resourceTypeList.toArray(
                        new String[resourceTypeList.size()]));
    }

    private native void setResourceTypeArray(String[] resourceTypeArray);

    /**
     * Method to get the list of resource interfaces
     *
     * @return List of resource interface
     */
    public native List<String> getResourceInterfaces();

    /**
     * Method to set the list of resource interfaces
     *
     * @param resourceInterfaceList list of interfaces
     */
    public void setResourceInterfaces(List<String> resourceInterfaceList) {
        if(null == resourceInterfaceList) {
            throw new InvalidParameterException("resourceInterfaceList cannot be null");
        }

        this.setResourceInterfaceArray(
                resourceInterfaceList.toArray(
                        new String[resourceInterfaceList.size()]));
    }

    private native void setResourceInterfaceArray(String[] resourceInterfaceArray);

    public native boolean isEmpty();

    public native int size();

    public native boolean remove(String attributeKey);

    public native boolean hasAttribute(String attributeKey);

    public native void setNull(String attributeKey);

    public native boolean isNull(String attributeKey);

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
