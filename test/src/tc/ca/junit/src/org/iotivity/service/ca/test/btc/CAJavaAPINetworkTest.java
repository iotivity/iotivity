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

package org.iotivity.service.ca.test.btc;

import android.app.Activity;
import android.content.Context;
import android.test.AndroidTestCase;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothAdapter;

import org.iotivity.ca.*;
import org.iotivity.ca.CaInterface.*;
import org.iotivity.service.ca.test.helper.*;
import org.iotivity.base.OcException;
import org.iotivity.base.OcConnectivityType;

import static org.iotivity.service.ca.test.helper.TestInfo.*;

public class CAJavaAPINetworkTest extends AndroidTestCase {

  private static Context  m_context  = null;
  private static Activity m_activity = null;
  private static final String BLE_ADDRESS = "E8:0A:E2:87:D5:06";
  private static final String DEVICE_MAC = "F4:09:D8:A4:16:53";

  CaInterface m_caInterface;

  public CAJavaAPINetworkTest() {
  }

  protected void setUp() throws Exception {
    super.setUp();
    m_context = getContext();
    m_activity = MainActivity.getActivity();
  }

  protected void tearDown() throws Exception {
    super.tearDown();
  }

  /**
   * @since 2018-08-09
   * @see void initialize(Activity activity, Context context)
   * @see void startManagerService(Context context,
   *      OnConnectionManagerStateListener onConnectionManagerStateListener)
   * @see void stopManagerService()
   * @objective Test 'startManagerService' positively to start connection
   *            manager service
   * @target void startManagerService(Context context,
   *         OnConnectionManagerStateListener onConnectionManagerStateListener)
   * @transport none
   * @test_data none
   * @pre_condition Do Initialize initialize(Activity activity, Context context)
   * @procedure Call the startManagerService
   * @post_condition Stop connection manager service using 'stopManagerService'
   *                 API
   * @expected It will start the connection manager service without any
   *           exception
   */
  public void testStartManagerService_SRC_P() {
    try {

      OnConnectionManagerStateListener m_onConnectionManagerStateListener = new OnConnectionManagerStateListenerImpl();
      m_caInterface.initialize(m_activity, m_context);
      m_caInterface.startManagerService(m_context,
          m_onConnectionManagerStateListener);
      m_caInterface.stopManagerService();
    } catch (Exception e) {
      fail();
    }
  }

  /**
   * @since 2018-08-09
   * @see void initialize(Activity activity, Context context)
   * @see void startManagerService(Context context,
   *      OnConnectionManagerStateListener onConnectionManagerStateListener)
   * @see void setAutoConnectionDevice(String address)
   * @see void unsetAutoConnectionDevice(String address)
   * @see void stopManagerService()
   * @objective Test 'setAutoConnectionDevice' positively to set device
   *            information for Auto-Connection
   * @target void setAutoConnectionDevice(String address)
   * @transport none
   * @test_data none
   * @pre_condition 1. Do Initialize initialize(Activity activity, Context
   *                context) 2. Call the startManagerService
   * @procedure 1. Call setAutoConnectionDevice API 2. Call
   *            unsetAutoConnectionDevice API
   * @post_condition Stop connection manager service using 'stopManagerService'
   *                 API
   * @expected It will set device information for Auto-Connection without any
   *           exception
   */
  public void testSetAutoConnectionDevice_SRC_P() {
    try {
      OnConnectionManagerStateListener onConnectionManagerStateListener = new OnConnectionManagerStateListenerImpl();

      m_caInterface.initialize(m_activity, m_context);
      m_caInterface.startManagerService(m_context,
          onConnectionManagerStateListener);

      m_caInterface.setAutoConnectionDevice(BLE_ADDRESS);
      m_caInterface.unsetAutoConnectionDevice(BLE_ADDRESS);

      m_caInterface.stopManagerService();
    } catch (OcException e) {
      fail();
    }
  }

