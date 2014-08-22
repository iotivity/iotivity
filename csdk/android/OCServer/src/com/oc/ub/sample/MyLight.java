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
