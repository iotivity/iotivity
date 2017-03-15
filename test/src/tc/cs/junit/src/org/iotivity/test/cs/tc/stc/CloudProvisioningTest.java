/******************************************************************
 * Copyright 2017 Samsung Electronics All Rights Reserved.
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

package org.iotivity.test.cs.tc.stc;

import android.content.Context;
import android.test.InstrumentationTestCase;
import android.util.Log;

import org.iotivity.base.ModeType;
import org.iotivity.base.OcAccountManager;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;
import org.iotivity.cloud.GetAuthCode;
import org.iotivity.configuration.IConfiguration;
import org.iotivity.cloud.OcAccountManagerHelper;
import org.iotivity.test.cs.tc.helper.CSConstants;
import org.iotivity.test.cs.tc.helper.OcCloudProvisioningHelper;

public class CloudProvisioningTest extends InstrumentationTestCase implements
        IConfiguration {

    private OcAccountManagerHelper    mCloudHelper = new OcAccountManagerHelper();
    private OcCloudProvisioningHelper mCsHelper    = new OcCloudProvisioningHelper();
    private OcAccountManager          mAccountManager;
    private Context context;
    protected void setUp() throws Exception {
        super.setUp();
        context = getInstrumentation().getContext();
        // create platform config
        mCsHelper.copyCborFromAsset(getInstrumentation().getContext(),
                CSConstants.OIC_CLIENT_CBOR_DB_FILE);
        OcCloudProvisioningHelper.mFilePath = getInstrumentation().getContext()
                .getFilesDir().getPath()
                + "/"; // data/data/<package>/files/
        PlatformConfig cfg = new PlatformConfig(getInstrumentation()
                .getTargetContext(), ServiceType.IN_PROC,
                ModeType.CLIENT_SERVER, "0.0.0.0", // bind to all available
                // interfaces
                0, QualityOfService.LOW, OcCloudProvisioningHelper.mFilePath
                        + CSConstants.OIC_CLOUD_CLIENT);

        OcPlatform.Configure(cfg);
        mAccountManager = OcAccountManagerHelper
                .getAccountMangerInstance(TLS.DISABLED);
        
        OcAccountManagerHelper.init(context);
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
     * @test_data None
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit
     * @post_condition None
     * @expected Should not throw any exception
     */
    public void testSignUp_SQV_P() {

        mCloudHelper.singUp(mAccountManager, DEFAULT_AUTH_PROVIDER,
                OcAccountManagerHelper.authCode, mCloudHelper);

        mCloudHelper.singIn(mAccountManager, OcAccountManagerHelper.s_CloudUid,
                OcAccountManagerHelper.s_CloudAccesstoken, mCloudHelper);

        mCloudHelper.singOut(mAccountManager,
                OcAccountManagerHelper.s_CloudAccesstoken, mCloudHelper);

    }
}