  /**
   * @since 2018-08-09
   * @see void initialize(Activity activity, Context context)
   * @see void startManagerService(Context context,
   *      OnConnectionManagerStateListener onConnectionManagerStateListener)
   * @see void setAutoConnectionDevice(String address)
   * @see void unsetAutoConnectionDevice(String address)
   * @see void stopManagerService()
   * @objective Test 'setAutoConnectionDevice' negatively with empty string as
   *            address
   * @target void setAutoConnectionDevice(String address)
   * @transport none
   * @test_data Empty string as address
   * @pre_condition 1. Do Initialize initialize(Activity activity, Context
   *                context) 2. Call the startManagerService
   * @procedure 1. Call setAutoConnectionDevice API 2. Call
   *            unsetAutoConnectionDevice API
   * @post_condition Stop connection manager service using 'stopManagerService'
   *                 API
   * @expected It will not throw any Ocexception
   */
  public void testSetAutoConnectionDevice_ESV_N() {
    try {
      String leAddress = "";
      OnConnectionManagerStateListener onConnectionManagerStateListener = new OnConnectionManagerStateListenerImpl();

      m_caInterface.initialize(m_activity, m_context);
      m_caInterface.startManagerService(m_context,
          onConnectionManagerStateListener);

      m_caInterface.setAutoConnectionDevice(leAddress);

      m_caInterface.stopManagerService();
    } catch (OcException e) {
      fail();
    }
  }

  /**
   * @since 2018-08-09
   * @see void initialize(Activity activity, Context context)
   * @see void startManagerService(Context context,
   *      OnConnectionManagerStateListener onConnectionManagerStateListener)
   * @see void setAutoConnectionDevice(String address)
   * @see void unsetAutoConnectionDevice(String address)
   * @see void stopManagerService()
   * @objective Test 'setAutoConnectionDevice' negatively with NULL value as
   *            address
   * @target void setAutoConnectionDevice(String address)
   * @transport none
   * @test_data NULL as address
   * @pre_condition 1. Do Initialize initialize(Activity activity, Context
   *                context) 2. Call the startManagerService
   * @procedure 1. Call setAutoConnectionDevice API 2. Call
   *            unsetAutoConnectionDevice API
   * @post_condition Stop connection manager service using 'stopManagerService'
   *                 API
   * @expected It will not throw any Ocexception
   */
  public void testSetAutoConnectionDevice_NV_N() {
    try {
      String leAddress = null;
      OnConnectionManagerStateListener onConnectionManagerStateListener = new OnConnectionManagerStateListenerImpl();

      m_caInterface.initialize(m_activity, m_context);
      m_caInterface.startManagerService(m_context,
          onConnectionManagerStateListener);

      m_caInterface.setAutoConnectionDevice(leAddress);

      m_caInterface.stopManagerService();
    } catch (OcException e) {
      fail();
    }
  }

  /**
   * @since 2018-08-09
   * @see void initialize(Activity activity, Context context)
   * @see void startManagerService(Context context,
   *      OnConnectionManagerStateListener onConnectionManagerStateListener)
   * @see void setAutoConnectionDevice(String address)
   * @see void unsetAutoConnectionDevice(String address)
   * @see void stopManagerService()
   * @objective Test 'unsetAutoConnectionDevice' negatively with empty string
   *            value as address
   * @target void unsetAutoConnectionDevice(String address)
   * @transport none
   * @test_data Empty String as address
   * @pre_condition 1. Do Initialize initialize(Activity activity, Context
   *                context) 2. Call the startManagerService
   * @procedure 1. Call setAutoConnectionDevice API 2. Call
   *            unsetAutoConnectionDevice API
   * @post_condition Stop connection manager service using 'stopManagerService'
   *                 API
   * @expected It will not throw any Ocexception
   */
  public void testUnsetAutoConnectionDevice_ESV_N() {
    try {
      String leAddress = "";
      OnConnectionManagerStateListener onConnectionManagerStateListener = new OnConnectionManagerStateListenerImpl();

      m_caInterface.initialize(m_activity, m_context);
      m_caInterface.startManagerService(m_context,
          onConnectionManagerStateListener);

      m_caInterface.unsetAutoConnectionDevice(leAddress);

      m_caInterface.stopManagerService();
    } catch (OcException e) {
      fail();
    }
  }

