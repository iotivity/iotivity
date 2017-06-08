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
 ******************************************************************/

package org.iotivity.test.ic.tc.stc;

import java.util.ArrayList;
import java.util.EnumSet;
import java.util.List;

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
import org.iotivity.cloud.*;
import org.iotivity.configuration.*;
import org.iotivity.test.ic.tc.helper.ICHelper;
import org.iotivity.test.ic.tc.helper.ICResourceDirectoryCommonAdapter;
import org.iotivity.test.ic.tc.helper.OcAccountManagerAdapter;
import org.iotivity.test.ic.tc.helper.*;
import static org.iotivity.test.ic.tc.helper.ICHelperStaticUtil.*;

public class ICAccountMangerTest extends InstrumentationTestCase
        implements IConfiguration {
    private OcAccountManagerAdapter    mOcAccountManagerAdapter;
    private OconGetGroupInfoAllAdapter mOconGetGroupInfoAll;
    private ICHelper                   mICHelper;
    private Context                    mContext;
    private OcAccountManagerHelper     mCloudHelper  = new OcAccountManagerHelper();
    private OcAccountManager           mAccountManager;
    private OcRepresentation           mPropertyValue;
    private String[]                   mValues       = { VALUE };
    private String[]                   mModifyvalues = { "Iotivity" };
    

    protected void setUp() throws Exception {
        super.setUp();
        mICHelper = new ICHelper();
        mContext = getInstrumentation().getContext();
        mOconGetGroupInfoAll = new OconGetGroupInfoAllAdapter();

        mOcAccountManagerAdapter = new OcAccountManagerAdapter();
        mPropertyValue = new OcRepresentation();
        
        mICHelper.copyCborFromAsset(getInstrumentation().getContext(),
                "cloud.dat");

        ICHelper.filePath = getInstrumentation().getContext()
                .getFilesDir().getPath()
                + "/"; // data/data/<package>/files/

        PlatformConfig cfg = new PlatformConfig(
                getInstrumentation().getTargetContext(), ServiceType.IN_PROC,
                ModeType.CLIENT_SERVER, "0.0.0.0", // bind to all available
                // interfaces
                0, QualityOfService.LOW, ICHelper.filePath + "cloud.dat");
        OcPlatform.Configure(cfg);

        mAccountManager = OcAccountManagerHelper
                .getAccountMangerInstance(TLS.DISABLED);
        OcAccountManagerHelper.init(mContext);
    }

    protected void tearDown() throws Exception {
        super.tearDown();
        assertTrue(mCloudHelper.singOut(mAccountManager,
                OcAccountManagerHelper.s_CloudAccesstoken, mCloudHelper));
    }

    /**
     * @since 2017-02-15
     * @see none
     * @objective test publishResourceToRD API Positively
     * @target publishResourceToRD(java.lang.String host,
     *         java.util.EnumSet<OcConnectivityType> connectivityTypeSet,
     *         OcPlatform.OnPublishResourceListener onPublishResourceListener)
     * @test_data 1. host Host address 2. connectivityTypeSet CT_DEFAULT as
     *            connectivity 3. onPublishResourceListener callback
     * @pre_condition none
     * @procedure 1. Call publishResourceToRD API 2. Check it's return value
     * @post_condition none
     * @expected It will Publish Virtual Resource to Resource Directory & return
     *           OK
     */
    public void testSignInSignOut_SRC_FSV_P() {

        assertTrue(mCloudHelper.singUp(mAccountManager, DEFAULT_AUTH_PROVIDER,
                OcAccountManagerHelper.authCode, mCloudHelper));

        assertTrue(mCloudHelper.singIn(mAccountManager,
                OcAccountManagerHelper.s_CloudUid,
                OcAccountManagerHelper.s_CloudAccesstoken, mCloudHelper));
    }

    /**
     * @since 2017-02-15
     * @see public void signUp(String authProvider, String authCode,
     *      OnPostListener onPostListener)
     * @see public void signIn(String userUuid, String accessToken,
     *      OnPostListener onPostListener)
     * @see public void signOut(String accessToken, OnPostListener
     *      onPostListener)
     * @objective Test 'searchUser' API With valid scenario
     * @target string searchUser() API
     * @test_data None
     * @pre_condition constructAccountManagerObject(host, connectivity_type),
     *                SignUp(), SignIn() API
     * @procedure Call searchUser with valid ID
     * @post_condition SignOut()
     * @expected 'SearchUser' API works properly without exception.
     */
    public void testSearchUserWithUID_SRC_P() {
        assertTrue(mCloudHelper.singUp(mAccountManager, DEFAULT_AUTH_PROVIDER,
                OcAccountManagerHelper.authCode, mCloudHelper));
        assertTrue(mCloudHelper.singIn(mAccountManager,
                OcAccountManagerHelper.s_CloudUid,
                OcAccountManagerHelper.s_CloudAccesstoken, mCloudHelper));

        try {
            MY_MAP.clear();
            MY_MAP.put("userid", OcAccountManagerHelper.s_CloudUid);

            mAccountManager.searchUser(MY_MAP, mOcAccountManagerAdapter);
            Thread.sleep(10000);
            assertTrue("search not working",
                    OcAccountManagerAdapter.onGetCompletedOk);
        } catch (Exception ex) {
            ex.printStackTrace();
            fail("SearchUser API Exception occurred: "
                    + ex.getLocalizedMessage());
        }
    }

    /**
     * @since 2017-02-15
     * @see public void signUp(String authProvider, String authCode,
     *      OnPostListener onPostListener)
     * @see public void signIn(String userUuid, String accessToken,
     *      OnPostListener onPostListener)
     * @see public void signOut(String accessToken, OnPostListener
     *      onPostListener)
     * @objective Test 'searchUser' API With valid scenario
     * @target string searchUser() API
     * @test_data None
     * @pre_condition constructAccountManagerObject(host, connectivity_type),
     *                SignUp(), SignIn() API
     * @procedure Call searchUser with valid ID
     * @post_condition SignOut()
     * @expected 'SearchUser' API works properly without exception.
     */
    public void testSearchUserWithGroupMasterID_SRC_P() {
        assertTrue(mCloudHelper.singUp(mAccountManager, DEFAULT_AUTH_PROVIDER,
                OcAccountManagerHelper.authCode, mCloudHelper));
        assertTrue(mCloudHelper.singIn(mAccountManager,
                OcAccountManagerHelper.s_CloudUid,
                OcAccountManagerHelper.s_CloudAccesstoken, mCloudHelper));
        MY_MAP.clear();
        MY_MAP.put("uid", OcAccountManagerHelper.s_CloudUid); 
        try {
            mAccountManager.searchUser(MY_MAP, mOcAccountManagerAdapter);
            Thread.sleep(5000);
            assertTrue("search not working",
                    OcAccountManagerAdapter.onGetCompletedOk);
        } catch (Exception ex) {
            ex.printStackTrace();
            fail("SignUp API Exception occurred: " + ex.getLocalizedMessage());
        }
    }

    /**
     * @since 2017-02-15
     * @see public void signUp(String authProvider, String authCode,
     *      OnPostListener onPostListener)
     * @see public void signIn(String userUuid, String accessToken,
     *      OnPostListener onPostListener)
     * @see public void signOut(String accessToken, OnPostListener
     *      onPostListener)
     * @objective Test 'searchUser' API With valid scenario
     * @target string searchUser() API
     * @test_data None
     * @pre_condition constructAccountManagerObject(host, connectivity_type),
     *                SignUp(), SignIn() API
     * @procedure Perform string host() API
     * @post_condition SignOut()
     * @expected 'SearchUser' API works properly without exception.
     */
    public void testSearchUserWithEmptyUID_ESV_N() {
        assertTrue(mCloudHelper.singUp(mAccountManager, DEFAULT_AUTH_PROVIDER,
                OcAccountManagerHelper.authCode, mCloudHelper));

        assertTrue(mCloudHelper.singIn(mAccountManager,
                OcAccountManagerHelper.s_CloudUid,
                OcAccountManagerHelper.s_CloudAccesstoken, mCloudHelper));
        MY_MAP.put("uid", "");
        try {
            mAccountManager.searchUser(MY_MAP, mOcAccountManagerAdapter);
            assertFalse("search not working",
                    OcAccountManagerAdapter.onGetCompletedOk);
        } catch (Exception ex) {
            ex.printStackTrace();
            fail("SignUp API Exception occurred: " + ex.getLocalizedMessage());
        }
    }

    /**
     * @since 2017-01-31
     * @see public void signUp(String authProvider, String authCode,
     *      OnPostListener onPostListener)
     * @see public void signIn(String userUuid, String accessToken,
     *      OnPostListener onPostListener)
     * @see public void signOut(String accessToken, OnPostListener
     *      onPostListener)
     * @objective Test 'createGroup' API With valid scenario
     * @target string createGroup() API
     * @test_data cloudConnectGetHandler
     * @pre_condition constructAccountManagerObject(host, connectivity_type),
     *                SignUp(), SignIn() API
     * @procedure 1. Perform string createGroup() API 2. Check createGroup works
     *            properly
     * @post_condition SignOut()
     * @expected 'createGroup' API works properly without exception.
     */
    public void testCreateAndGetGroupInfoAll_SRC_P() {
        assertTrue(mCloudHelper.singUp(mAccountManager, DEFAULT_AUTH_PROVIDER,
                OcAccountManagerHelper.authCode, mCloudHelper));
        assertTrue(mCloudHelper.singIn(mAccountManager,
                OcAccountManagerHelper.s_CloudUid,
                OcAccountManagerHelper.s_CloudAccesstoken, mCloudHelper));
        try {
            mAccountManager.createGroup(mOcAccountManagerAdapter);
            Thread.sleep(10000);
            assertNotNull("groupID not found after create group",
                    OcAccountManagerAdapter.sGroupId);
            
            OcAccountManagerAdapter.sGroupId = null;
            mAccountManager.getGroupInfoAll(mOconGetGroupInfoAll);
            Thread.sleep(5000);
            assertNotNull("groupID not found with getGroupInfoAll API",
                    OconGetGroupInfoAllAdapter.sGroupId);
        } catch (Exception ex) {
            ex.printStackTrace();
            fail("createGroup Exception occurred: " + ex.getLocalizedMessage());
        }
    }

    /**
     * @since 2017-01-31
     * @see public void signUp(String authProvider, String authCode,
     *      OnPostListener onPostListener)
     * @see public void signIn(String userUuid, String accessToken,
     *      OnPostListener onPostListener)
     * @see public void signOut(String accessToken, OnPostListener
     *      onPostListener)
     * @objective Test 'createGroup' API With valid scenario
     * @target string createGroup() API
     * @test_data cloudConnectGetHandler
     * @pre_condition constructAccountManagerObject(host, connectivity_type),
     *                SignUp(), SignIn() API
     * @procedure 1. Perform string createGroup() API 2. Perform getGroupInfo()
     *            API 3. Perform getGroupInfoAll() API 4. Check ALL APIs work
     *            properly
     * @post_condition SignOut()
     * @expected 'createGroup' and getGroupInfo API works properly without
     *           exception.
     */
    public void testCreateAndGetGroup_SRC_P() {
        assertTrue(mCloudHelper.singUp(mAccountManager, DEFAULT_AUTH_PROVIDER,
                OcAccountManagerHelper.authCode, mCloudHelper));
        assertTrue(mCloudHelper.singIn(mAccountManager,
                OcAccountManagerHelper.s_CloudUid,
                OcAccountManagerHelper.s_CloudAccesstoken, mCloudHelper));
        try {
            mAccountManager.createGroup(mOcAccountManagerAdapter);
            Thread.sleep(5000);
            assertNotNull("groupID not found after createGroup",
                    OcAccountManagerAdapter.sGroupId);

            mAccountManager.getGroupInfo(OcAccountManagerAdapter.sGroupId,
                    mOconGetGroupInfoAll);
            Thread.sleep(5000);
            assertTrue("groupID not found after getGroupInfo",
                    OconGetGroupInfoAllAdapter.sGroupInfoCallbackinvoked);
            mAccountManager.getGroupInfoAll(mOconGetGroupInfoAll);
            Thread.sleep(5000);
            assertNotNull("groupID not found after getGroupInfoAll",
                    OconGetGroupInfoAllAdapter.sGroupId);
        } catch (Exception ex) {
            ex.printStackTrace();
            fail("SignUp API Exception occurred: " + ex.getLocalizedMessage());
        }
    }

    /**
     * @since 2017-01-31
     * @see none
     * @objective Test 'observeGroup' API With valid scenario
     * @target string createGroup() API
     * @test_data None
     * @pre_condition constructAccountManagerObject(host, connectivity_type),
     *                SignUp(), SignIn() API
     * @procedure 1. Perform string createGroup() API 2. Perform observeGroup()
     *            API 3. Perform getGroupInfoAll() API 4. Check ALL APIs work
     *            properly
     * @post_condition SignOut()
     * @expected 'observeGroup' API works properly without exception.
     */
    public void testObserveAndGetGroupList_SRC_P() {
        assertTrue(mCloudHelper.singUp(mAccountManager, DEFAULT_AUTH_PROVIDER,
                OcAccountManagerHelper.authCode, mCloudHelper));
        assertTrue(mCloudHelper.singIn(mAccountManager,
                OcAccountManagerHelper.s_CloudUid,
                OcAccountManagerHelper.s_CloudAccesstoken, mCloudHelper));
        try {
            mAccountManager.createGroup(mOcAccountManagerAdapter);
            Thread.sleep(5000);
            assertNotNull("groupID not found",
                    OcAccountManagerAdapter.sGroupId);

            mAccountManager.observeGroup(mOcAccountManagerAdapter);
            Thread.sleep(5000);
            assertTrue("observeGroup not worked",
                    OcAccountManagerAdapter.sIsObserveCompleted);

            mAccountManager.getGroupInfoAll(mOconGetGroupInfoAll);
            Thread.sleep(5000);
            assertNotNull("groupID not found after mOconGetGroupInfoAll",
                    OconGetGroupInfoAllAdapter.sGroupId);
        } catch (Exception ex) {
            ex.printStackTrace();
            fail("SignUp API Exception occurred: " + ex.getLocalizedMessage());
        }
    }

    /**
     * @since 2017-01-31
     * @see none
     * @objective Test 'cancelObserveGroup' API With valid scenario
     * @target string cancelObserveGroup() API
     * @test_data None
     * @pre_condition constructAccountManagerObject(host, connectivity_type),
     *                SignUp(), SignIn() API
     * @procedure 1. Perform string createGroup() API 2. Perform observeGroup()
     *            API 3. Perform getGroupInfoAll() API 4. Perform
     *            cancelObserveGroup() API 5. Check ALL APIs work properly
     * @post_condition SignOut()
     * @expected 'cancelObserveGroup' API run without any exception.
     */
    public void testCancelobserveAndGetGroupList_SRC_P() {
        assertTrue(mCloudHelper.singUp(mAccountManager, DEFAULT_AUTH_PROVIDER,
                OcAccountManagerHelper.authCode, mCloudHelper));
        assertTrue(mCloudHelper.singIn(mAccountManager,
                OcAccountManagerHelper.s_CloudUid,
                OcAccountManagerHelper.s_CloudAccesstoken, mCloudHelper));
        try {
            mAccountManager.createGroup(mOcAccountManagerAdapter);
            Thread.sleep(5000);
            assertNotNull("groupID not found",
                    OcAccountManagerAdapter.sGroupId);

            mAccountManager.observeGroup(mOcAccountManagerAdapter);
            Thread.sleep(5000);
            assertTrue("observeGroup not worked",
                    OcAccountManagerAdapter.sIsObserveCompleted);

            mAccountManager.getGroupInfoAll(mOconGetGroupInfoAll);
            Thread.sleep(5000);
            assertNotNull("groupID not found after mOconGetGroupInfoAll",
                    OconGetGroupInfoAllAdapter.sGroupId);

            mAccountManager.cancelObserveGroup();
        } catch (Exception ex) {
            ex.printStackTrace();
            fail("SignUp API Exception occurred: " + ex.getLocalizedMessage());
        }
    }

    /**
     * @since 2017-01-31
     * @see public void signUp(String authProvider, String authCode,
     *      OnPostListener onPostListener)
     * @see public void signIn(String userUuid, String accessToken,
     *      OnPostListener onPostListener)
     * @see public void signOut(String accessToken, OnPostListener
     *      onPostListener)
     * @objective Test 'deleteGroup' API With valid scenario
     * @target string deleteGroup() API
     * @test_data cloudConnectGetHandler
     * @pre_condition constructAccountManagerObject(host, connectivity_type),
     *                SignUp(), SignIn() API
     * @procedure 1.CreateGroup with valid queryParams 2. Check group is created
     *            with given group id 3. Perform string deleteGroup() API 4.
     *            Check group deleted successfully
     * @post_condition SignOut()
     * @expected 'createGroup' API will provides OC_STACK_OK and delete the
     *           group
     */
    public void testDeleteGroupWithGroupID_SRC_P() {
        assertTrue(mCloudHelper.singUp(mAccountManager, DEFAULT_AUTH_PROVIDER,
                OcAccountManagerHelper.authCode, mCloudHelper));
        assertTrue(mCloudHelper.singIn(mAccountManager,
                OcAccountManagerHelper.s_CloudUid,
                OcAccountManagerHelper.s_CloudAccesstoken, mCloudHelper));
        try {
            mAccountManager.createGroup(mOcAccountManagerAdapter);
            Thread.sleep(5000);
            assertNotNull("groupID not found",
                    OcAccountManagerAdapter.sGroupId);

            mAccountManager.getGroupInfo(OcAccountManagerAdapter.sGroupId,
                    mOconGetGroupInfoAll);
            Thread.sleep(5000);
            assertTrue("mGroupInfoCallbackinvoked not found",
                    OconGetGroupInfoAllAdapter.sGroupInfoCallbackinvoked);

            mAccountManager.deleteGroup(OcAccountManagerAdapter.sGroupId,
                    mOcAccountManagerAdapter);
            Thread.sleep(5000);
            assertTrue("mGroupInfoCallbackinvoked not found",
                    OcAccountManagerAdapter.sIsonDeleteCompletedCBInvoked);

            mAccountManager.getGroupInfo(OcAccountManagerAdapter.sGroupId,
                    mOconGetGroupInfoAll);
            Thread.sleep(5000);
            assertTrue("mGroupInfoCallbackinvoked not invoked",
                    OconGetGroupInfoAllAdapter.sGroupInfoCallbackinvoked);
        } catch (Exception ex) {
            ex.printStackTrace();
            fail("SignUp API Exception occurred: " + ex.getLocalizedMessage());
        }
    }

    /**
     * @since 2017-02-01
     * @see none
     * @objective Test 'observeGroup and sendInvitation' API With valid scenario
     * @target string observeGroup and sendInvitation() API
     * @test_data None
     * @pre_condition constructAccountManagerObject(host, connectivity_type),
     *                SignUp(), SignIn() API
     * @procedure 1. Perform string createGroup() API 2. Perform
     *            observeInvitation() API 3. Perform sendInvitation() API 4.
     *            Check ALL APIs work properly
     * @post_condition replyToInvitation which help to delete new invitation and
     *                 SignOut()
     * @expected 'observeGroup and sendInvitation' API will provides
     *           OC_STACK_OK.
     */
    public void testSendAndObserveInvitation_SRC_P() {
        assertTrue(mCloudHelper.singUp(mAccountManager, DEFAULT_AUTH_PROVIDER,
                OcAccountManagerHelper.authCode, mCloudHelper));
        assertTrue(mCloudHelper.singIn(mAccountManager,
                OcAccountManagerHelper.s_CloudUid,
                OcAccountManagerHelper.s_CloudAccesstoken, mCloudHelper));
        try {
            mAccountManager.createGroup(mOcAccountManagerAdapter);
            Thread.sleep(5000);
            assertNotNull("groupID not found",
                    OcAccountManagerAdapter.sGroupId);

            mAccountManager.observeInvitation(mOcAccountManagerAdapter);
            Thread.sleep(5000);
            assertTrue("mGroupInfoCallbackinvoked not found",
                    OcAccountManagerAdapter.sIsObserveCompleted);
            Log.i(TAG, "observeInvitation successful");
            mAccountManager.sendInvitation(OcAccountManagerAdapter.sGroupId,
                    OcAccountManagerHelper.s_CloudUid,
                    mOcAccountManagerAdapter);
            Thread.sleep(5000);
            assertTrue("mGroupInfoCallbackinvoked not found",
                    OcAccountManagerAdapter.sIssendInvitationCBInvoked);

            mAccountManager.replyToInvitation(OcAccountManagerAdapter.sGroupId,
                    true, mOcAccountManagerAdapter);
            Thread.sleep(5000);
            assertTrue("mGroupInfoCallbackinvoked not invoked",
                    OcAccountManagerAdapter.sIsonDeleteCompletedCBInvoked);
        } catch (Exception ex) {
            ex.printStackTrace();
            fail("SignUp API Exception occurred: " + ex.getLocalizedMessage());
        }
    }

    /**
     * @since 2017-02-01
     * @see none
     * @objective Test 'observeGroup and cancelObserveInvitation' API With valid
     *            scenario
     * @target string observeGroup and cancelObserveInvitation() API
     * @test_data None
     * @pre_condition constructAccountManagerObject(host, connectivity_type),
     *                SignUp(), SignIn() API
     * @procedure 1. Perform string createGroup() API 2. Perform
     *            observeInvitation() API 3. Perform cancelObserveInvitation()
     *            API 4. Check ALL APIs work properly
     * @post_condition replyToInvitation which help to delete new invitation and
     *                 SignOut()
     * @expected 'observeGroup and cancelObserveInvitation' API will provides
     *           OC_STACK_OK.
     */
    public void testObserveandCancalobserveInvitation_SRC_P() {
        assertTrue(mCloudHelper.singUp(mAccountManager, DEFAULT_AUTH_PROVIDER,
                OcAccountManagerHelper.authCode, mCloudHelper));
        assertTrue(mCloudHelper.singIn(mAccountManager,
                OcAccountManagerHelper.s_CloudUid,
                OcAccountManagerHelper.s_CloudAccesstoken, mCloudHelper));
        try {
            mAccountManager.createGroup(mOcAccountManagerAdapter);
            Thread.sleep(5000);
            assertNotNull("groupID not found",
                    OcAccountManagerAdapter.sGroupId);

            mAccountManager.observeInvitation(mOcAccountManagerAdapter);
            Thread.sleep(5000);
            assertTrue("mGroupInfoCallbackinvoked not found after observeInvitation",
                    OcAccountManagerAdapter.sIsObserveCompleted);

            mAccountManager.cancelObserveInvitation();
            Thread.sleep(5000);
            mAccountManager.observeInvitation(mOcAccountManagerAdapter);
            Thread.sleep(5000);
            assertTrue("mGroupInfoCallbackinvoked not found after observeInvitation 2",
                    OcAccountManagerAdapter.sIsObserveCompleted);

            mAccountManager.replyToInvitation(OcAccountManagerAdapter.sGroupId,
                    true, mOcAccountManagerAdapter);
            Thread.sleep(5000);
            assertFalse("mGroupInfoCallbackinvoked not invoked after replyToInvitation",
                    OcAccountManagerAdapter.sIsonDeleteCompletedCBInvoked);
        } catch (Exception ex) {
            ex.printStackTrace();
            fail("SignUp API Exception occurred: " + ex.getLocalizedMessage());
        }
    }

    /**
     * @since 2017-02-01
     * @see none
     * @objective Test 'sendInvitation and cancelInvitation' API With valid
     *            scenario
     * @target string sendInvitation and cancelInvitation() API
     * @test_data None
     * @pre_condition constructAccountManagerObject(host, connectivity_type),
     *                SignUp(), SignIn() API
     * @procedure 1. Perform string createGroup() API 2. Perform
     *            observeInvitation() API 3. Perform sendInvitation() API 4.
     *            Perform cancelObserveInvitation() API 5. Check ALL APIs work
     *            properly
     * @post_condition replyToInvitation which help to delete new invitation and
     *                 SignOut()
     * @expected 'sendInvitation and cancelInvitation' API will provides
     *           OC_STACK_OK.
     */
    public void testSendAndCancelInvitation_SRC_P() {
        assertTrue(mCloudHelper.singUp(mAccountManager, DEFAULT_AUTH_PROVIDER,
                OcAccountManagerHelper.authCode, mCloudHelper));
        assertTrue(mCloudHelper.singIn(mAccountManager,
                OcAccountManagerHelper.s_CloudUid,
                OcAccountManagerHelper.s_CloudAccesstoken, mCloudHelper));
        try {
            mAccountManager.createGroup(mOcAccountManagerAdapter);
            Thread.sleep(5000);
            assertNotNull("groupID not found",
                    OcAccountManagerAdapter.sGroupId);

            mAccountManager.observeInvitation(mOcAccountManagerAdapter);
            Thread.sleep(5000);
            assertTrue("mGroupInfoCallbackinvoked not found observeInvitation",
                    OcAccountManagerAdapter.sIsObserveCompleted);

            mAccountManager.sendInvitation(OcAccountManagerAdapter.sGroupId,
                    OcAccountManagerHelper.s_CloudUid,
                    mOcAccountManagerAdapter);
            Thread.sleep(5000);
            assertTrue("mGroupInfoCallbackinvoked not found sendInvitation",
                    OcAccountManagerAdapter.sIssendInvitationCBInvoked);

            mAccountManager.cancelObserveInvitation();
            Thread.sleep(5000);
            mAccountManager.observeInvitation(mOcAccountManagerAdapter);
            Thread.sleep(5000);
            assertTrue("mGroupInfoCallbackinvoked not found observeInvitation",
                    OcAccountManagerAdapter.sIsObserveCompleted);

            mAccountManager.replyToInvitation(OcAccountManagerAdapter.sGroupId,
                    true, mOcAccountManagerAdapter);
            Thread.sleep(5000);
            assertTrue("mGroupInfoCallbackinvoked not invoked replyToInvitation",
                    OcAccountManagerAdapter.sIsonDeleteCompletedCBInvoked);
        } catch (Exception ex) {
            ex.printStackTrace();
            fail("SignUp API Exception occurred: " + ex.getLocalizedMessage());
        }
    }

    /**
     * @since 2017-02-01
     * @see none
     * @objective Test 'replyToInvitation' API With valid scenario
     * @target string replyToInvitation() API
     * @test_data None
     * @pre_condition constructAccountManagerObject(host, connectivity_type),
     *                SignUp(), SignIn() API
     * @procedure 1. Perform string createGroup() API 2. Perform
     *            observeInvitation() API 3. Perform sendInvitation() API 4.
     *            Perform cancelObserveInvitation() API 5. Perform
     *            replyToInvitation API 5. Check ALL APIs work properly
     * @post_condition SignOut()
     * @expected 'replyToInvitation' API works properly without exception
     */
    public void testReplyToInvitation_SRC_P() {
        assertTrue(mCloudHelper.singUp(mAccountManager, DEFAULT_AUTH_PROVIDER,
                OcAccountManagerHelper.authCode, mCloudHelper));
        assertTrue(mCloudHelper.singIn(mAccountManager,
                OcAccountManagerHelper.s_CloudUid,
                OcAccountManagerHelper.s_CloudAccesstoken, mCloudHelper));
        try {
            mAccountManager.createGroup(mOcAccountManagerAdapter);
            Thread.sleep(3000);
            assertNotNull("groupID not found",
                    OcAccountManagerAdapter.sGroupId);

            mAccountManager.observeInvitation(mOcAccountManagerAdapter);
            Thread.sleep(3000);
            assertTrue("mGroupInfoCallbackinvoked not found after observeInvitation",
                    OcAccountManagerAdapter.sIsObserveCompleted);

            mAccountManager.sendInvitation(OcAccountManagerAdapter.sGroupId,
                    OcAccountManagerHelper.s_CloudUid,
                    mOcAccountManagerAdapter);
            Thread.sleep(3000);
            assertTrue("mGroupInfoCallbackinvoked not found sendInvitation",
                    OcAccountManagerAdapter.sIssendInvitationCBInvoked);

            mAccountManager.cancelObserveInvitation();
            Thread.sleep(3000);
            mAccountManager.observeInvitation(mOcAccountManagerAdapter);
            Thread.sleep(3000);
            assertTrue("mGroupInfoCallbackinvoked not found observeInvitation 2",
                    OcAccountManagerAdapter.sIsObserveCompleted);

            mAccountManager.replyToInvitation(OcAccountManagerAdapter.sGroupId,
                    true, mOcAccountManagerAdapter);
            Thread.sleep(3000);
            assertTrue("mGroupInfoCallbackinvoked not invoked replyToInvitation",
                    OcAccountManagerAdapter.sIsonDeleteCompletedCBInvoked);
        } catch (Exception ex) {
            ex.printStackTrace();
            fail("SignUp API Exception occurred: " + ex.getLocalizedMessage());
        }
    }

    /**
     * @since 2017-02-01
     * @see none
     * @objective Test 'replyToInvitation' API With valid scenario
     * @target string replyToInvitation() API
     * @test_data None
     * @pre_condition constructAccountManagerObject(host, connectivity_type),
     *                SignUp(), SignIn() API
     * @procedure 1. Perform string createGroup() API 2. Perform
     *            observeInvitation() API 3. Perform sendInvitation() API 4.
     *            Perform cancelObserveInvitation() API 5. Perform
     *            replyToInvitation API 5. Check ALL APIs work properly
     * @post_condition SignOut()
     * @expected 'replyToInvitation' API works properly without exception.
     */
    public void testReplyToInvitationAndRejecttoJoin_SRC_P() {
        assertTrue(mCloudHelper.singUp(mAccountManager, DEFAULT_AUTH_PROVIDER,
                OcAccountManagerHelper.authCode, mCloudHelper));
        assertTrue(mCloudHelper.singIn(mAccountManager,
                OcAccountManagerHelper.s_CloudUid,
                OcAccountManagerHelper.s_CloudAccesstoken, mCloudHelper));
        try {
            mAccountManager.createGroup(mOcAccountManagerAdapter);
            Thread.sleep(3000);
            assertNotNull("groupID not found",
                    OcAccountManagerAdapter.sGroupId);

            mAccountManager.observeInvitation(mOcAccountManagerAdapter);
            Thread.sleep(3000);
            assertTrue("mGroupInfoCallbackinvoked not found observeInvitation",
                    OcAccountManagerAdapter.sIsObserveCompleted);

            mAccountManager.sendInvitation(OcAccountManagerAdapter.sGroupId,
                    OcAccountManagerHelper.s_CloudUid,
                    mOcAccountManagerAdapter);
            Thread.sleep(3000);
            assertTrue("mGroupInfoCallbackinvoked not found sendInvitation",
                    OcAccountManagerAdapter.sIssendInvitationCBInvoked);

            mAccountManager.cancelObserveInvitation();
            Thread.sleep(3000);
            mAccountManager.observeInvitation(mOcAccountManagerAdapter);
            Thread.sleep(3000);
            assertTrue("mGroupInfoCallbackinvoked not found observeInvitation 2",
                    OcAccountManagerAdapter.sIsObserveCompleted);

            mAccountManager.replyToInvitation(OcAccountManagerAdapter.sGroupId,
                    false, mOcAccountManagerAdapter);
            Thread.sleep(3000);
            assertTrue("mGroupInfoCallbackinvoked not invoked replyToInvitation",
                    OcAccountManagerAdapter.sIsonDeleteCompletedCBInvoked );
        } catch (Exception ex) {
            ex.printStackTrace();
            fail("SignUp API Exception occurred: " + ex.getLocalizedMessage());
        }
    }

    /**
     * @since 2017-02-01
     * @see none
     * @objective Test 'deleteDevice' API With valid scenario
     * @target string deleteDevice() API
     * @test_data None
     * @pre_condition constructAccountManagerObject(host, connectivity_type),
     *                SignUp(), SignIn() API
     * @procedure 1. Perform string createGroup() API 2. Perform
     *            getGroupInfoAll() API 3. Perform deleteDevice() API 4. Perform
     *            getGroupInfoAll() API 5. Check ALL APIs work properly
     * @post_condition SignOut()
     * @expected 'deleteDevice' API works properly without exception.
     */
    public void testDeleteDeviceWithDeviceID_SRC_P() {
        assertTrue(mCloudHelper.singUp(mAccountManager, DEFAULT_AUTH_PROVIDER,
                OcAccountManagerHelper.authCode, mCloudHelper));
        assertTrue(mCloudHelper.singIn(mAccountManager,
                OcAccountManagerHelper.s_CloudUid,
                OcAccountManagerHelper.s_CloudAccesstoken, mCloudHelper));
        try {
            mAccountManager.createGroup(mOcAccountManagerAdapter);
            Thread.sleep(3000);
            assertNotNull("groupID not found",
                    OcAccountManagerAdapter.sGroupId);

            mAccountManager.getGroupInfoAll(mOconGetGroupInfoAll);
            Thread.sleep(3000);
            assertNotNull("groupID not found getGroupInfoAll",
                    OconGetGroupInfoAllAdapter.sGroupId);

            mAccountManager.deleteDevice(
                    OcAccountManagerHelper.s_CloudAccesstoken,
                    OcAccountManagerHelper.s_CloudUid,
                    mOcAccountManagerAdapter);
            Thread.sleep(3000);
            assertTrue("mGroupInfoCallbackinvoked not found",
                    OcAccountManagerAdapter.sIsonDeleteCompletedCBInvoked);

            mAccountManager.getGroupInfoAll(mOconGetGroupInfoAll);
            Thread.sleep(3000);
            assertNull("groupID not found getGroupInfoAll 2",
                    OconGetGroupInfoAllAdapter.sGroupId);
        } catch (Exception ex) {
            ex.printStackTrace();
            fail("SignUp API Exception occurred: " + ex.getLocalizedMessage());
        }
    }

    /**
     * @since 2017-02-01
     * @see none
     * @objective Test 'addPropertyValueToGroup' API With valid scenario
     * @target string addPropertyValueToGroup() API
     * @test_data Group ID
     * @pre_condition constructAccountManagerObject(host, connectivity_type),
     *                SignUp(), SignIn() API
     * @procedure 1. Perform string createGroup() API 2. Perform
     *            getGroupInfoAll() API 3. Perform addPropertyValueToGroup() API
     *            4. Perform getGroupInfoAll() API 5. Check ALL APIs work
     *            properly
     * @post_condition SignOut()
     * @expected 'addPropertyValueToGroup' API works properly without exception.
     */
    public void testAddPropertyValueToGroup_SRC_P() {
        try {
            mPropertyValue.setValue(KEY, mValues);
            assertTrue(
                    mCloudHelper.singUp(mAccountManager, DEFAULT_AUTH_PROVIDER,
                            OcAccountManagerHelper.authCode, mCloudHelper));
            assertTrue(mCloudHelper.singIn(mAccountManager,
                    OcAccountManagerHelper.s_CloudUid,
                    OcAccountManagerHelper.s_CloudAccesstoken, mCloudHelper));

            mAccountManager.createGroup(mOcAccountManagerAdapter);
            Thread.sleep(3000);
            assertNotNull("groupID not found",
                    OcAccountManagerAdapter.sGroupId);

            mAccountManager.getGroupInfoAll(mOconGetGroupInfoAll);
            Thread.sleep(3000);
            assertNotNull("groupID not found getGroupInfoAll",
                    OconGetGroupInfoAllAdapter.sGroupId);

            mAccountManager.addPropertyValueToGroup(
                    OcAccountManagerAdapter.sGroupId, mPropertyValue,
                    mOcAccountManagerAdapter);
            Thread.sleep(3000);
            assertTrue("addPropertyValueToGroup callback is not not invoked.",
                    OcAccountManagerAdapter.sIssendInvitationCBInvoked);

            mAccountManager.getGroupInfoAll(mOconGetGroupInfoAll);
            Thread.sleep(3000);
            assertNotNull("groupID not foundgetGroupInfoAll 2",
                    OconGetGroupInfoAllAdapter.sGroupId);


        } catch (Exception ex) {
            ex.printStackTrace();
            fail("SignUp API Exception occurred: " + ex.getLocalizedMessage());
        }
    }

    /**
     * @since 2017-02-01
     * @see none
     * @objective Test 'deletePropertyValueFromGroup' API With valid scenario
     * @target string deletePropertyValueFromGroup() API
     * @test_data Group ID
     * @pre_condition constructAccountManagerObject(host, connectivity_type),
     *                SignUp(), SignIn() API
     * @procedure 1. Perform string createGroup() API 2. Perform
     *            getGroupInfoAll() API 3. Perform addPropertyValueToGroup() API
     *            4. Perform deletePropertyValueFromGroup() API 5. Check ALL
     *            APIs work properly
     * @post_condition SignOut()
     * @expected 'deletePropertyValueFromGroup' API works properly without
     *           exception.
     */
    public void testDeletePropertyValueFromGroup_SRC_P() {
        try {
            mPropertyValue.setValue(KEY, mValues);
            assertTrue(
                    mCloudHelper.singUp(mAccountManager, DEFAULT_AUTH_PROVIDER,
                            OcAccountManagerHelper.authCode, mCloudHelper));
            assertTrue(mCloudHelper.singIn(mAccountManager,
                    OcAccountManagerHelper.s_CloudUid,
                    OcAccountManagerHelper.s_CloudAccesstoken, mCloudHelper));

            mAccountManager.createGroup(mOcAccountManagerAdapter);
            Thread.sleep(3000);
            assertNotNull("groupID not found",
                    OcAccountManagerAdapter.sGroupId);

            mAccountManager.getGroupInfoAll(mOconGetGroupInfoAll);
            Thread.sleep(3000);
            assertNotNull("groupID not found getGroupInfoAll",
                    OconGetGroupInfoAllAdapter.sGroupId);

            mAccountManager.addPropertyValueToGroup(
                    OcAccountManagerAdapter.sGroupId, mPropertyValue,
                    mOcAccountManagerAdapter);
            Thread.sleep(3000);
            assertTrue("addPropertyValueToGroup callback is not not invoked.",
                    OcAccountManagerAdapter.sIssendInvitationCBInvoked);

            mAccountManager.getGroupInfoAll(mOconGetGroupInfoAll);
            Thread.sleep(3000);
            assertNotNull("groupID not found getGroupInfoAll 2",
                    OconGetGroupInfoAllAdapter.sGroupId);

            mAccountManager.deletePropertyValueFromGroup(
                    OcAccountManagerAdapter.sGroupId, mPropertyValue,
                    mOcAccountManagerAdapter);
            Thread.sleep(3000);
            assertTrue(
                    "deletePropertyValueFromGroup callback is not not invoked.",
                    OcAccountManagerAdapter.sIssendInvitationCBInvoked);

            mAccountManager.getGroupInfoAll(mOconGetGroupInfoAll);
            assertNotNull("groupID not found getGroupInfoAll 3",
                    OconGetGroupInfoAllAdapter.sGroupId);
            assertNotSame("Gname is not updated", "SRBD",
                    OconGetGroupInfoAllAdapter.sGname);

        } catch (Exception ex) {
            ex.printStackTrace();
            fail("SignUp API Exception occurred: " + ex.getLocalizedMessage());
        }
    }

    /**
     * @since 2017-02-01
     * @see none
     * @objective Test 'updatePropertyValueOnGroup' API With valid scenario
     * @target string updatePropertyValueOnGroup() API
     * @test_data Group ID
     * @pre_condition constructAccountManagerObject(host, connectivity_type),
     *                SignUp(), SignIn() API
     * @procedure 1. Perform string createGroup() API 2. Perform
     *            getGroupInfoAll() API 3. Perform addPropertyValueToGroup() API
     *            4. Perform updatePropertyValueOnGroup() API 5. Check ALL APIs
     *            work properly
     * @post_condition SignOut()
     * @expected 'updatePropertyValueOnGroup' API works properly without
     *           exception.
     */
    public void testUpdatePropertyValueOnGroup_SRC_P() {
        try {
            mPropertyValue.setValue(KEY, mValues);
            assertTrue(
                    mCloudHelper.singUp(mAccountManager, DEFAULT_AUTH_PROVIDER,
                            OcAccountManagerHelper.authCode, mCloudHelper));
            assertTrue(mCloudHelper.singIn(mAccountManager,
                    OcAccountManagerHelper.s_CloudUid,
                    OcAccountManagerHelper.s_CloudAccesstoken, mCloudHelper));

            mAccountManager.createGroup(mOcAccountManagerAdapter);
            Thread.sleep(3000);
            assertNotNull("groupID not found",
                    OcAccountManagerAdapter.sGroupId);

            mAccountManager.getGroupInfoAll(mOconGetGroupInfoAll);
            Thread.sleep(3000);
            assertNotNull("groupID not found getGroupInfoAll" ,
                    OconGetGroupInfoAllAdapter.sGroupId);

            mAccountManager.addPropertyValueToGroup(
                    OcAccountManagerAdapter.sGroupId, mPropertyValue,
                    mOcAccountManagerAdapter);
            Thread.sleep(3000);
            assertTrue("addPropertyValueToGroup callback is not not invoked.",
                    OcAccountManagerAdapter.sIssendInvitationCBInvoked);

            mAccountManager.getGroupInfoAll(mOconGetGroupInfoAll);
            Thread.sleep(3000);
            assertNotNull("groupID not found getGroupInfoAll 2",
                    OconGetGroupInfoAllAdapter.sGroupId);

            OcRepresentation           cPropertyValue = new OcRepresentation();;
            cPropertyValue.setValue("devices", mModifyvalues);
            System.out.println("group update start for : " + OcAccountManagerAdapter.sGroupId);
            OcAccountManagerAdapter.sIssendInvitationCBInvoked = false;
            mAccountManager.updatePropertyValueOnGroup(
                    OcAccountManagerAdapter.sGroupId, cPropertyValue,
                    mOcAccountManagerAdapter);
            assertTrue(
                    "updatePropertyValueOnGroup callback is not not invoked.",
                    OcAccountManagerAdapter.sIssendInvitationCBInvoked);

            mAccountManager.getGroupInfoAll(mOconGetGroupInfoAll);
            Thread.sleep(3000);
            assertNotNull("groupID not found getGroupInfoAll 3" ,
                    OconGetGroupInfoAllAdapter.sGroupId);

        } catch (Exception ex) {
            ex.printStackTrace();
            fail("SignUp API Exception occurred: " + ex.getLocalizedMessage());
        }
    }
}
