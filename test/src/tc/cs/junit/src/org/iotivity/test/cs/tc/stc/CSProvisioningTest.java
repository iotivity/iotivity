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
import java.util.ArrayList;

import org.iotivity.base.ModeType;
import org.iotivity.base.OcAccountManager;
import org.iotivity.base.OcCloudProvisioning;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;
import org.iotivity.configuration.IConfiguration;
import org.iotivity.cloud.OcAccountManagerHelper;
import org.iotivity.test.cs.tc.helper.CSConstants;
import org.iotivity.test.cs.tc.helper.OcCloudProvisioningHelper;

public class CSProvisioningTest extends InstrumentationTestCase implements
        IConfiguration {

    private OcAccountManagerHelper    mCloudHelper = new OcAccountManagerHelper();
    private OcCloudProvisioningHelper mOcCloudProvisioningHelper    = new OcCloudProvisioningHelper();
    private OcCloudProvisioning mOcCloudProvisioning = new OcCloudProvisioning(
            OcCloudProvisioningHelper.DEFAULT_HOST_IP,
            OcCloudProvisioningHelper.DEFAULT_PORT);
    private OcAccountManager mAccountManager;
    private Context context;

    protected void setUp() throws Exception {
        super.setUp();
        context = getInstrumentation().getContext();
        // create platform config
        mOcCloudProvisioningHelper.copyCborFromAsset(getInstrumentation().getContext(),
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
                .getAccountMangerInstance(TLS.ENABLED);
        OcAccountManagerHelper.init(context);
    }

    protected void tearDown() throws Exception {
        super.tearDown();
    }

    /**
     * @since 2017-04-02
     * @see public void signUp(String authProvider, String authCode, OcAccountManager.OnPostListener onPostListener) throws OcException
     * @see public void public void signIn(String userUuid, String accessToken, OcAccountManager.OnPostListener onPostListener) throws OcException
     * @see public public void signOut(String accessToken, OcAccountManager.OnPostListener onPostListener) throws OcException
     * @objective SignUp, SignIn and SignOut in Cloud
     * @target public void signUp(String authProvider, String authCode, OcAccountManager.OnPostListener onPostListener) throws OcException     	    * @test_data None
     * @pre_condition Start two JustWorks Server Manually
     * @procedure 1. call provisionInit
     * @post_condition None
     * @expected Should not throw any exception
     */
    public void testSignUp_SQV_P() {

        assertTrue(CSConstants.mErrorMessage, mCloudHelper.singUp(mAccountManager, DEFAULT_AUTH_PROVIDER,
                OcAccountManagerHelper.authCode, mCloudHelper));

        assertTrue(CSConstants.mErrorMessage, mCloudHelper.singIn(mAccountManager, OcAccountManagerHelper.s_CloudUid,
                OcAccountManagerHelper.s_CloudAccesstoken, mCloudHelper));

        assertTrue(CSConstants.mErrorMessage, mCloudHelper.singOut(mAccountManager,
                OcAccountManagerHelper.s_CloudAccesstoken, mCloudHelper));

    }

    /**
     * @since 2017-04-02
     * @see public void signUp(String authProvider, String authCode, OcAccountManager.OnPostListener onPostListener) throws OcException
     * @see public void public void signIn(String userUuid, String accessToken, OcAccountManager.OnPostListener onPostListener) throws OcException
     * @see public void signUp(String authProvider, String authCode, OcAccountManager.OnPostListener onPostListener) throws OcException 
     * @see public public void signOut(String accessToken, OcAccountManager.OnPostListener onPostListener) throws OcException
     * @objective SignUp, SignIn and SignOut in Cloud
     * @target public public void signUp(String authProvider, String authCode, OcAccountManager.OnPostListener onPostListener) throws OcException 
     * @test_data None
     * @pre_condition start rd, mq, account and ci server
     * @procedure 1. call provisionInit
     * @post_condition None
     * @expected Should not throw any exception
     */
    public void testRequestCertificate_SRC_RV_P() {

        assertTrue(CSConstants.mErrorMessage, mCloudHelper.singUp(mAccountManager, DEFAULT_AUTH_PROVIDER,
                OcAccountManagerHelper.authCode, mCloudHelper));

        assertTrue(CSConstants.mErrorMessage, mCloudHelper.singIn(mAccountManager, OcAccountManagerHelper.s_CloudUid,
                OcAccountManagerHelper.s_CloudAccesstoken, mCloudHelper));

 	assertTrue(CSConstants.mErrorMessage, mOcCloudProvisioningHelper
              .requestCertificate(mOcCloudProvisioning, mOcCloudProvisioningHelper));

        assertTrue(CSConstants.mErrorMessage, mCloudHelper.singOut(mAccountManager,
                OcAccountManagerHelper.s_CloudAccesstoken, mCloudHelper));

    }

    /**
     * @since 2017-04-02
     * @see public void signUp(String authProvider, String authCode, OcAccountManager.OnPostListener onPostListener) throws OcException
     * @see public void public void signIn(String userUuid, String accessToken, OcAccountManager.OnPostListener onPostListener) throws OcException
     * @see public void signUp(String authProvider, String authCode, OcAccountManager.OnPostListener onPostListener) throws OcException 
     * @see public public void signOut(String accessToken, OcAccountManager.OnPostListener onPostListener) throws OcException
     * @objective SignUp, SignIn and SignOut in Cloud
     * @target public void postCRL(String thisUpdate, String nextUpdate, String crl, ArrayList<String> serialNumbers, OcCloudProvisioning.PostCRLListener cloudPostCRLHandler) throws OcException
     * @test_data None
     * @pre_condition start rd, mq, account and ci server
     * @procedure 1. call provisionInit
     * @post_condition None
     * @expected Should not throw any exception
     */
    public void testPostCRL_SQV_P() {

        assertTrue(CSConstants.mErrorMessage, mCloudHelper.singUp(mAccountManager, DEFAULT_AUTH_PROVIDER,
                OcAccountManagerHelper.authCode, mCloudHelper));

        assertTrue(CSConstants.mErrorMessage, mCloudHelper.singIn(mAccountManager, OcAccountManagerHelper.s_CloudUid,
                OcAccountManagerHelper.s_CloudAccesstoken, mCloudHelper));

        ArrayList<String> serialNumbers = new ArrayList<String>();
        serialNumbers.add(CSConstants.CERT_SERIAL_ONE);
        assertTrue(CSConstants.mErrorMessage, mOcCloudProvisioningHelper.postCRL(mOcCloudProvisioning,
                OcCloudProvisioningHelper.thisUpdate,
                OcCloudProvisioningHelper.nextUpdate,
                CSConstants.DEFAULT_CRL, serialNumbers, mOcCloudProvisioningHelper));

        assertTrue(CSConstants.mErrorMessage, mCloudHelper.singOut(mAccountManager,
                OcAccountManagerHelper.s_CloudAccesstoken, mCloudHelper));

    }

    /**
     * @since 2017-04-02
     * @see public void signUp(String authProvider, String authCode, OcAccountManager.OnPostListener onPostListener) throws OcException
     * @see public void public void signIn(String userUuid, String accessToken, OcAccountManager.OnPostListener onPostListener) throws OcException
     * @see public void signUp(String authProvider, String authCode, OcAccountManager.OnPostListener onPostListener) throws OcException 
     * @see public public void signOut(String accessToken, OcAccountManager.OnPostListener onPostListener) throws OcException
     * @objective SignUp, SignIn and SignOut in Cloud
     * @target public native void getCRL(OcCloudProvisioning.GetCRLListener var1) throws OcException
     * @test_data None
     * @pre_condition start rd, mq, account and ci server
     * @procedure 1. call provisionInit
     * @post_condition None
     * @expected Should not throw any exception
     */
    public void testGetCRL_SQV_P() {

        assertTrue(CSConstants.mErrorMessage, mCloudHelper.singUp(mAccountManager, DEFAULT_AUTH_PROVIDER,
                OcAccountManagerHelper.authCode, mCloudHelper));

        assertTrue(CSConstants.mErrorMessage, mCloudHelper.singIn(mAccountManager, OcAccountManagerHelper.s_CloudUid,
                OcAccountManagerHelper.s_CloudAccesstoken, mCloudHelper));


        assertTrue(CSConstants.mErrorMessage, mOcCloudProvisioningHelper.getCRL(mOcCloudProvisioning, mOcCloudProvisioningHelper));

        assertTrue(CSConstants.mErrorMessage, mCloudHelper.singOut(mAccountManager,
                OcAccountManagerHelper.s_CloudAccesstoken, mCloudHelper));

    }

    /**
     * @since 2017-04-02
     * @see public void signUp(String authProvider, String authCode, OcAccountManager.OnPostListener onPostListener) throws OcException
     * @see public void public void signIn(String userUuid, String accessToken, OcAccountManager.OnPostListener onPostListener) throws OcException
     * @see public void signUp(String authProvider, String authCode, OcAccountManager.OnPostListener onPostListener) throws OcException 
     * @see public public void signOut(String accessToken, OcAccountManager.OnPostListener onPostListener) throws OcException
     * @objective SignUp, SignIn and SignOut in Cloud
     * @target public native void getAclIdByDevice(String var1, OcCloudProvisioning.GetAclIdByDeviceListener var2) throws OcException
     * @test_data None
     * @pre_condition start rd, mq, account and ci server
     * @procedure 1. call provisionInit
     * @post_condition None
     * @expected Should not throw any exception
     */
    public void testGetAclIdByDevice_SQV_P() {

        assertTrue(CSConstants.mErrorMessage, mCloudHelper.singUp(mAccountManager, DEFAULT_AUTH_PROVIDER,
                OcAccountManagerHelper.authCode, mCloudHelper));

        assertTrue(CSConstants.mErrorMessage, mCloudHelper.singIn(mAccountManager, OcAccountManagerHelper.s_CloudUid,
                OcAccountManagerHelper.s_CloudAccesstoken, mCloudHelper));

        assertTrue(CSConstants.mErrorMessage, mOcCloudProvisioningHelper.getAclIdByDevice(
              mOcCloudProvisioning,
              OcCloudProvisioningHelper.DEFAULT_DEVICE_ID, mOcCloudProvisioningHelper));

        assertTrue(CSConstants.mErrorMessage, mCloudHelper.singOut(mAccountManager,
                OcAccountManagerHelper.s_CloudAccesstoken, mCloudHelper));

    }

    /**
     * @since 2017-04-02
     * @see public void signUp(String authProvider, String authCode, OcAccountManager.OnPostListener onPostListener) throws OcException
     * @see public void public void signIn(String userUuid, String accessToken, OcAccountManager.OnPostListener onPostListener) throws OcException
     * @see public void signUp(String authProvider, String authCode, OcAccountManager.OnPostListener onPostListener) throws OcException 
     * @see public public void signOut(String accessToken, OcAccountManager.OnPostListener onPostListener) throws OcException
     * @objective SignUp, SignIn and SignOut in Cloud
     * @target public native void getIndividualAclInfo(String var1, OcCloudProvisioning.GetIndividualAclInfoListener var2)
     * @test_data None
     * @pre_condition start rd, mq, account and ci server
     * @procedure 1. call provisionInit
     * @post_condition None
     * @expected Should not throw any exception
     */
    public void testGetIndividualAclInfo_SQV_P() {

        assertTrue(CSConstants.mErrorMessage, mCloudHelper.singUp(mAccountManager, DEFAULT_AUTH_PROVIDER,
                OcAccountManagerHelper.authCode, mCloudHelper));

        assertTrue(CSConstants.mErrorMessage, mCloudHelper.singIn(mAccountManager, OcAccountManagerHelper.s_CloudUid,
                OcAccountManagerHelper.s_CloudAccesstoken, mCloudHelper));

        assertTrue(CSConstants.mErrorMessage, mOcCloudProvisioningHelper.getIndividualAclInfo(
              mOcCloudProvisioning,
              OcCloudProvisioningHelper.DEFAULT_ACL_ID, mOcCloudProvisioningHelper));

        assertTrue(CSConstants.mErrorMessage, mCloudHelper.singOut(mAccountManager,
                OcAccountManagerHelper.s_CloudAccesstoken, mCloudHelper));

    }
}
