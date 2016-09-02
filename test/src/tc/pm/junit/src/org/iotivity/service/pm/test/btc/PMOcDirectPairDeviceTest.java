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
 *      LICENSE-2.0" target="_blank">http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 ******************************************************************/

package org.iotivity.service.pm.test.btc;

import java.io.File;
import android.test.InstrumentationTestCase;
import org.iotivity.base.ModeType;
import org.iotivity.base.OcDirectPairDevice;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcPrmType;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;
import org.iotivity.service.pm.test.helper.OCPlatformHelper;
import org.iotivity.service.pm.test.helper.OcDirectPairDeviceHelper;
import org.iotivity.service.pm.test.helper.PMConstants;

public class PMOcDirectPairDeviceTest extends InstrumentationTestCase {
  public OcDirectPairDeviceHelper mOcDirectPairDeviceHelper = new OcDirectPairDeviceHelper();
  public OCPlatformHelper         mOCPlatformHelper         = new OCPlatformHelper();

  protected void setUp() throws Exception {
    super.setUp();

    PMConstants.mErrorMessage = PMConstants.EMPTY_STRING;

    // create platform config
    mOCPlatformHelper.copyCborFromAsset(getInstrumentation().getContext(),
        PMConstants.OIC_DP_CLIENT_CBOR_DB_FILE);
    OCPlatformHelper.mFilePath = getInstrumentation().getContext()
        .getFilesDir().getPath()
        + "/"; // data/data/<package>/files/
    PlatformConfig cfg = new PlatformConfig(getInstrumentation()
        .getTargetContext(), ServiceType.IN_PROC, ModeType.CLIENT_SERVER,
        "0.0.0.0", // bind to all available
        // interfaces
        0, QualityOfService.LOW, OcDirectPairDeviceHelper.mFilePath
            + PMConstants.OIC_DP_CLIENT_CBOR_DB_FILE);

    OcPlatform.Configure(cfg);

    OCPlatformHelper.mSqlDbPath = getInstrumentation().getContext()
        .getFilesDir().getAbsolutePath()
        .replace(PMConstants.DIR_NAME_FILE, PMConstants.DIR_NAME_DATABASES)
        + File.separator;

    mOCPlatformHelper.initOICStack(OcDirectPairDeviceHelper.mSqlDbPath,
        PMConstants.OIC_SQL_DB_FILE);
  }

  protected void tearDown() throws Exception {
    super.tearDown();
  }

  /**
   * @since 2016-08-22
   * @see None
   * @objective Find all devices which are eligible for direct pairing and
   *            return the list
   * @target public static native void findDirectPairingDevices(int timeout,
   *         FindDirectPairingListener onFindDirectPairingListener) throws
   *         OcException
   * @test_data timeout = 10
   * @pre_condition Start Two JustWork Simulators and Initialize provisioning
   *                manager
   * @procedure Call findDirectPairingDevices() API
   * @post_condition none
   * @expected Should be called successfully
   */
  public void testFindDirectPairingDevices_SRC_RV_P() {
    assertTrue(PMConstants.mErrorMessage,
        mOCPlatformHelper.findDirectPairingDevices(
            PMConstants.DISCOVERY_TIMEOUT_05,
            OCPlatformHelper.finddirectPairing));
  }

  /**
   * @since 2016-08-23
   * @see None
   * @objective Find all devices which are eligible for direct pairing and
   *            return the list
   * @target public static native void findDirectPairingDevices(int timeout,
   *         FindDirectPairingListener onFindDirectPairingListener) throws
   *         OcException
   * @test_data timeout = 1
   * @pre_condition Start Two JustWork Simulators and Initialize provisioning
   *                manager
   * @procedure Call findDirectPairingDevices() API
   * @post_condition none
   * @expected Should be called successfully
   */
  public void testFindDirectPairingDevices_LBV_P() {
    assertTrue(PMConstants.mErrorMessage,
        mOCPlatformHelper.findDirectPairingDevices(
            PMConstants.DISCOVERY_TIMEOUT_ONE,
            OCPlatformHelper.finddirectPairing));
  }

