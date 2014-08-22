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

import java.util.UUID;

/**
 * 
 * TODO: Consider including Attachment like java.nio.CompletionHandler
 * 
 * @param <T>
 */
public class OCObserver<T> implements Comparable<OCObserver<T>> {
  private static UUID observerId = UUID.randomUUID();

  /**
   * 
   * The OC service calls this at intervals n until the expired callback occurs.
   * 
   */
  public void onChanged(T data) {}

  /**
   * Called if a failure occurred. No more events will occur after this is callback.
   * 
   * @param throwable - The error that caused this callback sequence to fail.
   */
  public void onFailed(Throwable throwable) {}

  @Override
  public int compareTo(OCObserver<T> another) {
    return observerId.compareTo(another.getObserverId());
  }

  /**
   * 
   * @return observerId associated with this OCObserver object.
   */
  UUID getObserverId() {
    return observerId;
  }
}
