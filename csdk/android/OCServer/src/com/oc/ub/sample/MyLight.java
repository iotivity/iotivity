//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

package com.oc.ub.sample;

import com.oc.OCObject;
import com.oc.OCObserver;

public class MyLight implements OCObject, LightControl {

	public MyLight() {
		// TODO Auto-generated constructor stub
	}

	@Override
	public boolean isVisible() {
		return true;
	}
	
	@Override
	public void setPowered(boolean powered) {
		// TODO Auto-generated method stub

	}

	@Override
	public boolean isPowered() {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public void setLevel(int level) {
		// TODO Auto-generated method stub

	}

	@Override
	public int getLevel() {
		// TODO Auto-generated method stub
		return 0;
	}

	@Override
	public void flash() {
		// TODO Auto-generated method stub

	}

    @Override
    public void addPoweredObserver(OCObserver<Boolean> observer) {
      // TODO Auto-generated method stub
      
    }

    @Override
    public void removePoweredObserver(OCObserver<Boolean> observer) {
      // TODO Auto-generated method stub
      
    }

}
