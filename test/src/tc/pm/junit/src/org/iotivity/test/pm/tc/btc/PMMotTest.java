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

import org.iotivity.base.CredType;
import org.iotivity.base.DeviceStatus;
import org.iotivity.base.KeySize;
import org.iotivity.base.ModeType;
import org.iotivity.base.MomType;
import org.iotivity.base.OcException;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcSecureResource;
import org.iotivity.base.OicSecAcl;
import org.iotivity.base.OwnedStatus;
import org.iotivity.base.OxmType;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;

import org.iotivity.testcase.IoTivityLog;
import org.iotivity.testcase.IoTivityTc;
import org.iotivity.test.pm.tc.helper.PMHelper;
import org.iotivity.test.pm.tc.helper.PMConstants;
import org.iotivity.test.ri.common.RIHelperCommon;

public class PMMotTest extends IoTivityTc {
    public PMHelper       mPMHelper;

    // Variable for Creating Access Control List (ACL)
    static List<String>   mResources   = new ArrayList<String>();
    static List<String>   mPeriods     = new ArrayList<String>();
    static List<String>   mRecurrences = new ArrayList<String>();

    protected void setUp() throws Exception {
        super.setUp();
        mPMHelper = new PMHelper(this);
        mPMHelper.enableMocking();

        PMConstants.mErrorMessage = PMConstants.EMPTY_STRING;
        mPMHelper.clearAll();
        mPMHelper.stopServers();
        mPMHelper.startSecuredServer(mPMHelper.START_PRE_CONFIG_SERVER_01);
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
     * @objective Change MOT Mode with JUSTWORKS method
     * @target public void changeMOTMode(MomType momType, ChangeMOTModeListener
     *         changeMOTModeListener) throws OcException
     * @test_data Regular data for changeMOTMode
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
     *            setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
     *            discoverOwnedDevices
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
     * @since 2017-03-22
     */
    public void testChangeMOTMode_RV_P() {
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

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.changeMOTMode(ownedDevice01,
                        MomType.OIC_MULTIPLE_OWNER_ENABLE,
                        PMHelper.changeMOTModeListener));
    }

