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

package org.iotivity.service.phy.test.helper;

import java.io.IOException;

import org.iotivity.service.easysetup.mediator.EasySetupService;
import org.iotivity.service.easysetup.mediator.ESException;
import org.iotivity.service.easysetup.mediator.EasySetupStatus;
import org.iotivity.service.easysetup.mediator.EnrolleeDevice;
import org.iotivity.service.easysetup.mediator.EnrolleeState;
import org.iotivity.service.easysetup.mediator.IpOnBoardingConnection;

import android.content.Context;
import android.os.Handler;
import android.os.Message;

public class PHYEasySetupServiceHelper {

	static EasySetupService easySetupService;
	private PHYEnrolleeDeviceWiFiOnboardingHelper phyEnrolleeDeviceWiFiOnboardingHelper;
	static EasySetupStatus easySetupStatus;
	private static boolean mRunningStatus = false;
	public static final EnrolleeDevice NULL_ENROLLEE_DEVICE = null;
	private Handler mHandler = new Handler();
	public static final int SUCCESS = 0;
	public static final int FAILED = 1;
	public static final int STATE_CHANGED = 2;
	public static String result = null;
	public static boolean isSuccess = false;
	public static boolean isgetIP = false;
	public static boolean isgetMac = false;
	public static EnrolleeState keepState;
	private Context context;
	final Object lock = new Object();

	public PHYEasySetupServiceHelper(Context context) {
		this.context = context;
		phyEnrolleeDeviceWiFiOnboardingHelper = new PHYEnrolleeDeviceWiFiOnboardingHelper(this.context);
	}

	public EasySetupService getInstance(Context context) {

		easySetupService = EasySetupService.getInstance(context, new EasySetupStatus() {

			@Override
			public void onFinished(final EnrolleeDevice enrolledevice) {
				Utility.toNotify(lock);

				if (enrolledevice.isSetupSuccessful()) {

					IpOnBoardingConnection conn = (IpOnBoardingConnection) enrolledevice.getConnection();
					String ip = conn.getIp();
					if (ip == null || ip.isEmpty()) {
						isgetIP = false;

					} else
						isgetIP = true;
					String mac = conn.getHardwareAddress();
					if (mac == null || mac.isEmpty()) {
						isgetMac = false;
						return;
					} else
						isgetMac = true;
				}

				isSuccess = true;

				if (enrolledevice.isSetupSuccessful()) {
					mHandler.sendEmptyMessage(SUCCESS);
					result = "Pass";
				} else {
					mHandler.sendEmptyMessage(FAILED);
					result = "Pass";
				}
			}

			@Override
			public void onProgress(EnrolleeDevice enrolleeDevice) {
				EnrolleeState state = enrolleeDevice.getState();
				switch (state) {
				case DEVICE_INIT_STATE:

					break;
				case DEVICE_ON_BOARDING_STATE:
					keepState = EnrolleeState.DEVICE_ON_BOARDING_STATE;
					break;

				case DEVICE_ON_BOARDED_STATE:
					keepState = EnrolleeState.DEVICE_ON_BOARDED_STATE;
					break;

				case DEVICE_PROVISIONING_STATE:
					keepState = EnrolleeState.DEVICE_PROVISIONING_STATE;

					break;

				case DEVICE_PROVISIONED_STATE:
					keepState = EnrolleeState.DEVICE_PROVISIONED_STATE;
					break;

				default:
					keepState = null;
					break;
				}
			}

		});

		return easySetupService;
	}

	public void startSetup(Context context) throws IOException, ESException {
		getInstance(context).startSetup(phyEnrolleeDeviceWiFiOnboardingHelper.getEnrolleeDevice());
		Utility.toWait(lock);
	}

	public void startSetup(EnrolleeDevice enrolleeDevice) throws ESException, IOException {
		getInstance(context).startSetup(enrolleeDevice);

	}

	public void stopSetup(Context context) throws ESException {
		getInstance(context).stopSetup(phyEnrolleeDeviceWiFiOnboardingHelper.getEnrolleeDevice());
	}

	class ThreadHandler extends Handler {
		@Override
		public void handleMessage(Message msg) {

			if (!mRunningStatus)
				return;

			switch (msg.what) {
			case SUCCESS: {
				result = "Pass";
				break;
			}
			case FAILED: {

				result = "Fail";
				break;
			}

			case STATE_CHANGED: {
				result = "Pass";
				break;
			}

			}

		}
	}

}
