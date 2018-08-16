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

package org.iotivity.test.ic.app;

import java.util.List;
import java.util.Map;
import java.util.EnumSet;
import java.util.HashMap;

import android.util.Log;
import android.content.Context;
import android.test.InstrumentationTestCase;
import android.app.Activity;
import android.app.Instrumentation;

import org.iotivity.base.OcConnectivityType;
import org.iotivity.base.OcAccountManager.OnDeleteListener;
import org.iotivity.base.OcAccountManager.OnGetListener;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcResource;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcException;
import org.iotivity.base.OcAccountManager.OnObserveListener;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.ServiceType;
import org.iotivity.base.ModeType;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.EntityHandlerResult;
import org.iotivity.base.OcResourceRequest;
import org.iotivity.base.ResourceProperty;
import org.iotivity.base.OcResourceHandle;
import org.iotivity.base.OcAccountManager.OnPostListener;
import org.iotivity.base.OcHeaderOption;
import org.iotivity.base.ErrorCode;
import org.iotivity.base.OcRDClient;
import org.iotivity.base.OcAccountManager;

import org.iotivity.service.testapp.framework.Base;
import org.iotivity.service.testapp.framework.MenuInfo;
import static org.iotivity.test.ic.app.ICUtility.*;
import static org.iotivity.test.ic.app.ICMessageQueueHelper.*;
import static org.iotivity.test.ic.app.ICHelper.*;
import org.iotivity.test.ic.app.ICHelper.*;

import org.iotivity.cloud.*;
import org.iotivity.cloud.OcAccountManagerHelper;
import org.iotivity.configuration.*;
import org.iotivity.configuration.IConfiguration.*;

import org.iotivity.test.ic.app.MainActivity;

public class ICAction extends Base {
    private Context                          mControleeContext;
    private Context                          mControllerContext;
    private ICHelper                         mICHelper      = null;
    private OcAccountManagerHelper           mCloudHelper   = new OcAccountManagerHelper();
    private OcAccountManagerAdapter          mOcAccountManagerAdapter;
    private OcRepresentation                 mPropertyValue = new OcRepresentation();
    private OconGetGroupInfoAllAdapter       mOconGetGroupInfoAllAdapter;
    private OconGetGroupInfoAdapter          mOconGetGroupInfoAdapter;
    private ICResourceDirectoryCommonAdapter mICResourceDirectoryAdapter;
    private ICMessageAdapter                 micMessageAdapter;
    private OcAccountManager                 mController;
    private OcAccountManager                 mControllee;
    private ICMessageQueueHelper             mICMessageQueueHelper;
    private boolean                          mIslogIn       = false;
    private boolean                          mSwitchingFlag = true;
    private String                           mCmd;
    private String                           mProperty      = "testProperty";
    private String[]                         mValues        = { "testValue" };

    public ICAction() {
        mICHelper = ICHelper.getInstance();
        mOcAccountManagerAdapter = new OcAccountManagerAdapter();
        mOconGetGroupInfoAllAdapter = new OconGetGroupInfoAllAdapter();
        mOconGetGroupInfoAdapter = new OconGetGroupInfoAdapter();
        mICResourceDirectoryAdapter = new ICResourceDirectoryCommonAdapter();
        mICMessageQueueHelper = new ICMessageQueueHelper();
        micMessageAdapter = new ICMessageAdapter();
    }

    public void initOcPlatform(ModeType type, Activity mActivity,
            Context mContext) {
        PlatformConfig cfg = new PlatformConfig(mContext, ServiceType.IN_PROC,
                type, "0.0.0.0", 0, QualityOfService.LOW);
        OcPlatform.Configure(cfg);
        mControllee = OcAccountManagerHelper
                .getAccountMangerInstance(TLS.DISABLED);
        mControleeContext = mContext;
        //OcAccountManagerHelper.init(mContext);
    }

    public void initOcPlatformForConroller(ModeType type, Activity mActivity,
            Context mContext) {
        PlatformConfig cfg = new PlatformConfig(mContext, ServiceType.IN_PROC,
                type, "0.0.0.0", 0, QualityOfService.LOW);
        OcPlatform.Configure(cfg);
        mController = OcAccountManagerHelper
                .getAccountMangerInstance(TLS.DISABLED);
        mControllerContext = mContext;        
        //OcAccountManagerHelper.init(mContext);
    }

    public void startControlee() {
        if (null == mControllee) {
            mControllee = OcAccountManagerHelper
                    .getAccountMangerInstance(TLS.DISABLED);
        }
        showOutPut("Controlee start Successfully");
    }

