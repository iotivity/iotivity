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
 * @file    Builder.java
 *
 *    This file contains Builder class which provides the easy creation of Resource.
 *
 */

package org.iotivity.ResourceEncapsulation.server;

import org.iotivity.ResourceEncapsulation.common.RCSException;
import org.iotivity.ResourceEncapsulation.common.RCSResourceAttributes;

/**
 * This class provides APIs for resource creation, setting properties and
 * attributes for the constructed resource. It provides the build() API which
 * builds a resource and returns RCSResourceObject.
 *
 * {@link RCSResourceObject}
 */
public class Builder {
    static {
        System.loadLibrary("gnustl_shared");
        System.loadLibrary("oc_logger");
        System.loadLibrary("connectivity_abstraction");
        System.loadLibrary("ca-interface");
        System.loadLibrary("octbstack");
        System.loadLibrary("oc");
        System.loadLibrary("ocstack-jni");
        System.loadLibrary("rcs_common");
        System.loadLibrary("rcs_client");
        System.loadLibrary("rcs_server");
        System.loadLibrary("rcs_container");
        System.loadLibrary("re-jni");
    }

    private String                m_resourceUri;
    private String                m_resourceType;
    private String                m_resourceInterface;
    private boolean               m_observableFlag;
    private boolean               m_discovervableFlag;
    private RCSResourceAttributes m_attributes;

    /**
     * Native method for creation of Resource
     */
    private native RCSResourceObject nativeBuild(String resourceURI,
            String resourceType, String resourceInterface,
            boolean observableFlag, boolean discovervableFlag,
            RCSResourceAttributes attributes);

    /**
     * Constructor
     *
     * @param resourceURI
     *            Resource URI value to be set
     * @param resourceType
     *            Resource type value to be set
     * @param resourceInterface
     *            Interface value to be set
     *
     */
    public Builder(String resourceURI, String resourceType,
            String resourceInterface) {
        m_resourceUri = resourceURI;
        m_resourceType = resourceType;
        m_resourceInterface = resourceInterface;
    }

    /**
     * Sets the discoverable(OC_DISCOVERABLE) property for the resource.
     *
     * @param isDiscoverable
     *            Whether to be discovered or not
     *
     */
    public void setDiscoverable(boolean isDiscoverable) {
        m_discovervableFlag = isDiscoverable;
    }

    /**
     * Sets the observable(OC_OBSERVABLE) property of the resource.
     *
     * @param isObservable
     *            Whether to be observed or not
     *
     */
    public void setObservable(boolean isObservable) {
        m_observableFlag = isObservable;
    }

    /**
     * API for setting attributes of the resource.
     *
     * @param attributes
     *            Resource Attributes to set
     *
     *            {@link RCSResourceAttributes}
     */
    public void setAttributes(RCSResourceAttributes attributes) {
        m_attributes = attributes;
    }

    /**
     * API for constructing a new RCSResourceObject.
     *
     * @return RCSResourceObject object (It is the resource that is created)
     *
     * @throws RCSException
     *             It catches exception from registerResource API of OCPlatform
     *             and throws it to developer.
     *
     *             <p>
     *             Steps for creation of Resource : - Get the object of this
     *             class by calling Constructor with resourceURI, resourceType,
     *             resourceInterface - Using the object set the Discoverable and
     *             Observable as needed. [optional] - Set the attributes for the
     *             resource. [optional] - At last call the build() method to
     *             create the resource.
     *
     */
    public RCSResourceObject build() throws RCSException {
        return this.nativeBuild(m_resourceUri, m_resourceType,
                m_resourceInterface, m_observableFlag, m_discovervableFlag,
                m_attributes);
    }
}
