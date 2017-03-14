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
    private String[]                   mModifyvalues = { "IOTIVITY_SRBD" };

    protected void setUp() throws Exception {
        super.setUp();
        mICHelper = new ICHelper();
        mContext = getInstrumentation().getContext();
        mOconGetGroupInfoAll = new OconGetGroupInfoAllAdapter();

        mOcAccountManagerAdapter = new OcAccountManagerAdapter();
        mPropertyValue = new OcRepresentation();

        PlatformConfig cfg = new PlatformConfig(
                getInstrumentation().getTargetContext(), ServiceType.IN_PROC,
                ModeType.CLIENT_SERVER, "0.0.0.0", // bind to all available
                // interfaces
                0, QualityOfService.LOW);
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
            Log.d(TAG, "map initialization start");
            MY_MAP.put("userid", OcAccountManagerHelper.s_CloudUid);
            Log.d(TAG, "map initialization stop");

            mAccountManager.searchUser(MY_MAP, mOcAccountManagerAdapter);
            Thread.sleep(10000);
            assertEquals("search not working",
                    OcAccountManagerHelper.s_CloudUid,
                    OcAccountManagerAdapter.sInviteeUuid);
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

        MY_MAP.put("uid", OcAccountManagerHelper.s_CloudUid); // invitation.getValue("mid");get
                                                              // Id after
                                                              // signIn and
                                                              // put here
        try {
            mAccountManager.searchUser(MY_MAP, mOcAccountManagerAdapter);
            assertEquals("search not working",
                    OcAccountManagerHelper.s_CloudUid,
                    OcAccountManagerAdapter.sInviteeUuid);
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
            assertEquals("search not working", null,
                    OcAccountManagerAdapter.sInviteeUuid);
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
            assertNotNull("groupID not found",
                    OcAccountManagerAdapter.sGroupId);
            OcAccountManagerAdapter.sGroupId = null;
            mAccountManager.getGroupInfoAll(mOconGetGroupInfoAll);
            assertNotNull("groupID not found",
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
            assertNotNull("groupID not found",
                    OcAccountManagerAdapter.sGroupId);

            mAccountManager.getGroupInfo(OcAccountManagerAdapter.sGroupId,
                    mOconGetGroupInfoAll);
            assertTrue("groupID not found",
                    OconGetGroupInfoAllAdapter.sGroupInfoCallbackinvoked);
            mAccountManager.getGroupInfoAll(mOconGetGroupInfoAll);
            assertNotNull("groupID not found",
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
            assertNotNull("groupID not found",
                    OcAccountManagerAdapter.sGroupId);

            mAccountManager.observeGroup(mOcAccountManagerAdapter);
            assertTrue("observeGroup not worked",
                    OcAccountManagerAdapter.sIsObserveCompleted);

            mAccountManager.getGroupInfoAll(mOconGetGroupInfoAll);
            assertNotNull("groupID not found",
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
            assertNotNull("groupID not found",
                    OcAccountManagerAdapter.sGroupId);

            mAccountManager.observeGroup(mOcAccountManagerAdapter);
            assertTrue("observeGroup not worked",
                    OcAccountManagerAdapter.sIsObserveCompleted);

            mAccountManager.getGroupInfoAll(mOconGetGroupInfoAll);
            assertNotNull("groupID not found",
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
            assertNotNull("groupID not found",
                    OcAccountManagerAdapter.sGroupId);

            mAccountManager.getGroupInfo(OcAccountManagerAdapter.sGroupId,
                    mOconGetGroupInfoAll);
            assertTrue("mGroupInfoCallbackinvoked not found",
                    OconGetGroupInfoAllAdapter.sGroupInfoCallbackinvoked);

            mAccountManager.deleteGroup(OcAccountManagerAdapter.sGroupId,
                    mOcAccountManagerAdapter);
            assertTrue("mGroupInfoCallbackinvoked not found",
                    OcAccountManagerAdapter.sIsonDeleteCompletedCBInvoked);

            mAccountManager.getGroupInfo(OcAccountManagerAdapter.sGroupId,
                    mOconGetGroupInfoAll);
            assertFalse("mGroupInfoCallbackinvoked not invoked",
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
            assertNotNull("groupID not found",
                    OcAccountManagerAdapter.sGroupId);

            mAccountManager.observeInvitation(mOcAccountManagerAdapter);
            assertTrue("mGroupInfoCallbackinvoked not found",
                    OcAccountManagerAdapter.sIsObserveCompleted);

            mAccountManager.sendInvitation(OcAccountManagerAdapter.sGroupId,
                    OcAccountManagerAdapter.sInviteeUuid,
                    mOcAccountManagerAdapter);
            assertTrue("mGroupInfoCallbackinvoked not found",
                    OcAccountManagerAdapter.sIssendInvitationCBInvoked);

            mAccountManager.replyToInvitation(OcAccountManagerAdapter.sGroupId,
                    true, mOcAccountManagerAdapter);
            assertFalse("mGroupInfoCallbackinvoked not invoked",
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
            assertNotNull("groupID not found",
                    OcAccountManagerAdapter.sGroupId);

            mAccountManager.observeInvitation(mOcAccountManagerAdapter);
            assertTrue("mGroupInfoCallbackinvoked not found",
                    OcAccountManagerAdapter.sIsObserveCompleted);

            mAccountManager.cancelObserveInvitation();

            mAccountManager.observeInvitation(mOcAccountManagerAdapter);
            assertFalse("mGroupInfoCallbackinvoked not found",
                    OcAccountManagerAdapter.sIsObserveCompleted);

            mAccountManager.replyToInvitation(OcAccountManagerAdapter.sGroupId,
                    true, mOcAccountManagerAdapter);
            assertFalse("mGroupInfoCallbackinvoked not invoked",
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
            assertNotNull("groupID not found",
                    OcAccountManagerAdapter.sGroupId);

            mAccountManager.observeInvitation(mOcAccountManagerAdapter);
            assertTrue("mGroupInfoCallbackinvoked not found",
                    OcAccountManagerAdapter.sIsObserveCompleted);

            mAccountManager.sendInvitation(OcAccountManagerAdapter.sGroupId,
                    OcAccountManagerAdapter.sInviteeUuid,
                    mOcAccountManagerAdapter);
            assertTrue("mGroupInfoCallbackinvoked not found",
                    OcAccountManagerAdapter.sIssendInvitationCBInvoked);

            mAccountManager.cancelObserveInvitation();

            mAccountManager.observeInvitation(mOcAccountManagerAdapter);
            assertFalse("mGroupInfoCallbackinvoked not found",
                    OcAccountManagerAdapter.sIsObserveCompleted);

            mAccountManager.replyToInvitation(OcAccountManagerAdapter.sGroupId,
                    true, mOcAccountManagerAdapter);
            assertFalse("mGroupInfoCallbackinvoked not invoked",
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
            assertNotNull("groupID not found",
                    OcAccountManagerAdapter.sGroupId);

            mAccountManager.observeInvitation(mOcAccountManagerAdapter);
            assertTrue("mGroupInfoCallbackinvoked not found",
                    OcAccountManagerAdapter.sIsObserveCompleted);

            mAccountManager.sendInvitation(OcAccountManagerAdapter.sGroupId,
                    OcAccountManagerAdapter.sInviteeUuid,
                    mOcAccountManagerAdapter);
            assertTrue("mGroupInfoCallbackinvoked not found",
                    OcAccountManagerAdapter.sIssendInvitationCBInvoked);

            mAccountManager.cancelObserveInvitation();

            mAccountManager.observeInvitation(mOcAccountManagerAdapter);
            assertFalse("mGroupInfoCallbackinvoked not found",
                    OcAccountManagerAdapter.sIsObserveCompleted);

            mAccountManager.replyToInvitation(OcAccountManagerAdapter.sGroupId,
                    true, mOcAccountManagerAdapter);
            assertFalse("mGroupInfoCallbackinvoked not invoked",
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
            assertNotNull("groupID not found",
                    OcAccountManagerAdapter.sGroupId);

            mAccountManager.observeInvitation(mOcAccountManagerAdapter);
            assertTrue("mGroupInfoCallbackinvoked not found",
                    OcAccountManagerAdapter.sIsObserveCompleted);

            mAccountManager.sendInvitation(OcAccountManagerAdapter.sGroupId,
                    OcAccountManagerAdapter.sInviteeUuid,
                    mOcAccountManagerAdapter);
            assertTrue("mGroupInfoCallbackinvoked not found",
                    OcAccountManagerAdapter.sIssendInvitationCBInvoked);

            mAccountManager.cancelObserveInvitation();

            mAccountManager.observeInvitation(mOcAccountManagerAdapter);
            assertFalse("mGroupInfoCallbackinvoked not found",
                    OcAccountManagerAdapter.sIsObserveCompleted);

            mAccountManager.replyToInvitation(OcAccountManagerAdapter.sGroupId,
                    false, mOcAccountManagerAdapter);
            assertFalse("mGroupInfoCallbackinvoked not invoked",
                    OcAccountManagerAdapter.sIsonDeleteCompletedCBInvoked);
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
    public void testDeleteDeviceWithDeviceID_SRC_P_SRC_P() {
        assertTrue(mCloudHelper.singUp(mAccountManager, DEFAULT_AUTH_PROVIDER,
                OcAccountManagerHelper.authCode, mCloudHelper));
        assertTrue(mCloudHelper.singIn(mAccountManager,
                OcAccountManagerHelper.s_CloudUid,
                OcAccountManagerHelper.s_CloudAccesstoken, mCloudHelper));
        try {
            mAccountManager.createGroup(mOcAccountManagerAdapter);
            assertNotNull("groupID not found",
                    OcAccountManagerAdapter.sGroupId);

            mAccountManager.getGroupInfoAll(mOconGetGroupInfoAll);
            assertNotNull("groupID not found",
                    OconGetGroupInfoAllAdapter.sGroupId);

            mAccountManager.deleteDevice(
                    OcAccountManagerHelper.s_CloudAccesstoken,
                    OcAccountManagerHelper.s_CloudUid,
                    mOcAccountManagerAdapter);
            assertTrue("mGroupInfoCallbackinvoked not found",
                    OcAccountManagerAdapter.sIsonDeleteCompletedCBInvoked);

            mAccountManager.getGroupInfoAll(mOconGetGroupInfoAll);
            assertNull("groupID not found",
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
    public void testAddPropertyValueToGroup_SRC_P_SRC_P() {
        try {
            mPropertyValue.setValue(KEY, mValues);
            assertTrue(
                    mCloudHelper.singUp(mAccountManager, DEFAULT_AUTH_PROVIDER,
                            OcAccountManagerHelper.authCode, mCloudHelper));
            assertTrue(mCloudHelper.singIn(mAccountManager,
                    OcAccountManagerHelper.s_CloudUid,
                    OcAccountManagerHelper.s_CloudAccesstoken, mCloudHelper));

            mAccountManager.createGroup(mOcAccountManagerAdapter);
            assertNotNull("groupID not found",
                    OcAccountManagerAdapter.sGroupId);

            mAccountManager.getGroupInfoAll(mOconGetGroupInfoAll);
            assertNotNull("groupID not found",
                    OconGetGroupInfoAllAdapter.sGroupId);

            mAccountManager.addPropertyValueToGroup(
                    OcAccountManagerAdapter.sGroupId, mPropertyValue,
                    mOcAccountManagerAdapter);
            assertTrue("addPropertyValueToGroup callback is not not invoked.",
                    OcAccountManagerAdapter.sIssendInvitationCBInvoked);

            mAccountManager.getGroupInfoAll(mOconGetGroupInfoAll);
            assertNull("groupID not found",
                    OconGetGroupInfoAllAdapter.sGroupId);
            assertEquals("Gname is not updated", "SRBD",
                    OconGetGroupInfoAllAdapter.sGname);

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
    public void testDeletePropertyValueFromGroup_SRC_P_SRC_P() {
        try {
            mPropertyValue.setValue(KEY, mValues);
            assertTrue(
                    mCloudHelper.singUp(mAccountManager, DEFAULT_AUTH_PROVIDER,
                            OcAccountManagerHelper.authCode, mCloudHelper));
            assertTrue(mCloudHelper.singIn(mAccountManager,
                    OcAccountManagerHelper.s_CloudUid,
                    OcAccountManagerHelper.s_CloudAccesstoken, mCloudHelper));

            mAccountManager.createGroup(mOcAccountManagerAdapter);
            assertNotNull("groupID not found",
                    OcAccountManagerAdapter.sGroupId);

            mAccountManager.getGroupInfoAll(mOconGetGroupInfoAll);
            assertNotNull("groupID not found",
                    OconGetGroupInfoAllAdapter.sGroupId);

            mAccountManager.addPropertyValueToGroup(
                    OcAccountManagerAdapter.sGroupId, mPropertyValue,
                    mOcAccountManagerAdapter);
            assertTrue("addPropertyValueToGroup callback is not not invoked.",
                    OcAccountManagerAdapter.sIssendInvitationCBInvoked);

            mAccountManager.getGroupInfoAll(mOconGetGroupInfoAll);
            assertNull("groupID not found",
                    OconGetGroupInfoAllAdapter.sGroupId);
            assertEquals("Gname is not updated", "SRBD",
                    OconGetGroupInfoAllAdapter.sGname);

            mAccountManager.deletePropertyValueFromGroup(
                    OcAccountManagerAdapter.sGroupId, mPropertyValue,
                    mOcAccountManagerAdapter);
            assertTrue(
                    "deletePropertyValueFromGroup callback is not not invoked.",
                    OcAccountManagerAdapter.sIssendInvitationCBInvoked);

            mAccountManager.getGroupInfoAll(mOconGetGroupInfoAll);
            assertNull("groupID not found",
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
    public void testUpdatePropertyValueOnGroup_SRC_P_SRC_P() {
        try {
            mPropertyValue.setValue(KEY, mValues);
            assertTrue(
                    mCloudHelper.singUp(mAccountManager, DEFAULT_AUTH_PROVIDER,
                            OcAccountManagerHelper.authCode, mCloudHelper));
            assertTrue(mCloudHelper.singIn(mAccountManager,
                    OcAccountManagerHelper.s_CloudUid,
                    OcAccountManagerHelper.s_CloudAccesstoken, mCloudHelper));

            mAccountManager.createGroup(mOcAccountManagerAdapter);
            assertNotNull("groupID not found",
                    OcAccountManagerAdapter.sGroupId);

            mAccountManager.getGroupInfoAll(mOconGetGroupInfoAll);
            assertNotNull("groupID not found",
                    OconGetGroupInfoAllAdapter.sGroupId);

            mAccountManager.addPropertyValueToGroup(
                    OcAccountManagerAdapter.sGroupId, mPropertyValue,
                    mOcAccountManagerAdapter);
            assertTrue("addPropertyValueToGroup callback is not not invoked.",
                    OcAccountManagerAdapter.sIssendInvitationCBInvoked);

            mAccountManager.getGroupInfoAll(mOconGetGroupInfoAll);
            assertNull("groupID not found",
                    OconGetGroupInfoAllAdapter.sGroupId);
            assertEquals("Gname is not updated", "SRBD",
                    OconGetGroupInfoAllAdapter.sGname);

            mPropertyValue.setValue(KEY, mModifyvalues);
            mAccountManager.updatePropertyValueOnGroup(
                    OcAccountManagerAdapter.sGroupId, mPropertyValue,
                    mOcAccountManagerAdapter);
            assertTrue(
                    "updatePropertyValueOnGroup callback is not not invoked.",
                    OcAccountManagerAdapter.sIssendInvitationCBInvoked);

            mAccountManager.getGroupInfoAll(mOconGetGroupInfoAll);
            assertNull("groupID not found",
                    OconGetGroupInfoAllAdapter.sGroupId);
            assertEquals("Gname is not updated", "IOTIVITY_SRBD",
                    OconGetGroupInfoAllAdapter.sGname);

        } catch (Exception ex) {
            ex.printStackTrace();
            fail("SignUp API Exception occurred: " + ex.getLocalizedMessage());
        }
    }
}
