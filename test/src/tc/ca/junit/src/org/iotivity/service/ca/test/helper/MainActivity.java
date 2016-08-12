/******************************************************************
*
* Copyright 2016 Samsung Electronics All Rights Reserved.
*
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/

package org.iotivity.service.ca.test.helper;

import android.app.Application;
import android.content.Context;
import android.app.Activity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.app.Activity;

public class MainActivity extends Activity {

	private static Context context = null;
	private static Activity activity = null;

	public static RMInterface RM = new RMInterface();

	public void onCreate() {
		MainActivity.context = getApplicationContext();
		RM.setResponseListener(this);
	}

	public static Context getAppContext() {
		return MainActivity.context;
	}

	public static Activity getActivity() {
		if (activity == null) {
			activity = new MainActivity();
		}
		return activity;
	}

	public void OnResponseReceived(String subject, String receivedData) {
		String callbackData = subject + receivedData;
	}
}
