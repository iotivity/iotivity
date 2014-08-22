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

package com.oc.ub.sample;

import com.oc.OCObserver;
import com.oc.annotations.OCInterface;
import com.oc.annotations.OCProperty;

/**
 * APPLICATION NOTE: This interface is exposed to via the framework because of the @OCInterface
 * annotation. Without the annotation, this interface would be ignored by the framework.
 * 
 * APPLICATION NOTE: This interface needs to be accessible on the client and the server as the
 * contract for operation.
 * 
 */
@OCInterface
public interface LightControl {
  /**
   * APPLICATION NOTE: This is a synchronous request. It will throw immediately if the stack is down
   * or the network is otherwise not available. It may throw is the remote side does not responds or
   * returns an error.
   * 
   * @param powered - The new power state of this implemented interface.
   */
  @OCProperty(name = "powered")
  void setPowered(boolean powered);

  /**
   * APPLICATION NOTE: This is a synchronous request. It will throw immediately if the stack is down
   * or the network is otherwise not available. It may throw is the remote side does not responds or
   * returns an error.
   * 
   * @return Returns power state of this implemented interface.
   */
  @OCProperty(name = "powered")
  boolean isPowered();

  /**
   * APPLICATION NOTE: It is the objects responsibility to throw an exception (TBD) if it cannot
   * handle an additional observers which will in turn return an error to the requestor.
   * 
   * @param observer the observer that will be notified when a change to the power property has
   *        occurred.
   */
  @OCProperty(name = "powered")
  void addPoweredObserver(OCObserver<Boolean> observer);

  /**
   * APPLICATION NOTE: Even properties (or methods) that are not annotated are exposed, by default,
   * via the framework. Getter and setter methods are assumed to be properties unless annotated
   * otherwise.
   * 
   * @param level - Sets the power level of this implemented interface.
   */
  void setLevel(int level);

  int getLevel();

  /**
   * APPLICATION NOTE: This is an example of a method. The annotation is not required except to
   * override the defaults.
   */
  void flash();
}