    public void signUpWithControlee() {

        try {
            Thread.sleep(1000);
            OcAccountManagerHelper.init(getContext().getFilesDir().getPath());
            showOutPut("signUp Controlee with AuthCode "+OcAccountManagerHelper.s_mAuthCode);
            boolean mbool = mCloudHelper.singUp(mControllee,
                    DEFAULT_AUTH_PROVIDER, OcAccountManagerHelper.s_mAuthCode,
                    mCloudHelper);
            System.out.println("test for signUp");
            Thread.sleep(5000);
            if (mbool) {
                mIslogIn = true;
                showOutPut("Successfully SignUp into cloud from testapp.");
            } else {
                showOutPut("SignUp failed.");
            }
        } catch (Exception e) {
            showOutPut("Exception Occured..");
            Log.e(TAG, e.toString());
        }

    }

    public void signIntoCloud() {
        try {
            boolean mbool = mCloudHelper.singIn(mControllee,
                    OcAccountManagerHelper.s_mCloudUid,
                    OcAccountManagerHelper.s_mCloudAccessToken, mCloudHelper);
            Thread.sleep(10000);
            if (mbool) {
                mIslogIn = true;
                showOutPut("Successfully SignIn into cloud.");
            } else {
                showOutPut("SignIn failed.");
            }
        } catch (Exception e) {
            showOutPut("Exception Occured..");
            Log.e(TAG, e.toString());
        }
    }

    public void signOutfromCloud() {
        try {
            if (mCloudHelper.singOut(mControllee,
                    OcAccountManagerHelper.s_mCloudAccessToken, mCloudHelper)) {
                mIslogIn = true;
                showOutPut("Successfully SignOut from cloud.");
            } else {
                showOutPut("SignOut failed.");
            }
        } catch (Exception e) {
            showOutPut("Exception Occured..");
            Log.e(TAG, e.toString());
        }
    }

    public void createGroup() {
        mICHelper.showLog(LogLevel.DEBUG, "createGroup IN", false);
        if (!mIslogIn) {
            mICHelper.showLog(LogLevel.INFO, "***SignIn first***", false);
        } else {
            try {
                if (null == mControllee) {
                    showOutPut("mControllee is null");
                } else {
                    showOutPut("CreateGroup start....");
                    mControllee.createGroup(mOcAccountManagerAdapter);
                    Thread.sleep(3000);
                }
            } catch (Exception e) {
                mICHelper.showLog(LogLevel.ERROR,
                        "CreateGroup failed, due to " + e.getLocalizedMessage(),
                        true);
            }
        }
       mICHelper.showLog(LogLevel.DEBUG, "createGroup OUT", false);
     
    }

    public void createGroupwithQuery() {
        mICHelper.showLog(LogLevel.DEBUG, "createGroupwithQuery IN", false);
        if (!mIslogIn) {
            mICHelper.showLog(LogLevel.INFO, "***SignIn first***", false);
        } else {
            try {
                if (null == mControllee) {
                    showOutPut("mControllee is null");
                } else {
                    showOutPut("CreateGroup with Query start....");
                    Map<String, String> queryParameters = new HashMap<>();
                    queryParameters.put("gname", "IOTSRBD");
                    mControllee.createGroup(queryParameters,
                            mOcAccountManagerAdapter);
                    Thread.sleep(3000);
                }
            } catch (Exception e) {
                mICHelper.showLog(LogLevel.ERROR,
                        "CreateGroup failed, due to " + e.getLocalizedMessage(),
                        true);
            }
        }

        mICHelper.showLog(LogLevel.DEBUG, "createGroupwithQuery OUT", false);
    }

    public void sendInvitation() {
        mICHelper.showLog(LogLevel.DEBUG, "sendInvitation IN", false);
        if (!mIslogIn)

        {
            mICHelper.showLog(LogLevel.INFO, "***SignUp first***", false);
        } else {
            try {
                mControllee.sendInvitation(OcAccountManagerAdapter.sGroupId,
                        OcAccountManagerAdapter.sInviteeUuid,
                        mOcAccountManagerAdapter);
            } catch (Exception e) {
                mICHelper.showLog(LogLevel.ERROR,
                        "SearchUserInfo failed, due to "
                                + e.getLocalizedMessage(),
                        true);
            }
        }
        
        mICHelper.showLog(LogLevel.DEBUG, "sendInvitation OUT", false);
    }

