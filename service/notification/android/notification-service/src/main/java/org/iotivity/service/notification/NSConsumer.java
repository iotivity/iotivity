package org.iotivity.service.notification;

import android.util.Log;

public class NSConsumer {
	
	String mId = null;
	String mAddress = null;
	
	public NSConsumer(String id) {
	    this.mId = id;
	}

	public String getId() {
	    return mId;
	}

	public String getAddress() {
	    return mAddress;
	}

	public void setAddress(String address) {
	    this.mAddress = address;
	}

}
