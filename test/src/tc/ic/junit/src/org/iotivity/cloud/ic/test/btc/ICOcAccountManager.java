/******************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
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

package org.iotivity.cloud.ic.test.btc;

import static org.iotivity.cloud.ic.test.helper.ICHelperStaticUtil.*;

import java.util.EnumSet;

import org.iotivity.base.AclGroupType;
import org.iotivity.base.ErrorCode;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcAccountManager;
import org.iotivity.base.OcAccountManager.OnObserveListener;
import org.iotivity.base.OcResource;
import org.iotivity.base.OcConnectivityType;
import org.iotivity.base.OcException;
import org.iotivity.base.ModeType;
import org.iotivity.cloud.ic.test.helper.ICHelper;
import org.iotivity.cloud.ic.test.helper.OcAccountManagerAdapter;

import android.content.Context;
import android.test.InstrumentationTestCase;


public class ICOcAccountManager extends InstrumentationTestCase {

    public OcAccountManager m_accountManager;
    
    private OcAccountManagerAdapter m_OcAccountManagerAdapter;
    private Context                          m_Context;

    protected void setUp() throws Exception {
        super.setUp();
        m_Context = getInstrumentation().getTargetContext();

        ICHelper.icConfigurePlatform(m_Context);
        
        m_OcAccountManagerAdapter = new OcAccountManagerAdapter();
        
        m_accountManager = OcPlatform.constructAccountManagerObject(IC_HOST_ADDRESS, EnumSet.of(OcConnectivityType.CT_DEFAULT));
    }

    protected void tearDown() throws Exception {
        super.tearDown();
    }
    

    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'signUp' function with valid authProvide, authCode and OnPostListener
	 * @target void signUp(String authProvider, String authCode, OnPostListener onPostListener)
	 * @test_data authProvider authCode and OnPostListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform signUp(String authProvider, String authCode, OnPostListener onPostListener)API
	 * @post_condition None
	 * @expected signUp throws OcException if failure.
     */
    public void testSignUp_SRC_P() {
        try {
            m_accountManager.signUp(AUTH_PROVIDER, AUTH_CODE, m_OcAccountManagerAdapter);
        } catch (OcException ex) {
            ex.printStackTrace();
            fail("SignUp API Exception occurred: " + ex.getLocalizedMessage());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'signUp' function with empty authProvide, authCode and valid OnPostListener
	 * @target void signUp(String authProvider, String authCode, OnPostListener onPostListener)
	 * @test_data authProvider authCode and OnPostListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform signUp(String authProvider, String authCode, OnPostListener onPostListener)API
	 * @post_condition None
	 * @expected signUp throws OcException
     */
    public void testSignUp_ESV_N() {
        try {
            m_accountManager.signUp(EMPTY_VALUE, EMPTY_VALUE, m_OcAccountManagerAdapter);
            fail("SignUp API does not throw exception occurred");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'signUp' function with invalid authProvide, authCode and valid OnPostListener
	 * @target void signUp(String authProvider, String authCode, OnPostListener onPostListener)
	 * @test_data authProvider authCode and OnPostListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform signUp(String authProvider, String authCode, OnPostListener onPostListener)API
	 * @post_condition None
	 * @expected signUp throws OcException
     */
    public void testSignUp_USV_N() {
        try {
            m_accountManager.signUp(INVALID_PARAMETER, INVALID_PARAMETER, m_OcAccountManagerAdapter);
            fail("SignUp API does not throw exception occurred");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'signUp' function with NULL authProvide, authCode and valid OnPostListener
	 * @target void signUp(String authProvider, String authCode, OnPostListener onPostListener)
	 * @test_data authProvider authCode and OnPostListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform signUp(String authProvider, String authCode, OnPostListener onPostListener)API
	 * @post_condition None
	 * @expected signUp throws OcException
     */
    public void testSignUp_NV_N() {
        try {
            m_accountManager.signUp(NULL_VAL, NULL_VAL, m_OcAccountManagerAdapter);
            fail("SignUp API does not throw exception occurred");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'signUp' function with authProvide, authCode and NULL OnPostListener
	 * @target void signUp(String authProvider, String authCode, OnPostListener onPostListener)
	 * @test_data authProvider authCode and OnPostListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform signUp(String authProvider, String authCode, OnPostListener onPostListener)API
	 * @post_condition None
	 * @expected signUp throws OcException
     */
    public void testSignUpNULLCallback_NV_N() {
        try {
            m_accountManager.signUp(AUTH_PROVIDER, AUTH_CODE, IC_OcAccountManager_OnPostListener_NULL);
            fail("SignUp API does not throw exception occurred");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'signUp' function with valid authProvide, authCode, Map Option and OnPostListener
	 * @target void signUp(String authProvider, String authCode, Map<String, String> options, OnPostListener onPostListener)
	 * @test_data authProvider authCode and OnPostListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform signUp(String authProvider, String authCode, Map<String, String> options, OnPostListener onPostListener)API
	 * @post_condition None
	 * @expected signUp throws OcException if failure.
     */
    public void testSignUpWithMap_SRC_P() {
        try {
            m_accountManager.signUp(AUTH_PROVIDER, AUTH_CODE, MY_MAP,m_OcAccountManagerAdapter);
        } catch (OcException ex) {
            ex.printStackTrace();
            fail("SignUp API Exception occurred: " + ex.getLocalizedMessage());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'signUp' function with empty authProvide, authCode, Map Option and valid OnPostListener
	 * @target void signUp(String authProvider, String authCode, Map<String, String> options, OnPostListener onPostListener)
	 * @test_data authProvider authCode and OnPostListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform signUp(String authProvider, String authCode, Map<String, String> options, OnPostListener onPostListener)API
	 * @post_condition None
	 * @expected signUp throws OcException.
     */
    public void testSignUpWithMap_ESV_N() {
        try {
            m_accountManager.signUp(EMPTY_VALUE, EMPTY_VALUE, EMPTY_MY_MAP,m_OcAccountManagerAdapter);
            fail("SignUp API does not throw exception occurred");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'signUp' function with invalid authProvide, authCode, valid Map Option and valid OnPostListener
	 * @target void signUp(String authProvider, String authCode, Map<String, String> options, OnPostListener onPostListener)
	 * @test_data authProvider authCode and OnPostListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform signUp(String authProvider, String authCode, Map<String, String> options, OnPostListener onPostListener)API
	 * @post_condition None
	 * @expected signUp throws OcException.
     */
    public void testSignUpWithMap_USV_N() {
        try {
            m_accountManager.signUp(INVALID_PARAMETER, INVALID_PARAMETER, MY_MAP,m_OcAccountManagerAdapter);
            fail("SignUp API does not throw exception occurred");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'signUp' function with empty authProvide, authCode, Map Option and valid OnPostListener
	 * @target void signUp(String authProvider, String authCode, Map<String, String> options, OnPostListener onPostListener)
	 * @test_data authProvider authCode and OnPostListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform signUp(String authProvider, String authCode, Map<String, String> options, OnPostListener onPostListener)API
	 * @post_condition None
	 * @expected signUp throws OcException.
     */
    public void testSignUpWithMap_NV_N() {
        try {
            m_accountManager.signUp(NULL_VAL, NULL_VAL, EMPTY_MY_MAP,m_OcAccountManagerAdapter);
            fail("SignUp API does not throw exception occurred");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'signUp' function with authProvide, authCode, Map Option and NULL OnPostListener
	 * @target void signUp(String authProvider, String authCode, Map<String, String> options, OnPostListener onPostListener)
	 * @test_data authProvider authCode and OnPostListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform signUp(String authProvider, String authCode, Map<String, String> options, OnPostListener onPostListener)API
	 * @post_condition None
	 * @expected signUp throws OcException
     */
    public void testSignUpWithMapNULLCallback_NV_N() {
        try {
            m_accountManager.signUp(AUTH_PROVIDER, AUTH_CODE, MY_MAP, IC_OcAccountManager_OnPostListener_NULL);
            fail("SignUp API does not throw exception occurred");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'signIn' function with valid userUuid, accessToken and OnPostListener
	 * @target void signIn(String userUuid, String accessToken, OnPostListener onPostListener)
	 * @test_data authProvider authCode and OnPostListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform signIn(String userUuid, String accessToken, OnPostListener onPostListener) API
	 * @post_condition None
	 * @expected signIn throws OcException if failure.
     */
    public void testSignIn_SRC_P() {
        try {
            m_accountManager.signIn(USER_ID, ACCESS_TOKEN, m_OcAccountManagerAdapter);
        } catch (OcException ex) {
            ex.printStackTrace();
            fail("SignUp API Exception occurred: " + ex.getLocalizedMessage());
        }
    }

    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'signIn' function with invalid userUuid, accessToken and valid OnPostListener
	 * @target void signIn(String userUuid, String accessToken, OnPostListener onPostListener)
	 * @test_data authProvider authCode and OnPostListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform signIn(String userUuid, String accessToken, OnPostListener onPostListener) API
	 * @post_condition None
	 * @expected signIn throws OcException.
     */
    public void testSignIn_USV_N() {
        try {
            m_accountManager.signIn(INVALID_PARAMETER, INVALID_PARAMETER, m_OcAccountManagerAdapter);
            fail("SignIn does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'signIn' function with empty userUuid, accessToken and valid OnPostListener
	 * @target void signIn(String userUuid, String accessToken, OnPostListener onPostListener)
	 * @test_data authProvider authCode and OnPostListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform signIn(String userUuid, String accessToken, OnPostListener onPostListener) API
	 * @post_condition None
	 * @expected signIn throws OcException.
     */
    public void testSignIn_ESV_N() {
        try {
            m_accountManager.signIn(EMPTY_VALUE, EMPTY_VALUE, m_OcAccountManagerAdapter);
            fail("SignIn does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'signIn' function with NULL userUuid, accessToken and valid OnPostListener
	 * @target void signIn(String userUuid, String accessToken, OnPostListener onPostListener)
	 * @test_data authProvider authCode and OnPostListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform signIn(String userUuid, String accessToken, OnPostListener onPostListener) API
	 * @post_condition None
	 * @expected signIn throws OcException.
     */
    public void testSignIn_NV_N() {
        try {
            m_accountManager.signIn(NULL_VAL, NULL_VAL, m_OcAccountManagerAdapter);
            fail("SignIn does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'signIn' function with valid userUuid, accessToken and NULL OnPostListener
	 * @target void signIn(String userUuid, String accessToken, OnPostListener onPostListener)
	 * @test_data authProvider authCode and OnPostListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform signIn(String userUuid, String accessToken, OnPostListener onPostListener) API
	 * @post_condition None
	 * @expected signIn throws OcException.
     */
    public void testSignInNULLCallback_NV_N() {
        try {
            m_accountManager.signIn(USER_ID, ACCESS_TOKEN, IC_OcAccountManager_OnPostListener_NULL);
            fail("SignIn does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'signOut' function with valid OnPostListener
	 * @target void signOut(OnPostListener onPostListener)
	 * @test_data OnPostListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform signOut(OnPostListener onPostListener) API
	 * @post_condition None
	 * @expected signOut throws OcException if failure.
     */
    public void testSignOut_SRC_P() {
        try {
            m_accountManager.signOut(m_OcAccountManagerAdapter);
        } catch (OcException ex) {
            ex.printStackTrace();
            fail("SignOut API Exception occurred: " + ex.getLocalizedMessage());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'signOut' function with NULL OnPostListener
	 * @target void signOut(OnPostListener onPostListener)
	 * @test_data OnPostListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform signOut(OnPostListener onPostListener) API
	 * @post_condition None
	 * @expected signOut throws OcException if failure.
     */
    public void testSignOut_NV_N() {
        try {
            m_accountManager.signOut(IC_OcAccountManager_OnPostListener_NULL);
            fail("SignOut does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'refreshAccessToken' function with valid userId, refreshToken and OnPostListener
	 * @target void refreshAccessToken(userUuid, refreshToken, onPostListener)
	 * @test_data OnPostListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform refreshAccessToken(userUuid, refreshToken, onPostListener) API
	 * @post_condition None
	 * @expected refreshAccessToken throws OcException if failure.
     */
    public void testRefreshAccessToken_SRC_P() {
        try {
            m_accountManager.refreshAccessToken(USER_ID, REFRESH_TOKEN, m_OcAccountManagerAdapter);
        } catch (OcException ex) {
            ex.printStackTrace();
            fail("refreshAccessToken API Exception occurred: " + ex.getLocalizedMessage());
        }
    }
   
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'refreshAccessToken' function with invalid userId, refreshToken and valid OnPostListener
	 * @target void refreshAccessToken(userUuid, refreshToken, onPostListener)
	 * @test_data OnPostListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform refreshAccessToken(userUuid, refreshToken, onPostListener) API
	 * @post_condition None
	 * @expected refreshAccessToken throws OcException.
     */
    public void testRefreshAccessToken_USV_N() {
        try {
            m_accountManager.refreshAccessToken(INVALID_PARAMETER, INVALID_PARAMETER, m_OcAccountManagerAdapter);
            fail("refreshAccessToken does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
   }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'refreshAccessToken' function with invalid userId, refreshToken and valid OnPostListener
	 * @target void refreshAccessToken(userUuid, refreshToken, onPostListener)
	 * @test_data OnPostListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform refreshAccessToken(userUuid, refreshToken, onPostListener) API
	 * @post_condition None
	 * @expected refreshAccessToken throws OcException.
     */
    public void testRefreshAccessToken_ESV_N() {
        try {
            m_accountManager.refreshAccessToken(EMPTY_VALUE, EMPTY_VALUE, m_OcAccountManagerAdapter);
            fail("refreshAccessToken does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
   }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'refreshAccessToken' function with NULL userId, refreshToken and valid OnPostListener
	 * @target void refreshAccessToken(userUuid, refreshToken, onPostListener)
	 * @test_data OnPostListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform refreshAccessToken(userUuid, refreshToken, onPostListener) API
	 * @post_condition None
	 * @expected refreshAccessToken throws OcException.
     */
    public void testRefreshAccessToken_NV_N() {
        try {
            m_accountManager.refreshAccessToken(NULL_VAL, NULL_VAL, m_OcAccountManagerAdapter);
            fail("refreshAccessToken does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
   }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'refreshAccessToken' function with valid userId, refreshToken and NULL OnPostListener
	 * @target void refreshAccessToken(userUuid, refreshToken, onPostListener)
	 * @test_data OnPostListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform refreshAccessToken(userUuid, refreshToken, onPostListener) API
	 * @post_condition None
	 * @expected refreshAccessToken throws OcException.
     */
    public void testRefreshAccessTokenNULLCallback_NV_N() {
        try {
            m_accountManager.refreshAccessToken(USER_ID, REFRESH_TOKEN, IC_OcAccountManager_OnPostListener_NULL);
            fail("refreshAccessToken does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
   }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'searchUser' function with valid userId and OnGetListener
	 * @target void searchUser(String userUuid, OnGetListener)
	 * @test_data queryMap and OnGetListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform searchUser(String userUuid, OnGetListener) API
	 * @post_condition None
	 * @expected searchUser throws OcException if failure.
     */
    public void testSearchUser_SRC_P() {
        try {
            m_accountManager.searchUser(USER_ID, m_OcAccountManagerAdapter);
        } catch (OcException ex) {
            ex.printStackTrace();
            fail("SearchUser API Exception occurred: " + ex.getLocalizedMessage());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'searchUser' function with empty userId and valid OnGetListener
	 * @target void searchUser(String userUuid, OnGetListener)
	 * @test_data queryMap and OnGetListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform searchUser(String userUuid, OnGetListener) API
	 * @post_condition None
	 * @expected searchUser throws OcException.
     */
    public void testSearchUser_ESV_N() {
        try {
            m_accountManager.searchUser(EMPTY_VALUE, m_OcAccountManagerAdapter);
            fail("searchUser does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'searchUser' function with invalid userId and valid OnGetListener
	 * @target void searchUser(String userUuid, OnGetListener)
	 * @test_data queryMap and OnGetListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform searchUser(String userUuid, OnGetListener) API
	 * @post_condition None
	 * @expected searchUser throws OcException.
     */
    public void testSearchUser_USV_N() {
        try {
            m_accountManager.searchUser(INVALID_PARAMETER, m_OcAccountManagerAdapter);
            fail("searchUser does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'searchUser' function with NULL userId and valid OnGetListener
	 * @target void searchUser(String userUuid, OnGetListener)
	 * @test_data queryMap and OnGetListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform searchUser(String userUuid, OnGetListener) API
	 * @post_condition None
	 * @expected searchUser throws OcException.
     */
    public void testSearchUser_NV_N() {
        try {
            m_accountManager.searchUser(NULL_VAL, m_OcAccountManagerAdapter);
            fail("searchUser does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'searchUser' function with valid userId and NULL OnGetListener
	 * @target void searchUser(String userUuid, OnGetListener)
	 * @test_data queryMap and OnGetListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform searchUser(String userUuid, OnGetListener) API
	 * @post_condition None
	 * @expected searchUser throws OcException.
     */
    public void testSearchUserNULLCallback_NV_N() {
        try {
            m_accountManager.searchUser(USER_ID, IC_OcAccountManager_OnGetListener_NULL);
            fail("searchUser does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'searchUser' function with valid queryMap and OnGetListener
	 * @target void searchUser(Map<String, String> queryMap, OnGetListener)
	 * @test_data queryMap and OnGetListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform searchUser(Map<String, String> queryMap, OnGetListener) API
	 * @post_condition None
	 * @expected searchUser throws OcException if failure.
     */
    public void testSearchUserWithQueryMap_SRC_P() {
        try {
            m_accountManager.searchUser(MY_MAP, m_OcAccountManagerAdapter);
        } catch (OcException ex) {
            ex.printStackTrace();
            fail("SearchUser API Exception occurred: " + ex.getLocalizedMessage());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'searchUser' function with invalid queryMap and valid OnGetListener
	 * @target void searchUser(Map<String, String> queryMap, OnGetListener)
	 * @test_data queryMap and OnGetListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform searchUser(Map<String, String> queryMap, OnGetListener) API
	 * @post_condition None
	 * @expected searchUser throws OcException.
     */
    public void testSearchUserWithQueryMap_ESV_N() {
        try {
            m_accountManager.searchUser(EMPTY_MY_MAP, m_OcAccountManagerAdapter);
            fail("searchUser does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'searchUser' function with valid queryMap and NULL OnGetListener
	 * @target void searchUser(Map<String, String> queryMap, OnGetListener)
	 * @test_data queryMap and OnGetListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform searchUser(Map<String, String> queryMap, OnGetListener) API
	 * @post_condition None
	 * @expected searchUser throws OcException.
     */
    public void testSearchUserWithQueryMap_NV_N() {
        try {
            m_accountManager.searchUser(MY_MAP, IC_OcAccountManager_OnGetListener_NULL);
            fail("searchUser does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'deleteDevice' function with valid deviceId and onDeleteListener
	 * @target void deleteDevice(String deviceId, OnDeleteListener onDeleteListener)
	 * @test_data deviceId and onDeleteListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform deleteDevice(String deviceId, OnDeleteListener onDeleteListener) API
	 * @post_condition None
	 * @expected searchUser throws OcException if failure.
     */
    public void testDeleteDevice_SRC_P() {
        try {
            m_accountManager.deleteDevice(DEVICE_ID, m_OcAccountManagerAdapter);
        } catch (OcException ex) {
            ex.printStackTrace();
            fail("deleteDevice API Exception occurred: " + ex.getLocalizedMessage());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'deleteDevice' function with invalid deviceId and valid onDeleteListener
	 * @target void deleteDevice(String deviceId, OnDeleteListener onDeleteListener)
	 * @test_data deviceId and onDeleteListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform deleteDevice(String deviceId, OnDeleteListener onDeleteListener) API
	 * @post_condition None
	 * @expected searchUser throws OcException.
     */
    public void testDeleteDevice_USV_N() {
        try {
            m_accountManager.deleteDevice(INVALID_PARAMETER, m_OcAccountManagerAdapter);
            fail("deleteDevice does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'deleteDevice' function with empty deviceId and valid onDeleteListener
	 * @target void deleteDevice(String deviceId, OnDeleteListener onDeleteListener)
	 * @test_data deviceId and onDeleteListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform deleteDevice(String deviceId, OnDeleteListener onDeleteListener) API
	 * @post_condition None
	 * @expected searchUser throws OcException.
     */
    public void testDeleteDevice_ESV_N() {
        try {
            m_accountManager.deleteDevice(EMPTY_VALUE, m_OcAccountManagerAdapter);
            fail("deleteDevice does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'deleteDevice' function NULL valid deviceId and valid onDeleteListener
	 * @target void deleteDevice(String deviceId, OnDeleteListener onDeleteListener)
	 * @test_data deviceId and onDeleteListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform deleteDevice(String deviceId, OnDeleteListener onDeleteListener) API
	 * @post_condition None
	 * @expected searchUser throws OcException.
     */
    public void testDeleteDevice_NV_N() {
        try {
            m_accountManager.deleteDevice(NULL_VAL, m_OcAccountManagerAdapter);
            fail("deleteDevice does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'deleteDevice' function with valid deviceId and NULL onDeleteListener
	 * @target void deleteDevice(String deviceId, OnDeleteListener onDeleteListener)
	 * @test_data deviceId and onDeleteListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform deleteDevice(String deviceId, OnDeleteListener onDeleteListener) API
	 * @post_condition None
	 * @expected searchUser throws OcException.
     */
    public void testDeleteDeviceNULLCallback_NV_N() {
        try {
            m_accountManager.deleteDevice(DEVICE_ID, IC_OcAccountManager_OnDeleteListener_NULL);
            fail("deleteDevice does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'createGroup' function with valid deviceId and OnPostListener
	 * @target void createGroup(AclGroupType groupType, OnPostListener onPostListener)
	 * @test_data grouptype and onPostListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform createGroup(AclGroupType groupType, OnPostListener onPostListener) API
	 * @post_condition None
	 * @expected searchUser throws OcException if failure.
     */
    public void testCreateGroup_SRC_P() {
        try {
            m_accountManager.createGroup(AclGroupType.PUBLIC, m_OcAccountManagerAdapter);
        } catch (OcException ex) {
            ex.printStackTrace();
            fail("deleteDevice API Exception occurred: " + ex.getLocalizedMessage());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'createGroup' function with valid deviceId and NULL OnPostListener
	 * @target void createGroup(AclGroupType groupType, OnPostListener onPostListener)
	 * @test_data grouptype and onPostListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform createGroup(AclGroupType groupType, OnPostListener onPostListener) API
	 * @post_condition None
	 * @expected searchUser throws OcException.
     */
    public void testCreateGroup_NV_N() {
        try {
            m_accountManager.createGroup(AclGroupType.PUBLIC, IC_OcAccountManager_OnPostListener_NULL);
            fail("createGroup does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'getGroupList' function with onGetListener
	 * @target void getGroupList(OnGetListener onGetListener)
	 * @test_data onGetListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform getGroupList(OnGetListener onGetListener) API
	 * @post_condition None
	 * @expected searchUser throws OcException if failure.
     */
    public void testGetGroupList_SRC_P() {
        try {
            m_accountManager.getGroupList(m_OcAccountManagerAdapter);
        } catch (OcException ex) {
            ex.printStackTrace();
            fail("getGroupList API Exception occurred: " + ex.getLocalizedMessage());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'getGroupList' function with NULL onGetListener
	 * @target void getGroupList(OnGetListener onGetListener)
	 * @test_data onGetListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform getGroupList(OnGetListener onGetListener) API
	 * @post_condition None
	 * @expected searchUser throws OcException.
     */
    public void testGetGroupList_NV_N() {
        try {
            m_accountManager.getGroupList(IC_OcAccountManager_OnGetListener_NULL);
            fail("getGroupList does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'deleteGroup' function with valid groupId and onGetListener
	 * @target void deleteGroup(String groupId, OnDeleteListener onDeleteListener)
	 * @test_data groupI and onDeleteListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform deleteGroup(String groupId, OnDeleteListener onDeleteListener) API
	 * @post_condition None
	 * @expected deleteGroup throws OcException if failure.
     */
    public void testDeleteGroup_SRC_P() {
        try {
            m_accountManager.deleteGroup(GROUP_ID, m_OcAccountManagerAdapter);
        } catch (OcException ex) {
            ex.printStackTrace();
            fail("getGroupList API Exception occurred: " + ex.getLocalizedMessage());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'deleteGroup' function with invalid groupId and valid onDeleteListener
	 * @target void deleteGroup(String groupId, OnDeleteListener onDeleteListener)
	 * @test_data groupI and onDeleteListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform deleteGroup(String groupId, OnDeleteListener onDeleteListener) API
	 * @post_condition None
	 * @expected deleteGroup throws OcException.
     */
    public void testDeleteGroup_USV_N() {
        try {
            m_accountManager.deleteGroup(INVALID_PARAMETER, m_OcAccountManagerAdapter);
            fail("deleteGroup does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'deleteGroup' function with empty groupId and valid onDeleteListener
	 * @target void deleteGroup(String groupId, OnDeleteListener onDeleteListener)
	 * @test_data groupI and onDeleteListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform deleteGroup(String groupId, OnDeleteListener onDeleteListener) API
	 * @post_condition None
	 * @expected deleteGroup throws OcException.
     */
    public void testDeleteGroup_ESV_N() {
        try {
            m_accountManager.deleteGroup(EMPTY_VALUE, m_OcAccountManagerAdapter);
            fail("deleteGroup does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'deleteGroup' function with NULL groupId and valid onDeleteListener
	 * @target void deleteGroup(String groupId, OnDeleteListener onDeleteListener)
	 * @test_data groupI and onDeleteListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform deleteGroup(String groupId, OnDeleteListener onDeleteListener) API
	 * @post_condition None
	 * @expected deleteGroup throws OcException.
     */
    public void testDeleteGroup_NV_N() {
        try {
            m_accountManager.deleteGroup(NULL_VAL, m_OcAccountManagerAdapter);
            fail("deleteGroup does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'deleteGroup' function with groupId and NULL onDeleteListener
	 * @target void deleteGroup(String groupId, OnDeleteListener onDeleteListener)
	 * @test_data groupI and onDeleteListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform deleteGroup(String groupId, OnDeleteListener onDeleteListener) API
	 * @post_condition None
	 * @expected deleteGroup throws OcException.
     */
    public void testDeleteGroupNULLCallback_NV_N() {
        try {
            m_accountManager.deleteGroup(GROUP_ID, IC_OcAccountManager_OnDeleteListener_NULL);
            fail("deleteGroup does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'joinGroup' function with groupId and onPostListener
	 * @target void joinGroup(String groupId, OnPostListener onPostListener)
	 * @test_data groupI and onPostListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform joinGroup(String groupId, OnPostListener onPostListener) API
	 * @post_condition None
	 * @expected joinGroup throws OcException if fail.
     */
    public void testJoinGroup_SRC_P() {
        try {
            m_accountManager.joinGroup(GROUP_ID, m_OcAccountManagerAdapter);
        } catch (OcException ex) {
            ex.printStackTrace();
            fail("joinGroup API Exception occurred: " + ex.getLocalizedMessage());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'joinGroup' function with invalid groupId and valid onPostListener
	 * @target void joinGroup(String groupId, OnPostListener onPostListener)
	 * @test_data groupI and onPostListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform joinGroup(String groupId, OnPostListener onPostListener) API
	 * @post_condition None
	 * @expected joinGroup throws OcException.
     */
    public void testJoinGroup_USV_N() {
        try {
            m_accountManager.joinGroup(INVALID_PARAMETER, m_OcAccountManagerAdapter);
            fail("joinGroup does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'joinGroup' function with enpty groupId and valid onPostListener
	 * @target void joinGroup(String groupId, OnPostListener onPostListener)
	 * @test_data groupI and onPostListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform joinGroup(String groupId, OnPostListener onPostListener) API
	 * @post_condition None
	 * @expected joinGroup throws OcException.
     */
    public void testJoinGroup_ESV_N() {
        try {
            m_accountManager.deleteGroup(EMPTY_VALUE, m_OcAccountManagerAdapter);
            fail("joinGroup does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'joinGroup' function with NULL groupId and valid onPostListener
	 * @target void joinGroup(String groupId, OnPostListener onPostListener)
	 * @test_data groupI and onPostListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform joinGroup(String groupId, OnPostListener onPostListener) API
	 * @post_condition None
	 * @expected joinGroup throws OcException.
     */
    public void testJoinGroup_NV_N() {
        try {
            m_accountManager.deleteGroup(NULL_VAL, m_OcAccountManagerAdapter);
            fail("joinGroup does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'joinGroup' function with groupId and NULL onPostListener
	 * @target void joinGroup(String groupId, OnPostListener onPostListener)
	 * @test_data groupI and onPostListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform joinGroup(String groupId, OnPostListener onPostListener) API
	 * @post_condition None
	 * @expected joinGroup throws OcException.
     */
    public void testJoinGroupNULLCallback_NV_N() {
        try {
            m_accountManager.joinGroup(GROUP_ID, IC_OcAccountManager_OnPostListener_NULL);
            fail("deleteGroup does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'addDeviceToGroup' function with groupId, deviceId and onPostListener
	 * @target void addDeviceToGroup(String groupId, List<String> deviceId, OnPostListener onPostListener)
	 * @test_data groupId, deviceId and onPostListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform addDeviceToGroup(String groupId, List<String> deviceId, OnPostListener onPostListener) API
	 * @post_condition None
	 * @expected addDeviceToGroup throws OcException if fail.
     */
    public void testaddDeviceToGroup_SRC_P() {
        try {
            m_accountManager.addDeviceToGroup(GROUP_ID, LIST_DEVICE_ID, m_OcAccountManagerAdapter);
        } catch (OcException ex) {
            ex.printStackTrace();
            fail("addDeviceToGroup API Exception occurred: " + ex.getLocalizedMessage());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'addDeviceToGroup' function with invalid groupId, valid deviceId and onPostListener
	 * @target void addDeviceToGroup(String groupId, List<String> deviceId, OnPostListener onPostListener)
	 * @test_data groupId, deviceId and onPostListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform addDeviceToGroup(String groupId, List<String> deviceId, OnPostListener onPostListener) API
	 * @post_condition None
	 * @expected addDeviceToGroup throws OcException.
     */
    public void testaddDeviceToGroup_USV_N() {
        try {
            m_accountManager.addDeviceToGroup(INVALID_PARAMETER, LIST_DEVICE_ID, m_OcAccountManagerAdapter);
            fail("addDeviceToGroup does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'addDeviceToGroup' function with empty groupId, deviceId and valid onPostListener
	 * @target void addDeviceToGroup(String groupId, List<String> deviceId, OnPostListener onPostListener)
	 * @test_data groupId, deviceId and onPostListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform addDeviceToGroup(String groupId, List<String> deviceId, OnPostListener onPostListener) API
	 * @post_condition None
	 * @expected addDeviceToGroup throws OcException.
     */
    public void testaddDeviceToGroup_ESV_N() {
        try {
            m_accountManager.addDeviceToGroup(EMPTY_VALUE, EMPTY_LIST_DEVICE_ID, m_OcAccountManagerAdapter);
            fail("addDeviceToGroup does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'addDeviceToGroup' function with NULL groupId, valid deviceId and valid onPostListener
	 * @target void addDeviceToGroup(String groupId, List<String> deviceId, OnPostListener onPostListener)
	 * @test_data groupId, deviceId and onPostListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform addDeviceToGroup(String groupId, List<String> deviceId, OnPostListener onPostListener) API
	 * @post_condition None
	 * @expected addDeviceToGroup throws OcException.
     */
    public void testaddDeviceToGroup_NV_N() {
        try {
            m_accountManager.addDeviceToGroup(NULL_VAL, LIST_DEVICE_ID, m_OcAccountManagerAdapter);
            fail("addDeviceToGroup does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'addDeviceToGroup' function with NULL groupId, valid deviceId and valid onPostListener
	 * @target void addDeviceToGroup(String groupId, List<String> deviceId, OnPostListener onPostListener)
	 * @test_data groupId, deviceId and onPostListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform addDeviceToGroup(String groupId, List<String> deviceId, OnPostListener onPostListener) API
	 * @post_condition None
	 * @expected addDeviceToGroup throws OcException.
     */
    public void testaddDeviceToGroupNULLCallback_NV_N() {
        try {
            m_accountManager.addDeviceToGroup(GROUP_ID, LIST_DEVICE_ID, IC_OcAccountManager_OnPostListener_NULL);
            fail("addDeviceToGroup does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'getGroupInfo' function with groupId and onGetListener
	 * @target void getGroupInfo(String groupId, OnGetListener onGetListener)
	 * @test_data groupId and onGetListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform getGroupInfo(String groupId, OnGetListener onGetListener) API
	 * @post_condition None
	 * @expected getGroupInfo throws OcException if fail.
     */
    public void testGetGroupInfo_SRC_P() {
        try {
            m_accountManager.getGroupInfo(GROUP_ID, m_OcAccountManagerAdapter);
        } catch (OcException ex) {
            ex.printStackTrace();
            fail("getGroupInfo API Exception occurred: " + ex.getLocalizedMessage());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'getGroupInfo' function with invalid groupId and valid onGetListener
	 * @target void getGroupInfo(String groupId, OnGetListener onGetListener)
	 * @test_data groupId and onGetListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform getGroupInfo(String groupId, OnGetListener onGetListener) API
	 * @post_condition None
	 * @expected getGroupInfo throws OcException.
     */
    public void testGetGroupInfo_USV_N() {
        try {
            m_accountManager.getGroupInfo(INVALID_PARAMETER, m_OcAccountManagerAdapter);
            fail("getGroupInfo does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'getGroupInfo' function with empty groupId and valid onGetListener
	 * @target void getGroupInfo(String groupId, OnGetListener onGetListener)
	 * @test_data groupId and onGetListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform getGroupInfo(String groupId, OnGetListener onGetListener) API
	 * @post_condition None
	 * @expected getGroupInfo throws OcException.
     */
    public void testGetGroupInfo_ESV_N() {
        try {
            m_accountManager.getGroupInfo(EMPTY_VALUE, m_OcAccountManagerAdapter);
            fail("getGroupInfo does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'getGroupInfo' function with NULL groupId and valid onGetListener
	 * @target void getGroupInfo(String groupId, OnGetListener onGetListener)
	 * @test_data groupId and onGetListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform getGroupInfo(String groupId, OnGetListener onGetListener) API
	 * @post_condition None
	 * @expected getGroupInfo throws OcException.
     */
    public void testGetGroupInfo_NV_N() {
        try {
            m_accountManager.getGroupInfo(NULL_VAL, m_OcAccountManagerAdapter);
            fail("getGroupInfo does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'getGroupInfo' function with NULL groupId and valid onGetListener
	 * @target void getGroupInfo(String groupId, OnGetListener onGetListener)
	 * @test_data groupId and onGetListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform getGroupInfo(String groupId, OnGetListener onGetListener) API
	 * @post_condition None
	 * @expected getGroupInfo throws OcException.
     */
    public void testGetGroupInfoNULLCallback_NV_N() {
        try {
            m_accountManager.getGroupInfo(GROUP_ID, IC_OcAccountManager_OnGetListener_NULL);
            fail("getGroupInfo does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'leaveGroup' function with groupId and onDeleteListener
	 * @target void leaveGroup(String groupId, OnDeleteListener onDeleteListener)
	 * @test_data groupId and onDeleteListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform leaveGroup(String groupId, OnDeleteListener onDeleteListener) API
	 * @post_condition None
	 * @expected leaveGroup throws OcException if fail.
     */
    public void testLeaveGroup_SRC_P() {
        try {
            m_accountManager.leaveGroup(GROUP_ID, m_OcAccountManagerAdapter);
        } catch (OcException ex) {
            ex.printStackTrace();
            fail("leaveGroup API Exception occurred: " + ex.getLocalizedMessage());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'leaveGroup' function with invalid groupId and valid onDeleteListener
	 * @target void leaveGroup(String groupId, OnDeleteListener onDeleteListener)
	 * @test_data groupId and onDeleteListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform leaveGroup(String groupId, OnDeleteListener onDeleteListener) API
	 * @post_condition None
	 * @expected leaveGroup throws OcException.
     */
    public void testLeaveGroup_USV_N() {
        try {
            m_accountManager.leaveGroup(INVALID_PARAMETER, m_OcAccountManagerAdapter);
            fail("leaveGroup does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'leaveGroup' function with empty groupId and valid onDeleteListener
	 * @target void leaveGroup(String groupId, OnDeleteListener onDeleteListener)
	 * @test_data groupId and onDeleteListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform leaveGroup(String groupId, OnDeleteListener onDeleteListener) API
	 * @post_condition None
	 * @expected leaveGroup throws OcException.
     */
    public void testLeaveGroup_ESV_N() {
        try {
            m_accountManager.leaveGroup(EMPTY_VALUE, m_OcAccountManagerAdapter);
            fail("leaveGroup does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'leaveGroup' function with NULL groupId and valid onDeleteListener
	 * @target void leaveGroup(String groupId, OnDeleteListener onDeleteListener)
	 * @test_data groupId and onDeleteListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform leaveGroup(String groupId, OnDeleteListener onDeleteListener) API
	 * @post_condition None
	 * @expected leaveGroup throws OcException.
     */
    public void testLeaveGroup_NV_N() {
        try {
            m_accountManager.leaveGroup(NULL_VAL, m_OcAccountManagerAdapter);
            fail("leaveGroup does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'leaveGroup' function with groupId and NULL onDeleteListener
	 * @target void leaveGroup(String groupId, OnDeleteListener onDeleteListener)
	 * @test_data groupId and onDeleteListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform leaveGroup(String groupId, OnDeleteListener onDeleteListener) API
	 * @post_condition None
	 * @expected leaveGroup throws OcException.
     */
    public void testLeaveGroupNULLCallback_NV_N() {
        try {
            m_accountManager.leaveGroup(GROUP_ID, IC_OcAccountManager_OnDeleteListener_NULL);
            fail("leaveGroup does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'deleteDeviceFromGroup' function with groupId, deviceId and onDeleteListener
	 * @target void deleteDeviceFromGroup(String groupId, List<String> deviceId, OnDeleteListener onDeleteListener)
	 * @test_data groupId and onDeleteListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform deleteDeviceFromGroup(String groupId, List<String> deviceId, OnDeleteListener onDeleteListener) API
	 * @post_condition None
	 * @expected deleteDeviceFromGroup throws OcException if fail.
     */
    public void testDeleteDeviceFromGroup_SRC_P() {
        try {
            m_accountManager.deleteDeviceFromGroup(GROUP_ID, LIST_DEVICE_ID, m_OcAccountManagerAdapter);
        } catch (OcException ex) {
            ex.printStackTrace();
            fail("deleteDeviceFromGroup API Exception occurred: " + ex.getLocalizedMessage());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'deleteDeviceFromGroup' function with invalid groupId, valid deviceId and onDeleteListener
	 * @target void deleteDeviceFromGroup(String groupId, List<String> deviceId, OnDeleteListener onDeleteListener)
	 * @test_data groupId and onDeleteListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform deleteDeviceFromGroup(String groupId, List<String> deviceId, OnDeleteListener onDeleteListener) API
	 * @post_condition None
	 * @expected deleteDeviceFromGroup throws OcException.
     */
    public void testDeleteDeviceFromGroup_USV_N() {
        try {
            m_accountManager.deleteDeviceFromGroup(INVALID_PARAMETER, LIST_DEVICE_ID, m_OcAccountManagerAdapter);
            fail("deleteDeviceFromGroup does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'deleteDeviceFromGroup' function with empty groupId, deviceId and valid onDeleteListener
	 * @target void deleteDeviceFromGroup(String groupId, List<String> deviceId, OnDeleteListener onDeleteListener)
	 * @test_data groupId and onDeleteListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform deleteDeviceFromGroup(String groupId, List<String> deviceId, OnDeleteListener onDeleteListener) API
	 * @post_condition None
	 * @expected deleteDeviceFromGroup throws OcException.
     */
    public void testDeleteDeviceFromGroup_ESV_N() {
        try {
            m_accountManager.deleteDeviceFromGroup(EMPTY_VALUE, EMPTY_LIST_DEVICE_ID, m_OcAccountManagerAdapter);
            fail("deleteDeviceFromGroup does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'deleteDeviceFromGroup' function with NULL groupId, valid deviceId and valid onDeleteListener
	 * @target void deleteDeviceFromGroup(String groupId, List<String> deviceId, OnDeleteListener onDeleteListener)
	 * @test_data groupId and onDeleteListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform deleteDeviceFromGroup(String groupId, List<String> deviceId, OnDeleteListener onDeleteListener) API
	 * @post_condition None
	 * @expected deleteDeviceFromGroup throws OcException.
     */
    public void testDeleteDeviceFromGroup_NV_N() {
        try {
            m_accountManager.deleteDeviceFromGroup(NULL_VAL, LIST_DEVICE_ID, m_OcAccountManagerAdapter);
            fail("deleteDeviceFromGroup does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'deleteDeviceFromGroup' function with NULL groupId, valid deviceId and valid onDeleteListener
	 * @target void deleteDeviceFromGroup(String groupId, List<String> deviceId, OnDeleteListener onDeleteListener)
	 * @test_data groupId and onDeleteListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform deleteDeviceFromGroup(String groupId, List<String> deviceId, OnDeleteListener onDeleteListener) API
	 * @post_condition None
	 * @expected deleteDeviceFromGroup throws OcException.
     */
    public void testDeleteDeviceFromGroupNULLCallback_NV_N() {
        try {
           
            m_accountManager.deleteDeviceFromGroup(GROUP_ID, LIST_DEVICE_ID, IC_OcAccountManager_OnDeleteListener_NULL);
            fail("deleteDeviceFromGroup does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'observeGroup' function with groupId and onObserveListener
	 * @target void observeGroup(String groupId, OnObserveListener onObserveListener)
	 * @test_data groupId and onDeleteListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform observeGroup(String groupId, OnObserveListener onObserveListener) API
	 * @post_condition None
	 * @expected observeGroup throws OcException if fail.
     */
    public void testObserveGroup_SRC_P() {
        try {
            m_accountManager.observeGroup(GROUP_ID, m_OcAccountManagerAdapter);
        } catch (OcException ex) {
            ex.printStackTrace();
            fail("observeGroup API Exception occurred: " + ex.getLocalizedMessage());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'observeGroup' function with invalid groupId and valid onObserveListener
	 * @target void observeGroup(String groupId, OnObserveListener onObserveListener)
	 * @test_data groupId and onDeleteListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform observeGroup(String groupId, OnObserveListener onObserveListener) API
	 * @post_condition None
	 * @expected observeGroup throws OcException.
     */
    public void testObserveGroup_USV_N() {
        try {
            m_accountManager.observeGroup(INVALID_PARAMETER, m_OcAccountManagerAdapter);
            fail("observeGroup does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'observeGroup' function with empty groupId and valid onObserveListener
	 * @target void observeGroup(String groupId, OnObserveListener onObserveListener)
	 * @test_data groupId and onDeleteListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform observeGroup(String groupId, OnObserveListener onObserveListener) API
	 * @post_condition None
	 * @expected observeGroup throws OcException.
     */
    public void testObserveGroup_ESV_N() {
        try {
            m_accountManager.observeGroup(EMPTY_VALUE, m_OcAccountManagerAdapter);
            fail("observeGroup does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'observeGroup' function with NULL groupId and valid onObserveListener
	 * @target void observeGroup(String groupId, OnObserveListener onObserveListener)
	 * @test_data groupId and onDeleteListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform observeGroup(String groupId, OnObserveListener onObserveListener) API
	 * @post_condition None
	 * @expected observeGroup throws OcException.
     */
    public void testObserveGroup_NV_N() {
        try {
            m_accountManager.observeGroup(NULL_VAL, m_OcAccountManagerAdapter);
            fail("observeGroup does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'observeGroup' function with invalid groupId and valid onObserveListener
	 * @target void observeGroup(String groupId, OnObserveListener onObserveListener)
	 * @test_data groupId and onDeleteListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform observeGroup(String groupId, OnObserveListener onObserveListener) API
	 * @post_condition None
	 * @expected observeGroup throws OcException.
     */
    public void testObserveGroupNULLCallback_NV_N() {
        try {
            m_accountManager.observeGroup(GROUP_ID, (OnObserveListener)IC_OcAccountManager_OnObserveListener_NULL);
            fail("observeGroup does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'cancelObserveGroup' function with groupId
	 * @target void cancelObserveGroup(String groupId)
	 * @test_data groupId
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform cancelObserveGroup(String groupId) API
	 * @post_condition None
	 * @expected cancelObserveGroup throws OcException if fail.
     */
    public void testCancelObserveGroup_SRC_P() {
        try {
	    m_accountManager.observeGroup(GROUP_ID, m_OcAccountManagerAdapter);
            m_accountManager.cancelObserveGroup(GROUP_ID);
        } catch (OcException ex) {
            ex.printStackTrace();
            fail("cancelObserveGroup API Exception occurred: " + ex.getLocalizedMessage());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'cancelObserveGroup' function with invalid groupId
	 * @target void cancelObserveGroup(String groupId)
	 * @test_data groupId
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform cancelObserveGroup(String groupId) API
	 * @post_condition None
	 * @expected cancelObserveGroup throws OcException.
     */
    public void testCancelObserveGroup_USV_N() {
        try {
            m_accountManager.cancelObserveGroup(INVALID_PARAMETER);
            fail("cancelObserveGroup does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'cancelObserveGroup' function with invalid groupId
	 * @target void cancelObserveGroup(String groupId)
	 * @test_data groupId
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform cancelObserveGroup(String groupId) API
	 * @post_condition None
	 * @expected cancelObserveGroup throws OcException.
     */
    public void testCancelObserveGroup_ESV_N() {
        try {
            m_accountManager.cancelObserveGroup(EMPTY_VALUE);
            fail("cancelObserveGroup does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'cancelObserveGroup' function with NULL groupId
	 * @target void cancelObserveGroup(String groupId)
	 * @test_data groupId
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform cancelObserveGroup(String groupId) API
	 * @post_condition None
	 * @expected cancelObserveGroup throws OcException.
     */
    public void testCancelObserveGroup_NV_N() {
        try {
            m_accountManager.cancelObserveGroup(NULL_VAL);
            fail("cancelObserveGroup does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'observeInvitation' function with onObserveListener
	 * @target void observeInvitation(OnObserveListener onObserveListener)
	 * @test_data groupId
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform observeInvitation(OnObserveListener onObserveListener) API
	 * @post_condition None
	 * @expected observeInvitation throws OcException if fail.
     */
    public void testObserveInvitation_SRC_P() {
        try {
            m_accountManager.observeInvitation( m_OcAccountManagerAdapter);
        } catch (OcException ex) {
            ex.printStackTrace();
            fail("observeInvitation API Exception occurred: " + ex.getLocalizedMessage());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'observeInvitation' function with NULL onObserveListener
	 * @target void observeInvitation(OnObserveListener onObserveListener)
	 * @test_data groupId
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform observeInvitation(OnObserveListener onObserveListener) API
	 * @post_condition None
	 * @expected observeInvitation throws OcException.
     */
    public void testObserveInvitation_NV_N() {
        try {
            m_accountManager.observeInvitation((OnObserveListener) IC_OcAccountManager_OnObserveListener_NULL);
            fail("observeInvitation does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'cancelObserveInvitation' function poditively
	 * @target void cancelObserveInvitation()
	 * @test_data groupId
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform cancelObserveInvitation() API
	 * @post_condition None
	 * @expected cancelObserveInvitation throws OcException if fail.
     */
    public void testCancelObserveInvitation_SRC_P() {
        try {
	    //m_accountManager.sendInvitation(GROUP_ID, USER_UUID, m_OcAccountManagerAdapter);
	    m_accountManager.observeInvitation( m_OcAccountManagerAdapter);
            m_accountManager.cancelObserveInvitation();
        } catch (OcException ex) {
            ex.printStackTrace();
            fail("cancelObserveInvitation API Exception occurred: " + ex.getLocalizedMessage());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'sendInvitation' function with groupId, UserUuid and onPostListener
	 * @target void sendInvitation(String groupId, String userUuid, OnPostListener onPostListener)
	 * @test_data groupId, UserUuid and onPostListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform sendInvitation(String groupId, String userUuid, OnPostListener onPostListener) API
	 * @post_condition None
	 * @expected sendInvitation throws OcException if fail.
     */
    public void testSendInvitation_SRC_P() {
        try {
            m_accountManager.sendInvitation(GROUP_ID, USER_UUID, m_OcAccountManagerAdapter);
        } catch (OcException ex) {
            ex.printStackTrace();
            fail("sendInvitation API Exception occurred: " + ex.getLocalizedMessage());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'sendInvitation' function with invalid groupId, UserUuid and valid onPostListener
	 * @target void sendInvitation(String groupId, String userUuid, OnPostListener onPostListener)
	 * @test_data groupId, UserUuid and onPostListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform sendInvitation(String groupId, String userUuid, OnPostListener onPostListener) API
	 * @post_condition None
	 * @expected sendInvitation throws OcException.
     */
    public void testSendInvitation_USV_N() {
        try {
            m_accountManager.sendInvitation(INVALID_PARAMETER, INVALID_PARAMETER, m_OcAccountManagerAdapter);
            fail("observeInvitation does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'sendInvitation' function with empty groupId, UserUuid and valid onPostListener
	 * @target void sendInvitation(String groupId, String userUuid, OnPostListener onPostListener)
	 * @test_data groupId, UserUuid and onPostListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform sendInvitation(String groupId, String userUuid, OnPostListener onPostListener) API
	 * @post_condition None
	 * @expected sendInvitation throws OcException.
     */
    public void testSendInvitation_ESV_N() {
        try {
            m_accountManager.sendInvitation(EMPTY_VALUE, EMPTY_VALUE, m_OcAccountManagerAdapter);
            fail("observeInvitation does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'sendInvitation' function with NULL groupId, UserUuid and valid onPostListener
	 * @target void sendInvitation(String groupId, String userUuid, OnPostListener onPostListener)
	 * @test_data groupId, UserUuid and onPostListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform sendInvitation(String groupId, String userUuid, OnPostListener onPostListener) API
	 * @post_condition None
	 * @expected sendInvitation throws OcException.
     */
    public void testSendInvitation_NV_N() {
        try {
            m_accountManager.sendInvitation(NULL_VAL, NULL_VAL, m_OcAccountManagerAdapter);
            fail("observeInvitation does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'sendInvitation' function with NULL groupId, UserUuid and valid onPostListener
	 * @target void sendInvitation(String groupId, String userUuid, OnPostListener onPostListener)
	 * @test_data groupId, UserUuid and onPostListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform sendInvitation(String groupId, String userUuid, OnPostListener onPostListener) API
	 * @post_condition None
	 * @expected sendInvitation throws OcException.
     */
    public void testSendInvitationNULLCallback_NV_N() {
        try {
            m_accountManager.sendInvitation(GROUP_ID, USER_UUID, IC_OcAccountManager_OnPostListener_NULL);
            fail("observeInvitation does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'cancelInvitation' function with groupId, UserUuid and onDeleteListener
	 * @target void cancelInvitation(String groupId, String userUuid, OnDeleteListener onDeleteListener)
	 * @test_data groupId, UserUuid and onDeleteListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform cancelInvitation(String groupId, String userUuid, OnDeleteListener onDeleteListener) API
	 * @post_condition None
	 * @expected cancelInvitation throws OcException if fail.
     */
    public void testCancelInvitation_SRC_P() {
        try {
            m_accountManager.cancelInvitation(GROUP_ID, USER_UUID, m_OcAccountManagerAdapter);
        } catch (OcException ex) {
            ex.printStackTrace();
            fail("cancelInvitation API Exception occurred: " + ex.getLocalizedMessage());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'cancelInvitation' function with invalid groupId, UserUuid and valid onDeleteListener
	 * @target void cancelInvitation(String groupId, String userUuid, OnDeleteListener onDeleteListener)
	 * @test_data groupId, UserUuid and onDeleteListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform cancelInvitation(String groupId, String userUuid, OnDeleteListener onDeleteListener) API
	 * @post_condition None
	 * @expected cancelInvitation throws OcException.
     */
    public void testCancelInvitation_USV_N() {
        try {
            m_accountManager.cancelInvitation(INVALID_PARAMETER, INVALID_PARAMETER, m_OcAccountManagerAdapter);
            fail("cancelInvitation does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'cancelInvitation' function with empty groupId, UserUuid and valid onDeleteListener
	 * @target void cancelInvitation(String groupId, String userUuid, OnDeleteListener onDeleteListener)
	 * @test_data groupId, UserUuid and onDeleteListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform cancelInvitation(String groupId, String userUuid, OnDeleteListener onDeleteListener) API
	 * @post_condition None
	 * @expected cancelInvitation throws OcException.
     */
    public void testCancelInvitation_ESV_N() {
        try {
            m_accountManager.cancelInvitation(EMPTY_VALUE, EMPTY_VALUE, m_OcAccountManagerAdapter);
            fail("cancelInvitation does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'cancelInvitation' function with NULL groupId, UserUuid and valid onDeleteListener
	 * @target void cancelInvitation(String groupId, String userUuid, OnDeleteListener onDeleteListener)
	 * @test_data groupId, UserUuid and onDeleteListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform cancelInvitation(String groupId, String userUuid, OnDeleteListener onDeleteListener) API
	 * @post_condition None
	 * @expected cancelInvitation throws OcException.
     */
    public void testCancelInvitation_NV_N() {
        try {
            m_accountManager.cancelInvitation(NULL_VAL, NULL_VAL, m_OcAccountManagerAdapter);
            fail("cancelInvitation does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'cancelInvitation' function with NULL groupId, UserUuid and valid onDeleteListener
	 * @target void cancelInvitation(String groupId, String userUuid, OnDeleteListener onDeleteListener)
	 * @test_data groupId, UserUuid and onDeleteListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform cancelInvitation(String groupId, String userUuid, OnDeleteListener onDeleteListener) API
	 * @post_condition None
	 * @expected cancelInvitation throws OcException.
     */
    public void testCancelInvitationNULLCallback_NV_N() {
        try {
            m_accountManager.cancelInvitation(GROUP_ID, USER_UUID, IC_OcAccountManager_OnDeleteListener_NULL);
            fail("cancelInvitation does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'deleteInvitation' function with groupId and onDeleteListener
	 * @target void deleteInvitation(String groupId, OnDeleteListener onDeleteListener)
	 * @test_data groupId and onDeleteListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform deleteInvitation(String groupId, OnDeleteListener onDeleteListener) API
	 * @post_condition None
	 * @expected deleteInvitation throws OcException if fail.
     */
    public void testDeleteInvitation_SRC_P() {
        try {
            m_accountManager.deleteInvitation(GROUP_ID, m_OcAccountManagerAdapter);
        } catch (OcException ex) {
            ex.printStackTrace();
            fail("deleteInvitation API Exception occurred: " + ex.getLocalizedMessage());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'deleteInvitation' function with invalid groupId and onDeleteListener
	 * @target void deleteInvitation(String groupId, OnDeleteListener onDeleteListener)
	 * @test_data groupId and onDeleteListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform deleteInvitation(String groupId, OnDeleteListener onDeleteListener) API
	 * @post_condition None
	 * @expected deleteInvitation throws OcException.
     */
    public void testDeleteInvitation_USV_N() {
        try {
            m_accountManager.deleteInvitation(INVALID_PARAMETER, m_OcAccountManagerAdapter);
            fail("cancelInvitation does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'deleteInvitation' function with empty groupId and valid onDeleteListener
	 * @target void deleteInvitation(String groupId, OnDeleteListener onDeleteListener)
	 * @test_data groupId and onDeleteListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform deleteInvitation(String groupId, OnDeleteListener onDeleteListener) API
	 * @post_condition None
	 * @expected deleteInvitation throws OcException.
     */
    public void testDeleteInvitation_ESV_N() {
        try {
            m_accountManager.deleteInvitation(EMPTY_VALUE, m_OcAccountManagerAdapter);
            fail("cancelInvitation does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'deleteInvitation' function with NULL groupId and valid onDeleteListener
	 * @target void deleteInvitation(String groupId, OnDeleteListener onDeleteListener)
	 * @test_data groupId and onDeleteListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform deleteInvitation(String groupId, OnDeleteListener onDeleteListener) API
	 * @post_condition None
	 * @expected deleteInvitation throws OcException.
     */
    public void testDeleteInvitation_NV_N() {
        try {
            m_accountManager.deleteInvitation(NULL_VAL, m_OcAccountManagerAdapter);
            fail("cancelInvitation does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
    
    /**
     * @since 2016-08-29
	 * @see None
	 * @objective Test 'deleteInvitation' function with valid groupId and NULL onDeleteListener
	 * @target void deleteInvitation(String groupId, OnDeleteListener onDeleteListener)
	 * @test_data groupId and onDeleteListener
	 * @pre_condition constructAccountManagerObject(host, connectivity_type) API
	 * @procedure Perform deleteInvitation(String groupId, OnDeleteListener onDeleteListener) API
	 * @post_condition None
	 * @expected deleteInvitation throws OcException.
     */
    public void testDeleteInvitationNULLCallback_NV_N() {
        try {
            m_accountManager.deleteInvitation(GROUP_ID, IC_OcAccountManager_OnDeleteListener_NULL);
            fail("cancelInvitation does not throw exception!");
        } catch (OcException ex) {
            ex.printStackTrace();
            assertEquals(ErrorCode.INVALID_PARAM, ex.getErrorCode());
        }
    }
}