    public void publishResource() {
        mICHelper.showLog(LogLevel.DEBUG, "publishResource IN", false);
        
        if (!mIslogIn)

        {
            mICHelper.showLog(LogLevel.INFO, "***SignUp first***", false);
        } else {
            if (sResourceHandleList == null) {
                mICMessageQueueHelper.createResource();
            }
            try {
                // Publish Virtual Resource to Resource-Directory.
                Log.d(TAG, "Publish Virtual Resource to Resource-Directory.");
                OcRDClient.publishResourceToRD(IC_HOST_ADDRESS,
                        EnumSet.of(OcConnectivityType.CT_ADAPTER_TCP),
                        mICResourceDirectoryAdapter);

                // Publish Local Resource to Resource-Directory.
                Log.d(TAG, "Publish Local Resource to Resource-Directory.");
                OcRDClient.publishResourceToRD(IC_HOST_ADDRESS,
                        EnumSet.of(OcConnectivityType.CT_ADAPTER_TCP),
                        sResourceHandleList, mICResourceDirectoryAdapter);
            } catch (OcException e) {
                Log.e(TAG, e.toString());
            }
        }

        mICHelper.showLog(LogLevel.DEBUG, "publishResource OUT", false);
    }

    public void publishTrougleResource() {

        mICHelper.showLog(LogLevel.DEBUG, "publishTrougleResource IN", false);
        if (!mIslogIn)

        {
            mICHelper.showLog(LogLevel.INFO, "***SignUp first***", false);
        } else {
            Log.d(TAG, "Implement in future.");
        }
        mICHelper.showLog(LogLevel.DEBUG, "publishTrougleResource OUT", false);
    }

    public void startController() {
        if (null == mController) {
            mController = OcAccountManagerHelper
                    .getAccountMangerInstance(TLS.DISABLED);
        }
        showOutPut("Controller start Successfully");
    }

    public void signUpWithController() {
        try {
            Thread.sleep(1000);
            OcAccountManagerHelper.init(getContext().getFilesDir().getPath());
            showOutPut("signUp Controller with AuthCode "+OcAccountManagerHelper.s_mAuthCode);
            boolean mbool = mCloudHelper.singUp(mController,
                    DEFAULT_AUTH_PROVIDER, OcAccountManagerHelper.s_mAuthCode,
                    mCloudHelper);
            Thread.sleep(5000);
            if (mbool) {
                mIslogIn = true;
                showOutPut("Successfully SignUp into cloud from Controller");
            } else {
                showOutPut("SignUp failed.");
            }
        } catch (Exception e) {
            showOutPut("Exception Occured..");
            Log.e(TAG, e.toString());
        }
    }

    public void signInWithController() {
        try {
            boolean mbool = mCloudHelper.singIn(mController,
                    OcAccountManagerHelper.s_mCloudUid,
                    OcAccountManagerHelper.s_mCloudAccessToken, mCloudHelper);
            Thread.sleep(5000);
            if (mbool) {
                mIslogIn = true;
                showOutPut("Successfully SignIn into cloud.");
            } else {
                showOutPut("SignIn failed.");
            }
        } catch (Exception e) {
            showOutPut("Exception Occured..");
            Log.e(TAG, e.toString());
        }
    }

    public void signOutfromCloudController() {
        try {
            if (mCloudHelper.singOut(mController,
                    OcAccountManagerHelper.s_mCloudAccessToken, mCloudHelper)) {
                mIslogIn = false;
                showOutPut("SignOut Success.");
            } else {
                showOutPut("SignOut failed.");
            }
        } catch (Exception e) {
            showOutPut("Exception Occured..");
            Log.e(TAG, e.toString());
        }
    }

    public void searchUserInformationWithDevUUID() {
        mICHelper.showLog(LogLevel.DEBUG, "searchUserInformationWithDevUUID IN", false);
        if (!mIslogIn) {
            mICHelper.showLog(LogLevel.INFO, "***SignUp first***", false);
        } else {
            try {
                mICHelper.showLog(LogLevel.INFO,
                        "***map initialization start***", false);
                mICHelper.showLog(LogLevel.INFO,"Uuid is "+ OcAccountManagerHelper.s_mCloudUid, false);
                MY_MAP.put("uid", OcAccountManagerHelper.s_mCloudUid);
                mICHelper.showLog(LogLevel.INFO,
                        "***map initialization stop***", false);
                mController.searchUser(MY_MAP, mOcAccountManagerAdapter);
                Thread.sleep(1000);
            } catch (Exception e) {
                mICHelper.showLog(LogLevel.ERROR,
                        "SearchUserInfo failed, due to "
                                + e.getLocalizedMessage(),
                        true);
            }
        }
        mICHelper.showLog(LogLevel.DEBUG, "searchUserInformationWithDevUUID OUT", false);
    }

