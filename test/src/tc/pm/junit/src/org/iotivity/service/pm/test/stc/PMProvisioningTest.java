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

package org.iotivity.service.pm.test.stc;

import java.io.File;
import java.util.ArrayList;
import java.util.EnumSet;
import java.util.List;

import android.test.InstrumentationTestCase;

import org.iotivity.base.CredType;
import org.iotivity.base.DeviceStatus;
import org.iotivity.base.KeySize;
import org.iotivity.base.ModeType;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcSecureResource;
import org.iotivity.base.OicSecAcl;
import org.iotivity.base.OwnedStatus;
import org.iotivity.base.OxmType;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;

import org.iotivity.service.pm.test.helper.PMHelper;
import org.iotivity.service.pm.test.helper.PMConstants;

public class PMProvisioningTest extends InstrumentationTestCase {

    private PMHelper mPMHelper = new PMHelper();
    private List<String> mResources = new ArrayList<String>();
    private List<String> mPeriods = new ArrayList<String>();
    private List<String> mRecurrences = new ArrayList<String>();

    protected void setUp() throws Exception {
        super.setUp();

        mRecurrences.clear();
        mResources.clear();
        mPeriods.clear();
        mRecurrences.add(PMConstants.DEFAULT_RECURRENCES);
        mResources.add(PMConstants.DEFAULT_RESOURCES);
        mPeriods.add(PMConstants.DEFAULT_PERIOD);

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

    /**
     * @since 2015-11-16
     * @see None
     * @objective Provisioning Two Secured Devices and Unlink them
     * @target public static native void provisionInit(String dbPath) throws
     *         OcException
     * @target public static List<OcSecureResource> discoverUnownedDevices(int
     *         timeout) throws OcException
     * @target public static void setownershipTransferCBdata(OxmType type,
     *         PinCallbackListener pinCallbackListener) throws OcException
     * @target public native void doOwnershipTransfer(DoOwnershipTransferListener
     *         doOwnershipTransferListener) throws OcException
     * @target public static List<OcSecureResource> discoverOwnedDevices(int
     *         timeout) throws OcException
     * @target public void provisionPairwiseDevices(EnumSet <CredType>
     *         credTypeSet, KeySize keysize, Object acl1, Object device2, Object
     *         acl2, ProvisionPairwiseDevicesListener
     *         provisionPairwiseDevicesListener) throws OcException
     * @target public native void unlinkDevices(Object device2,
     *         UnlinkDevicesListener unlinkDevicesListener) throws OcException
     * @target public native void removeDevice(int timeout, RemoveDeviceListener
     *         removeDeviceListener) throws OcException
     * @test_data None
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
     *            setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
     *            discoverOwnedDevices 6. call provisionPairwiseDevices 7. call
     *            unlinkDevices 8. call removeDevice
     * @post_condition None
     * @expected Should not throw any exception
     */
    public void testProvisioningManager_SQV_P() {
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

        List<OicSecAcl> aclArrayList = mPMHelper.createAcl(ownedDevice01,
                ownedDevice02);

        assertTrue(PMConstants.mErrorMessage, mPMHelper.provisionPairwiseDevices(
                EnumSet.of(CredType.SYMMETRIC_PAIR_WISE_KEY),
                KeySize.OWNER_PSK_LENGTH_128, ownedDevice01, aclArrayList.get(0),
                ownedDevice02, aclArrayList.get(1), mPMHelper));

        assertTrue(PMConstants.mErrorMessage, mPMHelper.unlinkDevices(
                ownedDevice01, ownedDevice02, PMHelper.unlinkDevicesListener));

        assertTrue(PMConstants.mErrorMessage, mPMHelper.removeDevice(ownedDevice01,
                PMConstants.DISCOVERY_TIMEOUT_10, PMHelper.removeDeviceListener));
    }

    /**
     * @since 2015-11-16
     * @see None
     * @objective Provisioning Credentials between two devices multiple times
     * @target public static native void provisionInit(String dbPath) throws
     *         OcException
     * @target public static List<OcSecureResource> discoverUnownedDevices(int
     *         timeout) throws OcException
     * @target public static void setownershipTransferCBdata(OxmType type,
     *         PinCallbackListener pinCallbackListener) throws OcException
     * @target public native void doOwnershipTransfer(DoOwnershipTransferListener
     *         doOwnershipTransferListener) throws OcException
     * @target public static List<OcSecureResource> discoverOwnedDevices(int
     *         timeout) throws OcException
     * @target public void provisionCredentials(EnumSet<CredType> credTypeSet,
     *         KeySize keysize, Object device2, ProvisionCredentialsListener
     *         provisionCredentialsListener) throws OcException
     * @test_data None
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
     *            setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
     *            discoverOwnedDevices 6. call provisionCredentials 7. call
     *            provisionCredentials
     * @post_condition None
     * @expected Should throw INVALID_PARAM exception
     */
    public void testProvisionCredentialsMultipleTime_EG_N() {
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

        assertFalse(PMConstants.mErrorMessage, mPMHelper.provisionCredentials(
                EnumSet.of(CredType.SYMMETRIC_PAIR_WISE_KEY),
                KeySize.OWNER_PSK_LENGTH_128, ownedDevice01, ownedDevice02,
                PMHelper.provisionCredentialsListener));

        assertTrue(PMConstants.WRONG_ERROR_CODE,
                PMConstants.mErrorMessage.contains(PMConstants.ERROR_INVALID_PARAM));
    }

    /**
     * @since 2015-11-16
     * @see None
     * @objective Getting linked devices of a certain secured device
     * @target public static native void provisionInit(String dbPath) throws
     *         OcException
     * @target public static List<OcSecureResource> discoverUnownedDevices(int
     *         timeout) throws OcException
     * @target public static void setownershipTransferCBdata(OxmType type,
     *         PinCallbackListener pinCallbackListener) throws OcException
     * @target public native void doOwnershipTransfer(DoOwnershipTransferListener
     *         doOwnershipTransferListener) throws OcException
     * @target public static List<OcSecureResource> discoverOwnedDevices(int
     *         timeout) throws OcException
     * @target public native List<String> getLinkedDevices()
     * @test_data None
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
     *            setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
     *            discoverOwnedDevices 6. call getLinkedDevices
     * @post_condition None
     * @expected Should not throw any exception
     */
    public void testGetLinkedDevice_SQV_P() {
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

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.getLinkedDevices(ownedDevice01));
    }

