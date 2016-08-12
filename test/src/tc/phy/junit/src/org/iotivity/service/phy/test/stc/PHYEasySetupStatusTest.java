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

package org.iotivity.service.phy.test.stc;

import android.test.AndroidTestCase;
import android.util.Log;

import java.io.IOException;

import org.iotivity.service.easysetup.mediator.EasySetupService;
import org.iotivity.service.easysetup.mediator.ESException;
import org.iotivity.service.easysetup.mediator.EasySetupStatus;
import org.iotivity.service.easysetup.mediator.EnrolleeDevice;
import org.iotivity.service.easysetup.mediator.EnrolleeSetupError;
import org.iotivity.service.easysetup.mediator.EnrolleeState;
import org.iotivity.service.easysetup.mediator.IpOnBoardingConnection;

import static org.iotivity.service.phy.test.helper.PHYEasySetupProcessHelper.*;
import org.iotivity.service.phy.test.helper.PHYEasySetupProcessHelper;
import org.iotivity.service.phy.test.helper.Utility;

public class PHYEasySetupStatusTest extends AndroidTestCase {

	EasySetupService mService;

	protected void setUp() throws Exception {
		super.setUp();
		IS_ONFINISHED_CALLED = false;
		IS_ONPROGRESS_CALLED = false;
	}

	protected void tearDown() throws Exception {
		super.tearDown();
		if (mService != null) {
			mService.finish();
			mService = null;
		}
		IS_ONFINISHED_CALLED = false;
		IS_ONPROGRESS_CALLED = false;
	}

	/**
	 * @throws IOException
	 * @since 2016-03-01
	 * @see public void setSSId(String ssid)
	 * @see public void setSharedKey(String sharedKey)
	 * @see public void setAuthAlgo(int aurthAlgo)
	 * @see public void setKms(int kms)
	 * @see public static EnrolleeDeviceFactory newInstance(Context context)
	 * @see public EnrolleeDevice newEnrolleeDevice(OnBoardingConfig
	 *      onboardingConfig, ProvisioningConfig provConfig)
	 * @see public static EasySetupService getInstance(EasySetupCallback
	 *      callback)
	 * @see public void finish()
	 * @objective test if mediator can be On Boarded enrollee with WiFi
	 *            connection type
	 * @target public void startSetup(EnrolleeDevice enrolledevice) throws
	 *         IOException
	 * @test_data EasySetup123 WifiConfiguration.AuthAlgorithm.OPEN
	 *            WifiConfiguration.KeyMgmt.WPA_PSK hub2.4G 11112222
	 * @pre_condition none
	 * @procedure 1. Create On boarding configuration 2. Create provisioning
	 *            configuration 3. Create enrolling device factory instance 4.
	 *            Create enrolling device 5. Create Easy Setup service and
	 *            implement callback method 6. Start setup 7. Check Enrollee
	 *            state
	 * @post_condition finish setup
	 * @expected Enrollee state will be DEVICE_ON_BOARDING_STATE
	 */
	public void testStopSetupOfOngoingEasySetupProcess_CCC_P() {
		final Object lock = new Object();
		EnrolleeDevice enrolleeDevice = getEnrolleeDevice(getContext());

		mService = EasySetupService.getInstance(getContext(),
				new EasySetupStatus() {
					@Override
					public void onFinished(EnrolleeDevice enrolleedevice) {
						Utility.toNotify(lock);
						IS_ONFINISHED_CALLED = true;
						Log.d("onFinished : ", "called");
						EnrolleeSetupError error = enrolleedevice.getError();
						System.out.println(error);
						Log.d("onFinished : ", "end");
					}

					@Override
					public void onProgress(EnrolleeDevice enrolleeDevice) {
						IS_ONPROGRESS_CALLED = true;
						EnrolleeState state = enrolleeDevice.getState();

						switch (state) {
						case DEVICE_INIT_STATE:
							assertTrue(true);
							Log.d("Enrollee state : ", "DEVICE_INIT_STATE");
							break;
						case DEVICE_ON_BOARDING_STATE:
							assertTrue(true);
							try {
								mService.stopSetup(enrolleeDevice);
							} catch (ESException e) {
								assertTrue(e.getMessage(), false);
							}
							System.out.println("State after stop  : "
									+ enrolleeDevice.getState());
							assertEquals(EnrolleeState.DEVICE_INIT_STATE,
									enrolleeDevice.getState());
							Log.d("Enrollee state : ",
									"DEVICE_ON_BOARDING_STATE");
							break;

						case DEVICE_ON_BOARDED_STATE:
							System.out
									.println("DEVICE_ON_BOARDED_STATE called  : "
											+ enrolleeDevice.getState());
							try {
								mService.stopSetup(enrolleeDevice);
							} catch (ESException e) {
								assertTrue(e.getMessage(), false);
							}
							System.out.println("State after stop  : "
									+ enrolleeDevice.getState());
							assertEquals(EnrolleeState.DEVICE_INIT_STATE,
									enrolleeDevice.getState());
							break;
						default:
							Log.d("Enrollee state : ", "UNKNOWN_STATE");
							break;
						}

					}
				});

		try {
			mService.startSetup(enrolleeDevice);
		} catch (IOException e) {
			assertTrue(e.getMessage(), false);
		} catch (ESException e) {
			assertTrue(e.getMessage(), false);
		}
		try {
			Utility.toWait(lock);
			assertTrue("onFinished callback not called", IS_ONFINISHED_CALLED);
			assertTrue("onProgress callback not called", IS_ONPROGRESS_CALLED);

		} catch (Exception e) {
			e.printStackTrace();
		}

	}

