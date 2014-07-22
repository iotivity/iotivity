//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

package com.oc.unittests;

import com.oc.OCObserver;
import com.oc.annotations.OCInterface;
import com.oc.annotations.OCProperty;

@OCInterface
public interface UnitTestControlItf {
  /**
   * APPLICATION NOTE: This is a synchronous request. It will throw immediately if the stack is down
   * or the network is otherwise not available. It may throw if the remote side does not responds or
   * returns an error.
   * 
   * @param state - The new state of this implemented interface.
   */
  @OCProperty(name = "UnitTestName")
  void setState(boolean state);

  /**
   * APPLICATION NOTE: This is a synchronous request. It will throw immediately if the stack is down
   * or the network is otherwise not available. It may throw is the remote side does not responds or
   * returns an error.
   * 
   * @return Returns state of this implemented interface.
   */
  @OCProperty(name = "UnitTestName")
  boolean isTrue();

  /**
   * APPLICATION NOTE: It is the objects responsibility to throw an exception if it cannot handle
   * any additional observers which will in turn return an error to the requestor.
   * 
   * @param observer the observer that will be notified when a change to the state property has
   *        occurred.
   */
  @OCProperty(name = "UnitTestName")
  void addMyUnitTestObserver(OCObserver<Boolean> observer);
}