  /**
   * @since 2016-08-23
   * @see None
   * @objective Find all devices which are eligible for direct pairing and
   *            return the list with timeout equal to 0 second
   * @target public static native void findDirectPairingDevices(int timeout,
   *         FindDirectPairingListener onFindDirectPairingListener) throws
   *         OcException
   * @test_data timeout = 0
   * @pre_condition Start Two JustWork Simulators and Initialize provisioning
   *                manager
   * @procedure Call findDirectPairingDevices() API
   * @post_condition none
   * @expected Should throw INVALID_PARAM exception
   */
  public void testFindDirectPairingDevices_LOBV_N() {
    assertFalse(PMConstants.mErrorMessage,
        mOCPlatformHelper.findDirectPairingDevices(
            PMConstants.DISCOVERY_TIMEOUT_ZERO,
            OCPlatformHelper.finddirectPairing));
    assertTrue(PMConstants.WRONG_ERROR_CODE,
        PMConstants.mErrorMessage.contains(PMConstants.ERROR_INVALID_PARAM));
  }

  /**
   * @since 2016-08-23
   * @see None
   * @objective Find all devices which are eligible for direct pairing and
   *            return the list with timeout equal to 0 second
   * @target public static native void findDirectPairingDevices(int timeout,
   *         FindDirectPairingListener onFindDirectPairingListener) throws
   *         OcException
   * @test_data timeout = 0
   * @pre_condition Start Two JustWork Simulators and Initialize provisioning
   *                manager
   * @procedure Call findDirectPairingDevices() API
   * @post_condition none
   * @expected Should throw INVALID_CALLBACK exception
   */
  public void testFindDirectPairingDevicesCB_NV_N() {
    assertFalse(PMConstants.mErrorMessage,
        mOCPlatformHelper.findDirectPairingDevices(
            PMConstants.DISCOVERY_TIMEOUT_ZERO,
            OCPlatformHelper.finddirectPairing));
    assertTrue(PMConstants.WRONG_ERROR_CODE,
        PMConstants.mErrorMessage.contains(PMConstants.ERROR_INVALID_CALLBACK));
  }

  /**
   * @since 2016-08-22
   * @see public static native void findDirectPairingDevices(int timeout,
   *      FindDirectPairingListener onFindDirectPairingListener) throws
   *      OcException
   * @objective Perform direct pairing between two devices.
   * @target public static void doDirectPairing(OcDirectPairDevice peer,
   *         OcPrmType prmType, String pin, DirectPairingListener
   *         onDirectPairingListener) throws OcException
   * @test_data prmType = OcPrmType.DP_PRE_CONFIGURED and pin = "00000000"
   * @pre_condition Start Two JustWork Simulators and Initialize provisioning
   *                manager
   * @procedure Call doDirectPairing() API
   * @post_condition none
   * @expected Should be called successfully
   */
  public void testDoDirectPairing_SRC_RV_P() {
    assertTrue(PMConstants.mErrorMessage,
        mOCPlatformHelper.findDirectPairingDevices(
            PMConstants.DISCOVERY_TIMEOUT_ZERO,
            OCPlatformHelper.finddirectPairing));

    OcDirectPairDevice dicDirectPairDev = OcDirectPairDeviceHelper.discoveredListObj
        .get(PMConstants.DEVICE_INDEX_ZERO);

    assertTrue(PMConstants.mErrorMessage, mOCPlatformHelper.doDirectPairing(
        dicDirectPairDev, OcPrmType.DP_PRE_CONFIGURED,
        PMConstants.DP_PRE_CONFIGURED_PIN, OCPlatformHelper.doDPListener));
  }

