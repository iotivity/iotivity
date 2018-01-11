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

package org.iotivity.test.pm.tc.btc;

import java.io.File;
import java.util.ArrayList;
import java.util.EnumSet;
import java.util.List;

import org.iotivity.base.ErrorCode;
import org.iotivity.base.ModeType;
import org.iotivity.base.OcException;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcProvisioning;
import org.iotivity.base.OcSecureResource;
import org.iotivity.base.OicSecAcl;
import org.iotivity.base.OxmType;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;
import org.iotivity.base.PinType;
import org.iotivity.base.MVJustWorksOptionMask;

import org.iotivity.test.pm.tc.helper.PMHelper;
import org.iotivity.test.pm.tc.helper.PMConstants;

import org.iotivity.testcase.IoTivityLog;
import org.iotivity.testcase.IoTivityTc;
import org.iotivity.test.pm.tc.helper.PMHelper;
import org.iotivity.test.pm.tc.helper.PMConstants;
import org.iotivity.test.ri.common.RIHelperCommon;

public class PMOcProvisioningTest extends IoTivityTc {
    public PMHelper mPMHelper;

    protected void setUp() throws Exception {
        super.setUp();
        mPMHelper = new PMHelper(this);
        mPMHelper.enableMocking();

        byte[] key = { (byte) 0xa5, (byte) 0x84, (byte) 0x99, (byte) 0x8d,
                (byte) 0x0d, (byte) 0xbd, (byte) 0xb1, (byte) 0x54, (byte) 0xbb,
                (byte) 0xc5, (byte) 0x4f, (byte) 0xed, (byte) 0x86, (byte) 0x9a,
                (byte) 0x66, (byte) 0x11, (byte) 0xa5, (byte) 0x84, (byte) 0x99,
                (byte) 0x8d, (byte) 0x0d, (byte) 0xbd, (byte) 0xb1, (byte) 0x54,
                (byte) 0xbb, (byte) 0xc5, (byte) 0x4f, (byte) 0xed, (byte) 0x86,
                (byte) 0x9a, (byte) 0x66, (byte) 0x11 };
        PMConstants.mErrorMessage = PMConstants.EMPTY_STRING;
        mPMHelper.clearAll();
        mPMHelper.stopServers();
        mPMHelper.startSecuredServer(mPMHelper.START_JUSTWORKS_SERVER_01);
        mPMHelper.startSecuredServer(mPMHelper.START_JUSTWORKS_SERVER_02);
        PMHelper.delay(5);
        // create platform config
        mPMHelper.copyCborFromAsset(PMConstants.OIC_CLIENT_CBOR_DB_FILE);
        mPMHelper.configClientServerPlatform(
                PMConstants.OIC_CLIENT_CBOR_DB_FILE);

        mPMHelper.initOICStack(PMHelper.s_sqLPath, PMConstants.OIC_SQL_DB_FILE);
    }

    protected void tearDown() throws Exception {
        mPMHelper.stopServers();
        mPMHelper.clearAll();
        super.tearDown();
    }

