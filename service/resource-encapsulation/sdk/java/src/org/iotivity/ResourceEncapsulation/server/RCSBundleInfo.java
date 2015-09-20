/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 *
 *
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
 *
 ******************************************************************/
/**
 * @file
 * This file contains RCSBundleInfo class, which provides APIs related to Bundle information.
 */

package org.iotivity.ResourceEncapsulation.server;

/**
 * This class provides APIs for getting and setting the Bundle Information
 */
public class RCSBundleInfo {

    private long    m_nativeHandle;
    private boolean m_nativeNeedsDelete;

    // native methods
    private native String nativeGetID();

    private native String nativeGetPath();

    private native String nativeGetActivatorName();

    private native String nativeGetLibraryPath();

    private native String nativeGetVersion();

    private native void nativeSetID(String id);

    private native void nativeSetPath(String path);

    private native void nativeSetActivatorName(String activatorName);

    private native void nativeSetLibraryPath(String libraryPath);

    private native void nativeSetVersion(String version);

    private RCSBundleInfo(long nativeHandle) {
        this.m_nativeHandle = nativeHandle;
    }

    /**
     * API for getting the Id of the bundle
     *
     * @return string - Id of the bundle
     */
    public String getID() {
        return this.nativeGetID();
    }

    /**
     * API for getting the path of the bundle
     *
     * @return path - path of the bundle
     */
    public String getPath() {
        return this.nativeGetPath();
    }

    /**
     * API for setting the Activator name for the bundle
     *
     * @return string - Name of the activator
     */
    public String getActivatorName() {
        return nativeGetActivatorName();
    }

    /**
     * API for getting the library path for the bundle
     *
     * @return string - Library path in string form
     */
    public String getLibraryPath() {
        return nativeGetLibraryPath();
    }

    /**
     * API for getting the version of the bundle
     *
     * @return string - version of the bundle
     */
    public String getVersion() {
        return nativeGetVersion();
    }

    /**
     * API for setting the Id of the bundle
     *
     * @param id
     *            - Id of the bundle in string form
     */
    public void setID(String id) {
        this.nativeSetID(id);
    }

    /**
     * API for setting the path of the bundle
     *
     * @param path
     *            - path of the bundle in string form
     */
    public void setPath(String path) {
        this.nativeSetPath(path);
    }

    /**
     * API for setting the Activator name for the bundle
     *
     * @param activatorName
     *            - Activator name in string form
     */
    public void setActivatorName(String activatorName) {
        this.nativeSetActivatorName(activatorName);
    }

    /**
     * API for setting the library path for the bundle
     *
     * @param libraryPath
     *            - Library path in string form
     */
    public void setLibraryPath(String libraryPath) {
        this.nativeSetLibraryPath(libraryPath);
    }

    /**
     * API for setting the version of the bundle
     *
     * @param version
     *            - version of the bundle in string form
     */
    public void setVersion(String version) {
        this.nativeSetVersion(version);
    }
}