  /**
   * @since 2016-08-23
   * @see public static native void findDirectPairingDevices(int timeout,
   *      FindDirectPairingListener onFindDirectPairingListener) throws
   *      OcException
   * @objective Perform direct pairing between two devices with
   *            onDirectPairingListener as null
   * @target public static void doDirectPairing(OcDirectPairDevice peer,
   *         OcPrmType prmType, String pin, DirectPairingListener
   *         onDirectPairingListener) throws OcException
   * @test_data onDirectPairingListener = null
   * @pre_condition Start Two JustWork Simulators and Initialize provisioning
   *                manager
   * @procedure Call doDirectPairing() API
   * @post_condition none
   * @expected Should throw INVALID_CALLBACK exception
   */
  public void testDoDirectPairingCB_NV_N() {
    assertTrue(PMConstants.mErrorMessage,
        mOCPlatformHelper.findDirectPairingDevices(
            PMConstants.DISCOVERY_TIMEOUT_ZERO,
            OCPlatformHelper.finddirectPairing));

    OcDirectPairDevice dicDirectPairDev = OcDirectPairDeviceHelper.discoveredListObj
        .get(PMConstants.DEVICE_INDEX_ZERO);

    assertFalse(PMConstants.mErrorMessage, mOCPlatformHelper.doDirectPairing(
        dicDirectPairDev, OcPrmType.DP_PRE_CONFIGURED,
        PMConstants.DP_PRE_CONFIGURED_PIN, null));
    assertTrue(PMConstants.WRONG_ERROR_CODE,
        PMConstants.mErrorMessage.contains(PMConstants.ERROR_INVALID_CALLBACK));
  }

  /**
   * @since 2016-08-23
   * @see public static native void findDirectPairingDevices(int timeout,
   *      FindDirectPairingListener onFindDirectPairingListener) throws
   *      OcException
   * @objective Perform direct pairing negatively with peer as null
   * @target public static void doDirectPairing(OcDirectPairDevice peer,
   *         OcPrmType prmType, String pin, DirectPairingListener
   *         onDirectPairingListener) throws OcException
   * @test_data peer = null
   * @pre_condition Start Two JustWork Simulators and Initialize provisioning
   *                manager
   * @procedure Call doDirectPairing() API
   * @post_condition none
   * @expected Should throw INVALID_CALLBACK exception
   */
  public void testDoDirectPairingPeer_NV_N() {
    assertTrue(PMConstants.mErrorMessage,
        mOCPlatformHelper.findDirectPairingDevices(
            PMConstants.DISCOVERY_TIMEOUT_ZERO,
            OCPlatformHelper.finddirectPairing));

    assertFalse(PMConstants.mErrorMessage, mOCPlatformHelper.doDirectPairing(
        null, OcPrmType.DP_PRE_CONFIGURED, PMConstants.DP_PRE_CONFIGURED_PIN,
        OCPlatformHelper.doDPListener));

    assertTrue(PMConstants.WRONG_ERROR_CODE,
        PMConstants.mErrorMessage.contains(PMConstants.ERROR_INVALID_PARAM));
  }

  /**
   * @since 2016-08-23
   * @see public static native void findDirectPairingDevices(int timeout,
   *      FindDirectPairingListener onFindDirectPairingListener) throws
   *      OcException
   * @objective Perform direct pairing negatively with pin as null
   * @target public static void doDirectPairing(OcDirectPairDevice peer,
   *         OcPrmType prmType, String pin, DirectPairingListener
   *         onDirectPairingListener) throws OcException
   * @test_data pin = null
   * @pre_condition Start Two JustWork Simulators and Initialize provisioning
   *                manager
   * @procedure Call doDirectPairing() API
   * @post_condition none
   * @expected Should throw INVALID_PARAM exception
   */
  public void testDoDirectPairingPin_NV_N() {
    assertTrue(PMConstants.mErrorMessage,
        mOCPlatformHelper.findDirectPairingDevices(
            PMConstants.DISCOVERY_TIMEOUT_ZERO,
            OCPlatformHelper.finddirectPairing));

    OcDirectPairDevice dicDirectPairDev = OcDirectPairDeviceHelper.discoveredListObj
        .get(PMConstants.DEVICE_INDEX_ZERO);

    assertFalse(PMConstants.mErrorMessage, mOCPlatformHelper.doDirectPairing(
        dicDirectPairDev, OcPrmType.DP_PRE_CONFIGURED, null,
        OCPlatformHelper.doDPListener));
    assertTrue(PMConstants.WRONG_ERROR_CODE,
        PMConstants.mErrorMessage.contains(PMConstants.ERROR_INVALID_PARAM));
  }

