package org.iotivity.service.notification;

import android.util.Log;

/**
 * Created by jaesick.shin on 2016-04-26.
 */
public class NSMessage {
	
	String id = null;
	String title = null;
	String body = null;
	
	public NSMessage(String id) {
	    this.id = id;
	}

	public String getTitle() {
		return title;
	}

	public void setTitle(String title) {
		this.title = title;
	}

	public String getBody() {
		return body;
	}

	public void setBody(String body) {
		this.body = body;
	}

	public String getId() {
		return id;
	}
	
}
