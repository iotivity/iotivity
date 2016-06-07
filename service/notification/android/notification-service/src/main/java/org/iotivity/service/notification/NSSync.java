package org.iotivity.service.notification;

import android.util.Log;

public class NSSync {

	String mMessageId = null;
	String mDeviceId = null;
	String mSourceId = null;

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

	public String getSourceId() {
	    return mSourceId;
	}

	public void setSourceId(String id) {
	    this.mSourceId = id;
	}

}