  /**
   * @since 2016-08-22
   * @see public static native void findDirectPairingDevices(int timeout,
   *      FindDirectPairingListener onFindDirectPairingListener) throws
   *      OcException
   * @see public static void doDirectPairing(OcDirectPairDevice peer, OcPrmType
   *      prmType, String pin, DirectPairingListener onDirectPairingListener)
   *      throws OcException
   * @objective Get list of all paired devices for a given device.
   * @target public native void getDirectPairedDevices(GetDirectPairedListener
   *         getDirectPairedListener) throws OcException
   * @test_data Regular Data
   * @pre_condition Start Two JustWork Simulators and Initialize provisioning
   *                manager
   * @procedure Call getDirectPairedDevices() API
   * @post_condition none
   * @expected Should be called successfully
   */
  public void testGetDirectPairedDevices_SRC_RV_P() {
    assertTrue(PMConstants.mErrorMessage,
        mOCPlatformHelper.findDirectPairingDevices(
            PMConstants.DISCOVERY_TIMEOUT_ZERO,
            OCPlatformHelper.finddirectPairing));

    OcDirectPairDevice dicDirectPairDev = OcDirectPairDeviceHelper.discoveredListObj
        .get(PMConstants.DEVICE_INDEX_ZERO);

    assertTrue(PMConstants.mErrorMessage, mOCPlatformHelper.doDirectPairing(
        dicDirectPairDev, OcPrmType.DP_PRE_CONFIGURED,
        PMConstants.DP_PRE_CONFIGURED_PIN, OCPlatformHelper.doDPListener));

    assertTrue(PMConstants.mErrorMessage,
        mOcDirectPairDeviceHelper.getDirectPairedDevices(dicDirectPairDev,
            mOcDirectPairDeviceHelper));
  }

  /**
   * @since 2016-08-22
   * @see public static native void findDirectPairingDevices(int timeout,
   *      FindDirectPairingListener onFindDirectPairingListener) throws
   *      OcException
   * @see public static void doDirectPairing(OcDirectPairDevice peer, OcPrmType
   *      prmType, String pin, DirectPairingListener onDirectPairingListener)
   *      throws OcException
   * @objective Get list of all paired devices negatively with
   *            onDirectPairingListener as null
   * @target public native void getDirectPairedDevices(GetDirectPairedListener
   *         getDirectPairedListener) throws OcException
   * @test_data onDirectPairingListener = null
   * @pre_condition Start Two JustWork Simulators and Initialize provisioning
   *                manager
   * @procedure Call getDirectPairedDevices() API
   * @post_condition none
   * @expected Should throw INVALID_CALLBACK exception
   */
  public void testGetDirectPairedDevicesCB_NV_N() {
    assertTrue(PMConstants.mErrorMessage,
        mOCPlatformHelper.findDirectPairingDevices(
            PMConstants.DISCOVERY_TIMEOUT_ZERO,
            OCPlatformHelper.finddirectPairing));

    OcDirectPairDevice dicDirectPairDev = OcDirectPairDeviceHelper.discoveredListObj
        .get(PMConstants.DEVICE_INDEX_ZERO);

    assertTrue(PMConstants.mErrorMessage, mOCPlatformHelper.doDirectPairing(
        dicDirectPairDev, OcPrmType.DP_PRE_CONFIGURED,
        PMConstants.DP_PRE_CONFIGURED_PIN, OCPlatformHelper.doDPListener));

    assertFalse(PMConstants.mErrorMessage,
        mOcDirectPairDeviceHelper
            .getDirectPairedDevices(dicDirectPairDev, null));
    assertTrue(PMConstants.WRONG_ERROR_CODE,
        PMConstants.mErrorMessage.contains(PMConstants.ERROR_INVALID_CALLBACK));
  }