	/**
	 * @throws IOException
	 * @since 2016-03-01
	 * @see public void setSSId(String ssid)
	 * @see public void setSharedKey(String sharedKey)
	 * @see public void setAuthAlgo(int aurthAlgo)
	 * @see public void setKms(int kms)
	 * @see public static EnrolleeDeviceFactory newInstance(Context context)
	 * @see public EnrolleeDevice newEnrolleeDevice(OnBoardingConfig
	 *      onboardingConfig, ProvisioningConfig provConfig)
	 * @see public static EasySetupService getInstance(EasySetupCallback
	 *      callback)
	 * @see public void finish()
	 * @objective test if mediator get enrollee IP and Mac address
	 * @target java.lang.Object getConnection()
	 * @test_data EasySetup123 WifiConfiguration.AuthAlgorithm.OPEN
	 *            WifiConfiguration.KeyMgmt.WPA_PSK hub2.4G 11112222
	 * @pre_condition none
	 * @procedure 1. Create On boarding configuration 2. Create provisioning
	 *            configuration 3. Create enrolling device factory instance 4.
	 *            Create enrolling device 5. Create Easy Setup service and
	 *            implement callback method 6. Start setup 7. Check Enrollee IP
	 *            and Mac adress
	 * @post_condition finish setup
	 * @expected Enrollee IP and Mac Address
	 */
	public void testEnrolleeIPAndMac_CCC_P() {
		final Object lock = new Object();
		EnrolleeDevice enrolleeDevice = getEnrolleeDevice(getContext());
		EasySetupService mService;
		mService = EasySetupService.getInstance(getContext(),
				new EasySetupStatus() {
					@Override
					public void onFinished(EnrolleeDevice enrolledevice) {
						Utility.toNotify(lock);
					}

					@Override
					public void onProgress(EnrolleeDevice enrolleeDevice) {

					}
				});

		try {
			mService.startSetup(enrolleeDevice);
		} catch (IOException e) {
			assertTrue(e.getMessage(), false);
		} catch (ESException e) {
			assertTrue(e.getMessage(), false);
		}
		try {
			Utility.toWait(lock);
			Log.i("EasySetupTest", "Lock is released");
			IpOnBoardingConnection conn = (IpOnBoardingConnection) enrolleeDevice
					.getConnection();
			assertEquals("Fail to get enrollee IP address", "192.168.43.38",
					conn.getIp());
			assertEquals("Fail to get enrollee MAC address",
					"78:c4:0e:02:51:9d", conn.getHardwareAddress());
			Log.i("Enrollee", "Ip : " + conn.getIp());
			Log.i("Enrollee", "MAC : " + conn.getHardwareAddress());
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	/**
	 * @throws IOException
	 * @since 2016-03-01
	 * @see public void setSSId(String ssid)
	 * @see public void setSharedKey(String sharedKey)
	 * @see public void setAuthAlgo(int aurthAlgo)
	 * @see public void setKms(int kms)
	 * @see public static EnrolleeDeviceFactory newInstance(Context context)
	 * @see public EnrolleeDevice newEnrolleeDevice(OnBoardingConfig
	 *      onboardingConfig, ProvisioningConfig provConfig)
	 * @see public static EasySetupService getInstance(EasySetupCallback
	 *      callback)
	 * @see public void finish()
	 * @objective test if mediator can be On Boarded enrollee with WiFi
	 *            connection type
	 * @target public void startSetup(EnrolleeDevice enrolledevice) throws
	 *         IOException
	 * @test_data EasySetup123 WifiConfiguration.AuthAlgorithm.OPEN
	 *            WifiConfiguration.KeyMgmt.WPA_PSK hub2.4G 11112222
	 * @pre_condition none
	 * @procedure 1. Create On boarding configuration 2. Create provisioning
	 *            configuration 3. Create enrolling device factory instance 4.
	 *            Create enrolling device 5. Create Easy Setup service and
	 *            implement callback method 6. Start setup 7. Check Enrollee
	 *            state
	 * @post_condition finish setup
	 * @expected Mediator succesfully connects enrollee to enroller
	 */
	public void testWiFiOnOnBoardingAndProvisioningState_CCC_P() {
		final Object lock = new Object();
		EnrolleeDevice enrolleeDevice = getEnrolleeDevice(getContext());
		EasySetupService mService;
		mService = EasySetupService.getInstance(getContext(),
				new EasySetupStatus() {
					@Override
					public void onFinished(EnrolleeDevice enrolledevice) {
						Utility.toNotify(lock);
						IS_ONFINISHED_CALLED = true;
						assertTrue("Enrolle device easy setup unsuccessful ",
								enrolledevice.isSetupSuccessful());
						IpOnBoardingConnection conn = (IpOnBoardingConnection) enrolledevice
								.getConnection();
						Log.i("Enrollee", "Ip : " + conn.getIp());
						Log.i("Enrollee", "MAC : " + conn.getHardwareAddress());
						String ip = conn.getIp();
						String mac = conn.getHardwareAddress();
						assertNotNull("IP is null", ip);
						assertTrue("IP is empty ", !(ip.isEmpty()));
						assertNotNull("Mac is null", mac);
						assertTrue("Mac is empty ", !(mac.isEmpty()));
					}

					@Override
					public void onProgress(EnrolleeDevice enrolleeDevice) {
						IS_ONPROGRESS_CALLED = true;
						EnrolleeState state = enrolleeDevice.getState();
						switch (state) {
						case DEVICE_INIT_STATE:
							assertTrue(true);
							Log.d("Enrollee state", "DEVICE_INIT_STATE");
							break;

						case DEVICE_ON_BOARDING_STATE:
							assertTrue(true);
							Log.d("Enrollee state", "DEVICE_ON_BOARDING_STATE");
							break;

						case DEVICE_ON_BOARDED_STATE:
							assertTrue(true);
							Log.d("Enrollee state", "DEVICE_ON_BOARDED_STATE");
							break;

						case DEVICE_PROVISIONING_STATE:
							assertTrue(true);
							Log.d("enrollee state", "DEVICE_PROVISIONING_STATE");
							break;

						case DEVICE_PROVISIONED_STATE:
							assertTrue(true);
							Log.d("Enrollee state",
									"DEVICE_PROVISIONING_SUCCESS_STATE");
							break;

						default:
							Log.d("Enrollee state", "unknown state");
							assertTrue("Unknown state", false);
							break;
						}

					}
				});

		try {
			mService.startSetup(enrolleeDevice);
		} catch (IOException e) {
			assertTrue(e.getMessage(), false);
		} catch (ESException e) {
			assertTrue(e.getMessage(), false);
		}
		try {
			Utility.toWait(lock);
			assertTrue("onFinished callback not called", IS_ONFINISHED_CALLED);
			assertTrue("onProgress callback not called", IS_ONPROGRESS_CALLED);
			Log.i("EasySetupTest", "Lock is released");
		} catch (Exception e) {
			e.printStackTrace();
		}

	}

	/**
	 * @throws IOException
	 * @since 2016-03-01
	 * @see public void setSSId(String ssid)
	 * @see public void setSharedKey(String sharedKey)
	 * @see public void setAuthAlgo(int aurthAlgo)
	 * @see public void setKms(int kms)
	 * @see public static EnrolleeDeviceFactory newInstance(Context context)
	 * @see public EnrolleeDevice newEnrolleeDevice(OnBoardingConfig
	 *      onboardingConfig, ProvisioningConfig provConfig)
	 * @see public static EasySetupService getInstance(EasySetupCallback
	 *      callback)
	 * @see public void finish()
	 * @objective test if mediator can be On Boarded enrollee with WiFi
	 *            connection type
	 * @target public void startSetup(EnrolleeDevice enrolledevice) throws
	 *         IOException
	 * @test_data EasySetup123 WifiConfiguration.AuthAlgorithm.OPEN
	 *            WifiConfiguration.KeyMgmt.WPA_PSK hub2.4G 11112222
	 * @pre_condition none
	 * @procedure 1. Create On boarding configuration 2. Create provisioning
	 *            configuration 3. Create enrolling device factory instance 4.
	 *            Create enrolling device 5. Create Easy Setup service and
	 *            implement callback method 6. Start setup 7. Check Enrollee
	 *            state
	 * @post_condition finish setup
	 * @expected Enrollee state will be DEVICE_ON_BOARDING_STATE
	 */
	public void testWiFiOnOnBoardedState_CCC_P() {
		final Object lock = new Object();
		EnrolleeDevice enrolleeDevice = getEnrolleeDevice(getContext());

		mService = EasySetupService.getInstance(getContext(),
				new EasySetupStatus() {
					@Override
					public void onFinished(EnrolleeDevice enrolleedevice) {
						Utility.toNotify(lock);
						IS_ONFINISHED_CALLED = true;
						Log.d("onFinished : ", "called");
						EnrolleeSetupError error = enrolleedevice.getError();
						System.out.println(error);
						Log.d("onFinished : ", "end");
					}

					@Override
					public void onProgress(EnrolleeDevice enrolleeDevice) {

						IS_ONPROGRESS_CALLED = true;
						EnrolleeState state = enrolleeDevice.getState();

						switch (state) {
						case DEVICE_INIT_STATE:
							assertTrue(true);
							Log.d("Enrollee state : ", "DEVICE_INIT_STATE");
							break;
						case DEVICE_ON_BOARDING_STATE:
							assertTrue(true);
							Log.d("Enrollee state : ",
									"DEVICE_ON_BOARDING_STATE");
							break;

						case DEVICE_ON_BOARDED_STATE:
							assertTrue(true);
							Log.d("Enrollee state : ",
									"DEVICE_ON_BOARDED_STATE");
							EnrolleeSetupError error = enrolleeDevice
									.getError();
							System.out.println(error);
							Log.d("onFinished : ", "end");
							break;
						default:
							Log.d("Enrollee state : ", "UNKNOWN_STATE");
							break;
						}

					}
				});

		try {
			mService.startSetup(enrolleeDevice);
		} catch (IOException e) {
			assertTrue(e.getMessage(), false);
		} catch (ESException e) {
			assertTrue(e.getMessage(), false);
		}
		try {
			Utility.toWait(lock);
			assertTrue("onFinished callback not called", IS_ONFINISHED_CALLED);
			assertTrue("onProgress callback not called", IS_ONPROGRESS_CALLED);
		} catch (Exception e) {
			e.printStackTrace();
		}

	}

	/**
	 * @throws IOException
	 * @since 2016-03-01
	 * @see public void setSSId(String ssid)
	 * @see public void setSharedKey(String sharedKey)
	 * @see public void setAuthAlgo(int aurthAlgo)
	 * @see public void setKms(int kms)
	 * @see public static EnrolleeDeviceFactory newInstance(Context context)
	 * @see public EnrolleeDevice newEnrolleeDevice(OnBoardingConfig
	 *      onboardingConfig, ProvisioningConfig provConfig)
	 * @see public static EasySetupService getInstance(EasySetupCallback
	 *      callback)
	 * @see public void finish()
	 * @objective test if easy setup process send error when error occured
	 *            during setup process
	 * @target public void startSetup(EnrolleeDevice enrolledevice) throws
	 *         IOException
	 * @test_data EasySetup123 WifiConfiguration.AuthAlgorithm.OPEN
	 *            WifiConfiguration.KeyMgmt.WPA_PSK hub2.4G 11112222
	 * @pre_condition none
	 * @procedure 1. Create On boarding configuration 2. Create provisioning
	 *            configuration 3. Create enrolling device factory instance 4.
	 *            Create enrolling device 5. Create Easy Setup service and
	 *            implement callback method 6. Stop setup 7. Check Enrollee
	 *            setup state
	 * @post_condition finish setup
	 * @expected Easy setup process will be failed with error
	 */
	public void testStopSetupWithoutStartSetup_DSCC_N() {
		final Object lock = new Object();
		EnrolleeDevice enrolleeDevice = PHYEasySetupProcessHelper
				.getEnrolleeDevice(getContext());

		mService = EasySetupService.getInstance(getContext(),
				new EasySetupStatus() {

					@Override
					public void onFinished(EnrolleeDevice enrolleeDevice) {
						Utility.toNotify(lock);
					}

					@Override
					public void onProgress(EnrolleeDevice enrolleeDevice) {

					}

				});
		try {
			mService.stopSetup(enrolleeDevice);
		} catch (ESException e) {
			assertTrue(e.getMessage(), false);
		}
		try {
			Utility.toWait(lock);
			assertNull("Enrollee state should null", enrolleeDevice.getState());
		} catch (Exception e) {
			e.printStackTrace();
		}

	}

}