    /**
     * @since 2015-11-16
     * @see None
     * @objective ACL Provisioning of Resources
     * @target public static native void provisionInit(String dbPath) throws
     *         OcException
     * @target public static List<OcSecureResource> discoverUnownedDevices(int
     *         timeout) throws OcException
     * @target public static void setownershipTransferCBdata(OxmType type,
     *         PinCallbackListener pinCallbackListener) throws OcException
     * @target public native void doOwnershipTransfer(DoOwnershipTransferListener
     *         doOwnershipTransferListener) throws OcException
     * @target public static List<OcSecureResource> discoverOwnedDevices(int
     *         timeout) throws OcException
     * @target public native void provisionACL(Object acl, ProvisionAclListener
     *         provisionACLListener) throws OcException
     * @test_data None
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
     *            setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
     *            discoverOwnedDevices 5. call provisionACL 6. call provisionACL
     * @post_condition None
     * @expected Should not throw any exception
     */
    public void testProvisioningAclMultipleTime_SQV_P() {
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

        List<OicSecAcl> aclArrayList = mPMHelper.createAcl(ownedDevice01);

        assertTrue(PMConstants.mErrorMessage, mPMHelper.provisionACL(ownedDevice01,
                aclArrayList.get(0), PMHelper.provisionAclListener));

        assertTrue(PMConstants.mErrorMessage, mPMHelper.provisionACL(ownedDevice01,
                aclArrayList.get(0), PMHelper.provisionAclListener));
    }

    /**
     * @since 2015-11-16
     * @see None
     * @objective Get Secured Device's Status
     * @target public static native void provisionInit(String dbPath) throws
     *         OcException
     * @target public static List<OcSecureResource> discoverUnownedDevices(int
     *         timeout) throws OcException
     * @target public DeviceStatus getDeviceStatus() throws OcException
     * @test_data None
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
     *            getDeviceStatus
     * @post_condition None
     * @expected Should not throw any exception
     */
    public void testCheckUnownedDeviceStatus_P() {
        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

        assertTrue(PMConstants.mErrorMessage, mPMHelper.getDeviceStatus(
                PMHelper.mUnownedDeviceList.get(PMConstants.DEVICE_INDEX_ZERO),
                DeviceStatus.ON));
    }

    /**
     * @since 2015-11-16
     * @see None
     * @objective Get Secured Device's Owned Status
     * @target public static native void provisionInit(String dbPath) throws
     *         OcException
     * @target public static List<OcSecureResource> discoverUnownedDevices(int
     *         timeout) throws OcException
     * @target public OwnedStatus getOwnedStatus() throws OcException
     * @test_data None
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
     *            getOwnedStatus
     * @post_condition None
     * @expected Should not throw any exception
     */
    public void testCheckOwnedStatus_P() {
        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

        assertTrue(PMConstants.mErrorMessage, mPMHelper.getOwnedStatus(
                PMHelper.mUnownedDeviceList.get(PMConstants.DEVICE_INDEX_ZERO),
                OwnedStatus.UNOWNED));
    }

    /**
     * @since 2015-11-16
     * @see None
     * @objective Get Secured Device's ID and IP Address
     * @target public static native void provisionInit(String dbPath) throws
     *         OcException
     * @target public static List<OcSecureResource> discoverUnownedDevices(int
     *         timeout) throws OcException
     * @target public native String getDeviceID()
     * @target public native String getIpAddr()
     * @test_data None
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
     *            getDeviceID 4. call getIpAddr
     * @post_condition None
     * @expected Should not throw any exceptions
     */
    public void testGetIdandIpAddress_P() {
        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_10));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.getDeviceID(PMHelper.mUnownedDeviceList
                        .get(PMConstants.DEVICE_INDEX_ZERO)));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.getIpAddr(PMHelper.mUnownedDeviceList
                        .get(PMConstants.DEVICE_INDEX_ZERO)));
    }
}
