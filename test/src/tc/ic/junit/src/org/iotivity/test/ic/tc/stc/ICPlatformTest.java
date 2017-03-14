/******************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
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
 **************************************************************** */

package org.iotivity.test.ic.tc.stc;

import java.util.EnumSet;
import java.util.List;
import java.util.ArrayList;

import android.test.InstrumentationTestCase;
import android.content.Context;
import android.test.AndroidTestCase;
import android.util.Log;

import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcConnectivityType;
import org.iotivity.base.OcException;
import org.iotivity.base.OcHeaderOption;
import org.iotivity.base.ErrorCode;
import org.iotivity.base.ModeType;
import org.iotivity.base.OcAccountManager;
import org.iotivity.base.OcPresenceHandle;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ResourceProperty;
import org.iotivity.base.ServiceType;
import org.iotivity.test.ic.tc.helper.ICHelper;
import org.iotivity.test.ic.tc.helper.ICResourceDirectoryCommonAdapter;
import org.iotivity.test.ic.tc.helper.OcAccountManagerAdapter;
import org.iotivity.test.ic.tc.helper.*;
import org.iotivity.cloud.*;
import org.iotivity.configuration.*;

import static org.iotivity.test.ic.tc.helper.ICHelperStaticUtil.*;
import static org.iotivity.test.ic.tc.helper.ICResourceDirectoryCommonAdapter.*;

public class ICPlatformTest extends InstrumentationTestCase
        implements IConfiguration {
    private static final String              ALL_INTERFACE_TYPE = "0.0.0.0";
    public OcAccountManager                  mAccountManager;
    private ICResourceDirectoryCommonAdapter mICResourceDirectoryCommonAdapter;
    private ICHelper                         mICHelper;
    private Context                          mContext;
    private OcPresenceHandle                 mOcPresenceHandle;
    private List<String>                     mDi;
    private OcAccountManagerHelper           mCloudHelper;

    protected void setUp() throws Exception {
        super.setUp();
        mICHelper = new ICHelper();
        mICResourceDirectoryCommonAdapter = new ICResourceDirectoryCommonAdapter();
        ICHelper.icConfigurePlatform(mContext);
        mOcPresenceHandle = null;
        mDi = new ArrayList<>();
        mCloudHelper = new OcAccountManagerHelper();
        PlatformConfig cfg = new PlatformConfig(
                getInstrumentation().getTargetContext(), ServiceType.IN_PROC,
                ModeType.CLIENT_SERVER, ALL_INTERFACE_TYPE, // bind to all
                                                            // available
                // interfaces
                0, QualityOfService.LOW);
        OcPlatform.Configure(cfg);
        mAccountManager = OcAccountManagerHelper
                .getAccountMangerInstance(TLS.DISABLED);
        OcAccountManagerHelper.init(mContext);
        // signUp and SignIn for all TC
        assertTrue(mCloudHelper.singUp(mAccountManager, DEFAULT_AUTH_PROVIDER,
                OcAccountManagerHelper.authCode, mCloudHelper));
        assertTrue(mCloudHelper.singIn(mAccountManager,
                OcAccountManagerHelper.s_CloudUid,
                OcAccountManagerHelper.s_CloudAccesstoken, mCloudHelper));
    }

    protected void tearDown() throws Exception {
        super.tearDown();
    }

    /**
     * @since 2017-02-22
     * @see public void signUp(String authProvider, String authCode,
     *      OnPostListener onPostListener)
     * @see public void signIn(String userUuid, String accessToken,
     *      OnPostListener onPostListener)
     * @see public void signOut(String accessToken, OnPostListener
     *      onPostListener)
     * @objective Test 'subscribeDevicePresence' API With valid scenario
     * @target "OCStackResult subscribeDevicePresence(OCPresenceHandle&
     *         presenceHandle, const std::string& host, const std::vector<std::string>& di,                            
     *                          OCConnectivityType connectivityType,            
     *                                          ObserveCallback callback);"
     * @test_data host, presenceHandle, di, connectivityType
     * @pre_condition constructAccountManagerObject(host, connectivity_type),
     *                SignUp(), SignIn() API
     * @procedure Called subscribeDevicePresence API
     * @post_condition None
     * @expected 'subscribeDevicePresence' API will provides OC_STACK_OK.
     */
    public void testsubscribeDevicePresence_SRC_P() {
        try {
            mOcPresenceHandle = OcPlatform.subscribeDevicePresence(
                    IC_HOST_ADDRESS, mDi,
                    EnumSet.of(OcConnectivityType.CT_ADAPTER_TCP),
                    mICResourceDirectoryCommonAdapter);
            assertTrue("observeGroup not worked",
                    ICResourceDirectoryCommonAdapter.sIsPlatformObserveCompleted);
            assertNotNull("mOcPresenceHandle is null", mOcPresenceHandle);
        } catch (Exception ex) {
            ex.printStackTrace();
            fail("SignUp API Exception occurred: " + ex.getLocalizedMessage());
        }
    }

    /**
     * @since 2017-02-06
     * @see public void signUp(String authProvider, String authCode,
     *      OnPostListener onPostListener)
     * @see public void signIn(String userUuid, String accessToken,
     *      OnPostListener onPostListener)
     * @see public void signOut(String accessToken, OnPostListener
     *      onPostListener)
     * @objective Test 'UnsubscribeDevicePresence' API With valid scenario
     * @target "OCStackResult unsubscribeDevicePresence(OCPresenceHandle&
     *         presenceHandle);"
     * @test_data host, presenceHandle, di, connectivityType
     * @pre_condition constructAccountManagerObject(host, connectivity_type),
     *                SignUp(), SignIn() API
     * @procedure Called unsubscribeDevicePresence API
     * @post_condition None
     * @expected 'unsubscribeDevicePresence' API will provides OC_STACK_OK.
     */
    public void testUnsubscribeDevicePresence_SRC_P() {
        try {
            mOcPresenceHandle = OcPlatform.subscribeDevicePresence(
                    IC_HOST_ADDRESS, mDi,
                    EnumSet.of(OcConnectivityType.CT_ADAPTER_TCP),
                    mICResourceDirectoryCommonAdapter);
            assertTrue("observeGroup not worked",
                    ICResourceDirectoryCommonAdapter.sIsPlatformObserveCompleted);
            assertNotNull("mOcPresenceHandle is null", mOcPresenceHandle);

            OcPlatform.unsubscribePresence(mOcPresenceHandle);
        } catch (Exception ex) {
            ex.printStackTrace();
            fail("SignUp API Exception occurred: " + ex.getLocalizedMessage());
        }
    }
}