    public void searchUserWithPhone() {
        mICHelper.showLog(LogLevel.DEBUG, "searchUserWithPhone IN", false);
        if (!mIslogIn) {
            mICHelper.showLog(LogLevel.INFO, "***SignUp first***", false);
        } else {
            try {
                mICHelper.showLog(LogLevel.INFO,
                        "***map initialization start***", false);
                Map<String, String> queryParameters = new HashMap<>();
                queryParameters.put("phone", "+8807111111111");

                mICHelper.showLog(LogLevel.INFO,
                        "***map initialization stop***", false);
                mController.searchUser(queryParameters,
                        mOcAccountManagerAdapter);
            } catch (Exception e) {
                mICHelper.showLog(LogLevel.ERROR,
                        "SearchUserInfo failed, due to "
                                + e.getLocalizedMessage(),
                        true);
            }
        }
        mICHelper.showLog(LogLevel.DEBUG, "searchUserWithPhone OUT", false);
    }

    public void searchUserWithEmail() {
        mICHelper.showLog(LogLevel.DEBUG, "searchUserWithEmail IN", false);
        if (!mIslogIn) {
            mICHelper.showLog(LogLevel.INFO, "***SignUp first***", false);
        } else {
            try {
                Map<String, String> queryParameters = new HashMap<>();
                queryParameters.put("email", "iotsrbd@gamil.com");
                mController.searchUser(queryParameters,
                        mOcAccountManagerAdapter);
            } catch (Exception e) {
                mICHelper.showLog(LogLevel.ERROR,
                        "SearchUserInfo failed, due to "
                                + e.getLocalizedMessage(),
                        true);
            }
        }
        mICHelper.showLog(LogLevel.DEBUG, "searchUserWithEmail OUT", false);
    }

    public void deleteDevice() {
        mICHelper.showLog(LogLevel.DEBUG, "deleteDevice IN", false);
        if (!mIslogIn) {
            mICHelper.showLog(LogLevel.INFO, "***SignUp first***", false);
        } else {
            try {
                mController.deleteDevice(
                        OcAccountManagerHelper.s_mCloudAccessToken,
                        OcAccountManagerHelper.s_mCloudUid,
                        mOcAccountManagerAdapter);
            } catch (Exception e) {
                mICHelper.showLog(LogLevel.ERROR,
                        "SearchUserInfo failed, due to "
                                + e.getLocalizedMessage(),
                        true);
            }
        }
        mICHelper.showLog(LogLevel.DEBUG, "deleteDevice OUT", false);
    }

    public void deleteGroup() {
        mICHelper.showLog(LogLevel.DEBUG, "deleteGroup IN", false);
        if (!mIslogIn)

        {
            mICHelper.showLog(LogLevel.INFO, "***SignUp first***", false);
        } else {
            try {
                mController.deleteGroup(OconGetGroupInfoAllAdapter.sGroupId,
                        mOcAccountManagerAdapter);
            } catch (Exception e) {
                mICHelper.showLog(LogLevel.ERROR,
                        "SearchUserInfo failed, due to "
                                + e.getLocalizedMessage(),
                        true);
            }
        }
        mICHelper.showLog(LogLevel.DEBUG, "deleteGroup OUT", false);
    }

    public void getGroupInfoALL() {
        mICHelper.showLog(LogLevel.DEBUG, "getGroupInfoALL IN", false);
        if (!mIslogIn)

        {
            mICHelper.showLog(LogLevel.INFO, "***SignUp first***", false);
        } else {
            try {
                mController.getGroupInfoAll(mOconGetGroupInfoAllAdapter);
            } catch (Exception e) {
                mICHelper.showLog(LogLevel.ERROR,
                        "SearchUserInfo failed, due to "
                                + e.getLocalizedMessage(),
                        true);
            }
        }
        mICHelper.showLog(LogLevel.DEBUG, "getGroupInfoALL OUT", false);
    }

