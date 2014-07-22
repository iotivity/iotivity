//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

package com.oc.unittests;

import com.oc.OCObject;
import com.oc.OCObserver;

public class UnitTestResource implements OCObject, UnitTestControlItf {

  @Override
  public void setState(boolean state) {
    // TODO Auto-generated method stub

  }

  @Override
  public boolean isTrue() {
    // TODO Auto-generated method stub
    return false;
  }

  @Override
  public void addMyUnitTestObserver(OCObserver<Boolean> observer) {
    // TODO Auto-generated method stub

  }

  @Override
  public boolean isVisible() {
    // TODO Auto-generated method stub
    return false;
  }

}
