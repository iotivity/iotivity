//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
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

package com.oc;

import java.util.HashMap;

public class OCDiscovery {
  static private native void jniFindResource(String host, HashMap<String, String> filter,
      OCResourceResultHandler handler);

  /**
   * Internal API for Service and Resource Discovery
   * 
   * @param host - Host IP Address of a service to direct resource discovery query. If null or
   *        empty, performs service discovery OR resource discovery query to ALL group members.
   * @param filter<String FilterType, String Value> - Filter Type - Can be one of two of the link
   *        format equivalent attribute types declared in table 7 of the
   *        OC-Lite-Product-Specification-V1 Document. Either "n" (i.e. Resource Name) or "rt"
   *        (i.e. Resource Type). Value - Can be any valid String that's defined to work with CoRE
   *        Link Attributes as defined in the RFC6690.
   * @param handler - Handles callbacks, success states and failure states.
   * 
   *        Four modes of discovery defined as follows: 
   *        (NULL/Empty, NULL/Empty) - Performs ALL service discovery AND ALL resource discovery. 
   *        (NULL/Empty, Not Empty) - Performs query for a filtered/scoped/particular resource(s) 
   *                                  from ALL services. 
   *        (Not Empty, NULL/Empty) - Performs ALL resource discovery on a particular service. 
   *        (Not Empty, Not Empty) - Performs query for a filtered/scoped/particular resource(s) 
   *                                  from a particular service.
   * 
   *        Thread-Safety: TODO Determine if it is going to be safe to call in a UI thread
   */
  private void findResource(String host, HashMap<String, String> filter,
      OCResourceResultHandler handler) {
    /*** Have to make sure all values are valid so the pass down to JNI runs smoothly. ***/
    String tempHost = host;
    HashMap<String, String> tempFilter = filter;
    if (host == null) {
      tempHost = "";
    }
    if (filter == null) {
      tempFilter = new HashMap<String, String>();
      tempFilter.put("", ""); // TODO: NOt sure if putting an empty key is necessary if the hashmap is empty.
    }
    // Todo: Throw exception if handler is invalid (i.e. null).
    /*** Done ensuring all values are valid.  ***/
    jniFindResource(tempHost, tempFilter, handler);
  }

  /**
   * Queries the network for available resources.
   * 
   * NOTE: This gets posted to a new thread so it doesn't hold the UI or network thread.
   * 
   * @param serviceURL - The URL to the service. NULL for a broadcast/multicast.
   * @param resourceType - The resource type filter. NULL for match any.
   * 
   * @throws This may throw if the service is not started, network is bad, etc.
   * 
   *         Thread-Safety: TODO Determine if it is going to be safe to call in a UI thread
   */
  public void findResourceByType(String serviceURL, String resourceType,
      OCResourceResultHandler handler) {
    HashMap<String, String> filteredMap = new HashMap<String, String>();
    filteredMap.put("rt", resourceType);
    findResource(serviceURL, filteredMap, handler);
  }
}
