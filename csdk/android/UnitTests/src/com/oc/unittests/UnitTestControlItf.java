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
