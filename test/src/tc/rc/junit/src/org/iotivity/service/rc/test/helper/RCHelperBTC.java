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

package org.iotivity.service.rc.test.helper;

import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import org.iotivity.service.resourcecontainer.BundleResource;
import org.iotivity.service.resourcecontainer.RcsResourceAttributes;

import static org.iotivity.service.rc.test.helper.RCCommonUtilBTC.*;
import android.content.Context;
import android.content.res.AssetManager;
import android.util.Log;

public class RCHelperBTC {
	private final String LOG_TAG = "RCTest : "
			+ this.getClass().getSimpleName();
	private static RCHelperBTC g_RCHelperInstance = null;

	public static final String RCS_KEY = "testKey";
	public static final int RCS_VALUE = 30;
	public static final String ASSET_MANAGER_LIB = "lib";

	private RCHelperBTC() {
	}

	public static RCHelperBTC getInstance() {
		if (g_RCHelperInstance == null) {
			g_RCHelperInstance = new RCHelperBTC();
		}

		return g_RCHelperInstance;
	}

	/**
	 * Copy the files from assets/lib folder to the device.
	 * 
	 * @param context
	 *            Application context
	 * @return true if it can copy successfully, false otherwise
	 **/
	public boolean copyFilesToDevice(Context context) {
		boolean isSuccess = true;
		AssetManager assetManager = context.getAssets();
		String[] files = null;
		try {
			files = assetManager.list(ASSET_MANAGER_LIB);
		} catch (IOException e) {
			Log.e(LOG_TAG, e.getMessage());
			isSuccess = false;
		}

		for (String filename : files) {
			InputStream in = null;
			OutputStream out = null;
			try {
				in = assetManager.open("lib/" + filename);
				out = new FileOutputStream(context.getFilesDir().getParent()
						+ "/files/" + filename);
				copyIndividualFile(in, out);
				in.close();
				in = null;
				out.flush();
				out.close();
				out = null;
			} catch (Exception e) {
				Log.e(LOG_TAG, e.getMessage());
				isSuccess = false;
			}
		}

		return isSuccess;
	}

	private void copyIndividualFile(InputStream in, OutputStream out)
			throws IOException {

		Log.i(LOG_TAG, "copyIndividualFile");
		byte[] buffer = new byte[2048];
		int read;
		while ((read = in.read(buffer)) != -1) {
			out.write(buffer, 0, read);
		}
	}

	/**
	 * @param time
	 *            waited time in seconds
	 **/
	public void waitInSecond(int time) {
		int timeInSec = time * 1000;
		try {
			Thread.sleep(timeInSec);
		} catch (InterruptedException ex) {
			Thread.currentThread().interrupt();
		}
	}

	/**
	 * Create BundleResource object reference and send back to the user.
	 * 
	 * @param context
	 *            Application context
	 * @return BundleResource class reference
	 **/
	public static BundleResource getBundleResource(Context context) {
		BundleResource bundleResource = new BundleResource(context) {

			@Override
			protected void initAttributes() {
				this.m_name = "hueBundle";
				this.m_resourceType = "oic.r.light";
				this.m_uri = "data/data/org.iotivity.service.rc.utc/files/hue-0.1.jar";
				this.m_attributes.put(DI_SENSOR_KEY, 0);
				this.m_attributes.put(HUMIDITY_SENSOR_KEY, 0);
				this.m_attributes.put(TEMPERATURE_SENSOR_KEY, 0);
				this.m_attributes.put(LIGHT_SENSOR_KEY, 0);
			}

			@Override
			public void handleSetAttributesRequest(RcsResourceAttributes attrs) {
				this.setAttributes(attrs);
			}

			@Override
			public RcsResourceAttributes handleGetAttributesRequest() {
				return this.getAttributes();
			}

			@Override
			public void deactivateResource() {
			}
		};

		return bundleResource;
	}

	/**
	 * To get the files path of the application
	 * 
	 * @param context
	 *            Application context
	 * @return application file path
	 **/
	public String getFilesPath(Context context) {
		try {
			return context.getFilesDir().getPath();
		} catch (Exception e) {
			return "";
		}
	}

	/**
	 * To create and get the test attribute class
	 * 
	 * @return ResourceAttribute class reference
	 **/
	public static RcsResourceAttributes getResourceAttributes() {
		RcsResourceAttributes resourceAttributes = new RcsResourceAttributes();

		resourceAttributes.put(RCS_KEY, RCS_VALUE);

		return resourceAttributes;
	}
}