  /**
   * @since 2018-08-09
   * @see void initialize(Activity activity, Context context)
   * @see void startManagerService(Context context,
   *      OnConnectionManagerStateListener onConnectionManagerStateListener)
   * @see void setAutoConnectionDevice(String address)
   * @see void unsetAutoConnectionDevice(String address)
   * @see void stopManagerService()
   * @objective Test 'unsetAutoConnectionDevice' negatively with NULL value as
   *            address
   * @target void unsetAutoConnectionDevice(String address)
   * @transport none
   * @test_data NULL as address
   * @pre_condition 1. Do Initialize initialize(Activity activity, Context
   *                context) 2. Call the startManagerService
   * @procedure 1. Call setAutoConnectionDevice API 2. Call
   *            unsetAutoConnectionDevice API
   * @post_condition Stop connection manager service using 'stopManagerService'
   *                 API
   * @expected It will not throw any Ocexception
   */
  public void testUnsetAutoConnectionDevice_NV_N() {
    try {
      String leAddress = null;
      OnConnectionManagerStateListener onConnectionManagerStateListener = new OnConnectionManagerStateListenerImpl();

      m_caInterface.initialize(m_activity, m_context);
      m_caInterface.startManagerService(m_context,
          onConnectionManagerStateListener);

      m_caInterface.unsetAutoConnectionDevice(leAddress);

      m_caInterface.stopManagerService();
    } catch (OcException e) {
      fail();
    }
  }

  /**
   * @since 2018-08-10
   * @see void startBtPairingService(Context context, OnBtDeviceFoundListener
   *      listener)
   * @see void void stopBtPairingService()
   * @objective Test 'startBtPairingService' positively to start bluetooth
   *            pairing service
   * @target void startBtPairingService(Context context, OnBtDeviceFoundListener
   *         listener)
   * @transport bt
   * @test_data none
   * @pre_condition none
   * @procedure Call startBtPairingService API
   * @post_condition Call 'stopBtPairingService' API to stop bluetooth pairing
   *                 service
   * @expected It will start bluetooth pairing service without any Ocexception
   */
  public void testStartBtPairingService_SRC_P() {
    try {
      OnBtDeviceFoundListener onBtDeviceFoundListener = new OnBtDeviceFoundListenerImpl();

      m_caInterface.startBtPairingService(m_context, onBtDeviceFoundListener);
      m_caInterface.stopBtPairingService();
    } catch (Exception e) {
      fail();
    }
  }

  /**
   * @since 2018-08-10
   * @see void startBtPairingService(Context context, OnBtDeviceFoundListener
   *      listener)
   * @see void startScan()
   * @see void stopScan()
   * @see void void stopBtPairingService()
   * @objective Test 'startScan' positively to start bluetooth device scan
   * @target void startScan()
   * @transport bt
   * @test_data none
   * @pre_condition Call startBtPairingService API
   * @procedure 1. Call startScan API 2. Call stopScan API
   * @post_condition Call 'stopBtPairingService' API to stop bluetooth pairing
   *                 service
   * @expected It will start bluetooth device scan without any Ocexception
   */
  public void testStartScan_SRC_P() {
    try {
      OnBtDeviceFoundListener onBtDeviceFoundListener = new OnBtDeviceFoundListenerImpl();

      m_caInterface.startBtPairingService(m_context, onBtDeviceFoundListener);
      m_caInterface.startScan();
      m_caInterface.stopScan();
      m_caInterface.stopBtPairingService();
    } catch (OcException e) {
      fail();
    }
  }

