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

package org.iotivity.service.ic;

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
import static org.iotivity.service.ic.ICUtility.*;
import static org.iotivity.service.ic.ICMessageQueueHelper.*;
import static org.iotivity.service.ic.ICHelper.*;
import org.iotivity.service.ic.ICHelper.*;

import org.iotivity.cloud.*;
import org.iotivity.configuration.*;
import org.iotivity.configuration.IConfiguration.*;

public class ICAction extends Base {
    private ICHelper                         mICHelper      = null;
    private OcAccountManagerHelper           mCloudHelper   = new OcAccountManagerHelper();
    private OcAccountManagerAdapter          mOcAccountManagerAdapter;
    private OcRepresentation                 mPropertyValue = new OcRepresentation();
    private OconGetGroupInfoAllAdapter       mOconGetGroupInfoAllAdapter;
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
        mICResourceDirectoryAdapter = new ICResourceDirectoryCommonAdapter();
        mICMessageQueueHelper = new ICMessageQueueHelper();
        micMessageAdapter = new ICMessageAdapter();
    }

    public static void initOcPlatform(ModeType type, Activity mActivity,
            Context mContext) {
        PlatformConfig cfg = new PlatformConfig(mActivity, mContext,
                ServiceType.IN_PROC, type, "0.0.0.0", 0, QualityOfService.LOW);
        OcPlatform.Configure(cfg);
        OcAccountManagerHelper.init(mContext);
    }

    public void signUpWithControlee() {
        if (null == mControllee) {
            mControllee = OcAccountManagerHelper
                    .getAccountMangerInstance(TLS.DISABLED);
        }
        if (mCloudHelper.singUp(mControllee, DEFAULT_AUTH_PROVIDER,
                OcAccountManagerHelper.authCode, mCloudHelper)) {
            mIslogIn = true;
            showOutPut("Successfully SignUp into cloud from testapp.");
        } else {
            showOutPut("SignUp failed.");
        }
    }

    public void signIntoCloud() {
        if (null == mControllee) {
            mControllee = OcAccountManagerHelper
                    .getAccountMangerInstance(TLS.DISABLED);
        }
        if (mCloudHelper.singIn(mControllee, OcAccountManagerHelper.s_CloudUid,
                OcAccountManagerHelper.s_CloudAccesstoken, mCloudHelper)) {
            mIslogIn = true;
            showOutPut("Successfully SignIn into cloud.");
        } else {
            showOutPut("SignIn failed.");
        }
    }

    public void signOutfromCloud() {
        if (null == mControllee) {
            mControllee = OcAccountManagerHelper
                    .getAccountMangerInstance(TLS.DISABLED);
        }
        if (mCloudHelper.singOut(mControllee,
                OcAccountManagerHelper.s_CloudAccesstoken, mCloudHelper)) {
            mIslogIn = true;
            showOutPut("Successfully SignOut from cloud.");
        } else {
            showOutPut("SignOut failed.");
        }
    }

    public void searchUserInformationWithDevUUID() {
        if (!mIslogIn) {
            mICHelper.showLog(LogLevel.INFO, "***SignUp first***", false);
        } else {
            try {
                mICHelper.showLog(LogLevel.INFO,
                        "***map initialization start***", false);
                MY_MAP.put("userid", OcAccountManagerHelper.s_CloudUid);
                mICHelper.showLog(LogLevel.INFO,
                        "***map initialization stop***", false);
                mControllee.searchUser(MY_MAP, mOcAccountManagerAdapter);

            } catch (Exception e) {
                mICHelper.showLog(LogLevel.ERROR,
                        "SearchUserInfo failed, due to "
                                + e.getLocalizedMessage(),
                        true);
            }
        }
    }

    public void deleteDevice() {
        if (!mIslogIn) {
            mICHelper.showLog(LogLevel.INFO, "***SignUp first***", false);
        } else {
            try {
                mControllee.deleteDevice(
                        OcAccountManagerHelper.s_CloudAccesstoken,
                        OcAccountManagerHelper.s_CloudUid,
                        mOcAccountManagerAdapter);
            } catch (Exception e) {
                mICHelper.showLog(LogLevel.ERROR,
                        "SearchUserInfo failed, due to "
                                + e.getLocalizedMessage(),
                        true);
            }
        }
    }

    public void createGroup() {
        if (!mIslogIn) {
            mICHelper.showLog(LogLevel.INFO, "***SignUp first***", false);
        } else {
            try {
                if (null == mControllee) {
                    showOutPut("mControllee is null");
                } else {
                    showOutPut("CreateGroup start....");
                    mControllee.createGroup(mOcAccountManagerAdapter);
                }
            } catch (Exception e) {
                mICHelper.showLog(LogLevel.ERROR,
                        "CreateGroup failed, due to " + e.getLocalizedMessage(),
                        true);
            }
        }
    }

    public void deleteGroup() {
        if (!mIslogIn)

        {
            mICHelper.showLog(LogLevel.INFO, "***SignUp first***", false);
        } else {
            try {
                mControllee.deleteGroup(OcAccountManagerAdapter.sGroupId,
                        mOcAccountManagerAdapter);
            } catch (Exception e) {
                mICHelper.showLog(LogLevel.ERROR,
                        "SearchUserInfo failed, due to "
                                + e.getLocalizedMessage(),
                        true);
            }
        }
    }

    public void getGroupInfoALL() {
        if (!mIslogIn)

        {
            mICHelper.showLog(LogLevel.INFO, "***SignUp first***", false);
        } else {
            try {
                mControllee.getGroupInfoAll(mOconGetGroupInfoAllAdapter);
            } catch (Exception e) {
                mICHelper.showLog(LogLevel.ERROR,
                        "SearchUserInfo failed, due to "
                                + e.getLocalizedMessage(),
                        true);
            }
        }
    }

    public void getGroupInfo() {
        if (!mIslogIn)

        {
            mICHelper.showLog(LogLevel.INFO, "***SignUp first***", false);
        } else {
            try {
                mControllee.getGroupInfo(OcAccountManagerAdapter.sGroupId,
                        mOconGetGroupInfoAllAdapter);
            } catch (Exception e) {
                mICHelper.showLog(LogLevel.ERROR,
                        "SearchUserInfo failed, due to "
                                + e.getLocalizedMessage(),
                        true);
            }
        }
    }

    public void observeGroup() {
        if (!mIslogIn)

        {
            mICHelper.showLog(LogLevel.INFO, "***SignUp first***", false);
        } else {
            try {
                mControllee.observeGroup(mOcAccountManagerAdapter);
            } catch (Exception e) {
                mICHelper.showLog(LogLevel.ERROR,
                        "SearchUserInfo failed, due to "
                                + e.getLocalizedMessage(),
                        true);
            }
        }
    }

    public void cancelObserveGroup() {
        if (!mIslogIn)

        {
            mICHelper.showLog(LogLevel.INFO, "***SignUp first***", false);
        } else {
            try {
                mControllee.cancelObserveGroup();
            } catch (Exception e) {
                mICHelper.showLog(LogLevel.ERROR,
                        "SearchUserInfo failed, due to "
                                + e.getLocalizedMessage(),
                        true);
            }
        }
    }

    public void observeInvitation() {
        if (!mIslogIn)

        {
            mICHelper.showLog(LogLevel.INFO, "***SignUp first***", false);
        } else {
            try {
                mControllee.observeInvitation(mOcAccountManagerAdapter);
            } catch (Exception e) {
                mICHelper.showLog(LogLevel.ERROR,
                        "SearchUserInfo failed, due to "
                                + e.getLocalizedMessage(),
                        true);
            }
        }
    }

    public void cancelObserveInvitation() {
        if (!mIslogIn)

        {
            mICHelper.showLog(LogLevel.INFO, "***SignUp first***", false);
        } else {
            try {
                mControllee.cancelObserveInvitation();
            } catch (Exception e) {
                mICHelper.showLog(LogLevel.ERROR,
                        "SearchUserInfo failed, due to "
                                + e.getLocalizedMessage(),
                        true);
            }
        }
    }

    public void sendInvitation() {
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
    }

    public void cancelInvitation() {
        if (!mIslogIn)

        {
            mICHelper.showLog(LogLevel.INFO, "***SignUp first***", false);
        } else {
            try {
                mControllee.replyToInvitation(OcAccountManagerAdapter.sGroupId,
                        false, mOcAccountManagerAdapter);
            } catch (Exception e) {
                mICHelper.showLog(LogLevel.ERROR,
                        "SearchUserInfo failed, due to "
                                + e.getLocalizedMessage(),
                        true);
            }
        }
    }

    public void deleteInvitation() {
        if (!mIslogIn)

        {
            mICHelper.showLog(LogLevel.INFO, "***SignUp first***", false);
        } else {
            try {
                mControllee.replyToInvitation(OcAccountManagerAdapter.sGroupId,
                        true, mOcAccountManagerAdapter);

            } catch (Exception e) {
                mICHelper.showLog(LogLevel.ERROR,
                        "SearchUserInfo failed, due to "
                                + e.getLocalizedMessage(),
                        true);
            }
        }
    }

    public void publishResource() {
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
    }

    public void signUpWithController() {
        if (null == mController) {
            mController = OcAccountManagerHelper
                    .getAccountMangerInstance(TLS.DISABLED);
        }
        if (mCloudHelper.singUp(mController, DEFAULT_AUTH_PROVIDER,
                OcAccountManagerHelper.authCode, mCloudHelper)) {
            mIslogIn = true;
            showOutPut("Successfully SignUp into cloud from testapp.");
        } else {
            showOutPut("SignUp failed.");
        }
    }

    public void signInWithController() {
        if (null == mController) {
            mController = OcAccountManagerHelper
                    .getAccountMangerInstance(TLS.DISABLED);
        }
        if (mCloudHelper.singIn(mController, OcAccountManagerHelper.s_CloudUid,
                OcAccountManagerHelper.s_CloudAccesstoken, mCloudHelper)) {
            mIslogIn = true;
            showOutPut("Successfully SignIn into cloud from testapp.");
        } else {
            showOutPut("SignIn failed.");
        }
    }

    public void signOutfromCloudController() {
        if (null == mController) {
            mController = OcAccountManagerHelper
                    .getAccountMangerInstance(TLS.DISABLED);
        }
        if (mCloudHelper.singOut(mController,
                OcAccountManagerHelper.s_CloudAccesstoken, mCloudHelper)) {
            mIslogIn = false;
            showOutPut("SignOut Success.");
        } else {
            showOutPut("SignOut failed.");
        }
    }

    public void addpropertyvaluetogroup() {
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
    }

    public void deletepropertyvaluetogroup() {
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
    }

    public void updatepropertyvaluetogroup() {
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
    }

    public void sendDeleteRequesttoresource() {
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
    }

    public void discoverallTopics() {
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
    }

    public void createTopic() {
        if (!mIslogIn)

        {
            mICHelper.showLog(LogLevel.INFO, "***SignUp first***", false);
        } else {

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
    }

    public void subscribeTopic() {
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
    }

    public void unsubscribeTopic() {
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
    }

    public void publishTopic() {
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
                sMQbrokerResource.publishMQTopic(rep, MY_MAP, micMessageAdapter,
                        IC_OC_QUALITY_OF_SERVICE);
            } catch (Exception e) {
                mICHelper.showLog(LogLevel.ERROR,
                        "SearchUserInfo failed, due to "
                                + e.getLocalizedMessage(),
                        true);
            }
        }
    }

    public void Exit() {
        android.os.Process.killProcess(android.os.Process.myPid());
    }

}
