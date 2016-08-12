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

import static org.iotivity.service.rc.test.helper.RCCommonUtil.*;

import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.iotivity.service.resourcecontainer.*;

import android.content.Context;
import android.content.res.AssetManager;
import android.util.Log;

public class RCHelper {
	private static final String LOG_TAG = "RCTest : RCHelper";
	private static RCHelper g_RCHelperInstance = null;

	public static final String s_LIB = "lib";
	public static final String s_VERSION = "Version";
	public static final String s_TEST_KEY = "testKey";

	public static final int n_VALUE = 30;
	public static final int n_ZERO = 0;
	public static final int n_MAX_WAIT = 2;
	public static final int n_MAX_TIMER = 1000;

	public static final String RCS_KEY = "testKey";
	public static final int RCS_VALUE = 30;
	public static final String ASSET_MANAGER_LIB = "lib";

	private RCHelper() {
	}

	public static RCHelper getInstance() {
		if (g_RCHelperInstance == null) {
			g_RCHelperInstance = new RCHelper();
		}

		return g_RCHelperInstance;
	}

	public boolean copyFilesToDevice(Context context) {
		boolean isSuccess = true;
		AssetManager assetManager = context.getAssets();
		String[] files = null;
		try {
			files = assetManager.list(s_LIB);
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

		Log.d(LOG_TAG, "copyIndividualFile");
		byte[] buffer = new byte[2048];
		int read;
		while ((read = in.read(buffer)) != -1) {
			out.write(buffer, n_ZERO, read);
		}
	}

	public void waitInSecond(int time) {
		int timeInSec = time * n_MAX_TIMER;
		try {
			Thread.sleep(timeInSec);
		} catch (InterruptedException ex) {
			Thread.currentThread().interrupt();
		}
	}

	public RcsBundleInfo getBundle(RcsResourceContainer container,
			String bundleID) {
		waitInSecond(n_MAX_WAIT);
		List<RcsBundleInfo> info = container.listBundles();
		for (int i = n_ZERO; i < info.size(); i++) {
			Log.i(LOG_TAG, "BundleID: " + info.get(i).getID());
			if (bundleID.equals(info.get(i).getID()))
				return info.get(i);
		}
		return null;
	}

	public boolean isBundleInAddedList(RcsResourceContainer container,
			String bundleID) {
		if (getBundle(container, bundleID) == null) {
			Log.i(LOG_TAG, "Bundle is not in Added Bundle List. The ID is "
					+ bundleID);
			return false;
		}
		return true;
	}

	public boolean startBundle(RcsResourceContainer container, String bundleID) {
		container.startBundle(bundleID);

		RcsBundleInfo info = getBundle(container, bundleID);

		if (info == null) {
			Log.i(LOG_TAG, "Bundle is not created. ID : " + bundleID);
			return false;
		}

		return true;
	}

	public boolean stopBundle(RcsResourceContainer container, String bundleID) {
		container.stopBundle(bundleID);

		return true;
	}

	public boolean removeBundle(RcsResourceContainer container, String bundleID) {
		try {
			container.removeBundle(bundleID);
		} catch (Exception e) {
			return false;
		}

		return true;
	}

	public boolean addResourceConfigVersion(RcsResourceContainer container,
			String bundleID, String value) {
		Map<String, String> params = new HashMap<String, String>();
		params.put(s_VERSION, value);
		container.addResourceConfig(bundleID, EMPTY_STRING, params);

		RcsBundleInfo info = getBundle(container, bundleID);
		if (info == null) {
			Log.i(LOG_TAG, "Bundle is not created. ID : " + bundleID);
			return false;
		}

		if (!info.getVersion().equals(value)) {
			Log.i(LOG_TAG, "Bundle version is not matched. ID : " + bundleID
					+ ", value : " + value);
			return false;
		}
		return true;
	}

	public String getFilesPath(Context context) {
		try {
			return context.getFilesDir().getPath();
		} catch (Exception e) {
			return EMPTY_STRING;
		}
	}

	/**
	 * Create BundleResource object reference and send back to the user.
	 * 
	 * @param context
	 *            Application context
	 * @return BundleResource class reference
	 **/
	public BundleResource getBundleResource(Context context) {
		BundleResource bundleResource = new BundleResource(context) {

			@Override
			protected void initAttributes() {
				this.m_name = DI_SENSOR_NAME;
				this.m_resourceType = DI_RESOURCE_TYPE;
				this.m_attributes.put(DI_SENSOR_KEY, n_ZERO);
				this.m_attributes.put(HUMIDITY_SENSOR_KEY, n_ZERO);
				this.m_attributes.put(TEMPERATURE_SENSOR_KEY, n_ZERO);
				this.m_attributes.put(LIGHT_SENSOR_KEY, n_ZERO);
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

	public RcsResourceAttributes getResourceAttributes() {
		RcsResourceAttributes resourceAttributes = new RcsResourceAttributes();

		resourceAttributes.put(s_TEST_KEY, n_VALUE);

		return resourceAttributes;
	}

}