  /**
   * @since 2016-08-22
   * @see public static native void findDirectPairingDevices(int timeout,
   *      FindDirectPairingListener onFindDirectPairingListener) throws
   *      OcException
   * @objective Get the connectivity type of this resource
   * @target public EnumSet<OcConnectivityType> getConnectivityTypeSet()
   * @test_data None
   * @pre_condition Start Two JustWork Simulators and Initialize provisioning
   *                manager
   * @procedure Call getConnectivityTypeSet() API
   * @post_condition none
   * @expected Should be called successfully
   */
  public void testGetConnectivityTypeSet_SRC_RV_P() {
    assertTrue(PMConstants.mErrorMessage,
        mOCPlatformHelper.findDirectPairingDevices(
            PMConstants.DISCOVERY_TIMEOUT_ZERO,
            OCPlatformHelper.finddirectPairing));

    OcDirectPairDevice dicDirectPairDev = OcDirectPairDeviceHelper.discoveredListObj
        .get(PMConstants.DEVICE_INDEX_ZERO);

    assertTrue(PMConstants.mErrorMessage,
        mOcDirectPairDeviceHelper.getConnectivityTypeSet(dicDirectPairDev));
  }

  /**
   * @since 2016-08-22
   * @see public static native void findDirectPairingDevices(int timeout,
   *      FindDirectPairingListener onFindDirectPairingListener) throws
   *      OcException
   * @objective Get pairing method list for a Device
   * @target public List<Integer> getPairingMethodList()
   * @test_data timeout = 0
   * @pre_condition Start Two JustWork Simulators and Initialize provisioning
   *                manager
   * @procedure Call getPairingMethodList() API
   * @post_condition none
   * @expected Should be called successfully
   */
  public void testGetPairingMethodList_SRC_RV_P() {
    assertTrue(PMConstants.mErrorMessage,
        mOCPlatformHelper.findDirectPairingDevices(
            PMConstants.DISCOVERY_TIMEOUT_ZERO,
            OCPlatformHelper.finddirectPairing));

    OcDirectPairDevice dicDirectPairDev = OcDirectPairDeviceHelper.discoveredListObj
        .get(PMConstants.DEVICE_INDEX_ZERO);

    assertTrue(PMConstants.mErrorMessage,
        mOcDirectPairDeviceHelper.getPairingMethodList(dicDirectPairDev));
  }

  /**
   * @since 2016-08-22
   * @see public static native void findDirectPairingDevices(int timeout,
   *      FindDirectPairingListener onFindDirectPairingListener) throws
   *      OcException
   * @objective Get the device ID
   * @target public String getDevId()
   * @test_data none
   * @pre_condition Start Two JustWork Simulators and Initialize provisioning
   *                manager
   * @procedure Call getDevId() API
   * @post_condition none
   * @expected Should be called successfully
   */
  public void testDirectPairedDevId_SRC_RV_P() {
    assertTrue(PMConstants.mErrorMessage,
        mOCPlatformHelper.findDirectPairingDevices(
            PMConstants.DISCOVERY_TIMEOUT_ZERO,
            OCPlatformHelper.finddirectPairing));

    OcDirectPairDevice dicDirectPairDev = OcDirectPairDeviceHelper.discoveredListObj
        .get(PMConstants.DEVICE_INDEX_ZERO);

    assertTrue(PMConstants.mErrorMessage,
        mOcDirectPairDeviceHelper.getDevId(dicDirectPairDev));
  }

  /**
   * @since 2016-08-22
   * @see public static native void findDirectPairingDevices(int timeout,
   *      FindDirectPairingListener onFindDirectPairingListener) throws
   *      OcException
   * @objective Get the Host
   * @target public String getHost()
   * @test_data none
   * @pre_condition Start Two JustWork Simulators and Initialize provisioning
   *                manager
   * @procedure Call getHost() API
   * @post_condition none
   * @expected Should be called successfully
   */
  public void testDirectPairedHostAddress_SRC_RV_P() {
    assertTrue(PMConstants.mErrorMessage,
        mOCPlatformHelper.findDirectPairingDevices(
            PMConstants.DISCOVERY_TIMEOUT_ZERO,
            OCPlatformHelper.finddirectPairing));

    OcDirectPairDevice dicDirectPairDev = OcDirectPairDeviceHelper.discoveredListObj
        .get(PMConstants.DEVICE_INDEX_ZERO);

    assertTrue(PMConstants.mErrorMessage,
        mOcDirectPairDeviceHelper.getHost(dicDirectPairDev));
  }
}
