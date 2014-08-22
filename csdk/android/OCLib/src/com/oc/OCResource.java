//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
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

import java.net.URI;
import java.util.ArrayList;

/**
 * Static methods allow you to call to the OC stack, while non-static methods provide traversal
 * through the tree of hierarchical resources. Non-static methods also retrieve properties of the
 * current OCResource object.
 */
public class OCResource {
  private OCResource _parent = null;
  private ArrayList<OCResource> _children = null;
  private boolean _isContainer = false;

  public OCResource(OCResource parent, ArrayList<OCResource> children, boolean isContainer) {
    _parent = parent;
    _children = children;
    _isContainer = isContainer;
  }

  OCResource getParent() {
    return _parent;
  }

  ArrayList<OCResource> getChildren() {
    return _children;
  }

  /**
   * Recursively gets all the operable resources below this in the tree to a depth of 2.
   * 
   * @return Returns an ArrayList of operable OCResource objects below this OCResource's position in
   *         the tree.
   */
  ArrayList<OCResource> getResource() {
    return getResources(2);
  }

  /**
   * Recursively gets all the operable resources below this in the tree.
   * 
   * @param depth - The depth to traverse the tree downward.
   * @return Returns an ArrayList of operable OCResource objects below this OCResource's position in
   *         the tree.
   */
  ArrayList<OCResource> getResources(int depth) {
    if (depth - 1 <= 0) {
      return null;
    }
    ArrayList<OCResource> retRes = new ArrayList<OCResource>();
    if (_isContainer == true) { // If this is a resource container, add all the resources below me
                                // in my tree.
      for (OCResource child : getChildren()) {
        retRes.addAll(child.getResources(depth - 1));
      }
    } else {
      retRes.add(OCResource.this);
    }
    return retRes;
  }

  /**
   * Recursively gets all the container (i.e. inoperable) resources below this in the tree to a 
   * depth of 2.
   * 
   * @return Returns an ArrayList of inoperable OCResource objects below this OCResource's position
   *         in the tree.
   */
  ArrayList<OCResource> getResourceContainers() {
    return getResourceContainers(2);
  }

  /**
   * Recursively gets all the container (i.e. inoperable) resources below this in the tree.
   * 
   * @param depth - The depth to traverse the tree downward.
   * 
   * @return Returns an ArrayList of inoperable OCResource objects below this OCResource's position
   *         in the tree.
   */
  ArrayList<OCResource> getResourceContainers(int depth) {
    if (depth - 1 <= 0) {
      return null;
    }
    ArrayList<OCResource> retRes = new ArrayList<OCResource>();
    if (_isContainer == true) {
      for (OCResource child : getChildren()) {
        retRes.addAll(child.getResourceContainers(depth - 1));
      }
      retRes.add(OCResource.this);
    }
    return retRes;
  }
  /**
   * Recursively gets all the resources (operable & inoperable) resources below this in the tree
   * to a depth of 2.
   * 
   * @return Returns an ArrayList of operable & inoperable OCResource objects below this
   *         OCResource's position in the tree.
   */
  ArrayList<OCResource> getAllResources() {
    return getAllResources(2);
  }

  /**
   * Recursively gets all the resources (operable & inoperable) resources below this in the tree.
   * 
   * @param depth - The depth to traverse the tree downward.
   * 
   * @return Returns an ArrayList of operable & inoperable OCResource objects below this
   *         OCResource's position in the tree.
   */
  ArrayList<OCResource> getAllResources(int depth) {
    if (depth - 1 <= 0) {
      return null;
    }
    ArrayList<OCResource> retRes = new ArrayList<OCResource>();
    if (_isContainer == true) {
      for (OCResource child : getChildren()) {
        retRes.addAll(child.getAllResources(depth - 1));
      }
      retRes.add(OCResource.this);
    } else {
      retRes.add(OCResource.this);
    }
    return retRes;
  }

  /**
   * 
   * @return Returns the resource URI to address this Resource. You can determine the service that
   *         is hosting this resource by parsing this URI.
   */
  URI getURI() {
    return null;
  }

  /**
   * Returns the requested interface for this resource
   * 
   * @param interfaceClass - The interface that is desired.
   * 
   * @return The interface requested or NULL if not supported.
   */
  public <T> T getInterface(java.lang.Class<T> interfaceClass) {
    return null;
  }
}