    /**
     * @objective Change MOT Mode with JUSTWORKS method as Disable Mode
     * @target public void changeMOTMode(MomType momType, ChangeMOTModeListener
     *         changeMOTModeListener) throws OcException
     * @test_data momType = OIC_MULTIPLE_OWNER_DISABLE
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
     *            setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
     *            discoverOwnedDevices
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
     * @since 2017-03-22
     */
    public void testChangeMOTModeDisable_RV_P() {
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

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.changeMOTMode(ownedDevice01,
                        MomType.OIC_MULTIPLE_OWNER_DISABLE,
                        PMHelper.changeMOTModeListener));
    }

    /**
     * @objective Change MOT Mode with JUSTWORKS method
     * @target public void changeMOTMode(MomType momType, ChangeMOTModeListener
     *         changeMOTModeListener) throws OcException
     * @test_data changeMOTModeListener = null
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
     *            setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
     *            discoverOwnedDevices
     * @post_condition None
     * @expected Should throw INVALID_CALLBACK exception
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
     * @since 2017-03-22
     */
    public void testChangeMOTModeCB_NV_N() {
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

        assertTrue(PMConstants.mErrorMessage, mPMHelper.changeMOTMode(
                ownedDevice01, MomType.OIC_MULTIPLE_OWNER_ENABLE, null));

        assertTrue(PMConstants.WRONG_ERROR_CODE, PMConstants.mErrorMessage
                .contains(PMConstants.ERROR_INVALID_CALLBACK));
    }

    /**
     * @objective Discover MOT Enable Devices
     * @target public static List<OcSecureResource>
     *         discoverMOTEnabledDevices(int timeout) throws OcException
     * @test_data Regular data for discoverMOTEnabledDevices
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
     *            setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
     *            discoverOwnedDevices 6. call changeMOTMode
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
     * @see public void changeMOTMode(MomType momType, ChangeMOTModeListener
     *      changeMOTModeListener) throws OcException
     * @since 2017-03-22
     */
    public void testDiscoverMOTEnabledDevices_RV_P() {
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

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.changeMOTMode(ownedDevice01,
                        MomType.OIC_MULTIPLE_OWNER_ENABLE,
                        PMHelper.changeMOTModeListener));

        assertTrue(PMConstants.mErrorMessage, mPMHelper
                .discoverMOTEnabledDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        assertTrue(PMConstants.ERROR_NOT_SUFFICIENT_DEVICES,
                PMHelper.mMOTEnableDeviceCount == PMConstants.EXPECTED_MOT_DEVICE_COUNT);
    }

    /**
     * @objective Discover MOT Enable Devices
     * @target public static List<OcSecureResource>
     *         discoverMOTEnabledDevices(int timeout) throws OcException
     * @test_data timeout = DISCOVERY_TIMEOUT_ONE
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
     *            setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
     *            discoverOwnedDevices 6. call changeMOTMode
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
     * @see public void changeMOTMode(MomType momType, ChangeMOTModeListener
     *      changeMOTModeListener) throws OcException
     * @since 2017-03-22
     */
    public void testDiscoverMOTEnabledDevices_LBV_P() {
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

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.changeMOTMode(ownedDevice01,
                        MomType.OIC_MULTIPLE_OWNER_ENABLE,
                        PMHelper.changeMOTModeListener));

        assertTrue(PMConstants.mErrorMessage, mPMHelper
                .discoverMOTEnabledDevices(PMConstants.DISCOVERY_TIMEOUT_ONE));

        assertTrue(PMConstants.ERROR_NOT_SUFFICIENT_DEVICES,
                PMHelper.mMOTEnableDeviceCount == PMConstants.EXPECTED_MOT_DEVICE_COUNT);
    }

    /**
     * @objective Discover MOT Enable Devices
     * @target public static List<OcSecureResource>
     *         discoverMOTEnabledDevices(int timeout) throws OcException
     * @test_data timeout = DISCOVERY_TIMEOUT_ZERO
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
     *            setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
     *            discoverOwnedDevices 6. call changeMOTMode
     * @post_condition None
     * @expected Should throw ErrorCode.INVALID_PARAM
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
     * @see public void changeMOTMode(MomType momType, ChangeMOTModeListener
     *      changeMOTModeListener) throws OcException
     * @since 2017-03-22
     */
    public void testDiscoverMOTEnabledDevices_LOBV_N() {
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

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.changeMOTMode(ownedDevice01,
                        MomType.OIC_MULTIPLE_OWNER_ENABLE,
                        PMHelper.changeMOTModeListener));

        assertTrue(PMConstants.mErrorMessage, mPMHelper
                .discoverMOTEnabledDevices(PMConstants.DISCOVERY_TIMEOUT_ZERO));

        assertTrue(PMConstants.WRONG_ERROR_CODE, PMConstants.mErrorMessage
                .equals(PMConstants.ERROR_INVALID_PARAM));
    }

    /**
     * @objective Provision MOT Enable Devices
     * @target public void provisionPreConfigPIN(String pin,
     *         ProvisionPreConfigPINListener provisionPreConfigPINListener)
     *         throws OcException
     * @test_data Regular data for provisionPreConfigPIN
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
     *            setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
     *            discoverOwnedDevices 6. call changeMOTMode 7. call
     *            discoverMOTEnableDevices
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
     * @see public void changeMOTMode(MomType momType, ChangeMOTModeListener
     *      changeMOTModeListener) throws OcException
     * @see public static List<OcSecureResource> discoverMOTEnabledDevices(int
     *      timeout) throws OcException
     * @since 2017-03-22
     */
    public void testProvisionPreConfigPIN_RV_P() {
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

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.changeMOTMode(ownedDevice01,
                        MomType.OIC_MULTIPLE_OWNER_ENABLE,
                        PMHelper.changeMOTModeListener));

        assertTrue(PMConstants.mErrorMessage, mPMHelper
                .discoverMOTEnabledDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        assertTrue(PMConstants.ERROR_NOT_SUFFICIENT_DEVICES,
                PMHelper.mMOTEnableDeviceCount == PMConstants.EXPECTED_MOT_DEVICE_COUNT);

        OcSecureResource motEnableDevice01 = PMHelper.mMOTEnableDeviceList
                .get(PMConstants.DEVICE_INDEX_ZERO);

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.provisionPreConfigPIN(motEnableDevice01,
                        PMConstants.PRE_CONFIG_PIN,
                        PMHelper.provisionPreConfigPINListener));
    }

    /**
     * @objective Provision MOT Enable Devices
     * @target public void provisionPreConfigPIN(String pin,
     *         ProvisionPreConfigPINListener provisionPreConfigPINListener)
     *         throws OcException
     * @test_data Regular data for provisionPreConfigPIN
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
     *            setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
     *            discoverOwnedDevices 6. call changeMOTMode 7. call
     *            discoverMOTEnableDevices
     * @post_condition None
     * @expected Should throw INVALID_CALLBACK exception
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
     * @see public void changeMOTMode(MomType momType, ChangeMOTModeListener
     *      changeMOTModeListener) throws OcException
     * @see public static List<OcSecureResource> discoverMOTEnabledDevices(int
     *      timeout) throws OcException
     * @since 2017-03-22
     */
    public void testProvisionPreConfigPINCB_NV_N() {
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

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.changeMOTMode(ownedDevice01,
                        MomType.OIC_MULTIPLE_OWNER_ENABLE,
                        PMHelper.changeMOTModeListener));

        assertTrue(PMConstants.mErrorMessage, mPMHelper
                .discoverMOTEnabledDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        assertTrue(PMConstants.ERROR_NOT_SUFFICIENT_DEVICES,
                PMHelper.mMOTEnableDeviceCount == PMConstants.EXPECTED_MOT_DEVICE_COUNT);

        OcSecureResource motEnableDevice01 = PMHelper.mMOTEnableDeviceList
                .get(PMConstants.DEVICE_INDEX_ZERO);

        assertTrue(PMConstants.mErrorMessage, mPMHelper.provisionPreConfigPIN(
                motEnableDevice01, PMConstants.PRE_CONFIG_PIN, null));

        assertTrue(PMConstants.WRONG_ERROR_CODE, PMConstants.mErrorMessage
                .contains(PMConstants.ERROR_INVALID_CALLBACK));
    }

    /**
     * @objective Select MOT Method
     * @target public void selectMOTMethod(OxmType oxmType,
     *         SelectOTMMethodListener selectOTMMethodListener) throws
     *         OcException
     * @test_data Regular data for selectMOTMethod
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
     *            setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
     *            discoverOwnedDevices 6. call changeMOTMode 7. call
     *            discoverMOTEnableDevices 8. call provisionPreConfigPIN
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
     * @see public void changeMOTMode(MomType momType, ChangeMOTModeListener
     *      changeMOTModeListener) throws OcException
     * @see public static List<OcSecureResource> discoverMOTEnabledDevices(int
     *      timeout) throws OcException
     * @see public void provisionPreConfigPIN(String pin,
     *      ProvisionPreConfigPINListener provisionPreConfigPINListener) throws
     *      OcException
     * @since 2017-03-22
     */
    public void testSelectMOTMethod_RV_P() {
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

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.changeMOTMode(ownedDevice01,
                        MomType.OIC_MULTIPLE_OWNER_ENABLE,
                        PMHelper.changeMOTModeListener));

        assertTrue(PMConstants.mErrorMessage, mPMHelper
                .discoverMOTEnabledDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        assertTrue(PMConstants.ERROR_NOT_SUFFICIENT_DEVICES,
                PMHelper.mMOTEnableDeviceCount == PMConstants.EXPECTED_MOT_DEVICE_COUNT);

        OcSecureResource motEnableDevice01 = PMHelper.mMOTEnableDeviceList
                .get(PMConstants.DEVICE_INDEX_ZERO);

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.provisionPreConfigPIN(motEnableDevice01,
                        PMConstants.PRE_CONFIG_PIN,
                        PMHelper.provisionPreConfigPINListener));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.selectMOTMethod(motEnableDevice01,
                        OxmType.OIC_PRECONFIG_PIN,
                        PMHelper.selectOTMMethodListener));
    }

    /**
     * @objective Select MOT Method
     * @target public void selectMOTMethod(OxmType oxmType,
     *         SelectOTMMethodListener selectOTMMethodListener) throws
     *         OcException
     * @test_data selectOTMMethodListener = null
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverUnownedDevices 3. call
     *            setownershipTransferCBdata 4. call doOwnershipTransfer 5. call
     *            discoverOwnedDevices 6. call changeMOTMode 7. call
     *            discoverMOTEnableDevices 8. call provisionPreConfigPIN
     * @post_condition None
     * @expected Should throw INVALID_CALLBACK exception
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
     * @see public void changeMOTMode(MomType momType, ChangeMOTModeListener
     *      changeMOTModeListener) throws OcException
     * @see public static List<OcSecureResource> discoverMOTEnabledDevices(int
     *      timeout) throws OcException
     * @see public void provisionPreConfigPIN(String pin,
     *      ProvisionPreConfigPINListener provisionPreConfigPINListener) throws
     *      OcException
     * @since 2017-03-22
     */
    public void testSelectMOTMethodCB_NV_N() {
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

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.changeMOTMode(ownedDevice01,
                        MomType.OIC_MULTIPLE_OWNER_ENABLE,
                        PMHelper.changeMOTModeListener));

        assertTrue(PMConstants.mErrorMessage, mPMHelper
                .discoverMOTEnabledDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        assertTrue(PMConstants.ERROR_NOT_SUFFICIENT_DEVICES,
                PMHelper.mMOTEnableDeviceCount == PMConstants.EXPECTED_MOT_DEVICE_COUNT);

        OcSecureResource motEnableDevice01 = PMHelper.mMOTEnableDeviceList
                .get(PMConstants.DEVICE_INDEX_ZERO);

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.provisionPreConfigPIN(motEnableDevice01,
                        PMConstants.PRE_CONFIG_PIN,
                        PMHelper.provisionPreConfigPINListener));

        assertTrue(PMConstants.mErrorMessage, mPMHelper.selectMOTMethod(
                motEnableDevice01, OxmType.OIC_PRECONFIG_PIN, null));

        assertTrue(PMConstants.WRONG_ERROR_CODE, PMConstants.mErrorMessage
                .contains(PMConstants.ERROR_INVALID_CALLBACK));
    }

    /**
     * @objective Add pre config pin
     * @target public void addPreConfigPIN(String pin) throws OcException
     * @test_data Regular data for addPreConfigPIN
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverMOTEnabledDevices
     * @post_condition None
     * @expected Should not throw any exception
     * @see public static native void provisionInit(String dbPath) throws
     *      OcException
     * @see public static List<OcSecureResource> discoverMOTEnabledDevices(int
     *      timeout) throws OcException
     * @since 2017-03-23
     */
    public void testAddPreConfigPINs_RV_P() {

        assertTrue(PMConstants.mErrorMessage, mPMHelper
                .discoverMOTEnabledDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        assertTrue(PMConstants.ERROR_NOT_SUFFICIENT_DEVICES,
                PMHelper.mMOTEnableDeviceCount == PMConstants.EXPECTED_MOT_DEVICE_COUNT);

        OcSecureResource motEnableDevice01 = PMHelper.mMOTEnableDeviceList
                .get(PMConstants.DEVICE_INDEX_ZERO);

        assertTrue(PMConstants.mErrorMessage, mPMHelper.addPreConfigPIN(
                motEnableDevice01, PMConstants.PRE_CONFIG_PIN));
    }

    /**
     * @objective Multiple OwnerShipTransfer
     * @target public native void doMultipleOwnershipTransfer(
     *         DoMultipleOwnershipTransferListener
     *         doMultipleOwnershipTransferListener)
     * @test_data Regular data for doMultipleOwnershipTransfer
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverMOTEnabledDevices 3.
     *            call addPreConfigPIN
     * @post_condition None
     * @expected Should not throw any exception
     * @see public static native void provisionInit(String dbPath) throws
     *      OcException
     * @see public static List<OcSecureResource> discoverMOTEnabledDevices(int
     *      timeout) throws OcException
     * @see public void addPreConfigPIN(String pin) throws OcException
     * @since 2017-03-23
     */
    public void testDoMultipleOwnershipTransfer_RV_P() {

        assertTrue(PMConstants.mErrorMessage, mPMHelper
                .discoverMOTEnabledDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        assertTrue(PMConstants.ERROR_NOT_SUFFICIENT_DEVICES,
                PMHelper.mMOTEnableDeviceCount == PMConstants.EXPECTED_MOT_DEVICE_COUNT);

        OcSecureResource motEnableDevice01 = PMHelper.mMOTEnableDeviceList
                .get(PMConstants.DEVICE_INDEX_ZERO);

        assertTrue(PMConstants.mErrorMessage, mPMHelper.addPreConfigPIN(
                motEnableDevice01, PMConstants.PRE_CONFIG_PIN));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.doMultipleOwnershipTransfer(mPMHelper));
    }

    /**
     * @objective Discover Multiple Enable Owned Device Discovery
     * @target public static List<OcSecureResource>
     *         discoverMOTEnabledOwnedDevices(int timeout) throws OcException
     * @test_data Regular data for doMultipleOwnershipTransfer
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit 2. call discoverMOTEnabledDevices 3.
     *            call addPreConfigPIN 4.call doMultipleOwnershipTransfer
     * @post_condition None
     * @expected Should not throw any exception
     * @see public static native void provisionInit(String dbPath) throws
     *      OcException
     * @see public static List<OcSecureResource> discoverMOTEnabledDevices(int
     *      timeout) throws OcException
     * @see public void addPreConfigPIN(String pin) throws OcException
     * @see public native void doMultipleOwnershipTransfer(
     *      DoMultipleOwnershipTransferListener
     *      doMultipleOwnershipTransferListener)
     * @since 2017-03-23
     */
    public void testDiscoverMOTEnabledOwnedDevices_RV_P() {

        assertTrue(PMConstants.mErrorMessage, mPMHelper
                .discoverMOTEnabledDevices(PMConstants.DISCOVERY_TIMEOUT_05));

        assertTrue(PMConstants.ERROR_NOT_SUFFICIENT_DEVICES,
                PMHelper.mMOTEnableDeviceCount == PMConstants.EXPECTED_MOT_DEVICE_COUNT);

        OcSecureResource motEnableDevice01 = PMHelper.mMOTEnableDeviceList
                .get(PMConstants.DEVICE_INDEX_ZERO);

        assertTrue(PMConstants.mErrorMessage, mPMHelper.addPreConfigPIN(
                motEnableDevice01, PMConstants.PRE_CONFIG_PIN));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.doMultipleOwnershipTransfer(mPMHelper));

        assertTrue(PMConstants.mErrorMessage,
                mPMHelper.discoverMOTEnabledOwnedDevices(
                        PMConstants.DISCOVERY_TIMEOUT_05));

        assertTrue(PMConstants.ERROR_NOT_SUFFICIENT_DEVICES,
                PMHelper.mMOTEnableOwnedDeviceCount == PMConstants.EXPECTED_MOT_DEVICE_COUNT);
    }
}