    public void getGroupInfo() {
        mICHelper.showLog(LogLevel.DEBUG, "getGroupInfo IN", false);
        if (!mIslogIn)

        {
            mICHelper.showLog(LogLevel.INFO, "***SignUp first***", false);
        } else {
            try {
                mController.getGroupInfo(OconGetGroupInfoAllAdapter.sGroupId,
                        mOconGetGroupInfoAdapter);
            } catch (Exception e) {
                mICHelper.showLog(LogLevel.ERROR,
                        "SearchUserInfo failed, due to "
                                + e.getLocalizedMessage(),
                        true);
            }
        }
        mICHelper.showLog(LogLevel.DEBUG, "getGroupInfo OUT", false);
    }

    public void observeGroup() {
        mICHelper.showLog(LogLevel.DEBUG, "observeGroup IN", false);
        if (!mIslogIn)

        {
            mICHelper.showLog(LogLevel.INFO, "***SignUp first***", false);
        } else {
            try {
                mController.observeGroup(mOcAccountManagerAdapter);
            } catch (Exception e) {
                mICHelper.showLog(LogLevel.ERROR,
                        "SearchUserInfo failed, due to "
                                + e.getLocalizedMessage(),
                        true);
            }
        }
        mICHelper.showLog(LogLevel.DEBUG, "observeGroup OUT", false);
    }

    public void cancelObserveGroup() {
        mICHelper.showLog(LogLevel.DEBUG, "cancelObserveGroup IN", false);
        if (!mIslogIn)

        {
            mICHelper.showLog(LogLevel.INFO, "***SignUp first***", false);
        } else {
            try {
                mController.cancelObserveGroup();
            } catch (Exception e) {
                mICHelper.showLog(LogLevel.ERROR,
                        "SearchUserInfo failed, due to "
                                + e.getLocalizedMessage(),
                        true);
            }
        }
        mICHelper.showLog(LogLevel.DEBUG, "cancelObserveGroup OUT", false);
    }

    public void observeInvitation() {
        mICHelper.showLog(LogLevel.DEBUG, "observeInvitation IN", false);
        if (!mIslogIn)

        {
            mICHelper.showLog(LogLevel.INFO, "***SignUp first***", false);
        } else {
            try {
                mController.observeInvitation(mOcAccountManagerAdapter);
            } catch (Exception e) {
                mICHelper.showLog(LogLevel.ERROR,
                        "SearchUserInfo failed, due to "
                                + e.getLocalizedMessage(),
                        true);
            }
        }
        mICHelper.showLog(LogLevel.DEBUG, "observeInvitation OUT", false);
    }

    public void cancelObserveInvitation() {
        mICHelper.showLog(LogLevel.DEBUG, "cancelObserveInvitation IN", false);
        if (!mIslogIn)

        {
            mICHelper.showLog(LogLevel.INFO, "***SignUp first***", false);
        } else {
            try {
                mController.cancelObserveInvitation();
            } catch (Exception e) {
                mICHelper.showLog(LogLevel.ERROR,
                        "SearchUserInfo failed, due to "
                                + e.getLocalizedMessage(),
                        true);
            }
        }
        mICHelper.showLog(LogLevel.DEBUG, "cancelObserveInvitation OUT", false);
    }

    public void cancelInvitation() {
        mICHelper.showLog(LogLevel.DEBUG, "cancelInvitation IN", false);
        if (!mIslogIn)

        {
            mICHelper.showLog(LogLevel.INFO, "***SignUp first***", false);
        } else {
            try {
                mController.replyToInvitation(OcAccountManagerAdapter.sGroupId,
                        false, mOcAccountManagerAdapter);
            } catch (Exception e) {
                mICHelper.showLog(LogLevel.ERROR,
                        "SearchUserInfo failed, due to "
                                + e.getLocalizedMessage(),
                        true);
            }
        }
        mICHelper.showLog(LogLevel.DEBUG, "cancelInvitation OUT", false);
    }

    public void deleteInvitation() {
        mICHelper.showLog(LogLevel.DEBUG, "deleteInvitation IN", false);
        if (!mIslogIn)
        {
            mICHelper.showLog(LogLevel.INFO, "***SignUp first***", false);
        } else {
            try {
                mController.replyToInvitation(OcAccountManagerAdapter.sGroupId,
                        true, mOcAccountManagerAdapter);

            } catch (Exception e) {
                mICHelper.showLog(LogLevel.ERROR,
                        "SearchUserInfo failed, due to "
                                + e.getLocalizedMessage(),
                        true);
            }
        }
        mICHelper.showLog(LogLevel.DEBUG, "deleteInvitation OUT", false);
    }