    /**
     * @since 2015-11-16
     * @see none
     * @objective Discover Unowned Devices
     * @target List<OcSecureResource> discoverUnwnedDevices(int timeout)
     * @test_data none
     * @pre_condition Start Two JustWork Simulators and Initialize provisioning
     *                manager
     * @procedure Call discoverUnownedDevices() API
     * @post_condition none
     * @expected Should not throw any exception
     */
    public void testDiscoverUnownedDevice_SRC_RV_P() {
        assertTrue(PMConstants.mErrorMessage, mPMHelper
                .discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_05));
    }

    /**
     * @since 2015-11-16
     * @see none
     * @objective Discover Unowned Devices with Lower Boundary value of time
     * @target List<OcSecureResource> discoverUnwnedDevices(int timeout)
     * @test_data timeout = 1
     * @pre_condition Start Two JustWork Simulators and Initialize provisioning
     *                manager
     * @procedure Call discoverUnownedDevices() API
     * @post_condition none
     * @expected Should not throw any exception
     */
    public void testDiscoverUnownedDevice_LBV_P() {
        assertTrue(PMConstants.mErrorMessage, mPMHelper
                .discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_ONE));
    }

    /**
     * @since 2015-11-16
     * @see none
     * @objective Discover Unowned Devices with Lower Outer Boundary value of
     *            time
     * @target List<OcSecureResource> discoverUnwnedDevices(int timeout)
     * @test_data timeout = 0
     * @pre_condition Start Two JustWork Simulators and Initialize provisioning
     *                manager
     * @procedure Call discoverUnownedDevices() API with negative time
     * @post_condition none
     * @expected Should throw ErrorCode.INVALID_PARAM
     */
    public void testDiscoverUnownedDevice_LOBV_ETC_N() {
        assertFalse(PMConstants.mErrorMessage, mPMHelper
                .discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_ZERO));
        assertTrue(PMConstants.WRONG_ERROR_CODE, PMConstants.mErrorMessage
                .equals(PMConstants.ERROR_INVALID_PARAM));

    }

    /**
     * @since 2015-11-16
     * @see none
     * @objective Discover Owned Devices positively
     * @target List<OcSecureResource> discoverOwnedDevices(int timeout)
     * @test_data none
     * @pre_condition Start Two JustWork Simulators and Initialize provisioning
     *                manager
     * @procedure Call discoverOwnedDevices() API
     * @post_condition none
     * @expected Should not throw any exception
     */
    public void testDiscoverOwnedDevice_SRC_RV_P() {
        assertTrue(PMConstants.mErrorMessage, mPMHelper
                .discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_05));
    }

    /**
     * @since 2015-03-01
     * @see none
     * @objective Discover Owned Devices positively with Lower Boundary Value of
     *            time
     * @target List<OcSecureResource> discoverOwnedDevices(int timeout)
     * @test_data timeout = 1
     * @pre_condition Start Two JustWork Simulators and Initialize provisioning
     *                manager
     * @procedure Call discoverOwnedDevices() API
     * @post_condition none
     * @expected Should not throw any exception
     */
    public void testDiscoverOwnedDevice_LBV_P() {
        assertTrue(PMConstants.mErrorMessage, mPMHelper
                .discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_ONE));
    }

    /**
     * @since 2015-11-16
     * @see none
     * @objective Discover Owned Devices with Lower Outer Boundary value of time
     * @target List<OcSecureResource> discoverOwnedDevices(int timeout)
     * @test_data timeout = 0
     * @pre_condition Start Two JustWork Simulators and Initialize provisioning
     *                manager
     * @procedure Call discoverOwnedDevices() API with negative time
     * @post_condition none
     * @expected Should throw ErrorCode.INVALID_PARAM
     */
    public void testDiscoverOwnedDevice_LOBV_ETC_N() {
        assertFalse(PMConstants.mErrorMessage, mPMHelper
                .discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_ZERO));
        assertTrue(PMConstants.WRONG_ERROR_CODE, PMConstants.mErrorMessage
                .equals(PMConstants.ERROR_INVALID_PARAM));
    }

    /**
     * @since 2015-11-16
     * @see none
     * @objective Set Ownership Transfer Callback Data
     * @target void SetownershipTransferCBdata(OxmType type, PinCallbackListener
     *         pinCallbackListener)
     * @test_data type = OxmType.OIC_JUST_WORKS
     * @pre_condition Start Two JustWork Simulators and Initialize provisioning
     *                manager
     * @procedure Call SetownershipTransferCBdata() API
     * @post_condition none
     * @expected Should not throw any exception
     */
    public void testSetownershipTransferCBdataOxm0_CLU_P() {
        assertTrue(PMConstants.mErrorMessage, mPMHelper
                .setOwnershipTransferCBdata(OxmType.OIC_JUST_WORKS, null));
    }

    /**
     * @since 2016-03-31
     * @see none
     * @objective Set Ownership Transfer Callback Data
     * @target void SetownershipTransferCBdata(OxmType type, PinCallbackListener
     *         pinCallbackListener)
     * @test_data type = OxmType.OIC_RANDOM_DEVICE_PIN
     * @pre_condition Start Two JustWork Simulators and Initialize provisioning
     *                manager
     * @procedure Call SetownershipTransferCBdata() API
     * @post_condition none
     * @expected Should not throw any exception
     */
    public void testSetownershipTransferCBdataOxm1_CLU_P() {
        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.setOwnershipTransferCBdata(
                        OxmType.OIC_RANDOM_DEVICE_PIN,
                        PMHelper.pinCallbackListener));
    }

    /**
     * @since 2016-03-31
     * @see none
     * @objective Set Ownership Transfer Callback Data
     * @target void SetownershipTransferCBdata(OxmType type, PinCallbackListener
     *         pinCallbackListener)
     * @test_data type = OxmType.OIC_OXM_COUNT
     * @pre_condition Start Two JustWork Simulators and Initialize provisioning
     *                manager
     * @procedure Call SetownershipTransferCBdata() API
     * @post_condition none
     * @expected Should throw ErrorCode.ERROR
     */
    public void testSetownershipTransferCBdataOxm3_CLU_N() {
        assertFalse(PMConstants.mErrorMessage, mPMHelper
                .setOwnershipTransferCBdata(OxmType.OIC_OXM_COUNT, null));
        assertTrue(PMConstants.WRONG_ERROR_CODE, PMConstants.mErrorMessage
                .contentEquals(PMConstants.ERROR_ERROR));
    }

    /**
     * @since 2015-11-16
     * @see none
     * @objective Set Ownership Transfer Callback Data
     * @target void SetownershipTransferCBdata(OxmType type, PinCallbackListener
     *         pinCallbackListener)
     * @test_data none
     * @pre_condition Start Two JustWork Simulators and Initialize provisioning
     *                manager
     * @procedure Call SetownershipTransferCBdata() API with null pinListener
     * @post_condition none
     * @expected Should throw ErrorCode.INVALID_CALLBACK
     */
    public void testSetownershipTransferCB_NV_ETC_N() {
        assertFalse(PMConstants.mErrorMessage,
                mPMHelper.setOwnershipTransferCBdata(
                        OxmType.OIC_RANDOM_DEVICE_PIN, null));
        assertEquals(PMConstants.ERROR_INVALID_CALLBACK,
                PMConstants.mErrorMessage);
    }

    /**
     * @since 2015-11-16
     * @see none
     * @objective Get Device Status List with Lower Boundary Value of time
     * @target List<OcSecureResource> getDeviceStatusList(int timeout)
     * @test_data timeout = 2
     * @pre_condition Start Two JustWork Simulators and Initialize provisioning
     *                manager
     * @procedure Call getDeviceStatusList() API
     * @post_condition none
     * @expected Should not throw any exception
     */
    public void testGetDeviceStatusList_SRC_RV_P() {
        assertTrue(PMConstants.mErrorMessage, mPMHelper
                .getDeviceStatusList(PMConstants.DISCOVERY_TIMEOUT_05));
    }

    /**
     * @since 2015-11-16
     * @see none
     * @objective Get Device Status List with Lower Boundary Value of time
     * @target List<OcSecureResource> getDeviceStatusList(int timeout)
     * @test_data timeout = 2
     * @pre_condition Start Two JustWork Simulators and Initialize provisioning
     *                manager
     * @procedure Call getDeviceStatusList() API
     * @post_condition none
     * @expected Should not throw any exception
     */
    public void testGetDeviceStatusList_LBV_P() {
        assertTrue(PMConstants.mErrorMessage, mPMHelper
                .getDeviceStatusList(PMConstants.DISCOVERY_TIMEOUT_TWO));
    }

    /**
     * @since 2015-11-16
     * @see none
     * @objective Get Device Status List with Lower Outer Boundary Value of time
     * @target List<OcSecureResource> getDeviceStatusList(int timeout)
     * @test_data timeout = 1
     * @pre_condition Start Two JustWork Simulators and Initialize provisioning
     *                manager
     * @procedure Call getDeviceStatusList() API
     * @post_condition none
     * @expected Should throw ErrorCode.INVALID_PARAM
     */
    public void testGetDeviceStatusList_LOBV_ETC_N() {
        assertFalse(PMConstants.mErrorMessage, mPMHelper
                .getDeviceStatusList(PMConstants.DISCOVERY_TIMEOUT_ONE));
        assertEquals(PMConstants.WRONG_ERROR_CODE,
                ErrorCode.INVALID_PARAM.name(), PMConstants.mErrorMessage);
    }

    /**
     * @since 2016-03-01
     * @see none
     * @objective Set Display Pin Listener
     * @target setDisplayPinListener(DisplayPinListener displayPinListener)
     * @test_data none
     * @pre_condition Start Two JustWork Simulators and Initialize provisioning
     *                manager
     * @procedure Call setDisplayPinListener() API
     * @post_condition none
     * @expected Should not throw any expection
     */
    public void testSetDisplayPinListener_SRC_P() {
        try {
            OcProvisioning.setDisplayPinListener(PMHelper.displayPinListener);
        } catch (OcException e) {
            assertTrue(false);
        }
    }

    /**
     * @since 2016-03-01
     * @see none
     * @objective Set Display Pin Listener
     * @target setDisplayPinListener(DisplayPinListener displayPinListener)
     * @test_data displayPinListener = Null
     * @pre_condition Start Two JustWork Simulators and Initialize provisioning
     *                manager
     * @procedure Call setDisplayPinListener() API
     * @post_condition none
     * @expected Should throw ErrorCode.INVALID_CALLBACK
     */
    public void testSetDisplayPinListenerCB_NV_N() {
        try {
            OcProvisioning.setDisplayPinListener(null);
            fail();
        } catch (OcException e) {
            assertEquals(PMConstants.ERROR_INVALID_CALLBACK,
                    e.getErrorCode().name());
        }
    }

    /**
     * @since 2017-03-20
     * @see none
     * @objective Set Display Num Listener
     * @target setDisplayNumListener(DisplayNumListener displayNumListener)
     * @test_data none
     * @pre_condition Start Two JustWork Simulators and Initialize provisioning
     *                manager
     * @procedure Call setDisplayNumListener() API
     * @post_condition none
     * @expected Should not throw any expection
     */
    public void testSetDisplayNumListener_SRC_P() {
        try {
            OcProvisioning.setDisplayNumListener(PMHelper.displayNumListener);
        } catch (OcException e) {
            assertTrue(false);
        }
    }

    /**
     * @since 2017-03-20
     * @see none
     * @objective Set Display Num Listener
     * @target setDisplayNumListener(DisplayNumListener displayNumListener)
     * @test_data displayNumListener = NULL
     * @pre_condition Start Two JustWork Simulators and Initialize provisioning
     *                manager
     * @procedure Call setDisplayNumListener() API
     * @post_condition none
     * @expected Should throw ErrorCode.INVALID_CALLBACK
     */
    public void testSetDisplayNumListener_NV_N() {
        try {
            OcProvisioning.setDisplayNumListener(null);
            fail();
        } catch (OcException e) {
            assertEquals(PMConstants.ERROR_INVALID_CALLBACK,
                    e.getErrorCode().name());
        }
    }

    /**
     * @since 2017-03-20
     * @see none
     * @objective Unset Display Num Listener
     * @target int unsetDisplayNumListener()
     * @test_data none
     * @pre_condition Start Two JustWork Simulators and Initialize provisioning
     *                manager
     * @procedure Call unsetDisplayNumListener() API
     * @post_condition none
     * @expected Should not throw any expection
     */
    public void testUnsetDisplayNumListener_SRC_P() {
        try {
            assertEquals(PMConstants.mErrorMessage, 0,
                    OcProvisioning.unsetDisplayNumListener());
        } catch (OcException e) {
            assertTrue(false);
        }
    }

    /**
     * @since 2017-03-20
     * @see none
     * @objective Set Confirm Num Listener
     * @target void setConfirmNumListener(ConfirmNumListener confirmNumListener)
     * @test_data none
     * @pre_condition Start Two JustWork Simulators and Initialize provisioning
     *                manager
     * @procedure Call setConfirmNumListener() API
     * @post_condition none
     * @expected Should not throw any expection
     */
    public void testConfirmNumListener_SRC_P() {
        try {
            OcProvisioning.setConfirmNumListener(PMHelper.confirmNumListener);
        } catch (OcException e) {
            assertTrue(false);
        }
    }

    /**
     * @since 2017-03-20
     * @see none
     * @objective Set Confirm Num Listener
     * @target void setConfirmNumListener(ConfirmNumListener confirmNumListener)
     * @test_data confirmNumListener = NULL
     * @pre_condition Start Two JustWork Simulators and Initialize provisioning
     *                manager
     * @procedure Call setConfirmNumListener() API
     * @post_condition none
     * @expected Should throw ErrorCode.INVALID_CALLBACK
     */
    public void testConfirmNumListenerCB_NV_N() {
        try {
            OcProvisioning.setConfirmNumListener(null);
            fail();
        } catch (OcException e) {
            assertEquals(PMConstants.ERROR_INVALID_CALLBACK,
                    e.getErrorCode().name());
        }
    }

    /**
     * @since 2017-03-20
     * @see none
     * @objective Unset Confirm Num Listener
     * @target int unsetConfirmNumListener()
     * @test_data none
     * @pre_condition Start Two JustWork Simulators and Initialize provisioning
     *                manager
     * @procedure Call unsetConfirmNumListener() API
     * @post_condition none
     * @expected Should not throw any expection
     */
    public void testUnsetConfirmNumListener_SRC_P() {
        try {
            assertEquals(PMConstants.mErrorMessage, 0,
                    OcProvisioning.unsetConfirmNumListener());
        } catch (OcException e) {
            assertTrue(false);
        }
    }

    /**
     * @since 2017-03-20
     * @see none
     * @objective Set Pin Type
     * @target int setPinType(int pinSize, EnumSet<PinType> pinType)
     * @test_data pinType = NUM_PIN
     * @pre_condition Start Two JustWork Simulators and Initialize provisioning
     *                manager
     * @procedure Call setPinType() API
     * @post_condition none
     * @expected Should not throw any expection
     */
    public void testSetPinTypeNumPin_SRC_P() {
        try {
            assertTrue(PMConstants.mErrorMessage,
                    mPMHelper.setPinType(PMConstants.RANDOM_PIN_DEFAULT_SIZE,
                            EnumSet.of(PinType.NUM_PIN)));
        } catch (Exception e) {
            assertTrue(false);
        }
    }

    /**
     * @since 2017-03-20
     * @see none
     * @objective Set Pin Type
     * @target int setPinType(int pinSize, EnumSet<PinType> pinType)
     * @test_data pinType = UPPERCASE_CHAR_PIN
     * @pre_condition Start Two JustWork Simulators and Initialize provisioning
     *                manager
     * @procedure Call setPinType() API
     * @post_condition none
     * @expected Should not throw any expection
     */
    public void testSetPinTypeUppercaseCharPin_SRC_P() {
        try {
            assertTrue(PMConstants.mErrorMessage,
                    mPMHelper.setPinType(PMConstants.RANDOM_PIN_DEFAULT_SIZE,
                            EnumSet.of(PinType.UPPERCASE_CHAR_PIN)));
        } catch (Exception e) {
            assertTrue(false);
        }
    }

    /**
     * @since 2017-03-20
     * @see none
     * @objective Set Pin Type
     * @target int setPinType(int pinSize, EnumSet<PinType> pinType)
     * @test_data pinType = LOWERCASE_CHAR_PIN
     * @pre_condition Start Two JustWork Simulators and Initialize provisioning
     *                manager
     * @procedure Call setPinType() API
     * @post_condition none
     * @expected Should not throw any expection
     */
    public void testSetPinTypeLowercaseCharPin_SRC_P() {
        try {
            assertTrue(PMConstants.mErrorMessage,
                    mPMHelper.setPinType(PMConstants.RANDOM_PIN_DEFAULT_SIZE,
                            EnumSet.of(PinType.LOWERCASE_CHAR_PIN)));
        } catch (Exception e) {
            assertTrue(false);
        }
    }

    /**
     * @since 2017-03-20
     * @see none
     * @objective Set Mutual Verified Just-works option
     * @target int setMVJustWorksOptions(EnumSet<MVJustWorksOptionMask>
     *         optionMask)
     * @test_data optionMask = NOT_APPLICABLE
     * @pre_condition Start Two JustWork Simulators and Initialize provisioning
     *                manager
     * @procedure Call setMVJustWorksOptions() API
     * @post_condition none
     * @expected Should not throw any expection
     */
    public void testSetMVJustWorksOptionsNotApplicable_SRC_P() {
        try {
            assertTrue(PMConstants.mErrorMessage,
                    mPMHelper.setMVJustWorksOptions(
                            EnumSet.of(MVJustWorksOptionMask.NOT_APPLICABLE)));
        } catch (Exception e) {
            assertTrue(false);
        }
    }

    /**
     * @since 2017-03-20
     * @see none
     * @objective Set Mutual Verified Just-works option
     * @target int setMVJustWorksOptions(EnumSet<MVJustWorksOptionMask>
     *         optionMask)
     * @test_data optionMask = DISPLAY_MUTUAL_VERIF_NUM
     * @pre_condition Start Two JustWork Simulators and Initialize provisioning
     *                manager
     * @procedure Call setMVJustWorksOptions() API
     * @post_condition none
     * @expected Should not throw any expection
     */
    public void testSetMVJustWorksOptionsDisplayMVNum_SRC_P() {
        try {
            assertTrue(PMConstants.mErrorMessage,
                    mPMHelper.setMVJustWorksOptions(EnumSet.of(
                            MVJustWorksOptionMask.DISPLAY_MUTUAL_VERIF_NUM)));
        } catch (Exception e) {
            assertTrue(false);
        }
    }

    /**
     * @since 2017-03-20
     * @see none
     * @objective Set Mutual Verified Just-works option
     * @target int setMVJustWorksOptions(EnumSet<MVJustWorksOptionMask>
     *         optionMask)
     * @test_data optionMask = CONFIRM_MUTUAL_VERIF_NUM
     * @pre_condition Start Two JustWork Simulators and Initialize provisioning
     *                manager
     * @procedure Call setMVJustWorksOptions() API
     * @post_condition none
     * @expected Should not throw any expection
     */
    public void testSetMVJustWorksOptionsConfirmMVNum_SRC_P() {
        try {
            assertTrue(PMConstants.mErrorMessage,
                    mPMHelper.setMVJustWorksOptions(EnumSet.of(
                            MVJustWorksOptionMask.CONFIRM_MUTUAL_VERIF_NUM)));
        } catch (Exception e) {
            assertTrue(false);
        }
    }

    /**
     * @objective Test saveACL positively
     * @target public static native void saveACL(Object acl) throws OcException
     * @test_data Regular data for saveACL
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
     *            setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
     *            discoverOwnedDevices 5. call provisionACL
     * @post_condition None
     * @expected Should not throw any exception
     * @see public static native void provisionInit(String dbPath) throws
     *      OcException
     * @see public static List<OcSecureResource> discoverUnownedDevices(int
     *      timeout) throws OcException
     * @see public static void setownershipTransferCBdata(OxmType type,
     *      PinCallbackListener pinCallbackListener) throws OcException
     * @see public native void doOwnershipTransfer(DoOwnershipTransferListener
     *      doOwnershipTransferListener) throws OcException
     * @see public static List<OcSecureResource> discoverOwnedDevices(int
     *      timeout) throws OcException
     * @see provisionACL(Object acl, ProvisionAclListener provisionACLListener)
     * @since 2017-03-23
     **/
    public void testSaveACL_SRC_RV_P() {
        assertTrue(PMConstants.mErrorMessage, mPMHelper
                .discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_05));
        assertTrue(PMConstants.ERROR_NOT_SUFFICIENT_DEVICES,
                PMHelper.mUnownedDeviceCount == PMConstants.EXPECTED_DEVICE_COUNT);

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.setOwnershipTransferCBdata(OxmType.OIC_JUST_WORKS,
                        PMHelper.pinCallbackListener));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.doOwnershipTransfer(mPMHelper));

        assertTrue(PMConstants.mErrorMessage, mPMHelper
                .discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_05));
        assertTrue(PMConstants.ERROR_NOT_SUFFICIENT_DEVICES,
                PMHelper.mOwnedDeviceCount == PMConstants.EXPECTED_DEVICE_COUNT);

        OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ZERO);

        List<OicSecAcl> aclArrayList = mPMHelper.createAcl(ownedDevice01);

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.saveACL(aclArrayList.get(0)));

    }
}