  /**
   * @since 2018-08-10
   * @see void setLeScanIntervalTime(int intervalTime, int workingCount)
   * @objective Test 'setLeScanIntervalTime' positively to set BLE scan interval
   *            time and working count
   * @target void startScan()
   * @transport ble
   * @test_data none
   * @pre_condition none
   * @procedure Call setLeScanIntervalTime API
   * @post_condition none
   * @expected It will set BLE scan interval time and working count without any
   *           Ocexception
   */
  public void testSetLeScanIntervalTime_SRC_P() {
    try {
      int intervalTime = 10000;
      int workingCount = 10;

      m_caInterface.setLeScanIntervalTime(intervalTime, workingCount);
    } catch (Exception e) {
      fail();
    }
  }

  /**
   * @since 2018-08-10
   * @see void setLeScanIntervalTime(int intervalTime, int workingCount)
   * @objective Test 'setLeScanIntervalTime' negatively as negative value in
   *            interval time
   * @target void startScan()
   * @transport ble
   * @test_data Negative value in interval time
   * @pre_condition none
   * @procedure Call setLeScanIntervalTime API
   * @post_condition none
   * @expected It will not throw exception
   */
  public void testSetLeScanIntervalTime_ETC_N() {
    try {
      int intervalTime = -1;
      int workingCount = 10;

      m_caInterface.setLeScanIntervalTime(intervalTime, workingCount);
    } catch (Exception e) {
      fail();
    }
  }

  /**
   * @since 2018-08-12
   * @see void startBtPairingService(Context context, OnBtDeviceFoundListener
   *      listener)
   * @see void createBond(BluetoothDevice device)
   * @see void void stopBtPairingService()
   * @objective Test 'createBond' positively to create bond
   * @target void createBond(BluetoothDevice device)
   * @transport bt
   * @test_data none
   * @pre_condition Call startBtPairingService API
   * @procedure Call createBond API
   * @post_condition Call 'stopBtPairingService' API to stop bluetooth pairing
   *                 service
   * @expected It will create bond without any Ocexception
   */
  public void testCreateBond_SRC_P() {
    try {
      BluetoothAdapter bleAdapter = BluetoothAdapter.getDefaultAdapter();
      BluetoothDevice bleDevice = bleAdapter.getRemoteDevice(DEVICE_MAC);

      OnBtDeviceFoundListener onBtDeviceFoundListener = new OnBtDeviceFoundListenerImpl();

      m_caInterface.startBtPairingService(m_context, onBtDeviceFoundListener);
      m_caInterface.createBond(bleDevice);
      m_caInterface.stopBtPairingService();
    } catch (OcException e) {
      fail();
    }
  }

  /**
   * @since 2018-08-12
   * @see void startBtPairingService(Context context, OnBtDeviceFoundListener
   *      listener)
   * @see void createBond(BluetoothDevice device)
   * @see void void stopBtPairingService()
   * @objective Test 'createBond' negatively with no device address
   * @target void createBond(BluetoothDevice device)
   * @transport bt
   * @test_data Empty string in device address
   * @pre_condition Call startBtPairingService API
   * @procedure Call createBond API
   * @post_condition Call 'stopBtPairingService' API to stop bluetooth pairing
   *                 service
   * @expected It will throw Ocexception
   */
  public void testCreateBond_NV_P() {
    try {
      OnBtDeviceFoundListener onBtDeviceFoundListener = new OnBtDeviceFoundListenerImpl();

      m_caInterface.startBtPairingService(m_context, onBtDeviceFoundListener);
      m_caInterface.createBond(null);
      m_caInterface.stopBtPairingService();
    } catch (OcException e) {
      fail();
    }
  }

  private class OnConnectionManagerStateListenerImpl implements
      OnConnectionManagerStateListener {
    public void onAdapterStateChanged(OcConnectivityType type, boolean enabled) {
    }

    public void onConnectionStateChanged(OcConnectivityType type,
        String address, boolean connected) {
    }
  }

  private class OnBtDeviceFoundListenerImpl implements OnBtDeviceFoundListener {
    public void onBtDeviceFound(BluetoothDevice device) {
    }
  }
}