    public void findResouce() {
        mICHelper.showLog(LogLevel.DEBUG, "findResouce IN", false);
        if (!mIslogIn) {
            mICHelper.showLog(LogLevel.INFO, "***SignUp first***", false);
        } else {
            try {
                Log.d(TAG, "Find Resource from Resource-Directory.");
                OcPlatform.findResource(
                        IC_HOST_ADDRESS,
                        OcPlatform.WELL_KNOWN_QUERY+"?"+"/oic/ps/",
                        EnumSet.of(OcConnectivityType.CT_ADAPTER_IP),
                        mICResourceDirectoryAdapter, IC_OC_QUALITY_OF_SERVICE
                );

            } catch (Exception e) {
                mICHelper.showLog(LogLevel.ERROR,
                        "SearchUserInfo failed, due to "
                                + e.getLocalizedMessage(),
                        true);
            }
        }
        mICHelper.showLog(LogLevel.DEBUG, "findResouce OUT", false);
    }
    
    public void findResouceWithOutQurey() {
        mICHelper.showLog(LogLevel.DEBUG, "findResouceWithOutQurey IN", false);
        if (!mIslogIn) {
            mICHelper.showLog(LogLevel.INFO, "***SignUp first***", false);
        } else {
            try {
                Log.d(TAG, "Find Resource from Resource-Directory.");
                OcPlatform.findResource(
                        IC_HOST_ADDRESS,
                        OcPlatform.WELL_KNOWN_QUERY,
                        EnumSet.of(OcConnectivityType.CT_ADAPTER_IP),
                        mICResourceDirectoryAdapter, IC_OC_QUALITY_OF_SERVICE
                );

            } catch (Exception e) {
                mICHelper.showLog(LogLevel.ERROR,
                        "SearchUserInfo failed, due to "
                                + e.getLocalizedMessage(),
                        true);
            }
        }
        mICHelper.showLog(LogLevel.DEBUG, "findResouceWithOutQurey OUT", false);
    }
    
    public void trougleResource(){
        mICHelper.showLog(LogLevel.DEBUG, "trougleResource IN", false);
        if (!mIslogIn) {
            mICHelper.showLog(LogLevel.INFO, "***SignUp first***", false);
        }
        else
        {
            Log.d(TAG, "Should be implement in near future.");
        }
        mICHelper.showLog(LogLevel.DEBUG, "trougleResource OUT", false);
    }
    
    public void addpropertyvaluetogroup() {
        mICHelper.showLog(LogLevel.DEBUG, "addpropertyvaluetogroup IN", false);
        if (!mIslogIn)
        {
            mICHelper.showLog(LogLevel.INFO, "***SignUp first***", false);
        } else {
            try {
                mPropertyValue.setValue(mProperty, mValues);
                mControllee.addPropertyValueToGroup(
                        OcAccountManagerAdapter.sGroupId, mPropertyValue,
                        mOcAccountManagerAdapter);
            } catch (Exception e) {
                mICHelper.showLog(LogLevel.ERROR,
                        "SearchUserInfo failed, due to "
                                + e.getLocalizedMessage(),
                        true);
            }
        }
        mICHelper.showLog(LogLevel.DEBUG, "addpropertyvaluetogroup OUT", false);
    }

    public void deletepropertyvaluetogroup() {
        mICHelper.showLog(LogLevel.DEBUG, "deletepropertyvaluetogroup IN", false);
        if (!mIslogIn)

        {
            mICHelper.showLog(LogLevel.INFO, "***SignUp first***", false);
        } else {
            try {
                mPropertyValue.setValue(mProperty, mValues);
                mControllee.deletePropertyValueFromGroup(
                        OcAccountManagerAdapter.sGroupId, mPropertyValue,
                        mOcAccountManagerAdapter);

            } catch (Exception e) {
                mICHelper.showLog(LogLevel.ERROR,
                        "SearchUserInfo failed, due to "
                                + e.getLocalizedMessage(),
                        true);
            }
        }
        mICHelper.showLog(LogLevel.DEBUG, "deletepropertyvaluetogroup OUT", false);
    }

    public void updatepropertyvaluetogroup() {
        mICHelper.showLog(LogLevel.DEBUG, "updatepropertyvaluetogroup IN", false);
        if (!mIslogIn)

        {
            mICHelper.showLog(LogLevel.INFO, "***SignUp first***", false);
        } else {
            try {
                mPropertyValue.setValue(mProperty, mValues);
                mControllee.updatePropertyValueOnGroup(
                        OcAccountManagerAdapter.sGroupId, mPropertyValue,
                        mOcAccountManagerAdapter);
            } catch (Exception e) {
                mICHelper.showLog(LogLevel.ERROR,
                        "SearchUserInfo failed, due to "
                                + e.getLocalizedMessage(),
                        true);
            }
        }
        mICHelper.showLog(LogLevel.DEBUG, "updatepropertyvaluetogroup OUT", false);
    }

