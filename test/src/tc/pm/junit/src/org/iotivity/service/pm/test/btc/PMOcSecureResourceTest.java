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

import org.mockito.Mockito;
import java.io.File;
import java.util.ArrayList;
import java.util.EnumSet;
import java.util.List;
import android.test.InstrumentationTestCase;
import android.util.Log;

import org.iotivity.base.CredType;
import org.iotivity.base.DeviceStatus;
import org.iotivity.base.ErrorCode;
import org.iotivity.base.KeySize;
import org.iotivity.base.ModeType;
import org.iotivity.base.OcException;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcProvisioning;
import org.iotivity.base.OcSecureResource;
import org.iotivity.base.OcSecureResource.DoOwnershipTransferListener;
import org.iotivity.base.OicSecAcl;
import org.iotivity.base.OwnedStatus;
import org.iotivity.base.OxmType;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.ProvisionResult;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;

import org.iotivity.service.pm.test.helper.PMHelper;
import org.iotivity.service.pm.test.helper.PMConstants;

//TODO Real scenarios should be implemented instead of Mocking
public class PMOcSecureResourceTest extends InstrumentationTestCase implements
    DoOwnershipTransferListener {

  public PMHelper     mPMHelper    = new PMHelper();

  // Variable for Creating Access Control List (ACL)
  static List<String> mResources   = new ArrayList<String>();
  static List<String> mPeriods     = new ArrayList<String>(); ;
  static List<String> mRecurrences = new ArrayList<String>();

  protected void setUp() throws Exception {
    super.setUp();

    mRecurrences.clear();
    mResources.clear();
    mPeriods.clear();
    mRecurrences.add(PMConstants.DEFAULT_RECURRENCES);
    mResources.add(PMConstants.DEFAULT_RESOURCES);
    mPeriods.add(PMConstants.DEFAULT_PERIOD);

    System.setProperty("dexmaker.dexcache", getInstrumentation()
        .getTargetContext().getCacheDir().getPath());

    // create platform config
    mPMHelper.copyCborFromAsset(getInstrumentation().getContext());
    PMHelper.mFilePath = getInstrumentation().getContext().getFilesDir()
        .getPath()
        + "/"; // data/data/<package>/files/
    PlatformConfig cfg = new PlatformConfig(getInstrumentation()
        .getTargetContext(), ServiceType.IN_PROC, ModeType.CLIENT_SERVER,
        "0.0.0.0", // bind to all available
        // interfaces
        0, QualityOfService.LOW, PMHelper.mFilePath
            + PMConstants.OIC_CLIENT_CBOR_DB_FILE);

    OcPlatform.Configure(cfg);

    PMHelper.mSqlDbPath = getInstrumentation().getContext().getFilesDir()
        .getAbsolutePath()
        .replace(PMConstants.DIR_NAME_FILE, PMConstants.DIR_NAME_DATABASES)
        + File.separator;

    mPMHelper.initOICStack(PMHelper.mSqlDbPath, PMConstants.OIC_SQL_DB_FILE);
  }

  protected void tearDown() throws Exception {
    super.tearDown();
  }

  @Override
  public void doOwnershipTransferListener(List<ProvisionResult> arg0, int arg1) {

  }

  /**
   * @since 2015-11-16
   * @see public static native void provisionInit(String dbPath) throws
   *      OcException
   * @see public static List<OcSecureResource> discoverUnownedDevices(int
   *      timeout) throws OcException
   * @see public static void setownershipTransferCBdata(OxmType type,
   *      PinCallbackListener pinCallbackListener) throws OcException
   * @objective Do ownership trasfer with JUSTWORKS method
   * @target public native void doOwnershipTransfer(DoOwnershipTransferListener
   *         doOwnershipTransferListener) throws OcException
   * @test_data Regular data for doOwnershipTransfer
   * @pre_condition Start two JustWorks Server Manually
   * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
   *            setownershipTransferCBdata 4. call doOwnershipTransfer
   * @post_condition None
   * @expected Should not throw any exception
   */
  public void testDoOwnershipTransfer_RV_P() {
    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

    assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
        OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.doOwnershipTransfer(mPMHelper));
  }

  /**
   * @since 2015-11-16
   * @see public static native void provisionInit(String dbPath) throws
   *      OcException
   * @see public static List<OcSecureResource> discoverUnownedDevices(int
   *      timeout) throws OcException
   * @see public static void setownershipTransferCBdata(OxmType type,
   *      PinCallbackListener pinCallbackListener) throws OcException
   * @objective Do ownership trasfer with doOwnershipTransferListener as null
   * @target public native void doOwnershipTransfer(DoOwnershipTransferListener
   *         doOwnershipTransferListener) throws OcException
   * @test_data doOwnershipTransferListener = null
   * @pre_condition Start two JustWorks Server Manually
   * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
   *            setownershipTransferCBdata 4. call doOwnershipTransfer
   * @post_condition None
   * @expected Should throw INVALID_PARAM exception
   */
  public void testDoOwnershipTransferCB_NV_N() {

    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

    assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
        OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

    assertFalse(PMConstants.mErrorMessage, mPMHelper.doOwnershipTransfer(null));

    assertTrue(PMConstants.WRONG_ERROR_CODE,
        PMConstants.mErrorMessage.contains(PMConstants.ERROR_INVALID_PARAM));
  }

  /**
   * @since 2015-11-16
   * @see public static native void provisionInit(String dbPath) throws
   *      OcException
   * @see public static List<OcSecureResource> discoverUnownedDevices(int
   *      timeout) throws OcException
   * @see public static void setownershipTransferCBdata(OxmType type,
   *      PinCallbackListener pinCallbackListener) throws OcException
   * @see public native void doOwnershipTransfer(DoOwnershipTransferListener
   *      doOwnershipTransferListener) throws OcException
   * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
   *      throws OcException
   * @objective Test provisionPairwiseDevices positively with keysize as
   *            KeySize.OWNER_PSK_LENGTH_128
   * @target public void provisionPairwiseDevices(EnumSet <CredType>
   *         credTypeSet, KeySize keysize, Object acl1, Object device2, Object
   *         acl2, ProvisionPairwiseDevicesListener
   *         provisionPairwiseDevicesListener) throws OcException
   * @test_data keysize = KeySize.OWNER_PSK_LENGTH_128
   * @pre_condition Start two JustWorks Server Manually
   * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
   *            setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
   *            discoverOwnedDevices 6. call provisionPairwiseDevices
   * @post_condition None
   * @expected Should not throw any exception
   */
  public void testProvisionPairwiseDevicesKeySize_SRC_LBV_RV_P() {
    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

    assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
        OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.doOwnershipTransfer(mPMHelper));

    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

    OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
        .get(PMConstants.DEVICE_INDEX_ZERO);
    OcSecureResource ownedDevice02 = PMHelper.mOwnedDeviceList
        .get(PMConstants.DEVICE_INDEX_ONE);

    OicSecAcl acl1 = new OicSecAcl(ownedDevice01.getDeviceID(), mRecurrences,
        mPeriods, PMConstants.DEFAULT_PERMISSION, mResources,
        PMConstants.DEFAULT_ROWNER_ID);
    OicSecAcl acl2 = new OicSecAcl(ownedDevice02.getDeviceID(), mRecurrences,
        mPeriods, PMConstants.DEFAULT_PERMISSION, mResources,
        PMConstants.DEFAULT_ROWNER_ID);

    assertTrue(PMConstants.mErrorMessage, mPMHelper.provisionPairwiseDevices(
        EnumSet.of(CredType.SYMMETRIC_PAIR_WISE_KEY),
        KeySize.OWNER_PSK_LENGTH_128, ownedDevice01, acl1, ownedDevice02, acl2,
        mPMHelper));
  }

  /**
   * @since 2015-11-16
   * @see public static native void provisionInit(String dbPath) throws
   *      OcException
   * @see public static List<OcSecureResource> discoverUnownedDevices(int
   *      timeout) throws OcException
   * @see public static void setownershipTransferCBdata(OxmType type,
   *      PinCallbackListener pinCallbackListener) throws OcException
   * @see public native void doOwnershipTransfer(DoOwnershipTransferListener
   *      doOwnershipTransferListener) throws OcException
   * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
   *      throws OcException
   * @objective Test provisionPairwiseDevices positively with keysize as
   *            KeySize.OWNER_PSK_LENGTH_256
   * @target public void provisionPairwiseDevices(EnumSet <CredType>
   *         credTypeSet, KeySize keysize, Object acl1, Object device2, Object
   *         acl2, ProvisionPairwiseDevicesListener
   *         provisionPairwiseDevicesListener) throws OcException
   * @test_data keysize = KeySize.OWNER_PSK_LENGTH_256
   * @pre_condition None
   * @procedure Call provisionPairwiseDevices API
   * @post_condition none
   * @expected Should not throw any exception
   */
  public void testProvisionPairwiseDevicesKeySize_UBV_P() {
    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

    assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
        OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.doOwnershipTransfer(mPMHelper));

    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

    OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
        .get(PMConstants.DEVICE_INDEX_ZERO);
    OcSecureResource ownedDevice02 = PMHelper.mOwnedDeviceList
        .get(PMConstants.DEVICE_INDEX_ONE);

    OicSecAcl acl1 = new OicSecAcl(ownedDevice01.getDeviceID(), mRecurrences,
        mPeriods, PMConstants.DEFAULT_PERMISSION, mResources,
        PMConstants.DEFAULT_ROWNER_ID);
    OicSecAcl acl2 = new OicSecAcl(ownedDevice02.getDeviceID(), mRecurrences,
        mPeriods, PMConstants.DEFAULT_PERMISSION, mResources,
        PMConstants.DEFAULT_ROWNER_ID);

    assertTrue(PMConstants.mErrorMessage, mPMHelper.provisionPairwiseDevices(
        EnumSet.of(CredType.SYMMETRIC_PAIR_WISE_KEY),
        KeySize.OWNER_PSK_LENGTH_256, ownedDevice01, acl1, ownedDevice02, acl2,
        mPMHelper));
  }

  /**
   * @since 2015-11-16
   * @see public static native void provisionInit(String dbPath) throws
   *      OcException
   * @objective Test provisionPairwiseDevices positively with credTypeSet =
   *            CredType.NO_SECURITY_MODE
   * @target public void provisionPairwiseDevices(EnumSet <CredType>
   *         credTypeSet, KeySize keysize, Object acl1, Object device2, Object
   *         acl2, ProvisionPairwiseDevicesListener
   *         provisionPairwiseDevicesListener) throws OcException
   * @test_data credTypeSet = CredType.NO_SECURITY_MODE
   * @pre_condition None
   * @procedure Call provisionPairwiseDevices API
   * @post_condition none
   * @expected Should not throw any exception
   */
  public void testProvisionPairwiseDevicesCredType0_CLU_P() {
    OicSecAcl acl1 = new OicSecAcl(PMConstants.SUBJECT_O1, mRecurrences,
        mPeriods, PMConstants.DEFAULT_PERMISSION, mResources,
        PMConstants.DEFAULT_ROWNER_ID);
    OicSecAcl acl2 = new OicSecAcl(PMConstants.SUBJECT_O2, mRecurrences,
        mPeriods, PMConstants.DEFAULT_PERMISSION, mResources,
        PMConstants.DEFAULT_ROWNER_ID);

    OcSecureResource mockDevice1 = Mockito.mock(OcSecureResource.class);
    OcSecureResource mockDevice2 = Mockito.mock(OcSecureResource.class);

    try {
      mockDevice1.provisionPairwiseDevices(
          EnumSet.of(CredType.NO_SECURITY_MODE), KeySize.OWNER_PSK_LENGTH_128,
          acl1, mockDevice2, acl2, mPMHelper);
    } catch (OcException e) {
      assertTrue(e.getErrorCode().name(), false);
    }
  }

  /**
   * @since 2015-11-16
   * @see public static native void provisionInit(String dbPath) throws
   *      OcException
   * @see public static List<OcSecureResource> discoverUnownedDevices(int
   *      timeout) throws OcException
   * @see public static void setownershipTransferCBdata(OxmType type,
   *      PinCallbackListener pinCallbackListener) throws OcException
   * @see public native void doOwnershipTransfer(DoOwnershipTransferListener
   *      doOwnershipTransferListener) throws OcException
   * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
   *      throws OcException
   * @objective Test provisionPairwiseDevices positively with acl1 as null
   * @target public void provisionPairwiseDevices(EnumSet <CredType>
   *         credTypeSet, KeySize keysize, Object acl1, Object device2, Object
   *         acl2, ProvisionPairwiseDevicesListener
   *         provisionPairwiseDevicesListener) throws OcException
   * @test_data acl1 = null
   * @pre_condition Start two JustWorks Server Manually
   * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
   *            setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
   *            discoverOwnedDevices 6. call provisionPairwiseDevices
   * @post_condition None
   * @expected Should not throw any exception
   */
  public void testProvisionPairwiseDevicesAcl1_NV_P() {
    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

    assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
        OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.doOwnershipTransfer(mPMHelper));

    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

    OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
        .get(PMConstants.DEVICE_INDEX_ZERO);
    OcSecureResource ownedDevice02 = PMHelper.mOwnedDeviceList
        .get(PMConstants.DEVICE_INDEX_ONE);

    OicSecAcl acl2 = new OicSecAcl(ownedDevice02.getDeviceID(), mRecurrences,
        mPeriods, PMConstants.DEFAULT_PERMISSION, mResources,
        PMConstants.DEFAULT_ROWNER_ID);

    assertTrue(PMConstants.mErrorMessage, mPMHelper.provisionPairwiseDevices(
        EnumSet.of(CredType.SYMMETRIC_PAIR_WISE_KEY),
        KeySize.OWNER_PSK_LENGTH_128, ownedDevice01, null, ownedDevice02, acl2,
        mPMHelper));
  }

  /**
   * @since 2015-11-16
   * @see public static native void provisionInit(String dbPath) throws
   *      OcException
   * @see public static List<OcSecureResource> discoverUnownedDevices(int
   *      timeout) throws OcException
   * @see public static void setownershipTransferCBdata(OxmType type,
   *      PinCallbackListener pinCallbackListener) throws OcException
   * @see public native void doOwnershipTransfer(DoOwnershipTransferListener
   *      doOwnershipTransferListener) throws OcException
   * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
   *      throws OcException
   * @objective Test provisionPairwiseDevices positively with acl2 as null
   * @target public void provisionPairwiseDevices(EnumSet <CredType>
   *         credTypeSet, KeySize keysize, Object acl1, Object device2, Object
   *         acl2, ProvisionPairwiseDevicesListener
   *         provisionPairwiseDevicesListener) throws OcException
   * @test_data acl2 = null
   * @pre_condition Start two JustWorks Server Manually
   * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
   *            setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
   *            discoverOwnedDevices 6. call provisionPairwiseDevices
   * @post_condition None
   * @expected Should not throw any exception
   */
  public void testProvisionPairwiseDevicesAcl2_NV_P() {
    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

    assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
        OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.doOwnershipTransfer(mPMHelper));

    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

    OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
        .get(PMConstants.DEVICE_INDEX_ZERO);
    OcSecureResource ownedDevice02 = PMHelper.mOwnedDeviceList
        .get(PMConstants.DEVICE_INDEX_ONE);

    OicSecAcl acl1 = new OicSecAcl(ownedDevice01.getDeviceID(), mRecurrences,
        mPeriods, PMConstants.DEFAULT_PERMISSION, mResources,
        PMConstants.DEFAULT_ROWNER_ID);

    assertTrue(PMConstants.mErrorMessage, mPMHelper.provisionPairwiseDevices(
        EnumSet.of(CredType.SYMMETRIC_PAIR_WISE_KEY),
        KeySize.OWNER_PSK_LENGTH_128, ownedDevice01, acl1, ownedDevice02, null,
        mPMHelper));
  }

  /**
   * @since 2015-11-16
   * @see public static native void provisionInit(String dbPath) throws
   *      OcException
   * @see public static List<OcSecureResource> discoverUnownedDevices(int
   *      timeout) throws OcException
   * @see public static void setownershipTransferCBdata(OxmType type,
   *      PinCallbackListener pinCallbackListener) throws OcException
   * @see public native void doOwnershipTransfer(DoOwnershipTransferListener
   *      doOwnershipTransferListener) throws OcException
   * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
   *      throws OcException
   * @objective Test provisionPairwiseDevices negatively with
   *            provisionPairwiseDevicesListener as null
   * @target public void provisionPairwiseDevices(EnumSet <CredType>
   *         credTypeSet, KeySize keysize, Object acl1, Object device2, Object
   *         acl2, ProvisionPairwiseDevicesListener
   *         provisionPairwiseDevicesListener) throws OcException
   * @test_data provisionPairwiseDevicesListener = null
   * @pre_condition Start two JustWorks Server Manually
   * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
   *            setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
   *            discoverOwnedDevices 6. call provisionPairwiseDevices
   * @post_condition None
   * @expected Should throw INVALID_PARAM exception
   */
  public void testProvisionPairwiseDevicesCB_NV_N() {
    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

    assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
        OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.doOwnershipTransfer(mPMHelper));

    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

    OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
        .get(PMConstants.DEVICE_INDEX_ZERO);
    OcSecureResource ownedDevice02 = PMHelper.mOwnedDeviceList
        .get(PMConstants.DEVICE_INDEX_ONE);

    OicSecAcl acl1 = new OicSecAcl(ownedDevice01.getDeviceID(), mRecurrences,
        mPeriods, PMConstants.DEFAULT_PERMISSION, mResources,
        PMConstants.DEFAULT_ROWNER_ID);

    OicSecAcl acl2 = new OicSecAcl(ownedDevice02.getDeviceID(), mRecurrences,
        mPeriods, PMConstants.DEFAULT_PERMISSION, mResources,
        PMConstants.DEFAULT_ROWNER_ID);

    assertFalse(PMConstants.mErrorMessage, mPMHelper.provisionPairwiseDevices(
        EnumSet.of(CredType.SYMMETRIC_PAIR_WISE_KEY),
        KeySize.OWNER_PSK_LENGTH_128, ownedDevice01, acl1, ownedDevice02, acl2,
        null));

    assertTrue(PMConstants.WRONG_ERROR_CODE,
        PMConstants.mErrorMessage.contains(PMConstants.ERROR_INVALID_PARAM));
  }

  /**
   * @since 2015-11-16
   * @see public static native void provisionInit(String dbPath) throws
   *      OcException
   * @see public static List<OcSecureResource> discoverUnownedDevices(int
   *      timeout) throws OcException
   * @see public static void setownershipTransferCBdata(OxmType type,
   *      PinCallbackListener pinCallbackListener) throws OcException
   * @see public native void doOwnershipTransfer(DoOwnershipTransferListener
   *      doOwnershipTransferListener) throws OcException
   * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
   *      throws OcException
   * @objective To provision credential devices with keysize =
   *            KeySize.OWNER_PSK_LENGTH_128
   * @target provisionCredentials(EnumSet<CredType> credTypeSet, KeySize
   *         keysize, Object device2, ProvisionCredentialsListener
   *         provisionCredentialsListener)
   * @test_data keysize = KeySize.OWNER_PSK_LENGTH_128
   * @pre_condition Start two JustWorks Server Manually
   * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
   *            setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
   *            discoverOwnedDevices 6. call provisionCredentials
   * @post_condition none
   * @expected Should not throw any exception
   */
  public void testProvisionCredentialsKeySize_SRC_LBV_RV_P() {
    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

    assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
        OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.doOwnershipTransfer(mPMHelper));

    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

    OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
        .get(PMConstants.DEVICE_INDEX_ZERO);
    OcSecureResource ownedDevice02 = PMHelper.mOwnedDeviceList
        .get(PMConstants.DEVICE_INDEX_ONE);

    assertTrue(PMConstants.mErrorMessage, mPMHelper.provisionCredentials(
        EnumSet.of(CredType.SYMMETRIC_PAIR_WISE_KEY),
        KeySize.OWNER_PSK_LENGTH_128, ownedDevice01, ownedDevice02,
        PMHelper.provisionCredentialsListener));
  }

  /**
   * @since 2015-11-16
   * @see public static native void provisionInit(String dbPath) throws
   *      OcException
   * @see public static List<OcSecureResource> discoverUnownedDevices(int
   *      timeout) throws OcException
   * @see public static void setownershipTransferCBdata(OxmType type,
   *      PinCallbackListener pinCallbackListener) throws OcException
   * @see public native void doOwnershipTransfer(DoOwnershipTransferListener
   *      doOwnershipTransferListener) throws OcException
   * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
   *      throws OcException
   * @objective To provision credential devices with keysize =
   *            KeySize.OWNER_PSK_LENGTH_256
   * @target provisionCredentials(EnumSet<CredType> credTypeSet, KeySize
   *         keysize, Object device2, ProvisionCredentialsListener
   *         provisionCredentialsListener)
   * @test_data keysize = KeySize.OWNER_PSK_LENGTH_256
   * @pre_condition Start two JustWorks Server Manually
   * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
   *            setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
   *            discoverOwnedDevices 6. call provisionCredentials
   * @post_condition none
   * @expected Should not throw any exception
   */
  public void testProvisionCredentialsKeySize_UBV_P() {
    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

    assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
        OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.doOwnershipTransfer(mPMHelper));

    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

    OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
        .get(PMConstants.DEVICE_INDEX_ZERO);
    OcSecureResource ownedDevice02 = PMHelper.mOwnedDeviceList
        .get(PMConstants.DEVICE_INDEX_ONE);

    assertTrue(PMConstants.mErrorMessage, mPMHelper.provisionCredentials(
        EnumSet.of(CredType.SYMMETRIC_PAIR_WISE_KEY),
        KeySize.OWNER_PSK_LENGTH_256, ownedDevice01, ownedDevice02,
        PMHelper.provisionCredentialsListener));
  }

  /**
   * @since 2015-11-16
   * @see public static native void provisionInit(String dbPath) throws
   *      OcException
   * @objective Test provisionCredentials positively with credTypeSet =
   *            CredType.NO_SECURITY_MODE
   * @target provisionCredentials(EnumSet<CredType> credTypeSet, KeySize
   *         keysize, Object device2, ProvisionCredentialsListener
   *         provisionCredentialsListener)
   * @test_data credTypeSet = CredType.NO_SECURITY_MODE
   * @pre_condition Initialize provisioning manager
   * @procedure Call provisionCredentials API
   * @post_condition none
   * @expected Should not throw any exception
   */
  public void testProvisionCredentialsCredType0_CLU_P() {
    OcSecureResource mockDevice1 = Mockito.mock(OcSecureResource.class);
    OcSecureResource mockDevice2 = Mockito.mock(OcSecureResource.class);

    try {
      mockDevice1.provisionCredentials(EnumSet.of(CredType.NO_SECURITY_MODE),
          KeySize.OWNER_PSK_LENGTH_256, mockDevice2,
          PMHelper.provisionCredentialsListener);
    } catch (OcException e) {
      assertTrue(e.getErrorCode().name(), false);
    }
  }

  /**
   * @since 2015-11-16
   * @see public static native void provisionInit(String dbPath) throws
   *      OcException
   * @see public static List<OcSecureResource> discoverUnownedDevices(int
   *      timeout) throws OcException
   * @see public static void setownershipTransferCBdata(OxmType type,
   *      PinCallbackListener pinCallbackListener) throws OcException
   * @see public native void doOwnershipTransfer(DoOwnershipTransferListener
   *      doOwnershipTransferListener) throws OcException
   * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
   *      throws OcException
   * @objective To provision credential devices negatively with
   *            provisionCredentialsListener = null
   * @target provisionCredentials(EnumSet<CredType> credTypeSet, KeySize
   *         keysize, Object device2, ProvisionCredentialsListener
   *         provisionCredentialsListener)
   * @test_data provisionCredentialsListener = null
   * @pre_condition Start two JustWorks Server Manually
   * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
   *            setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
   *            discoverOwnedDevices 6. call provisionCredentials
   * @post_condition none
   * @expected Should throw INVALID_PARAM exception
   */
  public void testProvisionCredentialsCB_NV_N() {
    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

    assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
        OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.doOwnershipTransfer(mPMHelper));

    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

    OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
        .get(PMConstants.DEVICE_INDEX_ZERO);
    OcSecureResource ownedDevice02 = PMHelper.mOwnedDeviceList
        .get(PMConstants.DEVICE_INDEX_ONE);

    assertFalse(PMConstants.mErrorMessage, mPMHelper.provisionCredentials(
        EnumSet.of(CredType.SYMMETRIC_PAIR_WISE_KEY),
        KeySize.OWNER_PSK_LENGTH_128, ownedDevice01, ownedDevice02, null));

    assertTrue(PMConstants.WRONG_ERROR_CODE,
        PMConstants.mErrorMessage.contains(PMConstants.ERROR_INVALID_PARAM));
  }

  /**
   * @since 2015-11-16
   * @see public static native void provisionInit(String dbPath) throws
   *      OcException
   * @see public static List<OcSecureResource> discoverUnownedDevices(int
   *      timeout) throws OcException
   * @see public static void setownershipTransferCBdata(OxmType type,
   *      PinCallbackListener pinCallbackListener) throws OcException
   * @see public native void doOwnershipTransfer(DoOwnershipTransferListener
   *      doOwnershipTransferListener) throws OcException
   * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
   *      throws OcException
   * @objective Test provisionACL positively
   * @target provisionACL(Object acl, ProvisionAclListener provisionACLListener)
   * @test_data Regular data for provisionACL
   * @pre_condition Start two JustWorks Server Manually
   * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
   *            setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
   *            discoverOwnedDevices 5. call provisionACL
   * @post_condition None
   * @expected Should not throw any exception
   **/
  public void testProvisionACL_SRC_RV_P() {
    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

    assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
        OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.doOwnershipTransfer(mPMHelper));

    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

    OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
        .get(PMConstants.DEVICE_INDEX_ZERO);

    OicSecAcl acl = new OicSecAcl(ownedDevice01.getDeviceID(), mRecurrences,
        mPeriods, PMConstants.DEFAULT_PERMISSION, mResources,
        PMConstants.DEFAULT_ROWNER_ID);

    assertTrue(PMConstants.mErrorMessage, mPMHelper.provisionACL(ownedDevice01,
        acl, PMHelper.provisionAclListener));
  }

  /**
   * @since 2015-11-16
   * @see public static native void provisionInit(String dbPath) throws
   *      OcException
   * @see public static List<OcSecureResource> discoverUnownedDevices(int
   *      timeout) throws OcException
   * @see public static void setownershipTransferCBdata(OxmType type,
   *      PinCallbackListener pinCallbackListener) throws OcException
   * @see public native void doOwnershipTransfer(DoOwnershipTransferListener
   *      doOwnershipTransferListener) throws OcException
   * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
   *      throws OcException
   * @objective Test provisionACL negatively with acl as null
   * @target provisionACL(Object acl, ProvisionAclListener provisionACLListener)
   * @test_data acl = null
   * @pre_condition Start two JustWorks Server Manually
   * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
   *            setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
   *            discoverOwnedDevices 5. call provisionACL
   * @post_condition None
   * @expected Should throw INVALID_PARAM exception
   **/
  public void testProvisionACLAcl_NV_N() {

    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

    assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
        OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.doOwnershipTransfer(mPMHelper));

    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

    OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
        .get(PMConstants.DEVICE_INDEX_ZERO);

    assertFalse(PMConstants.mErrorMessage, mPMHelper.provisionACL(
        ownedDevice01, null, PMHelper.provisionAclListener));

    assertTrue(PMConstants.WRONG_ERROR_CODE,
        PMConstants.mErrorMessage.contains(PMConstants.ERROR_INVALID_PARAM));
  }

  /**
   * @since 2015-11-16
   * @see public static native void provisionInit(String dbPath) throws
   *      OcException
   * @see public static List<OcSecureResource> discoverUnownedDevices(int
   *      timeout) throws OcException
   * @see public static void setownershipTransferCBdata(OxmType type,
   *      PinCallbackListener pinCallbackListener) throws OcException
   * @see public native void doOwnershipTransfer(DoOwnershipTransferListener
   *      doOwnershipTransferListener) throws OcException
   * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
   *      throws OcException
   * @objective Test provisionACL negatively with provisionACLListener as null
   * @target provisionACL(Object acl, ProvisionAclListener provisionACLListener)
   * @test_data provisionACLListener = null
   * @pre_condition Start two JustWorks Server Manually
   * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
   *            setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
   *            discoverOwnedDevices 5. call provisionACL
   * @post_condition None
   * @expected Should throw INVALID_PARAM exception
   **/
  public void testProvisionACLCB_NV_N() {
    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

    assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
        OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.doOwnershipTransfer(mPMHelper));

    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

    OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
        .get(PMConstants.DEVICE_INDEX_ZERO);

    OicSecAcl acl = new OicSecAcl(ownedDevice01.getDeviceID(), mRecurrences,
        mPeriods, PMConstants.DEFAULT_PERMISSION, mResources,
        PMConstants.DEFAULT_ROWNER_ID);

    assertFalse(PMConstants.mErrorMessage,
        mPMHelper.provisionACL(ownedDevice01, acl, null));

    assertTrue(PMConstants.WRONG_ERROR_CODE,
        PMConstants.mErrorMessage.contains(PMConstants.ERROR_INVALID_PARAM));
  }

  /**
   * @since 2015-11-16
   * @see public static native void provisionInit(String dbPath) throws
   *      OcException
   * @see public static List<OcSecureResource> discoverUnownedDevices(int
   *      timeout) throws OcException
   * @see public static void setownershipTransferCBdata(OxmType type,
   *      PinCallbackListener pinCallbackListener) throws OcException
   * @see public native void doOwnershipTransfer(DoOwnershipTransferListener
   *      doOwnershipTransferListener) throws OcException
   * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
   *      throws OcException
   * @see public void provisionPairwiseDevices(EnumSet <CredType> credTypeSet,
   *      KeySize keysize, Object acl1, Object device2, Object acl2,
   *      ProvisionPairwiseDevicesListener provisionPairwiseDevicesListener)
   *      throws OcException
   * @objective Remove credential and relation between two devices positively
   * @target public native void unlinkDevices(Object device2,
   *         UnlinkDevicesListener unlinkDevicesListener) throws OcException
   * @test_data Regular data for unlinkDevices
   * @pre_condition Start two JustWorks Server Manually
   * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
   *            setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
   *            discoverOwnedDevices 6. call provisionPairwiseDevices 7. call
   *            unlinkDevices
   * @post_condition None
   * @expected Should not throw any exception
   **/
  public void testUnlinkDevices_SRC_RV_P() {
    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

    assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
        OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.doOwnershipTransfer(mPMHelper));

    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

    OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
        .get(PMConstants.DEVICE_INDEX_ZERO);
    OcSecureResource ownedDevice02 = PMHelper.mOwnedDeviceList
        .get(PMConstants.DEVICE_INDEX_ONE);

    OicSecAcl acl1 = new OicSecAcl(ownedDevice01.getDeviceID(), mRecurrences,
        mPeriods, PMConstants.DEFAULT_PERMISSION, mResources,
        PMConstants.DEFAULT_ROWNER_ID);
    OicSecAcl acl2 = new OicSecAcl(ownedDevice02.getDeviceID(), mRecurrences,
        mPeriods, PMConstants.DEFAULT_PERMISSION, mResources,
        PMConstants.DEFAULT_ROWNER_ID);

    assertTrue(PMConstants.mErrorMessage, mPMHelper.provisionPairwiseDevices(
        EnumSet.of(CredType.SYMMETRIC_PAIR_WISE_KEY),
        KeySize.OWNER_PSK_LENGTH_128, ownedDevice01, acl1, ownedDevice02, acl2,
        mPMHelper));

    assertTrue(PMConstants.mErrorMessage, mPMHelper.unlinkDevices(
        ownedDevice01, ownedDevice02, PMHelper.unlinkDevicesListener));
  }

  /**
   * @since 2015-11-16
   * @see public static native void provisionInit(String dbPath) throws
   *      OcException
   * @see public static List<OcSecureResource> discoverUnownedDevices(int
   *      timeout) throws OcException
   * @see public static void setownershipTransferCBdata(OxmType type,
   *      PinCallbackListener pinCallbackListener) throws OcException
   * @see public native void doOwnershipTransfer(DoOwnershipTransferListener
   *      doOwnershipTransferListener) throws OcException
   * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
   *      throws OcException
   * @see public void provisionPairwiseDevices(EnumSet <CredType> credTypeSet,
   *      KeySize keysize, Object acl1, Object device2, Object acl2,
   *      ProvisionPairwiseDevicesListener provisionPairwiseDevicesListener)
   *      throws OcException
   * @objective Remove credential and relation between two devices negatively
   *            with device2 = null
   * @target public native void unlinkDevices(Object device2,
   *         UnlinkDevicesListener unlinkDevicesListener) throws OcException
   * @test_data device2 = null
   * @pre_condition Start two JustWorks Server Manually
   * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
   *            setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
   *            discoverOwnedDevices 6. call provisionPairwiseDevices 7. call
   *            unlinkDevices
   * @post_condition None
   * @expected Should throw INVALID_PARAM exception
   **/
  public void testUnlinkDevicesDevice2_ETC_NV_N() {
    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

    assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
        OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.doOwnershipTransfer(mPMHelper));

    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

    OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
        .get(PMConstants.DEVICE_INDEX_ZERO);
    OcSecureResource ownedDevice02 = PMHelper.mOwnedDeviceList
        .get(PMConstants.DEVICE_INDEX_ONE);

    OicSecAcl acl1 = new OicSecAcl(ownedDevice01.getDeviceID(), mRecurrences,
        mPeriods, PMConstants.DEFAULT_PERMISSION, mResources,
        PMConstants.DEFAULT_ROWNER_ID);
    OicSecAcl acl2 = new OicSecAcl(ownedDevice02.getDeviceID(), mRecurrences,
        mPeriods, PMConstants.DEFAULT_PERMISSION, mResources,
        PMConstants.DEFAULT_ROWNER_ID);

    assertTrue(PMConstants.mErrorMessage, mPMHelper.provisionPairwiseDevices(
        EnumSet.of(CredType.SYMMETRIC_PAIR_WISE_KEY),
        KeySize.OWNER_PSK_LENGTH_128, ownedDevice01, acl1, ownedDevice02, acl2,
        mPMHelper));

    assertFalse(PMConstants.mErrorMessage, mPMHelper.unlinkDevices(
        ownedDevice01, null, PMHelper.unlinkDevicesListener));

    assertTrue(PMConstants.WRONG_ERROR_CODE,
        PMConstants.mErrorMessage.contains(PMConstants.ERROR_INVALID_PARAM));
  }

  /**
   * @since 2015-11-16
   * @see public static native void provisionInit(String dbPath) throws
   *      OcException
   * @see public static List<OcSecureResource> discoverUnownedDevices(int
   *      timeout) throws OcException
   * @see public static void setownershipTransferCBdata(OxmType type,
   *      PinCallbackListener pinCallbackListener) throws OcException
   * @see public native void doOwnershipTransfer(DoOwnershipTransferListener
   *      doOwnershipTransferListener) throws OcException
   * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
   *      throws OcException
   * @see public void provisionPairwiseDevices(EnumSet <CredType> credTypeSet,
   *      KeySize keysize, Object acl1, Object device2, Object acl2,
   *      ProvisionPairwiseDevicesListener provisionPairwiseDevicesListener)
   *      throws OcException
   * @objective Remove credential and relation between two devices negatively
   *            with unlinkDevicesListener = null
   * @target public native void unlinkDevices(Object device2,
   *         UnlinkDevicesListener unlinkDevicesListener) throws OcException
   * @test_data unlinkDevicesListener = null
   * @pre_condition Start two JustWorks Server Manually
   * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
   *            setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
   *            discoverOwnedDevices 6. call provisionPairwiseDevices 7. call
   *            unlinkDevices
   * @post_condition None
   * @expected Should throw INVALID_PARAM exception
   **/
  public void testUnlinkDevicesCB_NV_N() {
    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

    assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
        OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.doOwnershipTransfer(mPMHelper));

    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

    OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
        .get(PMConstants.DEVICE_INDEX_ZERO);
    OcSecureResource ownedDevice02 = PMHelper.mOwnedDeviceList
        .get(PMConstants.DEVICE_INDEX_ONE);

    OicSecAcl acl1 = new OicSecAcl(ownedDevice01.getDeviceID(), mRecurrences,
        mPeriods, PMConstants.DEFAULT_PERMISSION, mResources,
        PMConstants.DEFAULT_ROWNER_ID);
    OicSecAcl acl2 = new OicSecAcl(ownedDevice02.getDeviceID(), mRecurrences,
        mPeriods, PMConstants.DEFAULT_PERMISSION, mResources,
        PMConstants.DEFAULT_ROWNER_ID);

    assertTrue(PMConstants.mErrorMessage, mPMHelper.provisionPairwiseDevices(
        EnumSet.of(CredType.SYMMETRIC_PAIR_WISE_KEY),
        KeySize.OWNER_PSK_LENGTH_128, ownedDevice01, acl1, ownedDevice02, acl2,
        mPMHelper));

    assertFalse(PMConstants.mErrorMessage,
        mPMHelper.unlinkDevices(ownedDevice01, ownedDevice02, null));

    assertTrue(PMConstants.WRONG_ERROR_CODE,
        PMConstants.mErrorMessage.contains(PMConstants.ERROR_INVALID_PARAM));
  }

  /**
   * @since 2015-11-16
   * @see public static native void provisionInit(String dbPath) throws
   *      OcException
   * @see public static List<OcSecureResource> discoverUnownedDevices(int
   *      timeout) throws OcException
   * @see public static void setownershipTransferCBdata(OxmType type,
   *      PinCallbackListener pinCallbackListener) throws OcException
   * @see public native void doOwnershipTransfer(DoOwnershipTransferListener
   *      doOwnershipTransferListener) throws OcException
   * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
   *      throws OcException
   * @see public void provisionPairwiseDevices(EnumSet <CredType> credTypeSet,
   *      KeySize keysize, Object acl1, Object device2, Object acl2,
   *      ProvisionPairwiseDevicesListener provisionPairwiseDevicesListener)
   *      throws OcException
   * @objective Provisioning Secured Devices and Unlink them
   * @target public native void removeDevice(int timeout, RemoveDeviceListener
   *         removeDeviceListener) throws OcException
   * @test_data Regular Data for removeDevice
   * @pre_condition Start two JustWorks Server Manually
   * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
   *            setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
   *            discoverOwnedDevices 6. call provisionPairwiseDevices 7. call
   *            removeDevice
   * @post_condition None
   * @expected Should not throw any exception
   */
  public void testRemoveDevice_SRC_RV_P() {
    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

    assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
        OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.doOwnershipTransfer(mPMHelper));

    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

    OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
        .get(PMConstants.DEVICE_INDEX_ZERO);
    OcSecureResource ownedDevice02 = PMHelper.mOwnedDeviceList
        .get(PMConstants.DEVICE_INDEX_ONE);

    OicSecAcl acl1 = new OicSecAcl(ownedDevice01.getDeviceID(), mRecurrences,
        mPeriods, PMConstants.DEFAULT_PERMISSION, mResources,
        PMConstants.DEFAULT_ROWNER_ID);
    OicSecAcl acl2 = new OicSecAcl(ownedDevice02.getDeviceID(), mRecurrences,
        mPeriods, PMConstants.DEFAULT_PERMISSION, mResources,
        PMConstants.DEFAULT_ROWNER_ID);

    assertTrue(PMConstants.mErrorMessage, mPMHelper.provisionPairwiseDevices(
        EnumSet.of(CredType.SYMMETRIC_PAIR_WISE_KEY),
        KeySize.OWNER_PSK_LENGTH_128, ownedDevice01, acl1, ownedDevice02, acl2,
        mPMHelper));

    assertTrue(PMConstants.mErrorMessage, mPMHelper.removeDevice(ownedDevice01,
        PMConstants.DISCOVERY_TIMEOUT_10, PMHelper.removeDeviceListener));
  }

  /**
   * @since 2015-11-16
   * @see public static native void provisionInit(String dbPath) throws
   *      OcException
   * @see public static List<OcSecureResource> discoverUnownedDevices(int
   *      timeout) throws OcException
   * @see public static void setownershipTransferCBdata(OxmType type,
   *      PinCallbackListener pinCallbackListener) throws OcException
   * @see public native void doOwnershipTransfer(DoOwnershipTransferListener
   *      doOwnershipTransferListener) throws OcException
   * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
   *      throws OcException
   * @see public void provisionPairwiseDevices(EnumSet <CredType> credTypeSet,
   *      KeySize keysize, Object acl1, Object device2, Object acl2,
   *      ProvisionPairwiseDevicesListener provisionPairwiseDevicesListener)
   *      throws OcException
   * @objective Provisioning Secured Devices and Unlink them with the Lower
   *            Boundary Value of timeout
   * @target public native void removeDevice(int timeout, RemoveDeviceListener
   *         removeDeviceListener) throws OcException
   * @test_data timeout = 1 second
   * @pre_condition Start two JustWorks Server Manually
   * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
   *            setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
   *            discoverOwnedDevices 6. call provisionPairwiseDevices 7. call
   *            removeDevice
   * @post_condition None
   * @expected Should not throw any exception
   */
  public void testRemoveDevice_LBV_P() {
    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

    assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
        OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.doOwnershipTransfer(mPMHelper));

    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

    OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
        .get(PMConstants.DEVICE_INDEX_ZERO);
    OcSecureResource ownedDevice02 = PMHelper.mOwnedDeviceList
        .get(PMConstants.DEVICE_INDEX_ONE);

    OicSecAcl acl1 = new OicSecAcl(ownedDevice01.getDeviceID(), mRecurrences,
        mPeriods, PMConstants.DEFAULT_PERMISSION, mResources,
        PMConstants.DEFAULT_ROWNER_ID);
    OicSecAcl acl2 = new OicSecAcl(ownedDevice02.getDeviceID(), mRecurrences,
        mPeriods, PMConstants.DEFAULT_PERMISSION, mResources,
        PMConstants.DEFAULT_ROWNER_ID);

    assertTrue(PMConstants.mErrorMessage, mPMHelper.provisionPairwiseDevices(
        EnumSet.of(CredType.SYMMETRIC_PAIR_WISE_KEY),
        KeySize.OWNER_PSK_LENGTH_128, ownedDevice01, acl1, ownedDevice02, acl2,
        mPMHelper));

    assertTrue(PMConstants.mErrorMessage, mPMHelper.removeDevice(ownedDevice01,
        PMConstants.DISCOVERY_TIMEOUT_ONE, PMHelper.removeDeviceListener));
  }

  /**
   * @since 2015-11-16
   * @see public static native void provisionInit(String dbPath) throws
   *      OcException
   * @see public static List<OcSecureResource> discoverUnownedDevices(int
   *      timeout) throws OcException
   * @see public static void setownershipTransferCBdata(OxmType type,
   *      PinCallbackListener pinCallbackListener) throws OcException
   * @see public native void doOwnershipTransfer(DoOwnershipTransferListener
   *      doOwnershipTransferListener) throws OcException
   * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
   *      throws OcException
   * @see public void provisionPairwiseDevices(EnumSet <CredType> credTypeSet,
   *      KeySize keysize, Object acl1, Object device2, Object acl2,
   *      ProvisionPairwiseDevicesListener provisionPairwiseDevicesListener)
   *      throws OcException
   * @objective Provisioning Secured Devices and Unlink them with the Lower
   *            Outer Boundary Value of timeout
   * @target public native void removeDevice(int timeout, RemoveDeviceListener
   *         removeDeviceListener) throws OcException
   * @test_data timeout = 0 second
   * @pre_condition Start two JustWorks Server Manually
   * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
   *            setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
   *            discoverOwnedDevices 6. call provisionPairwiseDevices 7. call
   *            removeDevice
   * @post_condition None
   * @expected Should throw INVALID_PARAM exception
   */
  public void testRemoveDevice_LOBV_ETC_N() {
    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

    assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
        OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.doOwnershipTransfer(mPMHelper));

    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

    OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
        .get(PMConstants.DEVICE_INDEX_ZERO);
    OcSecureResource ownedDevice02 = PMHelper.mOwnedDeviceList
        .get(PMConstants.DEVICE_INDEX_ONE);

    OicSecAcl acl1 = new OicSecAcl(ownedDevice01.getDeviceID(), mRecurrences,
        mPeriods, PMConstants.DEFAULT_PERMISSION, mResources,
        PMConstants.DEFAULT_ROWNER_ID);
    OicSecAcl acl2 = new OicSecAcl(ownedDevice02.getDeviceID(), mRecurrences,
        mPeriods, PMConstants.DEFAULT_PERMISSION, mResources,
        PMConstants.DEFAULT_ROWNER_ID);

    assertTrue(PMConstants.mErrorMessage, mPMHelper.provisionPairwiseDevices(
        EnumSet.of(CredType.SYMMETRIC_PAIR_WISE_KEY),
        KeySize.OWNER_PSK_LENGTH_128, ownedDevice01, acl1, ownedDevice02, acl2,
        mPMHelper));

    assertFalse(PMConstants.mErrorMessage, mPMHelper.removeDevice(
        ownedDevice01, PMConstants.DISCOVERY_TIMEOUT_ZERO,
        PMHelper.removeDeviceListener));

    assertTrue(PMConstants.WRONG_ERROR_CODE,
        PMConstants.mErrorMessage.contains(PMConstants.ERROR_INVALID_PARAM));
  }

  /**
   * @since 2015-11-16
   * @see public static native void provisionInit(String dbPath) throws
   *      OcException
   * @see public static List<OcSecureResource> discoverUnownedDevices(int
   *      timeout) throws OcException
   * @see public static void setownershipTransferCBdata(OxmType type,
   *      PinCallbackListener pinCallbackListener) throws OcException
   * @see public native void doOwnershipTransfer(DoOwnershipTransferListener
   *      doOwnershipTransferListener) throws OcException
   * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
   *      throws OcException
   * @see public void provisionPairwiseDevices(EnumSet <CredType> credTypeSet,
   *      KeySize keysize, Object acl1, Object device2, Object acl2,
   *      ProvisionPairwiseDevicesListener provisionPairwiseDevicesListener)
   *      throws OcException
   * @objective Provisioning Secured Devices and Unlink them with
   *            removeDeviceListener = null
   * @target public native void removeDevice(int timeout, RemoveDeviceListener
   *         removeDeviceListener) throws OcException
   * @test_data removeDeviceListener = null
   * @pre_condition Start two JustWorks Server Manually
   * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
   *            setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
   *            discoverOwnedDevices 6. call provisionPairwiseDevices 7. call
   *            removeDevice
   * @post_condition None
   * @expected Should throw INVALID_PARAM exception
   */
  public void testRemoveDeviceCB_NV_N() {

    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

    assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
        OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.doOwnershipTransfer(mPMHelper));

    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

    OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
        .get(PMConstants.DEVICE_INDEX_ZERO);
    OcSecureResource ownedDevice02 = PMHelper.mOwnedDeviceList
        .get(PMConstants.DEVICE_INDEX_ONE);

    OicSecAcl acl1 = new OicSecAcl(ownedDevice01.getDeviceID(), mRecurrences,
        mPeriods, PMConstants.DEFAULT_PERMISSION, mResources,
        PMConstants.DEFAULT_ROWNER_ID);
    OicSecAcl acl2 = new OicSecAcl(ownedDevice02.getDeviceID(), mRecurrences,
        mPeriods, PMConstants.DEFAULT_PERMISSION, mResources,
        PMConstants.DEFAULT_ROWNER_ID);

    assertTrue(PMConstants.mErrorMessage, mPMHelper.provisionPairwiseDevices(
        EnumSet.of(CredType.SYMMETRIC_PAIR_WISE_KEY),
        KeySize.OWNER_PSK_LENGTH_128, ownedDevice01, acl1, ownedDevice02, acl2,
        mPMHelper));

    assertFalse(PMConstants.mErrorMessage, mPMHelper.removeDevice(
        ownedDevice01, PMConstants.DISCOVERY_TIMEOUT_10, null));

    assertTrue(PMConstants.WRONG_ERROR_CODE,
        PMConstants.mErrorMessage.contains(PMConstants.ERROR_INVALID_PARAM));
  }

  /**
   * @since 2015-11-16
   * @see public static native void provisionInit(String dbPath) throws
   *      OcException
   * @see public static List<OcSecureResource> discoverUnownedDevices(int
   *      timeout) throws OcException
   * @see public static void setownershipTransferCBdata(OxmType type,
   *      PinCallbackListener pinCallbackListener) throws OcException
   * @see public native void doOwnershipTransfer(DoOwnershipTransferListener
   *      doOwnershipTransferListener) throws OcException
   * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
   *      throws OcException
   * @see public void provisionPairwiseDevices(EnumSet <CredType> credTypeSet,
   *      KeySize keysize, Object acl1, Object device2, Object acl2,
   *      ProvisionPairwiseDevicesListener provisionPairwiseDevicesListener)
   *      throws OcException
   * @objective Get List of device ID of devices linked with invoking device
   * @target public native List<String> getLinkedDevices()
   * @test_data None
   * @pre_condition Start two JustWorks Server Manually
   * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
   *            setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
   *            discoverOwnedDevices 6. call provisionPairwiseDevices 7. call
   *            getLinkedDevices
   * @post_condition None
   * @expected Should not throw any exception
   **/
  public void testGetLinkedDevices_SRC_P() {
    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

    assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
        OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.doOwnershipTransfer(mPMHelper));

    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

    OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
        .get(PMConstants.DEVICE_INDEX_ZERO);
    OcSecureResource ownedDevice02 = PMHelper.mOwnedDeviceList
        .get(PMConstants.DEVICE_INDEX_ONE);

    OicSecAcl acl1 = new OicSecAcl(ownedDevice01.getDeviceID(), mRecurrences,
        mPeriods, PMConstants.DEFAULT_PERMISSION, mResources,
        PMConstants.DEFAULT_ROWNER_ID);
    OicSecAcl acl2 = new OicSecAcl(ownedDevice02.getDeviceID(), mRecurrences,
        mPeriods, PMConstants.DEFAULT_PERMISSION, mResources,
        PMConstants.DEFAULT_ROWNER_ID);

    assertTrue(PMConstants.mErrorMessage, mPMHelper.provisionPairwiseDevices(
        EnumSet.of(CredType.SYMMETRIC_PAIR_WISE_KEY),
        KeySize.OWNER_PSK_LENGTH_128, ownedDevice01, acl1, ownedDevice02, acl2,
        mPMHelper));

    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.getLinkedDevices(ownedDevice01));
  }

  /**
   * @since 2015-11-16
   * @see public static native void provisionInit(String dbPath) throws
   *      OcException
   * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
   *      throws OcException
   * @objective Get IP address of secure discovered device
   * @target String getIpAddr()
   * @test_data None
   * @pre_condition Start two JustWorks Server Manually
   * @procedure 1. call provisionInit 2. call discoverOwnedDevices 6. call
   *            getIpAddr
   * @post_condition None
   * @expected Should not throw any exception
   **/
  public void testGetIpAddr_SRC_P() {
    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

    OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
        .get(PMConstants.DEVICE_INDEX_ZERO);

    assertTrue(PMConstants.mErrorMessage, mPMHelper.getIpAddr(ownedDevice01));
  }

  /**
   * @since 2015-11-16
   * @see public static native void provisionInit(String dbPath) throws
   *      OcException
   * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
   *      throws OcException
   * @objective Get device id of a device
   * @target String getDeviceID()
   * @test_data None
   * @pre_condition Start two JustWorks Server Manually
   * @procedure 1. call provisionInit 2. call discoverOwnedDevices 6. call
   *            getDeviceID
   * @post_condition None
   * @expected Should not throw any exception
   **/
  public void testGetDeviceID_SRC_P() {
    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

    OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
        .get(PMConstants.DEVICE_INDEX_ZERO);

    assertTrue(PMConstants.mErrorMessage, mPMHelper.getDeviceID(ownedDevice01));
  }

  /**
   * @since 2015-11-16
   * @see public static native void provisionInit(String dbPath) throws
   *      OcException
   * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
   *      throws OcException
   * @objective Get device status (ON/OFF) of a device
   * @target DeviceStatus getDeviceStatus()
   * @test_data None
   * @pre_condition Start two JustWorks Server Manually
   * @procedure 1. call provisionInit 2. call discoverOwnedDevices 6. call
   *            getDeviceStatus
   * @post_condition none
   * @expected Should return DeviceStatus.ON
   **/
  public void testGetDeviceStatusOn_P() {   
    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

    OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
        .get(PMConstants.DEVICE_INDEX_ZERO);

    assertTrue(PMConstants.mErrorMessage, mPMHelper.getDeviceStatus(ownedDevice01, DeviceStatus.ON));
  }

  /**
   * @since 2016-03-31
   * @see public static native void provisionInit(String dbPath) throws
   *      OcException
   * @objective Get device status (ON/OFF) of a device
   * @target DeviceStatus getDeviceStatus()
   * @test_data none
   * @pre_condition Initialize provisioning manager
   * @procedure Call getDeviceStatus() API
   * @post_condition none
   * @expected Should return DeviceStatus.OFF
   **/
  public void testGetDeviceStatusOff_P() {
    OcSecureResource mockDevice = Mockito.mock(OcSecureResource.class);

    try {
      Mockito.doReturn(DeviceStatus.OFF).when(mockDevice).getDeviceStatus();
      assertEquals(DeviceStatus.OFF, mockDevice.getDeviceStatus());
    } catch (OcException e) {
      assertTrue(false);
    }
  }

  /**
   * @since 2016-03-31
   * @see public static native void provisionInit(String dbPath) throws
   *      OcException
   * @objective Get device status (ON/OFF) of a device
   * @target DeviceStatus getDeviceStatus()
   * @test_data none
   * @pre_condition Initialize provisioning manager
   * @procedure Call getDeviceStatus() API
   * @post_condition none
   * @expected Should return DeviceStatus.INVALID
   **/
  public void testGetDeviceStatusInvalid_P() {
    OcSecureResource mockDevice = Mockito.mock(OcSecureResource.class);

    try {
      Mockito.doReturn(DeviceStatus.INVALID).when(mockDevice).getDeviceStatus();
      assertEquals(DeviceStatus.INVALID, mockDevice.getDeviceStatus());
    } catch (OcException e) {
      assertTrue(false);
    }
  }

  /**
   * @since 2015-11-16
   * @see public static native void provisionInit(String dbPath) throws
   *      OcException
   * @see public static List<OcSecureResource> discoverUnownedDevices(int
   *      timeout) throws OcException
   * @objective Get device ownership (OWNED/UNOWNED) status
   * @target OwnedStatus getOwnedStatus()
   * @test_data None
   * @pre_condition Start two JustWorks Server Manually
   * @procedure 1. call provisionInit 2. call discoverOwnedDevices 6. call
   *            getOwnedStatus
   * @post_condition none
   * @expected Should return OwnedStatus.UNOWNED
   **/
  public void testGetOwnedStatusUnowned_SRC_P() {
    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

    OcSecureResource unOwnedDevice01 = PMHelper.mUnownedDeviceList
        .get(PMConstants.DEVICE_INDEX_ZERO);
    
    assertTrue(PMConstants.mErrorMessage, mPMHelper.getOwnedStatus(unOwnedDevice01, OwnedStatus.UNOWNED));
  }

  /**
   * @since 2015-11-16
   * @see public static native void provisionInit(String dbPath) throws
   *      OcException
   * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
   *      throws OcException
   * @objective Get device ownership (OWNED/UNOWNED) status
   * @target OwnedStatus getOwnedStatus()
   * @test_data None
   * @pre_condition Start two JustWorks Server Manually
   * @procedure 1. call provisionInit 2. call discoverOwnedDevices 6. call
   *            getOwnedStatus
   * @post_condition none
   * @expected Should return OwnedStatus.OWNED
   **/
  public void testGetOwnedStatusOwned_SRC_P() {
    assertTrue(PMConstants.mErrorMessage,
        mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

    OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
        .get(PMConstants.DEVICE_INDEX_ZERO);

    assertTrue(PMConstants.mErrorMessage, mPMHelper.getOwnedStatus(ownedDevice01, OwnedStatus.OWNED));
  }

  /**
   * @since 2015-03-31
   * @see public static native void provisionInit(String dbPath) throws
   *      OcException
   * @objective Get device ownership (OWNED/UNOWNED) status
   * @target OwnedStatus getOwnedStatus()
   * @test_data none
   * @pre_condition Initialize provisioning manager
   * @procedure Call getOwnedStatus() API
   * @post_condition none
   * @expected Should return OwnedStatus.INVALID
   **/
  public void testGetOwnedStatusInvalid_SRC_P() {
    OcSecureResource mockDevice = Mockito.mock(OcSecureResource.class);

    try {
      Mockito.doReturn(OwnedStatus.INVALID).when(mockDevice).getOwnedStatus();
      assertEquals(OwnedStatus.INVALID, mockDevice.getOwnedStatus());
    } catch (OcException e) {
      assertTrue(false);
    }
  }
}