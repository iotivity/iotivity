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

import java.io.File;

import android.test.InstrumentationTestCase;
import android.util.Log;

import org.iotivity.base.ErrorCode;
import org.iotivity.base.ModeType;
import org.iotivity.base.OcException;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcProvisioning;
import org.iotivity.base.OxmType;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;

import org.iotivity.service.pm.test.helper.PMHelper;
import org.iotivity.service.pm.test.helper.PMConstants;

public class PMOcProvisioningTest extends InstrumentationTestCase {
    public PMHelper mPMHelper = new PMHelper();

    protected void setUp() throws Exception {
        super.setUp();
        System.setProperty("dexmaker.dexcache", getInstrumentation()
                .getTargetContext().getCacheDir().getPath());
        PMConstants.mErrorMessage = PMConstants.EMPTY_STRING;

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
     * @objective Discover Unowned Devices
     * @target List<OcSecureResource> discoverUnwnedDevices(int timeout)
     * @test_data none
     * @pre_condition Start Two JustWork Simulators and Initialize provisioning
     * manager
     * @procedure Call discoverUnownedDevices() API
     * @post_condition none
     * @expected Should not throw any exception
     * @see none
     * @since 2015-11-16
     */
    public void testDiscoverUnownedDevice_SRC_RV_P() {
        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_10));
    }

    /**
     * @objective Discover Unowned Devices with Lower Boundary value of time
     * @target List<OcSecureResource> discoverUnwnedDevices(int timeout)
     * @test_data timeout = 1
     * @pre_condition Start Two JustWork Simulators and Initialize provisioning
     * manager
     * @procedure Call discoverUnownedDevices() API
     * @post_condition none
     * @expected Should not throw any exception
     * @see none
     * @since 2015-11-16
     */
    public void testDiscoverUnownedDevice_LBV_P() {
        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_ONE));
    }

    /**
     * @objective Discover Unowned Devices with Lower Outer Boundary value of time
     * @target List<OcSecureResource> discoverUnwnedDevices(int timeout)
     * @test_data timeout = 0
     * @pre_condition Start Two JustWork Simulators and Initialize provisioning
     * manager
     * @procedure Call discoverUnownedDevices() API with negative time
     * @post_condition none
     * @expected Should throw ErrorCode.INVALID_PARAM
     * @see none
     * @since 2015-11-16
     */
    public void testDiscoverUnownedDevice_LOBV_ETC_N() {
        assertFalse(PMConstants.mErrorMessage,
                mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_ZERO));
    }

    /**
     * @objective Discover Owned Devices positively
     * @target List<OcSecureResource> discoverOwnedDevices(int timeout)
     * @test_data none
     * @pre_condition Start Two JustWork Simulators and Initialize provisioning
     * manager
     * @procedure Call discoverOwnedDevices() API
     * @post_condition none
     * @expected Should not throw any exception
     * @see none
     * @since 2015-11-16
     */
    public void testDiscoverOwnedDevice_SRC_RV_P() {
        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_10));
    }

    /**
     * @objective Discover Owned Devices positively with Lower Boundary Value of
     * time
     * @target List<OcSecureResource> discoverOwnedDevices(int timeout)
     * @test_data timeout = 1
     * @pre_condition Start Two JustWork Simulators and Initialize provisioning
     * manager
     * @procedure Call discoverOwnedDevices() API
     * @post_condition none
     * @expected Should not throw any exception
     * @see none
     * @since 2015-03-01
     */
    public void testDiscoverOwnedDevice_LBV_P() {
        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_ONE));
    }

    /**
     * @objective Discover Owned Devices with Lower Outer Boundary value of time
     * @target List<OcSecureResource> discoverOwnedDevices(int timeout)
     * @test_data timeout = 0
     * @pre_condition Start Two JustWork Simulators and Initialize provisioning
     * manager
     * @procedure Call discoverOwnedDevices() API with negative time
     * @post_condition none
     * @expected Should throw ErrorCode.INVALID_PARAM
     * @see none
     * @since 2015-11-16
     */
    public void testDiscoverOwnedDevice_LOBV_ETC_N() {
        assertFalse(PMConstants.mErrorMessage,
                mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_ZERO));
    }

    /**
     * @objective Set Ownership Transfer Callback Data
     * @target void SetownershipTransferCBdata(OxmType type, PinCallbackListener
     * pinCallbackListener)
     * @test_data type = OxmType.OIC_JUST_WORKS
     * @pre_condition Start Two JustWork Simulators and Initialize provisioning
     * manager
     * @procedure Call SetownershipTransferCBdata() API
     * @post_condition none
     * @expected Should not throw any exception
     * @see none
     * @since 2015-11-16
     */
    public void testSetownershipTransferCBdataOxm0_CLU_P() {
        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.setOwnershipTransferCBdata(OxmType.OIC_JUST_WORKS, null));
    }

    /**
     * @objective Set Ownership Transfer Callback Data
     * @target void SetownershipTransferCBdata(OxmType type, PinCallbackListener
     * pinCallbackListener)
     * @test_data type = OxmType.OIC_RANDOM_DEVICE_PIN
     * @pre_condition Start Two JustWork Simulators and Initialize provisioning
     * manager
     * @procedure Call SetownershipTransferCBdata() API
     * @post_condition none
     * @expected Should not throw any exception
     * @see none
     * @since 2016-03-31
     */
    public void testSetownershipTransferCBdataOxm1_CLU_P() {
        assertTrue(PMConstants.mErrorMessage, mPMHelper.setOwnershipTransferCBdata(
                OxmType.OIC_RANDOM_DEVICE_PIN, PMHelper.pinCallbackListener));
    }

    /**
     * @objective Set Ownership Transfer Callback Data
     * @target void SetownershipTransferCBdata(OxmType type, PinCallbackListener
     * pinCallbackListener)
     * @test_data type = OxmType.OIC_OXM_COUNT
     * @pre_condition Start Two JustWork Simulators and Initialize provisioning
     * manager
     * @procedure Call SetownershipTransferCBdata() API
     * @post_condition none
     * @expected Should throw ErrorCode.ERROR
     * @see none
     * @since 2016-03-31
     */
    public void testSetownershipTransferCBdataOxm3_CLU_N() {
        assertFalse(PMConstants.mErrorMessage,
                mPMHelper.setOwnershipTransferCBdata(OxmType.OIC_OXM_COUNT, null));
        assertEquals(PMConstants.WRONG_ERROR_CODE, ErrorCode.ERROR,
                PMConstants.mErrorMessage);
    }

    /**
     * @objective Set Ownership Transfer Callback Data
     * @target void SetownershipTransferCBdata(OxmType type, PinCallbackListener
     * pinCallbackListener)
     * @test_data none
     * @pre_condition Start Two JustWork Simulators and Initialize provisioning
     * manager
     * @procedure Call SetownershipTransferCBdata() API with null pinListener
     * @post_condition none
     * @expected Should throw ErrorCode.ERROR
     * @see none
     * @since 2015-11-16
     */
    public void testSetownershipTransferCB_NV_ETC_N() {
        assertFalse(PMConstants.mErrorMessage,
                mPMHelper.setOwnershipTransferCBdata(OxmType.OIC_RANDOM_DEVICE_PIN,
                        null));
        assertEquals(PMConstants.WRONG_ERROR_CODE, ErrorCode.ERROR.name(),
                PMConstants.mErrorMessage);
    }

    /**
     * @objective Get Device Status List positively
     * @target List<OcSecureResource> getDeviceStatusList(int timeout)
     * @test_data none
     * @pre_condition Start Two JustWork Simulators and Initialize provisioning
     * manager
     * @procedure Call getDeviceStatusList() API
     * @post_condition none
     * @expected Should not throw any exception
     * @see none
     * @since 2015-11-16
     */
    public void testGetDeviceStatusList_SRC_RV_P() {
        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.getDeviceStatusList(PMConstants.DISCOVERY_TIMEOUT_10));
    }

    /**
     * @objective Get Device Status List with Lower Boundary Value of time
     * @target List<OcSecureResource> getDeviceStatusList(int timeout)
     * @test_data timeout = 2
     * @pre_condition Start Two JustWork Simulators and Initialize provisioning
     * manager
     * @procedure Call getDeviceStatusList() API
     * @post_condition none
     * @expected Should not throw any exception
     * @see none
     * @since 2015-11-16
     */
    public void testGetDeviceStatusList_LBV_P() {
        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.getDeviceStatusList(PMConstants.DISCOVERY_TIMEOUT_TWO));
    }

    /**
     * @objective Get Device Status List with Lower Outer Boundary Value of time
     * @target List<OcSecureResource> getDeviceStatusList(int timeout)
     * @test_data timeout = 1
     * @pre_condition Start Two JustWork Simulators and Initialize provisioning
     * manager
     * @procedure Call getDeviceStatusList() API
     * @post_condition none
     * @expected Should throw ErrorCode.INVALID_PARAM
     * @see none
     * @since 2015-11-16
     */
    public void testGetDeviceStatusList_LOBV_ETC_N() {
        assertFalse(PMConstants.mErrorMessage,
                mPMHelper.getDeviceStatusList(PMConstants.DISCOVERY_TIMEOUT_ONE));
        assertEquals(PMConstants.WRONG_ERROR_CODE, ErrorCode.INVALID_PARAM.name(),
                PMConstants.mErrorMessage);
    }

    /**
     * @objective Set Display Pin Listener
     * @target setDisplayPinListener(DisplayPinListener displayPinListener)
     * @test_data none
     * @pre_condition Start Two JustWork Simulators and Initialize provisioning
     * manager
     * @procedure Call setDisplayPinListener() API
     * @post_condition none
     * @expected Should not throw any expection
     * @see none
     * @since 2016-03-01
     */
    public void testSetDisplayPinListener_SRC_P() {
        try {
            OcProvisioning.setDisplayPinListener(PMHelper.displayPinListener);
        } catch (OcException e) {
            assertTrue(false);
        }
    }

    /**
     * @objective Set Display Pin Listener
     * @target setDisplayPinListener(DisplayPinListener displayPinListener)
     * @test_data displayPinListener = Null
     * @pre_condition Start Two JustWork Simulators and Initialize provisioning
     * manager
     * @procedure Call setDisplayPinListener() API
     * @post_condition none
     * @expected Should throw ErrorCode.INVALID_PARAM
     * @see none
     * @since 2016-03-01
     */
    public void testSetDisplayPinListenerCB_NV_N() {
        try {
            OcProvisioning.setDisplayPinListener(null);
            fail();
        } catch (OcException e) {
            assertEquals(PMConstants.WRONG_ERROR_CODE, ErrorCode.INVALID_PARAM,
                    e.getErrorCode());
        }
    }
}