    public void sendDeleteRequesttoresource() {
        mICHelper.showLog(LogLevel.DEBUG, "sendDeleteRequesttoresource IN", false);
        if (!mIslogIn)

        {
            mICHelper.showLog(LogLevel.INFO, "***SignUp first***", false);
        } else {
            try {
                mICHelper.showLog(LogLevel.INFO,
                        "***Delete Resource from RD***", false);
                OcRDClient.deleteResourceFromRD(IC_HOST_ADDRESS,
                        EnumSet.of(OcConnectivityType.CT_ADAPTER_TCP),
                        mICResourceDirectoryAdapter);
            } catch (Exception e) {
                mICHelper.showLog(LogLevel.ERROR,
                        "SearchUserInfo failed, due to "
                                + e.getLocalizedMessage(),
                        true);
            }
        }
        mICHelper.showLog(LogLevel.DEBUG, "sendDeleteRequesttoresource OUT", false);
    }

    public void discoverallTopics() {
        mICHelper.showLog(LogLevel.DEBUG, "discoverallTopics IN", false);
        if (!mIslogIn) {
            mICHelper.showLog(LogLevel.INFO, "***SignUp first***", false);
        } else {
            try {
                mICMessageQueueHelper.getMQBroker();
                if (null != sMQbrokerResource) {
                    sMQbrokerResource.discoveryMQTopics(
                            new HashMap<String, String>(), micMessageAdapter,
                            IC_OC_QUALITY_OF_SERVICE);
                } else {
                    mICHelper.showLog(LogLevel.INFO,
                            "***MQbrokerResource is null***", false);
                }
            } catch (Exception e) {
                mICHelper.showLog(LogLevel.ERROR,
                        "discoverallTopics failed, due to "
                                + e.getLocalizedMessage(),
                        true);
            }
        }
        mICHelper.showLog(LogLevel.DEBUG, "discoverallTopics OUT", false);
    }
    
    public void discoverTypedTopics() {
        mICHelper.showLog(LogLevel.DEBUG, "discoverTypedTopics IN", false);
        if (!mIslogIn) {
            mICHelper.showLog(LogLevel.INFO, "***SignUp first***", false);
        } else {
            try {
                mICMessageQueueHelper.getMQBroker();
                if (null != sMQbrokerResource) {
                    Map<String, String> queryParameters = new HashMap<>();
                    queryParameters.put("rt", "light");
                    sMQbrokerResource.discoveryMQTopics(
                            queryParameters, micMessageAdapter,
                            IC_OC_QUALITY_OF_SERVICE);
                } else {
                    mICHelper.showLog(LogLevel.INFO,
                            "***MQbrokerResource is null***", false);
                }
            } catch (Exception e) {
                mICHelper.showLog(LogLevel.ERROR,
                        "discoverallTopics failed, due to "
                                + e.getLocalizedMessage(),
                        true);
            }
        }
        mICHelper.showLog(LogLevel.DEBUG, "discoverTypedTopics OUT", false);
    }

    public void createTopic() {
        mICHelper.showLog(LogLevel.DEBUG, "createTopic IN", false);
        if (!mIslogIn)
        {
            mICHelper.showLog(LogLevel.INFO, "***SignUp first***", false);
        } else {
            mICMessageQueueHelper.getMQBroker();
            try {
                Map<String, String> queryParameters = new HashMap<>();
                sMQbrokerResource.createMQTopic(new OcRepresentation(),
                        MQ_DEFAULT_TOPIC_URI, queryParameters,
                        micMessageAdapter, IC_OC_QUALITY_OF_SERVICE);
            } catch (Exception e) {
                mICHelper.showLog(LogLevel.ERROR,
                        "createTopic failed, due to " + e.getLocalizedMessage(),
                        true);
            }
        }
        mICHelper.showLog(LogLevel.DEBUG, "createTopic OUT", false);
    }
    
