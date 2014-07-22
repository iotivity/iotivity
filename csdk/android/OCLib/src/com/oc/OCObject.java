//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

package com.oc;

/**
 * 
 * Represents a basic local resource to be offered to remote services.
 * 
 */
public interface OCObject {
  /**
   * Determines if the resource should be listed in the well-known core list.
   * 
   * NOTE: This may be better handled at a different location.
   * 
   * @return Returns true if resource should be listed.
   */
  boolean isVisible();
}
