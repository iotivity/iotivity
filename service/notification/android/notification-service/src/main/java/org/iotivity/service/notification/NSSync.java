package org.iotivity.service.notification;

import android.util.Log;

public class NSSync {
	
	String mMessageId = null;
	String mDeviceId = null;
	
	public NSSync(String id) {
	    this.mMessageId = id;
	}

	public String getMessageId() {
	    return mMessageId;
	}

	public String getDeviceId() {
	    return mDeviceId;
	}

	public void setDeviceId(String id) {
	    this.mDeviceId = id;
	}

}