    public void createTypedTopic() {
        mICHelper.showLog(LogLevel.DEBUG, "createTypedTopic IN", false);
        if (!mIslogIn)
        {
            mICHelper.showLog(LogLevel.INFO, "***SignUp first***", false);
        } else {
            mICMessageQueueHelper.getMQBroker();
            try {
                Map<String, String> queryParameters = new HashMap<>();
                queryParameters.put("rt", "light");
                sMQbrokerResource.createMQTopic(new OcRepresentation(),
                        MQ_DEFAULT_TOPIC_URI, queryParameters,
                        micMessageAdapter, IC_OC_QUALITY_OF_SERVICE);
            } catch (Exception e) {
                mICHelper.showLog(LogLevel.ERROR,
                        "createTopic failed, due to " + e.getLocalizedMessage(),
                        true);
            }
        }
        mICHelper.showLog(LogLevel.DEBUG, "createTypedTopic OUT", false);
    }
    public void selectIndexforsubUnsub(){
        mICHelper.showLog(LogLevel.DEBUG, "selectIndexforsubUnsub IN", false);
        if (!mIslogIn)
        {
            mICHelper.showLog(LogLevel.INFO, "***SignUp first***", false);
        } else {
            if(ICMessageAdapter.sCurrentTopicResource == null)
            {
                showOutPut("Please create a topic first");
            }
        }
        mICHelper.showLog(LogLevel.DEBUG, "selectIndexforsubUnsub OUT", false);
    }

    public void subscribeTopic() {
        mICHelper.showLog(LogLevel.DEBUG, "subscribeTopic IN", false);
        if (!mIslogIn)

        {
            mICHelper.showLog(LogLevel.INFO, "***SignUp first***", false);
        } else {
            try {
                ICMessageAdapter.sCurrentTopicResource.subscribeMQTopic(MY_MAP,
                        micMessageAdapter, IC_OC_QUALITY_OF_SERVICE);
            } catch (Exception e) {
                mICHelper.showLog(LogLevel.ERROR,
                        "SearchUserInfo failed, due to "
                                + e.getLocalizedMessage(),
                        true);
            }
        }
        mICHelper.showLog(LogLevel.DEBUG, "subscribeTopic OUT", false);
    }

    public void unsubscribeTopic() {
        mICHelper.showLog(LogLevel.DEBUG, "unsubscribeTopic IN", false);
        if (!mIslogIn)
        {
            mICHelper.showLog(LogLevel.INFO, "***SignUp first***", false);
        } else {
            try {
                ICMessageAdapter.sCurrentTopicResource
                        .unsubscribeMQTopic(IC_OC_QUALITY_OF_SERVICE);
            } catch (Exception e) {
                mICHelper.showLog(LogLevel.ERROR,
                        "SearchUserInfo failed, due to "
                                + e.getLocalizedMessage(),
                        true);
            }
        }
        mICHelper.showLog(LogLevel.DEBUG, "unsubscribeTopic OUT", false);
    }
    
    public void seletcIndex() {
        mICHelper.showLog(LogLevel.DEBUG, "seletcIndex IN", false);
        if (!mIslogIn) {
            mICHelper.showLog(LogLevel.INFO, "***SignUp first***", false);
        } else {
            mICMessageQueueHelper.getMQBroker();
        }
        mICHelper.showLog(LogLevel.DEBUG, "seletcIndex OUT", false);
    }

    public void publishTopic() {
        mICHelper.showLog(LogLevel.DEBUG, "publishTopic IN", false);
        if (!mIslogIn) {
            mICHelper.showLog(LogLevel.INFO, "***SignUp first***", false);
        } else {
            try {
                OcRepresentation rep = new OcRepresentation();
                OcRepresentation msg = new OcRepresentation();
                if (mSwitchingFlag) {
                    msg.setValue("blue", "on");
                    msg.setValue("red", "off");
                    mSwitchingFlag = false;
                } else {
                    msg.setValue("blue", "off");
                    msg.setValue("red", "on");
                    mSwitchingFlag = true;
                }
                rep.setValue("message", msg);
                if(sMQbrokerResource != null){
                sMQbrokerResource.publishMQTopic(rep, MY_MAP, micMessageAdapter,
                        IC_OC_QUALITY_OF_SERVICE);
                }
                else
                {
                    showOutPut("No topic selected");
                }
            } catch (Exception e) {
                mICHelper.showLog(LogLevel.ERROR,
                        "SearchUserInfo failed, due to "
                                + e.getLocalizedMessage(),
                        true);
            }
        }
        mICHelper.showLog(LogLevel.DEBUG, "publishTopic OUT", false);
    }

    public void Exit() {
        android.os.Process.killProcess(android.os.Process.myPid());
    }

}
