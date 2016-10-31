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

package org.iotivity.cloud.ic.test.stc;

import static org.iotivity.cloud.ic.test.helper.ICHelperStaticUtil.*;

import java.util.EnumSet;
import java.util.List;

import org.iotivity.cloud.ic.test.helper.ICResourceDirectoryCommonAdapter;
import org.iotivity.cloud.ic.test.helper.ICHelper;
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

import android.test.AndroidTestCase;
import android.util.Log;

public class CIAccountMangerTest extends AndroidTestCase {

    private static final String TAG = "SignUp";
    public static final int    DATA_SIZE          = 3000;
    public static String       HOST               = "coap+tcp://127.0.0.1:5685";
    public static final String COAP_TCP           = "coap+tcp://";
    public static String       TCP_ADDRESS        = "192.168.0.1";
    public static String       TCP_PORT           = "8000";
    public static final String PORT_SEPARATOR     = ":";
    public static final String IP_ADDRESS         = "0.0.0.0";
    public static final int    IP_PORT            = 0;
    public static final String GET_COMMAND        = "get_command";
    public static final String STATE_KEY          = "state_key";
    public static final String STATE_GET          = "state_get";
    public static final String LARGE_KEY          = "large_key";
    public static final String LARGE_GET          = "large_get";
    public static final String RESOURCE_URI       = "/a/light";
    public static final String RESOURCE_TYPE      = "core.light";
    public static final String RESOURCE_INTERFACE = OcPlatform.DEFAULT_INTERFACE;
    public static final EnumSet<ResourceProperty> RESOURCE_PROPERTIES =
            EnumSet.of(ResourceProperty.DISCOVERABLE, ResourceProperty.OBSERVABLE);
    private QualityOfService mQos = QualityOfService.LOW;
    private OcAccountManager mAccountManager;
    private String mAuthCode = "633e6bc97a2ea71ef7df";


    protected void setUp() throws Exception {
        super.setUp();
    }

    protected void tearDown() throws Exception {
        super.tearDown();
    }

    /**
     * @since 2016-08-30
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
    
    OcAccountManager.OnPostListener onSignUp = new OcAccountManager.OnPostListener() {
        @Override
        public synchronized void onPostCompleted(List<OcHeaderOption> list,
                                                 OcRepresentation ocRepresentation) {
            Log.d(TAG,"signUp was successful");
            try {
                Object mUserUuid = ocRepresentation.getValue("uid");
                Object mAccesstoken = ocRepresentation.getValue("accesstoken");
                Object mRefreshtoken = ocRepresentation.getValue("refreshtoken");
                String tokenType = ocRepresentation.getValue("tokentype");
                Log.d(TAG+"\tuserID: " , mUserUuid.toString());
                Log.d(TAG+"\taccessToken: " , mAccesstoken.toString());
                Log.d(TAG+"\trefreshToken: " , mRefreshtoken.toString());
                Log.d(TAG+"\ttokenType: " , tokenType.toString());

                if (ocRepresentation.hasAttribute("expiresin"))
                {
                    int expiresIn = ocRepresentation.getValue("expiresin");
                    //Log.d(TAG+"\texpiresIn: " , expiresIn);
                }
            } catch (OcException e) {
                Log.e(TAG, e.toString());
            }
        }

        @Override
        public synchronized void onPostFailed(Throwable throwable) {
            Log.d("","Failed to signUp");
            if (throwable instanceof OcException) {
                OcException ocEx = (OcException) throwable;
                Log.e(TAG, ocEx.toString());
                ErrorCode errCode = ocEx.getErrorCode();
                Log.d(TAG+"Error code: " , errCode.toString());
            }
        }
    };

    public void testPublishResourceToRD_SRC_FSV_P() {
        
        PlatformConfig cfg = new PlatformConfig(getContext(),
                ServiceType.IN_PROC,
                ModeType.CLIENT_SERVER,
                IP_ADDRESS,
                IP_PORT,
                mQos);
        OcPlatform.Configure(cfg);
        try {
            mAccountManager = OcPlatform.constructAccountManagerObject(
                    HOST,
                    EnumSet.of(OcConnectivityType.CT_ADAPTER_TCP));
        } catch (OcException e) {
            e.printStackTrace();
        }
        
        try {
            
            mAccountManager.signUp("github", mAuthCode, onSignUp);
        } catch (OcException e) {
            e.printStackTrace();
        }

    }

 
 }
