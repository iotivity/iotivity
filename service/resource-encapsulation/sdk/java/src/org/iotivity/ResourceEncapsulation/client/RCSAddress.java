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
 * This file provides a class for a set of APIs relating to resource address and the
 * the type of discovery request for a resource of interest.
 */

package org.iotivity.ResourceEncapsulation.client;

import org.iotivity.ResourceEncapsulation.utils.DiscoveryType;

/**
 * This class provides a set of APIs for constructing RCSAddress object.
 *
 * <p>
 * RCSAddress object is the first parameter for Discover Resource APIs of
 * RCSDiscoveryManager Class.
 * <p>
 * {@link RCSDiscoveryManager}
 */
public class RCSAddress {

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

    private String            m_address;
    private DiscoveryType     m_typeOfDiscovery;
    private static RCSAddress m_rcsAddressInstance;

    private RCSAddress(String addr, DiscoveryType discoveryType) {
        m_address = addr;
        m_typeOfDiscovery = discoveryType;
    }

    /**
     * Constructs a RCSAddress object for multicast discovery.
     *
     * @return RCSAddress - Instance of RCSAddress.
     *
     *         {@link RCSAddress}
     */
    public static RCSAddress multicast() {
        m_rcsAddressInstance = new RCSAddress("", DiscoveryType.MULTICAST);
        return m_rcsAddressInstance;
    }

    /**
     * Constructs a RCSAddress object for setting unicast discovery.
     *
     * @param address
     *            required discovery address for the unicast resource discovery.
     *
     * @return RCSAddress -Instance of RCSAddress.
     *
     *         {@link RCSAddress}
     */
    public static RCSAddress unicast(String address) {
        m_rcsAddressInstance = new RCSAddress(address, DiscoveryType.UNICAST);
        return m_rcsAddressInstance;
    }

    // APIs used by DiscoveryManager class for getting the address information.

    /**
     * Get the discovery resource address in string format.
     *
     * @return String - Discovery address in string format.
     */
    public String getAddress() {
        return m_address;
    }

    /**
     * Get the resource discovery type(unicast or multicast).
     *
     * @return int - 0 for unicast and 1 for multicast.
     */
    public DiscoveryType getTypeOfDiscovery() {
        return m_typeOfDiscovery;
    }
}
