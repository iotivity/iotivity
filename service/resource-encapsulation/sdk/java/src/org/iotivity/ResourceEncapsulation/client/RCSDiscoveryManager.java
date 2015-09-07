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
 * @file    RCSDiscoveryManager.java
 *
 * @brief    This file provides a class for a set of APIs relating to discovery of a particular
 *         resource in the network
 *
 */

package org.iotivity.ResourceEncapsulation.client;

import org.iotivity.ResourceEncapsulation.common.RCSException;
import org.iotivity.ResourceEncapsulation.utils.DiscoveryType;

import android.util.Log;

/**
 * This class contains the resource discovery method.
 *
 * {@link RCSRemoteResourceObject}
 */
public class RCSDiscoveryManager {

    private IDiscoverResourceListener  m_discoverListener;
    private static RCSDiscoveryManager m_rcsDiscoveryManagerInstacne;
    private final String               OC_RSRVD_WELL_KNOWN_URI = "/oic/res";
    private final String               LOG_TAG                 = this.getClass()
                                                                       .getSimpleName();

    private native void discoverResource(String address, int typeOfDiscovery,
            String relativeUri, String resourceType,
            IDiscoverResourceListener listener);

    /**
     * Constructor for RCSDiscoveryManager. Set listener to null on constructing
     * new RCSDiscoveryManager.
     */
    private RCSDiscoveryManager() {
    }

    /**
     * Returns RCSDiscoveryManager instance if already present else creates a
     * new instance of RCSDiscoveryManager.
     *
     * @return RCSDiscoveryManager - Instance of RCSDiscoveryManager.
     */
    public static RCSDiscoveryManager getInstance() {
        if (null == m_rcsDiscoveryManagerInstacne) {
            m_rcsDiscoveryManagerInstacne = new RCSDiscoveryManager();
        }
        return m_rcsDiscoveryManagerInstacne;
    }

    /**
     * Provides interface for receiving the callback for discovering a resource
     * of interest.
     */
    public interface IDiscoverResourceListener {

        /**
         * This callback method is called when a asynchronous response for the
         * discoverResource request is received.
         *
         * @param rcsRemoteResourceObject
         *            It is the resource for which discovery is requested..
         *
         *            {@link RCSDiscoveryManager}
         *            {@link RCSRemoteResourceObject}
         */
        public void onResourceDiscovered(
                RCSRemoteResourceObject rcsRemoteResourceObject);

    }

    /**
     * Set listener for receiving asynchronous response for resource discovery
     * request.
     *
     * @param listener
     *            setDiscoverResourceListener to receive the discovered
     *            resources.
     */
    public void setDiscoverResourceListener(IDiscoverResourceListener listener) {
        m_discoverListener = listener;
    }

    /**
     * API for discovering the resource of Interest, regardless of URI and
     * resource type
     *
     * @param address
     *            A RCSAddress object
     *
     *            {@link RCSAddress}
     */
    public void discoverResource(RCSAddress address) throws RCSException {

        Log.i(LOG_TAG, "discoverResource called");
        discoverResourceByType(address, OC_RSRVD_WELL_KNOWN_URI, "");
    }

    /**
     * API for discovering the resource of Interest, regardless of resource
     * type.
     *
     * @param address
     *            A RCSAddress object
     * @param relativeURI
     *            The relative uri of resource to be searched
     *
     *            {@link RCSAddress}
     */
    public void discoverResource(RCSAddress address, String relativeURI)
            throws RCSException {

        Log.i(LOG_TAG, "discoverResource called");
        discoverResourceByType(address, relativeURI, "");
    }

    /**
     * API for discovering the resource of Interest by resource type.
     *
     * @param address
     *            A RCSAddress object
     * @param resourceType
     *            Ressource Type
     *
     *            {@link RCSAddress}
     */
    public void discoverResourceByType(RCSAddress address, String resourceType)
            throws RCSException {

        Log.i(LOG_TAG, "discoverResource called");
        discoverResourceByType(address, OC_RSRVD_WELL_KNOWN_URI, resourceType);
    }

    /**
     * API for discovering the resource of Interest by Resource type with
     * provided relativeURI
     *
     * @param address
     *            A RCSAddress object
     * @param relativeURI
     *            The relative uri of resource to be searched
     * @param resourceType
     *            Ressource Type
     *
     *            {@link RCSAddress}
     */
    public void discoverResourceByType(RCSAddress address, String relativeURI,
            String resourceType) throws RCSException {

        Log.i(LOG_TAG, "discoverResource called");
        String addr = address.getAddress();
        DiscoveryType typeOfDiscovery = address.getTypeOfDiscovery();
        this.discoverResource(addr, typeOfDiscovery.ordinal(), relativeURI,
                resourceType, m_discoverListener);
    }

}
