//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
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
