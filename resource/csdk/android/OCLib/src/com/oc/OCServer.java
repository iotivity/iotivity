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

/**
 * This represents a running open connection server.
 * 
 * WARNING: This may change to an interface so that we may transition to a factory pattern
 * 
 */
public class OCServer {
  private native void jniRegisterResource(OCObject object, String url);

  private native void jniUnregisterResource(OCObject object);

  private native void jniStartOCServer();

  private native void jniStopOCServer();

  private native OCDiscovery jniGetDiscoverySingleton();

  /**
   * Constructor for OCService.
   */
  public OCServer() {}

  /**
   * This registers a security model for access control to resources under this service.
   * 
   * @param model - Security Model required for access to this service and control/view of it's
   *        services.
   */
  public void setSecurityModel(OCSecurityModel model) {
    // Todo: Pat, What will we do here?
  }

  /**
   * Registers a resource with the service.
   * 
   * @param object - The OCObject that handles the resource requests
   * @param url - The URL under the resource.
   * @param accessControl - The access control handler that determines whether the remote device is
   *        allowed to access this resource. If NULL, all access is granted.
   */
  public void registerResource(OCObject object, String url/* TODO: , AccessControl accessControl */) {
    jniRegisterResource(object, url);
  }

  /**
   * Unregisters a resource with the service.
   * 
   * @param object - The OCObject to be unregistered.
   */
  public void unregisterResource(OCObject object) {
    jniUnregisterResource(object);
  }

  /**
   * Starts the open connectivity service.
   * 
   * Thread-Safety: TODO Determine if it is going to be safe to call in a UI thread
   */
  public void start() {
    jniStartOCServer();
  }

  /**
   * Stops the open connectivity service.
   * 
   * Thread-Safety: TODO Determine if it is going to be safe to call in a UI thread
   */
  public void stop() {
    jniStopOCServer();
  }

  /**
   * Retrieves access to discovery APIs via the Discovery Singleton delegated by the SDK. Delegation
   * is limited to just one singleton per OCServer object.
   * 
   * @return Returns an object which permits access to the open connectivity discovery APIs to just
   *         one discovery singleton object.
   * 
   * @throws //TODO throws an exception when the discovery singleton has already been checked out.
   */
  public OCDiscovery getDiscoverySingleton() {
    return jniGetDiscoverySingleton();
  }
}
