/******************************************************************
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * LICENSE-2.0" target="_blank">http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
import org.iotivity.base.OcPrmType;
import org.iotivity.base.OcProvisioning;
import org.iotivity.base.OcSecureResource;
import org.iotivity.base.OicSecAce;
import org.iotivity.base.OcSecureResource.DoOwnershipTransferListener;
import org.iotivity.base.OicSecAcl;
import org.iotivity.base.OicSecPdAcl;
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

    public PMHelper mPMHelper = new PMHelper();

    // Variable for Creating Access Control List (ACL)
    static List<String> mResources = new ArrayList<String>();
    static List<String> mPeriods = new ArrayList<String>();
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
        mPMHelper.copyCborFromAsset(getInstrumentation().getContext(),
                PMConstants.OIC_CLIENT_CBOR_DB_FILE);
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
     * @objective Do ownership trasfer with JUSTWORKS method
     * @target public native void doOwnershipTransfer(DoOwnershipTransferListener
     * doOwnershipTransferListener) throws OcException
     * @test_data Regular data for doOwnershipTransfer
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
     * setownershipTransferCBdata 4. call doOwnershipTransfer
     * @post_condition None
     * @expected Should not throw any exception
     * @see public static native void provisionInit(String dbPath) throws
     * OcException
     * @see public static List<OcSecureResource> discoverUnownedDevices(int
     * timeout) throws OcException
     * @see public static void setownershipTransferCBdata(OxmType type,
     * PinCallbackListener pinCallbackListener) throws OcException
     * @since 2015-11-16
     */
    public void testDoOwnershipTransfer_RV_P() {
        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
                OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.doOwnershipTransfer(mPMHelper));
    }

    /**
     * @objective Do ownership trasfer with doOwnershipTransferListener as null
     * @target public native void doOwnershipTransfer(DoOwnershipTransferListener
     * doOwnershipTransferListener) throws OcException
     * @test_data doOwnershipTransferListener = null
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
     * setownershipTransferCBdata 4. call doOwnershipTransfer
     * @post_condition None
     * @expected Should throw INVALID_CALLBACK exception
     * @see public static native void provisionInit(String dbPath) throws
     * OcException
     * @see public static List<OcSecureResource> discoverUnownedDevices(int
     * timeout) throws OcException
     * @see public static void setownershipTransferCBdata(OxmType type,
     * PinCallbackListener pinCallbackListener) throws OcException
     * @since 2015-11-16
     */
    public void testDoOwnershipTransferCB_NV_N() {

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
                OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

        assertFalse(PMConstants.mErrorMessage, mPMHelper.doOwnershipTransfer(null));

        assertTrue(PMConstants.WRONG_ERROR_CODE,
                PMConstants.mErrorMessage.contains(PMConstants.ERROR_INVALID_CALLBACK));
    }

    /**
     * @objective Test provisionPairwiseDevices positively with keysize as
     * KeySize.OWNER_PSK_LENGTH_128
     * @target public void provisionPairwiseDevices(EnumSet <CredType>
     * credTypeSet, KeySize keysize, Object acl1, Object device2, Object
     * acl2, ProvisionPairwiseDevicesListener
     * provisionPairwiseDevicesListener) throws OcException
     * @test_data keysize = KeySize.OWNER_PSK_LENGTH_128
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
     * setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
     * discoverOwnedDevices 6. call provisionPairwiseDevices
     * @post_condition None
     * @expected Should not throw any exception
     * @see public static native void provisionInit(String dbPath) throws
     * OcException
     * @see public static List<OcSecureResource> discoverUnownedDevices(int
     * timeout) throws OcException
     * @see public static void setownershipTransferCBdata(OxmType type,
     * PinCallbackListener pinCallbackListener) throws OcException
     * @see public native void doOwnershipTransfer(DoOwnershipTransferListener
     * doOwnershipTransferListener) throws OcException
     * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
     * throws OcException
     * @since 2015-11-16
     */
    public void testProvisionPairwiseDevicesKeySize_SRC_LBV_RV_P() {
        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
                OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.doOwnershipTransfer(mPMHelper));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ZERO);
        OcSecureResource ownedDevice02 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ONE);

        List<OicSecAcl> aclArrayList = mPMHelper.createAcl(ownedDevice01,
                ownedDevice02);

        assertTrue(PMConstants.mErrorMessage, mPMHelper.provisionPairwiseDevices(
                EnumSet.of(CredType.SYMMETRIC_PAIR_WISE_KEY),
                KeySize.OWNER_PSK_LENGTH_128, ownedDevice01, aclArrayList.get(0),
                ownedDevice02, aclArrayList.get(1), mPMHelper));
    }

    /**
     * @objective Test provisionPairwiseDevices positively with keysize as
     * KeySize.OWNER_PSK_LENGTH_256
     * @target public void provisionPairwiseDevices(EnumSet <CredType>
     * credTypeSet, KeySize keysize, Object acl1, Object device2, Object
     * acl2, ProvisionPairwiseDevicesListener
     * provisionPairwiseDevicesListener) throws OcException
     * @test_data keysize = KeySize.OWNER_PSK_LENGTH_256
     * @pre_condition None
     * @procedure Call provisionPairwiseDevices API
     * @post_condition none
     * @expected Should not throw any exception
     * @see public static native void provisionInit(String dbPath) throws
     * OcException
     * @see public static List<OcSecureResource> discoverUnownedDevices(int
     * timeout) throws OcException
     * @see public static void setownershipTransferCBdata(OxmType type,
     * PinCallbackListener pinCallbackListener) throws OcException
     * @see public native void doOwnershipTransfer(DoOwnershipTransferListener
     * doOwnershipTransferListener) throws OcException
     * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
     * throws OcException
     * @since 2015-11-16
     */
    public void testProvisionPairwiseDevicesKeySize_UBV_P() {
        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
                OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.doOwnershipTransfer(mPMHelper));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ZERO);
        OcSecureResource ownedDevice02 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ONE);

        List<OicSecAcl> aclArrayList = mPMHelper.createAcl(ownedDevice01,
                ownedDevice02);

        assertTrue(PMConstants.mErrorMessage, mPMHelper.provisionPairwiseDevices(
                EnumSet.of(CredType.SYMMETRIC_PAIR_WISE_KEY),
                KeySize.OWNER_PSK_LENGTH_256, ownedDevice01, aclArrayList.get(0),
                ownedDevice02, aclArrayList.get(1), mPMHelper));
    }

    /**
     * @objective Test provisionPairwiseDevices positively with credTypeSet =
     * CredType.NO_SECURITY_MODE
     * @target public void provisionPairwiseDevices(EnumSet <CredType>
     * credTypeSet, KeySize keysize, Object acl1, Object device2, Object
     * acl2, ProvisionPairwiseDevicesListener
     * provisionPairwiseDevicesListener) throws OcException
     * @test_data credTypeSet = CredType.NO_SECURITY_MODE
     * @pre_condition None
     * @procedure Call provisionPairwiseDevices API
     * @post_condition none
     * @expected Should not throw any exception
     * @see public static native void provisionInit(String dbPath) throws
     * OcException
     * @since 2015-11-16
     */
    public void testProvisionPairwiseDevicesCredType0_CLU_P() {

        OicSecAcl acl1 = Mockito.mock(OicSecAcl.class);
        OicSecAcl acl2 = Mockito.mock(OicSecAcl.class);

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
     * @objective Test provisionPairwiseDevices positively with acl1 as null
     * @target public void provisionPairwiseDevices(EnumSet <CredType>
     * credTypeSet, KeySize keysize, Object acl1, Object device2, Object
     * acl2, ProvisionPairwiseDevicesListener
     * provisionPairwiseDevicesListener) throws OcException
     * @test_data acl1 = null
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
     * setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
     * discoverOwnedDevices 6. call provisionPairwiseDevices
     * @post_condition None
     * @expected Should not throw any exception
     * @see public static native void provisionInit(String dbPath) throws
     * OcException
     * @see public static List<OcSecureResource> discoverUnownedDevices(int
     * timeout) throws OcException
     * @see public static void setownershipTransferCBdata(OxmType type,
     * PinCallbackListener pinCallbackListener) throws OcException
     * @see public native void doOwnershipTransfer(DoOwnershipTransferListener
     * doOwnershipTransferListener) throws OcException
     * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
     * throws OcException
     * @since 2015-11-16
     */
    public void testProvisionPairwiseDevicesAcl1_NV_P() {
        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
                OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.doOwnershipTransfer(mPMHelper));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ZERO);
        OcSecureResource ownedDevice02 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ONE);

        List<OicSecAcl> aclArrayList = mPMHelper.createAcl(ownedDevice01,
                ownedDevice02);

        assertTrue(PMConstants.mErrorMessage, mPMHelper.provisionPairwiseDevices(
                EnumSet.of(CredType.SYMMETRIC_PAIR_WISE_KEY),
                KeySize.OWNER_PSK_LENGTH_128, ownedDevice01, null, ownedDevice02,
                aclArrayList.get(1), mPMHelper));
    }

    /**
     * @objective Test provisionPairwiseDevices positively with acl2 as null
     * @target public void provisionPairwiseDevices(EnumSet <CredType>
     * credTypeSet, KeySize keysize, Object acl1, Object device2, Object
     * acl2, ProvisionPairwiseDevicesListener
     * provisionPairwiseDevicesListener) throws OcException
     * @test_data acl2 = null
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
     * setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
     * discoverOwnedDevices 6. call provisionPairwiseDevices
     * @post_condition None
     * @expected Should not throw any exception
     * @see public static native void provisionInit(String dbPath) throws
     * OcException
     * @see public static List<OcSecureResource> discoverUnownedDevices(int
     * timeout) throws OcException
     * @see public static void setownershipTransferCBdata(OxmType type,
     * PinCallbackListener pinCallbackListener) throws OcException
     * @see public native void doOwnershipTransfer(DoOwnershipTransferListener
     * doOwnershipTransferListener) throws OcException
     * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
     * throws OcException
     * @since 2015-11-16
     */
    public void testProvisionPairwiseDevicesAcl2_NV_P() {
        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
                OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.doOwnershipTransfer(mPMHelper));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ZERO);
        OcSecureResource ownedDevice02 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ONE);

        List<OicSecAcl> aclArrayList = mPMHelper.createAcl(ownedDevice01,
                ownedDevice02);

        assertTrue(PMConstants.mErrorMessage, mPMHelper.provisionPairwiseDevices(
                EnumSet.of(CredType.SYMMETRIC_PAIR_WISE_KEY),
                KeySize.OWNER_PSK_LENGTH_128, ownedDevice01, aclArrayList.get(0),
                ownedDevice02, null, mPMHelper));
    }

    /**
     * @objective Test provisionPairwiseDevices negatively with
     * provisionPairwiseDevicesListener as null
     * @target public void provisionPairwiseDevices(EnumSet <CredType>
     * credTypeSet, KeySize keysize, Object acl1, Object device2, Object
     * acl2, ProvisionPairwiseDevicesListener
     * provisionPairwiseDevicesListener) throws OcException
     * @test_data provisionPairwiseDevicesListener = null
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
     * setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
     * discoverOwnedDevices 6. call provisionPairwiseDevices
     * @post_condition None
     * @expected Should throw INVALID_CALLBACK exception
     * @see public static native void provisionInit(String dbPath) throws
     * OcException
     * @see public static List<OcSecureResource> discoverUnownedDevices(int
     * timeout) throws OcException
     * @see public static void setownershipTransferCBdata(OxmType type,
     * PinCallbackListener pinCallbackListener) throws OcException
     * @see public native void doOwnershipTransfer(DoOwnershipTransferListener
     * doOwnershipTransferListener) throws OcException
     * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
     * throws OcException
     * @since 2015-11-16
     */
    public void testProvisionPairwiseDevicesCB_NV_N() {
        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
                OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.doOwnershipTransfer(mPMHelper));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ZERO);
        OcSecureResource ownedDevice02 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ONE);

        List<OicSecAcl> aclArrayList = mPMHelper.createAcl(ownedDevice01,
                ownedDevice02);

        assertFalse(PMConstants.mErrorMessage, mPMHelper.provisionPairwiseDevices(
                EnumSet.of(CredType.SYMMETRIC_PAIR_WISE_KEY),
                KeySize.OWNER_PSK_LENGTH_128, ownedDevice01, aclArrayList.get(0),
                ownedDevice02, aclArrayList.get(1), null));

        assertTrue(PMConstants.WRONG_ERROR_CODE,
                PMConstants.mErrorMessage.contains(PMConstants.ERROR_INVALID_CALLBACK));
    }

    /**
     * @objective Test doProvisionDirectPairing positively
     * @target public void doProvisionDirectPairing(String pin, OicSecPdAcl[]
     * pdacls, List<OcPrmType> type, boolean edp ,
     * ProvisionDirectPairingListener provisionDirectPairingListener)
     * @test_data Regular Data for doProvisionDirectPairing API
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
     * setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
     * discoverOwnedDevices 6. call doProvisionDirectPairing
     * @post_condition None
     * @expected Should not throw any exception
     * @see public static native void provisionInit(String dbPath) throws
     * OcException
     * @see public static List<OcSecureResource> discoverUnownedDevices(int
     * timeout) throws OcException
     * @see public static void setownershipTransferCBdata(OxmType type,
     * PinCallbackListener pinCallbackListener) throws OcException
     * @see public native void doOwnershipTransfer(DoOwnershipTransferListener
     * doOwnershipTransferListener) throws OcException
     * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
     * throws OcException
     * @since 2016-08-23
     */
    public void testDoProvisionDirectPairing_SRC_RV_P() {
        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
                OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.doOwnershipTransfer(mPMHelper));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ZERO);

        List<OcPrmType> prmTypes = new ArrayList<OcPrmType>();
        prmTypes.add(OcPrmType.DP_PRE_CONFIGURED);
        boolean edp = true;

        List<String> resources = new ArrayList<String>();
        List<String> periods = new ArrayList<String>();
        List<String> recurrences = new ArrayList<String>();
        resources.add(PMConstants.DEFAULT_RESOURCES);
        OicSecPdAcl pdAcl = new OicSecPdAcl(recurrences, periods,
                PMConstants.DEFAULT_PERMISSION, resources);
        OicSecPdAcl[] oicSecPdAcls = new OicSecPdAcl[1];
        oicSecPdAcls[0] = pdAcl;

        assertTrue(PMConstants.mErrorMessage, mPMHelper.doProvisionDirectPairing(
                ownedDevice01, PMConstants.DP_PRE_CONFIGURED_PIN, oicSecPdAcls,
                prmTypes, edp, mPMHelper));
    }

    /**
     * @objective Test doProvisionDirectPairing negatively with pdacls as null
     * @target public void doProvisionDirectPairing(String pin, OicSecPdAcl[]
     * pdacls, List<OcPrmType> type, boolean edp ,
     * ProvisionDirectPairingListener provisionDirectPairingListener)
     * @test_data pdacls = null
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
     * setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
     * discoverOwnedDevices 6. call doProvisionDirectPairing
     * @post_condition None
     * @expected Should throw INVALID_PARAM exception
     * @see public static native void provisionInit(String dbPath) throws
     * OcException
     * @see public static List<OcSecureResource> discoverUnownedDevices(int
     * timeout) throws OcException
     * @see public static void setownershipTransferCBdata(OxmType type,
     * PinCallbackListener pinCallbackListener) throws OcException
     * @see public native void doOwnershipTransfer(DoOwnershipTransferListener
     * doOwnershipTransferListener) throws OcException
     * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
     * throws OcException
     * @since 2016-08-23
     */
    public void testDoProvisionDirectPairingOicSecPdAcl_NV_N() {
        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
                OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.doOwnershipTransfer(mPMHelper));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ZERO);
        OcSecureResource ownedDevice02 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ONE);

        mPMHelper.createAcl(ownedDevice01, ownedDevice02);

        List<OcPrmType> prmTypes = new ArrayList<OcPrmType>();
        prmTypes.add(OcPrmType.DP_PRE_CONFIGURED);
        boolean edp = true;

        List<String> resources = new ArrayList<String>();
        List<String> periods = new ArrayList<String>();
        List<String> recurrences = new ArrayList<String>();
        resources.add(PMConstants.DEFAULT_RESOURCES);
        OicSecPdAcl pdAcl = new OicSecPdAcl(recurrences, periods,
                PMConstants.DEFAULT_PERMISSION, resources);
        OicSecPdAcl[] oicSecPdAcls = new OicSecPdAcl[1];
        oicSecPdAcls[0] = pdAcl;

        assertFalse(PMConstants.mErrorMessage, mPMHelper.doProvisionDirectPairing(
                ownedDevice01, PMConstants.DP_PRE_CONFIGURED_PIN, null, prmTypes, edp,
                mPMHelper));

        assertTrue(PMConstants.WRONG_ERROR_CODE,
                PMConstants.mErrorMessage.contains(PMConstants.ERROR_INVALID_PARAM));
    }

    /**
     * @objective Test doProvisionDirectPairing negatively with List<OcPrmType> as
     * null
     * @target public void doProvisionDirectPairing(String pin, OicSecPdAcl[]
     * pdacls, List<OcPrmType> type, boolean edp ,
     * ProvisionDirectPairingListener provisionDirectPairingListener)
     * @test_data List<OcPrmType> = null
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
     * setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
     * discoverOwnedDevices 6. call doProvisionDirectPairing
     * @post_condition None
     * @expected Should throw INVALID_PARAM exception
     * @see public static native void provisionInit(String dbPath) throws
     * OcException
     * @see public static List<OcSecureResource> discoverUnownedDevices(int
     * timeout) throws OcException
     * @see public static void setownershipTransferCBdata(OxmType type,
     * PinCallbackListener pinCallbackListener) throws OcException
     * @see public native void doOwnershipTransfer(DoOwnershipTransferListener
     * doOwnershipTransferListener) throws OcException
     * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
     * throws OcException
     * @since 2016-08-23
     */
    public void testDoProvisionDirectPairingOcPrmType_NV_N() {
        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
                OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.doOwnershipTransfer(mPMHelper));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ZERO);
        OcSecureResource ownedDevice02 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ONE);

        mPMHelper.createAcl(ownedDevice01, ownedDevice02);

        List<OcPrmType> prmTypes = new ArrayList<OcPrmType>();
        prmTypes.add(OcPrmType.DP_PRE_CONFIGURED);
        boolean edp = true;

        List<String> resources = new ArrayList<String>();
        List<String> periods = new ArrayList<String>();
        List<String> recurrences = new ArrayList<String>();
        resources.add(PMConstants.DEFAULT_RESOURCES);
        OicSecPdAcl pdAcl = new OicSecPdAcl(recurrences, periods,
                PMConstants.DEFAULT_PERMISSION, resources);
        OicSecPdAcl[] oicSecPdAcls = new OicSecPdAcl[1];
        oicSecPdAcls[0] = pdAcl;

        assertFalse(PMConstants.mErrorMessage, mPMHelper.doProvisionDirectPairing(
                ownedDevice01, PMConstants.DP_PRE_CONFIGURED_PIN, oicSecPdAcls, null,
                edp, mPMHelper));

        assertTrue(PMConstants.WRONG_ERROR_CODE,
                PMConstants.mErrorMessage.contains(PMConstants.ERROR_INVALID_PARAM));
    }

    /**
     * @objective Test doProvisionDirectPairing negatively with List<OcPrmType> as
     * null
     * @target public void doProvisionDirectPairing(String pin, OicSecPdAcl[]
     * pdacls, List<OcPrmType> type, boolean edp ,
     * ProvisionDirectPairingListener provisionDirectPairingListener)
     * @test_data pin = null
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
     * setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
     * discoverOwnedDevices 6. call doProvisionDirectPairing
     * @post_condition None
     * @expected Should throw INVALID_PARAM exception
     * @see public static native void provisionInit(String dbPath) throws
     * OcException
     * @see public static List<OcSecureResource> discoverUnownedDevices(int
     * timeout) throws OcException
     * @see public static void setownershipTransferCBdata(OxmType type,
     * PinCallbackListener pinCallbackListener) throws OcException
     * @see public native void doOwnershipTransfer(DoOwnershipTransferListener
     * doOwnershipTransferListener) throws OcException
     * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
     * throws OcException
     * @since 2016-08-23
     */
    public void testDoProvisionDirectPairingPin_NV_N() {
        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
                OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.doOwnershipTransfer(mPMHelper));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ZERO);
        OcSecureResource ownedDevice02 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ONE);

        mPMHelper.createAcl(ownedDevice01, ownedDevice02);

        List<OcPrmType> prmTypes = new ArrayList<OcPrmType>();
        prmTypes.add(OcPrmType.DP_PRE_CONFIGURED);
        boolean edp = true;

        List<String> resources = new ArrayList<String>();
        List<String> periods = new ArrayList<String>();
        List<String> recurrences = new ArrayList<String>();
        resources.add(PMConstants.DEFAULT_RESOURCES);
        OicSecPdAcl pdAcl = new OicSecPdAcl(recurrences, periods,
                PMConstants.DEFAULT_PERMISSION, resources);
        OicSecPdAcl[] oicSecPdAcls = new OicSecPdAcl[1];
        oicSecPdAcls[0] = pdAcl;

        assertFalse(PMConstants.mErrorMessage, mPMHelper.doProvisionDirectPairing(
                ownedDevice01, null, oicSecPdAcls, prmTypes, edp, mPMHelper));

        assertTrue(PMConstants.WRONG_ERROR_CODE,
                PMConstants.mErrorMessage.contains(PMConstants.ERROR_INVALID_PARAM));
    }

    /**
     * @objective Test doProvisionDirectPairing negatively with List<OcPrmType> as
     * null
     * @target public void doProvisionDirectPairing(String pin, OicSecPdAcl[]
     * pdacls, List<OcPrmType> type, boolean edp ,
     * ProvisionDirectPairingListener provisionDirectPairingListener)
     * @test_data pin = null
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
     * setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
     * discoverOwnedDevices 6. call doProvisionDirectPairing
     * @post_condition None
     * @expected Should throw INVALID_PARAM exception
     * @see public static native void provisionInit(String dbPath) throws
     * OcException
     * @see public static List<OcSecureResource> discoverUnownedDevices(int
     * timeout) throws OcException
     * @see public static void setownershipTransferCBdata(OxmType type,
     * PinCallbackListener pinCallbackListener) throws OcException
     * @see public native void doOwnershipTransfer(DoOwnershipTransferListener
     * doOwnershipTransferListener) throws OcException
     * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
     * throws OcException
     * @since 2016-08-23
     */
    public void testDoProvisionDirectPairingCb_NV_N() {
        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
                OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.doOwnershipTransfer(mPMHelper));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ZERO);
        OcSecureResource ownedDevice02 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ONE);

        mPMHelper.createAcl(ownedDevice01, ownedDevice02);

        List<OcPrmType> prmTypes = new ArrayList<OcPrmType>();
        prmTypes.add(OcPrmType.DP_PRE_CONFIGURED);
        boolean edp = true;

        List<String> resources = new ArrayList<String>();
        List<String> periods = new ArrayList<String>();
        List<String> recurrences = new ArrayList<String>();
        resources.add(PMConstants.DEFAULT_RESOURCES);
        OicSecPdAcl pdAcl = new OicSecPdAcl(recurrences, periods,
                PMConstants.DEFAULT_PERMISSION, resources);
        OicSecPdAcl[] oicSecPdAcls = new OicSecPdAcl[1];
        oicSecPdAcls[0] = pdAcl;

        assertFalse(PMConstants.mErrorMessage, mPMHelper.doProvisionDirectPairing(
                ownedDevice01, PMConstants.DP_PRE_CONFIGURED_PIN, oicSecPdAcls,
                prmTypes, edp, null));

        assertTrue(PMConstants.WRONG_ERROR_CODE,
                PMConstants.mErrorMessage.contains(PMConstants.ERROR_INVALID_CALLBACK));
    }

    /**
     * @objective To provision credential devices with keysize =
     * KeySize.OWNER_PSK_LENGTH_128
     * @target provisionCredentials(EnumSet<CredType> credTypeSet, KeySize
     *keysize, Object device2, ProvisionCredentialsListener
     *provisionCredentialsListener)
     * @test_data keysize = KeySize.OWNER_PSK_LENGTH_128
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
     * setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
     * discoverOwnedDevices 6. call provisionCredentials
     * @post_condition none
     * @expected Should not throw any exception
     * @see public static native void provisionInit(String dbPath) throws
     * OcException
     * @see public static List<OcSecureResource> discoverUnownedDevices(int
     * timeout) throws OcException
     * @see public static void setownershipTransferCBdata(OxmType type,
     * PinCallbackListener pinCallbackListener) throws OcException
     * @see public native void doOwnershipTransfer(DoOwnershipTransferListener
     * doOwnershipTransferListener) throws OcException
     * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
     * throws OcException
     * @since 2015-11-16
     */
    public void testProvisionCredentialsKeySize_SRC_LBV_RV_P() {
        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
                OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.doOwnershipTransfer(mPMHelper));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

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
     * @objective To provision credential devices with keysize =
     * KeySize.OWNER_PSK_LENGTH_256
     * @target provisionCredentials(EnumSet<CredType> credTypeSet, KeySize
     *keysize, Object device2, ProvisionCredentialsListener
     *provisionCredentialsListener)
     * @test_data keysize = KeySize.OWNER_PSK_LENGTH_256
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
     * setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
     * discoverOwnedDevices 6. call provisionCredentials
     * @post_condition none
     * @expected Should not throw any exception
     * @see public static native void provisionInit(String dbPath) throws
     * OcException
     * @see public static List<OcSecureResource> discoverUnownedDevices(int
     * timeout) throws OcException
     * @see public static void setownershipTransferCBdata(OxmType type,
     * PinCallbackListener pinCallbackListener) throws OcException
     * @see public native void doOwnershipTransfer(DoOwnershipTransferListener
     * doOwnershipTransferListener) throws OcException
     * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
     * throws OcException
     * @since 2015-11-16
     */
    public void testProvisionCredentialsKeySize_UBV_P() {
        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
                OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.doOwnershipTransfer(mPMHelper));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

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
     * @objective Test provisionCredentials positively with credTypeSet =
     * CredType.NO_SECURITY_MODE
     * @target provisionCredentials(EnumSet<CredType> credTypeSet, KeySize
     *keysize, Object device2, ProvisionCredentialsListener
     *provisionCredentialsListener)
     * @test_data credTypeSet = CredType.NO_SECURITY_MODE
     * @pre_condition Initialize provisioning manager
     * @procedure Call provisionCredentials API
     * @post_condition none
     * @expected Should not throw any exception
     * @see public static native void provisionInit(String dbPath) throws
     * OcException
     * @since 2015-11-16
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
     * @objective To provision credential devices negatively with
     * provisionCredentialsListener = null
     * @target provisionCredentials(EnumSet<CredType> credTypeSet, KeySize
     *keysize, Object device2, ProvisionCredentialsListener
     *provisionCredentialsListener)
     * @test_data provisionCredentialsListener = null
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
     * setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
     * discoverOwnedDevices 6. call provisionCredentials
     * @post_condition none
     * @expected Should throw INVALID_PARAM exception
     * @see public static native void provisionInit(String dbPath) throws
     * OcException
     * @see public static List<OcSecureResource> discoverUnownedDevices(int
     * timeout) throws OcException
     * @see public static void setownershipTransferCBdata(OxmType type,
     * PinCallbackListener pinCallbackListener) throws OcException
     * @see public native void doOwnershipTransfer(DoOwnershipTransferListener
     * doOwnershipTransferListener) throws OcException
     * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
     * throws OcException
     * @since 2015-11-16
     */
    public void testProvisionCredentialsCB_NV_N() {
        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
                OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.doOwnershipTransfer(mPMHelper));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ZERO);
        OcSecureResource ownedDevice02 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ONE);

        assertFalse(PMConstants.mErrorMessage, mPMHelper.provisionCredentials(
                EnumSet.of(CredType.SYMMETRIC_PAIR_WISE_KEY),
                KeySize.OWNER_PSK_LENGTH_128, ownedDevice01, ownedDevice02, null));

        assertTrue(PMConstants.WRONG_ERROR_CODE,
                PMConstants.mErrorMessage.contains(PMConstants.ERROR_INVALID_CALLBACK));
    }

    /**
     * @objective Test provisionACL positively
     * @target provisionACL(Object acl, ProvisionAclListener provisionACLListener)
     * @test_data Regular data for provisionACL
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
     * setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
     * discoverOwnedDevices 5. call provisionACL
     * @post_condition None
     * @expected Should not throw any exception
     * @see public static native void provisionInit(String dbPath) throws
     * OcException
     * @see public static List<OcSecureResource> discoverUnownedDevices(int
     * timeout) throws OcException
     * @see public static void setownershipTransferCBdata(OxmType type,
     * PinCallbackListener pinCallbackListener) throws OcException
     * @see public native void doOwnershipTransfer(DoOwnershipTransferListener
     * doOwnershipTransferListener) throws OcException
     * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
     * throws OcException
     * @since 2015-11-16
     **/
    public void testProvisionACL_SRC_RV_P() {
        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
                OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.doOwnershipTransfer(mPMHelper));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ZERO);

        List<OicSecAcl> aclArrayList = mPMHelper.createAcl(ownedDevice01);

        assertTrue(PMConstants.mErrorMessage, mPMHelper.provisionACL(ownedDevice01,
                aclArrayList.get(0), PMHelper.provisionAclListener));
    }

    /**
     * @objective Test provisionACL negatively with acl as null
     * @target provisionACL(Object acl, ProvisionAclListener provisionACLListener)
     * @test_data acl = null
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
     * setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
     * discoverOwnedDevices 5. call provisionACL
     * @post_condition None
     * @expected Should throw INVALID_PARAM exception
     * @see public static native void provisionInit(String dbPath) throws
     * OcException
     * @see public static List<OcSecureResource> discoverUnownedDevices(int
     * timeout) throws OcException
     * @see public static void setownershipTransferCBdata(OxmType type,
     * PinCallbackListener pinCallbackListener) throws OcException
     * @see public native void doOwnershipTransfer(DoOwnershipTransferListener
     * doOwnershipTransferListener) throws OcException
     * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
     * throws OcException
     * @since 2015-11-16
     **/
    public void testProvisionACLAcl_NV_N() {

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
                OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.doOwnershipTransfer(mPMHelper));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ZERO);

        assertFalse(PMConstants.mErrorMessage, mPMHelper.provisionACL(
                ownedDevice01, null, PMHelper.provisionAclListener));

        assertTrue(PMConstants.WRONG_ERROR_CODE,
                PMConstants.mErrorMessage.contains(PMConstants.ERROR_INVALID_PARAM));
    }

    /**
     * @objective Test provisionACL negatively with provisionACLListener as null
     * @target provisionACL(Object acl, ProvisionAclListener provisionACLListener)
     * @test_data provisionACLListener = null
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
     * setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
     * discoverOwnedDevices 5. call provisionACL
     * @post_condition None
     * @expected Should throw INVALID_PARAM exception
     * @see public static native void provisionInit(String dbPath) throws
     * OcException
     * @see public static List<OcSecureResource> discoverUnownedDevices(int
     * timeout) throws OcException
     * @see public static void setownershipTransferCBdata(OxmType type,
     * PinCallbackListener pinCallbackListener) throws OcException
     * @see public native void doOwnershipTransfer(DoOwnershipTransferListener
     * doOwnershipTransferListener) throws OcException
     * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
     * throws OcException
     * @since 2015-11-16
     **/
    public void testProvisionACLCB_NV_N() {
        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
                OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.doOwnershipTransfer(mPMHelper));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ZERO);

        List<OicSecAcl> aclArrayList = mPMHelper.createAcl(ownedDevice01);

        assertFalse(PMConstants.mErrorMessage,
                mPMHelper.provisionACL(ownedDevice01, aclArrayList.get(0), null));

        assertTrue(PMConstants.WRONG_ERROR_CODE,
                PMConstants.mErrorMessage.contains(PMConstants.ERROR_INVALID_CALLBACK));
    }

    /**
     * @objective Remove credential and relation between two devices positively
     * @target public native void unlinkDevices(Object device2,
     * UnlinkDevicesListener unlinkDevicesListener) throws OcException
     * @test_data Regular data for unlinkDevices
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
     * setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
     * discoverOwnedDevices 6. call provisionPairwiseDevices 7. call
     * unlinkDevices
     * @post_condition None
     * @expected Should not throw any exception
     * @see public static native void provisionInit(String dbPath) throws
     * OcException
     * @see public static List<OcSecureResource> discoverUnownedDevices(int
     * timeout) throws OcException
     * @see public static void setownershipTransferCBdata(OxmType type,
     * PinCallbackListener pinCallbackListener) throws OcException
     * @see public native void doOwnershipTransfer(DoOwnershipTransferListener
     * doOwnershipTransferListener) throws OcException
     * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
     * throws OcException
     * @see public void provisionPairwiseDevices(EnumSet <CredType> credTypeSet,
     * KeySize keysize, Object acl1, Object device2, Object acl2,
     * ProvisionPairwiseDevicesListener provisionPairwiseDevicesListener)
     * throws OcException
     * @since 2015-11-16
     **/
    public void testUnlinkDevices_SRC_RV_P() {
        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
                OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.doOwnershipTransfer(mPMHelper));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ZERO);
        OcSecureResource ownedDevice02 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ONE);

        List<OicSecAcl> aclArrayList = mPMHelper.createAcl(ownedDevice01,
                ownedDevice02);

        assertTrue(PMConstants.mErrorMessage, mPMHelper.provisionPairwiseDevices(
                EnumSet.of(CredType.SYMMETRIC_PAIR_WISE_KEY),
                KeySize.OWNER_PSK_LENGTH_128, ownedDevice01, aclArrayList.get(0),
                ownedDevice02, aclArrayList.get(1), mPMHelper));

        assertTrue(PMConstants.mErrorMessage, mPMHelper.unlinkDevices(
                ownedDevice01, ownedDevice02, PMHelper.unlinkDevicesListener));
    }

    /**
     * @objective Remove credential and relation between two devices negatively
     * with device2 = null
     * @target public native void unlinkDevices(Object device2,
     * UnlinkDevicesListener unlinkDevicesListener) throws OcException
     * @test_data device2 = null
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
     * setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
     * discoverOwnedDevices 6. call provisionPairwiseDevices 7. call
     * unlinkDevices
     * @post_condition None
     * @expected Should throw INVALID_PARAM exception
     * @see public static native void provisionInit(String dbPath) throws
     * OcException
     * @see public static List<OcSecureResource> discoverUnownedDevices(int
     * timeout) throws OcException
     * @see public static void setownershipTransferCBdata(OxmType type,
     * PinCallbackListener pinCallbackListener) throws OcException
     * @see public native void doOwnershipTransfer(DoOwnershipTransferListener
     * doOwnershipTransferListener) throws OcException
     * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
     * throws OcException
     * @see public void provisionPairwiseDevices(EnumSet <CredType> credTypeSet,
     * KeySize keysize, Object acl1, Object device2, Object acl2,
     * ProvisionPairwiseDevicesListener provisionPairwiseDevicesListener)
     * throws OcException
     * @since 2015-11-16
     **/
    public void testUnlinkDevicesDevice2_ETC_NV_N() {
        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
                OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.doOwnershipTransfer(mPMHelper));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ZERO);
        OcSecureResource ownedDevice02 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ONE);

        List<OicSecAcl> aclArrayList = mPMHelper.createAcl(ownedDevice01,
                ownedDevice02);

        assertTrue(PMConstants.mErrorMessage, mPMHelper.provisionPairwiseDevices(
                EnumSet.of(CredType.SYMMETRIC_PAIR_WISE_KEY),
                KeySize.OWNER_PSK_LENGTH_128, ownedDevice01, aclArrayList.get(0),
                ownedDevice02, aclArrayList.get(1), mPMHelper));

        assertFalse(PMConstants.mErrorMessage, mPMHelper.unlinkDevices(
                ownedDevice01, null, PMHelper.unlinkDevicesListener));

        assertTrue(PMConstants.WRONG_ERROR_CODE,
                PMConstants.mErrorMessage.contains(PMConstants.ERROR_INVALID_PARAM));
    }

    /**
     * @objective Remove credential and relation between two devices negatively
     * with unlinkDevicesListener = null
     * @target public native void unlinkDevices(Object device2,
     * UnlinkDevicesListener unlinkDevicesListener) throws OcException
     * @test_data unlinkDevicesListener = null
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
     * setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
     * discoverOwnedDevices 6. call provisionPairwiseDevices 7. call
     * unlinkDevices
     * @post_condition None
     * @expected Should throw INVALID_PARAM exception
     * @see public static native void provisionInit(String dbPath) throws
     * OcException
     * @see public static List<OcSecureResource> discoverUnownedDevices(int
     * timeout) throws OcException
     * @see public static void setownershipTransferCBdata(OxmType type,
     * PinCallbackListener pinCallbackListener) throws OcException
     * @see public native void doOwnershipTransfer(DoOwnershipTransferListener
     * doOwnershipTransferListener) throws OcException
     * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
     * throws OcException
     * @see public void provisionPairwiseDevices(EnumSet <CredType> credTypeSet,
     * KeySize keysize, Object acl1, Object device2, Object acl2,
     * ProvisionPairwiseDevicesListener provisionPairwiseDevicesListener)
     * throws OcException
     * @since 2015-11-16
     **/
    public void testUnlinkDevicesCB_NV_N() {
        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
                OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.doOwnershipTransfer(mPMHelper));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ZERO);
        OcSecureResource ownedDevice02 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ONE);

        List<OicSecAcl> aclArrayList = mPMHelper.createAcl(ownedDevice01,
                ownedDevice02);

        assertTrue(PMConstants.mErrorMessage, mPMHelper.provisionPairwiseDevices(
                EnumSet.of(CredType.SYMMETRIC_PAIR_WISE_KEY),
                KeySize.OWNER_PSK_LENGTH_128, ownedDevice01, aclArrayList.get(0),
                ownedDevice02, aclArrayList.get(1), mPMHelper));

        assertFalse(PMConstants.mErrorMessage,
                mPMHelper.unlinkDevices(ownedDevice01, ownedDevice02, null));

        assertTrue(PMConstants.WRONG_ERROR_CODE,
                PMConstants.mErrorMessage.contains(PMConstants.ERROR_INVALID_CALLBACK));
    }

    /**
     * @objective Provisioning Secured Devices and Unlink them
     * @target public native void removeDevice(int timeout, RemoveDeviceListener
     * removeDeviceListener) throws OcException
     * @test_data Regular Data for removeDevice
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
     * setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
     * discoverOwnedDevices 6. call provisionPairwiseDevices 7. call
     * removeDevice
     * @post_condition None
     * @expected Should not throw any exception
     * @see public static native void provisionInit(String dbPath) throws
     * OcException
     * @see public static List<OcSecureResource> discoverUnownedDevices(int
     * timeout) throws OcException
     * @see public static void setownershipTransferCBdata(OxmType type,
     * PinCallbackListener pinCallbackListener) throws OcException
     * @see public native void doOwnershipTransfer(DoOwnershipTransferListener
     * doOwnershipTransferListener) throws OcException
     * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
     * throws OcException
     * @see public void provisionPairwiseDevices(EnumSet <CredType> credTypeSet,
     * KeySize keysize, Object acl1, Object device2, Object acl2,
     * ProvisionPairwiseDevicesListener provisionPairwiseDevicesListener)
     * throws OcException
     * @since 2015-11-16
     */
    public void testRemoveDevice_SRC_RV_P() {
        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
                OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.doOwnershipTransfer(mPMHelper));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ZERO);
        OcSecureResource ownedDevice02 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ONE);

        List<OicSecAcl> aclArrayList = mPMHelper.createAcl(ownedDevice01,
                ownedDevice02);

        assertTrue(PMConstants.mErrorMessage, mPMHelper.provisionPairwiseDevices(
                EnumSet.of(CredType.SYMMETRIC_PAIR_WISE_KEY),
                KeySize.OWNER_PSK_LENGTH_128, ownedDevice01, aclArrayList.get(0),
                ownedDevice02, aclArrayList.get(1), mPMHelper));

        assertTrue(PMConstants.mErrorMessage, mPMHelper.removeDevice(ownedDevice01,
                PMConstants.DISCOVERY_TIMEOUT_05, PMHelper.removeDeviceListener));
    }

    /**
     * @objective Provisioning Secured Devices and Unlink them with the Lower
     * Boundary Value of timeout
     * @target public native void removeDevice(int timeout, RemoveDeviceListener
     * removeDeviceListener) throws OcException
     * @test_data timeout = 1 second
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
     * setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
     * discoverOwnedDevices 6. call provisionPairwiseDevices 7. call
     * removeDevice
     * @post_condition None
     * @expected Should not throw any exception
     * @see public static native void provisionInit(String dbPath) throws
     * OcException
     * @see public static List<OcSecureResource> discoverUnownedDevices(int
     * timeout) throws OcException
     * @see public static void setownershipTransferCBdata(OxmType type,
     * PinCallbackListener pinCallbackListener) throws OcException
     * @see public native void doOwnershipTransfer(DoOwnershipTransferListener
     * doOwnershipTransferListener) throws OcException
     * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
     * throws OcException
     * @see public void provisionPairwiseDevices(EnumSet <CredType> credTypeSet,
     * KeySize keysize, Object acl1, Object device2, Object acl2,
     * ProvisionPairwiseDevicesListener provisionPairwiseDevicesListener)
     * throws OcException
     * @since 2015-11-16
     */
    public void testRemoveDevice_LBV_P() {
        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
                OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.doOwnershipTransfer(mPMHelper));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ZERO);
        OcSecureResource ownedDevice02 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ONE);

        List<OicSecAcl> aclArrayList = mPMHelper.createAcl(ownedDevice01,
                ownedDevice02);

        assertTrue(PMConstants.mErrorMessage, mPMHelper.provisionPairwiseDevices(
                EnumSet.of(CredType.SYMMETRIC_PAIR_WISE_KEY),
                KeySize.OWNER_PSK_LENGTH_128, ownedDevice01, aclArrayList.get(0),
                ownedDevice02, aclArrayList.get(1), mPMHelper));

        assertTrue(PMConstants.mErrorMessage, mPMHelper.removeDevice(ownedDevice01,
                PMConstants.DISCOVERY_TIMEOUT_ONE, PMHelper.removeDeviceListener));
    }

    /**
     * @objective Provisioning Secured Devices and Unlink them with the Lower
     * Outer Boundary Value of timeout
     * @target public native void removeDevice(int timeout, RemoveDeviceListener
     * removeDeviceListener) throws OcException
     * @test_data timeout = 0 second
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
     * setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
     * discoverOwnedDevices 6. call provisionPairwiseDevices 7. call
     * removeDevice
     * @post_condition None
     * @expected Should throw INVALID_PARAM exception
     * @see public static native void provisionInit(String dbPath) throws
     * OcException
     * @see public static List<OcSecureResource> discoverUnownedDevices(int
     * timeout) throws OcException
     * @see public static void setownershipTransferCBdata(OxmType type,
     * PinCallbackListener pinCallbackListener) throws OcException
     * @see public native void doOwnershipTransfer(DoOwnershipTransferListener
     * doOwnershipTransferListener) throws OcException
     * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
     * throws OcException
     * @see public void provisionPairwiseDevices(EnumSet <CredType> credTypeSet,
     * KeySize keysize, Object acl1, Object device2, Object acl2,
     * ProvisionPairwiseDevicesListener provisionPairwiseDevicesListener)
     * throws OcException
     * @since 2015-11-16
     */
    public void testRemoveDevice_LOBV_ETC_N() {
        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
                OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.doOwnershipTransfer(mPMHelper));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ZERO);
        OcSecureResource ownedDevice02 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ONE);

        List<OicSecAcl> aclArrayList = mPMHelper.createAcl(ownedDevice01,
                ownedDevice02);

        assertTrue(PMConstants.mErrorMessage, mPMHelper.provisionPairwiseDevices(
                EnumSet.of(CredType.SYMMETRIC_PAIR_WISE_KEY),
                KeySize.OWNER_PSK_LENGTH_128, ownedDevice01, aclArrayList.get(0),
                ownedDevice02, aclArrayList.get(1), mPMHelper));

        assertFalse(PMConstants.mErrorMessage, mPMHelper.removeDevice(
                ownedDevice01, PMConstants.DISCOVERY_TIMEOUT_ZERO,
                PMHelper.removeDeviceListener));

        assertTrue(PMConstants.WRONG_ERROR_CODE,
                PMConstants.mErrorMessage.contains(PMConstants.ERROR_INVALID_PARAM));
    }

    /**
     * @objective Provisioning Secured Devices and Unlink them with
     * removeDeviceListener = null
     * @target public native void removeDevice(int timeout, RemoveDeviceListener
     * removeDeviceListener) throws OcException
     * @test_data removeDeviceListener = null
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
     * setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
     * discoverOwnedDevices 6. call provisionPairwiseDevices 7. call
     * removeDevice
     * @post_condition None
     * @expected Should throw INVALID_PARAM exception
     * @see public static native void provisionInit(String dbPath) throws
     * OcException
     * @see public static List<OcSecureResource> discoverUnownedDevices(int
     * timeout) throws OcException
     * @see public static void setownershipTransferCBdata(OxmType type,
     * PinCallbackListener pinCallbackListener) throws OcException
     * @see public native void doOwnershipTransfer(DoOwnershipTransferListener
     * doOwnershipTransferListener) throws OcException
     * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
     * throws OcException
     * @see public void provisionPairwiseDevices(EnumSet <CredType> credTypeSet,
     * KeySize keysize, Object acl1, Object device2, Object acl2,
     * ProvisionPairwiseDevicesListener provisionPairwiseDevicesListener)
     * throws OcException
     * @since 2015-11-16
     */
    public void testRemoveDeviceCB_NV_N() {

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
                OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.doOwnershipTransfer(mPMHelper));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ZERO);
        OcSecureResource ownedDevice02 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ONE);

        List<OicSecAcl> aclArrayList = mPMHelper.createAcl(ownedDevice01,
                ownedDevice02);

        assertTrue(PMConstants.mErrorMessage, mPMHelper.provisionPairwiseDevices(
                EnumSet.of(CredType.SYMMETRIC_PAIR_WISE_KEY),
                KeySize.OWNER_PSK_LENGTH_128, ownedDevice01, aclArrayList.get(0),
                ownedDevice02, aclArrayList.get(1), mPMHelper));

        assertFalse(PMConstants.mErrorMessage, mPMHelper.removeDevice(
                ownedDevice01, PMConstants.DISCOVERY_TIMEOUT_05, null));

        assertTrue(PMConstants.WRONG_ERROR_CODE,
                PMConstants.mErrorMessage.contains(PMConstants.ERROR_INVALID_CALLBACK));
    }

    /**
     * @objective Get List of device ID of devices linked with invoking device
     * @target public native List<String> getLinkedDevices()
     * @test_data None
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
     * setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
     * discoverOwnedDevices 6. call provisionPairwiseDevices 7. call
     * getLinkedDevices
     * @post_condition None
     * @expected Should not throw any exception
     * @see public static native void provisionInit(String dbPath) throws
     * OcException
     * @see public static List<OcSecureResource> discoverUnownedDevices(int
     * timeout) throws OcException
     * @see public static void setownershipTransferCBdata(OxmType type,
     * PinCallbackListener pinCallbackListener) throws OcException
     * @see public native void doOwnershipTransfer(DoOwnershipTransferListener
     * doOwnershipTransferListener) throws OcException
     * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
     * throws OcException
     * @see public void provisionPairwiseDevices(EnumSet <CredType> credTypeSet,
     * KeySize keysize, Object acl1, Object device2, Object acl2,
     * ProvisionPairwiseDevicesListener provisionPairwiseDevicesListener)
     * throws OcException
     * @since 2015-11-16
     **/
    public void testGetLinkedDevices_SRC_P() {
        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
                OxmType.OIC_JUST_WORKS, PMHelper.pinCallbackListener));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.doOwnershipTransfer(mPMHelper));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ZERO);
        OcSecureResource ownedDevice02 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ONE);

        List<OicSecAcl> aclArrayList = mPMHelper.createAcl(ownedDevice01,
                ownedDevice02);

        assertTrue(PMConstants.mErrorMessage, mPMHelper.provisionPairwiseDevices(
                EnumSet.of(CredType.SYMMETRIC_PAIR_WISE_KEY),
                KeySize.OWNER_PSK_LENGTH_128, ownedDevice01, aclArrayList.get(0),
                ownedDevice02, aclArrayList.get(1), mPMHelper));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.getLinkedDevices(ownedDevice01));
    }

    /**
     * @objective Get IP address of secure discovered device
     * @target String getIpAddr()
     * @test_data None
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverOwnedDevices 6. call
     * getIpAddr
     * @post_condition None
     * @expected Should not throw any exception
     * @see public static native void provisionInit(String dbPath) throws
     * OcException
     * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
     * throws OcException
     * @since 2015-11-16
     **/
    public void testGetIpAddr_SRC_P() {
        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ZERO);

        assertTrue(PMConstants.mErrorMessage, mPMHelper.getIpAddr(ownedDevice01));
    }

    /**
     * @objective Get device id of a device
     * @target String getDeviceID()
     * @test_data None
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverOwnedDevices 6. call
     * getDeviceID
     * @post_condition None
     * @expected Should not throw any exception
     * @see public static native void provisionInit(String dbPath) throws
     * OcException
     * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
     * throws OcException
     * @since 2015-11-16
     **/
    public void testGetDeviceID_SRC_P() {
        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ZERO);

        assertTrue(PMConstants.mErrorMessage, mPMHelper.getDeviceID(ownedDevice01));
    }

    /**
     * @objective Get device status (ON/OFF) of a device
     * @target DeviceStatus getDeviceStatus()
     * @test_data None
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverOwnedDevices 6. call
     * getDeviceStatus
     * @post_condition none
     * @expected Should return DeviceStatus.ON
     * @see public static native void provisionInit(String dbPath) throws
     * OcException
     * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
     * throws OcException
     * @since 2015-11-16
     **/
    public void testGetDeviceStatusOn_P() {
        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ZERO);

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.getDeviceStatus(ownedDevice01, DeviceStatus.ON));
    }

    /**
     * @objective Get device status (ON/OFF) of a device
     * @target DeviceStatus getDeviceStatus()
     * @test_data none
     * @pre_condition Initialize provisioning manager
     * @procedure Call getDeviceStatus() API
     * @post_condition none
     * @expected Should return DeviceStatus.OFF
     * @see public static native void provisionInit(String dbPath) throws
     * OcException
     * @since 2016-03-31
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
     * @objective Get device status (ON/OFF) of a device
     * @target DeviceStatus getDeviceStatus()
     * @test_data none
     * @pre_condition Initialize provisioning manager
     * @procedure Call getDeviceStatus() API
     * @post_condition none
     * @expected Should return DeviceStatus.INVALID
     * @see public static native void provisionInit(String dbPath) throws
     * OcException
     * @since 2016-03-31
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
     * @objective Get device ownership (OWNED/UNOWNED) status
     * @target OwnedStatus getOwnedStatus()
     * @test_data None
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverOwnedDevices 6. call
     * getOwnedStatus
     * @post_condition none
     * @expected Should return OwnedStatus.UNOWNED
     * @see public static native void provisionInit(String dbPath) throws
     * OcException
     * @see public static List<OcSecureResource> discoverUnownedDevices(int
     * timeout) throws OcException
     * @since 2015-11-16
     **/
    public void testGetOwnedStatusUnowned_SRC_P() {
        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        OcSecureResource unOwnedDevice01 = PMHelper.mUnownedDeviceList
                .get(PMConstants.DEVICE_INDEX_ZERO);

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.getOwnedStatus(unOwnedDevice01, OwnedStatus.UNOWNED));
    }

    /**
     * @objective Get device ownership (OWNED/UNOWNED) status
     * @target OwnedStatus getOwnedStatus()
     * @test_data None
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverOwnedDevices 6. call
     * getOwnedStatus
     * @post_condition none
     * @expected Should return OwnedStatus.OWNED
     * @see public static native void provisionInit(String dbPath) throws
     * OcException
     * @see public static List<OcSecureResource> discoverOwnedDevices(int timeout)
     * throws OcException
     * @since 2015-11-16
     **/
    public void testGetOwnedStatusOwned_SRC_P() {
        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ZERO);

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.getOwnedStatus(ownedDevice01, OwnedStatus.OWNED));
    }

    /**
     * @objective Get device ownership (OWNED/UNOWNED) status
     * @target OwnedStatus getOwnedStatus()
     * @test_data none
     * @pre_condition Initialize provisioning manager
     * @procedure Call getOwnedStatus() API
     * @post_condition none
     * @expected Should return OwnedStatus.INVALID
     * @see public static native void provisionInit(String dbPath) throws
     * OcException
     * @since 2015-03-31
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