//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
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
